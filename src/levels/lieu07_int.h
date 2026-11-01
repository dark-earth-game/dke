#ifndef DKE_LIEU07_INT_H
#define DKE_LIEU07_INT_H

/*
 * lieu07_int.h — shared internals of the LIEU07.DLL port, split between
 * lieu07.c (entry, INIT, tick, helpers) and lieu07_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001E2AC. */

/* Script variables of level 7: state block slots this level reuses as its own scratch (not game-wide). */
#define L07_UNUSED_64            0x064   /* never read, only cleared at INIT */
#define L07_ZED27_TURNED         0x068   /* 0 = Zed27 may do a random taunt anim (SP27ATA08..10), 1 = already turned */
#define L07_ZED27_ZONE2_SEEN     0x06C   /* 1 = hero entered zone 2 with Zed27 on guard (repositioned once) */
#define L07_ZED_TIMER_PHASE      0x070   /* Zed27 timer scene: 0 idle, 1 armed */
#define L07_PHEDORIA_SEEN        0x074   /* 1 = Phedoria arrival animation played */
#define L07_PHEDORIA_TALK_STEP   0x078   /* Phedoria talk L07D58..L07D59: 0 first, 1 second, 2 done */
#define L07_PHEDORIA_SCENE_STEP  0x07C   /* Phedoria scene L07D74..77 step 0..3 */
#define L07_D79_SAID             0x080   /* 1 = L07D79/D80/D81 sequence played */
#define L07_D26_SAID             0x088   /* 1 = L07D26/27 said */
#define L07_UNUSED_90            0x090   /* never read, only cleared at INIT */
#define L07_ZED27_ZONE_LOCK      0x094   /* 1 = hero held in zone 39 / zone 2 by Zed27 (cleared at INIT) */
#define L07_UNUSED_B8            0x0B8   /* never read, only cleared at INIT */
#define L07_ZONE_ENTER_LOCK      0x0BC   /* 1 = zone 0x21/0x22 entry handled, reset when the hero engages */
#define L07_SCENE_100            0x100   /* 1 = one-shot scene armed */
#define L07_PHEDORIA_APPROACH    0x114   /* 1 = Phedoria approach handled */
#define L07_FLAG_750             0x750   /* 0/1 scene toggle in the tick body */
#define L07_PHEDORIA_D2_DONE     0x754   /* 5 = L07D02 Phedoria scene done, 0 = pending */
#define L07_D22_STAGE            0x758   /* L07D22..29 talk stage 0..2 */
#define L07_PHEDORIA_TALK_A      0x75C   /* Phedoria talk chain A (L07D60A..64A) stage 0..3 */
#define L07_PHEDORIA_TALK_B      0x760   /* Phedoria talk chain B (L07D70A..72) stage 0..3 */
#define L07_PHEDORIA_ARRIVED     0x764   /* 1 = Phedoria arrived (L07D59A/B), shared by zones 3 and 25 */
#define L07_FLAG_82C             0x82C   /* 1 = start preset set, read by zone 21 */
#define L07_ZED_REF_B            0x8D4   /* read only: compared +-5 with L07_ZED_REF_A to choose L07D01A/B/C (written by other rooms) */
#define L07_ZED_REF_A            0x8E0   /* read only: see L07_ZED_REF_B */
#define L07_D16_SAID             0x8FC   /* 1 = L07D16 said */
#define L07_PHEDORIA_BLOCK       0x900   /* 1 = Phedoria blocks the passage */
#define L07_L2_SPAWN             0xAFC   /* arrival spot: 1..4 set by start points, 3/4 selected when leaving */
#define L07_ZED_BRIEFING         0xBDC   /* Zed briefing flag: 0 = Zed27 scene active, 2/3 = done */
#define L07_ZED27_TRAINING       0xE5C   /* Zed27 training progress 0..5 (4 = ready, 5 = fought) */
#define L07_PHEDORIA_TALK_STAGE  0xE60   /* Phedoria L07D11..30 talk stage 0..3 */
#define L07_PHEDORIA_FIGHT       0xEB8   /* read only: 0 = Phedoria fight pending (L07D02 scene) */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu07_trig.c). */
void l07_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu07_trig.c):
 *   l07_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l07_trig00(void);

#endif /* DKE_LIEU07_INT_H */
