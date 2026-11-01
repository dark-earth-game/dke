#ifndef DKE_LIEU08_INT_H
#define DKE_LIEU08_INT_H

/*
 * lieu08_int.h - shared internals of the LIEU08.DLL port, split between
 * lieu08.c (entry, INIT, tick, helpers) and lieu08_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1003FC5C. */

/* Script variables of level 8: state block slots this level reuses as its own scratch (not game-wide). */
#define L08_FRAME_COUNTER         0x038   /* engine frame counter; scripts throttle with % 20 */
#define L08_KALHI_FIGHT_TALK      0x04C   /* 1 = Kalhi33 combat lines (L08D2xx) already queued near zone 33 */
#define L08_KALHI13_D220_PENDING  0x054   /* 1 = L08D220 line pending when Kalhi13 reaches the hero */
#define L08_ARENA_ROUND_CMD       0x064   /* arena fight command from the cry (event 0): 0x2B Guardian3, 0x2C Guardian4, 0x2D Guardian5, 0x2E Guardian6, 0x63 done */
#define L08_KALHI13_ZONE_STEP     0x068   /* Kalhi13 camera/zone follow: 0 idle, 1 left camera 3, 2 back, 3 waiting */
#define L08_FOLLOWER_MODE         0x06C   /* companion follow mode: 1 following, 0x14 waiting at a gate, 0x63 held (Delia / Kalhi13) */
#define L08_PATROL_PICK           0x070   /* random(3) choice of the next Guardian3 patrol point */
#define L08_KALHI13_CHAT_READY    0x074   /* 1 = Kalhi13 may start idle chatter, 0xA paused */
#define L08_FOLLOWER_NEAR         0x078   /* 1 = companion (Kalhi13 / Delia) reached the hero, 0 = needs a new order */
#define L08_G6_PATROL_STEP        0x07C   /* Guardian6 patrol leg: 0 east, 1 west, 0xA chasing the hero */
#define L08_KALHI33_STEP          0x080   /* Kalhi33 escort step: 1 walking, 2 arrived, 3 following, 6 waiting for the hero, 0xA at the exit, 0x63/0x64 transient */
#define L08_COMPANION_REUNION     0x084   /* 1 = companion rejoin scene armed (E9C 6), 2 = played */
#define L08_G7_STEP               0x088   /* Guardian7 step: 0xA ready to walk, 0x63 walking, 0x6F arrived, 3 attacking */
#define L08_KALHI33_CHAT_READY    0x090   /* Kalhi33 chatter ready flag (cleared at INIT) */
#define L08_KALHI33_POS_SAVED     0x094   /* Kalhi33 position saved by trigger 31 */
#define L08_DELIA_TRIG8_DONE      0x098   /* 1 = Delia camera-3 scene (trigger 8) played */
#define L08_G8_STATE              0x0B8   /* Guardian8: 5 = walking back, 0xA = removed; 0/1 = Guardian3 stance toggle */
#define L08_G4_STANCE             0x0BC   /* 1 = Guardian4 in guard stance (reset with animation P44ARR02) */
#define L08_UNUSED_D4             0x0D4   /* never read, only cleared at INIT */
#define L08_UNUSED_D8             0x0D8   /* never read, only cleared at INIT */
#define L08_G5_TALK_DONE          0x0DC   /* 1 = Guardian5 first talk done (zone 4) */
#define L08_G6_TALK_DONE          0x0E0   /* 1 = Guardian6 talk done (zone 5) */
#define L08_KALHI33_STUCK_CHECK   0x0F0   /* 1 = Kalhi33 distance warning (L08D229..) already given */
#define L08_CREATURE34_ENGAGED    0x0F4   /* 1 = creature 34 attack started */
#define L08_KALHI33_RUN_STEP      0x0F8   /* Kalhi33 catch-up: 1 started, 2 stopped */
#define L08_KALHI13_GAR_STEP      0x0FC   /* Kalhi13 guard-stance lines (L08D326/327) step 0, 1, 0xA */
#define L08_UNUSED_100            0x100   /* never read, only cleared at INIT */
#define L08_CAM_ANIM_ARMED        0x114   /* 1 = camera animation played, re-arms when it ends */
#define L08_LAST_WEAPON_SEEN      0x118   /* last weapon the Guardians reacted to (compared with ST_PLAYER_WEAPON) */
#define L08_UNUSED_144            0x144   /* never read, only cleared at INIT */
#define L08_FLAG_170              0x170   /* 1 = pending, set to 2 by triggers 8/9 (Guardian7 arena scene) */
#define L08_ARENA_ROUND_A         0x338   /* arena round (event 6 slot): 0 Guardians 3/4, 1 Guardians 5/6, 2 Guardians 7/8, 0x63 companion present */
#define L08_ARENA_ROUND_B         0x348   /* arena round (event 10 slot): mirrors ARENA_ROUND_A */
#define L08_FLAG_740              0x740   /* read only: L08D251B..253 lines when 2..4 (written by room 6) */
#define L08_FLAG_780              0x780   /* set to 6 by trigger 12 */
#define L08_G7_INTRO_DONE         0x78C   /* 1 = Guardian7 first sight handled (trigger 6) */
#define L08_D175_STAGE            0x790   /* trigger 31 L08D175 / L08D203 stage 0, 1 */
#define L08_KALHI13_STAYS         0x794   /* 1 = Kalhi13 held back at the camera boundary */
#define L08_TRIG2_TALK_DONE       0x798   /* trigger 2 L08D25..32 talk done */
#define L08_TRIG3_TALK_DONE       0x79C   /* trigger 3 L08D58..64 talk done */
#define L08_G3_TALK_STAGE         0x7A0   /* Guardian3/4 conversation stage 0..6 (trigger 2) */
#define L08_G3_FIGHT_STAGE        0x7A4   /* Guardian3/4 fight/talk stage 0..0xA (trigger 3) */
#define L08_G5_TALK_STAGE         0x7A8   /* Guardian5/6 conversation stage 0..2 (trigger 4) */
#define L08_G7_TALK_STAGE         0x7AC   /* Guardian7/8 conversation stage 0..2 (trigger 5) */
#define L08_G3_STANCE_TALK        0x7B0   /* 1 = Guardian3 stance toggle talk (trigger 2) */
#define L08_G6_APPROACH           0x7B4   /* Guardian6 approach: 0 idle, 1 patrol, 2 chasing, 0xA warned */
#define L08_ARENA_UNLOCKED        0x804   /* nonzero enables the Guardian7/8 round (start point 6 clears it) */
#define L08_TRIG7_DONE            0x8C0   /* 1 = trigger 7 one-shot played */
#define L08_G3_PATROL_STEP        0x918   /* Guardian3 patrol leg 0..5 (0xA = returning) */
#define L08_G3_PATROL_ARMED       0x91C   /* 1 = Guardian3 patrol armed, resets when the hero is far */
#define L08_G4_PATROL_STATE       0x920   /* Guardian4 patrol state 0, 1, 0xA, 0xF, 0x14 */
#define L08_G5_ALERT              0x924   /* Guardian5 alert: 0 none, 1 returning, 2 / 0xA alerted */
#define L08_KALHI13_CHAT_STAGE    0x948   /* Kalhi13 chatter lines L08D155..157 stage 0..2 */
#define L08_TRIG21_DONE           0x94C   /* 1 = trigger 21 one-shot played */
#define L08_RETURN_PHASE          0xAF8   /* 0 first pass, 1 return pass */
#define L08_FROM_ROOM2            0xB44   /* 1 = arrived from room 2 (start points) */
#define L08_ROOM_VARIANT          0xB4C   /* entry variant 0..3 selecting the arrival spot (triggers 10/11) */
#define L08_FROM_ROOM10           0xB54   /* 1..4 = arrived from room 10 via route n */
#define L08_G34_STATE             0xB98   /* Guardian3/4 round state 0..5 (0/1 round open, 2 only Guardian4, 4 both, 5 done) */
#define L08_G7_DEAD_FLAG          0xBC0   /* 0x63 = Guardian7 killed; 1 set by start points */
#define L08_FLAG_BD0              0xBD0   /* 2 = start point preset for room 10 return */
#define L08_BONUS_ROUND           0xC04   /* arena bonus round: 0 none, 1 / 0xB / 0x15 states */
#define L08_G7_ROUND              0xC08   /* Guardian7 round: 0 none, 1 / 2 states set by start points */
#define L08_G10_STATE             0xDE8   /* Guardian10 ambush: 0x14 attacking, 0x1E dead */
#define L08_G4_WAYPOINT           0xE1C   /* Guardian4 patrol waypoint 0..9 */
#define L08_FLAG_E24              0xE24   /* 2 -> 3 at INIT */
#define L08_DELIA_ROUTE           0xE48   /* 1 = Delia scene route (start point), 0 = normal */
#define L08_COMPANION_KILLED      0xE58   /* 1 = Kalhi13/33 killed */
#define L08_FORCE_DEFAULT_INIT    0xE8C   /* 1 = run default INIT even when the hero is dead */
#define L08_KALHI_COMPANION       0xE9C   /* companion state: 0 none, 5 following, 6 rejoining, 7+ gone, 0x64 / 0xC8 special */
#define L08_ROUND_RESULT          0xEC8   /* arena round result code: 1, 0xA, 0xF, 0x10, 0x14, 0x19 */
#define L08_COMPANION_ENTRY_DONE  0xEF0   /* 0 = place companion at the room entry, 1 = already placed */
#define L08_ARENA_LOCKED          0xF20   /* 1 = arena rounds suspended */
#define L08_CREATURE34_SEEN       0xF7C   /* 1 = creature 34 attack handled */
#define L08_KALHI33_D_DONE        0xF80   /* 1 = Kalhi33 line played in trigger 41 */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu08_trig.c). */
void l08_trigger(i32 idx);

/* Trigger handlers the core also calls directly (defined in lieu08_trig.c):
 *   l08_trig00 = trigger 0, sub_10001200 (thunk sub_10001073; INIT default)
 *   l08_trig08 = trigger 8, sub_10012CE2 (the tick starts it) */
void l08_trig00(void);
void l08_trig08(void);

#endif /* DKE_LIEU08_INT_H */
