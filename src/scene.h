#pragma once

/*
 * scene.h — Level loader / scene bootstrap
 *
 * Exports scene_load_level (0046DFEC) for wiring into the main game loop.
 * See src/scene.c for implementation notes and TODO stubs.
 */

#include "libsys/common.h"
#include "libsys/system.h"

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

/*
 * scene_render — per-frame render of the currently loaded scene.
 *
 * First-camera pass: blits the level's FL<lvl>C0L0.CIF backdrop into the
 * front buffer using its embedded CIF palette.  No entities, no fade —
 * just a static room.  Enough to prove NEW GAME reaches level data instead
 * of stalling on the "LOADING LEVEL N" text.
 *
 * Camera index (0..6 per L01) can later be switched by input.  For now the
 * scene always displays camera 0.
 */
void scene_render(system_t *sys);

/*
 * scene_release — free any bmaps loaded by scene_load_level / scene_render.
 * Call before returning to the menu or on shutdown.
 */
void scene_release(void);
