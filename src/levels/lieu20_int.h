#ifndef DKE_LIEU20_INT_H
#define DKE_LIEU20_INT_H

/*
 * lieu20_int.h — shared internals of the LIEU20.DLL port, split between
 * lieu20.c (entry, INIT, tick, helpers) and lieu20_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002409C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu20_trig.c). */
void l20_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu20_trig.c):
 *   l20_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l20_trig00(void);

/* Level 20 script variables (scratch slots 0x4C..0x114 are per level; 0x32C/0x330/0x338/0x348 are op-4 event slots reused as story flags) */
#define L20_UNUSED_04C                     0x04C   /* reset at INIT, never used */
#define L20_THANANDAR_EXIT_STEP            0x050   /* event 39 after the Thanandar25 beating: 0 = he leaves (L20D95), 1 = hero stands up */
#define L20_LORY_WALK_STARTED              0x064   /* INIT sets 1 when Lory is sent to the gate (phase before the Lory chain); never read */
#define L20_UNUSED_068                     0x068   /* reset at INIT, never used */
#define L20_LORY_GATE_STEP                 0x06C   /* Arkhan1 gate scene: 0 idle, 1 Lory ordered, 222/2 opening the first door, 3 second door; 10 = Guardian1 intro played */
#define L20_GATE_OPEN_STEP                 0x070   /* event 1 at the gate: 0 = open the door (L20D12/L20D13), 1 = leave to room 27 */
#define L20_WEAPON_WARNINGS                0x074   /* number of weapon warnings given by Guardian1 (0..2) */
#define L20_SQUAD_SCENE_STEP               0x078   /* Guardian9/10 squad scene (camera lock 2): 0 idle, 1..3 steps, 10 = done */
#define L20_SQUAD_FAR                      0x07C   /* 1 = hero is far (>500) from the squad zones 6/7 and 26, 0 = near again */
#define L20_EXIT_COMMENT_BUSY              0x088   /* zone 10 exit: 1 while the hero's exit remark plays */
#define L20_ESCORT_WALK_SYNC               0x090   /* walk sync counter: modify_var 2 = Lory walking, 4 = Phedoria walking, 0 when done */
#define L20_SCENE_STATE                    0x094   /* Arkhan1: Zed84 chase 0 none, 1 started, 2 in sight; after the story advance: 1 = escort anim running, 2 = camera paused */
#define L20_ELAPSED_MS                     0x0A4   /* time since L20_SCENE_TIME; picks how far Lory/Phedoria have walked at INIT */
#define L20_SCENE_TIME                     0x0A8   /* game time when the Lory/Phedoria walk last advanced; -666 = none */
#define L20_ZONE20_TIMING                  0x0B8   /* 0 idle, 1 timing the hero in zone 20, 2 gate scene running */
#define L20_ZONE20_START_TIME              0x0BC   /* game time when the hero entered zone 20 or Guardian1 last spoke */
#define L20_PHEDORIA_SENT                  0x0D4   /* 1 once Phedoria was sent to the door spot */
#define L20_CORNER_REMARK_DONE             0x0E0   /* zones 36..39: 0 = no remark yet, 1 = the hero already commented */
#define L20_FIGHT_HERO                     0x0E8   /* hero persona saved when the Thanandar25 scene starts */
#define L20_CORNER_ZONE                    0x0F0   /* last zone (36..39) where the remark was made */
#define L20_CAMERA_PAN_STATE               0x0FC   /* Lory/Phedoria camera: 0 idle, 1 panning, 99 done */
#define L20_UNUSED_100                     0x100   /* reset at INIT, never used */
#define L20_INIT_TIME                      0x114   /* game time at INIT; camera/walk logic waits 4000 ms after it */
#define L20_RYLSADHAR_ARRIVAL              0x32C   /* 1 = Rylsadhar21 route (Arkhan2) active, 0 none */
#define L20_ZED_ARRIVAL                    0x330   /* 1 = Zed27 route (Arkhan3) active, 0 none */
#define L20_EVENT6_SLOT                    0x338   /* op-4 event 6 slot; INIT sets 1 when L20_GUARD4_STAGE == 4 */
#define L20_EVENT10_SLOT                   0x348   /* op-4 event 10 slot; INIT sets 1 when L20_GUARD4_STAGE == 4 */
#define L20_GUARDIAN1_FACING               0x830   /* 1 = Guardian1 follows the hero with his eyes, reset when the hero is >220 away */
#define L20_TALK_GUARDIAN1                 0x834   /* Arkhan1 conversation stage with Guardian1: 0, 1, 2, then counts up; 666+ = final lines */
#define L20_SWORD_CHALLENGE_DONE           0x83C   /* 1 once Guardian1 reacted to the sword (OB38) after 4 s in zone 20 */
#define L20_ALCOVE_STAGE                   0x840   /* zone 13 (OB03 alcove): 0 first look, 1 seen */
#define L20_GUARDIAN1_STRIKE_STAGE         0x844   /* Guardian1 hit: 0 warned, 1 second hit (Zed84 arrives), 2 over */
#define L20_ZONE20_ELAPSED_MS              0x848   /* ms the hero stayed in zone 20; triggers the sword and weapon remarks */
#define L20_TALK_GUARDIAN9_A3              0x850   /* Arkhan3 conversation stage with Guardian9, 0..2 */
#define L20_ENGAGED_GUARDS                 0x858   /* persona ids of the guards fighting at room exit, packed: 4950, 49, 5152; restored as V_ENGAGED_PERSONA */
#define L20_TALK_LORY                      0x870   /* Lory conversation stage, 0..1 */
#define L20_TALK_PHEDORIA                  0x874   /* Phedoria conversation stage, 0..5 */
#define L20_TALK_GUARDIAN9_A2              0x880   /* Arkhan2 conversation stage with Guardian9, 0..2 */
#define L20_STORY_ADVANCED                 0xAF8   /* 0 = Arkhan1 prison phase (gate and Guardian1), 1 = Arkhan2/3 phase */
#define L20_NIGHT_FLAG                     0xB10   /* day/night pending flag (read by the zone 10 exit: 1 plays the night cinematic SC10) */
#define L20_OB03_STAGE                     0xB28   /* alcove object OB03: 0 closed, 1 hero knows, 2 opened */
#define L20_ALERT_STAGE                    0xB3C   /* 0 = calm music 1, 1 = alarm music 2 (after cinematic SC35), 2 = alarm at start point 6 */
#define L20_ROOM10_ENTRY                   0xB4C   /* which entrance of room 10 the Arkhan2/3 exit uses (0..2, 3 = room 51) */
#define L20_ESCORT_STAGE                   0xB58   /* Lory/Phedoria escort: 0 none, 1 arrived in room, 2..5 walking steps, 10 = over, 100 = scared by weapon */
#define L20_GUARD4_STAGE                   0xB98   /* Guardian4 patrol: 0, 2 or 4 (4 arms events 6 and 10) */
#define L20_GUARD_SQUAD_STAGE              0xE74   /* Guardian9/10/11 squad: 1/10 = two guards, 111/112 = Guardian9 only, 2/20 = Guardian11 and persona 52, 11/115/21 = dead, 99 = off */
#define L20_FLAG_0E78                      0xE78   /* set to 7 when the Lory gate chain restarts at INIT; not read here */
#define L20_LORY_CHAIN_STAGE               0xE7C   /* Lory chain: 1 started, 2 Lory sent to the gate, 3 finished (door passed) */
#define L20_DEATH_ENDING_DONE              0xE8C   /* 1 = death ending already played, INIT runs the normal setup (same slot in every level) */
#define L20_PASSAGE_OPEN                   0xF38   /* 1 = the way back is open after the fight (same slot in L21) */
#define L20_THANANDAR_SCENE                0xF64   /* 0 = the Thanandar25 arrival scene has not run, 1 = started */

#endif /* DKE_LIEU20_INT_H */
