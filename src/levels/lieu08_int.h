#ifndef DKE_LIEU08_INT_H
#define DKE_LIEU08_INT_H

/*
 * lieu08_int.h - shared internals of the LIEU08.DLL port, split between
 * lieu08.c (entry, INIT, tick, helpers) and lieu08_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1003FC5C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu08_trig.c). */
void l08_trigger(i32 idx);

/* Trigger handlers the core also calls directly (defined in lieu08_trig.c):
 *   l08_trig00 = trigger 0, sub_10001200 (thunk sub_10001073; INIT default)
 *   l08_trig08 = trigger 8, sub_10012CE2 (the tick starts it) */
void l08_trig00(void);
void l08_trig08(void);

#endif /* DKE_LIEU08_INT_H */
