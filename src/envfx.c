/*
 * envfx.c — environment tint and flash palette effects. See envfx.h.
 */

#include "envfx.h"
#include "gamma.h"

#include <stdlib.h>
#include <string.h>

static envfx_config_t s_cfg;
static i32 s_cam;

/* Flash state: dword_4B7B28 (level), 4B7B2C (end time), 4B7B30 (start time),
 * 509228 (target), 5099D0 (start level). */
static i32 s_flash_on;
static i32 s_level, s_t_end, s_t_start, s_target, s_from;
static i32 s_frz, s_fgz, s_fbz;         /* 5099C8, 5099CC, 509258 */
static i32 s_freq, s_rand;              /* 5099C4, 509A38 */
static i32 s_tint[3];                   /* added to 6-bit components */

void envfx_configure(const envfx_config_t *cfg) {
    if (cfg) s_cfg = *cfg; else memset(&s_cfg, 0, sizeof(s_cfg));
    envfx_set_camera(0);
}

void envfx_set_camera(i32 camera) {
    if (camera < 0 || camera >= ENVFX_MAX_CAMERAS) camera = 0;
    s_cam = camera;

    for (i32 i = 0; i < 3; i++) s_tint[i] = 2 * s_cfg.env[camera][i];

    /* 0x46BE40 can_do_32k */
    s_level = s_t_end = s_t_start = 0;
    s_frz = s_cfg.flash_rgb[camera][0];
    s_fgz = s_cfg.flash_rgb[camera][1];
    s_fbz = s_cfg.flash_rgb[camera][2];
    s_freq = s_cfg.flash_freq[camera];
    s_rand = s_cfg.flash_rand[camera];
    s_flash_on = s_cfg.flash_present && (s_frz || s_fgz || s_fbz);
}

static i32 rnd(i32 n) { return n > 0 ? rand() % n : 0; }

/* 0x42F565 timing part: pick a new target level when the ramp ends. */
void envfx_tick(i32 now) {
    if (!s_flash_on) return;
    i32 v14 = (s_freq << 6) / 255;
    i32 v7 = (abs(s_rand) << 6) / 255;
    if (s_t_end <= now) {
        if (v7 <= 0) s_target = s_target ? 0 : 6;
        else         s_target = rnd(6);
        s_from = s_level;
        i32 v3 = v7 + rnd(v14);
        s_t_end = now + abs(s_target - s_from) * v3;
        s_t_start = now;
        if (s_t_end == now) s_t_end = s_t_start + 100;
    }
    s_level = (now - s_t_start) * (s_target - s_from) / (s_t_end - s_t_start) + s_from;
}

b8 envfx_active(void) {
    return (s_tint[0] || s_tint[1] || s_tint[2]) || (s_flash_on && s_level != 0);
}

static i32 clamp63(i32 v) { return v < 0 ? 0 : v > 63 ? 63 : v; }

void envfx_apply_pixels(u32 *px, i32 n) {
    if (!px || gamma_bypassed() || !envfx_active()) return;
    b8 tint = s_tint[0] || s_tint[1] || s_tint[2];
    b8 flash = s_flash_on && s_level != 0;
    for (i32 i = 0; i < n; i++) {
        u32 p = px[i];
        i32 c[3] = { (i32)(p & 0xFF) * 63 / 255, (i32)((p >> 8) & 0xFF) * 63 / 255,
                     (i32)((p >> 16) & 0xFF) * 63 / 255 };
        i32 d[3] = { 0, 0, 0 };
        if (tint) {
            for (i32 k = 0; k < 3; k++) {
                i32 t = clamp63(c[k] + s_tint[k]);
                d[k] = t - c[k];
                c[k] = t;
            }
        }
        if (flash) {
            i32 sq = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
            i32 v13 = s_level * sq / 1024;
            i32 add[3] = { (i32)((double)v13 * ((double)s_frz / 255.0)),
                           (i32)((double)v13 * ((double)s_fgz / 255.0)),
                           (i32)((double)v13 * ((double)s_fbz / 255.0)) };
            for (i32 k = 0; k < 3; k++) {
                i32 t = clamp63(c[k] + add[k]);
                d[k] += t - c[k];
            }
        }
        u32 out = p & 0xFF000000u;
        for (i32 k = 0; k < 3; k++) {
            i32 v = (i32)((p >> (8 * k)) & 0xFF) + d[k] * 255 / 63;
            out |= (u32)(v < 0 ? 0 : v > 255 ? 255 : v) << (8 * k);
        }
        px[i] = out;
    }
}
