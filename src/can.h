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

/*
 * can_load_named — load the clip whose name hashes to clip_name (e.g.
 * "p12arr01" selects the ARR01 arrival clip). Falls back to the first clip
 * if no hash match. Returns 1 on success, 0 on failure.
 */
i32 can_load_named(const c8 *host_path, const c8 *clip_name, can_anim_t *out);

/* -------------------------------------------------------------------------
 * Engine animation records — get_animation_id @ 0x41E26B.
 *
 * The engine keeps a 200-slot cache of 0x110-byte records (dword_511744)
 * whose layout is the CAN block header verbatim. The keyframe and message
 * pointer arrays inside that header (+0x0C / +0x5C) are 32-bit heap pointers
 * in the original; here they live in frame[] / mes[] and the header copies
 * are left untouched.
 *
 * Header fields (original offsets):
 *   +0x00 c8[8] name        +0x09 i8 n_mes        +0x0A i8 n_frames
 *   +0xAC i16[20] keyframe time stamps (animation ticks, [0] = 0)
 *   +0xD4 u8[19] ease-in per segment    +0xE7 u8[19] ease-out per segment
 *   +0xFA i16 flags:
 *          0x0001 root yaw carried by the clip (process_entity_state_change)
 *          0x0020 remember the start position (+0xE88 / +0xE90)
 *          0x0080 "master" hold (+0xC6C)
 *          0x0100 blend time in +0x108 low word
 *          0x0200 animation is over when the last keyframe is reached
 *          0x0800 matrix (slerp) blend instead of per-angle lerp
 *          0x1000 blend duration = +0x108 >> 16
 *   +0xFC i32 root yaw          +0x108 i32 blend times
 * Keyframe: i16 header, then 34 x (rx, ry, rz) BAM, one triple per bone slot.
 * Message:  12 bytes; +1 type char, +8 parameter.
 * ---------------------------------------------------------------------- */
#define CAN_REC_HDR    0x110
#define CAN_REC_SLOTS  200
#define CAN_MES_BYTES  12

typedef struct {
    u8   hdr[CAN_REC_HDR];
    i16 *frame[CAN_MAX_FRAMES];
    u8  *mes[CAN_MAX_FRAMES];
} can_rec_t;

#define CAN_NMES(r)        ((i32)(i8)(r)->hdr[0x09])
#define CAN_NFRAMES(r)     ((i32)(i8)(r)->hdr[0x0A])
#define CAN_TIME(r, i)     (*(i16 *)&(r)->hdr[0xAC + 2 * (i)])
#define CAN_EASE_IN(r, i)  ((r)->hdr[0xD4 + (i)])
#define CAN_EASE_OUT(r, i) ((r)->hdr[0xE7 + (i)])
#define CAN_FLAGS(r)       (*(i16 *)&(r)->hdr[0xFA])
#define CAN_ROOT_YAW(r)    (*(i32 *)&(r)->hdr[0xFC])
#define CAN_BLEND(r)       (*(i32 *)&(r)->hdr[0x108])
/* Duration = time stamp of the last keyframe (asm reads +0xAA + n*2). */
#define CAN_DURATION(r)    (CAN_NFRAMES(r) > 0 ? (i32)CAN_TIME(r, CAN_NFRAMES(r) - 1) : 0)

/* get_animation_id @ 0x41E26B — fetch (loading on demand) the record for a
 * clip name such as "p12arr03" or "g01arr01". Never NULL once the CAN files
 * are present: unknown names fall back to g01arr01 like the original. The
 * returned record can be recycled by later calls, as in the engine. */
can_rec_t *can_get_anim(const c8 *name);

/* Drop every cached record (level change / shutdown). */
void can_cache_reset(void);
