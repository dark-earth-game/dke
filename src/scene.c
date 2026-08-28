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
 *   g_player_entity+0xD04  — zeroed
 *   g_player_entity+0xD08  — set from g_player_entity+0xD04
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "libsys/movie.h"
#include "dialogue.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/font.h"
#include "libsys/graphics/shading.h"
#include "state.h"
#include "scene.h"
#include "game_init.h"
#include "world.h"
#include "nav_grid.h"
#include "anim2d.h"
#include "can.h"
#include "model.h"
#include "model_ome.h"
#include "obc.h"
#include "entity.h"
#include "lieu_cb.h"
#include "player.h"

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

/* dword_4DD9B4 @ 0x4DD9B4 — combat-enabled flag, set to 1 during scene load. */
static i32 g_combat_enabled = 0;

/* dword_505C58 @ 0x505C58 — shadow of world_state, set in Phase 25. */
static i32 g_dword_505C58 = 0;

/* dword_4B2AB8 @ 0x4B2AB8 — zeroed in Phase 25 (level transition counter?). */
static i32 g_dword_4B2AB8 = 0;

/* last_event_check_time — gametime snapshot from Phase 23. */
static i32 g_last_event_check_time = 0;

/* dword_543C60 @ 0x543C60 — 0xF00-byte buffer cleared before world controller init.
 * Used by sub_46D305 as staging area for INF/camera data. */
static u8 g_dword_543C60[0xF00] = {0};

/* game_structures @ 0x543CD0 — 16-entry array, stride 0xF0 (240 bytes).
 * Each entry holds an entity/object state block. Field +0x00 = entity pointer
 * (non-NULL = active), field +0x2C = last-synced game time. */
#define GAME_STRUCT_COUNT  16
#define GAME_STRUCT_STRIDE 0xF0
static u8 g_game_structures[GAME_STRUCT_COUNT * GAME_STRUCT_STRIDE] = {0};

/* render_buffer2 — 640x480 8bpp bmap (allocated in game_init phase 28).
 * Used by sub_45E070 to build the coarse brightness map. */
extern bmap_t *g_render_buffer2;

/* INTFANI3 bmap — cursor/mask overlay source (loaded in game_init phase 14).
 * Mirrors asm dword_4F4DF4.  Set externally; NULL until wired. */
static bmap_t *g_intfani3_bmap = NULL;

/* 768-entry (24 rows x 32 cols) coarse brightness map: max pixel value
 * per 20x20 tile of render_buffer2. Built by sub_45E070, queried for
 * approximate screen-space brightness when shading 3D character models. */
static u8 g_brightness_map[768];

/* HUD rendering globals — used by sub_45CD9C and sub_45D0BA. */
static i32 g_render_mode_flag = 0;     /* render_mode_flag @ 0x4B4528 */
static i32 g_hud_active = 0;          /* dword_4B52DC @ 0x4B52DC — -1 = HUD disabled */
static i32 g_combat_meter = 0;        /* dword_506B9C @ 0x506B9C — current meter value */
static i32 g_combat_meter_max = 1;    /* dword_506B80 @ 0x506B80 — max meter value */
static i32 g_cached_meter = 0;        /* dword_4B6F60 @ 0x4B6F60 — dirty-check cache */
static i32 g_cached_color_mode2 = -1; /* dword_4B6F68 @ 0x4B6F68 — cached color_mode for 45D0BA */
static i32 g_cached_health = 0;       /* dword_4B6F5C @ 0x4B6F5C — cached descriptor health */
static i32 g_cached_color_mode = 0;   /* dword_4B6F64 @ 0x4B6F64 — cached color_mode for 45CD9C */
static i32 g_cached_special_mode = 0; /* dword_4B6F74 @ 0x4B6F74 */
static i32 g_cached_score = 0;        /* dword_4B6F78 @ 0x4B6F78 */
static i32 g_special_mode_active = 0; /* special_mode_active — combat/special mode flag */
static i32 g_score_counter = 0;       /* score_counter — player score/energy counter */
static i32 g_player_movement_enabled = 0; /* player_movement_enabled */
static i32 g_color_mode = 0;          /* color_mode @ 0x4B4514 — 0=8bit 1=16bit */

/* Viewport globals — set in Phase 18, used by rendering pipeline. */
static i32 g_viewport_active = 0;  /* dword_4DDE20 @ 0x4DDE20 */
static i32 g_viewport_x0 = 0;     /* dword_4B7B14 @ 0x4B7B14 */
static i32 g_viewport_y0 = 0;     /* dword_4B7B18 @ 0x4B7B18 */
static i32 g_viewport_x1 = 639;   /* dword_4B7B1C @ 0x4B7B1C */
static i32 g_viewport_y1 = 479;   /* dword_4B7B20 @ 0x4B7B20 */
static i32 g_viewport_flags = 0;  /* dword_4B7B24 @ 0x4B7B24 */

/* -------------------------------------------------------------------------
 * External globals referenced from asm — declared here as opaque pointers
 * so the skeleton links without error.  They will be properly typed when the
 * entity / rendering subsystems land in later slices.
 * ------------------------------------------------------------------------- */

/*
 * The player entity is the one allocated by player_entity_init (game_init
 * phase 33) and exported as g_player_entity via player.h. Scene phase-7/22
 * writes and the LIEU zone-detection (lieu_cb.c player_get_pos) both use it.
 */

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
 *   F (foreground) — 640×480 8bpp CIF, the coloured room image; blit
 *                    directly to the framebuffer as the scene backdrop.
 *   L (light map)  — 320×240 grayscale CIF (palette[i]=(i,i,i)); used
 *                    later to shade 3D models placed in the scene, NOT
 *                    applied to F.
 *   Z (depth)      — 640×480 8bpp CIF, per-pixel depth of the F image;
 *                    used to depth-test 3D models against the 2.5D
 *                    background (per-pixel occlusion).
 * Any of L/Z may be NULL if the CIF is absent; F is required to render.
 * All freed in scene_release. */
static bmap_t *g_scene_bg_bmap = NULL;   /* F */
static u8      g_scene_grd[256 * 64];    /* character shade ramp (FL..L0.GRD) */
static i32     g_scene_grd_loaded = 0;

/* Per-camera active region (INF zone polygon, world X/Z). The engine picks
 * the camera whose region contains the player (dkev sub_45A67C). */
typedef struct { i32 nv; i32 vx[32], vz[32]; } cam_region_t;
static cam_region_t g_cam_region[32];   /* SCENE_MAX_CAMERAS (defined below) */
static i32          g_cam_region_count = 0;
static bmap_t *g_scene_l_bmap  = NULL;   /* L */
static bmap_t *g_scene_z_bmap  = NULL;   /* Z */

/* Level id whose backdrop is currently in g_scene_bg_bmap.  -1 = none. */
static i32 g_scene_bg_level = -1;

/* Camera index within the level (0..6 for L01). Runtime-cycled via
 * scene_render input handling (Left/Right arrow keys). */
static i32 g_scene_camera = 0;
static i32 g_camera_forced = 0;   /* script locked the camera (ChangeCamera mode 2) */

static i32 g_scene_cam_sx = 320;  /* projected character screen X */
static i32 g_scene_cam_sy = 240;  /* projected character screen Y */

/* Cached INF camera projection (filled by scene_load_camera_anchor) so any
 * world coord can be projected to screen — used for exact persona placement.
 * Projection: v = (R * w) >> 14 + Tv;  sx = 320 + v.x*scale; sy = 240 - v.y*scale. */
static i32   g_proj_valid     = 0;
static i32   g_proj_R[9];               /* camera matrix ints param[0..8].
                                         * R(row,col) = param[row + col*3]
                                         * (dkev transform_vector 0x48F424). */
static i32   g_proj_T[3];               /* raw translation param[9..11],
                                         * added directly after >>14.        */
static i32   g_proj_gscale    = 21695;  /* param[24] = round(10240/tan(fov/2)) */
static i32   g_proj_ground_wy = 0;      /* character ground-plane world Y */
static i32   g_proj_zone_z    = 0;      /* camera zone centroid Z         */
static i32   g_proj_anchor_wx = 0;      /* solved character world X        */

/* Fill a solid ARGB rectangle into the front buffer (subtitle box, etc). */
static void scene_fill_rect(state_t *state, i32 x, i32 y, i32 w, i32 h, u32 argb) {
    if (!state || !state->graphic.front_buffer) return;
    u32 *fb = state->graphic.front_buffer;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > fw) w = fw - x;
    if (y + h > fh) h = fh - y;
    for (i32 j = 0; j < h; j++) {
        u32 *row = fb + (y + j) * fw + x;
        for (i32 i = 0; i < w; i++) row[i] = argb;
    }
}

/* Project world (wx,wy,wz) -> screen (sx,sy), reproducing the dkev pipeline:
 * transform_vector (0x48F424) world->camera, then the perspective 1/z divide
 * in render_entity_bounds (0x416F70). Camera matrix R(row,col)=param[row+col*3];
 * T added directly; near plane cz<-32768; scale = gscale/16384 / ((-cz)>>9).
 * Returns 1 if visible, 0 if culled (behind near plane); still writes sx,sy. */
static i32 scene_project_world(i32 wx, i32 wy, i32 wz, i32 *osx, i32 *osy) {
    if (!g_proj_valid) {
        if (osx) *osx = g_scene_cam_sx;
        if (osy) *osy = g_scene_cam_sy;
        return 1;
    }
    const i32 *R = g_proj_R;
    i64 cx = (( (i64)wx*R[0] + (i64)wy*R[3] + (i64)wz*R[6] ) >> 14) + g_proj_T[0];
    i64 cy = (( (i64)wx*R[1] + (i64)wy*R[4] + (i64)wz*R[7] ) >> 14) + g_proj_T[1];
    i64 cz = (( (i64)wx*R[2] + (i64)wy*R[5] + (i64)wz*R[8] ) >> 14) + g_proj_T[2];

    i32 vis = 1;
    if (cz >= -32768) { cz = -32768; vis = 0; }   /* near-plane cull */
    i32 d = (i32)((-cz) >> 9);
    if (d < 1) d = 1;
    double s = ((double)g_proj_gscale / 16384.0) / (double)d;
    i32 sx = 320 + (i32)(cx * s + (cx < 0 ? -0.5 : 0.5));
    i32 sy = 240 - (i32)(cy * s + (cy < 0 ? -0.5 : 0.5));
    if (osx) *osx = sx;
    if (osy) *osy = sy;
    return vis;
}

/* Player-controlled cube world position (dev handle for gameplay
 * skeleton). Reset to camera-relative default on each camera load; WASD
 * moves the cube around within the current room. Depth is world units so
 * the model's per-pixel Z-test against the scene Z bmap keeps working. */
static i32 g_cube_world_x = 0;
static i32 g_cube_world_z = 0;   /* seeded from zfar/3 on camera load */

/* Player position in LIEU world units (same space as zones + personas), so
 * walking fires op-3 zone triggers. Seeded from the character world anchor. */
static i32 g_player_wx = 0;
static i32 g_player_wz = 0;
static i32 g_player_seeded = 0;
static i32 g_player_facing  = 0;   /* cosine_table units (0..4095) from movement */
static i32 g_player_moving  = 0;   /* 1 while the player has walk input this frame */

/* Tick of last scene_render call — used to derive dt for cube movement
 * so speed stays consistent across frame rates. 0 = first frame. */
static u32 g_scene_prev_tick = 0;

/* Highest camera index we've successfully loaded so far, used to cap
 * cycling. Updated by scene_load_camera_backdrop on the first successful
 * load. -1 = unknown; we'll probe on wrap. */
static i32 g_scene_camera_max = 6;   /* L01 has cameras 0..6 */

/* CAN character animation for P00 (player character).
 * Frame advances at ~10 fps via g_can_last_tick. */
static can_anim_t g_scene_can;
static i32        g_scene_can_loaded  = 0;
static i32        g_scene_can_frame   = 0;
static u32        g_scene_can_last    = 0;
#define CAN_FRAME_MS 100   /* ~10 fps anim cycle */

/* ANI sprite loaded for animation 1 of the current level/camera.
 * Cycled in scene_render to show a character walking. */
static ani_t   g_scene_ani;
static i32     g_scene_ani_loaded   = 0;
static i32     g_scene_ani_frame    = 0;
static u32     g_scene_ani_last_tick = 0;
#define ANI_FRAME_MS 100  /* ~10 fps sprite cycle */

#define ATOM_SCALE 2  /* OME_VERT_SCALE factor applied in model_from_ome_atom */

/* All geometry ATOMFs decoded from P00.OME, one model_t per ATOMF.
 * model_face_t uses u8 indices so we can't merge atoms into one model_t
 * (combined vertex count > 255); each atom renders separately instead.
 * Populated by scene_load_level; freed in scene_release. */
#define SCENE_OME_MAX_ATOMS 32
static model_t *g_scene_ome_atoms[SCENE_OME_MAX_ATOMS];
static i32      g_scene_ome_atom_count = 0;

/* OBJC hierarchy parsed from P00.OME — provides CAN slot→atom mapping
 * and world translations computed from the bone parent chain. */
static ome_objc_t g_scene_objc;
static i32        g_scene_objc_loaded = 0;

/* -------------------------------------------------------------------------
 * Persona mesh cache — loads PERSOS>P<nn>.OME on demand so LIEU-placed
 * personas render with their own model (P00, P12, ...). Static pose (no
 * per-persona CAN animation yet). Keyed by persona number.
 * ---------------------------------------------------------------------- */
#define PERSONA_CACHE_MAX 12
typedef struct {
    i32        num;                            /* persona number, -1 = empty */
    model_t   *atoms[SCENE_OME_MAX_ATOMS];
    i32        count;
    ome_objc_t objc;                           /* skeleton for FK            */
    i32        objc_loaded;
    can_anim_t can;                            /* current clip               */
    i32        can_loaded;
    i32        frame;
    u32        can_last;
    i32        clip_serial;                    /* last LIEU clip applied     */
    i32        foot_off;                       /* lowest bone, model units   */
} persona_mesh_t;
static persona_mesh_t g_persona_cache[PERSONA_CACHE_MAX];
static i32            g_persona_cache_n = 0;

/* Parse a persona number from a name like "P12ARR01" -> 12; -1 if none. */
static i32 persona_num_from_name(const c8 *name) {
    if (!name || name[0] != 'P' || name[1] < '0' || name[1] > '9') return -1;
    i32 n = 0, i = 1;
    while (name[i] >= '0' && name[i] <= '9' && i <= 3) { n = n*10 + (name[i]-'0'); i++; }
    return n;
}

/* Load (or fetch cached) PERSOS>P<num>.OME geometry atoms. Returns cache
 * index or -1 on failure. */
static i32 persona_mesh_load(i32 num) {
    for (i32 i = 0; i < g_persona_cache_n; i++)
        if (g_persona_cache[i].num == num) return i;
    if (num < 0 || g_persona_cache_n >= PERSONA_CACHE_MAX) return -1;

    c8 vpath[32], host[512];
    snprintf(vpath, sizeof(vpath), "PERSOS>P%02d.OME", num);
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, vpath, host, (i32)sizeof(host))) return -1;

    ome_file_t ome = {0};
    if (!model_ome_open(host, &ome)) return -1;

    persona_mesh_t *pm = &g_persona_cache[g_persona_cache_n];
    pm->num = num; pm->count = 0;

    ome_matr_t matr = {0}, *matr_ptr = NULL;
    for (i32 ci = 0; ci < ome.chunk_count; ci++) {
        if (ome.chunks[ci].kind != OME_CHUNK_MATR) continue;
        if (ome_parse_matr(ome.raw + ome.chunks[ci].offset + 0x60,
                           ome.chunks[ci].length - 0x60, &matr))
            matr_ptr = &matr;
        break;
    }
    /* Index atoms by ATOMF ordinal so atom slot i == OBJC bone slot i. A
     * failed/empty ATOMF leaves a NULL gap rather than shifting later atoms
     * onto the wrong bone (which would break the skeletal pose). */
    for (i32 i = 0; i < SCENE_OME_MAX_ATOMS; i++) pm->atoms[i] = NULL;
    i32 aidx = 0;
    for (i32 ci = 0; ci < ome.chunk_count; ci++) {
        if (ome.chunks[ci].kind != OME_CHUNK_ATOMF) continue;
        if (aidx >= SCENE_OME_MAX_ATOMS) break;
        ome_atom_t atom = {0};
        model_t *m = NULL;
        if (ome_decode_atomf(ome.raw + ome.chunks[ci].offset + 0x60,
                             ome.chunks[ci].length - 0x60, &atom) &&
            atom.vert_count > 0 && atom.face_count > 0) {
            m = model_from_ome_atom(&atom, matr_ptr);
        } else {
            print("scene: P%02d ATOMF slot %d decode empty/failed\n", num, aidx);
        }
        pm->atoms[aidx++] = m;   /* keep the slot even if NULL */
        ome_atom_free(&atom);
    }
    pm->count = aidx;
    /* OBJC skeleton (parse before freeing the OME raw buffer). */
    pm->objc_loaded = 0;
    for (i32 ci = 0; ci < ome.chunk_count; ci++) {
        if (ome.chunks[ci].kind == OME_CHUNK_OBJC) {
            pm->objc_loaded = ome_parse_objc(ome.raw, ome.raw_size,
                                             ome.chunks[ci].offset, &pm->objc);
            break;
        }
    }
    model_ome_free(&ome);
    if (pm->count == 0) return -1;

    /* Load the persona's own CAN, selecting the ARR01 arrival clip by hash;
     * fall back to P01.CAN (g01arr01) for personas without a CAN. */
    pm->can_loaded = 0; pm->frame = 0; pm->can_last = 0;
    c8 clip[16], cvp[32], chost[512];
    snprintf(clip, sizeof(clip), "p%02darr01", num);
    snprintf(cvp,  sizeof(cvp),  "ANM>P%02d.CAN", num);
    if (ls_resolve_path(base, cvp, chost, (i32)sizeof(chost)) &&
        can_load_named(chost, clip, &pm->can)) {
        pm->can_loaded = 1;
    } else if (ls_resolve_path(base, "ANM>P01.CAN", chost, (i32)sizeof(chost))) {
        pm->can_loaded = can_load_named(chost, "g01arr01", &pm->can);
    }

    pm->clip_serial = 0;
    /* Ground offset. A persona's skeleton is laid out along bone X — head at
     * the positive end, feet at the negative end (P00: X[-121123..34416]) —
     * and the render transform maps that axis to world height. The root sits
     * at the hips, so dropping the model by its lowest bone puts the feet on
     * the ground. This replaces the hand-tuned -120000 that only ever suited
     * P00 (P12's feet are at -109645, so it sank ~11k units into the floor). */
    pm->foot_off = -120000;
    if (pm->objc_loaded && pm->objc.n_atomslots > 0) {
        i32 lo = 0x7FFFFFFF;
        for (i32 b = 0; b < pm->objc.n_atomslots; b++)
            if (pm->objc.world_tx[b] < lo) lo = pm->objc.world_tx[b];
        if (lo != 0x7FFFFFFF) pm->foot_off = lo;
        print("scene: P%02d ground offset %d (lowest bone)\n", num, pm->foot_off);
    }
    print("scene: loaded persona P%02d.OME (%d atoms, objc=%d, can=%d)\n",
          num, pm->count, pm->objc_loaded, pm->can_loaded);
    return g_persona_cache_n++;
}

/*
 * persona_apply_script_clip — swap a persona onto the CAN clip the LIEU
 * script asked for.
 *
 * ANIMATION actions carry the clip id ("p12arr03"), which is what the engine
 * feeds play_entity_animation. We reload the persona's CAN when the requested
 * clip changes, falling back to whatever was loaded if the clip is missing
 * from the file.
 */
