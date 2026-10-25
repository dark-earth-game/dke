#ifndef DKE_LIEU18_INT_H
#define DKE_LIEU18_INT_H

/*
 * lieu18_int.h — shared internals of the LIEU18.DLL port, split between
 * lieu18.c (entry, INIT, tick, helpers) and lieu18_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002AD5C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu18_trig.c). */
void l18_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu18_trig.c):
 *   l18_trig00 = trigger 0, sub_10007400 (thunk sub_10001078; INIT default) */
void l18_trig00(void);

/* sub_10013756 (thunk sub_10001019): the "PRE80" weapon-warning scene of
 * persona sp with the hero-side clips of g. Nothing in the DLL calls it. */
void l18_pre80_scene(i32 g, i32 sp);

#endif /* DKE_LIEU18_INT_H */
