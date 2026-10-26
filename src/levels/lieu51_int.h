#ifndef DKE_LIEU51_INT_H
#define DKE_LIEU51_INT_H

/*
 * lieu51_int.h — shared internals of the LIEU51.DLL port, split between
 * lieu51.c (entry, INIT, tick, tick body) and lieu51_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001A81C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu51_trig.c). */
void l51_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu51_trig.c):
 *   l51_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l51_trig00(void);

#endif /* DKE_LIEU51_INT_H */