static void persona_apply_script_clip(persona_mesh_t *pm) {
    if (!pm || pm->num < 0) return;

    i32 serial = 0;
    const c8 *clip = lieu_persona_clip(pm->num, &serial);
    if (!clip || serial == pm->clip_serial) return;
    pm->clip_serial = serial;

    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    c8 cvp[32], chost[512];
    snprintf(cvp, sizeof(cvp), "ANM>P%02d.CAN", pm->num);
    if (!ls_resolve_path(base, cvp, chost, (i32)sizeof(chost))) return;

    can_anim_t next = {0};
    if (!can_load_named(chost, clip, &next)) {
        print("scene: P%02d clip '%s' not in %s — keeping current\n",
              pm->num, clip, cvp);
        return;
    }
    pm->can        = next;
    pm->can_loaded = 1;
    pm->frame      = 0;
    pm->can_last   = 0;
    print("scene: P%02d -> script clip '%s' (%d frames)\n",
          pm->num, clip, next.n_frames);
}


/* -------------------------------------------------------------------------
 * Per-level config from L<lvl>.DAT (plain-text INI-style, French keys).
 *
 * Example L01.DAT block:
 *   ncamera : 7
 *   zbuffer0 : 1000        // per-camera Z-buffer far plane (world units)
 *   ...
 *   lightx0 : 8967         // per-camera key-light position
 *   lighty0 : 30000
 *   lightz0 : 34040
 *   redAmbiant : 256       // global ambient RGB (0..256 for Q8)
 *
 * We keep only the fields the model renderer will consume: per-camera
 * zbuffer (needed to un-normalise the 8-bit Z bmap into world depth) and
 * per-camera light position (needed for L-map correction / model shading).
 * ------------------------------------------------------------------------- */
#define SCENE_MAX_CAMERAS 32

typedef struct {
    i32 zbuffer[SCENE_MAX_CAMERAS];   /* far plane per camera, world units */
    i32 lightx [SCENE_MAX_CAMERAS];   /* key light X per camera            */
    i32 lighty [SCENE_MAX_CAMERAS];   /* key light Y per camera            */
    i32 lightz [SCENE_MAX_CAMERAS];   /* key light Z per camera            */
    i32 ncamera;                       /* number of cameras (echoes INF)   */
    i32 r_amb, g_amb, b_amb;           /* global ambient (Q8, 256=identity)*/
    i32 multilights;                   /* per-camera light on/off          */
    i32 loaded;                        /* 1 if L<lvl>.DAT parsed cleanly   */
} scene_config_t;

static scene_config_t g_scene_cfg = {0};

i32 scene_get_camera_zfar(i32 camera) {
    if (!g_scene_cfg.loaded) return 1000;   /* sane default for L01-ish rooms */
    if (camera < 0 || camera >= SCENE_MAX_CAMERAS) return 1000;
    return g_scene_cfg.zbuffer[camera] > 0 ? g_scene_cfg.zbuffer[camera] : 1000;
}

/*
 * parse_dat_int — locate "key : <int>" in a null-terminated buffer and
 * write the integer into *out. Whitespace-tolerant. Returns 1 on match.
 */
static i32 parse_dat_int(const c8 *buf, const c8 *key, i32 *out) {
    const c8 *p = strstr(buf, key);
    if (!p) return 0;
    p += strlen(key);
    while (*p == ' ' || *p == '\t') p++;
    if (*p != ':') return 0;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    i32 sign = 1;
    if (*p == '-') { sign = -1; p++; }
    else if (*p == '+') p++;
    i32 v = 0;
    if (*p < '0' || *p > '9') return 0;
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (*p - '0');
        p++;
    }
    *out = sign * v;
    return 1;
}

/*
 * scene_load_dat_config — read L<lvl>.DAT into memory and pull out the
 * per-camera zbuffer + light fields plus global ambient / multilights.
 * Non-fatal on any error; leaves g_scene_cfg.loaded=0 so callers fall
 * back to defaults.
 */
static void scene_load_dat_config(i32 level_id) {
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    c8 vpath[64];
    snprintf(vpath, sizeof(vpath), "DATAS>L%02d>L%02d.DAT",
             level_id, level_id);
    c8 host[512];
    if (!ls_resolve_path(base, vpath, host, (i32)sizeof(host))) {
        print("scene: DAT resolve failed for '%s'\n", vpath);
        return;
    }
    FILE *f = ls_open(host, "rb");
    if (!f) {
        print("scene: DAT '%s' not found\n", host);
        return;
    }
    /* Whole file is ~1 KB of plain ASCII — read it all. */
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0 || sz > 65536) {
        print("scene: DAT '%s' size %ld out of range\n", host, sz);
        ls_close(f);
        return;
    }
    c8 *buf = (c8 *)malloc((size_t)sz + 1);
    if (!buf) { ls_close(f); return; }
    fread(buf, 1, (size_t)sz, f);
    buf[sz] = '\0';
    ls_close(f);

    memset(&g_scene_cfg, 0, sizeof(g_scene_cfg));
    parse_dat_int(buf, "ncamera",      &g_scene_cfg.ncamera);
    parse_dat_int(buf, "multilights",  &g_scene_cfg.multilights);
    parse_dat_int(buf, "redAmbiant",   &g_scene_cfg.r_amb);
    parse_dat_int(buf, "greenAmbiant", &g_scene_cfg.g_amb);
    parse_dat_int(buf, "blueAmbiant",  &g_scene_cfg.b_amb);

    for (i32 c = 0; c < SCENE_MAX_CAMERAS; c++) {
        c8 key[24];
        snprintf(key, sizeof(key), "zbuffer%d", c);
        parse_dat_int(buf, key, &g_scene_cfg.zbuffer[c]);
        snprintf(key, sizeof(key), "lightx%d", c);
        parse_dat_int(buf, key, &g_scene_cfg.lightx[c]);
        snprintf(key, sizeof(key), "lighty%d", c);
        parse_dat_int(buf, key, &g_scene_cfg.lighty[c]);
        snprintf(key, sizeof(key), "lightz%d", c);
        parse_dat_int(buf, key, &g_scene_cfg.lightz[c]);
    }

    g_scene_cfg.loaded = 1;
    free(buf);

    print("scene: DAT loaded — ncamera=%d ambient=(%d,%d,%d) multilights=%d\n",
          g_scene_cfg.ncamera,
          g_scene_cfg.r_amb, g_scene_cfg.g_amb, g_scene_cfg.b_amb,
          g_scene_cfg.multilights);
    for (i32 c = 0; c < g_scene_cfg.ncamera && c < SCENE_MAX_CAMERAS; c++) {
        print("scene:   cam %d  zfar=%d  light=(%d,%d,%d)\n",
              c, g_scene_cfg.zbuffer[c],
              g_scene_cfg.lightx[c],
              g_scene_cfg.lighty[c],
              g_scene_cfg.lightz[c]);
    }
}

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
        /* One-shot histogram probe so we can guess the near/far
         * convention: sample four fixed screen points and log their raw
         * u8 depth. Combined with the DAT zfar, this tells us whether
         * the bmap is linear 0..zfar (0=near) or inverse (0=far). */
        if (zb->pixels && zb->width > 0 && zb->height > 0) {
            i32 zw = (i32)zb->width;
            i32 zh = zb->height;
            i32 cx = zw / 2, cy = zh / 2;
            u8 zc = zb->pixels[cy * zw + cx];
            u8 ztl = zb->pixels[0];
            u8 zbr = zb->pixels[(zh - 1) * zw + (zw - 1)];
            u8 ztop = zb->pixels[(zh / 4) * zw + cx];
            i32 zfar = scene_get_camera_zfar(camera);
            print("scene:   Z sample: centre=%u top=%u tl=%u br=%u  zfar=%d\n",
                  zc, ztop, ztl, zbr, zfar);
        }
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

    /* Character shade ramp: DATAS>L<lvl>>PAL>FL<lvl>C<cam>L0.GRD (256*64 =
     * 16384 bytes). Maps (base palette index, light level) -> shaded palette
     * index for the character rasterizer; RGB from the backdrop palette. */
    g_scene_grd_loaded = 0;
    {
        c8 gp[80], host[512];
        snprintf(gp, sizeof(gp), "DATAS>L%02d>PAL>FL%02dC%dL0.GRD",
                 level_id, level_id, camera);
        const c8 *base = (state && state->game_path) ? state->game_path : ".";
        if (ls_resolve_path(base, gp, host, (i32)sizeof(host))) {
            FILE *gf = fopen(host, "rb");
            if (gf) {
                size_t n = fread(g_scene_grd, 1, sizeof(g_scene_grd), gf);
                fclose(gf);
                if (n == sizeof(g_scene_grd)) {
                    g_scene_grd_loaded = 1;
                    print("scene: GRD shade ramp loaded (%s)\n", gp);
                } else {
                    print("scene: GRD short read %zu for %s\n", n, gp);
                }
            }
        }
    }

    /* Reset cube world position on camera swap so it re-centres for the
     * new room's zfar. Removed once real character world coords come
     * from LIEU / .INF. */
    g_cube_world_x = 0;
    g_cube_world_z = scene_get_camera_zfar(camera) / 3;
    print("scene: camera %d loaded (F %dx%d, L=%s, Z=%s)\n",
          camera, (i32)fb->width, fb->height,
          lb ? "yes" : "no", zb ? "yes" : "no");
    return 1;
}

/* Forward declarations for functions defined later in this file. */
static void build_inverse_z_lut(i32 zfar_value);

/* -------------------------------------------------------------------------
 * Platform no-ops and partial implementations for scene_load_level sub-calls.
 * DirectDraw / CD-ROM functions are intentional no-ops on modern platforms.
 * Partial implementations note their blocking dependencies in comments.
 * ------------------------------------------------------------------------- */

/* set_display_mode — ECX=0x1D sets hardware display mode.
 * Original: DirectDraw SetDisplayMode. No-op on modern platforms.
 * Mirrors asm: mov ecx, 1Dh / call set_display_mode */
static void set_display_mode(i32 mode) {
    (void)mode;
}

/* sub_469E34 @ 0x469E34 — CD-ROM disc detection.
 * Windows: GetLogicalDriveStringsA + GetDriveTypeA + GetVolumeInformationA.
 * No-op on modern platforms (game data accessed via filesystem). */
static void detect_cdrom(i32 arg) {
    /* sub_469E34 @ 0x469E34 — CD-ROM detection, no-op on modern platforms */
    (void)arg;
}

/* TODO: handle_data_l44_faux — called when level_id == 0x2C (44).
 * Likely special-case for a test/dummy level.
 * Mirrors asm: cmp [ebp+var_4], 2Ch / call handle_data_l44_faux */
static void handle_data_l44_faux(void) {
    /* handle_data_l44_faux — special-case for test level 0x2C, no-op */
}

/* sub_46D305 — world controller: load INF zone data + build level paths. */
static void load_world_controller(void) { /* sub_46D305 @ 0x46D305 */
    const c8 *gp = (state && state->game_path) ? state->game_path : ".";
    world_load_level_data(g_world_state, gp,
                          state ? state->is_demo : 0,
                          0, state ? state->debug_enabled : 0);
    g_world_controller = g_world.world_controller;
}

/* sub_46D0EA @ 0x46D0EA — asset loader dispatcher.
 * 7-phase pipeline: F background, Z depth, G gradient/GRD, L overlay,
 * M color LUT, rendering params, 1/z perspective LUT.
 * Level assets loaded from DATAS>L{nn}>.
 * Currently handled by scene_load_camera_backdrop + scene_load_dat_config. */
static void load_level_assets(void) {
    /* sub_46D0EA @ 0x46D0EA — asset loader dispatcher.
     * Full 7-phase pipeline (F/Z/G/L/M + params + inv-Z) depends on
     * sub_46C930 file loader. Partial: build inverse-Z LUT only. */
    i32 zfar = scene_get_camera_zfar(0);
    build_inverse_z_lut(zfar);
}

/* can_do_32k — colour-mode capability check. Always true on modern HW. */
static i32 can_do_32k(i32 color_mode) {
    /* can_do_32k — always true on modern HW (original checked DirectDraw caps) */
    (void)color_mode;
    return 1;
}

/* sub_4640BC @ 0x4640BC — pos_perso: entity placement in world.
 * Full function: 968 bytes, dispatches on life_mode (0=place, 1=init, 2=combat,
 * 3=nop, 4=remove). Handles player char swaps (types 0x62-0x67), enemy/companion
 * creation. During scene_load_level, applies default placement to player entity. */
static void place_player_defaults(void) { /* sub_4640BC @ 0x4640BC */
    if (!g_player_entity) return;
    u8 *pe = (u8 *)g_player_entity;

    *(i32 *)(pe + 0x070) = 1;
    *(i32 *)(pe + 0x074) = 1;
    *(i32 *)(pe + 0x078) = 0;
    *(i32 *)(pe + 0x07C) = 0;
    *(i32 *)(pe + 0x080) = 639;
    *(i32 *)(pe + 0x084) = 479;
    *(i32 *)(pe + 0x064) = 0x50;
    *(i32 *)(pe + 0x090) = 2;
    *(i32 *)(pe + 0x094) = 2;

    print("scene: pos_perso — player entity placed with defaults\n");
}

/* -------------------------------------------------------------------------
 * Zone trigger table — 135 entries, 28-byte stride (byte_4B3448 in IDA).
 * offset 0x00: active flag (byte)
 * offset 0x04: name string (4+ bytes, compared with strncmp)
 * offset 0x11: pending/triggered flag (byte)
 * ------------------------------------------------------------------------- */
#define TRIGGER_TABLE_ENTRIES 135
#define TRIGGER_ENTRY_SIZE    28

static u8 g_trigger_table[TRIGGER_TABLE_ENTRIES * TRIGGER_ENTRY_SIZE];

/* -------------------------------------------------------------------------
 * Degradable object pool — 8 entries at unk_4DAB60, stride 0x10C (268 bytes).
 * sub_475BB6 searches this pool by name; sub_476BEE releases entries.
 *
 * Struct layout from IDA (sub_475BB6 + sub_476BEE + sub_47398A):
 *   +0x00 .. +0x4F: entity/render data
 *   +0x50: active flag (dword, non-zero = in use)
 *   +0x88: inventory handle (dword)
 *   +0xA8: name string (char[32], matched with strncmp 4)
 *   +0xB0: memory handle (dword, freed by sub_476BEE)
 *   +0xC4: flags (dword, bit 5=0x20 = recyclable/pending release)
 *   +0xF0: particle effect buffer index (dword)
 *   +0xF4: secondary inventory handle (dword)
 * Total: 0x10C = 268 bytes per entry.
 * ------------------------------------------------------------------------- */
#define DEGRAD_POOL_SIZE    8
#define DEGRAD_ENTRY_STRIDE 0x10C

typedef struct {
    u8  entity_data[0x50];   /* +0x00: entity/render blob */
    i32 active;              /* +0x50: non-zero = slot in use */
    u8  pad_54[0x34];        /* +0x54 .. +0x87 */
    i32 inventory;           /* +0x88: inventory handle */
    u8  pad_8C[0x1C];        /* +0x8C .. +0xA7 */
    c8  name[32];            /* +0xA8: object name string */
    i32 mem_handle;          /* +0xC8: memory handle — NOTE: actual +0xB0, but
                              * struct packing aligns name[32] to +0xA8..+0xC7,
                              * pushing this to +0xC8. Use raw offsets instead. */
    u8  pad2[0x10C - 0xC8];  /* remainder */
} degrad_entry_raw_t;

static u8 g_degrad_pool[DEGRAD_POOL_SIZE * DEGRAD_ENTRY_STRIDE];

/* Access helpers — use raw byte offsets matching IDA exactly. */
/* Forward declarations for zone system functions. */
static i32 activate_trigger_entries(void *zone_obj, i32 activate);

static inline i32  *degrad_active(i32 i)  { return (i32 *)(g_degrad_pool + i * DEGRAD_ENTRY_STRIDE + 0x50); }
static inline c8   *degrad_name(i32 i)    { return (c8  *)(g_degrad_pool + i * DEGRAD_ENTRY_STRIDE + 0xA8); }
static inline i32  *degrad_flags(i32 i)   { return (i32 *)(g_degrad_pool + i * DEGRAD_ENTRY_STRIDE + 0xC4); }
static inline i32  *degrad_pfx_idx(i32 i) { return (i32 *)(g_degrad_pool + i * DEGRAD_ENTRY_STRIDE + 0xF0); }
static inline u8   *degrad_entry(i32 i)   { return g_degrad_pool + i * DEGRAD_ENTRY_STRIDE; }

/* sub_475BB6 @ 0x475BB6 — find or allocate degradable object by name.
 * 4-phase search on 8-entry pool at unk_4DAB60:
 *   Phase 0: reject "00" prefix (skip-name sentinel)
 *   Phase 1: exact name match + active slot -> reuse
 *   Phase 2: find empty slot (active==0) via particle heap match -> allocate
 *   Phase 3: recycle a slot with flag bit 0x20 set (pending release)
 *   Phase 4: overflow — log error, return NULL
 *
 * ECX = object name (const char *, e.g. "OB03").
 * Returns pointer to pool entry, or NULL. */
void *scene_find_degradable_object(const c8 *name) { /* sub_475BB6 @ 0x475BB6 */
    if (!name) return NULL;

    /* Phase 0: reject "00" prefix at name[2..3] */
    if (strncmp(name + 2, "00", 2) == 0) return NULL;

    /* Phase 1: search for existing entry with matching name + active */
    for (i32 i = 0; i < DEGRAD_POOL_SIZE; i++) {
        if (strncmp(degrad_name(i), name, 4) != 0) continue;
        if (*degrad_active(i) == 0) continue;
        /* Found — copy full name and clear bit 5 */
        strcpy(degrad_name(i), name);
        *degrad_flags(i) &= ~0x20;
        return degrad_entry(i);
    }

    /* Phase 2: find empty slot — requires particle_effect_buffer + heap check.
     * Simplified: find first inactive slot directly. */
    for (i32 i = 0; i < DEGRAD_POOL_SIZE; i++) {
        if (*degrad_active(i) != 0) continue;
        /* Allocate this slot */
        strcpy(degrad_name(i), name);
        *degrad_flags(i) &= ~0x20;
        return degrad_entry(i);
    }

    /* Phase 3: recycle slot with bit 0x20 (pending release) */
    for (i32 i = 0; i < DEGRAD_POOL_SIZE; i++) {
        if (!(*degrad_flags(i) & 0x20)) continue;
        /* Release old entry (sub_476BEE) — simplified: just clear fields */
        *degrad_active(i) = 0;
        *degrad_flags(i) &= ~0x20;
        strcpy(degrad_name(i), name);
        return degrad_entry(i);
    }

    /* Phase 4: overflow */
    print("find_degradable_object: Too many objects loaded!\n");
    for (i32 i = 0; i < DEGRAD_POOL_SIZE; i++) {
        print("  slot %d: '%s' flags=0x%X\n", i, degrad_name(i), *degrad_flags(i));
    }
    return NULL;
}

/* sub_4728DF @ 0x4728DF — resolve creature poly index from degradable object.
 * Validates name at +0xA8: must be (O|Q)B<digit>. Parses "%c%c%02d" to get
 * creature id. Returns poly_index if creature flag bit 0 set, else 0. */
static i32 degrad_get_creature_poly(void *degrad_obj) { /* sub_4728DF @ 0x4728DF */
    if (!degrad_obj) return 0;
    u8 *obj = (u8 *)degrad_obj;
    c8 type_c = (c8)obj[0xA8];
    if (type_c != 'O' && type_c != 'Q') return 0;
    if ((c8)obj[0xA9] != 'B') return 0;
    c8 digit = (c8)obj[0xAA];
    if (digit < '0' || digit > '9') return 0;

    c8 c1, c2;
    i32 id = 0;
    sscanf((c8 *)(obj + 0xA8), "%c%c%02d", &c1, &c2, &id);

    const creature_entry_t *cr = game_init_get_creature(id);
    if (!cr) return 0;
    if (!(cr->flags & 1)) return 0;
    return (i32)cr->poly_index;
}

