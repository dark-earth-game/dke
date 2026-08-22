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
| M7 | Entity / combat / render pipeline | 🔄 pass 1-5 landed (OME static mesh, MATR colours, 19-atom multi-render, CAN keyframe decode, OBJC translations, ANI env sprites) |
| M8 | Interactive gameplay | ⏳ deferred |

## In flight (background)

*(none — Slice B pass 4 landed: real hitboxes + GENICO single-sheet crop + cursor crop + palette fix. Intro fade / MENUPERS / save/options panels deferred to pass 5.)*

## Recently landed

- **Engine: character rig, animation channels, stray faces** (Opus, 2026-08):
  three separate defects behind "the actors look wrong".
  1. **Rig hierarchy.** `ome_parse_objc` took each bone's parent from the OBJC
     record's +0x78. That field is *runtime state*: `sub_4699D9` @ 0x4699D9
     walks the first-child/next-sibling links at +0x7C/+0x84 and WRITES the
     parent to +0x78 (and the previous sibling to +0x80). What the modelling
     tool left in the file is stale and disagrees with the real rig wherever a
     sibling chain starts — P00's 'hangau' (left hip) claimed 'piedro' (the
     RIGHT foot) as parent and 'epagau' (left shoulder) claimed 'poidro' (the
     right wrist), so the entire left side hung off the end of the right limb.
     Parents are now derived from the child/sibling links, as the engine does.
     Verified: left/right bones are siblings again with mirrored translations.
  2. **Animation channels.** A CAN keyframe is 34 channels in the engine's
     fixed bone order (the table `sub_41F6BA` @ 0x41F6BA matches names
     against). The port indexed keyframes by the bone's position in the OME
     file, a completely different order (P00 file: tree, basdos, handro,
     gendro...; channel order: tree, basdos, hangau, handro...), so every bone
     was driven by another bone's channel. `ome_objc_t` now carries
     `can_slot[]` and all three keyframe sites index through it.
  3. **Stray white geometry.** Faces with `mat_id == -1` ("no material") were
     painted with `MAT_COLOURS[0]`, a bright debug colour, and appeared as
     white shards stuck to the characters. They are now skipped.
  Also settled: the bone record's +0x20 is the ATOMF slot index, so bone ->
  atom mapping no longer relies on matching names.
  **Textures: none ship.** Every persona OME references a `Pnn.TGA`
  (P00.TGA, P12.TGA, P26.TGA...) but the only TGA in the game tree *or* inside
  data.z is FLAM.TGA. Character colour therefore comes from the MATR palette
  index plus the per-camera GRD ramp, which is what the port already does —
  missing data, not a port defect.
- **Engine: room transitions carry their own world** (Opus, 2026-08): walking
  to another room loaded the new backdrop but kept the PREVIOUS room's boundary
  polygon, collision zones and camera regions — L02 was building a nav grid
  byte-identical to L01's. `scene_load_level` phase 8 faithfully mirrors the
  asm's "cmp world_controller, 0 / jnz skip", but the port's CHANGEPIECE path
  never ran the engine's teardown (`sub_46E4A8` @ 0x46E4A8) that clears it.
  Now frees the world data on room change: L02 loads `dll=LIEU02` and builds
  its own grid (`cell=33013x16041 free=2164 wall=534 blocked=1247`) and the
  hall renders.
  That transition also **crashed** before reaching the new room, in
  `process_color_palette` walking entity+0x50 as a scene hierarchy. Same
  entity-blob overlap: `apply_model_transform` wrote i32 state at entity+0x54,
  which is the high half of the 64-bit pointer at +0x50, turning a NULL root
  into `0x0000000200000000`. Nothing reads +0x54, so both writes are dropped.
  Also corrected: `entity+0xCD8` is the per-entity 0x1000-byte **nav
  workspace** that `sub_45720D` flood fills, not the "dialogue buffer" it was
  named (the 0x3000 size was already right — three entity slots).
  Verified `sub_459CD2` @ 0x459CD2: the port's collision classification is
  faithful (point-in-polygon, flags&1 gate, flags&2 = wall).
