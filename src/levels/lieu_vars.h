#pragma once

/*
 * lieu_vars.h — script variables in the LIEU state block (dword_505C40,
 * byte offsets for ST()) that keep one meaning across levels. The block
 * persists across levels, but many slots are scratch space each level
 * reuses for itself; those are named per level, LNN_* in lieuNN_int.h.
 */

#define V_PLAYER_STRIKE       0x014   /* engine-set >=1 when the player hits/acts on the persona in front; scripts consume by writing -1 (L1, L2) */
#define V_ENGAGED_PERSONA     0x034   /* persona id currently engaged in an interaction/fight (0 = none; 0x57 Kabak, 0x58 Guardian23, 0x39..0x3B Guardians 17-19, 0x21 Kalhi33, 0x0D Kalhi13) (L1, L2) */
#define V_LAST_WEAPON         0x040   /* last ST_PLAYER_WEAPON the persona already reacted to; compared to avoid repeating the weapon-warning line (L1, L2) */
#define V_EVENT15_STEP        0x08C   /* event 15 step chain 0->1, 2->3, 4->5 ...0xC->0xD (shared event4 code; level-local otherwise) (L1, L2) */
#define V_FROM_LEVEL          0x108   /* set at level INIT from ST_LEVEL (the level the player came from); L2 tests 6/7 (L2) */
#define V_CRI_KEY             0x10C   /* shout (cri) key state, 1 = pressed; debug overlay prints "CRI:" (L1, L2) */
#define V_CRI_USED            0x110   /* shout already handled for this encounter (debug "cri_on:"); 0 = free, 1 = used, scripts modify_var back to 0 (L1, L2) */
#define V_HIT_DAMAGE          0x130   /* engine-set damage amount subtracted from the target HP in the hit-drain event (L2) */
#define V_PENDING_EVENT       0x134   /* event id minus 0x59 queued by the engine; tick fires CB->evenement(v+0x59,1000) then clears (L1, L2) */
#define V_GUARD17_AWAY        0x15C   /* 1 = Guardian17 left his post (hero far), 0 = back on guard; persists across rooms (L2) */
#define V_ATT20_SEEN          0x160   /* 0 = first ATT20 event plays SPxxATT20 anim, then 1 (L1, L2) */
#define V_DEATH_EVENT_SENT    0x1A0   /* ST_VIE(4) used as flag: hero dead (ST_HERO==4) game-over event 0x2C already queued (L1, L2) */
#define V_EVENT_0             0x320   /* set/advanced by op-4 event 0 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_1             0x324   /* set/advanced by op-4 event 1 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_2             0x328   /* set/advanced by op-4 event 2 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_7             0x33C   /* set/advanced by op-4 event 7 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_8             0x340   /* set/advanced by op-4 event 8 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_9             0x344   /* set/advanced by op-4 event 9 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_11            0x34C   /* set/advanced by op-4 event 11 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_12            0x350   /* set/advanced by op-4 event 12 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_16            0x360   /* set/advanced by op-4 event 16 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_17            0x364   /* set/advanced by op-4 event 17 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_21            0x374   /* set/advanced by op-4 event 21 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_22            0x378   /* set/advanced by op-4 event 22 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_DELIA_PHASE         0x380   /* Delia escort phase: 7 = pending, 8 = active/following, 0x14 = finished (L2) */
#define V_EVENT_25            0x384   /* set/advanced by op-4 event 25 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_27            0x38C   /* set/advanced by op-4 event 27 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_28            0x390   /* set/advanced by op-4 event 28 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_29            0x394   /* set/advanced by op-4 event 29 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_30            0x398   /* set/advanced by op-4 event 30 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_31            0x39C   /* set/advanced by op-4 event 31 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_33            0x3A4   /* set/advanced by op-4 event 33 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_34            0x3A8   /* set/advanced by op-4 event 34 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_35            0x3AC   /* set/advanced by op-4 event 35 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_36            0x3B0   /* set/advanced by op-4 event 36 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_37            0x3B4   /* set/advanced by op-4 event 37 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_38            0x3B8   /* set/advanced by op-4 event 38 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_39            0x3BC   /* set/advanced by op-4 event 39 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_40            0x3C0   /* set/advanced by op-4 event 40 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_41            0x3C4   /* set/advanced by op-4 event 41 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_END_GAME_REQ        0x3D0   /* event 44 sets 1; tick plays SC22/SC23 cinematic then DV_END_GAME (L1, L2) */
#define V_EVENT_51            0x3EC   /* set/advanced by op-4 event 51 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_52            0x3F0   /* set/advanced by op-4 event 52 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_EVENT_53            0x3F4   /* set/advanced by op-4 event 53 in event4 (0->1 fired, then consumer sets next step); consumer differs per level (L1, L2) */
#define V_GATE_OB25_OPEN      0x5DC   /* 0 = east gate OB25 closed (zone 2, vire_col 6), 1 = opened; L1 (starts 2+ preset 1), L2 init sets 1 */
#define V_KABAK_DOOR_ZONE     0x6F4   /* 1 once the door zone 16 is added after Kabak is dead (0 = not yet); L1 */
#define V_OB38A_STATE         0x700   /* 0 = OB38A pickup present (zones 5/19), 1 = already gone (start 2+), 2 = taken (trig05 zone 0x13); L1 */
#define V_KALHI12_TALK_NIGHT  0x708   /* Kalhi12 conversation stage while V_ZED_BRIEFING==0 (0 none, 1..4); L1 */
#define V_FRONT_DOOR_STAGE    0x70C   /* Arkhan0 front door: 0 = first exit to Sparta (L01D32 + change_piece), 1 = door message L01D33; L1 */
#define V_KALHI12_TALK_POST   0x710   /* Kalhi12 conversation stage once V_ZED_BRIEFING>=2 (0,1,..); L1 */
#define V_G17_TALK_H23        0x71C   /* Guardian17 talk stage, Arkhan2/3 (0..3); L2 */
#define V_G19_TALK            0x720   /* Guardian19 first interaction: 0 = not yet, 1 = done; L2 */
#define V_ZED84_TALK_A        0x724   /* Zed84 conversation stage (L02D71..), 0,1; L2 */
#define V_LCOD36_SHOWN        0x768   /* one-shot LCOD36 (Arkhan3) shown; cleared by LCOD37; L1/L2 event6 */
#define V_LCOD37_SHOWN        0x76C   /* one-shot LCOD37 (Arkhan3) shown; cleared by LCOD35; L1/L2 event6 */
#define V_ZED84_TALK_B        0x778   /* Zed84 second conversation stage (L02D82/83), 0..2; L2 */
#define V_KABAK_TALK_B88      0x77C   /* Kabak conversation stage on the second-round branch (B88!=0): 0,1,2; L1 */
#define V_LCOD35_SHOWN        0x7C8   /* one-shot LCOD35 (Arkhan2) shown; cleared by LCOD37; L1/L2 event6 */
#define V_DELIA_HITS          0x7CC   /* Delia: 0 = untouched, 1 = warned (L45D04), second hit kills her; L2 */
#define V_KALHI13_TALK_B      0x828   /* Kalhi13 stage 0/1 -> 2 (L04D63); L2 */
#define V_KABAK_TALK_H1       0x898   /* Kabak conversation stage when hero is Arkhan1 (0..3+); L1 */
#define V_KALHI13_TALK_A      0x8A0   /* Kalhi13 (0x0D) conversation stage 0,1 (L04D51..); L2 */
#define V_KALHI13_TALK_C      0x8A8   /* Kalhi13 stage 0..2 (L04D41...); L2 */
#define V_KABAK_TALK_H23      0x8B4   /* Kabak conversation stage for Arkhan2/3 (1 = L01D66B/C said, 2..4 follow-ups); L1 */
#define V_KALHI12_TALK_BRIEF  0x8B8   /* Kalhi12 conversation stage while V_ZED_BRIEFING==1 (0,1,..); L1 */
#define V_KABAK_TALK_G23      0x8BC   /* Kabak talk stage while V_GUARD_WAVE==1 (Guardian23 arrived), 0..3; L1 */
#define V_KABAK_INTRO         0x8C8   /* Kabak attack intro: 0 = not started, 1 = ready for L01D86, 2 = said; L1 */
#define V_PHEDORIA_TALK_B     0x8D0   /* Phedoria second talk (L02D215), 0/1; L2 */
#define V_OB44D_TAKEN         0x8D8   /* 0 = object OB44D still in room, 1 = taken (SG00PRE07); L1 */
#define V_DLG_RANDOM          0x928   /* scratch: CB->random(n) result choosing a LCOD/animation variant (2/3 choices); L1/L2 */
#define V_LCOD34_SHOWN        0x938   /* one-shot LCOD34 (Arkhan2 zone trigger) shown; L1/L2 event6 */
#define V_KALHI12_D34_DONE    0xACC   /* 0x29A once the Kalhi12 L01D34 scene played (one-shot guard); L1 */
#define V_PHEDORIA_TALK_A     0xAEC   /* Phedoria (0x1A) conversation stage (L02D200..), 0/1; L2 */
#define V_INTRO_STATE         0xAF0   /* 0 = intro not started (Arkhan0 nightclothes), 1 = intro running, 2 = past intro; L1 (also seeded by L2) */
#define V_GUARD_FIGHT_DONE    0xB04   /* 0 = Kabak/guard encounter unresolved, 2 = resolved (cinematic SC23 vs SC22 on level change); L1 + L2 */
#define V_KALHI_ROUTE         0xB2C   /* 1 = Kalhi13 route, 2 = Kalhi33 route (entry position in level 3); L2 */
#define V_ZED_BRIEFING        0xBA0   /* Zed briefing at game start: 0 not yet, 1/2 pending (Arkhan1 / Arkhan0 variant), 3 done; L1 + L2 */
#define V_MUSIC_TRACK         0xC54   /* music track id for DV_MUSIC fade events (-1 = none); L1/L2 */
#define V_KABAK_D80_SAID      0xC80   /* 1 once L01D80 said by Kabak to Arkhan2/3; L1 */
#define V_KABAK_D81_SAID      0xC84   /* 1 once L01D81/82 Kabak greeting said; L1 */
#define V_G17_TALK_H1         0xC88   /* Guardian17 (0x39) talk stage, Arkhan1 (0,1,2); L2 */
#define V_KABAK_MET_H1        0xCC8   /* 1 once Arkhan1 had the first Kabak encounter (L01D66A); L1 */
#define V_G23_TALK            0xCD0   /* Guardian23 (0x58) conversation stage: 0,1,2; L1 */
#define V_KABAK_TALK_END      0xCD4   /* Kabak talk stage after the guard talks (0..3); L1 */
#define V_KABAK_TALK_G24      0xCD8   /* Kabak talk stage while V_GUARD_WAVE==2 (Guardian24 arrived), 0..2; L1 */
#define V_G24_TALK            0xCDC   /* Guardian24 (0x59) conversation stage: 0,1,2; L1 */
#define V_PRE90_OBJECT        0xD38   /* object number n for link_objet "OB%02d" in the PRE90 scene; L2 */
#define V_WEAPON_WARNED       0xE00   /* 1 once L02D111C (weapon warning) said; L2 */
#define V_LCOD02_SEEN         0xEA8   /* imp interaction LCOD02/03: 0 = first time (extra G,H lines), 1 = seen; L1/L2 */
#define V_LCOD23_SEEN         0xEAC   /* set to 1 on the LCOD23/24 interaction; L1/L2 */
#define V_LEFT_HOME           0xEC0   /* 0 = still in the starting house area, 1 = left it / story moved on (Kabak guard encounter); L1 + L2 */
#define V_GUARD_WAVE          0xEC4   /* which extra guard has joined Kabak: 0 none, 1 = Guardian23, 2 = Guardian24; L1 (preset by start point) */
#define V_KABAK_FIGHT_ON      0xF04   /* 1 once combat with Kabak was triggered; L1 */
#define V_DELIA_KILLED        0xF1C   /* 2 once Delia was killed (0x380=0x14); L2 */
#define V_SOB86_ACTIVE        0xF44   /* SOB86 room-logic busy flag (1 while transition is running); L1/L2 */
#define V_HUD_TIMER_MS        0xF5C   /* countdown in ms, event 0x12 subtracts 0x1388 per step with HUD refresh; L1/L2 */
#define V_SAVED_HERO          0xF68   /* hero persona saved when the Kalhi event started (compared to ST_HERO); L2 */
#define V_G23_FIGHT_ON        0xF74   /* 1 once Guardian23 is fighting (at GAR01 spot); reset when he dies; L1 */

