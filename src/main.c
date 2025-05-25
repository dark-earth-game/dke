#include <stdio.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/file_system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/audio/audio.h"

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
            while (fgets(line, sizeof(line), file)) {
                c8 key[64];
                c8 value[64];

                sscanf(line, "%[^\n]", line);
                sscanf(line, "%[^=]=%s", key, value);

                if (strcmp(key, "[LIBSYS]") == 0) {
                    if (strcmp(key, "DEBUG") == 0) {
                        state->ls_debug_enabled = (strcmp(value, "TRUE") == 0);
                    }
                } else if (strcmp(key, "[DKE]") == 0) {
                    if (strcmp(value, "HICOLOR") == 0) {
                        state->colour_mode = (strcmp(value, "TRUE") == 0);
                    }
                    else if (strcmp(value, "DEMO") == 0) {
                        state->is_demo = (strcmp(value, "TRUE") == 0);
                    }
                    else if (strcmp(value, "DEBUG") == 0) {
                        state->debug_enabled = (strcmp(value, "TRUE") == 0);
                    }
                    else if (strcmp(value, "KEY") == 0) {
                        state->use_key = (strcmp(value, "TRUE") == 0);
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

// 0048F190
void preload_shading_table() {
    // TODO
}

// 0047CBA0
void ls_startup(system_t *system) {
    // dword_55C790 = 0;
    // sub_487E30();
    // ls_init_memory_manager(this);
    // sub_485840();
    // sub_487D40();
    // lsys_init_mouse_system();
    ls_init_graphic_system(&state->graphic, system);
    // ls_init_sound_system();
    if (state->audio_enabled) {
        ls_init_audio(&state->audio, 22050, 2, 16);
    }
    // lsys_init_cd_system();
    // lsys_init_joy_system();

    print("System started successfully.\n");
}

// 00491E20
void init_all_systems(system_t *system) {
    ls_startup(system);
    // ls_set_video_mode(v6, a2, a5);
    // dword_56A5E0 = ls_init_bmap_ex(v6, v7, a2, a4);

    preload_shading_table();
}

// 00460B6C
i32 ls_main(system_t *system, c8 **argv) {
    audio_t *audio = &state->audio;

    state->game_path = ls_get_base_path();
    load_configuration(state);

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

    // ls_init_movie(0, 0);

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
