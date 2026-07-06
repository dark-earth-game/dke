# LIEU DLL — Level Scripting

Compiled MSVC x86 DLL per level (`LIEU%02d.DLL`, 55 total).
Loaded on level enter, invoked with action opcode.

## Loading (engine side: sub_448F4D @ 0x448F4D)

```
sprintf(name, "LIEU%02d", world_state)
hModule = LoadLibraryA(name)                  // sub_435560
event_callback = GetProcAddress(hModule, "@FonctionEditor@16")   // sub_4355AB
result = event_callback(1, &state_buf, &callback_table, reload_flag)
    // ecx = opcode (1 = init)
    // edx = state buffer ptr (dword_505C40, 0xFA0 = 4000 bytes)
    // arg0 = engine callback table ptr (dword_4D6198, 40 fn ptrs)
    // arg4 = dword_4B476C (reload flag)
dword_4B53FC = result
```

Return value stored globally. Engine then runs its own frame loop
(sub_44A263 or render_sprite_time_based_effect + sub_44CDB8 loop).

## DLL entry (LIEU01 example)

`FonctionEditor@16` at RVA 0x1046 → JMP thunk → real code at 0x18C60.

```
FonctionEditor(ecx=opcode, edx=state, arg0=cb_table, arg4=flag):
    var_c = opcode
    // On first call: ingest all callback pointers from cb_table
    //   into DLL globals (0x1002c*)
    dispatch: table[opcode+1] via jump table at 0x100192F9
```

Dispatch table (8 entries, indexed by opcode+1):
- op+1=1 → 0x1927C  (opcode 0)
- op+1=2 → 0x18C7A  (opcode 1: callback ingestion — first call)
- op+1=3 → 0x19233
- op+1=4 → 0x19238
- op+1=5 → 0x19252
- op+1=6 → 0x1925F
- op+1=7 → 0x1926F
- default → 0x19319 (return state[0])

Engine calls once with opcode=1 to initialise; other opcodes drive
per-frame updates (poll object states, tick timers, etc.).

## Engine callback table (dword_4D6198 → 4D6234, 40 slots)

Populated in sub_448F4D @ 0x449259..0x4493E3. Each 4-byte slot =
pointer to engine scripting primitive. LIEU DLL copies these into
its own globals at 0x1002c* and calls them by name.

Table layout (offset → engine function):

| Off  | Engine fn   | Type / purpose                                    |
|------|-------------|---------------------------------------------------|
| 0x00 | sub_44453E  | NamedObject action, 1 str arg (action_id=0)       |
| 0x04 | sub_4449A5  | NamedObject + int flag (action_id=1)              |
| 0x08 | sub_444A3E  | NamedObject + 2 ints (action_id=2)                |
| 0x0C | sub_444AC0  | Cinematique (movie name, action_id=3)             |
| 0x10 | sub_444FF5  | KillPerso — sprintf("KillPerso %ld") + sub_444170 |
| 0x14 | sub_44502D  | 2-str action (action_id=5)                        |
| 0x18 | sub_445189  | 1-str action                                       |
| 0x1C | sub_4452CD  | ?                                                  |
| 0x20 | sub_4453E1  | ?                                                  |
| 0x24 | sub_4454F2  | ?                                                  |
| 0x28 | sub_4455EE  | ?                                                  |
| 0x2C | sub_44569A  | ?                                                  |
| 0x30 | sub_445765  | ?                                                  |
| 0x34 | sub_4458AD  | ?                                                  |
| 0x38 | sub_445919  | ?                                                  |
| 0x3C | sub_44599B  | ?                                                  |
| 0x40 | sub_445C82  | ?                                                  |
| 0x44 | sub_445E99  | ?                                                  |
| 0x48 | sub_445FD5  | ?                                                  |
| 0x4C | sub_446159  | ?                                                  |
| 0x50 | sub_446227  | ?                                                  |
| 0x54 | sub_44637B  | ?                                                  |
| 0x58 | sub_44648B  | ?                                                  |
| 0x5C | sub_44660D  | ?                                                  |
| 0x60 | sub_446685  | ?                                                  |
| 0x64 | sub_4466E9  | ?                                                  |
| 0x68 | sub_446763  | ?                                                  |
| 0x6C | sub_446808  | ?                                                  |
| 0x70 | sub_4468E1  | ?                                                  |
| 0x74 | sub_4469BA  | ?                                                  |
| 0x78 | sub_446A05  | ?                                                  |
| 0x7C | sub_446B54  | ?                                                  |
| 0x80 | sub_446BE9  | ?                                                  |
| 0x84 | sub_446CBC  | ?                                                  |
| 0x88 | sub_446E19  | ?                                                  |
| 0x8C | sub_4430B0  | ?                                                  |
| 0x90 | sub_443273  | ?                                                  |
| 0x94 | sub_44356F  | ?                                                  |
| 0x98 | sub_443643  | ?                                                  |
| 0x9C | sub_446D84  | ?                                                  |

