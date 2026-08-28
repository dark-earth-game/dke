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

## Bounds and control points

- `obc_node_bounds` — `get_node_bounds @ 0x436651`. Walks the sub-tree with an
  explicit 16-level stack (the original's frame allows exactly 16),
  accumulating each node's local transform, transforms every vertex of every
  face and keeps the component-wise min/max. Both outputs are relative to the
  root node's own pivot.
- `obc_create_control_point` — `create_single_control_point @ 0x404D92`.
  radius = largest AABB extent / 2 (`scaling_constant` is 2.0), centre = AABB
  centre, stored in a 0x1C-byte "Clefs" block at node+0xA0.

## Still unported

- `process_scene_hierarchy_control_points @ 0x4040B8` (~640 instructions), the
  per-node control points behind `process_scene_hierarchy_with_memory
  @ 0x404C26`.
- `sub_436C56 @ 0x436C56` — builds an 8-vertex bounding-box proxy atom at
  node+0x24 for every node that has geometry, plus the "arme"/"armea"/"armem"
  weapon-chain sanity checks.
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
  control point `r=55126 centre=(-11926,1901,-37)`. Loader run over the whole
  corpus: 105/106 `persos` and 103/105 `objets` `.OME` files, plus the single
  `.OBC` (`OBJETS/FLAM`); the 3 misses are the MATR-only files listed above.
