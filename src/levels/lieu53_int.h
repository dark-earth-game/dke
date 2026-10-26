#ifndef DKE_LIEU53_INT_H
#define DKE_LIEU53_INT_H

/*
 * lieu53_int.h - shared internals of the LIEU53.DLL port, split between
 * lieu53.c (entry, INIT, tick, events, helpers) and lieu53_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001990C. */

/* Resets the DLL .data globals (the DLL is loaded afresh for every room). */
void l53_reset_dll_globals(void);

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu53_trig.c). */
void l53_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu53_trig.c):
 *   l53_trig00 = trigger 0, sub_100065C0 (thunk sub_1000106E; INIT default) */
void l53_trig00(void);

#endif /* DKE_LIEU53_INT_H */
