#ifndef DKE_LIEU39_INT_H
#define DKE_LIEU39_INT_H

/*
 * lieu39_int.h - shared internals of the LIEU39.DLL port, split between
 * lieu39.c (entry, INIT, tick, helpers) and lieu39_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001E7AC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu39_trig.c). */
void l39_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu39_trig.c):
 *   l39_trig00 = trigger 0, trig00_10001200 (thunk 0x10001073; INIT default) */
void l39_trig00(void);

#endif /* DKE_LIEU39_INT_H */
