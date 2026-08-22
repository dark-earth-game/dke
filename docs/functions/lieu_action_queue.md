# LIEU action queue ("les piles")

How a LIEU script actually makes something happen in dkev.exe, and how the
port replays it. Source: `src/lieu_action.[ch]` (queue + timers) and the
`lieu_action_dispatch` at the bottom of `src/lieu_cb.c` (effects).

Related: [lieu_dll.md](lieu_dll.md) (how the DLL is loaded and the 40-slot
callback table is handed over), [lieu01_handlers.md](lieu01_handlers.md),
[lieu_runtime.md](lieu_runtime.md).

## The shape of every primitive

The 40 engine callbacks a LIEU DLL can call (`sub_444xxx`..`sub_446xxx`) do
**not** act. Each one does exactly three things:

1. `memset(dword_4F4CC0, 0, 0x30)` and fill that scratch record with the
   action type and its arguments, then call the replay recorder
   `sub_45D560 @ 0x45D560` (writes the record plus the player/companion
   transforms to a demo file; only when `dword_4B6FA4 == 1`, not ported).
2. `sub_4441A5 @ 0x4441A5` — allocate a node in the active pile.
3. Fill the node's fields and set `dword_4FFB50 @ 0x4FFB50` (queue non-empty).

Effects happen later, on the frame the node reaches the head of its pile.

## Piles

Three independent queues. The script picks the allocation target with
SELECTPILE (`sub_446A05 @ 0x446A05` → `dword_505C30`).

| pile | nodes | count | enable | drain stamp | overflow scratch |
|---|---|---|---|---|---|
| 0 | `dword_4FC1A0` | `dword_4FC190` | `dword_4FC194` | `dword_4B5428` | `unk_4FF008` |
| 1 | `dword_502BB0` | `dword_50724C` | `dword_507274` | `dword_4B542C` | `unk_505A18` |
| 2 | `dword_4FFCD0` | `dword_507250` | `dword_507270` | `dword_4B5430` | `unk_502B38` |

100 nodes each, stride **0x78** (120 bytes) — from the allocator's index math
`base + 120*(count-1)` and from `memset(pile, -1, 0x2EE0)` in SELECTPILE mode 2
(0x2EE0 = 100 × 0x78). Overflow logs `"la pile N est saturee"` and hands back
the pile's scratch node, so a saturated pile silently drops work.

`dword_507248` holds the pile currently being drained; `dword_4B5424` its
count; `dword_4B5418[pile]` marks the head node as holding.

## Node layout (0x78)

Offsets recovered from the primitives that write them and the dispatch cases
that read them:

| off | meaning |
|---|---|
| +0x00 | action type; `-1` = free/complete |
| +0x04 | type of the action that completed before this one (`dword_4FC1A4`) |
| +0x0C | entity / object id (always `abs()`-ed by the primitive) |
| +0x10 | JOINDRE a4 / JOINDREPERM target |
| +0x14 | world X (script tile × 600) |
| +0x18 | world Y (script tile × 600) |
| +0x1C | world Z (script tile × 600) |
| +0x20, +0x24, +0x28 | per-type parameters |
| +0x30 | name buffer, 16 bytes (`_strupr`ed or `_strlwr`ed by the primitive) |
| +0x40 | hold time in ms; `-1` = complete immediately |
| +0x48 | EVENEMENT event id / anim slot |
| +0x4C, +0x54, +0x5C, +0x68, +0x70 | per-type parameters |

The ×600 scale shows up as the idiom `lea eax,[eax+eax*4]; shl eax,3;
lea eax,[eax+eax*2]; lea eax,[eax+eax*4]` and is the same 0x258 the HUD
divides by. **LIEU tile × 600 == dke world units** — see "Coordinate space".

## Action types

Index into the 16-byte-stride name table at **0x4B5438**, and identical to the
callback slot the script called:

