#ifndef DKE_LIEU43_INT_H
#define DKE_LIEU43_INT_H

/*
 * lieu43_int.h — shared internals of the LIEU43.DLL port, split between
 * lieu43.c (entry, INIT, tick, tick body) and lieu43_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001923C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu43_trig.c). */
void l43_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu43_trig.c):
 *   l43_trig00 = trigger 0, trig00_10001200 (thunk sub_10001073; INIT default) */
void l43_trig00(void);

/* DLL globals shared by the tick body and the trigger handlers (lieu43.c).
 * All are .data of the DLL and restart on every room load. The two levers
 * (left, right) each keep a position (-1/0/1; starts 666 = unset), an
 * "initialised" flag, the current animation slot and per-move flags. */
extern c8  g_l43_txt[256];   /* debug sprintf buffer, dword_10018F50 (starts "") */
extern i32 g_l43_hits;       /* dword_10015A30: hit counter of persona 40 */
extern i32 g_l43_left_pos;   /* dword_10015A38: left lever position (starts 666) */
extern i32 g_l43_right_pos;  /* dword_10015A3C: right lever position (starts 666) */
extern i32 g_l43_left_init;  /* dword_10015A40: left lever first use done */
extern i32 g_l43_right_init; /* dword_10015A44: right lever first use done */
extern i32 g_l43_cur_left;   /* dword_10015A48: current_gauche (anim slot) */
extern i32 g_l43_cur_right;  /* dword_10015A4C: current_droit (anim slot) */
extern i32 g_l43_flag[10];   /* dword_10015A50..A74: per-move flags; index (addr-0x10015A50)/4 */

#define G_A50   g_l43_flag[0]   /* dword_10015A50 */
#define G_A54   g_l43_flag[1]   /* dword_10015A54 */
#define G_A58   g_l43_flag[2]   /* dword_10015A58 */
#define G_A5C   g_l43_flag[3]   /* dword_10015A5C */
#define G_A60   g_l43_flag[4]   /* dword_10015A60 */
#define G_A64   g_l43_flag[5]   /* dword_10015A64 */
#define G_A68   g_l43_flag[6]   /* dword_10015A68 */
#define G_A6C   g_l43_flag[7]   /* dword_10015A6C */
#define G_A70   g_l43_flag[8]   /* dword_10015A70 */
#define G_A74   g_l43_flag[9]   /* dword_10015A74 */

#endif /* DKE_LIEU43_INT_H */
