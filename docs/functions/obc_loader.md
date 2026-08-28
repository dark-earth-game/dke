# Object hierarchy loader — `.OBC` / `.ATM` / `.OME`

Source: `src/obc.[ch]`. Wired from `setup_creature` (`src/game_init.c`) and
consumed by `src/player.c` and `src/scene.c`.

Every 3D asset in the game — characters, weapons, props — is a **hierarchy of
0xB4-byte nodes**, each optionally pointing at up to three **atoms** (meshes).
This is the structure the whole render/animation/collision side hangs off:
`entity+0x50` is the root node, `entity+0xFC` is the 34-slot bone table filled
by matching node names, and `entity+0x184` is an *embedded* node of the same
type named "gravit" that carries the entity's world transform.

## File shapes

The same two chunk bodies appear in three containers:

| container | contents | used when |
|---|---|---|
| `<name>.OBC` + `<name>.ATM` | one OBJC chunk + one ATOM chunk per file | CD install (`is_using_hdd == 0`) |
| `<name>.OME` | OBJC chunk + N ATOM chunks concatenated | HDD install |
| `<name>.OME` with a `CTX3` wrapper | payload starts at `u32 @ +0x08` | `PERSOS/P*.OME` |

Chunk bodies are byte-identical across containers, so the port has one parser
with two front ends: `obc_load()` (file pair) and `obc_load_ome()`.

A few `.OME` files (`PERSOS/SERPENT`, `OBJETS/PLAQUE`, `OBJETS/RAQUETTE`) hold
only a MATR chunk and legitimately have no object tree.

## Chunk layouts

**OBJC header, 0x30 bytes** — only three fields are read:

| off | meaning |
|---|---|
| +0x00 | `"3DITOR vX.Y OBJC"` |
| +0x10 | node count |
| +0x20 | atom-name count |
| +0x24 | atom-name table offset, **relative to the chunk** |

Then `node_count` length-prefixed node records, then the name table
(`name_count` × 16 bytes). `"tree"` is the reserved "this slot has no
geometry" marker.

**Node record, 0xB4 bytes.** The reader (`sub_452EA6` case 2) copies disk
`+0x04..+0xA3` into the live node and forces `+0x88 = 0`; the trailing 16
bytes are dropped.

| off | field |
|---|---|
| +0x00 | record size (read as a **byte**, so records cap at 255) |
| +0x04 | name (16 bytes) — matched against the 34 bone names |
| +0x20/+0x24/+0x28 | atom-name indices, `-1` = none |
| +0x3C/+0x40/+0x44 | local translation |
| +0x54/+0x58/+0x5C | local rotation (16-bit BAM) |
| +0x78/+0x7C/+0x80/+0x84 | parent / first child / previous sibling / next sibling, as node indices, `-1` = none |
| +0x9C/+0xA0 | control-point flag and pointer ("obj->Clefs", 0x1C bytes) |

`sub_452B20 @ 0x452B20` resolves every index into a pointer in a second pass.

**ATOM header, 0x30 bytes.** The version digits are patched to `"----"` before
the signature compare, so v1.x and v2.x both pass. `+0x20` and `+0x24` carry
the on-disk face and vertex record sizes (100 and 32), written over the middle
of the exporter's path string.

**Atom record, 0x40 bytes** — name at +0x04, flags +0x14, vertex count/max at
+0x1C/+0x20, face count/max at +0x24/+0x28, vertex array +0x38, face array
+0x3C.

**Vertex, 32 bytes**: x, y, z, nx, ny, nz, then two exporter heap fields.
**Face, 100 bytes**: size, vertex count (3 or 4), four indices, face normal,
material id at +0x24, flags at +0x30, four (u,v) pairs at +0x3C..+0x58, and a
runtime scratch dword at +0x60 that the loader zeroes.

## Port notes

- **Arrays are stored by live count, not max count.** `sub_453346 @ 0x453346`
  passes the *max* slot count to the array reader, which over-runs the stored
  data — harmless in a standalone `.ATM` whose atom is the only thing in the
  file, but inside a `.OME` it walks into the next chunk, and for
  `OBJETS/OB58.OME`'s "armeb" (vert max 64 vs 12 stored) it runs past EOF. The
  port allocates the max (the runtime appends to these arrays) and reads the
  live count.