LIEU01 DLL loads all 40 slots except +0x10 (KillPerso — not used in
level 1 init). Ingestion at DLL raw offset 0x18C7A performs a series
of `mov eax, [ebp+8]; mov eax, [eax+N]; mov [dword_1002cXXX], eax`
triples — one per slot.

## Scripting action struct (at dword_4F4CC0)

Callbacks that build queued actions use this 48-byte struct:
```
+0x00: i32 action_id  (0=NamedObj, 1=NamedObj+flag, 2=NamedObj+2int,
                      3=Cinematique, 5=NamedObj+extra_str, ...)
+0x04: i32 flag / int arg 1
+0x08: i32 int arg 2
+0x24: char name[N]        (for str-arg variants)
+0x30: char cinematique[N] (for movie-arg variant)
+0x40: i32 param1
+0x48: i32 param2
```

After filling struct, callbacks call:
- `sub_45D560` — queue/dispatch action (mutates dword_4FFB50)
- `sub_4441A5` — allocate action node in queue

## .data section (per level — LIEU01)

.data (~40 KB) holds identifier strings that appear as arguments in
the compiled callback calls:

- `SGxxDIAyy` — story-goal dialogue (e.g. SG01DIA01)
- `SPxxACTyy` — player action (e.g. SP01ACT96, SP03ACT17)
- `SGxxATTyy` — story-goal attach (e.g. SG01ATT01)
- `LCODxx` / `LCODxxN` — location code (zone triggers; e.g. LCOD41, LCOD102F)
- `SOBxx` — script object (e.g. SOB24, SOB77, SOB86)
- `Sar_xxx` — Sar character state (e.g. Sar_500, Sar_50)
- `LxxByy` / `LxxDyy` — cross-level references (e.g. L08B06, L31D83A)
- `SPxxPIPIy` — pipe-related actions

These are hardcoded C strings baked into each LIEU DLL by the level
designer. FonctionEditor code calls the engine API with these
strings as arguments to trigger dialogue, spawn objects, etc.

## Reproduction options (M6 target)

Three viable paths:

### (a) Per-level static hand-port
Decompile each `FonctionEditor@16` chain (55 levels × ~40 KB code
each = 2 MB total). Hand-write equivalent C script per level.
- Pro: no runtime dependency, clean native code
- Con: massive manual effort, prone to bugs, hard to verify

### (b) x86 emulator (unicorn engine)
Load LIEU DLL as mapped memory, hook the 40 callback slots to
native C stubs, run `FonctionEditor@16` via unicorn from opcode=1.
- Pro: bit-exact behaviour, one implementation for all 55 levels
- Con: adds unicorn dependency, security surface (DLL is untrusted
  input), performance overhead per callback
- Setup: ~1 week (mapper + callback bridge + verification harness)

### (c) Bytecode extraction via emulator + replay
Same as (b) but instead of running LIEU on every level enter,
record the sequence of callback IDs + string args during a one-time
emulator dump run. Store as a table `LIEU%02d.script`. Native engine
replays scripts from tables — no DLL loading in shipping build.
- Pro: shipping build has no emulator dependency, deterministic
- Con: assumes scripts are pure sequences (no conditional branching
  based on player state). Verification: run one level through both
  paths, diff callback trace.

Currently the LIEU init call likely only registers static level
data (spawn scripts + trigger tables); dynamic per-frame updates
via opcodes 3..7 may need actual emulation.

## Files

- LIEU01.DLL: 197632 bytes, 5 PE sections
- .text VA=0x1000 size=0x21C4D (135 KB compiled code)
- .rdata VA=0x23000 size=0x800 (2 KB const)
- .data VA=0x24000 size=0x9EB4 (40 KB baked strings + tables)
- .idata VA=0x2E000 size=0x61C (only KERNEL32 imports)
- .reloc VA=0x2F000 size=0x4044

## Cross-references

- sub_448F4D @ 0x448F4D — engine LIEU loader
- dword_4D6198 @ 0x4D6198 — callback table base (40 × 4 bytes)
- dword_4F4CC0 @ 0x4F4CC0 — action struct scratch
- dword_505C40 @ 0x505C40 — LIEU state buffer (0xFA0 bytes)
- dword_4B53FC @ 0x4B53FC — FonctionEditor return value
- dword_4B476C @ 0x4B476C — reload flag (level enter vs. reload)
