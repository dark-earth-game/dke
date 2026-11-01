#ifndef DKE_LIEU36_INT_H
#define DKE_LIEU36_INT_H

/*
 * lieu36_int.h - shared internals of the LIEU36.DLL port, split between
 * lieu36.c (entry, INIT, tick, helpers) and lieu36_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* Script variables of this level (state block slots; low slots are per-level scratch). */
#define L36_CHASE_TARGET                 0x50   /* persona id (Lodar 86 or Konkalite 82) currently sent after the hero; used by the EV_0 chase tick */
#define L36_LODAR_PANIC_ANIM_DONE        0x54   /* 0 = SP86GAR27/P86 reaction not played yet, 1 = played; reset when Lodar is re-engaged */
#define L36_CAM4_CINEMATIC_STAGE         0x64   /* 0 = nothing, 1 = camera-4 cinematic shown, 2 = Lodar line L36D146 said (camera 6) */
#define L36_PAIR_SEEN_AT_EXIT            0x68   /* 1 once the hero reached the guards' cameras (1 or 6) while the guard-pair phase was 1..3; read by the room-5 exit trigger to advance L36_GUARD_PAIR_PHASE */
#define L36_GUARD_PASS_STAGE             0x6C   /* passing guard (Guardian1 or persona 85) walk stage 0..6 (camera 2): 0 idle, 1 talking/walking, 2/3, 4/5 legs, 6 gone */
#define L36_KONK_STRUCK_FIRST            0x70   /* 1 = the player struck Konkalite first (selects the Lodar-death branch with Konkalite still alive) */
#define L36_KILL_SEQ_ACTIVE              0x74   /* 1 while a guard death (hit) sequence runs; blocks the alert/dead-guard handlers */
#define L36_LODAR_GREET_TOGGLE           0x78   /* Lodar greeting alternates: 0 = first (L36D87..L36D92), 1 = repeat (L36D78/79, L36D99) */
#define L36_LODAR_TALK_STAGE             0x7C   /* guard conversation stage 0..2 (L36D96.., L36D123A..) while the pair is still in phase 1 */
#define L36_G20_SPOKE                    0x80   /* 1 = Guardian20 has already spoken his challenge (L36D147/148, L36D08 uses a dialogue instead of a registered action) */
#define L36_UNUSED_CLEARED_84            0x84   /* cleared by trigger 0, never read in this level */
#define L36_G20_POST_STATE               0x88   /* Guardian20 post: 0 = not noticed hero, 1 = noticed hero, 2 = returned to post */
#define L36_UNUSED_CLEARED_90            0x90   /* cleared by trigger 0, never read in this level */
#define L36_UNUSED_CLEARED_94            0x94   /* cleared by trigger 0, never read in this level */
#define L36_KONK_D200_SAID               0xE0   /* 1 = Konkalite already said L36D200 when the player first struck */
#define L36_LODAR_ALERT_STEP             0xF0   /* 0 = calm, 1 = alerted by Konkalite fight, 2 = alert anim P86GAR02 played */
#define L36_KONK_ALERT_STEP              0xF4   /* 0 = calm, 1 = alerted, 2 = alert anim P82GAR03 played, 3 = Konkalite dead */
#define L36_LODAR_HELMET_TALK_ALT        0xF8   /* toggle choosing the L36D71/L36D77 variant of the helmet haggling lines */
#define L36_LODAR_HELMET_TALK            0x568   /* helmet (OB_HELMET) negotiation stage with Lodar after Konkalite died, 0..3 */
#define L36_LODAR_FIGHT_TALK             0x570   /* 0 = Lodar not yet confronted (L36D85), 1 = L36D85 said, 2 = L36D86 said and Lodar attacks */
#define L36_PLAYER_HIT_COUNT             0x8D4   /* engine counter of hits received by the player; a change costs the hero 50 HP (also read by levels 7 and 32) */
#define L36_GUARD_VARIANT                0xA50   /* 0 = Guardian1 passes through camera 2, 1 = persona 85 does (set before this room; read only) */
#define L36_HELMET_STAGE                 0xC14   /* Lodar's helmet: 0 untouched, 1 pair alerted, 2 Konkalite called over, 3 helmet already taken/given */
#define L36_DEATH_ENDING_BYPASS          0xE8C   /* 1 = hero death does not play the death ending in INIT (runs trigger 0 instead); shared flag */
#define L36_GUARD_PAIR_PHASE             0xF50   /* Lodar/Konkalite encounter phase: 1 both on guard, 2 Lodar dead, 3 left room, 4 Konkalite dead, 5 Lodar left alone, 6/7 after-fight, 8 finished */
#define L36_FIGHT_TARGET_STATE           0xF54   /* 0 = no fight, 1 = Guardian20 fight, 2 = Konkalite engaged, 3 = Lodar engaged */

/* ST() reads g_lvl_S, the DLL's dword_10020C5C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu36_trig.c). */
void l36_trigger(i32 idx);

/* Trigger handlers the core also calls directly (defined in lieu36_trig.c):
 *   l36_trig00 = trigger 0, sub_10007547 (thunk sub_10001073; INIT default)
 *   l36_trig02 = trigger 2, sub_10009C0A (thunk sub_100010BE; tick, cri)
 *   l36_trig04 = trigger 4, sub_1000C0E1 (thunk sub_10001082; tick, cri) */
void l36_trig00(void);
void l36_trig02(void);
void l36_trig04(void);

/* Send persona `who` (Lodar 86 / Konkalite 82) at the hero: sub_10007400
 * (thunk sub_100010C3), __fastcall(ecx = persona); defined in lieu36.c. */
void l36_sub_7400(i32 who);

/* dword_10020C90: ST(0x8D4) as of the last tick; trigger 0 seeds it. */
extern i32 l36_prev_8d4;

#endif /* DKE_LIEU36_INT_H */
