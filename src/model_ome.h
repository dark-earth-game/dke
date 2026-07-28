#pragma once

/*
 * model_ome.h — Dark Earth .OME (Object Mesh Editor) loader.
 *
 * .OME files are the "3DITOR v1.3 / v2.0" nested chunk format used for
 * every 3D asset in the game (level objects, characters, weapons).
 * Structure (empirical, from tools/src/dump_ome.py):
 *
 *   outer MATR   — material table (CUIR, ACIER, ROUIL, ... tags)
 *   nested OBJC  — object container (holds N ATOMF descriptors)
 *   nested ATOMF — per-atom geometry (vertices + faces + material refs)
 *
 * Each chunk begins with:
 *   +0x00  16-byte signature "3DITOR v?.? TAG "  (17 for ATOMF)
 *   +0x10  16-byte dashes or per-chunk header bytes
 *   +0x20  16-byte filename padded to 16
 *   +0x30  48-byte full path (F:\DEV\DKE\...) padded to 48
 *   +0x60  chunk body starts
 *
 * Blocker (documented in PLAN.md): ATOMF bodies contain runtime-process
 * heap pointers (0x009Axxxx style) that were serialised as-is. Any
 * loader that walks the body must skip / re-derive those addresses.
 *
 * This pass exposes an inventory-only API: parse the outer structure and
 * log which chunks are present without decoding the geometry. Enough to
 * drive next-pass field-layout guessing and let scene.c prefetch mesh
 * paths without stalling on the pointer fix-up work.
 */

#include "libsys/common.h"

typedef enum {
    OME_CHUNK_MATR,   /* material table */
    OME_CHUNK_OBJC,   /* object container */
    OME_CHUNK_ATOMF,  /* atom / geometry */
    OME_CHUNK_UNKNOWN
} ome_chunk_kind_e;

typedef struct {
    ome_chunk_kind_e kind;
    i32              offset;      /* byte offset in the source file */
    i32              length;      /* bytes until next chunk / EOF   */
    c8               name[16];    /* filename from the +0x20 field   */
} ome_chunk_t;

typedef struct {
    ome_chunk_t     *chunks;      /* heap-owned; free via model_ome_free */
    i32              chunk_count;
    i32              matr_count;
    i32              objc_count;
    i32              atomf_count;
    /* Raw file kept live for later chunk-body decode passes. */
    u8              *raw;
    i32              raw_size;
} ome_file_t;

/* -------------------------------------------------------------------------
 * MATR material colour table
 *
 * Empirical layout (verified on P00.OME):
 *   MATR body header: "3Ditor v1.1 material library.\0" at body+0x0D0.
 *   Material count (i32 LE) at body+0x230 (may be garbage — scan instead).
 *   Records start at body+0x240; each record is 72 bytes (0x48):
 *     [0..3]   i32  diffuse R (low byte = u8 value, upper 3 bytes = 0)
 *     [4..7]   i32  diffuse G
 *     [8..11]  i32  diffuse B
 *     [12..19] 8 bytes unknown / padding
 *     [20..31] char name[12] (null-terminated material name, Latin-1)
 *     [32..47] secondary colour fields (mirrors of R/G, often)
 *     [48..59] 12 bytes unknown
 *     [60..63] heap ptr (ignored)
 *     [64..67] heap ptr (ignored)
 *     [68..71] heap ptr (ignored)
 *
 * Record is considered valid when:
 *   R ≤ 255, G ≤ 255, B ≤ 255, AND (R|G|B) != 0 OR name[0] is printable.
 * Scan stops at the first all-zero record (up to OME_MATR_MAX_MATS).
 * ------------------------------------------------------------------------- */

#define OME_MATR_MAX_MATS 24

typedef struct {
    u8   r, g, b;       /* diffuse colour (raw from record) */
    c8   name[12];      /* null-terminated material name */
} ome_mat_t;

typedef struct {
    ome_mat_t mats[OME_MATR_MAX_MATS];
    i32       mat_count;
} ome_matr_t;

/*
 * ome_parse_matr — decode the MATR chunk body into a colour table.
 * raw = pointer to sig_offset + 0x60 in the raw file buffer.
 * raw_len = bytes from raw to end of chunk (span - 0x60).
 * Returns 1 on success (at least one material parsed), 0 on failure.
 */
i32 ome_parse_matr(const u8 *raw, i32 raw_len, ome_matr_t *out);

/*
 * model_ome_open — load an .OME file from a real host path and populate
 * the chunk table. Returns 1 on success, 0 on failure. Frees any prior
 * contents of *out.
 */
i32 model_ome_open(const c8 *host_path, ome_file_t *out);

