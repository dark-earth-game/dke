#ifndef DKE_LIEU24_INT_H
#define DKE_LIEU24_INT_H

/*
 * lieu24_int.h - shared internals of the LIEU24.DLL port, split between
 * lieu24.c (entry, INIT, tick, helpers) and lieu24_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x10021C9C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu24_trig.c). */
void l24_trigger(i32 idx);

/* Trigger handlers the core also calls directly (defined in lieu24_trig.c):
 *   l24_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default)
 *   l24_trig11 = trigger 11, sub_1000D1BA (thunk sub_10001028; the tick's
 *                battle-cry branch re-runs it) */
void l24_trig00(void);
void l24_trig11(void);

/* DLL global shared by the tick body and trigger 8 (lieu24.c): dword_1001DB34
 * (.data, starts 0). Thanandar's walk-back-to-the-hero step: 0 idle, 1 walking,
 * 2 done. */
extern i32 g_l24_heisgoing;

/* Value the engagement slot (V_ENGAGED_PERSONA) holds while the Guardian
 * fight is on; the DLL compares it against the literal 0x56. */
#define L24_ENGAGED_GUARDIAN 0x56

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L24_THANANDAR_DIALOGUE_KIND        0x04C  /* which fight/reconcile dialogue set the Thanandar24 battle cry uses: 0 none, 1 hostile, 2 after the warning (L24D08) */
#define L24_THANANDAR_APPROACH_STEP        0x064  /* Thanandar24 greeting chain (Arkhan2/3, step >= 4): 0 idle, 1 hero near, 2 talking/following, 3 hero left again */
#define L24_THANANDAR_CHAT_COUNT           0x068  /* 0 = first chat (L24D12/13), 1 = repeated chat (L24D14A/B) */
#define L24_THANANDAR_WEAPON_WARNED        0x06C  /* 1 once Thanandar24 warned about the drawn weapon (L24D08/D09), back to 0 when sheathed */
#define L24_TRIG08_TALK_STEP               0x070  /* step of the trigger 8 talk at step ARKHAN23_STEP == 5 (0 first, 1 said, 2 done) */
#define L24_THANANDAR_MOOD                 0x074  /* Thanandar24: 0 calm, 1 first meeting, 2 warned, 4 reconciled, 10 struck by the hero */
#define L24_ZONE29_LOOK_COUNT              0x078  /* times the hero looked at the trigger 29 object (LCOD29/30 spoken while <= 2) */
#define L24_THANANDAR_ESCORT_STATE         0x07C  /* 0 none, 1 first chat done, 3 second talk done (L24D20) */
#define L24_THANANDAR_ARRIVAL_STEP         0x080  /* Thanandar24 walking to meet the hero: 1 start, 2 walking, 3 attacks, 10/1000 transient */
#define L24_UNUSED_84                      0x084  /* only zeroed at INIT, never read in this level */
#define L24_THANANDAR_TALK_ABORTED         0x088  /* set to 1 by trigger 8 when the talk was cut short */
#define L24_UNUSED_90                      0x090  /* only zeroed at INIT, never read in this level */
#define L24_THANANDAR_WALK_BACK            0x094  /* Thanandar24 walks back to the hero: 1 walking, 2 arrived */
#define L24_THANANDAR_MET                  0x098  /* 1 once the Thanandar24 greeting started */
#define L24_FIGHT_PAIR_MODE                0x0B8  /* who fights: 1 Thanandar24, 2 Konkalite1, 3 both (set by trigger 41) */
#define L24_GUARDIAN16_AFTERMATH           0x0BC  /* 1 = Guardian16 beaten, camera scene pending; 2 = done */
#define L24_ZONE34_ONCE                    0x0D4  /* one-shot guard for the zone 34 event */
#define L24_UNUSED_D8                      0x0D8  /* only zeroed at INIT, never read in this level */
#define L24_UNUSED_DC                      0x0DC  /* only zeroed at INIT, never read in this level */
#define L24_UNUSED_E0                      0x0E0  /* only zeroed at INIT, never read in this level */
#define L24_UNUSED_F0                      0x0F0  /* only zeroed at INIT, never read in this level */
#define L24_GUARDIAN16_STATE               0x0F4  /* 0 fresh, 1 hurt, 2 dead */
#define L24_HERO_NEAR_AT_ENTRY             0x0F8  /* 1 when the hero was near Thanandar24 at the scene start */
#define L24_ARRIVAL_ANIM_PAUSE             0x0FC  /* 1 = arrival anim running, 2 = paused while the camera is locked */
#define L24_GUARDIAN16_MOVED               0x100  /* 1 once Guardian16 was repositioned during the fight */
#define L24_DOOR_ANIM_DONE                 0x114  /* exit door: 0 = opening anim not yet played, 1 = played (second event 37 changes room) */
#define L24_OB86_TAKEN                     0x6D0  /* 1 once OB86 was picked up */
#define L24_OB91A_TAKEN                    0x6E4  /* 1 once OB91A was picked up (SG00PRE07) */
#define L24_UNUSED_734                     0x734  /* zeroed at INIT only */
#define L24_THANANDAR_SEARCHED             0x8E4  /* 1 once the dead Thanandar24 was searched (fouille) */
#define L24_ENGAGED_SAVED                  0x940  /* V_ENGAGED_PERSONA saved when leaving the room (restored on return: 0x24xx Thanandar24, 0x71 Konkalite1) */
#define L24_TRIG08_TALK_A                  0x958  /* one-shot step of the trigger 8 talk (0 -> 2) */
#define L24_TRIG08_TALK_B                  0x95C  /* one-shot guard of the trigger 8 talk */
#define L24_ZONE9_ONCE                     0xA58  /* one-shot guard of the trigger 9 scene (Guardian16) */
#define L24_THANANDAR_ENCOUNTER            0xA5C  /* encounter stage 0..2 (start points 7/9 preset 1; 2 = talked) */
#define L24_THANANDAR_STRIKE_STAGE         0xA60  /* reaction to a hit on Thanandar24: 0 none, 1 warned, 2 repeated */
#define L24_THANANDAR_WARN_STAGE           0xA64  /* weapon warning: 0 none, 1 warned once, 2 warned twice or more */
#define L24_KONKALITE_ZONE_STAGE           0xA68  /* trigger 11 scene: 0 first, 1 second, 2 done */
#define L24_ZONE20_ONCE                    0xA74  /* one-shot guard of the trigger 20 scene */
#define L24_ZONE25_LOOK_STAGE              0xA78  /* trigger 25 object: 0 first message (LCOD13), 1 repeat (LCOD16) */
#define L24_ARKHAN23_STEP                  0xBA8  /* Arkhan2/3 storyline step 0..8 (2 meet Thanandar24, 4 reconciled, 5/6 later) */
#define L24_THANANDAR_FIGHT_STATE          0xBB0  /* 0 not fought, 1 fighting, 2 dead */
#define L24_THANANDAR_COMPANION            0xBB4  /* 1 = Thanandar24 joined/follows the hero (set when he dies before step 6) */
#define L24_KONKALITE_FIGHT_STATE          0xBB8  /* 1 = Konkalite1 fights with Thanandar24, 2 = Konkalite1 beaten */
#define L24_ZONE34_DONE                    0xC4C  /* 1 once the zone 34 event ran */
#define L24_ZONE_DIALOGUE_STAGE            0xDE0  /* trigger 11 first dialogue: 0 not said, 1 said (L24D21), 2 done */
#define L24_THANANDAR_VISIT_COUNT          0xE14  /* 0..4 visits/warnings counted at BA8 == 6; 2+ makes Thanandar24 hostile */
#define L24_THANANDAR_PROVOKED             0xE18  /* set by other rooms; 1 = Thanandar24 attacks at once */
#define L24_ENTERED_FROM_27                0xE3C  /* 1 when arriving from room 27 or after the trigger 8 scene */
#define L24_THANANDAR_KILLED_MARK          0xF24  /* set to 5 when Thanandar24 dies */
#define L24_THANANDAR_GREETED              0xF2C  /* 1 once Thanandar24 greeted the hero */

#endif /* DKE_LIEU24_INT_H */
