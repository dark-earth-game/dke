#ifndef DKE_LIEU16_INT_H
#define DKE_LIEU16_INT_H

/*
 * lieu16_int.h — shared internals of the LIEU16.DLL port, split between
 * lieu16.c (entry, INIT, tick, helpers) and lieu16_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10024D9C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu16_trig.c). */
void l16_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu16_trig.c):
 *   l16_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l16_trig00(void);

/* Level 16 script variables (scratch slots 0x4C..0x118 are per level) */
#define L16_TICK_COUNTER                   0x038   /* engine tick counter; Delia's follow logic runs every 20th tick */
#define L16_CAM_RESET_PENDING              0x04C   /* 1 = reset the camera to 4 when the up/down key is pressed */
#define L16_MAOR_ANGRY_LINE_SAID           0x050   /* 1 once Maor said L16D998 (he turned hostile) */
#define L16_DELIA_WAIT_RUN                 0x054   /* 1 = Delia must run to catch up after the creature died; 666 once she was sent */
#define L16_DELIA_SENT_AWAY                0x064   /* 1 once Delia walked to (2716,3801) during the Maor intro */
#define L16_MAOR_CONFRONTED                0x068   /* 1 once Maor confronted the armed hero (zone 14 or after the Delia talk) */
#define L16_RANDOM_STEP                    0x06C   /* random pick of Maor's patrol branch; also bumped by modify_var(+1) as a sync counter */
#define L16_DELIA_ATTACKED                 0x070   /* 1 = hero struck Delia, 2 = she died of it (sets V_DELIA_KILLED 16) */
#define L16_MAOR_INTRO_SEEN                0x074   /* 1 once the Maor intro at zone 2 started */
#define L16_MAOR_TALK_ZONE15               0x078   /* 1 once the zone 15 Maor talk played */
#define L16_MAOR_ATTACK_ZONE16             0x07C   /* 1 once Maor attacked at zone 16 (L16D08) */
#define L16_HP_BEFORE_SIGN                 0x080   /* hero HP before the sign trap (L16D900/901); HP is lowered by 10 */
#define L16_MAOR_CHASING                   0x084   /* 1 = Maor chases the hero out of zone 2; reset when the hero is >180 away */
#define L16_DELIA_WALK_SYNC                0x088   /* walk sync: modify_var +1 while Delia walks, 4 when she arrived */
#define L16_TALK_DELIA_A3                  0x090   /* Arkhan3 conversation stage with Delia, 0..1 */
#define L16_DEATH_EVENT_SENT               0x094   /* hero dead: game-over event 44 already queued */
#define L16_TALK_DELIA_A2_B                0x098   /* Arkhan2 second conversation with Delia (special mode off), 0..1 */
#define L16_MAOR_WALKING                   0x0B8   /* 1 = Maor is walking, start combat when he stops */
#define L16_DELIA_CAMERA_SEEN              0x0BC   /* 1 once the Delia camera-locked scene (cameras 4/5) started */
#define L16_MAOR_CAMERA_ZONE               0x0D4   /* 1 = Maor active while the camera is locked on cameras 4/5 */
#define L16_DELIA_FOLLOW_ARMED             0x0D8   /* 1 once Delia (phase 2) follows the hero */
#define L16_DELIA_CLOSE                    0x0DC   /* 1 = Delia stopped next to the hero (<100), 0 = she must catch up */
#define L16_CAMERA_LOCKED_45               0x0E0   /* 1 while the camera is locked on cameras 4/5 */
#define L16_UNUSED_0F0                     0x0F0   /* reset at INIT, never used */
#define L16_DELIA_CRY_STATE                0x0F4   /* shout at Delia: 10 = armed, 99 = answered */
#define L16_MAOR_CRY_STATE                 0x0F8   /* shout at Maor: 17 = armed, 99 = answered */
#define L16_TALK_MAOR_A2                   0x0FC   /* Arkhan2 first talk with Maor at zone 4: 0 first, 1 later */
#define L16_MAOR_ZONE2_STARTED             0x100   /* 1 once Maor's zone 2 intro ran; reset when he returns to his post */
#define L16_DELIA_TALK_A2_NOTE             0x114   /* 1 once Arkhan2 added the L16D510 remark to the Delia talk */
#define L16_DELIA_LAST_WEAPON              0x118   /* weapon the hero held at the last Delia talk (avoids repeating the warning) */
#define L16_OB24C_INIT_STATE               0x148   /* set to 3 at INIT while the OB24C chest is still closed */
#define L16_CREATURE_GATE                  0x17C   /* 1 = creature (persona 90) gate active after the Maor fight; wall 21 removed when Delia is hit */
#define L16_MAOR_HOSTILE                   0x4D4   /* 1 = Maor is fighting the hero (restored at INIT) */
#define L16_DELIA_HIT_FLAG                 0x508   /* 1 = Delia was hit (restored for Maor's reaction) */
#define L16_TALK_DELIA_A3_B                0x510   /* Arkhan3 conversation flag with Delia, 0..1 */
#define L16_TALK_MAOR_A3                   0x518   /* Arkhan3 conversation stage with Maor, 0..2 */
#define L16_CREATURE_HOSTILE               0x528   /* 1 = creature (persona 90) fights the hero */
#define L16_CHEST_OB44B_STAGE              0x534   /* chest OB44B/OB65 (zone 30): 0 closed, 1 opened, 2 taken */
#define L16_TALK_AFTER_MAOR_DEAD           0x540   /* Arkhan comment after Maor died, 0..1 */
#define L16_TALK_DELIA_A2                  0x544   /* Arkhan2 conversation flag with Delia, 0..1 */
#define L16_CHEST_OB04_STAGE               0x588   /* chest OB04 (zone 11): 0 closed, 1 opened */
#define L16_OB65_TAKEN                     0x67C   /* 0 = OB65 still on the chest */
#define L16_DELIA_ESCAPE_STAGE             0x770   /* Delia leaves with the hero: 0 none, 1 may leave, 2 dead, 99 refused */
#define L16_CHEST_OB24C_STAGE              0x788   /* chest OB24C (zone 11): 0 closed, 1 opened */
#define L16_DELIA_TALK_STAGE               0xAE8   /* main conversation stage with Delia, 0..7 */
#define L16_ARMAL_STAGE                    0xBC8   /* Armal story stage (L18 sets it); changes the Delia talk */
#define L16_MAOR_ENCOUNTER                 0xBF0   /* Maor encounter: 0 none, 1 started, 2 Delia sent away */
#define L16_MAOR_PATROL_STEP               0xCF4   /* Maor patrol: 0 idle, 1..3 next leg (event 1 steps it) */
#define L16_TALK_MAOR_A2_B                 0xD00   /* Arkhan2 second conversation with Maor, 0..3 */
#define L16_DELIA_WARNING_STAGE            0xE08   /* Delia warning about Maor: 0 none, 1 given */
#define L16_DEATH_ENDING_DONE              0xE8C   /* 1 = death ending already played, INIT runs the normal setup (same slot in every level) */
#define L16_DELIA_WEAPON_STAGE             0xEE4   /* Delia weapon talk: 0 none, 1 asked, 2 axe shown, 3 done */

#endif /* DKE_LIEU16_INT_H */
