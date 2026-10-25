#ifndef DKE_LIEU36_INT_H
#define DKE_LIEU36_INT_H

/*
 * lieu36_int.h - shared internals of the LIEU36.DLL port, split between
 * lieu36.c (entry, INIT, tick, helpers) and lieu36_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10020C5C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu36_trig.c). */
void l36_trigger(i32 idx);

/* Trigger handlers the core also calls directly (defined in lieu36_trig.c):
 *   l36_trig00 = trigger 0, sub_10007547 (thunk sub_10001073; INIT default)
 *   l36_trig02 = trigger 2, sub_10009C0A (thunk sub_100010BE; tick, cri)
 *   l36_trig04 = trigger 4, sub_1000C0E1 (thunk sub_10001082; tick, cri) */
void l36_trig00(void);
void l36_trig02(void);
void l36_trig04(void);

/* Send persona `who` (Lodar 86 / Konkalite 82) at the hero: sub_10007400
 * (thunk sub_100010C3), __fastcall(ecx = persona); defined in lieu36.c. */
void l36_sub_7400(i32 who);

/* dword_10020C90: ST(0x8D4) as of the last tick; trigger 0 seeds it. */
extern i32 l36_prev_8d4;

#endif /* DKE_LIEU36_INT_H */
