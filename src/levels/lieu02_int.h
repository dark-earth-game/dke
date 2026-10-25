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

/* The state buffer of the call in flight (the DLL's dword_1002C96C). */
extern lieu_state_t *g_l02_S;

#define ST(off)   LIEU_ST(g_l02_S, (off))
#define CB        (g_lieu_cb)

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu02_trig.c). */
void l02_trigger(i32 idx);

/* DLL helper functions used from both files are declared here with an
 * l02_ prefix and defined in lieu02.c. */

#endif /* DKE_LIEU02_INT_H */
