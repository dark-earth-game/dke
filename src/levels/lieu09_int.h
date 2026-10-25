#ifndef DKE_LIEU09_INT_H
#define DKE_LIEU09_INT_H

/*
 * lieu09_int.h — shared internals of the LIEU09.DLL port, split between
 * lieu09.c (entry, INIT, tick, helpers) and lieu09_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001A78C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu09_trig.c). */
void l09_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu09_trig.c):
 *   l09_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l09_trig00(void);

#endif /* DKE_LIEU09_INT_H */