/*
 * model_ome_free — release the chunk table and raw buffer owned by *f.
 */
void model_ome_free(ome_file_t *f);

/* -------------------------------------------------------------------------
 * ATOMF body decoder (Slice F pass 3)
 * -------------------------------------------------------------------------
 *
 * Empirical layout (verified on OB98.OME and P00.OME):
 *
 *   body header (16 bytes):
 *     +0x00  i32 = 0
 *     +0x04  i32 = 0
 *     +0x08  i32 = heap ptr (original process face-list head, ignored)
 *     +0x0C  i32 = heap ptr (ignored)
 *
 *   vertex records (32 bytes each):
 *     [0] x   [1] y   [2] z          — position (signed, raw units)
 *     [3] nx  [4] ny  [5] nz         — per-vertex normal (same units)
 *     [6] next_ptr (heap, ignored)
 *     [7] flag: 0x00200000 if ptr present, else 0
 *     → last vertex record has flag == 0.
 *     → scan until field[7] is neither 0 nor 0x200000 to find face section.
 *
 *   face records (96 bytes each, preceded by a 4-byte heap ptr for faces 1+
 *   and followed by a 4-byte tail ptr; n_faces = remaining_bytes / 100):
 *     [0]  face type  (always 100 = 0x64)
 *     [1]  vcnt       (3 = triangle, 4 = quad)
 *     [2]  idx[0]
 *     [3]  idx[1]
 *     [4]  idx[2]
 *     [5]  idx[3]     (valid only when vcnt == 4)
 *     [6]  face_nx    (Q14: 16384 == 1.0)
 *     [7]  face_ny
 *     [8]  face_nz
 *     [9]  mat_id     (index into MATR material table)
 *     [10..23]        material params + UV data (not decoded in pass 3)
 * ------------------------------------------------------------------------- */

typedef struct {
    i32 x, y, z;     /* vertex position (raw .OME signed integers) */
    i32 nx, ny, nz;  /* per-vertex normal (raw, Q14-ish) */
} ome_vert_t;

typedef struct {
    u8  vcnt;        /* 3 = triangle, 4 = quad */
    u8  idx[4];      /* vertex indices (vcnt entries valid) */
    i32 nx, ny, nz;  /* face normal, Q14 */
    i32 mat_id;      /* material index */
} ome_face_t;

typedef struct {
    ome_vert_t *verts;
    i32         vert_count;
    ome_face_t *faces;
    i32         face_count;
} ome_atom_t;

/* -------------------------------------------------------------------------
 * OBJC chunk parser
 * ------------------------------------------------------------------------- */

#define OME_MAX_BONES      32
#define OME_MAX_ATOMSLOTS  24   /* P12 has 21 atom slots; keep headroom */

typedef struct {
    char name[16];
    i32  tx, ty, tz;
    i32  h2;               /* parent index; -1 = root */
} ome_bone_t;

typedef struct {
    ome_bone_t  bones[OME_MAX_BONES];
    i32         n_bones;
    char        atomf_slot_name[OME_MAX_ATOMSLOTS][16];
    i32         n_atomslots;
    i32         world_tx[OME_MAX_ATOMSLOTS];
    i32         world_ty[OME_MAX_ATOMSLOTS];
    i32         world_tz[OME_MAX_ATOMSLOTS];
    /* Accumulated world rotation per atom (Q14 3x3, row-major m[row*3+col]),
     * from the CAN keyframe hierarchy — applied to the atom's vertices so the
     * limb geometry rotates with the pose, not just its pivot position. */
    i32         world_rot[OME_MAX_ATOMSLOTS][9];
    i32         can_to_render[OME_MAX_BONES]; /* CAN slot j → render atom (-1 = no geo) */
} ome_objc_t;

/* Parse OBJC chunk body. objc_sig_off = offset of "3DITOR v2.0 OBJC" in raw[]. */
i32 ome_parse_objc(const u8 *raw, i32 raw_len, i32 objc_sig_off, ome_objc_t *out);

/*
 * ome_decode_atomf — decode one ATOMF chunk body into verts + faces.
 *
 * raw     = pointer to sig_offset + 0x60 in the raw file buffer.
 * raw_len = bytes from raw to end of chunk (span - 0x60).
 * out     = caller-provided struct; filled on success.
 *
 * out->verts and out->faces are heap-allocated with malloc.
 * Call ome_atom_free() to release.
 * Returns 1 on success, 0 on failure (bad data or alloc error).
 */
i32  ome_decode_atomf(const u8 *raw, i32 raw_len, ome_atom_t *out);
void ome_atom_free(ome_atom_t *atom);
