#ifndef DKE_LIEU30_INT_H
#define DKE_LIEU30_INT_H

/*
 * lieu30_int.h - shared internals of the LIEU30.DLL port, split between
 * lieu30.c (entry, INIT, tick, helpers) and lieu30_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001C9AC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu30_trig.c). */
void l30_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu30_trig.c):
 *   l30_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l30_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L30_CAMERA_NEAR_ONCE               0x064  /* 1 once the hero came within 1200 of zone 2 with the camera free */
#define L30_KONKALITE5_ALERT_ONCE          0x068  /* 1 once Konkalite5 noticed the hero from zone 2 (camera locked, distance <= 500) */
#define L30_UNUSED_6C                      0x06C  /* only zeroed at INIT, never read in this level */
#define L30_UNUSED_70                      0x070  /* only zeroed at INIT, never read in this level */
#define L30_UNUSED_74                      0x074  /* only zeroed at INIT, never read in this level */
#define L30_UNUSED_78                      0x078  /* only zeroed at INIT, never read in this level */
#define L30_UNUSED_7C                      0x07C  /* only zeroed at INIT, never read in this level */
#define L30_UNUSED_80                      0x080  /* only zeroed at INIT, never read in this level */
#define L30_UNUSED_84                      0x084  /* only zeroed at INIT, never read in this level */
#define L30_GORK_WALK_STEP                 0x088  /* 1 while Gork walks to the hero, cleared when he arrives */
#define L30_DEATH_HANDLED                  0x0B8  /* 1 once the hero-dead check ran in the tick */
#define L30_KONKALITE2_DEAD_HANDLED        0x0BC  /* 1 once the Konkalite2 death was handled (collision 39 removed) */
#define L30_UNUSED_E0                      0x0E0  /* only zeroed at INIT, never read in this level */
#define L30_CAMERA_SWITCH_PENDING          0x100  /* 1 when the camera switch waits for the hero z < -2830 */
#define L30_ROOM35_RETURN                  0x4DC  /* 1 on return from room 35 (set to 3 when F24 != 1) */
#define L30_KONKALITE2_SAVED_LIFE          0x8C4  /* ST_VIE(P_KONKALITE2) saved while the camera is in the side view; 2 = dead */
#define L30_KONKALITE94_BEATEN             0x904  /* 2 once persona 94 was beaten */
#define L30_KONKALITE5_BEATEN              0x914  /* 2 once Konkalite5 was beaten */
#define L30_ARKHAN23_PART                  0xAF8  /* 0 = Arkhan1 chapter, 1 = Arkhan2/3 chapter started */
#define L30_ZONE6_DIALOGUE_FLAG            0xC50  /* 0 = third line (L31D83C/84C) still to say */
#define L30_ROOM35_HUD_RESET               0xC58  /* 1 = clear the HUD timer on return from room 35 */
#define L30_CAMERA_AREA_STATE              0xDB8  /* camera area of the Konkalite fight: 1 area A (camera 3), 2 area B (camera 4) */
#define L30_CAMERA_AREA_VISITED            0xDBC  /* 1 once camera area B was entered */
#define L30_CHASE_STATE                   0xE8C  /* Gork/Sordos chase state (set by rooms 31/32): 1 = chase running, INIT then skips the hero-dead ending */
#define L30_KONKALITE_FIGHT_STAGE          0xE98  /* Konkalite2 sequence: 0/1 start, 2 fight, 20-22 hit variants, 56 won, 60 cleared, 65 already done (preset) */
#define L30_STORY_MARK_F24                 0xF24  /* 1 = story mark that cancels ROOM35_RETURN */

#endif /* DKE_LIEU30_INT_H */
