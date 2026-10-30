/*
 * dialogue.c — LIEU dialogue: subtitle + spoken WAV.
 *
 * NWDIAL%02d.TXT (+ COMMON.TXT) is a flat line-keyed text DB:
 *     L01D02
 *      Kalhi, tu dors? XXX          <- text, leading space, " XXX" sentinel
 * Lookup is a linear scan for a line == <code>; the next line is the text.
 * WAV is dialog/L%02d/<code>.WAV (port layout). One line shows at a time;
 * the script's DIALOGUE action waits for the slot (sub_46BC74).
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "libsys/audio/wav.h"
#include "dialogue.h"
#include "sfx.h"
#include "actor.h"
#include "entity.h"
#include "obc.h"

#define DLG_BUF     (96 * 1024)

static c8   g_buf[DLG_BUF];
static i32  g_buf_len = 0;
static i32  g_loaded_lvl = -1;
static c8   g_active[256];
static u32  g_start_ms    = 0;   /* world_timer @ line start              */
static u32  g_duration_ms = 0;   /* dword_4FF098: 0 = no line showing     */
static i32  g_ctx_level = 1;
static c8   g_ctx_root[256] = ".";
static c8   g_ctx_lang[8] = "US";

static i32 read_whole(const c8 *host, c8 *dst, i32 max) {
    FILE *f = ls_open(host, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0) n = 0;
    if (n > max) n = max;
    i32 r = (i32)fread(dst, 1, (size_t)n, f);
    ls_close(f);
    return r;
}

static void dlg_load(i32 lvl, const c8 *root, const c8 *lang) {
    if (lvl == g_loaded_lvl) return;
    c8 p[512];
    g_buf_len = 0;
    snprintf(p, sizeof(p), "%s/datas/dialogue/%s/COMMON.TXT", root, lang);
    g_buf_len += read_whole(p, g_buf + g_buf_len, DLG_BUF - 1 - g_buf_len);
    snprintf(p, sizeof(p), "%s/datas/dialogue/%s/NWDIAL%02d.TXT", root, lang, lvl);
    g_buf_len += read_whole(p, g_buf + g_buf_len, DLG_BUF - 1 - g_buf_len);
    g_buf[g_buf_len] = '\0';
    g_loaded_lvl = lvl;
    print("dlg: loaded dialogue DB for L%02d (%d bytes)\n", lvl, g_buf_len);
}

/* Language int -> 2-letter code (dkev language_list @ 0x4B52C0).
 * 0=FR 1=GB 2=US 3=DE 4=IT 5=SP 6=JP. English = 2 ("US" -> disk folder us). */
const c8 *dlg_lang_code(i32 id) {
    static const c8 *const codes[] = { "FR", "GB", "US", "DE", "IT", "SP", "JP" };
    if (id < 0 || id > 6) id = 0;
    return codes[id];
}

/* Copy the text line following a "<code>" key line; strip " XXX" + trim. */
static i32 dlg_lookup(const c8 *code, c8 *out, i32 outmax) {
    i32 cl = (i32)strlen(code);
    const c8 *end = g_buf + g_buf_len;
    for (const c8 *s = g_buf; s < end; ) {
        if (!strncmp(s, code, (size_t)cl) &&
            (s[cl] == '\r' || s[cl] == '\n' || s[cl] == ' ' || s[cl] == '\t')) {
            /* advance to the next line = the text */
            const c8 *t = s;
            while (t < end && *t != '\n') t++;
            if (t < end) t++;
            i32 o = 0;
            while (t < end && *t != '\n' && *t != '\r' && o < outmax - 1) out[o++] = *t++;
            out[o] = '\0';
            c8 *x = strstr(out, "XXX");
            if (x) *x = '\0';
            /* trim trailing / leading whitespace */
            o = (i32)strlen(out);
            while (o > 0 && (out[o-1] == ' ' || out[o-1] == '\t')) out[--o] = '\0';
            i32 st = 0;
            while (out[st] == ' ' || out[st] == '\t') st++;
            if (st) memmove(out, out + st, strlen(out + st) + 1);
            return out[0] ? 1 : 0;
        }
        while (s < end && *s != '\n') s++;
        if (s < end) s++;
    }
    return 0;
}

void dlg_reset(void) {
    g_active[0] = '\0';
    g_start_ms = 0;
    g_duration_ms = 0;
}

/* sub_46BC74 @ 0x46BC74: show line `code`, play its voice, and arm the
 * display timer (world_timer / dword_4FF098) for the voice length — or
 * 0xFA0 ms when the line has no voice. Returns the duration. */
u32 dlg_start(const c8 *code, i32 speaker, u32 now_ms) {
    if (!code || !code[0]) return 0;
    dlg_load(g_ctx_level, g_ctx_root, g_ctx_lang);
    c8 text[256];
    if (!dlg_lookup(code, text, (i32)sizeof(text)))
        snprintf(text, sizeof(text), "[%s]", code);
    snprintf(g_active, sizeof(g_active), "%s", text);

    /* sub_44E57B voice slot 0 (shared with Bruitage one-shots, mixed by
     * sfx.c), placed at the speaker (sub_44411D -> entity+0x50 x/z / 600);
     * no speaker -> (0, 0), unplaced. No voice file -> 0xFA0 ms. */
    entity_t *e = actor_find(speaker);
    i32 x = 0, z = 0;
    if (e && EROOT(e)) { x = EROOT(e)->pos[0] / 600; z = EROOT(e)->pos[2] / 600; }
    u32 dur = sfx_voice_play(e, code, x, z);
    if (!dur) dur = 0xFA0;
    g_start_ms    = now_ms;
    g_duration_ms = dur;
    print("dlg: '%s' -> \"%s\" (%.1fs)\n", code, g_active, dur / 1000.0);
    return dur;
}

/* sub_44968B @ 0x4498E7: close the line once its timer runs out. The port
 * also lets the player cut a line short (after a 500 ms debounce). */
void dlg_tick(i32 level, const c8 *root, const c8 *lang, u32 now_ms, i32 advance) {
    g_ctx_level = level;
    snprintf(g_ctx_root, sizeof(g_ctx_root), "%s", root ? root : ".");
    snprintf(g_ctx_lang, sizeof(g_ctx_lang), "%s", lang ? lang : "US");
    if (!g_duration_ms) return;
    i32 elapsed = (i32)(now_ms - g_start_ms);
    if (elapsed > (i32)g_duration_ms || (advance && elapsed > 500)) {
        if (advance && elapsed <= (i32)g_duration_ms)
            sfx_voice_stop();
        g_active[0] = '\0';
        g_duration_ms = 0;
    }
}

const c8 *dlg_active_text(void) {
    return g_active[0] ? g_active : NULL;
}

i32 dlg_is_active(void) { return g_duration_ms != 0; }
