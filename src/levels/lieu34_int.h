#ifndef DKE_LIEU34_INT_H
#define DKE_LIEU34_INT_H

/*
 * lieu34_int.h - shared internals of the LIEU34.DLL port, split between
 * lieu34.c (entry, INIT, tick, tick body) and lieu34_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's state pointer global (0x1001BA2C). */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu34_trig.c). */
void l34_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu34_trig.c):
 *   l34_trig00 = trigger 0, trig00_100073F0 (thunk 0x10001073; INIT default) */
void l34_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L34_DEATH_HANDLED                  0x064  /* 1 once the hero-dead check ran in the tick */
#define L34_SORDOS_ATTACK_STEP             0x068  /* Sordos attack on the hero: 0 idle, 1 anim started, 3 OB50 thrown/Sordos engaged */
#define L34_SORDOS_ENGAGED                 0x06C  /* 10 once Sordos/Thanandar24 fight was set up (blocks a repeat) */
#define L34_LAST_LIVING_HERO               0x0E8  /* last ST_HERO value that was not the dead hero (4) */
#define L34_ZONE8_STAGE                    0x54C  /* trigger 8 object stage: 0 first look, 1 looked */
#define L34_ZONE8_STAGE_B                  0x784  /* trigger 8: 1 once the first scene ran */
#define L34_ZONE10_ONCE                    0x824  /* trigger 10: 0 first, 1 repeated */
#define L34_TALK_REPEATED                  0xB1C  /* 0 = first talk (trigger 3), 1 = repeated */
#define L34_STORY_PART                     0xBB0  /* arriving state: 2 Sordos/Thanandar24 duel pending, 10 after it (shared with L31, L32) */
#define L34_SORDOS_SCENE_KIND              0xC64  /* 1 = Sordos and Thanandar24 present (scene), 2 = Sordos alone */
#define L34_SORDOS_DUEL_TALKED             0xEB8  /* trigger 5: 1 once the post-duel talk ran */

#endif /* DKE_LIEU34_INT_H */
