# Reverse-Engineering Plan

Living plan for the Dark Earth engine port. Tracks the current milestone, in-flight work, and prioritised next slices. Cross-links: [ROADMAP.md](ROADMAP.md) (milestone view), [FUNCTION_MAP.md](FUNCTION_MAP.md) (addr → C map), [SUBSYSTEMS.md](SUBSYSTEMS.md) (per-subsystem notes), [TODO.md](../TODO.md) (top-level bullets).

## Delegation policy

- **Opus** (this thread): planning, scoping, ambiguity, cross-file refactors, inline surgical fixes when scope is bounded.
- **Sonnet** (background agents): single-target ports and multi-file wiring with a clear spec. Every prompt names the exact asm addr, files, verification steps.
- **Haiku / cavecrew-builder**: 1-2 file mechanical edits, typo fixes, small mechanical rewrites.

Rule: never delegate understanding. Prompts include file paths, addresses, and expected diffs.

## Verification protocol (per port)

For every landed port:

1. `make dke` — must exit 0, no new warnings.
2. Launch smoke: `( cd bin/game && ./dke > /tmp/dke.log 2>&1 & PID=$!; sleep 3; kill $PID; ... )` — expect exit 143. Look for `Abort trap` / `Segmentation fault`.
3. `/tmp/dke.log` — the new function must log a phase line so runtime order is verifiable.
4. Update `docs/FUNCTION_MAP.md` row: `todo`/`stub` → `ported`/`partial`.
5. Update `docs/SUBSYSTEMS.md` per-subsystem section if new capability.
6. Update `docs/functions/<name>.md` for non-trivial ports.

## Milestone status (2026-07)

| M | Name | Status |
|---|---|---|
| M0 | Foundations — window/blit/config/memory manager | ✅ done |
| M1 | Libsys subsystem stubs (mouse/sound/cd/joy/video_mode/bmap_ex) | ✅ done |
| M2 | Intro cutscenes (movie SEB + CRH decoder + CoreAudio) | ✅ done |
| M3 | Font system + CIF decoder + palette-aware text | ✅ done |
| M4 | Game bootstrap (`game_init_46BEEB` skeleton + top stubs) | ✅ done (skeleton + real image loaders + font finalize) |
| M5 | Main menu (`process_mainmenu_437EB7`) | ⏳ queued |
| M6 | Level loader + first LIEU DLL | ⏳ queued |
| M7 | Entity / combat / render pipeline | ⏳ deferred |
| M8 | Interactive gameplay | ⏳ deferred |

## In flight (background)

*(none — Slice A up next)*

## Recently landed

- **Task #15** (Sonnet): `select_language_version`, `image_load_compressed` (CIF wrapper w/ `.GIF → .CIF` ext fallback), `image_load_compressed_free`, `sub_415FB0` font finalize, `load_image_color_data`. Real bytes now hit `g_loading_bmap` (512×64), `g_menugen_palette`, `g_objets_gif` (256×512), `INTFANI3` (189×119), FONTE11 atlas (691×224). Confirmed via log.

## Next slices (priority order)

### Slice A — Real menu backdrop rendering (unlocks visible UI)

**Blocker for**: seeing anything on screen besides intro cutscene.

1. **`draw_image` / `ls_blit_bmap`** — real bmap → framebuffer blit with palette lookup. Currently only `draw_text_at` writes to `front_buffer`. Blit needs to handle: 8bpp indexed + palette → XRGB; 16bpp direct copy; source clip; dest offset.
2. Wire loaded CIF images (`g_menugen_palette`, `g_loading_bmap`) into `ls_main`'s main loop so something shows up after `game_init`.
3. Idle main-loop state: display a splash while waiting for user input.

Scope: 1 Sonnet delegation. Address anchor `~40C0XX` blit helpers in asm.

### Slice B — Main menu (`process_mainmenu_437EB7`)

**Blocker for**: user interaction. Full trailer/new game/load/options.

Scope: ~600 asm lines. Deep tree — has mouse handlers, menu tree state, sub-menus. Requires Slice A first (needs draw_image + text on backdrop).

Split into two Sonnet passes:
1. Menu state machine + hitbox layout (skeleton). Draws static menu w/o interaction.
2. Mouse click routing → menu action handlers.

### Slice C — Level bootstrap (first playable frame)

**Blocker for**: entering the game world.

1. `LIEU01.DLL` .. `LIEU55.DLL` — level DLLs. Analyse w/ `objdump -d bin/game/LIEU01.DLL` to identify data layout. Level data likely embedded in .rdata as arrays of entity descriptors, geometry, palette.
2. Level loader in `src/level.c`: parse chosen LIEU DLL, allocate world entities, init spatial grid.
3. Wire DKE.CFG `FirstRoom` to select which LIEU to load.

