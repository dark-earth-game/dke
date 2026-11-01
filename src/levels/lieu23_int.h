#ifndef DKE_LIEU23_INT_H
#define DKE_LIEU23_INT_H

/*
 * lieu23_int.h — shared internals of the LIEU23.DLL port, split between
 * lieu23.c (entry, INIT, tick, helpers) and lieu23_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001CCBC). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu23_trig.c). */
void l23_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu23_trig.c):
 *   l23_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l23_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L23_RYLSADHAR_SCENE_STEP           0x064  /* Arkhan1 Rylsadhar20 arrival scene: 0 idle, 1 armed (trigger 41), 4 scene played, 5 done */
#define L23_ZED_MEET_STEP                  0x068  /* Arkhan1 Zed84 meeting: 0 idle, 1 start, 2 Zed walking to the hero (L23D19/L23D20) */
#define L23_PHEDORIA_HIT_DONE              0x06C  /* bumped after the Phedoria hit scene (L23D13); zeroed at INIT */
#define L23_ZED_SCOLD_STEP                 0x070  /* 1 = L23D11 scold pending after the Zed84 dialogue, 2 = said */
#define L23_UNUSED_74                      0x074  /* only zeroed at INIT, never read in this level */
#define L23_PHEDORIA_APPROACH              0x078  /* 1 = Phedoria walking to the hero (Arkhan2/3), cleared when she arrives */
#define L23_UNUSED_7C                      0x07C  /* only zeroed at INIT, never read in this level */
#define L23_UNUSED_80                      0x080  /* only zeroed at INIT, never read in this level */
#define L23_PHEDORIA_TALK_STEP             0x084  /* Phedoria/Kalhi13 scene: 0 first meeting, 1 Kalhi13 present (arrived from room 25), 4..7 hit scene and Kalhi13 arrival */
#define L23_UNUSED_88                      0x088  /* only zeroed at INIT, never read in this level */
#define L23_DOOR25_ANIM_DONE               0x090  /* north door: 0 = opening anim not yet played, 1 = played (second event 0 changes to room 25) */
#define L23_DOOR24_ANIM_DONE               0x094  /* south door: 0 = opening anim not yet played, 1 = played (second event 27 changes to room 24) */
#define L23_UNUSED_98                      0x098  /* only zeroed at INIT, never read in this level */
#define L23_ELAPSED_MS                     0x09C  /* game time spent away from the room (ms); presets 2000/10000 for start points 1/2 */
#define L23_LEAVE_TIME                     0x0A0  /* ST_GAME_TIME stamped when the Rylsadhar20 scene started */
#define L23_GUARDS_ENGAGE_STATE            0x0B8  /* guards 13/14 fight gate: 0 idle, 1 charging the hero, 99 in a dialogue */
#define L23_STRIKE_SEEN                    0x0BC  /* 1 once the player's first strike was registered (delays the guards' reaction event 38) */
#define L23_GUARDS_AMBUSH_ARMED            0x0D4  /* 1 once the guards 13/14 ambush is armed (set by trigger 41) */
#define L23_GUARD_PAIR_STEP_A              0x338  /* guard-pair progress (event 6 slot); 1 once Guardian3/4 pair is beaten (shared with levels 1, 3, 6-11) */
#define L23_GUARD_PAIR_STEP_B              0x348  /* guard-pair progress (event 10 slot); kept equal to GUARD_PAIR_STEP_A */
#define L23_GUARDS1314_STAGE               0x4EC  /* Guardians 13/14 encounter: 0 not started, 1 talked, 2 fighting, 3 both dead */
#define L23_GUARDS1314_TALKING             0x4F0  /* 1 while the guards 13/14 dialogue/ambush runs, 0 otherwise */
#define L23_THANANDAR_STAGE                0xA5C  /* Thanandar24 encounter stage preset at start points 7/9/10 (1 = met) */
#define L23_ARKHAN23_PART                  0xAF8  /* 0 = Arkhan1 chapter, 1 = Arkhan2/3 chapter started (guards 13/14 appear) */
#define L23_OB75_STATE                     0xB10  /* story counter on the OB75 item: engine sets 2 on the OB75 combination; non-zero arms ZED_QUEST_SEEN */
#define L23_KALHI_PART_STEP                0xB4C  /* Arkhan2/3 story step: 0..3 selects the exit target of trigger 1 (room 10 variants / room 51); 3 = OB51 chest present */
#define L23_GUARD34_FIGHT_STATE            0xB98  /* Guardian3/4 fight: 0 none, 2 Guardian4 beaten first, 4 both dead (shared with levels 1, 3, 6-11) */
#define L23_ARKHAN23_STEP                  0xBA8  /* Arkhan2/3 storyline step 0..8 (exit variant of room 24: <2, 2..4, >=5) */
#define L23_PHEDORIA_WITH_HERO             0xBB4  /* 1 = Phedoria accompanies the Arkhan2/3 hero in this room */
#define L23_ZED_QUEST_SEEN                 0xE74  /* set to 1 once OB75_STATE is non-zero */
#define L23_RYLSADHAR_ZED_STAGE            0xE78  /* Arkhan1 story stage: 1 start, 2 Rylsadhar20 waiting, 6 scene done, 7 finished, 10 Zed84 scene */
#define L23_RYLSADHAR_AREA_DONE            0xE7C  /* set to 3 on the first INIT of the Arkhan1 stage (3 = Rylsadhar20 area visited) */
#define L23_ENDING_SHOWN                   0xE8C  /* 1 = skip the hero-dead ending at INIT */
#define L23_OB51_TAKEN                     0xF30  /* 1 once the OB51 chest was searched (zone 13 removed) */

#endif /* DKE_LIEU23_INT_H */
