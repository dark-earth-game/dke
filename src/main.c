#include <stdio.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/memory.h"
#include "libsys/system.h"
#include "libsys/file_system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/shading.h"
#include "libsys/graphics/colour_tables.h"
#include "libsys/audio/audio.h"
#include "libsys/mouse.h"
#include "libsys/sound.h"
#include "libsys/cd.h"
#include "libsys/joy.h"
#include "libsys/movie.h"
#include "libsys/lsys_startup.h"
#include "libsys/graphics/font.h"
#include "libsys/graphics/palette.h"

#include "state.h"
#include "game_detection.h"
#include "game_init.h"
#include "menu.h"
#include "player.h"
#include "scene.h"
#include "libsys/graphics/graphic.h"


state_t *state;

/**
 * Load configuration from win.ini file
 * Format:
 * [LIBSYS]
 * DEBUG=TRUE
 * [DKE]
 * HICOLOR=TRUE
 * DEMO=FALSE
 * DEBUG=TRUE
 * KEY=TRUE
 */
// 0046091B
void load_configuration(state_t *state) {
    c8 *config_path[256];
    sprintf((char *)config_path, "%s/win.ini", state->game_path);
    if (ls_exists((const c8 *)config_path)) {
        FILE *file = ls_open((const c8 *)config_path, "r");
        if (file) {
            char line[256];
            c8 current_section[64] = {0};
            while (fgets(line, sizeof(line), file)) {
                c8 key[64];
                c8 value[64];
                c8 trimmed[256];

                sscanf(line, "%[^\n]", trimmed);

                if (trimmed[0] == '[' && trimmed[strlen(trimmed) - 1] == ']') {
                    sscanf(trimmed, "[%[^]]", current_section);
                } else if (sscanf(trimmed, "%[^=]=%s", key, value) == 2) {
                    if (strcmp(current_section, "LIBSYS") == 0) {
                        if (strcmp(key, "DEBUG") == 0) {
                            state->ls_debug_enabled = (strcmp(value, "TRUE") == 0);
                        } else if (strcmp(key, "Master") == 0) {
                            state->ls_master_enabled = (strcmp(value, "TRUE") == 0);
                        } else if (strcmp(key, "Info") == 0) {
                            state->ls_info_enabled = (strcmp(value, "TRUE") == 0);
                        }
                    } else if (strcmp(current_section, "DKE") == 0) {
                        if (strcmp(key, "HICOLOR") == 0) {
                            state->colour_mode = (strcmp(value, "TRUE") == 0);
                        }
                        else if (strcmp(key, "DEMO") == 0) {
                            state->is_demo = (strcmp(value, "TRUE") == 0);
                        }
                        else if (strcmp(key, "DEBUG") == 0) {
                            state->debug_enabled = (strcmp(value, "TRUE") == 0);
                        }
                        else if (strcmp(key, "KEY") == 0) {
                            state->use_key = (strcmp(value, "TRUE") == 0);
                        }
                    }
                }
            }
            print("Configuration loaded from %s\n", (char *)config_path);
            ls_close(file);
        }
    }
    else {
        print("Configuration file not found: %s\n", (char *)config_path);
    }
}

/**
 * Load DKE.CFG game-side configuration
 * Format:
 * // comments
 * Key : Value
 * Ends at CONEX_SAID_END_OF_FILE
 */
