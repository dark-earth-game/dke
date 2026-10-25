#ifndef DKE_LIEU01_INT_H
#define DKE_LIEU01_INT_H

/*
 * lieu01_int.h — shared internals of the LIEU01.DLL port, split between
 * lieu01.c (entry, INIT, tick, events, helpers) and lieu01_trig.c (the
 * zone trigger handlers).
 */

#include "lieu_level.h"
#include "lieu01.h"

/* ST() reads g_lvl_S, the DLL's dword_1002C96C. */

/* Trigger dispatch (op 3) and trigger 0 (INIT), lieu01_trig.c. */
void l01_trigger(i32 idx);
void l01_trig00(void);

/* DLL helpers shared by both files (lieu01.c). */
void l01_spawn_ar(void);                                                               /* "ar" at the hero's spot */

#endif /* DKE_LIEU01_INT_H */
