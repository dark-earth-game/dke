#ifndef DKE_LIEU51_INT_H
#define DKE_LIEU51_INT_H

/*
 * lieu51_int.h — shared internals of the LIEU51.DLL port, split between
 * lieu51.c (entry, INIT, tick, tick body) and lieu51_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001A81C). */

/* Level 51 script variables */
#define L51_ENTRY_X            0x054  /* hero X at room entry (debug "test30"); used to pick the arrival spot */
#define L51_ENTRY_Z            0x058  /* hero Z at room entry (debug "test31") */
#define L51_ARRIVAL_PENDING    0x064  /* 1 = hero just arrived from room 10 (screen fade, L51D17/D18 line pending in trigger 41), then 0 */
#define L51_LEAVE_STAGE        0x068  /* exit walk: 1 = joindre to the exit running, 2 = arrived, 3 = ready for music/cinematic, 4 = SC18/SC19 cinematic and change_piece(40) */
#define L51_EVENT1_ZONE        0x06C  /* zone id (7..22) that event 1 re-adds with ajoute_zone; 0 after INIT */
#define L51_ZONE25_USED        0x070  /* 1 = zone 25 line (L51D111/D112) said and zone retired; tick re-adds zone 25 once hero Z < -180 and resets to 0 */
#define L51_ZONE11_SINCE       0x088  /* game time (ST_GAME_TIME) when hero entered zone 11 facing area; 0 = not timing; after 500 ms hero comments (LCOD67/68) */
#define L51_DIST_X2550         0x0F0  /* |2550 - L51_ENTRY_X|: distance to the west exit zones 1/2 (debug "test21") */
#define L51_UNUSED_F4          0x0F4  /* reset to 0 in trigger 0, never read */
#define L51_DIST_X5170         0x0F8  /* |5170 - L51_ENTRY_X|, then min with L51_DIST_X4475_B (debug "test23") */
#define L51_DIST_X5650         0x0FC  /* |5650 - L51_ENTRY_X|: distance to the east side (debug "test24") */
#define L51_DIST_X4475         0x100  /* |4475 - L51_ENTRY_X|: distance to the south end (debug "test25") */
#define L51_DIST_X4475_B       0x114  /* |4475 - L51_ENTRY_X| again, folded into L51_DIST_X5170 (debug "test26") */
#define L51_CAM5_SEEN          0x51C  /* camera 5 first lock: L51D107 said once; 0 = not yet, 1 = done */
#define L51_ZONE5_FIRST_LOOK   0x530  /* zone 5 (broken pulley) first examine: 0 = L51D100 plays, 1 = short L51D03/D04 afterwards */
#define L51_ROOM8_EXIT         0xB54  /* room-8 link: written 2 (trigger 1) / 1 (trigger 2) when leaving to room 8; LIEU10 writes 3/4 on the way back and INIT places hero at x 2712, z -1622 (3) / -1100 (4); cleared at end of INIT */
#define L51_PULLEY_STATE       0xF30  /* well pulley: 0 = untouched, 1 = seen (set by LIEU23), 2 = Pulley Blade (item 51) fitted, OB29A replaces OB48 */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu51_trig.c). */
void l51_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu51_trig.c):
 *   l51_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l51_trig00(void);

#endif /* DKE_LIEU51_INT_H */
