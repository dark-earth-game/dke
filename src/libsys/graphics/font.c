#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "../file_system.h"
#include "font.h"
#include "graphic.h"
#include "cif.h"
#include "../../state.h"

/* =========================================================================
 * Embedded CP437-style 8×8 fallback font
 *
 * Covers printable ASCII 0x20 (' ') through 0x7E ('~') — 95 glyphs.
 * Each glyph is 8 bytes (one byte per row, MSB = leftmost pixel).
 * Nonzero bit → foreground; zero bit → transparent background.
 *
 * This table is synthesised from the classic CP437 8×8 font bitmaps.
 * Embedded so draw_text() produces visible output even when .CAR/.CIF
 * files are absent or the CIF decoder is not yet ported.
 * ====================================================================== */

/* 95 glyphs × 8 bytes = 760 bytes */
static const u8 cp437_8x8[95 * 8] = {
    /* 0x20 ' ' */  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    /* 0x21 '!' */  0x18,0x18,0x18,0x18,0x00,0x00,0x18,0x00,
    /* 0x22 '"' */  0x6C,0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,
    /* 0x23 '#' */  0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00,
    /* 0x24 '$' */  0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00,
    /* 0x25 '%' */  0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00,
    /* 0x26 '&' */  0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00,
    /* 0x27 '\''*/  0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,
    /* 0x28 '(' */  0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00,
    /* 0x29 ')' */  0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00,
    /* 0x2A '*' */  0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00,
    /* 0x2B '+' */  0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,
    /* 0x2C ',' */  0x00,0x00,0x00,0x00,0x18,0x18,0x0C,0x00,
    /* 0x2D '-' */  0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,
    /* 0x2E '.' */  0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,
    /* 0x2F '/' */  0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00,
    /* 0x30 '0' */  0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00,
    /* 0x31 '1' */  0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00,
    /* 0x32 '2' */  0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00,
    /* 0x33 '3' */  0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00,
    /* 0x34 '4' */  0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00,
    /* 0x35 '5' */  0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00,
    /* 0x36 '6' */  0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00,
    /* 0x37 '7' */  0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00,
    /* 0x38 '8' */  0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00,
    /* 0x39 '9' */  0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00,
    /* 0x3A ':' */  0x00,0x18,0x18,0x00,0x18,0x18,0x00,0x00,
    /* 0x3B ';' */  0x00,0x18,0x18,0x00,0x18,0x18,0x0C,0x00,
    /* 0x3C '<' */  0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00,
    /* 0x3D '=' */  0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00,
    /* 0x3E '>' */  0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00,
    /* 0x3F '?' */  0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00,
    /* 0x40 '@' */  0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00,
    /* 0x41 'A' */  0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00,
    /* 0x42 'B' */  0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00,
    /* 0x43 'C' */  0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00,
    /* 0x44 'D' */  0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00,
    /* 0x45 'E' */  0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00,
    /* 0x46 'F' */  0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00,
    /* 0x47 'G' */  0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00,
    /* 0x48 'H' */  0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00,
    /* 0x49 'I' */  0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,
    /* 0x4A 'J' */  0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00,
    /* 0x4B 'K' */  0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00,
    /* 0x4C 'L' */  0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00,
    /* 0x4D 'M' */  0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00,
    /* 0x4E 'N' */  0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00,
    /* 0x4F 'O' */  0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00,
    /* 0x50 'P' */  0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00,
    /* 0x51 'Q' */  0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00,
    /* 0x52 'R' */  0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00,
    /* 0x53 'S' */  0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00,
    /* 0x54 'T' */  0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,
    /* 0x55 'U' */  0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00,
    /* 0x56 'V' */  0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00,
    /* 0x57 'W' */  0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00,
    /* 0x58 'X' */  0x63,0x63,0x36,0x1C,0x36,0x63,0x63,0x00,
    /* 0x59 'Y' */  0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00,
    /* 0x5A 'Z' */  0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00,
    /* 0x5B '[' */  0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00,
    /* 0x5C '\\'*/  0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00,
    /* 0x5D ']' */  0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00,
    /* 0x5E '^' */  0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00,
    /* 0x5F '_' */  0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,
    /* 0x60 '`' */  0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00,
    /* 0x61 'a' */  0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00,
    /* 0x62 'b' */  0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00,
    /* 0x63 'c' */  0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00,
    /* 0x64 'd' */  0x38,0x30,0x30,0x3E,0x33,0x33,0x6E,0x00,
    /* 0x65 'e' */  0x00,0x00,0x1E,0x33,0x3F,0x03,0x1E,0x00,
    /* 0x66 'f' */  0x1C,0x36,0x06,0x0F,0x06,0x06,0x0F,0x00,
    /* 0x67 'g' */  0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F,
    /* 0x68 'h' */  0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00,
    /* 0x69 'i' */  0x0C,0x00,0x0E,0x0C,0x0C,0x0C,0x1E,0x00,
    /* 0x6A 'j' */  0x30,0x00,0x30,0x30,0x30,0x33,0x33,0x1E,
    /* 0x6B 'k' */  0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00,
    /* 0x6C 'l' */  0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,
    /* 0x6D 'm' */  0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00,
    /* 0x6E 'n' */  0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00,
    /* 0x6F 'o' */  0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00,
    /* 0x70 'p' */  0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F,
    /* 0x71 'q' */  0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78,
    /* 0x72 'r' */  0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00,
    /* 0x73 's' */  0x00,0x00,0x1E,0x03,0x1E,0x30,0x1F,0x00,
    /* 0x74 't' */  0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00,
    /* 0x75 'u' */  0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00,
    /* 0x76 'v' */  0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00,
    /* 0x77 'w' */  0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00,
    /* 0x78 'x' */  0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00,
    /* 0x79 'y' */  0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F,
    /* 0x7A 'z' */  0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00,
    /* 0x7B '{' */  0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00,
    /* 0x7C '|' */  0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00,
    /* 0x7D '}' */  0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00,
    /* 0x7E '~' */  0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00,
};

