/*
 * dial.c — sub_46A4D5 — dialogues (MENUDIAL).
 *
 * Opened from the main menu. It is NOT a replay screen: it is a log of the
 * dialogue lines the player has heard, browsed like a book. No voice is
 * played, no text pages are stepped through.
 *
 * Data: byte_5492B0[dword_4B787C] — 8-byte records, 7 bytes of line code
 * without its leading 'L' ("01D17A") and, at +7, the speaker id (byte_5492B7).
 * Records are appended by sub_46A358, which sub_46BC74 (dlg_start) calls
 * for every line shown (speaker ids 0/1 -> 1, 2/0x62 -> 2, 3/0x63 -> 3,
 * 0xC/0xD/0x21 -> 0xC, 0x14/0x15 -> 0x14, 0x1B/0x54 -> 0x1B, ids < 4 come
 * from player_entity+0xB0; nothing is logged while dword_4B4768 == 1).
 * The log is saved in the savegame (0x43B74B, 0x43B760).
 *
 * Layout (640x480, backdrop MENUDIAL.GIF, sprites DIALIMAG.GIF):
 *   - three rows, i = 0..2: a 130x100 portrait P%02d.GIF at (8, 54 + 136 i)
 *     with the speaker's name (NAME_<lang>.GIF, 22 px strip per id,
 *     blitted from (0, 22 id) to (11, 74)) and a 300x100 text panel at
 *     (151, 48 + 136 i) (sub_46BAEB: text wrapped to 280 px, drawn at
 *     (5, 5); style 1 for ids 1..3, style 4 for everybody else).
 *     Row 1 is the current line, rows 0 / 2 the previous / next one
 *     (sub_46B984 loads the portraits; out of range rows show P00).
 *   - the big speaker filter portrait at (497, 192) (sub_46BA63).
 *
 * Buttons (40-byte table at [ebp-0x4120], sub_43F7BD hit test), pressed
 * sprites come from DIALIMAG.GIF at the same sub-rect:
 *   1 up    (547,150) 30x28  key Up     previous line of the filter (sub_46B909)
 *   2 down  (546,348) 31x31  key Down   next line of the filter (sub_46B883)
 *   3 left  (527,292) 30x32  key Left   previous speaker in the filter list
 *   4 right (573,292) 31x31  key Right  next speaker in the filter list
 *   5 exit  (567,399) 50x50  key Esc    leave
 *   6 big portrait (533,199) 57x63, key Enter: jump to the latest line of
 *     the filtered speaker
 * Home resets the filter to "everybody". The filter list (var_4308) is the
 * distinct speaker ids of the log, sorted, with 0 = everybody first.
 * Buttons act on release, Enter / area 6 on press. The screen opens on the
 * newest line (var_4158 = dword_4B787C - 1).
 *
 * Headless: DKE_FRAMES leaves after ~1.5 s, DKE_MENUSHOT writes a PPM.
 * DKE_DIAL_FAKE=1 fills the log from the NWDIAL01/02 text files (speaker =
 * the "persoNN" tag in the comment above each line).
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu/screens.h"
#include "state.h"
#include "game_init.h"
#include "dialogue.h"
#include "sfx.h"
#include "screen_fx.h"
#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/file_system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/font.h"
#include "libsys/audio/wav.h"

#define LOG_MAX     0x1000                  /* sub_46A358 stops at 0x1000 */
#define PORTRAITS   128

static c8  g_log_code[LOG_MAX][8];          /* byte_5492B0 */
static u8  g_log_who[LOG_MAX];              /* byte_5492B7 */
static i32 g_log_n = 0;                     /* dword_4B787C */

/*
 * sub_46A358: append a heard line to the log. `code` is the full line code
 * ("L01D17A"), `speaker` the id the script gave; called by the LIEU dialogue
 * action as sub_46BC74 does right after its busy check. TODO: save / load
 * g_log_* with the savegame (0x43B74B); take `speaker` < 4 from
 * player_entity+0xB0 like the original.
 */
