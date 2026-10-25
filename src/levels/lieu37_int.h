#ifndef DKE_LIEU37_INT_H
#define DKE_LIEU37_INT_H

/*
 * lieu37_int.h — shared internals of the LIEU37.DLL port, split between
 * lieu37.c (entry, INIT, tick, tick body) and lieu37_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001B8EC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu37_trig.c). */
void l37_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu37_trig.c):
 *   l37_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l37_trig00(void);

#endif /* DKE_LIEU37_INT_H */
