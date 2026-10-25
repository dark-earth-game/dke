#ifndef DKE_LIEU23_INT_H
#define DKE_LIEU23_INT_H

/*
 * lieu23_int.h — shared internals of the LIEU23.DLL port, split between
 * lieu23.c (entry, INIT, tick, helpers) and lieu23_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001CCBC). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu23_trig.c). */
void l23_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu23_trig.c):
 *   l23_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l23_trig00(void);

#endif /* DKE_LIEU23_INT_H */
