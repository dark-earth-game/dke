#ifndef DKE_LIEU27_INT_H
#define DKE_LIEU27_INT_H

/*
 * lieu27_int.h - shared internals of the LIEU27.DLL port, split between
 * lieu27.c (entry, INIT, tick, helpers) and lieu27_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001D93C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu27_trig.c). */
void l27_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu27.c):
 *   l27_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l27_trig00(void);

/* DLL .data dword_1001AB34: destination room of the door the hero used
 * (20, 28 or 29); set by the trigger handlers, read by the tick body.
 * Starts at 0 on every room load (reset at the top of INIT). */
extern i32 g_dest_room;

#endif /* DKE_LIEU27_INT_H */