/* sub_472A48 @ 0x472A48 — check if degradable object is a weapon type.
 * Validates name at +0xA8: must be (O|Q)<any><digit>. Parses "%c%c%02d",
 * returns creature flags & 2 (non-zero = is weapon). */
static i32 is_weapon_object(void *degrad_obj) { /* sub_472A48 @ 0x472A48 */
    if (!degrad_obj) return 0;
    u8 *obj = (u8 *)degrad_obj;
    c8 type_c = (c8)obj[0xA8];
    if (type_c != 'O' && type_c != 'Q') return 0;
    c8 digit = (c8)obj[0xAA];
    if (digit < '0' || digit > '9') return 0;

    c8 c1, c2;
    i32 id = 0;
    sscanf((c8 *)(obj + 0xA8), "%c%c%02d", &c1, &c2, &id);

    const creature_entry_t *cr = game_init_get_creature(id);
    if (!cr) return 0;
    return (i32)(cr->flags & 2);
}

/* sub_4373EF @ 0x4373EF — link two bone slots in entity hierarchy.
 * ECX=entity, EDX=src_slot, arg0=dst_slot. Reads entity+0xFC bone table,
 * links src->+0x7C = dst (first child) and dst->+0x78 = src (parent). */
static void bone_link_slots(void *entity, i32 src_slot, i32 dst_slot) {
    /* sub_4373EF @ 0x4373EF */
    if (!entity) return;
    u8 *ent = (u8 *)entity;
    obc_node_t **bones = (obc_node_t **)(ent + 0xFC);
    obc_node_t *src = bones[src_slot];
    obc_node_t *dst = bones[dst_slot];
    if (!src || !dst) return;
    src->child  = dst;   /* src->+0x7C = first child */
    dst->parent = src;   /* dst->+0x78 = parent      */
}

/* sub_474731 @ 0x474731 — weapon conflict resolution.
 * When entity picks up weapon_obj, checks if any OTHER entity (player,
 * companion, secondary companion) already holds it. If so, detaches it
 * from the conflicting entity and clears their weapon fields.
 * ECX=entity (new holder), EDX=weapon_obj. */
static void weapon_resolve_conflicts(void *entity, void *weapon_obj) {
    /* sub_474731 @ 0x474731 */
    if (!entity || !weapon_obj) return;

    void *others[3] = { NULL, NULL, NULL };
    if (entity != g_player_entity)
        others[0] = g_player_entity;
    if (entity != g_companion_entity)
        others[1] = g_companion_entity;
    if (entity != g_secondary_companion_entity)
        others[2] = g_secondary_companion_entity;

    for (i32 i = 0; i < 3; i++) {
        if (!others[i]) continue;
        u8 *other = (u8 *)others[i];

        for (i32 slot = 0; slot < 2; slot++) {
            void *held = *(void **)(other + 0xD64 + slot * 4);
            if (held != weapon_obj) continue;

            /* Conflict: other entity holds same weapon */
            print("weapon_resolve_conflicts: conflict entity=%p slot=%d\n",
                  others[i], slot);

            /* Clear weapon reference */
            *(void **)(other + 0xD64 + slot * 4) = NULL;

            if (slot == 0) {
                /* Primary weapon: clear bone attach + weapon name + type */
                *(void **)(other + 0x178) = NULL;
                *(u8 *)(other + 0xD2) = 0;
                u8 *other_desc = *(u8 **)(other + 0xAC);
                if (other_desc) other_desc[0x97] = 0;
            } else {
                /* Secondary weapon: clear secondary fields */
                *(i32 *)(other + 0x164) = 0;
                *(u8 *)(other + 0xD8) = 0;
            }
        }
    }
}

/* sub_472C5C @ 0x472C5C — weapon entity init (object mesh load).
 * Takes degradable object entry. Formats "OBJETS>" path + "OB<name>" object
 * name. Calls setup_creature to load mesh/animation data into the object's
 * +0x50 slot. Special cases: weapon "27" → secondary OB54, weapons
 * "23"/"35"/"66"/"68" → FLAM effect, "32" → FLAM2 effect.
 *
 * Dependencies: setup_creature (not ported), particle_effect_buffer (not
 * ported). Structure preserved for when setup_creature lands. */
void scene_weapon_entity_init(void *degrad_obj) { /* sub_472C5C @ 0x472C5C */
    if (!degrad_obj) return;
    u8 *obj = (u8 *)degrad_obj;

    /* Already active? Skip re-init */
    if (*(i32 *)(obj + 0x50) != 0) return;

    /* Save + override render_mode @ 0x472C95 */
    c8 path[64];
    c8 obj_name[64];
    snprintf(path, sizeof(path), "OBJETS>");

    /* Format object name: digit at +0xAA → "OB%.2s", else "OB%s" */
    c8 name_byte = (c8)obj[0xAA];
    if (name_byte >= '0' && name_byte <= '9') {
        snprintf(obj_name, sizeof(obj_name), "OB%.2s", (c8 *)(obj + 0xAA));
    } else {
        snprintf(obj_name, sizeof(obj_name), "OB%s", (c8 *)(obj + 0xAA));
    }

    /* setup_creature @ 0x44F2A0 — load MAT + OBC for this weapon object.
     * mat_out = obj+0x88 (inventory handle), obc_out = obj+0x50 (active/scene ptr) */
    {
        void *mat_result = NULL;
        void *obc_result = NULL;
        i32 pfx_idx = *(i32 *)(obj + 0xF0);
        setup_creature(path, obj_name, &mat_result, &obc_result, pfx_idx);
        if (mat_result) *(void **)(obj + 0x88) = mat_result;
        if (obc_result) *(void **)(obj + 0x50) = obc_result;
    }

    /* Clear secondary inventory handle @ 0x472D76 */
    *(i32 *)(obj + 0xF4) = 0;

    /* Special case: weapon "27" → load OB54 secondary @ 0x472D8D */
    if (strncmp((c8 *)(obj + 0xAA), "27", 2) == 0) {
        void *mat2 = NULL, *obc2 = NULL;
        setup_creature("OBJETS>", "OB54", &mat2, &obc2,
                       *(i32 *)(obj + 0xF0));
        if (obc2) *(void **)(obj + 0xF4) = obc2;
    }

    /* Special case: weapons "23"/"35"/"66"/"68" → FLAM effect @ 0x472DF7 */
    if (strncmp((c8 *)(obj + 0xAA), "23", 2) == 0 ||
        strncmp((c8 *)(obj + 0xAA), "35", 2) == 0 ||
        strncmp((c8 *)(obj + 0xAA), "66", 2) == 0 ||
        strncmp((c8 *)(obj + 0xAA), "68", 2) == 0) {
        void *flam_mat = NULL, *flam_obc = NULL;
        setup_creature("OBJETS>", "FLAM", &flam_mat, &flam_obc,
                       *(i32 *)(obj + 0xF0));
    }

    /* Special case: weapon "32" → FLAM2 effect @ 0x472F09 */
    if (strncmp((c8 *)(obj + 0xAA), "32", 2) == 0) {
        void *flam2_mat = NULL, *flam2_obc = NULL;
        setup_creature("OBJETS>", "FLAM2", &flam2_mat, &flam2_obc,
                       *(i32 *)(obj + 0xF0));
    }

    /* Restore render_mode @ 0x472FA5 */
    print("weapon_entity_init: done for '%s'\n", obj_name);
}

/* sub_473756 @ 0x473756 — attach weapon entity to player bone hierarchy.
 * ECX=entity, EDX=weapon degradable object, arg0=flag (1=full init, 0=reattach).
 *
 * Walks weapon's scene hierarchy to find deepest "armeb" chain → link to
 * entity+0x178. Resets frame counters on non-"armeb" nodes. When weapon
 * sub-type is 'B', updates creature ability table: clears bit 1, sets bit 2.
 * Finally stores weapon at entity+0xD64 and dispatches animation event.
 *
 * Dependencies: sub_474731 (entity-weapon property sync — not ported),
 * sub_4373EF (animation state dispatch — not ported). */
void scene_weapon_entity_attach(void *entity, void *weapon_obj, i32 full_init) {
    /* sub_473756 @ 0x473756 */
    if (!entity || !weapon_obj) return;
    u8 *ent = (u8 *)entity;
    u8 *wobj = (u8 *)weapon_obj;

    if (full_init) {
        /* Traverse weapon's scene hierarchy: [wobj+0x50] is root creature node */
        obc_node_t *creature = *(obc_node_t **)(wobj + 0x50);
        if (creature) {
            /* Find attachment point: if creature->+0x7C exists, walk to
             * creature->+0x7C->+0x7C (two levels deep) @ 0x473782 */
            obc_node_t *child = creature->child;
            if (child) {
                *(void **)(ent + 0x178) = child->child;
            } else {
                *(void **)(ent + 0x178) = creature;
            }

            /* Walk hierarchy: reset frame counters on non-"armeb" nodes @ 0x4737BE */
            for (obc_node_t *node = creature; node; node = node->child) {
                if (strcmp(node->name, "armeb") != 0) {
                    node->pos[2] = 0;   /* +0x44 */
                    node->pos[1] = 0;   /* +0x40 */
                    node->pos[0] = 0;   /* +0x3C */
                }
            }
        }
    } else {
        /* Reattach: just set entity+0x178 = weapon->+0x50 @ 0x473854 */
        *(void **)(ent + 0x178) = *(void **)(wobj + 0x50);
    }

    /* Clear attachment offsets if link exists @ 0x47380F */
    void *attach = *(void **)(ent + 0x178);
    if (attach) {
        u8 *a = (u8 *)attach;
        *(i32 *)(a + 0x54) = 0;
        *(i32 *)(a + 0x58) = 0;
        *(i32 *)(a + 0x5C) = 0;
    }

    /* Creature ability table update: if weapon sub-type is 'B' @ 0x473863 */
    if ((c8)wobj[0xA9] == 'B') {
        i32 creature_id = degrad_get_creature_poly(weapon_obj);
        i32 slot = (c8)wobj[0xAC] - 'A';
        if (slot == -0x41) slot = 0;   /* NUL char → slot 0 */

        if (creature_id > 0 && creature_id < 0x23 && slot >= 0) {
            /* byte_4DCCC0[slot + creature_id*6] &= ~2, |= 4 @ 0x4738C0 */
            game_init_set_ability(creature_id, slot, 0x02, 0x04);
        } else {
            /* Out of range: clear bit 1 on weapon flags @ 0x473919 */
            *(i32 *)(wobj + 0xC4) &= ~2;
        }
    }

    /* sub_474731 @ 0x474731 — resolve weapon conflicts with other entities */
    weapon_resolve_conflicts(entity, weapon_obj);

    /* Store weapon reference at entity+0xD64 @ 0x473934 */
    *(void **)(ent + 0xD64) = weapon_obj;

    /* sub_4373EF @ 0x4373EF — link weapon bone to entity skeleton.
     * Path 1 (full_init || entity==player): bone_link_slots(entity, 0x1B, 0x1F)
     * Path 2 (reattach, non-player): slot = desc[0x99], bone_link_slots(entity, slot, 0x1F) */
    if (full_init || entity == g_player_entity) {
        bone_link_slots(entity, 0x1B, 0x1F);  /* @ 0x473952 */
    } else {
        u8 *desc = *(u8 **)((u8 *)entity + 0xAC);
        i32 slot = desc ? (i32)(u32)desc[0x99] : 0x1B;
        bone_link_slots(entity, slot, 0x1F);   /* @ 0x473966 */
    }
}

/* sub_47398A @ 0x47398A — process zone transition for player entity.
 * Parses zone object name as "%c%c%02d" (e.g. "OB03" -> type='O', sub='B',
 * weapon_id=3). Swaps player weapon type, copies name to entity+0xD2,
 * triggers animation state change, calls sub_471B80 (post-weapon setup).
 * Returns 1 on success, 0 on failure/NULL input. */
static i32 process_zone_transition(void *zone_obj) { /* sub_47398A @ 0x47398A */
    if (!zone_obj) goto fail;
    if (!g_player_entity) goto fail;

    u8 *pe = (u8 *)g_player_entity;
    u8 *desc = *(u8 **)(pe + 0xAC);
    if (!desc) goto fail;

    /* sub_426BB5(zone_obj, 1) — activate trigger entries matching this object */
    activate_trigger_entries(zone_obj, 1); /* sub_426BB5 @ 0x426BB5 */

    /* Check player not already holding this object */
    if (*(i32 *)(pe + 0xD64) == (i32)(intptr_t)zone_obj) goto fail; /* already equipped */

    /* Parse zone object name at [zone_obj+0xA8] as "%c%c%02d" */
    c8 *obj_name = (c8 *)((u8 *)zone_obj + 0xA8);
    i32 weapon_id = 0;

    /* Save old weapon type */
    u8 old_weapon = desc[0x97];

    /* Set new weapon type from parsed object */
    {
        c8 c1 = 0, c2 = 0;
        i32 parsed_id = 0;
        if (sscanf(obj_name, "%c%c%02d", &c1, &c2, &parsed_id) >= 3) {
            weapon_id = parsed_id;
            desc[0x97] = (u8)weapon_id;
        }
    }

    /* Copy object name to entity+0xD2 (weapon name buffer) */
    strcpy((c8 *)(pe + 0xD2), obj_name);

    /* Store zone object pointer at entity+0xD64 */
    *(i32 *)(pe + 0xD64) = (i32)(intptr_t)zone_obj;

    /* sub_472A48 @ 0x472A48 — check if this is a weapon object */
    i32 has_weapon = is_weapon_object(zone_obj);
    print("process_zone_transition: is_weapon_object = %d\n", has_weapon);

    /* sub_472C5C @ 0x472C5C — init weapon mesh (if weapon type) */
    if (has_weapon) {
        scene_weapon_entity_init(zone_obj);
    }

    /* sub_473756 @ 0x473756 — attach weapon entity to player */
    scene_weapon_entity_attach(g_player_entity, zone_obj, has_weapon ? 1 : 0);

    /* Update weapon globals */
    u8 new_weapon = desc[0x97];
    *(i32 *)((u8 *)&g_dword_4EDC30 + 0) = (i32)new_weapon; /* dword_505DDC */

    print("process_zone_transition: weapon swap '%s' (old=%d new=%d)\n",
          obj_name, old_weapon, new_weapon);
    return 1;

fail:
    if (g_player_entity) {
        u8 *pe = (u8 *)g_player_entity;
        u8 *desc = *(u8 **)(pe + 0xAC);
        if (desc) desc[0x97] = 0;
        *(u8 *)(pe + 0xD2) = 0;
    }
    return 0;
}

/* sub_462BCC @ 0x462BCC — dispatch zone trigger event to scripting.
 * Stub until scripting dispatch lands. */
static void dispatch_zone_trigger(u8 zone_id, void *entity) {
    /* sub_462BCC @ 0x462BCC — creature descriptor loader, blocked on sub_462C9E/sub_461DC4 */
    (void)zone_id; (void)entity;
}

/* sub_426BB5 @ 0x426BB5 — activate/deactivate trigger table entries matching
 * a degradable object name. Walks 135 trigger entries (byte_4B3448, stride 28),
 * compares first 4 chars of entry name against zone_obj+0xA8.
 *
 * ECX = zone_obj pointer, EDX = activate (1=set pending, 0=clear pending).
 * When activate=1: sets entry[0x11]=1 (pending flag).
 * When activate=0: sets entry[0x11]=0 (clear flag).
 * Returns 1 if any match found, 0 otherwise. */
static i32 activate_trigger_entries(void *zone_obj, i32 activate) {
    /* sub_426BB5 @ 0x426BB5 */
    if (!zone_obj) return 0;

    const c8 *obj_name = (const c8 *)((u8 *)zone_obj + 0xA8);
    i32 found = 0;

    for (i32 i = 0; i < TRIGGER_TABLE_ENTRIES; i++) {
        u8 *entry = &g_trigger_table[i * TRIGGER_ENTRY_SIZE];

        /* Entry[0] must be non-zero (active trigger slot) */
        if (entry[0] == 0) continue;

        /* Compare first 4 chars of trigger name at entry+0x00 vs obj_name */
        if (strncmp((const c8 *)entry, obj_name, 4) != 0) continue;

        if (activate) {
            entry[0x11] = 1;   /* set pending flag */
            found = 1;
        } else {
            entry[0x11] = 0;   /* clear pending flag */
        }
    }

    return found;
}

/* sub_426D2C @ 0x426D2C — per-frame zone trigger scanner.
 * Walks all 135 trigger table entries looking for one that is both
 * active and pending. If found, resolves to a zone object and attempts
 * a zone transition; on success dispatches the trigger to scripting.
 * Only one trigger fires per frame. */
static i32 check_zone_triggers(void) { /* sub_426D2C @ 0x426D2C */
    i32 result = 0;
    for (i32 i = 0; i < TRIGGER_TABLE_ENTRIES; i++) {
        u8 *entry = &g_trigger_table[i * TRIGGER_ENTRY_SIZE];
        if (entry[0x00] == 0) continue;   /* not active */
        if (entry[0x11] == 0) continue;   /* not pending */

        /* If player is in advanced action state (phase >= 4)
         * and already has a zone assigned, consume & clear trigger */
        if (g_player_entity) {
            i32 phase = ent_get_i32(g_player_entity, 0xB0);
            if (phase >= 4) {
                /* action_ptr = [entity+0xAC], zone_id = [action_ptr+0x97] */
                u8 *action_ptr = *(u8 **)((u8 *)g_player_entity + 0xAC);
                if (action_ptr) {
                    u8 cur_zone = action_ptr[0x97];
                    if (cur_zone != 0) {
                        entry[0x11] = 0;  /* consume trigger */
                        break;
                    }
                }
            }
        }

        /* Resolve trigger entry to a degradable/zone object */
        void *zone_obj = scene_find_degradable_object((const c8 *)(entry + 0x04));  /* sub_475BB6 @ 0x475BB6 */

        /* Attempt zone transition for the player */
        if (process_zone_transition(zone_obj)) {          /* sub_47398A @ 0x47398A */
            /* Dispatch trigger event to scripting */
            if (g_player_entity) {
                u8 *action_ptr = *(u8 **)((u8 *)g_player_entity + 0xAC);
                u8 zone_id = action_ptr ? action_ptr[0x97] : 0;
                dispatch_zone_trigger(zone_id, g_player_entity);  /* sub_462BCC @ 0x462BCC */
            }
            result = 1;
            break;  /* only one trigger per frame */
        }
    }
    return result;
}

/* process_color_palette @ 0x40C2A0 — per-entity palette remapping.
 * Walks the gravit/model hierarchy tree from entity+0x50, remaps each face's
 * packed colour through g_color_lookup[]. Sets g_palette_state = 0xFC.
 * Face structure: +0x5C = packed RGB, +0x28 = remapped colour index.
 * Node structure: +0x20 = atom, +0x7C = first child, +0x84 = next sibling —
 * the walk parks the sibling and descends into the child, same shape as
 * get_node_bounds @ 0x436651.
 * Atom: +0x3C = face array base, +0x24 = face count, face stride = 0x64. */
static u8 g_color_lookup[4096];  /* color_lookup table — populated from GRD */
static u8 g_palette_state = 0;

