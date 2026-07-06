/*
 * model.c — placeholder 3D model + wireframe renderer.
 * See model.h for the module contract.
 *
 * All math is Q16.16 fixed-point using the existing cosine_table
 * (Q14, 4096 entries) from libsys/graphics/shading.h.
 */

#include "libsys/common.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/shading.h"
#include "state.h"
#include "model.h"


/* -------------------------------------------------------------------------
 * Test cube: 8 verts, 12 triangles.
 *
 * Model space is a unit cube: coords are Q16.16 in ±0x10000 (±1.0).
 * Coordinate frame: +X right, +Y up, +Z out of the screen.
 * Face indices are wound counter-clockwise when viewed from outside.
 * ------------------------------------------------------------------------- */

#define ONE  0x10000   /* 1.0 in Q16.16 */

static const model_vec3_t CUBE_VERTS[8] = {
    { -ONE, -ONE, -ONE },   /* 0: back-bottom-left  */
    {  ONE, -ONE, -ONE },   /* 1: back-bottom-right */
    {  ONE,  ONE, -ONE },   /* 2: back-top-right    */
    { -ONE,  ONE, -ONE },   /* 3: back-top-left     */
    { -ONE, -ONE,  ONE },   /* 4: front-bottom-left */
    {  ONE, -ONE,  ONE },   /* 5: front-bottom-right*/
    {  ONE,  ONE,  ONE },   /* 6: front-top-right   */
    { -ONE,  ONE,  ONE },   /* 7: front-top-left    */
};

/* Colours packed 0xFF000000 | (B<<16) | (G<<8) | R to match blit byte order. */
#define RGB_TO_XRGB(r, g, b) \
    ((u32)0xFF000000u | ((u32)(b) << 16) | ((u32)(g) << 8) | (u32)(r))

static const model_face_t CUBE_FACES[12] = {
    /* Front (+Z)  — bright red */
    { RGB_TO_XRGB(220,  60,  60), 4, 5, 6, 0 },
    { RGB_TO_XRGB(220,  60,  60), 4, 6, 7, 0 },
    /* Back  (-Z)  — dark red */
    { RGB_TO_XRGB(140,  30,  30), 0, 2, 1, 0 },
    { RGB_TO_XRGB(140,  30,  30), 0, 3, 2, 0 },
    /* Right (+X)  — bright green */
    { RGB_TO_XRGB( 60, 220,  60), 1, 2, 6, 0 },
    { RGB_TO_XRGB( 60, 220,  60), 1, 6, 5, 0 },
    /* Left  (-X)  — dark green */
    { RGB_TO_XRGB( 30, 140,  30), 0, 4, 7, 0 },
    { RGB_TO_XRGB( 30, 140,  30), 0, 7, 3, 0 },
    /* Top   (+Y)  — bright blue */
    { RGB_TO_XRGB( 60,  60, 220), 3, 7, 6, 0 },
    { RGB_TO_XRGB( 60,  60, 220), 3, 6, 2, 0 },
    /* Bot   (-Y)  — dark blue */
    { RGB_TO_XRGB( 30,  30, 140), 0, 1, 5, 0 },
    { RGB_TO_XRGB( 30,  30, 140), 0, 5, 4, 0 },
};

static const model_t CUBE = {
    CUBE_VERTS, 8,
    CUBE_FACES, 12,
};

const model_t *model_test_cube(void) { return &CUBE; }

/* -------------------------------------------------------------------------
 * Renderer
 * ------------------------------------------------------------------------- */

/*
 * Projected vertex — screen (x, y) plus depth for the Z-test.
 * depth is packed to fit the Z bmap's u8 space (0=near, 255=far) so it can
 * be compared directly against z_bmap pixels.
 */
typedef struct {
    i32 sx, sy;   /* pixel coords in the front buffer */
    i32 depth;   /* clamped to 0..255 */
} proj_vertex_t;

/*
 * Sample the light map at (fb_x, fb_y). Returns 128 (neutral) when l_bmap
 * is NULL or coords are out of bounds — no shading applied.
 * L is grayscale palette (index == brightness), quarter-res upscale ×2.
 */
static inline u32 sample_light(const bmap_t *l_bmap, i32 fb_x, i32 fb_y,
                                i32 fb_w, i32 fb_h) {
    if (!l_bmap || !l_bmap->pixels) return 128;
    i32 lw = (i32)l_bmap->width;
    i32 lh = l_bmap->height;
    i32 lx = (fb_x * lw) / fb_w;
    i32 ly = (fb_y * lh) / fb_h;
    if ((u32)lx >= (u32)lw || (u32)ly >= (u32)lh) return 128;
    u32 l = l_bmap->pixels[ly * lw + lx];
    /* Floor at 48 so the model doesn't vanish in unlit corners.
     * Removed once we understand the L semantics per real .OME mesh. */
    if (l < 48) l = 48;
    return l;
}

/*
 * Sample scene depth. Returns 255 (far) when no Z bmap, so the model
 * always draws in front — that keeps the wireframe visible on backdrops
 * that ship without a Z layer.
 */
