#ifndef DKE_LIEU42_INT_H
#define DKE_LIEU42_INT_H

/*
 * lieu42_int.h — shared internals of the LIEU42.DLL port, split between
 * lieu42.c (entry, INIT, tick, tick body) and lieu42_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001BA0C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu42_trig.c). */
void l42_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu42_trig.c):
 *   l42_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l42_trig00(void);

#endif /* DKE_LIEU42_INT_H */
