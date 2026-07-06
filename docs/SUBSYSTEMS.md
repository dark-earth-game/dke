# Subsystems

Notes per libsys/game subsystem. Detail intent, key entry points, and what's ported.

## Memory manager

- Original: `ls_init_memory_manager`, `allocate_memory` (called all over the game), `free_memory`, `register_game_image_buffers`.
- Custom pool + bookkeeping in original (visible in asm — `allocate_memory` XREFs to `process_scene_hierarchy_control_points+8AE`, etc.).
- **Port strategy**: wrap `malloc`/`calloc`/`free` behind `memory_alloc`/`memory_calloc`/`memory_free` (already exist in `libsys/common.c`). Add tracking (`total_memory_used`) matching what `MEM_DEBUG` expects.
- Movie playback uses its own `seb_*` sub-allocator — port separately with the movie code.

## Filesystem

- Original: uses virtual paths like `HDISK:>EARTH>`. See `ls_main+21` (`strcpy(game_path, "HDISK:>EARTH>")`).
- Port: `ls_get_base_path`, `ls_exists`, `ls_open`, `ls_close` — see `libsys/file_system.[ch]`.
- **Missing**: path translation `HDISK:>EARTH>foo` → `<basepath>/foo`. Add when actual assets are loaded.

## Palette

- `palette_copy_alternative` (00491450) — copies `alternative_palette_data[136]` → `palette_target_buffer[136]` (34 dwords, `rep movsd ecx=0x22`). Source and destination globals are zero-initialised at startup; the original data segment values are not yet recovered. Called from `ls_main` in the same sequence as the original. Source: `src/libsys/graphics/palette.c/.h`.
- `palette_postprocess_configure` (00425D50) — loads persona/inventory slot data into `word_509B40[0x310]` from `PERSOS>INVENT.CTX` (cached binary) or `PERSOS>INVENT.TXT` (parsed text, 98 entries × 8 i16 slots). Writes CTX cache after parsing. Status: `partial` (`ls_switch_memory`, `ls_load_file_at`, `ls_get_filedate` stubbed). Source: `src/libsys/graphics/palette.c/.h`.
- `color_table[0x300]` — 256-entry RGB palette table written by game_init phase 7 (MENUGEN.GIF palette); read by `configure_game_palette` / rendering paths. Ptr stored at `[bmap+18h]` by `initialize_resource_slots`.
- `word_509B40[0x310]` — 98-persona × 8-slot inventory table; populated by `palette_postprocess_configure`.
- TODO: identify what populates `alternative_palette_data` in the original game (likely a palette-load path that precedes the call sequence).

## Graphics