void menu_dial_log(const c8 *code, i32 speaker) {
    if (!code || !code[0] || g_log_n >= LOG_MAX || speaker < 0 || speaker == 0x45) return;
    if (speaker == 0 || speaker == 1) speaker = 1;
    if (speaker == 2 || speaker == 0x62) speaker = 2;
    if (speaker == 3 || speaker == 0x63) speaker = 3;
    if (speaker == 0xC || speaker == 0xD || speaker == 0x21) speaker = 0xC;
    if (speaker == 0x14 || speaker == 0x15) speaker = 0x14;
    if (speaker == 0x1B || speaker == 0x54) speaker = 0x1B;
    memset(g_log_code[g_log_n], 0, 8);
    strncpy(g_log_code[g_log_n], code + 1, 7);
    g_log_who[g_log_n] = (u8)speaker;
    g_log_n++;
}

/* ---- text database (NWDIAL%02d.TXT, sub_4161D0 / sub_4166A0) ---- */

static c8 *g_db[100];
static i32 g_db_len[100];

static const c8 *db_get(i32 lvl, i32 *len) {
    if (lvl < 0 || lvl >= 100 || !state || !state->game_path) return NULL;
    if (!g_db[lvl]) {
        c8 p[512];
        snprintf(p, sizeof(p), "%s/datas/dialogue/%s/NWDIAL%02d.TXT", state->game_path,
                 dlg_lang_code(state->language_id), lvl);
        FILE *f = ls_open(p, "rb");
        c8 *buf = NULL; i32 n = 0;
        if (f) {
            fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
            buf = (c8 *)malloc((size_t)sz + 1);
            if (buf) { n = (i32)fread(buf, 1, (size_t)sz, f); buf[n] = 0; }
            ls_close(f);
        }
        if (!buf) { buf = (c8 *)calloc(1, 1); n = 0; }
        g_db[lvl] = buf; g_db_len[lvl] = n;
    }
    *len = g_db_len[lvl];
    return g_db[lvl];
}

/* sub_4166A0: text of line `code` ("L01D17A"): the lines after the key line
 * up to "XXX", joined with spaces. A missing line gives "Dialogue not found !!!". */
static void fetch_text(i32 lvl, const c8 *code, c8 *out, i32 max) {
    i32 len = 0, cl = (i32)strlen(code);
    const c8 *db = db_get(lvl, &len);
    out[0] = 0;
    for (i32 i = 0; db && i < len; ) {
        const c8 *ln = db + i;
        if (!strncasecmp(ln, code, (size_t)cl) && (u8)ln[cl] <= ' ') {
            i32 o = 0, j = i + cl;
            while (j < len && o < max - 2) {
                c8 ch = db[j++];
                if (ch == 'X' && j + 1 < len && db[j] == 'X' && db[j + 1] == 'X') break;
                if (ch == '\r' || ch == '\n') ch = ' ';
                if (ch == ' ' && (o == 0 || out[o - 1] == ' ')) continue;
                out[o++] = ch;
            }
            while (o > 0 && out[o - 1] == ' ') o--;
            out[o] = 0;
            return;
        }
        while (i < len && db[i] != '\n') i++;
        i++;
    }
    snprintf(out, (size_t)max, "Dialogue not found !!!");
}

static i32 glyph_w(const font_t *f, u8 ch) {
    if (f && f->glyph_entries && ch < FONT_NUM_GLYPHS) return f->glyph_entries[ch].glyph_width;
    return f && f->glyph_w ? f->glyph_w : 8;
}

/* ---- images ---- */

static void free_bmap(bmap_t *b) {
    if (!b) return;
    if (b->owns_buffer && b->pixels) free(b->pixels);
    if (b->owns_palette && b->palette) free(b->palette);
    free(b);
}

static void blit_sub(u32 *fb, i32 fw, i32 fh, const bmap_t *src, const u8 *pal,
                     i32 sx, i32 sy, i32 sw, i32 sh, i32 dx, i32 dy, i32 masked) {
    if (!src || !src->pixels || !pal) return;
    for (i32 r = 0; r < sh; r++) {
        i32 y = dy + r, ys = sy + r;
        if (y < 0 || y >= fh || ys < 0 || ys >= src->height) continue;
        for (i32 c = 0; c < sw; c++) {
            i32 x = dx + c, xs = sx + c;
            if (x < 0 || x >= fw || xs < 0 || xs >= (i32)src->width) continue;
            u8 idx = src->pixels[ys * src->width + xs];
            if (masked && idx == 0) continue;            /* index 0 is transparent */
            fb[y * fw + x] = 0xFF000000u | ((u32)pal[idx * 3 + 2] << 16) |
                             ((u32)pal[idx * 3 + 1] << 8) | pal[idx * 3];
        }
    }
}

