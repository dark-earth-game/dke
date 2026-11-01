#ifndef DKE_LIEU32_INT_H
#define DKE_LIEU32_INT_H

/*
 * lieu32_int.h - shared internals of the LIEU32.DLL port, split between
 * lieu32.c (entry, INIT, tick, helpers) and lieu32_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_100219FC. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu32_trig.c). */
void l32_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu32_trig.c):
 *   l32_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l32_trig00(void);

/* Script variables: slots of the shared state block this level uses as its own
 * (the low slots are scratch and mean something different in every level). */
#define L32_SORDOS_SCENE_STEP              0x064  /* Sordos/Gork chase scene: 0 start, 2/3/51/100/250 first-variant steps, 20-25 second variant, 100 waits for Sordos */
#define L32_GORK_TAUNT_ACTIVE              0x068  /* 1 while the random Gork taunt (event 2) is pending, then cleared */
#define L32_TAUNT_VARIANT                  0x06C  /* random(3)+1 selects the taunt line (switch), reused for the second taunt */
#define L32_GORK_ENCOUNTER_STEP            0x070  /* Gork encounter: 0/1 free, 2 triggered, 99 resolved */
#define L32_KONKALITE7_AMBUSH_ONCE         0x074  /* 1 once Konkalite7 and Gork stopped the hero (L32D13/D14) */
#define L32_KONKALITE7_FOLLOWUP            0x078  /* 1 -> 2: Konkalite7 follow-up step (2 = Gork walks to the hero) */
#define L32_TIMER_OR_FLAG                  0x07C  /* dual use: Konkalite7 chase flag (0/1), later the ST_GAME_TIME stamp of the Konkalite3 wait */
#define L32_KONKALITE3_TAUNT               0x080  /* random(3)+1 line chosen for the Konkalite3 taunt after 5 s, 0 = none yet */
#define L32_CHEER_PLAYED                   0x084  /* 1 once the Malrot6 reaction scene started */
#define L32_ZONE14_ONCE                    0x088  /* trigger 14 message: 0 first, 1 repeat */
#define L32_CAMERA1_PAUSED                 0x0E0  /* 1 while the camera lock 1 pauses the ambient anim */
#define L32_LAST_LIVING_HERO               0x0E8  /* last ST_HERO value that was not the dead hero (4); restored after the cinematic */
#define L32_CAMERA_ANIM_PAUSED             0x0F0  /* 1 while the room anim is paused because the camera is locked */
#define L32_HIT_COUNT_SEEN                 0x0FC  /* last PLAYER_HIT_COUNT value that was handled (cuts speech when the hero takes a new hit) */
#define L32_DEATH_HANDLED                  0x100  /* 1 once the hero-dead check ran in the tick */
#define L32_ZONE14_HINT_STAGE              0x114  /* trigger 14: 0 first hint, 1 later hint */
#define L32_DHORKAN95_TALK_STAGE           0x4F4  /* trigger 6 conversation with Dhorkan95: 0 first, 1..4 */
#define L32_ZONE20_STAGE                   0x604  /* trigger 20 object: 0 first, 1 second */
#define L32_PLAYER_HIT_COUNT               0x8D4  /* counts hits the hero takes (written by other rooms/engine); HIT_COUNT_SEEN follows it */
#define L32_ROOM_ANIM_KIND                 0xB70  /* which ambient anim runs: 1 or 2 (pauses anim slot 0 or 15 with the camera) */
#define L32_STORY_PART                     0xBB0  /* arriving state: 2 after the Konkalite fight, 10 later (shared with L31) */
#define L32_MALROT6_DEAD                   0xC28  /* 10 once Malrot6 was killed; 1 = set by trigger 14 */
#define L32_GORK_DEAD                      0xC38  /* 0 = Gork alive, 1 = Gork dead (the Konkalite11 chase takes over) */
#define L32_ZONE11_HANDLED                 0xC3C  /* 1 once the trigger 11 scene ran (zone 11 instead of zone 30) */
#define L32_DHORKAN11_SEARCHED             0xC6C  /* 1 once Dhorkan11 was searched (trigger 6) */
#define L32_GORK_FIGHT_STARTED             0xD80  /* 1 once the Gork fight started (engaged persona set) */
#define L32_DHORKAN11_SEARCH_DONE          0xED4  /* 1 once the Dhorkan11 search ended */
#define L32_BORK_ROUTE_STAGE               0xEE0  /* preset 1 at start point 2 (see L31) */
#define L32_GORK_KONKALITE7_STATE          0xF00  /* Gork/Konkalite7 fight: 0 none, 1-3 fighting, 10 both beaten */
#define L32_ROOM_PLOT_STAGE                0xF24  /* room plot: 0 start, 1 Dhorkan95 present, 2 seen, 3 Konkalite3/Malrot6, 5 Gork, 6 door opened */
#define L32_KONKALITE3_STATE               0xF28  /* Konkalite3 fight: 0 fresh, 1 wounded, 2 dead, 3 beaten by the Malrot6 scene */
#define L32_DOORS_OPENED                   0xF48  /* 1 once the OB_KONK_KEY door (zone 13) was opened */
#define L32_CAMERA_CUT_DONE                0xF78  /* 1 once the Dhorkan95 camera cut ran */

#endif /* DKE_LIEU32_INT_H */
