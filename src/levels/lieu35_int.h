#ifndef DKE_LIEU35_INT_H
#define DKE_LIEU35_INT_H

/*
 * lieu35_int.h - shared internals of the LIEU35.DLL port, split between
 * lieu35.c (entry, INIT, tick, helpers) and lieu35_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001B88C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu35_trig.c). */
void l35_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu35_trig.c):
 *   l35_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l35_trig00(void);

#endif /* DKE_LIEU35_INT_H */