static void process_color_palette(void *entity) { /* process_color_palette @ 0x40C2A0 */
    if (!entity) return;
    obc_node_t *root = *(obc_node_t **)((u8 *)entity + 0x50);
    if (!root) return;

    g_palette_state = 0xFC;

    obc_node_t *stack[64];
    i32 sp = 0;
    obc_node_t *node = root;

    while (node) {
        atm_atom_t *atom = node->atom[0];
        if (atom && atom->faces) {
            for (i32 i = 0; i < atom->face_count; i++) {
                i32 *face = atom->faces + (size_t)i * ATM_FACE_STRIDE;
                u32 val = (u32)face[0x5C / 4];
                u32 r_part = ((val & 0xF00000) >> 16) << 4;
                u32 g_part = (val & 0xE0) >> 5;
                u32 b_part = (val & 0xF800) >> 8;
                u32 idx = r_part + g_part;
                if (idx + b_part < sizeof(g_color_lookup))
                    face[0x28 / 4] = (i32)g_color_lookup[idx + b_part];
            }
        }
        if (node->next && sp < 64) stack[sp++] = node->next;
        node = node->child;
        if (!node && sp > 0) node = stack[--sp];
    }
}

/* Perspective frustum normal components — computed by build_inverse_z_lut.
 * dbl_510F48: A/sqrt(A²+1) where A = -10240/zfar
 * dbl_510F50: -1/sqrt(A²+1)
 * dbl_510F68: B/sqrt(B²+1) where B = 7680/zfar
 * dbl_510F60: -1/sqrt(B²+1) */
static f64 g_frustum_nx = 0.0;  /* dbl_510F48 @ 0x510F48 */
static f64 g_frustum_nz = 0.0;  /* dbl_510F50 @ 0x510F50 */
static f64 g_frustum_ny = 0.0;  /* dbl_510F68 @ 0x510F68 */
static f64 g_frustum_nw = 0.0;  /* dbl_510F60 @ 0x510F60 */

/* sub_4219B0 @ 0x4219B0 — build inverse-Z lookup table + frustum normals.
 * Fills g_inv_z_array[1..4095] with (zfar * 2^-14) / i; arr[0] stays 0.
 * Then computes 4 frustum plane normal components from zfar. */
static void build_inverse_z_lut(i32 zfar_value) {
    /* sub_4219B0 @ 0x4219B0 */
    void **arr_ptr = game_init_get_inv_z_array_ptr();
    if (!arr_ptr || !*arr_ptr || zfar_value <= 0) return;

    f32 *arr = (f32 *)(*arr_ptr);
    f64 scale = (f64)zfar_value * (1.0 / 16384.0); /* 2^(-14) = dbl_4B0408 @ 0x4B0408 */

    /* arr[0] stays 0 from memset at allocation — sub_421960 @ 0x421960 */
    for (i32 i = 1; i < 0x1000; i++)  /* 0x4219DD–0x4219F7: eax=1..4095 */
        arr[i] = (f32)(scale / (f64)i);

    f64 timer = (f64)zfar_value;
    f64 a = -10240.0 / timer;  /* dbl_4B0430 / timer */
    f64 b =   7680.0 / timer;  /* dbl_4B0440 / timer */

    f64 inv_len_a = 1.0 / sqrt(a * a + 1.0);
    f64 inv_len_b = 1.0 / sqrt(b * b + 1.0);

    g_frustum_nx =  a * inv_len_a;  /* dbl_510F48 */
    g_frustum_nz = -inv_len_a;      /* dbl_510F50 */
    g_frustum_ny =  b * inv_len_b;  /* dbl_510F68 */
    g_frustum_nw = -inv_len_b;      /* dbl_510F60 */
}

/* render_sprite_time_based_effect @ 0x404F80 — periodic animated effect.
 * Reads system timer, extracts 3-bit frame index (0..7) from timer>>6,
 * blits a 64-pixel strip from graphics_context into loading bmap. */
static void render_sprite_time_based_effect(void) {
    /* render_sprite_time_based_effect @ 0x404F80 */
    bmap_t *loading_bmap = NULL;
    u8 *pal = NULL;
    game_init_get_menu_backdrop(&loading_bmap, &pal);
    if (!loading_bmap || !g_graphics_context) return;

    i32 frame = (ls_tick() >> 6) & 7;
    draw_image(loading_bmap, g_graphics_context,
               frame * 64, 0, frame * 64 + 64, 64, 10, 406);
}

/* sub_45619D — build 64×64 navigation grid from world controller zones. */
static void build_nav_grid(void) { /* sub_45619D @ 0x45619D */
    if (!g_world.world_controller) return;
    nav_grid_build(g_world.world_controller,
                   (sol_limit_t *const *)g_world.collision_zones,
                   g_world.num_collisions);
}

/* set_entities_combat_mode @ 0x406AC3 — (re)assert entity type flags.
 * Sets entity+0x90=2 and entity+0x94=2 on player, companion, and
 * secondary companion. Called from game_init phase 38 and scene_load_level. */
void set_entities_combat_mode(void) { /* set_entities_combat_mode @ 0x406AC3 */
    if (g_player_entity) {
        ent_put(g_player_entity, 0x90, 2);
        ent_put(g_player_entity, 0x94, 2);
    }
    if (g_companion_entity) {
        ent_put(g_companion_entity, 0x90, 2);
        ent_put(g_companion_entity, 0x94, 2);
    }
    if (g_secondary_companion_entity) {
        ent_put(g_secondary_companion_entity, 0x90, 2);
        ent_put(g_secondary_companion_entity, 0x94, 2);
    }
}

/* sub_448F4D — LIEU DLL loader / combat-scripting bootstrap.
 * Resets state, installs 40-slot handler vtable, loads LIEU DLL, calls
 * FonctionEditor(INIT). We have LIEU01 natively ported — call lieu_enter. */
static void init_level_scripting(void) { /* sub_448F4D @ 0x448F4D */
    lieu_enter(g_world_state);
}

/* ls_clear_bmap — clear a bmap's pixel buffer to zero.
 * Called for graphics_context and game_context. */
static void clear_bmap(void *bmap) {
    if (!bmap) return;
    bmap_t *b = (bmap_t *)bmap;
    if (b->pixels && b->stride > 0 && b->height > 0)
        memset(b->pixels, 0, (u32)(b->stride * b->height));
}

/* overlay_cursor_image @ 0x45459A — blit sprite region with colour-key.
 * Thin wrapper around ls_bit_move(mode=1). Optional debug outline when
 * global_state_flags bit 1 set and dst==debug_surface. Skips if
 * dword_4B69BC!=0 and dst==graphics_context. */
static void overlay_cursor_image(bmap_t *src, bmap_t *dst,
                                 i32 sx, i32 sy, i32 sw, i32 sh,
                                 i32 dx, i32 dy) {
    /* overlay_cursor_image @ 0x45459A */
    if (!src || !dst) return;
    if (dst == g_graphics_context) return;   /* dword_4B69BC skip simplified */
    ls_bit_move(dst, src, sx, sy, sw, sh, dx, dy, 1);
}

/* sub_45D27A @ 0x45D27A — HUD mask stamp + overlay (77 instructions).
 * If mode != 0: stamps 0xFF into render_buffer2 wherever INTFANI3 has non-zero
 * pixels in a 70×119 region starting at byte offset 0x56 from pixel base.
 * Then always calls overlay_cursor_image to blit the HUD background sprite. */
static void sub_45D27A(void *buf, i32 mode) {
    /* sub_45D27A @ 0x45D27A */
    if (!g_intfani3_bmap || !g_intfani3_bmap->pixels) return;
    if (!g_render_buffer2 || !g_render_buffer2->pixels) return;

    if (mode != 0) {
        i32 width  = 0x46;  /* 70 */
        i32 height = 0x77;  /* 119 */

        /* src starts at flat byte offset 0x56 from INTFANI3 pixel base */
        u8 *src = g_intfani3_bmap->pixels + 0x56;
        /* dst starts at (rb2.height - 119) rows down in render_buffer2 */
        u8 *dst = g_render_buffer2->pixels
                  + (g_render_buffer2->height - height) * g_render_buffer2->stride;

        for (i32 row = 0; row < height; row++) {
            for (i32 col = 0; col < width; col++) {
                if (src[col] != 0)
                    dst[col] = 0xFF;
            }
            src += g_intfani3_bmap->stride;
            dst += g_render_buffer2->stride;
        }
    }

    overlay_cursor_image(g_intfani3_bmap, (bmap_t *)buf,
                         0x56, 0, 0x9C, 0x77, 0, 0x169);
}

/* ls_lock_bmap / ls_unlock_bmap — DirectDraw surface lock/unlock.
 * No-op in software renderer port: pixels are always accessible. */
static void ls_lock_bmap(bmap_t *b) { (void)b; }
static void ls_unlock_bmap(bmap_t *b) { (void)b; }

/* sub_45CAE8 @ 0x45CAE8 — HUD masked darken pass.
 * Applies a brightness remap (index_buffer[pixel+0x600]) to the HUD region,
 * masked by INTFANI3 sprite pixels. Mode 1: health/stamina area (70×119).
 * Mode !=1: combat meter area (32×84). Writes to both game_context and
 * render_buffer1 so the effect persists across frames. */
static void hud_animate_ticker(i32 mode) {
    /* sub_45CAE8 @ 0x45CAE8 */
    bmap_t *intfani3 = game_init_get_intfani3();
    bmap_t *rb1 = game_init_get_render_buffer1();
    bmap_t *gc = (bmap_t *)game_init_get_game_context();
    if (!intfani3 || !rb1 || !gc) return;

    u8 *idx_buf = (u8 *)game_init_get_index_buffer();
    if (!idx_buf) return;

    ls_lock_bmap(intfani3);
    ls_lock_bmap(rb1);
    ls_lock_bmap(gc);

    i32 width, height;
    u8 *src;   /* mask pixels from INTFANI3 */
    u8 *dst_gc;
    u8 *dst_rb;

    if (mode == 1) {
        /* Health/stamina area: 70×119 at bottom of screen */
        width = 0x46;
        height = 0x77;
        dst_gc = gc->pixels + (gc->height - height) * gc->stride;
        dst_rb = rb1->pixels + (rb1->height - height) * rb1->stride;
        src = intfani3->pixels + 0x56;
    } else {
        /* Combat meter area: 32×84 at bottom-right */
        width = 0x20;
        height = 0x54;
        dst_gc = gc->pixels + (gc->height - 0x14 - height) * gc->stride
                 + 0x26C - width;
        dst_rb = rb1->pixels + (rb1->height - 0x14 - height) * rb1->stride
                 + 0x26C - width;
        src = intfani3->pixels + 0x9D;
    }

    /* 8-bit darken pass (colour_mode == 0) */
    for (i32 row = 0; row < height; row++) {
        for (i32 col = 0; col < width; col++) {
            if (src[col] != 0) {
                u8 darkened = idx_buf[dst_rb[col] + 0x600];
                dst_gc[col] = darkened;
                dst_rb[col] = darkened;
            }
        }
        src += intfani3->stride;
        dst_rb += rb1->stride;
        dst_gc += gc->stride;
    }

    ls_unlock_bmap(gc);
    ls_unlock_bmap(intfani3);
    ls_unlock_bmap(rb1);
}

/* sub_45CD9C @ 0x45CD9C — HUD health bar + weapon/special mode indicator.
 * Reads player descriptor health (entity+0xAC→+0x84), scales to 66-pixel bar.
 * Also draws weapon icon (entity+0xD60) or special mode icon, plus stamina bar
 * (score_counter scaled to 98 pixels). Dirty-check caches prevent redundant redraws. */
static void hud_draw_health_bar(i32 mode) {
    /* sub_45CD9C @ 0x45CD9C */
    extern u8 color_table[];
    if (g_hud_active == -1) return;

    bmap_t *intfani3 = game_init_get_intfani3();
    if (!intfani3) return;
    intfani3->reserved_18 = (uintptr_t)color_table;

    bmap_t *rb1 = game_init_get_render_buffer1();
    bmap_t *gc = (bmap_t *)game_init_get_game_context();
    if (!rb1 || !gc) return;

    /* Dirty check: skip redraw if nothing changed and mode==0 */
    i32 cur_health = 0;
    if (g_player_entity) {
        u8 *desc = *(u8 **)((u8 *)g_player_entity + 0xAC);
        if (desc) cur_health = *(i32 *)(desc + 0x84);
    }

    if (cur_health == g_cached_health &&
        g_color_mode == g_cached_color_mode &&
        g_special_mode_active == g_cached_special_mode &&
        g_score_counter == g_cached_score &&
        mode == 0) {
        goto health_done;
    }

    /* Health bar: 66 pixels, scale = 66/1024 = 0.064453125 */
    overlay_cursor_image(intfani3, rb1, 0x56, 0, 0x9C, 0x77, 0, 0x169);

    {
        f64 fill = (f64)cur_health * (66.0 / 1024.0);
        i32 bar_y = (i32)(66.0 - fill);

        overlay_cursor_image(intfani3, rb1,
                             0, bar_y, 9, 0x42, 0x1F, bar_y + 0x194);
        overlay_cursor_image(intfani3, gc,
                             0, bar_y, 9, 0x42, 0x1F, bar_y + 0x194);
    }
    g_cached_health = cur_health;

    /* Weapon/special mode icon */
    if (g_player_entity && *(i32 *)((u8 *)g_player_entity + 0xD60) != 0) {
        overlay_cursor_image(intfani3, rb1, 0x0A, 0x1B, 0x22, 0x35, 0x18, 0x177);
        overlay_cursor_image(intfani3, gc,  0x0A, 0x1B, 0x22, 0x35, 0x18, 0x177);
    } else if (g_special_mode_active) {
        overlay_cursor_image(intfani3, rb1, 0x0A, 0x00, 0x22, 0x1A, 0x18, 0x177);
        overlay_cursor_image(intfani3, gc,  0x0A, 0x00, 0x22, 0x1A, 0x18, 0x177);
    }
    g_cached_special_mode = g_special_mode_active;

    /* Stamina bar: 98 pixels, scale = 98/1024 = 0.095703125 */
    {
        f64 fill2 = (f64)g_score_counter * (98.0 / 1024.0);
        f64 bar_val = 98.0 - fill2;
        if (g_score_counter == 0x3FF && bar_val == 0.0) bar_val = 1.0;
        i32 bar_y2 = (i32)bar_val;

        overlay_cursor_image(intfani3, rb1,
                             0x23, bar_y2, 0x55, 0x62, 0x0A, bar_y2 + 0x16C);
        overlay_cursor_image(intfani3, gc,
                             0x23, bar_y2, 0x55, 0x62, 0x0A, bar_y2 + 0x16C);
    }
    g_cached_score = g_score_counter;

    if (g_player_movement_enabled)
        hud_animate_ticker(1);  /* sub_45CAE8(1) */

    /* Final blit to graphics context (mode==1 only, not render_mode 1) */
    if (mode == 1 && g_render_mode_flag != 1 && g_viewport_active == 0) {
        draw_image(g_graphics_context, gc, 0, 0x169, 0x46, 0x1E0, 0, 0x169);
    }

health_done:
    g_cached_color_mode = g_color_mode;
}

/* sub_45D0BA @ 0x45D0BA — HUD combat/stamina meter indicator (right side).
 * Only active when render_mode_flag==1. Draws a 61-pixel vertical bar from
 * INTFANI3 onto render_buffer1, scaled by combat_meter/combat_meter_max ratio.
 * Dirty-check caches prevent redundant redraws. */
static void hud_draw_mode_indicator(i32 mode) {
    /* sub_45D0BA @ 0x45D0BA */
    extern u8 color_table[];
    if (g_hud_active == -1) return;

    bmap_t *intfani3 = game_init_get_intfani3();
    if (!intfani3) return;
    intfani3->reserved_18 = (uintptr_t)color_table;

    bmap_t *rb1 = game_init_get_render_buffer1();
    bmap_t *gc = (bmap_t *)game_init_get_game_context();
    if (!rb1 || !gc) return;

    /* Dirty check */
    if (g_combat_meter == g_cached_meter &&
        g_color_mode == g_cached_color_mode2 &&
        mode == 0) {
        goto indicator_done;
    }

    /* Background frame for indicator area */
    overlay_cursor_image(intfani3, rb1, 0x9D, 0, 0xBD, 0x54, 0x24C, 0x178);

    /* Bar fill: 61 pixels, ratio = combat_meter / combat_meter_max */
    if (g_combat_meter < 0) g_combat_meter = 0;
    {
        f64 ratio = (g_combat_meter_max > 0)
            ? (f64)g_combat_meter / (f64)g_combat_meter_max
            : 0.0;
        i32 bar_y = (i32)(61.0 - ratio * 61.0);

        overlay_cursor_image(intfani3, rb1,
                             9, bar_y + 0x36, 0x0C, 0x73, 0x24F, bar_y + 0x189);
    }
    g_cached_meter = g_combat_meter;

    /* Blit composited region from render_buffer1 to game_context */
    draw_image(gc, rb1, 0x24C, 0x178, 0x26C, 0x1CC, 0x24C, 0x178);

indicator_done:
    g_cached_color_mode2 = g_color_mode;
}

/* update_player_interface @ 0x45D1F6 — HUD overlay controller.
 * Composites INTFANI3 health bar + mode indicator sprites onto game_context.
 * ECX=game_context bmap, EDX=mode (2=full init). */
static void update_player_interface(void *ctx, i32 mode) {
    /* update_player_interface @ 0x45D1F6 */
    bmap_t *intfani3 = game_init_get_intfani3();
    bmap_t *dst = (bmap_t *)ctx;
    if (!intfani3 || !dst) return;

    /* Primary HUD overlay — health bar area @ 0x45D205 */
    overlay_cursor_image(intfani3, dst, 0x56, 0, 0x9C, 0x77, 0, 0x169);

    /* Secondary indicator area — only when render_mode_flag == 1 */
    if (g_render_mode_flag == 1) {
        overlay_cursor_image(intfani3, dst, 0x9D, 0, 0xBD, 0x54, 0x24C, 0x178);
    }

    /* Health bar / stamina / weapon indicators */
    hud_draw_health_bar(mode);  /* sub_45CD9C @ 0x45CD9C */

    /* Combat meter indicator (right side) */
    if (g_render_mode_flag == 1) {
        hud_draw_mode_indicator(mode);  /* sub_45D0BA @ 0x45D0BA */
    }
}

/* sub_45E070 @ 0x45E070 — screen light intensity map downsampler.
 * Scans render_buffer2 (640x480 8bpp) in 20x20 pixel tiles (24 rows x 32
 * cols = 768 tiles), records the brightest pixel in each tile into
 * g_brightness_map[]. Result is a coarse screen-space brightness lookup
 * used to shade 3D character models to match pre-rendered background
 * lighting. */
static void build_brightness_map(void) { /* sub_45E070 @ 0x45E070 */
    if (!g_render_buffer2 || !g_render_buffer2->pixels) return;
    u8 *pixels = g_render_buffer2->pixels;
    i32 stride = g_render_buffer2->stride;
    for (i32 i = 0; i < 768; i++) {
        i32 row = i / 32;
        i32 col = i % 32;
        u8 *tile = pixels + row * 20 * stride + col * 20;
        u8 mx = 0;
        for (i32 ty = 0; ty < 20; ty++) {
            u8 *sl = tile + ty * stride;
            for (i32 tx = 0; tx < 20; tx++) {
                if (sl[tx] > mx) mx = sl[tx];
            }
        }
        g_brightness_map[i] = mx;
    }
}

/* sub_47674F — composite_entity_pass: renders all entities onto scene buffers.
 * Iterates 8 entity slots, builds draw order, composites animated frames.
 * ECX=0 draws all entities without context filtering.
 * Depends on sub_4762BF (single entity renderer), sub_4728DF (anim parser),
 * sub_4731F6 (multi-frame animated renderer). No-op until entity system lands. */
static void composite_entity_pass(i32 arg) {
    /* sub_47674F @ 0x47674F — entity compositor (967 bytes, blocked on entity renderer) */
    (void)arg;
}

