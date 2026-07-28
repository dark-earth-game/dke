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

#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/system.h"
#include "state.h"
#include "player.h"

/* -------------------------------------------------------------------------
 * g_player_entity — global player entity opaque blob.
 * Allocated in player_entity_init; referenced by scene.c, game_init.c.
 * ---------------------------------------------------------------------- */
void *g_player_entity = NULL;   /* player_entity (asm sym) */

/* -------------------------------------------------------------------------
 * Entity struct allocation size from asm:
 *   mov ecx, 0EA0h
 *   call allocate_memory
 * 0xEA0 = 3744 bytes.
 *
 * Field offsets identified from asm writes after memset:
 *   +0x090  (0x90)  = 2       (dword — entity type flag)
 *   +0x094  (0x94)  = 2       (dword — secondary type flag)
 *   +0x078  (0x78)  = 0       (dword — screen-clip left)
 *   +0x07C  (0x7C)  = 0       (dword — screen-clip top)
 *   +0x080  (0x80)  = 0x27F   (dword — screen-clip right  = 639)
 *   +0x084  (0x84)  = 0x1DF   (dword — screen-clip bottom = 479)
 *   +0x010  (0x10)  = 0       (dword — position_x)
 *   +0x000  (0x00)  = [+0x10] (dword — mirror of position_x)
 *   +0x014  (0x14)  = 0       (dword — position_y)
 *   +0x004  (0x04)  = [+0x14] (dword — mirror of position_y)
 *   +0x018  (0x18)  = 0x27F   (dword — bound_right  = 639)
 *   +0x008  (0x08)  = [+0x18] (dword — mirror of bound_right)
 *   +0x01C  (0x1C)  = 0x1DF   (dword — bound_bottom = 479)
 *   +0x00C  (0x0C)  = [+0x1C] (dword — mirror of bound_bottom)
 *   +0x064  (0x64)  = 0x50    (dword — 80)
 *   +0x070  (0x70)  = 1       (dword)
 *   +0x074  (0x74)  = 1       (dword)
 *   +0xE94  (0xE94) = 0xFFFF  (dword — 65535)
 *   +0x030  (0x30)  = 0xFFFF4000 (dword)
 *   +0x038  (0x38)  = 0xFFFFC000 (dword)
 *   +0xD64  (0xD64) = 0       (dword)
 *   +0xD68  (0xD68) = 0       (dword)
 *   +0xC04  (0xC04) = 0xFFFFFFFF (dword)
 *   +0xC64  (0xC64) = 0x10000 (dword)
 *   +0xC68  (0xC68) = 0x4000  (dword)
 *   +0xD34  (0xD34) = current_game_time (dword)
 *   +0xD80  (0xD80) = 0 or 1  (dword — NPC squad flag; 0 for player)
 *
 * Nested sub-call stubs (>100 asm lines each):
 *   sub_462BCC  — creature descriptor / persona file load
 *   setup_creature — creature mesh/animation init
 *   process_scene_hierarchy_with_memory — control-point hierarchy
 *   create_single_control_point — root control point
 *   sub_4699D9  — anim state init
 *   sub_41F6BA  — entity registration
 *   sub_42DDED  — squad/companion wiring
 * ---------------------------------------------------------------------- */

/* Helper: write a u32 at byte offset into the entity blob. */
static void ent_set_u32(void *ent, i32 off, u32 val) {
    u8 *p = (u8 *)ent;
    u32 *slot = (u32 *)(p + off);
    *slot = val;
}

/* -------------------------------------------------------------------------
 * 0046493B
 * player_entity_init — allocate + zero-init + default-field-set the player
 * entity struct.  Mirrors sub_46493B for the player (ECX < 0x62 path).
 * ---------------------------------------------------------------------- */
/* -------------------------------------------------------------------------
 * player_get_pos / player_get_facing — live player transform accessors.
 *
 * In dkev the live transform is the embedded "gravit" block at entity+0x184
 * (engine reaches it via [entity+0x50] after a re-point). Position X/Y/Z sit
 * at +0x3C/+0x40/+0x44 of that block = entity+0x1C0/+0x1C4/+0x1C8; facing
 * (16-bit BAM) at +0x5C = entity+0x1E0. We read the embedded offsets
 * directly rather than through the [0x50] pointer, since storing a 64-bit
 * pointer in the 32-bit blob field would truncate on this build.
 *
 * NOTE: nothing writes velocity into the transform yet (movement not
 * ported), so position stays (0,0,0) — the accessors are correct, the data
 * is just static until movement integration lands.
 * ---------------------------------------------------------------------- */
