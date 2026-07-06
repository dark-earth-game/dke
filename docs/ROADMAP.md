# Roadmap

Goal: playable game (or at least: launch → intro cutscenes → main menu).

## Milestone 0 — foundations (in progress)

- [x] Window/blit loop (macOS Cocoa via `libsys/platforms/macos.m`)
- [x] Config loader stub
- [ ] **Fix `load_configuration` parser bugs** (section tracking + key/value mixup)
- [ ] Memory manager port (`ls_init_memory_manager`, `allocate_memory`, `free_memory`)
- [ ] Filesystem: verify `HDISK:>EARTH>`-style path translation

## Milestone 1 — subsystem stubs

Get every `ls_startup` call site out of comments and into stubs that at minimum log + return success. Order:

1. `lsys_init_mouse_system`
2. `ls_init_sound_system` / `ls_init_sound_stream`
3. `lsys_init_cd_system` (no-CD build — likely nop)
4. `lsys_init_joy_system` (nop until needed)
5. `ls_set_video_mode` + `ls_init_bmap_ex`
6. `preload_shading_table` (currently empty)

## Milestone 2 — intro screens (TODO.md target)

- `ls_init_movie` chain: `init_my_memory` → `seb_get_memory`
- `ls_play_movie`: `init_decompress` → `read_and_init_sbt` → `decompress_anim` → `cine_clear_movie_background`
- `ls_shut_movie` + `seb_release_all`
- Wire into `ls_main` after `init_all_systems`.

## Milestone 3 — game init

- `game_init_46BEEB` (the big function): font, HUD, weapons inventory, entities.
- Font system: `init_font`, `init_font_system`.
- HUD: `initialize_health_hud`, `display_weapons_inventory`.

## Milestone 4 — game loop content

- Level loading, entity spawn, combat pipeline (`process_entity_damage`, `process_combat_block`, ...).
- Rendering: `graphics_triangle_rasteriser`, `create_model_render_data`, `render_entity_with_ui`.

## Non-goals (for now)

- Networking (none).
- Original DirectDraw path — using platform blit instead.
- CD music — patched no-CD build, defer.

## Delegation policy

- **Opus** (plan): scoping, architecture, cross-file refactors, ambiguity.
- **Sonnet** (impl): port a named function chain from asm to C, wiring, docs updates.
- **Haiku** (low effort): typo fixes, small parser bugs, comment normalization, single-function stubs.