/* =========================================================================
 * Globals (mirror original asm data symbols)
 * ====================================================================== */

/* font_file_handle @ DATA:819349 */
font_t *font_file_handle = NULL;

/* font_handle @ DATA:238530 */
font_t *font_handle = NULL;

/* g_default_font — convenience alias */
font_t *g_default_font = NULL;

/* cursor / target positions @ DATA:238522..238528 */
i32 cursor_x_position = 0;
i32 cursor_y_position = 0;
i32 target_x_position = 0;
i32 target_y_position = 0;

/* =========================================================================
 * Internal — install embedded CP437 fallback into a font_t
 * ====================================================================== */

static void install_fallback_font(font_t *f) {
    f->glyph_w        = 8;
    f->glyph_h        = 8;
    f->first_char     = 0x20;   /* space */
    f->num_chars      = 95;     /* 0x20..0x7E inclusive */
    f->glyph_data     = NULL;   /* uses cp437_8x8 directly */
    f->glyph_entries  = NULL;
    f->atlas_pixels   = NULL;
    f->atlas_width    = 0;
    f->atlas_height   = 0;
    f->band_height    = 8;
    f->palette        = NULL;
    f->use_fallback   = true;
}

/* =========================================================================
 * init_font  (addr 430BEA)
 *
 * Loads one font by logical name.
 *   "SYSFONT" → datas/menus/FONTE11.CAR + FONTE11.CIF.
 *   Other names → append ".FNT", read header, load atlas via CIF (sub_482200/sub_4822E0).
 *
 * Both paths fall back to embedded CP437 8×8 if files are missing or decode fails.
 * ====================================================================== */

