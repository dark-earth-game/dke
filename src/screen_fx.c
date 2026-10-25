/*
 * screen_fx.c — the engine's screen transition (sub_42E0E0 cloudy dissolve
 * to black) and the "keep black" flag (dword_4B69BC).
 */

#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/system.h"
#include "state.h"
#include "sfx.h"
#include "screen_fx.h"

/*
 * sub_42E0E0 @ 0x42E0E0 (ecx = duration ms). Blocking.
 *
 *  - plays "trans" from the ANIMS sound dir (byte_4FB1F0 temporarily = byte_4FC010)
 *  - locks graphics_context (the visible surface) and writes into it directly.
 *    No flip is called, so each pass shows up as soon as it is written.
 *  - builds a 257x257 midpoint-displacement "plasma" map: centre 0xFF, edges
 *    and corners 0, random offset 0.95 * (rand in [0, hi-lo] - (hi-lo)/2)
 *    around the midpoint. The result is a cloud that is low at the edges and
 *    high in the middle.
 *  - maps the 640x480 screen onto 256x256 cells (col = (i32)(x * 257/640),
 *    row = (i32)(y * 256/480)). Each cell is about 2-3 px wide and 1-2 lines tall.
 *  - each pass: t = (elapsed * 256) / duration (clamped to 255). Every cell
 *    whose map value is in [prev, t] is filled with 0 (black: palette index 0
 *    in 8-bit, 0x0000 in 16-bit).
 *  - stops after the pass with t == 255, which leaves the screen fully black.
 * Visually it is a cloudy dissolve to black: it starts at the screen edges and
 * corners and closes on the centre over `duration` ms. It does not fade the
 * palette, does not wipe, and the old image is never dissolved into a new one.
 * The loop busy-waits on system_get_gametime and pumps no input.
 * duration == 0 divides by zero in the original.
 *
 * Port: the LIEU callbacks run inside scene_render before it draws, so
 * front_buffer still holds the last presented frame. Dissolve it in place
 * and present each pass yourself, the way ls_play_movie does.
 */

i32 g_screen_keep_black = 0;                   /* dword_4B69BC */

static i32 fx_rand(i32 n) {                    /* ls_random -> lsys_random: (u16)n * rand() / 0x7FFF */
    return (i32)(((u32)(u16)n * (u32)(rand() & 0x7FFF)) / 0x7FFFu);
}

static u8 fx_between(i32 lo, i32 hi) {         /* sub_42DF9E (ecx = lo, edx = hi) */
    if (hi == lo) return (u8)lo;
    i32 r = fx_rand(hi - lo) & 0xFFFF;
    i32 v = r - (hi - lo) / 2;
    i32 d = (i32)((f64)v * 0.95);              /* dbl_4B0510 */
    return (u8)(d + (hi + lo) / 2);
}
static u8 fx_mid2(i32 a, i32 b) {              /* sub_42E013 */
    return a < b ? fx_between(a, b) : fx_between(b, a);
}
static u8 fx_mid4(i32 a, i32 b, i32 c, i32 d) { /* sub_42E060 */
    i32 lo = a, hi = a;
    if (b < lo) lo = b; if (c < lo) lo = c; if (d < lo) lo = d;
    if (b > hi) hi = b; if (c > hi) hi = c; if (d > hi) hi = d;
    return fx_between(lo, hi);
}

void fx_present(void) {
    if (!state) return;
    ls_blit(&state->system, state->graphic.front_buffer);
    ls_flip(&state->system);
    ls_update_events(&state->system);
}

void screen_dissolve(i32 duration_ms) {
    if (!state || !state->graphic.front_buffer) return;
    sfx_play_anims("trans");
    if (getenv("DKE_NOTRANS")) return;         /* headless runs */
    if (duration_ms <= 0) duration_ms = 1;     /* original: idiv by 0 */

    u32 *fb = state->graphic.front_buffer;
    i32  w  = state->graphic.width, h = state->graphic.height;

    /* 0x42E17C: plasma map, 0x10201 bytes. */
    u8 *map = (u8 *)calloc(257 * 257, 1);
    if (!map) return;
    map[128 * 257 + 128] = 0xFF;
    for (i32 step = 0x80; step > 1; step /= 2) {
        i32 half = step / 2;
        for (i32 i = 0; i <= 256 - step; i += step)          /* column (outer) */
            for (i32 j = 0; j <= 256 - step; j += step) {    /* row (inner)    */
                u8 *p = map + j * 257 + i;
                u8 a = p[0], b = p[step], c = p[step * 257 + step], d = p[step * 257];
                p[half]                = fx_mid2(a, b);
                p[half * 257]          = fx_mid2(a, d);
                p[half * 257 + half]   = fx_mid4(a, b, c, d);
                p[half * 257 + step]   = fx_mid2(a, c);      /* sic: a/c, not b/c */
                p[step * 257 + half]   = fx_mid2(d, c);
            }
    }

    /* 0x42E3CD: cell -> pixel spans. Exact for 640x480 (257/640 = 0.4015625,
     * 256/480 = 0.5333..); scaled for other framebuffer sizes. */
    i32 xs[257], ys[257];
    for (i32 x = w - 1; x >= 0; x--) {
        i32 c = (i32)((f64)x * (257.0 / w));
        if (c <= 256) xs[c] = x;
    }
    xs[256] = w;                                /* var_10C = 0x280 */
    for (i32 y = h - 1; y >= 0; y--) {
        i32 r = (i32)((f64)y * (256.0 / h));
        if (r < 256) ys[r] = y;
    }
    ys[256] = h;                                /* var_524 = pitch * 480 */

    u32 t0 = ls_tick(), last = t0;
    i32 prev = 0;
    while (prev < 0xFF) {                       /* 0x42E584 */
        u8 on[256] = {0};
        i32 t = (i32)(((last - t0) << 8) / (u32)duration_ms);
        if (t > 0xFF) t = 0xFF;
        for (i32 k = prev; k <= t; k++) on[k] = 1;
        for (i32 r = 0; r < 256; r++) {
            const u8 *row = map + r * 257;
            i32 y0 = ys[r], lines = ys[r + 1] - y0;   /* original: 1 or 2 lines */
            if (lines > 2) lines = 2;                 /* faithful at 480 lines */
            for (i32 c = 0; c < 256; c++) {
                if (!on[row[c]]) continue;
                for (i32 l = 0; l < lines; l++) {
                    u32 *dst = fb + (size_t)(y0 + l) * w;
                    for (i32 x = xs[c]; x < xs[c + 1]; x++) dst[x] = 0xFF000000u;
                }
            }
        }
        prev = t;
        fx_present();
        ls_delay(1);
        last = ls_tick();
    }
    free(map);
}

