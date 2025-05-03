#include <stdio.h>

#include "core/common.h"
#include "core/system.h"
#include "state.h"
#include "game.h"
#include "game_detection.h"


c8 title[64];
state_t *state;

b8 lock_fps = false;

#ifndef _WIN32
i32 main(i32 argc, c8 **argv) {
#else
#include <windows.h>
i32 CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, i32 nCmdShow) {
#endif
    print("Initializing...\n");
    state = memory_calloc(sizeof(state_t));
    state_init(state);
    print("Detecting Game...\n");
    state->game_type = detect_game();

    system_t *system = &state->system;

    system_init(system, "DKE: Dark Earth", 640, 480, 4);
    game_init(state);

    while(!system->quit) {
        system->timer.tick = system_tick();
        system_events(system);

        game_update(state);
        game_draw(state);
        game_flip(state);

        // system_blit(system, renderer->front_buffer);
        system_flip(system);

        system->timer.frame_count++;
        if (system->timer.tick - system->timer.last_tick_fps >= 1000) {
            snprintf(title, 64, "%s - fps: %d", system->title, system->timer.frame_count);
            system_set_title(system, title);
            system->timer.frame_count = 0;
            system->timer.last_tick_fps = system->timer.tick;
        }

        if (system->actions[102] == true) {
            lock_fps = !lock_fps;
        }
        if (lock_fps) {
            system->timer.last_tick = system_tick();
            // limit fps to 40
            if (system->timer.last_tick - system->timer.tick < 25) {
                system_delay(25 - (system->timer.last_tick - system->timer.tick));
            }
        }
        else {
            system_delay(1);
        }
    }

    game_release(state);
    system_release(system);
    state_release(state);

    return 0;
}
