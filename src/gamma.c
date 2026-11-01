/*
 * gamma.c — brightness / contrast / saturation colour correction.
 *
 *   0x40C920 update_palette            brightness add, then contrast + saturation
 *   0x40CA80 convert_rgb_to_color_space  RGB -> (hue, saturation, intensity)
 *   0x40CBF0 process_color_values        (hue, saturation, intensity) -> RGB
 *
 * All three work on 6-bit components. The sliders (rgb_brightness_slider_control
 * 0x4B21AC, rgb_contrast_slider_control 0x4B21B4, rgb_saturation_slider_control
 * 0x4B21BC) are 0..16 in the picker; contrast scales the intensity by
 * c/24 + 1, saturation by s/16 + 0.8, and the correction is skipped when both
 * factors are exactly 1.0. Hex-Rays dropped the x87 compare flags, they were
 * read from the asm: every "if ( !(c0 | c3) )" is a strict "greater than".
 */

#include <math.h>
#include <string.h>

#include "gamma.h"
#include "gamecfg.h"

#define PI_3_2   2.094395102393195      /* 2 pi / 3 (dbl_4B0178) */
#define PI_3_4   4.188790204786391      /* 4 pi / 3 */
#define PI_3_1   1.047197551196598      /* pi / 3 */

/* __ftol: truncating fistp, the low dword is stored. NaN gives 0. */
static i32 ftol(f64 d) {
    if (d != d || d >= 9.2e18 || d <= -9.2e18) return 0;
    return (i32)(i64)d;
}

/* 0040CA80 */
static void convert_rgb_to_color_space(i32 ri, i32 gi, i32 bi, f64 *hue, f64 *sat, f64 *inten) {
    f64 r = (f64)ri * 0.01587301587301587;
    f64 g = (f64)gi * 0.01587301587301587;
    f64 b = (f64)bi * 0.01587301587301587;
    f64 sum = b + g + r;
    *inten = sum * 0.3333333333333333;
    f64 mn = g;
    if (r <= g) mn = r;
    if (b <= mn) mn = b;
    *sat = mn / sum * -3.0 + 1.0;
    if (*sat == 0.0 || *sat != *sat) {          /* fcom + test ah,40h: equal or unordered */
        *hue = 0.0;
    } else {
        f64 v = (r * 2.0 - g - b) / (sqrt((r - b) * (g - b) + (r - g) * (r - g)) * 2.0);
        if (v > 1.0) v = 1.0;
        if (v < -1.0) v = -1.0;
        *hue = acos(v);
        if (g < b) *hue = 6.283185307179586 - *hue;
    }
}

/* 0040CBF0 */
static void process_color_values(i32 *r, i32 *g, i32 *b, f64 hue, f64 sat, f64 inten) {
    f64 v20, v21, v22;
    if (hue > PI_3_2) {
        if (hue > PI_3_4) {
            v21 = 1.0 - sat;
            v22 = cos(hue - PI_3_4) / cos(PI_3_1 - (hue - PI_3_4)) * sat + 1.0;
            v20 = 3.0 - (v22 + v21);
        } else {
            v20 = 1.0 - sat;
            v21 = cos(hue - PI_3_2) / cos(PI_3_1 - (hue - PI_3_2)) * sat + 1.0;
            v22 = 3.0 - (v21 + v20);
        }
    } else {
        v22 = 1.0 - sat;
        v20 = cos(hue) / cos(PI_3_1 - hue) * sat + 1.0;
        v21 = 3.0 - (v20 + v22);
    }
    *r = ftol(v20 * inten * 63.0);
    *g = ftol(v21 * inten * 63.0);
    *b = ftol(v22 * inten * 63.0);
    if (*r < 0) *r = 0;
    if (*g < 0) *g = 0;
    if (*b < 0) *b = 0;
    /* Scale back so no component passes 63, red first, then green, then blue. */
    f64 v = (f64)*r;
    if (v > 63.0) {
        *r = ftol(v * (63.0 / v));
        *g = ftol((f64)*g * (63.0 / v));
        *b = ftol(63.0 / v * (f64)*b);
    }
    v = (f64)*g;
    if (v > 63.0) {
        f64 k = 63.0 / v;
        *r = ftol((f64)*r * k);
        *g = ftol((f64)*g * k);
        *b = ftol(k * (f64)*b);
    }
    v = (f64)*b;
    if (v > 63.0) {
        f64 k = 63.0 / v;
        *r = ftol((f64)*r * k);
        *g = ftol((f64)*g * k);
        *b = ftol(k * (f64)*b);
    }
}

