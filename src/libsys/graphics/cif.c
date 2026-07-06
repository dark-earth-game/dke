/*
 * cif.c — CIF image format decoder
 *
 * Ported verbatim from tools/src/dump_cif.py (decompress_lzw + parse_cif_file).
 * All Python-only constructs (numpy, dict comprehensions, closures) unrolled to
 * plain C99 arithmetic.
 *
 * Format spec is in cif.h.
 * Port mapping is in docs/functions/cif_decoder.md.
 */

#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "../file_system.h"
#include "cif.h"

/* =========================================================================
 * LZW constants — identical to dump_cif.py
 * ====================================================================== */

#define LZW_INITIAL_CODE_SIZE  9
#define LZW_CLEAR_CODE        256
#define LZW_END_CODE          257
#define LZW_FIRST_CODE        258
#define LZW_MAX_CODE_SIZE      12
#define LZW_MAX_DICT_ENTRIES  4096   /* 2^12 */

/* =========================================================================
 * LZW dictionary
 *
 * The Python implementation uses a dict mapping code → bytes.  We replicate
 * this with a flat table of fixed-capacity byte strings.
 *
 * Each entry stores up to LZW_MAX_ENTRY bytes (the longest possible chain
 * when codes grow to 12 bits is bounded by the total output — but individual
 * entries grow by one byte per step, so at 12 bits we have at most
 * 4096 entries each at most ~4096 bytes long in the worst case.  Real images
 * are far smaller, but we allocate conservatively per-entry with a linked
 * pool to keep stack usage low).
 *
 * Implementation: each dict slot stores the entry as a back-pointer chain.
 *   dict_prefix[code] → parent code (-1 = root byte literal)
 *   dict_suffix[code] → byte value appended at this level
 *
 * To emit an entry we walk the chain backwards into a local reversal buffer.
 * This is equivalent to the Python `entry = dictionary[old_code] + bytes([entry[0]])`
 * pattern but without heap allocation per step.
 * ====================================================================== */

#define DICT_NONE  (-1)

/* Reversal buffer: worst-case chain length is bounded by output size, but in
 * practice LZW chains are short.  4096 bytes is safe for 12-bit codes. */
#define REVERSAL_BUF_SIZE  LZW_MAX_DICT_ENTRIES

typedef struct {
    i32 prefix;   /* parent code index, or DICT_NONE for root literals */
    u8  suffix;   /* byte value of this node                            */
} lzw_entry_t;

/* =========================================================================
 * Bit-reader state (mirrors the read_bits closure in Python)
 * ====================================================================== */

typedef struct {
    const u8 *data;
    i32       size;
    i32       pos;         /* current byte offset into data[] */
    i32       bits_buf;    /* pending bits, packed LSB-first  */
    i32       bits_left;   /* how many bits in bits_buf are valid */
} bitreader_t;

/* read_bits — extract n bits from the LSB-first bitstream.
 * Matches the Python nested function read_bits() in decompress_lzw. */
static i32 bitreader_read(bitreader_t *br, i32 n) {
    while (br->bits_left < n && br->pos < br->size) {
        br->bits_buf |= (i32)br->data[br->pos] << br->bits_left;
        br->bits_left += 8;
        br->pos++;
    }
    i32 value = br->bits_buf & ((1 << n) - 1);
    br->bits_buf >>= n;
    br->bits_left -= n;
    return value;
}

/* =========================================================================
 * decompress_lzw
 *
 * Direct port of dump_cif.py::decompress_lzw.
 * Returns the number of bytes written to out_pixels (== width*height on
 * success), or -1 on fatal error (stream doesn't start with clear code).
 * Partial output (stream ended early) returns what was decoded; caller pads.
 * ====================================================================== */

