/*
 * scene.c — Level loader skeleton  (0046DFEC)
 *
 * Ported from scene_load_level_46DFEC in disassembly/dkev.asm
 * (lines 147869–148134, ~265 lines of asm).
 *
 * Slice C — first pass (skeleton).
 *
 * Scope for this pass:
 *   1. Log the level path being loaded.
 *   2. Read L%02d.INF header bytes into a heap buffer (if present).
 *   3. Set state globals to sane defaults so subsequent code won't crash.
 *   4. Stub all heavy sub-function calls with // TODO: sub_XXXXXX comments.
 *
 * NOT ported in this pass:
 *   - Entity spawn / geometry parsing
 *   - Rendering setup (render_buffer, graphics_context wiring)
 *   - Full world_controller allocation (sub_46D305)
 *   - Texture-library wiring (dword_507480 / can_do_32k)
 *
 * Asm parameter mapping:
 *   ECX = level_id (var_4 in IDA local frame)
 *
 * Globals identified in asm:
 *   world_state          — set to level_id
 *   world_controller     — tested (non-null branch skips dword_543C60 memset)
 *   player_entity+0xD04  — zeroed
 *   player_entity+0xD08  — set from player_entity+0xD04
 *   dword_4EDC30/34/38   — zeroed
 *   byte_4B7280          — set to 1
 *   byte_511760          — set to 0
 *   dword_505C54         — set to 0
 *   dword_508E18         — not touched in skeleton pass (sub_46D0EA owns it)
 *
 * Files loaded (per asm strings):
 *   DATAS>L%02d             — level asset folder existence check
 *   L%02d.INF               — level config header  (read in skeleton pass)
 *   L%02d.DAT               — level geometry data  (TODO)
 *   IMAGES/                 — level image directory (TODO)
 *   PAL/                    — level palette directory (TODO)
 *   ANIM2D/                 — 2-D animations directory (TODO)
 *   IM%02d.OME              — per-level overlay image (TODO)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "libsys/graphics/graphic.h"
#include "state.h"
#include "scene.h"
#include "game_init.h"

/* -------------------------------------------------------------------------
 * File-local globals — mirrors of asm data symbols.
 * These are intentionally simple value holders until the full port lands.
 * ------------------------------------------------------------------------- */

/*
 * world_state — current level id; set by scene_load_level, read by entity
 * system and rendering to know which level assets are active.
 * Mirrors asm symbol "world_state".
 */
static i32 g_world_state = 0;

/*
 * world_controller — opaque pointer to the level's world-control block.
 * Allocated/populated by sub_46D305 (TODO stub).
 * 0 = not yet allocated (triggers memset of dword_543C60 in asm).
 * Mirrors asm symbol "world_controller".
 */
static void *g_world_controller = NULL;

/*
 * dword_4EDC30/34/38 — per-level counters zeroed at load time.
 */
static i32 g_dword_4EDC30 = 0;
static i32 g_dword_4EDC34 = 0;
static i32 g_dword_4EDC38 = 0;

/*
 * byte_4B7280 — set to 1 by scene_load_level before asset loads.
 */
static u8 g_byte_4B7280 = 0;

/*
 * byte_511760 — set to 0 at level load start.
 */
static u8 g_byte_511760 = 0;

/*
 * dword_505C54 — zeroed at entry.
 */
static i32 g_dword_505C54 = 0;

/* -------------------------------------------------------------------------
 * External globals referenced from asm — declared here as opaque pointers
 * so the skeleton links without error.  They will be properly typed when the
 * entity / rendering subsystems land in later slices.
 * ------------------------------------------------------------------------- */

/*
 * player_entity — opaque pointer; we only write to offsets 0xD04 and 0xD08
 * in this skeleton pass, so no full struct definition is needed yet.
 *
 * Currently NULL (g_player_entity in game_init.c is static and not yet exported).
 * Writes to player_entity offsets are guarded by NULL-check so they are safe
 * no-ops until the entity system lands in Slice D.
 *
 * TODO: export g_player_entity from game_init.[ch] and link here.
 */
void *player_entity = NULL;

/* state singleton — defined in main.c; extern declared in state.h (already included) */

/* -------------------------------------------------------------------------
 * Scene render state — first-camera backdrop only for now.
 *
 * The real engine loads seven per-camera images (FL01C0L0..FL01C6L0),
 * plus L (mid) and Z (depth) layers per camera, driven by sub_46D0EA.
 * We haven't ported the asset loader yet, so the first playable frame just
 * blits camera 0's foreground image full-screen.
 * ------------------------------------------------------------------------- */

