#ifndef DKE_LIEU28_INT_H
#define DKE_LIEU28_INT_H

/*
 * lieu28_int.h - shared internals of the LIEU28.DLL port, split between
 * lieu28.c (entry, INIT, tick, helpers) and lieu28_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001FDAC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu28_trig.c). */
void l28_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu28_trig.c):
 *   l28_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l28_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L28_UNUSED_64                      0x064  /* zeroed at INIT, set to 4 in trigger 2, never read */
#define L28_ZONE4_STAGE                    0x068  /* trigger 4 comment: 0 first (LCOD10/11), 1 repeat */
#define L28_ARCHIVIST_LEFT_POST            0x06C  /* 1 once the Archivist walked away while the hero was far */
#define L28_UNUSED_70                      0x070  /* only zeroed at INIT, never read in this level */
#define L28_DOOR_EVENT1_STEP               0x074  /* exit event 1: 0 start (door anim), 1 door opened, reset when the camera unlocks */
#define L28_DEATH_HANDLED                  0x078  /* 1 once the hero-dead check ran in the tick */
#define L28_UNUSED_7C                      0x07C  /* only zeroed at INIT, never read in this level */
#define L28_ARCHIVIST_BATTLE_CRY           0x080  /* 99 idle, 5 = battle cry heard (event 34 plays L28D22) */
#define L28_PARCHMENT_SHOWN                0x084  /* 1 once the parchment was shown to the Archivist (trigger 6) */
#define L28_ARCHIVIST_TALK_FLAG            0x190  /* trigger 2: 0 first, 1 seen */
#define L28_ARCHIVIST_TALK_STAGE           0x4C4  /* trigger 2 conversation round with the Archivist: 0..3 */
#define L28_ARCHIVIST_ENTER_ONCE           0x548  /* 1 once the entry line at B38 == 0 was played */
#define L28_ZONE7_ONCE                     0x5C8  /* trigger 7: 0 first message, 1 repeat */
#define L28_ARCHIVIST_GREETED              0x774  /* 1 once the greeting was played at B38 >= 1 */
#define L28_PARCHMENT_HINT_COUNT           0x8DC  /* trigger 6 hint: 0 first, 1 repeated */
#define L28_ARCHIVIST_TALK_DONE            0xB28  /* set to 1 at the end of the trigger 2 conversation */
#define L28_ARCHIVIST_STAGE                0xB38  /* story stage in the Archivist room: 0 first visit, 1 returning, 2 later, 3 finished (shared with L26, L27, L29) */
#define L28_STORY_MARK_C48                 0xC48  /* 99 = preset, 100 = past (shared with L26, L27, L29) */
#define L28_ARCHIVIST_ITEM_STAGE           0xD30  /* trigger 2 item request: 0 first, 1 repeated */
#define L28_CHASE_STATE                   0xE8C  /* Gork/Sordos chase state (set by rooms 31/32): 1 = chase running, INIT then skips the hero-dead ending */
#define L28_ARCHIVIST_QUEST_STAGE          0xEB0  /* Archivist quest: 0 not started, 1 started (start point 1), 2 asked, 3 answered */
#define L28_ARCHIVIST_ANSWER_STAGE         0xEBC  /* answer sub-stage of trigger 2 round 3: 0 first, 1 given, 2 pending, 3 done */

#endif /* DKE_LIEU28_INT_H */