static i32 decompress_lzw(const u8 *compressed, i32 comp_size,
                           i32 width, i32 height,
                           u8 *out_pixels) {
    const i32 target = width * height;

    /* -- allocate dictionary on the heap to avoid blowing the stack -- */
    lzw_entry_t *dict = (lzw_entry_t *)memory_alloc(
        LZW_MAX_DICT_ENTRIES * (i32)sizeof(lzw_entry_t));
    if (!dict) return -1;

    /* reversal buffer for chain-walking */
    u8 revbuf[REVERSAL_BUF_SIZE];

    /* -- state variables mirroring Python locals -- */
    i32 code_size = LZW_INITIAL_CODE_SIZE;
    i32 max_code  = (1 << code_size) - 1;  /* 511 initially */
    i32 next_code = LZW_FIRST_CODE;         /* 258 initially */
    i32 old_code  = DICT_NONE;

    i32 out_pos   = 0;  /* bytes written to out_pixels */

    /* initialise dictionary: literals 0..255 */
    for (i32 i = 0; i < 256; i++) {
        dict[i].prefix = DICT_NONE;
        dict[i].suffix = (u8)i;
    }
    /* clear / end codes are handled procedurally; leave slots 256/257 blank */

    /* initialise bitreader */
    bitreader_t br;
    br.data      = compressed;
    br.size      = comp_size;
    br.pos       = 0;
    br.bits_buf  = 0;
    br.bits_left = 0;

    /* -- helper: emit the chain for 'code' into out_pixels -- */
    /* Returns first byte of the emitted string (needed for dict extension). */
    /* Writes bytes in correct order using chain reversal. */
    /* Python: output.extend(entry) */
    #define EMIT_CHAIN(code, first_byte_out) do { \
        i32 _c   = (code); \
        i32 _len = 0; \
        /* walk chain backwards */ \
        while (_c != DICT_NONE && _len < REVERSAL_BUF_SIZE) { \
            revbuf[_len++] = dict[_c].suffix; \
            _c = dict[_c].prefix; \
        } \
        (first_byte_out) = revbuf[_len - 1]; \
        /* reverse into output */ \
        for (i32 _i = _len - 1; _i >= 0 && out_pos < target; _i--) { \
            out_pixels[out_pos++] = revbuf[_i]; \
        } \
    } while(0)

    /* -- read initial clear code (Python: code = read_bits; if code != clear_code: return None) -- */
    i32 code = bitreader_read(&br, code_size);
    if (code != LZW_CLEAR_CODE) {
        memory_free(dict);
        return -1;
    }

    /* -- first real code after clear -- */
    code = bitreader_read(&br, code_size);
    {
        u8 dummy;
        EMIT_CHAIN(code, dummy);
        (void)dummy;
    }
    old_code = code;

    /* -- main decode loop -- */
    while (br.pos < br.size && out_pos < target) {
        code = bitreader_read(&br, code_size);

        if (code == LZW_END_CODE) {
            break;
        }

        if (code == LZW_CLEAR_CODE) {
            /* reset decoder state (Python clear_code branch) */
            code_size = LZW_INITIAL_CODE_SIZE;
            max_code  = (1 << code_size) - 1;
            next_code = LZW_FIRST_CODE;

            /* re-init literal entries */
            for (i32 i = 0; i < 256; i++) {
                dict[i].prefix = DICT_NONE;
                dict[i].suffix = (u8)i;
            }

            code = bitreader_read(&br, code_size);
            if (code == LZW_END_CODE) break;

            {
                u8 dummy;
                EMIT_CHAIN(code, dummy);
                (void)dummy;
            }
            old_code = code;
            continue;
        }

        u8 first_byte;

        if (code < next_code) {
            /* code already in dictionary (Python: if code < next_code branch) */
            EMIT_CHAIN(code, first_byte);

            if (old_code != DICT_NONE && next_code < LZW_MAX_DICT_ENTRIES) {
                /* new_entry = dictionary[old_code] + bytes([entry[0]]) */
                dict[next_code].prefix = old_code;
                dict[next_code].suffix = first_byte;
                next_code++;
            }
        } else {
            /* code not yet in dictionary (Python: else branch) */
            /* entry = dictionary[old_code] + bytes([entry[0]])
             * where entry[0] == first byte of old_code's chain */

            /* find first byte of old_code's chain */
            i32 tmp = old_code;
            while (dict[tmp].prefix != DICT_NONE) {
                tmp = dict[tmp].prefix;
            }
            first_byte = dict[tmp].suffix;

            /* add to dictionary before emit so the chain resolve works */
            if (next_code < LZW_MAX_DICT_ENTRIES) {
                dict[next_code].prefix = old_code;
                dict[next_code].suffix = first_byte;
                EMIT_CHAIN(next_code, first_byte);
                next_code++;
            }
        }

        old_code = code;

        /* grow code size when dictionary fills current range */
        if (next_code > max_code && code_size < LZW_MAX_CODE_SIZE) {
            code_size++;
            max_code = (1 << code_size) - 1;
        }
    }

    #undef EMIT_CHAIN

    memory_free(dict);
    return out_pos;
}

/* =========================================================================
 * cif_load
 * ====================================================================== */

