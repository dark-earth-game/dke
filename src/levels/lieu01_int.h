#ifndef DKE_LIEU01_INT_H
#define DKE_LIEU01_INT_H

/*
 * lieu01_int.h — shared internals of the LIEU01.DLL port, split between
 * lieu01.c (entry, INIT, tick, events, helpers) and lieu01_trig.c (the
 * zone trigger handlers).
 */

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels.h"
#include "lieu01.h"

/* The state buffer of the call in flight (the DLL's dword_1002C96C). */
extern lieu_state_t *g_l01_S;

#define ST(off)   LIEU_ST(g_l01_S, (off))
#define CB        (g_lieu_cb)

/* Trigger dispatch (op 3) and trigger 0 (INIT), lieu01_trig.c. */
void l01_trigger(i32 idx);
void l01_trig00(void);

/* DLL helpers shared by both files (lieu01.c). */
void l01_action_register(const c8 *name, i32 count, const c8 *arg0, const c8 *arg4);  /* sub_1001932C */
void l01_spawn_ar(void);                                                               /* "ar" at the hero's spot */

#endif /* DKE_LIEU01_INT_H */