- Present: `graphic_t` with back/front/frame buffers, `ls_init_graphic_system`, `ls_graphic_flip`.
- Original: `graphics_init`, `graphics_triangle_rasteriser`, `apply_palette_transformation`, `configure_game_palette`, `initialize_color_mapping_tables` (for 16-bit mode).
- Two colour modes: `colour_mode = 0` → 8-bit palette, `colour_mode = 1` → 16-bit hicolor. `main.c` forces 16-bit.
- **M5 ported**: `initialize_color_mapping_tables` (00407D49) → `src/libsys/graphics/colour_tables.c`. Fills `primary_color_table[32×32]` and `secondary_color_table[33×32]` with Bresenham shading LUTs. Sets `color_transform_function` to `transform_color_6bit` (565 RGB, since `colour_mode != 4`). Both transform functions (`lookup_color_mapping` for 555, `transform_color_6bit` for 565) ported. Called from `init_all_systems` when `state->colour_mode` is set.
- **M1 ported**: `ls_init_bmap_ex` (0x0047CEBE) → `src/libsys/graphics/graphic.c` — allocates `bmap_t` (0x34-byte header + pixel buffer); surface-backed paths stubbed to raw alloc (status: `partial`). `ls_set_video_mode` (0x0048418A) → same file — tears down and recreates `g_graphics_context` + `g_back_screen`; DirectDraw mode-set skipped (status: `partial`). New types: `bmap_t`, globals `g_graphics_context`, `g_back_screen`, `g_bmap_count`.
- **Slice A landed (2026-07)**: Bit-blit primitive pipeline now live.
  - `ls_bit_move` (0x0047DC3C) → `src/libsys/graphics/graphic.c`: rect blit src→dst bmap with source+dest clipping. Mode 0 = opaque `memcpy` row-by-row; mode 1 = colour-key (skip zero pixels). Mode 2 (alpha) and cross-format `lsi_remap_bmap` path are TODO stubs.
  - `ls_box` (0x0047D466) → same file: debug outline rectangle (1-pixel border, colour byte 0xFF). Original ls_fill_rect/ls_invert_rect sub-paths stubbed; sufficient for current debug use.
  - `draw_image` (0x00453FA3) → same file: thin wrapper — optional ls_box outline (TODO: wire debug_surface global), then ls_bit_move mode=0. Logs every invocation.
  - `blit_bmap_to_framebuffer` (new, no asm addr) → same file: expand 8bpp indexed bmap → u32 XRGB front_buffer using 768-byte palette lookup. Clips to fb bounds.
  - `game_init_get_menu_backdrop` (new) → `src/game_init.c`: accessor returning g_loading_bmap + color_table to ls_main.
  - `ls_main` wired: after intro CRH, fetches LOADING.GIF (512×64) + MENUGEN palette, calls `blit_bmap_to_framebuffer` centred in the 640×480 front_buffer every main-loop iteration. Confirmed firing via log (`blit_bmap_to_framebuffer: 512x64 src -> fb offset (64,208) copy 512x64`).

## CIF image format

- Format: proprietary compressed still-image format used for font atlases (`.CAR` + `.CIF` pairs) and backdrop sprites (`biganims/`, menus).
- File layout: 16-byte header (version, signature, width, height) + 768-byte RGB palette + LZW-compressed pixel data (9-bit initial code size, LSB-first bitstream, clear=256, end=257, grows to 12 bits).
- Pixel data: 8-bit palette indices (one byte per pixel, row-major top-left origin).
- Source: `src/libsys/graphics/cif.c/.h`. Ported from `tools/src/dump_cif.py`.
- **Public API**: `cif_load` / `cif_free` / `cif_to_xrgb`.
- **Ported (M5)**: full LZW decoder, header parser, palette reader. Verified on `biganims/DIAB_0.CIF` (350×112) and `DATAS/MENUS/FONTE11.CIF` (691×224).
- **Font atlas wire-up**: `init_font()` calls `cif_load()` for FONTE11.CIF; on success sets `font_t.atlas_pixels/width/height` and disables CP437 fallback. Draw-text atlas pixel blit is next TODO.
- See `docs/functions/cif_decoder.md` for format spec and port mapping.

## Audio

- Present: `audio_t { sample_rate, channels, bit_depth }`, `ls_init_audio` at 22050Hz stereo 16-bit.
- Original: `ls_init_sound_system`, `ls_init_sound_stream`, `lsys_set_sound_format`.
- Music vs samples split (see `state_t.samples_enabled` / `music_enabled`).
- **M1 ported**: `ls_init_sound_system` (0x0047FDF8) → `src/libsys/sound.c`; inner `lsys_init_sound_system` (0x0048502B) stubbed (DirectSound absent). Status: `stub`.
- **M4 ported**: Real streaming API added:
  - `ls_audio_push(audio, pcm, bytes)` — forwards i16 LE PCM to platform backend.
  - `ls_audio_stop_stream(audio)` — drain/restart.
  - Platform backend contract: `ls_platform_audio_open/push/close` — implemented in each platform file.
  - **macOS** (`platforms/macos.m`): CoreAudio `AudioQueueNewOutput` backend. Linear PCM i16 LE, 4 buffers × 4096 bytes (~23 ms each), pre-filled with silence to avoid initial underrun. Buffer recycling via `pthread_mutex_t` + `pthread_cond_t` free-list; push waits ≤2 ms for a free buffer, drops block if none available. Linked via `-framework AudioToolbox` (Darwin-only in Makefile).
  - **Linux** (`platforms/linux.c`): stub — returns success/passthrough; ALSA deferred.
  - **Windows** (`platforms/windows.c`): stub — returns success/passthrough; WASAPI deferred.
  - `state->audio_enabled` defaults to `true` (set in `state_init`).