- **Engine: facing tests + player heading** (Opus, 2026-08): `DirectionPerso`
  (`sub_44599B` @ 0x44599B) and `DirectionZone` (`sub_445C82` @ 0x445C82)
  decoded and ported — they were returning a blanket "yes". Shared maths:
  `target = -(atan2(dx, dz) * 65536 / 2pi)`, signed shortest arc, pass under
  0x2400 (+/- 50.6 deg); DirectionPerso also passes unconditionally within
  30000 units (dbl_4B0678). Note the atan2 argument order — the X delta goes
  first. `bam_angle` was computing it the other way round, which also affected
  JOINDRE's facing.
  The other half was plumbing: **nothing ever wrote the player's heading**, so
  `player_get_facing()` read a slot that was always 0 and every script query
  about the player's orientation was answered with heading 0. scene.c now
  mirrors the live heading into entity+0x1E0 (cosine-table units << 4 = BAM).
  DirectionSpec (`sub_445E99`) stays permissive — its target resolution is not
  decoded. Neither test is reached by L01's opening, so this is groundwork.
  **Found while verifying**: the opening conversation has been riding on an
  accident. It fires from zone 4's trigger, and the player only lands in zone 4
  because the seed came from the camera-anchor solve. trig00's PosPerso for P00
  puts Arkhan ~34000 units away on Z, outside every zone, where nothing fires
  at all. One of the two mappings is wrong; the anchor stays the default
  because it is the one that plays, and `DKE_SCRIPTSEED=1` selects the scripted
  placement. Also: scripted placements are no longer snapped to a walkable cell
  (Arkhan starts on the bed, which the grid calls furniture).
- **Engine: collision + character scale** (Opus, 2026-08): the nav grid
  `sub_45619D` @ 0x45619D built has been consulted by nothing since it landed —
  the player walked through walls. Ported the engine's grid queries
  (`sub_456FB7` @ 0x456FB7 snap-out-of-blocked, plus cell lookups) and wired
  them to the player input path, per axis so a diagonal into a wall slides.
  The player seed is now snapped onto a walkable cell. Grid axes documented:
  it spans world (X, Z), and the `sol_limit_t` field the port calls `min_y`
  actually holds Z — the room polygon's Y components are all ~0, so there is
  no floor height in the INF at all.
  **Character rendering fixed**: personas rendered as scattered triangles
  because the default vertex shift was 1 (half scale); at 0 the figures are
  coherent — Kaly now reads as a person asleep on the bed. The hand-tuned
  `-120000` ground drop was really the model's own root-to-feet distance along
  bone X (P00: X[-121123..34416]), so it is taken per persona from the
  skeleton instead of hardcoded; P12's feet are at -109645 and it had been
  sinking ~11k units into the floor. `DKE_MSHIFT` / `DKE_MYOFF` still override.
  Also recorded, not yet acted on: `entity+0xCD8` is the per-entity 0x1000-byte
  nav workspace flood-filled by `sub_45720D`, **not** a dialogue buffer, and
  `sub_469222` @ 0x469222 is the actor walk state machine JOINDRE hands off to.
