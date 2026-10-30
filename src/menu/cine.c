/*
 * cine.c — sub_441E43 — cinematics (MENUCINE).
 *
 * DATAS>MENUS>MENUCINE.GIF is the backdrop, CINEIMAG.GIF the button sprites
 * and CURS128.GIF the pointer. A 225x181 stamp (DATAS>STAMPS>SCnn.GIF) shows
 * the current movie; the arrows slide to the next / previous movie that the
 * player has already seen (byte_507350, bit n = SCnn; SC01 is the start page
 * and always available), the stamp replays it. Hit boxes (sub_43F7BD, table
 * at [ebp-0xC0], 4 entries):
 *
 *   1  exit button   (0x1DB,0x19F) 53x53   src (0,0)     in CINEIMAG
 *   2  next arrow    (0x145,0x14A) 17x16   src (0x35,0x10)   sub_441D2F
 *   3  prev arrow    (0x144,0x17F) 18x16   src (0x35,0)      sub_441DB9
 *   4  stamp         (0x171,0x73)  225x115                    play
 *
 * Keys: Up / Down = next / previous movie, Enter = play, Esc or the right
 * button leaves. Not ported: subtitles (DATAS>SBT>SCnn_<lang>.TXT, the
 * port's movie player has none), the CD/HDD path juggling (sub_469EE8 /
 * sub_469E34) and the debug "scan SC>SCnn.CRH" cheat at 0x442000.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu/screens.h"
#include "state.h"
#include "game_init.h"
#include "sfx.h"
#include "screen_fx.h"
#include "lieu_cb.h"
#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/movie.h"
#include "libsys/graphics/graphic.h"
#include "libsys/audio/wav.h"

#define STAMP_W   0xE1
#define STAMP_H   0xB5
#define STAMP_X   0x171
#define STAMP_Y   0x73
#define NUM_MOVIES 0x24

typedef struct { i32 sx, sy, w, h, dx, dy; } cine_btn_t;

static const cine_btn_t BTN[4] = {
    { 0x00, 0x00, 0x35, 0x35, 0x1DB, 0x19F },   /* exit */
    { 0x35, 0x10, 0x11, 0x10, 0x145, 0x14A },   /* next (higher index) */
    { 0x35, 0x00, 0x12, 0x10, 0x144, 0x17F },   /* previous */
    { 0x00, 0x00, STAMP_W, 0x73, STAMP_X, STAMP_Y },   /* stamp: play */
};

static wav_t s_click;

static void cine_free_bmap(bmap_t *b) {
    if (!b) return;
    if (b->owns_buffer && b->pixels) free(b->pixels);
    if (b->owns_palette && b->palette) free(b->palette);
    free(b);
}

static void blit_sub(u32 *fb, i32 fw, i32 fh, const bmap_t *src, const u8 *pal,
                     i32 sx, i32 sy, i32 sw, i32 sh, i32 dx, i32 dy) {
    if (!src || !src->pixels || !pal) return;
    for (i32 r = 0; r < sh; r++) {
        i32 y = dy + r, ys = sy + r;
        if (y < 0 || y >= fh || ys < 0 || ys >= src->height) continue;
        for (i32 c = 0; c < sw; c++) {
            i32 x = dx + c, xs = sx + c;
            if (x < 0 || x >= fw || xs < 0 || xs >= (i32)src->width) continue;
            u8 idx = src->pixels[ys * src->width + xs];
            if (idx == 0) continue;                       /* index 0 is transparent */
            fb[y * fw + x] = 0xFF000000u | ((u32)pal[idx * 3 + 2] << 16) |
                             ((u32)pal[idx * 3 + 1] << 8) | pal[idx * 3];
        }
    }
}

/* sub_43F7BD: strict inside test, returns index + 1 or 0. */
static i32 hit_test(i32 mx, i32 my) {
    for (i32 i = 0; i < 4; i++)
        if (BTN[i].dx < mx && BTN[i].dx + BTN[i].w > mx &&
            BTN[i].dy < my && BTN[i].dy + BTN[i].h > my) return i + 1;
    return 0;
}

/* DKE_CINE_ALL (test hook, not in the original): mark every movie as seen so
 * the screen can be exercised headless on a fresh game. */
