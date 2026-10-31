/*
 * effect.c -- the 16 effect slots and the ambient creatures that live in them
 * (the level scripts' PlaceAnimal).
 *
 * Mirrors (dkev.exe addresses):
 *   register_player_creature   @0x40D265  type -> PERSOS> model (setup_creature)
 *   process_creature_effects   @0x40D4C1  take a free slot and fill it
 *   process_entity_interaction @0x40D98B  walkability of a world point
 *   calculate_entity_position  (just before process_entity_direction) position `now - lastupdate` ahead
 *   process_entity_direction   @0x40DBE1  per-type wander / flee AI
 *   process_entity_transformations (just after process_entity_direction) per-type body animation
 *   update_game_hud type 9     @0x467C83  the slot's per-frame body
 *   update_overlay_effects     @0x46F63E  slots in state 2 are freed
 *   sub_40636D                 @0x40636D  clock refresh (effect_sync_time)
 *   sub_446E19 op 31           @0x448656  distance player -> slot
 *
 * The array sits at 0x543C60, 16 x 0xF0. A slot is shaped like the head of an
 * actor record, and every field below is named by its offset from the slot
 * start (IDA labels the +0x70 word game_structures @ 0x543CD0):
 *
 *   +0x00/+0x04 = 1000, +0x08/+0x0C = -1   energy box (update_overlay_effects)
 *   +0x10/+0x14 = 1000, +0x18/+0x1C = -1
 *   +0x30       fish tilt (type 3), written by process_entity_transformations
 *   +0x48       blade_flag: 0x100 for flyers, copied to the tree root +0x6C
 *   +0x50       object tree root (shell_ammo_counters)  -- kept in s_tree[]
 *   +0x6C       game_effects: spawn time; update_game_hud rewrites it to 2
 *   +0x70       used: 0 free, 1 live, 2 dying (game_structures)
 *   +0x74       effect type, 9 = creature (shell_effect_types)
 *   +0x8C       floor height at the spawn point (shell_effect_ids)
 *   +0x9C       last update time (game_structures_time_values)
 *   +0xA8       tree instance (shell_effect_instances) -- kept in s_tree[]
 *   +0xAC/B0/B4 world position (shell_position_x/y/z)
 *   +0xC4/C8/CC tree root rotation; +0xCC is the heading (velocity_x/y/rotation)
 *   +0xD0       speed (shell_spin_x)
 *   +0xD4       turn applied this frame (shell_spin_y)
 *   +0xD8       1 = spawned on a blocked cell (shell_spin_z)
 *   +0xDC/+0xE0 turning toward heading +0xE0 while +0xDC == 1
 *   +0xE4       creature type 1..7 (shell_lifetimes)
 *
 * Left out: type 7 (LAME, the flying blades) follows a path table at
 * 0x544B60 that nothing in the port fills, so it is not spawned; the dynamic
 * blockers (entity_data_array) in process_entity_interaction; the shell
 * casing effect (type 100); the screen-space texture pass.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "libsys/graphics/shading.h"
#include "effect.h"
#include "actor.h"
#include "entity.h"
#include "game_init.h"
#include "model.h"
#include "model_ome.h"
#include "nav_grid.h"
#include "state.h"

typedef struct { i32 d[EFFECT_STRIDE / 4]; } effect_slot_t;
_Static_assert(sizeof(effect_slot_t) == EFFECT_STRIDE, "effect slot stride");

#define F(s, off) ((s)->d[(off) >> 2])

#define CREATURE_COUNT 8
#define ZONE_UNIT      600          /* script units -> world units */
#define NEAR_PLAYER    1.0404e10    /* dbl_4B01A8: (170 * 600)^2 */

/* Slots at 0x543C60. */
static effect_slot_t g_slot[EFFECT_SLOTS];
/* Host-pointer fields of a slot (+0x50 / +0xA8 hold the tree). */
static obc_node_t *s_tree[EFFECT_SLOTS];

/* register_player_creature: jump table, index = type - 1. */
static const c8 *const k_names[CREATURE_COUNT] = {
    NULL, "RAT", "SERPENT", "POISSON", "PIAF", "CHSOURIS", "TITI", "LAME"
};