/* 0040C920 */
void gamma_update_palette(i32 *r, i32 *g, i32 *b, i32 brightness, i32 contrast, i32 saturation) {
    *r += brightness; if (*r > 63) *r = 63;
    *g += brightness; if (*g > 63) *g = 63;
    *b += brightness; if (*b > 63) *b = 63;
    f64 sat_k = (f64)saturation * 0.0625 + 0.8;
    f64 con_k = (f64)contrast * 0.04166666666666666 + 1.0;
    if (con_k != 1.0 || sat_k != 1.0) {
        f64 hue, sat, inten;
        convert_rgb_to_color_space(*r, *g, *b, &hue, &sat, &inten);
        inten *= con_k;
        if (inten > 1.0) inten = 1.0;
        sat *= sat_k;
        if (sat > 1.0) sat = 1.0;
        process_color_values(r, g, b, hue, sat, inten);
    }
}

/* ---- port: apply to the shown frame --------------------------------- */

static b8 s_bypass;

void gamma_bypass(b8 on) { s_bypass = on; }

b8 gamma_active(void) {
    return !s_bypass && g_gamecfg.colour_chosen &&
           (g_gamecfg.brightness || g_gamecfg.contrast || g_gamecfg.saturation);
}

#define CACHE_BITS 16
static struct { u32 key; u32 val; } s_cache[1u << CACHE_BITS];
static i32 s_cached_for[3] = { 0x7FFFFFFF, 0, 0 };

static u32 correct_one(u32 p) {
    u32 key = p & 0x00FFFFFFu;
    u32 h = (key * 2654435761u) >> (32 - CACHE_BITS);
    if (s_cache[h].key == (key | 0x80000000u)) return (p & 0xFF000000u) | s_cache[h].val;
    i32 r = (i32)((p & 0xFF) >> 2), g = (i32)(((p >> 8) & 0xFF) >> 2), b = (i32)(((p >> 16) & 0xFF) >> 2);
    gamma_update_palette(&r, &g, &b, g_gamecfg.brightness, g_gamecfg.contrast, g_gamecfg.saturation);
    /* 6-bit back to 8-bit like the DAC does: v * 255 / 63. */
    u32 rr = (u32)(r * 255 / 63), gg = (u32)(g * 255 / 63), bb = (u32)(b * 255 / 63);
    u32 val = rr | (gg << 8) | (bb << 16);
    s_cache[h].key = key | 0x80000000u;
    s_cache[h].val = val;
    return (p & 0xFF000000u) | val;
}

static void cache_sync(void) {
    if (s_cached_for[0] != g_gamecfg.brightness || s_cached_for[1] != g_gamecfg.contrast ||
        s_cached_for[2] != g_gamecfg.saturation) {
        memset(s_cache, 0, sizeof(s_cache));
        s_cached_for[0] = g_gamecfg.brightness;
        s_cached_for[1] = g_gamecfg.contrast;
        s_cached_for[2] = g_gamecfg.saturation;
    }
}

void gamma_apply_pixels(u32 *px, i32 n) {
    if (!px || !gamma_active()) return;
    cache_sync();
    for (i32 i = 0; i < n; i++) px[i] = correct_one(px[i]);
}

void gamma_copy_pixels(u32 *dst, const u32 *src, i32 n) {
    memcpy(dst, src, (size_t)n * sizeof(u32));
    gamma_apply_pixels(dst, n);
}
