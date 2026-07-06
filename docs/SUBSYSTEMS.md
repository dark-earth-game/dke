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

## Graphics

- Present: `graphic_t` with back/front/frame buffers, `ls_init_graphic_system`, `ls_graphic_flip`.
- Original: `graphics_init`, `graphics_triangle_rasteriser`, `apply_palette_transformation`, `configure_game_palette`, `initialize_color_mapping_tables` (for 16-bit mode).
- Two colour modes: `colour_mode = 0` → 8-bit palette, `colour_mode = 1` → 16-bit hicolor. `main.c` forces 16-bit.
- **M5 ported**: `initialize_color_mapping_tables` (00407D49) → `src/libsys/graphics/colour_tables.c`. Fills `primary_color_table[32×32]` and `secondary_color_table[33×32]` with Bresenham shading LUTs. Sets `color_transform_function` to `transform_color_6bit` (565 RGB, since `colour_mode != 4`). Both transform functions (`lookup_color_mapping` for 555, `transform_color_6bit` for 565) ported. Called from `init_all_systems` when `state->colour_mode` is set.
- **M1 ported**: `ls_init_bmap_ex` (0x0047CEBE) → `src/libsys/graphics/graphic.c` — allocates `bmap_t` (0x34-byte header + pixel buffer); surface-backed paths stubbed to raw alloc (status: `partial`). `ls_set_video_mode` (0x0048418A) → same file — tears down and recreates `g_graphics_context` + `g_back_screen`; DirectDraw mode-set skipped (status: `partial`). New types: `bmap_t`, globals `g_graphics_context`, `g_back_screen`, `g_bmap_count`.

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
- `ls_libsys_init_flags` (00487E30) — reads `ls_master_enabled`, `ls_debug_enabled`, `ls_info_enabled` from `state_t` (populated by `load_configuration` from `win.ini [LIBSYS]`); derives `g_ls_lang_id` from POSIX `LANG` env-var (Unix) or `GetSystemDefaultLangID` (Windows). Sets `g_ls_master`, `g_ls_debug`, `g_ls_info`, `g_ls_lang_id` globals; stores result in `state->ls_lang_id`. `lsi_open_stat_app` stubbed (Win32 companion process).

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

## Timing

- Present: `timer_t { tick, last_tick, last_tick_fps, frame_count }`, `ls_tick`, `ls_delay`.