/* Per-camera assets. Loaded together by scene_load_camera_backdrop:
 *   F (foreground) — 640×480 8bpp CIF, the coloured room image.
 *   L (light map)  — 320×240 8bpp CIF, grayscale palette; each pixel
 *                    modulates F brightness (128 = neutral, 0 = black,
 *                    255 = 2× bright).  Nearest-upscaled ×2 at blit.
 *   Z (depth)      — 640×480 8bpp CIF, depth per pixel (0=near, 255=far).
 *                    Held for future entity-vs-background occlusion.
 * Any of L/Z may be NULL if the CIF is absent; F is required to render.
 * All freed in scene_release. */
static bmap_t *g_scene_bg_bmap = NULL;   /* F */
static bmap_t *g_scene_l_bmap  = NULL;   /* L */
static bmap_t *g_scene_z_bmap  = NULL;   /* Z */

/* Level id whose backdrop is currently in g_scene_bg_bmap.  -1 = none. */
static i32 g_scene_bg_level = -1;

/* Camera index within the level (0..6 for L01). Runtime-cycled via
 * scene_render input handling (Left/Right arrow keys). */
static i32 g_scene_camera = 0;

/* Highest camera index we've successfully loaded so far, used to cap
 * cycling. Updated by scene_load_camera_backdrop on the first successful
 * load. -1 = unknown; we'll probe on wrap. */
static i32 g_scene_camera_max = 6;   /* L01 has cameras 0..6 */

static void scene_free_bmap(bmap_t **pb) {
    if (!pb || !*pb) return;
    bmap_t *b = *pb;
    if (b->owns_buffer  && b->pixels)  free(b->pixels);
    if (b->owns_palette && b->palette) free(b->palette);
    free(b);
    *pb = NULL;
}

/*
 * scene_load_camera_backdrop — load DATAS>L<lvl>>IMAGES>FL<lvl>C<cam>L0.CIF
 * into g_scene_bg_bmap, freeing any previously-loaded backdrop.  Called by
 * scene_load_level (initial) and scene_render (camera cycle).
 *
 * Returns 1 on success, 0 on failure (bmap left untouched).
 */
static i32 scene_load_camera_backdrop(i32 level_id, i32 camera) {
    c8 vpath[80];

    /* F layer — required. */
    snprintf(vpath, sizeof(vpath),
             "DATAS>L%02d>IMAGES>FL%02dC%dL0.CIF",
             level_id, level_id, camera);
    print("scene: loading F '%s'\n", vpath);
    bmap_t *fb = game_init_load_cursor(vpath);
    if (!fb) {
        print("scene: F load failed for '%s'\n", vpath);
        return 0;
    }

    /* L layer — grayscale light map at half res. Optional. */
    snprintf(vpath, sizeof(vpath),
             "DATAS>L%02d>IMAGES>LL%02dC%dL0.CIF",
             level_id, level_id, camera);
    bmap_t *lb = game_init_load_cursor(vpath);
    if (lb) {
        print("scene: L loaded -> %dx%d\n", (i32)lb->width, lb->height);
    } else {
        print("scene: L absent — rendering without light modulation\n");
    }

    /* Z layer — depth. Optional, currently unused (reserved for entity
     * occlusion). */
    snprintf(vpath, sizeof(vpath),
             "DATAS>L%02d>IMAGES>ZL%02dC%dL0.CIF",
             level_id, level_id, camera);
    bmap_t *zb = game_init_load_cursor(vpath);
    if (zb) {
        print("scene: Z loaded -> %dx%d (stored for later occlusion)\n",
              (i32)zb->width, zb->height);
    } else {
        print("scene: Z absent — no depth buffer for this camera\n");
    }

    /* Swap in atomically. */
    scene_free_bmap(&g_scene_bg_bmap);
    scene_free_bmap(&g_scene_l_bmap);
    scene_free_bmap(&g_scene_z_bmap);
    g_scene_bg_bmap  = fb;
    g_scene_l_bmap   = lb;
    g_scene_z_bmap   = zb;
    g_scene_bg_level = level_id;
    g_scene_camera   = camera;
    print("scene: camera %d loaded (F %dx%d, L=%s, Z=%s)\n",
          camera, (i32)fb->width, fb->height,
          lb ? "yes" : "no", zb ? "yes" : "no");
    return 1;
}

/* -------------------------------------------------------------------------
 * Stub declarations for sub-functions called by scene_load_level_46DFEC.
 * Each corresponds to one asm CALL that is NOT ported in this skeleton pass.
 * They are static no-ops here so the skeleton compiles cleanly.
 * Replace with real implementations in subsequent Slice C passes.
 * ------------------------------------------------------------------------- */

/* TODO: set_display_mode — ECX=0x1D sets hardware display mode
 * Mirrors asm: mov ecx, 1Dh / call set_display_mode */