```
 0 ANIMATION     10 CHANGEPIECE   20 BLOQUE        30 SELECTPILE
 1 DIALOGUE      11 BRUITAGE      21 FOUILLE       31 ONJOINDRE
 2 EVENEMENT     12 AJOUTEZONE    22 STOPJOINDRE   32 LINKOBJET
 3 CINEMA        13 RETIREZONE    23 VIRECOL       33 PLACEANIMAL
 4 KILLPERSO     14 COMBAT        24 SIMULT        34 DIVERS
 5 POSPERSO      15 DIRECPERSO    25 PRINT         35 PLAYANIM
 6 ETATOBJET     16 DIRECZONE     26 JOINOBJ       36 PAUSEANIM
 7 COLOBJET      17 DIRECSPEC     27 JOINDREPERM   37 DIVERSEMPILE
 8 JOINDRE       18 DISTANCE      28 MODIFYVAR     38 LOADANIM
 9 CHANGECAMERA  19 ANIMOBJET     29 RANDOM        39 FREEANIM
```

The tail slots do not line up with the callback index: slot 34 is DIVERS,
slot 36 is FREEANIM (0x27), slot 37 is PLAYANIM (0x23), slot 39 is
DIVERSEMPILE (0x25).

## Draining

`sub_44CDB8 @ 0x44CDB8`, called once per frame from the world update
`sub_461468 @ 0x46180A`:

- If `dword_4FFB50` is set, for each pile whose enable flag is 1 and whose
  head type is not `-1`: set `dword_507248`, call `sub_44A58B @ 0x44A58B`.
- **One head node per pile per frame.** A node stays at the head until its
  handler calls `sub_4442EE @ 0x4442EE`, which is how walks and timed
  animations span frames.
- 500 ms after a pile empties, the engine re-runs `sub_44A2B5 @ 0x44A2B5`
  once. That function is debug drawing only — not ported.

`sub_4442EE` carries the finishing type into `head+4` so the next action can
see what preceded it (ANIMATION checks for a preceding JOINDRE that way);
MODIFYVAR (0x1C) is skipped when carrying, keeping the type before it.

## Object timers → opcode 4

`EVENEMENT` is how a script schedules itself. Dispatch case 2
(`0x44AD41`) maintains a 20-entry table at `object_timers @ 0x4FFB90`,
16 bytes per record:

| off | field |
|---|---|
| +0x00 | gametime when armed |
| +0x04 | gametime of the last scan (`dword_4FFB94`) |
| +0x08 | duration in ms; `-1` = free slot (`dword_4FFB98`) |
| +0x0C | event id (`dword_4FFB9C`) |

- delay >= 0 → append (or reuse a free slot when `object_count` hits 0x13).
- delay == -1 → find the record with that id, drop it, compact the table.

`sub_44968B @ 0x4499A5` scans the table every frame (gated on
`time_delta == 0`) and, for each expired record, compacts it out and calls
`FonctionEditor(opcode 4, id)`. That is the only thing that fires the ported
`event4` handlers in `lieu01.c` — without it, every timed script beat stalls.

## What the port does

`src/lieu_action.c` mirrors the machinery: `lieu_action_push` /
`lieu_action_done` / `lieu_action_run` / `lieu_timer_add` /
`lieu_timer_remove`, driven from `lieu_tick(now_ms)` in `src/lieu_cb.c`.

Divergences, all deliberate:

- Nodes are zeroed on allocation and `+0x40` defaults to `-1`. The original
  leaves the previous occupant's bytes in place.
- A held node measures from `node->start_ms` instead of the owning entity's
  `+0xC28` timestamp, since dke has no per-entity animation clock yet.
- `DKE_NOQUEUE=1` parks the drain (actions still queue) as an A/B switch.

### Queued vs immediate

Only the callbacks that were previously log-only stubs go through the queue.
These stay immediate even though the original queues them: AJOUTEZONE,
RETIREZONE, CHANGECAMERA, CHANGEPIECE, POSPERSO, DIALOGUE, CINEMA and the
BLOQUE brackets. INIT emits a burst of those and `scene.c` consumes them on
the same frame; moving them behind a one-per-frame drain would stagger level
setup across ~40 frames and would need the scene side to cope first.

### Effects implemented

