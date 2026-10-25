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
#include "levels.h"

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
