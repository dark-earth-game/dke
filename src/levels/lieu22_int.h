#ifndef DKE_LIEU22_INT_H
#define DKE_LIEU22_INT_H

/*
 * lieu22_int.h — shared internals of the LIEU22.DLL port, split between
 * lieu22.c (entry, INIT, tick, helpers) and lieu22_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001A93C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu22_trig.c). */
void l22_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu22_trig.c):
 *   l22_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l22_trig00(void);

#endif /* DKE_LIEU22_INT_H */
