#pragma once

/*
 * gamma.h — screen colour correction (brightness, contrast, saturation).
 *
 * The original applies the three sliders of DKEGAME.CFG to the palette:
 * update_palette_with_default_colors (0x40C840) runs update_palette (0x40C920)
 * on each of the 256 entries of the colour table, in 6-bit VGA units (0..63).
 * The port draws 8-bit indices through many palettes straight into an RGB
 * frame buffer, so the same per-colour function is applied to the finished
 * frame as it is handed to the window (ls_blit). A palette remap and a remap
 * of the resulting colours are the same function.
 */

#include "libsys/common.h"

/* 0x40C920 update_palette: correct one colour in place, 6-bit components
 * (0..63) with the given slider values. */
void gamma_update_palette(i32 *r, i32 *g, i32 *b, i32 brightness, i32 contrast, i32 saturation);

/* The correction is applied to the shown frame. It is off while the player
 * has not chosen settings (no DKEGAME.CFG yet and the picker not used) and
 * while all three sliders are 0 and unchosen; see gamma_active. */
b8   gamma_active(void);

/* Correct n pixels in place (byte 0 = R, byte 1 = G, byte 2 = B). */
void gamma_apply_pixels(u32 *px, i32 n);

/* Copy of src corrected into dst (for screenshots). */
void gamma_copy_pixels(u32 *dst, const u32 *src, i32 n);

/* Movies are not palette driven in the port; they bypass the correction
 * (the original corrects them in ls_set_movie_correction instead). */
void gamma_bypass(b8 on);

/* True while movies bypass the frame corrections (also used by envfx). */
b8   gamma_bypassed(void);