- **LIEU action queue** (Opus, 2026-08): the deferred-action machinery every
  LIEU primitive actually targets. New `src/lieu_action.[ch]`: 3 piles x 100
  nodes x 0x78 (`sub_4441A5` @ 0x4441A5 alloc, `sub_4442EE` @ 0x4442EE pop,
  `sub_44CDB8` @ 0x44CDB8 per-frame drain of one head node per pile), plus the
  20-slot object-timer table from `sub_44968B` @ 0x4499A5 whose expiry fires
  FonctionEditor **opcode 4** — the path that drives every timed script beat
  and had no driver before. `lieu_action_dispatch` in `src/lieu_cb.c` ports the
  reachable half of the 40-case switch in `sub_44A58B` @ 0x44A58B. Ten
  previously log-only callbacks now build real nodes (ANIMATION, EVENEMENT,
  ETATOBJET, JOINDRE, BRUITAGE, ANIMOBJET, STOPJOINDRE, VIRECOL, JOINDREPERM,
  LINKOBJET, PLAY/FREEANIM), and SELECTPILE (`sub_446A05` @ 0x446A05) now does
  the real thing: pick the pile, set its enable flag, reset it on mode 2. Zone/
  camera/room-change/POSPERSO/dialogue/cutscene callbacks stay immediate on
  purpose (see docs/functions/lieu_action_queue.md — "Queued vs immediate").
  Verified headless: 46 actions dispatched over 400 frames of the L01 opening,
  dialogue and camera behaviour unchanged, `DKE_NOQUEUE=1` A/B switch added.
  **Coordinate calibration settled**: zone rects, PosPerso spawns, JOINDRE
  destinations, INF camera regions and the player transform are all script
  tile x600 — no LIEU-vs-world conversion needed on X/Z. `lieu_zone_debug` now
  prints all three together so drift is visible. Floor Y remains a per-camera
  constant.
- **JOINDRE decoded + script-driven animation** (Opus, 2026-08): case 8 of
  `sub_44A58B` @ 0x44B5E3 disassembled, settling the argument shape:
  `Joindre(actor, tileX, tileZ, facing, target)` where node+0x10 is a target
  actor (> 0), a zone reference `-(v)-1` (< 0) or nothing (0), and node+0x24 is
  a FACING angle from `sub_4440A6` @ 0x4440A6 — not a coordinate. Crucially the
  action **completes immediately** and the entity update walks the actor over
  later frames; the first pass held the node until arrival, which stalls every
  dialogue line behind a walk. `lieu_walk_update` now plays the movement
  system's part. ANIMATION names are CAN clip ids carrying their own persona
  ("p12arr03" -> P12), so they are published via `lieu_persona_clip` and
  `persona_apply_script_clip` swaps the persona's CAN over: the L01 opening now
  animates from the script (P12 lev01 -> arr03 -> arr01, P00 dia05/06/07)
  instead of looping a hardcoded arrival clip.
- **Shutdown crash fixed** (Opus, 2026-08): `ls_release` in
  `src/libsys/platforms/macos.m` called `free()` on the `NSWindow`, which
  aborted in libsystem_malloc ("pointer being freed was not allocated")
  whenever the autorelease-pool drain above it had not already reclaimed the
  window — intermittent SIGABRT on exit, ~1 run in 3 on longer sessions.
  `-close` already drops the window's own reference (releasedWhenClosed
  defaults YES), so neither `free()` nor `release` is correct: the pointer is
  simply cleared. Pre-existing and unrelated to LIEU (reproduced with
  `DKE_NOQUEUE=1`). Exit is now clean over 400/600/1000-frame runs and the
  menu-only path.
- **64-bit entity-blob pointer collisions** (Opus, 2026-08): three crashes on
  the new-game path, all the same root cause — the entity blob keeps the
  original's 32-bit field offsets while the port stores 8-byte host pointers in
  it, so a pointer at +N spans N..N+7 and the neighbouring 4-byte slot shreds
  its high half. Fixed: `player.c` read +0xAC through the 32-bit `ent_get`
  accessor (truncated pointer -> segfault in `player_entity_init_secondary`);
  `game_init.c` @ 0x46C484 stored a pointer at +0xB0 over the top half of the
  descriptor pointer at +0xAC (segfault in `hud_draw_health_bar`); and
  `apply_model_transform` set +0x54 before loading the scene root at +0x50,
  shredding it (segfault in scene phase 22). +0x8C is a 32-bit field consumed
  as a world Y, not a pointer — now read as one. **The blob still has no
  64-bit-safe layout; expect more of these** until it gets one.

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