// 430BEA
b8 init_font(font_t *out, const c8 *name) {
    if (!out || !name) {
        install_fallback_font(out ? out : &(font_t){0});
        return false;
    }

    /* Clear the output struct */
    memset(out, 0, sizeof(font_t));

    if (strcmp(name, "SYSFONT") == 0) {
        /*
         * SYSFONT maps to datas/menus/FONTE11.CAR + FONTE11.CIF.
         * Original: copies font_file_handle → dword_508570, sets dword_508AA4=1.
         * We attempt to load the .CAR index; CIF decoding is TODO.
         */
        c8 car_path[512];
        const c8 *base = ls_get_base_path();
        ls_resolve_path(base, "DATAS>MENUS>FONTE11.CAR", car_path, sizeof(car_path));

        FILE *fp = ls_open(car_path, "rb");
        if (fp) {
            /* Read 255 glyph entries (3×i32 each) + trailing i32 band_height */
            font_glyph_entry_t *entries = (font_glyph_entry_t *)memory_alloc(
                FONT_NUM_GLYPHS * sizeof(font_glyph_entry_t));
            if (entries) {
                i32 n = (i32)fread(entries, sizeof(font_glyph_entry_t),
                                   FONT_NUM_GLYPHS, fp);
                if (n == FONT_NUM_GLYPHS) {
                    i32 band_h = FONT_BAND_HEIGHT_DEFAULT;
                    fread(&band_h, sizeof(i32), 1, fp);

                    out->glyph_entries = entries;
                    out->band_height   = band_h;
                    /* glyph_w / glyph_h set to per-glyph values from index */
                    out->glyph_w   = 0;  /* variable-width font */
                    out->glyph_h   = band_h;
                    out->first_char = 0;
                    out->num_chars  = FONT_NUM_GLYPHS;

                    print("init_font: loaded FONTE11.CAR (%d glyphs, band_h=%d)\n",
                          n, band_h);
                } else {
                    memory_free(entries);
                    entries = NULL;
                    print("init_font: short read from FONTE11.CAR (%d/%d)\n",
                          n, FONT_NUM_GLYPHS);
                }
            }
            ls_close(fp);
        } else {
            print("init_font: FONTE11.CAR not found at %s\n", car_path);
        }

        /*
         * Decode FONTE11.CIF atlas bitmap.
         * Replaces: sub_482200 / sub_4822E0 / sub_48B660 (addr anchors preserved
         * in FUNCTION_MAP.md as partial stubs until full asm comparison is done).
         *
         * The .CIF file shares the same base name as the .CAR index:
         *   DATAS>MENUS>FONTE11.CAR  →  DATAS>MENUS>FONTE11.CIF
         */
        {
            c8 cif_path[512];
            ls_resolve_path(base, "DATAS>MENUS>FONTE11.CIF", cif_path, sizeof(cif_path));

            cif_image_t atlas_img;
            if (cif_load(&atlas_img, cif_path)) {
                /* atlas_img.pixels is an 8-bit indexed bitmap (width × height).
                 * Store it directly in the font_t atlas fields.
                 * draw_text_at uses atlas_pixels[atlas_y * atlas_width + atlas_x]
                 * to fetch indexed pixel values.
                 */
                out->atlas_pixels = atlas_img.pixels;   /* font_t takes ownership */
                out->atlas_width  = atlas_img.width;
                out->atlas_height = atlas_img.height;
                out->palette = atlas_img.palette;       /* take ownership from cif_image_t */
                atlas_img.palette = NULL;               /* prevent cif_free from releasing */
                /* do NOT call cif_free — we transferred ownership of pixels */

                /* atlas successfully decoded — disable CP437 fallback */
                out->use_fallback = false;
                print("init_font: CIF atlas loaded %dx%d, atlas-based rendering enabled\n",
                      out->atlas_width, out->atlas_height);
            } else {
                print("init_font: FONTE11.CIF not found or decode failed — using CP437 fallback\n");
            }
        }

        /* If atlas decode failed, install fallback for rendering */
        if (!out->atlas_pixels) {
            install_fallback_font(out);
            /* preserve the CAR entries if loaded (future use) */
        }

        return true;

    } else {
        /*
         * Non-SYSFONT: sub_482200 (font atlas loader) + sub_4822E0 (descriptor builder).
         * 1. Open <name>.FNT, read 6-int header
         * 2. Load atlas image (<name>.CIF) via cif_load
         * 3. Build glyph_entries from grid layout
         * 4. If spacing_flag nonzero, compute proportional widths by scanning cells
         * Falls back to CP437 if FNT or atlas not found.
         */
        const c8 *base = ls_get_base_path();

        /* sub_482200 step 1: append ".FNT", load file, read header */
        c8 fnt_vpath[256];
        c8 fnt_path[512];
        snprintf(fnt_vpath, sizeof(fnt_vpath), "%s.FNT", name);
        ls_resolve_path(base, fnt_vpath, fnt_path, sizeof(fnt_path));

        FILE *fp = ls_open(fnt_path, "rb");
        if (!fp) {
            print("init_font: %s not found — fallback\n", fnt_vpath);
            install_fallback_font(out);
            return true;
        }

        /* 6 header integers: cols_per_row, glyph_h, (passed), glyph_w, spacing_flag, char_count */
        i32 hdr[6];
        if (fread(hdr, sizeof(i32), 6, fp) != 6) {
            print("init_font: %s header short read — fallback\n", fnt_vpath);
            ls_close(fp);
            install_fallback_font(out);
            return true;
        }
        ls_close(fp);

        i32 cols_per_row  = hdr[0];
        i32 glyph_h       = hdr[1];
        /* hdr[2] passed to sub_4822E0 (unused beyond forwarding) */
        i32 glyph_w       = hdr[3];
        i32 spacing_flag  = hdr[4];
        i32 char_count    = hdr[5];

        /* Original: if spacing_flag == 0, forced to 0 (monospaced) */
        if (spacing_flag == 0) spacing_flag = 0;

        if (glyph_w <= 0 || glyph_h <= 0 || char_count <= 0) {
            print("init_font: %s invalid header (w=%d h=%d n=%d) — fallback\n",
                  fnt_vpath, glyph_w, glyph_h, char_count);
            install_fallback_font(out);
            return true;
        }

        /* sub_4822E0 step 3: load atlas image (CIF) */
        c8 cif_vpath[256];
        c8 cif_path[512];
        snprintf(cif_vpath, sizeof(cif_vpath), "%s.CIF", name);
        ls_resolve_path(base, cif_vpath, cif_path, sizeof(cif_path));

        cif_image_t atlas_img;
        if (!cif_load(&atlas_img, cif_path)) {
            print("init_font: %s atlas not found — fallback\n", cif_vpath);
            install_fallback_font(out);
            return true;
        }

        /* sub_4822E0 [+0x10]: glyphs_per_row = atlas_bitmap_width / glyph_width */
        i32 glyphs_per_row = atlas_img.width / glyph_w;
        if (glyphs_per_row <= 0) glyphs_per_row = cols_per_row;

        /* Build glyph_entries from grid layout so existing draw_text_at renderer works */
        font_glyph_entry_t *entries = (font_glyph_entry_t *)memory_alloc(
            char_count * (i32)sizeof(font_glyph_entry_t));
        if (!entries) {
            cif_free(&atlas_img);
            install_fallback_font(out);
            return true;
        }

        for (i32 i = 0; i < char_count; i++) {
            i32 col = i % glyphs_per_row;
            i32 row = i / glyphs_per_row;
            entries[i].atlas_x     = col * glyph_w;
            entries[i].atlas_y     = row * glyph_h;
            entries[i].glyph_width = glyph_w;
        }

        /* sub_4822E0 step 4: if spacing_flag nonzero, scan each glyph cell
         * right-to-left to find rightmost non-transparent pixel → advance width */
        if (spacing_flag != 0) {
            for (i32 i = 0; i < char_count; i++) {
                i32 ax = entries[i].atlas_x;
                i32 ay = entries[i].atlas_y;
                i32 rightmost = -1;
                for (i32 cx = glyph_w - 1; cx >= 0 && rightmost < 0; cx--) {
                    for (i32 ry = 0; ry < glyph_h; ry++) {
                        i32 px = ax + cx;
                        i32 py = ay + ry;
                        if (px < atlas_img.width && py < atlas_img.height &&
                            atlas_img.pixels[py * atlas_img.width + px] != 0) {
                            rightmost = cx;
                            break;
                        }
                    }
                }
                /* advance = rightmost pixel column + 2 (1 pixel padding).
                 * Empty glyphs (e.g. space) keep the full cell width. */
                entries[i].glyph_width = (rightmost >= 0) ? (rightmost + 2) : glyph_w;
            }
        }

        /* Fill font_t — sub_4822E0 descriptor fields */
        out->atlas_pixels  = atlas_img.pixels;
        out->atlas_width   = atlas_img.width;
        out->atlas_height  = atlas_img.height;
        out->palette       = atlas_img.palette;
        atlas_img.palette  = NULL;
        out->glyph_entries = entries;
        out->glyph_w       = glyph_w;
        out->glyph_h       = glyph_h;
        out->band_height   = glyph_h;
        out->first_char    = 0;
        out->num_chars     = char_count;
        out->use_fallback  = false;

        print("init_font: loaded %s (%d chars, %dx%d, proportional=%d)\n",
              name, char_count, glyph_w, glyph_h, spacing_flag != 0);
        return true;
    }
}

