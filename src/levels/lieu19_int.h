#ifndef DKE_LIEU19_INT_H
#define DKE_LIEU19_INT_H

/*
 * lieu19_int.h -- shared internals of the LIEU19.DLL port, split between
 * lieu19.c (entry, INIT, tick) and lieu19_trig.c (the zone trigger
 * handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001E80C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu19_trig.c). */
void l19_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu19_trig.c):
 *   l19_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l19_trig00(void);

#endif /* DKE_LIEU19_INT_H */
