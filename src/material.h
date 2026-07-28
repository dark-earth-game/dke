#ifndef DKE_MATERIAL_H
#define DKE_MATERIAL_H

#include "libsys/common.h"

#define MAT_HEADER_SIZE     32
#define MAT_LIB_SIZE        352   /* 0x160 */
#define MAT_BITMAP_SIZE     276
#define MAT_OBJECT_SIZE     72    /* 0x48 */
#define MAT_MATERIAL_SIZE   72    /* 0x48 */
#define MAT_NAME_LEN        256
#define MAT_MAX_TEXTURES    16

typedef struct {
    c8   filename[MAT_NAME_LEN]; /* +0x000: texture filename from 3DITOR */
    u32  uv_field;               /* +0x100: UV-related, copied to material */
    void *texture;               /* +0x104: runtime loaded image pointer */
    u8   extra[12];              /* +0x108: remaining metadata */
} mat_bitmap_t;

typedef struct {
    u8   data[32];               /* +0x00 */
    u8   visible_flag;           /* +0x20 */
    u8   pad[19];                /* +0x21..0x33 */
    u32  u_offset;               /* +0x34 */
    u32  pad2;                   /* +0x38 */
    u32  v_offset;               /* +0x3C */
    u32  flags;                  /* +0x40 */
    u32  texture_index;          /* +0x44 */
} mat_object_t;

typedef struct {
    u8   header[16];             /* +0x000 */
    c8   name[MAT_NAME_LEN];    /* +0x010: filename, high bytes reused as reverse LUT */
    u8   misc[0x34];             /* +0x110..0x143 */
    u32  nmaterials;             /* +0x144 */
    u32  nobjects;               /* +0x148 */
    u32  nbitmaps;               /* +0x14C */
    u32  reserved[2];            /* +0x150, +0x154 */
    void *materials;             /* +0x158: mat_material_t[] */
    void *bitmaps;               /* +0x15C: mat_bitmap_t[] */
} mat_lib_t;

mat_lib_t *mat_load(const c8 *path, const c8 *game_path, i32 is_hdd, void *default_tex);
void mat_free(mat_lib_t *lib);

#endif /* DKE_MATERIAL_H */