static inline u32 sample_depth(const bmap_t *z_bmap, i32 fb_x, i32 fb_y) {
    if (!z_bmap || !z_bmap->pixels) return 255;
    i32 zw = (i32)z_bmap->width;
    i32 zh = z_bmap->height;
    if ((u32)fb_x >= (u32)zw || (u32)fb_y >= (u32)zh) return 255;
    return z_bmap->pixels[fb_y * zw + fb_x];
}

/*
 * Modulate an XRGB colour by an L (0..255) sample. Formula matches the
 * era-standard 2× lighting convention: L=128 → identity, L=255 → 2× (clamped).
 */
static inline u32 apply_light(u32 col, u32 l) {
    u32 r = (col >>  0) & 0xFF;
    u32 g = (col >>  8) & 0xFF;
    u32 b = (col >> 16) & 0xFF;
    r = (r * l) >> 7;   if (r > 255) r = 255;
    g = (g * l) >> 7;   if (g > 255) g = 255;
    b = (b * l) >> 7;   if (b > 255) b = 255;
    return 0xFF000000u | (b << 16) | (g << 8) | r;
}

/*
 * Write one pixel with depth + light. depth is the model pixel's projected
 * depth (0..255). Only draws when depth < scene depth (model in front).
 */
static void plot_pixel(u32 *fb, i32 fb_w, i32 fb_h,
                       i32 x, i32 y, i32 depth,
                       u32 colour,
                       const bmap_t *l_bmap, const bmap_t *z_bmap) {
    if ((u32)x >= (u32)fb_w || (u32)y >= (u32)fb_h) return;
    u32 scene_z = sample_depth(z_bmap, x, y);
    if ((u32)depth > scene_z) return;   /* behind scene → skip */
    u32 l = sample_light(l_bmap, x, y, fb_w, fb_h);
    fb[y * fb_w + x] = apply_light(colour, l);
}

/*
 * Draw a horizontal scanline with linear depth interpolation.
 * xL/xR are inclusive left/right edges; zL/zR are depths at those edges.
 */
static void draw_scanline(u32 *fb, i32 fb_w, i32 fb_h,
                          i32 y, i32 xL, i32 xR, i32 zL, i32 zR,
                          u32 colour,
                          const bmap_t *l_bmap, const bmap_t *z_bmap) {
    if (y < 0 || y >= fb_h) return;
    if (xL > xR) {
        i32 t = xL; xL = xR; xR = t;
        i32 tz = zL; zL = zR; zR = tz;
    }
    i32 span = xR - xL;
    if (span < 0) return;
    i32 dz = zR - zL;

    /* Clip to fb bounds while keeping z interpolation correct. */
    i32 start = xL;
    i32 end   = xR;
    if (start < 0)       start = 0;
    if (end   >= fb_w)   end   = fb_w - 1;
    if (start > end)     return;

    for (i32 x = start; x <= end; x++) {
        i32 depth = span > 0 ? zL + (dz * (x - xL)) / span : zL;
        plot_pixel(fb, fb_w, fb_h, x, y, depth, colour, l_bmap, z_bmap);
    }
}

/*
 * Solid triangle fill with linear depth interpolation.  Splits at the
 * middle vertex into a flat-bottom + flat-top pair and walks scanlines
 * between the interpolated left/right edges.  Depth is interpolated
 * along each edge in Q16.16 so per-pixel Z-tests get sub-scanline
 * accuracy on tall triangles.
 */
static void fill_triangle(u32 *fb, i32 fb_w, i32 fb_h,
                          i32 x0, i32 y0, i32 z0,
                          i32 x1, i32 y1, i32 z1,
                          i32 x2, i32 y2, i32 z2,
                          u32 colour,
                          const bmap_t *l_bmap, const bmap_t *z_bmap) {
    /* Sort by y ascending: (x0,y0,z0) top, (x1,y1,z1) middle, (x2,y2,z2) bottom. */
    #define SWAP_VERT(A, B) do {                              \
        i32 _t; _t = x##A; x##A = x##B; x##B = _t;             \
                _t = y##A; y##A = y##B; y##B = _t;             \
                _t = z##A; z##A = z##B; z##B = _t;             \
    } while (0)
    if (y0 > y1) SWAP_VERT(0, 1);
    if (y0 > y2) SWAP_VERT(0, 2);
    if (y1 > y2) SWAP_VERT(1, 2);
    #undef SWAP_VERT

    if (y2 == y0) return;   /* degenerate — collapsed triangle */

    /* Edge (0→2) is the long edge that spans the whole triangle.
     * Compute the x + z on that edge at the middle vertex's y. */
    i32 total_dy = y2 - y0;
    i32 mid_dy   = y1 - y0;
    i32 mid_edge_x = x0 + ((x2 - x0) * mid_dy) / total_dy;
    i32 mid_edge_z = z0 + ((z2 - z0) * mid_dy) / total_dy;

    /* Upper half: y0..y1. */
    if (y1 != y0) {
        i32 dy_up = y1 - y0;
        for (i32 y = y0; y <= y1; y++) {
            i32 t = y - y0;
            i32 xL = x0 + ((x1 - x0)     * t) / dy_up;
            i32 xR = x0 + ((mid_edge_x - x0) * t) / dy_up;
            i32 zL = z0 + ((z1 - z0)     * t) / dy_up;
            i32 zR = z0 + ((mid_edge_z - z0) * t) / dy_up;
            draw_scanline(fb, fb_w, fb_h, y, xL, xR, zL, zR,
                          colour, l_bmap, z_bmap);
        }
    }

    /* Lower half: y1..y2. */
    if (y2 != y1) {
        i32 dy_lo = y2 - y1;
        for (i32 y = y1; y <= y2; y++) {
            i32 t = y - y1;
            i32 xL = x1        + ((x2 - x1)        * t) / dy_lo;
            i32 xR = mid_edge_x + ((x2 - mid_edge_x) * t) / dy_lo;
            i32 zL = z1        + ((z2 - z1)        * t) / dy_lo;
            i32 zR = mid_edge_z + ((z2 - mid_edge_z) * t) / dy_lo;
            draw_scanline(fb, fb_w, fb_h, y, xL, xR, zL, zR,
                          colour, l_bmap, z_bmap);
        }
    }
}