typedef struct {
    i32         tried;
    obc_node_t *root;
    ome_matr_t  matr;
    i32         matr_ok;
} creature_model_t;
static creature_model_t g_model[CREATURE_COUNT];

/* g_blade_flag: next index handed to a LAME, reset when LAME registers. */
static i32 g_blade_flag = 0;

static i32 now_ms(void) { return state ? (i32)state->current_game_time : 0; }

/* ls_random(n) & 0xFFFF: same formula as lieu_cb.c lo_random(). */
static i32 rnd(i32 n) {
    return (i32)(((u32)n * ((u32)rand() & 0x7FFFu)) / 32767u) & 0xFFFF;
}

/* sine_table[(a + off) >> 4 & 0xFFF] -- the asm's table is the port's
 * cosine_table, indexed identically. */
static i32 sine_at(i32 a) { return cosine_table[((u32)a >> 4) & 0xFFF]; }

/* ------------------------------------------------------------------------
 * register_player_creature @ 0x40D265
 * ---------------------------------------------------------------------- */
static creature_model_t *creature_register(i32 type) {
    if (type < 1 || type >= CREATURE_COUNT) return NULL;
    creature_model_t *m = &g_model[type];
    if (m->tried) return m->root ? m : NULL;
    m->tried = 1;
    if (type == 7) g_blade_flag = 0;

    void *mat = NULL, *obc = NULL;
    setup_creature("PERSOS>", k_names[type], &mat, &obc, 0);
    m->root = (obc_node_t *)obc;
    if (!m->root) {
        print("effect: creature %d (%s) has no object tree\n", type, k_names[type]);
        return NULL;
    }

    /* Material library + texture atlas for the port's renderer. */
    c8 vpath[48], host[512];
    snprintf(vpath, sizeof(vpath), "PERSOS>%s.OME", k_names[type]);
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    ome_file_t ome = {0};
    if (ls_resolve_path(base, vpath, host, (i32)sizeof(host)) && model_ome_open(host, &ome)) {
        m->matr_ok = ome_load_matr(ome.raw, ome.raw_size, &m->matr);
        model_ome_free(&ome);
    }
    print("effect: registered creature %d (%s)\n", type, k_names[type]);
    return m;
}

/* ------------------------------------------------------------------------
 * process_entity_interaction @ 0x40D98B -- may a creature stand on (x, z)?
 * The 64x64 interaction grid holds 0 free, 2/3 border / wall, 0xFE/0xFF
 * blocked; ground creatures are stopped by anything non-zero, flyers (arg 1,
 * threshold 0x80) only by the hard cells. Cells are found with idiv, so a
 * point just outside the grid truncates into row / column 0. The original
 * then tests 8 dynamic blockers (entity_data_array); not ported.
 * ---------------------------------------------------------------------- */
static i32 interaction(i32 x, i32 z, i32 flying) {
    if (g_nav.cell_width <= 0 || g_nav.cell_height <= 0) return 1;
    i32 cx = (x - g_nav.world_x_origin) / g_nav.cell_width;
    i32 cz = (z - g_nav.world_y_origin) / g_nav.cell_height;
    i32 thr = flying ? 0x80 : 0;
    if (cx >= 0 && cz >= 0 && cx <= 63 && cz <= 63 &&
        g_nav.grid[cz * NAV_GRID_SIZE + cx] > thr)
        return 0;
    return 1;
}

/* ------------------------------------------------------------------------
 * calculate_entity_position: the point `now - last update` ahead along the
 * heading +0xCC at speed +0xD0.
 * ---------------------------------------------------------------------- */
static void calc_pos(const effect_slot_t *s, i32 now, i32 *ox, i32 *oz) {
    i32 vx = sine_at(F(s, 0xCC) + 0x4008) * F(s, 0xD0) / 256;
    i32 vz = sine_at(F(s, 0xCC) + 8) * F(s, 0xD0) / 256;
    i32 dt = now - F(s, 0x9C);
    *ox = F(s, 0xAC) + (i32)((u32)dt * (u32)vx) / 1024;
    *oz = F(s, 0xB4) + (i32)((u32)dt * (u32)vz) / 1024;
}

