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
| M5 | Main menu (`process_mainmenu_437EB7`) | 🔄 pass 3 landed (overlay + INVENT1.WAV; hitboxes/sub-panels pending) |
| M6 | Level loader + first LIEU DLL | ⏳ queued |
| M7 | Entity / combat / render pipeline | ⏳ deferred |
| M8 | Interactive gameplay | ⏳ deferred |

## In flight (background)

*(none — Slice B pass 4 landed: real hitboxes + GENICO single-sheet crop + cursor crop + palette fix. Intro fade / MENUPERS / save/options panels deferred to pass 5.)*

## Recently landed

- **Slice B pass 4** (Opus, 2026-07): Real `var_4AE0[8]` hitboxes; GENICO_<lang>.CIF as single sprite sheet with per-item sub-rect crop (no more animation cycling); CURS128 shape crop per hover state; `blit_bmap_subrect_masked` helper with correct BGR-order XRGB packing (previous helper had R/B swap → blueish hover art). See Slice B section for detail.
- **Slice B pass 3 refinement** (Sonnet, 2026-07): Real menu image overlay + INVENT1.WAV path fix. `MENU_1.CIF` (640×480 full-screen sprite) loaded in `menu_init` → `menu->overlay_bmap`; blitted with `blit_bmap_masked_to_framebuffer` after MENUGEN backdrop; plain text suppressed when overlay present. `menu_load_click_sfx` search order fixed — `wav/ANIMS/INVENT1.WAV` first (real asm path). Per-frame cursor log noise suppressed (log only on frame_idx change). `overlay_bmap` field added to `menu_t`; freed by `menu_release_cursors`. Build clean, exit 143 confirmed. Smoke log: `menu: overlay loaded MENU_1.CIF -> 640x480`, `wav_load: .../wav/ANIMS/INVENT1.WAV -> 1ch/11025Hz/16bit/samples=26711`.
- **Slice B pass 3 cursor** (Sonnet, 2026-07): Animated mouse cursor sprite overlay landed. `CURS128.CIF` (65×35 pointer) + `GENICO_0..6.CIF` (7-frame animated icon) loaded in `menu_init`. `blit_bmap_masked_to_framebuffer` added (index-0 transparent). `bmap_t` extended with `palette`/`owns_palette` fields; `image_load_compressed` preserves per-image CIF palette. `game_init_load_cursor` exported. GENICO drawn at (item.x-20, item.y+2) next to hovered item; CURS128 drawn at (motion_x, motion_y). Build clean, exit 143 confirmed. Smoke log: `menu: cursor loaded CURS128.CIF -> 65x35`, `menu: GENICO frames loaded 7/7`, `menu: render cursor frame=N`.
- **Slice B pass 2** (Sonnet, 2026-07): Mouse click routing + action dispatch wired. `menu_dispatch` centralises click/keyboard resolution. `MENU_ACTION_QUIT` → `MENU_STATE_QUIT`, `MENU_ACTION_TRAILER` → `pending_trailer` → `ls_play_movie("HDISK:>EARTH>sc>EADKE.CRH")`. `LOAD`/`OPTIONS`/`NEW_GAME` log stubs. Keyboard nav (Up/Down/Return/Esc) keycodes wired in `macos.m` (keyCodes 126/125/36/53 → actions[38/40/13/27]). Mouse Y-flip applied (Cocoa bottom-origin → top-left framebuffer). Build clean, exit 143 confirmed, `menu: click` + `menu: hover` log lines present.
- **Slice A** (Sonnet, 2026-07): Real bmap → framebuffer blit landed. `ls_bit_move` (0x0047DC3C), `ls_box` (stub), `draw_image` (0x00453FA3), `blit_bmap_to_framebuffer`, `game_init_get_menu_backdrop`. After intro CRH, LOADING.GIF (512×64) now blits centred into the 640×480 front_buffer using the MENUGEN.GIF palette every frame. Confirmed via log: `blit_bmap_to_framebuffer: 512x64 src -> fb offset (64,208)`. Build clean, exit 143 on SIGTERM. ls_box: stub (1-px outline); lsi_remap_bmap (cross-format) and lsi_bit_move_alpha (mode==2) are TODO.
- **Task #15** (Sonnet): `select_language_version`, `image_load_compressed` (CIF wrapper w/ `.GIF → .CIF` ext fallback), `image_load_compressed_free`, `sub_415FB0` font finalize, `load_image_color_data`. Real bytes now hit `g_loading_bmap` (512×64), `g_menugen_palette`, `g_objets_gif` (256×512), `INTFANI3` (189×119), FONTE11 atlas (691×224). Confirmed via log.