/* =========================================================================
 * init_font_system  (addr ~405DAB)
 *
 * Top-level font subsystem init.  Called from init_all_systems.
 * Loads the system font ("SYSFONT").  On failure logs a warning and
 * installs the embedded CP437 fallback.  Always returns true.
 * ====================================================================== */

// 405DAB
b8 init_font_system(void) {
    /*
     * Original: checks font_file_handle (loaded earlier by sub_482560 /
     * ls_set_video_mode chain), copies it to font_handle.  If NULL, logs
     * "cannot open FNT file".
     *
     * Port: allocate a font_t and call init_font("SYSFONT").
     */
    if (!font_file_handle) {
        font_file_handle = (font_t *)memory_calloc(sizeof(font_t));
        if (!font_file_handle) {
            print("init_font_system: allocation failed\n");
            return false;
        }
        init_font(font_file_handle, "SYSFONT");
    }

    font_handle   = font_file_handle;
    g_default_font = font_handle;

    if (!font_handle) {
        print("init_font_system: cannot open FNT file — using fallback\n");
    } else {
        print("init_font_system: font system ready (fallback=%d)\n",
              (i32)font_handle->use_fallback);
    }

    return true;
}

/* =========================================================================
 * set_position_coordinates  (addr ~405DC7)
 *
 * Writes x/y into both cursor and target position globals.
 * Original: __fastcall ECX=x, EDX=y.
 * ====================================================================== */

