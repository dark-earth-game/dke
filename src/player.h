#pragma once

/*
 * player.h — player entity init + input routing
 *
 * Covers Slice D pass 1:
 *   sub_46493B  (0046493B) — player entity allocation + default field init
 *   sub_464F6D  (00464F6D) — player entity secondary init (weapon / companion wire)
 *   player_input_update     — keyboard/mouse → velocity skeleton
 */

#include "libsys/common.h"
#include "libsys/system.h"
#include "actor.h"

/* -------------------------------------------------------------------------
 * Player entity global — allocated by player_entity_init.
 * Exposed here so scene.c and game_init.c can read the pointer.
 * ---------------------------------------------------------------------- */
/* g_player_entity / g_companion_entity / g_secondary_companion_entity are
 * declared in actor.h. */
void entity_register_bones(void *entity, obc_node_t *scene_root);

/* Live player transform accessors (read the embedded transform block).
 * player_get_pos returns 1 on success, 0 if no player entity. Position is
 * i32 world units; facing is 16-bit BAM (0..0xFFFF = full circle). */
i32 player_get_pos(i32 *x, i32 *y, i32 *z);
u16 player_get_facing(void);

/* -------------------------------------------------------------------------
 * player_input_t — per-frame input snapshot polled from system_t.actions[].
 * ---------------------------------------------------------------------- */
typedef struct {
    f32 velocity_x;   /* -1 left / +1 right (A/D) */
    f32 velocity_y;   /* reserved (vertical strafing — not yet mapped) */
    f32 velocity_z;   /* -1 forward / +1 backward (W/S) */
    i32 want_action;  /* 1 = attack edge (mouse_left edge); 0 = idle */
} player_input_t;

/* -------------------------------------------------------------------------
 * sub_46493B — allocate + zero-init + default-field-set player entity struct.
 * Called from game_init phase 33.
 * ---------------------------------------------------------------------- */
void player_entity_init(void);

/* -------------------------------------------------------------------------
 * sub_464F6D — secondary init: weapon/companion buffer wire-up.
 * Called from game_init phase 34.
 * ---------------------------------------------------------------------- */
void player_entity_init_secondary(void);

/* -------------------------------------------------------------------------
 * player_input_update — poll actions[] + mouse state → fill *out.
 * Resets all fields to zero each call; sets only pressed directions.
 * Logs velocity when non-zero.
 * ---------------------------------------------------------------------- */
void player_input_update(player_input_t *out, system_t *sys);