**LIEU DLL analysis landed 2026-07 (docs/functions/lieu_dll.md)**:
- Engine loader: `sub_448F4D @ 0x448F4D`. Loads DLL via `LoadLibraryA`,
  looks up `@FonctionEditor@16` via `GetProcAddress`, calls with 4 stdcall
  args: `(opcode=1, state_buf=&dword_505C40, cb_table=&dword_4D6198,
  reload_flag)`. Result at `dword_4B53FC`.
- Callback table: 40 slots at `dword_4D6198..dword_4D6234`, each = pointer
  to engine scripting primitive (populated at asm 0x449259..0x4493E3).
  Primitives: NamedObject action, cinematic dispatch, KillPerso, dialogue
  spawn, plus ~35 unnamed API fns (per-object state / trigger / animation).
- DLL entry (LIEU01): `FonctionEditor@16` at RVA 0x1046 = JMP thunk →
  real code at 0x18C60. On first call: ingests all 40 callback ptrs into
  DLL-local globals (0x1002c*). Then dispatch table[opcode+1] at 0x100192F9
  jumps to per-opcode handler. Engine invokes with opcode=1 (init).
- Action struct: primitives write `dword_4F4CC0` (48 bytes) with action_id +
  string/int args, then call `sub_45D560` (queue) + `sub_4441A5` (alloc node).
- `.data` strings = ARGUMENTS to primitive calls (SG..DIA, SP..ACT, LCOD,
  SOB, Sar, L..B / L..D refs).

Options to unlock LIEU execution (see lieu_dll.md for detail):
- (a) Per-level static hand-port (~55 × 40 KB = 2 MB of asm to port).
- (b) Unicorn x86 emulator + native callback bridge (~1 week setup).
- **(c) LANDED 2026-07: One-time bytecode extraction via emulator →
  LIEU%02d.script tables → native engine replay in shipping build.**

**LIEU extraction landed 2026-07 (option (c) prototype):**
- `tools/src/lieu_extract.py` — unicorn-based extractor. Maps DLL at
  ImageBase, wires 40 trampolines with UC_HOOK_CODE traps, calls
  FonctionEditor(opcode=1) with stubbed callback table. Records slot
  + ecx/edx/stack args per callback, simulates stdcall pop from
  CB_POPS table (scanned from dkev.exe).
- 54 / 55 DLLs extract cleanly (LIEU55 has malformed export table).
- 2002 total calls across all levels, avg 37/level.
- Only 19 of 40 callback slots invoked during init. Top 3: slot 0x30
  (722 = object spawn?), 0x08 NamedObjectTwoInts (572), 0x78 (210).
- 172 unique string args (SG..DIA, SP..ACT, LCOD, SOB, L%02dAMB1,
  P%02dARR01, etc.). Deterministic across runs.
- Scripts written to `dke/bin/game/lieu/LIEU%02d.script` (55 files, 248 KB).

**M6 pass 1 landed 2026-07 (native replay skeleton):**
- `src/lieu.[ch]` — parses `.script` text, dispatches by slot to
  log-only C stubs. LIEU_MAX_RECORDS=256. Arena for interned strings.
- 19 identified slots wired: NamedObject (0x00), NamedObjectFlag (0x04),
  NamedObjectTwoInts (0x08), Cinematique (0x0C), KillPerso (0x10),
  NamedObjectStrPair (0x14), NamedObjectStr (0x18), + 12 generic CB_XX.
- Called from `scene_load_level` after `scene_load_camera_anchor`.
  `scene_release` frees the arena.
- Scripts moved to `bin/game/lieu/` (gitignore exception for
  `bin/game/lieu/`) so `state->game_path` resolution finds them.
- Build clean.

**Scope clarification (2026-07):** LIEU DLL has 7 opcodes:
- opcode 1 (INIT) — verified state-independent (patched extractor, 
  eax=0 vs eax=1 = same trace). Option (c) fully covers this.