/* ------------------------------------------------------------------------
 * process_creature_effects @ 0x40D4C1
 * ---------------------------------------------------------------------- */
i32 effect_place_creature(i32 type, i32 x, i32 y, i32 z, i32 a, i32 b) {
    creature_model_t *m = creature_register(type);
    if (!m) return -1;
    if (type == 7) {
        print("effect: LAME path table (0x544B60) not ported, not spawned\n");
        return -1;
    }
    for (i32 i = 0; i < EFFECT_SLOTS; i++) {
        effect_slot_t *s = &g_slot[i];
        if (F(s, 0x70)) continue;
        i32 now = now_ms();
        F(s, 0x6C) = now;
        i32 floor = actor_floor_height(x * ZONE_UNIT, z * ZONE_UNIT, NULL);
        F(s, 0x8C) = floor;
        F(s, 0x74) = 9;
        F(s, 0x9C) = now;
        F(s, 0xE4) = type;
        F(s, 0x70) = 1;
        s_tree[i] = m->root;
        F(s, 0xAC) = x * ZONE_UNIT;
        F(s, 0xB4) = z * ZONE_UNIT;
        if (type >= 3 && type <= 7) {
            F(s, 0x48) = 0x100;
            F(s, 0xB0) = y * ZONE_UNIT;
        } else {
            F(s, 0xB0) = floor;
        }
        if (type == 7) F(s, 0x48) = g_blade_flag++;
        F(s, 0xC4) = (i32)0xFFFF4000;
        F(s, 0xC8) = (i32)0xFFFFC000;
        F(s, 0xCC) = a;
        F(s, 0xD0) = b;
        F(s, 0xD4) = 0;
        F(s, 0xD8) = 0;
        F(s, 0x10) = F(s, 0x14) = 1000;
        F(s, 0x00) = F(s, 0x04) = 1000;
        F(s, 0x18) = F(s, 0x1C) = -1;
        F(s, 0x08) = F(s, 0x0C) = -1;
        /* The tree takes the position now; initialize_effect_position's
         * light sample is redone by the renderer every frame. */
        m->root->pos[0] = F(s, 0xAC);
        m->root->pos[1] = F(s, 0xB0);
        m->root->pos[2] = F(s, 0xB4);
        if (!interaction(x * ZONE_UNIT, z * ZONE_UNIT, 0)) F(s, 0xD8) = 1;
        print("effect: slot %d creature %d at (%d,%d,%d) heading %d speed %d%s\n",
              i, type, F(s, 0xAC), F(s, 0xB0), F(s, 0xB4), a, b,
              F(s, 0xD8) ? " (blocked cell)" : "");
        return i;
    }
    return -1;
}

void effect_reset(void) {
    memset(g_slot, 0, sizeof(g_slot));
    memset(s_tree, 0, sizeof(s_tree));
}

i32 effect_used_count(void) {
    i32 n = 0;
    for (i32 i = 0; i < EFFECT_SLOTS; i++)
        if (F(&g_slot[i], 0x70)) n++;
    return n;
}

void effect_sync_time(i32 t) {
    for (i32 i = 0; i < EFFECT_SLOTS; i++)
        if (F(&g_slot[i], 0x70)) F(&g_slot[i], 0x9C) = t;
}

i32 effect_slot_xz(i32 slot, i32 *x, i32 *z) {
    if (slot < 0 || slot >= EFFECT_SLOTS || !F(&g_slot[slot], 0x70)) return 0;
    if (x) *x = F(&g_slot[slot], 0xAC);
    if (z) *z = F(&g_slot[slot], 0xB4);
    return 1;
}

void effect_frame_end(void) {
    for (i32 i = 0; i < EFFECT_SLOTS; i++)
        if (F(&g_slot[i], 0x70) == 2) F(&g_slot[i], 0x70) = 0;
}