typedef struct { i32 sx, sy, w, h, dx, dy; } btn_t;

static const btn_t BTN[6] = {                           /* [ebp-0x4120] */
    {  0,  0, 0x1E, 0x1C, 0x223, 0x096 },               /* 1 up */
    { 0x1F, 0x20, 0x1F, 0x1F, 0x222, 0x15C },           /* 2 down */
    {  0, 0x1C, 0x1E, 0x20, 0x20F, 0x124 },             /* 3 left */
    { 0x1F, 0,  0x1F, 0x1F, 0x23D, 0x124 },             /* 4 right */
    {  0, 0x3C, 0x32, 0x32, 0x237, 0x18F },             /* 5 exit */
    {  0,  0, 0x39, 0x3F, 0x215, 0x0C7 },               /* 6 big portrait */
};
static const i32 BTN_KEY[5] = { 38, 40, 37, 39, 27 };   /* [ebp-0x413C]: 26 28 25 27 1B */

/* sub_43F7BD: strict inside test, returns index + 1 or 0. */
static i32 hit_test(i32 mx, i32 my) {
    for (i32 i = 0; i < 6; i++)
        if (BTN[i].dx < mx && BTN[i].dx + BTN[i].w > mx &&
            BTN[i].dy < my && BTN[i].dy + BTN[i].h > my) return i + 1;
    return 0;
}

/* sub_46B909: previous line of speaker `who` (0 = any), or `cur`. */
static i32 find_prev(i32 cur, i32 who) {
    if (cur == 0) return 0;
    if (who == 0) return cur - 1;
    for (i32 i = cur - 1; i >= 0; i--) if (g_log_who[i] == who) return i;
    return cur;
}

/* sub_46B883: next line of speaker `who`, or `cur`. */
static i32 find_next(i32 cur, i32 who) {
    if (cur == g_log_n - 1) return cur;
    if (who == 0) return cur + 1;
    for (i32 i = cur + 1; i < g_log_n - 1; i++) if (g_log_who[i] == who) return i;
    return cur;
}

static void shot(void) {
    const c8 *path = getenv("DKE_MENUSHOT");
    if (!path) return;
    FILE *f = fopen(path, "wb");
    if (!f) return;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    fprintf(f, "P6\n%d %d\n255\n", fw, fh);
    for (i32 i = 0; i < fw * fh; i++) {
        u32 p = state->graphic.front_buffer[i];
        fputc((u8)p, f); fputc((u8)(p >> 8), f); fputc((u8)(p >> 16), f);
    }
    fclose(f);
}

/* DKE_DIAL_FAKE: fill the log from NWDIAL01 / 02 (speaker = "persoNN"). */
static void fake_log(void) {
    for (i32 lvl = 1; lvl <= 2; lvl++) {
        i32 len = 0, who = 1;
        const c8 *db = db_get(lvl, &len);
        for (i32 i = 0; db && i < len; ) {
            const c8 *ln = db + i;
            if (ln[0] == '/' && ln[1] == '/') {
                const c8 *p = strstr(ln, "perso");
                const c8 *e = memchr(ln, '\n', (size_t)(len - i));
                if (p && (!e || p < e)) who = atoi(p + 5);
            } else if (ln[0] == 'L' && isdigit((u8)ln[1])) {
                c8 code[16]; i32 k = 0;
                while ((u8)ln[k] > ' ' && k < 15) { code[k] = ln[k]; k++; }
                code[k] = 0;
                menu_dial_log(code, who);
            }
            while (i < len && db[i] != '\n') i++;
            i++;
        }
    }
}

/* ---- screen ---- */

static bmap_t *g_port[PORTRAITS];           /* P%02d.GIF cache */
static bmap_t *g_names;                     /* NAME_<lang>.GIF */

static bmap_t *portrait(i32 id) {
    if (id < 0 || id >= PORTRAITS) id = 0;
    if (!g_port[id]) {
        c8 p[64];
        snprintf(p, sizeof(p), "DATAS>PORTRAIT>P%02d.GIF", id);
        g_port[id] = game_init_load_cursor(p);
    }
    return g_port[id];
}

