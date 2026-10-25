#ifndef DKE_LIEU10_INT_H
#define DKE_LIEU10_INT_H

/*
 * lieu10_int.h - shared internals of the LIEU10.DLL port, split between
 * lieu10.c (entry, INIT, tick, helpers) and lieu10_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002BE8C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu10_trig.c). */
void l10_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu10_trig.c):
 *   l10_trig00 = trigger 0, sub_10007530 (thunk sub_10001078; INIT default) */
void l10_trig00(void);

#endif /* DKE_LIEU10_INT_H */
