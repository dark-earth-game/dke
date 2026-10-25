#ifndef DKE_LIEU28_INT_H
#define DKE_LIEU28_INT_H

/*
 * lieu28_int.h - shared internals of the LIEU28.DLL port, split between
 * lieu28.c (entry, INIT, tick, helpers) and lieu28_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001FDAC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu28_trig.c). */
void l28_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu28_trig.c):
 *   l28_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l28_trig00(void);

#endif /* DKE_LIEU28_INT_H */
