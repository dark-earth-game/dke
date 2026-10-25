/*
 * actor.c — character runtime: creation, placement, animation playback.
 *
 * Ported from dkev.exe:
 *   sub_46493B @ 0x46493B  actor allocation + persona load
 *   sub_462C9E @ 0x462C9E  persona descriptor from the .OME header (HDD)
 *   sub_4640BC @ 0x4640BC  POSPERSO
 *   sub_463FB5 @ 0x463FB5  placement + zone attachment
 *   sub_463C2E @ 0x463C2E  placement, first animation, floor standing
 *   sub_44411D @ 0x44411D  actor lookup by persona id
 *   sub_41918C, prepare_entity_animation, play_entity_animation,
 *   is_entity_in_animation, process_entity_state_change
 *   sub_414B8C / sub_414C8A / sub_414E66  keyframe sampling
 *   sub_414DDE / sub_415021               pose -> bone nodes
 *   sub_463A24 @ 0x463A24  node world positions
 *   sub_457345 @ 0x457345  floor height ("generate_effect")
 *   update_entity_animation_timings @ 0x405A6C
 *
 * See actor.h for the model; entity.h for the record layout.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/graphics/shading.h"
#include "libsys/file_system.h"
#include "material.h"
#include "state.h"
#include "actor.h"
#include "game_init.h"
#include "world.h"
#include "lieu_cb.h"

persona_desc_t g_persona_desc[PERSONA_COUNT];
u8             g_hero_desc[HERO_COUNT][HERO_DESC_SIZE];

entity_t *g_player_entity              = NULL;
entity_t *g_companion_entity           = NULL;
entity_t *g_secondary_companion_entity = NULL;

f64 g_anim_divisor = 0.48;          /* animation_timing_divisor, dbl_4B72A0 */

/* game_state @ 0x4B70E8 side: the hero currently controlled (sub_46493B
 * stores it when the player slot is created). */
static i32 g_hero_id = 0;

i32 actor_hero_id(void) { return g_hero_id; }

/* dword_4EC6A0 / dword_4ECD3C — list of live actors, appended by
 * sub_46493B and shortened by sub_4658D4. */
static entity_t *g_actor_list[0x62];
static i32       g_actor_count = 0;

/* byte_4F60E0 — 0x1000-byte nav workspace per actor slot (entity+0xCD8). */
static u8 g_nav_workspace[0x3000];


static u32 now_ms(void) { return state ? state->current_game_time : 0; }

/* =========================================================================
 * Tables
 * ========================================================================= */

void actor_tables_reset(void) {
    /* 0x46C10C: four hero records, stride 0x318 */
    memset(g_hero_desc, 0, sizeof(g_hero_desc));
    for (i32 i = 0; i < HERO_COUNT; i++) {
        *(i32 *)&g_hero_desc[i][0x84] = 0x3FF;
        for (i32 j = 0; j < 0x1F; j++) *(i32 *)&g_hero_desc[i][4 + j * 4] = -1;
    }
    /* 0x46C199: 98 persona records, stride 0x220 */
    memset(g_persona_desc, 0, sizeof(g_persona_desc));
    for (i32 i = 0; i < PERSONA_COUNT; i++)
        for (i32 j = 0; j < 0x1F; j++) *(i32 *)&g_persona_desc[i].raw[4 + j * 4] = -1;
    /* 0x46C23C: memset(companion_entity, 0, 8) */
    g_companion_entity = NULL;
    g_secondary_companion_entity = NULL;
    g_actor_count = 0;
}

entity_t *actor_find(i32 id) {                        /* sub_44411D */
    if (id < 0) return NULL;
    if (id < HERO_COUNT) return g_player_entity;
    if (id >= PERSONA_COUNT) return NULL;
    return g_persona_desc[id].entity;
}

/* =========================================================================
 * Persona descriptor — sub_462C9E @ 0x462C9E, HDD install path.
 *
 * The descriptor lives in the header of the persona's .OME:
 *   "CTX3", file date, header size, block size
 *   heroes:  7 blocks of 0x280 bytes -> hero record +0x98, one per weapon
 *            family; only the matching block is kept, the rest are skipped
 *   others:  one 0x189-byte block    -> persona record +0x97
 *   i32 n, n bytes                   -> entity+0xD2 (default weapon name)
 *   i32 ai flag (dword_4ECA28); when set five AI tuning dwords follow
 *   five dwords                      -> entity+0xC50, C54, C5C, C60, C58
 *   i32 energy                       -> record+0x84 (persona, if unset)
 * ========================================================================= */

static i32 read_i32(FILE *f, i32 *out) { return fread(out, 4, 1, f) == 1; }

static i32 g_ai_flag, g_ai_param[5];   /* dword_4ECA28, 4ECD40, 4EC828, 4EC82C, 4DF5B8.. */

