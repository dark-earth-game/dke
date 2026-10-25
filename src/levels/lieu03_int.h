#ifndef DKE_LIEU03_INT_H
#define DKE_LIEU03_INT_H

/*
 * lieu03_int.h — shared internals of the LIEU03.DLL port, split between
 * lieu03.c (entry, INIT, tick, helpers) and lieu03_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001DFFC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu03_trig.c). */
void l03_trigger(i32 idx);

/* Engine slots without a typed lieu_cb_t member (lieu03.c): PlaceAnimal
 * (slot 33) and LoadAnim (slot 35). No-ops while the bridge has no
 * implementation. */
void l03_place_animal(i32 a, i32 b, i32 c, i32 d, i32 e, i32 f);
void l03_load_anim(const c8 *name, i32 a);

/* Trigger handler the core also calls directly (defined in lieu03_trig.c):
 *   l03_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l03_trig00(void);

#endif /* DKE_LIEU03_INT_H */
