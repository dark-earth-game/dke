#ifndef DKE_LIEU06_INT_H
#define DKE_LIEU06_INT_H

/*
 * lieu06_int.h — shared internals of the LIEU06.DLL port, split between
 * lieu06.c (entry, INIT, tick, helpers) and lieu06_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002192C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu06_trig.c). */
void l06_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu06_trig.c):
 *   l06_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l06_trig00(void);

/* Dhorkan's taunt line cycle (lieu06.c), inlined by the DLL at each site. */
void l06_taunt_a(void);
void l06_taunt_b(void);

#endif /* DKE_LIEU06_INT_H */
