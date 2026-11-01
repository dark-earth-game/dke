/*
 * gamma.c — gamma picker (menu_process_gamma 0x442B29, handle_screen_gamma_picker 0x40ADE0).
 *
 * Shown on the first run (no DKEGAME.CFG, process_mainmenu_437EB7 0x43839C) before the
 * EADKE movie. DATAS>MENUS>REGLRGB.GIF is the full-screen backdrop and its palette is
 * the one in force; DATAS>GLOBAL>IMAGES>RGB.GIF is the 160x100 panel, with the slider
 * knob at (160,0)-(167,12) and the held (lit) knob at (160,13)-(167,25), and CURS128.GIF
 * the pointer. The panel is drawn at (240,190):
 *
 *   row 0  brightness  rgb_brightness_slider_control 0x4B21AC  knob y 24
 *   row 1  contrast    rgb_contrast_slider_control   0x4B21B4  knob y 52
 *   row 2  saturation  rgb_saturation_slider_control 0x4B21BC  knob y 80
 *
 * Each slider is 0..16; the knob x is 130 * value / 16 + 10. The pointer is kept in
 * x 240..392, y 190..282. Pressing the left button within a knob row (y + 190 < my <
 * y + 203) grabs it: the pointer is then locked to the row (y + 196), x 254..384, and
 * the value is 16 * (x - 250) / 130 with x clamped to 250..380. The picture is redrawn
 * with the new colours as the value changes (here the correction is applied to the
 * shown frame, gamma.h). Escape, Space, Enter or the right button leave.
 *
 * Left out: the in-game call (F10, process_menu 0x4080AC), which redraws the HUD
 * (render_game_background 0x40ACC0) instead of a backdrop.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu/screens.h"
#include "state.h"
#include "game_init.h"
#include "gamecfg.h"
#include "gamma.h"
#include "screen_fx.h"
#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/graphics/graphic.h"

#define PANEL_X 240
#define PANEL_Y 190
#define PANEL_W 160
#define PANEL_H 100

static const i32 ROW_Y[3] = { 24, 52, 80 };          /* v25, v26, v27 */

static i32 *slider(i32 i) {
    return i == 0 ? &g_gamecfg.brightness : i == 1 ? &g_gamecfg.contrast : &g_gamecfg.saturation;
}

static void free_bmap(bmap_t *b) {
    if (!b) return;
    if (b->owns_buffer && b->pixels) free(b->pixels);
    if (b->owns_palette && b->palette) free(b->palette);
    free(b);
}

/* draw_image(src, dst, x0, y0, x1, y1, dx, dy): the rect [x0,x1) x [y0,y1) copied to
 * (dx,dy); `keyed` skips index 0 (overlay_cursor_image). */
static void draw_rect(u32 *dst, i32 dw, i32 dh, const bmap_t *src, const u8 *pal,
                      i32 x0, i32 y0, i32 x1, i32 y1, i32 dx, i32 dy, b8 keyed) {
    if (!src || !src->pixels || !pal) return;
    for (i32 y = y0; y < y1; y++) {
        i32 py = dy + (y - y0);
        if (py < 0 || py >= dh || y < 0 || y >= src->height) continue;
        for (i32 x = x0; x < x1; x++) {
            i32 px = dx + (x - x0);
            if (px < 0 || px >= dw || x < 0 || x >= (i32)src->width) continue;
            u8 idx = src->pixels[y * src->width + x];
            if (keyed && idx == 0) continue;
            dst[py * dw + px] = 0xFF000000u | ((u32)pal[idx * 3 + 2] << 16) |
                                ((u32)pal[idx * 3 + 1] << 8) | pal[idx * 3];
        }
    }
}

static void shot(void) {
    const c8 *path = getenv("DKE_MENUSHOT");
    if (!path) return;
    FILE *f = fopen(path, "wb");
    if (!f) return;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    u32 *cfb = (u32 *)malloc((size_t)fw * fh * sizeof(u32));   /* as shown: colour corrected */
    if (cfb) gamma_copy_pixels(cfb, state->graphic.front_buffer, fw * fh);
    fprintf(f, "P6\n%d %d\n255\n", fw, fh);
    for (i32 i = 0; i < fw * fh; i++) {
        u32 p = cfb ? cfb[i] : state->graphic.front_buffer[i];
        fputc((u8)p, f); fputc((u8)(p >> 8), f); fputc((u8)(p >> 16), f);
    }
    free(cfb);
    fclose(f);
}

static i32 clampi(i32 v, i32 lo, i32 hi) { return v < lo ? lo : (v > hi ? hi : v); }

