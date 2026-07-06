#pragma once

/*
 * anim2d.h — 2D pre-rendered character animation loader (Slice F pass 4).
 *
 * Dark Earth characters are NOT rendered in real-time 3D. The OME/CAN data
 * was used offline to generate pre-rendered sprite frames stored as ANI files.
 *
 * ANI file naming: N{level:02d}C{cam}A{anim}.ANI
 *   in DATAS>L{level:02d}>ANIM2D>
 * Example: N11C0A1.ANI = level 11, camera 0, animation 1.
 *
 * ANI frame record (sequential, from file start):
 *   i16 LE  x1  (left,   inclusive)
 *   i16 LE  y1  (top,    inclusive)
 *   i16 LE  x2  (right,  inclusive)  width  = x2-x1+1
 *   i16 LE  y2  (bottom, inclusive)  height = y2-y1+1
 *   i32 LE  uncomp_size  (= width * height uncompressed pixels)
 *   i32 LE  comp_size    (bytes stored in file for this frame)
 *   [comp_size bytes]    pixel data (LZSS-compressed if comp_size != uncomp_size)
 *
 * ANI file footer (last 12 bytes):
 *   [sz-12]  u8  direction flag
 *   [sz-11]  u8  loop flag
 *   [sz-10]  i32 LE  timing value
 *   [sz- 6]  u8  visible_frames count
 *   [sz- 2]  u8  format_type (1 = extended footer present)
 *   [sz- 1]  u8  n_frames
 *
 * Compression: classic LZSS, 4096-byte ring buffer, init'd to 0x20 spaces,
 *   initial fill pos = 0xFEE.  4-byte uncompressed-count header precedes
 *   the bitstream.  Literal flag = bit set; back-reference = bit clear,
 *   2-byte code: 12-bit offset + 4-bit (run_len - 2), run = 3..18.
 *
 * Source asm: sub_41333A (ANI loader), sub_4780B4 (LZSS decompressor),
 *   sub_4130FB (animation table builder).
 */

#include "libsys/common.h"

#define ANI_MAX_FRAMES    32
#define ANI_MAX_PIXELS    (640 * 480)   /* worst-case sprite buffer */

/* Per-frame sprite descriptor. */
typedef struct {
    i16 x1, y1, x2, y2;    /* screen bounding box (inclusive) */
    i32 uncomp_size;         /* uncompressed pixel byte count */
    i32 comp_size;           /* bytes in file (compressed or raw) */
    u8 *pixels;              /* heap-allocated uncompressed pixels; NULL until loaded */
} ani_frame_t;

typedef struct {
    ani_frame_t frames[ANI_MAX_FRAMES];
    i32         n_frames;
    i32         visible_frames;
    u8          direction;
    u8          loop_flag;
    i32         timing;
} ani_t;

/*
 * ani_load — load and decompress an ANI file from the given host path.
 * out->frames[i].pixels are heap-allocated; call ani_free to release.
 * Returns 1 on success, 0 on failure.
 */
i32  ani_load(const c8 *host_path, ani_t *out);
void ani_free(ani_t *ani);

/*
 * lzss_decompress — decompress LZSS-compressed data (sub_4780B4 port).
 * src: compressed bytes (first 4 bytes = uncompressed count, little-endian).
 * dst: output buffer (must be at least uncomp_size bytes).
 * Returns number of bytes written, or 0 on error.
 */
i32 lzss_decompress(const u8 *src, i32 src_size, u8 *dst, i32 dst_size);
