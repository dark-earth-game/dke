#ifndef DKE_LIEU26_INT_H
#define DKE_LIEU26_INT_H

/*
 * lieu26_int.h - shared internals of the LIEU26.DLL port, split between
 * lieu26.c (entry, INIT, tick, helpers) and lieu26_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001970C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu26_trig.c). */
void l26_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu26_trig.c):
 *   l26_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l26_trig00(void);

#endif /* DKE_LIEU26_INT_H */