/* One 130x100 portrait + name strip (sub_46B984 / sub_46BA63). */
static void draw_portrait(u32 *fb, i32 fw, i32 fh, i32 id, i32 x, i32 y) {
    bmap_t *p = portrait(id);
    if (!p) p = portrait(0);
    if (p && p->palette) blit_sub(fb, fw, fh, p, p->palette, 0, 0, 0x82, 0x64, x, y, 1);  /* ls_bit_move mode 1 */
    if (id && g_names && g_names->palette)
        blit_sub(fb, fw, fh, g_names, g_names->palette, 0, 22 * id, 108, 21, x + 11, y + 74, 1);
}

/* sub_46BAEB: the text panel of line `e` at (x, y). */
static void draw_text_panel(i32 e, i32 x, i32 y, c8 (*cache)[512], u8 *have) {
    if (e < 0 || e >= g_log_n) return;
    if (!have[e]) {
        c8 code[16] = "L";
        memcpy(code + 1, g_log_code[e], 7); code[8] = 0;
        fetch_text(atoi(g_log_code[e]), code, cache[e], 512);
        have[e] = 1;
    }
    font_t *f = font_handle ? font_handle : g_default_font;
    u8 who = g_log_who[e];
    u32 col = (who >= 1 && who <= 3) ? 0xFFF0D890u : 0xFFB8C8D8u;     /* style 1 / 4 */
    i32 lh = (f && f->glyph_h) ? f->glyph_h + 2 : 12, line = 0;
    c8 buf[512]; strncpy(buf, cache[e], sizeof(buf) - 1); buf[511] = 0;
    c8 *p = buf;
    while (*p && line < 9) {
        i32 w = 0; c8 *q = p, *brk = NULL;
        while (*q) {
            if (*q == ' ') brk = q;
            w += glyph_w(f, (u8)*q);
            if (w >= 280) break;
            q++;
        }
        c8 save = 0, *next;
        if (*q && brk) { next = brk + 1; q = brk; }      /* backtrack to the last space */
        else next = q;
        save = *q; *q = 0;
        draw_text_at(f, x + 5, y + 5 + line * lh, p, col);
        *q = save; p = next;
        while (*p == ' ') p++;
        line++;
    }
}

