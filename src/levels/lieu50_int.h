#ifndef DKE_LIEU50_INT_H
#define DKE_LIEU50_INT_H

/*
 * lieu50_int.h - shared internals of the LIEU50.DLL port, split between
 * lieu50.c (entry, INIT, tick, events, helpers) and lieu50_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002248C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu50_trig.c). */
void l50_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu50_trig.c):
 *   l50_trig00 = trigger 0, sub_10001200 (thunk 0x1000106E; INIT default) */
void l50_trig00(void);

/* Hero greets persona n (sub_10010876, thunk 0x100010AA): the LIEU50
 * variant of lieu_pre_scene (lieu50.c). */
void l50_pre_scene(i32 hero, i32 n, i32 a1, i32 a2, i32 a3);

#endif /* DKE_LIEU50_INT_H */
