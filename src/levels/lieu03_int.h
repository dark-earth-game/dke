#ifndef DKE_LIEU03_INT_H
#define DKE_LIEU03_INT_H

/*
 * lieu03_int.h — shared internals of the LIEU03.DLL port, split between
 * lieu03.c (entry, INIT, tick, helpers) and lieu03_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001DFFC. */

/* Script variables of level 3: state block slots this level reuses as its own scratch (not game-wide). */
#define L03_FRAME_COUNTER       0x038   /* engine frame counter; scripts throttle with % 20 / % 100 */
#define L03_UNUSED_64           0x064   /* never read, only cleared at INIT */
#define L03_DELIA_FOLLOW        0x068   /* Delia follow state: 1 following, 0xA stopped/killed, 0x14 waiting at the exit */
#define L03_DELIA_ESCORT_SET    0x06C   /* 0 -> 1 once Delia's escort phase moved 8 -> 9 */
#define L03_COMPANION_REJOINED  0x070   /* 1 = Kalhi13/33 rejoined the hero with L03D500A/B */
#define L03_KALHI_SWAP          0x074   /* Kalhi13 -> Kalhi33 swap scene: 1 armed, 2 played, 0x63 Kalhi33 path watch active */
#define L03_KALHI13_CHATTER     0x078   /* Kalhi13 idle chatter: 1 ready, 0 waiting, 2 paused */
#define L03_KALHI13_FOLLOW      0x07C   /* Kalhi13 follow mode: 1 following, 0xA/0x64 stopped by an exit trigger */
#define L03_UNUSED_80           0x080   /* never read, only cleared at INIT */
#define L03_DELIA_RUN           0x084   /* 0 = Delia needs a catch-up order, 1 = following in range */
#define L03_KALHI13_NEAR        0x088   /* 1 = Kalhi13 reached the hero and follows, 0 = needs a new order */
#define L03_KALHI33_ZONE_DONE   0x090   /* 0x3E8 = Kalhi33 stopped at zone 13 (L03 path finished) */
#define L03_SAVED_CAMERA        0x094   /* camera locked when the Kalhi13 exit scene started */
#define L03_KALHI33_POS_X       0x0E0   /* last sampled Kalhi33 position X (-1 = scene played) */
#define L03_KALHI33_POS_Y       0x0F0   /* last sampled Kalhi33 position Y (-1 = scene played) */
#define L03_KALHI33_POS_Z       0x0F4   /* last sampled Kalhi33 position Z (-1 = scene played) */
#define L03_KALHI33_WATCH_OFF   0x0FC   /* 0x63 disables the Kalhi33 stuck check (never set in this room) */
#define L03_CAM_ZONE_ARMED      0x100   /* 1 = camera 1 zone sound/anim played, re-arms when the hero leaves */
#define L03_EVENT_6             0x338   /* event 6 step (0x320 + 4*6); set with EVENT_10 by the Guardian4 check */
#define L03_EVENT_10            0x348   /* event 10 step (0x320 + 4*10) */
#define L03_KALHI13_CHAT_STAGE  0x948   /* Kalhi13 chatter lines L03D601..603, 0..2 */
#define L03_RETURN_PHASE        0xAF8   /* 0 first pass, 1 return pass */
#define L03_KALHI_RECRUIT       0xB40   /* Kalhi recruitment stage; 3 = recruited */
#define L03_GUARDIAN4_STATE     0xB98   /* Guardian4 encounter state: 2 -> reset to 0 while he is alive, 4 with AF8==1 sets events 6/10 */
#define L03_COMPANION_KILLED    0xE58   /* 1 = Kalhi13/33 or a companion was killed */
#define L03_FORCE_DEFAULT_INIT  0xE8C   /* 1 = run default INIT even when the hero is dead */
#define L03_KALHI_COMPANION     0xE9C   /* companion state: 0 none, 1 met/following, 2 dead or lost, 3 following in later rooms */
#define L03_KALHI33_ROUTE       0xF3C   /* Kalhi33 route: 1 / 2 = zone he was seen in (6 / 1), 0 unknown */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu03_trig.c). */
void l03_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu03_trig.c):
 *   l03_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l03_trig00(void);

#endif /* DKE_LIEU03_INT_H */
