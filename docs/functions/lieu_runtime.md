# LIEU Runtime Emulation — Design Plan

Draft: 2026-07. Prototype spec for M6 pass 3 (gameplay-time LIEU opcodes).

## Problem

LIEU DLL has 7 opcodes. Only opcode 1 (INIT) is state-independent —
already extracted statically via `tools/src/lieu_extract.py`.

Opcodes 0 (per-frame tick), 3 (event dispatch), 4-6 (subroutine calls)
are **state-conditional**: branch on engine callback return values and
on state buffer contents. Static extraction captures one path only.

Need runtime execution of these opcodes with real engine state.

## Options

### Option R1 — Embedded unicorn (recommended prototype)

Embed a lightweight unicorn instance inside dke. Load LIEU DLL into
mapped memory. Bridge 40 engine callbacks to native C primitives via
UC_HOOK_CODE trampolines. Invoke opcodes 0/3-6 on demand.

**Pros:**
- Bit-exact — same behaviour as original x86 game.
- One implementation covers all 55 DLLs.
- Weeks to ship, not months.
- Unicorn cross-platform: Linux/macOS/Windows/*BSD × x86/ARM64/RISC-V hosts.

**Cons:**
- Runtime dependency on unicorn (LGPL). Adds ~2 MB to binary.
- x86 emulation overhead per callback (~10-100 µs). For ~40 calls/frame ×
  60 fps = ~24k calls/s → 240 ms/s = 24 % overhead. May need caching.
- Debugging: hybrid stack (native C ↔ x86 emulated). Trace tooling
  required for issue triage.

### Option R2 — Symbolic decision-tree extraction

Use angr or similar to symbolically execute each opcode entry point.
Explore all reachable branches. Serialise as decision tree
(`opcode → tree of {state_query_id, if_true_actions, if_false_actions}`).
Native engine walks tree with real state queries.

**Pros:**
- Zero runtime x86 dependency. Pure C in shipping build.
- Traces deterministic and testable per level.
- Enables level editor / modding without emulator dep.

**Cons:**
- Setup: ~4-6 weeks (symbolic engine plumbing, output format, replay VM).
- Path explosion: unbounded loops in tick handlers may need cutoffs.
- Data size: decision trees per opcode per level → estimated 5-20 MB total.
- Correctness harder to verify — decision tree may miss corner-case paths.

### Option R3 — Full hand-port (rejected — see earlier session)

Per-DLL manual C rewrite. Cost: 6-12 months FTE. Skip.

## Recommendation: R1 (embedded unicorn) with R2 as future upgrade

R1 ships fastest with bit-exact behaviour. R2 becomes viable once R1
proves the callback bridge is correct — then per-level trace dumps from
R1 become oracle for validating R2's decision trees.

Ship as **optional feature flag** at build time:

```
make dke                    # default: R1 embedded unicorn
make dke LIEU_MODE=static    # extract-only (works today, gameplay-limited)
make dke LIEU_MODE=tree      # R2 (future)
```

## Architecture (R1)

```
┌──────────────────────────────────────────┐
│           dke game engine (C)             │
│                                            │
│    ┌───────────────────────────────────┐  │
│    │        scene_load_level(id)         │  │
│    └────────────┬──────────────────────┘  │
│                 │                          │
│                 ▼                          │
│    ┌───────────────────────────────────┐  │
│    │       lieu.c (existing)            │  │
│    │       - static init: log stubs     │  │
│    │       - static init: real effects  │  │
│    └────────────┬──────────────────────┘  │
│                 │                          │
│                 ▼                          │
│    ┌───────────────────────────────────┐  │
│    │      lieu_vm.c (new)              │  │
│    │      - unicorn init/shutdown       │  │
│    │      - DLL mapper (one per level)  │  │
│    │      - callback bridge (40 slots)  │  │
│    │      - opcode invoker              │  │
│    └────────────┬──────────────────────┘  │
│                 │                          │
│                 ▼                          │
│    ┌───────────────────────────────────┐  │
│    │      unicorn engine (LGPL)         │  │
│    │      x86-32 interpreter            │  │
│    └───────────────────────────────────┘  │
│                                            │
│    ┌───────────────────────────────────┐  │
│    │  Engine primitive stubs (in main   │  │
│    │  engine code — same as static      │  │
│    │  callback wiring in pass 2)        │  │
│    │  - object spawn, kill, query       │  │
│    │  - script trigger enqueue          │  │
│    │  - audio play, movie play          │  │
│    │  - dialogue open, choice, state    │  │
│    └───────────────────────────────────┘  │
└──────────────────────────────────────────┘
```

## Interface (proposed)

### `src/lieu_vm.h`

```c
#ifndef DKE_LIEU_VM_H
#define DKE_LIEU_VM_H

#include "libsys/common.h"

/* Opaque handle for a loaded LIEU DLL instance. */
typedef struct lieu_vm_s lieu_vm_t;

