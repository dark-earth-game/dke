#ifndef DKE_LIEU05_INT_H
#define DKE_LIEU05_INT_H

/*
 * lieu05_int.h — shared internals of the LIEU05.DLL port, split between
 * lieu05.c (entry, INIT, tick, helpers) and lieu05_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002512C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu05_trig.c). */
void l05_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu05_trig.c):
 *   l05_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l05_trig00(void);

#endif /* DKE_LIEU05_INT_H */
