#ifndef DKE_LIEU49_INT_H
#define DKE_LIEU49_INT_H

/*
 * lieu49_int.h — shared internals of the LIEU49.DLL port, split between
 * lieu49.c (entry, INIT, tick, tick body) and lieu49_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001BC3C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu49_trig.c). */
void l49_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu49_trig.c):
 *   l49_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l49_trig00(void);

#endif /* DKE_LIEU49_INT_H */
