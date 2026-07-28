# LIEU01 Runtime Handlers — Reverse Map

Pilot for hand-porting LIEU runtime opcodes (0, 3-6) to native C.
Init (opcode 1) already covered by `.script` replay; this doc covers the
**state-conditional** runtime behaviour that replay cannot capture.

Source: LIEU01.DLL (ImageBase 0x10000000). Entry
`@FonctionEditor@16` @ RVA 0x1046 → `@FonctionEditor@16_0` @ 0x18C60.

## Opcode dispatch (jump table @ 0x100192F9, index = opcode+1)

| Opcode | Entry     | Resolves to           | Size  | Role                    |
|--------|-----------|-----------------------|-------|-------------------------|
| 0 tick | 0x1927C   | inline + `sub_100088FE` | 11 KB | per-frame; drains pending sound at state+0x134 |
| -1 fin | (0x18C60) | inline + `sub_100088FE` | —     | deinit: stop pending sound |
| 1 init | 0x18C7A   | inline (ingest 40 cb) + arm | —  | already extracted (script) |
| 2      | 0x19233   | no-op                 | —     | reserved                |
| 3 evt  | 0x19238   | `trigger[arg]()`      | table | dispatch registered handler |
| 4      | 0x100010F5→`sub_1000123A` | 4 KB | 1 stack arg |
| 5      | 0x10001005→`sub_100022B4` | 14 KB | 2 stack args |
| 6      | 0x10001041→`sub_10005BEC` | 1 KB | 1 stack arg |

## Tick handler (opcode 0 @ 0x1927C) — PORTED

```
if (state[0x134] != 0) {              // pending sound/cine id
    named_two_ints(2, 1);             // cb_464 / state_toggle(mode=2,layer=1)
    named_two_ints(state[0x134]+0x59, 1000);   // cb_674 start sound
    state[0x134] = 0;
    named_two_ints(0, 1);             // state_toggle(0,1)
}
sub_100088FE();                       // 11 KB per-frame body — TODO
return state[0];
```
(-1/deinit is the same drain, then sub_100088FE.)

## Trigger table (opcode 3) — dword_1002C880[42]

Each slot holds a 5-byte jmp thunk (0x10001000+5*i region) → real handler.
Decoded thunk→target below. **stub** = 0xB-byte no-op (ret/tiny); skip.