static void stub_set_display_mode(i32 mode) {
    (void)mode;
    /* TODO: set_display_mode */
}

/* TODO: sub_469E34 — ECX=1; called when level folder NOT found.
 * Likely triggers fallback asset path or error screen.
 * Mirrors asm: mov ecx, 1 / call sub_469E34 */
static void stub_sub_469E34(i32 arg) {
    (void)arg;
    /* TODO: sub_469E34 */
}

/* TODO: handle_data_l44_faux — called when level_id == 0x2C (44).
 * Likely special-case for a test/dummy level.
 * Mirrors asm: cmp [ebp+var_4], 2Ch / call handle_data_l44_faux */
static void stub_handle_data_l44_faux(void) {
    /* TODO: handle_data_l44_faux */
}

/* TODO: sub_46D305 — world controller allocation.
 * Called after memset of dword_543C60 (0xF00 bytes) when world_controller==0.
 * Mirrors asm: push 0F00h / push 0 / push offset dword_543C60 / call _memset
 *              then call sub_46D305 */
static void stub_sub_46D305(void) {
    /* TODO: sub_46D305 — allocate + init world_controller */
}

/* TODO: sub_46D0EA — asset loader dispatcher.
 * Loads L%02d.DAT, IMAGES/, PAL/, ANIM2D/, IM%02d.OME.
 * Mirrors asm: call sub_46D0EA */
static void stub_sub_46D0EA(void) {
    /* TODO: sub_46D0EA — load level assets (DAT/IMAGES/PAL/ANIM2D/OME) */
}

/* TODO: can_do_32k — colour-mode capability check.
 * ECX=color_mode; result used to select texture_library entries.
 * Mirrors asm: mov ecx, color_mode / call can_do_32k */
static void stub_can_do_32k(i32 color_mode) {
    (void)color_mode;
    /* TODO: can_do_32k */
}

/* TODO: sub_4640BC — player entity placement in world.
 * Called when dword_4B727C != 0 (world controller valid).
 * Mirrors asm: push eax/edx ... / mov ecx, game_state / call sub_4640BC */
static void stub_sub_4640BC(void) {
    /* TODO: sub_4640BC */
}

/* TODO: sub_426D2C — unknown; called after sub_4640BC block.
 * Mirrors asm: call sub_426D2C */
static void stub_sub_426D2C(void) {
    /* TODO: sub_426D2C */
}

/* TODO: process_color_palette — per-entity palette processing.
 * Called for player_entity, companion_entity, secondary_companion_entity.
 * Mirrors asm: mov eax, player_entity / mov ecx, [eax+50h] / call process_color_palette */
static void stub_process_color_palette(void *entity) {
    (void)entity;
    /* TODO: process_color_palette */
}

/* TODO: render_sprite_time_based_effect — periodic animated effect.
 * Mirrors asm: call render_sprite_time_based_effect */
static void stub_render_sprite_time_based_effect(void) {
    /* TODO: render_sprite_time_based_effect */
}

/* TODO: sub_45619D — unknown; called after render_sprite_time_based_effect.
 * Mirrors asm: call sub_45619D */
static void stub_sub_45619D(void) {
    /* TODO: sub_45619D */
}

/* TODO: set_entities_combat_mode — enables combat for all entities.
 * Mirrors asm: call set_entities_combat_mode */
static void stub_set_entities_combat_mode(void) {
    /* TODO: set_entities_combat_mode */
}

/* TODO: sub_448F4D — unknown; called after set_entities_combat_mode.
 * Mirrors asm: call sub_448F4D */
static void stub_sub_448F4D(void) {
    /* TODO: sub_448F4D */
}

/* TODO: ls_clear_bmap — clear a bmap's pixel buffer to zero.
 * Called for graphics_context and game_context.
 * Mirrors asm: mov ecx, graphics_context / call ls_clear_bmap */
static void stub_ls_clear_bmap(void *bmap) {
    (void)bmap;
    /* TODO: ls_clear_bmap */
}

/* TODO: sub_45D27A — render buffer update.
 * EDX=1, ECX=render_buffer1.
 * Mirrors asm: mov edx, 1 / mov ecx, render_buffer1 / call sub_45D27A */
static void stub_sub_45D27A(void *buf, i32 mode) {
    (void)buf; (void)mode;
    /* TODO: sub_45D27A */
}

/* TODO: update_player_interface — HUD/inventory render.
 * EDX=2, ECX=game_context.
 * Mirrors asm: mov edx, 2 / mov ecx, game_context / call update_player_interface */
static void stub_update_player_interface(void *ctx, i32 mode) {
    (void)ctx; (void)mode;
    /* TODO: update_player_interface */
}

