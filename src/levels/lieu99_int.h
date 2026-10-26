#ifndef DKE_LIEU99_INT_H
#define DKE_LIEU99_INT_H

/*
 * lieu99_int.h — shared internals of the LIEU99.DLL port, split between
 * lieu99.c (entry, INIT, tick, helpers) and lieu99_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_100183DC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu99_trig.c). */
void l99_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu99_trig.c):
 *   l99_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l99_trig00(void);

#endif /* DKE_LIEU99_INT_H */
