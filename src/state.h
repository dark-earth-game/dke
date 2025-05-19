#pragma once

#include "libsys/common.h"
#include "libsys/system.h"
#include "graphics/renderer.h"

#include "game_detection.h"

typedef struct {
    game_type_e game_type;
    system_t system;
    renderer_t renderer;
} state_t;

extern state_t *state;

void state_init(state_t *state);
void state_release(state_t *state);
