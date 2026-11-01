#ifndef DKE_LIEU40_INT_H
#define DKE_LIEU40_INT_H

/*
 * lieu40_int.h - shared internals of the LIEU40.DLL port, split between
 * lieu40.c (entry, INIT, tick, helpers) and lieu40_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* Script variables of this level (state block slots; low slots are per-level scratch). */
#define L40_DEATH_HANDLED                0x64   /* 1 once the hero-dead cleanup (zero HP of every present Arkhan, unlock) has run; trigger 0 clears */
#define L40_ENTRY_CUTSCENE_DONE          0x4F8   /* 0 = room-entry cutscene (L40D07A/B) not played yet, 1 = played */
#define L40_DEATH_ENDING_BYPASS          0xE8C   /* 1 = hero death does not play the death ending in INIT (runs trigger 0 instead); shared flag */

/* ST() reads g_lvl_S, the DLL's dword_1001826C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu40_trig.c). */
void l40_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu40_trig.c):
 *   l40_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l40_trig00(void);

#endif /* DKE_LIEU40_INT_H */