static i32 actor_desc_load(const c8 *dir_name, i32 id, i32 weapon, entity_t *e) {
    c8 vpath[64], host[512];
    snprintf(vpath, sizeof(vpath), "PERSOS>%s.OME", dir_name);
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, vpath, host, (i32)sizeof(host))) {
        print("actor: %s not found\n", vpath);
        return 0;
    }
    FILE *f = ls_open(host, "rb");
    if (!f) return 0;

    i32 ok = 0, magic = 0, v = 0, size = 0;
    if (!read_i32(f, &magic) || magic != 0x33585443) goto done;   /* "CTX3" */
    if (!read_i32(f, &v) || !read_i32(f, &v) || !read_i32(f, &size)) goto done;

    if (size == HERO_DESC_SIZE - 0x98) {
        /* Heroes: block 0 is the bare-handed set; blocks 1..6 belong to the
         * weapon families tested by is_character_type_a/b/c, sub_47271C,
         * sub_472746, sub_472770 against combat_ability_table. Weapons are
         * not wired yet, so only block 0 can match. */
        for (i32 k = 0; k < 7; k++) {
            if (k == 0 && weapon == 0) {
                if (fread(&g_hero_desc[0][0x98], 1, (size_t)size, f) != (size_t)size) goto done;
            } else {
                fseek(f, size, SEEK_CUR);
            }
        }
        ok = 1;
    } else if (id >= 0 && id < PERSONA_COUNT && size == PERSONA_DESC_SIZE - 0x97) {
        persona_desc_t *d = &g_persona_desc[id];
        if (fread(&d->raw[0x97], 1, (size_t)size, f) != (size_t)size) goto done;
        i32 n = 0;
        if (!read_i32(f, &n) || n < 0 || n > 0x20) goto done;
        if (n && fread(ESTR(e, 0xD2), 1, (size_t)n, f) != (size_t)n) goto done;
        d->entity = e;
        ok = 1;
    }
    if (!ok) goto done;

    if (read_i32(f, &g_ai_flag) && g_ai_flag)
        for (i32 k = 0; k < 5; k++) read_i32(f, &g_ai_param[k]);
    {
        static const i32 offs[5] = { 0xC50, 0xC54, 0xC5C, 0xC60, 0xC58 };
        for (i32 k = 0; k < 5; k++) read_i32(f, &E32(e, offs[k]));
    }
    if (id >= HERO_COUNT && id < PERSONA_COUNT) {
        persona_desc_t *d = &g_persona_desc[id];
        if (*(i32 *)&d->raw[0x84] == 0 && d->raw[0x96] == 0)
            read_i32(f, (i32 *)&d->raw[0x84]);
    }
done:
    ls_close(f);
    return ok;
}

/* =========================================================================
 * Creation — sub_46493B @ 0x46493B
 * ========================================================================= */

entity_t *actor_create(i32 id) {
    i32 num = (id >= 0x62) ? id - 0x60 : id;             /* var_88 */

    entity_t *e = (entity_t *)calloc(1, sizeof(entity_t));
    if (!e) return NULL;

    c8 name[16];
    snprintf(name, sizeof(name), "p%02d", id < 0 ? -id : id);  /* 464A07 */

    /* +0xAC: heroes share dword_4ECD50, others get their own record whose
     * first dword points back at the actor (464AD1..464B2A). */
    if (num < HERO_COUNT) {
        g_hero_id = num;
        EPTR(e, 0xAC) = g_hero_desc[0];
    } else if (id < PERSONA_COUNT) {
        g_persona_desc[id].entity = e;
        EPTR(e, 0xAC) = g_persona_desc[id].raw;
    }
    actor_desc_load(name, id < 0 ? -id : id, 0, e);       /* sub_462BCC */

    /* setup_creature("PERSOS>", name) -> +0x88 material, +0x50 tree. */
    void *mat = NULL, *obc = NULL;
    setup_creature("PERSOS>", name, &mat, &obc, 0);
    EPTR(e, 0x88) = mat;
    EPTR(e, 0x50) = obc;
    E32(e, 0xD64) = 0;  EPTR(e, 0xD64) = NULL;
    E32(e, 0xD68) = 0;  EPTR(e, 0xD68) = NULL;
    E32(e, 0x90) = 2;
    E32(e, 0x94) = 2;

    obc_node_t *skel = (obc_node_t *)obc;
    if (!skel) {
        print("actor: p%02d has no object tree\n", num);
        free(e);
        return NULL;
    }
    skel->pos[0] = skel->pos[1] = skel->pos[2] = 0;

    /* Hang the skeleton under the embedded "gravit" node (464C1F..464C90). */
    memset(&e->gravit, 0, sizeof(e->gravit));
    snprintf(e->gravit.name, sizeof(e->gravit.name), "gravit");
    e->gravit.child = skel;          /* +0x200 = gravit+0x7C */
    skel->parent    = &e->gravit;
    EPTR(e, 0x50)   = &e->gravit;

    E32(e, 0xD34) = (i32)now_ms();
    E32(e, 0x78) = 0;     E32(e, 0x7C) = 0;
    E32(e, 0x80) = 0x27F; E32(e, 0x84) = 0x1DF;
    E32(e, 0x10) = 0;     E32(e, 0x00) = 0;
    E32(e, 0x14) = 0;     E32(e, 0x04) = 0;
    E32(e, 0x18) = 0x27F; E32(e, 0x08) = 0x27F;
    E32(e, 0x1C) = 0x1DF; E32(e, 0x0C) = 0x1DF;
    E32(e, 0x64) = 0x50;
    E32(e, 0x70) = 1;
    E32(e, 0x74) = 1;
    E32(e, 0xE94) = 0xFFFF;
    E32(e, 0x30) = (i32)0xFFFF4000;
    E32(e, 0x38) = (i32)0xFFFFC000;
    e->gravit.rot[0] -= 0xC000;      /* 464DB3 */
    e->gravit.rot[1] -= 0x4000;

    if (g_actor_count < (i32)(sizeof(g_actor_list) / sizeof(g_actor_list[0])))
        g_actor_list[g_actor_count++] = e;

    obc_process_hierarchy(skel);                 /* process_scene_hierarchy_with_memory */
    obc_create_control_point(skel);              /* create_single_control_point */
    E32(e, 0xC04) = -1;

    /* sub_41F6BA: bone slots by node name. */
    {
        obc_node_t *stack[256];
        i32 sp = 0;
        stack[sp++] = skel;
        while (sp > 0) {
            obc_node_t *n = stack[--sp];
            i32 slot = obc_node_type(n->name);
            if (slot >= 0 && slot < ENTITY_BONE_SLOTS)
                e->ptr[(0xFC >> 2) + slot] = n;
            for (obc_node_t *c = n->child; c; c = c->next)
                if (sp < 256) stack[sp++] = c;
        }
    }
    EPTR(e, 0xF0) = EPTR(e, 0x144);              /* 464E2C: slot 18 */

    /* sub_4699D9: parent / previous-sibling links from the child chains. */
    {
        obc_node_t *stack[256];
        i32 sp = 0;
        stack[sp++] = &e->gravit;
        while (sp > 0) {
            obc_node_t *n = stack[--sp], *prev = NULL;
            for (obc_node_t *c = n->child; c; prev = c, c = c->next) {
                c->parent = n;
                c->prev   = prev;
                if (sp < 256) stack[sp++] = c;
            }
        }
    }

    if (g_ai_flag && id < 0x62) {
        memset(&e->raw.b[0xD98], 0xFF, 0x20);
        E32(e, 0xD80) = 1;
        E32(e, 0xD84) = g_ai_param[0];
        /* +0xD98.. AI tuning, sub_42DDED (AI) not ported */
    } else {
        E32(e, 0xD80) = 0;
    }
    E32(e, 0xC64) = 0x10000;
    E32(e, 0xC68) = 0x4000;

    print("actor: created p%02d (%s)\n", num,
          num < HERO_COUNT ? "hero" : "persona");
    return e;
}

