#ifndef DKE_LIEU27_INT_H
#define DKE_LIEU27_INT_H

/*
 * lieu27_int.h - shared internals of the LIEU27.DLL port, split between
 * lieu27.c (entry, INIT, tick, helpers) and lieu27_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001D93C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu27_trig.c). */
void l27_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu27.c):
 *   l27_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l27_trig00(void);

/* DLL .data dword_1001AB34: destination room of the door the hero used
 * (20, 28 or 29); set by the trigger handlers, read by the tick body.
 * Starts at 0 on every room load (reset at the top of INIT). */
extern i32 g_dest_room;

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L27_ZONE33_ENTERED                 0x054  /* 1 once the zone 33 scene (Arkhan2 sees Phedoria/Lory) ran */
#define L27_UNUSED_64                      0x064  /* only zeroed at INIT, never read in this level */
#define L27_UNUSED_68                      0x068  /* only zeroed at INIT, never read in this level */
#define L27_UNUSED_6C                      0x06C  /* only zeroed at INIT, never read in this level */
#define L27_LORY_PATH_STEP                 0x070  /* Lory walking path: 0 start, 1..3 waypoints reached (zones 10, 12, 13), 100 stopped by the fight */
#define L27_PHEDORIA_PATH_STEP             0x074  /* Phedoria walking path: 0 start, 1..2 waypoints reached (zones 10, 11), 100 stopped by the fight */
#define L27_KONKALITE_SCENE_TIME           0x084  /* ST_GAME_TIME stamped when the Konkalite4 scene started */
#define L27_KONKALITE_SCENE_ON             0x088  /* 1 while the Konkalite4 scene runs */
#define L27_UNUSED_90                      0x090  /* only zeroed at INIT, never read in this level */
#define L27_UNUSED_94                      0x094  /* only zeroed at INIT, never read in this level */
#define L27_UNUSED_98                      0x098  /* only zeroed at INIT, never read in this level */
#define L27_AWAY_TIME_MS                   0x0A4  /* ms since the hero left (ST_GAME_TIME - LEAVE_TIME), -1 if never left */
#define L27_LEAVE_TIME                     0x0A8  /* ST_GAME_TIME stamped when the hero left, -1 = none */
#define L27_GATE_SCENE_DONE                0x0B8  /* 1 once the gate approach scene (position check z > -450) ran */
#define L27_CAMERA_LOCK_ZONE               0x0E0  /* 1 while the camera is locked near zone 37 */
#define L27_UNUSED_F0                      0x0F0  /* only zeroed at INIT, never read in this level */
#define L27_PHEDORIA_HOSTILE               0x874  /* set to 5 when the hero attacks Phedoria (trigger 9) */
#define L27_ZONE_SEEN                      0x950  /* 1 once the trigger 8/9 scene was seen */
#define L27_PHEDORIA_STAGE                 0xB58  /* Phedoria/Lory plot stage: 4/5 = trap sprung (time-based block), 10 = start preset */
#define L27_STORY_MARK_C48                 0xC48  /* 99 = preset, 100 = past the Lory scene (shared with L26, L28, L29) */
#define L27_ZONE17_STAGE                   0xF34  /* trigger 17: 0 first message, 1 repeat */

#endif /* DKE_LIEU27_INT_H */