void load_dke_config(state_t *state) {
    c8 *config_path[256];
    sprintf((char *)config_path, "%s/DKE.CFG", state->game_path);
    if (ls_exists((const c8 *)config_path)) {
        FILE *file = ls_open((const c8 *)config_path, "r");
        if (file) {
            char line[256];
            while (fgets(line, sizeof(line), file)) {
                char *trimmed = line;
                // skip leading whitespace
                while (*trimmed == ' ' || *trimmed == '\t') {
                    trimmed++;
                }
                // skip blank lines and comments
                if (*trimmed == '\0' || *trimmed == '\n' || (trimmed[0] == '/' && trimmed[1] == '/')) {
                    continue;
                }
                // check for end sentinel
                if (strncmp(trimmed, "CONEX_SAID_END_OF_FILE", 22) == 0) {
                    break;
                }
                // parse Key : Value
                c8 key[64];
                i32 value;
                if (sscanf(trimmed, " %63[^: ] : %d", key, &value) == 2) {
                    if (strcmp(key, "FirstRoom") == 0) {
                        state->first_room = value;
                    }
                    else if (strcmp(key, "Lum") == 0) {
                        state->lum = value;
                    }
                    else if (strcmp(key, "FirstPerso") == 0) {
                        state->first_perso = value;
                    }
                    else if (strcmp(key, "FirstObject") == 0) {
                        state->first_object = value;
                    }
                    else if (strcmp(key, "Brightness") == 0) {
                        state->brightness = value;
                    }
                    else if (strcmp(key, "Contrast") == 0) {
                        state->contrast = value;
                    }
                    else if (strcmp(key, "Shadow") == 0) {
                        state->shadow = value;
                    }
                    else if (strcmp(key, "Detail") == 0) {
                        state->detail = value;
                    }
                    else if (strcmp(key, "Language") == 0) {
                        state->language_id = value;
                    }
                    else {
                        print("Unknown DKE.CFG key: %s\n", key);
                    }
                }
            }
            print("DKE.CFG loaded from %s\n", (char *)config_path);
            ls_close(file);
        }
    }
}

// 0047CBA0
void ls_startup(system_t *system) {
    // dword_55C790 = 0;
    ls_libsys_init_flags(state);   /* sub_487E30 — apply LIBSYS flags, detect lang */
    ls_init_memory_manager();
    ls_build_virtual_root();       /* sub_485840 — build g_virtual_root */
    ls_libsys_flag_reset();        /* sub_487D40 — reset g_ls_pause_state */
    lsys_init_mouse_system();
    ls_init_graphic_system(&state->graphic, system);
    ls_init_sound_system();
    if (state->audio_enabled) {
        ls_init_audio(&state->audio, 22050, 2, 16);
    }
    lsys_init_cd_system();
    lsys_init_joy_system();

    print("System started successfully.\n");
}

// 00491E20
void init_all_systems(system_t *system) {
    ls_startup(system);
    /* Set video mode using dimensions and colour depth from system_t.
     * bit_depth: 16 when colour_mode == 1 (hicolor), 8 otherwise.
     * bpp_flags: 0x200 (original default for non-movie mode).
     * ls_set_video_mode creates g_graphics_context + g_back_screen internally. */
    i32 bit_depth = (state->colour_mode) ? 16 : 8;
    ls_set_video_mode(system->width, system->height, bit_depth, 0x200);

    preload_shading_table();

    /* ls_main+138: 16-bit colour path — build colour mapping tables */
    if (state->colour_mode) {
        initialize_color_mapping_tables();
    }

    /* Font subsystem — must follow colour table init */
    init_font_system();
}

