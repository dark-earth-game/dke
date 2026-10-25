#ifndef DKE_LIEU16_INT_H
#define DKE_LIEU16_INT_H

/*
 * lieu16_int.h — shared internals of the LIEU16.DLL port, split between
 * lieu16.c (entry, INIT, tick, helpers) and lieu16_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10024D9C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu16_trig.c). */
void l16_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu16_trig.c):
 *   l16_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l16_trig00(void);

#endif /* DKE_LIEU16_INT_H */