- opcode 0 (per-frame tick) — reads state, state-conditional branches.
- opcode 3 (event dispatch) — calls handlers from table populated in INIT.
- opcodes 4/5/6 — internal LIEU subroutine calls from engine.

Option (c) covers INIT only. Gameplay-time opcodes still need either:
- Runtime unicorn emulator embedded in engine (option b hybrid), OR
- Hand-port of opcode 0/3-6 handlers per level (~2 MB asm).

**M6 pass 2 (next):** Progressively wire INIT slots to real engine effects.
- Slot 0x30 (722 calls, "object spawn?"): first candidate — signature
  looks like (id, param, x, y, z, flag). Wire to entity spawn API.
- Slot 0x08 NamedObjectTwoInts (572): script trigger table registration.
- Slot 0x2C L%02dAMB1: ambient audio queue.
- Slot 0x0C Cinematique: `ls_play_movie` call.
- Slot 0x10 KillPerso: entity kill.

Each wiring pass: identify engine primitive from sub_44XXXX handler in
`docs/functions/lieu_dll.md`, port to native C effect, verify via
smoke test (New Game → level renders with expected state).

**M6 pass 3+ (deferred):** Runtime opcode emulation. Embed lightweight
unicorn instance in dke, hook 40 callbacks to native engine primitives,
invoke opcodes 0/3-6 as the game demands. Cross-platform via unicorn
(LGPL, Linux/macOS/Windows/x86+ARM hosts). Alternative: extract per-opcode
decision trees via symbolic execution — larger data payload but no
runtime x86 dependency.

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

**Pass 1+2 landed 2026-07 (placeholder cube):**
- `src/model.[ch]` — model_t (verts + faces), model_test_cube() as a
  stand-in until .OME loads.
- Software rasteriser: scanline triangle fill with linear depth
  interpolation. Backface cull uses cross_z sign (front face is CCW
  in screen because we flip Y in project).
- Per-pixel `plot_pixel` samples the scene L bmap (nearest ×2
  upscale) and modulates face colour: `out.c = clamp((c * L) >> 7)`.
- Perspective projection: camera at (0,0,+4·ONE) looking down -Z,
  focal_len = fb_w, ~26° pitch tilt.

**Pass 3+4 landed 2026-07 (OME static mesh + MATR colours):**
- `src/model_ome.[ch]` — `ome_decode_atomf` (greedy ATOMF decoder,
  verified on 227 files): vertex records discriminated by field[7] flag
  (0 or 0x200000); face records 96 bytes each with 4-byte heap-ptr
  separator. `ome_parse_matr` decodes MATR body at sig+0x240 (72-byte
  records, R/G/B at offsets 0/4/8). Both verified against P00.OME +
  OB98.OME.
- `model_from_ome_atom(atom, matr)` builds render-ready model_t: quads
  split to 2 triangles, vertices scaled ×2 (Q16.16), face colours from
  MATR table or fallback palette.
- `scene.c` loads all 19 geometry ATOMFs from PERSOS/P00.OME; each atom
  renders separately (model_face_t uses u8 indices, combined vert count
  >255 prevents merging).

**Pass 5 landed 2026-07 (CAN animation + OBJC translations):**
- `src/can.[ch]` — CAN file loader: ptr table (640×8-byte entries,
  hash+offset), animation blocks (0x110-byte header + n_frames×0xCE
  keyframes). Keyframe format decoded via IDA (sub_415021): 2-byte
  header + 34 atom slots × 6 bytes (u16 rx,ry,rz each). Angle encoding:
  u16 0..65535 = 0..360°, convert to cosine_table index via >>4.
- `model_render` extended with per-atom override angles (rx/ry/rz) and
  character-relative translation (atom_tx/ty/tz). Camera rotation is
  applied to the translation vector before perspective projection.
