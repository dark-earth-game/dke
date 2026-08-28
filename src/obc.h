#ifndef DKE_OBC_H
#define DKE_OBC_H

/*
 * obc.h — 3DITOR object hierarchy (.OBC) + atom geometry (.ATM) loader,
 *         plus the fixed-point matrix primitives the engine builds on.
 *
 * Asm references (dkev.exe):
 *   sub_4538FB @ 0x4538FB — OBC file loader (.COB disk-cache wrapper)
 *   sub_452B20 @ 0x452B20 — OBC body parser: node array + name table + fixup
 *   sub_452EA6 @ 0x452EA6 — generic length-prefixed record reader (4 kinds)
 *   sub_4531D6 @ 0x4531D6 — atom name -> "<base_dir><name>.ATM" path
 *   sub_4532C7 @ 0x4532C7 — "does this name already carry an extension?"
 *   sub_453346 @ 0x453346 — ATM file loader (atom header + verts + faces)
 *   sub_450BEB @ 0x450BEB — material lookup through the MAT reverse LUT
 *   get_node_bounds @ 0x436651 — hierarchy-wide transformed AABB
 *   create_single_control_point @ 0x404D92 — root "Clefs" bounding sphere
 *   process_scene_hierarchy_with_memory @ 0x404C26 — per-node control points
 *   create_rotation_matrix @ 0x48FA20, create_position_matrix @ 0x48FD60,
 *   multiply_matrices     @ 0x4903C0, transform_vertex       @ 0x4901E0,
 *   transform_vector      @ 0x48F424
 *
 * The original keeps every one of these as a flat 32-bit blob with embedded
 * pointers (node = 0xB4 bytes, atom = 0x40 bytes).  On a 64-bit host those
 * layouts alias — see docs/functions/nav_collision.md and the entity blob
 * notes — so the structs below name every field the engine touches and the
 * loader converts disk records field by field instead of memcpy'ing the blob.
 * Disk offsets are quoted next to each field so the mapping stays checkable.
 */

#include "libsys/common.h"

/* -------------------------------------------------------------------------
 * Fixed-point transform, 0x30 bytes in the original.
 *
 *   m[0..8] — 3x3 rotation, Q14 (0x4000 == 1.0), row-major on disk but
 *             applied transposed by transform_vector / multiply_matrices
 *             (matches the in-game camera convention).
 *   t[0..2] — translation, raw world units (asm offsets 0x24 / 0x28 / 0x2C).
 * ---------------------------------------------------------------------- */
typedef struct {
    i32 m[9];
    i32 t[3];
} fmat_t;

/* create_rotation_matrix @ 0x48FA20 — angles are 16-bit BAM (0x10000 = 360°). */
void fmat_rotation(fmat_t *out, i32 ax, i32 ay, i32 az);

/* create_position_matrix @ 0x48FD60 — identity rotation + translation. */
void fmat_position(fmat_t *out, i32 x, i32 y, i32 z);

/* multiply_matrices @ 0x4903C0 — b->t = transpose(a->m) * b->t.
 * Only the translation of b is touched; b->m is left alone. */
void fmat_rotate_translation(const fmat_t *a, fmat_t *b);

/* transform_vertex @ 0x4901E0 — m->m = r->m * m->m (translation untouched). */
void fmat_premul_rotation(const fmat_t *r, fmat_t *m);

/* transform_vector @ 0x48F424 — v = transpose(m->m) * v + m->t, in place. */
void fmat_transform_point(const fmat_t *m, i32 *v);

/* -------------------------------------------------------------------------
 * Atom geometry (.ATM / ATOMF chunk)
 * ---------------------------------------------------------------------- */

/* Vertex record: 8 dwords / 32 bytes on disk and in memory. */
#define ATM_VERT_STRIDE 8
#define ATM_VERT_X   0
#define ATM_VERT_Y   1
#define ATM_VERT_Z   2
#define ATM_VERT_NX  3
#define ATM_VERT_NY  4
#define ATM_VERT_NZ  5

/* Face record: 25 dwords / 100 bytes (0x64) on disk and in memory. */
#define ATM_FACE_STRIDE  25
#define ATM_FACE_SIZE    0   /* +0x00 — always 100 */
#define ATM_FACE_VCNT    1   /* +0x04 — 3 = triangle, 4 = quad */
#define ATM_FACE_IDX     2   /* +0x08..+0x14 — 4 vertex indices */
#define ATM_FACE_NX      6   /* +0x18 — face normal, Q14 */
#define ATM_FACE_NY      7   /* +0x1C */
#define ATM_FACE_NZ      8   /* +0x20 */
#define ATM_FACE_MAT     9   /* +0x24 — material id, or -1 for none */
#define ATM_FACE_FLAGS  12   /* +0x30 — bit2|bit3 set = textured */
#define ATM_FACE_UV     15   /* +0x3C..+0x58 — 4 (u,v) pairs, Q16.16 */
#define ATM_FACE_RT     24   /* +0x60 — runtime scratch, zeroed on load */

typedef struct {
    c8   name[16];       /* +0x04 */
    i32  flags;          /* +0x14 — bit 0x400 cleared once loaded */
    i32  vert_count;     /* +0x1C */
    i32  vert_max;       /* +0x20 */
    i32  face_count;     /* +0x24 */
    i32  face_max;       /* +0x28 */
    i32 *verts;          /* +0x38 — vert_max * ATM_VERT_STRIDE dwords */
    i32 *faces;          /* +0x3C — face_max * ATM_FACE_STRIDE dwords */
} atm_atom_t;

