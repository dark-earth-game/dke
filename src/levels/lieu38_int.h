#ifndef DKE_LIEU38_INT_H
#define DKE_LIEU38_INT_H

/*
 * lieu38_int.h - shared internals of the LIEU38.DLL port, split between
 * lieu38.c (entry, INIT, tick, helpers) and lieu38_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001BE4C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu38_trig.c). */
void l38_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu38_trig.c):
 *   l38_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l38_trig00(void);

#endif /* DKE_LIEU38_INT_H */
