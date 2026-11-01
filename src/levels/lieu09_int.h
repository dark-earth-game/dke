#ifndef DKE_LIEU09_INT_H
#define DKE_LIEU09_INT_H

/*
 * lieu09_int.h — shared internals of the LIEU09.DLL port, split between
 * lieu09.c (entry, INIT, tick, helpers) and lieu09_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001A78C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu09_trig.c). */
void l09_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu09_trig.c):
 *   l09_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l09_trig00(void);

/* Level 9 script variables (the low slots are scratch private to this level; 0x338.. are game-wide story flags) */
#define L09_UNUSED_64       0x064   /* only zeroed by the INIT trigger; never read in this level */
#define L09_UNUSED_68       0x068   /* only zeroed by the INIT trigger; never read in this level */
#define L09_UNUSED_6C       0x06C   /* only zeroed by the INIT trigger; never read in this level */
#define L09_UNUSED_88       0x088   /* only zeroed by the INIT trigger; never read in this level */
#define L09_GATE_STATE      0xE44   /* gate OB33C/OB33D opened with the Bogd key; 0 = closed, 1 = first leaf open (OB33C), 2 = fully open (OB33D) */

#endif /* DKE_LIEU09_INT_H */
