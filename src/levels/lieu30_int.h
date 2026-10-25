#ifndef DKE_LIEU30_INT_H
#define DKE_LIEU30_INT_H

/*
 * lieu30_int.h - shared internals of the LIEU30.DLL port, split between
 * lieu30.c (entry, INIT, tick, helpers) and lieu30_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001C9AC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu30_trig.c). */
void l30_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu30_trig.c):
 *   l30_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l30_trig00(void);

#endif /* DKE_LIEU30_INT_H */