- OBJC hierarchy decoded via IDA (sub_4228E0) + binary parse of P00.OME:
  29 Object structs (each 0xB4 bytes at sig+0x30), translations at
  struct[0x3C..0x44], h2[0] field (struct[0x78]) = upward hierarchy link.
  Character model lies along X axis: head (+44000) → spine → feet
  (-121000). 26° pitch projects X → vertical on screen.
  20 atom translations hardcoded in `G_P00_TRANS[20][3]` in scene.c.
- `src/anim2d.[ch]` — ANI 2D sprite loader: format decoded via IDA
  (sub_41333A, sub_4780B4). Per-frame bounding box (x1,y1,x2,y2,
  uncomp_size, comp_size) + LZSS-compressed pixel data. Footer: 12
  bytes at EOF (direction, loop, timing, visible_frames, format_type,
  n_frames). ANI sprites are environmental overlays (fire, levers etc.),
  NOT characters — characters are real-time 3D.
- scene.c loads P00.CAN (walk anim "p00mar14", 14 frames), cycles frames
  at 10 fps, feeds per-atom rx/ry to model_render for each of 19 atoms.

**Architecture clarified (IDA + user correction):**
- Characters: real-time 3D (OME mesh + CAN pose + camera projection).
- Background: pre-rendered CIF images.
- ANI sprites: small environmental animations layered over background.

**Pass 6a landed 2026-07 (OBJC hierarchy + CAN slot→atom mapping):**
- `ome_parse_objc()` added to `model_ome.[ch]`: decodes OBJC chunk body
  (29 bones × 0xB4 bytes each; header[0x10]=n_bones, header[0x20]=n_atomslots;
  bone record: +0x04=name, +0x3C=tx, +0x40=ty, +0x44=tz, +0x78=h2).
  Builds `can_to_render[j]` (CAN slot j → render atom index by name match)
  and `world_tx/ty/tz[i]` (accumulated translation from bone up parent chain,
  excluding root "tree" bone). Verified: slot[0] basdos→(222,100,22),
  slot[18] tete→(32444,1589,155) match prior G_P00_TRANS values exactly.
- `scene.c`: G_P00_TRANS hardcoded table removed; per-atom world position
  now from `g_scene_objc.world_tx/ty/tz[]`. CAN slot→atom loop now iterates
  j=0..n_bones using `can_to_render[j]` to drive the correct render atom;
  bones without geometry (tree, ddpdro, talgau, etc.) silently skipped.

**Pass 6c landed 2026-07 (camera pitch from INF):**
- `scene_load_inf_pitch()` parses `L%02d.INF` binary (format: magic(4)+
  ts(4)+world_ctrl(0x208)+n_cam(4)+n_cam×0x208+n_inex(4)+n_inex×0x208+
  n_stairs(4)+n_stairs×0x208+n_extra(4)+n_extra×0xA8 extra section).
  Extra section entry cam_idx contains 120-byte projection parameter table;
  param[5]=R[1][2]=sin(camera_pitch) Q14 (16384=1.0). Converts to
  cosine_table index via asinf. `model_default_pitch` global in model.c
  (extern in model.h) replaces literal 300. Reset to 300 in scene_release.
- L01 camera 0: param[5]=2187 → ~7.7° pitch (was 26°).

**Pass 6d landed 2026-07 (per-frame bone hierarchy propagation):**
- `scene_update_bone_positions(kf)` added to `scene.c`. Each CAN frame:
  iterates bones 0..n_bones-1 (topologically sorted in P00.OME — parents
  always have lower index than children). For each bone j: reads rx_idx/ry_idx
  from kf->atoms[j] (or 0 if no CAN), builds Ry×Rx rotation matrix via
  cosine_table, accumulates parent_rot→child_rot via mat3_mul, rotates
  local translation by parent_accum_rot via mat3_apply, adds to parent world
  pos. Writes final world pos to g_scene_objc.world_tx/ty/tz[ai].
  Called in scene_render() after CAN frame advance, before atom loop.
  T-pose verify: CAN angles=0 → identity rot → pure translation accumulation
  → matches static ome_parse_objc output (basdos=(222,100,22), tete=(32444,
  1589,155) etc.) ✓. Arms/legs now swing through world space during walk cycle.
