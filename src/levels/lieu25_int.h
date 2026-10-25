#ifndef DKE_LIEU25_INT_H
#define DKE_LIEU25_INT_H

/*
 * lieu25_int.h - shared internals of the LIEU25.DLL port, split between
 * lieu25.c (entry, INIT, tick, helpers) and lieu25_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001FC8C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu25_trig.c). */
void l25_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu25_trig.c):
 *   l25_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l25_trig00(void);

#endif /* DKE_LIEU25_INT_H */
