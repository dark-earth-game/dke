#ifndef DKE_LIEU07_INT_H
#define DKE_LIEU07_INT_H

/*
 * lieu07_int.h — shared internals of the LIEU07.DLL port, split between
 * lieu07.c (entry, INIT, tick, helpers) and lieu07_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001E2AC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu07_trig.c). */
void l07_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu07_trig.c):
 *   l07_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l07_trig00(void);

#endif /* DKE_LIEU07_INT_H */