/* render_entity_with_ui @ 0x4024AF — full companion entity rendering pipeline.
 * 968 bytes, 268 instructions, 15 sub-function calls. Copies framebuffers,
 * applies model transform, sets entity texture, renders with depth test,
 * handles combat overlay effects. Needs entire rendering subsystem — no-op. */
static void render_entity_with_ui(void *entity, i32 mode) {
    /* render_entity_with_ui @ 0x4024AF — 968 bytes, blocked on entity renderer */
    (void)entity; (void)mode;
}

/* dword_511624 @ 0x511624 — rendering state flag, set during effect position init. */
static i32 g_dword_511624 = 0;

/* initialize_effect_position @ 0x4357CF — project entity to screen, sample
 * half-res lightmap for background brightness at entity position.
 * ECX=scene_root (entity+0x50 bone hierarchy), EDX=transform context,
 * arg0=output brightness pointer (entity+0x64).
 * Uses sub_4915C0 (camera projection) — substituted with scene_project_world. */
static void initialize_effect_position(void *scene_root, void *transform, i32 *out) {
    /* initialize_effect_position @ 0x4357CF */
    bmap_t *halfres = game_init_get_halfres_buffer();
    if (!halfres || !halfres->pixels) return;
    if (!scene_root) return;

    u8 *root = (u8 *)scene_root;
    i32 wx = *(i32 *)(root + 0x3C);
    i32 wy = (i32)(intptr_t)transform;
    i32 wz = *(i32 *)(root + 0x44);

    g_dword_511624 = 2;  /* dword_511624 @ 0x511624 */

    i32 sx = 0, sy = 0;
    scene_project_world(wx, wy, wz, &sx, &sy);

    if (sx < 0) sx = 0;
    if (sx > 639) sx = 639;
    if (sy < 0) sy = 0;
    if (sy > 479) sy = 479;

    /* Half coordinates → index into 320×240 buffer */
    sx >>= 1;
    sy >>= 1;

    i32 stride = halfres->stride > 0 ? halfres->stride : (i32)halfres->width;
    i32 offset = stride * sy + sx;
    u8 brightness = halfres->pixels[offset];

    if (out) *out = (i32)(brightness * 0.75);  /* dbl_4B05E8 = 0.75 */
}

/* apply_model_transform @ 0x4359FF — position entity model in screen space.
 * Sets entity+0x54=2 (transforming), calls initialize_effect_position to
 * project world coords and sample background brightness, then sets
 * entity+0x54=1 (ready). */
static void apply_model_transform(void *entity, void *transform) {
    /* apply_model_transform @ 0x4359FF */
    if (!entity) return;
    u8 *ent = (u8 *)entity;
    /* Read the scene root BEFORE touching +0x54. The original walks the two
     * as separate 32-bit slots; here +0x50 holds a 64-bit host pointer that
     * spans 0x50..0x57, so the asm's "set state, then load root" order would
     * shred the pointer's high half and hand a wild address to
     * initialize_effect_position. */
    void *scene_root = *(void **)(ent + 0x50);
    /* The original brackets this with entity+0x54 = 2 (transforming) then 1
     * (ready). Both writes are DROPPED here: +0x50 holds a 64-bit host
     * pointer spanning 0x50..0x57, so storing an i32 at +0x54 overwrites its
     * high half. That corruption persisted across level loads and crashed
     * process_color_palette on the next room, which walks entity+0x50 as a
     * scene hierarchy. Nothing in the port reads +0x54. */
    initialize_effect_position(scene_root, transform, (i32 *)(ent + 0x64));
}

/* system_get_gametime — read current system game timer.
 * Returns milliseconds from platform tick counter (ls_tick).
 * Mirrors asm: call system_get_gametime */
static i32 system_get_gametime(void) {
    return (i32)ls_tick();
}

/* restore_display_mode — EDX=0x1D, ECX=1.
 * Original: DirectDraw RestoreDisplayMode. No-op on modern platforms.
 * Mirrors asm: mov edx, 1Dh / mov ecx, 1 / call restore_display_mode */
static void restore_display_mode(i32 ecx, i32 edx) {
    /* restore_display_mode — DirectDraw, no-op on modern platforms */
    (void)ecx; (void)edx;
}

/* sub_406316 @ 0x406316 — per-entity time sync.
 * Zeros +0xC40, sets +0xC34/+0xC2C/+0xC28 = current_game_time. */
static void sync_entity_time(void *entity, i32 game_time) {
    /* sub_406316 @ 0x406316 */
    if (!entity) return;
    u8 *e = (u8 *)entity;
    *(i32 *)(e + 0xC40) = 0;
    *(i32 *)(e + 0xC34) = game_time;
    *(i32 *)(e + 0xC2C) = game_time;
    *(i32 *)(e + 0xC28) = game_time;
}

/* sub_40636D @ 0x40636D — game time update + entity time sync.
 * Backs up current_game_time, refreshes from system timer, syncs all
 * entities and game_structures time fields. */
static void update_game_time_entities(void) { /* sub_40636D @ 0x40636D */
    if (!state) return;
    state->time_backup = state->current_game_time;  /* time_backup @ 0x4ED9D0 */
    state->current_game_time = (u32)system_get_gametime(); /* current_game_time @ 0x4EDFB4 */
    i32 t = (i32)state->current_game_time;

    sync_entity_time(g_player_entity, t);
    sync_entity_time(g_companion_entity, t);
    sync_entity_time(g_secondary_companion_entity, t);

    for (i32 i = 0; i < GAME_STRUCT_COUNT; i++) {
        u8 *entry = g_game_structures + i * GAME_STRUCT_STRIDE;
        if (*(void **)entry) {  /* +0x00: entity pointer non-NULL = active */
            *(i32 *)(entry + 0x2C) = t;  /* +0x2C: time field */
        }
    }
}

/*
 * scene_load_camera_anchor — load INF projection params and estimate the
 * character's screen anchor position for camera cam_idx.
 *
 * Uses the camera's zone centroid as world (X,Z) and solves for world Y
 * such that the character appears at target screen (320, 350). Falls back
 * to (320, 350) on any parse failure.
 *
 * Sets g_scene_cam_sx, g_scene_cam_sy.
 */
static void scene_load_camera_anchor(i32 level_id, i32 cam_idx) {
    g_scene_cam_sx = 320;
    g_scene_cam_sy = 350;

    c8 virt[64];
    snprintf(virt, sizeof(virt), "DATAS>L%02d>L%02d.INF", level_id, level_id);
    c8 host[512];
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, virt, host, (i32)sizeof(host))) return;

    FILE *f = ls_open(host, "rb");
    if (!f) return;

    /* Validate magic */
    i32 magic = 0;
    if (fread(&magic, 4, 1, f) != 1 || magic != 7) { ls_close(f); return; }

    /* Skip to camera zone struct for cam_idx */
    /* Format: ts(4) + world_ctrl(0x208) + n_cam(4) + n_cam×0x208 + ... */
    fseek(f, 4 + 0x208, SEEK_CUR);
    i32 n_cam = 0;
    if (fread(&n_cam, 4, 1, f) != 1 || cam_idx >= n_cam) { ls_close(f); return; }

    /* Start of the camera-struct array (independent of cam_idx). */
    long cams_base = ftell(f);
    /* Seek to cam_idx zone struct (+0x1D8 for n_verts, +0x1DC for vertices) */
    long cam_struct_off = cams_base + (long)cam_idx * 0x208;
    fseek(f, cam_struct_off + 0x1D8, SEEK_SET);
    i32 n_verts = 0;
    if (fread(&n_verts, 4, 1, f) != 1 || n_verts <= 0 || n_verts > 64) {
        ls_close(f); return;
    }
    /* Read zone polygon vertices (3 i32 each: x, y_ignored, z) */
    i64 sum_x = 0, sum_z = 0; i32 valid = 0;
    for (i32 i = 0; i < n_verts && i < 32; i++) {
        i32 vx = 0, vy_ign = 0, vz = 0;
        if (fread(&vx, 4, 1, f) != 1) break;
        if (fread(&vy_ign, 4, 1, f) != 1) break;
        if (fread(&vz, 4, 1, f) != 1) break;
        /* Filter out garbage vertices (reasonable world range) */
        if (vx > -3000000 && vx < 3000000 && vz > -3000000 && vz < 3000000) {
            sum_x += vx; sum_z += vz; valid++;
        }
    }
    if (valid == 0) { ls_close(f); return; }
    i32 zone_x = (i32)(sum_x / valid);
    i32 zone_z = (i32)(sum_z / valid);

    /* Skip to extra section (projection matrices). The extra section begins
     * after ALL n_cam camera structs — measured from cams_base, NOT from the
     * per-camera cam_struct_off (which already includes cam_idx*0x208). */
    fseek(f, cams_base + (long)n_cam * 0x208, SEEK_SET);
    /* n_inex */
    i32 n_inex = 0;
    if (fread(&n_inex, 4, 1, f) != 1) { ls_close(f); return; }
    fseek(f, (long)n_inex * 0x208, SEEK_CUR);
    /* n_stairs */
    i32 n_stairs = 0;
    if (fread(&n_stairs, 4, 1, f) != 1) { ls_close(f); return; }
    fseek(f, (long)n_stairs * 0x208, SEEK_CUR);
    /* n_extra */
    i32 n_extra = 0;
    if (fread(&n_extra, 4, 1, f) != 1 || cam_idx >= n_extra) { ls_close(f); return; }

    /* Read cam_idx projection param table (first 0x78 bytes of 0xA8 entry) */
    fseek(f, (long)cam_idx * 0xA8, SEEK_CUR);
    i32 param[30];
    if (fread(param, 4, 30, f) != 30) { ls_close(f); return; }
    ls_close(f);

    if (getenv("DKE_CAMDBG")) {
        print("cam param R[0..8]=%d %d %d %d %d %d %d %d %d\n",
              param[0],param[1],param[2],param[3],param[4],param[5],param[6],param[7],param[8]);
        print("cam param T[9..11]=%d %d %d  s[24..26]=%d %d %d\n",
              param[9],param[10],param[11],param[24],param[25],param[26]);
    }

    /* Cache the camera matrix as the engine stores it (30 ints, param[0..8]
     * = R with R(row,col)=param[row+col*3], param[9..11]=T raw, param[24]=
     * globalScale = round(10240/tan(fov/2))). Projection is done by
     * scene_project_world (perspective 1/z). */
    if (param[24] == 0) return;
    for (i32 i = 0; i < 9; i++) g_proj_R[i] = param[i];
    g_proj_T[0] = param[9]; g_proj_T[1] = param[10]; g_proj_T[2] = param[11];
    g_proj_gscale    = param[24];
    g_proj_zone_z    = (i32)zone_z;
    g_proj_anchor_wx = zone_x;
    /* Floor plane Y = 0 (PosPerso characters are placed at world Y 0); the
     * player shares it. Personas still use their own PosPerso ground. */
    g_proj_ground_wy = 0;
    g_proj_valid     = 1;

    /* Project the zone centroid as the fallback anchor screen point. */
    i32 sx = 320, sy = 240;
    scene_project_world(zone_x, g_proj_ground_wy, (i32)zone_z, &sx, &sy);
    g_scene_cam_sx = sx;
    g_scene_cam_sy = sy;

    print("scene: camera cam=%d zone=(%d,%d) R0=%d T=(%d,%d,%d) gscale=%d anchor_screen=(%d,%d)\n",
          cam_idx, zone_x, (i32)zone_z, param[0], param[9], param[10], param[11],
          param[24], sx, sy);
}

/* -------------------------------------------------------------------------
 * scene_load_camera_regions — read every camera's INF zone polygon (world
 * X/Z) into g_cam_region, so scene_camera_for_position can pick the active
 * camera by player position (dkev auto-camera sub_45A67C).
 * ------------------------------------------------------------------------- */
static void scene_load_camera_regions(i32 level_id) {
    g_cam_region_count = 0;
    c8 virt[64], host[512];
    snprintf(virt, sizeof(virt), "DATAS>L%02d>L%02d.INF", level_id, level_id);
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, virt, host, (i32)sizeof(host))) return;
    FILE *f = ls_open(host, "rb");
    if (!f) return;
    i32 magic = 0;
    if (fread(&magic, 4, 1, f) != 1 || magic != 7) { ls_close(f); return; }
    fseek(f, 4 + 0x208, SEEK_CUR);          /* timestamp + world_ctrl */
    i32 n_cam = 0;
    if (fread(&n_cam, 4, 1, f) != 1 || n_cam <= 0) { ls_close(f); return; }
    if (n_cam > 32) n_cam = 32;
    long cams_base = ftell(f);
    for (i32 c = 0; c < n_cam; c++) {
        fseek(f, cams_base + (long)c * 0x208 + 0x1D8, SEEK_SET);
        i32 nv = 0;
        if (fread(&nv, 4, 1, f) != 1 || nv <= 0 || nv > 32) { g_cam_region[c].nv = 0; continue; }
        cam_region_t *r = &g_cam_region[c];
        r->nv = 0;
        for (i32 i = 0; i < nv; i++) {
            i32 vx = 0, vy = 0, vz = 0;
            if (fread(&vx, 4, 1, f) != 1 || fread(&vy, 4, 1, f) != 1 || fread(&vz, 4, 1, f) != 1) break;
            if (vx > -3000000 && vx < 3000000 && vz > -3000000 && vz < 3000000) {
                r->vx[r->nv] = vx; r->vz[r->nv] = vz; r->nv++;
            }
        }
    }
    g_cam_region_count = n_cam;
    ls_close(f);
    print("scene: loaded %d camera regions\n", n_cam);
    if (getenv("DKE_CAMDBG")) {
        for (i32 c = 0; c < n_cam; c++) {
            cam_region_t *r = &g_cam_region[c];
            i32 minx = 1<<30, maxx = -(1<<30), minz = 1<<30, maxz = -(1<<30);
            for (i32 i = 0; i < r->nv; i++) {
                if (r->vx[i] < minx) minx = r->vx[i];
                if (r->vx[i] > maxx) maxx = r->vx[i];
                if (r->vz[i] < minz) minz = r->vz[i];
                if (r->vz[i] > maxz) maxz = r->vz[i];
            }
            print("  cam[%d] nv=%d X[%d..%d] Z[%d..%d]\n", c, r->nv, minx, maxx, minz, maxz);
        }
    }
}

/* Point-in-polygon (X/Z plane), ray-casting. */
static i32 scene_pt_in_region(i32 px, i32 pz, const cam_region_t *r) {
    if (r->nv < 3) return 0;
    i32 inside = 0;
    for (i32 i = 0, j = r->nv - 1; i < r->nv; j = i++) {
        if ((r->vz[i] > pz) != (r->vz[j] > pz)) {
            i64 xcross = (i64)(r->vx[j] - r->vx[i]) * (pz - r->vz[i])
                       / (r->vz[j] - r->vz[i]) + r->vx[i];
            if (px < xcross) inside = !inside;
        }
    }
    return inside;
}

/* Camera whose region contains (wx,wz), or -1. Prefers the current camera
 * (hysteresis) so the player must fully leave a region before switching. */
static i32 scene_camera_for_position(i32 wx, i32 wz, i32 current) {
    if (current >= 0 && current < g_cam_region_count &&
        scene_pt_in_region(wx, wz, &g_cam_region[current]))
        return current;
    for (i32 c = 0; c < g_cam_region_count; c++)
        if (scene_pt_in_region(wx, wz, &g_cam_region[c])) return c;
    return -1;
}

/* -------------------------------------------------------------------------
 * scene_load_inf_pitch — read camera pitch from L%02d.INF projection matrix.
 *
 * Returns a cosine_table index (0..4095) for the camera pitch angle,
 * or 300 (~26°) as a safe default on any parse error.
 * ------------------------------------------------------------------------- */
