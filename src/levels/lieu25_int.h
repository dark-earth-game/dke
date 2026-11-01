#ifndef DKE_LIEU25_INT_H
#define DKE_LIEU25_INT_H

/*
 * lieu25_int.h - shared internals of the LIEU25.DLL port, split between
 * lieu25.c (entry, INIT, tick, helpers) and lieu25_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001FC8C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu25_trig.c). */
void l25_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu25_trig.c):
 *   l25_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l25_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L25_RYLSADHAR_STANCE_WARNED        0x064  /* 1 once Rylsadhar20 reacted to the drawn weapon (L25D27); cleared when the hero leaves */
#define L25_RYLSADHAR_INTRO_DONE           0x068  /* Arkhan1: 1 once Rylsadhar20 stood up and walked to his post (SP20LEV01), 10 set by trigger 2 */
#define L25_DIALOGUE_VARIANT               0x06C  /* random(3)+1 picks one of three comment variants in trigger 8, then cleared */
#define L25_RYLSADHAR_WATCHING             0x070  /* 1 while Rylsadhar20 looks at the hero, cleared when the hero moves away */
#define L25_DOOR_EVENT1_STEP               0x074  /* exit event 1 chain: 0 start, 1 door open sound, 2 anim played, then change_piece 26 */
#define L25_RYLSADHAR_TALK_STATE           0x078  /* Arkhan2/3 meeting with Rylsadhar20/Kalhi13: 0 not started, 1 start pending, 2 running */
#define L25_KALHI_MEETING_STEP             0x07C  /* Kalhi13/Rylsadhar20 scene step 1..6, 10 finished, 99 busy between steps */
#define L25_KALHI13_STANCE_WARNED          0x080  /* 1 while Kalhi13 reacts to the drawn weapon (L25 trig 3), 0 after sheathing */
#define L25_DOOR_EVENT0_DONE               0x088  /* exit event 0: 0 = first run (opening anim), 1 = second run (change room) */
#define L25_ELAPSED_MS                     0x09C  /* time away from the room (ms); preset 3000 for start point 1, selects Rylsadhar20 position */
#define L25_LEAVE_TIME                     0x0A0  /* ST_GAME_TIME stamped when the hero left */
#define L25_UNUSED_118                     0x118  /* only zeroed at INIT, never read in this level */
#define L25_TRIG8_MESSAGE_SHOWN            0x5D0  /* 1 once the trigger 8 message was shown */
#define L25_ARKHAN2_TALKED                 0x958  /* 1 once Arkhan2 talked with Rylsadhar20 (trigger 2) */
#define L25_ARKHAN23_PART                  0xAF8  /* 0 = Arkhan1 chapter, 1 = Arkhan2/3 chapter started */
#define L25_RYLSADHAR_STAGE                0xB10  /* story stage of the Rylsadhar20 encounter: 1 set at start point, 5 = music B (resolved) */
#define L25_RYLSADHAR_DIALOGUE_COUNT       0xB40  /* < 2 adds the second line (L25D200B/L25D201B) after the fight */
#define L25_KALHI_PART_STEP                0xB4C  /* Arkhan2/3 story step; 3 leaves for variant 1 of room 23 */
#define L25_RYLSADHAR_SCENE_ENDED          0xBB4  /* 1 once the Rylsadhar20 scene ended (hero may move freely) */
#define L25_RYLSADHAR_KALHI_STATE          0xBD0  /* 0 none, 1 Kalhi13 present in the room, 2 meeting finished */
#define L25_RYLSADHAR_GATE_OPENED          0xBE4  /* 1 once the room 26 gate event ran */
#define L25_RYLSADHAR_ALERT                0xD04  /* 1 while Rylsadhar20 watches the hero from close */
#define L25_RYLSADHAR_TALK_STAGE           0xD08  /* Arkhan1 conversation with Rylsadhar20: 0 not started, 1 talked */
#define L25_RYLSADHAR_GREETED              0xD0C  /* 0 = greeting L25D07 not said, 1 = said */
#define L25_OB81_TAKEN                     0xD20  /* 1 once OB81 was picked up (zone 17 removed) */
#define L25_KALHI13_TALK_A                 0xD24  /* Kalhi13 talk stage for Arkhan2 (0..2) */
#define L25_KALHI13_TALK_B                 0xD28  /* Kalhi13 talk stage for Arkhan3 (0..2) */
#define L25_POST_MEETING_SAID              0xE0C  /* 1 once the post-meeting line (L25D200/201) was said */
#define L25_KALHI13_GREETED                0xE58  /* 1 once Kalhi13 greeted the hero on entry */
#define L25_RYLSADHAR_ZED_STAGE            0xE78  /* Arkhan1 story stage: 7 Rylsadhar20 at post, 9 Kalhi13 meeting done, 10 Zed84 scene */
#define L25_CHASE_STATE                   0xE8C  /* Gork/Sordos chase state (set by rooms 31/32): 1 = chase running, INIT then skips the hero-dead ending */
#define L25_ARKHAN2_DIALOGUE_SEEN          0xEB0  /* 1 once the Arkhan2 conversation with Rylsadhar20 started */

#endif /* DKE_LIEU25_INT_H */
