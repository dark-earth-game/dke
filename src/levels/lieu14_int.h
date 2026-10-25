#ifndef DKE_LIEU14_INT_H
#define DKE_LIEU14_INT_H

/*
 * lieu14_int.h — shared internals of the LIEU14.DLL port, split between
 * lieu14.c (entry, INIT, tick, helpers) and lieu14_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002BEEC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu14_trig.c). */
void l14_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu14_trig.c):
 *   l14_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l14_trig00(void);

#endif /* DKE_LIEU14_INT_H */
