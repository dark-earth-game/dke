#ifndef DKE_LIEU33_INT_H
#define DKE_LIEU33_INT_H

/*
 * lieu33_int.h - shared internals of the LIEU33.DLL port, split between
 * lieu33.c (entry, INIT, tick, events, helpers) and lieu33_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10025A08. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu33_trig.c). */
void l33_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu33_trig.c):
 *   l33_trig00 = trigger 0, sub_10005FB0 (thunk sub_10001073; INIT default) */
void l33_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L33_FRAME_COUNTER                  0x038  /* engine frame counter, used modulo 20 to re-aim Kalhi at the hero */
#define L33_GUARDIAN15_ESCORT              0x04C  /* 1 while Guardian15 escorts the hero (cleared when it ends) */
#define L33_GUARDIAN15_FOLLOW_DONE         0x050  /* 1 once the Guardian15 follow-up was done */
#define L33_ZONE_TRIGGER1_HIT              0x064  /* 1 once trigger 1 was entered (Zed84 meeting condition) */
#define L33_UNUSED_68                      0x068  /* only zeroed at INIT, never read in this level */
#define L33_KALHI_GUIDE_ANIM               0x06C  /* Kalhi guiding anim state: 1 walking, 10 waiting at the stop, 100 leaving */
#define L33_RYLSADHAR_WALKING              0x070  /* 1 while Rylsadhar20 walks to the hero, cleared on arrival */
#define L33_IDLE_HINT_ARMED                0x074  /* 1 while Kalhi13 may give an idle hint (after 10 s idle) */
#define L33_KALHI_AT_ZONE                  0x078  /* 1 while the hero stands at the Kalhi stop (zones 31/32), debounce */
#define L33_ZED_MEET_STEP                  0x07C  /* Zed84 meeting: 1 Zed84 placed, 2 hero close and facing */
#define L33_KALHI_STOP_STAGE               0x080  /* Kalhi stop: 0 not reached, 1 reached, 2 passed */
#define L33_KALHI_GATE_STAGE               0x084  /* 1 = Kalhi waits at the gate, 2 = gate scene played */
#define L33_UNUSED_88                      0x088  /* only zeroed at INIT, never read in this level */
#define L33_CAMERA3_LOCKED                 0x090  /* 1 while camera 3 or 7 is locked */
#define L33_KALHI_GUIDE_STEP               0x094  /* Kalhi escort step 0..10 (7 at the stop, 8 walking to Guardian15, 9 near, 10 done) */
#define L33_ZONE31_ONCE                    0x098  /* trigger 31: 0 first, 1 repeated */
#define L33_RYLSADHAR_TIME                 0x0A0  /* ST_GAME_TIME stamped when Rylsadhar20 appeared */
#define L33_LORY_TIME                      0x0A8  /* ST_GAME_TIME stamped when Lory appeared */
#define L33_SAVED_CAMERA                   0x0AC  /* locked camera saved before a scene, restored afterwards (0 = none) */
#define L33_UNUSED_B8                      0x0B8  /* only zeroed at INIT, never read in this level */
#define L33_UNUSED_BC                      0x0BC  /* only zeroed at INIT, never read in this level */
#define L33_SUNSEER1_WALK_STEP             0x0D4  /* Sunseer1 walk: 1 start, 2 walking, 3 arrived */
#define L33_TRIG_D8_FLAG                   0x0D8  /* set to 1 by INIT when its condition holds; zeroed otherwise */
#define L33_CAMERA_RESTORE_STATE           0x0DC  /* 1 start, 2 waiting for the camera to change back */
#define L33_KALHI_REACTION                 0x0E0  /* Kalhi reaction to a hit: 0 calm, 1..4 warned, 5 hostile */
#define L33_ZONE_ONCE_F0                   0x0F0  /* 1 once the trigger 1/2 Kalhi scene was triggered */
#define L33_KALHI_APPEAR                   0x0F4  /* 1 pending, 2 done (Kalhi appearance scene) */
#define L33_RYLSADHAR_SPAWNED              0x0F8  /* 1 once Rylsadhar20 was placed at the zone 27 marker */
#define L33_UNUSED_FC                      0x0FC  /* only zeroed at INIT, never read in this level */
#define L33_GUARDIAN15_SCENE_DONE          0x100  /* 1 once the Guardian15 aftermath scene ran */
#define L33_UNUSED_114                     0x114  /* only zeroed at INIT, never read in this level */
#define L33_KALHI_GUIDE_ANIM_SAVED         0x12C  /* copy of KALHI_GUIDE_ANIM kept across room visits */
#define L33_GUARD_PAIR_STEP_A              0x338  /* guard-pair progress (event 6 slot, see L23) */
#define L33_GUARD_PAIR_STEP_B              0x348  /* guard-pair progress (event 10 slot, see L23) */
#define L33_ZONE34_SEEN                    0x4E8  /* 1 once the trigger 34 object was inspected */
#define L33_PROGRESS_6DC                   0x6DC  /* 4 -> 5 once B40 == 4 (Kalhi door progress) */
#define L33_ZONE12_STAGE_A                 0x7D4  /* trigger 12: 0 first, 1, 2 */
#define L33_ZONE12_STAGE_B                 0x7F4  /* trigger 12: 0 first, 1, 2 */
#define L33_ARRIVAL_FLAG                   0x7F8  /* 0 = first tick, then 1 (selects Kalhi position) */
#define L33_ZONE31_STAGE                   0x7FC  /* trigger 31: 0 first, 1 repeated */
#define L33_KALHI_HINT_COUNT               0x948  /* idle-hint line counter 0..2 */
#define L33_STORY_MARK_A84                 0xA84  /* set to 3 by trigger 31 */
#define L33_ARKHAN1_PROGRESS               0xAF4  /* Arkhan1 exploration: 1 started, 2 near zone 19, 3 Zed84 placed */
#define L33_ARKHAN23_PART                  0xAF8  /* 0 = Arkhan1 chapter, 1 = Arkhan2/3 chapter started */
#define L33_HERO_MEETING_STATE             0xB40  /* 0 none, 4 .., 99 meeting finished (pairs with BD0) */
#define L33_KALHI_PART_STEP                0xB4C  /* Arkhan2/3 story step; 2 set by the room 51 cinematic SC15 */
#define L33_ARKHAN23_SUBSTEP               0xB54  /* Arkhan2/3 sub-step: 3 set at start points, 4 = hero came back */
#define L33_ARKHAN23_STEP                  0xBA8  /* Arkhan2/3 storyline step (4 -> 5) */
#define L33_HERO_MEETING_STATE_B           0xBD0  /* pairs with B40; 99 = finished, 2 set by trigger 6 */
#define L33_RYLSADHAR_GATE_OPENED          0xBE4  /* room 26 gate event done (set in room 25) */
#define L33_STORY_MARK_C08                 0xC08  /* 3 -> 10 progress mark */
#define L33_DELIA_SCENE_DONE               0xE48  /* 1 once the Delia scene at triggers 1/2 ran */
#define L33_KALHI_FOLLOW_MODE              0xE58  /* 0 none, 1 guide, 2 following, 10 hostile */
#define L33_RYLSADHAR_ZED_STAGE            0xE78  /* 1 once Rylsadhar20 appeared (Arkhan1 stage; see L23) */
#define L33_LORY_APPEARED                  0xE7C  /* 1 once Lory appeared at the zone 20 marker */
#define L33_KALHI_ESCORT_PHASE             0xE9C  /* Kalhi escort: 7 guiding, 8 waiting at the gate, 10 aborted/hostile (same encoding as V_DELIA_PHASE) */
#define L33_ESCORT_EVENT_B8                0xEB8  /* 2 = escort event finished (blocks the Guardian15 approach) */
#define L33_ZED84_TALK_STAGE               0xEC8  /* Zed84 talk at the Arkhan1 start: 0 none, 1 ready, 2 talking, 3 finished */
#define L33_KALHI_SEEN                     0xEF0  /* 1 once Kalhi was seen guiding */
#define L33_GUARDIAN15_STATE               0xEF4  /* 0 fresh, 2 beaten, 3 engaged by the escort */
#define L33_ZONE_SCENE_F14                 0xF14  /* 1 pending, 2 done */
#define L33_DELIA_KILLED_MARK              0xF20  /* 1 when Delia was killed in room 8/10 (blocks the Delia scene) */

#endif /* DKE_LIEU33_INT_H */
