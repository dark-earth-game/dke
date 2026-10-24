/*
 * player.c — player entity init + input routing  (Slice D pass 1)
 *
 * Asm references:
 *   sub_46493B  @ 0046493B  lines 136208..136567 (359 asm lines)
 *   sub_464F6D  @ 00464F6D  lines 136575..136796 (221 asm lines)
 *
 * Port strategy:
 *   Both functions are >100 asm lines with nested sub-calls each >100 lines
 *   (setup_creature, sub_462BCC, process_scene_hierarchy_with_memory,
 *    create_single_control_point, sub_4699D9, sub_41F6BA, sub_42DDED,
 *    sub_4648DC, sub_475BB6, sub_472C5C, sub_473756, sub_471B80).
 *   Per spec: stub-log each nested call with // TODO: sub_XXXXXX.
 *   Allocate entity struct using asm-observed size from allocate_memory call:
 *     "mov ecx, 0EA0h; call allocate_memory"  → 0xEA0 = 3744 bytes.
 *   Zero-init (asm: push 0EA0h / push 0 / push eax / call _memset).
 *   Set default fields identified from asm field writes after memset.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/system.h"
#include "state.h"
#include "entity.h"
#include "obc.h"
#include "player.h"
#include "scene.h"
#include "game_init.h"
#include "actor.h"

/* The actor globals and their creation live in actor.c (sub_46493B). */

/* dword_505D68 — active weapon class index (0='A', 1='B' player, 2='C', 3='D') */
static i32 g_weapon_class_index = 0;  /* sub_471B80 @ 0x471B80 */

/* dword_505DDC — raw weapon type byte from persona descriptor +0x97 */
static u8 g_active_weapon_type = 0;   /* 0x505DDC */

/* byte_4F60E0 @ 0x4F60E0 — per-actor 0x1000-byte NAVIGATION workspace, read
 * by sub_45720D's flood fill. The player owns the first 0x1000 bytes. */
static u8 g_nav_workspace[0x1000] = {0};

/* sub_41F6BA — re-register bone slots after the tree changed (weapon attach). */
void entity_register_bones(void *entity, obc_node_t *scene_root) {
    entity_t *e = (entity_t *)entity;
    obc_node_t *stack[256];
    i32 sp = 0;
    if (!e || !scene_root) return;
    stack[sp++] = scene_root;
    while (sp > 0) {
        obc_node_t *node = stack[--sp];
        const i32 slot = obc_node_type(node->name);
        if (slot >= 0 && slot < ENTITY_BONE_SLOTS)
            e->ptr[(0xFC >> 2) + slot] = node;
        for (obc_node_t *child = node->child; child; child = child->next)
            if (sp < 256) stack[sp++] = child;
    }
}

/* Live player transform: the gravit node (entity+0x50) holds the world
 * position and the heading (rot[2], 16-bit BAM). */
i32 player_get_pos(i32 *x, i32 *y, i32 *z) {
    if (!g_player_entity || !EROOT(g_player_entity)) return 0;
    const obc_node_t *g = EROOT(g_player_entity);
    if (x) *x = g->pos[0];
    if (y) *y = g->pos[1];
    if (z) *z = g->pos[2];
    return 1;
}

u16 player_get_facing(void) {
    if (!g_player_entity || !EROOT(g_player_entity)) return 0;
    return (u16)(EROOT(g_player_entity)->rot[2] & 0xFFFF);
}

/* game_init @ 0x46C49B: player_entity = sub_46493B(dword_4B70E8). */
void player_entity_init(void) {
    g_player_entity = actor_create(0);
    print("player: player_entity = %p\n", (void *)g_player_entity);
}

/* sub_4648DC @ 0x4648DC — entity class identifier (0x5F bytes).
 * Returns single char: 'B' for player, 'C' companion, 'D' secondary, 'A' default.
 * Used to build weapon object names: sprintf("O%c%02d", class, weapon_type). */
static c8 entity_get_class_char(void *entity) { /* sub_4648DC @ 0x4648DC */
    if (entity == g_player_entity) return 'B';
    if (entity == g_companion_entity) return 'C';
    if (entity == g_secondary_companion_entity) return 'D';
    return 'A';
}

/* sub_471B80 @ 0x471B80 — post-weapon global setup (0x5F bytes).
 * Sets g_weapon_class_index from player entity+0xD6 (4th byte of weapon name
 * string at entity+0xD2). Standard "OB03" → +0xD6 is NUL → index 0. */
static void post_weapon_global_setup(void) { /* sub_471B80 @ 0x471B80 */
    g_weapon_class_index = 0;
    if (!g_player_entity) return;
    u8 *desc = EDESC(g_player_entity);
    if (!desc) return;
    u8 weapon_type = desc[0x97];
    if (weapon_type != 0) {
        i8 class_char = (i8)E8(g_player_entity, 0xD6);
        i32 index = class_char - 'A';
        if (index == -0x41) index = 0; /* NUL terminator case */
        g_weapon_class_index = index;
    }
}