- Helpers: scene_mat3_t (Q14 3×3), scene_mat3_from_rx_ry, scene_mat3_mul,
  scene_mat3_apply. All static in scene.c, no new headers.

**Pass 6e landed 2026-07 (per-camera screen anchor from INF):**
- `scene_load_camera_anchor(level_id, cam_idx)` added to scene.c.
  Parses INF extra section (same format as scene_load_inf_pitch):
  reads param[0..11] (R 3×3 Q14 + T raw), reads cam zone polygon
  (at cam_struct[+0x1D8]=n_verts, [+0x1DC..] = vertex (x,y,z) × 12 bytes
  each; filters garbage, computes centroid X,Z).
  Solves 2×2 system to find world (wx, wy, zone_z) projecting to
  screen (320, 350): T_view = (R×T)>>14; rhs_x = -T_view[0]*16384 - R[2]*zone_z;
  rhs_y = (target_vy - T_view[1])*16384 - R[5]*zone_z; det = R[0]*R[4]-R[1]*R[3];
  wx = (rhs_x*R[4] - rhs_y*R[1])/det; wy = (rhs_y*R[0]-rhs_x*R[3])/det.
  Degenerate cameras (|det|<1000, ≈90° rotation) use zone_x + solved wy.
  Projects back to screen, clamps to [10..629]×[10..469], sets
  g_scene_cam_sx/g_scene_cam_sy. Replaces fb_w/2, fb_h/2 anchor.
  Verified: cam0 → zone=(305083,-1802945) → world=(301k,237k,-1803k)
  → screen=(320,350). L01 lights (lighty=30k-80k) use model-space coords,
  INF world Y ≈ 170k-240k (different coordinate system — deferred).

**Remaining (pass 7+):**
- Correct character world spawn position (from LIEU DLL or static analysis)
  to replace the zone-centroid estimate. Anchor currently approximate.
- Texture loading from MAT/TGA files (face UV at face[0x3C..0x5B];
  sub_422E31 does texture atlas offset fixup at load). P00 uses flat
  shading (no TGA in persos/ — MATR colours ARE the visual).
- Z-bmap occlusion tuning: world_size=60 approximate for OME scale.
- Menu pass 5: save/load panel, options, CREDITS, intro reveal animation.

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
| CAN | anim (`bin/game/anm/P*.CAN`) | asm sub_41DE86, sub_415021 | 🔄 partial — ptr table + keyframe decoded; CAN slot→atom name mapping pending |
| ANI | 2D env sprites (`DATAS/L*/ANIM2D/N*.ANI`) | asm sub_41333A, sub_4780B4 | ✅ ported — LZSS + bounding box per frame |
| OME | 3D character mesh | asm sub_4228E0, sub_422E31 | 🔄 partial — ATOMF + MATR decoded; OBJC full hierarchy pending |
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
- IDA MCP server: available as `mcp__ida-mcp__*` tools in Claude Code sessions
  for live decompile/disasm/xref queries against the open IDA database.
- Full asm dump: `disassembly/dkev.asm` (~970k lines, 1404 procs).
- Python tooling: `tools/src/*.py` (CRH, CIF, PCX, CTX, OME dumpers).
- Original executable: `disassembly/dkev.exe` (patched no-CD).

## When to update this doc

- After each Sonnet delegation completes: mark milestone / slice status.
- When a slice priority changes: re-order the "Next slices" section.
- When a new format decoder lands: update the backlog table.

Do NOT let this doc replace `FUNCTION_MAP.md`. Address-level detail belongs there.
