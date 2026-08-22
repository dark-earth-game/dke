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

/*
 * Grid queries. The grid spans (world X, world Z): the builder takes its
 * bounds from the world controller's min/max pair at +0x1DC / +0x1E4 and
 * tests vertex components 0 and 2, so the axis this struct calls "y" is the
 * world Z axis.
 *
 * nav_cell        — raw cell value, NAV_BLOCKED when outside the grid.
 * nav_is_walkable — 1 only for NAV_FREE cells.
 * nav_snap_to_walkable — port of sub_456FB7 at 0x456FB7: if (x,z) sits in a
 *   blocked cell, spiral outward over growing rings until a free cell is
 *   found and rewrite the caller's x/z to that cell's origin. Used to place
 *   things that
 *   must not end up inside geometry. Returns 1 if it moved the point.
 */
i32  nav_cell(i32 world_x, i32 world_z);
i32  nav_is_walkable(i32 world_x, i32 world_z);
i32  nav_snap_to_walkable(i32 *world_x, i32 *world_z);

#endif /* DKE_NAV_GRID_H */
