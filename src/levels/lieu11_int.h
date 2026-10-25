#ifndef DKE_LIEU11_INT_H
#define DKE_LIEU11_INT_H

/*
 * lieu11_int.h — shared internals of the LIEU11.DLL port, split between
 * lieu11.c (entry, INIT, tick, helpers) and lieu11_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001AAFC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu11_trig.c). */
void l11_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu11_trig.c):
 *   l11_trig00 = trigger 0, sub_10007463 (thunk sub_10001078; INIT default) */
void l11_trig00(void);

/* sub_10007400 (thunk 0x1000102D): preloads the pickup-reaction animations
 * of the guard scene. The three stack arguments are ignored by the DLL
 * (lieu11_trig.c calls it with the hero, the guard, the item and 6, 12). */
void l11_preload_anims(i32 persona, i32 target, i32 item, i32 a, i32 b);

/* DLL text buffers: dword_1001A810 (g_txt) and dword_1001A610 (g_txt2). */
extern c8 g_txt[256];
extern c8 g_txt2[256];

#endif /* DKE_LIEU11_INT_H */
