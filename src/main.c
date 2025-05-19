#include <stdio.h>

#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/graphics/renderer.h"

#include "state.h"
#include "game_detection.h"


c8 title[64];
state_t *state;

b8 lock_fps = false;

i32 ls_main(system_t *system, c8 **argv) {
    renderer_t *renderer = &state->renderer;
    renderer_init(renderer, system);

    while(!system->quit) {
        system->timer.tick = ls_tick();
        ls_update_events(system);

        //

        ls_blit(system, renderer->front_buffer);
        ls_flip(system);

        system->timer.frame_count++;
        if (system->timer.tick - system->timer.last_tick_fps >= 1000) {
            snprintf(title, 64, "%s - fps: %d", system->title, system->timer.frame_count);
            ls_set_title(system, title);
            system->timer.frame_count = 0;
            system->timer.last_tick_fps = system->timer.tick;
        }

        if (system->actions[102] == true) {
            lock_fps = !lock_fps;
        }
        if (lock_fps) {
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
