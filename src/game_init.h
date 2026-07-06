#pragma once

/*
 * game_init.h — game-side bootstrap (0046BEEB)
 *
 * game_init() runs after ls_startup() + init_all_systems() complete.
 * It wires up the HUD (health, GINETTE.CIF), weapons inventory,
 * entity tables, combat buffers, render bmaps, font, and palette.
 *
 * All work previously done in game_init_46BEEB (called from ls_main+66D).
 */

#include "libsys/common.h"
#include "state.h"

/* 0046BEEB — game-side bootstrap. Runs after ls_startup + init_all_systems. */
b8 game_init(state_t *state);
