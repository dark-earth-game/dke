/*
 * dialogue.c — LIEU dialogue: subtitle + spoken WAV.
 *
 * NWDIAL%02d.TXT (+ COMMON.TXT) is a flat line-keyed text DB:
 *     L01D02
 *      Kalhi, tu dors? XXX          <- text, leading space, " XXX" sentinel
 * Lookup is a linear scan for a line == <code>; the next line is the text.
 * WAV is dialog/L%02d/<code>.WAV (port layout). Lines play serialized with
 * a display-duration timer (mirrors engine single-slot sub_46BC74).
 */

#include <stdio.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "libsys/audio/wav.h"
#include "dialogue.h"

#define DLG_BUF     (96 * 1024)
#define DLG_QUEUE   32
#define DLG_QMASK   (DLG_QUEUE - 1)

static c8   g_buf[DLG_BUF];
static i32  g_buf_len = 0;
static i32  g_loaded_lvl = -1;
static c8   g_queue[DLG_QUEUE][16];
static i32  g_qh = 0, g_qt = 0;
static c8   g_active[256];
static u32  g_min_ms = 0;   /* earliest tick the active line may be dismissed */

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

void dlg_reset(void) {
    g_qh = g_qt = 0;
    g_active[0] = '\0';
    g_min_ms = 0;
}

void dlg_say(const c8 *code) {
    if (!code || !code[0]) return;
    if (g_qt - g_qh >= DLG_QUEUE - 1) return;   /* queue full */
    i32 k = 0;
    while (code[k] && k < 15) { g_queue[g_qt & DLG_QMASK][k] = code[k]; k++; }
    g_queue[g_qt & DLG_QMASK][k] = '\0';
    g_qt++;
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

void dlg_tick(i32 level, const c8 *root, const c8 *lang, u32 now_ms, i32 advance) {
    /* Retail Dark Earth auto-advances by a voice-length timer (dkev
     * sub_44968B). We instead HOLD each line until the player advances
     * (click/key), so the conversation is user-paced. A line may be
     * dismissed only after a short minimum so the click that opened it
     * (or the previous line's click) doesn't skip it instantly. */
    if (g_active[0]) {
        if (!advance || now_ms < g_min_ms) return;   /* wait for input */
        g_active[0] = '\0';                            /* advance -> next  */
    }
    if (g_qh == g_qt) return;                          /* queue empty */

    const c8 *code = g_queue[g_qh & DLG_QMASK];
    g_qh++;
    dlg_load(level, root, lang);

    c8 text[256];
    if (!dlg_lookup(code, text, (i32)sizeof(text)))
        snprintf(text, sizeof(text), "[%s]", code);
    strncpy(g_active, text, sizeof(g_active) - 1);
    g_active[sizeof(g_active) - 1] = '\0';

    /* Spoken WAV (port layout dialog/L%02d/<code>.WAV). */
    c8 wp[512];
    wav_t w = {0};
    u32 dur = 2500;
    snprintf(wp, sizeof(wp), "%s/dialog/L%02d/%s.WAV", root, level, code);
    if (wav_load(&w, wp)) {
        sfx_play(&w);
        if (w.sample_rate) dur = (u32)((u64)w.num_samples * 1000 / w.sample_rate);
        wav_free(&w);
    }
    (void)dur;
    g_min_ms = now_ms + 500;   /* debounce: can't skip for first 500 ms */
    print("dlg: '%s' -> \"%s\" (hold for input)\n", code, g_active);
}

const c8 *dlg_active_text(void) {
    return g_active[0] ? g_active : NULL;
}

i32 dlg_is_active(void) { return g_active[0] ? 1 : 0; }
