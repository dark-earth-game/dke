#ifndef DKE_LIEU39_INT_H
#define DKE_LIEU39_INT_H

/*
 * lieu39_int.h - shared internals of the LIEU39.DLL port, split between
 * lieu39.c (entry, INIT, tick, helpers) and lieu39_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001E7AC. */

/* Level-local script variables (scratch slots reused per level; 0x32C..0xD94 are shared with other levels) */
#define L39_LEONA_GUARD_UP           0x64   /* Leona reacted to the hero's combat stance: 0 = not yet, 1 = she took guard (reset when the hero lowers it) */
#define L39_DIALOGUE_PICK            0x68   /* random(3) pick of Leona's idle reply line (L39D88-90 / L39D91..) */
#define L39_ARKHAN3_GESTURE          0x6C   /* Arkhan3 toggle for the alternating Leona gesture/reply: 0 = GAR26 stance next, 1 = GAR27 next */
#define L39_LEONA_CRY_STAGE          0x70   /* Leona reply to the shout (cri) event: 0 = first time (talks), 1 = repeat (fights) */
#define L39_FIGHT_EVENT_PENDING      0x88   /* 1 = fight-start event 1 queued (trigger 41 timer), blocks the tick fight restart; cleared when the event fires */
#define L39_FATHER_INFECTED          0x32C   /* shared story flag: non-zero adds Arkhan2 line L39D24 'Thanandar infected my father' */
#define L39_ZED_INFECTED             0x330   /* shared story flag: non-zero adds Arkhan2 line L39D25 'Thanandar infected Zed' */
#define L39_NOTES_QUEST              0xB10   /* shared story stage: 1 = father's key/notes sought (set by level 15 fight outcomes), 2 = found; != 2 adds the 'find his notes' lines */
#define L39_ENTRY_SIDE               0xD5C   /* which room-38 exit the hero came through: 1 = near (1196,827), 2 = far (395,1585); shared with LIEU38 */
#define L39_EXIT_SIDE                0xD84   /* which entrance of room 38 the hero re-enters: 1 = trigger 4 exit, 2 = trigger 3 exit; shared with LIEU38 */
#define L39_LEONA_STATE              0xD88   /* Leona relation: 0 = unmet, 1 = met (hostile/greeted), 2 = talked, 10 = fighting; +200 = same state deferred until the hero leaves camera 2 (201, 202, 210) */
#define L39_AMBIENCE_CAM2            0xD8C   /* 1 = L38AMB1 ambience playing (camera 2), 0 = L39AMB1 */
#define L39_ARKHAN2_TALK             0xD90   /* Arkhan2 conversation stage with Leona, 0..9 (9 = random repeat lines) */
#define L39_ARKHAN3_TALK             0xD94   /* Arkhan3 conversation stage with Leona, 0..9 (9 = random repeat lines) */
#define L39_DEATH_ENDING_SKIP        0xE8C   /* engine flag: 1 = INIT skips the hero-dead ending even if ST_HERO == 4 */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu39_trig.c). */
void l39_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu39_trig.c):
 *   l39_trig00 = trigger 0, trig00_10001200 (thunk 0x10001073; INIT default) */
void l39_trig00(void);

#endif /* DKE_LIEU39_INT_H */