/* TODO: sub_45E070 — unknown; called after update_player_interface.
 * Mirrors asm: call sub_45E070 */
static void stub_sub_45E070(void) {
    /* TODO: sub_45E070 */
}

/* TODO: sub_47674F — unknown; ECX=0.
 * Mirrors asm: xor ecx, ecx / call sub_47674F */
static void stub_sub_47674F(i32 arg) {
    (void)arg;
    /* TODO: sub_47674F */
}

/* TODO: render_entity_with_ui — render companion entities with HUD.
 * Mirrors asm: xor edx, edx / mov ecx, companion_entity / call render_entity_with_ui */
static void stub_render_entity_with_ui(void *entity, i32 mode) {
    (void)entity; (void)mode;
    /* TODO: render_entity_with_ui */
}

/* TODO: apply_model_transform — apply transform matrix to entity.
 * Mirrors asm: mov edx, [eax+8Ch] / mov ecx, player_entity / call apply_model_transform */
static void stub_apply_model_transform(void *entity, void *transform) {
    (void)entity; (void)transform;
    /* TODO: apply_model_transform */
}

/* TODO: system_get_gametime — read current system game timer.
 * Mirrors asm: call system_get_gametime */
static i32 stub_system_get_gametime(void) {
    /* TODO: system_get_gametime */
    return 0;
}

/* TODO: restore_display_mode — EDX=0x1D, ECX=1.
 * Mirrors asm: mov edx, 1Dh / mov ecx, 1 / call restore_display_mode */
static void stub_restore_display_mode(i32 ecx, i32 edx) {
    (void)ecx; (void)edx;
    /* TODO: restore_display_mode */
}

/* TODO: sub_40636D — unknown tail call.
 * Mirrors asm: call sub_40636D */
static void stub_sub_40636D(void) {
    /* TODO: sub_40636D */
}

/* -------------------------------------------------------------------------
 * scene_load_level — 0046DFEC
 * ------------------------------------------------------------------------- */

