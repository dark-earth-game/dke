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
#include "libsys/graphics/graphic.h"
#include "state.h"

/* 0046BEEB — game-side bootstrap. Runs after ls_startup + init_all_systems. */
b8 game_init(state_t *state);

/*
 * game_init_get_menu_backdrop — return loaded backdrop bmap and palette.
 *
 * Provides the LOADING.GIF bmap and MENUGEN.GIF palette loaded during
 * game_init phase 3 and phase 7 respectively.  Used by ls_main to blit
 * a visible menu backdrop into the front_buffer after game_init completes.
 *
 * @param out_bmap     receives pointer to g_loading_bmap (may be NULL if load failed)
 * @param out_palette  receives pointer to g_menugen_palette (768-byte RGB table)
 */
void game_init_get_menu_backdrop(bmap_t **out_bmap, u8 **out_palette);
