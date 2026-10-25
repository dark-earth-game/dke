#ifndef DKE_LIEU12_INT_H
#define DKE_LIEU12_INT_H

/*
 * lieu12_int.h — shared internals of the LIEU12.DLL port, split between
 * lieu12.c (entry, INIT, tick, helpers) and lieu12_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_100245BC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu12_trig.c). */
void l12_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu12_trig.c):
 *   l12_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l12_trig00(void);

#endif /* DKE_LIEU12_INT_H */