// 0046DFEC
i32 scene_load_level(i32 level_id) {
    /*
     * Skeleton pass — mirrors the control flow and global writes of
     * scene_load_level_46DFEC without any real asset decode or entity spawn.
     *
     * Each phase is logged so /tmp/dke.log confirms execution order.
     */

    print("scene: scene_load_level(%d) — enter\n", level_id);

    /* -----------------------------------------------------------------
     * Phase 1 — set_display_mode(0x1D)
     * Asm: mov ecx, 1Dh / call set_display_mode
     * ----------------------------------------------------------------- */
    print("scene: phase 1 — set_display_mode(0x1D)\n");
    stub_set_display_mode(0x1D);

    /* -----------------------------------------------------------------
     * Phase 2 — zero dword_505C54
     * Asm: mov dword_505C54, 0
     * ----------------------------------------------------------------- */
    print("scene: phase 2 — zero dword_505C54\n");
    g_dword_505C54 = 0;

    /* -----------------------------------------------------------------
     * Phase 3 — build level folder path "DATAS>L%02d" and check existence
     * Asm: sprintf(game_temp_path, "DATAS>L%02d", level_id)
     *      then ls_is_path_exist(game_temp_path)
     * ----------------------------------------------------------------- */
    c8 level_folder[64];
    snprintf(level_folder, sizeof(level_folder), "DATAS>L%02d", level_id);
    print("scene: phase 3 — level folder path = '%s'\n", level_folder);

    /* Resolve virtual path to host path */
    c8 host_folder[512];
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, level_folder, host_folder, (i32)sizeof(host_folder))) {
        print("scene: path resolve failed for '%s'\n", level_folder);
        snprintf(host_folder, sizeof(host_folder), "%s/datas/L%02d", base, level_id);
    }
    print("scene: host folder = '%s'\n", host_folder);

    /* Check folder existence; if missing call stub_sub_469E34(1) */
    if (!ls_exists(host_folder)) {
        print("scene: level folder '%s' not found — calling sub_469E34(1)\n", host_folder);
        stub_sub_469E34(1);   /* TODO: sub_469E34 */
        /* Original asm continues (does not return) after this call */
    } else {
        print("scene: level folder exists\n");
    }

    /* -----------------------------------------------------------------
     * Phase 4 — special-case for level 44 (0x2C)
     * Asm: cmp [ebp+var_4], 2Ch / call handle_data_l44_faux
     * ----------------------------------------------------------------- */
    if (level_id == 0x2C) {
        print("scene: phase 4 — level 44 special case: handle_data_l44_faux\n");
        stub_handle_data_l44_faux();   /* TODO: handle_data_l44_faux */
    }

    /* -----------------------------------------------------------------
     * Phase 5 — zero byte_511760; zero player_entity offsets
     * Asm: mov byte_511760, 0
     *      cmp current_entity, 0 / jz skip
     *        mov [eax+0A0h], 0   ; current_entity->+0xA0 = 0
     * ----------------------------------------------------------------- */
    print("scene: phase 5 — zero byte_511760, clear current_entity->+0xA0\n");
    g_byte_511760 = 0;
    /* current_entity->+0xA0: handled by entity system (TODO) */

    /* -----------------------------------------------------------------
     * Phase 6 — set byte_4B7280=1, world_state=level_id, zero 4EDC30/34/38
     * Asm: mov byte_4B7280, 1
     *      mov world_state, eax        (eax = level_id)
     *      mov dword_4EDC30/34/38, 0
     * ----------------------------------------------------------------- */
    print("scene: phase 6 — set byte_4B7280=1, world_state=%d, zero 4EDC30/34/38\n",
          level_id);
    g_byte_4B7280 = 1;
    g_world_state = level_id;
    g_dword_4EDC30 = 0;
    g_dword_4EDC34 = 0;
    g_dword_4EDC38 = 0;

    /* -----------------------------------------------------------------
     * Phase 7 — zero player_entity+0xD04 and copy to +0xD08
     * Asm: mov [eax+0D04h], 0
     *      mov eax, [eax+0D04h] / mov [ecx+0D08h], eax
     * ----------------------------------------------------------------- */
    print("scene: phase 7 — zero player_entity+0xD04 / +0xD08\n");
    if (player_entity) {
        u8 *pe = (u8 *)player_entity;
        *(i32 *)(pe + 0xD04) = 0;
        *(i32 *)(pe + 0xD08) = *(i32 *)(pe + 0xD04); /* = 0 */
    }

    /* -----------------------------------------------------------------
     * Phase 8 — world_controller init
     * Asm: cmp world_controller, 0 / jnz skip
     *        memset(dword_543C60, 0, 0xF00)
     *        call sub_46D305
     * ----------------------------------------------------------------- */
    print("scene: phase 8 — world_controller check\n");
    if (!g_world_controller) {
        print("scene: world_controller is NULL — memset dword_543C60 + sub_46D305\n");
        /* memset dword_543C60 0xF00: TODO once dword_543C60 is declared */
        stub_sub_46D305();   /* TODO: sub_46D305 — world controller alloc */
    } else {
        print("scene: world_controller already set, skip alloc\n");
    }

    /* -----------------------------------------------------------------
     * Phase 9 — debug banner "Loading Gifs ..."
     * Asm: cmp debug_enabled, 0 / jz skip
     *        set_position_coordinates(0, 0x20)
     *        draw_text("Loading Gifs ...")
     * ----------------------------------------------------------------- */
    if (state && state->debug_enabled) {
        print("scene: phase 9 — debug: Loading Gifs ...\n");
    }

    /* -----------------------------------------------------------------
     * Phase 10 — sub_46D0EA (asset loader dispatcher)
     * Asm: call sub_46D0EA
     * Loads L%02d.DAT, IMAGES/, PAL/, ANIM2D/, IM%02d.OME
     * ----------------------------------------------------------------- */
    print("scene: phase 10 — sub_46D0EA (asset load dispatcher) — STUB\n");
    stub_sub_46D0EA();   /* TODO: sub_46D0EA */

    /* -----------------------------------------------------------------
     * Phase 11 — colour-mode / texture library setup
     * Asm: mov eax, color_mode
     *      mov eax, dword_4F9430[eax*4]  ; dbl_5115A8 source
     *      fild / fmul / fdivr / fstp dbl_5115A8
     *      texture_library[0..2] = dword_507480[color_mode*3 + 0..2]
     *      game_state_flags = dword_4DF590[color_mode*4]
     *      dword_4B7B0C = dword_4DD780[color_mode*4]
     *      can_do_32k(color_mode)
     * ----------------------------------------------------------------- */
    print("scene: phase 11 — colour-mode / texture library setup — STUB\n");
    stub_can_do_32k(state ? (i32)state->colour_mode : 1);   /* TODO: can_do_32k */

    /* -----------------------------------------------------------------
     * Phase 12 — sub_4640BC (player placement in world)
     * Asm: cmp dword_4B727C, 0 / jz skip
     *        push ... / mov ecx, game_state / call sub_4640BC
     * ----------------------------------------------------------------- */
    print("scene: phase 12 — sub_4640BC (player world placement) — STUB\n");
    stub_sub_4640BC();   /* TODO: sub_4640BC */

    /* -----------------------------------------------------------------
     * Phase 13 — sub_426D2C + damage counter zero
     * Asm: call sub_426D2C
     *      mov dword_506520, 0
     *      mov damage_counter, 0
     * ----------------------------------------------------------------- */
    print("scene: phase 13 — sub_426D2C, zero damage counters — STUB\n");
    stub_sub_426D2C();   /* TODO: sub_426D2C */

    /* -----------------------------------------------------------------
     * Phase 14 — player entity combat defaults
     * Asm: mov [eax+0C64h], 10000h
     *      mov [eax+0C68h], 4000h
     *      mov dword_4DD9B4, 1
     * ----------------------------------------------------------------- */
    print("scene: phase 14 — player entity combat defaults\n");
    if (player_entity) {
        u8 *pe = (u8 *)player_entity;
        *(i32 *)(pe + 0xC64) = 0x10000;
        *(i32 *)(pe + 0xC68) = 0x4000;
    }
    /* dword_4DD9B4 = 1 (TODO: add global) */

    /* -----------------------------------------------------------------
     * Phase 15 — process_color_palette for player + companions
     * Asm: mov ecx, [eax+50h] / call process_color_palette (×3)
     * ----------------------------------------------------------------- */
    print("scene: phase 15 — process_color_palette (player+companions) — STUB\n");
    stub_process_color_palette(player_entity);   /* TODO: process_color_palette */

    /* -----------------------------------------------------------------
     * Phase 16 — player entity position / viewport defaults
     * Asm: mov [eax+30h], 0FFFF4000h  ; x = -0xC000
     *      mov [eax+38h], 0FFFFC000h  ; y = -0x4000
     *      mov [eax+50h+54h], 0FFFF4000h
     *      mov [eax+50h+58h], 0FFFFC000h
     *      zero +0CECh/0CF4h/0CF0h/0CF8h
     *      mov [eax+0C04h], 0FFFFFFFFh
     *      mov [eax+64h], 80h
     * ----------------------------------------------------------------- */
    print("scene: phase 16 — player entity position defaults\n");
    if (player_entity) {
        u8 *pe = (u8 *)player_entity;
        *(i32 *)(pe + 0x030) = (i32)0xFFFF4000;
        *(i32 *)(pe + 0x038) = (i32)0xFFFFC000;
        *(i32 *)(pe + 0xCEC) = 0;
        *(i32 *)(pe + 0xCF4) = 0;
        *(i32 *)(pe + 0xCF0) = 0;
        *(i32 *)(pe + 0xCF8) = 0;
        *(i32 *)(pe + 0xC04) = (i32)0xFFFFFFFF;
        *(i32 *)(pe + 0x064) = 0x80;
    }

    /* -----------------------------------------------------------------
     * Phase 17 — render_sprite_time_based_effect + sub_45619D
     * ----------------------------------------------------------------- */
    print("scene: phase 17 — render_sprite_time_based_effect + sub_45619D — STUB\n");
    stub_render_sprite_time_based_effect();   /* TODO: render_sprite_time_based_effect */
    stub_sub_45619D();                        /* TODO: sub_45619D */

    /* -----------------------------------------------------------------
     * Phase 18 — viewport globals
     * Asm: mov dword_4DDE20, 1
     *      mov dword_4B7B24/14/18, 0
     *      mov dword_4B7B1C, 27Fh   ; 639
     *      mov dword_4B7B20, 1DFh   ; 479
     * ----------------------------------------------------------------- */
    print("scene: phase 18 — viewport globals (640x480)\n");
    /* dword_4DDE20 = 1, dword_4B7B1C = 0x27F, dword_4B7B20 = 0x1DF: TODO globals */

    /* -----------------------------------------------------------------
     * Phase 19 — set_entities_combat_mode + sub_448F4D
     * ----------------------------------------------------------------- */
    print("scene: phase 19 — set_entities_combat_mode + sub_448F4D — STUB\n");
    stub_set_entities_combat_mode();   /* TODO: set_entities_combat_mode */
    stub_sub_448F4D();                 /* TODO: sub_448F4D */

    /* -----------------------------------------------------------------
     * Phase 20 — ls_clear_bmap (graphics_context, game_context)
     *           sub_45D27A(render_buffer1, 1)
     *           update_player_interface(game_context, 2)
     *           sub_45E070, sub_47674F(0)
     * ----------------------------------------------------------------- */
    print("scene: phase 20 — clear bmaps + interface update — STUB\n");
    stub_ls_clear_bmap(NULL);              /* TODO: ls_clear_bmap(graphics_context) */
    stub_ls_clear_bmap(NULL);              /* TODO: ls_clear_bmap(game_context) */
    stub_sub_45D27A(NULL, 1);             /* TODO: sub_45D27A(render_buffer1, 1) */
    stub_update_player_interface(NULL, 2); /* TODO: update_player_interface(game_context,2) */
    stub_sub_45E070();                     /* TODO: sub_45E070 */
    stub_sub_47674F(0);                    /* TODO: sub_47674F(0) */

    /* -----------------------------------------------------------------
     * Phase 21 — render companion entities (if present)
     * Asm: cmp companion_entity, 0 / jz skip
     *        render_entity_with_ui(companion_entity, 0)
     *      cmp secondary_companion_entity, 0 / jz skip
     *        render_entity_with_ui(secondary_companion_entity, 0)
     * ----------------------------------------------------------------- */
    print("scene: phase 21 — render companions — STUB\n");
    stub_render_entity_with_ui(NULL, 0); /* TODO: companion_entity */
    stub_render_entity_with_ui(NULL, 0); /* TODO: secondary_companion_entity */

    /* -----------------------------------------------------------------
     * Phase 22 — apply_model_transform for player + companions
     * ----------------------------------------------------------------- */
    print("scene: phase 22 — apply_model_transform — STUB\n");
    stub_apply_model_transform(NULL, NULL); /* TODO: player_entity */

    /* -----------------------------------------------------------------
     * Phase 23 — system_get_gametime → last_event_check_time
     * ----------------------------------------------------------------- */
    print("scene: phase 23 — system_get_gametime\n");
    i32 t = stub_system_get_gametime();   /* TODO: system_get_gametime */
    (void)t;
    /* last_event_check_time = t;  TODO global */

    /* -----------------------------------------------------------------
     * Phase 24 — restore_display_mode(1, 0x1D)
     * Asm: mov edx, 1Dh / mov ecx, 1 / call restore_display_mode
     * ----------------------------------------------------------------- */
    print("scene: phase 24 — restore_display_mode(1, 0x1D)\n");
    stub_restore_display_mode(1, 0x1D);   /* TODO: restore_display_mode */

    /* -----------------------------------------------------------------
     * Phase 25 — sub_40636D, world_state → dword_505C58, zero dword_4B2AB8
     * Asm: call sub_40636D
     *      mov eax, world_state / mov dword_505C58, eax
     *      mov dword_4B2AB8, 0
     * ----------------------------------------------------------------- */
    print("scene: phase 25 — sub_40636D, wire dword_505C58 — STUB\n");
    stub_sub_40636D();   /* TODO: sub_40636D */
    /* dword_505C58 = g_world_state; dword_4B2AB8 = 0; TODO globals */

    /* -----------------------------------------------------------------
     * Read L%02d.INF header into heap buffer
     * Not in the asm for this proc (owned by sub_46D0EA), but included here
     * as a skeleton demonstration of file access so /tmp/dke.log confirms the
     * path and can be inspected before sub_46D0EA is ported.
     * ----------------------------------------------------------------- */
    {
        c8 inf_vpath[64];
        snprintf(inf_vpath, sizeof(inf_vpath), "DATAS>L%02d>L%02d.INF",
                 level_id, level_id);
        c8 inf_host[512];
        if (ls_resolve_path(base, inf_vpath, inf_host, (i32)sizeof(inf_host))) {
            print("scene: INF path = '%s'\n", inf_host);
            FILE *f = ls_open(inf_host, "rb");
            if (f) {
                /* Read up to 256 bytes as a header sample */
                u8 *hdr = (u8 *)malloc(256);
                if (hdr) {
                    i32 n = (i32)fread(hdr, 1, 256, f);
                    print("scene: L%02d.INF — read %d header bytes\n", level_id, n);
                    free(hdr);
                } else {
                    print("scene: L%02d.INF — malloc failed\n", level_id);
                }
                ls_close(f);
            } else {
                print("scene: L%02d.INF not found — continuing (non-fatal)\n", level_id);
            }
        } else {
            print("scene: L%02d.INF path resolve failed\n", level_id);
        }
    }

    /* -----------------------------------------------------------------
     * Update state globals on success
     * ----------------------------------------------------------------- */
    if (state) {
        state->current_level_id = level_id;
        print("scene: state->current_level_id = %d\n", level_id);
    }

    /* Post-skeleton: load camera 0 backdrop so scene_render has something
     * to blit.  Each CIF ships its own 256-colour palette (no .GRD parse
     * needed for this pass). */
    g_scene_camera = 0;
    scene_load_camera_backdrop(level_id, g_scene_camera);

    print("scene: scene_load_level(%d) — complete\n", level_id);
    return 0;
}

