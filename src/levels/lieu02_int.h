#ifndef DKE_LIEU02_INT_H
#define DKE_LIEU02_INT_H

/*
 * lieu02_int.h — shared internals of the LIEU02.DLL port, split between
 * lieu02.c (entry, INIT, tick, events, helpers) and lieu02_trig.c (the
 * zone trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002C98C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu02_trig.c). */
void l02_trigger(i32 idx);

/* Trigger handlers the core also calls directly (defined in
 * lieu02_trig.c):
 *   l02_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default)
 *   l02_trig31 = trigger 31, sub_100149FB (thunk sub_10001000; tick body) */
void l02_trig00(void);
void l02_trig31(void);

#endif /* DKE_LIEU02_INT_H */
