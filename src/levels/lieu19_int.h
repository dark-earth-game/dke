#ifndef DKE_LIEU19_INT_H
#define DKE_LIEU19_INT_H

/*
 * lieu19_int.h -- shared internals of the LIEU19.DLL port, split between
 * lieu19.c (entry, INIT, tick) and lieu19_trig.c (the zone trigger
 * handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001E80C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu19_trig.c). */
void l19_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu19_trig.c):
 *   l19_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l19_trig00(void);

/* Level 19 script variables (scratch slots 0x64..0x118 are per level) */
#define L19_TICK_COUNTER                   0x038   /* engine frame/tick counter; Delia's follow logic runs every 20th tick */
#define L19_MALROT_WATCHING_A2             0x064   /* Arkhan2: 1 = Malrot5 turned to watch the drawn weapon (L19D94), 0 = relaxed again */
#define L19_KALHI_SCENE_STARTED            0x068   /* 1 once the Kalhi13/Malrot5 intro scene (L19D01/02) started */
#define L19_KALHI_LEAVING                  0x06C   /* 1 while Kalhi13 walks off (set by event 1), cleared on arrival when L19_KALHI_MALROT_SCENE becomes 6 */
#define L19_UNUSED_070                     0x070   /* reset at INIT, never used */
#define L19_UNUSED_074                     0x074   /* reset at INIT, never used */
#define L19_DELIA_FOLLOWING                0x078   /* 1 = Delia (phase 3) follows the hero; re-run every 20th tick */
#define L19_DELIA_CLOSE                    0x07C   /* 1 = Delia stopped next to the hero (within 100), 0 = she must catch up */
#define L19_MALROT_CRY_STATE               0x080   /* shout at Malrot5: 65 = armed, 99 = answered (L19D88) */
#define L19_DEATH_EVENT_SENT               0x084   /* hero dead: game-over event 44 already queued */
#define L19_DELIA_CRY_STATE                0x088   /* shout at Delia: 10 = armed, 99 = answered (L16D122) */
#define L19_MALROT_FIGHT_STARTED_A3        0x090   /* Arkhan3: 1 once Malrot5 turned hostile after the warning */
#define L19_UNUSED_118                     0x118   /* reset at INIT, never used */
#define L19_MALROT_HOSTILE                 0x4D0   /* 1 = Malrot5 is in combat with the hero (restored at INIT); cleared when he dies */
#define L19_ENTRY_STEP                     0x4DC   /* 1 or 3 on entry makes INIT set L19_FLAG_0F24 (if 0F78 == 0) and step to 2 (also used by L18, L30, L49) */
#define L19_MALROT_A3_STEP                 0x578   /* Arkhan3 meeting with Malrot5: 0 not met, 1 he approached/watches, 2 talked */
#define L19_OB42_EXAMINED                  0x620   /* 0 = first look at OB42, 1 = examined */
#define L19_STORY_ADVANCED                 0xAF8   /* 0 = early story, 1 = set by later start points (tested 0/1 in L01-L03 too) */
#define L19_KALHI_MALROT_SCENE             0xBBC   /* Kalhi13/Malrot5 scene: 5 = running, 6 = over (Kalhi13 left), 10 = Kalhi13 died */
#define L19_ARMAL_STAGE                    0xBC8   /* Armal story stage 0..2 (L18 sets it); >=2 changes Malrot5 dialogue */
#define L19_MALROT_PLACE                   0xBF0   /* Malrot5 placement stage: 0 = east spot (4052,3033), 2 = west spot (3030,3123); also read by L16 */
#define L19_TALK_MALROT_A2                 0xD4C   /* Arkhan2 conversation stage with Malrot5, 0..6 */
#define L19_TALK_MALROT_A2_B               0xD58   /* Arkhan2 second conversation with Malrot5, 0..2 */
#define L19_TALK_MALROT_A3                 0xDF8   /* Arkhan3 conversation stage with Malrot5, 0..4 */
#define L19_TALK_MALROT_A3_B               0xDFC   /* Arkhan3 second conversation with Malrot5, 0..2 */
#define L19_FLAG_0E3C                      0xE3C   /* story flag set by start point 2 and rooms 4, 5, 24, 43, 52, 54 */
#define L19_DEATH_ENDING_DONE              0xE8C   /* 1 = death ending already played, INIT runs the normal setup (same slot in every level) */
#define L19_KONKALITE_STATE                0xE98   /* Konkalite progress: 60 = Konkalite2 body, 65 = resolved (written 65 here; L30 sets it) */
#define L19_FLAG_0F24                      0xF24   /* flag raised when Dhorkan11 dies (L14) or entry step fires, if L19_FLAG_0F78 == 0 */
#define L19_FLAG_0F78                      0xF78   /* flag that blocks L19_FLAG_0F24 when set */

#endif /* DKE_LIEU19_INT_H */
