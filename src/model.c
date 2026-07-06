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

static inline i32 iabs(i32 v) { return v < 0 ? -v : v; }

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
    return l_bmap->pixels[ly * lw + lx];
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
 * Bresenham line with per-pixel depth interpolation + Z-test + L modulation.
 */
static void draw_line(u32 *fb, i32 fb_w, i32 fb_h,
                      i32 x0, i32 y0, i32 z0,
                      i32 x1, i32 y1, i32 z1,
                      u32 colour,
                      const bmap_t *l_bmap, const bmap_t *z_bmap) {
    i32 dx =  iabs(x1 - x0);
    i32 dy = -iabs(y1 - y0);
    i32 sx = x0 < x1 ? 1 : -1;
    i32 sy = y0 < y1 ? 1 : -1;
    i32 err = dx + dy;
    i32 steps = (dx > -dy ? dx : -dy);
    if (steps <= 0) steps = 1;
    i32 dz  = z1 - z0;
    i32 acc = 0;

    for (;;) {
        i32 depth = z0 + (dz * acc) / steps;
        plot_pixel(fb, fb_w, fb_h, x0, y0, depth, colour, l_bmap, z_bmap);
        if (x0 == x1 && y0 == y1) break;
        i32 e2 = err * 2;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
        acc++;
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

    /* Wireframe: 3 edges per face. We duplicate edges across shared triangles
     * — acceptable for a placeholder cube. Triangle fill lands next pass. */
    for (i32 f = 0; f < m->face_count; f++) {
        const model_face_t *face = &m->faces[f];
        const proj_vertex_t *a = &projected[face->i0];
        const proj_vertex_t *b = &projected[face->i1];
        const proj_vertex_t *c = &projected[face->i2];
        draw_line(fb, fb_w, fb_h,
                  a->sx, a->sy, a->depth,
                  b->sx, b->sy, b->depth,
                  face->colour, l_bmap, z_bmap);
        draw_line(fb, fb_w, fb_h,
                  b->sx, b->sy, b->depth,
                  c->sx, c->sy, c->depth,
                  face->colour, l_bmap, z_bmap);
        draw_line(fb, fb_w, fb_h,
                  c->sx, c->sy, c->depth,
                  a->sx, a->sy, a->depth,
                  face->colour, l_bmap, z_bmap);
    }
}
