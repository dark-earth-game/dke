#ifndef DKE_LIEU49_INT_H
#define DKE_LIEU49_INT_H

/*
 * lieu49_int.h — shared internals of the LIEU49.DLL port, split between
 * lieu49.c (entry, INIT, tick, tick body) and lieu49_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001BC3C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu49_trig.c). */
void l49_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu49_trig.c):
 *   l49_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l49_trig00(void);

/* Script variables of level 49 (Kalhi13 / Malrot8 / Dhorkan11 encounters). Slots below 0x100 are per-level scratch. */
#define L49_MALROT8_FIGHT              0x064   /* 0 = not yet, 1 = Malrot8 fight started (L49D40) */
#define L49_UNUSED_68                  0x068   /* only reset at INIT */
#define L49_KALHI_GIFT_STEP            0x06C   /* after Malrot8 dies: 0 start, 1 Kalhi walks to hero, 2 conversation done, 1000 finished */
#define L49_KALHI_WALK_STEP            0x070   /* Kalhi leaving waypoints 1..3, then she is repositioned */
#define L49_DHORKAN_INTRO_DONE         0x074   /* 1 once the Dhorkan11 arrival scene (L49D07..L49D12) played */
#define L49_MALROT8_TAUNT_SAID         0x078   /* 1 once L49D41A/B said after Kalhi was killed */
#define L49_GAMEOVER_SENT              0x07C   /* 1 once the hero-dead EV_END_GAME was queued */
#define L49_KALHI_JOINED               0x080   /* 1 once Kalhi joined the hero */
#define L49_KALHI_REGROUP_STARTED      0x084   /* 1 once Kalhi started walking to the hero after the fight */
#define L49_MALROT8_DEAD_HOLD          0x088   /* 1 = hero blocked after Malrot8 died (Kalhi alive) */
#define L49_STANCE_SAVED               0x090   /* 1 = combat stance was on and toggled off for the Dhorkan11 scene */
#define L49_DHORKAN_STAGE              0x4DC   /* 0 not met, 1 fighting, 2 dead / handled, 3 reset to 2 */
#define L49_KALHI_LEFT                 0x52C   /* 1 once Kalhi has left (walked off or room changed with stage 3) */
#define L49_KALHI_HIT_STAGE            0x53C   /* 3 = Kalhi killed by a hit, 4 = Malrot8 reaction said */
#define L49_OB97_OPENED                0x6FC   /* 0 = OB97 closed (placed at setup), 1 = opened */
#define L49_MALROT8_AMMO_TAKEN         0x90C   /* 1 once the OB_SM_TH_AMMO was taken from the Malrot8 corpse */
#define L49_KALHI_STORY_STAGE          0xBBC   /* Kalhi13 chase chapter (shared): 2 = Kalhi with hero, 3 = Kalhi left, 10 = Kalhi dead, 20 = Malrot8 taunt */
#define L49_PLATE_USED                 0xC20   /* 1 once the plate item was used on the wall */
#define L49_AMBUSH_PHASE               0xC2C   /* 0 none, 1 Kalhi taken, 2 Malrot8 taunt / fight, 3 Kalhi dead, Malrot8 hunts */
#define L49_DHORKAN_SEARCHED           0xC6C   /* 1 once the Dhorkan11 corpse was searched */
#define L49_ENDING_PLAYED              0xE8C   /* 1 = death ending already played (test at INIT) */
#define L49_MAIN_PROGRESS              0xE98   /* main story counter shared by many levels; 0x3C -> 0x41 on entry */
#define L49_LOOT_COUNT                 0xED0   /* corpse loot counter 0..5, advanced by each search */
#define L49_LOOT_FLAG                  0xED4   /* 1 = a corpse was searched */
#define L49_FLAG_F24                   0xF24   /* set to 1 when F78 is 0 and Dhorkan11 died; meaning shared with several levels */
#define L49_FLAG_F78                   0xF78   /* tested against 0 before setting F24 */

#endif /* DKE_LIEU49_INT_H */
