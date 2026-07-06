#include <stdio.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/memory.h"
#include "libsys/system.h"
#include "libsys/file_system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/shading.h"
#include "libsys/audio/audio.h"
#include "libsys/mouse.h"
#include "libsys/sound.h"
#include "libsys/cd.h"
#include "libsys/joy.h"
#include "libsys/movie.h"

#include "state.h"
#include "game_detection.h"


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
    // sub_487E30();
    ls_init_memory_manager();
    // sub_485840();
    // sub_487D40();
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

    // sub_4843F0(v13, v14);
    // sub_4953B0();
    ls_set_title(system, state->title);
    // sub_491450();
    // sub_495390();

    if (state->audio_enabled) {
        // Initialize audio system
        if (ls_init_audio(audio, 22050, 2, 16)) {
            print("Audio system initialized successfully.\n");
        } else {
            print("Failed to initialize audio system.\n");
        }
    }

    init_all_systems(system);

    ls_init_movie(0, 0);

    while(!system->quit) {
        system->timer.tick = ls_tick();
        ls_update_events(system);

        //

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
