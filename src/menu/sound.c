/*
 * sound.c — sub_440B92 — volume (MENUSON).
 *
 * Opened from the main menu. DATAS>MENUS>MENUSON.GIF is the backdrop,
 * SONIMAG.GIF holds the button / slider sprites and CURS128.GIF the
 * pointer. Eight hit boxes (the 40-byte table at [ebp-0x150]):
 *
 *   0  exit button          (0x1C4,0x19D) 55x56   src (53,100) in SONIMAG
 *   1  music on/off icon    (0x213,0x47)  50x49   src (0,0)
 *   2  effects on/off icon  (0x213,0x96)  49x48   src (0,60)
 *   3  subtitles toggle     (0x216,0xE4)  51x50   src (0,156)   byte_4B60AC
 *   4  reverse stereo       (0x1C3,0x11D) 50x49   src (53,0)    byte_4B60A8
 *   5  voices toggle        (0x16F,0xE6)  48x47   src (53,52)   byte_4B60B0
 *   6  music slider         (0x1AD,0x64)  100x32  dword_4B60B4 (sub_48DFB0)
 *   7  effects slider       (0x1AC,0xB3)  32x100  dword_4B60B8 (sub_44D5BA)
 *
 * Keys: Up / Down walk the six buttons (the pointer jumps to the centre of
 * the chosen one), Enter presses it, Left / Right change the volume of the
 * icon under the pointer, Esc or the right button leaves.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu/screens.h"
#include "state.h"
#include "game_init.h"
#include "music.h"
#include "sfx.h"
#include "gamecfg.h"
#include "screen_fx.h"
#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/audio/wav.h"

typedef struct {
    i32 sx, sy, w, h;       /* +0 .. +C: sub-rect of SONIMAG */
    i32 dx, dy;             /* +10, +14: hit box and draw position */
} snd_btn_t;

static const snd_btn_t BTN[8] = {
    { 0x35, 0x64, 0x37, 0x38, 0x1C4, 0x19D },   /* exit */
    { 0x00, 0x00, 0x32, 0x31, 0x213, 0x047 },   /* music icon */
    { 0x00, 0x3C, 0x31, 0x30, 0x213, 0x096 },   /* effects icon */
    { 0x00, 0x9C, 0x33, 0x32, 0x216, 0x0E4 },   /* byte_4B60AC */
    { 0x35, 0x00, 0x32, 0x31, 0x1C3, 0x11D },   /* byte_4B60A8 */
    { 0x35, 0x34, 0x30, 0x2F, 0x16F, 0x0E6 },   /* byte_4B60B0 */
    { 0x00, 0x33, 0x64, 0x20, 0x1AD, 0x064 },   /* music slider */
    { 0x00, 0x33, 0x64, 0x20, 0x1AC, 0x0B3 },   /* effects slider */
};

/* Values that outlive the screen (dword_4B60B4 is g_music_option). */
/* Settings live in DKEGAME.CFG (gamecfg.c); sfx.c owns the volume and
 * reverse stereo flag. */

static wav_t s_click;           /* INVENT1.WAV */
static wav_t s_test;            /* head of L08AMB1.WAV, scaled to the volume */

static i32 clampv(i32 v) { return v < 0 ? 0 : (v > 64 ? 64 : v); }

static void snd_free_bmap(bmap_t *b) {
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
    for (i32 i = 0; i < 8; i++)
        if (BTN[i].dx < mx && BTN[i].dx + BTN[i].w > mx &&
            BTN[i].dy < my && BTN[i].dy + BTN[i].h > my) return i + 1;
    return 0;
}

static void load_click(void) {
    if (s_click.samples || !state || !state->game_path) return;
    c8 p[512];
    snprintf(p, sizeof(p), "%s/wav/ANIMS/INVENT1.WAV", state->game_path);
    wav_load(&s_click, p);
}

/* sub_44CFF6 / ls_set_sound_volume: attenuation in 1/100 dB. */
static float vol_gain(i32 v) {
    if (v <= 0) return 0.0f;
    return powf(10.0f, -(float)((64 - v) * 32) / 2000.0f);
}

/* ls_play_sound on the test sample: only a short head, replayed while the
 * effects slider is held. */
