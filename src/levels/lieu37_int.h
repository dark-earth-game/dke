#ifndef DKE_LIEU37_INT_H
#define DKE_LIEU37_INT_H

/*
 * lieu37_int.h — shared internals of the LIEU37.DLL port, split between
 * lieu37.c (entry, INIT, tick, tick body) and lieu37_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001B8EC. */

/* Level-local script variables (scratch slots reused per level; 0x100+ are shared with other levels) */
#define L37_FLOOR                    0x64   /* which floor the hero is on: 1 = upper (stairs trigger 3 goes down), 2 = lower (trigger 4 goes up) */
#define L37_UNUSED_68                0x68   /* reset to 0 at INIT, never read in this DLL */
#define L37_UNUSED_6C                0x6C   /* reset to 0 at INIT, never read in this DLL */
#define L37_TRUCE_39                 0x70   /* persona 39 pacified: 1 or 2 = talks instead of fighting (never set by the DLL, reset to 0 at INIT); 2 adds L37D23/D24 */
#define L37_UNUSED_74                0x74   /* reset to 0 at INIT, never read in this DLL */
#define L37_UNUSED_78                0x78   /* reset to 0 at INIT, never read in this DLL */
#define L37_TRUCE_38                 0x7C   /* persona 38 pacified: 2 = talks instead of fighting (never set by the DLL, reset to 0 at INIT); adds L37D13B/D15B */
#define L37_EXIT_STAGE               0x80   /* exit-to-room-35 sequence: 0 = idle, 1 = exit dialogue started, 2 = fighters re-engaged, next use changes room */
#define L37_EXIT_JOIN                0x84   /* 2 = hero is being walked to the exit by joindre; tick ends it when the walk is over */
#define L37_UNUSED_88                0x88   /* reset to 0 at INIT, never read in this DLL */
#define L37_P39_AGGRO                0x90   /* persona 39 attack trigger: 0 = waiting, 2 = attack started (tick sets L37_FIGHT_STATE = 1) */
#define L37_SOB86_ENTRY_DONE         0x94   /* 1 = trigger 1 already took the V_SOB86_ACTIVE exit to room 35, 0 = not yet */
#define L37_P39_HIT_STATE            0x100   /* first hit on persona 39 while still calm: 0 = not hit, 99 = hit being processed, then 1 (lets the aggro start with no warning line) */
#define L37_INTRO_DONE               0x550   /* 1 = trigger 41 intro dialogue (L37D01/L37D02) played; reset to 0 on entry from room 35 */
#define L37_FAR_DIALOGUE_SEEN        0x554   /* 1 = hero passed z > 15940, L37D05/L37D06 said */
#define L37_CAM1_HINT_SEEN           0x558   /* 1 = camera-1 hint L37D07/L37D08 said */
#define L37_CAM3_HINT_SEEN           0x55C   /* 1 = camera-3 hint L37D09/L37D10 said */
#define L37_ZONE13_EXAMINED          0x624   /* 0 = zone 13 not yet examined (plays SG00PRE08 look anim), 1 = examined (short lines L37D33/L37D34) */
#define L37_DELIA_GONE               0xC44   /* set to 1 at INIT when Delia is dead or V_DELIA_PHASE >= 10; shared with levels 14 and 54 */
#define L37_FIGHT_STATE              0xE2C   /* persona 38/39 fight: 0 = not started, 1 = fighting, 2 = both dead */
#define L37_DEATH_ENDING_SKIP        0xE8C   /* engine flag: 1 = INIT skips the hero-dead ending even if ST_HERO == 4 */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu37_trig.c). */
void l37_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu37_trig.c):
 *   l37_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l37_trig00(void);

#endif /* DKE_LIEU37_INT_H */
