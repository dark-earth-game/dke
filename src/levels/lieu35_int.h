#ifndef DKE_LIEU35_INT_H
#define DKE_LIEU35_INT_H

/*
 * lieu35_int.h - shared internals of the LIEU35.DLL port, split between
 * lieu35.c (entry, INIT, tick, helpers) and lieu35_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* Script variables of this level (state block slots; low slots are per-level scratch). */
#define L35_GUARD92_ENGAGED              0x64   /* 1 once the guard (persona 92) has been put into combat (set when hero is near zone 20/40 under locked camera 2/3); trig0 clears */
#define L35_DEATH_SCENE_STARTED          0x68   /* 1 once the timeout/drowning death scene (SP0xNOY01) has begun; trig0 clears */
#define L35_UNUSED_CLEARED               0x6C   /* cleared by trig0, never read in this level */
#define L35_LAST_LIVE_HERO               0xE8   /* last hero id that was not 4 (dead); restored for DV_HP when the end-game request fires */
#define L35_INTRO_DIALOGUE_DONE          0x4D8   /* 0 = arrival dialogue (L35D01/L35D02) not played yet, 1 = played */
#define L35_MALROT6_SEARCHED             0x4E0   /* 0 = Malrot6 body not searched yet, 1 = searched (L35D03/L35D04) */
#define L35_OB72_EXAMINED                0x698   /* 0 = first examine (SG00PRE08, L35D21/22, OB72 state -3), 1 = repeat examine (SG00PRE09, L35D19/20) */
#define L35_GUARD92_SEEN                 0x908   /* 0 = guard 92 not triggered yet (parked far away at z 75600), 1 = triggered, placed at z 6903/7560 */
#define L35_MALROT_STAGE                 0xC28   /* Malrot6 / hostile stage: 0 or 10 idle, 2 start-point preset, 3 Malrot6 already placed with OB90 (shared with other levels) */
#define L35_DEATH_ENDING_BYPASS          0xE8C   /* 1 = hero death does not play the death ending in INIT (runs trigger 0 instead); shared flag */
#define L35_STORY_STAGE                  0xF24   /* story stage; 3 forces Malrot6 present (shared with other levels) */
#define L35_TIMER2_MS                    0xF40   /* 1200000 ms (20 min) countdown value seeded by trigger 0, next to V_HUD_TIMER_MS (600000) */

/* ST() reads g_lvl_S, the DLL's dword_1001B88C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu35_trig.c). */
void l35_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu35_trig.c):
 *   l35_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l35_trig00(void);

#endif /* DKE_LIEU35_INT_H */
