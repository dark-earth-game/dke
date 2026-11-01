#ifndef DKE_LIEU05_INT_H
#define DKE_LIEU05_INT_H

/*
 * lieu05_int.h — shared internals of the LIEU05.DLL port, split between
 * lieu05.c (entry, INIT, tick, helpers) and lieu05_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002512C. */

/* Script variables of level 5: state block slots this level reuses as its own scratch (not game-wide). */
#define L05_BOGDARAN_FIGHT_STEP  0x04C   /* Bogdaran fight cleanup/arming: 1 Bogdaran attacks, 3 hero left the zone; cleared when he dies */
#define L05_D150_ARMED           0x050   /* 1 while the L05D150A strike warning plays (avoids repeating it) */
#define L05_BOGDARAN_TALKING     0x064   /* 1 = Bogdaran came to the hero for a talk, 0 = back to patrol */
#define L05_BOGDARAN_RETURN      0x068   /* 1 = Bogdaran allowed to walk back to his spot, 0 = talk pending */
#define L05_BOGDARAN_MET         0x06C   /* 0 = not met yet, 1 = met (L05D05/06 intro played) */
#define L05_UNUSED_70            0x070   /* never read, only cleared at INIT */
#define L05_D65_SAID             0x074   /* 1 = L05D65 / L05D78 line (B7C stage 2) already played */
#define L05_UNUSED_78            0x078   /* never read, only cleared at INIT */
#define L05_TRIG3_TALK_STAGE     0x07C   /* zone 3 talk (L05D162..170): 0 first, 1 / 2 follow-ups */
#define L05_QUIP_PICK            0x080   /* random Bogdaran quip 1..3 (L05D14/15/16) */
#define L05_WEAPON_GIVE_STEP     0x084   /* 1 = Bogdaran hands over the weapon (L05D17..20), 2 done */
#define L05_D13_STEP             0x088   /* 1 = L05D13A/B queued, 2 played */
#define L05_UNUSED_90            0x090   /* never read, only cleared at INIT */
#define L05_DELIA_FOLLOW_STEP    0x094   /* Delia joins after the fight: 1 talked, 2 stance down, 3 walking, 4 following; 5/10 set by triggers */
#define L05_D151_STAGE           0x098   /* Bogdaran dialogue L05D151/152 stage 0..2 */
#define L05_DELIA_D21_STEP       0x0B8   /* 1 = Delia walks to the hero, 2 = L05D21A..C said */
#define L05_UNUSED_BC            0x0BC   /* never read, only cleared at INIT */
#define L05_D201_SAID            0x0D4   /* 1 = L05D201 said */
#define L05_BOGDARAN_LEFT        0x0D8   /* Bogdaran walking-away phase: 0 free, 1/2/3 steps */
#define L05_FIRST_TALK_DONE      0x0DC   /* 1 = Arkhan2/3 first talk with Bogdaran (L05D11A/12B) done */
#define L05_UNUSED_E0            0x0E0   /* never read, only cleared at INIT */
#define L05_TALK_IN_FIGHT_OK     0x0F0   /* 1 = dialogue allowed while in combat stance */
#define L05_OB16_STATE           0x5B8   /* OB16 object: 0 present, 1/2 taken or used */
#define L05_D48_SAID             0x728   /* 1 = L05D48 said */
#define L05_BOGDARAN_TOGGLE      0x744   /* alternates 0/1 on each approach (SP08MAR29 variants) */
#define L05_D22_STAGE            0x748   /* L05D22 / 23A / 23B stage 0..2 */
#define L05_D150B_SAID           0x74C   /* 1 = L05D150B said */
#define L05_DELIA_FIGHT_PHASE    0x81C   /* Delia / Bogdaran duel phase 0..2 */
#define L05_D114_STAGE           0x820   /* L05D114..118 talk done */
#define L05_D45_STAGE            0x890   /* L05D45A/B and L05D72 stage 0..2 */
#define L05_D56_SAID             0x894   /* 1 = L05D56 said */
#define L05_D62_SAID             0x89C   /* 1 = L05D62 said */
#define L05_D150_FOLLOWUP        0x8AC   /* 1 = L05D150A/B follow-up played */
#define L05_D142_SAID            0x8F0   /* 1 = L05D142A..C said */
#define L05_BOGDARAN_D13_STATE   0x8F4   /* 1 = hero struck, 2 = L05D13A/B played */
#define L05_D53_SAID             0x8F8   /* 1 = L05D53A/B said */
#define L05_FLAG_B08             0xB08   /* start point preset 0/1, no reader here */
#define L05_BOGDARAN_QUEST       0xB7C   /* Bogdaran quest stage: 0 start, 2 L05D65, 3/4 later talks, 5 weapon given, 10 done */
#define L05_BOGDARAN_STAGE       0xB9C   /* Bogdaran talk/fight stage: 0 first, 1 greeted, 10/11/12 fight armed */
#define L05_DUEL_MODE            0xC60   /* 1 = Delia fights Bogdaran, 2 = Delia dead and Bogdaran fights the hero */
#define L05_D72_STAGE            0xC68   /* L05D72 / L05D68 talk stage 0..2 */
#define L05_D171_SAID            0xDD4   /* 1 = L05D171 said */
#define L05_D59_SAID             0xDD8   /* 1 = L05D59 said */
#define L05_START_PRESET_E3C     0xE3C   /* start point preset 1 / 2 (read by later rooms) */
#define L05_FLAG_E40             0xE40   /* read only: 0 enables L05D48 line (written by other rooms) */
#define L05_FLAG_EB0             0xEB0   /* read only: nonzero enables L05D59 line (written by other rooms) */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu05_trig.c). */
void l05_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu05_trig.c):
 *   l05_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l05_trig00(void);

#endif /* DKE_LIEU05_INT_H */
