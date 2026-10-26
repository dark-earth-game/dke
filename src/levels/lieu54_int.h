#ifndef DKE_LIEU54_INT_H
#define DKE_LIEU54_INT_H

/*
 * lieu54_int.h - shared internals of the LIEU54.DLL port, split between
 * lieu54.c (entry, INIT, tick, events, helpers) and lieu54_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10029CAC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu54_trig.c). */
void l54_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu54_trig.c):
 *   l54_trig00 = trigger 0, sub_10001D60 (thunk sub_1000106E; INIT default) */
void l54_trig00(void);

/* Hero greets persona n (sub_10001856, thunk sub_100010AA): the LIEU54
 * variant of lieu_pre_scene (lieu54.c). */
void l54_pre_scene(i32 hero, i32 n, i32 a1, i32 a2, i32 a3);

#endif /* DKE_LIEU54_INT_H */
