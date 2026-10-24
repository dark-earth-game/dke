#pragma once

/*
 * cif.h — CIF image format decoder
 *
 * CIF is the proprietary compressed image format used by the Dark Earth engine
 * for font atlases (.CIF paired with .CAR glyph index) and still-image
 * backdrops (biganims/DIAB_0.CIF etc., menus CIF files).
 *
 * File layout
 * -----------
 *   Offset  Size  Field
 *   0x00     4    version   (u32 LE, always 1)
 *   0x04     4    signature (4 raw bytes, e.g. 0x5E187C33)
 *   0x08     4    width     (u32 LE, pixels)
 *   0x0C     4    height    (u32 LE, pixels)
 *   0x10   768    palette   (256 × RGB triplets, 8-bit per channel)
 *   0x310   var   compressed pixel data (LZW, 9-bit initial code, LSB-first)
 *
 * LZW variant
 * -----------
 *   - Initial code size: 9 bits
 *   - Clear code: 256  (resets the dictionary)
 *   - End code:   257
 *   - Codes grow from 9 → 12 bits as the dictionary fills
 *   - Bits are packed LSB-first (same endianness as GIF)
 *   - First code in stream must be the clear code
 *
 * Ported from tools/src/dump_cif.py (decompress_lzw + parse_cif_file).
 * No external dependencies; pure C99 with types from libsys/common.h.
 */

#include "../common.h"

/* -------------------------------------------------------------------------
 * cif_image_t — decoded CIF image
 * ---------------------------------------------------------------------- */

typedef struct {
    i32  width;    /* pixel columns                                         */
    i32  height;   /* pixel rows                                            */
    i32  bpp;      /* 8 for indexed; 16 for RGB565; 24 for RGB (always 8)  */
    u8  *pixels;   /* width × height bytes (8-bit palette indices)          */
    u8  *palette;  /* 256 × 3 bytes (R, G, B) when bpp == 8; NULL otherwise */
} cif_image_t;

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

/*
 * cif_load — open and fully decode a .CIF file.
 *
 * path: host filesystem path to the .CIF file.
 * out:  caller-supplied struct to fill; must not be NULL.
 *
 * On success:
 *   out->pixels  points to a width*height byte buffer (8-bit palette indices).
 *   out->palette points to a 768-byte RGB palette.
 *   Returns true.
 *
 * On failure:
 *   *out is zeroed; returns false.
 *   All buffers are freed before returning.
 */
b8   cif_load(cif_image_t *out, const c8 *path);

/*
 * cif_free — release buffers owned by cif_image_t.
 * Safe to call on a zero-initialised struct (NULL pixel/palette are ignored).
 */
void cif_free(cif_image_t *img);

/*
 * cif_decompress_lzw — decode a raw CIF LZW stream (decompress_lzw_428E60:
 * 9..12-bit LSB-first codes, 256 clear, 257 end) into width*height bytes.
 * Returns the number of pixels written, or -1 on allocation failure.
 */
i32 cif_decompress_lzw(const u8 *src, i32 src_len, i32 width, i32 height, u8 *out);

/*
 * cif_to_xrgb — expand indexed image to 32-bit XRGB (0xFF000000|R<<16|G<<8|B).
 *
 * img: decoded 8-bit indexed image (cif_load must have succeeded).
 * dst: caller-allocated u32 array of width*height elements.
 *
 * Writes top-left-origin scanlines suitable for blitting into
 * state->graphic.front_buffer.
 */
void cif_to_xrgb(const cif_image_t *img, u32 *dst);