// 00460B6C
i32 ls_main(system_t *system, c8 **argv) {
    audio_t *audio = &state->audio;

    state->game_path = ls_get_base_path();
    load_configuration(state);
    load_dke_config(state);

    // force colour mode to 16-bit
    state->colour_mode = true;

    // skip memory check

    // sub_4843F0(v13, v14); — CPU/memory check
    cpu_info_t cpu_info;
    ls_measure_cpu_speed(&cpu_info);
    print("ls_main: CPU speed ~%u MHz, perf_freq=%llu\n",
          cpu_info.cpu_speed_mhz, (unsigned long long)cpu_info.perf_counter_freq);

    // sub_4953B0(); — copy unknown buffers a→b
    ls_copy_unknown_a_to_b();

    ls_set_title(system, state->title);

    // sub_491450(); — copy alternative palette data to palette target buffer
    palette_copy_alternative();

    // sub_495390(); — install default error/warn handlers
    ls_install_default_error_handlers();

    if (state->audio_enabled) {
        // Initialize audio system
        if (ls_init_audio(audio, 22050, 2, 16)) {
            print("Audio system initialized successfully.\n");
        } else {
            print("Failed to initialize audio system.\n");
        }
    }

    init_all_systems(system);

    if (!game_init(state)) {
        print("game_init failed — cannot start game\n");
    }

    /* ls_main debug text — mirrors original ls_main reference:
     *   set_position_coordinates(0, 32); draw_text(aInitLibsysFini); */
    if (state->debug_enabled) {
        set_position_coordinates(0, 32);
        draw_text("Init libsys finished");
    }

    ls_init_movie(0, 0);

    /* Play intro cutscene chain. Original engine has multiple entry paths
     * (game_play_intro_469F32 does CD-drive detection first; sub_437DDD plays
     * EADKE.CRH from the main-menu "trailer" option). For now, play the
     * publisher/title splash (EADKE) at boot so we exercise the CRH decoder
     * + CoreAudio streaming end-to-end. ls_play_movie skips gracefully if
     * the file is missing rather than crash. */
    ls_play_movie("HDISK:>EARTH>sc>EADKE.CRH");

    /* --- Slice B: main menu state machine ---
     * menu_init fetches the MENUGEN backdrop from game_init (same source as
     * Slice A) and sets up the menu_t state.  menu_render owns the per-frame
     * backdrop blit + text overlay, replacing the raw Slice A blit. */
    menu_t menu;
    menu_init(&menu);

    while(!system->quit) {
        system->timer.tick = ls_tick();
        ls_update_events(system);

        /* Update hover / keyboard nav state */
        menu_update(&menu, system);

        /* Render menu vs scene depending on state. Once the user picks
         * NEW GAME, menu_dispatch calls scene_load_level and flips the
         * state to LOADING; from that frame on we blit the level's
         * camera-0 backdrop instead of the menu backdrop. */
        if (menu.state == MENU_STATE_LOADING) {
            /* scene_render returns 1 when Esc is pressed; flip back to
             * the main menu so the user can pick another action. */
            if (scene_render(system) == 1) {
                menu.state = MENU_STATE_MAIN;
                menu.dirty = 1;
                menu.startup_frames_remaining = 15;  /* re-arm click grace */
            }

            /* Slice D: keep polling WASD + mouse_left so the input path is
             * exercised.  No entity movement wired yet. */
            static player_input_t pinput;
            player_input_update(&pinput, system);
        } else {
            menu_render(&menu, system);
        }

        /* Exit main loop when user selects QUIT */
        if (menu.state == MENU_STATE_QUIT) {
            print("ls_main: menu QUIT selected, breaking loop\n");
            break;
        }

        /* Overlay debug text if enabled */
        if (state->debug_enabled) {
            set_position_coordinates(4, 4);
            draw_text("DKE - main menu");
        }

        ls_blit(system, state->graphic.front_buffer);
        ls_flip(system);

        system->timer.frame_count++;
        if (system->timer.tick - system->timer.last_tick_fps >= 1000) {
            snprintf(state->title, 64, "%s - fps: %d", system->title, system->timer.frame_count);
            ls_set_title(system, state->title);
            system->timer.frame_count = 0;
            system->timer.last_tick_fps = system->timer.tick;
        }

        if (system->actions[102] == true) {
            state->lock_fps = !state->lock_fps;
        }
        if (state->lock_fps) {
            system->timer.last_tick = ls_tick();
            // limit fps to 40
            if (system->timer.last_tick - system->timer.tick < 25) {
                ls_delay(25 - (system->timer.last_tick - system->timer.tick));
            }
        }
        else {
            ls_delay(1);
        }
    }
    ls_shut_movie();
    scene_release();
    menu_release();
    return 0;
}

#ifndef _WIN32
i32 main(i32 argc, c8 **argv) {
#else
#include <windows.h>
i32 CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, i32 nCmdShow) {
#endif
    state = memory_calloc(sizeof(state_t));
    state_init(state);
    state->game_type = detect_game();
    state->system.title = (c8*)get_game_title(state->game_type);

    system_t *system = &state->system;

    if (ls_init_instance(system, 640, 480, 4)) {
        // TODO: hide cursor
        ls_main(system, argv);
    }
    
    ls_release(system);
    state_release(state);
    
    return 0;
}
