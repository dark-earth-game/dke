#ifndef DKE_LIEU50_INT_H
#define DKE_LIEU50_INT_H

/*
 * lieu50_int.h - shared internals of the LIEU50.DLL port, split between
 * lieu50.c (entry, INIT, tick, events, helpers) and lieu50_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002248C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu50_trig.c). */
void l50_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu50_trig.c):
 *   l50_trig00 = trigger 0, sub_10001200 (thunk 0x1000106E; INIT default) */
void l50_trig00(void);

/* Level-50 script variables (a copy of the level-12 script: Bandor and Khorad, Master Builder's room). Slots 0x64-0x100 are level scratch; the rest are game-wide story state. */
#define L50_INTRO_LINE_PICK        0x64  /* random(3) pick of the start-up greeting line (L12D03/04/05) */
#define L50_DIAGRAM_REACTION_DONE  0x68  /* 1 = Bandor/Khorad reaction to the diagram (OB70) or to the Bandor-diagram weapon already played */
#define L50_STANCE_WARNED          0x6C  /* 1 = guards warned the armed hero standing close (combat stance); cleared when the stance drops */
#define L50_TALK_TARGET            0x70  /* which guard the hero last talked to: 0 = Bandor, 1 = Khorad */
#define L50_CRY_PHASE              0x74  /* guard cry for help after the hero's cry key: 83 = cry pending, 99 = cry spoken (rearmed to 83 next tick) */
#define L50_LEVER_PAUSE_ARMED      0x78  /* 1 = lever anim (slot 0) may be paused the next time locked camera 3 is on; 0 once paused */
#define L50_END_GAME_TIMER_SET     0x7C  /* 1 = hero died, the EV_END_GAME timer is armed */
#define L50_LEVIER_PAUSED          0x80  /* 1 = lever anim (slot 1) paused at locked camera 0 after the guard fight; cleared on camera change */
#define L50_QUIP_TOGGLE_84         0x84  /* alternates 0/1 between two Bandor/Arkhan2 line sets in stage 3 */
#define L50_QUIP_TOGGLE_88         0x88  /* alternates 0/1 between Bandor lines L12D18A / L12D18B */
#define L50_APPROACH_SCENE         0x100  /* 666 = hero walked close to the guards and the approach scene (SP07MAR29...) was played */
#define L50_WEAPON_SCENE_CAM       0x180  /* 1 = scripted Bandor-diagram weapon scene running (camera is locked, so no auto camera at its end) */
#define L50_OB21B_TAKEN            0x184  /* 1 = object OB21B taken from the shrine */
#define L50_SHRINE_USED            0x4BC  /* 1 = shrine lever pulled (first-use gate; always set together with 0x4C0) */
#define L50_SHRINE_OPEN            0x4C0  /* 1 = shrine lever pulled, so its compartment can be searched */
#define L50_OB12_TAKEN             0x5A8  /* 1 = object OB12 taken (from Bandor's body or the shrine) */
#define L50_DIAGRAM_TAKEN          0x690  /* 1 = OB70 (Master Builder diagram) taken */
#define L50_FIRST_TALK_STAGE       0x80C  /* first Bandor/Khorad conversation stage 0-3 (also gates the approach scene while 0) */
#define L50_UNUSED_810             0x810  /* never written; always 0 */
#define L50_LATE_TALK_STAGE        0x814  /* conversation stage 0-3 while L50_TALK_STAGE is 4 (Leona / bunker questions) */
#define L50_STRIKE_WARNED          0x818  /* 0 = Bandor not yet warned the hero for striking, 1 = warned (next strike starts the fight) */
#define L50_LEONA_SEEN             0xB0C  /* 1 = hero has met Leona (changes Bandor's dialogue); set outside this level */
#define L50_GREETING_STAGE         0xC18  /* 0 = none, 1 = first greeting pending (L12D01/02), 2 = greeted (random greeting at start-up) */
#define L50_TALK_STAGE             0xC1C  /* Bandor/Khorad story conversation stage 0-4 (0 first meeting ... 4 Leona/bunker questions) */
#define L50_BUNKER_SEEN            0xC20  /* 1 = hero saw the bunker door in the Darklands, 2 = told (shared with levels 12, 13, 49, 53) */
#define L50_TALK_MARK_CB0          0xCB0  /* set to 2 after the Bandor/Khorad warning talk; also used by levels 12, 14, 54 */
#define L50_TALK_MARK_CB4          0xCB4  /* set to 1 after Arkhan3's apology talk; also used by levels 12, 14, 54 */
#define L50_CRYSTAL_PROGRESS       0xED0  /* crystal quest step 0-3 (5 = done), advanced when the hero shows proof (shared with levels 4, 5, 12, 15, 32, 49) */
#define L50_PROOF_SHOWN            0xEDC  /* 1 = hero showed the crystal bow-key as proof (shrine loot is then OB21B, not OB12) */
#define L50_ALT_MUSIC              0xEFC  /* 1 = alternate music set and room music variant 2 (start point 2); also levels 12, 13, 53 */

#endif /* DKE_LIEU50_INT_H */
