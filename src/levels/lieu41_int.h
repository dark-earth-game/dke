#ifndef DKE_LIEU41_INT_H
#define DKE_LIEU41_INT_H

/*
 * lieu41_int.h - shared internals of the LIEU41.DLL port, split between
 * lieu41.c (entry, INIT, tick, helpers) and lieu41_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* Script variables of this level (state block slots; low slots are per-level scratch). */
#define L41_CAM1_REMARK_DONE             0x64   /* 1 once the first-time camera-1 remark (L41D01) has been said; trigger 0 clears */
#define L41_SEARCH_REMARK_COUNT          0x68   /* line counter of the hero's remarks on the action key away from zones 3..7: 0 = first (L41D10/11), 1 = second (L41D12/13), 2 = repeat (L41D14/15) */
#define L41_SIDE_ZONE_REMARK_DONE        0x6C   /* 0 = side zone 6/7 remark L41D09 not said yet, 1 = said */
#define L41_ENTRY_REMARK_DONE            0x8E8   /* 0 = room-entry line L41D02 not said yet, 1 = said (persists across visits) */
#define L41_DEATH_ENDING_BYPASS          0xE8C   /* 1 = hero death does not play the death ending in INIT (runs trigger 0 instead); shared flag */

/* ST() reads g_lvl_S, the DLL's dword_1001BE4C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu41_trig.c). */
void l41_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu41_trig.c):
 *   l41_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l41_trig00(void);

#endif /* DKE_LIEU41_INT_H */
