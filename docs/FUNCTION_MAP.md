# Function Map

Canonical map: original address → asm name → C name → source file → status.

Status: `todo` · `stub` · `partial` · `ported` · `verified`.

Sorted by address.

| addr | asm name | C name | file | status | notes |
|---|---|---|---|---|---|
| 0046DFEC | scene_load_level_46DFEC | `scene_load_level` | src/scene.c | partial | Slice C skeleton: 25 logged phases, globals set, .INF header read attempt, all heavy sub-calls stubbed; player_entity writes NULL-guarded; state->current_level_id updated on success |
| — | load_dke_config | `load_dke_config` | src/main.c | ported | game-side settings from DKE.CFG |
| 00437EB7 | process_mainmenu_437EB7 | `process_mainmenu` | src/menu.c | partial | pass 2: action dispatch wired (QUIT/TRAILER real, LOAD/OPTIONS/NEW_GAME stubs); sprite overlay / sound / real hitboxes TODO (pass 3) |
| — | — | `menu_init` | src/menu.c | ported | inits menu_t: state=MAIN, hovered=0, pending_trailer=0, fetches backdrop bmap+palette from game_init |
| — | — | `menu_dispatch` | src/menu.c | partial | routes menu_action_e → QUIT state / pending_trailer / log stubs; called from mouse click and keyboard confirm paths |
| — | — | `menu_update` | src/menu.c | partial | mouse hover (motion_x/y) + click (mouse_left==2) + keyboard nav (actions[38/40/13/27]); sub_43F7BD exact hitboxes TODO (pass 3) |
| — | — | `menu_render` | src/menu.c | partial | blits MENUGEN backdrop; blits MENU_<lang>.CIF overlay (masked, index-0 transparent); skips text labels when overlay present (sprite IS the menu); plays EADKE.CRH if pending_trailer; blits GENICO animated icon next to hovered item; blits CURS128 at (motion_x,motion_y) with mask_idx=0 transparency; per-frame cursor log suppressed (emits only on frame_idx change) |
| 0046091B | load_configuration | `load_configuration` | src/main.c | partial | ini parser broken (see [ROADMAP.md](ROADMAP.md) M0) |
| 00460B6C | ls_main | `ls_main` | src/main.c | partial | main loop OK, many subsystem calls still commented out |
| 0047CEBE | ls_init_bmap_ex | `ls_init_bmap_ex` | src/libsys/graphics/graphic.c | partial | raw-buffer path ported; DirectDraw surface-backed cases (bpp_flags 1/2/4/8) stubbed to raw alloc |
| 0047CBA0 | ls_startup | `ls_startup` | src/main.c | ported | all subsystem calls wired; sub_485840/487D40/487E30 ported and called |
| 0047FDF8 | ls_init_sound_system | `ls_init_sound_system` | src/libsys/sound.c | stub | DirectSound replaced by no-op; returns 1; existing ls_init_audio() owns host audio |
| 0048118A | lsys_init_joy_system | `lsys_init_joy_system` | src/libsys/joy.c | stub | winmm joystick API skipped; NOP until joystick input is targeted |
| 0048418A | ls_set_video_mode | `ls_set_video_mode` | src/libsys/graphics/graphic.c | partial | DirectDraw SetDisplayMode skipped; creates g_graphics_context + g_back_screen via ls_init_bmap_ex |
| 00480C30 | lsi_set_window_title | `ls_set_title` | src/libsys/platforms/macos.m | ported | |
| 00485840 | sub_485840 | `ls_build_virtual_root` | src/libsys/lsys_startup.c | ported | zero-inits workspace bufs; builds g_virtual_root ("HDISK0:" etc.); sub_48C5D0 TODO |
| 00487D40 | sub_487D40 | `ls_libsys_flag_reset` | src/libsys/lsys_startup.c | ported | resets g_ls_pause_state (word_4C0B68) to 0 |
| 00487E30 | sub_487E30 | `ls_libsys_init_flags` | src/libsys/lsys_startup.c | ported | applies LIBSYS flags from state_t; derives lang via LANG env on POSIX; calls lsi_open_stat_app |
| ~487F10 | lsi_open_stat_app | `lsi_open_stat_app` (static) | src/libsys/lsys_startup.c | ported | opens dke_stat.log in game_path for append; writes timestamped startup line; non-fatal on open fail |
| 00487770 | lsys_init_cd_system | `lsys_init_cd_system` | src/libsys/cd.c | stub | no-CD build; sets g_cd_present=0, returns 0 matching original MCI-fail path |
| 00487BB0 | lsys_init_mouse_system | `lsys_init_mouse_system` | src/libsys/mouse.c | stub | sets g_mouse_enabled=1; platform layer owns cursor pos/buttons via system_t |
| 004843F0 | sub_4843F0 | `ls_measure_cpu_speed` | src/libsys/lsys_startup.c | ported | fills cpu_info_t: perf_counter_freq=1e9, cpu_speed_mhz via sysctlbyname/proc/cpuinfo, flags=0 |
| 0048502B | lsys_init_sound_system | `lsys_init_sound_system` (static) | src/libsys/sound.c | stub | inner DirectSound init; returns 1; called by ls_init_sound_system |
| 0048F190 | preload_shading_table | `preload_shading_table` | src/libsys/graphics/shading.c | ported | initialises 4096-entry cosine LUT (IDA: sine_table); called from graphics_init |
| 00491450 | sub_491450 | `palette_copy_alternative` | src/libsys/graphics/palette.c | ported | rep movsd 34 dwords: alternative_palette_data → palette_target_buffer (136 bytes) |
| 00491E20 | init_all_systems | `init_all_systems` | src/main.c | partial | ls_set_video_mode now wired; ls_init_bmap_ex called internally |
| — | ls_init_memory_manager | — | — | todo | see [SUBSYSTEMS.md](SUBSYSTEMS.md) memory |
| 00407D49 | initialize_color_mapping_tables | `initialize_color_mapping_tables` | src/libsys/graphics/colour_tables.c | ported | builds primary/secondary 32×32 shading LUTs; sets color_transform_function (565 mode) |
| 00430A50 | seb_get_memory | `seb_get_memory` (static) | src/libsys/movie.c | ported | 64K-aligned or plain malloc wrapper; zeros; records base ptr for release_all |
| 00430A08 | init_my_memory | `init_my_memory` (static) | src/libsys/movie.c | ported | zeros allocation counters |
| 00430B00 | seb_release_all | `seb_release_all` (static) | src/libsys/movie.c | ported | frees all seb_slots via free() (orig: ls_release) |
| 00431EA0 | ls_init_movie | `ls_init_movie` | src/libsys/movie.c | ported | tables + seb allocations + heap slot; cp_init and COMPR ini read still TODO |
| 00432120 | ls_shut_movie | `ls_shut_movie` | src/libsys/movie.c | ported | releases heap slot + seb pool; cp_uninit TODO |
| 00432550 | ls_play_movie | `ls_play_movie` | src/libsys/movie.c | partial | CRH decoder ported via CRH_to_MP4.py ref (crh_open+crh_next_frame); audio still deferred |
| — | crh_open | `crh_open` | src/libsys/movie_decoder.c | ported | parse_header + detect_dims; opens file, parses 52-byte hdr, auto-detects dims |
| — | crh_next_frame | `crh_next_frame` | src/libsys/movie_decoder.c | ported | wrapper around crh_next_frame_ex(NULL audio) |
| — | crh_next_frame_ex | `crh_next_frame_ex` | src/libsys/movie_decoder.c | ported | decode video + optionally capture raw CRH_A_BYTES audio block |
| — | crh_decode_audio | `crh_decode_audio` | src/libsys/movie_decoder.c | ported | port of Python decode_audio_block; i16-seed + sign7 delta codec → interleaved i16 PCM |
| — | crh_close | `crh_close` | src/libsys/movie_decoder.c | ported | closes file handle, frees prev_plane buffers |
| — | ls_audio_push | `ls_audio_push` | src/libsys/audio/audio.c | ported | forwards interleaved i16 PCM to ls_platform_audio_push |
| — | ls_audio_stop_stream | `ls_audio_stop_stream` | src/libsys/audio/audio.c | ported | closes and re-opens platform audio stream |
| — | ls_platform_audio_open | `ls_platform_audio_open` | src/libsys/platforms/macos.m | ported | CoreAudio AudioQueue init: LPCM i16 LE, 4 buffers, pre-filled silence |
| — | ls_platform_audio_push | `ls_platform_audio_push` | src/libsys/platforms/macos.m | ported | grabs free AQ buffer (2 ms wait), copies PCM, enqueues |
| — | ls_platform_audio_close | `ls_platform_audio_close` | src/libsys/platforms/macos.m | ported | AudioQueueStop (sync) + AudioQueueDispose |
| — | ls_platform_audio_open | `ls_platform_audio_open` | src/libsys/platforms/linux.c | stub | ALSA not implemented; returns true |
| — | ls_platform_audio_push | `ls_platform_audio_push` | src/libsys/platforms/linux.c | stub | no-op; returns n |
| — | ls_platform_audio_close | `ls_platform_audio_close` | src/libsys/platforms/linux.c | stub | no-op |
| — | ls_platform_audio_open | `ls_platform_audio_open` | src/libsys/platforms/windows.c | stub | WASAPI not implemented; returns true |
| — | ls_platform_audio_push | `ls_platform_audio_push` | src/libsys/platforms/windows.c | stub | no-op; returns n |
| — | ls_platform_audio_close | `ls_platform_audio_close` | src/libsys/platforms/windows.c | stub | no-op |
| — | — | `wav_load` | src/libsys/audio/wav.c | ported | RIFF/WAVE PCM loader; 8-bit unsigned → i16 conversion; logs path/channels/rate/bit_depth/samples on success |
| — | — | `wav_free` | src/libsys/audio/wav.c | ported | frees samples buffer; safe on zeroed wav_t |
| — | — | `sfx_play` | src/libsys/audio/wav.c | ported | one-shot PCM playback: mono→stereo upmix; 11025→22050 nearest-neighbour resample; drops if queue full or audio_enabled=false |
| — | — | `menu_load_click_sfx` (static) | src/menu.c | ported | finds best click WAV in wav/NORMAL/ (INVENT1.WAV → MENU.WAV → CLICK.WAV → SELECT.WAV → first .WAV); logs chosen file |
| — | — | `menu_release` | src/menu.c | ported | frees sfx_click wav_t; call once after menu loop exits |
| — | — | `menu_release_cursors` (static) | src/menu.c | ported | frees cursor_bmap + genico_frames[7] bmaps; called by process_mainmenu on QUIT state |
| — | — | `blit_bmap_masked_to_framebuffer` | src/libsys/graphics/graphic.c | ported | like blit_bmap_to_framebuffer but skips pixels with palette index == mask_idx (transparency); used by menu cursor/sprite overlay |
| — | — | `game_init_load_cursor` | src/game_init.c | ported | wraps image_load_compressed for external callers; used by menu_init to load CURS128 and GENICO_* frames |
| — | init_decompress | — | — | todo | replaced by crh_open for CRH files; original SEB chain may differ |
| — | decompress_anim | — | — | todo | replaced by crh_next_frame for CRH files |
| — | read_and_init_sbt | — | — | todo | |
| — | cif_load | `cif_load` | src/libsys/graphics/cif.c | ported | open+decode .CIF file: 16-byte hdr, 768-byte palette, LZW pixel data; returns cif_image_t |
| — | cif_free | `cif_free` | src/libsys/graphics/cif.c | ported | free pixels + palette buffers; safe on zeroed struct |
| — | cif_to_xrgb | `cif_to_xrgb` | src/libsys/graphics/cif.c | ported | expand 8-bit indexed image to 32-bit XRGB (0xFF000000\|R<<16\|G<<8\|B) |
| ~405DAB | init_font_system | `init_font_system` | src/libsys/graphics/font.c | partial | loads FONTE11.CAR + FONTE11.CIF atlas via cif_load; atlas rendering branch now reachable; inner pixel blit in draw_text_at still TODO |
| ~430BEA | init_font | `init_font` | src/libsys/graphics/font.c | partial | SYSFONT: reads .CAR glyph index + decodes .CIF atlas via cif_load; .FNT path (sub_482200) stubbed |
| ~405DC7 | set_position_coordinates | `set_position_coordinates` | src/libsys/graphics/font.c | ported | writes cursor_x/y + target_x/y globals |
| ~405EDB | draw_text | `draw_text` | src/libsys/graphics/font.c | ported | renders to front_buffer via draw_text_at; fallback CP437 8×8 always works |
| — | draw_text_at | `draw_text_at` | src/libsys/graphics/font.c | ported | explicit font/x/y/color; atlas path stubbed (TODO when CIF decoded) |
| ~430D09 | read_line_file | `read_line_file` | src/libsys/graphics/font.c | ported | reads one \\n-terminated line from FILE*; used by read_and_init_sbt |
| 00495390 | sub_495390 | `ls_install_default_error_handlers` | src/libsys/lsys_startup.c | ported | installs g_error_handler_1/2 → no-op stubs (mirrors sub_494560/sub_4945F0) |
| 004953B0 | sub_4953B0 | `ls_copy_unknown_a_to_b` | src/libsys/lsys_startup.c | ported | rep movsd 34 dwords: unknown_data_a (off_4C7748) → unknown_data_b (off_4C77D0); purpose TODO |
| 0046BEEB | game_init_46BEEB | `game_init` | src/game_init.c | partial | 45-phase skeleton: call graph preserved; all sub-functions stubbed; bmap allocs wired; font init wired; logs all phases |
| — | select_language_version | `select_language_version` (static) | src/game_init.c | partial | resolves virtual path → host; tries .GIF then .CIF; returns path if file exists, NULL otherwise; no language-prefix substitution yet |
| — | image_load_compressed | `image_load_compressed` (static) | src/game_init.c | ported | wraps cif_load; accepts .GIF names, retries with .CIF; returns heap-owned bmap_t; buf_ignored (allocates internally) |
| — | image_load_compressed_free | `image_load_compressed_free` (static) | src/game_init.c | ported | releases bmap_t and pixel buffer returned by image_load_compressed; TODO: wire to cleanup paths |
| — | load_image_color_data | — (stub in game_init.c) | src/game_init.c | stub | returns NULL; real impl extracts palette from image |
| — | ls_free_palette | — (stub in game_init.c) | src/game_init.c | stub | no-op; real impl frees palette handle |
| — | initialize_health_hud | — | — | todo | allocates 2048-byte HUD buffer w/ numeric pattern |
| — | allocate_and_initialize_health_buffer | — | — | todo | allocates health buffer, logs if flag set, zeros it |
| — | register_player_creature | — | — | todo | ECX=1; configures serpent/fish/bird creature types |
| — | display_weapons_inventory | — | — | todo | displays ammo/items in game UI |
| 00425D50 | sub_425D50 | `palette_postprocess_configure` | src/libsys/graphics/palette.c | partial | loads persona/inventory slot data into word_509B40 from PERSOS>INVENT.CTX cache or PERSOS>INVENT.TXT; ls_switch_memory/ls_load_file_at/ls_get_filedate stubbed |
| 0045D395 | sub_45D395 | `initialize_mouse_and_animation` | src/game_init.c | partial | zeros mouse-state globals (4D7F0C/24/10/20/14/18); allocates animation bmap 51×99; ls_set_mouse/ls_set_mouse_range stubbed; image_load_compressed stubbed |
| 00421960 | sub_421960 | — | — | todo | unknown; called from game_init phase 11 |
| 0041F7C8 | sub_41F7C8 | — | — | todo | unknown; called from game_init phase 12 |
| 0044D706 | sub_44D706 | — | — | todo | unknown; called from game_init phase 13 |
| 0043040A | sub_43040A | — | — | todo | unknown; called from game_init phase 14 |
| 0042F88E | sub_42F88E | — | — | todo | unknown; called from game_init phase 21 |
| 0044D676 | sub_44D676 | — | — | todo | labelled __wsetargv in IDA (doubtful); called from game_init phase 24 |
| 0048F2C0 | sub_48F2C0 | — | — | todo | texture config init; ECX=&texture_config, EDX/push=0x3E8 |
| 0048F230 | sub_48F230 | — | — | todo | entity buffer setup; 640×480 |
| 0048F1D0 | sub_48F1D0 | — | — | todo | render surface setup; called twice |
| 0046493B | sub_46493B | `player_entity_init` | src/player.c | partial | Slice D pass 1: alloc 0xEA0 bytes, zero-init, default fields (clip rect, position, bounds, type flags, current_game_time at +0xD34); nested sub-calls stubbed (setup_creature, sub_462BCC, process_scene_hierarchy_with_memory, create_single_control_point, sub_4699D9, sub_41F6BA) |
| 00464F6D | sub_464F6D | `player_entity_init_secondary` | src/player.c | partial | Slice D pass 1: +0xCD8 dialogue buf ptr, weapon slot read at [entity+0xACh]+0x97, weapon entity load chain (sub_4648DC, sub_475BB6, sub_472C5C, sub_473756, sub_41F6BA) all stubbed |
| — | — | `player_input_update` | src/player.c | ported | polls sys->actions[119/115/97/100] (WASD) → velocity_z/x; mouse_left==2 → want_action=1; logs when moving |
| 004130AA | sub_4130AA | `initialize_resource_slots` | src/game_init.c | ported | allocates 640×800 8-bit bmap (dword_4B297C); stores color_table ptr at [bmap+18h] |
| 00415FB0 | sub_415FB0 | `sub_415FB0` (static) | src/game_init.c | partial | loads FONTE11.CAR (0xBF4 glyph bytes into desc+4) + FONTE11.GIF→.CIF atlas via image_load_compressed; stores bmap ptr at desc+0, height 0x13 at desc+0BF8h; glyph bitmask walk (word_4D0A40) deferred |
| 004161B0 | sub_4161B0 | `game_finalize` | src/game_init.c | ported | calls sub_415FB0("DATAS>MENUS>FONTE11", &g_unk_4CFE40); font descriptor fully built |
| 00426335 | sub_426335 | — | — | todo | unknown; called from game_init phase 39 |
| 00471C18 | sub_471C18 | — | — | todo | loads objets>objets.txt |
| 0045B207 | sub_45B207 | — | — | todo | unknown; called from game_init phase 43 |
| 00491ED0 | sub_491ED0 | — | — | todo | 16-bit colour init step 1 |
| 00491FD0 | sub_491FD0 | — | — | todo | 16-bit colour init step 2; push 0x3C |
| 0045018B | sub_45018B | — | — | todo | material/resource loader; loads FLAM.MAT |
| — | set_entities_combat_mode | — | — | todo | sets combat mode for all entities |
| — | initialize_resource | — | — | todo | resource init returning palette_data handle |
| — | system_get_gametime | — | — | todo | reads system game timer |
| — | update_palette_with_default_colors | — | — | todo | applies default colour adjustments to palette |
| — | ls_build_palette_colour | — | — | todo | builds display palette from data |
| 0047DC3C | ls_bit_move | `ls_bit_move` | src/libsys/graphics/graphic.c | ported | rect blit src→dst bmap with clip; mode 0=opaque, mode 1=colour-key; mode 2 (alpha) TODO; cross-format (lsi_remap_bmap) TODO |
| 0047D466 | ls_box | `ls_box` | src/libsys/graphics/graphic.c | stub | 1-pixel outline rectangle in dst pixel buffer; original ls_fill_rect/ls_invert_rect sub-paths not ported |
| 00453FA3 | draw_image | `draw_image` | src/libsys/graphics/graphic.c | ported | thin wrapper: optional ls_box debug outline (TODO: wire debug_surface), then ls_bit_move mode=0; logs invocation |
| — | blit_bmap_to_framebuffer | `blit_bmap_to_framebuffer` | src/libsys/graphics/graphic.c | ported | expand 8bpp indexed bmap → u32 XRGB front_buffer via palette lookup; clips to fb bounds; fires every frame in main loop |
| — | game_init_get_menu_backdrop | `game_init_get_menu_backdrop` | src/game_init.c | ported | returns g_loading_bmap + color_table pointers for ls_main backdrop blit |

