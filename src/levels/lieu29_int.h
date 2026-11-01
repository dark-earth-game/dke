#ifndef DKE_LIEU29_INT_H
#define DKE_LIEU29_INT_H

/*
 * lieu29_int.h - shared internals of the LIEU29.DLL port, split between
 * lieu29.c (entry, INIT, tick, helpers) and lieu29_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001C4DC). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu29_trig.c). */
void l29_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu29_trig.c):
 *   l29_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l29_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L29_LORY_NEAR                      0x064  /* 1 while the hero stands close to Lory (distance < 100), cleared when far */
#define L29_UNUSED_68                      0x068  /* only zeroed at INIT, never read in this level */
#define L29_PHEDORIA_WALKING               0x06C  /* 1 while Phedoria walks to the hero (tick re-faces her when she arrives) */
#define L29_PHEDORIA_SENT_AWAY             0x070  /* 1 once Phedoria was sent to her post after the guard fight */
#define L29_UNUSED_74                      0x074  /* only zeroed at INIT, never read in this level */
#define L29_ZONE41_SCENE_DONE              0x078  /* 1 once the entry scene (trigger 41) ran */
#define L29_UNUSED_7C                      0x07C  /* only zeroed at INIT, never read in this level */
#define L29_ZONE7_ONCE                     0x080  /* trigger 7: 1 once the first scene ran */
#define L29_PHEDORIA_STANCE_WARNED         0x084  /* 1 while Phedoria reacts to the hero's drawn weapon */
#define L29_ZONE7_PRE_FIGHT_ONCE           0x088  /* trigger 7 scene before the guard fight, once */
#define L29_DEATH_HANDLED                  0x090  /* 1 once the hero-dead check ran in the tick */
#define L29_PHEDORIA_CRY_TARGET            0x094  /* battle cry target: 99 reset, P_PHEDORIA once the shout hit Phedoria */
#define L29_PHEDORIA_CRY_STAGE             0x098  /* 0 first battle cry (L29D52), 1 repeated */
#define L29_UNUSED_B8                      0x0B8  /* only zeroed at INIT, never read in this level */
#define L29_LORY_CRY_TARGET                0x0BC  /* battle cry target: 99 reset, P_LORY once the shout hit Lory */
#define L29_LORY_STRIKE_WARNED             0x164  /* trigger 3: 1 once Lory was warned about the strike */
#define L29_PHEDORIA_STAGE                 0xB58  /* Phedoria/Lory plot stage: 10 = Lory and Phedoria present */
#define L29_STORY_MARK_C48                 0xC48  /* 99 = preset, 100 = past (shared with L26-L28) */
#define L29_LORY_TALK_STAGE                0xCA0  /* trigger 2 conversation with Lory: 0 first, 1 talked, 2 follow-up */
#define L29_LORY_ITEM_STAGE                0xCA4  /* trigger 2 second branch: 0 first, 1 repeated */
#define L29_LORY_ITEM_STAGE_B              0xCAC  /* trigger 2 third branch: 0 first, 1 repeated */
#define L29_ZONE9_STAGE                    0xD2C  /* trigger 9 scene (special mode): 0 first, 1 repeated */
#define L29_PHEDORIA_SCENE_STAGE           0xE04  /* Phedoria follow stage: 0 not met, 1 met, 2 leaving, 3 gone to room 27 */

#endif /* DKE_LIEU29_INT_H */
