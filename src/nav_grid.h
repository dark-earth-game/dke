#ifndef DKE_NAV_GRID_H
#define DKE_NAV_GRID_H

#include "libsys/common.h"
#include "world.h"

#define NAV_GRID_SIZE    64
#define NAV_GRID_CELLS   (NAV_GRID_SIZE * NAV_GRID_SIZE)

#define NAV_FREE     0x00
#define NAV_BORDER   0x02
#define NAV_WALL     0x03
#define NAV_BLOCKED  0xFF

typedef struct {
    u8  grid[NAV_GRID_CELLS];
    i32 world_x_origin;
    i32 world_y_origin;
    i32 cell_width;
    i32 cell_height;
} nav_grid_t;

extern nav_grid_t g_nav;

void nav_grid_build(const sol_limit_t *world_ctrl,
                    sol_limit_t *const *collision_zones, i32 num_collisions);

i32 nav_point_in_polygon(i32 n_verts, i32 test_x, i32 test_y,
                         const i32 *vertices);

#endif /* DKE_NAV_GRID_H */