## Next slices (priority order)

### ~~Slice A~~ — Real menu backdrop rendering ✅ DONE

**Landed 2026-07.** `ls_bit_move`, `draw_image`, `ls_box` (stub), `blit_bmap_to_framebuffer`, `game_init_get_menu_backdrop` all in `src/libsys/graphics/graphic.c` + `src/game_init.c`. LOADING.GIF blits every frame after intro. See SUBSYSTEMS.md Graphics section.

### ~~Slice B~~ — Main menu (`process_mainmenu_437EB7`) — pass 2 ✅ LANDED

**Pass 1 landed 2026-07.** Menu state machine skeleton ported. `menu_init`, `menu_update`, `menu_render`, `process_mainmenu` in `src/menu.c` + `src/menu.h`. Static menu items render as text overlay on MENUGEN.GIF backdrop (640×480). Hover state and keyboard nav (up/down/enter/esc) wired to `sys->actions[]`. QUIT state breaks the main loop.

**Pass 2 landed 2026-07.** Mouse click routing + action dispatch wired:
- `menu_dispatch()` centralises action routing for mouse and keyboard paths.
- `QUIT` → `MENU_STATE_QUIT` (main loop breaks).
- `TRAILER` → `pending_trailer=1` → `ls_play_movie("HDISK:>EARTH>sc>EADKE.CRH")` in `menu_render`.
- `NEW_GAME`, `LOAD`, `OPTIONS` → log stubs (`sub_4380F9`, `sub_437ADC`, `sub_438CA5` TODO).
- Keyboard: Up/Down/Return/Esc mapped in `macos.m` (keyCodes 126/125/36/53 → actions[38/40/13/27]).
- Mouse Y-flip: Cocoa bottom-origin → top-left framebuffer coordinates.
- `menu->pending_trailer` field added to `menu_t`.

**Pass 3 (landed 2026-07):**
- **LANDED (cursor subitem)**: Animated mouse cursor sprite overlay. `CURS128.CIF` (65×35) loaded in `menu_init` → stored in `menu->cursor_bmap`. `GENICO_0..6.CIF` (7 frames, 720×116 each) loaded → `menu->genico_frames[7]`. `blit_bmap_masked_to_framebuffer` added to `graphic.c` (index-0 transparent). `bmap_t` extended with `palette` + `owns_palette` fields; `image_load_compressed` preserves per-image CIF palette. `game_init_load_cursor` exported from `game_init.[ch]`.
- **LANDED (pass 3 refinement)**: Real menu image overlay + INVENT1.WAV path fix.
  - `MENU_<lang>.CIF` (640×480 full-screen) loaded in `menu_init` → `menu->overlay_bmap`. Language from `state->language_id` (default 1 = English UK). Blitted in `menu_render` with `blit_bmap_masked_to_framebuffer` (index-0 transparent) after MENUGEN backdrop. Plain text labels suppressed when overlay present — sprite IS the visual menu. Pixel-density scan logs button regions. Freed by `menu_release_cursors`.
  - `INVENT1.WAV` path fixed: `menu_load_click_sfx` now searches `wav/ANIMS/INVENT1.WAV` first (real asm path `"%sINVENT1.WAV"` at line 78082). Confirmed: `wav_load: .../wav/ANIMS/INVENT1.WAV -> 1ch/11025Hz/16bit/samples=26711`.
  - Per-frame verbose log suppressed: `menu: render cursor at (X,Y) frame=N` replaced with `menu: render cursor frame=N` emitted only on frame index change.
  - Smoke: exit 143 confirmed. Log: `menu: overlay loaded MENU_1.CIF -> 640x480`, `menu: click sfx using '.../wav/ANIMS/INVENT1.WAV'`.