/* ------------------------------------------------------------------------
 * process_entity_direction @ 0x40DBE1 -- creature branches
 * ---------------------------------------------------------------------- */

/* Squared distance to the player and the vector toward them, from (x, z). */
static f64 player_vec(i32 x, i32 z, f64 *dx, f64 *dz) {
    *dx = *dz = 0;
    if (!g_player_entity || !EROOT(g_player_entity)) return 1e30;
    *dx = (f64)(EROOT(g_player_entity)->pos[0] - x);
    *dz = (f64)(EROOT(g_player_entity)->pos[2] - z);
    return *dx * *dx + *dz * *dz;
}

/* Turn toward +0xE0 by `turn`, then step forward (the DC == 1 branch). */
static void turn_to_target(effect_slot_t *s, i32 now, i32 shift, i32 flying) {
    i32 step = (now - F(s, 0x9C)) << shift;
    i32 d = F(s, 0xCC) - F(s, 0xE0);
    if (abs(d) < step) {
        F(s, 0xCC) = F(s, 0xE0);
        F(s, 0xDC) = 0;
    } else if (F(s, 0xE0) < F(s, 0xCC)) {
        F(s, 0xD4) = -step;
        F(s, 0xCC) -= step;
    } else {
        F(s, 0xD4) = step;
        F(s, 0xCC) += step;
    }
    i32 x, z;
    calc_pos(s, now, &x, &z);
    if (interaction(x, z, flying)) { F(s, 0xAC) = x; F(s, 0xB4) = z; }
}

/* Blocked ahead: draw up to 50 new random headings until one is free, then
 * turn toward it from the old one. */
static void pick_new_heading(effect_slot_t *s, i32 now, i32 flying, i32 rng, i32 base) {
    i32 saved = F(s, 0xCC);
    for (i32 tries = 0; tries < 50; tries++) {
        F(s, 0xD0) = rnd(rng) + base;
        F(s, 0xCC) = rnd(0x400) << 6;
        i32 x, z;
        calc_pos(s, now, &x, &z);
        if (interaction(x, z, flying)) {
            F(s, 0xDC) = 1;
            F(s, 0xE0) = F(s, 0xCC);
            break;
        }
    }
    F(s, 0xCC) = saved;
}

/* Heading change toward / away from the player, speed 0x800 (types 1, 2). */
static void steer_to_player(effect_slot_t *s, i32 now, f64 dx, f64 dz) {
    f64 cross = (f64)sine_at(F(s, 0xCC) + 0x4008) * dz - (f64)sine_at(F(s, 0xCC) + 8) * dx;
    F(s, 0xD0) = 0x800;
    i32 step = (now - F(s, 0x9C)) << 6;
    if (cross < 0.0) { F(s, 0xCC) += step; F(s, 0xD4) = step; }
    else             { F(s, 0xCC) -= step; F(s, 0xD4) = -step; }
}

