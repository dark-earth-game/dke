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
- **M1 ported**: `ls_init_bmap_ex` (0x0047CEBE) → `src/libsys/graphics/graphic.c` — allocates `bmap_t` (0x34-byte header + pixel buffer); surface-backed paths stubbed to raw alloc (status: `partial`). `ls_set_video_mode` (0x0048418A) → same file — tears down and recreates `g_graphics_context` + `g_back_screen`; DirectDraw mode-set skipped (status: `partial`). New types: `bmap_t`, globals `g_graphics_context`, `g_back_screen`, `g_bmap_count`.

## Audio

- Present: `audio_t { sample_rate, channels, bit_depth }`, `ls_init_audio` at 22050Hz stereo 16-bit.
- Original: `ls_init_sound_system`, `ls_init_sound_stream`, `lsys_set_sound_format`.
- Music vs samples split (see `state_t.samples_enabled` / `music_enabled`).
- **M1 ported**: `ls_init_sound_system` (0x0047FDF8) → `src/libsys/sound.c`; inner `lsys_init_sound_system` (0x0048502B) stubbed (DirectSound absent). Status: `stub`.

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
  - **Audio still pending**: each frame's 2942-byte audio block is fseek'd past. Wire `decode_audio_block` from Python ref when CoreAudio/SDL streaming is available.
  - See `docs/functions/crh_decoder.md` for format spec and port mapping.

## Input

- Present: `system_t.actions[256]`, `motion_x/y`. Platform layer fills these.
- Original: `lsys_init_mouse_system`, `lsys_init_joy_system`, keyboard via WndProc.
- **M1 ported**: `lsys_init_mouse_system` (0x00487BB0) → `src/libsys/mouse.c` (status: `stub` — sets `g_mouse_enabled=1`; platform layer delivers all input). `lsys_init_joy_system` (0x004811A0) → `src/libsys/joy.c` (status: `stub` — winmm skipped).

## CD

- Original: `lsys_init_cd_system`. Patched no-CD build → stub to success.
- **M1 ported**: `lsys_init_cd_system` (0x00487770) → `src/libsys/cd.c` (status: `stub` — sets `g_cd_present=0`, returns 0 matching original MCI-fail path).

## Timing

- Present: `timer_t { tick, last_tick, last_tick_fps, frame_count }`, `ls_tick`, `ls_delay`.
- Original: `sub_487D40` likely timing/QueryPerformanceCounter init — TBD.