void actor_free(entity_t *e) {                        /* sub_4658D4 */
    if (!e) return;
    for (i32 i = 0; i < g_actor_count; i++)
        if (g_actor_list[i] == e) {
            g_actor_list[i] = g_actor_list[--g_actor_count];
            break;
        }
    for (i32 i = 0; i < PERSONA_COUNT; i++)
        if (g_persona_desc[i].entity == e) g_persona_desc[i].entity = NULL;
    lieu_zone_detach(e);
    if (e->gravit.child) obc_free(e->gravit.child);
    if (EPTR(e, 0x88)) mat_free((mat_lib_t *)EPTR(e, 0x88));
    if (g_companion_entity == e) g_companion_entity = NULL;
    if (g_secondary_companion_entity == e) g_secondary_companion_entity = NULL;
    if (g_player_entity == e) g_player_entity = NULL;
    free(e);
}

/* =========================================================================
 * Floor height — sub_457345 @ 0x457345
 * ========================================================================= */

/* sub_458F60 @ 0x458F60 — crossing test on the X/Z polygon of a zone. */
static i32 zone_contains(i32 n, i32 x, i32 z, const i32 *v) {
    i32 crossings = 0;
    for (i32 i = 0; i < n; i++) {
        i32 x0 = v[i * 3], z0 = v[i * 3 + 2];
        i32 x1, z1;
        if (i == n - 1) { x1 = v[0];           z1 = v[2]; }
        else            { x1 = v[i * 3 + 3];   z1 = v[i * 3 + 5]; }
        if (!((z0 < z && z <= z1) || (z1 < z && z <= z0))) continue;
        if (x0 < x && x1 < x) continue;
        if (x0 >= x && x1 >= x) { crossings++; continue; }
        i32 xc = x0 + (i32)((f64)(x1 - x0) * (f64)(z - z0) / (f64)(z1 - z0));
        if (xc >= x) crossings++;
    }
    return crossings & 1;
}

i32 actor_floor_height(i32 x, i32 z, const entity_t *e) {
    return actor_floor_zone(x, z, e, NULL);
}

i32 actor_floor_zone(i32 x, i32 z, const entity_t *e, void **zone) {
    if (zone) *zone = g_world.world_controller;
    if (!g_world.world_controller) return 0;
    /* With an actor whose foot node is one of the four leg slots, zones
     * flagged 0x10 are skipped (stairs the feet should not snap onto). */
    i32 skip_10 = 0;
    if (e) {
        void *foot = EPTR(e, 0xF0);
        skip_10 = foot && (foot == EPTR(e, 0x144) || foot == EPTR(e, 0x14C) ||
                           foot == EPTR(e, 0x150) || foot == EPTR(e, 0x148));
    }
    /* render_buffer_pool @ 0x4F9160 is the "sol%02d" list (stair_zones). */
    for (i32 i = 0; i < g_world.num_stairs; i++) {
        const sol_limit_t *s = g_world.stair_zones[i];
        if (!s) continue;
        const u8 *b = (const u8 *)s;
        i32 x0 = *(const i32 *)(b + 0x1DC), x1 = *(const i32 *)(b + 0x1E8);
        i32 z0 = *(const i32 *)(b + 0x1E4), z1 = *(const i32 *)(b + 0x1F0);
        if (!(x0 < x && x < x1 && z0 < z && z < z1)) continue;
        if (skip_10 && (s->flags & 0x10)) continue;
        if (!(s->flags & 1)) continue;
        if (zone_contains(*(const i32 *)(b + 0x1D8), x, z, (const i32 *)(b + 0x14))) {
            if (zone) *zone = (void *)s;
            return *(const i32 *)(b + 0x1EC);
        }
    }
    return *(const i32 *)((const u8 *)g_world.world_controller + 0x1E0);
}