// 405DC7
void set_position_coordinates(i32 x, i32 y) {
    cursor_y_position = y;
    target_y_position = cursor_y_position;
    cursor_x_position = x;
    target_x_position = cursor_x_position;
}

/* =========================================================================
 * Internal — render one glyph from the CP437 8×8 fallback table into a
 * 32-bit XRGB front_buffer at pixel (px, py).
 * ====================================================================== */

static void blit_fallback_glyph(u32 *fb, i32 fb_width, i32 fb_height,
                                  i32 px, i32 py, u8 ch, u32 color_xrgb) {
    if (ch < 0x20 || ch > 0x7E) return;
    i32 glyph_idx = (i32)(ch - 0x20);
    const u8 *glyph = &cp437_8x8[glyph_idx * 8];

    for (i32 row = 0; row < 8; row++) {
        i32 fy = py + row;
        if (fy < 0 || fy >= fb_height) continue;
        u8 bits = glyph[row];
        for (i32 col = 0; col < 8; col++) {
            i32 fx = px + col;
            if (fx < 0 || fx >= fb_width) continue;
            /* CP437 table above is LSB-leftmost packed (col 0 = bit 0). */
            if (bits & (1u << col)) {
                fb[fy * fb_width + fx] = color_xrgb;
            }
        }
    }
}

/* =========================================================================
 * draw_text_at — draw string with explicit font / position / colour.
 * ====================================================================== */

static i32 s_tint = 0;   /* draw_text_tinted: modulate the glyph palette by the colour */

