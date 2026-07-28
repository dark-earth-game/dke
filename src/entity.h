#ifndef DKE_ENTITY_H
#define DKE_ENTITY_H

/*
 * entity.h -- Dark Earth entity struct layout (0xEA0 = 3744 bytes).
 *
 * Recovered from IDA disassembly of dkev.exe: sub_46493B (player alloc),
 * sub_4640BC (entity placement), sub_463FB5 / sub_463C2E (field writes),
 * sub_41F6BA (bone registration), sub_42DDED (bone-anim mapping),
 * sub_448F4D (combat init), sub_426D2C (trigger scan).
 *
 * The struct is a flat 3744-byte blob in the original. We overlay named
 * fields at confirmed offsets; gaps are left as padding arrays. Use
 * ENT_FIELD(ent, off) for offsets not yet in the struct.
 */

#include "libsys/common.h"

#define ENTITY_SIZE       0xEA0   /* 3744 bytes */
#define ENTITY_BONE_SLOTS 34

/* Gravit transform sub-struct — embedded at entity+0x184, accessed via
 * the pointer at entity+0x50. World position + facing angle live here. */
typedef struct {
    u8  _pad0[0x3C];           /* 0x000..0x03B */
    i32 world_x;               /* 0x03C — world position X */
    i32 world_y;               /* 0x040 — world position Y (vertical) */
    i32 world_z;               /* 0x044 — world position Z */
    u8  _pad1[0x0C];           /* 0x048..0x053 */
    i32 viewport_x;            /* 0x054 — viewport X (init from entity+0x30) */
    i32 viewport_y;            /* 0x058 — viewport Y (init from entity+0x38) */
    i32 facing;                /* 0x05C — facing angle (16-bit BAM) */
} gravit_t;

/* Entity attributes sub-struct — pointed to by entity+0xAC. */
typedef struct {
    u8  _pad0[0x84];
    i32 energy;                /* 0x084 */
    u8  _pad1[0x11];           /* 0x088..0x095 */
    u8  active;                /* 0x096 */
    u8  incapacitated;         /* 0x097 */
    u8  _pad2;                 /* 0x098 */
    u8  combat_type;           /* 0x099 — set to 0x1B for combat mode 2 */
    u8  _pad3[0x60];           /* 0x09A..0x0FB */
    i32 stamina;               /* 0x0FC */
} entity_attribs_t;

typedef struct {
    /* 0x000..0x00F — current rect (mirrors of max rect) */
    i32 rect_cur[4];           /* left, top, right, bottom */

    /* 0x010..0x01F — max rect */
    i32 rect_max[4];           /* left, top, right, bottom */

    u8  _pad0[0x10];           /* 0x020..0x02F */

    /* 0x030 */
    i32 viewport_offset_x;     /* init: 0xFFFF4000 (-0xC000) */
    u8  _pad1[0x04];           /* 0x034..0x037 */
    i32 viewport_offset_y;     /* 0x038 — init: 0xFFFFC000 (-0x4000) */
    u8  _pad2[0x14];           /* 0x03C..0x04F */

    /* 0x050 — pointer to gravit transform block (→ entity+0x184) */
    gravit_t *gravit;          /* NOTE: 32-bit in original; padded on 64-bit */

    u8  _pad3_ptr[0x064 - 0x050 - (i32)sizeof(void*)]; /* pad to 0x064 */

    /* 0x064 */
    i32 render_scale;          /* init: 0x50 (player), 0x80 (scene_load) */
    u8  _pad4[0x08];           /* 0x068..0x06F */

    /* 0x070 */
    i32 render_mode;           /* 1=normal, 2=combat */
    i32 render_flag;           /* 1 */

    /* 0x078..0x084 — screen clip rect */
    i32 clip_left;             /* 0x078 — 0 */
    i32 clip_top;              /* 0x07C — 0 */
    i32 clip_right;            /* 0x080 — 639 */
    i32 clip_bottom;           /* 0x084 — 479 */

    u8  _pad5[0x04];           /* 0x088..0x08B */
    i32 effect_id;             /* 0x08C */

    /* 0x090 */
    i32 type_flag;             /* 2 */
    i32 type_flag2;            /* 0x094 — 2 */
    u8  _pad6[0x14];           /* 0x098..0x0AB */

    /* 0x0AC — EntityAttributes pointer */
    entity_attribs_t *attribs;

    /* 0x0B0 (on 32-bit; offset shifts on 64-bit due to pointer above) */
    i32 entity_type_id;        /* abs(spawn_id) from sub_463FB5 */
    u8  _pad7[0x10];           /* padding to 0x0C4 */

    /* 0x0C4 — entity name string (e.g. "g01arr01") */
    c8  name[32];              /* padded, null-terminated */

    u8  _pad8[0xFC - 0x0E4];   /* gap to bone table */

    /* 0x0FC — bone slot table: 34 pointers to scene hierarchy nodes */
    void *bones[ENTITY_BONE_SLOTS];

} entity_t;