### Facing tests

| addr | asm name | C name | file | status | notes |
|---|---|---|---|---|---|
| 0044599B | sub_44599B | `cb_query_954` | src/lieu_cb.c | ported | DirectionPerso: target is a persona (arg >= 0) or an object named "OB%02d" / "IM%02d" (arg < 0, -arg > 1000 selects IM with -arg-1000). Returns true unconditionally within 30000 world units (dbl_4B0678 = 9e8 squared), otherwise abs(shortest arc) < 0x2400 |
| 00445C82 | sub_445C82 | `cb_query_944` | src/lieu_cb.c | partial | DirectionZone: prefers the entity attached to the zone record (dword_506CA0[n], zone struct +0x20) and falls back to the zone centre (dword_506C80[n] +0 / +8); the port only tracks centres. No distance bypass |
| 00445E99 | sub_445E99 | `cb_query_998` | src/lieu_cb.c | stub | DirectionSpec: target resolution not decoded; stays permissive |
| — | — | `bam_angle` | src/lieu_cb.c | ported | -(atan2(dx, dz) * 65536 / 2pi) & 0xFFFF. The X delta is atan2's FIRST argument (asm 0x445BBE pushes dz then dx) |

Zone record stride is 0x24: centre X at +0, centre Z at +8, attached entity at +0x20.

