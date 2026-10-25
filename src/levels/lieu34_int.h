#ifndef DKE_LIEU34_INT_H
#define DKE_LIEU34_INT_H

/*
 * lieu34_int.h - shared internals of the LIEU34.DLL port, split between
 * lieu34.c (entry, INIT, tick, tick body) and lieu34_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001BA2C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu34_trig.c). */
void l34_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu34_trig.c):
 *   l34_trig00 = trigger 0, trig00_100073F0 (thunk 0x10001073; INIT default) */
void l34_trig00(void);

#endif /* DKE_LIEU34_INT_H */
