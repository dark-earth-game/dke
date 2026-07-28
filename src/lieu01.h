#ifndef DKE_LIEU01_H
#define DKE_LIEU01_H

/*
 * lieu01.h — Native hand-port of LIEU01.DLL (level-1 scripting).
 *
 * Reimplements @FonctionEditor@16 and all its runtime handlers directly
 * from the disassembly (NOT via the .script replay path). One entry
 * point mirrors the original opcode dispatch; the engine drives it each
 * frame exactly as the original DLL was driven.
 *
 * Requires g_lieu_cb (see lieu_cb.h) to be populated by the engine
 * bridge before the first call.
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
 * NB: there is no runtime op-0 call in the engine.
 */
typedef enum {
    LIEU_OP_TICK   = -1,  /* per-frame: drain pending sound + tick body */
    LIEU_OP_INIT   = 1,
    LIEU_OP_TEARDOWN = 2,
    LIEU_OP_EVENT  = 3,   /* dispatch trigger[arg] */
    LIEU_OP_CALL4  = 4,
    LIEU_OP_CALL5  = 5,
    LIEU_OP_CALL6  = 6,
} lieu_opcode_e;

/* Runtime dispatch trace: when non-zero, each lieu01_fonction_editor call
 * logs "lieu01: op=N arg=M". Toggle from the bridge / a debug keybind. */
extern i32 g_lieu_trace;

/*
 * lieu01_fonction_editor — the exported entry, hand-ported.
 *   op    : opcode (see above)
 *   state : per-level state buffer (0xFA0 bytes)
 *   arg   : opcode-specific (event index for op 3; sub arg for 4/5/6;
 *           reload flag for op 1)
 *   arg2  : secondary parameter (used by op 5 = event5 phase counter;
 *           0 for all other opcodes)
 * Returns state[0] (result code), matching the original.
 */
i32 lieu01_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2);

#endif /* DKE_LIEU01_H */
