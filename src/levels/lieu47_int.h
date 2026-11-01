#ifndef DKE_LIEU47_INT_H
#define DKE_LIEU47_INT_H

/*
 * lieu47_int.h - shared internals of the LIEU47.DLL port, split between
 * lieu47.c (entry, INIT, tick, events, helpers) and lieu47_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_10019B0C. */

/* Level 47 (room 11) script variables */
#define L47_UNUSED_64          0x064  /* reset to 0 in trigger 0, never read */
#define L47_ARKHAN3_SPOTTED    0x068  /* Arkhan3 within 500 of the guard: L11D01 + SP32GAR05 played; 0 = not yet, 1 = done */
#define L47_ARKHAN3_CHALLENGED 0x06C  /* Arkhan3 within 200: L11D02 said, guard fight starts; 0 = not yet, 1 = done */
#define L47_ARKHAN2_SPOTTED    0x070  /* Arkhan2 within 400 of the guard: L11D03 + SP32GAR05; 0 = not yet, 1 = done */
#define L47_ARKHAN2_WARNED     0x074  /* Arkhan2 within 250: L11D04 + SP32ARR04; 0 = not yet, 1 = done */
#define L47_TALK_BUSY          0x078  /* 1 while the L11D06/L11D07 refusal line plays (Arkhan2/3 acts on the live guard), then back to 0 */
#define L47_END_EVENT_SENT     0x07C  /* hero dead: EV_END_GAME queued once; 0 = not yet, 1 = sent */
#define L47_SHOUT_STATE        0x080  /* shout (cri) at the guard: 32 = pending (event 0 fires 1 s later), 99 = already handled */
#define L47_LEVER_ANIM_PAUSED  0x084  /* LEVIER anim paused while the camera is locked on 3 after the guard fight (V_GUARD_FIGHT_DONE == 2); event5 OB_PARCHMENT sets 1 in level 28 */
#define L47_GUARD_HOSTILE      0x4E4  /* 1 = the guard has been provoked and fights (hit, shout, bad gift, attack); room-10 view then reposts him; same slot in LIEU11 */
#define L47_GUARD_TALK_STAGE   0xC9C  /* Arkhan2 talk chain with the guard (action key in special mode): 0..4 = L11D08..L11D16 in order, 5 = chain done, food trading follows */
#define L47_GUARD_FOOD_STAGE   0xEA4  /* debug "F37": 0 = not asked, 1 = guard hungry (L11D17..19), 2 = fed fish/meat (wants a drink), 3 = drunk (gourd given, guard asleep) */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu47_trig.c). */
void l47_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu47_trig.c):
 *   l47_trig00 = trigger 0, sub_10007183 (thunk sub_10001073; INIT default) */
void l47_trig00(void);

#endif /* DKE_LIEU47_INT_H */