static void creature_direction(effect_slot_t *s, i32 now) {
    i32 type = F(s, 0xE4);
    i32 x, z;
    f64 dx, dz, d2;
    F(s, 0xD4) = 0;

    if (type == 1 || type == 2) {
        if (F(s, 0xD8) == 1) {
            /* Spawned on a blocked cell: drift along the heading until free.
             * The SERPENT stops there for the frame, the RAT carries on. */
            calc_pos(s, now, &x, &z);
            F(s, 0xAC) = x; F(s, 0xB4) = z;
            d2 = player_vec(x, z, &dx, &dz);
            if (d2 < NEAR_PLAYER) F(s, 0xD0) = 0x800;
            if (interaction(x, z, 0)) F(s, 0xD8) = 0;
            if (type == 2) goto done;
        }
        if (type == 2) {
            /* SERPENT: slithers toward the player and dies on a wall. */
            d2 = player_vec(F(s, 0xAC), F(s, 0xB4), &dx, &dz);
            if (d2 < NEAR_PLAYER) steer_to_player(s, now, dx, dz);
            calc_pos(s, now, &x, &z);
            if (interaction(x, z, 0)) { F(s, 0xAC) = x; F(s, 0xB4) = z; }
            else                      F(s, 0x70) = 2;
            goto done;
        }
        if (F(s, 0xDC) == 1) {
            turn_to_target(s, now, 6, 0);
            goto done;
        }
        /* RAT. The original reads dx / dz / d2 here from locals that only
         * the D8 branch above sets (stale stack); they are recomputed from
         * the current position instead, as the SERPENT block does. */
        d2 = player_vec(F(s, 0xAC), F(s, 0xB4), &dx, &dz);
        if (d2 < NEAR_PLAYER) steer_to_player(s, now, dx, dz);
        calc_pos(s, now, &x, &z);
        if (interaction(x, z, 0)) { F(s, 0xAC) = x; F(s, 0xB4) = z; goto done; }
        pick_new_heading(s, now, 0, 0x200, 0x300);
    } else if (type >= 3 && type <= 6) {
        /* Flyers / swimmers: wander, turn away from hard cells. */
        static const i32 k_turn[7]  = { 0, 0, 0, 6, 6, 8, 8 };   /* shl by type */
        static const i32 k_rng[7]   = { 0, 0, 0, 0x100, 0x400, 0x200, 0x200 };
        static const i32 k_base[7]  = { 0, 0, 0, 0x300, 0x400, 0x800, 0x800 };
        if (F(s, 0xD8) == 1) {
            calc_pos(s, now, &x, &z);
            F(s, 0xAC) = x; F(s, 0xB4) = z;
            if (interaction(x, z, 1)) F(s, 0xD8) = 0;
        }
        if (F(s, 0xDC) == 1) {
            turn_to_target(s, now, k_turn[type], 1);
            goto done;
        }
        calc_pos(s, now, &x, &z);
        if (interaction(x, z, 1)) { F(s, 0xAC) = x; F(s, 0xB4) = z; goto done; }
        pick_new_heading(s, now, 1, k_rng[type], k_base[type]);
    }
done:
    F(s, 0x9C) = now;
}

/* ------------------------------------------------------------------------
 * process_entity_transformations -- body animation
 * ---------------------------------------------------------------------- */

/* Face normal from the first three vertices, Q14, sign flipped . */
static void face_normal(atm_atom_t *a, i32 fi) {
    i32 *f = a->faces + (size_t)fi * ATM_FACE_STRIDE;
    i32 i0 = f[ATM_FACE_IDX], i1 = f[ATM_FACE_IDX + 1], i2 = f[ATM_FACE_IDX + 2];
    if (i0 < 0 || i1 < 0 || i2 < 0 || i0 >= a->vert_count || i1 >= a->vert_count ||
        i2 >= a->vert_count)
        return;
    const i32 *v0 = a->verts + (size_t)i0 * ATM_VERT_STRIDE;
    const i32 *v1 = a->verts + (size_t)i1 * ATM_VERT_STRIDE;
    const i32 *v2 = a->verts + (size_t)i2 * ATM_VERT_STRIDE;
    f64 ax = (f64)(v0[0] - v1[0]), ay = (f64)(v0[1] - v1[1]), az = (f64)(v0[2] - v1[2]);
    f64 bx = (f64)(v2[0] - v1[0]), by = (f64)(v2[1] - v1[1]), bz = (f64)(v2[2] - v1[2]);
    f64 nx = ay * bz - by * az;
    f64 ny = bx * az - ax * bz;
    f64 nz = by * ax - bx * ay;
    f64 len = sqrt(nx * nx + ny * ny + nz * nz);
    if (len <= 0.0) return;
    f64 k = 16384.0 / len;
    f[ATM_FACE_NX] = -(i32)(k * nx);
    f[ATM_FACE_NY] = -(i32)(k * ny);
    f[ATM_FACE_NZ] = -(i32)(k * nz);
}

