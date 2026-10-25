#ifndef DKE_LIEU45_INT_H
#define DKE_LIEU45_INT_H

/*
 * lieu45_int.h — shared internals of the LIEU45.DLL port, split between
 * lieu45.c (entry, INIT, tick, tick body) and lieu45_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001A6FC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu45_trig.c). */
void l45_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu45_trig.c):
 *   l45_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l45_trig00(void);

#endif /* DKE_LIEU45_INT_H */
