#ifndef DKE_LIEU20_INT_H
#define DKE_LIEU20_INT_H

/*
 * lieu20_int.h — shared internals of the LIEU20.DLL port, split between
 * lieu20.c (entry, INIT, tick, helpers) and lieu20_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002409C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu20_trig.c). */
void l20_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu20_trig.c):
 *   l20_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l20_trig00(void);

#endif /* DKE_LIEU20_INT_H */
