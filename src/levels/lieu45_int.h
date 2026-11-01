#ifndef DKE_LIEU45_INT_H
#define DKE_LIEU45_INT_H

/*
 * lieu45_int.h — shared internals of the LIEU45.DLL port, split between
 * lieu45.c (entry, INIT, tick, tick body) and lieu45_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001A6FC. */

/* Level 45 script variables */
#define L45_DELIA_FOLLOW       0x070  /* Delia state: 1 = follows the hero (set at INIT when Delia phase is 9/10), 10 = stopped to talk, modify_var restores 1 */
#define L45_DELIA_STOPPED      0x074  /* 1 = Delia halted next to the hero (distance < 140), 0 = free to follow again (>= 180 walk, >= 300 run) */
#define L45_DOOR_STATE         0x5B8  /* Bogdaran's door (zone 14): 0/1 = closed (LCOD12..14 locked messages; 0 on start point 1), 2 = opened with the Bogdaran key (item 16), IM23 anim */
#define L45_OB24A_TAKEN        0x5D8  /* gourd OB24A on the shelf: 0 = still there (INIT places it), 1 = taken */
#define L45_OB08_TAKEN         0xB80  /* items OB08A..C taken from the shelf (zone 4), one per action: 0..3 (3 = L45D07 "nothing left") */
#define L45_OB33_STAGE         0xB84  /* items OB33A/B taken from zone 5 (INIT sets 1 for Arkhan2/3): 0 = not available (LCOD06 shrug), 1 = OB33A there, 2 = OB33B there, 3 = both taken */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu45_trig.c). */
void l45_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu45_trig.c):
 *   l45_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l45_trig00(void);

#endif /* DKE_LIEU45_INT_H */