| type | effect |
|---|---|
| ANIMATION | publishes the CAN clip id for the named persona (the renderer swaps the persona's animation to it); honours the 500 ms "ar*" hold |
| EVENEMENT | arms/cancels an object timer → opcode 4 |
| ETATOBJET | writes the object registry (world position + parameters + owner) |
| JOINDRE | resolves the destination, hands it to the actor and completes; `lieu_walk_update` walks the placement at 3 tiles/s over later frames |
| JOINDREPERM | marks the actor joined |
| STOPJOINDRE | clears the actor's walk/join state |
| VIRECOL | writes the script flag table |
| LINKOBJET | records the object → holder link |
| ANIMOBJET | updates the object registry, honours the hold time |
| BRUITAGE | recorded only — no positional audio path yet |
| PLAY/PAUSE/LOAD/FREEANIM | 2D anim slot state (free/loaded/playing/paused) |

Everything else logs once and completes, so scripts keep advancing.

### Open questions

- **ANIMATION 2D path.** `'2'`-prefixed names build `<prefix><name>.ANI` and
  push it into the anim2d slot list (cap 20, "too many anim2d"). The port
  records the request but does not load the sprite. **LIEU01 never takes this
  branch** — its ANIMATION names are all persona clips — so wiring it has no
  test case at level 1. L01's `DATAS/L01/ANIM2D/*.ANI` files are the
  environment sprites loaded by the level loader (`sub_4130FB`), not by this
  callback.
- **`scene_release` leaks the persona mesh cache** (models + OBJC skeletons
  are never freed on level teardown).
- **Pile saturation.** With every INIT action queued, pile 0 hit its 100-node
  cap during bring-up. It no longer does, but the drain rate (1/frame/pile) is
  the original's, so a script that enqueues faster than that will saturate in
  the port exactly as it would have in 1997.

## JOINDRE (case 8 @ 0x44B5E3)

Worth its own section — it is the only action with real branching, and its
argument shape is not obvious from the call sites.

`Joindre(actor, tileX, tileZ, facing, target)` where `target` = node+0x10:

| target | meaning |
|---|---|
| `> 0` | a target **actor id**. With an explicit destination the target's position wins; without one the actor holds station and only the facing is computed. |
| `< 0` | a **zone reference**, index `-(target)-1` into the zone table (`dword_506C80`, stride 0x24). Only taken when the destination and facing are all zero: the actor faces the zone and stays put. |
| `== 0` | plain destination in node+0x14 / node+0x1C. |

node+0x24 is a **facing angle**, either supplied by the script or computed
actor → target by `sub_4440A6 @ 0x4440A6`. It is not a coordinate.

Then:

- `entity+0xCB4 = 0` — a Joindre cancels any permanent join.
- If the actor is already walking to the same destination, fall through to the
  re-issue path at 0x44B8DF instead of restarting.
- A non-player actor with `entity+0xD60 != 0` refuses:
  `"JOINDRE executed during COMBAT by perso %d"`.
- Otherwise write `entity+0xCC0/0xCC4/0xCC8` (destination X, Z, facing), raise
  `entity+0xCBC = 1` and `entity+0xC44 = 1`, and **complete the action**.

The walk itself is not the queue's job — the entity update moves the actor on
later frames while the pile keeps draining. Getting this wrong (holding the
node until arrival) stalls every dialogue line behind a walk.

In L01 the opening conversation uses the "hold station, face the target" form:
`actor_934(-submode, 0, 0, 0, 0x0C)` turns Arkhan toward Kaly.

## Coordinate space

Confirmed at runtime for L01 (`lieu_zone_debug` prints all three together):

```
lieu: zone space X[-37200..600000] Z[-1836600..-1224000] over 27 zones
lieu: spawn 'P12ARR01' world=(370800,-1875000)
lieu: spawn 'P00ARR01' world=(378000,-1837200)
scene: player -> zone 4 (wX=305083 wZ=-1802945)
```

AjouteZone rectangles, PosPerso placements, JOINDRE destinations, the camera
region polygons from the INF (cam0: X[172631..437536] Z[-1894218..-1711672])
and the player transform are all the same space: **script tile × 600**. No
conversion is needed between LIEU and dke world coordinates on X/Z, and zone
detection fires correctly on the seeded player position.

The remaining calibration gap is the **floor Y**, still a per-camera constant
(~-120000 for L01 cam0) rather than sampled terrain — see the in-game camera
notes, not this document.
