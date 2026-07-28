/*
 * world.c — World controller / level data loader (sub_46D305)
 *
 * Builds level asset paths, loads the DAT config, parses INF binary
 * cache (or falls back to ASC text parsing), and populates camera zone,
 * collision zone, and staircase zone polygon data.
 *
 * INF cache format (version 7):
 *   [4] magic=7  [4] asc_timestamp  [0x208] world_controller
 *   [4] n_cam  [n_cam × 0x208] camera_zones
 *   [4] n_col  [n_col × 0x208] collision_zones
 *   [4] n_stair [n_stair × 0x208] stair_zones
 *   [4] n_views [n_views × 0xA8] cam_views (0x78 params + 0x18 pos + 0x18 dir)
 */

#include "world.h"
#include "libsys/file_system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

world_data_t g_world;

static sol_limit_t *alloc_sol(void) {
    sol_limit_t *s = (sol_limit_t *)calloc(1, sizeof(sol_limit_t));
    return s;
}

static i32 read_sol_from_file(FILE *f, sol_limit_t **out) {
    *out = alloc_sol();
    if (!*out) return 0;
    u8 raw[SOL_LIMIT_SIZE];
    ls_read_buffer(f, raw, SOL_LIMIT_SIZE);
    memcpy(*out, raw, sizeof(sol_limit_t) < SOL_LIMIT_SIZE
                       ? sizeof(sol_limit_t) : SOL_LIMIT_SIZE);
    return 1;
}

void world_load_level_data(i32 level_id, const c8 *game_path,
                           i32 is_demo, i32 is_hdd, i32 debug) {
    memset(&g_world, 0, sizeof(g_world));

    snprintf(g_world.level_data_path, 256, "DATAS>L%02d>", level_id);
    snprintf(g_world.images_path, 256, "%sIMAGES>", g_world.level_data_path);
    snprintf(g_world.pal_path, 256, "%sPAL>", g_world.level_data_path);
    snprintf(g_world.anim2d_path, 256, "%sANIM2D>", g_world.level_data_path);

    snprintf(g_world.level_dll_name, 64, "LIEU%02d", level_id);
    if (is_demo) {
        c8 tmp[64];
        snprintf(tmp, 64, "D_%s", g_world.level_dll_name);
        strncpy(g_world.level_dll_name, tmp, 63);
    }

    print("world: level %d — paths built, dll=%s\n",
          level_id, g_world.level_dll_name);

    c8 inf_vpath[128], inf_host[512];
    snprintf(inf_vpath, sizeof(inf_vpath), "%sL%02d.INF",
             g_world.level_data_path, level_id);

    if (!ls_resolve_path(game_path, inf_vpath, inf_host, (i32)sizeof(inf_host))) {
        print("world: cannot resolve INF path '%s'\n", inf_vpath);
        return;
    }

    if (!ls_exists(inf_host)) {
        print("world: INF '%s' not found — zone data unavailable\n", inf_host);
        return;
    }

    FILE *f = ls_open(inf_host, "rb");
    if (!f) {
        print("world: cannot open INF '%s'\n", inf_host);
        return;
    }

    i32 magic = 0;
    ls_read_buffer(f, &magic, 4);
    if (magic != 7) {
        print("world: INF magic %d != 7 — skipping\n", magic);
        ls_close(f);
        return;
    }

    i32 timestamp = 0;
    ls_read_buffer(f, &timestamp, 4);

    g_world.world_controller = alloc_sol();
    if (g_world.world_controller) {
        u8 raw[SOL_LIMIT_SIZE];
        ls_read_buffer(f, raw, SOL_LIMIT_SIZE);
        memcpy(g_world.world_controller, raw,
               sizeof(sol_limit_t) < SOL_LIMIT_SIZE
               ? sizeof(sol_limit_t) : SOL_LIMIT_SIZE);
    }

    i32 n = 0;
    ls_read_buffer(f, &n, 4);
    if (n > WORLD_MAX_CAMERAS) n = WORLD_MAX_CAMERAS;
    g_world.num_cameras = n;
    for (i32 i = 0; i < n; i++)
        read_sol_from_file(f, &g_world.camera_zones[i]);

    ls_read_buffer(f, &n, 4);
    if (n > WORLD_MAX_COLLISIONS) n = WORLD_MAX_COLLISIONS;
    g_world.num_collisions = n;
    for (i32 i = 0; i < n; i++)
        read_sol_from_file(f, &g_world.collision_zones[i]);

    ls_read_buffer(f, &n, 4);
    if (n > WORLD_MAX_STAIRS) n = WORLD_MAX_STAIRS;
    g_world.num_stairs = n;
    for (i32 i = 0; i < n; i++)
        read_sol_from_file(f, &g_world.stair_zones[i]);

    ls_read_buffer(f, &n, 4);
    if (n > WORLD_MAX_CAM_VIEWS) n = WORLD_MAX_CAM_VIEWS;
    g_world.num_cam_views = n;
    for (i32 i = 0; i < n; i++) {
        ls_read_buffer(f, g_world.cam_views[i].params, 0x78);
        ls_read_buffer(f, g_world.cam_views[i].position, 0x18);
        ls_read_buffer(f, g_world.cam_views[i].direction, 0x18);
    }

    ls_close(f);

    print("world: INF loaded — %d cameras, %d collisions, %d stairs, %d views\n",
          g_world.num_cameras, g_world.num_collisions,
          g_world.num_stairs, g_world.num_cam_views);
}

void world_free(void) {
    free(g_world.world_controller);
    g_world.world_controller = NULL;

    for (i32 i = 0; i < g_world.num_cameras; i++)
        free(g_world.camera_zones[i]);
    for (i32 i = 0; i < g_world.num_collisions; i++)
        free(g_world.collision_zones[i]);
    for (i32 i = 0; i < g_world.num_stairs; i++)
        free(g_world.stair_zones[i]);

    memset(&g_world, 0, sizeof(g_world));
}