| idx | handler addr | size   | notes                                  |
|-----|--------------|--------|----------------------------------------|
| 0   | 0x100073F0   | 0x13A8 | real (5 KB)                            |
| 1   | 0x1000B563   | 0x04E4 | real                                   |
| 2   | 0x1000BA47   | 0x0380 | real                                   |
| 3   | 0x1000BDC7   | ~0x3AA | real (unlisted; runs to 0x1000C171)    |
| 4   | 0x1000C171   | 0x10DE | real (4 KB)                            |
| 5   | 0x1000D24F   | ~0xBF  | real (unlisted; runs to 0x1000D30E)    |
| 6   | 0x1000D30E   | 0x000B | stub                                   |
| 7   | 0x1000D319   | 0x131D | real (5 KB)                            |
| 8   | 0x1000E636   | 0x035B | real                                   |
| 9   | 0x1000E991   | 0x1B42 | real (7 KB)                            |
| 10  | 0x100104D3   | ~0x67  | small (unlisted; runs to 0x1001053A)   |
| 11  | 0x1001053A   | 0x053A | real                                   |
| 12  | 0x10010A74   | 0x000B | stub                                   |
| 13  | 0x10010A7F   | 0x000B | stub                                   |
| 14  | 0x10010A8A   | 0x000B | stub                                   |
| 15  | 0x10010A95   | 0x04FB | real                                   |
| 16  | 0x10010F90   | ~0xB1  | small (unlisted; runs to 0x10011041)   |
| 17  | 0x10011041   | 0x0AB2 | real                                   |
| 18  | 0x10011AF3   | 0x0721 | real                                   |
| 19  | 0x10012214   | 0x000B | stub                                   |
| 20  | 0x1001221F   | 0x000B | stub                                   |
| 21  | 0x1001222A   | 0x0373 | real                                   |
| 22  | 0x1001259D   | 0x000B | stub                                   |
| 23  | 0x100125A8   | ~0x4FE | real (unlisted; runs to 0x10012AA6)    |
| 24  | 0x10012AA6   | 0x0308 | real                                   |
| 25  | 0x10012DAE   | 0x024F | real                                   |
| 26  | 0x10012FFD   | 0x09CB | real (2.5 KB)                          |
| 27  | 0x100139C8   | 0x08B5 | real (2 KB)                            |
| 28  | 0x1001427D   | 0x0626 | real                                   |
| 29  | 0x100148A3   | 0x03B3 | real                                   |
| 30  | 0x10014C56   | 0x0287 | real                                   |
| 31  | 0x10014EDD   | 0x0287 | real                                   |
| 32  | 0x10015164   | 0x0287 | real                                   |
| 33  | 0x100153EB   | 0x000B | stub                                   |
| 34  | 0x100153F6   | 0x000B | stub                                   |
| 35  | 0x10015401   | 0x000B | stub                                   |
| 36  | 0x1001540C   | 0x000B | stub                                   |
| 37  | 0x10015417   | 0x000B | stub                                   |
| 38  | 0x10015422   | 0x000B | stub                                   |
| 39  | 0x1001542D   | 0x000B | stub                                   |
| 40  | 0x10015438   | 0x000B | stub                                   |
| 41  | 0x10008798   | 0x0166 | real                                   |

Real handlers to port: idx 0-5, 7-11, 15-18, 21, 23-32, 41 (~25 funcs,
~45 KB asm). 15 stubs (idx 6, 12-14, 19-20, 22, 33-40) → no-op.

**STATUS (2026-07): all 25 real trigger handlers PORTED** to `src/lieu01.c`
(20 via parallel agent fan-out + disasm; 5 by hand). trig30/31/32 share
`e_trig_spawn_act`; many share `e_spawn_ar` (spawn "ar" at layer-computed
coords). Internal thunks resolved: sub_10001014→trig27, sub_10001028→trig11,
sub_10001032→trig30, sub_100010B4→trig17, sub_100010CD→trig18,
sub_100010F0→trig29. Compiles clean at -Wextra.

Also ported: init, tick+deinit, tick_body (sub_100088FE full 2183 instr),
event6 (100%), event5 (~95%, 43 case labels), event4 (~92%).
Remaining: event5 cases 71/88, event4 cases 81/90-93/98 (float FSM /
var_C-heavy). Callback ABI (src/lieu_cb.h): ~38/40 slots typed.

## Engine callback ABI

See `src/lieu_cb.h` — 40-slot table, index→global→engine-sub mapping
recovered from INIT ingestion + `lieu_dll.md`. Five HOT slots
(named_two_ints, cinematique, state_toggle, layer_op, op_9c) drive
tick + INIT; port these first.

## State-buffer offsets (dword_505C40, 0xFA0)

| off   | role                                       |
|-------|--------------------------------------------|
| 0x000 | i32 result (handlers return state[0])      |
| 0x134 | i32 pending sound/cine id (+0x59 base)     |
| 0x194 | i32 level sub-mode (INIT special if ==4)   |
| 0x1A0 | i32 init-done flag                         |
| 0xB04 | i32 intro-cine select (0=SC22500,1=SC23500)|
| 0xE8C | i32 force-default-init                     |

## Tooling note

The `ida-mcp decompile` tool is currently returning the pinned 0x18C60
pseudocode widget for every address (widget-focus bug). Reverse via
`disasm` (per-address, reliable) until the decompiler is unstuck —
re-focusing/closing the Pseudocode tab in IDA usually fixes it.