/* Callback signature: engine primitive called from emulated LIEU code.
 * All 40 slots share this signature; specific args vary per slot.
 *   ctx  = user data (engine state pointer)
 *   slot = 0x00..0x9C (4-byte aligned) — matches static extraction
 *   ecx, edx = register args
 *   stack = pointer to first stack arg (may be NULL if no stack args)
 *   n_stack = count of valid stack args (0..8)
 * Return value assigned to eax before resuming emulation.
 */
typedef i32 (*lieu_cb_fn)(void *ctx, i32 slot, i32 ecx, i32 edx,
                          const i32 *stack, i32 n_stack);

/* Initialise VM subsystem. Called once at engine boot. */
i32 lieu_vm_init(void);

/* Shut down VM subsystem. Called at engine exit. */
void lieu_vm_shutdown(void);

/* Load LIEU%02d.DLL for `level_id` into a fresh VM instance.
 * Returns handle or NULL on error.
 * The VM ingests callbacks and runs INIT (opcode 1) synchronously
 * before returning. */
lieu_vm_t *lieu_vm_load(i32 level_id, lieu_cb_fn cb, void *cb_ctx);

/* Release VM instance and unmap the DLL. */
void lieu_vm_free(lieu_vm_t *vm);

/* Invoke an opcode. Args semantics documented per opcode:
 *   0 (tick)   : no args, called every frame
 *   3 (event)  : arg0 = event id, arg4 = optional param
 *   4          : arg0 = int arg
 *   5          : arg0, arg4 = 2 int args
 *   6          : arg0 = int arg
 * Return value = LIEU's return (typically 0 or a pointer). */
i32 lieu_vm_invoke(lieu_vm_t *vm, i32 opcode, i32 arg0, i32 arg4);

