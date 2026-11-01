#ifndef DKE_LIEU01_INT_H
#define DKE_LIEU01_INT_H

/*
 * lieu01_int.h — shared internals of the LIEU01.DLL port, split between
 * lieu01.c (entry, INIT, tick, events, helpers) and lieu01_trig.c (the
 * zone trigger handlers).
 */

#include "lieu_level.h"
#include "lieu01.h"

/* ST() reads g_lvl_S, the DLL's dword_1002C96C. */

/* Script variables of level 1: state block slots this level reuses as its own scratch (not game-wide). */
#define L01_UNUSED_64           0x064   /* never read, only cleared at INIT */
#define L01_KALHI12_ALERT       0x068   /* Kalhi12 idle state: 1 = attentive (looking at the hero), 0 = relaxed after the random SP12LEV03 anim */
#define L01_D21_SAID_COUNT      0x06C   /* times the L01D21A..D hero chatter ran; the lines play while <= 2 */
#define L01_UNUSED_70           0x070   /* never read, only cleared at INIT */
#define L01_KALHI12_RESUME      0x074   /* 1 = INIT re-entered with Arkhan0 mid Kalhi12 scene; trigger 41 plays L01D17A/B then clears */
#define L01_PHEDORIA_STEP       0x078   /* Kalhi12 fight lead-in: 1 = Phedoria enters (L01D45), 2 = hero knocked down (L01D44), -1 = idle/done */
#define L01_KABAK_NEAR_STATE    0x07C   /* Kabak proximity state: 0 idle, 1 hero near (<= 250), 2 engaged/dialogue running, 0x0A fight started */
#define L01_KABAK_GUARD         0x080   /* 1 = Kabak is in guard stance (P87GAR26 pose), 0 = relaxed */
#define L01_KABAK_MET           0x084   /* Kabak first-encounter dialogue already played (1) */
#define L01_KALHI12_ANIM_STATE  0x088   /* Kalhi12 idle-anim driver: 1 -> 2 first anim step, 0x29A idle ready for random anim, 0x3E8 hostile, -1 reset */
#define L01_UNUSED_90           0x090   /* never read, only cleared at INIT */
#define L01_G23_UP              0x094   /* 1 = Guardian23 has risen (SP88LEV01 played) and is on guard, 0 = still lying down */
#define L01_SCENE_CAMERA        0x098   /* 1 = camera 6 locked on a scene with Kalhi12/Kabak/guard; tick releases it when the hero walks away */
#define L01_SAVED_CAMERA        0x0B8   /* locked camera at the moment of the Kalhi12 intro (restored afterwards) */
#define L01_CAMERA_RESTORE      0x0BC   /* camera 6 restore step: 1 = locked for the intro, 2 = restored */
#define L01_G23_D167_DONE       0x0D4   /* 0 = Kabak reaction L01D167 not yet played in the Guardian23 L01D166 talk, 1 = played */
#define L01_KABAK_DOOR_DONE     0x0D8   /* 1 = door zone 16 spawn after Kabak's death handled */
#define L01_G23_FOLLOW_STEP     0x0DC   /* Guardian23 follow driver: 0x0A = set after repositioning, 1 -> 2 joindre_perm on the hero */
#define L01_GUARD_HIT           0x0E0   /* 1 = a guard (23/24) was hit; blocks the OB94 relink at the Guardian23 ambush */
#define L01_OB94_LINKED         0x0F0   /* 1 = OB94 (Kabak's weapon object) linked to Kabak, 0 = unlinked */
#define L01_KALHI12_WARNED      0x714   /* 1 = Kalhi12 already warned the hero (L01D41) */
#define L01_SECOND_ROUND        0xB88   /* 0 = first Kabak round, 1 = second round after the guard fight (start points 5/6) */
#define L01_GUARD_WAVE2_DONE    0xBE4   /* 1 = second guard wave finished (start point 6), 0 otherwise */
#define L01_G23_AMBUSH          0xCCC   /* Guardian23 ambush intro: 0 = not played, 1 = played (second part waits for the script to be free) */

/* Trigger dispatch (op 3) and trigger 0 (INIT), lieu01_trig.c. */
void l01_trigger(i32 idx);
void l01_trig00(void);

/* DLL helpers shared by both files (lieu01.c). */
void l01_spawn_ar(void);                                                               /* "ar" at the hero's spot */

#endif /* DKE_LIEU01_INT_H */
