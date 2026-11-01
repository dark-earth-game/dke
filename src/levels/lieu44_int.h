#ifndef DKE_LIEU44_INT_H
#define DKE_LIEU44_INT_H

/*
 * lieu44_int.h — shared internals of the LIEU44.DLL port, split between
 * lieu44.c (entry, INIT, tick, tick body) and lieu44_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_100180CC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu44_trig.c). */
void l44_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu44_trig.c):
 *   l44_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l44_trig00(void);

/* Level-44 script variables */
#define L44_HERO_ACTING            0x64  /* 1 = hero is playing the zone-7 action animation (creature check is paused), 0 = idle */
#define L44_CAUGHT_STATE           0x68  /* 0 = safe, 1 = a creature came within 100 (death sequence pending), 2 = death sequence started */
#define L44_CAMERA_RELEASE_PENDING 0x6C  /* 1 = intro dialogue done; the next up/down key press returns the camera to auto, then 0 */
#define L44_ACTION_ANIM_DONE       0x70  /* 1 = zone-7 action animation finished; the next action-edge plays Sar_10 and resets 0x64/0x70 */
#define L44_NO_DEATH_ENDING        0xE8C  /* 1 = INIT skips the dead-hero ending even if the hero is dead */

#endif /* DKE_LIEU44_INT_H */
