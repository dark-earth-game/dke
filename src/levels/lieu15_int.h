#ifndef DKE_LIEU15_INT_H
#define DKE_LIEU15_INT_H

/*
 * lieu15_int.h — shared internals of the LIEU15.DLL port, split between
 * lieu15.c (entry, INIT, tick, helpers) and lieu15_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1002EBAC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu15_trig.c). */
void l15_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu15_trig.c):
 *   l15_trig00 = trigger 0, trig00_10007543 (thunk sub_10001073; INIT default) */
void l15_trig00(void);

/* DLL globals shared by the tick body and the trigger handlers (lieu15.c). */
extern c8  g_l15_txt[256];     /* debug sprintf buffer, dword_1002E8C0 */
extern i32 g_dat_26B34;    /* dword_10026B34 (.data, starts 0) */
extern i32 g_dat_26B38;    /* dword_10026B38 (.data, starts 0): set by the tick when Danrys' Kalhi scene is armed */
extern i32 g_dat_26B3C;    /* dword_10026B3C (.data, starts 0): weapon-reaction kind (0/1) */

/* sub_10007400: Danrys' pre-scene (hero in ecx, persona in edx; the three
 * stack arguments are unused). */
void l15_pre_scene2(i32 hero, i32 n, i32 a1, i32 a2, i32 a3);

#endif /* DKE_LIEU15_INT_H */
