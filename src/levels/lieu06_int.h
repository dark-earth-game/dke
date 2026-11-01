#ifndef DKE_LIEU06_INT_H
#define DKE_LIEU06_INT_H

/*
 * lieu06_int.h — shared internals of the LIEU06.DLL port, split between
 * lieu06.c (entry, INIT, tick, helpers) and lieu06_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002192C. */

/* Script variables of level 6: state block slots this level reuses as its own scratch (not game-wide). */
#define L06_UNUSED_64            0x064   /* never read, only cleared at INIT */
#define L06_DHORKAN_CHASE_STEP   0x068   /* Dhorkan11 chase after the L06D06F warning: 0 not started, 1 walking, 2 arrived and fighting */
#define L06_DHORKAN_INTRO_STEP   0x06C   /* Dhorkan11 intro for Arkhan1: 0 pending, 1 armed, 2 played */
#define L06_DHORKAN_TALK_STEP    0x070   /* Dhorkan11 talk counter, bumped by each scripted line (set to 3 during the L06D06 chain) */
#define L06_DHORKAN_EVENT7_STEP  0x074   /* step of the event 7 chain L06D06A..F: 0 idle .. 5 done */
#define L06_D03_DONE             0x078   /* 1 = L06D03 talk done (blocks the second intro) */
#define L06_D52_STAGE            0x07C   /* zone 4 talk stage (L06D52/53/80C): 0 first, 1 repeated */
#define L06_ARRIVE_DONE          0x080   /* 1 = arrival animation played */
#define L06_D07_STEP             0x084   /* L06D07 sequence step 0..3 */
#define L06_D61_SAID             0x088   /* 1 = L06D61A/B said */
#define L06_D670_STAGE           0x090   /* zone 2 L06D670 stage 0..3 */
#define L06_CAMERA_HOLD          0x094   /* 1 = camera held on Dhorkan11 until the up/down key is pressed */
#define L06_D84_SAID             0x098   /* 1 = L06D84 said */
#define L06_DHORKAN_HP_TMP       0x0B8   /* scratch: Dhorkan11 HP read while applying a hit */
#define L06_TALK_DONE_BC         0x0BC   /* 0x29A = talk finished (trigger 4), 0 = not yet */
#define L06_DHORKAN_POS_X        0x0D4   /* Dhorkan11 position X saved at the chase start */
#define L06_DHORKAN_POS_Y        0x0D8   /* Dhorkan11 position Y saved at the chase start */
#define L06_TAUNT_INDEX          0x0DC   /* next Dhorkan11 taunt line 0..5 (wraps) */
#define L06_E0_FLAG              0x0E0   /* trigger 4 one-shot flag 0/1 */
#define L06_EVENT_6              0x338   /* event 6 step (0x320 + 4*6); set with EVENT_10 by the Guardian4 check */
#define L06_EVENT_10             0x348   /* event 10 step (0x320 + 4*10) */
#define L06_DHORKAN_MET          0x358   /* Dhorkan11 first meeting: 0 not yet, 2 happened */
#define L06_UNUSED_35C           0x35C   /* never read, only cleared at INIT */
#define L06_D51_SAID             0x4FC   /* 1 = L06D51A..C / L06D209B..210 said */
#define L06_D55C_SAID_A          0x5CC   /* 1 = L06D55C said in zone 6 */
#define L06_D55C_SAID_B          0x6EC   /* 1 = L06D55C said in zone 7 */
#define L06_D17A_SAID            0x72C   /* 1 = L06D17A said */
#define L06_D21A_SAID            0x730   /* 1 = L06D21A said */
#define L06_DHORKAN_ENGAGED      0x734   /* 1 = Dhorkan11 fight engaged (scripted) */
#define L06_D10_STAGE            0x73C   /* L06D10A..D / L06D54A stage 0..2 */
#define L06_DHORKAN_STORY        0x740   /* Dhorkan11 story stage 0..5 (selects the L06D43/44/49 lines) */
#define L06_D82_FLAG             0x7C0   /* 1 = L06D82 scene played */
#define L06_TAUNT_VARIANT        0x944   /* nonzero selects taunt variant B (L06D18C..) */
#define L06_RETURN_PHASE         0xAF8   /* 0 first pass, 1 return pass */
#define L06_D66_STAGE            0xB1C   /* L06D66/68/69 stage 0/1 */
#define L06_ROOM_STAGE_B34       0xB34   /* stage of the room 6 events: 1, 2 (selects exit/entry variants) */
#define L06_FLAG_B4C             0xB4C   /* read only: >= 3 changes the L06D82 scene (written by other rooms) */
#define L06_GUARDIAN4_STATE      0xB98   /* Guardian4 encounter state: 2 -> reset to 0 while he is alive, 4 with AF8==1 sets events 6/10 */
#define L06_FLAG_CA0             0xCA0   /* read only: == 5 enables L06D669/L06D82 variant */
#define L06_FORCE_DEFAULT_INIT   0xE8C   /* 1 = run default INIT even when the hero is dead */
#define L06_FLAG_EB0             0xEB0   /* read only: nonzero changes the zone 4 talk */
#define L06_PHEDORIA_FIGHT       0xEB8   /* Phedoria/Dhorkan11 guard stage: < 2 = fight not over, 2 = over */
#define L06_FLAG_F24             0xF24   /* read only: compared with 2 for L06D669 */
#define L06_D51_STAGE            0xF2C   /* zone 4 talk stage 0..2 (L06D48A, L06D51) */
#define L06_TRIG2_FLAG_F60       0xF60   /* trigger 2 flag 0/1 */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu06_trig.c). */
void l06_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu06_trig.c):
 *   l06_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l06_trig00(void);

/* Dhorkan's taunt line cycle (lieu06.c), inlined by the DLL at each site. */
void l06_taunt_a(void);
void l06_taunt_b(void);

#endif /* DKE_LIEU06_INT_H */
