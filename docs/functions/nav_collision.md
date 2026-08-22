# Navigation grid and collision

How dkev decides where an actor may stand, and what the port uses today.
Source: `src/nav_grid.[ch]`, applied in `src/scene.c` on the player input path.

## The grid

`sub_45619D @ 0x45619D` rasterises the room into a fixed **64×64 byte grid**
at `interaction_data_array @ 0x4FA0F0` (0x1000 bytes). Indexing globals:

| global | addr | meaning |
|---|---|---|
| `global_x_offset` | 0x4F945C | world X of cell column 0 (world controller +0x1DC) |
| `global_y_offset` | — | world Z of cell row 0 (world controller +0x1E4) |
| `global_x_divisor` | 0x4F5E28 | world units per column, `(max-min)/64 + 1` |
| `global_y_divisor` | — | world units per row |

`cell = grid[((z - z_off) / z_div) * 64 + (x - x_off) / x_div]`

**The grid spans world (X, Z), not (X, Y).** The builder takes its bounds from
the world controller's min/max pair at +0x1DC/+0x1E4 and its polygon tests read
vertex components 0 and 2. The `sol_limit_t` field the port names `min_y` in
fact holds Z; for L01 it spans -1894305..-1182380, which matches the room's Z
range, and the room polygon's actual Y components are all ~0 (a flat 2D
footprint — there is no floor height in the INF).

Cell values: `0x00` FREE, `0x02` BORDER (wall-adjacent), `0x03` WALL,
`0xFF` BLOCKED (outside the room), `0xFE` produced by the spread passes that
push a ~15000-unit keep-out margin away from boundaries. Every consumer treats
**non-zero as blocked**, so BORDER and the margin are not walkable.

L01 builds as `cell=9628x11124 free=1992 wall=1406 blocked=133`.

## Consumers

- **`sub_456FB7 @ 0x456FB7`** — snap a point out of blocked space. Takes
  `ecx = &x`, `edx = &z`; returns immediately if the cell is already free,
  otherwise rings outward (ring size 3, then 5, 7, … walking top edge, right
  edge, bottom edge, left edge) until it finds a free cell and rewrites the
  point to that cell's origin. The original has **no bound** — it spins until
  it finds one. Ported as `nav_snap_to_walkable`, with a ring cap so a fully
  blocked grid cannot hang the frame.
- **`sub_45720D @ 0x45720D`** — per-entity reachability. Copies the grid into
  the entity's own 0x1000-byte workspace (free → 0xFF, blocked → 0) and flood
  fills from the entity's cell via `sub_477FD4`. **That workspace is
  `entity+0xCD8`**, three slots of 0x1000 at `byte_4F60E0` (player, companion,
  secondary) — not the "dialogue buffer" it used to be named in the port.
  Not ported.
- **`sub_459CD2 @ 0x459CD2`** — "which collision zone contains this point":
  walks the zone list at `dword_4F5FB0`, requires `flags & 1`, rejects on the
  bounding box at +0x1DC/+0x1E8/+0x1E4/+0x1F0, then runs point-in-polygon
  (`sub_458F60`) over the vertices at +0x14. A second pass covers up to 8
  dynamic entity obstacles flagged 0x20. The port's classification matches.
- **`sub_456E6A @ 0x456E6A`** — debug overlay drawing every cell. Not ported.
- **`sub_469222 @ 0x469222`** — the actor walk state machine, below.

## Actor walking (sub_469222)

Not ported; recorded here because JOINDRE hands off to it.

- `entity+0xCB4 != 0` (permanent join): find the joined actor by matching
  `entity+0xCB8` against player/companion/secondary `+0xB0`, compute the angle
  to it with `sub_4440A6` and face it (`sub_428714`). This is what makes
  JOINDREPERM a *follow*.
- Otherwise, if `now - entity+0xC0 > 15000 ms` and the walk state is not 1,
  fall into the idle handler `sub_461BA0`.
- Walk state at `entity+0xCBC`: `1` = new request — stamp `entity+0xC0`, ask
  `sub_468FFE` for the next waypoint, store it in `entity+0xCC0/0xCC4`, set
  state 2 and start moving; `4` = arrived — clear `+0xD14`, `+0xCBC`, `+0xC44`
  and set `+0xCDC = 1`. Arrival is a squared-distance test against
  `dbl_4B0880`.

Destination lives in `entity+0xCC0` (X), `+0xCC4` (Z), `+0xCC8` (facing) —
the fields JOINDRE writes.

## What the port does

The player is driven directly from input, not by the pathfinder, so collision
sits on the input path in `scene_render`:

- Each axis is committed separately, so walking diagonally into a wall slides
  along it instead of stopping dead. The engine never needs this because its
  actors only ever move to waypoints that are already free.
- `DKE_WALK` / `DKE_WALKZ` debug sweeps go through the same check, so they
  exercise it headlessly. A blocked move logs at most once every 500 ms.
- The player's seed position (solved from the camera zone centroid, so it can
  land inside geometry) is passed through `nav_snap_to_walkable` — L01 moves
  from (305083,-1802945) to (314112,-1794189), still inside zone 4.

Verified: a `DKE_WALK` sweep now stops at `X=278112` against a BORDER cell
after crossing zones 4 and 7, where it previously walked through the wall.

## Not done

- Pathfinding (`sub_468FFE`) and the walk state machine, so scripted actors
  move in straight lines rather than around obstacles.
- The per-entity reachability fill, and correcting the `entity+0xCD8` label.
- Stair zones (`world_data_t.stair_zones`) are parsed but never consulted, so
  there is no height change anywhere.

## Player start position — unresolved conflict

Two independent sources disagree about where L01 starts the player, by ~34000
units on Z:

| source | position | in a zone? |
|---|---|---|
| trig00 PosPerso for P00 (Arkhan) | (378000, -1837200) | no — outside every zone |
| camera-zone anchor solve | (305083, -1802945) | yes — zone 4 |

The PosPerso placement is what the engine uses on a room entry and is almost
certainly the right one. But zone 4's trigger is what has been driving the
opening conversation the whole time: seed the player from the script and no
trigger fires, so the level starts and nothing happens.

So one of the two mappings is wrong — either the PosPerso tile scale or the
anchor solve. Until that is settled the anchor stays the default because it is
the one that produces a playable opening; `DKE_SCRIPTSEED=1` selects the
scripted placement for comparison.

Related: a scripted placement must NOT be snapped to a walkable cell. L01
starts Arkhan lying on the bed, which the grid marks as furniture, so
`nav_snap_to_walkable` only runs on the guessed anchor.