void menu_dial_run(void) {
    if (!state || !state->graphic.front_buffer) return;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    u32 *fb = state->graphic.front_buffer;
    system_t *sys = &state->system;

    bmap_t *bg   = game_init_load_cursor("DATAS>MENUS>MENUDIAL.GIF");
    bmap_t *img  = game_init_load_cursor("DATAS>MENUS>DIALIMAG.GIF");
    bmap_t *curs = game_init_load_cursor("DATAS>GLOBAL>IMAGES>CURS128.GIF");
    if (!curs) curs = game_init_load_cursor("DATAS>MENUS>CURS128.GIF");
    if (!bg || !img) { free_bmap(bg); free_bmap(img); free_bmap(curs); return; }
    const u8 *pal  = bg->palette ? bg->palette : img->palette;
    const u8 *ipal = img->palette ? img->palette : pal;
    const u8 *cpal = (curs && curs->palette) ? curs->palette : pal;

    /* NAME_<language>.GIF, else NAME_0 (0x46A861). */
    c8 np[64];
    snprintf(np, sizeof(np), "DATAS>PORTRAIT>NAME_%d.GIF", state->language_id);
    g_names = game_init_load_cursor(np);
    if (!g_names) g_names = game_init_load_cursor("DATAS>PORTRAIT>NAME_0.GIF");

    if (getenv("DKE_DIAL_FAKE") && g_log_n == 0) fake_log();

    /* Distinct speakers, sorted, 0 = everybody first (0x46A9A9..0x46AB34). */
    i32 chl[256], nch = 0;
    chl[nch++] = 0;
    for (i32 i = 0; i < g_log_n && nch < 256; i++) {
        i32 k = 0;
        while (k < nch && chl[k] != g_log_who[i]) k++;
        if (k == nch) chl[nch++] = g_log_who[i];
    }
    for (i32 a = 0; a < nch; a++)
        for (i32 b = 0; b < nch - 1; b++)
            if (chl[b + 1] < chl[b]) { i32 t = chl[b]; chl[b] = chl[b + 1]; chl[b + 1] = t; }

    static c8 txt[LOG_MAX][512];
    static u8 have[LOG_MAX];
    memset(have, 0, sizeof(have));

    wav_t click = {0};
    if (state->game_path) {
        c8 p[512];
        snprintf(p, sizeof(p), "%s/wav/ANIMS/INVENT1.WAV", state->game_path);
        wav_load(&click, p);
    }

    sys->motion_x = 0x208; sys->motion_y = 0xE6;            /* ls_set_mouse */
    i32 prev[256];
    memcpy(prev, sys->actions, sizeof(prev));
    #define EDGE(k) (sys->actions[k] && !prev[k])
    i32 cur = g_log_n - 1;                                  /* var_4158 */
    i32 ci = 0;                                             /* var_400C */
    i32 key_btn = 0, mouse_btn = 0, done = 0;
    u32 opened = ls_tick();

    while (!sys->quit && !done) {
        ls_update_events(sys);
        sfx_pump();
        u32 now = ls_tick();
        i32 enter = EDGE(13), home = EDGE(36);
        i32 click_edge = sys->mouse_left == 2, held = sys->mouse_left != 0;
        i32 who = chl[ci], act = 0;

        /* Keys act on release (var_402C / var_4). */
        i32 kd = 0;
        for (i32 i = 0; i < 5; i++) if (sys->actions[BTN_KEY[i]]) kd = i + 1;
        if (!kd && key_btn) act = key_btn;
        key_btn = kd;
        memcpy(prev, sys->actions, sizeof(prev));
        if (getenv("DKE_FRAMES") && now - opened > 1500) break;   /* headless */

        i32 mx = sys->motion_x, my = sys->motion_y;
        i32 hit = hit_test(mx, my);
        if (click_edge && hit == 6) enter = 1;                    /* 0x46B15F */
        else if (click_edge && hit) mouse_btn = hit;
        if (!held && mouse_btn) { if (hit == mouse_btn) act = mouse_btn; mouse_btn = 0; }
        if (mouse_btn && hit != mouse_btn) mouse_btn = 0;

        if (enter) {                                        /* 0x46B190 */
            sfx_play(&click);
            cur = find_prev(g_log_n, who);
        }
        if (home) { sfx_play(&click); ci = 0; }             /* 0x46B248 */
        if (act) sfx_play(&click);
        if (act == 1) cur = find_prev(cur, who);
        if (act == 2) cur = find_next(cur, who);
        if (act == 3 && ci > 0) ci--;
        if (act == 4 && ci < nch - 1) ci++;
        if (act == 5) done = 1;

        /* Draw. */
        blit_bmap_to_framebuffer(fb, fw, fh, bg, pal, 0, 0);
        /* Big speaker portrait: sub_46BA63 (P00 for everybody, no name). */
        draw_portrait(fb, fw, fh, chl[ci], 0x1F1, 0xC0);
        for (i32 i = 0; i < 3; i++) {
            i32 e = cur - 1 + i, py = 0x36 + 0x88 * i;
            if (e >= 0 && e < g_log_n) draw_portrait(fb, fw, fh, g_log_who[e], 8, py);
            else                       draw_portrait(fb, fw, fh, 0, 8, py);
            draw_text_panel(e, 0x97, 0x30 + 0x88 * i, txt, have);
        }
        /* Pressed button sprites (overlay_cursor_image on DIALIMAG). */
        i32 pressed = key_btn ? key_btn : (mouse_btn && hit == mouse_btn ? mouse_btn : 0);
        if (pressed >= 1 && pressed <= 5) {
            const btn_t *b = &BTN[pressed - 1];
            blit_sub(fb, fw, fh, img, ipal, b->sx, b->sy, b->w, b->h, b->dx, b->dy, 1);
        }
        if (curs) {
            if (hit == 0) blit_sub(fb, fw, fh, curs, cpal, 0x1E, 0, 0x20, 0x20, mx, my, 1);
            else          blit_sub(fb, fw, fh, curs, cpal, 0, 0, 0x1D, 0x20, mx, my, 1);
        }
        shot();
        fx_present();
        ls_delay(10);
    }
    #undef EDGE

    for (i32 i = 0; i < PORTRAITS; i++) { free_bmap(g_port[i]); g_port[i] = NULL; }
    free_bmap(g_names); g_names = NULL;
    free_bmap(curs); free_bmap(img); free_bmap(bg);
}
