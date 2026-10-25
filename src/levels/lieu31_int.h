#ifndef DKE_LIEU31_INT_H
#define DKE_LIEU31_INT_H

/*
 * lieu31_int.h - shared internals of the LIEU31.DLL port, split between
 * lieu31.c (entry, INIT, tick, helpers) and lieu31_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x10021C8C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu31_trig.c). */
void l31_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu31_trig.c):
 *   l31_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l31_trig00(void);

#endif /* DKE_LIEU31_INT_H */
