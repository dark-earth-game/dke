#ifndef DKE_LIEU26_INT_H
#define DKE_LIEU26_INT_H

/*
 * lieu26_int.h - shared internals of the LIEU26.DLL port, split between
 * lieu26.c (entry, INIT, tick, helpers) and lieu26_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001970C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu26_trig.c). */
void l26_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu26_trig.c):
 *   l26_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l26_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L26_HP_SAVED                       0x064  /* hero HP saved before the 10 HP penalty (trigger 2 special-mode branch) */
#define L26_DOOR_EVENT0_STEP               0x068  /* exit event 0 chain: 0 start, 1 door sound, 2 change room */
#define L26_TELEPORT_ANIM_PAUSED           0x06C  /* 1 = teleporter anim (226TELES) paused while the camera is locked, 0 = running */
#define L26_ZONE10_ONCE                    0x50C  /* trigger 10: 0 first use, 1 repeated */
#define L26_ZONE4_ONCE                     0x630  /* trigger 4: 0 first look (SG00PRE02), 1 seen */
#define L26_ZONE7_STAGE                    0x808  /* trigger 7 object comment stage: 0 first, 1 second, 2 third */
#define L26_LEVEL_INIT_DONE                0xB38  /* 0 = first visit presets E74/C48 to 99, then 1 */
#define L26_KALHI_PART_STEP                0xB4C  /* Arkhan2/3 story step; set to 1 by triggers 7 and 12 */
#define L26_STORY_MARK_C48                 0xC48  /* preset to 99 on the first visit (shared with L27-L29) */
#define L26_MUSIC_STARTED                  0xD14  /* 1 once the room music started after the door event */
#define L26_ZONE8_ONCE                     0xD18  /* trigger 8: 0 first comment (L26D23), 1 seen */
#define L26_STORY_MARK_E74                 0xE74  /* preset to 99 on the first visit */
#define L26_CHASE_STATE                   0xE8C  /* Gork/Sordos chase state (set by rooms 31/32): 1 = chase running, INIT then skips the hero-dead ending */

#endif /* DKE_LIEU26_INT_H */