/* The renderer caches a vertex copy per atom; push edited vertices into it. */
static void atom_resync(atm_atom_t *a) {
    model_t *m = (model_t *)a->render;
    if (!m || m->vertex_count != a->vert_count) return;
    model_vec3_t *v = (model_vec3_t *)m->vertices;
    for (i32 i = 0; i < a->vert_count; i++) {
        const i32 *s = a->verts + (size_t)i * ATM_VERT_STRIDE;
        v[i].x = s[ATM_VERT_X]; v[i].y = s[ATM_VERT_Y]; v[i].z = s[ATM_VERT_Z];
    }
}

/* Triangle wave of the clock, then its sine: the flap (types 3..6). */
static i32 flap(i32 now, i32 shift) {
    i32 t = (i32)(((u32)now << shift) & 0xFFFF);
    t = t < 0x8000 ? t - 0x4000 : 0xC000 - t;
    return sine_at(t - 0x3FF8);
}

static void creature_transform(effect_slot_t *s, obc_node_t *root, i32 now) {
    i32 type = F(s, 0xE4);
    atm_atom_t *atom = root->atom[0];

    if (type == 1 && atom && atom->vert_count > 15) {
        /* RAT: the head vertex swings with the turn; one face is re-lit. */
        i32 z = F(s, 0xD4) > 0 ? -6000 : F(s, 0xD4) < 0 ? 6000 : 0;
        atom->verts[15 * ATM_VERT_STRIDE + ATM_VERT_Z] = z;
        if (atom->face_count > 12) face_normal(atom, 12);
        atom_resync(atom);
    }
    if (type == 2 && atom) {
        /* SERPENT: a travelling sine along the body. */
        i32 phase = (i32)((u32)(now - F(s, 0x6C)) * (u32)F(s, 0xD0)) / 16;
        for (i32 i = 0; i < atom->vert_count; i++) {
            i32 *v = atom->verts + (size_t)i * ATM_VERT_STRIDE;
            v[ATM_VERT_Z] = sine_at(v[ATM_VERT_Y] + phase - 0x3FF8);
        }
        for (i32 i = 0; i < atom->face_count; i++) face_normal(atom, i);
        atom_resync(atom);
    }
    if (type >= 3 && type <= 6) {
        static const i32 k_shift[7] = { 0, 0, 0, 7, 6, 9, 7 };
        root->raw_60[3] = F(s, 0x48);                       /* root +0x6C */
        i32 v = flap(now, k_shift[type]);
        obc_node_t *c = root->child;
        if (type == 3) {
            if (c) c->rot[0] = v;                            /* +0x54 */
            F(s, 0x30) = -(v / 16);
        } else if (c) {
            c->rot[1] = v / 2;                               /* +0x58 */
            if (c->next) c->next->rot[1] = -(v / 2);
        }
    }
}

/* ------------------------------------------------------------------------
 * update_game_hud, effect type 9 @ 0x467C83
 * ---------------------------------------------------------------------- */
i32 effect_creature_step(i32 slot, effect_draw_t *out) {
    if (slot < 0 || slot >= EFFECT_SLOTS) return 0;
    effect_slot_t *s = &g_slot[slot];
    if (F(s, 0x70) != 1 || F(s, 0x74) != 9 || !s_tree[slot]) return 0;
    creature_model_t *m = &g_model[F(s, 0xE4)];
    obc_node_t *root = s_tree[slot];
    i32 now = now_ms();

    F(s, 0x6C) = 2;                         /* update_game_hud, before dispatch */
    creature_direction(s, now);
    if (F(s, 0x70) == 2) return 0;

    root->pos[0] = F(s, 0xAC); root->pos[1] = F(s, 0xB0); root->pos[2] = F(s, 0xB4);
    root->rot[0] = F(s, 0xC4); root->rot[1] = F(s, 0xC8); root->rot[2] = F(s, 0xCC);
    creature_transform(s, root, now);
    if (F(s, 0xE4) == 1) root->pos[1] += rnd(0x258) - 0x12C;   /* RAT shiver */
    if (F(s, 0xE4) == 3) root->rot[2] += F(s, 0x30);           /* fish tilt  */

    if (out) {
        out->root = root;
        out->matr = m->matr_ok ? &m->matr : NULL;
        out->ground_y = F(s, 0x8C);
    }
    return 1;
}
