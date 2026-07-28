/*
 * nav_grid.c — 64×64 navigation/collision grid builder (sub_45619D)
 *
 * Rasterizes the room boundary polygon and INF collision regions onto
 * a fixed 64×64 byte grid. Each cell maps to a world-space rectangle;
 * 5 sample points (center + 4 corners) are tested against the room
 * polygon and collision zones.
 *
 * Cell values:
 *   0x00 = FREE (walkable)
 *   0x02 = BORDER (adjacent to wall)
 *   0x03 = WALL (inside collision zone with flag bit 1)
 *   0xFF = BLOCKED (outside room polygon)
 *
 * After classification, horizontal and vertical spreading passes
 * propagate boundary costs into neighboring cells (up to 15000 world
 * units). A final pass marks wall-adjacent cells as BORDER.
 */

#include "nav_grid.h"
#include <string.h>

nav_grid_t g_nav;

i32 nav_point_in_polygon(i32 n_verts, i32 test_x, i32 test_y,
                         const i32 *vertices) {
    if (n_verts < 3) return 0;
    i32 inside = 0;
    for (i32 i = 0, j = n_verts - 1; i < n_verts; j = i++) {
        i32 xi = vertices[i * 3];
        i32 yi = vertices[i * 3 + 2];
        i32 xj = vertices[j * 3];
        i32 yj = vertices[j * 3 + 2];
        if ((yi > test_y) != (yj > test_y)) {
            double x_cross = (double)(xj - xi) * (double)(test_y - yi)
                           / (double)(yj - yi) + (double)xi;
            if ((double)test_x <= x_cross)
                inside = !inside;
        }
    }
    return inside;
}

static i32 find_collision_at(i32 wx, i32 wy,
                             sol_limit_t *const *zones, i32 n_zones,
                             u8 *out_flags) {
    for (i32 i = 0; i < n_zones; i++) {
        const sol_limit_t *z = zones[i];
        if (!z || !(z->flags & 1)) continue;
        if (wx <= z->min_x || wx >= z->max_x) continue;
        if (wy <= z->min_y || wy >= z->max_y) continue;
        if (nav_point_in_polygon(z->vertex_count, wx, wy,
                                 &z->vertices[0][0])) {
            if (out_flags) *out_flags = (u8)(z->flags & 0xFF);
            return 1;
        }
    }
    return 0;
}

static u8 classify_point(i32 wx, i32 wy,
                         const sol_limit_t *world_ctrl,
                         sol_limit_t *const *zones, i32 n_zones) {
    u8 flags = 0;
    if (find_collision_at(wx, wy, zones, n_zones, &flags))
        return (flags & 2) ? NAV_WALL : NAV_BLOCKED;
    if (!nav_point_in_polygon(world_ctrl->vertex_count, wx, wy,
                              &world_ctrl->vertices[0][0]))
        return NAV_BLOCKED;
    return NAV_FREE;
}

static void spread_row_forward(u8 *grid, i32 row, i32 limit) {
    u8 *base = grid + row * 64;
    i32 count = 0;
    for (i32 c = 0; c < 64; c++) {
        if (base[c] & 1) {
            count = limit;
        } else if (count > 0 && base[c] == 0) {
            base[c] = (u8)(NAV_BLOCKED - 1);
            count--;
        }
    }
}

static void spread_row_reverse(u8 *grid, i32 row, i32 limit) {
    u8 *base = grid + row * 64;
    i32 count = 0;
    for (i32 c = 63; c >= 0; c--) {
        if (base[c] & 1) {
            count = limit;
        } else if (count > 0 && base[c] == 0) {
            base[c] = (u8)(NAV_BLOCKED - 1);
            count--;
        }
    }
}

static void spread_col_forward(u8 *grid, i32 col, i32 limit) {
    i32 count = 0;
    for (i32 r = 0; r < 64; r++) {
        u8 *cell = &grid[r * 64 + col];
        if (*cell & 1) {
            count = limit;
        } else if (count > 0 && *cell == 0) {
            *cell = (u8)(NAV_BLOCKED - 1);
            count--;
        }
    }
}