void draw_text_at(font_t *font, i32 x, i32 y, const c8 *text, u32 color_argb) {
    if (!text || !font) return;
    if (!state || !state->graphic.front_buffer) return;

    u32 *fb       = state->graphic.front_buffer;
    i32  fb_w     = state->graphic.width;
    i32  fb_h     = state->graphic.height;
    u32  color    = 0xFF000000u | (color_argb & 0x00FFFFFFu);

    i32 cx = x;
    for (const c8 *p = text; *p; p++) {
        u8 ch = (u8)*p;

        if (ch == '\n') {
            cx  = x;
            y  += font->glyph_h ? font->glyph_h : 8;
            continue;
        }

        if (font->use_fallback) {
            /* Render from embedded CP437 8×8 bitmap */
            blit_fallback_glyph(fb, fb_w, fb_h, cx, y, ch, color);
            cx += 8; /* fixed 8-pixel advance for fallback */
        } else if (font->glyph_entries && font->atlas_pixels) {
            /* Atlas-based rendering (proportional / variable-width).
             * atlas_pixels is 8-bit indexed; nonzero index = foreground pixel.
             * Original engine uses the palette to colour glyphs; here we blit
             * as the caller-supplied colour to avoid needing the palette lookup
             * before it's ported. */
            if ((i32)ch < font->num_chars) {
                font_glyph_entry_t *g = &font->glyph_entries[(i32)ch];
                i32 gw = g->glyph_width;
                i32 gh = font->band_height > 0 ? font->band_height : font->glyph_h;
                for (i32 gy = 0; gy < gh; gy++) {
                    i32 fy = y + gy;
                    if (fy < 0 || fy >= fb_h) continue;
                    const u8 *src = font->atlas_pixels
                        + (g->atlas_y + gy) * font->atlas_width + g->atlas_x;
                    u32 *dst = fb + fy * fb_w + cx;
                    for (i32 gx = 0; gx < gw; gx++) {
                        i32 fx = cx + gx;
                        if (fx < 0 || fx >= fb_w) continue;
                        u8 idx = src[gx];
                        if (idx) {
                            if (font->palette) {
                                u8 r = font->palette[idx * 3 + 0];
                                u8 g = font->palette[idx * 3 + 1];
                                u8 b = font->palette[idx * 3 + 2];
                                if (s_tint) {
                                    r = (u8)(r * ((color >> 16) & 0xFF) / 255);
                                    g = (u8)(g * ((color >> 8) & 0xFF) / 255);
                                    b = (u8)(b * (color & 0xFF) / 255);
                                }
                                dst[gx] = 0xFF000000u | ((u32)b << 16) | ((u32)g << 8) | (u32)r;
                            } else {
                                dst[gx] = color;
                            }
                        }
                    }
                }
                cx += gw;
            } else {
                cx += 8;
            }
        } else {
            /* Fallback for partially-loaded fonts */
            blit_fallback_glyph(fb, fb_w, fb_h, cx, y, ch, color);
            cx += 8;
        }
    }
}

/* =========================================================================
 * draw_text  (addr ~405EDB)
 *
 * Draw null-terminated string at current cursor position (target_x/y).
 * Writes to state->graphic.front_buffer.
 *
 * Original:
 *   - Checks font_handle != NULL and game_context != NULL; skips if either null.
 *   - In 8-bit mode calls sub_47D330(0xFF) before and sub_47D310 after.
 *   - Loops over chars: calls sub_4820B0(font_handle, game_context,
 *       char, target_y + i*8, target_x, 0) for each character.
 *   - Calls draw_image(game_context, graphics_context, ...) at end.
 *
 * Port: game_context / graphics_context not yet available;  render
 *   directly to front_buffer using target_x/y_position.
 * ====================================================================== */

// 405EDB
void draw_text(const c8 *text) {
    if (!text) return;

    /* Original: skip if font_handle == 0 or game_context == 0 */
    if (!font_handle) {
        print("draw_text: font_handle is NULL, skipping\n");
        return;
    }

    /* Default foreground: white */
    u32 white = 0xFFFFFFFFu;

    draw_text_at(font_handle,
                 target_x_position,
                 target_y_position,
                 text,
                 white);
}

/* =========================================================================
 * read_line_file  (addr 430D09)
 *
 * Read one newline-terminated line from a binary file into dst.
 * Returns 1 on success (line stored in dst), 0 on EOF.
 * Used by read_and_init_sbt for subtitle text.
 * ====================================================================== */

// 430D09
i32 read_line_file(c8 *dst, FILE *fp) {
    if (!dst || !fp) return 0;

    /* Original uses a 0x100-byte stack buffer, reads byte-by-byte until '\n' */
    c8 buf[256];
    i32 pos = 0;

    /* Read first byte */
    if (fread(&buf[pos], 1, 1, fp) == 0) {
        return 0;  /* EOF */
    }
    pos++;

    /* Read subsequent bytes until '\n' or EOF */
    while (pos < 255) {
        c8 ch;
        if (fread(&ch, 1, 1, fp) == 0) break;
        buf[pos] = ch;
        pos++;
        if (ch == '\n') break;
    }
    buf[pos] = '\0';

    strcpy(dst, buf);
    return 1;
}

/* The palette's glyph ramp (light) scaled by `color` (0xFFRRGGBB): dark text
 * for pale panels. */
void draw_text_tinted(font_t *font, i32 x, i32 y, const c8 *text, u32 color_argb) {
    s_tint = 1;
    draw_text_at(font, x, y, text, color_argb);
    s_tint = 0;
}
