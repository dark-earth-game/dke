#ifndef DKE_LIEU22_INT_H
#define DKE_LIEU22_INT_H

/*
 * lieu22_int.h — shared internals of the LIEU22.DLL port, split between
 * lieu22.c (entry, INIT, tick, helpers) and lieu22_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001A93C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu22_trig.c). */
void l22_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu22_trig.c):
 *   l22_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l22_trig00(void);

/* Level 22 script variables (scratch slots are per level; 0x32C/0x330/0x338/0x348 are the op-4 event slots) */
#define L22_ZONE20_STAGE                   0x064   /* 0 = first press of the action key at zone 20 comments, 1 = second press takes OB58/OB35/OB73A */
#define L22_DARKNESS_PULSES                0x068   /* counter of event 2 pulses (max 10): each darkens the screen and drains hero HP */
#define L22_DAMAGE_TICKS                   0x06C   /* counter of event 33 hits (max 50) draining hero HP */
#define L22_ZONE20_ITEMS_TAKEN             0x604   /* set to 1 when OB58/OB35/OB73A are taken at zone 20 (same slot as L32 zone 20) */
#define L22_CAMERA_RESTORED                0x88C   /* 0 = arrival camera still locked, 1 = camera back to auto once the hero is >250 from persona 12 */
#define L22_ENTRY_STAGE                    0xB88   /* first-entry flag: 0 -> 1 on the first INIT (also used by L01 start points and L21) */
#define L22_ARRIVAL_PLAYED                 0xCE0   /* set to 1 after the Rylsadhar/Zed arrival cutscene */
#define L22_ZONE21_REMARK_DONE             0xCE4   /* 0 = first remark about the zone 21 object pending, 1 = already said (per fight stage) */
#define L22_VISITED                        0xE40   /* set to 1 on the first visit to level 22 (L05 and L15 test it) */

#endif /* DKE_LIEU22_INT_H */
