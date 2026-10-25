#ifndef DKE_LIEU04_INT_H
#define DKE_LIEU04_INT_H

/*
 * lieu04_int.h — shared internals of the LIEU04.DLL port, split between
 * lieu04.c (entry, INIT, tick, helpers) and lieu04_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10022A8C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu04_trig.c). */
void l04_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu04_trig.c):
 *   l04_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l04_trig00(void);

#endif /* DKE_LIEU04_INT_H */
