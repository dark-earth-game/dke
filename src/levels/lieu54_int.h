#ifndef DKE_LIEU54_INT_H
#define DKE_LIEU54_INT_H

/*
 * lieu54_int.h - shared internals of the LIEU54.DLL port, split between
 * lieu54.c (entry, INIT, tick, events, helpers) and lieu54_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10029CAC. */

/* Script variables of this room (state block offsets, ST()); the low slots
 * are scratch space the level reuses for itself, named per level. */
#define L54_PROVIDER_WARN_ARMED      0x064  /* 1 = Provider may warn Arkhan2 (L14D263) when he comes near; 0 after the warning, re-armed when hero is >400 away */
#define L54_GAMEOVER_QUEUED          0x068  /* 1 once hero is dead (ST_HERO==4) and game-over event 44 was queued */
#define L54_DELIA_HIT_MARK           0x06C  /* modify_var +1 when Delia is struck the first time (L10D95); never read */
#define L54_RANDOM_PICK              0x070  /* scratch: CB->random() result choosing the Malrot4 patrol waypoint; trig33 reuses it (10, +1) around Delia dialogue */
#define L54_FLAG_0074                0x074  /* reset at INIT, never used in this build */
#define L54_PROVIDER_FOLLOWING       0x078  /* 1 = Provider follows the hero (joindre hero), 0 = back at his post */
#define L54_KALHI13_ENTRY_STAGE      0x07C  /* Kalhi13 entry walk: 0 idle, 1 start (set by trig41), 40/2 -> 3/4 -> 5 waypoints, then the Malrot4 ambush spawns */
#define L54_ZONE4_UNBLOCKED          0x080  /* 1 once control is unblocked after hero is >700 from zone 4; trig25 temporarily holds hero HP here */
#define L54_MALROT1_ARMED_TALK       0x084  /* Malrot1 armed-hero talk stage 0/1 (L14D123..127); reset when hero is >400 from zone 25 */
#define L54_MALROT1_INTRO_DONE       0x088  /* 1 once the Malrot1 approach scene (L14D171/172) played */
#define L54_MALROT4_TAUNTED          0x090  /* 1 = Malrot4 taunted the armed hero (L14D169), 0 after L14D170 / reset */
#define L54_FLAG_0094                0x094  /* reset at INIT, never used in this build */
#define L54_MALROT1_CALLED           0x098  /* 1 = Malrot1 turned to hero on action; tick sends him back to his post when hero is >150 away */
#define L54_MALROT1_GUARDING         0x0B8  /* 1 = hero drew weapon near Malrot1 and he is watching (L14D204 when hero sheathes) */
#define L54_DELIA_NEAR               0x0BC  /* 1 = Delia stopped next to the hero (escort), 0 = she must catch up */
#define L54_DELIA_ESCORT_ON          0x0D4  /* 1 once the Delia escort (V_DELIA_PHASE 3/4) is running; enables follow distance checks */
#define L54_EXIT6_DELIA_SCENE        0x0D8  /* 1 once the Delia exit line (L03D01/02) was played at trigger 6 */
#define L54_MALROT4_SHOUT            0x0DC  /* Malrot4 reaction to the shout: 64 = pending (L14D235), 99 = done */
#define L54_PROVIDER_SHOUT           0x0E0  /* Provider reaction to the shout: 16 = pending (L14D236), 99 = done */
#define L54_MALROT1_SHOUT            0x0F0  /* Malrot1 reaction to the shout: 61 = pending (L14D237), 99 = done */
#define L54_MALROT1_D200_SAID        0x0F4  /* 1 once L14D200 said (then L14D242 instead) */
#define L54_MALROT4_D45B_SAID        0x0F8  /* 1 once the Malrot4 line L14D45B was said */
#define L54_CAM_ANIM_ON              0x0FC  /* 1 while the camera-2 animations/ambience of the motor room run */
#define L54_MOTOR_SND_DONE           0x100  /* 1 once the stopped-motor ambience (L14AMB1/2) was started for camera 2 */
#define L54_EXIT7_DELIA_SCENE        0x114  /* 1 once the Delia exit line (L03D01/02) was played at trigger 7 */
#define L54_POTION_GIVEN             0x154  /* set to 3 when the OB24F potion is shown to the Provider (L14D233) */
#define L54_DELIA_DEAD               0x170  /* 1 = Delia was killed in this room (with V_DELIA_KILLED 14) */
#define L54_ZONE18_SEEN              0x174  /* zone 18 object: 0 = first look (silent), 1 = repeat dialogue L14D100/101 */
#define L54_ZONE38_SEEN              0x178  /* zone 38 object (Provider corpse area): 0 = first, 1 = LCOD10 follow-up */
#define L54_PASSAGE25_OPEN           0x188  /* 1 = arrived from level 46, collision 25 removed (persistent) */
#define L54_GUARDS_HOSTILE           0x4CC  /* 1 = guards (Malrot4/Malrot1) are in fight mode; INIT re-places them near the entry used */
#define L54_PROVIDER_POSTFIGHT_TALK  0x538  /* 1 = Provider has a post-fight conversation pending (L14D243..257); set by INIT when L54_POST_FIGHT_STAGE==2, 0 once told */
#define L54_MOTOR_REFUELED           0x560  /* 1 = jerrycan fuel poured into the motor this visit (L14D269); cleared when camera 2 is re-entered */
#define L54_FLAG_05B8                0x5B8  /* story flag, only seeded 2 by start point 102 here */
#define L54_OB85_TAKEN               0x6CC  /* 0 = OB85 shown in room, 1 = taken (trigger 16) */
#define L54_JERRYCAN_FUEL            0x6FC  /* jerrycan fuel: 1 = full, 2 = emptied into the motor (L14D269) */
#define L54_FLAG_0718                0x718  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_073C                0x73C  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0740                0x740  /* story flag, only seeded 2 by start point 102 here */
#define L54_FLAG_07A4                0x7A4  /* story flag, only seeded 2 by start point 102 here */
#define L54_FLAG_0830                0x830  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_083C                0x83C  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0848                0x848  /* story value, only seeded 13857 by start point 102 here */
#define L54_FLAG_0948                0x948  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0A5C                0xA5C  /* story flag, only seeded 2 by start point 102 here */
#define L54_FLAG_0AE4                0xAE4  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0B44                0xB44  /* story flag, only seeded 2 by start point 102 here */
#define L54_FLAG_0B54                0xB54  /* story flag, only seeded 3 by start point 102 here */
#define L54_FLAG_0B84                0xB84  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0BA8                0xBA8  /* story stage, INIT advances 5 -> 6 */
#define L54_MALROT5_STAGE            0xBBC  /* Malrot5 chain: 1 -> 2, 4 -> 5 (Malrot5 alive) or 8 (dead), 7 -> 8 on leaving through trigger 8 */
#define L54_FLAG_0BD0                0xBD0  /* story flag, only seeded 2 by start point 102 here */
#define L54_FLAG_0BDC                0xBDC  /* story flag, only seeded 2 by start point 102 here */
#define L54_PROVIDER_HINT_GIVEN      0xBE0  /* 1 once the Provider said L14D52 (he now accepts item offers) */
#define L54_MALROT4_AMBUSH           0xBE8  /* 0 = no ambush yet, >=1 = Malrot4 ambush active (Provider follows hero), 3 -> 4 when leaving by trigger 5 */
#define L54_MALROT1_TRUST            0xBEC  /* Malrot1 stage: 0 = hostile talk, 1 = told secret (L14D119), 2 = gourd given (L14D175) */
#define L54_MOTOR_STOPPED            0xBF4  /* 1 = motor stopped (anims paused, ambience L14AMB1/2), 0 = running after refuel (L14AMB3/4) */
#define L54_MALROT4_WAYPOINT         0xCB0  /* Malrot4 patrol waypoint: 1 = pick, 2/3/4 = at one of the 3 posts, 10 = recalled */
#define L54_MALROT4_CHASE            0xCB4  /* 1 = Malrot4 notices hero at zone 2; cleared when hero is >200 away */
#define L54_MALROT4_APPROACH         0xCB8  /* Malrot4 approach: 0 not yet, 1 = ready (taunt/talk allowed) */
#define L54_MALROT4_TALK_ARMED       0xCBC  /* Malrot4 talk stage when Arkhan2 holds a fighting weapon, 0..2; reset when hero is >400 from zone 2 */
#define L54_MALROT4_TALK_A2          0xCC0  /* Malrot4 conversation stage for Arkhan2 (L14D05..43), 0..6 */
#define L54_PROVIDER_TALK_ARMED      0xCC4  /* Provider talk stage when Arkhan2 holds a fighting weapon (L14D28..31), 0/1 */
#define L54_PROVIDER_TALK_A2         0xCCC  /* Provider conversation stage for Arkhan2 (L14D49..63), 0..4 */
#define L54_MALROT1_TALK             0xCF0  /* Malrot1 conversation stage (L14D109..201), 0..4 */
#define L54_FLAG_0D0C                0xD0C  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0D20                0xD20  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0D24                0xD24  /* story flag, only seeded 2 by start point 102 here */
#define L54_MALROT4_TALK_POST_A2     0xDDC  /* Malrot4 conversation stage for Arkhan2 after the guard fight (L14D206..215), 0..2 */
#define L54_MALROT4_TALK_A3          0xDEC  /* Malrot4 conversation stage for Arkhan3 (L14D18..21), 0/1 */
#define L54_PROVIDER_TALK_A3         0xDF0  /* Provider conversation stage for Arkhan3 (L14D64..69), 0..2 */
#define L54_MALROT4_TALK_POST_A3     0xDF4  /* Malrot4 conversation stage for Arkhan3 after the guard fight (L14D217..220), 0/1 */
#define L54_FLAG_0E14                0xE14  /* story stage, INIT advances 1 -> 2 when L54_FLAG_0BA8==6 in level 10 */
#define L54_FLAG_0E1C                0xE1C  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0E24                0xE24  /* story stage, INIT advances 2 -> 3 */
#define L54_FLAG_0E34                0xE34  /* story flag, cleared (1 -> 0) when leaving by triggers 5, 7, 8 */
#define L54_FLAG_0E3C                0xE3C  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0E74                0xE74  /* set 1 by INIT when V_CRYSTAL_KEY_STAGE is non-zero */
#define L54_FLAG_0E7C                0xE7C  /* story value, only seeded 3 by start point 102 here */
#define L54_FLAG_0E90                0xE90  /* story value, only seeded 100 by start point 102 here */
#define L54_FLAG_0EA0                0xEA0  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0EB0                0xEB0  /* story flag, only seeded 1 by start point 102 here */
#define L54_POST_FIGHT_STAGE         0xEB8  /* 2 (start point 1) = Provider post-fight conversation pending, see L54_PROVIDER_POSTFIGHT_TALK */
#define L54_FLAG_0EF0                0xEF0  /* story flag, only seeded 1 by start point 102 here */
#define L54_FLAG_0F14                0xF14  /* story flag, only seeded 2 by start point 102 here */
#define L54_FLAG_0F24                0xF24  /* 1 when Dhorkan11 is dead, 5 after V_CHASE_ROUTE stage 2/3 */
#define L54_FLAG_0F28                0xF28  /* story stage, INIT advances 3 -> 4 */
#define L54_FLAG_0F3C                0xF3C  /* story flag, only seeded 2 by start point 102 here */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu54_trig.c). */
void l54_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu54_trig.c):
 *   l54_trig00 = trigger 0, sub_10001D60 (thunk sub_1000106E; INIT default) */
void l54_trig00(void);

#endif /* DKE_LIEU54_INT_H */