static i32 seen(i32 n) {
    if (getenv("DKE_CINE_ALL")) return 1;
    return (g_movie_seen[(n >> 3) & 7] >> (n & 7)) & 1;
}

/* sub_441D2F: next seen movie above `cur`, -1 if none. */
static i32 next_seen(i32 cur) {
    if (cur == NUM_MOVIES - 1) return -1;
    for (i32 i = 0; i < NUM_MOVIES; i++)
        if (cur < i && seen(i)) return i;
    return -1;
}

/* sub_441DB9: previous seen movie below `cur`, -1 if none (SC01 is the floor). */
static i32 prev_seen(i32 cur) {
    if (cur == 1) return -1;
    for (i32 i = NUM_MOVIES - 1; i >= 0; i--)
        if (cur > i && seen(i)) return i;
    return -1;
}

/* sub_47545A: load a stamp as 225x181 RGB (black when the file is missing). */
static void load_stamp(u32 *dst, i32 n) {
    c8 p[64];
    snprintf(p, sizeof(p), "DATAS>STAMPS>SC%02d.GIF", n);
    for (i32 i = 0; i < STAMP_W * STAMP_H; i++) dst[i] = 0xFF000000u;
    bmap_t *b = game_init_load_cursor(p);
    if (!b || !b->pixels || !b->palette) { cine_free_bmap(b); return; }
    for (i32 y = 0; y < STAMP_H && y < b->height; y++)
        for (i32 x = 0; x < STAMP_W && x < (i32)b->width; x++) {
            u8 idx = b->pixels[y * b->width + x];
            const u8 *c = b->palette + idx * 3;
            dst[y * STAMP_W + x] = 0xFF000000u | ((u32)c[2] << 16) | ((u32)c[1] << 8) | c[0];
        }
    cine_free_bmap(b);
}

