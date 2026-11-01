#ifndef DKE_LIEU31_INT_H
#define DKE_LIEU31_INT_H

/*
 * lieu31_int.h - shared internals of the LIEU31.DLL port, split between
 * lieu31.c (entry, INIT, tick, helpers) and lieu31_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x10021C8C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu31_trig.c). */
void l31_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu31_trig.c):
 *   l31_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l31_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L31_KONKALITE8_AMBUSH              0x064  /* 0 idle, 1 ambush started, 2 Konkalite8 chasing, 3 over */
#define L31_KONKALITE10_AMBUSH             0x068  /* 1 once the Konkalite10 ambush at trigger 3 ran; 2 = over */
#define L31_KONKALITE_RETREAT_WHO          0x06C  /* 0 none, 78 Konkalite8 retreating, 80 Konkalite10 retreating */
#define L31_BORK_APPROACH_STEP             0x070  /* Bork encounter: 0 idle, 1 camera locked on him, 2 talking */
#define L31_UNUSED_74                      0x074  /* only zeroed at INIT, never read in this level */
#define L31_UNUSED_78                      0x078  /* only zeroed at INIT, never read in this level */
#define L31_UNUSED_7C                      0x07C  /* only zeroed at INIT, never read in this level */
#define L31_UNUSED_80                      0x080  /* only zeroed at INIT, never read in this level */
#define L31_UNUSED_84                      0x084  /* only zeroed at INIT, never read in this level */
#define L31_DEATH_HANDLED                  0x088  /* 1 once the hero-dead check ran in the tick */
#define L31_UNUSED_B8                      0x0B8  /* only zeroed at INIT, never read in this level */
#define L31_UNUSED_BC                      0x0BC  /* only zeroed at INIT, never read in this level */
#define L31_UNUSED_D4                      0x0D4  /* only zeroed at INIT, never read in this level */
#define L31_AMBUSH_CAMERA_RELEASED         0x0E0  /* 1 once the locked camera 4 was released after the ambush */
#define L31_LAST_LIVING_HERO               0x0E8  /* last ST_HERO value that was not the dead hero (4) */
#define L31_KONKALITE8_CRY_ONCE            0x0FC  /* trigger 2: 1 once Konkalite8 reacted at EE8 == 99 */
#define L31_KONKALITE10_CRY_ONCE           0x100  /* trigger 3: 1 once Konkalite10 reacted at EEC == 99 */
#define L31_WEAPON_SEEN                    0x118  /* weapon the Konkalites already reacted to (compares with ST_PLAYER_WEAPON) */
#define L31_OB09_TAKEN                     0x59C  /* 1 once OB09 was picked up (trigger 14) */
#define L31_KONKALITE8_TALK_A              0xAD0  /* trigger 2 first line stage: 0 not said, 1 said */
#define L31_KONKALITE8_TALK_B              0xAD4  /* trigger 2 second line stage: 0 not said, 1 said */
#define L31_KONKALITE10_TALK_A             0xAD8  /* trigger 3 first line stage: 0 not said, 1 said */
#define L31_KONKALITE10_TALK_B             0xADC  /* trigger 3 second line stage: 0 not said, 1 said */
#define L31_KONKALITE_RETREAT_DONE         0xAE0  /* 1 once a Konkalite retreat was started (so only one runs) */
#define L31_TALK_REPEATED                  0xB1C  /* set to 1 when the Konkalite talk had to be repeated */
#define L31_STORY_PART                     0xBB0  /* arriving state: 0 fresh, 2 after the Konkalite fight, 10 later; picks the exit fights */
#define L31_ARRIVAL_KIND                   0xC64  /* room-32 chase variant: 1 first chase, 0 or 2 second chase */
#define L31_BORK_ROUTE_STAGE               0xEE0  /* Konkalite/Bork route: 1 Konkalites, 2 Bork, 10/20/30/35 return and second chase */
#define L31_KONKALITE8_HITS                0xEE8  /* Konkalite8 hit count: 1..5 fighting, 6 beaten, 99 retreated */
#define L31_KONKALITE10_HITS               0xEEC  /* Konkalite10 hit count: 1..5 fighting, 6 beaten, 99 retreated */
#define L31_STORY_MARK_F24                 0xF24  /* set to 5 at start point 2 */

#endif /* DKE_LIEU31_INT_H */
