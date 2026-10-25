#ifndef DKE_LIEU13_INT_H
#define DKE_LIEU13_INT_H

/*
 * lieu13_int.h — shared internals of the LIEU13.DLL port, split between
 * lieu13.c (entry, INIT, tick, helpers) and lieu13_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001A95C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu13_trig.c). */
void l13_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu13_trig.c):
 *   l13_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l13_trig00(void);

#endif /* DKE_LIEU13_INT_H */