/* =========================================================================
 * Node positions — sub_463A24 @ 0x463A24
 * ========================================================================= */

void actor_node_positions(obc_node_t *root, const fmat_t *start) {
    fmat_t stack_m[16];
    obc_node_t *stack_n[16];
    i32 sp = 0;

    fmat_t cur = *start;
    fmat_rotate_translation(&cur, &cur);          /* multiply_matrices(m, m) */
    obc_node_t *n = root;
    for (;;) {
        while (n) {
            if (n->next && sp < 16) {             /* push the sibling */
                stack_n[sp] = n->next;
                stack_m[sp] = cur;
                sp++;
            }
            fmat_t rot, pos;
            fmat_rotation(&rot, n->rot[0], n->rot[1], n->rot[2]);
            fmat_position(&pos, n->pos[0], n->pos[1], n->pos[2]);
            fmat_rotate_translation(&cur, &pos);
            fmat_premul_rotation(&rot, &cur);
            cur.t[0] += pos.t[0];
            cur.t[1] += pos.t[1];
            cur.t[2] += pos.t[2];
            n->raw_88[2] = cur.t[0];              /* +0x90 */
            n->raw_88[3] = cur.t[1];              /* +0x94 */
            n->raw_88[4] = cur.t[2];              /* +0x98 */
            n = n->child;
        }
        if (sp == 0) break;
        sp--;
        n   = stack_n[sp];
        cur = stack_m[sp];
    }
}

/* =========================================================================
 * Animation lookup
 * ========================================================================= */

i32 actor_anim_state(entity_t *e, const c8 *anim) {          /* sub_41918C */
    if (strcmp(anim, "attente") == 0)
        return (e == g_player_entity) ? 0x3B : -5;
    if (strcmp(anim, "ar") == 0) return 999;
    if (anim[0] == 'g' && strncmp(anim + 3, "arb", 3) == 0) return 1000;
    const u8 *d = EDESC(e);
    if (!d) return 0;
    for (i32 i = 0; i < 16; i++)
        if (strncmp((const c8 *)d + 0x9F + i * 8, anim, 8) == 0) return -(i + 1);
    i32 n = (e == g_player_entity) ? 0x3F : 0x20;
    for (i32 i = 0; i < n; i++)
        if (strncmp((const c8 *)d + 0x11F + i * 8, anim, 8) == 0) return i + 1;
    return 0;
}

i32 actor_prepare_anim(entity_t *e, i32 state, c8 *out) {    /* 0x419307 */
    if (e == g_player_entity && E32(e, 0xD58) == 0x1A) return 0;
    const u8 *d = EDESC(e);
    if (!d) return 0;
    const u8 *src = (state < 0) ? d + 0x9F + (-state - 1) * 8
                                : d + 0x117 + state * 8;
    E32(e, 0xD58) = state;
    if (src[0] == 'x') return 0;
    memcpy(out, src, 8);
    return 1;
}

void actor_bake_root_yaw(entity_t *e) {                       /* 0x41A9ED */
    can_rec_t *r = (can_rec_t *)EPTR(e, 0xD50);
    obc_node_t *g = EROOT(e);
    if (!r || !g || !g->child || !(CAN_FLAGS(r) & 1)) return;
    obc_node_t *skel = g->child;
    if (CAN_ROOT_YAW(r)) {
        i32 cur = skel->rot[0] & 0xFFFF;
        skel->rot[0] = CAN_ROOT_YAW(r);
        g->rot[2] -= cur - CAN_ROOT_YAW(r);
        g->rot[2] &= 0xFFFF;
        skel->rot[0] &= 0xFFFF;
    } else {
        g->rot[2] -= skel->rot[0];
        skel->rot[0] = 0;
    }
}


/* sub_465B47 — time stamp of the foot-contact message for `foot`. */
static i32 anim_foot_phase(entity_t *e, const can_rec_t *r, void *foot) {
    if (!foot) return -1;
    for (i32 i = 0; i < CAN_NMES(r); i++) {
        const u8 *m = r->mes[i];
        if (!m || m[1] != 0) continue;
        i32 slot = *(const i32 *)(m + 8);
        if (slot >= 0 && slot < ENTITY_BONE_SLOTS && e->ptr[(0xFC >> 2) + slot] == foot)
            return *(const i32 *)(m + 4);
    }
    return -1;
}

