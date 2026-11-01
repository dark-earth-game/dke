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
#define L09_EVENT6_DONE     0x338   /* event 6 (guardian-4 encounter) fired flag; 1 = already played (shared with L1, L3, L6-L8, L10, L11...); INIT sets 1 together with L09_EVENT10_DONE */
#define L09_EVENT10_DONE    0x348   /* event 10 fired flag; set 1 together with L09_EVENT6_DONE when the arc is past (shared across levels) */
#define L09_GUARD_ARC_PHASE 0xAF8   /* guardian-arc stage; 0 = Arkhan1 before the guardian challenge, 1 = after it (start point 2 sets 1 with V_GUARD_FIGHT_DONE=2); shared L1-L11,L14... */
#define L09_GUARDIAN4_STAGE 0xB98   /* guardian-4 encounter stage 0..5; 2 = pending (reset to 0 while guardian 4 alive), 4 = encounter finished; shared across levels */
#define L09_GUARDIAN8_STAGE 0xC08   /* guardian 8 fight; 0 = not started, 2 = Guardian8 present and fighting, 3 = he was killed */
#define L09_GATE_STATE      0xE44   /* gate OB33C/OB33D opened with the Bogd key; 0 = closed, 1 = first leaf open (OB33C), 2 = fully open (OB33D) */
#define L09_ENDING_OVERRIDE 0xE8C   /* game-wide; 1 = skip the hero-death ending in the tick and re-run INIT (set to 1 in L31 near the end) */

#endif /* DKE_LIEU09_INT_H */
