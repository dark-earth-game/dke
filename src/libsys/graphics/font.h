#pragma once

#include "../common.h"

/*
 * font.h — LIBSYS font subsystem
 *
 * Ported from:
 *   init_font_system  @ 405DAB
 *   set_position_coordinates @ 405DC7 (approx)
 *   draw_text         @ 405EDB (approx)
 *   init_font         @ 430BEA
 *
 * Font file format (.CAR + .CIF pair):
 *   .CAR: glyph atlas index table — 255 entries of 3×i32:
 *         { atlas_x, atlas_y, glyph_width }
 *         followed by a trailing i32 (band height, value 0x1C=28 for FONTE11).
 *   .CIF: compressed glyph atlas bitmap (proprietary format — decoding TODO).
 *
 * Fallback: if font files cannot be loaded or decoded, an embedded 8×8
 *   CP437-style bitmap font covers ASCII 0x20..0x7E.
 *   draw_text() always produces visible output regardless of load status.
 *
 * Globals mirror original asm symbols:
 *   font_handle             — Current font handle for active operations
 *   cursor_x/y_position     — Current draw cursor
 *   target_x/y_position     — Target/final cursor position after draw
 */

/*
 * font_glyph_entry_t — one entry in the .CAR glyph atlas index.
 * Matches the 3×i32 record layout in FONTE10.CAR / FONTE11.CAR.
 */
typedef struct {
    i32 atlas_x;      /* x pixel offset into the atlas bitmap */
    i32 atlas_y;      /* y pixel offset into the atlas bitmap (band y) */
    i32 glyph_width;  /* per-glyph advance/width in pixels */
} font_glyph_entry_t;

#define FONT_NUM_GLYPHS     255   /* CAR table covers chars 0..254 */
#define FONT_BAND_HEIGHT_DEFAULT 28

/*
 * font_t — engine font descriptor.
 * Opaque to callers; adjusted as more of init_font is decoded.
 *
 * When use_fallback is true the font uses the embedded 8×8 CP437 table
 * (g_cp437_8x8) and glyph_data / atlas are NULL.
 */
typedef struct {
    i32  glyph_w;        /* fixed glyph cell width  (fallback: 8) */
    i32  glyph_h;        /* fixed glyph cell height (fallback: 8) */
    i32  first_char;     /* first ASCII code in table (fallback: 0x20) */
    i32  num_chars;      /* number of glyphs in table (fallback: 95) */
    u8  *glyph_data;     /* num_chars * glyph_w * glyph_h bytes, 8-bit indexed;
                            NULL when use_fallback is true */

    /* atlas fields (populated from .CAR + .CIF pair) */
    font_glyph_entry_t *glyph_entries; /* 255-entry CAR table; NULL = fallback */
    u8  *atlas_pixels;                 /* raw atlas bitmap (8-bit indexed);
                                          NULL = fallback or not yet decoded */
    i32  atlas_width;
    i32  atlas_height;
    i32  band_height;    /* row stride in atlas (from CAR trailing dword) */

    b8   use_fallback;   /* true → render via embedded CP437 8×8 table */
} font_t;

/* -------------------------------------------------------------------------
 * Globals — mirror original asm data symbols
 * ---------------------------------------------------------------------- */

/* font_file_handle @ DATA:819349 — handle to loaded SYSFONT */
extern font_t *font_file_handle;

/* font_handle @ DATA:238530 — current active font handle */
extern font_t *font_handle;

/* g_default_font — exposed for convenience; points at the active system font */
extern font_t *g_default_font;

/* cursor / target positions @ DATA:238522..238528 */
extern i32 cursor_x_position;
extern i32 cursor_y_position;
extern i32 target_x_position;
extern i32 target_y_position;

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

/*
 * init_font_system — top-level font subsystem init (addr ~405DAB).
 *
 * Attempts to load the system font ("SYSFONT" / FONTE11) via init_font().
 * On failure logs a warning and installs the embedded CP437 fallback.
 * Always returns true so callers succeed.
 */
b8 init_font_system(void);

/*
 * init_font — load one font by logical name (addr 430BEA).
 *
 * name: logical font name, e.g. "SYSFONT".
 *   "SYSFONT" maps to datas/menus/FONTE11.CAR + .CIF.
 *   Other names: appended with ".FNT" and opened via ls_load_file (TODO).
 * out: caller-supplied font_t to fill.
 * Returns true on success, false if files not found / decode fails.
 *
 * NOTE: CIF atlas decoding is not yet ported (TODO: sub_482200 / sub_4822E0).
 *       Currently installs fallback regardless of file presence.
 */
b8 init_font(font_t *out, const c8 *name);

/*
 * set_position_coordinates — write cursor position into globals (addr 405DC7).
 * __fastcall: ECX=x, EDX=y in original; we use normal args here.
 */
void set_position_coordinates(i32 x, i32 y);

/*
 * draw_text — draw null-terminated string at current cursor position (addr 405EDB).
 * Writes to state->graphic.front_buffer (u32 XRGB per pixel).
 * Falls back gracefully when font_handle is NULL.
 */
void draw_text(const c8 *text);

/*
 * draw_text_at — draw string with explicit font / position / colour.
 * color_argb: 0xFFRRGGBB — alpha byte is ignored; foreground colour.
 * Writes to state->graphic.front_buffer.
 */
void draw_text_at(font_t *font, i32 x, i32 y, const c8 *text, u32 color_argb);

/*
 * read_line_file — read one '\n'-terminated line from a binary file (addr 430D09).
 * Returns 1 on success, 0 on EOF.
 * Used by read_and_init_sbt for subtitle data.
 */
i32 read_line_file(c8 *dst, FILE *fp);