i32 actor_play_anim(entity_t *e, can_rec_t *rec, i32 keep_phase) { /* 0x465BDF */
    if (!e || !rec) return 0;
    const u8 *d = EDESC(e);
    if (e == g_player_entity && d && *(const i32 *)(d + 0x84) <= 0 && E32(e, 0xD58) != 0x1A)
        return 0;
    if (E32(e, 0xD58) == 0x1A && actor_anim_state(e, (const c8 *)rec->hdr) != 0x1A)
        return 0;
    if (E32(e, 0xD0C)) keep_phase = 0;
    /* 465C9D..465D4A: player combat-level bookkeeping (dword_505C54,
     * dword_505D4C) — combat is not ported. */

    E32(e, 0xC48) = 0;
    snprintf(ESTR(e, 0xC4), 14, "%.8s", (const c8 *)rec->hdr);
    E32(e, 0xC40) = 0;
    if (CAN_FLAGS(rec) & 0x20) {
        E32(e, 0xE88) = EROOT(e)->pos[0];
        E32(e, 0xE90) = EROOT(e)->pos[2];
    }
    E32(e, 0xD3C)++;

    i32 phase = -1;
    can_rec_t *cur = (can_rec_t *)EPTR(e, 0xD50);
    if (cur) {
        if (CAN_FLAGS(rec) & 0x800) {
            /* 465E65: snapshot the current keyframe as per-bone blend
             * matrices (sub_492C60) — the matrix blend path is not ported,
             * so the per-angle lerp in actor_blend_pose is used instead. */
        }
        E32(e, 0xD4C) = (CAN_FLAGS(cur) & 0x1000) ? (CAN_BLEND(cur) >> 16) : 0;
        if ((CAN_FLAGS(cur) & 0x80) && 0 /* is_master_eligible_creature */) {
            E32(e, 0xC6C) = 1;
            E32(e, 0xC70) = (i32)now_ms();
        } else if ((i32)now_ms() - E32(e, 0xC70) > 500) {
            E32(e, 0xC6C) = 0;
        }
        if (keep_phase && !E32(e, 0xC6C)) {
            void *foot = EPTR(e, 0xF0);
            if (E32(e, 0xB0) != 0x40) {
                if (EPTR(e, 0x144) == foot) foot = EPTR(e, 0x14C);
                if (EPTR(e, 0x148) == foot) foot = EPTR(e, 0x150);
                phase = anim_foot_phase(e, rec, foot);
                if (phase == -1) keep_phase = 0;
            } else {
                keep_phase = 0;
            }
        }
    } else {
        keep_phase = 0;
    }

    for (i32 o = 0xC80; o <= 0xCA8; o += 4) E32(e, o) = 0;
    EPTR(e, 0xD50) = rec;

    i32 t = (i32)now_ms();
    if (!keep_phase) {
        E32(e, 0xC28) = t;
        E32(e, 0xC2C) = t;
        E32(e, 0xC34) = t;
    } else {
        i32 back = (i32)((f64)t - (f64)phase / g_anim_divisor);
        E32(e, 0xC28) = back;
        E32(e, 0xC2C) = t;
        E32(e, 0xC34) = back;
    }
    E32(e, 0xD54) = 0;

    /* Root yaw of the first keyframe (466350..4663C2). */
    i32 yaw0 = rec->frame[0] ? rec->frame[0][1] : 0;
    obc_node_t *g = EROOT(e);
    if (g && g->child) {
        if ((CAN_FLAGS(rec) & 1) && CAN_ROOT_YAW(rec) == 0) g->rot[2] += yaw0;
        g->child->rot[0] = yaw0;
    }
    E32(e, 0xCE8) = 0;
    return 1;
}

i32 actor_play_state(entity_t *e, i32 st) {
    c8 name[16] = {0};
    actor_bake_root_yaw(e);
    if (!actor_prepare_anim(e, st, name)) return 0;
    return actor_play_anim(e, can_get_anim(name), 0);
}

i32 actor_play_named(entity_t *e, const c8 *anim) {
    if (!e || !anim) return 0;
    c8 name[16];
    snprintf(name, sizeof(name), "%s", anim);
    for (c8 *p = name; *p; p++) if (*p >= 'A' && *p <= 'Z') *p += 32;
    actor_bake_root_yaw(e);
    i32 ok = actor_play_anim(e, can_get_anim(name), 0);
    E32(e, 0xD58) = actor_anim_state(e, name);
    return ok;
}

i32 actor_anim_time(const entity_t *e) {
    return (i32)((f64)((i32)now_ms() - E32(e, 0xC28)) * g_anim_divisor);
}

i32 actor_anim_done(entity_t *e) {                            /* 0x418D7F */
    if (!e) return 1;
    if (lieu_loading()) return 1;                             /* dword_4B52DC == -1 */
    can_rec_t *r = (can_rec_t *)EPTR(e, 0xD50);
    if (!r) return 1;
    if (E32(e, 0xC48))
        return E32(e, 0xC48) <= actor_anim_time(e);
    if (CAN_FLAGS(r) & 0x200)
        return CAN_DURATION(r) - 1 == E32(e, 0xC40);
    return CAN_DURATION(r) <= actor_anim_time(e);
}

/* =========================================================================
 * Sampling — sub_414B8C, sub_414E66 (easing), sub_414C8A (lerp)
 * ========================================================================= */

