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
 * Blits the current camera's foreground CIF into the front buffer.
 * Handles input: Left/Right arrows cycle camera 0..6; Esc requests a
 * return to the main menu.
 *
 * Return value:
 *   0 — stay in scene state
 *   1 — user pressed Esc; caller should flip menu.state back to MAIN
 */
i32 scene_render(system_t *sys);

/*
 * scene_release — free any bmaps loaded by scene_load_level / scene_render.
 * Call before returning to the menu or on shutdown.
 */
void scene_release(void);

/*
 * scene_get_camera_zfar — far-plane world distance for a camera as read
 * from L<lvl>.DAT (zbufferN). Returns 1000 (a sane default for L01-ish
 * rooms) when the DAT config wasn't loaded or the index is out of range.
 * Callers use this to un-normalise the 8-bit Z bmap into world depth.
 */
i32 scene_get_camera_zfar(i32 camera);

/* Weapon system — degradable object pool + weapon entity lifecycle.
 * Used by player.c during player_entity_init_secondary. */
void *scene_find_degradable_object(const c8 *name);
void  scene_weapon_entity_init(void *degrad_obj);
void  scene_weapon_entity_attach(void *entity, void *weapon_obj, i32 full_init);
