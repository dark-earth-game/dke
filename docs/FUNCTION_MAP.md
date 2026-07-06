# Function Map

Canonical map: original address → asm name → C name → source file → status.

Status: `todo` · `stub` · `partial` · `ported` · `verified`.

Sorted by address.

| addr | asm name | C name | file | status | notes |
|---|---|---|---|---|---|
| — | load_dke_config | `load_dke_config` | src/main.c | ported | game-side settings from DKE.CFG |
| 0046091B | load_configuration | `load_configuration` | src/main.c | partial | ini parser broken (see [ROADMAP.md](ROADMAP.md) M0) |
| 00460B6C | ls_main | `ls_main` | src/main.c | partial | main loop OK, many subsystem calls still commented out |
| 0047CEBE | ls_init_bmap_ex | `ls_init_bmap_ex` | src/libsys/graphics/graphic.c | partial | raw-buffer path ported; DirectDraw surface-backed cases (bpp_flags 1/2/4/8) stubbed to raw alloc |
| 0047CBA0 | ls_startup | `ls_startup` | src/main.c | partial | all M1 subsystem calls now wired; sub_485840/487D40/487E30 still commented |
| 0047FDF8 | ls_init_sound_system | `ls_init_sound_system` | src/libsys/sound.c | stub | DirectSound replaced by no-op; returns 1; existing ls_init_audio() owns host audio |
| 0048118A | lsys_init_joy_system | `lsys_init_joy_system` | src/libsys/joy.c | stub | winmm joystick API skipped; NOP until joystick input is targeted |
| 0048418A | ls_set_video_mode | `ls_set_video_mode` | src/libsys/graphics/graphic.c | partial | DirectDraw SetDisplayMode skipped; creates g_graphics_context + g_back_screen via ls_init_bmap_ex |
| 00480C30 | lsi_set_window_title | `ls_set_title` | src/libsys/platforms/macos.m | ported | |
| 00485840 | sub_485840 | — | — | todo | called from `ls_startup` |
| 00487D40 | sub_487D40 | — | — | todo | called from `ls_startup` |
| 00487E30 | sub_487E30 | — | — | todo | called from `ls_startup` |
| 00487770 | lsys_init_cd_system | `lsys_init_cd_system` | src/libsys/cd.c | stub | no-CD build; sets g_cd_present=0, returns 0 matching original MCI-fail path |
| 00487BB0 | lsys_init_mouse_system | `lsys_init_mouse_system` | src/libsys/mouse.c | stub | sets g_mouse_enabled=1; platform layer owns cursor pos/buttons via system_t |
| 0048502B | lsys_init_sound_system | `lsys_init_sound_system` (static) | src/libsys/sound.c | stub | inner DirectSound init; returns 1; called by ls_init_sound_system |
| 0048F190 | preload_shading_table | `preload_shading_table` | src/libsys/graphics/shading.c | ported | initialises 4096-entry cosine LUT (IDA: sine_table); called from graphics_init |
| 00491E20 | init_all_systems | `init_all_systems` | src/main.c | partial | ls_set_video_mode now wired; ls_init_bmap_ex called internally |
| — | ls_init_memory_manager | — | — | todo | see [SUBSYSTEMS.md](SUBSYSTEMS.md) memory |
| — | initialize_color_mapping_tables | — | — | todo | 16-bit path |
| 00430A50 | seb_get_memory | `seb_get_memory` (static) | src/libsys/movie.c | ported | 64K-aligned or plain malloc wrapper; zeros; records base ptr for release_all |
| 00430A08 | init_my_memory | `init_my_memory` (static) | src/libsys/movie.c | ported | zeros allocation counters |
| 00430B00 | seb_release_all | `seb_release_all` (static) | src/libsys/movie.c | ported | frees all seb_slots via free() (orig: ls_release) |
| 00431EA0 | ls_init_movie | `ls_init_movie` | src/libsys/movie.c | ported | tables + seb allocations + heap slot; cp_init and COMPR ini read still TODO |
| 00432120 | ls_shut_movie | `ls_shut_movie` | src/libsys/movie.c | ported | releases heap slot + seb pool; cp_uninit TODO |
| 00432550 | ls_play_movie | `ls_play_movie` | src/libsys/movie.c | partial | CRH decoder ported via CRH_to_MP4.py ref (crh_open+crh_next_frame); audio still deferred |
| — | crh_open | `crh_open` | src/libsys/movie_decoder.c | ported | parse_header + detect_dims; opens file, parses 52-byte hdr, auto-detects dims |
| — | crh_next_frame | `crh_next_frame` | src/libsys/movie_decoder.c | ported | decode_component (dec_comp_framepred) + reconstruct_rgb per frame; audio skipped |
| — | crh_close | `crh_close` | src/libsys/movie_decoder.c | ported | closes file handle, frees prev_plane buffers |
| — | init_decompress | — | — | todo | replaced by crh_open for CRH files; original SEB chain may differ |
| — | decompress_anim | — | — | todo | replaced by crh_next_frame for CRH files |
| — | read_and_init_sbt | — | — | todo | |
| — | init_font_system | — | — | todo | |
| — | game_init_46BEEB | — | — | todo | big entry: HUD, weapons, entities |

Update this table whenever a function's status changes.
