#pragma once

/*
 * lieu_level.h — what every level script port includes: the state buffer
 * of the call in flight, the ST() / CB accessors, the shared names and the
 * code all level DLLs carry unchanged (lieu_common.c).
 */

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels.h"
#include "lieu_names.h"

/* The state buffer of the call in flight (each DLL's state pointer global;
 * the level entry sets it on every call). */
extern lieu_state_t *g_lvl_S;

#define ST(off)   LIEU_ST(g_lvl_S, (off))
#define CB        (g_lieu_cb)

/* lieu_common.c */
/* action_register (LIEU01 sub_1001932C): animation(arg0), dialogue(name,
 * speaker), then the closing animation arg4 / "Sar_20" / "SPnnarr01_10". */
void lieu_action_register(const c8 *name, i32 speaker,
                          const c8 *arg0, const c8 *arg4);
/* LIEU02 sub_1001A35C: hero greets persona n; a2/a3 are zones for Distance. */
void lieu_pre_scene(i32 hero, i32 n, i32 a1, i32 a2, i32 a3);
/* Divers(DV_ANIM_NAME, persona, state): "s" + the persona's clip for that
 * state (the original returns a pointer to it); NULL when not loaded. */
const c8 *lieu_anim_name(i32 persona, i32 state);
void lieu_event4(i32 arg);              /* sub_1000123A, op 4 */
void lieu_event5(i32 arg, i32 phase);   /* sub_100022B4, op 5 */
void lieu_event6(i32 arg);              /* sub_10005BEC, op 6 */
