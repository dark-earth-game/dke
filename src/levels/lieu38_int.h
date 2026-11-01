#ifndef DKE_LIEU38_INT_H
#define DKE_LIEU38_INT_H

/*
 * lieu38_int.h - shared internals of the LIEU38.DLL port, split between
 * lieu38.c (entry, INIT, tick, helpers) and lieu38_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* Script variables of this level (state block slots; low slots are per-level scratch). */
#define L38_UNUSED_CLEARED_64            0x64   /* cleared by trigger 0, never read in this level */
#define L38_DRAIN_LAST_TIME              0x68   /* ST_GAME_TIME of the last drain step (also set when the key spot is first used) */
#define L38_DRAIN_DARKNESS_STEP          0x6C   /* darkness removed per drain step (3, set by trigger 0) */
#define L38_DRAIN_STEP_COUNT             0x70   /* number of drain steps done while V_EVENT_0 is armed; ends at 30 (31 forces the end) */
#define L38_DRAIN_HP_STEP                0x74   /* hero HP removed per drain step (10, set by trigger 0) */
#define L38_UNUSED_CLEARED_78            0x78   /* cleared by trigger 0, never read in this level */
#define L38_UNUSED_CLEARED_7C            0x7C   /* cleared by trigger 0, never read in this level */
#define L38_ZONE23_TALKED                0x80   /* 0 = first zone-23 line (L38D37), 1 = repeat line (L38D38) */
#define L38_ZONE25_TALKED                0x84   /* 0 = first zone-25 line (L38D45/47), 1 = repeat line (L38D46/48) */
#define L38_STANCE_WAS_ON                0x88   /* 1 = hero was in combat stance when the intro trigger fired; stance is restored afterwards */
#define L38_GUARD91_REARM                0xE0   /* 99 = hero entered the intro zone while guard 91 is hostile; tick re-arms combat(91) and clears it */
#define L38_TALK_RANDOM_INTRO            0x524   /* 0 = first talk with the zone 7..11 persona (three lines), 1 = random single line */
#define L38_VISITED                      0x55C   /* 1 once the level has been entered (set by trigger 0; no other reader) */
#define L38_OB45_EXAMINED                0x62C   /* 0 = first time the hero uses the item on OB45 (L38D11/L38D13), 1 = repeat (L38D12) */
#define L38_GUARD91_STATE                0xC40   /* skeleton guard (persona 91): 0 = unaware, 1 = noticed hero / fighting, 2 = dead */
#define L38_EXIT39_DOOR                  0xD5C   /* door used to leave towards room 39: 1 = zone 4, 2 = zone 5 */
#define L38_KEYSPOT_NEAR                 0xD60   /* 1 while the hero stands at the key spot (descriptor distance < 90) on camera 5/6; 0 when he moves away (> 100) */
#define L38_KEYSPOT_USES                 0xD64   /* times the key spot was used (0..2); the third use only says L38D05/06 */
#define L38_ENTRY_DOOR_FROM39            0xD84   /* door the hero came through from room 39 (1 or 2); selects arrival position, 2 also opens zone 25 */
#define L38_INTRO_DONE                   0xF34   /* 0 = intro dialogue L38D01/L38D02 not yet played, 1 = played */

/* ST() reads g_lvl_S, the DLL's dword_1001BE4C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu38_trig.c). */
void l38_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu38_trig.c):
 *   l38_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l38_trig00(void);

#endif /* DKE_LIEU38_INT_H */
