#ifndef DKE_LIEU48_INT_H
#define DKE_LIEU48_INT_H

/*
 * lieu48_int.h — shared internals of the LIEU48.DLL port, split between
 * lieu48.c (entry, INIT, tick, tick body) and lieu48_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001E41C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu48_trig.c). */
void l48_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu48_trig.c):
 *   l48_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l48_trig00(void);

#endif /* DKE_LIEU48_INT_H */