/* draw_image of a stamp sub-rect rows [y0,y1) at screen row dy. */
static void stamp_rows(u32 *fb, i32 fw, i32 fh, const u32 *st, i32 y0, i32 y1, i32 dy) {
    for (i32 r = y0; r < y1; r++) {
        i32 y = dy + (r - y0);
        if (y < 0 || y >= fh) continue;
        memcpy(fb + y * fw + STAMP_X, st + r * STAMP_W, STAMP_W * sizeof(u32));
    }
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

void menu_cine_run(void) {
    if (!state || !state->graphic.front_buffer) return;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    u32 *fb = state->graphic.front_buffer;
    system_t *sys = &state->system;

    bmap_t *bg   = game_init_load_cursor("DATAS>MENUS>MENUCINE.GIF");
    bmap_t *img  = game_init_load_cursor("DATAS>MENUS>CINEIMAG.GIF");
    bmap_t *curs = game_init_load_cursor("DATAS>GLOBAL>IMAGES>CURS128.GIF");
    if (!curs) curs = game_init_load_cursor("DATAS>MENUS>CURS128.GIF");
    if (!bg || !img) { cine_free_bmap(bg); cine_free_bmap(img); cine_free_bmap(curs); return; }
    const u8 *pal  = bg->palette ? bg->palette : img->palette;   /* CINEIMAG uses color_table */
    const u8 *cpal = (curs && curs->palette) ? curs->palette : pal;

    if (!s_click.samples && state->game_path) {
        c8 p[512];
        snprintf(p, sizeof(p), "%s/wav/ANIMS/INVENT1.WAV", state->game_path);
        wav_load(&s_click, p);
    }

    u32 *cur_st = malloc(STAMP_W * STAMP_H * sizeof(u32));   /* var_D0 */
    u32 *new_st = malloc(STAMP_W * STAMP_H * sizeof(u32));   /* var_8 */
    if (!cur_st || !new_st) { free(cur_st); free(new_st); return; }
    i32 cur = 1;                                             /* var_D4 */
    load_stamp(cur_st, cur);

    sys->motion_x = 0x208; sys->motion_y = 0xE6;             /* ls_set_mouse */
    i32 prev[256];
    memcpy(prev, sys->actions, sizeof(prev));
    #define EDGE(k) (sys->actions[k] && !prev[k])
    u32 opened = ls_tick(), anim_t0 = 0;
    i32 anim = 0;                                            /* var_14: 1 = next, 2 = prev */
    i32 pressed = 0;                                         /* button drawn pressed (0 = none) */

    while (!sys->quit) {
        ls_update_events(sys);
        sfx_pump();
        u32 now = ls_tick();
        i32 key_up = EDGE(38), key_dn = EDGE(40), key_enter = EDGE(13), key_esc = EDGE(27);
        i32 click = sys->mouse_left == 2;
        i32 btn = sys->mouse_right ? 2 : (sys->mouse_left ? 1 : 0);   /* var_1F0 */
        memcpy(prev, sys->actions, sizeof(prev));
        if (getenv("DKE_FRAMES") && now - opened > 1500) key_esc = 1;   /* headless */
        if (key_esc || btn == 2) break;

        i32 mx = sys->motion_x, my = sys->motion_y;
        i32 hit = hit_test(mx, my);

        i32 act = 0;                                         /* var_1E8 */
        if (click && hit) act = hit;
        if (key_up) act = 2;
        if (key_dn) act = 3;
        if (key_enter) act = 4;
        if (act == 1) break;                                 /* var_C4: leave */

        if (!anim && (act == 2 || act == 3)) {
            i32 n = act == 2 ? next_seen(cur) : prev_seen(cur);
            if (n != -1) {
                sfx_play(&s_click);
                load_stamp(new_st, n);
                cur = n;
                anim = act == 2 ? 1 : 2;
                anim_t0 = now;
            }
        }
        if (!anim && act == 4) {
            c8 path[64];
            snprintf(path, sizeof(path), "HDISK:>EARTH>sc>SC%02d.CRH", cur);   /* 0x44261C */
            ls_play_movie(path);                             /* no subtitles in the port's player */
            memcpy(prev, sys->actions, sizeof(prev));
        }

        /* Backdrop, then the stamp (0x442287..0x44244A). */
        blit_bmap_to_framebuffer(fb, fw, fh, bg, pal, 0, 0);
        if (anim) {
            /* 181 px per 200 ms. */
            i32 t = (i32)((now - anim_t0) * 181 / 200);
            if (t >= STAMP_H) {
                t = STAMP_H;
                anim = 0;
                memcpy(cur_st, new_st, STAMP_W * STAMP_H * sizeof(u32));
            }
            if (anim == 1) {            /* slides down, new one enters from the top */
                stamp_rows(fb, fw, fh, new_st, STAMP_H - t, STAMP_H, STAMP_Y);
                stamp_rows(fb, fw, fh, cur_st, 0, STAMP_H - t, STAMP_Y + t);
            } else if (anim == 2) {     /* slides up, new one enters from below */
                stamp_rows(fb, fw, fh, new_st, 0, t, STAMP_Y + STAMP_H - t);
                stamp_rows(fb, fw, fh, cur_st, t, STAMP_H, STAMP_Y);
            } else {
                stamp_rows(fb, fw, fh, cur_st, 0, STAMP_H, STAMP_Y);
            }
        } else {
            stamp_rows(fb, fw, fh, cur_st, 0, STAMP_H, STAMP_Y);
        }

        /* Pressed button overlay (0x4427A9 / 0x442831 / 0x4428BD). */
        pressed = (btn == 1 && hit && hit < 4) ? hit : 0;
        if (key_up) pressed = 2;
        if (key_dn) pressed = 3;
        if (pressed)
            blit_sub(fb, fw, fh, img, pal, BTN[pressed - 1].sx, BTN[pressed - 1].sy,
                     BTN[pressed - 1].w, BTN[pressed - 1].h, BTN[pressed - 1].dx, BTN[pressed - 1].dy);

        /* Pointer: 32 x 32, the hand over a button. */
        if (curs) {
            if (hit == 0) blit_sub(fb, fw, fh, curs, cpal, 0x1E, 0, 0x20, 0x20, mx, my);
            else          blit_sub(fb, fw, fh, curs, cpal, 0, 0, 0x1D, 0x20, mx, my);
        }
        shot();
        fx_present();
        ls_delay(10);
    }
    #undef EDGE

    sfx_play(&s_click);                                      /* 0x4429DC */
    free(cur_st);
    free(new_st);
    cine_free_bmap(curs);
    cine_free_bmap(img);
    cine_free_bmap(bg);
}