- **Chunks are located by signature search, not sequentially.** Chunk spans are
  not recorded anywhere and the over-read above makes a sequential walk
  unreliable.
- **The `.COB` cache is not reproduced.** `sub_4538FB @ 0x4538FB` writes a
  pre-parsed copy next to the object directory to avoid re-parsing from CD; the
  port always parses the source.
- **Per-face texture resolution is not done yet.** The original resolves the
  face material id through `sub_450BEB @ 0x450BEB` and then biases the four UV
  pairs by that material's atlas rect. The lookup goes through a reverse LUT
  packed into the high bytes of `mat_lib_t::name`, which `mat_load()` does not
  populate (it stops after the bitmap table). Faces keep their raw material id
  and unbiased UVs.
- **Nodes are C structs, not blobs.** The original layout embeds 32-bit
  pointers; keeping it as a flat blob aliases on a 64-bit host, which is the
  recurring crash class in this port. `src/scene.c` and `src/player.c` had
  three node walks still using raw offsets (`bone_link_slots`,
  `scene_weapon_entity_attach`, `process_color_palette`) — those were converted
  when the loader started returning real trees, and one of them segfaulted
  until it was.
- The comment on `process_color_palette @ 0x40C2A0` had `+0x7C` and `+0x84`
  labelled as sibling and child. They are the other way round; the traversal
  itself was already correct.

## Fixed-point transforms

`obc.c` also carries the five primitives the rest of the engine builds on,
transcribed from asm. The matrix is 0x30 bytes: a 3x3 Q14 rotation at +0x00
and an integer translation at +0x24, applied **transposed** — the same
convention as the in-game camera.

| function | asm | effect |
|---|---|---|
| `fmat_rotation` | `create_rotation_matrix @ 0x48FA20` | 3 BAM angles → rotation |
| `fmat_position` | `create_position_matrix @ 0x48FD60` | identity + translation |
| `fmat_rotate_translation` | `multiply_matrices @ 0x4903C0` | `b.t = aᵀ · b.t` (b.m untouched) |
| `fmat_premul_rotation` | `transform_vertex @ 0x4901E0` | `m.m = r.m · m.m` |
| `fmat_transform_point` | `transform_vector @ 0x48F424` | `v = mᵀ · v + m.t` |

The table the original calls `sine_table` is a full-period **cosine** table,
which the port already builds as `cosine_table`; the "+8" index form is
`cos(a)` and the "−0x3FF8" form is `sin(a)`.

## Bounds, boxes and control points

