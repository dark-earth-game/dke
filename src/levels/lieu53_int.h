#ifndef DKE_LIEU53_INT_H
#define DKE_LIEU53_INT_H

/*
 * lieu53_int.h - shared internals of the LIEU53.DLL port, split between
 * lieu53.c (entry, INIT, tick, events, helpers) and lieu53_trig.c (the zone
 * trigger handlers).
 */

#include "lieu_level.h"

/* ST() reads g_lvl_S, the DLL's dword_1001990C. */

/* Resets the DLL .data globals (the DLL is loaded afresh for every room). */
void l53_reset_dll_globals(void);

/* Level 53 (room 13) script variables */
#define L53_PANEL_ACTION       0x064  /* last console action: 1 = touched the live panel (shock, event 1 plays anim 1), 2 = switched the lights on (event 1 plays anims 2, 1 and sets L53_POWER_ON) */
#define L53_LIGHTS_ANIM_ON     0x068  /* tick: anim 2 (green lights) started once L53_POWER_ON == 1 and camera locked on 3; reset when the camera leaves */
#define L53_SWITCH_ON          0x06C  /* zone-13 switch toggle: 0 = next use says L13D03/D04, 1 = next use says L13D05/D06 (flips each time) */
#define L53_IDLE_ANIM_ON       0x070  /* tick: anim 5 started once while the camera is unlocked (0), reset when it locks */
#define L53_DEBRIS_SEARCHED    0x5D4  /* zone 5 debris: 0 = untouched, 1 = OB72 and OB23 taken (afterwards "Nothing") */
#define L53_OB41_TAKEN         0x61C  /* console cabinet item OB41: 0 = still there, 1 = taken (afterwards LCOD79 "There's nothing here") */
#define L53_OB41_QUEST_STAGE    0xB7C  /* OB41 quest stage, shared with level 5 (Bogdaran carries OB_OBJ_BEFORE41; 2/4 -> 10 when looted): 1 = hero took OB41 here */
#define L53_SHOCK_COUNT        0xC8C  /* electric shocks taken from the panel in special mode: 0..2 (each costs 30 HP while HP > 35, then L13D15 "it hurts too much") */
#define L53_POWER_ON           0xC90  /* bunker console power: 0 = off (event 0 plays LCOD39/40), 1 = lights on after the second console action */
#define L53_CONSOLE_STAGE      0xC94  /* console zone 6 inspection: 0 = fresh, 1 = lights seen (L13D17, event 0 plays the moving mechanism and cinematic), 2 = dead machine (L13D09/D10) */
#define L53_BUNKER_SEEN        0xEFC  /* first entry (trigger 41): 0 = L13D01/D02 "a bunker from Before" not yet said, 1 = said; same slot sets the music in LIEU12 */

/* Trigger dispatch (op 3): run trigger handler `idx` (lieu53_trig.c). */
void l53_trigger(i32 idx);

/* Trigger handler the core also calls directly (defined in lieu53_trig.c):
 *   l53_trig00 = trigger 0, sub_100065C0 (thunk sub_1000106E; INIT default) */
void l53_trig00(void);

#endif /* DKE_LIEU53_INT_H */