## Movie playback (SEB codec)

- Chain: `ls_init_movie` → `init_my_memory` → `seb_get_memory` → `ls_play_movie` (`init_decompress` → `read_entete` → `read_and_init_sbt` → `decompress_anim`) → `cine_clear_movie_background` → `ls_shut_movie` → `seb_release_all`.
- Callback: `ls_movie_callback`, `cp_restore_screen` (installed as thread `StartAddress`).
- Needed for TODO: "Play the intro cutscenes".
- **M2 ported**: `ls_init_movie` / `ls_shut_movie` / `seb_get_memory` / `seb_release_all` / `init_my_memory` → `src/libsys/movie.c`. Wired into `ls_main` (init before loop, shut after). Colour tables + all seb slot allocations happen with faithful sizes. **Pending**: `cp_init`/`cp_uninit`, `ls_get_heap`/`ls_release_heap` (movie uses `malloc(0x9EB10)` for now), and the audio-flip callback slots (recorded as opaque IDs).
- **M3 ported**: CRH decoder landed in `src/libsys/movie_decoder.c/.h` via direct port of `tools/src/CRH_to_MP4.py`:
  - `crh_open` — parses 52-byte header (version, sample_rate, channels, frame_count, fps) + runs `detect_dims` on first frame's tile data block, auto-selects `field-rev` (LSB) or `byte-rev` (MSB) tile bitstream mode.
  - `crh_next_frame` — decodes one frame: skip A_BYTES audio (TODO: stream), read 16-byte video header, read+decode 3 compressed planes via `decode_component` (port of `dec_comp_framepred`; LSB bitstream, flag=0 RLE fill, flag=1 back-ref with src-select for intra/inter prediction), reconstruct RGB via `reconstruct_rgb` (port of Python `reconstruct_rgb`; tile grid, per-tile 2-bit mode + 5-bit lookup tables, `five8` pixel expand).
  - `crh_close` — releases file handle and prev_plane buffers.
  - `ls_play_movie` wired: path resolved (`<game_path>/sc/<name>` for bare filenames), frame loop blit+flip per frame to `state->graphic.front_buffer` as `0xFF000000|R<<16|G<<8|B`, `ls_update_events` quit check, `ls_tick`/`ls_delay` frame timer.
  - **Audio wired (M4)**: `crh_decode_audio` ported from Python `decode_audio_block`. `crh_next_frame_ex` added — reads raw `CRH_A_BYTES` block into caller buffer instead of fseek-skipping. `ls_play_movie` allocates per-frame audio scratch, calls `crh_next_frame_ex`, decodes PCM, and pushes to `ls_audio_push`. Audio skipped when `state->audio_enabled` is false.
  - See `docs/functions/crh_decoder.md` for video format spec and port mapping.
  - See `docs/functions/audio_streaming.md` for audio pipeline overview.

## Input

- Present: `system_t.actions[256]`, `motion_x/y`. Platform layer fills these.
- Original: `lsys_init_mouse_system`, `lsys_init_joy_system`, keyboard via WndProc.
- **M1 ported**: `lsys_init_mouse_system` (0x00487BB0) → `src/libsys/mouse.c` (status: `stub` — sets `g_mouse_enabled=1`; platform layer delivers all input). `lsys_init_joy_system` (0x004811A0) → `src/libsys/joy.c` (status: `stub` — winmm skipped).

## CD