/* -------------------------------------------------------------------------
 * 00464F6D
 * player_entity_init_secondary — weapon/companion buffer wire-up.
 * Mirrors sub_464F6D.
 *
 * asm: ECX = player_entity (or companion_entity / secondary_companion_entity).
 * Called once for player_entity from game_init phase 34.
 *
 * Key asm logic:
 *   1. If entity == player_entity  → [entity+0xCD8] = &byte_4F60E0
 *   2. If entity == companion      → [entity+0xCD8] = &byte_4F60E0 + 0x1000
 *   3. If entity == secondary_comp → [entity+0xCD8] = &byte_4F60E0 + 0x2000
 *   4. Read [entity+0xACh] (slot ptr), get slot[0x97] → var_C (weapon type)
 *   5. If var_C == 0 → skip weapon init branch
 *   6. Player branch (var_94=0): sub_4648DC + sprintf "O%c%02d" + sub_475BB6
 *      → weapon entity load, sub_472C5C, sub_473756, sub_41F6BA
 *   7. After: if player_entity match → dword_505DDC = var_C; sub_471B80
 *
 * All nested calls are >100 asm lines — stubbed per spec.
 * ---------------------------------------------------------------------- */
void player_entity_init_secondary(void) {
    print("player: player_entity_init_secondary start\n");

    if (!g_player_entity) {
        print("player: player_entity_init_secondary: g_player_entity is NULL, skipping\n");
        return;
    }

    /* Phase 1: +0xCD8 — per-entity nav workspace pointer (0x1000 bytes).
     * asm: cmp [var_98], player_entity; jnz ...; mov [ecx+0CD8h], offset byte_4F60E0
     * The player takes slot 0; companions take +0x1000 / +0x2000. */
    EPTR(g_player_entity, 0xCD8) = &g_nav_workspace[0];

    /* Phase 2: read weapon type from persona descriptor at [entity+0xACh]+0x97
     * (asm 0x464F94: mov eax,[ecx+0ACh]; movzx eax,byte ptr [eax+97h]).
     * Read the slot as a native pointer — player_entity_init stores a real
     * host pointer there via *(void **), so the 32-bit ent_get accessor
     * truncates it and the deref segfaults. Same access form as
     * post_weapon_global_setup (sub_471B80). */
    u8 *desc = EDESC(g_player_entity);
    u8 weapon_type = desc ? desc[0x97] : 0;
    print("player: weapon_type = %d (from persona descriptor)\n", weapon_type);

    /* Phase 3: weapon entity load (player branch)
     * asm: sub_4648DC → class char; sprintf "O%c%02d"; sub_475BB6 → slot lookup */
    if (weapon_type != 0) {
        c8 class_char = entity_get_class_char(g_player_entity); /* sub_4648DC @ 0x4648DC */
        c8 weapon_name[8];
        snprintf(weapon_name, sizeof(weapon_name), "O%c%02d", class_char, weapon_type);
        print("player: weapon object name = '%s'\n", weapon_name);

        /* sub_475BB6 @ 0x475BB6 — find/allocate degradable object slot */
        void *weapon_obj = scene_find_degradable_object(weapon_name);
        if (weapon_obj) {
            /* sub_472C5C @ 0x472C5C — load weapon mesh + materials */
            scene_weapon_entity_init(weapon_obj);

            /* sub_473756 @ 0x473756 — attach weapon to player skeleton */
            scene_weapon_entity_attach(g_player_entity, weapon_obj, 1);

            /* sub_41F6BA @ 0x41F6BA — re-register bone table */
            if (EROOT(g_player_entity))
                entity_register_bones(g_player_entity, EROOT(g_player_entity));
        } else {
            print("player: weapon '%s' not found in degradable pool\n", weapon_name);
        }
    }

    /* Phase 4: set weapon globals + post-weapon setup */
    g_active_weapon_type = weapon_type; /* dword_505DDC @ 0x505DDC */
    post_weapon_global_setup(); /* sub_471B80 @ 0x471B80 */
    print("player: weapon_class_index = %d, active_weapon_type = %d\n",
          g_weapon_class_index, g_active_weapon_type);

    print("player: player_entity_init_secondary done\n");
}

/* -------------------------------------------------------------------------
 * player_input_update — poll system_t.actions[] + mouse → fill *out.
 *
 * Key mappings (from macos.m confirmed):
 *   actions[119] (W) → velocity_z = -1.0f  (forward)
 *   actions[115] (S) → velocity_z = +1.0f  (backward)
 *   actions[97]  (A) → velocity_x = -1.0f  (strafe left)
 *   actions[100] (D) → velocity_x = +1.0f  (strafe right)
 *   mouse_left == 2  → want_action = 1      (attack edge)
 *
 * Semantics: reset per call; set based on current held state.
 * ---------------------------------------------------------------------- */
void player_input_update(player_input_t *out, system_t *sys) {
    if (!out || !sys) return;

    /* Reset per call — no accumulation. */
    out->velocity_x  = 0.0f;
    out->velocity_y  = 0.0f;
    out->velocity_z  = 0.0f;
    out->want_action = 0;

    /* Movement — arrow keys (primary) and WASD (alternate). */
    if (sys->actions[38] || sys->actions[119]) out->velocity_z = -1.0f; /* Up / W — forward  */
    if (sys->actions[40] || sys->actions[115]) out->velocity_z = +1.0f; /* Down / S — back   */
    if (sys->actions[37] || sys->actions[97])  out->velocity_x = -1.0f; /* Left / A — left   */
    if (sys->actions[39] || sys->actions[100]) out->velocity_x = +1.0f; /* Right / D — right */

    /* Attack edge: mouse_left pressed-this-frame (value == 2) */
    if (sys->mouse_left == 2) out->want_action = 1;

    // /* Log only when moving (suppress per-frame noise). */
    // if (out->velocity_x != 0.0f || out->velocity_z != 0.0f) {
    //     print("player: input v=(%.1f, %.1f) action=%d\n",
    //           out->velocity_x, out->velocity_z, out->want_action);
    // }
}
