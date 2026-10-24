#ifndef DKE_ACTOR_H
#define DKE_ACTOR_H

/*
 * actor.h — character ("perso") runtime: creation, placement, animation.
 *
 * The original runs exactly three actors at a time: player_entity and the two
 * companion slots (companion_entity @ 0x4EDAF0, secondary_companion_entity
 * @ 0x4EDAF4). Persona ids 0..3 are the playable heroes and share the player
 * slot; ids >= 4 are everyone else and take a companion slot when a script
 * places them (POSPERSO, sub_4640BC).
 *
 * Every actor is an entity_t (entity.h) whose +0xAC points at a persona
 * descriptor: one 0x318-byte record per hero (dword_4ECD50) or one 0x220-byte
 * record per other persona (dword_4DF5D0). The descriptor holds the persona's
 * state -> animation name table, which the animation calls below look up.
 */

#include "libsys/common.h"
#include "entity.h"
#include "can.h"

#define PERSONA_COUNT      0x62    /* 98 descriptor slots   */
#define PERSONA_DESC_SIZE  0x220
#define HERO_COUNT         4
#define HERO_DESC_SIZE     0x318

/* dword_4DF5D0 — per-persona descriptor. The original keeps the entity
 * pointer in the record's first dword; it lives in `entity` here and raw[0..3]
 * is unused. EPTR(e, 0xAC) points at raw, so desc[0x97] etc. read as in the
 * asm. */
typedef struct {
    entity_t *entity;
    u8        raw[PERSONA_DESC_SIZE];
} persona_desc_t;

extern persona_desc_t g_persona_desc[PERSONA_COUNT];
extern u8             g_hero_desc[HERO_COUNT][HERO_DESC_SIZE];   /* dword_4ECD50 */

extern entity_t *g_player_entity;               /* player_entity               */
extern entity_t *g_companion_entity;            /* companion_entity @ 4EDAF0   */
extern entity_t *g_secondary_companion_entity;  /* secondary_companion @ 4EDAF4 */

/* animation_timing_divisor @ 0x511770: animation ticks per millisecond. */
extern f64 g_anim_divisor;

/* game_init phases 15/16 (0x46C10C..0x46C219): reset both descriptor tables. */
void actor_tables_reset(void);

/* sub_46493B @ 0x46493B — allocate and load the actor for persona `id`
 * (hero ids 0..3 also accept the 0x62.. "id + 0x60" form). */
entity_t *actor_create(i32 id);

/* sub_4663E3 + sub_4658D4 — release an actor and its object tree. */
void actor_free(entity_t *e);

/* sub_44411D @ 0x44411D — actor for persona id (heroes map to the player). */
entity_t *actor_find(i32 id);

/* sub_4640BC @ 0x4640BC — POSPERSO: create/fetch the persona's actor and put
 * it at (x, z) facing `facing`, starting animation `anim`. `vie` is the
 * script's life mode (0..4); `zone`/`radius` attach it to a zone record. */
entity_t *actor_pos_perso(i32 id, i32 x, i32 z, i32 facing, i32 vie,
                          const c8 *anim, i32 zone, i32 radius);

/* sub_463C2E @ 0x463C2E — place an actor and stand it on the floor. */
void actor_place(entity_t *e, i32 x, i32 z, i32 facing, const c8 *anim);

/* --- animation ------------------------------------------------------- */

/* sub_41918C @ 0x41918C — state id whose descriptor slot names `anim`
 * (negative for the -1..-16 table, 999 for "ar", 1000 for g..arb). */
i32 actor_anim_state(entity_t *e, const c8 *anim);

/* prepare_entity_animation @ 0x419307 — copy the 8-char clip name for
 * `state` into out[8]. Returns 0 when the persona has no clip ('x'). */
i32 actor_prepare_anim(entity_t *e, i32 state, c8 *out);

/* process_entity_state_change @ 0x41A9ED — fold a clip's root yaw into the
 * actor heading before switching clips. */
void actor_bake_root_yaw(entity_t *e);

/* play_entity_animation @ 0x465BDF — start `rec`. `keep_phase` continues at
 * the matching phase of a looping walk (sub_465B47). Returns 1 if started. */
i32 actor_play_anim(entity_t *e, can_rec_t *rec, i32 keep_phase);

/* Convenience: actor_prepare_anim + can_get_anim + actor_play_anim. */
i32 actor_play_state(entity_t *e, i32 state);
i32 actor_play_named(entity_t *e, const c8 *anim);

/* is_entity_in_animation @ 0x418D7F — 1 when the current clip has run out. */
i32 actor_anim_done(entity_t *e);

/* Animation ticks elapsed since the clip started (entity+0xC28). */
i32 actor_anim_time(const entity_t *e);

/* sub_414B8C @ 0x414B8C — interpolate `rec` at tick `t` into pose[103]
 * (i16 header + 34 rotation triples). Leaves pose untouched past the end. */
void anim_sample(const can_rec_t *rec, i16 *pose, i32 t);

/* sub_414DDE @ 0x414DDE — write a pose straight into the bone nodes. */
void actor_set_pose(entity_t *e, const i16 *pose);

/* sub_415021 @ 0x415021 — move the bone nodes toward `pose`, blending over
 * the clip's blend time; `t` is the tick since the clip started. */
void actor_blend_pose(entity_t *e, const i16 *pose, i32 t);

/* --- per-frame update (actor_move.c) ------------------------------- */

/* sub_41CCB7 @ 0x41CCB7 — clip clock, messages, floor, pose, root motion. */
void actor_update(entity_t *e);

/* sub_4193CC @ 0x4193CC — apply the pending move and drive the locomotion
 * clips from +0xCEC (mode) / +0xCF4 (turn) / +0xD04 (scripted state). */
void actor_control(entity_t *e);

/* sub_469222 @ 0x469222 — scripted walk state machine (+0xCBC). */
void actor_walk_update(entity_t *e);

/* sub_428714 @ 0x428714 — steer toward (x, z); 1 = arrived. */
i32 actor_steer(entity_t *e, i32 x, i32 z);

/* sub_461468 @ 0x46162B..0x461851, the actor part of the game tick: update
 * the three actors in the original order, run the player's walk, drain the
 * script action piles and run the player's controller. */
void actor_frame(u32 now_ms);

/* sub_43A63D: reset actor globals for a new game. */
void actor_new_game(void);

extern i32 g_cinematic_mode;          /* cinematic_mode        */
extern i32 g_special_mode_active;     /* special_mode_active   */
extern i32 g_player_movement_locked;  /* player_movement_enabled */
extern i32 g_level_slow;              /* dword_4F5E20 ("slow") */
extern i32 g_skip_next_animation;     /* dword_506BE0          */

/* sub_461468 @ 0x4617B7: recompute the divisor from the frame rate and
 * rescale every actor's clocks (update_entity_animation_timings). */
void actor_update_timing(u32 now_ms);

/* --- hierarchy ------------------------------------------------------- */

/* sub_463A24 @ 0x463A24 — world position of every node under `root`, written
 * to node->raw_88[2..4] (+0x90/+0x94/+0x98), starting from matrix `start`. */
void actor_node_positions(obc_node_t *root, const fmat_t *start);

/* sub_457345 @ 0x457345 ("generate_effect") — floor height at (x, z). */
i32 actor_floor_height(i32 x, i32 z, const entity_t *e);
/* Same, also returning the floor zone hit (NULL = world controller). */
i32 actor_floor_zone(i32 x, i32 z, const entity_t *e, void **zone);

#endif /* DKE_ACTOR_H */
