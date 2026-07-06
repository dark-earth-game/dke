#pragma once

/*
 * can.h — CAN character animation loader (Slice F pass 5).
 *
 * CAN files = packed character animations, one per character (P00.CAN etc.).
 * Located in game_path/ANM/P{num}.CAN
 *
 * File format (from sub_41DE86 IDA analysis):
 *   [0x0000..0x13FF]: ptr table — 640 × 8-byte entries
 *     each entry: { u32 name_hash, u32 file_offset }
 *     size of animation N = offset[N+1] - offset[N]
 *   [0x1400..EOF]: packed animation blocks
 *
 * Each animation block:
 *   [0x000..0x007]: c8 name[8]  (e.g. "p00mar14")
 *   [0x008]:        u8  zero
 *   [0x009]:        u8  n_mes    (message record count)
 *   [0x00A]:        u8  n_frames (keyframe count, max 20)
 *   [0x00B]:        u8  zero
 *   [0x00C..0x05B]: frame_ptrs[20] — original heap ptrs (ignored in port)
 *   [0x05C..0x09B]: mes_ptrs[16]   — original heap ptrs (ignored)
 *   [0x09C..0x10F]: timing / loop data
 *   [0x110 + i*0xCE]: keyframe i  (0xCE bytes, for i=0..n_frames-1)
 *
 * Keyframe layout (0xCE = 206 bytes, from sub_415021 IDA analysis):
 *   [0..1]:   i16  header (frame flags / speed)
 *   [2 + j*6 .. 2 + j*6 + 5]:  for atom_slot j (0..33):
 *     i16 rx   (rotation X, u16 0..65535 = 0..360°)
 *     i16 ry   (rotation Y)
 *     i16 rz   (rotation Z)
 *   Total: 2 + 34*6 = 206 bytes = 0xCE ✓
 *
 * Angle conversion: game u16 angle → cosine_table index
 *   table_idx = (game_angle >> 4) & 0xFFF   (4096-entry table = game 65536)
 *
 * Source asm: sub_415021 (keyframe→atom write), sub_41DE86 (CAN loader),
 *   sub_41AED1 (animation update), create_rotation_matrix (0x48fa20).
 */

#include "libsys/common.h"

#define CAN_MAX_FRAMES   20
#define CAN_MAX_ATOMSLOTS 34    /* max atom slots per keyframe (0x22) */
#define CAN_FRAME_BYTES  0xCE   /* 206 bytes per keyframe */
#define CAN_HEADER_BYTES 0x110  /* 272 bytes animation header */

/* Per-atom rotation from one keyframe (in game u16 angle units). */
typedef struct {
    u16 rx, ry, rz;   /* rotation X/Y/Z: u16 0..65535 = 0..360° */
} can_atom_rot_t;

/* One decoded keyframe: rotations for all 34 atom slots. */
typedef struct {
    i16           header;                     /* frame flags/speed */
    can_atom_rot_t atoms[CAN_MAX_ATOMSLOTS];  /* per-slot rotations */
} can_frame_t;

/* One loaded animation. */
typedef struct {
    c8          name[9];          /* null-terminated animation name */
    u8          n_frames;         /* actual keyframe count */
    can_frame_t frames[CAN_MAX_FRAMES];
} can_anim_t;

/*
 * can_load_first — open a .CAN file and load the first animation
 * whose name matches the given prefix (or load animation index 0 if
 * prefix is NULL).
 *
 * host_path: real filesystem path to the .CAN file.
 * out:       caller-supplied struct, filled on success.
 * Returns 1 on success, 0 on failure.
 */
i32 can_load_first(const c8 *host_path, can_anim_t *out);