- Original: `lsys_init_cd_system`. Patched no-CD build → stub to success.
- **M1 ported**: `lsys_init_cd_system` (0x00487770) → `src/libsys/cd.c` (status: `stub` — sets `g_cd_present=0`, returns 0 matching original MCI-fail path).

## LIBSYS startup flags

- `ls_libsys_flag_reset` (00487D40) — resets `g_ls_pause_state` (word_4C0B68) to 0. Called from `ls_startup` after the memory manager.
- `ls_libsys_init_flags` (00487E30) — reads `ls_master_enabled`, `ls_debug_enabled`, `ls_info_enabled` from `state_t` (populated by `load_configuration` from `win.ini [LIBSYS]`); derives `g_ls_lang_id` from POSIX `LANG` env-var (Unix) or `GetSystemDefaultLangID` (Windows). Sets `g_ls_master`, `g_ls_debug`, `g_ls_info`, `g_ls_lang_id` globals; stores result in `state->ls_lang_id`. Calls `lsi_open_stat_app` in non-master branch.
- `lsi_open_stat_app` (~487F10, static) — ported from Win32 original that wrote to `f:\TEMP\LS1.CFG`. Port opens `<game_path>/dke_stat.log` for append and writes a `%d/%m/%Y : %H:%M:%S` timestamped startup line. Non-fatal on open failure (mirrors original `loc_4880AC` early-exit path).
- `ls_measure_cpu_speed` (004843F0) — fills `cpu_info_t` with `perf_counter_freq=1000000000` (CLOCK_MONOTONIC nanoseconds), `cpu_speed_mhz` from `sysctlbyname("hw.cpufrequency")` on macOS / `/proc/cpuinfo` on Linux / fallback 2000 MHz, `flags=0`. Replaces Win32 `QueryPerformanceFrequency + rdtsc` calibration loop. Called from `ls_main` before `ls_set_title`. Verified: detects host CPU at 2900 MHz on macOS.
- `ls_install_default_error_handlers` (00495390) — installs `g_error_handler_1` / `g_error_handler_2` function pointers pointing at two no-op stubs (`ls_default_handler_a`, `ls_default_handler_b`), mirroring the original `sub_494560` / `sub_4945F0` targets. Called from `ls_main` after `palette_copy_alternative`.
- `ls_copy_unknown_a_to_b` (004953B0) — copies `unknown_data_a[136]` → `unknown_data_b[136]` (34 dwords, matching `rep movsd ecx=0x22`). Mirrors `off_4C7748` → `off_4C77D0` from original BSS. Buffer purpose is TODO. Called from `ls_main` after `ls_measure_cpu_speed`.

## Filesystem

- Original: uses virtual paths like `HDISK:>EARTH>`. See `ls_main+21` (`strcpy(game_path, "HDISK:>EARTH>")`).
- `ls_build_virtual_root` (00485840) — builds a virtual root string (`g_virtual_root`) from `g_ls_drive_type` and `g_ls_drive_num`. Drive-type table: 1/2→FLOPPY, 10/20→HDISK, 30→REMOTE_SYS, 31→REMOTE_DEV2, 32→REMOTE_TMP, 33→REMOTE_TOOLS, 34→REMOTE_DEV1, 35→REMOTE_USERS. Defaults: type=10 (HDISK), num=0 → `"HDISK0:"`. Also zero-inits `g_ls_workspace_a` (10880 bytes) and `g_ls_workspace_b` (272 bytes). `sub_48C5D0` TODO.
- Port: `ls_get_base_path`, `ls_exists`, `ls_open`, `ls_close` — see `libsys/file_system.[ch]`.
- **Missing**: path translation `HDISK:>EARTH>foo` → `<basepath>/foo`. Add when actual assets are loaded.

## Fonts / text