Scope: 2-3 Sonnet passes. Needs entity system (Slice E) partially online.

### Slice D — Player entity + input routing

- `sub_46493B` / `sub_464F6D` — player entity init (currently stub in game_init).
- Keyboard bindings from `state->use_key` — WASD/arrow keys → player velocity.
- `sub_45B207` — combat mode wire.

Scope: 1-2 Sonnet passes. Independent of Slice C — can run in parallel.

### Slice E — Combat / entity update pipeline

- `sub_4342C7` — main entity tick (already documented via XREFs).
- `process_combat_block` / `process_entity_damage` / `check_entity_special_abilities` — damage pipeline.
- `handle_entity_state_transition` — state machine.

Scope: 3-5 Sonnet passes. Big surface area. Defer until Slice C provides world context.

### Slice F — 3D render pipeline

- `graphics_triangle_rasteriser` — software rasteriser.
- `create_model_render_data` — model → triangle list.
- `render_entity_with_ui` — top-level scene render.
- `apply_palette_transformation` — palette-per-face shading.

Scope: massive. Defer.

## Small parallel work (Haiku)

Queue for cavecrew-builder passes whenever a slot is free:

- Palette lookup helper: `u32 palette_lookup(u8 idx, const u8 *palette)` in `palette.c` — used by future blit + already by draw_text.
- `image_load_compressed_free` symmetric release.
- Log filter: suppress verbose `game_init:` traces when `state->debug_enabled == false`.
- Rename all `sub_XXXXXX` in `game_init.c` static forward decls to semantic names once their function is confirmed.

## Format decoding backlog

Ordered by unlock value:

| ext | purpose | reference | status |
|---|---|---|---|
| CRH | cutscene | `tools/src/CRH_to_MP4.py` | ✅ ported |
| CIF | images / font atlas | `tools/src/dump_cif.py` | ✅ ported |
| CAR | font glyph index | asm | ✅ ported |
| GIF | HUD/menu images | *(shipped as CIF — ext-swap)* | 🔄 in flight (Slice A) |
| MAT | material textures | asm — inspect `FLAM.MAT` | ⏳ queued |
| CTX | binary caches (INVENT.CTX, OBJETS.CTX) | asm | partial (INVENT.CTX read path in `palette_postprocess_configure`) |
| TXT | game-side text tables (INVENT.TXT, objets.txt) | asm | partial (INVENT.TXT parse) |
| GRD | palette gradient | `bin/game/datas/global/PAL/*.GRD` | ⏳ queued |
| MAP | palette map | `bin/game/datas/global/PAL/*.MAP` | ⏳ queued |
| CAN | anim (`bin/game/anm/P*.CAN`) | asm | ⏳ queued |
| CJR | big anim (`bin/game/biganims/ANIM*.CJR`) | asm | ⏳ queued |
| OFF | anim offset table | asm | ⏳ queued |
| DLL | level (LIEU01..LIEU55, INVENT, HOTEL) | asm | ⏳ queued (Slice C) |
| WAV | sfx (`bin/game/wav/`) | riff wav | ⏳ queued |
| dialog/ | subtitle files | `bin/game/dialog/` | ⏳ queued |

## Known parity gaps

- Cocoa app suppresses stdout by default — fixed via `setvbuf` in `ls_init_instance` but only when launched from shell, not from Finder.
- `AQ_BUF_SIZE = 16 KB` (4× original) — CoreAudio buffer size chosen for CRH frame fit, may differ from original DirectSound.
- Video scaler uses nearest-neighbour 16.16 fixed point. Original engine may have used bilinear. Cosmetic.
- Font glyph colour looks up palette per-pixel. Original engine may have used a foreground/background mask flag. Verify against real menu screens once visible.
- `initialize_color_mapping_tables` produces 565 tables; original may branch on colour_mode == HICOLOR. Currently forced.

## Reference docs

- IDA Pro 8.4 project: `disassembly/dkev.i64`.
- Full asm dump: `disassembly/dkev.asm` (~970k lines, 1404 procs).
- Python tooling: `tools/src/*.py` (CRH, CIF, PCX, CTX dumpers).
- Original executable: `disassembly/dkev.exe` (patched no-CD).

## When to update this doc

- After each Sonnet delegation completes: mark milestone / slice status.
- When a slice priority changes: re-order the "Next slices" section.
- When a new format decoder lands: update the backlog table.

Do NOT let this doc replace `FUNCTION_MAP.md`. Address-level detail belongs there.
