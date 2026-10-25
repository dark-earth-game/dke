#ifndef DKE_LIEU44_INT_H
#define DKE_LIEU44_INT_H

/*
 * lieu44_int.h — shared internals of the LIEU44.DLL port, split between
 * lieu44.c (entry, INIT, tick, tick body) and lieu44_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_100180CC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu44_trig.c). */
void l44_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu44_trig.c):
 *   l44_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l44_trig00(void);

#endif /* DKE_LIEU44_INT_H */