static i32 scene_load_inf_pitch(i32 level_id, i32 cam_idx) {
    /* Build INF path: DATAS>L{level_id:02d}>L{level_id:02d}.INF */
    c8 virt[64];
    snprintf(virt, sizeof(virt), "DATAS>L%02d>L%02d.INF", level_id, level_id);
    c8 host[512];
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, virt, host, (i32)sizeof(host))) return 300;

    FILE *f = ls_open(host, "rb");
    if (!f) return 300;

    /* Read and validate magic */
    i32 magic = 0;
    if (fread(&magic, 4, 1, f) != 1 || magic != 7) { ls_close(f); return 300; }

    /* Skip: timestamp(4) + world_ctrl(0x208) */
    fseek(f, 4 + 0x208, SEEK_CUR);
    i32 n_cam = 0;
    if (fread(&n_cam, 4, 1, f) != 1) { ls_close(f); return 300; }
    /* Skip n_cam camera zone structs */
    fseek(f, (long)n_cam * 0x208, SEEK_CUR);

    /* Skip inexplorable section */
    i32 n_inex = 0;
    if (fread(&n_inex, 4, 1, f) != 1) { ls_close(f); return 300; }
    fseek(f, (long)n_inex * 0x208, SEEK_CUR);

    /* Skip stairs section */
    i32 n_stairs = 0;
    if (fread(&n_stairs, 4, 1, f) != 1) { ls_close(f); return 300; }
    fseek(f, (long)n_stairs * 0x208, SEEK_CUR);

    /* Extra section: n_extra × 0xA8 (projection matrices) */
    i32 n_extra = 0;
    if (fread(&n_extra, 4, 1, f) != 1) { ls_close(f); return 300; }
    if (cam_idx < 0 || cam_idx >= n_extra) { ls_close(f); return 300; }

    /* Seek to cam_idx's projection matrix (+20 bytes for param[5]) */
    long entry_off = (long)cam_idx * 0xA8 + 5 * 4;  /* param[5] = R[1][2] */
    fseek(f, entry_off, SEEK_CUR);

    i32 r12 = 0;
    if (fread(&r12, 4, 1, f) != 1) { ls_close(f); return 300; }
    ls_close(f);

    /* Convert sin(pitch) Q14 → cosine_table index */
    float sin_val = (float)r12 / 16384.0f;
    if (sin_val >  1.0f) sin_val =  1.0f;
    if (sin_val < -1.0f) sin_val = -1.0f;
    float theta_rad = asinf(sin_val);
    i32 idx = (i32)(theta_rad * 4096.0f / (2.0f * 3.14159265f));
    if (idx < 0) idx += 4096;
    idx &= 0xFFF;

    print("scene: INF pitch: cam=%d R[1][2]=%d -> theta=%.1f deg -> idx=%d\n",
          cam_idx, r12, theta_rad * 180.0f / 3.14159265f, idx);
    return idx;
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
    set_display_mode(0x1D);

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

    /* Check folder existence; if missing call detect_cdrom(1) */
    if (!ls_exists(host_folder)) {
        print("scene: level folder '%s' not found — calling detect_cdrom(1)\n", host_folder);
        detect_cdrom(1);   /* sub_469E34 @ 0x469E34 */
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
        handle_data_l44_faux();   /* handle_data_l44_faux — test level no-op */
    }

    /* -----------------------------------------------------------------
     * Phase 5 — zero byte_511760; zero g_player_entity offsets
     * Asm: mov byte_511760, 0
     *      cmp current_entity, 0 / jz skip
     *        mov [eax+0A0h], 0   ; current_entity->+0xA0 = 0
     * ----------------------------------------------------------------- */
    print("scene: phase 5 — zero byte_511760, clear current_entity->+0xA0\n");
    g_byte_511760 = 0;
    if (g_player_entity) {
        *(i32 *)((u8 *)g_player_entity + 0xA0) = 0;  /* current_entity->+0xA0 @ asm */
    }

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
     * Phase 7 — zero g_player_entity+0xD04 and copy to +0xD08
     * Asm: mov [eax+0D04h], 0
     *      mov eax, [eax+0D04h] / mov [ecx+0D08h], eax
     * ----------------------------------------------------------------- */
    print("scene: phase 7 — zero g_player_entity+0xD04 / +0xD08\n");
    if (g_player_entity) {
        u8 *pe = (u8 *)g_player_entity;
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
        memset(g_dword_543C60, 0, sizeof(g_dword_543C60));  /* dword_543C60 @ 0x543C60 */
        load_world_controller();   /* sub_46D305 @ 0x46D305 — world controller alloc */
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
    print("scene: phase 10 — load_level_assets (sub_46D0EA)\n");
    load_level_assets();   /* sub_46D0EA @ 0x46D0EA */

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
    print("scene: phase 11 — colour-mode / texture library setup (partial)\n");
    can_do_32k(state ? (i32)state->colour_mode : 1);   /* can_do_32k — always true */

    /* -----------------------------------------------------------------
     * Phase 12 — sub_4640BC (player placement in world)
     * Asm: cmp dword_4B727C, 0 / jz skip
     *        push ... / mov ecx, game_state / call sub_4640BC
     * ----------------------------------------------------------------- */
    print("scene: phase 12 — place_player_defaults (sub_4640BC)\n");
    place_player_defaults();   /* sub_4640BC @ 0x4640BC */

    /* -----------------------------------------------------------------
     * Phase 13 — sub_426D2C + damage counter zero
     * Asm: call sub_426D2C
     *      mov dword_506520, 0
     *      mov damage_counter, 0
     * ----------------------------------------------------------------- */
    print("scene: phase 13 — check_zone_triggers, zero damage counters\n");
    check_zone_triggers();   /* sub_426D2C @ 0x426D2C */

    /* -----------------------------------------------------------------
     * Phase 14 — player entity combat defaults
     * Asm: mov [eax+0C64h], 10000h
     *      mov [eax+0C68h], 4000h
     *      mov dword_4DD9B4, 1
     * ----------------------------------------------------------------- */
    print("scene: phase 14 — player entity combat defaults\n");
    if (g_player_entity) {
        u8 *pe = (u8 *)g_player_entity;
        *(i32 *)(pe + 0xC64) = 0x10000;
        *(i32 *)(pe + 0xC68) = 0x4000;
    }
    g_combat_enabled = 1;  /* dword_4DD9B4 @ 0x4DD9B4 */

    /* -----------------------------------------------------------------
     * Phase 15 — process_color_palette for player + companions
     * Asm: mov ecx, [eax+50h] / call process_color_palette (×3)
     * ----------------------------------------------------------------- */
    print("scene: phase 15 — process_color_palette (player+companions)\n");
    process_color_palette(g_player_entity);                /* process_color_palette @ 0x40C2A0 */
    process_color_palette(g_companion_entity);             /* process_color_palette @ 0x40C2A0 */
    process_color_palette(g_secondary_companion_entity);   /* process_color_palette @ 0x40C2A0 */

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
    if (g_player_entity) {
        u8 *pe = (u8 *)g_player_entity;
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
    print("scene: phase 17 — render_sprite_time_based_effect + sub_45619D\n");
    render_sprite_time_based_effect();   /* sub_404F80 @ 0x404F80 */
    build_nav_grid();                        /* sub_45619D @ 0x45619D */
    if (getenv("DKE_FLOORDBG") && g_world.world_controller) {
        const sol_limit_t *w = g_world.world_controller;
        print("floor: world_ctrl verts=%d minY=%d maxY=%d\n",
              w->vertex_count, w->min_y, w->max_y);
        for (i32 vi = 0; vi < w->vertex_count && vi < 16; vi++)
            print("floor:   v%d = (%d, %d, %d)\n", vi,
                  w->vertices[vi][0], w->vertices[vi][1], w->vertices[vi][2]);
        for (i32 zi = 0; zi < g_world.num_collisions; zi++) {
            const sol_limit_t *z = g_world.collision_zones[zi];
            if (!z) continue;
            print("floor: coll[%d] '%s' flags=0x%X X[%d..%d] Z[%d..%d] cx=%d cy=%d\n",
                  zi, z->name, z->flags, z->min_x, z->max_x, z->min_y, z->max_y,
                  z->center_x, z->center_y);
        }
        for (i32 zi = 0; zi < g_world.num_cameras; zi++) {
            const sol_limit_t *z = g_world.camera_zones[zi];
            if (!z) continue;
            print("floor: cam[%d] '%s' X[%d..%d] Z[%d..%d]\n",
                  zi, z->name, z->min_x, z->max_x, z->min_y, z->max_y);
        }
    }

    /* -----------------------------------------------------------------
     * Phase 18 — viewport globals
     * Asm: mov dword_4DDE20, 1
     *      mov dword_4B7B24/14/18, 0
     *      mov dword_4B7B1C, 27Fh   ; 639
     *      mov dword_4B7B20, 1DFh   ; 479
     * ----------------------------------------------------------------- */
    print("scene: phase 18 — viewport globals (640x480)\n");
    /* dword_4DDE20 = 1 (viewport active flag) @ 0x4DDE20 */
    g_viewport_active = 1;
    g_viewport_x0 = 0;         /* dword_4B7B14 @ 0x4B7B14 */
    g_viewport_y0 = 0;         /* dword_4B7B18 @ 0x4B7B18 */
    g_viewport_x1 = 0x27F;    /* dword_4B7B1C @ 0x4B7B1C (639) */
    g_viewport_y1 = 0x1DF;    /* dword_4B7B20 @ 0x4B7B20 (479) */
    g_viewport_flags = 0;     /* dword_4B7B24 @ 0x4B7B24 */

    /* -----------------------------------------------------------------
     * Phase 19 — set_entities_combat_mode + sub_448F4D
     * ----------------------------------------------------------------- */
    print("scene: phase 19 — set_entities_combat_mode + init_level_scripting\n");
    set_entities_combat_mode();   /* set_entities_combat_mode @ 0x406AC3 */
    init_level_scripting();                 /* sub_448F4D @ 0x448F4D */

    /* -----------------------------------------------------------------
     * Phase 20 — ls_clear_bmap (graphics_context, game_context)
     *           sub_45D27A(render_buffer1, 1)
     *           update_player_interface(game_context, 2)
     *           sub_45E070, sub_47674F(0)
     * ----------------------------------------------------------------- */
    print("scene: phase 20 — clear bmaps + interface update\n");
    clear_bmap(g_graphics_context);                      /* ls_clear_bmap(graphics_context) @ 0x46E63B */
    clear_bmap(game_init_get_game_context());             /* ls_clear_bmap(game_context) @ 0x46E644 */
    sub_45D27A(game_init_get_render_buffer1(), 1);        /* sub_45D27A(render_buffer1, 1) */
    update_player_interface(game_init_get_game_context(), 2); /* update_player_interface @ 0x45D1F6 */
    build_brightness_map();                     /* sub_45E070 @ 0x45E070 */
    composite_entity_pass(0);              /* sub_47674F @ 0x47674F */

    /* -----------------------------------------------------------------
     * Phase 21 — render companion entities (if present)
     * Asm: cmp companion_entity, 0 / jz skip
     *        render_entity_with_ui(companion_entity, 0)
     *      cmp secondary_companion_entity, 0 / jz skip
     *        render_entity_with_ui(secondary_companion_entity, 0)
     * ----------------------------------------------------------------- */
    print("scene: phase 21 — render companions\n");
    /* asm: guard on entity != NULL && entity+0xAC != NULL && descriptor[0x96] != 0 */
    if (g_companion_entity) {
        u8 *ce = (u8 *)g_companion_entity;
        void *desc = *(void **)(ce + 0xAC);
        if (desc && *((u8 *)desc + 0x96) != 0)
            render_entity_with_ui(g_companion_entity, 0);           /* render_entity_with_ui @ 0x4024AF */
    }
    if (g_secondary_companion_entity) {
        u8 *se = (u8 *)g_secondary_companion_entity;
        void *desc = *(void **)(se + 0xAC);
        if (desc && *((u8 *)desc + 0x96) != 0)
            render_entity_with_ui(g_secondary_companion_entity, 0); /* render_entity_with_ui @ 0x4024AF */
    }

    /* -----------------------------------------------------------------
     * Phase 22 — apply_model_transform for player + companions
     * ----------------------------------------------------------------- */
    print("scene: phase 22 — apply_model_transform\n");
    /* asm: EDX = entity+0x8C (transform context), ECX = entity */
    if (g_player_entity) {
        /* +0x8C is a 32-bit field in the original (consumed as a world Y by
         * initialize_effect_position @ 0x4357CF), not a host pointer — read it
         * as i32 so the adjacent +0x90 slot is not folded into the value. */
        void *xf = (void *)(intptr_t)ent_get_i32(g_player_entity, 0x8C);
        apply_model_transform(g_player_entity, xf);  /* apply_model_transform @ 0x4359FF */
    }
    if (g_companion_entity) {
        /* +0x8C is a 32-bit field in the original (consumed as a world Y by
         * initialize_effect_position @ 0x4357CF), not a host pointer — read it
         * as i32 so the adjacent +0x90 slot is not folded into the value. */
        void *xf = (void *)(intptr_t)ent_get_i32(g_companion_entity, 0x8C);
        apply_model_transform(g_companion_entity, xf);            /* apply_model_transform @ 0x4359FF */
    }
    if (g_secondary_companion_entity) {
        /* +0x8C is a 32-bit field in the original (consumed as a world Y by
         * initialize_effect_position @ 0x4357CF), not a host pointer — read it
         * as i32 so the adjacent +0x90 slot is not folded into the value. */
        void *xf = (void *)(intptr_t)ent_get_i32(g_secondary_companion_entity, 0x8C);
        apply_model_transform(g_secondary_companion_entity, xf);  /* apply_model_transform @ 0x4359FF */
    }

    /* -----------------------------------------------------------------
     * Phase 23 — system_get_gametime → last_event_check_time
     * ----------------------------------------------------------------- */
    print("scene: phase 23 — system_get_gametime\n");
    g_last_event_check_time = system_get_gametime();  /* last_event_check_time @ asm */

    /* -----------------------------------------------------------------
     * Phase 24 — restore_display_mode(1, 0x1D)
     * Asm: mov edx, 1Dh / mov ecx, 1 / call restore_display_mode
     * ----------------------------------------------------------------- */
    print("scene: phase 24 — restore_display_mode(1, 0x1D)\n");
    restore_display_mode(1, 0x1D);   /* restore_display_mode — DirectDraw no-op */

    /* -----------------------------------------------------------------
     * Phase 25 — sub_40636D, world_state → dword_505C58, zero dword_4B2AB8
     * Asm: call sub_40636D
     *      mov eax, world_state / mov dword_505C58, eax
     *      mov dword_4B2AB8, 0
     * ----------------------------------------------------------------- */
    print("scene: phase 25 — update_game_time_entities, wire dword_505C58\n");
    update_game_time_entities();   /* sub_40636D @ 0x40636D */
    g_dword_505C58 = g_world_state;  /* dword_505C58 @ 0x505C58 */
    g_dword_4B2AB8 = 0;              /* dword_4B2AB8 @ 0x4B2AB8 */

    /* -----------------------------------------------------------------
     * Parse L%02d.INF header (32 bytes decoded so far).
     * Not in the asm for this proc (owned by sub_46D0EA), but done here
     * so scene_render / scene_load_camera_backdrop can honour the real
     * camera count instead of a hard-coded 6.
     *
     * Layout inferred from bin/game/datas/L01/L01.INF:
     *   +00 u32  camera_count      ; L01 → 7
     *   +04 u32  version_or_magic  ; 0x3348F894 in L01
     *   +08 u32  flag              ; 1 in L01
     *   +0C u32  zone_tag ASCII    ; "sol\0" in L01
     *   +10..+1B  zero pad
     *   +1C u32  data_offset       ; 0x1780 in L01
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
                u8 hdr[32] = {0};
                i32 n = (i32)fread(hdr, 1, sizeof(hdr), f);
                if (n >= 16) {
                    u32 cam_count = (u32)hdr[0]
                                  | ((u32)hdr[1] << 8)
                                  | ((u32)hdr[2] << 16)
                                  | ((u32)hdr[3] << 24);
                    c8 zone[5] = {0};
                    memcpy(zone, hdr + 12, 4);
                    print("scene: L%02d.INF — %d bytes; cameras=%u zone='%s'\n",
                          level_id, n, cam_count, zone);
                    if (cam_count > 0 && cam_count <= 64) {
                        g_scene_camera_max = (i32)cam_count - 1;
                    }
                } else {
                    print("scene: L%02d.INF — read %d bytes (short header)\n",
                          level_id, n);
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

    /* Parse L<lvl>.DAT plain-text config (per-camera zbuffer + lights,
     * global ambient). Populates g_scene_cfg so model_render can later
     * do proper depth un-normalisation and light-position shading. */
    scene_load_dat_config(level_id);

    /* Inventory pass: log the level's object mesh + player mesh. Doesn't
     * decode geometry yet — just proves the .OME chunk walker is happy
     * and shows us how many atoms (body parts, objects) each mesh will
     * need once the ATOMF decoder lands. */
    {
        c8 vpath[64];
        c8 host[512];
        const c8 *base = (state && state->game_path) ? state->game_path : ".";
        ome_file_t ome = {0};

        /* Level object mesh. */
        snprintf(vpath, sizeof(vpath),
                 "DATAS>L%02d>IM%02d.OME", level_id, level_id);
        if (ls_resolve_path(base, vpath, host, (i32)sizeof(host)) &&
            model_ome_open(host, &ome)) {
            model_ome_free(&ome);
        }
        /* Player mesh — decode MATR colours then all geometry ATOMFs. */
        if (ls_resolve_path(base, "PERSOS>P00.OME", host, (i32)sizeof(host)) &&
            model_ome_open(host, &ome)) {
            /* Free any previous atoms. */
            for (i32 k = 0; k < g_scene_ome_atom_count; k++) {
                model_free(g_scene_ome_atoms[k]);
                g_scene_ome_atoms[k] = NULL;
            }
            g_scene_ome_atom_count = 0;

            /* Parse material colours from the MATR chunk. */
            ome_matr_t matr = {0};
            ome_matr_t *matr_ptr = NULL;
            for (i32 ci = 0; ci < ome.chunk_count; ci++) {
                if (ome.chunks[ci].kind != OME_CHUNK_MATR) continue;
                const u8 *mbody = ome.raw + ome.chunks[ci].offset + 0x60;
                i32 mbody_len   = ome.chunks[ci].length - 0x60;
                if (ome_parse_matr(mbody, mbody_len, &matr)) {
                    matr_ptr = &matr;
                }
                break; /* only one MATR per file */
            }

            for (i32 ci = 0; ci < ome.chunk_count; ci++) {
                if (ome.chunks[ci].kind != OME_CHUNK_ATOMF) continue;
                if (g_scene_ome_atom_count >= SCENE_OME_MAX_ATOMS) break;

                const u8 *body = ome.raw + ome.chunks[ci].offset + 0x60;
                i32 body_len   = ome.chunks[ci].length - 0x60;
                ome_atom_t atom = {0};
                if (ome_decode_atomf(body, body_len, &atom) &&
                    atom.vert_count > 0 && atom.face_count > 0) {
                    model_t *m = model_from_ome_atom(&atom, matr_ptr);
                    if (m) {
                        g_scene_ome_atoms[g_scene_ome_atom_count++] = m;
                    }
                }
                ome_atom_free(&atom);
            }
            /* Parse OBJC chunk for bone hierarchy and CAN slot mapping */
            g_scene_objc_loaded = 0;
            for (i32 ci = 0; ci < ome.chunk_count; ci++) {
                if (ome.chunks[ci].kind == OME_CHUNK_OBJC) {
                    g_scene_objc_loaded = ome_parse_objc(
                        ome.raw, ome.raw_size,
                        ome.chunks[ci].offset,
                        &g_scene_objc);
                    break;
                }
            }

            print("scene: P00 loaded %d geometry atoms (%d materials)\n",
                  g_scene_ome_atom_count, matr.mat_count);
            model_ome_free(&ome);
        }
    }
    if (g_scene_cfg.loaded && g_scene_cfg.ncamera > 0) {
        g_scene_camera_max = g_scene_cfg.ncamera - 1;
    }

    /* Post-skeleton: load camera 0 backdrop so scene_render has something
     * to blit.  Each CIF ships its own 256-colour palette (no .GRD parse
     * needed for this pass). */
    g_scene_camera = 0;
    scene_load_camera_backdrop(level_id, g_scene_camera);

    /* Read camera pitch from the INF projection matrix and apply to rendering. */
    model_default_pitch = scene_load_inf_pitch(g_scene_bg_level, g_scene_camera);

    /* Compute character screen anchor from INF projection params. */
    scene_load_camera_anchor(g_scene_bg_level, g_scene_camera);

    /* Per-camera active regions (for auto camera switching by player pos). */
    scene_load_camera_regions(g_scene_bg_level);

    /* LIEU scripting: native hand-port (src/lieu01.c for level 1). Runs
     * INIT now; per-frame tick driven from scene_render; deinit in release. */
    lieu_enter(level_id);

    /* Load P00.CAN for player character animation. */
    {
        c8 vpath[64];
        c8 host[512];
        g_scene_can_loaded = 0;
        g_scene_can_frame  = 0;
        const c8 *base = (state && state->game_path) ? state->game_path : ".";
        snprintf(vpath, sizeof(vpath), "ANM>P00.CAN");
        if (ls_resolve_path(base, vpath, host, (i32)sizeof(host))) {
            g_scene_can_loaded = can_load_first(host, &g_scene_can);
        }
    }

    /* Load ANI sprite for entity animation 1 at current level/camera 0.
     * Path: DATAS>L{level:02d}>ANIM2D>N{level:02d}C{cam}A1.ANI */
    {
        c8 vpath[64];
        c8 host[512];
        ani_free(&g_scene_ani);
        g_scene_ani_loaded = 0;
        g_scene_ani_frame  = 0;
        const c8 *base = (state && state->game_path) ? state->game_path : ".";
        snprintf(vpath, sizeof(vpath),
                 "DATAS>L%02d>ANIM2D>N%02dC%dA1.ANI",
                 level_id, level_id, g_scene_camera);
        if (ls_resolve_path(base, vpath, host, (i32)sizeof(host))) {
            if (ani_load(host, &g_scene_ani)) {
                g_scene_ani_loaded = 1;
            }
        }
        if (!g_scene_ani_loaded) {
            print("scene: no ANI sprite for level %d cam %d\n",
                  level_id, g_scene_camera);
        }
    }

    print("scene: scene_load_level(%d) — complete\n", level_id);
    return 0;
}

/* -----------------------------------------------------------------------
 * 3×3 rotation matrix in Q14 (16384 = 1.0), flat row-major [9] array.
 * Used for per-frame bone hierarchy propagation in scene_update_bone_positions.
 * ----------------------------------------------------------------------- */
typedef struct { i32 m[9]; } scene_mat3_t;

static scene_mat3_t scene_mat3_identity(void) __attribute__((unused));
static scene_mat3_t scene_mat3_identity(void) {
    scene_mat3_t r;
    i32 i;
    for (i = 0; i < 9; i++) r.m[i] = 0;
    r.m[0] = 16384; r.m[4] = 16384; r.m[8] = 16384;
    return r;
}

/* Build Ry×Rx rotation matrix from cosine_table indices. */
static scene_mat3_t scene_mat3_from_rx_ry(i32 rx_idx, i32 ry_idx) {
    i32 cx = cosine_table[rx_idx & 0xFFF];
    i32 sx = cosine_table[(rx_idx + 1024) & 0xFFF];
    i32 cy = cosine_table[ry_idx & 0xFFF];
    i32 sy = cosine_table[(ry_idx + 1024) & 0xFFF];
    scene_mat3_t r;
    r.m[0] = cy;
    r.m[1] = (i32)(((i64)sy * sx) >> 14);
    r.m[2] = (i32)(((i64)sy * cx) >> 14);
    r.m[3] = 0;
    r.m[4] = cx;
    r.m[5] = -sx;
    r.m[6] = -sy;
    r.m[7] = (i32)(((i64)cy * sx) >> 14);
    r.m[8] = (i32)(((i64)cy * cx) >> 14);
    return r;
}

/* Multiply two 3×3 Q14 matrices: result = a × b. */
static scene_mat3_t scene_mat3_mul(const scene_mat3_t *a, const scene_mat3_t *b) {
    scene_mat3_t r;
    i32 row, col, k;
    for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
            i64 s = 0;
            for (k = 0; k < 3; k++)
                s += (i64)a->m[row*3+k] * b->m[k*3+col];
            r.m[row*3+col] = (i32)(s >> 14);
        }
    }
    return r;
}

