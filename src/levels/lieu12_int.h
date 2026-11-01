#ifndef DKE_LIEU12_INT_H
#define DKE_LIEU12_INT_H

/*
 * lieu12_int.h — shared internals of the LIEU12.DLL port, split between
 * lieu12.c (entry, INIT, tick, helpers) and lieu12_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_100245BC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu12_trig.c). */
void l12_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu12_trig.c):
 *   l12_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l12_trig00(void);

/* Level 12 script variables (same layout as level 50, a copy of this script; slots 0x64-0x100 are level scratch, the rest game-wide story state) */
#define L12_INTRO_LINE_PICK       0x064   /* random(3) pick of the start-up greeting line (L12D03/04/05) */
#define L12_DIAGRAM_REACTION_DONE 0x068   /* 1 = Bandor/Khorad reaction to the diagram (OB70) or to the Bandor-diagram weapon already played */
#define L12_STANCE_WARNED         0x06C   /* 1 = guards warned the armed hero standing close (combat stance); cleared when the stance drops */
#define L12_TALK_TARGET           0x070   /* which guard the hero last talked to: 0 = Bandor, 1 = Khorad */
#define L12_CRY_PHASE             0x074   /* guard cry for help after the hero's cry key: 83 = cry pending, 99 = cry spoken (rearmed to 83 next tick) */
#define L12_LEVER_PAUSE_ARMED     0x078   /* 1 = lever anim (slot 0) may be paused the next time locked camera 3 is on; 0 once paused */
#define L12_END_GAME_TIMER_SET    0x07C   /* 1 = hero died, the EV_END_GAME timer is armed */
#define L12_LEVIER_PAUSED         0x080   /* 1 = lever anim (slot 1) paused at locked camera 0 after the guard fight; cleared on camera change */
#define L12_QUIP_TOGGLE_84        0x084   /* alternates 0/1 between two Bandor/Arkhan2 line sets in stage 3 */
#define L12_QUIP_TOGGLE_88        0x088   /* alternates 0/1 between Bandor lines L12D18A / L12D18B */
#define L12_BANDOR_WALK_STEP      0x090   /* 0 at INIT, 2 = set by the tick once Bandor reaches his spot (C1C stage 2), event 2 advances it by modify_var */
#define L12_BANDOR_REPOSITIONED   0x094   /* 1 = event 2 already sent Bandor and Khorad to their story spots (Bandor facing the right way) */
#define L12_APPROACH_SCENE        0x100   /* 666 = hero walked close to the guards and the approach scene (SP07MAR29...) was played */
#define L12_WEAPON_SCENE_CAM      0x180   /* 1 = scripted Bandor-diagram weapon scene running (camera is locked, so no auto camera at its end) */
#define L12_OB21B_TAKEN           0x184   /* 1 = object OB21B taken from the shrine */
#define L12_SHRINE_USED           0x4BC   /* 1 = shrine lever pulled (first-use gate; always set together with 0x4C0) */
#define L12_SHRINE_OPEN           0x4C0   /* 1 = shrine lever pulled, so its compartment can be searched */
#define L12_OB12_TAKEN            0x5A8   /* 1 = object OB12 taken (from Bandor's body or the shrine) */
#define L12_DIAGRAM_TAKEN         0x690   /* 1 = OB70 (Master Builder diagram) taken */
#define L12_FIRST_TALK_STAGE      0x80C   /* first Bandor/Khorad conversation stage 0-3 (also gates the approach scene while 0) */
#define L12_UNUSED_810            0x810   /* never written; always 0 */
#define L12_LATE_TALK_STAGE       0x814   /* conversation stage 0-3 while L50_TALK_STAGE is 4 (Leona / bunker questions) */
#define L12_STRIKE_WARNED         0x818   /* 0 = Bandor not yet warned the hero for striking, 1 = warned (next strike starts the fight) */
#define L12_LEONA_SEEN            0xB0C   /* 1 = hero has met Leona (changes Bandor's dialogue); set outside this level */
#define L12_GREETING_STAGE        0xC18   /* 0 = none, 1 = first greeting pending (L12D01/02), 2 = greeted (random greeting at start-up) */
#define L12_TALK_STAGE            0xC1C   /* Bandor/Khorad story conversation stage 0-4 (0 first meeting ... 4 Leona/bunker questions) */
#define L12_BUNKER_SEEN           0xC20   /* 1 = hero saw the bunker door in the Darklands, 2 = told (shared with levels 13, 49, 53) */
#define L12_TALK_MARK_CB0         0xCB0   /* set to 2 after the Bandor/Khorad warning talk; also used by levels 14, 54 */
#define L12_TALK_MARK_CB4         0xCB4   /* set to 1 after Arkhan3's apology talk; also used by levels 14, 54 */
#define L12_ENDING_OVERRIDE       0xE8C   /* game-wide; 1 = skip the hero-death ending in the tick and re-run INIT (set to 1 in L31 near the end) */
#define L12_CRYSTAL_PROGRESS      0xED0   /* crystal quest step 0-3 (5 = done), advanced when the hero shows proof (shared with levels 4, 5, 15, 32, 49) */
#define L12_PROOF_SHOWN           0xEDC   /* 1 = hero showed the crystal bow-key as proof (shrine loot is then OB21B, not OB12) */
#define L12_ALT_MUSIC             0xEFC   /* 1 = alternate music set and room music variant 2 (start point 2); also levels 13, 53 */

#endif /* DKE_LIEU12_INT_H */