- `obc_node_bounds` — `get_node_bounds @ 0x436651`. Walks the sub-tree with an
  explicit 16-level stack (the original's frame allows exactly 16),
  accumulating each node's local transform, transforms every vertex of every
  face and keeps the component-wise min/max. Both outputs are relative to the
  root node's own pivot.
- `atm_vertex_bounds` — `sub_436AB0 @ 0x436AB0`. Untransformed AABB over one
  atom's own vertices. Note it returns max in the register argument and min in
  the stack one — the opposite of `get_node_bounds`.
- `obc_build_box_atoms` — `sub_436C56 @ 0x436C56`. Every node with geometry
  gets an 8-vertex / 6-quad proxy box in its **second** atom slot (node+0x24)
  spanning that atom's AABB, faces pre-set to colour `0x2222` and index
  `0xFF`, the two fields `process_color_palette @ 0x40C2A0` reads and writes.
  Then the "arme" → "armem" → "arme" weapon-chain check, which only logs.
- `obc_create_control_point` — `create_single_control_point @ 0x404D92`.
  radius = largest AABB extent / 2 (`scaling_constant` is 2.0), centre = AABB
  centre, stored in a 0x1C-byte "Clefs" block at node+0xA0.
- `obc_process_hierarchy` — `process_scene_hierarchy_with_memory @ 0x404C26`
  and `process_scene_hierarchy_control_points @ 0x4040B8`. Per-limb collision
  spheres, `Clefs[count]` at node+0xA0 with count in node+0x9C.

### Per-limb collision spheres

`get_node_type @ 0x41DE03` maps a node name to an index in the 34-entry table
at 0x4B2BA0 — **the same table and the same index as the entity bone slots**
(`sub_41F6BA @ 0x41F6BA`), so a node's "type" *is* its bone slot. `"armeb"` is
special-cased to 0x22, unknown names give -1.

The 34-case switch at 0x4043BD (dispatched through the byte table at
0x40442D, on `type - 1`) gives each bone a sphere count and a fatness:

| bone | type | spheres | scale |
|---|--:|--:|--:|
| basdos | 1 | 1 | 0.7 |
| hangau / handro | 2, 3 | 1 | 0.3 |
| gendro / gengau | 4, 6 | 2 | 0.4 |
| piedro / piegau | 5, 7 | 2 | 0.5 |
| haudos | 9 | 1 | 0.6 |
| epagau / epadro | 11, 12 | 2 | 0.4 |
| coudro | 13 | 2 | 0.5 |
| poidro / poigau | 14, 16 | 1 | 0.8 |
| cougau | 15 | 1 | 0.5 |
| tete | 17 | 1 | 0.7 |
| arme | 31 | 3 | 0.3 |
| armem | 34 | 4 | 0.3 |

Everything else falls to the default slot and gets no spheres — including
`tree` (type 0, so `type - 1` wraps negative), and deliberately `mildos` and
`bascou`, the mid-torso and lower neck. The `coudro` / `cougau` asymmetry (2
vs 1 sphere) is in the original table, not a porting slip.

Given the atom's AABB, the longest axis becomes the chain direction,
`step = length / count`, and
`radius = (longest + shortest) * scale / 2`. The first centre sits half a step
in from the far end on the primary axis and mid-span on the other two; each
subsequent sphere steps back by `step`.

For P00 this produces a symmetric ragdoll — `epadro`/`epagau` and
`gendro`/`gengau` mirror exactly on Z — which is a good end-to-end check that
the vertex decode and the bone naming both line up.

**Deviation:** when a node already has control points the original jumps past
the type lookup into the "arme" test with a *stale* type still in its stack
slot, which can move the traversal cursor. The port initialises the type per
node instead.

## Still unported

- `sub_491470 @ 0x491470`, called at the tail of `sub_452B20`.
- The "gravit" re-point in `sub_46493B @ 0x46493B` (`464C53..464C90`): the
  original zeroes the loaded tree root's translation, re-parents it under the
  embedded node at entity+0x184, caches it at entity+0x200 and makes
  entity+0x50 point at the embedded node. The port still leaves entity+0x50
  pointing straight at the loaded tree, which is what the bone table and the
  control point currently read.

## Parity

- build: yes (`-Wall -Wextra`, clean)
- launch: yes — `DKE_NEWGAME=1 DKE_NOMOVIE=1 DKE_FRAMES=200`, exit 0
- behaviour: `PERSOS/P00.OME` loads as 29 nodes / 20 atoms with the expected
  bone names (`tree` → `basdos` → `mildos` → `haudos` → …), bone table fills,
  proxy boxes on all 20 geometry nodes, root control point
  `r=55126 centre=(-11926,1901,-37)`, and a left/right-symmetric sphere chain
  on every limb. Loader run over the whole corpus: 105/106 `persos` and
  103/105 `objets` `.OME` files, plus the single `.OBC` (`OBJETS/FLAM`); the 3
  misses are the MATR-only files listed above. All 211 files are
  AddressSanitizer-clean through load → boxes → control points → free.

## Not a useful target

`sub_462BCC @ 0x462BCC` loads the persona descriptor
`PERSOS>p%02d>p%02d.txt` (via `sub_462C9E` / `sub_461DC4` / `sub_46319A`).
No `persos/p00/` directory and no `.txt` descriptors ship in the retail
install — they are dev-only assets — so that path cannot succeed regardless of
whether it is ported. This is why `weapon_type` reads back as 255.
