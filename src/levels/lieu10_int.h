#ifndef DKE_LIEU10_INT_H
#define DKE_LIEU10_INT_H

/*
 * lieu10_int.h - shared internals of the LIEU10.DLL port, split between
 * lieu10.c (entry, INIT, tick, helpers) and lieu10_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002BE8C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu10_trig.c). */
void l10_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu10_trig.c):
 *   l10_trig00 = trigger 0, sub_10007530 (thunk sub_10001078; INIT default) */
void l10_trig00(void);

/* Level 10 script variables (low slots up to 0x13C are scratch private to this level; 0x338 and above are game-wide story state) */
#define L10_FRAME_COUNTER           0x038   /* engine frame counter; (value % 20) == 0 throttles the companion-follow distance checks */
#define L10_GUARDIAN15_ALERT        0x04C   /* 1 = Guardian15 alerted and chasing the hero (Arkhan2/3); cleared when the hero is hit by him */
#define L10_GUARDIAN15_FIGHT        0x050   /* 0 = no fight, 1 = fighting the hero, 2 = Guardian15 died in that fight */
#define L10_LORY_SCENE_DONE         0x054   /* 1 = Lory/Rylsadhar20 meeting scene finished (event 35); gates AF4 2 -> 3 */
#define L10_AMBIENT_ZONE_NEAR       0x058   /* 1 = ambience switched to the L10AMB3/4 set while camera 3 or 7 is locked */
#define L10_ZED_FORCE_MEET          0x064   /* 1 = hero stepped into the Zed trigger zone, forces the Zed meeting even before AF4 reaches 3 */
#define L10_UNUSED_68               0x068   /* only zeroed by the INIT trigger; never read in this level */
#define L10_COMPANION_MODE          0x06C   /* Kalhi/Delia companion mode: 1 = follows the hero, 10 = waits at his place, 100 = held by a scripted scene; copied to L10_COMPANION_MODE_SAVED on room change */
#define L10_LORY_FOLLOW_REQ         0x070   /* 1 = Rylsadhar20 must walk to Lory (set by trigger via modify_var); cleared when it starts */
#define L10_COMPANION_CHATTER_ARMED 0x074   /* 1 = idle chatter (L10D98/99/999) is armed for the Kalhi companion once the hero is idle 10 s */
#define L10_COMPANION_CLOSE         0x078   /* 1 = Kalhi/Delia companion is within range and being led; 0 = must catch up */
#define L10_ZED_APPROACH_STEP       0x07C   /* Zed (Arkhan1) walk step: 0 none, 1 waiting at his spot, 2 hero close and facing him (he stops); modify_var advances it */
#define L10_COMPANION_CATCHUP_STEP  0x080   /* Kalhi catch-up: 0 not near, 1 started, 2 reached within 300 units */
#define L10_KALHI_RETURN_STEP       0x084   /* 1 = Kalhi (E9C == 8) waits for the hero, 2 = reached and resumed (E9C back to 7) */
#define L10_UNUSED_88               0x088   /* only zeroed by the INIT trigger; never read in this level */
#define L10_DIALOG_TIMER_ARMED      0x090   /* 1 = random Zed/hero ambient line (L10D29A..I) timer (event 34) armed while camera 3 or 7 is locked; shown as "test12" in the debug overlay */
#define L10_GUARDIAN15_AMBUSH_STEP  0x094   /* Guardian15 / Kalhi ambush sequence 0..10 (1 chasing, 2 camera locked, 3-5 hero warning, 6-7 Kalhi flees to 4630,270, 8-9 Guardian15 returns, 10 fight) */
#define L10_KALHI_TALK_DONE         0x098   /* 1 = first Kalhi13 conversation (L10D61..) already played */
#define L10_RYLSADHAR_SPAWN_TIME    0x0A0   /* game time when Rylsadhar20 was placed in the Sunseer room (written, never read here) */
#define L10_LORY_SPAWN_TIME         0x0A8   /* game time when Lory was placed (written, never read here) */
#define L10_SAVED_CAMERA            0x0AC   /* locked camera saved before the Guardian15 scene (0 = none); also the last camera for the lever-anim pause */
#define L10_UNUSED_B8               0x0B8   /* only zeroed by the INIT trigger; never read in this level */
#define L10_UNUSED_BC               0x0BC   /* only zeroed by the INIT trigger; never read in this level */
#define L10_SUNSEER_STEP            0x0D4   /* Sunseer1 scene step: 1 = walking to the hero's spot, 2 = arrived, 3 = Sunseer1 and Guardian15 placed */
#define L10_KALHI_DEAD_NOTED        0x0D8   /* set to 1 when a Kalhi is found dead while Guardian15 lives (written only) */
#define L10_LEVER_ANIM_STEP         0x0DC   /* 1 = pause the grate anim when the locked camera changes (armed once the large key is used), 2 = paused */
#define L10_GUARDIAN15_CHASE_STEP   0x0E0   /* Guardian15 chase 0..5; 5 = scene over, hero is moved to level 14 */
#define L10_ESCORT_INTRO_DONE       0x0F0   /* 1 = first Kalhi escort line (E9C == 7, EF4 == 0) already played */
#define L10_KALHI_SWAP_REQ          0x0F4   /* 1 = replace Kalhi13 by Kalhi33 in the room (set by dialogue), 2 = done */
#define L10_RYLSADHAR_PLACED        0x0F8   /* 1 = Rylsadhar20 appeared at his zone (needs E78 == 1) */
#define L10_UNUSED_FC               0x0FC   /* only zeroed by the INIT trigger; never read in this level */
#define L10_GUARDIAN15_TALK_DONE    0x100   /* 1 = Guardian15 aftermath dialogue (L10D51..56) already played */
#define L10_UNUSED_114              0x114   /* only zeroed by the INIT trigger; never read in this level */
#define L10_COMPANION_MODE_SAVED    0x12C   /* copy of L10_COMPANION_MODE written before leaving the room; INIT restores it */
#define L10_LORY_FOLLOW_STATE       0x13C   /* game-wide; 0 = unseen, 1 = Lory must go to Rylsadhar20, 2 = done (shared with L33) */
#define L10_EVENT6_DONE             0x338   /* event 6 fired flag (game-wide, same pattern in L1, L3, L6-L9) */
#define L10_EVENT10_DONE            0x348   /* event 10 fired flag (game-wide) */
#define L10_GUARDIAN22_TAKEN        0x4E8   /* 1 = Guardian22 (persona 85) was searched/killed by the hero; otherwise he is restored alive at INIT */
#define L10_PLOT_6DC                0x6DC   /* game-wide plot counter; 4 = trigger that places Guardian25 (then 5); written elsewhere (L33) */
#define L10_ZED_TALK_STAGE_7D4      0x7D4   /* Zed conversation stage 0..2 at his zone */
#define L10_ZED_TALK_STAGE_7F4      0x7F4   /* Zed conversation stage 0..2 (L10D27 then L10D01B); the ambient lines (event 34) stop at 2 */
#define L10_GUARDIAN15_WARNED       0x7F8   /* 0 = first warning L10D34 not yet said, 1 = later warnings play L10D35 */
#define L10_KALHI_TALK_COUNT        0x7FC   /* 0 = first talk with Kalhi13 (L10D58..), 1 = repeat talks */
#define L10_COMPANION_CHATTER_LINE  0x948   /* index 0..2 of the next idle line (L10D999, L10D99, L10D98) */
#define L10_KALHI_TALK_MARK         0xA84   /* set to 3 after Kalhi's L10D108 line (written only here) */
#define L10_STORY_STAGE             0xAF4   /* game-wide story stage; 0 start, 1 Lory/Rylsadhar20 waiting, 2 meeting scene, 3 Zed appears (shared with L2, L11) */
#define L10_GUARD_ARC_PHASE         0xAF8   /* guardian-arc stage; 0 = Arkhan1 before the challenge, 1 = after it (game-wide) */
#define L10_PLOT_B40                0xB40   /* game-wide plot step; 2 with V_KALHI13_TALK_C == 3 retires both Kalhi; 4 -> Guardian25 placed; 99 = Guardian15 alert flag */
#define L10_RETURN_ROOM_STEP        0xB4C   /* progress of Arkhan2/3 through the grate: 1 = grate seen, 2 = large key used, 3 = OB_OBJ_BEFORE40 used, leave to level 51; L11 uses it to pick the return room */
#define L10_ROOM8_ENTRY             0xB54   /* 3 or 4 = which entry point the hero uses back in level 8 (y -1622 / -1100); set from start points */
#define L10_PLOT_BA8                0xBA8   /* game-wide plot step; 4 -> 5 at INIT */
#define L10_PLOT_BD0                0xBD0   /* game-wide plot step; 1 -> 2 on leaving; 99 = Guardian15 alert flag */
#define L10_DELIA_MUSIC_GATE        0xBE4   /* 0 allows the level music reset when V_DELIA_PHASE == 20 */
#define L10_GUARDIAN8_STAGE         0xC08   /* guardian 8 fight stage; 3 (killed) becomes 10 at INIT (shared with L2, L8, L9, L33) */
#define L10_DELIA_RETURN_DONE       0xE48   /* 1 = Delia scene after the grate (B4C == 1, V_GUARD_FIGHT_DONE == 2) already started */
#define L10_KALHI_FIGHT_STATE       0xE58   /* 0 none, 1 = a Kalhi died with Guardian15 alive, 2 = Guardian15 alert taken, 10 = a Kalhi was killed by the hero */
#define L10_RYLSADHAR_SCENE         0xE78   /* 1 = Rylsadhar20 scene started (after the L10D05 conversation) */
#define L10_LORY_PLACED             0xE7C   /* 1 = Lory reached her zone and was placed in the Sunseer room */
#define L10_ENDING_OVERRIDE         0xE8C   /* game-wide; 1 = skip the hero-death ending in the tick and re-run INIT (set to 1 in L31 near the end) */
#define L10_KALHI_QUEST_STAGE       0xE9C   /* game-wide Kalhi quest; 7 = escorting Kalhi, 8 = Kalhi waits, 10 = Kalhi gone/killed; >= 7 enables the Guardian15 ambush */
#define L10_ZED_ENCOUNTER_STAGE     0xEC8   /* Zed meeting: 0 not yet, 1 pending, 2 first line spoken, 3 menace scene (game-wide, shared with L2) */
#define L10_KALHI_ESCORT_STARTED    0xEF0   /* 1 = the Kalhi escort follow AI has started */
#define L10_GUARDIAN15_RESULT       0xEF4   /* game-wide; 2 = Guardian15 dead, 3 = Guardian15 scene over and he left (shared with L33) */
#define L10_GUARDIAN_CUTSCENE_STAGE 0xF14   /* 0 not shown, 1 = Guardian20 cutscene started on arriving from level 23, 2 = ended */
#define L10_DELIA_PRESENT_F20       0xF20   /* game-wide; 1 = suppresses the Delia scene (shared with L2, L8, L33) */
#define L10_L20_SCREEN_DONE         0xF38   /* 1 = screen effect after returning from level 20 already shown */

#endif /* DKE_LIEU10_INT_H */
