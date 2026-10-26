#ifndef DKE_LIEU50_INT_H
#define DKE_LIEU50_INT_H

/*
 * lieu50_int.h - shared internals of the LIEU50.DLL port, split between
 * lieu50.c (entry, INIT, tick, events, helpers) and lieu50_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002248C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu50_trig.c). */
void l50_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu50_trig.c):
 *   l50_trig00 = trigger 0, sub_10001200 (thunk 0x1000106E; INIT default) */
void l50_trig00(void);

#endif /* DKE_LIEU50_INT_H */