/*
 * scene_render — blit the current camera's backdrop and poll input.
 * Called from the main loop while state == MENU_STATE_LOADING.
 * Falls back to a "SCENE (no bmap)" text banner if the backdrop failed
 * to load, so the loop stays alive and the user can quit cleanly.
 */
i32 scene_render(system_t *sys) {
    if (!state) return 0;

    /* --- Input: camera cycling + Esc-to-menu ------------------------- */
    /* Left arrow (keyCode 123 → actions[37]) — previous camera */
    if (sys->actions[37] == 1) {
        sys->actions[37] = 0;
        i32 next = g_scene_camera - 1;
        if (next < 0) next = g_scene_camera_max;
        print("scene: cam prev %d -> %d\n", g_scene_camera, next);
        if (!scene_load_camera_backdrop(g_scene_bg_level, next)) {
            /* Failed — the previous cam may not exist; try wrapping to 0. */
            if (next != 0) scene_load_camera_backdrop(g_scene_bg_level, 0);
        }
    }
    /* Right arrow (keyCode 124 → actions[39]) — next camera */
    if (sys->actions[39] == 1) {
        sys->actions[39] = 0;
        i32 next = g_scene_camera + 1;
        if (next > g_scene_camera_max) next = 0;
        print("scene: cam next %d -> %d\n", g_scene_camera, next);
        if (!scene_load_camera_backdrop(g_scene_bg_level, next)) {
            /* Cap discovered — remember it and wrap. */
            g_scene_camera_max = g_scene_camera;
            scene_load_camera_backdrop(g_scene_bg_level, 0);
        }
    }
    /* Esc (keyCode 53 → actions[27]) — request return to main menu */
    if (sys->actions[27] == 1) {
        sys->actions[27] = 0;
        print("scene: ESC -> back to menu\n");
        return 1;
    }

    /* --- Blit current camera backdrop ------------------------------- */
    u32 *fb   = state->graphic.front_buffer;
    i32  fb_w = state->graphic.width;
    i32  fb_h = state->graphic.height;
    if (!fb) return 0;

    if (g_scene_bg_bmap && g_scene_bg_bmap->pixels && g_scene_bg_bmap->palette) {
        i32 bw = (i32)g_scene_bg_bmap->width;
        i32 bh = g_scene_bg_bmap->height;
        i32 ox = (fb_w > bw) ? (fb_w - bw) / 2 : 0;
        i32 oy = (fb_h > bh) ? (fb_h - bh) / 2 : 0;

        if (g_scene_l_bmap && g_scene_l_bmap->pixels) {
            /* Light-modulated blit.  L is grayscale (palette[i]=(i,i,i)),
             * quarter-res (320×240 vs 640×480 → each L pixel covers a
             * 2×2 F block via nearest upscale).  Convention: L=128
             * neutral, <128 darken, >128 brighten.  Formula:
             *   out.c = clamp((F.c * L) >> 7, 0, 255)
             * matches the multiply-by-2× light convention used in the
             * Dark Earth era 2.5D pipeline. */
            const u8 *lpx = g_scene_l_bmap->pixels;
            i32       lw  = (i32)g_scene_l_bmap->width;
            i32       lh  = g_scene_l_bmap->height;
            const u8 *pal = g_scene_bg_bmap->palette;
            const u8 *fpx = g_scene_bg_bmap->pixels;

            for (i32 y = 0; y < bh; y++) {
                i32 dy = oy + y;
                if (dy < 0 || dy >= fb_h) continue;
                i32 ly = (y * lh) / bh;
                const u8 *frow = fpx + y * bw;
                const u8 *lrow = lpx + ly * lw;
                u32      *drow = fb  + dy * fb_w;
                for (i32 x = 0; x < bw; x++) {
                    i32 dx = ox + x;
                    if (dx < 0 || dx >= fb_w) continue;
                    u8 idx = frow[x];
                    u32 l  = lrow[(x * lw) / bw];
                    u32 r  = pal[idx * 3 + 0];
                    u32 g  = pal[idx * 3 + 1];
                    u32 b  = pal[idx * 3 + 2];
                    r = (r * l) >> 7;   if (r > 255) r = 255;
                    g = (g * l) >> 7;   if (g > 255) g = 255;
                    b = (b * l) >> 7;   if (b > 255) b = 255;
                    /* Byte order matches blit_bmap_to_framebuffer. */
                    drow[dx] = 0xFF000000u | (b << 16) | (g << 8) | r;
                }
            }
        } else {
            blit_bmap_to_framebuffer(fb, fb_w, fb_h,
                                     g_scene_bg_bmap, g_scene_bg_bmap->palette,
                                     ox, oy);
        }
    }
    return 0;
}

void scene_release(void) {
    scene_free_bmap(&g_scene_bg_bmap);
    scene_free_bmap(&g_scene_l_bmap);
    scene_free_bmap(&g_scene_z_bmap);
    g_scene_bg_level = -1;
}