static i32 anim_ease(i32 frac, i32 ease_in, i32 ease_out) {   /* 0x414E66 */
    i32 a = ease_in * 2;
    i32 b = ease_out * 2 + 0x200;
    i32 c = 0x400 - b;
    b -= a;
    i32 f = frac << 5;
    i32 s = 0x2000 - (cosine_table[((u32)(f + 8) >> 4) & 0xFFF] / 2);
    if (f < 0x4000) {
        i32 w = cosine_table[((u32)(f - 0x3FF8) >> 4) & 0xFFF];
        return (s * b + w * a + 0x2000) >> 14;
    }
    i32 w = 0x4000 - cosine_table[((u32)(f - 0x3FF8) >> 4) & 0xFFF];
    return ((s * b + c * w + 0x2000) >> 14) + a;
}

void anim_sample(const can_rec_t *r, i16 *pose, i32 t) {      /* 0x414B8C */
    for (i32 i = 0; i < CAN_NFRAMES(r) - 1; i++) {
        i32 t0 = CAN_TIME(r, i), t1 = CAN_TIME(r, i + 1);
        if (t0 > t || t1 < t) continue;
        i32 frac = ((t - t0) << 10) / (t1 - t0 ? t1 - t0 : 1);
        frac = anim_ease(frac, CAN_EASE_IN(r, i), CAN_EASE_OUT(r, i));
        const i16 *a = r->frame[i], *b = r->frame[i + 1];
        if (!a || !b) return;
        for (i32 j = 0; j < 34; j++)                          /* 0x414C8A */
            for (i32 k = 1; k <= 3; k++) {
                i32 d = (i16)(b[j * 3 + k] - a[j * 3 + k]);
                pose[j * 3 + k] = (i16)(((d * frac) >> 10) + a[j * 3 + k]);
            }
        return;
    }
}

void actor_set_pose(entity_t *e, const i16 *pose) {           /* 0x414DDE */
    for (i32 j = 0; j < 34; j++) {
        obc_node_t *n = EBONE(e, j);
        if (!n) continue;
        n->rot[0] = pose[j * 3 + 1];
        n->rot[1] = pose[j * 3 + 2];
        n->rot[2] = pose[j * 3 + 3];
    }
}

void actor_blend_pose(entity_t *e, const i16 *pose, i32 t) {  /* 0x415021 */
    can_rec_t *r = (can_rec_t *)EPTR(e, 0xD50);
    i32 dur;
    if (E32(e, 0xD4C))
        dur = (i32)((f64)E32(e, 0xD4C) * 0.48 / g_anim_divisor);
    else if (r && (CAN_FLAGS(r) & 0x100))
        dur = (i32)((f64)(CAN_BLEND(r) & 0xFFFF) * 0.48 / g_anim_divisor);
    else
        dur = (i32)(168.0 / g_anim_divisor);

    if (dur <= t) {
        actor_set_pose(e, pose);
        return;
    }
    i32 w = (t << 10) / (dur ? dur : 1), iw = 0x400 - w;
    for (i32 j = 0; j < 34; j++) {
        obc_node_t *n = EBONE(e, j);
        if (!n) continue;
        for (i32 k = 0; k < 3; k++) {
            i32 p = pose[j * 3 + 1 + k];
            i32 diff = n->rot[k] - p;
            if (diff < 0) diff = -diff;
            if (diff < 0x8000) n->rot[k] = (p * w + n->rot[k] * iw) / 1024;
        }
    }
}

/* =========================================================================
 * Timing — sub_461468 @ 0x4614B5..0x4617EE, update_entity_animation_timings
 * ========================================================================= */

void actor_update_timing(u32 now) {
    static i32 frames = 0;
    static u32 window = 0;
    static f64 target = 0.48;                                 /* dbl_4B72A0 */
    if (window == 0) window = now;
    if (++frames == 10) {
        frames = 0;
        f64 dt = (f64)(now - window);
        window = now;
        f64 fps = dt > 0 ? 10000.0 / dt : 100.0;
        if (fps >= 15.0) target = 0.48;
        if (fps <= 10.0) target = 0.24;
        if (fps > 10.0 && fps < 15.0) target = (fps - 10.0) * 0.24 / 5.0 + 0.24;
    }
    f64 diff = target - g_anim_divisor;
    if (diff < 0) diff = -diff;
    if (diff <= 0.04) return;

    entity_t *list[3] = { g_companion_entity, g_secondary_companion_entity, g_player_entity };
    for (i32 i = 0; i < 3; i++) {
        entity_t *e = list[i];
        if (!e) continue;
        static const i32 clocks[3] = { 0xC28, 0xC34, 0xC2C };
        for (i32 k = 0; k < 3; k++) {
            f64 el = (f64)((i32)now - E32(e, clocks[k]));
            E32(e, clocks[k]) = (i32)((f64)now - el * g_anim_divisor / target);
        }
    }
    g_anim_divisor = target;
}

/* =========================================================================
 * Placement — sub_463C2E, sub_463FB5, sub_4640BC
 * ========================================================================= */

/* Camera matrix the node positions are taken in (parameter_table). The port
 * keeps the scene camera in scene.c; placement only needs heights, so an
 * identity start gives world-space node positions directly. */
static void identity_fmat(fmat_t *m) {
    memset(m, 0, sizeof(*m));
    m->m[0] = m->m[4] = m->m[8] = 0x4000;
}