**Pass 4 (landed 2026-07):**
- Real `var_4AE0[8]` hitboxes decoded from asm 77770..77834. `menu_item_t` gains `src_x`/`src_y` for GENICO sub-rect; `MAIN_MENU_ITEMS[6]` now uses actual `(dst_x, dst_y, w, h)` per button — hover regions align with the `MENU_<lang>.CIF` backdrop art.
- GENICO_0..6 reinterpreted: **per-language** icons, not animation frames. `menu_init` now loads a single `GENICO_<language_id>.CIF` sheet; render blits the per-item sub-rect `(src_x, src_y, w, h)` at `(dst_x, dst_y)` for the hovered item only. No more `tick/100 % 7` frame cycling.
- `blit_bmap_subrect_masked` helper added to `menu.c` (source sub-rect + index-0 mask). Existing whole-bmap variant couldn't crop; asm's `overlay_cursor_image → ls_bit_move` needs sub-rect copy. Byte packing matches `blit_bmap_to_framebuffer` (`B<<16 | G<<8 | R` for NSBitmapImageRep byte order).
- CURS128 cursor cropped per hover state (asm +2351/+237E): src (0,0)-(29,32) when over an item, (30,0)-(32,32) idle. Previously the whole 65×35 sheet blit as one garbled cursor.
- `menu->genico_frames[7]` replaced by single `menu->genico_bmap`; `MENU_GENICO_FRAME_COUNT` removed.
- Render log guards `hovered_item == -1` case.

**Pass 4b (landed 2026-07):**
- Intro-fade fill-in (asm `sub_4374B0` side effect only): `menu_init` now walks `MENU_<lang>.CIF` rows and promotes index-0 pixels after the first index-0 column to index 1. Sprite is then blitted opaque via `blit_bmap_to_framebuffer` (not masked). Result: MENU sprite covers MENUGEN fully in steady state, so the red border strip previously visible through transparent overlay regions is gone. Reveal-wave animation (`sub_4375DF`) still skipped; we render directly at fraction=1.0.

Still pending (pass 5):
- Intro reveal animation: `sub_4374B0` full row-scan (currently only side effect ported), `sub_4375DF` (0..350 ms tick, sine-driven wave), `sub_4378EA` (200 ms post-fade palette pulse). MENUPERS.GRD palette load into stack buffer (var_489C) — feeds palette cycling during the pulse.
- Save/load slot panel (SAVE.GIF, DKEGAME.CFG slot check, `sub_437ADC`).
- Options sub-menu (`sub_438CA5` — brightness/contrast/shadow sliders).
- CREDITS panel — entries 6/7 of `var_4AE0[8]` (bigger hitboxes, `var_4B40[2]` sub-table).

### Slice C — Level bootstrap (first playable frame) 🔄 pass 1 landed

**Pass 1 landed 2026-07.** `scene_load_level` (0046DFEC) skeleton ported in `src/scene.[ch]`.

- 25 logged phases mirror the asm control flow.
- Level folder path `DATAS>L%02d` built and existence-checked via `ls_resolve_path` + `ls_exists`.
- `L%02d.INF` header read attempted (non-fatal if missing).
- Globals set: `g_world_state=level_id`, `g_byte_4B7280=1`, `g_byte_511760=0`, `g_dword_505C54/4EDC30/34/38=0`, `state->current_level_id=level_id`.
- `player_entity` offset writes NULL-guarded (entity not yet exported from `game_init.c`).
- All heavy sub-calls stubbed: `sub_46D0EA`, `sub_46D305`, `set_display_mode`, `can_do_32k`, `sub_4640BC`, `process_color_palette`, `set_entities_combat_mode`, `render_entity_with_ui`, `apply_model_transform`, `update_player_interface`, `restore_display_mode`, `sub_40636D` + 11 others.
- Wired into `menu_dispatch` `MENU_ACTION_NEW_GAME` case → `scene_load_level(state->first_room)`.
- Build clean, exit 143 confirmed, `menu: starting new game (level=1)` + `scene: scene_load_level(1)` in log on NEW GAME click.

**Discovered blocker (2026-07)**: LIEU%02d.DLL are compiled x86 code, not data blobs. Each exports one `@FonctionEditor@16` (`__stdcall` 16-byte stack args) that runs level init. `.data` (~40 KB) holds identifier strings — dialog codes (`SG01DIA01`..`SG03DIA02`), action codes (`SP01ACT17`..`SP03ACT17`), LCOD (`LCOD666`..), SAR / SOB / character IDs. `.rdata` = 2 KB constants. `.text` = ~136 KB compiled code. Verified via `tools/src/dump_lieu.py --list LIEU01.DLL`.

Options to unlock LIEU execution:
- (a) Static analysis + hand-port each `FonctionEditor` to C.
- (b) Integrate x86 emulator (unicorn engine) — run LIEU exports natively.
- (c) Reconstruct level structure from `.data` strings + known engine API call graph.