i32 player_get_pos(i32 *x, i32 *y, i32 *z) {
    if (!g_player_entity) return 0;
    const u8 *e = (const u8 *)g_player_entity;
    if (x) *x = *(const i32 *)(e + 0x1C0);
    if (y) *y = *(const i32 *)(e + 0x1C4);
    if (z) *z = *(const i32 *)(e + 0x1C8);
    return 1;
}

u16 player_get_facing(void) {
    if (!g_player_entity) return 0;
    return (u16)(*(const i32 *)((const u8 *)g_player_entity + 0x1E0) & 0xFFFF);
}

void player_entity_init(void) {
    print("player: player_entity_init start\n");

    /* asm: mov ecx, 0EA0h; call allocate_memory → eax = ptr               */
    i32 entity_size = 0x0EA0; /* 3744 bytes */
    void *ent = calloc(1, (size_t)entity_size);
    if (!ent) {
        print("player: player_entity_init: calloc failed (size=%d)\n", entity_size);
        return;
    }
    print("player: player_entity_init: allocated %d bytes\n", entity_size);

    /* asm: push 0EA0h; push 0; push eax; call _memset — already zeroed by calloc */

    /* Default field writes from asm (player path, ECX < 0x62): ---------- */

    /* +0x090 = 2, +0x094 = 2 (entity type / secondary type flags) */
    ent_set_u32(ent, 0x090, 2);
    ent_set_u32(ent, 0x094, 2);

    /* Screen-clip rect: left=0, top=0, right=639 (0x27F), bottom=479 (0x1DF) */
    ent_set_u32(ent, 0x078, 0);
    ent_set_u32(ent, 0x07C, 0);
    ent_set_u32(ent, 0x080, 0x27F);
    ent_set_u32(ent, 0x084, 0x1DF);

    /* Position mirrors (asm: [+0] = [+0x10] = 0, [+4] = [+0x14] = 0) */
    ent_set_u32(ent, 0x010, 0);
    ent_set_u32(ent, 0x000, 0);
    ent_set_u32(ent, 0x014, 0);
    ent_set_u32(ent, 0x004, 0);

    /* Bound mirrors (asm: [+8]=[+0x18]=0x27F, [+0xC]=[+0x1C]=0x1DF) */
    ent_set_u32(ent, 0x018, 0x27F);
    ent_set_u32(ent, 0x008, 0x27F);
    ent_set_u32(ent, 0x01C, 0x1DF);
    ent_set_u32(ent, 0x00C, 0x1DF);

    /* Misc defaults */
    ent_set_u32(ent, 0x064, 0x50);       /* = 80 */
    ent_set_u32(ent, 0x070, 1);
    ent_set_u32(ent, 0x074, 1);
    ent_set_u32(ent, 0xE94, 0xFFFF);
    ent_set_u32(ent, 0x030, 0xFFFF4000U);
    ent_set_u32(ent, 0x038, 0xFFFFC000U);
    ent_set_u32(ent, 0xD64, 0);
    ent_set_u32(ent, 0xD68, 0);
    ent_set_u32(ent, 0xC04, 0xFFFFFFFFU);
    ent_set_u32(ent, 0xC64, 0x10000);
    ent_set_u32(ent, 0xC68, 0x4000);

    /* +0xD34 = current_game_time (from asm: mov eax,current_game_time; mov [ecx+0D34h],eax) */
    if (state) {
        ent_set_u32(ent, 0xD34, state->current_game_time);
    }

    /* +0xD80 = 0 for player (no squad) */
    ent_set_u32(ent, 0xD80, 0);

    /* Player weapon-slot table link at +0xACh: player always uses the
     * main weapon slot (dword_4ECD50 ptr — not yet tracked; NULL for now). */
    /* TODO: [ent+0xACh] = &dword_4ECD50 once weapon-slot table exported */

    /* Nested sub-calls — all >100 asm lines, stubbed per spec: ---------- */

    /* TODO: sub_462BCC — creature descriptor / persona file load
     *   asm: lea ecx,[var_108]; mov edx,abs(ecx_param); call sub_462BCC */
    print("player: TODO sub_462BCC (creature descriptor load)\n");

    /* TODO: setup_creature — mesh + animation init
     *   asm: lea edx,[var_80]; mov ecx,offset aPersos_12; call setup_creature */
    print("player: TODO setup_creature\n");

    /* TODO: process_scene_hierarchy_with_memory
     *   asm: mov edx,[var_114]; mov ecx,[var_84]; call process_scene_hierarchy_with_memory */
    print("player: TODO process_scene_hierarchy_with_memory\n");

    /* TODO: create_single_control_point
     *   asm: mov edx,[var_114]; mov ecx,[var_84]; call create_single_control_point */
    print("player: TODO create_single_control_point\n");

    /* TODO: sub_4699D9 — anim state init
     *   asm: mov eax,[var_84]; mov ecx,[eax+50h]; call sub_4699D9 */
    print("player: TODO sub_4699D9 (anim state init)\n");

    /* TODO: sub_41F6BA — entity registration
     *   asm: mov ecx,[var_84]; call sub_41F6BA */
    print("player: TODO sub_41F6BA (entity registration)\n");

    /* Store as global player entity */
    g_player_entity = ent;
    print("player: player_entity_init: g_player_entity = %p\n", g_player_entity);
    print("player: player_entity_init done\n");
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

    /* Phase 1: +0xCD8 — dialogue/animation buffer pointer
     * asm: cmp [var_98], player_entity; jnz ...; mov [ecx+0CD8h], offset byte_4F60E0
     * byte_4F60E0 is a fixed game-data segment address; NULL placeholder here. */
    /* TODO: wire +0xCD8 to byte_4F60E0 once that data segment is mapped */
    print("player: TODO +0xCD8 dialogue buffer ptr (byte_4F60E0 not mapped)\n");

    /* Phase 2: read weapon type from slot table at [entity+0xACh] + 0x97 offset
     * asm: mov eax,[ebp+var_98]; mov eax,[eax+0ACh]; ... cl,[eax+97h] → var_C
     * +0xACh is the weapon-slot table ptr (set in player_entity_init TODO).
     * Without it, var_C == 0, which takes the "skip" branch in asm. */
    /* TODO: sub_4648DC (weapon class decode) */
    print("player: TODO weapon slot read at [entity+0xACh]+0x97 (slot table not yet wired)\n");

    /* Phase 3: weapon entity load (player branch — var_94 = 0)
     * asm: sub_4648DC; sprintf "O%c%02d"; sub_475BB6; sub_472C5C; sub_473756; sub_41F6BA */
    print("player: TODO sub_4648DC (weapon class decode)\n");
    print("player: TODO sub_475BB6 (object lookup by name)\n");
    print("player: TODO sub_472C5C (weapon entity init)\n");
    print("player: TODO sub_473756 (weapon entity attach)\n");
    print("player: TODO sub_41F6BA (entity registration)\n");

    /* Phase 4: if player_entity → dword_505DDC = weapon_type; sub_471B80
     * asm: mov eax,player_entity; cmp [var_98],eax; jnz ...; call sub_471B80 */
    /* TODO: dword_505DDC = weapon_type; sub_471B80 */
    print("player: TODO dword_505DDC write + sub_471B80\n");

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

    /* WASD movement */
    if (sys->actions[119]) out->velocity_z = -1.0f; /* W — forward  */
    if (sys->actions[115]) out->velocity_z = +1.0f; /* S — backward */
    if (sys->actions[97])  out->velocity_x = -1.0f; /* A — left     */
    if (sys->actions[100]) out->velocity_x = +1.0f; /* D — right    */

    /* Attack edge: mouse_left pressed-this-frame (value == 2) */
    if (sys->mouse_left == 2) out->want_action = 1;

    /* Log only when moving (suppress per-frame noise). */
    if (out->velocity_x != 0.0f || out->velocity_z != 0.0f) {
        print("player: input v=(%.1f, %.1f) action=%d\n",
              out->velocity_x, out->velocity_z, out->want_action);
    }
}