/* sub_46399F — the node named by the clip's first type-0 message. */
static obc_node_t *actor_foot_node(entity_t *e) {
    can_rec_t *r = (can_rec_t *)EPTR(e, 0xD50);
    if (!r) return NULL;
    for (i32 i = 0; i < CAN_NMES(r); i++) {
        const u8 *m = r->mes[i];
        if (m && m[1] == 0) {
            i32 slot = *(const i32 *)(m + 8);
            return (slot >= 0 && slot < ENTITY_BONE_SLOTS) ? EBONE(e, slot) : NULL;
        }
    }
    return NULL;
}

void actor_place(entity_t *e, i32 x, i32 z, i32 facing, const c8 *anim) {
    obc_node_t *g = EROOT(e);
    g->pos[0] = x;
    g->pos[2] = z;
    E32(e, 0x8C) = actor_floor_height(x, z, NULL);
    g->rot[2] = facing;

    i32 st = E32(e, 0xD04);
    if (st != 0x18 && st != 0x1B && st != 0x1C && st != 0x1A) {
        E32(e, 0xD08) = 0;
        E32(e, 0xD04) = 0;
    }
    if (E32(e, 0xD04) == 0) {
        c8 name[16] = {0};
        snprintf(name, sizeof(name), "%s", anim ? anim : "");
        E32(e, 0xD58) = actor_anim_state(e, name);
        if (E32(e, 0xD58) == 999) {
            memset(name, 0, sizeof(name));
            if (E32(e, 0xD60))
                actor_prepare_anim(e, 5, name);
            else if (e != g_player_entity)
                actor_prepare_anim(e, -5, name);
            else
                actor_prepare_anim(e, 0x3B, name);
            E32(e, 0xD58) = actor_anim_state(e, name);
        } else {
            E32(e, 0xCF0) = 0;
        }
        actor_play_anim(e, can_get_anim(name), 0);
        E32(e, 0xD58) = actor_anim_state(e, name);
    }

    can_rec_t *r = (can_rec_t *)EPTR(e, 0xD50);
    if (r) {
        /* 463E17: 'e' messages re-parent one bone under another. */
        const u8 *d = EDESC(e);
        if (d && d[0x99]) {
            for (i32 i = 0; i < CAN_NMES(r); i++) {
                const u8 *m = r->mes[i];
                if (!m || m[1] != 'e') continue;
                i32 p = *(const i32 *)(m + 8);
                i32 a = p & 0xFF, b = (p & ~0xFF) >> 8;
                if (a >= ENTITY_BONE_SLOTS || b >= ENTITY_BONE_SLOTS) continue;
                obc_node_t *na = EBONE(e, a), *nb = EBONE(e, b);
                if (na && nb && na->parent) {
                    na->parent->child = NULL;
                    na->parent = nb;
                    nb->child  = na;
                }
            }
        }
        i32 last = CAN_NFRAMES(r) - 1;
        const i16 *kf = (E32(e, 0x70) == 2 && last >= 0) ? r->frame[last] : r->frame[0];
        if (kf) actor_set_pose(e, kf);
        EPTR(e, 0xF0) = actor_foot_node(e);
    }

    fmat_t cam;
    identity_fmat(&cam);
    actor_node_positions(g, &cam);
    obc_node_t *foot = (obc_node_t *)EPTR(e, 0xF0);
    if (foot)
        g->pos[1] += E32(e, 0x8C) - foot->raw_88[3];
    else
        g->pos[1] = E32(e, 0x8C) + 60000;
    if (getenv("DKE_ACTORDBG"))
        print("actor: place p%02d at (%d,%d,%d) floor=%d foot=%s footY=%d facing=%d anim=%s\n",
              E32(e, 0xB0), g->pos[0], g->pos[1], g->pos[2], E32(e, 0x8C),
              foot ? foot->name : "-", foot ? foot->raw_88[3] : 0, facing,
              ESTR(e, 0xC4));
}

static void actor_place_ex(i32 id, entity_t *e, i32 x, i32 z, i32 facing,
                           const c8 *anim, i32 zone, i32 radius) {   /* sub_463FB5 */
    E32(e, 0xB0) = id < 0 ? -id : id;
    snprintf(ESTR(e, 0xC4), 14, "%s", anim ? anim : "");
    actor_place(e, x, z, facing, anim);
    if (radius != -1 && zone != -1) {
        i32 half = radius / 2;
        lieu_zone_attach(zone, e, half ? half * 600 : 150000, radius & 1);
    }
}