**Pass 1b landed 2026-07 (first playable frame):**
- `scene_load_level` now loads `DATAS>L<lvl>>IMAGES>FL<lvl>C<cam>L0.CIF` (camera 0 by default) via `game_init_load_cursor` and stashes it in `g_scene_bg_bmap`. Each CIF ships its own 256-colour palette so the .GRD parse is deferred.
- New `scene_render(sys)` — blits the loaded backdrop centred into `state->graphic.front_buffer` via `blit_bmap_to_framebuffer`.
- New `scene_release()` — frees the backdrop bmap + palette. Called from `ls_main` on exit alongside `menu_release`.
- `ls_main`: when `menu.state == MENU_STATE_LOADING`, call `scene_render` instead of `menu_render`. NEW GAME click now flips the screen to the level's camera-0 room image rather than a "LOADING LEVEL N" text banner.
- No entities, no fade, no camera switch. `.INF` / `.OME` / `.ANI` still unread. First playable frame = static room image with cursor/HUD path still stubbed.

**Remaining (pass 2+):**

1. `tools/src/dump_lieu.py` — ✅ landed. Extracts sections + exports + strings + `.bin` files. See `docs/SUBSYSTEMS.md` Scene section.
2. Port `sub_46D0EA` — asset loader (DAT / IMAGES / PAL / ANIM2D / OME).
3. Port `sub_46D305` — world controller allocation.
4. Parse `datas/L%02d/L%02d.INF` binary → entity/geometry manifest.
5. Load `.OME` mesh files.
6. Palette load from `PAL/*.GRD` + `.MAP`.
7. Route one of (a)(b)(c) for LIEU script execution.
8. Wire DKE.CFG `FirstRoom` to select which LIEU to load.

Scope: 3-4 more Sonnet passes. LIEU execution route decision needed before deep progress.

### ~~Slice D~~ — Player entity + input routing — pass 1 ✅ LANDED

**Pass 1 landed 2026-07.** Player entity skeleton ported.

- `player_entity_init` (0046493B) — allocates 0xEA0 (3744) bytes via `calloc`, zero-inits, sets all default fields identified from asm (screen-clip rect, position mirrors, bounds, type flags, `current_game_time` at +0xD34, etc.). Nested sub-calls (setup_creature, sub_462BCC, process_scene_hierarchy_with_memory, etc.) stubbed per spec. Global `g_player_entity` populated. Source: `src/player.c`.
- `player_entity_init_secondary` (00464F6D) — secondary init: +0xCD8 dialogue buffer ptr, weapon slot read at [entity+0xACh]+0x97, weapon entity load chain (sub_4648DC, sub_475BB6, sub_472C5C, sub_473756, sub_41F6BA) all stubbed. Source: `src/player.c`.
- `player_input_update` — polls `sys->actions[119/115/97/100]` (WASD) → `velocity_z/x`; `sys->mouse_left==2` → `want_action=1`. Resets per call. Logs when velocity non-zero. Wired in `ls_main` MENU_STATE_LOADING block.
- `g_player_entity` moved from `game_init.c` static → `player.c` non-static; exposed via `player.h`. `game_init.c` phases 33/34 now call real functions.
- Build clean, exit 143 confirmed. Log: `player: player_entity_init: allocated 3744 bytes` + `game_init: sub_46493B player entity setup` + no crash.

**Remaining (pass 2+):**
- Port `setup_creature` / `sub_462BCC` — creature mesh/animation init (>100 asm lines each).
- Port weapon entity load chain: `sub_4648DC`, `sub_475BB6`, `sub_472C5C`, `sub_473756`.
- Wire +0xCD8 `byte_4F60E0` dialogue buffer once data segment is mapped.
- Wire weapon-slot table at [entity+0xACh] → `dword_4ECD50` once slot table is exported.
- `sub_45B207` — combat mode wire.

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
| DLL | level (LIEU01..LIEU55, INVENT, HOTEL) | asm + `tools/src/dump_lieu.py` | ⚠ blocker — LIEU DLLs are compiled x86 code (single export `@FonctionEditor@16`), not data blobs. Options: (a) rewrite each LIEU in C by static analysis; (b) integrate x86 emulator (unicorn); (c) reconstruct from strings + `.data` heuristics. `dump_lieu.py --list \| --all` extracts sections + exports + strings. |
| WAV | sfx (`bin/game/wav/`) | riff wav | ✅ ported |
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
