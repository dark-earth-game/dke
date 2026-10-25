#ifndef DKE_LIEU24_INT_H
#define DKE_LIEU24_INT_H

/*
 * lieu24_int.h - shared internals of the LIEU24.DLL port, split between
 * lieu24.c (entry, INIT, tick, helpers) and lieu24_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x10021C9C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu24_trig.c). */
void l24_trigger(i32 idx);

/* Trigger handlers the core also calls directly (defined in lieu24_trig.c):
 *   l24_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default)
 *   l24_trig11 = trigger 11, sub_1000D1BA (thunk sub_10001028; the tick's
 *                battle-cry branch re-runs it) */
void l24_trig00(void);
void l24_trig11(void);

/* DLL global shared by the tick body and trigger 8 (lieu24.c): dword_1001DB34
 * (.data, starts 0). Thanandar's walk-back-to-the-hero step: 0 idle, 1 walking,
 * 2 done. */
extern i32 g_l24_heisgoing;

/* Value the engagement slot (V_ENGAGED_PERSONA) holds while the Guardian
 * fight is on; the DLL compares it against the literal 0x56. */
#define L24_ENGAGED_GUARDIAN 0x56

#endif /* DKE_LIEU24_INT_H */
