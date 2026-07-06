/*
 * anim2d.c — 2D pre-rendered character animation loader.
 * See anim2d.h for the format specification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "anim2d.h"

/* -------------------------------------------------------------------------
 * LZSS decompressor — direct port of sub_4780B4.
 *
 * Classic LZSS variant:
 *   Ring buffer: 4096 bytes (0x1000), initialized to 0x20 (space).
 *   Initial write position: 0xFEE (4078).
 *   Input starts with a 4-byte LE uncompressed-count (= remaining output
 *   bytes to produce); this is the loop counter, not stored in output.
 *   Flag byte: 8 bits, LSB first.
 *     bit = 1 → literal: copy one byte to output + ring buf, advance pos.
 *     bit = 0 → back-ref: read 2 bytes (lo, hi).
 *                  ring_offset  = lo | ((hi >> 4) << 8)   (12-bit)
 *                  run_length   = (hi & 0xF) + 3          (3..18)
 *                  Copy run_length bytes from ring[ring_offset..].
 * ------------------------------------------------------------------------- */

#define LZSS_BUFSIZE 0x1000
#define LZSS_INITPOS 0x0FEE
#define LZSS_FILL    0x20

i32 lzss_decompress(const u8 *src, i32 src_size, u8 *dst, i32 dst_size) {
    if (!src || src_size < 4 || !dst || dst_size <= 0) return 0;

    u8 ring[LZSS_BUFSIZE];
    memset(ring, LZSS_FILL, sizeof(ring));

    /* Read uncompressed count from first 4 bytes (LE). */
    i32 count = (i32)((u32)src[0] | ((u32)src[1]<<8) | ((u32)src[2]<<16) | ((u32)src[3]<<24));
    if (count > dst_size) count = dst_size;

    const u8 *s  = src + 4;
    const u8 *end = src + src_size;
    u8       *d  = dst;
    i32       ring_pos = LZSS_INITPOS;
    i32       flags = 0;
    i32       flag_bits = 0;

    while (count > 0 && s < end) {
        if (flag_bits == 0) {
            flags     = *s++;
            flag_bits = 8;
        }
        i32 bit = flags & 1;
        flags >>= 1;
        flag_bits--;

        if (bit) {
            /* Literal byte. */
            if (s >= end) break;
            u8 c = *s++;
            *d++ = c;
            ring[ring_pos & 0xFFF] = c;
            ring_pos = (ring_pos + 1) & 0xFFF;
            count--;
        } else {
            /* Back-reference: 2 bytes. */
            if (s + 1 >= end) break;
            u8 lo = *s++;
            u8 hi = *s++;
            if (count <= 0x12) break; /* sentinel from original */
            i32 offset = (i32)lo | (((i32)(hi >> 4)) << 8);
            i32 run    = (hi & 0x0F) + 3;
            if (run > count) run = count;
            for (i32 k = 0; k < run; k++) {
                u8 c = ring[(offset + k) & 0xFFF];
                *d++ = c;
                ring[ring_pos & 0xFFF] = c;
                ring_pos = (ring_pos + 1) & 0xFFF;
                count--;
            }
        }
    }

    return (i32)(d - dst);
}

/* -------------------------------------------------------------------------
 * ANI file loader
 * ------------------------------------------------------------------------- */

static i32 read_le_i16(const u8 *p) {
    return (i32)((u32)p[0] | ((u32)p[1] << 8));
}
static i32 read_le_i32(const u8 *p) {
    return (i32)((u32)p[0] | ((u32)p[1]<<8) | ((u32)p[2]<<16) | ((u32)p[3]<<24));
}

i32 ani_load(const c8 *host_path, ani_t *out) {
    if (!host_path || !out) return 0;
    memset(out, 0, sizeof(*out));

    FILE *f = ls_open(host_path, "rb");
    if (!f) {
        print("ani_load: cannot open '%s'\n", host_path);
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 12) { ls_close(f); return 0; }

    u8 *data = (u8 *)malloc((size_t)sz);
    if (!data) { ls_close(f); return 0; }
    if (fread(data, 1, (size_t)sz, f) != (size_t)sz) {
        free(data); ls_close(f); return 0;
    }
    ls_close(f);

    /* Footer (last 12 bytes). */
    u8 direction   = data[sz - 12];
    u8 loop_flag   = data[sz - 11];
    i32 timing     = read_le_i32(data + sz - 10);
    u8 vis_frames  = data[sz - 6];
    u8 format_type = data[sz - 2];
    u8 n_frames    = data[sz - 1];

    (void)format_type; /* extended footer already consumed above */

    if (n_frames == 0 || n_frames > ANI_MAX_FRAMES) {
        print("ani_load: '%s' bad n_frames=%d\n", host_path, (i32)n_frames);
        free(data);
        return 0;
    }

    out->n_frames      = n_frames;
    out->visible_frames = vis_frames ? vis_frames : n_frames;
    out->direction     = direction;
    out->loop_flag     = loop_flag;
    out->timing        = timing;

    /* Parse frame records. */
    i32 pos = 0;
    for (i32 i = 0; i < n_frames; i++) {
        if (pos + 16 > (i32)sz - 12) {
            print("ani_load: '%s' truncated at frame %d\n", host_path, i);
            out->n_frames = i;
            break;
        }
        ani_frame_t *fr = &out->frames[i];
        fr->x1         = (i16)read_le_i16(data + pos); pos += 2;
        fr->y1         = (i16)read_le_i16(data + pos); pos += 2;
        fr->x2         = (i16)read_le_i16(data + pos); pos += 2;
        fr->y2         = (i16)read_le_i16(data + pos); pos += 2;
        fr->uncomp_size = read_le_i32(data + pos); pos += 4;
        fr->comp_size   = read_le_i32(data + pos); pos += 4;

        if (fr->uncomp_size <= 0 || fr->comp_size <= 0 ||
            pos + fr->comp_size > (i32)sz - 12) {
            print("ani_load: '%s' bad sizes at frame %d\n", host_path, i);
            out->n_frames = i;
            break;
        }

        fr->pixels = (u8 *)malloc((size_t)fr->uncomp_size);
        if (!fr->pixels) {
            out->n_frames = i;
            break;
        }

        if (fr->comp_size == fr->uncomp_size) {
            /* Uncompressed — direct copy. */
            memcpy(fr->pixels, data + pos, (size_t)fr->uncomp_size);
        } else {
            /* LZSS-compressed. */
            i32 written = lzss_decompress(data + pos, fr->comp_size,
                                          fr->pixels, fr->uncomp_size);
            if (written != fr->uncomp_size) {
                print("ani_load: '%s' frame %d decomp mismatch %d vs %d\n",
                      host_path, i, written, fr->uncomp_size);
            }
        }
        pos += fr->comp_size;
    }

    free(data);

    if (out->n_frames > 0) {
        ani_frame_t *f0 = &out->frames[0];
        print("ani_load: '%s' %d frames, frame0=(%d,%d)-(%d,%d) %dx%d\n",
              host_path, out->n_frames,
              (i32)f0->x1, (i32)f0->y1, (i32)f0->x2, (i32)f0->y2,
              (i32)(f0->x2 - f0->x1 + 1), (i32)(f0->y2 - f0->y1 + 1));
        return 1;
    }
    return 0;
}

void ani_free(ani_t *ani) {
    if (!ani) return;
    for (i32 i = 0; i < ANI_MAX_FRAMES; i++) {
        if (ani->frames[i].pixels) {
            free(ani->frames[i].pixels);
            ani->frames[i].pixels = NULL;
        }
    }
    ani->n_frames = 0;
}
