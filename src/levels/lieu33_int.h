#ifndef DKE_LIEU33_INT_H
#define DKE_LIEU33_INT_H

/*
 * lieu33_int.h - shared internals of the LIEU33.DLL port, split between
 * lieu33.c (entry, INIT, tick, events, helpers) and lieu33_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10025A08. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu33_trig.c). */
void l33_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu33_trig.c):
 *   l33_trig00 = trigger 0, sub_10005FB0 (thunk sub_10001073; INIT default) */
void l33_trig00(void);

#endif /* DKE_LIEU33_INT_H */