/* Shared across levels (collected from the per-level naming pass). */
#define V_FRAME_COUNTER       0x038   /* engine frame counter; scripts pace follow/patrol checks with % 20 or % 100 */
#define V_EVENT_3             0x32C   /* op-4 event 3 slot, also a story flag: Thanandar infected the father (L39D24) / Rylsadhar route (L20-L22); L5, L15, L18, L20-L22, L32, L39, L46 */
#define V_EVENT_4             0x330   /* op-4 event 4 slot, also a story flag: Thanandar infected Zed (L39D25) / Zed route (L20-L22); same levels as V_EVENT_3 */
#define V_EVENT_6             0x338   /* op-4 event 6 slot; guard-pair progress 0/1/2 with V_GUARDIAN4_STAGE, L8 arena round (0,1,2,0x63) */
#define V_EVENT_10            0x348   /* op-4 event 10 slot; set with V_EVENT_6 once V_GUARDIAN4_STAGE reaches 4 */
#define V_EVENT_14            0x358   /* op-4 event 14 slot; seeded by start points */
#define V_ARKHAN1_PROGRESS    0xAF4   /* Arkhan1 exploration step 0..3; L2 tests < 3, L10 advances it through the Lory/Rylsadhar and Zed scenes */
#define V_RETURN_PART         0xAF8   /* 0 = Arkhan1 chapter, 1 = the return part as Arkhan2/3; set by start points, read by guard, fight and Kalhi scenes */
#define V_CRYSTAL_KEY_STAGE   0xB10   /* OB75 Rylsadhar's Crystal Key: the engine sets 2 when OB02+OB12+OB18+OB19 combine (inventory auto-combine); L25 sets 1 and 5 */
#define V_LORY_STAGE          0xB38   /* Lory / Archivist room stage 0..3; written by L27/L28 start points */
#define V_ALERT_STAGE         0xB3C   /* 0 = calm (music 1), 1/2 = alarm (music 2); L18, L20, L21 */
#define V_KALHI_RECRUIT       0xB40   /* Kalhi recruitment stage 0..4, 3 = recruited; L2-L4, L10, L33 */
#define V_ROUTE_STAGE         0xB4C   /* stage 0..3 choosing the exit / return room variant (room 10 or 51); L33 sets 2 from SC15 */
#define V_GUARDIAN4_STAGE     0xB98   /* Guardian3/4 encounter 0..5 (0/2 on the Arkhan1 path, 4 = done); INIT resets it to 0 while Guardian4 lives */
#define V_ARMAL_STAGE         0xBC8   /* Armal stage 0/1/2 (written in L18); L16/L19 Malrot and Delia lines depend on it */
#define V_GUARDIAN8_STAGE     0xC08   /* Guardian8: 2 = present, 3 = killed (L10 turns 3 into 10) */
#define V_BUNKER_SEEN         0xC20   /* bunker entrance in the Darklands: 1 = seen (L49, plate used), 2 = told (L13/L53) */
#define V_DELIA_GONE          0xC44   /* 1 = Delia dead or her escort over (V_DELIA_PHASE >= 10); set in L37, read in L14/L54 */
#define V_SORDOS_DUEL         0xE20   /* Sordos / Thanandar24 duel: 2 = decided (L34); L31/L32 start the chase only below 2 */
#define V_KALHI_KILLED        0xE58   /* set when Kalhi13 / Kalhi33 is killed (1, or 10) */
#define V_RYLSADHAR_ZED_STAGE 0xE78   /* Rylsadhar/Zed storyline: 1 start, 2 waiting, 7 at post, 9 Kalhi done, 10 Zed84, 100 past */
#define V_CHASE_STATE         0xE8C   /* chase to room 32: 0 none, 1 requested, 2 running, 10 over; every INIT also runs the normal setup when 1 even if ST_HERO is 4 */
#define V_KONKALITE2_FIGHT    0xE98   /* Konkalite2 fight: 0/1 start, 20-22 hit variants, 56 won, 60 cleared, 65 already done (INIT maps 60 to 65) */
#define V_KALHI_PHASE         0xE9C   /* Kalhi escort phase, encoded like V_DELIA_PHASE: 3 following, 7 guiding, 8 waiting, 10 gone (L8 also 5/6) */
#define V_ZED84_TALK_STAGE    0xEC8   /* Zed84 meeting stage 0..3 at the Arkhan1 start */
#define V_CRYSTAL_PROGRESS    0xED0   /* crystal quest step 0..3, 5 = done; advanced when the hero shows proof */
#define V_CHASE_ROUTE         0xEF8   /* chase route: 1 or 11 = chase to room 32 requested; L34 also 0 and 2 */
#define V_PASSAGE_OPEN        0xF38   /* 1 = the way on is open after the guard fight; L10, L20, L21, L27, L29 */
