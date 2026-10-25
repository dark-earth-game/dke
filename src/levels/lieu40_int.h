#ifndef DKE_LIEU40_INT_H
#define DKE_LIEU40_INT_H

/*
 * lieu40_int.h - shared internals of the LIEU40.DLL port, split between
 * lieu40.c (entry, INIT, tick, helpers) and lieu40_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001826C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu40_trig.c). */
void l40_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu40_trig.c):
 *   l40_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l40_trig00(void);

#endif /* DKE_LIEU40_INT_H */