/* -------------------------------------------------------------------------
 * Hierarchy node (.OBC record, 0xB4 bytes on disk)
 * ---------------------------------------------------------------------- */

/* "Clefs" control point, allocated 0x1C bytes at node+0xA0. */
typedef struct {
    i32 radius;          /* +0x00 — half the largest AABB extent */
    i32 centre[3];       /* +0x04..+0x0C — AABB centre, root-relative */
    i32 reserved[3];     /* +0x10..+0x18 */
} obc_clefs_t;

typedef struct obc_node_s {
    c8   name[16];                 /* +0x04 — "tree", "arme", bone names */
    i32  raw_14, raw_18, raw_1C;   /* +0x14..+0x1C */
    atm_atom_t *atom[3];           /* +0x20/+0x24/+0x28 — LOD slots */
    i32  raw_2C[4];                /* +0x2C..+0x38 */
    i32  pos[3];                   /* +0x3C/+0x40/+0x44 — local translation */
    i32  raw_48[3];                /* +0x48..+0x50 */
    i32  rot[3];                   /* +0x54/+0x58/+0x5C — local BAM angles */
    i32  raw_60[6];                /* +0x60..+0x74 */
    struct obc_node_s *parent;     /* +0x78 */
    struct obc_node_s *child;      /* +0x7C — first child */
    struct obc_node_s *prev;       /* +0x80 — previous sibling */
    struct obc_node_s *next;       /* +0x84 — next sibling */
    i32  raw_88[5];                /* +0x88 (forced 0) .. +0x98 */
    i32  has_clefs;                /* +0x9C */
    obc_clefs_t *clefs;            /* +0xA0 */
} obc_node_t;

/*
 * obc_load — sub_4538FB @ 0x4538FB + sub_452B20 @ 0x452B20.
 *
 *   obc_vpath — LIBSYS virtual path of the .OBC ("OBJETS>FLAM>FLAM.OBC")
 *   base_dir  — directory prefix atom names resolve against ("OBJETS>FLAM>"),
 *               i.e. byte_4DE330 as built by setup_creature @ 0x44F2A0
 *   game_path — host root passed to ls_resolve_path
 *
 * The original also maintains a ".COB" pre-parsed cache next to the object
 * directory (written only on the CD install path, guarded by is_using_hdd).
 * The port always parses the source file, so that cache is not reproduced.
 *
 * Returns the root node, or NULL. Release with obc_free().
 */
obc_node_t *obc_load(const c8 *obc_vpath, const c8 *base_dir,
                     const c8 *game_path);

/*
 * obc_load_ome — sub_44F454 @ 0x44F454, the HDD-install path.
 *
 * setup_creature @ 0x44F2A0 only reaches obc_load() on a CD install; with the
 * game on disk it passes the .OME straight to sub_44F454, which strips the
 * "CTX3" wrapper and builds the same tree from the OBJC + ATOM chunks packed
 * into that single file. Chunk bodies are byte-identical to the standalone
 * .OBC / .ATM files, so both front ends share one parser.
 *
 * Returns the root node, or NULL. Release with obc_free().
 */
obc_node_t *obc_load_ome(const c8 *ome_vpath, const c8 *game_path);

/* Release a tree built by obc_load, including every atom it references. */
void obc_free(obc_node_t *root);

/*
 * obc_node_bounds — get_node_bounds @ 0x436651.
 *
 * Walks the whole sub-tree rooted at `root`, accumulating each node's local
 * rotation/translation, transforms every vertex of every atom face and keeps
 * the component-wise min/max. Both outputs are int[3] and are returned
 * relative to the root node's own pivot (root->pos is subtracted).
 */
void obc_node_bounds(const obc_node_t *root, i32 *out_min, i32 *out_max);

/* -------------------------------------------------------------------------
 * Node types
 *
 * get_node_type @ 0x41DE03 maps a node name to an index into a fixed 34-entry
 * table at 0x4B2BA0 (stride 7) — the same table sub_41F6BA @ 0x41F6BA uses to
 * fill the entity bone slots, so a node's "type" and its bone slot are one and
 * the same number. "armeb" is special-cased to 0x22; anything unknown is -1.
 * ---------------------------------------------------------------------- */
#define OBC_NODE_TYPE_COUNT 34
#define OBC_NODE_TYPE_TREE  0x00
#define OBC_NODE_TYPE_ARME  0x1F
#define OBC_NODE_TYPE_ARMEB 0x22

extern const c8 *const obc_node_type_names[OBC_NODE_TYPE_COUNT];

i32 obc_node_type(const c8 *name);

/*
 * obc_process_hierarchy — process_scene_hierarchy_with_memory @ 0x404C26 and
 * process_scene_hierarchy_control_points @ 0x4040B8.
 *
 * Gives every limb node a chain of 1..4 collision spheres sized from its own
 * mesh AABB, laid out along the limb's longest axis. How many spheres and how
 * fat they are is a per-bone constant — see the table in obc.c.
 *
 * Starts at `entity_root`'s first child when it has one, else at the root
 * itself, so the entity's own transform node is skipped.
 */
void obc_process_hierarchy(obc_node_t *entity_root);

/*
 * obc_create_control_point — create_single_control_point @ 0x404D92.
 *
 * Builds the root bounding sphere ("obj->Clefs") for the hierarchy hanging
 * off `root`: radius = max AABB extent / 2, centre = AABB centre.
 * Sets root->has_clefs = 1 and allocates root->clefs.
 */
void obc_create_control_point(obc_node_t *root);

#endif /* DKE_OBC_H */
