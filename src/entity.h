#ifndef DKE_ENTITY_H
#define DKE_ENTITY_H

/*
 * entity.h -- Dark Earth actor ("perso") record, 0xEA0 bytes in the original.
 *
 * Allocated by sub_46493B @ 0x46493B for every character: the player, the two
 * companion slots (companion_entity @ 0x4EDAF0, secondary_companion_entity
 * @ 0x4EDAF4) and the hero swap path. The original is a flat 32-bit blob
 * that mixes scalars, host pointers and an embedded hierarchy node. Kept as
 * one blob on a 64-bit host, every pointer store shreds its neighbour, which
 * is where most of the port's early entity crashes came from.
 *
 * Layout used here, all indexed by the ORIGINAL byte offset so the C reads
 * like the asm:
 *
 *   raw     — the 0xEA0 scalar bytes. i32/i16/u8/f64 fields live at their
 *             original offsets (E32/E16/E8/EF64).
 *   ptr     — one host pointer slot per original dword (EPTR). A field the
 *             original stores a pointer in is read and written here instead
 *             of in raw, so pointers never alias scalars.
 *   gravit  — the 0xB4-byte hierarchy node embedded at +0x184 ("gravit").
 *             sub_46493B re-points entity+0x50 at it and hangs the loaded
 *             skeleton under it, so gravit.pos is the actor's world position
 *             and gravit.rot[2] its heading. Nodes are C structs (obc.h), so
 *             raw[0x184..0x237] is unused.
 *
 * Known fields (offsets are the original's):
 *   +0x050 ptr  root node = &gravit after creation
 *   +0x088 ptr  material library (setup_creature)
 *   +0x08C i32  ground height under the actor (generate_effect)
 *   +0x0AC ptr  persona descriptor (persona_desc_t raw bytes)
 *   +0x0B0 i32  abs(persona id)
 *   +0x0C4 c8[] name of the animation currently playing
 *   +0x0D2 c8[] weapon object name
 *   +0x0F0 ptr  foot node (sub_46399F) used to stand the actor on the floor
 *   +0x0FC ptr[34] bone slot table (sub_41F6BA), indexed like CAN channels
 *   +0x178 ptr  weapon attach node
 *   +0x240 f64[34][9] per-bone blend matrices (play_entity_animation)
 *   +0xBD0 u8[34] per-bone "has blend matrix" flags
 *   +0xBF4 f32  blend weight
 *   +0xC28 i32  animation start time;  +0xC2C / +0xC34 companion clocks
 *   +0xC40 i32  current keyframe;      +0xC48 i32 duration override
 *   +0xCD8 ptr  0x1000-byte nav workspace (sub_45720D flood fill)
 *   +0xD04 i32  animation state;       +0xD08 previous state
 *   +0xD4C i32  blend duration (anim+0x108 >> 16 when flag 0x1000)
 *   +0xD50 ptr  animation record (can_anim_t) currently playing
 *   +0xD58 i32  state id of the last prepared animation
 *   +0xD64 ptr  held weapon object, +0xD68 ptr second weapon slot
 */

#include "libsys/common.h"
#include "obc.h"

#define ENTITY_SIZE       0xEA0   /* 3744 bytes */
#define ENTITY_BONE_SLOTS 34

typedef struct entity_s {
    union {
        u8  b[ENTITY_SIZE];
        i16 h[ENTITY_SIZE / 2];
        i32 d[ENTITY_SIZE / 4];
        f64 f[ENTITY_SIZE / 8];
    } raw;
    void      *ptr[ENTITY_SIZE / 4];
    obc_node_t gravit;
} entity_t;

/* Field accessors, original byte offsets. */
#define E32(e, off)  ((e)->raw.d[(off) >> 2])
#define E16(e, off)  (*(i16 *)&(e)->raw.b[(off)])
#define E8(e, off)   ((e)->raw.b[(off)])
#define EF32(e, off) (*(f32 *)&(e)->raw.b[(off)])
#define EF64(e, off) (*(f64 *)&(e)->raw.b[(off)])
#define EPTR(e, off) ((e)->ptr[(off) >> 2])
#define ESTR(e, off) ((c8 *)&(e)->raw.b[(off)])

/* Bone slot i (entity+0xFC + i*4). */
#define EBONE(e, i)  ((obc_node_t *)(e)->ptr[(0xFC >> 2) + (i)])

/* Root node (entity+0x50) and persona descriptor bytes (entity+0xAC). */
#define EROOT(e)     ((obc_node_t *)EPTR(e, 0x50))
#define EDESC(e)     ((u8 *)EPTR(e, 0xAC))

/* Legacy scalar helpers. raw is the first member, so a void* entity still
 * addresses the scalar bytes directly. Never use these for pointer fields. */
static inline u32 ent_get(const void *ent, i32 off) {
    return *(const u32 *)((const u8 *)ent + off);
}
static inline void ent_put(void *ent, i32 off, u32 val) {
    *(u32 *)((u8 *)ent + off) = val;
}
static inline i32 ent_get_i32(const void *ent, i32 off) {
    return *(const i32 *)((const u8 *)ent + off);
}

/* Named offsets for scalar fields. */
#define ENT_OFF_STATE_FLAG     0xC04  /* -1 initial */
#define ENT_OFF_COMBAT_READY   0xC44  /* 1 when in combat */
#define ENT_OFF_HEALTH         0xC64  /* 0x10000 initial */
#define ENT_OFF_ENERGY         0xC68  /* 0x4000 initial */
#define ENT_OFF_SPECIAL_STATE  0xC78  /* 1=combat mode */
#define ENT_OFF_ANIM_P1        0xCEC
#define ENT_OFF_ANIM_P2        0xCF0
#define ENT_OFF_ANIM_P3        0xCF4
#define ENT_OFF_ANIM_P4        0xCF8
#define ENT_OFF_ANIM_STATE     0xD04  /* current anim state */
#define ENT_OFF_PREV_ANIM      0xD08  /* previous anim state */
#define ENT_OFF_LAST_TIME      0xD34  /* last game time */
#define ENT_OFF_TYPE_STORED    0xD7C  /* entity type id (stored) */
#define ENT_OFF_SQUAD_FLAG     0xD80  /* 0=player, 1=NPC */
#define ENT_OFF_UNK_E94        0xE94  /* 0xFFFF */

#endif /* DKE_ENTITY_H */
