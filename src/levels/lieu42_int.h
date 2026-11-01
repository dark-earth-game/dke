#ifndef DKE_LIEU42_INT_H
#define DKE_LIEU42_INT_H

/*
 * lieu42_int.h — shared internals of the LIEU42.DLL port, split between
 * lieu42.c (entry, INIT, tick, tick body) and lieu42_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001BA0C. */

/* Level-local script variables (scratch slots reused per level) */
#define L42_LEVER1_STATE           0x64   /* lever 1 (trigger 2) position: -1 = up/initial, 1 = pulled; puzzle solution needs 1 */
#define L42_LEVER2_STATE           0x68   /* lever 2 (trigger 3/4) position: -1 initial, 1 toggled; solution 1 */
#define L42_LEVER3_STATE           0x6C   /* lever 3 position: -1 initial, 1 toggled; solution -1 */
#define L42_LEVER4_STATE           0x70   /* lever 4 position: -1 initial, 1 toggled; solution 1 */
#define L42_LEVER5_STATE           0x74   /* lever 5 position: -1 initial, 1 toggled; solution -1 */
#define L42_LEVER6_STATE           0x78   /* lever 6 position: -1 initial, 1 toggled; solution 1 */
#define L42_CAM4_ANIMS_ON          0x7C   /* 1 while camera 4 is locked and its lever anims have been started, else 0 */
#define L42_CAM5_ANIMS_ON          0x80   /* 1 while camera 5 is locked and its lever anims have been started, else 0 */
#define L42_LEVER_MOVE             0x84   /* last lever move for the tick animation: +-11..16 = lever 1..6, negative = toggled to 1, positive = back to -1; 0 none */
#define L42_CAM3_AMBIENT           0x88   /* 1 = L42AMB2 ambience playing (camera 3 locked), 0 = L42AMB1 */
#define L42_CAM_RELEASE_PENDING    0xB8   /* 1 = hero in the locked puzzle camera, released to auto camera when far (distance>500) or via lever; 1000 = already released */
#define L42_ARKHAN3_HINT_SEEN      0xE0   /* Arkhan3 first-contact hint line: 0 = not yet, 1 = shown (the tick also stores 1..4 per lever animation step) */
#define L42_HINT_B_SAID            0xF0   /* 1 once the second L42D13B line has been said, 0 = not yet */
#define L42_GATE_OPEN              0x56C   /* 0 = gate closed, 1 = all six levers set right, zone 20 swapped for zone 25 (exit) */
#define L42_INTRO_STAGE            0x8B0   /* 0 = intro SC31_500 not played, 1 = played (trigger 41), 2 = Arkhan3 dialogue registered */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu42_trig.c). */
void l42_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu42_trig.c):
 *   l42_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l42_trig00(void);

#endif /* DKE_LIEU42_INT_H */
