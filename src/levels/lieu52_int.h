#ifndef DKE_LIEU52_INT_H
#define DKE_LIEU52_INT_H

/*
 * lieu52_int.h — shared internals of the LIEU52.DLL port, split between
 * lieu52.c (entry, INIT, tick, tick body) and lieu52_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x10017F4C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu52_trig.c). */
void l52_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu52_trig.c):
 *   l52_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l52_trig00(void);

/* Level-52 script variables (credits room) */
#define L52_UNUSED_64              0x64  /* scratch; only zeroed by trigger 0 */
#define L52_UNUSED_68              0x68  /* scratch; only zeroed by trigger 0 */
#define L52_UNUSED_B8              0xB8  /* scratch; only zeroed by trigger 0 */
#define L52_UNUSED_BC              0xBC  /* scratch; only zeroed by trigger 0 */
#define L52_ENDING_VISIT           0xEC  /* 1 = arrived at the end of the game (start point 1; exit trigger 15 ends the game), 0 = normal visit (exit to room 2) */
#define L52_STORY_STAGE            0xE3C  /* game-wide story stage; reset to 0 for the ending visit */

#endif /* DKE_LIEU52_INT_H */