### Navigation / collision grid — see [functions/nav_collision.md](functions/nav_collision.md)

| addr | asm name | C name | file | status | notes |
|---|---|---|---|---|---|
| 0045619D | sub_45619D | `nav_grid_build` | src/nav_grid.c | ported | rasterises the room polygon + INF collision zones into the 64x64 byte grid `interaction_data_array` @ 0x4FA0F0, indexed via global_x_offset/divisor @ 0x4F945C/0x4F5E28 and their Z pair. Grid axes are world (X, Z) — the struct field the port calls `min_y` holds Z |
| 00456FB7 | sub_456FB7 | `nav_snap_to_walkable` | src/nav_grid.c | ported | snap a point out of blocked space: ring outward (3, 5, 7, ...) until a free cell is found, rewrite the point to that cell's origin. Port bounds the ring count; the original loops forever if no cell is free |
| — | — | `nav_is_walkable` / `nav_cell` | src/nav_grid.c | ported | grid lookups. Only NAV_FREE passes, matching the original's `grid != 0` blocked test |
| 0045720D | sub_45720D | — | — | todo | per-entity reachability flood-fill into the 0x1000-byte workspace at entity+0xCD8, seeded at the entity's cell via sub_477FD4. NOTE: +0xCD8 is this nav workspace, not the "dialogue buffer" the port currently labels it |
| 00456E6A | sub_456E6A | — | — | todo | debug overlay: draws every grid cell's value via sub_45A976 |
| 00469222 | sub_469222 | — | — | todo | actor walk state machine. entity+0xCB4 set = follow the joined actor (resolves it by matching entity+0xCB8 against target+0xB0, faces it via sub_4440A6/sub_428714); otherwise runs the walk states at entity+0xCBC (1 = new request -> sub_468FFE waypoint -> state 2, 4 = arrived), with a 15 s idle timeout to sub_461BA0 |
| 00468FFE | sub_468FFE | — | — | todo | next-waypoint query used by the walk state machine |
| 00428714 | sub_428714 | — | — | todo | start an entity walking toward (x, z) |

