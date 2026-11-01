#ifndef DKE_LIEU46_INT_H
#define DKE_LIEU46_INT_H

/*
 * lieu46_int.h — shared internals of the LIEU46.DLL port, split between
 * lieu46.c (entry, INIT, tick, tick body) and lieu46_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001859C. */

/* Level 46 script variables */
#define L46_BODY_SEARCHED      0x064  /* Rylsadhar21 corpse (zone 3): 0 = first action plays the remark, 1 = afterwards fouille */
#define L46_CAM2_LOCKED        0x068  /* tick: 1 = camera 2 forced locked (exit stage >= 3), reset to auto camera 1 when hero X < 2400 */
#define L46_ZONE4_SEEN         0x0E0  /* zone 4 remark (LCOD70/71) said once; 0 = not yet, 1 = done */
#define L46_QUEST_STAGE_EB0    0xEB0  /* quest slot shared with LIEU28/25/6/5: 1 or 2 (in progress) becomes 3 (done) when the ambush plays */
#define L46_PLOT_STAGE_F24     0xF24  /* plot stage shared with LIEU14/18/54/6: the ambush sets 3 */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu46_trig.c). */
void l46_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu46_trig.c):
 *   l46_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l46_trig00(void);

#endif /* DKE_LIEU46_INT_H */