// 00442B29 + 0040ADE0
void menu_gamma_run(void) {
    if (!state || !state->graphic.front_buffer) return;
    system_t *sys = &state->system;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    u32 *fb = state->graphic.front_buffer;

    bmap_t *bg   = game_init_load_cursor("DATAS>MENUS>REGLRGB.GIF");
    bmap_t *rgb  = game_init_load_cursor("DATAS>GLOBAL>IMAGES>RGB.GIF");
    bmap_t *curs = game_init_load_cursor("DATAS>GLOBAL>IMAGES>CURS128.GIF");
    if (!bg || !rgb || !curs) { free_bmap(bg); free_bmap(rgb); free_bmap(curs); return; }
    const u8 *pal = bg->palette ? bg->palette : rgb->palette;      /* palette_data = REGLRGB's */
    if (!pal) { free_bmap(bg); free_bmap(rgb); free_bmap(curs); return; }

    g_gamecfg.colour_chosen = 1;           /* the picker shows the settings in force */

    /* Headless: DKE_GAMMA_SET=b,c,s sets the sliders as a drag would. */
    if (getenv("DKE_GAMMA_SET")) {
        i32 b = 0, c = 0, s = 0;
        if (sscanf(getenv("DKE_GAMMA_SET"), "%d,%d,%d", &b, &c, &s) == 3) {
            g_gamecfg.brightness = b; g_gamecfg.contrast = c; g_gamecfg.saturation = s;
        }
    }

    sys->motion_x = 320; sys->motion_y = 240;                       /* ls_set_mouse(320, 240) */
    i32 prev_esc = sys->actions[27], prev_sp = sys->actions[32], prev_en = sys->actions[13];
    i32 grab = -1;                                                  /* a3: the held row */
    b8  first_drag_frame = 0;
    u32 opened = ls_tick();
    u32 panel[PANEL_W * PANEL_H];

    while (!sys->quit) {
        ls_update_events(sys);
        i32 esc = sys->actions[27] && !prev_esc;
        i32 spc = sys->actions[32] && !prev_sp;
        i32 ent = sys->actions[13] && !prev_en;
        prev_esc = sys->actions[27]; prev_sp = sys->actions[32]; prev_en = sys->actions[13];
        i32 btn = sys->mouse_right ? 2 : (sys->mouse_left ? 1 : 0);
        if (getenv("DKE_FRAMES") && ls_tick() - opened > 1500) esc = 1;       /* headless */
        if (esc || spc || ent || btn == 2) break;

        i32 mx = sys->motion_x, my = sys->motion_y;
        if (grab < 0) {
            mx = clampi(mx, 240, 392);
            my = clampi(my, 190, 282);
            if (btn == 1) {
                for (i32 i = 0; i < 3; i++)
                    if (ROW_Y[i] + 190 < my && ROW_Y[i] + 203 > my) grab = i;
                first_drag_frame = (grab >= 0);
            }
        }
        if (grab >= 0) {
            if (btn != 1) {
                grab = -1;
            } else {
                if (!first_drag_frame) mx = clampi(mx, 254, 384);
                first_drag_frame = 0;
                my = ROW_Y[grab] + 196;
                i32 v = 16 * (clampi(mx, 250, 380) - 250) / 130;
                if (*slider(grab) != v) *slider(grab) = v;
            }
        }
        sys->motion_x = mx; sys->motion_y = my;

        /* The backdrop, then the panel, its knobs and the pointer. */
        draw_rect(fb, fw, fh, bg, pal, 0, 0, bg->width, bg->height, 0, 0, 0);
        draw_rect(panel, PANEL_W, PANEL_H, rgb, pal, 0, 0, PANEL_W, PANEL_H, 0, 0, 0);
        for (i32 i = 0; i < 3; i++) {
            i32 kx = 130 * *slider(i) / 16 + 10;
            if (i == grab) draw_rect(panel, PANEL_W, PANEL_H, rgb, pal, 160, 13, 167, 25, kx, ROW_Y[i], 0);
            else           draw_rect(panel, PANEL_W, PANEL_H, rgb, pal, 160, 0, 167, 12, kx, ROW_Y[i], 0);
        }
        draw_rect(panel, PANEL_W, PANEL_H, curs, pal, 0, 0, 29, 32, mx - PANEL_X, my - PANEL_Y, 1);
        for (i32 y = 0; y < PANEL_H; y++)
            memcpy(fb + (PANEL_Y + y) * fw + PANEL_X, panel + y * PANEL_W, PANEL_W * sizeof(u32));
        shot();
        fx_present();
        ls_delay(10);
    }

    /* flush_display_buffer: leave a black screen for the movies that follow. */
    memset(fb, 0, (size_t)fw * fh * sizeof(u32));
    free_bmap(curs);
    free_bmap(rgb);
    free_bmap(bg);
}