static void spread_col_reverse(u8 *grid, i32 col, i32 limit) {
    i32 count = 0;
    for (i32 r = 63; r >= 0; r--) {
        u8 *cell = &grid[r * 64 + col];
        if (*cell & 1) {
            count = limit;
        } else if (count > 0 && *cell == 0) {
            *cell = (u8)(NAV_BLOCKED - 1);
            count--;
        }
    }
}

void nav_grid_build(const sol_limit_t *world_ctrl,
                    sol_limit_t *const *collision_zones, i32 num_collisions) {
    memset(&g_nav, 0, sizeof(g_nav));

    if (!world_ctrl || world_ctrl->vertex_count < 3) {
        print("nav_grid: no world controller — grid empty\n");
        return;
    }

    g_nav.world_x_origin = world_ctrl->min_x;
    g_nav.world_y_origin = world_ctrl->min_y;
    g_nav.cell_width  = (world_ctrl->max_x - world_ctrl->min_x) / 64 + 1;
    g_nav.cell_height = (world_ctrl->max_y - world_ctrl->min_y) / 64 + 1;

    if (g_nav.cell_width < 1) g_nav.cell_width = 1;
    if (g_nav.cell_height < 1) g_nav.cell_height = 1;

    for (i32 col = 0; col < 64; col++) {
        i32 wx0 = col * g_nav.cell_width + g_nav.world_x_origin;
        i32 wx1 = wx0 + g_nav.cell_width;
        i32 wcx = wx0 + g_nav.cell_width / 2;

        for (i32 row = 0; row < 64; row++) {
            i32 wy0 = row * g_nav.cell_height + g_nav.world_y_origin;
            i32 wy1 = wy0 + g_nav.cell_height;
            i32 wcy = wy0 + g_nav.cell_height / 2;

            u8 val = NAV_FREE;

            if (val == NAV_FREE) val = classify_point(wcx, wcy, world_ctrl, collision_zones, num_collisions);
            if (val == NAV_FREE) val = classify_point(wx0, wy0, world_ctrl, collision_zones, num_collisions);
            if (val == NAV_FREE) val = classify_point(wx0, wy1, world_ctrl, collision_zones, num_collisions);
            if (val == NAV_FREE) val = classify_point(wx1, wy1, world_ctrl, collision_zones, num_collisions);
            if (val == NAV_FREE) val = classify_point(wx1, wy0, world_ctrl, collision_zones, num_collisions);

            if (val != NAV_FREE)
                g_nav.grid[row * 64 + col] = val;
        }
    }

    i32 h_spread = 15000 / g_nav.cell_width;
    for (i32 r = 0; r < 64; r++) {
        spread_row_forward(g_nav.grid, r, h_spread);
        spread_row_reverse(g_nav.grid, r, h_spread);
    }

    i32 v_spread = 15000 / g_nav.cell_height;
    for (i32 c = 0; c < 64; c++) {
        spread_col_forward(g_nav.grid, c, v_spread);
        spread_col_reverse(g_nav.grid, c, v_spread);
    }

    for (i32 col = 1; col < 63; col++) {
        for (i32 row = 1; row < 63; row++) {
            u8 *cell = &g_nav.grid[row * 64 + col];
            if (*cell == NAV_FREE || *cell == NAV_WALL) continue;

            if (cell[-65] == NAV_WALL || cell[-64] == NAV_WALL || cell[-63] == NAV_WALL ||
                cell[-1]  == NAV_WALL ||                          cell[1]   == NAV_WALL ||
                cell[63]  == NAV_WALL || cell[64]  == NAV_WALL || cell[65]  == NAV_WALL) {
                *cell = NAV_BORDER;
            }
        }
    }

    i32 free_count = 0, wall_count = 0, blocked_count = 0;
    for (i32 i = 0; i < NAV_GRID_CELLS; i++) {
        if (g_nav.grid[i] == NAV_FREE) free_count++;
        else if (g_nav.grid[i] == NAV_WALL) wall_count++;
        else if (g_nav.grid[i] == NAV_BLOCKED) blocked_count++;
    }
    print("nav_grid: built — cell=%dx%d free=%d wall=%d blocked=%d\n",
          g_nav.cell_width, g_nav.cell_height, free_count, wall_count, blocked_count);
}
