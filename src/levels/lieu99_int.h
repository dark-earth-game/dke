#ifndef DKE_LIEU99_INT_H
#define DKE_LIEU99_INT_H

/*
 * lieu99_int.h — shared internals of the LIEU99.DLL port, split between
 * lieu99.c (entry, INIT, tick, helpers) and lieu99_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* Script variables of this level (state block slots; low slots are per-level scratch). */
#define L99_PHASE_STEP                   0x64   /* per-phase progress, reset at every phase change: phase 0 = intro lines queued (1), phase 1 = 10 s timer armed (1), phase 2 = Malrot2 approach 0 idle, 1 walking up, 2 talking */
#define L99_MAOR_LOOK_DONE               0x68   /* 1 once Maor was told to look at the player (hero > 750 from descriptor 3); reset at phase change */
#define L99_MALROT2_SCENE_REQ            0x6C   /* set to 1 (modify_var) to play the Malrot2 L18D188/189 scene, then cleared */
#define L99_CINE_TOGGLE                  0x70   /* alternates 0/1 each timeout: picks cinematic SC01_500 or SC02_500 (and Malrot2's P62ATT02/04 pose) */
#define L99_LEONA_LOOK_DONE              0x74   /* 1 once Leona was told to look at the player in phase 2; reset at the end of phase 2 */
#define L99_SCENE_PHASE                  0x100   /* test-room scene phase, runs 0 -> 1 -> 2 -> 0 as the event timers fire (cast and camera per phase) */
#define L99_DEATH_ENDING_BYPASS          0xE8C   /* 1 = hero death does not play the death ending in INIT (runs trigger 0 instead); shared flag */

/* ST() reads g_lvl_S, the DLL's dword_100183DC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu99_trig.c). */
void l99_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu99_trig.c):
 *   l99_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l99_trig00(void);

#endif /* DKE_LIEU99_INT_H */