void model_render(const model_t *m, system_t *sys,
                  i32 mx, i32 my, i32 scale, u32 tick,
                  const bmap_t *l_bmap, const bmap_t *z_bmap) {
    (void)sys;
    if (!m || !state) return;

    u32 *fb   = state->graphic.front_buffer;
    i32  fb_w = state->graphic.width;
    i32  fb_h = state->graphic.height;
    if (!fb) return;

    /* Rotation on Y axis. Angle in 12-bit trig table units (0..4095 = full
     * period). One rotation every ~4 seconds at 60 fps. */
    i32 angle = (i32)((tick / 4u) & 0xFFF);
    i32 cos_y = cosine_table[angle];                          /* Q14 */
    i32 sin_y = cosine_table[(angle + 1024) & 0xFFF];         /* Q14 */

    /* Project each vertex once. Q16.16 model × Q14 trig → Q30 → shift 14
     * back to Q16 → shift 16 down to integer pixels with `scale`. */
    proj_vertex_t projected[64];
    if (m->vertex_count > (i32)(sizeof(projected)/sizeof(projected[0]))) return;

    for (i32 i = 0; i < m->vertex_count; i++) {
        i32 vx = m->vertices[i].x;
        i32 vy = m->vertices[i].y;
        i32 vz = m->vertices[i].z;
        /* rotY: x' = cos*x + sin*z; z' = -sin*x + cos*z */
        i64 rx = ((i64)cos_y * vx + (i64)sin_y * vz) >> 14;
        i64 rz = (-(i64)sin_y * vx + (i64)cos_y * vz) >> 14;
        i32 ix = (i32)rx;
        i32 iy = vy;
        i32 iz = (i32)rz;

        /* Orthographic project: screen = center + (model * scale / Q). */
        projected[i].sx = mx + (i32)((i64)ix * scale >> 16);
        projected[i].sy = my - (i32)((i64)iy * scale >> 16);  /* Y up → screen down */
        /* Depth: map z from [-Q, +Q] into [0, 255]. Nearer = smaller u8. */
        i32 d = 128 - ((i32)((i64)iz * 127 >> 16));
        if (d < 0)   d = 0;
        if (d > 255) d = 255;
        projected[i].depth = d;
    }

    /* Solid triangle fill with backface culling. Winding: model faces are
     * CCW in model space. Our projection flips Y (screen down), which
     * inverts winding, so a front face is CCW in screen space and its
     * signed area (cross_z / 2) is NEGATIVE. Trace: face {4,5,6} at
     * angle=0, scale=80, centre=(320,240) →
     *   a=(240,320) b=(400,320) c=(400,160)
     *   cross_z = (400-240)(160-320) - (320-320)(400-240) = -25600
     * so keep cross_z < 0 and cull cross_z >= 0 as back faces. */
    for (i32 f = 0; f < m->face_count; f++) {
        const model_face_t *face = &m->faces[f];
        const proj_vertex_t *a = &projected[face->i0];
        const proj_vertex_t *b = &projected[face->i1];
        const proj_vertex_t *c = &projected[face->i2];

        i64 cross_z = (i64)(b->sx - a->sx) * (i64)(c->sy - a->sy)
                    - (i64)(b->sy - a->sy) * (i64)(c->sx - a->sx);
        if (cross_z >= 0) continue;   /* back face — skip */

        fill_triangle(fb, fb_w, fb_h,
                      a->sx, a->sy, a->depth,
                      b->sx, b->sy, b->depth,
                      c->sx, c->sy, c->depth,
                      face->colour, l_bmap, z_bmap);
    }
}