### LIEU action queue ("les piles") — see [functions/lieu_action_queue.md](functions/lieu_action_queue.md)

| addr | asm name | C name | file | status | notes |
|---|---|---|---|---|---|
| 004441A5 | sub_4441A5 | `lieu_action_push` | src/lieu_action.c | ported | allocate a node in the active pile (dword_505C30); 3 piles x 100 nodes x 0x78; overflow -> scratch node + "la pile N est saturee". Port zeroes the node and defaults +0x40 to -1 (original leaves stale bytes) |
| 004442EE | sub_4442EE | `lieu_action_done` | src/lieu_action.c | ported | head action complete: carry the type to head+4 (dword_4FC1A4, MODIFYVAR excepted), shift the pile down, stamp the drain time, clear dword_4FFB50 when all three piles empty |
| 0044CDB8 | sub_44CDB8 | `lieu_action_run` | src/lieu_action.c | partial | per-frame drain of each enabled pile's head node. The 500 ms post-drain re-run of sub_44A2B5 is not ported (debug overlay only) |
| 0044A58B | sub_44A58B | `lieu_action_dispatch` | src/lieu_cb.c | partial | 40-case action switch. Implemented: ANIMATION, EVENEMENT, ETATOBJET, JOINDRE (case 8 @ 0x44B5E3, fully decoded), BRUITAGE, ANIMOBJET, STOPJOINDRE, VIRECOL, JOINDREPERM, LINKOBJET, PLAY/PAUSE/LOAD/FREEANIM. Others log and complete |
| 0044A2B5 | sub_44A2B5 | — | — | todo | debug overlay: draws the pile contents as text via draw_image/draw_text. Confirms node stride 0x78 |
| 0044968B | sub_44968B | `lieu_timer_add/remove` + internal `lieu_timer_tick` | src/lieu_action.c | partial | object-timer scan at 0x4499A5..0x449ADD ported: 20 records of 16 bytes at object_timers @ 0x4FFB90, expiry fires FonctionEditor opcode 4. Rest of the function (frame pacing, debug HUD) not ported |
| 0045D560 | sub_45D560 | — | — | todo | demo/replay recorder: every primitive fills dword_4F4CC0 (0x30 bytes) and calls this to ls_write the record plus player/companion transforms. Only active when dword_4B6FA4 == 1 |
| 00446A05 | sub_446A05 | `cb_state_toggle` | src/lieu_cb.c | ported | SELECTPILE: sets dword_505C30 + the pile's enable flag; mode 2 resets the pile (memset 0x2EE0 to -1, count 0, enable 1) |
| 0044453E | sub_44453E | `cb_named_object` | src/lieu_cb.c | partial | ANIMATION: lowercase, '2' = 2D sprite (actor -2), 's' prefix = sound flag +0x70, "_NN" suffix -> +0x5C, "ar*" holds 500 ms. The ANI load path (sub_41333A + anim2d slot list) is not replayed |
| 00444A3E | sub_444A3E | `cb_named_two_ints` | src/lieu_cb.c | ported | EVENEMENT: id -> +0x48, delay -> +0x40; dispatch arms/cancels an object timer |
| 00445189 | sub_445189 | `cb_named_ex_988` | src/lieu_cb.c | ported | ETATOBJET: _strupr name -> +0x30; p/a1/a2 scaled x600 into +0x14/+0x18/+0x1C; a3/a4/a5 -> +0x20/+0x24/+0x28; a6 -> +0x0C; a7 -> +0x54 |
| 004453E1 | sub_4453E1 | `cb_actor_934` | src/lieu_cb.c | ported | JOINDRE: abs(actor) -> +0x0C, X*600 -> +0x14, Z*600 -> +0x1C, target -> +0x10, facing -> +0x24. Arg 2 is a COORDINATE; arg 4 is the facing. Refuses when the actor is already walking and the id is not negative |
| 0044569A | sub_44569A | `cb_place_95C` | src/lieu_cb.c | partial | BRUITAGE: name/source/coord queued; the 'Z'-prefix zone-sound path (sub_44D23D) is not ported |
| 00446159 | sub_446159 | `cb_cb_98C` | src/lieu_cb.c | ported | ANIMOBJET: name -> +0x30, p -> +0x20, a1 -> +0x24, a3 -> +0x28, a2 -> +0x40 (hold ms) |
| 0044648B | sub_44648B | `cb_select_468` | src/lieu_cb.c | partial | STOPJOINDRE: id > 4 with an unloaded entity is refused ("Perso %d non charge"); negative id stops the actor immediately (clears entity+0xCB4/0xCBC blocks), non-negative queues the action |
| 0044660D | sub_44660D | `cb_cb_964` | src/lieu_cb.c | ported | VIRECOL: flag id -> +0x68, value -> +0x4C |
| 00446808 | sub_446808 | `cb_flush_958` | src/lieu_cb.c | ported | JOINDREPERM: abs(actor) -> +0x0C, target -> +0x10; refused when the entity is missing, or already joined and the id is not negative |
| 00446BE9 | sub_446BE9 | `cb_attach_99C` | src/lieu_cb.c | ported | LINKOBJET: _strupr name -> +0x30, holder -> +0x0C, a -> +0x54, b -> +0x5C |
| 00443273 | sub_443273 | `cb_cb_950` | src/lieu_cb.c | ported | FREEANIM (type 0x27): slot -> +0x48 |
| 0044356F | sub_44356F | `cb_cb_97C` | src/lieu_cb.c | ported | PLAYANIM (type 0x23): slot -> +0x48, mode -> +0x70; slots >= 20 rejected ("Plantage! animation invalid") |
| 004440A6 | sub_4440A6 | `bam_angle` | src/lieu_cb.c | ported | angle from one world point to another, BAM u16; used by JOINDRE to face a target or zone |
| — | action type table @ 0x4B5438 | `LA_NAMES` / `lieu_action_name` | src/lieu_action.c | ported | 40 entries of 16 bytes: ANIMATION..FREEANIM. Index == action type == the engine callback slot the script called |

Update this table whenever a function's status changes.