b8 cif_load(cif_image_t *out, const c8 *path) {
    if (!out || !path) return false;
    memset(out, 0, sizeof(cif_image_t));

    FILE *fp = ls_open(path, "rb");
    if (!fp) {
        print("cif_load: cannot open %s\n", path);
        return false;
    }

    /* --- read 16-byte header (mirrors parse_cif_file / convert_cif_to_png) --- */
    u8 hdr[16];
    if (fread(hdr, 1, 16, fp) != 16) {
        print("cif_load: short header in %s\n", path);
        ls_close(fp);
        return false;
    }

    /* version  @ hdr[0..3] little-endian u32 */
    /* signature@ hdr[4..7] 4 raw bytes       */
    /* width    @ hdr[8..11] little-endian u32 */
    /* height   @ hdr[12..15] little-endian u32 */
    i32 width  = (i32)((u32)hdr[8]  | ((u32)hdr[9]  << 8) |
                        ((u32)hdr[10] << 16) | ((u32)hdr[11] << 24));
    i32 height = (i32)((u32)hdr[12] | ((u32)hdr[13] << 8) |
                        ((u32)hdr[14] << 16) | ((u32)hdr[15] << 24));

    if (width <= 0 || height <= 0 || width > 4096 || height > 4096) {
        print("cif_load: implausible dimensions %dx%d in %s\n", width, height, path);
        ls_close(fp);
        return false;
    }

    /* --- read 768-byte palette (256 × RGB) --- */
    u8 *palette = (u8 *)memory_alloc(768);
    if (!palette) {
        print("cif_load: palette alloc failed\n");
        ls_close(fp);
        return false;
    }
    if (fread(palette, 1, 768, fp) != 768) {
        print("cif_load: short palette in %s\n", path);
        memory_free(palette);
        ls_close(fp);
        return false;
    }

    /* --- read compressed pixel data (rest of file) --- */
    /* determine remaining size */
    long cur = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long end = ftell(fp);
    fseek(fp, cur, SEEK_SET);
    i32 comp_size = (i32)(end - cur);

    if (comp_size <= 0) {
        print("cif_load: no compressed data in %s\n", path);
        memory_free(palette);
        ls_close(fp);
        return false;
    }

    u8 *comp = (u8 *)memory_alloc(comp_size);
    if (!comp) {
        print("cif_load: compressed-data alloc failed (%d bytes)\n", comp_size);
        memory_free(palette);
        ls_close(fp);
        return false;
    }
    if (fread(comp, 1, (size_t)comp_size, fp) != (size_t)comp_size) {
        print("cif_load: short read on compressed data in %s\n", path);
        memory_free(comp);
        memory_free(palette);
        ls_close(fp);
        return false;
    }
    ls_close(fp);

    /* --- decompress into pixel buffer --- */
    i32 pixel_count = width * height;
    u8 *pixels = (u8 *)memory_alloc(pixel_count);
    if (!pixels) {
        print("cif_load: pixel buffer alloc failed (%d bytes)\n", pixel_count);
        memory_free(comp);
        memory_free(palette);
        return false;
    }
    memset(pixels, 0, (size_t)pixel_count);

    i32 decoded = decompress_lzw(comp, comp_size, width, height, pixels);
    memory_free(comp);

    if (decoded < 0) {
        /* stream did not start with clear code — fatal */
        print("cif_load: LZW stream missing initial clear code in %s\n", path);
        memory_free(pixels);
        memory_free(palette);
        return false;
    }

    if (decoded < pixel_count) {
        /* partial decode — pad with zeroes (mirrors Python behaviour) */
        print("cif_load: warning: partial decode for %s (%d/%d bytes)\n",
              path, decoded, pixel_count);
        /* pixels already memset to 0, so trailing area is already zero */
    }

    /* --- fill output struct --- */
    out->width   = width;
    out->height  = height;
    out->bpp     = 8;
    out->pixels  = pixels;
    out->palette = palette;

    print("cif_load: %s -> %dx%d bpp=%d\n", path, width, height, 8);
    return true;
}

/* =========================================================================
 * cif_free
 * ====================================================================== */

void cif_free(cif_image_t *img) {
    if (!img) return;
    if (img->pixels)  { memory_free(img->pixels);  img->pixels  = NULL; }
    if (img->palette) { memory_free(img->palette); img->palette = NULL; }
    img->width  = 0;
    img->height = 0;
    img->bpp    = 0;
}

/* =========================================================================
 * cif_to_xrgb
 * ====================================================================== */

void cif_to_xrgb(const cif_image_t *img, u32 *dst) {
    if (!img || !dst || !img->pixels || !img->palette) return;

    i32 n = img->width * img->height;
    const u8  *px  = img->pixels;
    const u8  *pal = img->palette;

    for (i32 i = 0; i < n; i++) {
        u8 idx = px[i];
        u8 r   = pal[idx * 3 + 0];
        u8 g   = pal[idx * 3 + 1];
        u8 b   = pal[idx * 3 + 2];
        dst[i] = 0xFF000000u | ((u32)b << 16) | ((u32)g << 8) | (u32)r;
    }
}
