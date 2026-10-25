#ifndef DKE_LIEU47_INT_H
#define DKE_LIEU47_INT_H

/*
 * lieu47_int.h - shared internals of the LIEU47.DLL port, split between
 * lieu47.c (entry, INIT, tick, events, helpers) and lieu47_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10019B0C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu47_trig.c). */
void l47_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu47_trig.c):
 *   l47_trig00 = trigger 0, sub_10007183 (thunk sub_10001073; INIT default) */
void l47_trig00(void);

#endif /* DKE_LIEU47_INT_H */
