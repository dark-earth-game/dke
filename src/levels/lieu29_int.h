#ifndef DKE_LIEU29_INT_H
#define DKE_LIEU29_INT_H

/*
 * lieu29_int.h - shared internals of the LIEU29.DLL port, split between
 * lieu29.c (entry, INIT, tick, helpers) and lieu29_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001C4DC). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu29_trig.c). */
void l29_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu29_trig.c):
 *   l29_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l29_trig00(void);

#endif /* DKE_LIEU29_INT_H */
