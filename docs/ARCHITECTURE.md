# Architecture

## Layering (target)

```
+-------------------------------------------------------------+
|                    game (src/*.c)                           |
|   main loop, state, game_detection, level/scene/entity...   |
+-------------------------------------------------------------+
|                 libsys (src/libsys/*)                       |
|   memory, filesystem, graphics, audio, input, timer, movie  |
+-------------------------------------------------------------+
|          platforms (src/libsys/platforms/*)                 |
|   macos.m, linux.c, windows.c — window, events, blit        |
+-------------------------------------------------------------+
```

`libsys` = the original engine's runtime layer ("LIBSYS" section in `win.ini`).
`dke` = game-specific code that consumes libsys.

## Current top-level files

| file | purpose |
|---|---|
| `src/main.c` | entry point, WinMain/main, `ls_main`, config load, main loop |
| `src/state.c/.h` | global `state_t` (renderer + audio + game handles) |
| `src/game_detection.c/.h` | locale/version enum, title strings |
| `src/libsys/common.[ch]` | fixed-width types, mem helpers, print macro |
| `src/libsys/file_system.[ch]` | `ls_open`, `ls_exists`, `ls_get_base_path` |
| `src/libsys/system.h` | `system_t`, timer, events, blit/flip API |
| `src/libsys/graphics/graphic.[ch]` | back/front/frame buffers, init/flip/clear |
| `src/libsys/graphics/image.[ch]` | image struct + loaders (stub) |
| `src/libsys/audio/audio.[ch]` | audio init/release |
| `src/libsys/platforms/macos.m` | Cocoa window/event/blit |
| `src/libsys/platforms/windows.c` | Win32 host (stub) |

## Main loop (present)

`main` (or `WinMain`) → `state_init` → `detect_game` → `ls_init_instance` → `ls_main` → per-frame `ls_update_events` + `ls_blit` + `ls_flip` + FPS gate.

## Missing engine pieces (top of ROADMAP)

- Memory manager (`ls_init_memory_manager`, `allocate_memory`, `free_memory`)
- Mouse / keyboard / joystick input abstractions (`lsys_init_mouse_system`, `lsys_init_joy_system`)
- Sound stream (`ls_init_sound_stream`, `lsys_set_sound_format`)
- CD subsystem (`lsys_init_cd_system`) — likely stub-able (patched no-CD build)
- Movie playback (`ls_init_movie`, `ls_play_movie`, `ls_shut_movie`, `decompress_anim`, `read_and_init_sbt`)
- Video mode / bitmap allocator (`ls_set_video_mode`, `ls_init_bmap_ex`)
- Palette / shading table preload (`preload_shading_table`, `initialize_color_mapping_tables`)
- Font system (`init_font`, `init_font_system`)
- Game init (`game_init_46BEEB`) — entity/HUD/inventory bootstrap

## Original executable layout (from asm)

- 1404 `proc near` functions across `.text`.
- MSVC-style CPPEH exception handling (SEH via `_EH3_EXCEPTION_REGISTRATION`).
- Standard Win32/GDI + DirectDraw for host (see `tagRECT`, `tagMSG`, `tagPALETTEENTRY`).
- Custom `seb_*` allocator for movies (SEB = movie codec used by Kalisto).
