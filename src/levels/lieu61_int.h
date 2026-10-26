#ifndef DKE_LIEU61_INT_H
#define DKE_LIEU61_INT_H

/*
 * lieu61_int.h - shared internals of the LIEU61.DLL port, split between
 * lieu61.c (entry, INIT, tick) and lieu61_trig.c (the zone trigger
 * handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1000721C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu61_trig.c). */
void l61_trigger(i32 idx);

/* Trigger handler the entry also calls directly (defined in lieu61_trig.c):
 *   l61_trig00 = trigger 0, sub_10001206 (thunk sub_10001064; INIT default) */
void l61_trig00(void);

#endif /* DKE_LIEU61_INT_H */
