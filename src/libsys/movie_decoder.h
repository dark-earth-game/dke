#pragma once

/*
 * CRH (SEB) movie decoder — C port of tools/src/CRH_to_MP4.py
 *
 * Format summary:
 *   Header (52 bytes):
 *     [0..1]   u16  version
 *     [2..49]  u8   audio header (48 bytes):
 *                     [0..3]   u32 sample_rate (LE)
 *                     [14..15] u16 channels (LE)
 *                     rest ignored
 *     [50..51] u16  frame_count
 *     [52..53] u16  fps
 *   payload_offset = 54
 *
 *   Per-frame:
 *     A_BYTES (2942) bytes  — audio block (skipped for now)
 *     16 bytes video header — u32 full, u32 c0, u32 c1, u32 c2
 *     c0 bytes              — plane 0 compressed (LSB bitstream)
 *     c1 bytes              — plane 1 compressed
 *     c2 bytes              — plane 2 compressed
 *     u32 tile_data_size
 *     tile_data_size bytes  — tile colour/mode table
 *
 *   Component decode: flag=0 → RLE fill (5-bit val, 8-bit len)
 *                     flag=1 → back-ref (src-select, OFFSET_BITS offset, 8-bit len)
 *   RGB reconstruction: tile grid, per-tile 2-bit mode + lookup tables
 *   Pixel expand: five8(v) = (v<<3)|(v>>2)
 *
 * Port mapping (Python → C):
 *   parse_header         → crh_open  (+ detect_dims)
 *   detect_dims          → static detect_dims
 *   dec_comp_framepred   → static decode_component
 *   reconstruct_rgb      → static reconstruct_rgb  (in movie_decoder.c)
 *
 * Audio: skipped — fseek(A_BYTES) per frame. Pending future port.
 */

#include "common.h"
#include <stdio.h>

/* Per the Python reference. */
#define CRH_A_BYTES      2942
#define CRH_OFFSET_BITS  16

/*
 * tile_bits_mode:
 *   0 = field-rev  (LSB bitreader, bits reversed per byte)  — preferred
 *   1 = byte-rev   (MSB bitreader on byte-reversed tile data)
 */
#define CRH_TILE_FIELD_REV  0
#define CRH_TILE_BYTE_REV   1

typedef struct {
    u16  version;
    u32  sample_rate;
    u16  channels;
    u16  frame_count;
    u16  fps;
    i32  width;
    i32  height;
    i32  tile_bits_mode;    /* CRH_TILE_FIELD_REV or CRH_TILE_BYTE_REV */

    FILE *fp;
    i64  payload_offset;    /* file offset of first frame */

    u8  *prev_plane[3];     /* previous-frame planes for inter-prediction */
    i32  plane_size;        /* width * height */

    /* current frame index (0-based) */
    u16  current_frame;
} crh_reader_t;

/*
 * crh_open — open a CRH file, parse header, detect dimensions.
 * Returns true on success.  On failure, fp is NULL (nothing to close).
 */
b8   crh_open(crh_reader_t *r, const c8 *path);

/*
 * crh_next_frame — decode the next video frame into rgb_out.
 * rgb_out must be at least width * height * 3 bytes.
 * Returns true if a frame was decoded, false on EOF or error.
 * Audio block is skipped (fseek forward by A_BYTES each call).
 */
b8   crh_next_frame(crh_reader_t *r, u8 *rgb_out);

/*
 * crh_close — release all resources associated with r.
 * Safe to call even if crh_open failed (checks fp != NULL).
 */
void crh_close(crh_reader_t *r);

/*
 * crh_next_frame_ex — decode the next frame into rgb_out.
 * If audio_out is non-NULL, the raw CRH_A_BYTES audio block is copied there
 * instead of being skipped.  Caller must provide a buffer of at least
 * CRH_A_BYTES bytes.  Pass NULL to skip (identical to crh_next_frame).
 * Returns true if a frame was decoded, false on EOF or error.
 */
b8   crh_next_frame_ex(crh_reader_t *r, u8 *rgb_out, u8 *audio_out);

/*
 * crh_decode_audio — port of Python decode_audio_block.
 *
 * Decodes a raw CRH audio block (CRH_A_BYTES bytes) to signed 16-bit PCM.
 *
 * Encoding:
 *   Head: channels × i16 LE seed values → emitted as first sample per channel.
 *   Remainder: interleaved per-channel bytes.
 *     b = next byte for channel ch.
 *     step  = (b & 1) ? 16 : 512   (LSB=1 → FINE_STEP, LSB=0 → COARSE_STEP)
 *     delta = sign7(b >> 1) * step
 *     sign7(v): (v & 0x40) ? v - 128 : v
 *     accumulate prev[ch] += delta, clamp to i16 range, emit.
 *
 * raw     — pointer to raw audio block (CRH_A_BYTES bytes)
 * raw_len — number of valid bytes in raw (pass CRH_A_BYTES)
 * channels — 1 or 2 (taken from crh_reader_t.channels)
 * dst     — output buffer for interleaved i16 PCM samples.
 *           Must be sized for: channels * (1 + samples_per_ch) * sizeof(i16)
 *           where samples_per_ch = max(0, raw_len - 2*channels) / max(1, channels)
 *
 * Returns total bytes written to dst.
 */
i32  crh_decode_audio(const u8 *raw, i32 raw_len, i32 channels, i16 *dst);
