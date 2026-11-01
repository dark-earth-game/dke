#ifndef DKE_LIEU13_INT_H
#define DKE_LIEU13_INT_H

/*
 * lieu13_int.h — shared internals of the LIEU13.DLL port, split between
 * lieu13.c (entry, INIT, tick, helpers) and lieu13_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001A95C. */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu13_trig.c). */
void l13_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu13_trig.c):
 *   l13_trig00 = trigger 0, sub_100073F0 (thunk sub_10001073; INIT default) */
void l13_trig00(void);

/* Level 13 script variables (low slots 0x64.. are scratch private to this level) */
#define L13_CLIMB_VARIANT     0x064   /* 1 = player climbs/lifts via special mode 1 (event 1 plays anim slot 1), 2 = special mode 0 variant (anim slot 2 then sets L13_PUZZLE_SOLVED); reset at INIT */
#define L13_PUZZLE_ANIM_ON    0x068   /* 1 while the post-solve anim (slot 2) is playing under the locked camera 3; reset at INIT */
#define L13_TALK_TOGGLE       0x06C   /* alternates 0/1 each time the hero talks to the zone: 0 plays L13D03, 1 plays L13D05; reset at INIT */
#define L13_AMBIENT_ANIM_ON   0x070   /* 1 while the ambient anim slot 5 runs (camera unlocked); reset at INIT */
#define L13_OB72_TAKEN        0x5D4   /* 1 = OB72/OB23 interaction at zone 5 already done (second use plays LCOD80) */
#define L13_PASSAGE_USED      0x61C   /* 1 = the hero already went through the OB41 passage (second try plays LCOD79) */
#define L13_PASSAGE_FLAG      0xB7C   /* set to 1 when the passage cutscene starts (never read in this level) */
#define L13_BUNKER_SEEN 0xC20   /* 1 = hero saw the bunker door, INIT turns it into 2 = told (same slot as L12_BUNKER_SEEN; read by L49) */
#define L13_HIT_COUNT         0xC8C   /* counts damaging strikes at the trap (0..2, capped; each hit costs the hero HP) */
#define L13_PUZZLE_SOLVED     0xC90   /* 0 = lever puzzle unsolved, 1 = solved (set after the anim slot 2 sequence); changes the event 0 sound/dialogue */
#define L13_REPLY_STAGE       0xD54   /* answer sequence at zone 5 after solving: 0 = none, 1 = first answer (event 0 plays L13D07/D08 and the SC13/SC14 cinematic), 2 = later answer (L13D09/D10) */
#define L13_ENDING_OVERRIDE   0xE8C   /* game-wide; 1 = skip the hero-death ending in the tick and re-run INIT (set to 1 in L31 near the end) */
#define L13_ALT_MUSIC        0xEFC   /* 1 = intro dialogue (L13D01..) already played (same slot as L12_ALT_MUSIC) */

#endif /* DKE_LIEU13_INT_H */