static void play_test(i32 vol) {
    if (!s_test.samples) return;
    wav_t t = s_test;
    u32 n = t.num_samples * t.channels;
    i16 *buf = (i16 *)malloc(n * sizeof(i16));
    if (!buf) return;
    float g = vol_gain(vol);
    for (u32 i = 0; i < n; i++) buf[i] = (i16)(s_test.samples[i] * g);
    t.samples = buf;
    sfx_play(&t);
    free(buf);
}

static void load_test(void) {
    wav_t w = {0};
    c8 p[512];
    snprintf(p, sizeof(p), "%s/datas/sound/L08AMB1.WAV", state->game_path);
    if (!wav_load(&w, p)) return;
    u32 keep = w.sample_rate / 4;                         /* 250 ms */
    if (w.num_samples > keep) w.num_samples = keep;
    s_test = w;
}

static void set_music(i32 v) {
    g_music_option = clampv(v);
    if (music_is_playing()) music_volume(g_music_option);   /* dword_4B729C */
}

static void set_sfx(i32 v) {
    sfx_set_master(clampv(v));                              /* sub_44D5BA(1) */
}

static void shot(system_t *sys) {
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
    (void)sys;
}

void menu_sound_run(void) {
    if (!state || !state->graphic.front_buffer) return;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    u32 *fb = state->graphic.front_buffer;
    system_t *sys = &state->system;

    bmap_t *bg   = game_init_load_cursor("DATAS>MENUS>MENUSON.GIF");
    bmap_t *img  = game_init_load_cursor("DATAS>MENUS>SONIMAG.GIF");
    bmap_t *curs = game_init_load_cursor("DATAS>GLOBAL>IMAGES>CURS128.GIF");
    if (!curs) curs = game_init_load_cursor("DATAS>MENUS>CURS128.GIF");
    if (!bg || !img) { snd_free_bmap(bg); snd_free_bmap(img); snd_free_bmap(curs); return; }
    const u8 *pal  = bg->palette ? bg->palette : img->palette;   /* SONIMAG uses color_table */
    const u8 *cpal = (curs && curs->palette) ? curs->palette : pal;

    load_click();
    load_test();

    sys->motion_x = 0x208; sys->motion_y = 0xE6;            /* ls_set_mouse */
    i32 prev[256];
    memcpy(prev, sys->actions, sizeof(prev));
    #define EDGE(k) (sys->actions[k] && !prev[k])
    u32 opened = ls_tick(), last_step = 0, last_test = 0;
    i32 focus = -1;                                         /* var_168 */
    b8 exit_pressed = 0;                                    /* var_154 */

    while (!sys->quit) {
        ls_update_events(sys);
        sfx_pump();
        u32 now = ls_tick();
        i32 key_up = EDGE(38), key_dn = EDGE(40), key_enter = EDGE(13), key_esc = EDGE(27);
        i32 held_l = sys->actions[37], held_r = sys->actions[39];
        i32 click = sys->mouse_left == 2;
        i32 btn = sys->mouse_right ? 2 : (sys->mouse_left ? 1 : 0);   /* var_16C */
        memcpy(prev, sys->actions, sizeof(prev));
        if (getenv("DKE_FRAMES") && now - opened > 1500) key_esc = 1;   /* headless */
        if (key_esc || exit_pressed || btn == 2) break;

        /* 0x440FD5: Down / Up walk the six buttons. */
        i32 moved = 0;
        if (key_dn && focus >= 0 && focus < 6) {
            static const i32 nx[6] = { 0, 2, 3, 5, 0, 4 };
            focus = nx[focus]; moved = 1;
        }
        if (key_up) {
            static const i32 up[7] = { 0, 4, -2, 1, 2, 5, 3 };   /* index focus + 1 */
            i32 u = up[focus + 1];
            if (u != -2) focus = u;
            moved = 1;
        }
        if (moved && focus >= 0) {
            sys->motion_x = BTN[focus].dx + BTN[focus].w / 2;
            sys->motion_y = BTN[focus].dy + BTN[focus].h / 2;
            sfx_play(&s_click);
        }

        i32 mx = sys->motion_x, my = sys->motion_y;
        i32 hit = hit_test(mx, my);
        i32 act = click || key_enter;
        i32 show_exit = 0;

        /* 0x441300: Left / Right on an icon change that volume. */
        if ((held_l || held_r) && (hit == 2 || hit == 3) && now - last_step >= 40) {
            i32 d = held_l ? -1 : 1;
            last_step = now;
            if (hit == 2) set_music(g_music_option + d);
            else          set_sfx(sfx_get_master() + d);
        }

        if (act) switch (hit) {
        case 1: exit_pressed = 1; show_exit = 1; sfx_play(&s_click); break;
        case 2: sfx_play(&s_click); set_music(g_music_option ? 0 : 0x3F); break;
        case 3: sfx_play(&s_click); set_sfx(sfx_get_master() ? 0 : 0x3F); break;
        case 4:
            sfx_play(&s_click);
            g_gamecfg.subtitles ^= 1;
            if (!g_gamecfg.subtitles && !g_gamecfg.voices) g_gamecfg.voices = 1;
            break;
        case 5:
            sfx_play(&s_click);
            sfx_set_reverse_stereo(!sfx_get_reverse_stereo());
            break;
        case 6:
            sfx_play(&s_click);
            g_gamecfg.voices = !g_gamecfg.voices;                    /* abs(b0 - 1) */
            if (!g_gamecfg.subtitles && !g_gamecfg.voices) g_gamecfg.subtitles = 1;
            break;
        default: break;
        }
        /* Sliders follow the held button. */
        if (btn == 1 && hit == 7) set_music((mx - BTN[6].dx - 20) * 64 / 0x3D);
        if (btn == 1 && hit == 8) {
            if (now - last_test >= 250) { play_test(sfx_get_master()); last_test = now; }
            set_sfx((mx - BTN[7].dx - 20) * 64 / 0x3D);
        }

        /* Draw. */
        blit_bmap_to_framebuffer(fb, fw, fh, bg, pal, 0, 0);
        if (show_exit)
            blit_sub(fb, fw, fh, img, pal, BTN[0].sx, BTN[0].sy, BTN[0].w, BTN[0].h, BTN[0].dx, BTN[0].dy);
        if (g_music_option)
            blit_sub(fb, fw, fh, img, pal, BTN[1].sx, BTN[1].sy, BTN[1].w, BTN[1].h, BTN[1].dx, BTN[1].dy);
        if (sfx_get_master())
            blit_sub(fb, fw, fh, img, pal, BTN[2].sx, BTN[2].sy, BTN[2].w, BTN[2].h, BTN[2].dx, BTN[2].dy);
        if (g_gamecfg.subtitles)
            blit_sub(fb, fw, fh, img, pal, BTN[3].sx, BTN[3].sy, BTN[3].w, BTN[3].h, BTN[3].dx, BTN[3].dy);
        if (sfx_get_reverse_stereo())
            blit_sub(fb, fw, fh, img, pal, BTN[4].sx, BTN[4].sy, BTN[4].w, BTN[4].h, BTN[4].dx, BTN[4].dy);
        if (g_gamecfg.voices)
            blit_sub(fb, fw, fh, img, pal, BTN[5].sx, BTN[5].sy, BTN[5].w, BTN[5].h, BTN[5].dx, BTN[5].dy);
        /* Slider knobs: 32 x 8 from (0,51). */
        blit_sub(fb, fw, fh, img, pal, 0, 0x33, 0x20, 8, 0x1B0 + g_music_option * 60 / 64, 0x6C);
        blit_sub(fb, fw, fh, img, pal, 0, 0x33, 0x20, 8, 0x1B1 + sfx_get_master() * 60 / 64, 0xBB);
        /* Pointer: 32 x 32, the hand over a button. */
        if (curs) {
            if (hit == 0) blit_sub(fb, fw, fh, curs, cpal, 0x1E, 0, 0x20, 0x20, mx, my);
            else          blit_sub(fb, fw, fh, curs, cpal, 0, 0, 0x1D, 0x20, mx, my);
        }
        shot(sys);
        fx_present();
        ls_delay(10);
    }
    #undef EDGE

    sfx_play(&s_click);                                     /* 0x441C09 */
    snd_free_bmap(curs);
    snd_free_bmap(img);
    snd_free_bmap(bg);
}
