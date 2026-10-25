#ifndef DKE_LIEU21_INT_H
#define DKE_LIEU21_INT_H

/*
 * lieu21_int.h -- shared internals of the LIEU21.DLL port, split between
 * lieu21.c (entry, INIT, tick) and lieu21_trig.c (the zone trigger
 * handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001F66C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu21_trig.c). */
void l21_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu21_trig.c):
 *   l21_trig00 = trigger 0, sub_10001200 (thunk sub_10001073; INIT default) */
void l21_trig00(void);

#endif /* DKE_LIEU21_INT_H */
