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
