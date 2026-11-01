#ifndef DKE_LIEU15_INT_H
#define DKE_LIEU15_INT_H

/*
 * lieu15_int.h — shared internals of the LIEU15.DLL port, split between
 * lieu15.c (entry, INIT, tick, helpers) and lieu15_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002EBAC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu15_trig.c). */
void l15_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu15_trig.c):
 *   l15_trig00 = trigger 0, trig00_10007543 (thunk sub_10001073; INIT default) */
void l15_trig00(void);

/* DLL globals shared by the tick body and the trigger handlers (lieu15.c). */
extern c8  g_l15_txt[256];     /* debug sprintf buffer, dword_1002E8C0 */
extern i32 g_dat_26B34;    /* dword_10026B34 (.data, starts 0) */
extern i32 g_dat_26B38;    /* dword_10026B38 (.data, starts 0): set by the tick when Danrys' Kalhi scene is armed */
extern i32 g_dat_26B3C;    /* dword_10026B3C (.data, starts 0): weapon-reaction kind (0/1) */

/* sub_10007400: Danrys' pre-scene (hero in ecx, persona in edx; the three
 * stack arguments are unused). */
void l15_pre_scene2(i32 hero, i32 n, i32 a1, i32 a2, i32 a3);

/* Level 15 script variables (low slots up to 0x118 and 0x32C..0x6A0 are level-private; 0xA80 and above are game-wide story state) */
#define L15_CRYSTAL_GIVEN_A        0x044   /* item id of the first crystal the hero already gave Danrys (18, 19 or 12 = OB crystals 1, 3, 2); set from the player's weapon slot */
#define L15_CRYSTAL_GIVEN_B        0x048   /* item id of the second crystal given to Danrys (same ids as L15_CRYSTAL_GIVEN_A) */
#define L15_STANCE_LOWERED         0x04C   /* 1 = combat stance lowered for a Danrys scene (DV_COMBAT_STANCE 0), 2 = restored afterwards, 0 = untouched */
#define L15_DANRYS_GREET_PICK      0x050   /* 0 = Danrys greeting line pending at the start-up scene; set 1 once the intro choice is made */
#define L15_GREETING_RANDOM_OK     0x054   /* 1 = random greeting choice (V_DLG_RANDOM == 1) allowed for the intro scene */
#define L15_DANRYS_TOPIC_58        0x058   /* 3 = hero already got the L15D387..392 topic; 2 blocks the Armal question; reset at INIT */
#define L15_DANRYS_SCENE_STATE     0x064   /* 0 idle, 1 = scene armed (hero close), 2 = scene running, 3 = finished (modify_var) */
#define L15_UNUSED_68              0x068   /* only zeroed by the INIT trigger; never read in this level */
#define L15_DANRYS_WARNED       0x06C   /* 1 = Danrys already reacted to the armed hero (L15D05); gates the intro dialogue */
#define L15_DANRYS_FIRST_TALK      0x070   /* 0 = first Danrys talk (L15D03A) pending, 1 = done (then L15D03B) */
#define L15_DANRYS_WALK_STEP       0x074   /* 1 = Danrys walking to the hero after the event-1 cut-scene (modify_var), 0 when he arrives */
#define L15_TRADE_TYPE             0x078   /* Danrys exchange: 0 = crystal exchange, 1 = weapon trade (item V_PRE90_OBJECT), 2 = trade done, 3 = hero comes back to collect */
#define L15_KONKALITE9_STATE       0x07C   /* 1 = Konkalite9 may attack when the hero is within -200, 2 = he is fighting */
#define L15_DANRYS_KALHI_FOLLOW    0x080   /* 1 = Kalhi13 follows the hero (set when the hero passes zone 2); if Danrys is hit he takes the fight */
#define L15_UNUSED_84              0x084   /* only zeroed by the INIT trigger; never read in this level */
#define L15_DANRYS_PRE_TALK_DONE   0x088   /* 1 = pre-trade conversation (L15D08A/B, L15D901..) already played */
#define L15_ARKHAN2_TOPIC_90       0x090   /* 0 -> 1 after the L15D300/301 conversation at trigger 2 */
#define L15_DANRYS_CALL_DONE       0x094   /* 1 = Danrys already came when called (L15D99) after the camera 2 scene */
#define L15_KALHI_DANRYS_DEAD_NOTE 0x098   /* 1 = hero killed Danrys; plays the Kalhi comment (L15D200) later */
#define L15_SAVED_CAMERA           0x0AC   /* locked camera at the time of the Danrys cut-scene; the tick compares it to detect a camera change */
#define L15_TRADE_SCENE_STEP       0x0B8   /* step 2..8 of the Danrys trade scene (walk, give, take, return); modify_var advances it */
#define L15_TRADE_ANIM_CAM         0x0BC   /* 0 = no meeting anim (215MEU0x) running in the current camera */
#define L15_DANRYS_ESCORT_STEP     0x0D4   /* 1 = Danrys walking to the hero, 2 = arrived */
#define L15_ANIM_PHASE             0x0D8   /* 1 = meeting anim in camera 0, 2 = meeting anim in camera 3 or cancelled; reset by modify_var */
#define L15_ROOM_PART              0x0DC   /* 0 = hero in the first part of the room, 1 = second part (camera 1) */
#define L15_CAMERA3_ENTERED        0x0E0   /* 1 = camera 3 scene (anim slot) running while the camera is locked to 3 */
#define L15_TRADE_REWARD_PENDING   0x0F0   /* 0 = no reward line pending for the crystal exchange */
#define L15_LEVER_CAM_STEP         0x0F4   /* 1 = lever anim playing outside camera 3; 2 = waiting for the camera to return */
#define L15_DANRYS_APPROACHED      0x0F8   /* 1 = Danrys already approached the hero at trigger 3 */
#define L15_TRADE_FINAL            0x0FC   /* 1 = final trade (weapon for the ending) active; selects L15D900C/D lines and the DV_END_GAME call */
#define L15_KALHI_FOLLOW_STEP      0x100   /* 1 = Kalhi13 asked to follow, 2 = walking, 3 = arrived and joined the hero */
#define L15_DANRYS_INTRO_114       0x114   /* 1 = intro line at trigger 4 already played */
#define L15_LAST_WEAPON_KALHI      0x118   /* weapon the hero showed Kalhi13 last (avoids repeating the warning) */
#define L15_TRIG_564               0x564   /* 0 -> 1 after Danrys' L15D606/607 lines */
#define L15_FIGHT_DONE_LINES       0x574   /* 1 = L15D407 lines (after V_GUARD_FIGHT_DONE) already said */
#define L15_TRIG9_SEEN             0x6A0   /* 1 = zone 9 message (LCOD76) already shown */
#define L15_DANRYS_TALK_STAGE_A80  0xA80   /* Danrys conversation 0..2 (L15D23..24) */
#define L15_DANRYS_PLOT_A84        0xA84   /* game-wide; 3 = Danrys died or fight went wrong, 4 = Danrys' reaction line said (shared with L10) */
#define L15_ROOM_ENTERED           0xA88   /* game-wide; 0 = first entry (intro), 1 = already visited (start points 2-5 set 1) */
#define L15_DANRYS_TALK_STAGE_A94  0xA94   /* 0 -> 1 conversation at trigger 2 (L15D335..341) */
#define L15_DANRYS_FIGHT_WARN      0xA9C   /* 0 -> 1 -> 2 Danrys warning steps while the hero is armed (L15D05) */
#define L15_CONVO_AA0              0xAA0   /* 0 -> 1 first part of the L15D312/317 conversation (needs E40) */
#define L15_CONVO_AA4              0xAA4   /* 1 = L15D307/308 conversation done */
#define L15_CONVO_AA8              0xAA8   /* 0..2 L15D380..386 conversation stage */
#define L15_CONVO_AAC              0xAAC   /* 0..2 L15D387..392 conversation stage */
#define L15_CONVO_AB0              0xAB0   /* 1 = L15D317..320 conversation done */
#define L15_CONVO_AB8              0xAB8   /* 1 = L15D55/56 conversation done */
#define L15_CONVO_ABC              0xABC   /* 0..2 L15D322..330 conversation stage */
#define L15_CONVO_AC0              0xAC0   /* 1 = L15D410/411 conversation done (needs L15_STORY_BCC) */
#define L15_KONKALITE9_TALK        0xAC8   /* 0 -> 1 first talk with Konkalite9 (L15D91..94) */
#define L15_PROGRESS_STEP          0xBBC   /* game-wide mission step 0..10 (start points 2-4 set 1..3; 2 = Kalhi/Danrys met, 5/8 = done, 10 = Kalhi13 killed, 20 = ended); shared with L14 */
#define L15_STORY_BCC              0xBCC   /* game-wide flag; non-zero allows the L15D410 line */
#define L15_DANRYS_INTERRUPTIBLE   0xBD4   /* 1 = Danrys scene can be cancelled by hitting him (he plays the 215MEU anim); reset at scene end */
#define L15_FLAG_BE4               0xBE4   /* game-wide; start points set 0 / read at trigger 3 (value 1 = special state) */
#define L15_DANRYS_STATE_BE8       0xBE8   /* game-wide; INIT start points set 2 */
#define L15_FLAG_BF4               0xBF4   /* game-wide; 0 = first time, 1 = later at triggers 3 and 5 */
#define L15_DANRYS_ENDING_FLAG     0xBFC   /* game-wide; 0 = ambience L15AMB1, otherwise L15AMB3 (Danrys ending path) */
#define L15_CONVO_C00              0xC00   /* 0 -> 1 after the L15D372/373 conversation; later lines depend on it */
#define L15_CONVO_C18              0xC18   /* 0 = first L15D328..330 line at trigger 2 */
#define L15_DANRYS_DEAD_SEEN       0xC34   /* 1 = Danrys' death already handled (event 30 queued) */
#define L15_ONCE_C50               0xC50   /* incremented once at trigger 5 (L15D421) */
#define L15_DANRYS_INTRO_STAGE     0xC98   /* 0 = intro not started, 1 = armed, 2 = intro seen (start points 2-5 set 2) */
#define L15_FLASK86_STAGE          0xD3C   /* 0 = flask OB_FLASK86 not shown yet, 1 = shown to Danrys, 2 = given */
#define L15_KALHI_STAGE_D40        0xD40   /* 1 = Kalhi13 walking to hero, 2 = arrived, 3 = ready; used with the Danrys-dead scene */
#define L15_KALHI_TALK_STAGE       0xD44   /* 0 = first Kalhi13 talk (L15D425..441), 1 = second, 2 = done */
#define L15_KALHI_DEAD_TALK_DONE   0xDE4   /* 1 = Kalhi13's comment after Danrys' death (L15D200) already said */
#define L15_KONKALITE9_ENGAGED     0xE34   /* game-wide; 0 = peaceful, 1 = Konkalite9 hostile, 2 = he is dead */
#define L15_ROOM_SEEN_E40          0xE40   /* game-wide; start point 1 sets 1; gates the L15D312/317 conversation */
#define L15_DANRYS_TALK_STAGE_E80  0xE80   /* Danrys talk 0..2 (L15D23, L15D21..) */
#define L15_STORY_EB0              0xEB0   /* game-wide story counter; >= 2 allows the L15D307 conversation */
#define L15_CRYSTAL_ROUND          0xECC   /* game-wide; 0 = first crystal round, 1..3 = later rounds (3 = no more trades) */
#define L15_CRYSTAL18_GIVEN        0xED4   /* game-wide; 1 = crystal OB18/OB19 already handed over (SOB18/SOB19 not restored in the trade scene) */
#define L15_CRYSTAL19_GIVEN        0xED8   /* game-wide; 1 = crystal OB18/OB19 already handed over (SOB18/SOB19 not restored in the trade scene) */
#define L15_CRYSTAL12_GIVEN            0xEDC   /* game-wide; 1 = crystal OB12 handed over (same slot as L12_PROOF_SHOWN) */
#define L15_ARMAL_STAGE            0xF4C   /* 0 -> 1 Armal conversation stage (with V_ARMAL_STAGE) */
#define L15_KALHI_DEAD_HANDLED     0xF58   /* 1 = Kalhi13's death already handled (progress step 10) */

#endif /* DKE_LIEU15_INT_H */
