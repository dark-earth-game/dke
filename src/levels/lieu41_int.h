#ifndef DKE_LIEU41_INT_H
#define DKE_LIEU41_INT_H

/*
 * lieu41_int.h - shared internals of the LIEU41.DLL port, split between
 * lieu41.c (entry, INIT, tick, helpers) and lieu41_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001BE4C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu41_trig.c). */
void l41_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu41_trig.c):
 *   l41_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l41_trig00(void);

#endif /* DKE_LIEU41_INT_H */
