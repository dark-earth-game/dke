#ifndef DKE_LIEU48_INT_H
#define DKE_LIEU48_INT_H

/*
 * lieu48_int.h — shared internals of the LIEU48.DLL port, split between
 * lieu48.c (entry, INIT, tick, tick body) and lieu48_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001E41C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu48_trig.c). */
void l48_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu48_trig.c):
 *   l48_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l48_trig00(void);

/* Script variables of level 48 (Kalhi13 / Malrot3 / Malrot7 encounters). Slots below 0x100 are per-level scratch. */
#define L48_KALHI_RUN_ACTIVE           0x04C   /* 1 while Kalhi13 runs to her flee spot, cleared when she arrives */
#define L48_MALROT3_KILLED             0x050   /* 1 = Malrot3 felled by the hero, 2 = Kalhi reaction handled */
#define L48_MALROT3_FIGHT_STARTED      0x054   /* 1 = Malrot3 combat / dialogue L48D51 already triggered */
#define L48_KALHI_REACT_STAGE          0x058   /* after Malrot3 is down: 0 = pending, 1 = Kalhi alive (flee scene), 2 = Kalhi dead */
#define L48_SAVED_ENGAGED              0x064   /* V_ENGAGED_PERSONA saved when leaving through the door zone, restored on return; 0 = none */
#define L48_MALROT7_ALERT              0x068   /* 0 = Malrot7 not yet noticed hero, 1 = noticed / room set up, 10 = Malrot7 hostile in the fight */
#define L48_MALROT7_AMBUSH_STEP        0x06C   /* 0 none, 1 armed, 2 gate anim playing, 3 ready, 4 combat started */
#define L48_UNUSED_70                  0x070   /* only reset at INIT */
#define L48_MALROT7_ARMED              0x074   /* 1 = Malrot7 waits to ambush (re-entry), 2 = triggered (L48D03) */
#define L48_UNUSED_78                  0x078   /* only reset at INIT */
#define L48_KALHI_WEAPON_WARNED        0x07C   /* 1 once Kalhi warned about the drawn weapon (L48D32) */
#define L48_PATROL_MEET_DONE           0x080   /* 1 once the Kalhi / Malrot3 proximity conversation played */
#define L48_MALROT3_FLEE_STEP          0x084   /* 1 = Malrot3 running away, 2 = arrived (repositioned P63ARR01) */
#define L48_KALHI_FLEE_STEP            0x088   /* 1 = Kalhi running away, 2 = arrived (repositioned P13ARR01) */
#define L48_KALHI_TALK_STAGE           0x090   /* Kalhi conversation stage 0/1 (L48D17..L48D26) */
#define L48_MALROT3_TALK_STAGE         0x094   /* Malrot3 conversation stage 0/1 (L48D33..L48D38) */
#define L48_MALROT7_BARRIER_ON         0x098   /* 1 = collision 16 blocks Malrot7 while the hero is behind him, 0 = open */
#define L48_UNUSED_B8                  0x0B8   /* only reset at INIT */
#define L48_MALROT7_FROM_L30           0x0BC   /* 1 = Malrot7 followed the hero from the side room, cleared on return */
#define L48_UNUSED_D4                  0x0D4   /* only reset at INIT */
#define L48_UNUSED_D8                  0x0D8   /* only reset at INIT */
#define L48_DOOR_COMMENT_SAID          0x0DC   /* 0 = first locked-door comment (L48D666/667), 1 = repeat (L48D668/669) */
#define L48_UNUSED_F0                  0x0F0   /* only reset at INIT */
#define L48_KALHI_STORY_STAGE          0xBBC   /* Kalhi13 chase chapter (shared with levels 15, 19, 49, 54): 8 = Kalhi/Malrot3 duo here, 9 = Malrot3 dead, 20 = Kalhi dead */
#define L48_SCENE_STAGE                0xC30   /* Kalhi/Malrot3 scene: 0 start, 1 idle chatter, 2 cinematic, 3 Kalhi alone, 4 Kalhi dead, 10 left room, 100 hostile */
#define L48_KONKALITE_D01_SAID         0xC5C   /* 1 once L48D01 (Konkalite5) said */
#define L48_COL10_OPENED               0xF0C   /* 1 once collision 10 was lifted on the first pass through the door */
#define L48_MALROT7_MODE               0xF10   /* 0 = absent, 1 = at his post (idle), 2 = alerted / hostile */

#endif /* DKE_LIEU48_INT_H */
