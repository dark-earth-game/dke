#ifndef DKE_LIEU46_INT_H
#define DKE_LIEU46_INT_H

/*
 * lieu46_int.h — shared internals of the LIEU46.DLL port, split between
 * lieu46.c (entry, INIT, tick, tick body) and lieu46_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001859C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu46_trig.c). */
void l46_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu46_trig.c):
 *   l46_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l46_trig00(void);

#endif /* DKE_LIEU46_INT_H */