entity_t *actor_pos_perso(i32 id, i32 x, i32 z, i32 facing, i32 vie,
                          const c8 *anim, i32 zone, i32 radius) {    /* sub_4640BC */
    /* 0x4640E9..0x464250: the 0x62..0x67 ids swap the player to another
     * hero mid-level; not reached by the scripts ported so far. */
    if (id >= 0x62 && id <= 0x67) {
        print("actor: PosPerso hero swap %d not ported\n", id);
        return NULL;
    }

    entity_t *e = NULL;
    switch (vie) {
    case 0: case 1: case 2:
        if (id >= 0 && id < HERO_COUNT) {
            /* sub_465278 switches hero when the id differs from the current
             * one; with a single hero loaded it just reuses the player. */
            if (id != g_hero_id && vie == 0)
                print("actor: PosPerso hero %d (current %d) — swap not ported\n", id, g_hero_id);
            e = g_player_entity;
            if (e) E32(e, 0xC78) = 0;
        } else {
            e = (id < 0) ? NULL : actor_find(id < 0 ? -id : id);
            if (!e) {
                if (g_companion_entity && g_secondary_companion_entity) {
                    print("actor: PosPerso %d : too enemies loaded !\n", id);
                    return NULL;
                }
                entity_t **slot = !g_companion_entity ? &g_companion_entity
                                                      : &g_secondary_companion_entity;
                e = actor_create(id < 0 ? -id : id);
                if (!e) return NULL;
                *slot = e;
                E32(e, 0x14) = 1000; E32(e, 0x10) = 1000;
                E32(e, 0x1C) = -1;   E32(e, 0x18) = -1;
                E32(e, 0x04) = 1000; E32(e, 0x00) = 1000;
                E32(e, 0x0C) = -1;   E32(e, 0x08) = -1;
                E32(e, 0xD7C) = id < 0 ? -id : id;
                if (id >= 0 && vie == 2 && EDESC(e)) {
                    EDESC(e)[0x99] = 0x1B;
                    EDESC(e)[0x96] = 1;
                }
                EPTR(e, 0xCD8) = &g_nav_workspace[(slot == &g_companion_entity ? 1 : 2) * 0x1000];
                if (id >= 0 && id < PERSONA_COUNT) {
                    g_persona_desc[id].entity = e;
                    g_persona_desc[id].raw[0x95] = 1;
                }
                if (vie == 2) E32(e, 0x70) = 2;
            }
        }
        if (!e) return NULL;
        lieu_set_persona_vie(id, vie);
        actor_place_ex(id, e, x, z, facing, anim, zone, radius);
        if (vie == 2 && radius != -1) {
            E32(e, 0x70) = 0;
            if (EDESC(e)) EDESC(e)[0x96] = 1;
        }
        break;
    case 4:
        if (id < HERO_COUNT) {
            print("actor: POSPERSO %d ne doit pas avoir la vie a 4\n", id);
            return NULL;
        }
        e = actor_find(id);
        if (!e) return NULL;
        lieu_zone_detach(e);
        g_persona_desc[id].entity = NULL;
        g_persona_desc[id].raw[0x95] = 0;
        if (g_companion_entity == e) g_companion_entity = NULL;
        if (g_secondary_companion_entity == e) g_secondary_companion_entity = NULL;
        actor_free(e);
        return NULL;
    default:
        break;
    }
    return e;
}

/* --- mesh scaling: sub_433490 / sub_433610 (DIVERS 38 / 37) ----------
 * Scales this actor's own tree in place (permanent, cumulative); the
 * atom render caches are dropped so the next draw rebuilds them. */
f64 g_character_scale_factor = 1.0;            /* character_scale_factor */

static void atom_invalidate_render(atm_atom_t *a) {
    if (a->render) {
        if (atm_render_free) atm_render_free(a->render);
        a->render = NULL;
    }
}

static void atom_scale_verts(atm_atom_t *a, f64 s) {
    /* a->raw_18 = (i32)((f64)(u32)a->raw_18 * s);  -- +0x18, add the field if wanted */
    for (i32 i = 0; i < a->vert_count; i++) {
        i32 *v = a->verts + (size_t)i * ATM_VERT_STRIDE;
        v[ATM_VERT_X] = (i32)((f64)v[ATM_VERT_X] * s);
        v[ATM_VERT_Y] = (i32)((f64)v[ATM_VERT_Y] * s);
        v[ATM_VERT_Z] = (i32)((f64)v[ATM_VERT_Z] * s);
    }
    atom_invalidate_render(a);
}

/* sub_433490 @ 0x433490 (ecx = entity, edx = bone, stack: double s, int mode) */
void actor_scale_bone(entity_t *e, i32 bone, f64 s, i32 mode) {
    if (!e || bone < 0 || bone >= ENTITY_BONE_SLOTS) return;   /* original: no bound */
    obc_node_t *n = EBONE(e, bone);
    if (!n) return;
    if (!(mode & 2)) {
        n->pos[0] = (i32)((f64)n->pos[0] * s);
        n->pos[1] = (i32)((f64)n->pos[1] * s);
        n->pos[2] = (i32)((f64)n->pos[2] * s);
    }
    if (n->atom[0]) {
        n->raw_14 = (i32)((f64)(u32)n->raw_14 * s);
        atom_scale_verts(n->atom[0], s);
    }
    if (n->atom[1]) atom_scale_verts(n->atom[1], s);
    if ((mode & 1) && n->child) {
        obc_node_t *c = n->child;
        c->pos[0] = (i32)((f64)c->pos[0] * s);
        c->pos[1] = (i32)((f64)c->pos[1] * s);
        c->pos[2] = (i32)((f64)c->pos[2] * s);
    }
}

/* sub_433610 @ 0x433610: bones 1..0x21 except 0x1F (arme) / 0x20 (armea),
 * mode 0. Slot 0 (tree) is skipped, so the root offset to gravit is not
 * scaled. */
void actor_scale_body(entity_t *e, f64 s) {
    if (!e) return;
    g_character_scale_factor *= s;
    for (i32 i = 0; i < 0x22; i++) {
        if (i == 0 || i == 0x1F || i == 0x20 || i == 0x22) continue;
        if (EBONE(e, i)) actor_scale_bone(e, i, s, 0);
    }
}