/* Apply Q14 rotation matrix r to translation vector (tx,ty,tz) → (ox,oy,oz). */
static void scene_mat3_apply(const scene_mat3_t *r, i32 tx, i32 ty, i32 tz,
                              i32 *ox, i32 *oy, i32 *oz) {
    *ox = (i32)(((i64)r->m[0]*tx + (i64)r->m[1]*ty + (i64)r->m[2]*tz) >> 14);
    *oy = (i32)(((i64)r->m[3]*tx + (i64)r->m[4]*ty + (i64)r->m[5]*tz) >> 14);
    *oz = (i32)(((i64)r->m[6]*tx + (i64)r->m[7]*ty + (i64)r->m[8]*tz) >> 14);
}

/*
 * Compute per-bone world positions for the current CAN animation frame.
 * Processes bones in index order 0..n_bones-1, which is topologically sorted
 * (parents always have lower indices than children) for P00.OME.
 * Updates g_scene_objc.world_tx/ty/tz[render_atom] in place.
 * kf may be NULL (T-pose: all rotations = identity).
 */
/* Per-skeleton bone FK — writes objc->world_tx/ty/tz[atom] from keyframe kf.
 * Parameterized so each persona drives its own OBJC skeleton. */
static void objc_update_bone_positions(ome_objc_t *objc, const can_frame_t *kf) {
    i32 n = objc->n_bones;
    if (n <= 0 || n > OME_MAX_BONES) return;

    i32          bone_wx[OME_MAX_BONES];
    i32          bone_wy[OME_MAX_BONES];
    i32          bone_wz[OME_MAX_BONES];
    scene_mat3_t bone_rot[OME_MAX_BONES];

    for (i32 j = 0; j < n; j++) {
        const ome_bone_t *b = &objc->bones[j];
        i32 rx_idx = 0, ry_idx = 0;
        i32 ch = objc->can_slot[j];      /* keyframe channel for this bone */
        if (kf && ch >= 0 && ch < CAN_MAX_ATOMSLOTS) {
            rx_idx = (i32)((u32)kf->atoms[ch].rx >> 4) & 0xFFF;
            ry_idx = (i32)((u32)kf->atoms[ch].ry >> 4) & 0xFFF;
        }
        scene_mat3_t local_rot = scene_mat3_from_rx_ry(rx_idx, ry_idx);

        i32 parent = b->h2;
        if (parent < 0 || parent >= n) {
            bone_wx[j] = 0; bone_wy[j] = 0; bone_wz[j] = 0;
            bone_rot[j] = local_rot;
        } else {
            i32 rx, ry, rz;
            scene_mat3_apply(&bone_rot[parent], b->tx, b->ty, b->tz, &rx, &ry, &rz);
            bone_wx[j] = bone_wx[parent] + rx;
            bone_wy[j] = bone_wy[parent] + ry;
            bone_wz[j] = bone_wz[parent] + rz;
            bone_rot[j] = scene_mat3_mul(&bone_rot[parent], &local_rot);
        }

        i32 ai = objc->can_to_render[j];
        if (ai >= 0 && ai < objc->n_atomslots) {
            objc->world_tx[ai] = bone_wx[j];
            objc->world_ty[ai] = bone_wy[j];
            objc->world_tz[ai] = bone_wz[j];
            for (i32 k = 0; k < 9; k++) objc->world_rot[ai][k] = bone_rot[j].m[k];
        }
    }
}

static void scene_update_bone_positions(const can_frame_t *kf) {
    if (!g_scene_objc_loaded) return;
    objc_update_bone_positions(&g_scene_objc, kf);
}

/*
 * scene_render — blit the current camera's backdrop and poll input.
 * Called from the main loop while state == MENU_STATE_LOADING.
 * Falls back to a "SCENE (no bmap)" text banner if the backdrop failed
 * to load, so the loop stays alive and the user can quit cleanly.
 */
