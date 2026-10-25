#ifndef DKE_LEVELS_H
#define DKE_LEVELS_H

/*
 * levels.h — the native ports of the LIEUnn.DLL level scripts. Each level
 * provides its @FonctionEditor@16 entry; the engine looks it up by level
 * number (the original LoadLibrary'd "LIEU%02d.DLL", sub_448F4D).
 */

#include "libsys/common.h"
#include "lieu_cb.h"

/*
 * Opcodes, matching the original jump table (index = opcode+1), verified
 * against dkev.exe drivers sub_44968B (per-frame) / sub_449BAD (teardown):
 *   op -1 = per-frame tick (drain pending sound + tick_body) — EVERY frame
 *   op  1 = INIT (arm; edx = state buffer dword_505C40; flag = reload)
 *   op  2 = teardown (DLL-side no-op; engine frees sounds + unloads)
 *   op  3 = dispatch trigger[arg], arg = zone id from sub_449F9F
 *   op  4 = event4(arg), arg = expired object-timer id
 *   op  5 = event5(arg), arg = pending event (dword_507258)
 *   op  6 = event6(arg) (LCOD zone sub-triggers)
 */
typedef enum {
    LIEU_OP_TICK     = -1,
    LIEU_OP_INIT     = 1,
    LIEU_OP_TEARDOWN = 2,
    LIEU_OP_EVENT    = 3,
    LIEU_OP_CALL4    = 4,
    LIEU_OP_CALL5    = 5,
    LIEU_OP_CALL6    = 6,
} lieu_opcode_e;

/* @FonctionEditor@16(op, state, arg, arg2); returns state[0]. */
typedef i32 (*lieu_entry_t)(i32 op, lieu_state_t *state, i32 arg, i32 arg2);

/* The level's entry, or NULL when the level has no native port yet. */
lieu_entry_t lieu_level_entry(i32 level);

/* Runtime dispatch trace (LIEU_TRACE=1). */
extern i32 g_lieu_trace;

i32 lieu01_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2);
i32 lieu02_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2);

#endif /* DKE_LEVELS_H */
