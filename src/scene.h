#pragma once

/*
 * scene.h — Level loader / scene bootstrap
 *
 * Exports scene_load_level (0046DFEC) for wiring into the main game loop.
 * See src/scene.c for implementation notes and TODO stubs.
 */

#include "libsys/common.h"

/*
 * 0046DFEC — scene_load_level_46DFEC
 *
 * Loads the LEVEL_ID-th level's config + assets from DATAS/L%02d/.
 * Sets world_state, world_controller, player_entity globals and
 * zeroes related counters before handing off to rendering.
 *
 * Returns 0 on success, non-zero on error (file missing = log + continue;
 * other errors log and return 1).
 *
 * Skeleton pass: logs each phase, reads L%02d.INF header, stubs all
 * heavy sub-function calls with TODO comments.
 */
i32 scene_load_level(i32 level_id);
