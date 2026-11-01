#ifndef DKE_LIEU04_INT_H
#define DKE_LIEU04_INT_H

/*
 * lieu04_int.h — shared internals of the LIEU04.DLL port, split between
 * lieu04.c (entry, INIT, tick, helpers) and lieu04_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10022A8C. */

/* Script variables of level 4: state block slots this level reuses as its own scratch (not game-wide). */
#define L04_FRAME_COUNTER       0x038   /* engine frame counter; scripts throttle with % 20 */
#define L04_HERO_WALK_STEP      0x04C   /* hero walk scripted after the arrival scene: 1 queued, 0x63 walking, 0 done */
#define L04_D212_SAID           0x064   /* 1 = L04D10 sound + L04D212 line already played */
#define L04_ARRIVAL_STEP        0x068   /* arrival cutscene step 1..6 (Kalhi13 walks to the hero, L04D03A/B) */
#define L04_GUARD_SCENE_ON      0x06C   /* 1 = a guard approach scene has started */
#define L04_UNUSED_70           0x070   /* never read, only cleared at INIT */
#define L04_KALHI_SWAP          0x074   /* Kalhi13 -> Kalhi33 swap scene: 1 armed, 2 played */
#define L04_KALHI_CHATTER       0x078   /* Kalhi13/33 idle chatter: 1 ready, 0 waiting, 2 paused */
#define L04_KALHI_FOLLOW        0x07C   /* Kalhi follow mode: 1 following, 0x64 stopped by a trigger */
#define L04_UNUSED_80           0x080   /* never read, only cleared at INIT */
#define L04_HIT_LIFE_TMP        0x084   /* scratch: Konkalite6 HP read while applying a hit */
#define L04_KALHI_NEAR          0x088   /* 1 = Kalhi reached the hero and follows, 0 = needs a new order */
#define L04_UNUSED_90           0x090   /* never read, only cleared at INIT */
#define L04_UNUSED_94           0x094   /* never read, only cleared at INIT */
#define L04_GUARD_FIGHT_STAGE   0x098   /* guard encounter: 0 none, 1 intro, 2 fighting, 3 fight talk done */
#define L04_ZONE7_LOCK          0x0B8   /* 0 = zone 7 usable, 0xA = up/down exit armed */
#define L04_GUARD_ALERT         0x0BC   /* 1 = guard alerted by a strike / the hero close */
#define L04_G25_PATROL_PICK     0x0D4   /* Guardian25 patrol pick from random(3): 1, 2, 3 (0 = none) */
#define L04_G25_PATROL_STEP     0x0D8   /* Guardian25 patrol step 0..5 */
#define L04_GUARD_APPROACH      0x0DC   /* guard approach: 1 talking, 2 attack started */
#define L04_ZONE3_SCENE         0x0E0   /* 1 = zone 3 scene pending (L04D23/24), 2 = running */
#define L04_UNUSED_F0           0x0F0   /* never read, only cleared at INIT */
#define L04_KONKALITE_DONE      0x0F4   /* 1 = Konkalite6 dead or event 35 seen; arms the L04D211.. lines */
#define L04_D211_STAGE          0x0F8   /* 0 = L04D211A/212 not yet said, 1 = said */
#define L04_KALHI13_HOLD        0x0FC   /* 1 = Kalhi13 must stop walking (set by trigger 7) */
#define L04_UNUSED_100          0x100   /* never read, only cleared at INIT */
#define L04_CAM_ANIM_ARMED      0x114   /* 1 = camera 1 anim played, cleared when the camera leaves */
#define L04_TERMINAL_STAGE      0x5C4   /* zone 7 terminal use: 0 first, 1 sound played, 2 used */
#define L04_D213_SAID           0x8EC   /* 1 = L04D213A/B said */
#define L04_KALHI_CHAT_STAGE    0x948   /* Kalhi chatter lines L04D07..09, 0..2 */
#define L04_GUARD_TALK_STATE    0xAE4   /* guard talk state: 0 first, 1 Guardian2 done, 0x29A/0x29B Guardian25 lines */
#define L04_RETURN_PHASE        0xAF8   /* 0 first pass, 1 return pass */
#define L04_KALHI_RECRUIT       0xB40   /* Kalhi recruit stage: 0 none, 1 Guardian2 guard, 2 Kalhi rescued, 3 recruited, 4 Guardian25 path */
#define L04_ARRIVAL_PATH        0xBD0   /* arrival path: 1 first arrival, 2 second (promotes B40 0 -> 1) */
#define L04_G25_INTRO_DONE      0xD48   /* 1 = Guardian25 first patrol finished and camera released */
#define L04_KONKALITE_STAGE     0xE30   /* Konkalite6 stage: 1 fight begun, 3 killed */
#define L04_FLAG_E3C            0xE3C   /* set to 1 by start point 2, used by later rooms */
#define L04_COMPANION_KILLED    0xE58   /* 1 = Kalhi13/33 killed */
#define L04_FORCE_DEFAULT_INIT  0xE8C   /* 1 = run default INIT even when the hero is dead */
#define L04_GUARD2_LINES        0xE90   /* Guardian2 lines L04D10/11 stage 0, 1, 2 */
#define L04_KALHI_COMPANION     0xE9C   /* companion state, set to 6 by start points */
#define L04_ARENA_ON            0xEA0   /* 1 = Konkalite6 arena active (gate anim 0), 0 = normal (anim 1) */
#define L04_TERMINAL_USES       0xED0   /* terminal use count 0..5 */
#define L04_TERMINAL_USED       0xED8   /* 1 = terminal used */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu04_trig.c). */
void l04_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu04_trig.c):
 *   l04_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l04_trig00(void);

#endif /* DKE_LIEU04_INT_H */