- Entry points: `init_font_system`, `init_font`, `set_position_coordinates`, `draw_text`, `draw_text_at`, `read_line_file`.
- Source: `src/libsys/graphics/font.c/.h`.
- Original font format: `.CAR` glyph atlas index (255 × 3×i32: `atlas_x`, `atlas_y`, `glyph_width`) + trailing `i32` band_height; paired `.CIF` compressed atlas bitmap.
  - Game fonts: `datas/menus/FONTE11.CAR` + `FONTE11.CIF`, `FONTE10.CAR`.
  - System font loaded by logical name `"SYSFONT"` → resolves to `FONTE11`.
  - Non-SYSFONT fonts: `.FNT` extension, loaded via `sub_482200` (not yet ported).
- **Port status**:
  - `init_font_system` (~405DAB): `partial` — allocates font_t, calls `init_font("SYSFONT")`, wires `font_handle` and `g_default_font`. Called from `init_all_systems` after colour table init.
  - `init_font` (~430BEA): `partial` — reads `.CAR` index table into `font_glyph_entry_t[]`; `.CIF` atlas decode (`sub_482200` / `sub_4822E0` / `sub_48B660`) is **TODO** — those functions involve a proprietary compressed image format.
  - `set_position_coordinates` (~405DC7): `ported` — writes `cursor_x/y_position` + `target_x/y_position` globals (faithful to asm).
  - `draw_text` (~405EDB): `ported` — checks `font_handle`, delegates to `draw_text_at`. Atlas-based path has a TODO stub; renders via fallback.
  - `draw_text_at`: `ported` — blits to `state->graphic.front_buffer` (u32 XRGB). Atlas path stubbed until CIF decode lands.
  - `read_line_file` (~430D09): `ported` — byte-by-byte newline-terminated line reader from FILE*; used by `read_and_init_sbt`.
- **Fallback strategy**: Embedded 8×8 CP437 bitmap (95 glyphs, ASCII 0x20..0x7E, 760 bytes) is used whenever the CIF atlas is unavailable. `draw_text("...")` always produces visible white pixels on `front_buffer`. `init_font_system` returns `true` and `font_handle` is non-NULL even in fallback mode.
- **Wire-up**: `init_all_systems` calls `init_font_system()` after `initialize_color_mapping_tables()`. `ls_main` calls `set_position_coordinates(0, 32)` + `draw_text("Init libsys finished")` when `state->debug_enabled`.
- **Pending**: CIF decompression (atlas decode); proportional-width atlas blit in `draw_text_at`; `render_text_with_style` (sub_4819B0); `display_text_message` dialog loop.

## Game bootstrap

Entry point: `game_init` (0046BEEB) in `src/game_init.c`. Called from `ls_main` after `init_all_systems` completes and before the intro cutscene.

Phases in execution order:

1. **Debug banner** — `set_position_coordinates(0,32)` + `draw_text("Init Scene ...")` when `debug_enabled`.
2. **HUD health bitmap** — `select_language_version("DATAS>MENUS>GINETTE.CIF")`. If localised variant found: `image_load_compressed("DATAS>MENUS>GINETTE.GIF", ui_health_buffer)` → `dword_4DDE1C`. Fallback: `ls_init_bmap_ex(0x14, 0x4B, 8, 8)`.
3. **LOADING.GIF** — `image_load_compressed("DATAS>MENUS>LOADING.GIF", _ls_mem_buffer)` → `renderer_context`.
4. **Transparency pass** — walk LOADING.GIF pixels; set non-0xFF pixels to 0.
5. **HUD bmap copies** — two `ls_init_bmap_ex` calls at GINETTE dims → `dword_4DE648`, `dword_4DE64C`.
6. **Gouraud index buffer** — `allocate_memory(MEM_SLOT_HEALTH, 0x4100)` → `dword_4DE640`. `index_buffer = (dword_4DE640 & 0xFFFFFF00) + 0x100`.
7. **MENUGEN.GIF palette** — `load_image_color_data("DATAS>MENUS>MENUGEN.GIF", ...)` ×2; `memcpy 0x300` to `unk_4DDF30`; `memset` first `0x180` to 0; `ls_free_palette`; `memset input_state_table[0x100]` to 0; `update_palette_with_default_colors`; `ls_build_palette_colour`.
8. **sub_425D50** — unknown post-palette call.
9. **initialize_health_hud** — allocates 2048-byte HUD buffer with numeric ramp pattern.
10. **sub_45D395, sub_421960, sub_41F7C8, sub_44D706, sub_43040A** — unknown sub-system inits.
11. **Weapon-slot table** — nested loop: 4 outer × 31 inner; zero-inits `dword_4ECD50`/`4ECDD4` sentinels, fills `dword_4ECD54` entries with `0xFFFFFFFF`.
12. **Entity-slot table** — 98 outer × 31 inner; same pattern in `dword_4DF5D0`/`4DF654`/`4DF5D4`.
13. **Zero-init global tables** — `dword_4ECD3C=0`; memset `dword_4EC6A0` (0x188), `companion_entity` (8), `dword_4EE0F0` (0x6BD0), `unk_4DAB60` (0x860); memset-0xFF `dword_4DCDA0`/`4DC2E0` (0x9D8 each); memset `byte_4DCCC0` (0xD2).
14. **Debug banner** — `draw_text("Init Divers ...")`.
15. **FLAM.MAT** — `sub_45018B("DATAS>OBJETS>FLAM.MAT", ui_health_buffer)` (HDD) or `sub_45018B("OBJETS>FLAM>FLAM.MAT", ...)` (CD) → `dword_4FB1E0`.
16. **allocate_and_initialize_health_buffer** — allocates and zeros health buffer.
17. **sub_42F88E** — unknown.
18. **register_player_creature(1)** — configures creature types (serpent/fish/bird) for the player.
19. **init_font_system** — loads SYSFONT (FONTE11.CAR + .CIF).
20. **sub_44D676** — unknown (IDA label `__wsetargv` likely incorrect).
21. **sub_48F2C0** — texture config init; 0x3E8 entries in `texture_config`.
22. **sub_48F230** — entity buffer setup; 640×480 frame dimensions.
23. **Render bmaps** — colour-mode branch:
    - 8-bit: `create_image_buffer` × 2 → `render_buffer1`, `debug_surface` / `dword_4B71FC`.
    - 16-bit (active): `ls_init_bmap_ex` × 4 → `render_buffer1`, `dword_4B71FC`, `dword_4B7200`, `debug_surface`.
24. **render_buffer2 / half-res** — `ls_init_bmap_ex(0x280, 0x1E0)` → `render_buffer2`; `ls_init_bmap_ex(0x140, 0x0F0)` → `dword_507474`.
25. **objets.gif** — `image_load_compressed("datas>objets>objets.gif", ui_health_buffer)`.
26. **sub_4130AA** — resource/sprite table initialiser.
27. **initialize_resource** — inits resource from `ui_health_buffer` → `palette_data`.
28. **game_state wire-up** — `game_state = game_death_state = dword_4B70E8`.
29. **sub_46493B** — player entity setup (ECX=dword_4B70E8, EDX=ui_weapon_slot_buffer) → `player_entity`.
30. **sub_464F6D** — player entity secondary init.
31. **Cross-wiring** — palette ptr written into `player_entity+B0h`, `game_context+18h`, `render_buffer1+18h`, `graphics_context+18h`; `game_context = entity_buffer_base`.
32. **system_get_gametime** → `player_entity+D5Ch`.
33. **sub_48F1D0 ×2** — render surface registration (render_surface2, unk_4EDC00).
34. **set_entities_combat_mode** — enables combat for all entities.
35. **sub_426335** — unknown.
36. **sub_471C18** — loads `objets>objets.txt`.
37. **special_mode_active → dword_505DD8**.
38. **16-bit branch** — `sub_491ED0` + `sub_491FD0(0x3C, graphics_context[+2Ch], dword_4B71F8)`.
39. **sub_45B207** — unknown.
40. **display_weapons_inventory** — populates weapon/ammo UI slots.
41. **sub_4161B0** — final tail call; `dword_4B787C = 0`.

