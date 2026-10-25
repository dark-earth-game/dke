#ifndef DKE_OBJECT_H
#define DKE_OBJECT_H

/*
 * object.h — level props and items ("objets").
 *
 * The original keeps 8 object slots (unk_4DAB60, 0x10C bytes each), filled
 * lazily by the script's EtatObjet (sub_445189 -> case 6 @ 0x44B00F):
 *   "IMnn"  level prop  -> DATAS>Lnn>IMnn.OME   (sub_412A2A / sub_412979)
 *   "OBnnX" item        -> OBJETS>OBnn.OME, X = instance letter A..F
 *                          (sub_472FB5 / sub_472C5C)
 * Placement (sub_475F7A) puts the object tree at the script position and
 * rotations; level props get a 4-vertex collision polygon from their
 * bounds (sub_47701F) that the actor collision (sub_459E80) tests.
 * AnimObjet (sub_412B94) turns one node about one axis over time.
 */

#include "libsys/common.h"
#include "obc.h"
#include "model_ome.h"

#define OBJ_SLOTS     8
#define OBJ_VARIANTS  6

#define OBJ_F_PLACED  0x02
#define OBJ_F_OPEN    0x08
#define OBJ_F_CLOSED  0x10
#define OBJ_F_FREE    0x20

typedef struct {
    i32         used;
    c8          name[8];          /* +0xA8, first 4 chars are the key   */
    obc_node_t *root;             /* +0x50 */
    ome_matr_t  matr;             /* +0x88 */
    i32         is_prop;          /* "IM" name                          */
    i32         flags;            /* +0xC4 */
    i32         mode;             /* +0xEC (a7): Z / shadow mode        */
    i32         pos[3];           /* +0xB4..+0xBC */
    i32         rot[3];           /* +0xE0..+0xE8 */
    /* Item instances A..F (dword_4DCDA0 / dword_4DC2E0 / byte_4DCCC0). */
    i32         var_on[OBJ_VARIANTS];
    i32         var_pos[OBJ_VARIANTS][3];
    i32         var_rot[OBJ_VARIANTS][3];
    /* Collision polygon (+0xB0, sol_limit layout) and its unrotated
     * footprint relative to the pivot. */
    u8          poly[0x208];
    i32         has_poly;
    i32         foot[4][2];
    i32         yaw_sign;
} game_object_t;

/* Drop every object (level change). */
void object_reset(void);

/* EtatObjet: a6 = state (-1/0 place, -6 remove, ...), a7 = mode. */
void object_etat(const c8 *name, i32 x, i32 y, i32 z,
                 i32 r0, i32 r1, i32 r2, i32 a6, i32 a7);

/* AnimObjet: returns 1 once the animation it started has finished (the
 * script action then completes), 0 while it is running or waiting. */
i32  object_anim(const c8 *name, i32 axis, i32 degrees, i32 ease, i32 ms, u32 now_ms);

/* sub_412B94: advance the running AnimObjet. */
void object_tick(u32 now_ms);

/* ColObjet (case 7): box [x +- w/2] x [z +- d/2]; all zero clears it. */
void object_colobjet(const c8 *name, i32 x, i32 z, i32 w, i32 d);

/* Footprint centre (+0xD8 / +0xDC) of a loaded object, for JoindreObj. */
i32 object_pos_xz(const c8 *name, i32 *x, i32 *z);

/* Slots for the renderer and the collision code. */
game_object_t *object_slot(i32 i);
const void    *object_poly(i32 i);    /* NULL when the slot has none */
i32            object_animating(const game_object_t *o);

#endif /* DKE_OBJECT_H */
