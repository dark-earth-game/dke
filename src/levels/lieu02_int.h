#ifndef DKE_LIEU02_INT_H
#define DKE_LIEU02_INT_H

/*
 * lieu02_int.h — shared internals of the LIEU02.DLL port, split between
 * lieu02.c (entry, INIT, tick, events, helpers) and lieu02_trig.c (the
 * zone trigger handlers).
 */

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels.h"
#include "lieu_names.h"

/* The state buffer of the call in flight (the DLL's dword_1002C98C). */
extern lieu_state_t *g_l02_S;

#define ST(off)   LIEU_ST(g_l02_S, (off))
#define CB        (g_lieu_cb)

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu02_trig.c). */
void l02_trigger(i32 idx);

/* DLL helper functions used from both files are declared here with an
 * l02_ prefix and defined in lieu02.c. */

/* sub_1001A1AC (thunk sub_1000100A) — same body as LIEU01's
 * action_register (sub_1001932C): __fastcall(ecx=name, edx=count) +
 * stack (arg0 = [ebp+8], arg4 = [ebp+0Ch]). Defined in lieu02.c. */
void l02_action_register(const c8 *name, i32 count,
                         const c8 *arg0, const c8 *arg4);

/* Trigger handlers the core also calls directly (defined in
 * lieu02_trig.c):
 *   l02_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default)
 *   l02_trig31 = trigger 31, sub_100149FB (thunk sub_10001000; tick body) */
void l02_trig00(void);
void l02_trig31(void);

#endif /* DKE_LIEU02_INT_H */
