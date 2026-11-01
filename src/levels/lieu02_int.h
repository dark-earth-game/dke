#ifndef DKE_LIEU02_INT_H
#define DKE_LIEU02_INT_H

/*
 * lieu02_int.h — shared internals of the LIEU02.DLL port, split between
 * lieu02.c (entry, INIT, tick, events, helpers) and lieu02_trig.c (the
 * zone trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002C98C. */

/* Script variables of level 2: state block slots this level reuses as its own scratch (not game-wide). */
#define L02_GATE_HERO_FACING      0x04C   /* hero depth/facing saved before the Dhorkan11 gate scene, replayed by joindre */
#define L02_KALHI33_WALK_STEP     0x050   /* Kalhi33 walk-back route progress 0..3 (3 = at his post) */
#define L02_DHORKAN11_SCENE_ON    0x054   /* 1 = Dhorkan11 gate scene (L02D50) started */
#define L02_GUARD_LEAVING         0x064   /* 1 = guard 17/19 left his post after the talk and returns to it when the hero is far */
#define L02_GUARD19_STATE         0x068   /* Guardian19 state: 0 free, 1 talking/engaged, 5 set by trigger */
#define L02_GUARD18_STAGE         0x06C   /* Guardian18 approach: 0 idle, 1 warned (L02D110), 2 attacking */
#define L02_COMPANION_JOINED      0x070   /* 1 = Kalhi13/33 joined the party (cleared at INIT) */
#define L02_ZED84_FOLLOW          0x074   /* Zed84 escort: 1 following hero 1, 2 waiting close, 0x2C reached the gate, 0x3E8/0xA scripted waits */
#define L02_UNUSED_78             0x078   /* never read, only cleared at INIT */
#define L02_ZED84_GATE_STAGE      0x07C   /* hero-1 briefing: 1 set at INIT (Zed84 present), 2 Zed84 sent to the gate */
#define L02_UNUSED_80             0x080   /* never read, only cleared at INIT */
#define L02_HIT_LIFE_TMP          0x084   /* scratch: guard HP read by the hit-drain helper */
#define L02_DHORKAN11_STEP        0x088   /* Dhorkan11 gate scene step 1 -> 2 -> 3; also Kalhi13 run flag (0/1) */
#define L02_KALHI13_CHATTER       0x090   /* Kalhi13 idle-chatter state: 1 ready, 0 waiting, 0xA paused at the bridge */
#define L02_UNUSED_94             0x094   /* never read, only cleared at INIT */
#define L02_UNUSED_98             0x098   /* never read, only cleared at INIT */
#define L02_KALHI13_FOLLOW        0x0B8   /* Kalhi13 follow state: 1 following hero, 0x14 waiting at the bridge, 0xA stopped on exit */
#define L02_DELIA_RUN             0x0BC   /* 1 = Delia is following at walking pace, 0 = needs to catch up */
#define L02_GUARD_RETURN          0x0D4   /* guard return-to-post phase: 1 walking back, 2 arrived (zone closed) */
#define L02_PHEDORIA_TALK_A_STEP  0x0D8   /* Phedoria L02D203..208 talk: 0 first, 1 second part */
#define L02_GUARD17_CHASING       0x0DC   /* 1 = Guardian17 chases the hero in guard stance */
#define L02_GUARD17_ALERT_STAGE   0x0E0   /* Guardian17 alert: 0 calm, 1/2 alerted, 3-5 Phedoria L02D217 sequence */
#define L02_UNUSED_F0             0x0F0   /* never read, only cleared at INIT */
#define L02_DELIA_FOLLOW          0x0F4   /* Delia follow state: 1 following, 0x14 waiting at the bridge, 0xA stopped */
#define L02_BRIDGE_CROSS          0x0F8   /* hero bridge crossing: 0 none, 1 crossed the line, 2 across, 3 coming back */
#define L02_KALHI33_FOLLOW        0x0FC   /* Kalhi33 follow state: 1 following, 2 close to the hero */
#define L02_GUARD17_CHASE_ARMED   0x100   /* 1 = set when arriving from room 6; cleared when guard 17 goes back to post */
#define L02_GUARD17_WALKING       0x114   /* 1 = Guardian17 is walking to his post (stop_joindre when arrived) */
#define L02_FLAG_2A8              0x2A8   /* read only: != 2 enables the L04D28B line (written by another room) */
#define L02_G17_TALK_PENDING      0x718   /* Guardian17 talk: 1 = first talk started, 0 = to be repeated */
#define L02_KALHI13_HOLD          0x794   /* nonzero stops the exit handler from halting Kalhi13 */
#define L02_KALHI_CHAT_STAGE      0x948   /* Kalhi13/33 chatter lines L04D07..09, 0..2 */
#define L02_L7_SPAWN              0xAFC   /* arrival spot from room 7: 1/2 = positions, 3/4 set when leaving to room 7 */
#define L02_ROOM6_DOOR            0xB34   /* 1 -> door to room 6 enters at variant 0, else variant 1 */
#define L02_L8_EXIT_STATE         0xB44   /* 1/2 = last exit used toward room 8/3 */
#define L02_FIRST_STEP_DONE       0xBD0   /* 1 -> 2 on the first trigger the hero steps on */
#define L02_ZED84_BRIEFING        0xBDC   /* Zed84 briefing: 1 pending, 2 at the gate, 3 returned */
#define L02_L8_ENTRY_E48          0xE48   /* nonzero -> room 8 entered at variant 1 */
#define L02_OB75_SEEN             0xE74   /* 1 once B10 was nonzero */
#define L02_PHEDORIA_FIGHT        0xEB8   /* 1 = Phedoria guards the entrance (fight not done yet) */
#define L02_COMPANION_ENTRY_DONE  0xEF0   /* 0 = place companion at the room entry, 1 = already placed */
#define L02_L8_ENTRY_F20          0xF20   /* 1 -> room 8 entered at variant 1 */
#define L02_KALHI33_ROUTE         0xF3C   /* Kalhi33 route: 1 north path, 2 south path */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu02_trig.c). */
void l02_trigger(i32 idx);

/* Trigger handlers the core also calls directly (defined in
 * lieu02_trig.c):
 *   l02_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default)
 *   l02_trig31 = trigger 31, sub_100149FB (thunk sub_10001000; tick body) */
void l02_trig00(void);
void l02_trig31(void);

#endif /* DKE_LIEU02_INT_H */
