#ifndef DKE_LIEU32_INT_H
#define DKE_LIEU32_INT_H

/*
 * lieu32_int.h - shared internals of the LIEU32.DLL port, split between
 * lieu32.c (entry, INIT, tick, helpers) and lieu32_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_100219FC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu32_trig.c). */
void l32_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu32_trig.c):
 *   l32_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l32_trig00(void);

#endif /* DKE_LIEU32_INT_H */
