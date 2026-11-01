#ifndef DKE_LIEU21_INT_H
#define DKE_LIEU21_INT_H

/*
 * lieu21_int.h -- shared internals of the LIEU21.DLL port, split between
 * lieu21.c (entry, INIT, tick) and lieu21_trig.c (the zone trigger
 * handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001F66C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu21_trig.c). */
void l21_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu21_trig.c):
 *   l21_trig00 = trigger 0, sub_10001200 (thunk sub_10001073; INIT default) */
void l21_trig00(void);

/* Level 21 script variables (scratch slots 0x64..0xB8 are per level; 0x32C/0x330 are op-4 event slots reused as story flags) */
#define L21_LORY_STATE                     0x064   /* Lory escort walk: 0 idle, 1 walking to zone 11, 2 arrived, 100 scared by the drawn weapon */
#define L21_PHEDORIA_STATE                 0x068   /* Phedoria escort walk: 0 idle, 1 start, 2 walking to zone 21, 3 arrived, 100 scared by the drawn weapon */
#define L21_EVENT1_PHASE                   0x06C   /* event 1 cutscene toggle: 0 = start the big-anim shot, 1 = finish and leave to room 21/22 */
#define L21_UNUSED_070                     0x070   /* reset at INIT, never used */
#define L21_HERO_HP_SNAPSHOT               0x074   /* hero HP at INIT; Thanandar25 fight ends when HP drops 10 below it */
#define L21_SCENE_SYNC                     0x078   /* event sync: modify_var +2 after the Thanandar25 knock-out, back to 0 when the scene ends */
#define L21_TALK_TOGGLE                    0x07C   /* alternates the two Sunseer18 replies for Arkhan3 (0 = L21D104, 1 = L21D72D) */
#define L21_SUNSEER2_GREETED               0x080   /* 1 once Sunseer19 came over to greet the hero (within 300 of zone 7) */
#define L21_END_REQUESTED                  0x088   /* 1 once the hero walked past the end line before the guard fight (DV_END_GAME) */
#define L21_UNUSED_090                     0x090   /* reset at INIT, never used */
#define L21_LORY_ARRIVED_TIME              0x0A8   /* game time Lory reached zone 11; -666 when INIT re-enters with Lory mid-walk */
#define L21_MUSIC_START_TIME               0x0B8   /* game time the current music track started */
#define L21_CAMERA_STATE                   0x0E0   /* exit-zone 20 camera: 0 free, 1 locked; also the shout target: 18 = Sunseer18, 19 = Sunseer19, 99 = shout disabled */
#define L21_RYLSADHAR_ARRIVAL              0x32C   /* 1 = Rylsadhar21 came with the Arkhan2 route, 5 = walked on to room 22, 0 none */
#define L21_ZED_ARRIVAL                    0x330   /* 1 = Zed27 came with the Arkhan3 route, 5 = walked on to room 22, 0 none */
#define L21_SUNSEER1_STATE                 0x85C   /* Sunseer18 patrol: 0 wandering, 1 following the hero, 2 walking to the hero, 5 talk starting */
#define L21_TALK_SUNSEER1_A1               0x860   /* Arkhan1 conversation stage with Sunseer18 (special mode on), 0..2 */
#define L21_TALK_SUNSEER1_A2               0x864   /* Arkhan2 conversation stage with Sunseer18, 0..2 */
#define L21_SUNSEER1_STRIKE_WARNED         0x868   /* Arkhan1 hit Sunseer18 once (warned, L21D21); the next hit starts the fight */
#define L21_ZED_AMBUSH                     0x86C   /* Arkhan1 after Sunseer18 died: 0 idle, 1 Zed84 attacks the hero, 2 hero reached zone 6 (killed) */
#define L21_TALK_LORY                      0x870   /* Lory conversation stage, 0..1 */
#define L21_TALK_PHEDORIA                  0x874   /* Phedoria conversation stage, 0..5 */
#define L21_THANANDAR_SCENE                0x878   /* 1 once the Thanandar25 arrival cutscene started (guard fight stage 1) */
#define L21_TALK_SUNSEER1_A3               0x87C   /* Arkhan3 conversation stage with Sunseer18, 0..1 */
#define L21_TALK_SUNSEER2                  0x884   /* conversation stage with Sunseer19, 0..2 (2 = hostile line L21D84B) */
#define L21_STORY_ADVANCED                 0xAF8   /* 1 = later story phase, set by start points 2, 3, 5 and 6 */
#define L21_RED_KEY_STAGE                  0xB28   /* red key door at the cell: 0 first try, 1 second, 2 third (L21D57/58/59) */
#define L21_SUNSEER2_STAGE                 0xB38   /* Sunseer19 presence after the fight: 0 absent, 1 or 2 present (comment variants L21D81/82), 5 gone */
#define L21_ALERT_STAGE                    0xB3C   /* 0 = calm (music 1), 1 = alarm after the break-in, 2 = alarm and exit zones 16/20 open */
#define L21_LORY_SCENE_ARMED               0xB58   /* 1 = after the fight the Lory/Phedoria events 2/34/35 may run */
#define L21_AFTERMATH_STAGE                0xB88   /* after the guard fight: 1 = Lory and Phedoria to be placed, 2 = placed, 3 = cutscene started (also L01 start points) */
#define L21_SUNSEER1_FATE                  0xE24   /* Sunseer18: 0 alive and unseen, 1 seen, 2 dead */
#define L21_DEATH_ENDING_DONE              0xE8C   /* 1 = death ending already played, INIT runs the normal setup (same slot in every level) */
#define L21_PASSAGE_OPEN                   0xF38   /* 1 = the way to room 22 is open after the fight; zone 20 then changes room */
#define L21_THANANDAR_KNOCKED_OUT          0xF64   /* set to 1 when the hero was beaten down by Thanandar25 (L21D71 scene) */

#endif /* DKE_LIEU21_INT_H */