**Port status**: `partial` — skeleton with all 45 phases logged and call graph preserved. Sub-functions 1–11, 15–22, 29–41 are stubs (`// TODO`). Phases 2–6, 11–13, 23–24 have inline asm-faithful structure (loops, bmap alloc sizes). `init_font_system` is fully ported. **Real image loading now active**: phases 3, 7, 29, 45 load real bytes via `image_load_compressed` → `cif_load`. No crashes — exit 143 on SIGTERM.

**M6 ported** (four additional phase functions):
- Phase 8 / **sub_425D50** → `palette_postprocess_configure` (`src/libsys/graphics/palette.c`): loads persona/inventory slot data (98 × 8 i16 = `word_509B40`) from `PERSOS>INVENT.CTX` cache or parses `PERSOS>INVENT.TXT` and writes the cache. Status: `partial` — `ls_switch_memory`, `ls_load_file_at`, `ls_get_filedate` stubbed.
- Phase 10 / **sub_45D395** → `initialize_mouse_and_animation` (`src/game_init.c`): zeros six mouse-state globals (4D7F0C/24/10/20/14/18 = offsets/ranges), loads INTFANI3.GIF animation, allocates 51×99 animation bmap. Status: `partial` — `ls_set_mouse`, `ls_set_mouse_range`, `image_load_compressed` stubbed.
- Phase 30 / **sub_4130AA** → `initialize_resource_slots` (`src/game_init.c`): allocates `dword_4B297C` bmap (640×800, 8-bit) and writes `color_table` pointer into `[bmap+18h]` (palette pointer field used by rendering). Status: `ported`.
- Phase 45 / **sub_4161B0** → `game_finalize` (`src/game_init.c`): outer flow ported — sets up the FONTE11 font descriptor block (`unk_4CFE40`) and queues a call to `sub_415FB0` (FONTE11 CAR+GIF loader). `sub_415FB0` is >200 asm lines and is deferred as a TODO stub. Status: `partial`.

New globals added:
- `color_table[0x300]` and `word_509B40[0x310]` in `src/libsys/graphics/palette.c` / `palette.h`.
- `g_dword_4B297C`, `g_dword_4D7F0C/24/10/20/14/18`, `g_dword_4D7F2C`, `g_dword_4F4DF4`, `g_unk_4CFE40` in `src/game_init.c`.

**Image loading landed (M7)**:
- `select_language_version(vpath, out, size)` — resolves virtual path → host path via `ls_resolve_path`; tries `.GIF` then `.CIF` extension; returns path if file exists. Single-language passthrough (no language-prefix substitution yet).
- `image_load_compressed(vpath, buf_ignored)` — wraps `cif_load`; accepts `.GIF` virtual paths, retries with `.CIF` on disk; returns heap-owned `bmap_t` on success, `NULL` on failure.
- `load_image_color_data(vpath, buf)` — uses `cif_load` directly; copies 768-byte palette to `color_table`; returns heap-owned palette buffer (`ls_free_palette` to release).
- `sub_415FB0(base_name, desc)` — loads `FONTE11.CAR` (0xBF4 glyph bytes into `desc+4`) + `FONTE11.GIF`→`.CIF` atlas via `image_load_compressed`; stores bmap ptr at `desc+0` and `0x13` at `desc+0BF8h`. Glyph bitmask walk (word_4D0A40) deferred.
- Verified log output: `LOADING.GIF -> 512x64`, `MENUGEN.GIF palette loaded`, `objets.gif -> 256x512`, `sub_415FB0: FONTE11 atlas -> 691x224`.

**Next steps**: port `sub_46493B` (player entity) + `display_weapons_inventory`; wire `word_4D0A40` for font bitmask walk in `sub_415FB0`.

## Timing

- Present: `timer_t { tick, last_tick, last_tick_fps, frame_count }`, `ls_tick`, `ls_delay`.
