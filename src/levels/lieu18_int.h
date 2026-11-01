#ifndef DKE_LIEU18_INT_H
#define DKE_LIEU18_INT_H

/*
 * lieu18_int.h — shared internals of the LIEU18.DLL port, split between
 * lieu18.c (entry, INIT, tick, helpers) and lieu18_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002AD5C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu18_trig.c). */
void l18_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu18_trig.c):
 *   l18_trig00 = trigger 0, sub_10007400 (thunk sub_10001078; INIT default) */
void l18_trig00(void);

/* sub_10013756 (thunk sub_10001019): the "PRE80" weapon-warning scene of
 * persona sp with the hero-side clips of g. Nothing in the DLL calls it. */
void l18_pre80_scene(i32 g, i32 sp);

/* Level 18 script variables (Armal's inn; scratch slots 0x4C..0x150 are per level) */
#define L18_MALROT2_TALK_STAGE             0x04C   /* Malrot2 first talk before the fight: 0 not yet, 666 done (L18D274) */
#define L18_UNUSED_050                     0x050   /* reset at INIT, never used */
#define L18_ARMAL_ARRIVAL_LOCK             0x054   /* 1 = Armal arrival scene (camera 1) played; reset when Armal returns to his post */
#define L18_MALROT2_FIGHT_STARTED          0x058   /* 1 once the Armal and Malrot2 fight scene was set up */
#define L18_DOOR_ZONE_PRESSED              0x064   /* which door zone the hero used before the inn scene: 1 = zone 7, 2 = zone 8 */
#define L18_ARMAL_TALKED                   0x068   /* 1 once Armal's first inn talk (L18D49/50) played, or after leaving the inn */
#define L18_ARMAL_STANCE_STATE             0x06C   /* weapon-stance reaction of Armal: 0 none, 1 warned (L18D10), 2 fight */
#define L18_MALROT2_ENCOUNTER_STEP         0x070   /* Malrot2 encounter: 0 idle, 1 coming, 2 talking, 10 struck, 20 fought, 1000 over */
#define L18_ARMAL_PATROL_STEP              0x074   /* Armal behaviour step: 0 at post, 1..2 approach, 5 fight, 10/40/60 scene steps */
#define L18_INN_MENU_CHOICE                0x078   /* answer of the inn menu (DV_INN_MENU): 1 or 3 = talk/sleep, 2 = rest */
#define L18_INN_RESULT                     0x07C   /* which inn action ran: 1 or 2 */
#define L18_ARMAL_REMARK_SEEN              0x080   /* 0 first remark (L18D258), 1 repeat (L18D257) */
#define L18_ARMAL_MOOD                     0x084   /* Armal mood: 0 normal, 1 angry (weapon drawn), 2 calmed */
#define L18_DOOR_RETURN_STATE              0x088   /* camera/door return state: 0 none, 1..3 which camera lock the hero left by */
#define L18_MALROT2_SEQ_STEP               0x090   /* Malrot2 walk sequence: 0 idle, 1..5 steps, 1000 reset */
#define L18_MALROT2_NEAR                   0x094   /* 1 while Malrot2 talks near zones 3/6 */
#define L18_MALROT2_REMARK                 0x098   /* 0 first remark, 1 repeated (L18D142..145) */
#define L18_CHEST_ANIM_ON                  0x0B8   /* 1 while the chest animation 8 plays (camera 2) */
#define L18_ZONE14_REMARK_DONE             0x0BC   /* 1 once the zone 14 remark was made */
#define L18_ZONE18_REMARK_DONE             0x0D4   /* 1 once the zone 18 remark was made */
#define L18_EVENT52_ARMED                  0x0D8   /* 1 once the Armal/Malrot2 strike armed event 52 */
#define L18_MALROT2_GREETED                0x0DC   /* 1 after Malrot2's first greeting (L18D189); 0 plays L18D189, 1 plays L18D188 */
#define L18_ARMAL_FIGHT_REQ                0x0E0   /* 1 = fight requested (event 51), 2 = fight started */
#define L18_ARMAL_ATTACKED                 0x0F0   /* 1 = hero hit Armal, 2 = reaction played */
#define L18_CAMERA_ZONE_STATE              0x0F4   /* 1 or 2 = which camera lock was last entered */
#define L18_FIGHT_REQ_ZONE                 0x0F8   /* 1 = fight requested from the zone 9 trigger */
#define L18_FIGHT_REQ_TRIG                 0x0FC   /* 1 = fight requested from the fight trigger (with L18_ARMAL_WARN) */
#define L18_ZONE2_REMOVED                  0x100   /* 1 = zone 2 removed; restored when the hero walks away */
#define L18_TRIG2_REMARK_DONE              0x114   /* 1 once the trigger 2 remark was made */
#define L18_SORDOS_LOOTED                  0x11C   /* 0 Sordos corpse not searched, 1 searched */
#define L18_UNUSED_150                     0x150   /* set to 0 when SOB77E is taken; never read */
#define L18_ARMAL_ANGER_COUNT              0x31C   /* counter of angry replies (max 6) in Armal's talk */
#define L18_ENTRY_STEP                     0x4DC   /* 1 or 3 on entry makes INIT set L18_FLAG_0F24 (if 0F78 == 0) and step to 2 (also used by L19, L30, L49) */
#define L18_ARMAL_FLAG_0570                0x570   /* story flag tested == 2 in Armal's late talk (L18D284) */
#define L18_MALROT2_TALK_B                 0x800   /* Malrot2 talk while Armal story stage < 2: 0 first, 1 repeat */
#define L18_ARMAL_DIALOGUE_SEEN            0x8A4   /* 1 once the stage-2 Armal approach played */
#define L18_ARMAL_L18D123_SEEN             0xB1C   /* 1 once Armal's L18D123 speech played */
#define L18_ARMAL_TALK_MODE                0xC0C   /* Armal talk mode: 0 normal, 1 angry, 2 reply, 3 after inn, 4 wants payment, 100 pending */
#define L18_MALROT_BRIEF_STAGE             0xC14   /* set to 2 by start point 11; 2 -> 3 in Armal's talk */
#define L18_CHEST_STAGE                    0xC24   /* chest SOB77E: 0 closed, 1 struck, 2 open, 3 emptied */
#define L18_MALROT_CHASE_STAGE             0xC28   /* Malrot2/6/7 chase: 0 none, 1 started, 2 Armal flees, 6/7 Malrot6 walking, 10 over */
#define L18_FLAG_0D88                      0xD88   /* story flag tested != 0 in Armal's talk */
#define L18_ARMAL_INTRO_STAGE              0xD9C   /* arrival lines: 0 none, 1 Malrot2 greeted, 2 Armal introduced */
#define L18_EVENT52_DONE                   0xDA4   /* 1 once event 52 was handled */
#define L18_FIGHT_PHASE                    0xDA8   /* fight phase: 0 none, 1 started, 3 running */
#define L18_TALK_ARMAL_B                   0xDAC   /* Armal talk B stage, 0..1 */
#define L18_MAIN_QUEST_STAGE               0xDB0   /* Armal main quest: 0 start, 1 accepted, 2..3 talks, 4 Dhorkan11 trouble, 5 inn night, 6 done talk, 7 final */
#define L18_ARMAL_L18D54_SEEN              0xDB4   /* 1 once L18D54 was played */
#define L18_ARMAL_L18D90_SEEN              0xDC0   /* 1 once L18D90 was played */
#define L18_ARMAL_L18D263_SEEN             0xDC4   /* 1 once L18D263/264 was played */
#define L18_FINAL_REMARK_A                 0xDC8   /* 0 L18D84 not yet played, 1 played */
#define L18_FINAL_REMARK_B                 0xDCC   /* 0 L18D87 not yet played, 1 played */
#define L18_INN_STATE                      0xDD0   /* inn state: 1 booked, 2 slept, 0 reset */
#define L18_BOTH_KNOCKED_OUT               0xF10   /* 1 once Armal and Malrot2 were both down */
#define L18_FLAG_0F24                      0xF24   /* flag raised by the entry step if L18_FLAG_0F78 == 0 (see L19) */
#define L18_MALROT2_FLAG_0F4C              0xF4C   /* set to 1 when Malrot2 is questioned while Armal stage is 0 or 1 */
#define L18_ARMAL_L18D275_SEEN             0xF50   /* 1 once L18D275/277 played */
#define L18_INN_ACTION                     0xF70   /* which inn action Armal handles next: 1 = talk, 2 = rest */
#define L18_FLAG_0F78                      0xF78   /* flag that blocks L18_FLAG_0F24 when set */

#endif /* DKE_LIEU18_INT_H */