#endif
```

### Callback bridge

Each of the 40 engine primitives lives in the main engine already
(sub_44453E → NamedObject queue, etc.). Runtime cb_fn dispatches:

```c
static i32 dke_lieu_dispatch(void *ctx, i32 slot, i32 ecx, i32 edx,
                              const i32 *stack, i32 n_stack)
{
    switch (slot) {
        case 0x00: return engine_named_object((const c8 *)ecx);
        case 0x04: return engine_named_object_flag((const c8 *)ecx, edx);
        case 0x08: return engine_named_object_two_ints(ecx, edx);
        case 0x0C: return engine_cinematique((const c8 *)ecx);
        case 0x10: return engine_kill_perso(ecx);
        case 0x14: return engine_named_object_str_pair(ecx, edx,
                       stack[0], stack[1], stack[2],
                       (const c8 *)stack[3]);
        case 0x18: return engine_named_object_str((const c8 *)ecx, edx,
                       stack[0], stack[1], stack[2], stack[3]);
        /* ...12 more CB_XX slots that we still need to identify... */
        default:
            print("lieu_vm: unhandled slot 0x%02x ecx=0x%x edx=0x%x\n",
                  slot, ecx, edx);
            return 0;
    }
}
```

## Memory layout (per VM instance)

Same layout as `lieu_extract.py` uses today:

```
0x10000000  DLL image (1 MB)          — mem_map RWX, PE sections
0x20000000  callback table (4 KB)     — 40 × 4-byte ptrs
0x21000000  trampolines (4 KB)        — 40 × 16 bytes, each = single INT3
0x30000000  state buffer (4 KB)       — engine ↔ LIEU communication area
0x40000000  stack (1 MB)              — grows down from 0x40100000
0x50000000  sentinel (4 KB)           — return-address halt page
```

Between opcode invocations, state buffer + DLL data persist. Stack rewinds.
DLL globals (`0x1002c*` region) persist since they live in DLL image.

## Callback stack-pop table (retn N)

Already computed for extractor. Reuse `CB_POPS` (see
`tools/src/lieu_extract.py:CB_POPS`). Move to shared C table:
`src/lieu_vm_pops.h` generated from Python or hand-baked.

## Milestones

### M6.3a — Bootstrap (est: 3-5 days)

- Add unicorn to build system (Makefile + brew/apt install docs).
- `src/lieu_vm.[ch]` with `lieu_vm_init/shutdown/load/free/invoke`.
- Port Python DLL mapper + trampoline setup to C.
- INIT smoke test: load LIEU01.DLL, invoke opcode 1, verify 57 callbacks
  fire in same order as Python extractor (byte-exact trace).
- No engine effects yet — cb_fn logs only.

### M6.3b — Static effect wiring (est: 5-10 days)

- Port sub_44453E ... sub_446E19 primitives (40 total) to native C in
  `src/engine_prims.[ch]` (or fold into `lieu_vm.c`). Reuse action queue
  (dword_4F4CC0 pattern).
- Wire cb_fn to real primitives. Static-init callbacks now have real
  effects (object spawn, audio queue, movie play).
- Verify LIEU01: New Game click → level 1 shows expected objects, plays
  L01AMB1, spawns character.

### M6.3c — Per-frame tick (est: 3-5 days)

- Wire `lieu_vm_invoke(vm, 0, 0, 0)` into game loop after `scene_render`.
- Handle callback returns that steer LIEU branches: engine primitives
  return real state (object count, player position, etc.).
- Regression check: no crashes across all 55 levels through boot + 10s
  of idle gameplay.

### M6.3d — Event dispatch (est: 5-10 days)

- Wire opcode 3 into event points: player enters zone, picks up item,
  dialogue choice. Engine emits `lieu_vm_invoke(vm, 3, event_id, param)`.
- Trigger table (populated during INIT via slot 0x08 NamedObjectTwoInts)
  drives which handler runs.
- Verify: LIEU01 zone triggers fire NPC AI, dialogue, cutscenes.

### M6.3e — Subroutine calls (est: 2-3 days)

- Opcodes 4, 5, 6: game asks LIEU to compute or provide data.
- Return values propagate to engine.
- Regression on all 55 levels.

**Total estimate: 3-5 weeks FTE.**

## Cross-platform status

| Host              | Unicorn | dke build       | Notes                          |
|-------------------|---------|-----------------|--------------------------------|
| macOS ARM64       | ✓       | already builds  | via `brew install unicorn`      |
| macOS x86_64      | ✓       | already builds  | brew                            |
| Linux x86_64      | ✓       | needs test      | apt / manual                    |
| Linux ARM64       | ✓       | needs test      | apt                             |
| Windows           | ✓       | needs mingw     | pacman / manual                 |
| iOS               | ✗       | not planned     | LGPL restriction + JIT ban      |
| Web (Emscripten)  | ✓       | not planned     | via wasm — future R2 target     |

Ship builds link libunicorn dynamically. Static link possible for
distributions that require it (verify LGPL §4 compliance — provide
object files for relinking).

## Risks

### R1 — Callback bridge miss

**Risk:** Native primitive semantics diverge from original. LIEU code
receives wrong data → wrong branch → crash or logic error.  
**Mitigation:** Golden trace tests. Run same level in static-extract
mode vs runtime mode, diff callback traces. Discrepancies isolated per
callback. High-coverage smoke test: LIEU01-LIEU10 full boot + 10 s idle.

### R2 — Unicorn thread safety

**Risk:** Unicorn instance not thread-safe. Engine may call from render
thread + audio thread + main tick thread.  
**Mitigation:** Single-thread all lieu_vm_invoke calls via engine's
main loop. Enforce via `assert(pthread_self() == g_main_thread)` in
lieu_vm.c.

### R3 — State buffer race

**Risk:** LIEU reads state[0x134] while engine writes it in parallel.  
**Mitigation:** State buffer only touched between invoke calls, never
during. Engine primitives run synchronously inside cb_fn.

### R4 — Callback re-entry

**Risk:** LIEU calls engine primitive → primitive calls back into LIEU
(e.g. triggers event opcode).  
**Mitigation:** Detect recursion in `lieu_vm_invoke`. Queue re-entrant
calls for next tick. If unavoidable, save/restore unicorn context (adds
~50 µs per re-entry).

### R5 — DLL bugs

**Risk:** Original LIEU DLL has bugs that engine worked around.  
**Mitigation:** Compare behaviour against Wine + original dkev.exe for
disputed cases. Document per-level workarounds in
`docs/functions/lieu_dll_quirks.md`.

### R6 — LGPL compliance

**Risk:** Unicorn = LGPL. Static linking triggers relink requirement.  
**Mitigation:** Dynamic link by default. Document LGPL obligations in
`README.md`. Provide object files on request for static builds.

## Testing strategy

### Golden trace tests

For each of 55 LIEUxx.DLL:

1. Run `tools/src/lieu_extract.py` → `LIEUxx.script` (static, eax=0).
2. Run new runtime VM with same setup (cb_fn logs, no side effects) →
   `LIEUxx.runtime_script`.
3. `diff LIEUxx.script LIEUxx.runtime_script` — must be identical.

Any diff = trampoline / stdcall pop / state seed bug in VM.

### Smoke tests

Per level (LIEU01, LIEU10, LIEU30, LIEU50):

1. Boot → New Game → target level.
2. 10 s idle observation.
3. WASD walk, one zone crossing.
4. Assert no crashes, no infinite loops, expected NPC visible.

### Regression harness

Automated CI:

```
make dke && bin/game/dke --autoplay=LIEU01 --duration=60s --headless
```

Logs replayed against `bin/game/lieu/regression/LIEU01.golden.log`.

## Open questions

- Q: Does engine ever load 2+ LIEU DLLs simultaneously (level transition)?  
  **A: Investigate.** If yes, need multiple `lieu_vm_t` instances.
- Q: Does opcode-0 tick expect precise timing (real-time coupled)?  
  **A: Investigate.** Per-frame invocation cadence may matter.
- Q: What is state buffer layout (dword_505C40, 0xFA0 bytes)?  
  **A: Reverse engineer.** Currently opaque. Documented offsets known:
  `[+0x134]` used in opcode-0 branches. More offsets emerge from LIEU
  disassembly.
- Q: Save/load — LIEU state persists across saves?  
  **A: Yes.** State buffer probably serialised. Confirm from
  `save_game` code path in dkev.asm.

## Non-goals for M6

- Deterministic replay of original game recordings.
- Multi-DLL cross-references (levels calling into each other's LIEU).
- Modding API. Future work once native code paths stabilise.

## Files affected

New:
- `src/lieu_vm.[ch]` — VM implementation.
- `src/engine_prims.[ch]` (or expand `src/lieu.c`) — 40 callback stubs.
- `docs/functions/lieu_dll_quirks.md` — per-level workaround notes.

Modified:
- `Makefile` — link libunicorn.
- `README.md` — LGPL notice + install instructions.
- `src/lieu.c` — replace static replay with runtime VM invocation.
- `src/scene.c` — call `lieu_vm_invoke(vm, 0, ...)` per frame.
- `src/main.c` — `lieu_vm_init/shutdown` bracketing game loop.

Regeneratable:
- `bin/game/lieu/LIEUxx.golden.log` — regression harness output.

## Decision point

Before starting M6.3a implementation:

1. Confirm unicorn is acceptable dependency (LGPL + ~2 MB binary size).
2. Confirm runtime overhead budget (~24 % worst case, likely much less).
3. Decide static vs dynamic linking default.

Once green-lit: begin M6.3a (bootstrap) as one Sonnet delegation with
this doc as brief.
