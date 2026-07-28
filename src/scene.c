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
#include "anim2d.h"
#include "can.h"
#include "model.h"
#include "model_ome.h"
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
static bmap_t *g_scene_l_bmap  = NULL;   /* L */
static bmap_t *g_scene_z_bmap  = NULL;   /* Z */

/* Level id whose backdrop is currently in g_scene_bg_bmap.  -1 = none. */
static i32 g_scene_bg_level = -1;

/* Camera index within the level (0..6 for L01). Runtime-cycled via
 * scene_render input handling (Left/Right arrow keys). */
static i32 g_scene_camera = 0;

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
    can_anim_t can;                            /* ARR01 arrival clip         */
    i32        can_loaded;
    i32        frame;
    u32        can_last;
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
    for (i32 ci = 0; ci < ome.chunk_count; ci++) {
        if (ome.chunks[ci].kind != OME_CHUNK_ATOMF) continue;
        if (pm->count >= SCENE_OME_MAX_ATOMS) break;
        ome_atom_t atom = {0};
        if (ome_decode_atomf(ome.raw + ome.chunks[ci].offset + 0x60,
                             ome.chunks[ci].length - 0x60, &atom) &&
            atom.vert_count > 0 && atom.face_count > 0) {
            model_t *m = model_from_ome_atom(&atom, matr_ptr);
            if (m) pm->atoms[pm->count++] = m;
        }
        ome_atom_free(&atom);
    }
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

    print("scene: loaded persona P%02d.OME (%d atoms, objc=%d, can=%d)\n",
          num, pm->count, pm->objc_loaded, pm->can_loaded);
    return g_persona_cache_n++;
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
 * Called for g_player_entity, companion_entity, secondary_companion_entity.
 * Mirrors asm: mov eax, g_player_entity / mov ecx, [eax+50h] / call process_color_palette */
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
 * Mirrors asm: mov edx, [eax+8Ch] / mov ecx, g_player_entity / call apply_model_transform */
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

    /* Seek to cam_idx zone struct (+0x1D8 for n_verts, +0x1DC for vertices) */
    long cam_struct_off = ftell(f) + (long)cam_idx * 0x208;
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

    /* Skip to extra section (projection matrices).
     * Continue reading: remaining cam structs + inexplorable + stairs + n_extra */
    fseek(f, cam_struct_off + (long)n_cam * 0x208, SEEK_SET);
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
    /* Ground plane Y: eye Y (T[1] = ey) is a reasonable default; personas
     * override with their own PosPerso ground when rendered. */
    g_proj_ground_wy = param[10];
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
     * Phase 5 — zero byte_511760; zero g_player_entity offsets
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
    if (g_player_entity) {
        u8 *pe = (u8 *)g_player_entity;
        *(i32 *)(pe + 0xC64) = 0x10000;
        *(i32 *)(pe + 0xC68) = 0x4000;
    }
    /* dword_4DD9B4 = 1 (TODO: add global) */

    /* -----------------------------------------------------------------
     * Phase 15 — process_color_palette for player + companions
     * Asm: mov ecx, [eax+50h] / call process_color_palette (×3)
     * ----------------------------------------------------------------- */
    print("scene: phase 15 — process_color_palette (player+companions) — STUB\n");
    stub_process_color_palette(g_player_entity);   /* TODO: process_color_palette */

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
    stub_apply_model_transform(NULL, NULL); /* TODO: g_player_entity */

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
        if (kf && j < CAN_MAX_ATOMSLOTS) {
            rx_idx = (i32)((u32)kf->atoms[j].rx >> 4) & 0xFFF;
            ry_idx = (i32)((u32)kf->atoms[j].ry >> 4) & 0xFFF;
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
    lieu_tick();
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
        g_player_wx = g_proj_anchor_wx;
        g_player_wz = g_proj_zone_z;
        g_player_seeded = 1;
    }

    if (sys) {
        u32 tick = sys->timer.tick;
        i32 dt_ms = g_scene_prev_tick > 0 ? (i32)(tick - g_scene_prev_tick) : 16;
        if (dt_ms < 0)     dt_ms = 0;
        if (dt_ms > 100)   dt_ms = 100;   /* clamp on long pauses */
        g_scene_prev_tick = tick;

        player_input_t pin = {0};
        player_input_update(&pin, sys);



        /* ~50k world units/s (LIEU zones are ~50-90k wide). */
        i32 step = (50000 * dt_ms) / 1000;
        g_player_wx += (i32)(pin.velocity_x * step);   /* A/D -> world X */
        g_player_wz += (i32)(pin.velocity_z * step);   /* W/S -> world Z */

        /* Debug headless sweep (fixed step, dt-independent). */
        if (getenv("DKE_WALK")) g_player_wx -= 6000;

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
            for (i32 j = 0; j < g_scene_objc.n_bones && j < CAN_MAX_ATOMSLOTS; j++) {
                i32 ri = g_scene_objc.can_to_render[j];
                if (ri < 0 || ri >= g_scene_ome_atom_count) continue;
                atom_rx[ri] = (i32)(kf->atoms[j].rx >> 4) & 0xFFF;
                atom_ry[ri] = (i32)(kf->atoms[j].ry >> 4) & 0xFFF;
            }
        }
        for (i32 ai = 0; ai < g_scene_ome_atom_count; ai++) {
            i32 tx = (g_scene_objc_loaded && ai < g_scene_objc.n_atomslots)
                       ? g_scene_objc.world_tx[ai] * ATOM_SCALE : 0;
            i32 ty = (g_scene_objc_loaded && ai < g_scene_objc.n_atomslots)
                       ? g_scene_objc.world_ty[ai] * ATOM_SCALE : 0;
            i32 tz = (g_scene_objc_loaded && ai < g_scene_objc.n_atomslots)
                       ? g_scene_objc.world_tz[ai] * ATOM_SCALE : 0;
            model_render(g_scene_ome_atoms[ai],
                         sys,
                         screen_x, screen_y,
                         focal,
                         tick,
                         render_depth,
                         cube_world_size,
                         z_far,
                         g_scene_l_bmap,
                         g_scene_z_bmap,
                         atom_rx[ai] ? 1 : 0, atom_ry[ai], atom_rx[ai],
                         tx, ty, tz);
        }

        /* Render each LIEU-placed persona (P00ARR01, P12ARR01, ...) as the
         * loaded character mesh, at a screen position near the INF anchor.
         * Placeholder: all personas share the player mesh until per-persona
         * models load; positions are clustered around the character stand. */
        for (i32 pi = 0; pi < lieu_spawn_count(); pi++) {
            i32 wxt = 0, wzt = 0, pview = 0;
            const c8 *nm = lieu_spawn_get(pi, &wxt, &wzt, &pview);
            if (!nm) continue;
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
                for (i32 jj = 0; jj < pm->objc.n_bones && jj < CAN_MAX_ATOMSLOTS; jj++) {
                    i32 ri = pm->objc.can_to_render[jj];
                    if (ri < 0 || ri >= pm->count) continue;
                    prx[ri] = (i32)((u32)kf->atoms[jj].rx >> 4) & 0xFFF;
                    pry[ri] = (i32)((u32)kf->atoms[jj].ry >> 4) & 0xFFF;
                }
            }
            for (i32 ai = 0; ai < pm->count; ai++) {
                i32 tx = (pm->objc_loaded && ai < pm->objc.n_atomslots)
                           ? pm->objc.world_tx[ai] * ATOM_SCALE : 0;
                i32 ty = (pm->objc_loaded && ai < pm->objc.n_atomslots)
                           ? pm->objc.world_ty[ai] * ATOM_SCALE : 0;
                i32 tz = (pm->objc_loaded && ai < pm->objc.n_atomslots)
                           ? pm->objc.world_tz[ai] * ATOM_SCALE : 0;
                model_render(pm->atoms[ai], sys, psx, psy,
                             focal, tick, render_depth, cube_world_size, z_far,
                             g_scene_l_bmap, g_scene_z_bmap,
                             prx[ai] ? 1 : 0, pry[ai], prx[ai], tx, ty, tz);
            }
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
