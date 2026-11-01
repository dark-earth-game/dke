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

/* Level 11 script variables (low slots 0x64.. are scratch private to this level; 0x338.. and above are game-wide story flags) */
#define L11_UNUSED_64             0x064   /* only zeroed by the INIT trigger; never read in this level */
#define L11_GUARD_WARN_FAR_DONE   0x068   /* 1 = guard's first warning (L11D01, distance < 500, Arkhan3 sneaking) already played; reset at INIT */
#define L11_GUARD_ATTACK_DONE     0x06C   /* 1 = guard attacks Arkhan3 (L11D02, distance < 200) already triggered; reset at INIT */
#define L11_GUARD_WARN2_FAR_DONE  0x070   /* 1 = guard's first warning to Arkhan2 (L11D03, distance < 400) already played; reset at INIT */
#define L11_GUARD_WARN2_NEAR_DONE 0x074   /* 1 = guard's second warning to Arkhan2 (L11D04, distance < 250) already played; reset at INIT */
#define L11_GUARD_TALK_BUSY       0x078   /* 1 while the hero's L11D06/L11D07 line to the guard is running (modify_var clears it); reset at INIT */
#define L11_DEATH_EVENT_SENT      0x07C   /* 1 = hero dead and the EV_END_GAME timer was already queued; reset at INIT */
#define L11_CRI_STATE             0x080   /* 32 = shout (cri) at the guard pending for event 0 (starts the fight), 99 = already consumed */
#define L11_CAMERA_PAUSED         0x084   /* 1 = lever animation paused while the camera is locked to 3 (V_GUARD_FIGHT_DONE == 2); reset when the camera is released */
#define L11_GUARD_HOSTILE         0x4E4   /* 1 = the guard is hostile / fight engaged (set by attack, hit, shout, bribe failure); 0 = still peaceful */
#define L11_GUARD_LOOT_HINT       0x60C   /* 1 = hint line (L11D43/L11D44) after searching the dead guard already shown */
#define L11_GUARD_BRIBE_DIALOG    0xC9C   /* conversation step with the guard (special mode): 0..5 = L11D08..L11D16 lines in order, 5 = the bribe offer is open */
#define L11_GUARD_BRIBE_STAGE     0xEA4   /* bribe of the guard: 0 = not offered, 1 = offered (L11D17-L11D19), 2 = given meat/fish (he is distracted), 3 = guard dealt with using the full gourd (he dies, gourd swapped for the flask); shown as "F37:" in the debug overlay */

#endif /* DKE_LIEU11_INT_H */
