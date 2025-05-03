#pragma once

#include "core/common.h"
#include "core/system.h"
#include "game_detection.h"

typedef struct {
    system_t system;
    game_type_e game_type;
} state_t;

extern state_t *state;

void state_init(state_t *state);
void state_release(state_t *state);
