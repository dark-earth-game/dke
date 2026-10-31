#ifndef DKE_EFFECT_H
#define DKE_EFFECT_H

/*
 * effect.h -- the 16 effect slots (ambient creatures, later shell casings).
 *
 * The original keeps them in one array at 0x543C60, 16 x 0xF0 bytes, shaped
 * like the head of an actor record: update_game_hud treats a
 * slot as an entity (+0x70 "used", +0x74 effect type, +0x50 object tree,
 * +0xAC/B0/B4 world position). The decompiler names the array from its
 * +0x70 field, game_structures @ 0x543CD0, which is why the rest of the
 * port calls it that. The record is addressed here by the original offsets
 * relative to 0x543C60.
 */

#include "libsys/common.h"
#include "obc.h"
#include "model_ome.h"

#define EFFECT_SLOTS  16
#define EFFECT_STRIDE 0xF0

/* Everything a renderer needs to draw one live creature this frame. */
typedef struct {
    obc_node_t       *root;      /* object tree, already posed */
    const ome_matr_t *matr;      /* material library + texture atlas */
    i32               ground_y;  /* +0x8C: floor height at spawn */
} effect_draw_t;

/* Level / room load: memset(dword_543C60, 0, 0xF00). */
void effect_reset(void);

/* Number of used slots (case 33 @ 0x44CA4B counts them). */
i32  effect_used_count(void);

/* process_creature_effects @ 0x40D4C1: take a free slot for creature `type`
 * (1 RAT, 2 SERPENT, 3 POISSON, 4 PIAF, 5 CHSOURIS, 6 TITI, 7 LAME) at script
 * units (x, y, z); a = heading (BAM), b = speed. Returns the slot, or -1. */
i32  effect_place_creature(i32 type, i32 x, i32 y, i32 z, i32 a, i32 b);

/* sub_40636D @ 0x40636D: refresh every used slot's clock (+0x9C). */
void effect_sync_time(i32 t);

/* sub_446E19 op 31 @ 0x448656: world x / z (+0xAC / +0xB4) of a used slot. */
i32  effect_slot_xz(i32 slot, i32 *x, i32 *z);

/* The slot's per-frame body in update_game_hud (type 9 @ 0x467C83): move,
 * animate, pose the tree. Returns 1 when the creature should be drawn. */
i32  effect_creature_step(i32 slot, effect_draw_t *out);

/* End of the frame (update_overlay_effects @ 0x46F63E): dying slots free. */
void effect_frame_end(void);

#endif /* DKE_EFFECT_H */
