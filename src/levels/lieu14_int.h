#ifndef DKE_LIEU14_INT_H
#define DKE_LIEU14_INT_H

/*
 * lieu14_int.h — shared internals of the LIEU14.DLL port, split between
 * lieu14.c (entry, INIT, tick, helpers) and lieu14_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002BEEC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu14_trig.c). */
void l14_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu14_trig.c):
 *   l14_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l14_trig00(void);

/* Level 14 script variables (low slots up to 0x188 and the 0x4xx-0x6xx ones are level-private; 0xAF8 and above are game-wide story state) */
#define L14_FRAME_COUNTER           0x038   /* engine frame counter; (value % 20) == 0 throttles Delia's follow distance checks */
#define L14_PROVIDER_THREAT_DONE    0x04C   /* 1 = Provider already switched to combat stance when the hero came within 250 */
#define L14_PERSONA35_COMBAT        0x058   /* 1 = persona 35 held in combat stance by the tick while camera 6 is locked */
#define L14_PROVIDER_GREET_ARMED    0x064   /* 1 = hero walked away from Provider (distance > 400), so he repeats L14D263 on return */
#define L14_DEATH_EVENT_SENT        0x068   /* 1 = hero dead and the game-over event (0x2C) was queued */
#define L14_UNUSED_6C               0x06C   /* zeroed by INIT and advanced by modify_var in trigger 38; never read in this level */
#define L14_TALK_RANDOM             0x070   /* random pick (0..2, 0..1) of the Malrot4 reaction animation/line */
#define L14_UNUSED_74               0x074   /* only zeroed by the INIT trigger; never read in this level */
#define L14_PROVIDER_FOLLOWING      0x078   /* 1 = Provider leads towards the hero (distance 200), 0 = returns to his post when the hero is farther than 480 */
#define L14_MALROT_ANIM_STEP        0x07C   /* step 1..5 / 40 of the Malrot1 reaction animation chain run from event 1 */
#define L14_SCRATCH_80              0x080   /* temp: 1 = hero left the room far side (distance > 700), also the hero's HP saved before a 10 HP penalty */
#define L14_TALK_COOLDOWN_84        0x084   /* non-zero while the hero is talking within 400 of zone 25; cleared when he walks away */
#define L14_MALROT1_MEET_DONE       0x088   /* 1 = Arkhan2 / Arkhan3 first meeting with Malrot1 (zone 24) already started */
#define L14_STANCE_WARNED_MALROT4   0x090   /* 1 = Malrot4 warned the armed hero standing in combat stance */
#define L14_UNUSED_94               0x094   /* only zeroed by the INIT trigger; never read in this level */
#define L14_MALROT1_SPOKEN          0x098   /* 1 = Malrot1 talked to the hero near zone 24; cleared after the hero walks 150 away */
#define L14_STANCE_WARNED_MALROT1   0x0B8   /* 1 = Malrot1 warned the armed hero standing in combat stance */
#define L14_DELIA_JOINED            0x0BC   /* 1 = Delia placed next to the hero at zone 33 and following (V_DELIA_PHASE == 4) */
#define L14_DELIA_FOLLOW_ON         0x0D4   /* 1 = Delia follow AI active (V_DELIA_PHASE 3/4); throttled by the frame counter */
#define L14_DELIA_SCENE_D8          0x0D8   /* 1 = Delia scene (V_DELIA_PHASE == 4) at this trigger already played */
#define L14_MALROT4_CRI_STATE       0x0DC   /* 64 = shout (cri) at Malrot4 pending, 99 = consumed */
#define L14_PROVIDER_CRI_STATE      0x0E0   /* 16 = shout (cri) at Provider pending, 99 = consumed */
#define L14_MALROT1_CRI_STATE       0x0F0   /* 61 = shout (cri) at Malrot1 pending, 99 = consumed (rearmed to 16 next tick) */
#define L14_SCENE_F4                0x0F4   /* 1 = scene at trigger 24 (Delia/Malrot) already played */
#define L14_SCENE_F8                0x0F8   /* 1 = scene at trigger 5 already played */
#define L14_AMBIENT_ANIM_ON         0x0FC   /* 1 = ambient anims (slots 0-3) and L14AMB3/4 sound running while camera 2 is locked */
#define L14_AMBIENT_SWAP_DONE       0x100   /* 1 = L14AMB1/2 swapped in and ambient anims paused when camera 2 is entered */
#define L14_DELIA_SCENE_114         0x114   /* 1 = Delia scene (V_DELIA_PHASE == 4) at this trigger already played */
#define L14_PROVIDER_ITEM_GIVEN     0x154   /* set to 3 when Arkhan2 gave object 31 to Provider (L14D233/234) */
#define L14_MALROT1_TALKED          0x168   /* 1 = Arkhan2 first talk with Malrot1 (L14D139) started */
#define L14_PERSONA35_ENGAGED       0x16C   /* 1 = persona 35 became the hero's engaged enemy (first hit) */
#define L14_DELIA_KILLED_14         0x170   /* 1 = Delia was killed in level 14 (V_DELIA_KILLED == 14); stops Malrot4 reactions */
#define L14_MALROT1_TALK_STAGE      0x174   /* Malrot1 talk (trigger 17) 0 = first line, 1 = repeat */
#define L14_TRIG38_TALK_STAGE       0x178   /* 0 = first talk at trigger 38 (LCOD10/11), 1 = later */
#define L14_GUARD_ZONE_OPEN         0x188   /* 1 = Delia phase set to 3 by the start point, so wall col 25 is removed */
#define L14_PROVIDER_ANGRY          0x4C8   /* 1 = Provider is hostile (shout received or fight started) */
#define L14_FIGHT_STARTED           0x4CC   /* 1 = a Malrot/Provider fight has started; blocks the first-meeting conversations */
#define L14_PROVIDER_LOCK           0x538   /* 1 = set once PLOT_EB8 == 2: Provider talk is locked (blocks his special-mode weapon talk) */
#define L14_CAMERA2_LOOT_STAGE      0x560   /* 0/1 loot-or-search state of the zone 21/30 object, reset when camera 2 is entered */
#define L14_CELL_TALK_STAGE         0x6CC   /* 0 = first talk at trigger 16 (LCOD10/11), 1 = repeat */
#define L14_CELL_SEARCH_STAGE       0x6FC   /* 1 -> 2 after the zone 21 object was searched (L14D269/270) */
#define L14_GUARD_ARC_PHASE         0xAF8   /* guardian-arc stage; start points set 1 (game-wide) */
#define L14_PLOT_B10                0xB10   /* game-wide; non-zero (with L14_FLAG_E74 == 0) sets L14_FLAG_E74 when trigger 3 runs */
#define L14_PLOT_B38                0xB38   /* game-wide plot step; 2 -> 3 at INIT */
#define L14_RETURN_ROOM_STEP        0xB4C   /* 0 / 1 which exit room the hero uses when leaving (same slot as L10_RETURN_ROOM_STEP) */
#define L14_PLOT_BA8                0xBA8   /* game-wide plot step; 5 -> 6 at INIT (shared with L10, L23, L24) */
#define L14_PROGRESS_STEP            0xBBC   /* game-wide mission step (same slot as L15_PROGRESS_STEP); 1 -> 2, 4 -> 5 if Malrot5 alive, 4 -> 8 if dead */
#define L14_WEAPON_REACT_STAGE      0xBE0   /* 0 = first time the hero shows a weapon to Provider, 1 = repeat */
#define L14_PROVIDER_ACTIVE         0xBE8   /* game-wide; >= 1 = Provider / Malrot4 follow logic enabled (start points 3 and 4 set 1); 0 with E9C == 10 sets E9C to 40 for Kalhi13 */
#define L14_MALROT1_STORY_STAGE     0xBEC   /* 1 = Malrot1 met, 2 = Malrot1 story told (changes his dialogue at triggers 17, 24) */
#define L14_CAMERA2_ENTERED         0xBF4   /* game-wide; 0 = ambient anims not yet started in the camera-2 room */
#define L14_DANRYS_PLOT_C34         0xC34   /* game-wide; 1 with Danrys dead and V_KALHI13_TALK_C == 3 triggers the Kalhi scene */
#define L14_DELIA_PRESENT           0xC44   /* game-wide; 1 = Delia on screen at this level (start point set it); suppresses the Malrot4 first meeting */
#define L14_MALROT4_REACT_STEP      0xCB0   /* Malrot4 reaction step 0..3 (modify_var), picks the random line */
#define L14_MALROT4_REACT_DONE      0xCB4   /* 1 = Malrot4 reaction already done; reset when the hero is 250 away */
#define L14_MALROT4_ENGAGED         0xCB8   /* game-wide; 1 = Malrot4 is fighting/dead */
#define L14_MALROT4_TALK_STAGE      0xCBC   /* 0 = first talk (L14D00..), 1 = repeat, 2 = long conversation; reset when the hero is 400 away */
#define L14_MALROT4_SECOND_TALK     0xCC0   /* 0..2 second Malrot4 conversation (L14D05, L14D07..), 2 only in special mode */
#define L14_PROVIDER_TALK_STAGE     0xCC4   /* 0 = first talk (L14D28..), 1 = repeat */
#define L14_PROVIDER_STAGE          0xCCC   /* 0 = not met, 1..3 = Provider conversation steps (L14D49/59.. weapon scene); >= 1 enables his greeting */
#define L14_MALROT1_TALK_STAGE_CF0  0xCF0   /* 0..2 Malrot1 conversation at trigger 24 (L14D109..116) */
#define L14_MALROT4_THIRD_TALK      0xDDC   /* 0..2 Malrot4 conversation at zone 2 (L14D206/207), 2 only in special mode */
#define L14_MALROT4_ARKHAN3_TALK    0xDEC   /* 0 = first Arkhan3 talk with Malrot4 (L14D18..), 1 = repeat */
#define L14_PROVIDER_TALK_STAGE_DF0 0xDF0   /* 0 = first special-mode talk with Provider (L14D48.. weapon lines), 1 = done */
#define L14_MALROT4_TALK_STAGE_DF4  0xDF4   /* 0 -> 1 special-mode conversation with Malrot4 (L14D185/217..) */
#define L14_PLOT_E14                0xE14   /* game-wide plot step; 1 -> 2 when BA8 == 6 and arriving from level 10 */
#define L14_PLOT_E24                0xE24   /* game-wide plot step; 2 -> 3 at trigger 3 */
#define L14_PENDING_E34             0xE34   /* pending flag cleared (1 -> 0) by the talk triggers; set by other levels */
#define L14_FLAG_E74                0xE74   /* game-wide flag set to 1 from PLOT_B10 (shared with L2, L20, L23, L26) */
#define L14_ENDING_OVERRIDE         0xE8C   /* game-wide; 1 = skip the hero-death ending in the tick and re-run INIT (set to 1 in L31 near the end) */
#define L14_KALHI_QUEST_STAGE       0xE9C   /* game-wide Kalhi quest; start point 1 sets 10; becomes 40 when Provider event runs without Kalhi13 alive (shared with L10) */
#define L14_PLOT_EB8                0xEB8   /* game-wide plot step; start point 1 sets 2, which locks Provider talk (L14_PROVIDER_LOCK) */
#define L14_PLOT_EF8                0xEF8   /* game-wide plot step; 2 or 3 become 10 at trigger 3 */
#define L14_PLOT_F24                0xF24   /* game-wide plot step; Dhorkan11 dead sets 1, 5 after EF8 closes, 6 -> 3 when persona 29 is dead */
#define L14_PLOT_F28                0xF28   /* game-wide plot step; 3 -> 4 at INIT */
#define L14_PLOT_F78                0xF78   /* game-wide flag; 0 with Dhorkan11 dead sets F24 = 1 */

#endif /* DKE_LIEU14_INT_H */