i32 scene_render(system_t *sys) {
    if (!state) return 0;

    /* Per-frame LIEU scripting (native hand-port; level 1 only for now),
     * mirroring the engine's per-frame driver (dkev sub_44968B):
     *   op -1 tick every frame, then op 3 for the current zone (if any). */
    lieu_tick(sys ? sys->timer.tick : 0);
    {
        static i32 s_last_zone = -2;
        i32 zone = lieu_detect_zone();
        if (zone != s_last_zone) {
            print("scene: player -> zone %d (wX=%d wZ=%d)\n",
                  zone, g_player_wx, g_player_wz);
            s_last_zone = zone;
        }
        if (zone >= 0) lieu_zone_trigger(zone);
    }

    /* Room change: the LIEU script fired CHANGEPIECE (slot 10) — walk to the
     * target room. Teardown the current level, load the target; the new room's
     * INIT (trig00 POSPERSO) places the player at the entry door. */
    {
        i32 entry = 0;
        i32 next  = lieu_poll_level_change(&entry);
        if (next < 0 && getenv("DKE_GOTO")) {         /* test hook: force a room change */
            static i32 fired = 0;
            static i32 frames = 0;
            if (!fired && ++frames > 60) { next = atoi(getenv("DKE_GOTO")); fired = 1; }
        }
        if (next >= 0 && next != g_scene_bg_level) {
            print("scene: CHANGEPIECE -> load level %d (entry %d)\n", next, entry);
            lieu_exit();
            g_player_seeded = 0;
            /* Release the old room's world data (the engine's teardown
             * sub_46E4A8 @ 0x46E4A8 does this before loading the target).
             * scene_load_level's phase 8 mirrors the asm's
             * "cmp world_controller, 0 / jnz skip", so without clearing it
             * the new room silently keeps the previous room's boundary
             * polygon, collision zones and camera regions — L02 was building
             * a nav grid byte-identical to L01's. */
            world_free();
            g_world_controller = NULL;
            scene_load_level(next);
            return 0;   /* new level loaded; resume next frame */
        }
    }

    /* Script-driven camera (ChangeCamera, slot 9): the LIEU script sets the
     * in-game camera (e.g. the opening bedroom view cb_458(6,1)). Apply it:
     * reload backdrop + projection + pitch. Mode 2 locks out auto-switching. */
    {
        i32 cmode = 1;
        i32 wantcam = lieu_poll_camera_change(&cmode);
        if (wantcam >= 0 && wantcam != g_scene_camera) {
            print("scene: ChangeCamera -> camera %d (mode %d)\n", wantcam, cmode);
            if (scene_load_camera_backdrop(g_scene_bg_level, wantcam)) {
                scene_load_camera_anchor(g_scene_bg_level, g_scene_camera);
                model_default_pitch = scene_load_inf_pitch(g_scene_bg_level, g_scene_camera);
            }
        }
        if (wantcam >= 0) g_camera_forced = (cmode == 2);
    }

    /* Advance dialogue (subtitle + spoken WAV) queued by the LIEU script.
     * Each line HOLDS until the player advances (mouse click edge). Polled
     * here before the movement block so we get the want_action edge first. */
    if (g_scene_bg_level >= 0) {
        const c8 *root = (state && state->game_path) ? state->game_path : ".";
        const c8 *lang = dlg_lang_code(state ? state->language_id : 0);
        u32 tick = sys ? sys->timer.tick : 0;
        i32 advance = 0;
        if (sys) {
            player_input_t din = {0};
            player_input_update(&din, sys);
            advance = din.want_action;               /* mouse-left click edge */
        }
        if (getenv("DKE_AUTOADV")) {                  /* headless: auto-pace */
            static u32 s_next = 0;
            if (tick >= s_next) { advance = 1; s_next = tick + 1200; }
        }
        dlg_tick(g_scene_bg_level, root, lang, tick, advance);
    }
    /* Play any cutscene the LIEU script queued (intro FMV, etc). Safe here
     * in the render phase; ls_play_movie runs its own blocking decode loop. */
    {
        const c8 *mv = lieu_poll_movie();
        if (mv) {
            if (getenv("DKE_NOMOVIE")) print("scene: DKE_NOMOVIE skip movie %s\n", mv);
            else                       ls_play_movie(mv);
            lieu_enter_room();   /* intro done -> place room characters */
        }
    }

    /* --- Input: manual camera cycle ([ / ]) + Esc-to-menu -----------
     * Arrow keys now WALK the player (see player_input_update); the camera
     * follows automatically by region (scene_camera_for_position). Manual
     * cycling stays available on [ (actions[91]) and ] (actions[93]) as a
     * debug override. */
    if (sys->actions[91] == 1 || sys->actions[93] == 1) {
        i32 forward = (sys->actions[93] == 1);
        sys->actions[91] = 0; sys->actions[93] = 0;
        i32 next = forward ? g_scene_camera + 1 : g_scene_camera - 1;
        if (next > g_scene_camera_max) next = 0;
        if (next < 0) next = g_scene_camera_max;
        print("scene: manual cam %d -> %d\n", g_scene_camera, next);
        if (!scene_load_camera_backdrop(g_scene_bg_level, next)) {
            g_scene_camera_max = g_scene_camera;
            scene_load_camera_backdrop(g_scene_bg_level, 0);
        }
        scene_load_camera_anchor(g_scene_bg_level, g_scene_camera);
        model_default_pitch = scene_load_inf_pitch(g_scene_bg_level, g_scene_camera);
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

    /* Step 1 — plain F blit as backdrop. */
    if (g_scene_bg_bmap && g_scene_bg_bmap->pixels && g_scene_bg_bmap->palette) {
        i32 bw = (i32)g_scene_bg_bmap->width;
        i32 bh = g_scene_bg_bmap->height;
        i32 ox = (fb_w > bw) ? (fb_w - bw) / 2 : 0;
        i32 oy = (fb_h > bh) ? (fb_h - bh) / 2 : 0;
        blit_bmap_to_framebuffer(fb, fb_w, fb_h,
                                 g_scene_bg_bmap, g_scene_bg_bmap->palette,
                                 ox, oy);
    }

    /* Step 1b — ANI sprite overlay.
     * Blit the current animation frame at its screen position.
     * Each pixel is a palette index; use the backdrop's palette (same level). */
    if (g_scene_ani_loaded && g_scene_ani.n_frames > 0 &&
        g_scene_bg_bmap && g_scene_bg_bmap->palette) {
        u32 tick = sys ? sys->timer.tick : 0;
        if (tick - g_scene_ani_last_tick >= ANI_FRAME_MS) {
            g_scene_ani_frame = (g_scene_ani_frame + 1) % g_scene_ani.n_frames;
            g_scene_ani_last_tick = tick;
        }
        const ani_frame_t *fr = &g_scene_ani.frames[g_scene_ani_frame];
        if (fr->pixels) {
            /* Build a temporary bmap_t pointing at the decompressed pixels. */
            i32 w = (i32)(fr->x2 - fr->x1 + 1);
            i32 h = (i32)(fr->y2 - fr->y1 + 1);
            bmap_t tmp = {0};
            tmp.width      = (i16)w;
            tmp.height     = h;
            tmp.stride     = w;
            tmp.bit_depth  = 8;
            tmp.pixels     = fr->pixels;
            tmp.first_pixel = fr->pixels;
            tmp.owns_buffer = 0;
            /* Blit at (x1,y1) with palette index 0 transparent. */
            blit_bmap_masked_to_framebuffer(fb, fb_w, fb_h,
                                            &tmp, g_scene_bg_bmap->palette,
                                            (i32)fr->x1, (i32)fr->y1, 0);
        }
    }

    /* Step 2 — player-controlled placeholder cube.
     *
     * Movement: poll WASD via player_input_update, integrate against dt
     * derived from sys->timer.tick, apply into g_cube_world_x/z. Speed
     * chosen so a full traverse across the room (~zfar) takes ~5 s.
     *
     * Rendering: model_render uses the same world_z + world_size + z_far
     * triple as before; scene Z bmap now depth-tests per pixel, so the
     * cube slides behind foreground scene geometry when WASD walks it
     * toward the room's far walls. */
    i32 z_far = scene_get_camera_zfar(g_scene_camera);

    /* Seed the player at the room's character world position once the INF
     * projection is available. Then WASD moves it in LIEU world units so
     * lieu_detect_zone (world-space rect test) fires when walking a zone. */
    if (!g_player_seeded && g_proj_valid) {
        /* Prefer the script's own placement for the player's persona: P00 is
         * Arkhan, and trig00's PosPerso is where the room actually puts him.
         * The camera-zone anchor below is only a fallback for rooms that
         * place nobody — it is solved from the zone centroid and lands tens
         * of thousands of units off, which throws every distance and facing
         * test the script runs against the player. */
        i32 seeded_from_script = 0;
        /*
         * UNRESOLVED — two sources disagree about where the player starts.
         *
         *  - trig00's PosPerso for P00 (Arkhan) puts him at (378000,-1837200),
         *    which is the placement the engine uses on a room entry and is
         *    almost certainly right. It sits outside EVERY LIEU zone, so no
         *    trigger fires and the opening conversation never starts.
         *  - The camera-zone anchor solve lands at (305083,-1802945), inside
         *    zone 4, whose trigger is what has actually been driving that
         *    conversation.
         *
         * They differ by ~34000 units on Z, so one of the two mappings (the
         * PosPerso tile scale or the anchor solve) is off. Until that is
         * settled the anchor stays the default because it is the one that
         * produces a playable opening; DKE_SCRIPTSEED=1 selects the scripted
         * placement. See docs/functions/nav_collision.md.
         */
        for (i32 pi = 0; getenv("DKE_SCRIPTSEED") && pi < lieu_spawn_count(); pi++) {
            i32 stx = 0, stz = 0, sview = 0;
            const c8 *snm = lieu_spawn_get(pi, &stx, &stz, &sview);
            if (!snm || persona_num_from_name(snm) != 0) continue;
            g_player_wx = stx * 600;
            g_player_wz = stz * 600;
            g_player_facing = (lieu_spawn_facing(pi) >> 4) & 0xFFF;
            seeded_from_script = 1;
            print("scene: player seeded from %s at (%d,%d) facing=0x%X\n",
                  snm, g_player_wx, g_player_wz, g_player_facing);
            break;
        }
        if (!seeded_from_script) {
            g_player_wx = g_proj_anchor_wx;
            g_player_wz = g_proj_zone_z;
        }
        /* Only the guessed anchor gets snapped (sub_456FB7 @ 0x456FB7). A
         * scripted placement is authoritative even when it sits in a blocked
         * cell — L01 starts Arkhan lying on the bed, which the grid marks as
         * furniture, and snapping would slide him off it. */
        if (!seeded_from_script &&
            nav_snap_to_walkable(&g_player_wx, &g_player_wz))
            print("scene: player seed snapped to walkable (%d,%d)\n",
                  g_player_wx, g_player_wz);
        g_player_seeded = 1;
    }

    if (sys) {
        u32 tick = sys->timer.tick;
        i32 dt_ms = g_scene_prev_tick > 0 ? (i32)(tick - g_scene_prev_tick) : 16;
        if (dt_ms < 0)     dt_ms = 0;
        if (dt_ms > 100)   dt_ms = 100;   /* clamp on long pauses */
        g_scene_prev_tick = tick;

        player_input_t pin = {0};
        if (!lieu_player_blocked())
            player_input_update(&pin, sys);

        /* ~50k world units/s (LIEU zones are ~50-90k wide). */
        i32 step = (50000 * dt_ms) / 1000;
        i32 want_x = g_player_wx + (i32)(pin.velocity_x * step);
        i32 want_z = g_player_wz + (i32)(pin.velocity_z * step);

        /* Debug headless sweeps (fixed step, dt-independent). They go through
         * the same collision path as real input so they exercise it. */
        g_player_moving = 0;
        if (getenv("DKE_WALK"))  { want_x -= 6000; g_player_moving = 1; }
        if (getenv("DKE_WALKZ")) { want_z += 6000; g_player_moving = 1; }

        /* Collide against the nav grid (sub_45619D's interaction_data_array).
         * Each axis is committed on its own so a diagonal into a wall slides
         * along it instead of stopping dead. The engine never does this — its
         * actors are walked by the pathfinder (sub_469222 @ 0x469222), which
         * only ever asks for waypoints that are already free — but dke drives
         * the player directly, so the check belongs on the input path. */
        i32 blocked = 0;
        if (want_x != g_player_wx) {
            if (nav_is_walkable(want_x, g_player_wz)) g_player_wx = want_x;
            else                                      blocked |= 1;
        }
        if (want_z != g_player_wz) {
            if (nav_is_walkable(g_player_wx, want_z)) g_player_wz = want_z;
            else                                      blocked |= 2;
        }
        if (blocked) {
            static u32 last_blocked_log = 0;
            if (tick - last_blocked_log > 500) {
                last_blocked_log = tick;
                print("scene: movement blocked (axes 0x%X) at (%d,%d) cell=%d\n",
                      blocked, g_player_wx, g_player_wz,
                      nav_cell(want_x, want_z));
            }
        }

        /* Face the movement direction (cosine_table units). */
        g_player_moving = g_player_moving ||
                          (pin.velocity_x != 0.0f || pin.velocity_z != 0.0f);
        if (pin.velocity_x != 0.0f || pin.velocity_z != 0.0f) {
            double ang = atan2((double)pin.velocity_x, (double)pin.velocity_z);
            i32 f = (i32)(ang * 4096.0 / (2.0 * 3.14159265358979));
            g_player_facing = f & 0xFFF;
        }

        /* Loose clamp around the character start. */
        i32 lim = 400000;
        if (g_player_seeded) {
            if (g_player_wx < g_proj_anchor_wx - lim) g_player_wx = g_proj_anchor_wx - lim;
            if (g_player_wx > g_proj_anchor_wx + lim) g_player_wx = g_proj_anchor_wx + lim;
            if (g_player_wz < g_proj_zone_z - lim)    g_player_wz = g_proj_zone_z - lim;
            if (g_player_wz > g_proj_zone_z + lim)    g_player_wz = g_proj_zone_z + lim;
        }
    }

    /* Feed the player world (X,Z) into the transform so lieu_detect_zone
     * (next frame) tests it against the LIEU zone rects. */
    if (g_player_entity) {
        *(i32 *)((u8 *)g_player_entity + 0x1C0) = g_player_wx;
        *(i32 *)((u8 *)g_player_entity + 0x1C8) = g_player_wz;
        /* Heading too (gravit+0x5C = entity+0x1E0, 16-bit BAM). Without this
         * player_get_facing() reads a slot nothing ever wrote, and every
         * DirectionPerso / DirectionZone test compares against heading 0.
         * g_player_facing is in cosine-table units (4096 per turn); BAM is
         * 65536 per turn, hence the <<4. */
        *(i32 *)((u8 *)g_player_entity + ENT_OFF_FACING) =
            (g_player_facing << 4) & 0xFFFF;
    }

    /* Auto camera switch: pick the camera whose region contains the player
     * (dkev auto mode). Skipped while the script locked the camera (mode 2). */
    if (g_player_seeded && g_cam_region_count > 0 && !g_camera_forced &&
        !getenv("DKE_NOAUTOCAM")) {
        i32 wantcam = scene_camera_for_position(g_player_wx, g_player_wz, g_scene_camera);
        if (wantcam >= 0 && wantcam != g_scene_camera) {
            print("scene: auto camera %d -> %d (player wX=%d wZ=%d)\n",
                  g_scene_camera, wantcam, g_player_wx, g_player_wz);
            scene_load_camera_backdrop(g_scene_bg_level, wantcam);
            scene_load_camera_anchor(g_scene_bg_level, g_scene_camera);
            model_default_pitch = scene_load_inf_pitch(g_scene_bg_level, g_scene_camera);
        }
    }

    /* Screen position from the INF projection. LIEU world Z is far outside
     * the z-buffer range, so use a fixed mid-room render depth for the mesh. */
    i32 focal = fb_w;
    i32 screen_x, screen_y;
    scene_project_world(g_player_wx, g_proj_ground_wy, g_player_wz, &screen_x, &screen_y);
    i32 render_depth = z_far / 2;

    i32 cube_world_size = 60;
    u32 tick = sys ? sys->timer.tick : 0;
    if (g_scene_ome_atom_count > 0) {
        /* Render all geometry atoms at the same world position — they share
         * a common coordinate system so layering at the same pivot gives a
         * complete character in its default (T-pose) orientation. */
        /* Advance CAN animation frame. */
        if (g_scene_can_loaded && tick - g_scene_can_last >= CAN_FRAME_MS) {
            g_scene_can_frame = (g_scene_can_frame + 1) % g_scene_can.n_frames;
            g_scene_can_last  = tick;
        }

        /* Update bone world positions for current animation frame */
        {
            const can_frame_t *kf = g_scene_can_loaded
                ? &g_scene_can.frames[g_scene_can_frame] : NULL;
            scene_update_bone_positions(kf);
        }

        /* Build per-render-atom rotation table from CAN keyframe + OBJC slot mapping */
        i32 atom_rx[20] = {0}, atom_ry[20] = {0};
        if (g_scene_can_loaded && g_scene_objc_loaded) {
            const can_frame_t *kf = &g_scene_can.frames[g_scene_can_frame];
            for (i32 j = 0; j < g_scene_objc.n_bones; j++) {
                i32 ri = g_scene_objc.can_to_render[j];
                i32 ch = g_scene_objc.can_slot[j];
                if (ri < 0 || ri >= g_scene_ome_atom_count) continue;
                if (ch < 0 || ch >= CAN_MAX_ATOMSLOTS) continue;
                atom_rx[ri] = (i32)(kf->atoms[ch].rx >> 4) & 0xFFF;
                atom_ry[ri] = (i32)(kf->atoms[ch].ry >> 4) & 0xFFF;
            }
        }
        /* Legacy placeholder: the scene's default OME mesh drawn at the player
         * anchor via the old toy camera. The LIEU personas (below) now render
         * the real cast (Arkhan/Kaly) through the world camera, so this is
         * off by default — gate on DKE_SHOWPLAYER for debugging. */
        if (getenv("DKE_SHOWPLAYER")) {
            for (i32 ai = 0; ai < g_scene_ome_atom_count; ai++) {
                i32 tx = (g_scene_objc_loaded && ai < g_scene_objc.n_atomslots)
                           ? g_scene_objc.world_tx[ai] * ATOM_SCALE : 0;
                i32 ty = (g_scene_objc_loaded && ai < g_scene_objc.n_atomslots)
                           ? g_scene_objc.world_ty[ai] * ATOM_SCALE : 0;
                i32 tz = (g_scene_objc_loaded && ai < g_scene_objc.n_atomslots)
                           ? g_scene_objc.world_tz[ai] * ATOM_SCALE : 0;
                model_render(g_scene_ome_atoms[ai], sys, screen_x, screen_y,
                             focal, tick, render_depth, cube_world_size, z_far,
                             g_scene_l_bmap, g_scene_z_bmap,
                             atom_rx[ai] ? 1 : 0, atom_ry[ai], atom_rx[ai],
                             tx, ty, tz);
            }
        }

        /* Render each LIEU-placed persona (P00ARR01, P12ARR01, ...) as the
         * loaded character mesh, at a screen position near the INF anchor.
         * Placeholder: all personas share the player mesh until per-persona
         * models load; positions are clustered around the character stand. */
        clock_t s_pers = getenv("DKE_PERFDBG") ? clock() : 0;
        /* Scene key light (L%02d.DAT lightN) rotated into camera space, so
         * character shading is lit from the room's real light direction. */
        float lcx = 0, lcy = 0, lcz = 0;
        if (g_scene_cfg.loaded && g_scene_camera < SCENE_MAX_CAMERAS) {
            i32 lwx = g_scene_cfg.lightx[g_scene_camera];
            i32 lwy = g_scene_cfg.lighty[g_scene_camera];
            i32 lwz = g_scene_cfg.lightz[g_scene_camera];
            lcx = (float)((i64)lwx*g_proj_R[0] + (i64)lwy*g_proj_R[3] + (i64)lwz*g_proj_R[6]);
            lcy = (float)((i64)lwx*g_proj_R[1] + (i64)lwy*g_proj_R[4] + (i64)lwz*g_proj_R[7]);
            lcz = (float)((i64)lwx*g_proj_R[2] + (i64)lwy*g_proj_R[5] + (i64)lwz*g_proj_R[8]);
        }
        for (i32 pi = 0; pi < lieu_spawn_count(); pi++) {
            if (getenv("DKE_ONLYP") && pi != atoi(getenv("DKE_ONLYP"))) continue;
            i32 wxt = 0, wzt = 0, pview = 0;
            const c8 *nm = lieu_spawn_get(pi, &wxt, &wzt, &pview);
            if (!nm) continue;
            /* Arkhan is both a scripted placement and the player avatar.
             * While a BLOQUE bracket is open the script owns him, so his
             * PosPerso placement is what renders; once control returns to the
             * player the avatar below takes over and this placement is
             * skipped, otherwise he is drawn twice. */
            if (!lieu_player_blocked() && persona_num_from_name(nm) == 0)
                continue;
            /* Project the persona's world position (tile x600, its own
             * PosPerso ground Y) to screen. */
            i32 pground = lieu_spawn_ground(pi);
            i32 psx, psy;
            i32 pvis = scene_project_world(wxt * 600, pground, wzt * 600, &psx, &psy);
            if (getenv("DKE_PROJDBG")) {
                static i32 s_seen[16] = {0};
                if (pi < 16 && !s_seen[pi]) {
                    s_seen[pi] = 1;
                    print("proj: %s world=(%d,%d,%d) -> screen=(%d,%d) vis=%d [fb %dx%d]\n",
                          nm, wxt * 600, pground, wzt * 600, psx, psy, pvis,
                          state ? state->graphic.width : 0,
                          state ? state->graphic.height : 0);
                }
            }
            /* Load this persona's own mesh + skeleton + ARR01 clip. */
            i32 ci = persona_mesh_load(persona_num_from_name(nm));
            if (ci < 0) {
                /* fallback: player mesh, static pose */
                for (i32 ai = 0; ai < g_scene_ome_atom_count; ai++)
                    model_render(g_scene_ome_atoms[ai], sys, psx, psy, focal, tick,
                                 render_depth, cube_world_size, z_far,
                                 g_scene_l_bmap, g_scene_z_bmap, 0, 0, 0, 0, 0, 0);
                continue;
            }
            persona_mesh_t *pm = &g_persona_cache[ci];
            persona_apply_script_clip(pm);   /* LIEU ANIMATION action */
            /* Advance the persona's animation frame. */
            if (pm->can_loaded && pm->can.n_frames > 0 &&
                tick - pm->can_last >= CAN_FRAME_MS) {
                pm->frame = (pm->frame + 1) % pm->can.n_frames;
                pm->can_last = tick;
            }
            const can_frame_t *kf = pm->can_loaded ? &pm->can.frames[pm->frame] : NULL;
            if (pm->objc_loaded) objc_update_bone_positions(&pm->objc, kf);
            /* Per-atom rotation table from the keyframe (via OBJC slot map). */
            i32 prx[SCENE_OME_MAX_ATOMS] = {0}, pry[SCENE_OME_MAX_ATOMS] = {0};
            if (kf && pm->objc_loaded) {
                for (i32 jj = 0; jj < pm->objc.n_bones; jj++) {
                    i32 ri = pm->objc.can_to_render[jj];
                    i32 ch = pm->objc.can_slot[jj];
                    if (ri < 0 || ri >= pm->count) continue;
                    if (ch < 0 || ch >= CAN_MAX_ATOMSLOTS) continue;
                    prx[ri] = (i32)((u32)kf->atoms[ch].rx >> 4) & 0xFFF;
                    pry[ri] = (i32)((u32)kf->atoms[ch].ry >> 4) & 0xFFF;
                }
            }
            /* Rasterize each atom through the real in-game camera (world
             * pipeline). Bone world_tx/ty/tz already reflect the animated
             * pose (objc_update_bone_positions from the CAN keyframe). */
            /* Empirical calibration for L01 cam0: MSHIFT=1 (OME<<1>>1 = OME
             * units) sizes the figures to the backdrop; PosPerso ground is 0
             * but the floor sits ~120000 world units below the camera plane,
             * so drop the figures onto the beds. TODO: derive the floor Y from
             * the room terrain instead of this per-camera constant. */
            i32 mshift = getenv("DKE_MSHIFT") ? atoi(getenv("DKE_MSHIFT")) : 0;
            i32 mflip  = getenv("DKE_MFLIPY") ? atoi(getenv("DKE_MFLIPY")) : 1;
            /* The drop is a WORLD-space offset applied to the character's
             * position, not a model-space one, so it takes the bone value as
             * it stands — scaling it by mshift the way the vertices are
             * scaled pushes the figure straight through the floor. */
            i32 myoff  = getenv("DKE_MYOFF")  ? atoi(getenv("DKE_MYOFF"))
                                             : pm->foot_off;
            i32 pfac   = lieu_spawn_facing(pi) & 0xFFF;
            i32 objy   = pground + myoff;
            (void)psx; (void)psy; (void)prx; (void)pry;
            for (i32 ai = 0; ai < pm->count; ai++) {
                if (!pm->atoms[ai]) continue;
                i32 have = pm->objc_loaded && ai < pm->objc.n_atomslots;
                i32 tx = have ? pm->objc.world_tx[ai] : 0;
                i32 ty = have ? pm->objc.world_ty[ai] : 0;
                i32 tz = have ? pm->objc.world_tz[ai] : 0;
                const i32 *brot = have ? pm->objc.world_rot[ai] : NULL;
                model_render_world(pm->atoms[ai],
                                   g_proj_R, g_proj_T, g_proj_gscale,
                                   wxt * 600, objy, wzt * 600, pfac,
                                   mshift, mflip, tx, ty, tz, brot,
                                   g_scene_l_bmap, NULL,
                                   g_scene_bg_bmap ? g_scene_bg_bmap->palette : NULL,
                                   (g_scene_grd_loaded && !getenv("DKE_NOGRD")) ? g_scene_grd : NULL,
                                   lcx, lcy, lcz);
            }
        }
        /* Player avatar: render the controlled character (P00 = Arkhan) at the
         * player world position once the player has walked away from the
         * scripted start, so the static opening cutscene shows only the
         * scripted personas but the room becomes playable with a visible,
         * movement-facing avatar. */
        {
            /* Under player control whenever the script is not holding a
             * BLOQUE bracket open (group(3)/group(0), sub_446227 @ 0x446227).
             * This replaces the old "has walked >3000 units" heuristic, which
             * left the avatar invisible until the player happened to move. */
            i32 controlled = !lieu_player_blocked();
            if ((controlled || getenv("DKE_PLAYER")) && g_proj_valid) {
                i32 pmshift = getenv("DKE_MSHIFT") ? atoi(getenv("DKE_MSHIFT")) : 0;
                i32 pmflip  = getenv("DKE_MFLIPY") ? atoi(getenv("DKE_MFLIPY")) : 1;
                i32 ci = persona_mesh_load(0);   /* P00 = Arkhan */
                if (ci >= 0) {
                    persona_mesh_t *pm = &g_persona_cache[ci];
                    i32 pfloor = getenv("DKE_MYOFF") ? atoi(getenv("DKE_MYOFF"))
                                                     : pm->foot_off;
                    persona_apply_script_clip(pm);   /* LIEU ANIMATION action */
                    if (pm->can_loaded && pm->can.n_frames > 0 &&
                        tick - pm->can_last >= CAN_FRAME_MS) {
                        pm->frame = (pm->frame + 1) % pm->can.n_frames;
                        pm->can_last = tick;
                    }
                    const can_frame_t *kf = pm->can_loaded ? &pm->can.frames[pm->frame] : NULL;
                    if (pm->objc_loaded) objc_update_bone_positions(&pm->objc, kf);
                    for (i32 ai = 0; ai < pm->count; ai++) {
                        if (!pm->atoms[ai]) continue;
                        i32 have = pm->objc_loaded && ai < pm->objc.n_atomslots;
                        i32 tx = have ? pm->objc.world_tx[ai] : 0;
                        i32 ty = have ? pm->objc.world_ty[ai] : 0;
                        i32 tz = have ? pm->objc.world_tz[ai] : 0;
                        const i32 *brot = have ? pm->objc.world_rot[ai] : NULL;
                        model_render_world(pm->atoms[ai],
                                           g_proj_R, g_proj_T, g_proj_gscale,
                                           g_player_wx, pfloor, g_player_wz, g_player_facing,
                                           pmshift, pmflip, tx, ty, tz, brot,
                                           g_scene_l_bmap, NULL,
                                           g_scene_bg_bmap ? g_scene_bg_bmap->palette : NULL,
                                           (g_scene_grd_loaded && !getenv("DKE_NOGRD")) ? g_scene_grd : NULL,
                                           lcx, lcy, lcz);
                    }
                }
            }
        }
        if (getenv("DKE_PERFDBG")) {
            static i32 pc = 0; static double acc = 0;
            acc += (double)(clock() - s_pers) * 1000.0 / CLOCKS_PER_SEC;
            if (++pc >= 30) { print("perf: persona render avg %.2f ms/frame\n", acc/pc); pc=0; acc=0; }
        }
    } else {
        model_render(model_test_cube(),
                     sys,
                     screen_x, screen_y,
                     focal,
                     tick,
                     render_depth,
                     cube_world_size,
                     z_far,
                     g_scene_l_bmap,
                     g_scene_z_bmap,
                     0, 0, 0,
                     0, 0, 0);
    }

    /* HUD — current level, camera, and cube world position so testers
     * can see WASD input landing without staring at the log. */
    if (state && state->debug_enabled) {
        c8 hud[128];
        snprintf(hud, sizeof(hud),
                 "L%02d cam=%d  wX=%d wZ=%d  zone=%d",
                 g_scene_bg_level, g_scene_camera,
                 g_player_wx, g_player_wz, lieu_detect_zone());
        draw_text_at(font_handle, 6, 6, hud, 0xFFFFFFFFu);
    }

    /* LIEU personas placed for this room (e.g. P00ARR01 + P12ARR01, the two
     * characters that open level 1). Until real character models render,
     * draw each as a name label so the LIEU-driven cast is visible. */
    {
        i32 np = lieu_spawn_count();
        for (i32 i = 0; i < np; i++) {
            i32 wxt = 0, wzt = 0, pv = 0;
            const c8 *nm = lieu_spawn_get(i, &wxt, &wzt, &pv);
            if (!nm) continue;
            i32 sx, sy;
            scene_project_world(wxt * 600, lieu_spawn_ground(i), wzt * 600, &sx, &sy);
            if (sx < 4)   sx = 4;
            if (sx > 560) sx = 560;
            draw_text_at(font_handle, sx, sy - 90, nm, 0xFFFFE060u);  /* name over head */
        }
    }

    /* Dialogue subtitle (current spoken line): full-width black caption bar
     * across the bottom with word-wrapped white text, mirroring the engine's
     * update_display_state (dkev sub_416AC0): wrap, black fill (palette idx
     * 0), then draw lines inset a few px. Held until the player advances. */
    {
        const c8 *sub = dlg_active_text();
        if (sub && sub[0]) {
            i32 fw = state ? state->graphic.width  : 640;
            i32 fh = state ? state->graphic.height : 480;
            const i32 lineh = 18;                 /* font cell height        */
            const i32 pad   = 8;
            i32 boxx = 24, boxw = fw - 48;         /* near-full-width bar      */
            i32 maxc = (boxw - 2 * pad) / 8;       /* chars per line (~8px)    */
            if (maxc < 8)  maxc = 8;
            if (maxc > 120) maxc = 120;

            /* Word-wrap sub into lines[]. */
            c8  lines[6][128];
            i32 nlines = 0;
            const c8 *p = sub;
            while (*p && nlines < 6) {
                i32 n = 0, lastsp = -1;
                while (p[n] && n < maxc && n < 127) {
                    if (p[n] == ' ') lastsp = n;
                    n++;
                }
                i32 take = n;
                if (p[n] && lastsp > 0) take = lastsp;   /* break at last space */
                i32 k = 0;
                for (i32 i = 0; i < take && k < 127; i++) lines[nlines][k++] = p[i];
                lines[nlines][k] = '\0';
                nlines++;
                p += take;
                while (*p == ' ') p++;
            }
            if (nlines == 0) nlines = 1;

            i32 boxh = nlines * lineh + 2 * pad;
            i32 boxy = fh - boxh - 8;              /* bottom-anchored bar      */
            scene_fill_rect(state, boxx, boxy, boxw, boxh, 0xFF000000u);
            for (i32 li = 0; li < nlines; li++) {
                i32 tw = (i32)strlen(lines[li]) * 8;
                i32 sx = boxx + (boxw - tw) / 2;   /* centre each line         */
                if (sx < boxx + pad) sx = boxx + pad;
                draw_text_at(font_handle, sx, boxy + pad + li * lineh,
                             lines[li], 0xFFFFFFFFu);
            }
        }
    }

    /* Headless verification: dump the composed frame to a PPM at frame
     * DKE_SHOT so the scene (backdrop + rasterized characters) can be
     * inspected without a display. */
    if (getenv("DKE_SHOT") && state && state->graphic.front_buffer) {
        static i32 s_frame = 0;
        i32 want = atoi(getenv("DKE_SHOT"));
        if (s_frame++ == want) {
            const c8 *out = getenv("DKE_SHOT_PATH");
            FILE *f = fopen(out ? out : "/tmp/dke_shot.ppm", "wb");
            if (f) {
                u32 *fb = state->graphic.front_buffer;
                i32 w = state->graphic.width, h = state->graphic.height;
                fprintf(f, "P6\n%d %d\n255\n", w, h);
                for (i32 i = 0; i < w * h; i++) {
                    u32 p = fb[i];
                    fputc((u8)(p & 0xFF), f);
                    fputc((u8)((p >> 8) & 0xFF), f);
                    fputc((u8)((p >> 16) & 0xFF), f);
                }
                fclose(f);
                print("scene: DKE_SHOT wrote frame %d\n", want);
            }
        }
    }
    return 0;
}

void scene_release(void) {
    model_default_pitch = 300;
    g_scene_cam_sx = 320;
    g_scene_cam_sy = 240;
    scene_free_bmap(&g_scene_bg_bmap);
    scene_free_bmap(&g_scene_l_bmap);
    scene_free_bmap(&g_scene_z_bmap);
    for (i32 k = 0; k < g_scene_ome_atom_count; k++) {
        model_free(g_scene_ome_atoms[k]);
        g_scene_ome_atoms[k] = NULL;
    }
    g_scene_ome_atom_count = 0;
    ani_free(&g_scene_ani);
    g_scene_ani_loaded = 0;
    g_scene_bg_level   = -1;
    g_scene_objc_loaded = 0;
    memset(&g_scene_objc, 0, sizeof(g_scene_objc));
    lieu_exit();
}