/*
 * The full struct can't be laid out portably because the original is a
 * flat 32-bit blob with embedded pointers. For high-offset fields that
 * scene.c / lieu_cb.c access, use the raw-offset helper below on the
 * g_player_entity opaque pointer — same as the existing ent_set_u32.
 */

/* Read/write a u32 at a raw byte offset into an entity blob. */
static inline u32 ent_get(const void *ent, i32 off) {
    return *(const u32 *)((const u8 *)ent + off);
}
static inline void ent_put(void *ent, i32 off, u32 val) {
    *(u32 *)((u8 *)ent + off) = val;
}
static inline i32 ent_get_i32(const void *ent, i32 off) {
    return *(const i32 *)((const u8 *)ent + off);
}

/* Named offset constants for fields beyond the struct overlay.
 * Use with ent_get / ent_put until the struct is fully ported. */
#define ENT_OFF_STATE_FLAG     0xC04  /* -1 initial */
#define ENT_OFF_COMBAT_READY   0xC44  /* 1 when in combat */
#define ENT_OFF_HEALTH         0xC64  /* 0x10000 initial */
#define ENT_OFF_ENERGY         0xC68  /* 0x4000 initial */
#define ENT_OFF_SPECIAL_STATE  0xC78  /* 1=combat mode */
#define ENT_OFF_DLG_BUF        0xCD8  /* dialogue buffer pointer */
#define ENT_OFF_ANIM_P1        0xCEC
#define ENT_OFF_ANIM_P2        0xCF0
#define ENT_OFF_ANIM_P3        0xCF4
#define ENT_OFF_ANIM_P4        0xCF8
#define ENT_OFF_ANIM_STATE     0xD04  /* current anim state */
#define ENT_OFF_PREV_ANIM      0xD08  /* previous anim state */
#define ENT_OFF_LAST_TIME      0xD34  /* last game time */
#define ENT_OFF_MODEL_DATA     0xD50  /* model data pointer */
#define ENT_OFF_TYPE_STORED    0xD7C  /* entity type id (stored) */
#define ENT_OFF_SQUAD_FLAG     0xD80  /* 0=player, 1=NPC */
#define ENT_OFF_UNK_E94        0xE94  /* 0xFFFF */

/* Gravit absolute offsets (entity+0x184 base + gravit field) */
#define ENT_OFF_WORLD_X   0x1C0  /* gravit+0x3C */
#define ENT_OFF_WORLD_Y   0x1C4  /* gravit+0x40 */
#define ENT_OFF_WORLD_Z   0x1C8  /* gravit+0x44 */
#define ENT_OFF_FACING    0x1E0  /* gravit+0x5C */

/* Bone name table (34 entries, stride 7, from IDA sub_41F6BA at 0x4B2BA0) */
#define BONE_TREE      0   /* root */
#define BONE_BASDOS    1   /* lower spine */
#define BONE_HANGAU    2   /* left hip */
#define BONE_HANDRO    3   /* right hip */
#define BONE_GENDRO    4   /* right knee */
#define BONE_PIEDRO    5   /* right foot */
#define BONE_GENGAU    6   /* left knee */
#define BONE_PIEGAU    7   /* left foot */
#define BONE_MILDOS    8   /* mid torso */
#define BONE_HAUDOS    9   /* upper torso */
#define BONE_BASCOU   10   /* lower neck */
#define BONE_EPAGAU   11   /* left shoulder */
#define BONE_EPADRO   12   /* right shoulder */
#define BONE_COUDRO   13   /* right elbow */
#define BONE_POIDRO   14   /* right wrist */
#define BONE_COUGAU   15   /* left elbow */
#define BONE_POIGAU   16   /* left wrist */
#define BONE_TETE     17   /* head */
#define BONE_TALGAU   18   /* left heel */
#define BONE_TALDRO   19   /* right heel */
#define BONE_DDPGAU   20
#define BONE_DDPDRO   21
#define BONE_ACCES0   22   /* accessory slot 0 */
#define BONE_ACCES1   23
#define BONE_ACCES2   24
#define BONE_ACCES3   25
#define BONE_ACCES4   26
#define BONE_PACDRO   27   /* pack/bag right */
#define BONE_PACGAU   28   /* pack/bag left */
#define BONE_HACDRO   29
#define BONE_HACGAU   30
#define BONE_ARME     31   /* weapon */
#define BONE_ARMEA    32   /* weapon attachment A */
#define BONE_ARMEM    33   /* weapon attachment M */

#endif /* DKE_ENTITY_H */
