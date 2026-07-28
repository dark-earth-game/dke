#ifndef DKE_WORLD_H
#define DKE_WORLD_H

#include "libsys/common.h"

#define WORLD_MAX_CAMERAS       10
#define WORLD_MAX_COLLISIONS    64
#define WORLD_MAX_STAIRS       110
#define WORLD_MAX_CAM_VIEWS     10

#define SOL_LIMIT_SIZE  0x208
#define SOL_MAX_VERTS   16

typedef struct {
    u32  flags;
    c8   name[16];
    i32  vertices[SOL_MAX_VERTS][3];
    u8   _pad[0x1D4 - 0x14 - SOL_MAX_VERTS * 12];
    i32  camera_index;
    i32  vertex_count;
    i32  min_x, min_z, min_y;
    i32  max_x, max_z, max_y;
    i32  center_x;
    i32  _unused;
    i32  center_y;
    u32  hash1;
    u32  hash2;
} sol_limit_t;

typedef struct {
    i32  params[30];
    u8   position[24];
    u8   direction[24];
} cam_view_t;

typedef struct {
    c8   level_data_path[256];
    c8   images_path[256];
    c8   pal_path[256];
    c8   anim2d_path[256];
    c8   wav_normal_path[256];
    c8   wav_anims_path[256];
    c8   level_dll_name[64];

    sol_limit_t *world_controller;
    sol_limit_t *camera_zones[WORLD_MAX_CAMERAS];
    i32          num_cameras;
    sol_limit_t *collision_zones[WORLD_MAX_COLLISIONS];
    i32          num_collisions;
    sol_limit_t *stair_zones[WORLD_MAX_STAIRS];
    i32          num_stairs;
    cam_view_t   cam_views[WORLD_MAX_CAM_VIEWS];
    i32          num_cam_views;
} world_data_t;

extern world_data_t g_world;

void world_load_level_data(i32 level_id, const c8 *game_path,
                           i32 is_demo, i32 is_hdd, i32 debug);
void world_free(void);

#endif /* DKE_WORLD_H */
