/*
 * model.c — placeholder 3D model + wireframe renderer.
 * See model.h for the module contract.
 *
 * All math is Q16.16 fixed-point using the existing cosine_table
 * (Q14, 4096 entries) from libsys/graphics/shading.h.
 */

#include <stdlib.h>
#include "libsys/common.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/shading.h"
#include "state.h"
#include "model_ome.h"
#include "model.h"


/* -------------------------------------------------------------------------
 * Test cube: 8 verts, 12 triangles.
 *
 * Model space is a unit cube: coords are Q16.16 in ±0x10000 (±1.0).
 * Coordinate frame: +X right, +Y up, +Z out of the screen.
 * Face indices are wound counter-clockwise when viewed from outside.
 * ------------------------------------------------------------------------- */

#define ONE  0x10000   /* 1.0 in Q16.16 */

i32 model_default_pitch = 300;

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
                  i32 world_z, i32 world_size, i32 z_far,
                  const bmap_t *l_bmap, const bmap_t *z_bmap,
                  i32 rx_override, i32 ry_override, i32 rz_override,
                  i32 atom_tx, i32 atom_ty, i32 atom_tz) {
    (void)sys;
    if (!m || !state) return;

    u32 *fb   = state->graphic.front_buffer;
    i32  fb_w = state->graphic.width;
    i32  fb_h = state->graphic.height;
    if (!fb) return;

    /* Rotation angles in cosine_table index units (0..4095 = 360°).
     * When rx_override != 0, use explicit per-atom angles from CAN keyframe.
     * Otherwise use tick-driven spin + fixed pitch camera tilt. */
    i32 yaw, pitch, roll;
    if (rx_override) {
        yaw   = ry_override & 0xFFF;   /* Y rotation = yaw */
        pitch = rx_override & 0xFFF;   /* X rotation = pitch */
        roll  = rz_override & 0xFFF;   /* Z rotation = roll */
    } else {
        yaw   = (i32)((tick / 4u) & 0xFFF);
        pitch = model_default_pitch;    /* from L%02d.INF camera projection */
        roll  = 0;
    }
    (void)roll; /* roll not yet applied in projection */

    i32 cy = cosine_table[yaw];
    i32 sy = cosine_table[(yaw   + 1024) & 0xFFF];
    i32 cp = cosine_table[pitch];
    i32 sp = cosine_table[(pitch + 1024) & 0xFFF];

    /* Apply camera rotation to the atom translation, then project to
     * a screen offset and adjust the render centre.
     * Matches the vertex-transform path: rotY → rotX → perspective.
     * All values are Q14 fixed-point from cosine_table. */
    if (atom_tx | atom_ty | atom_tz) {
        i64 rx = ((i64)cy * atom_tx + (i64)sy * atom_tz) >> 14;
        i64 rz = (-(i64)sy * atom_tx + (i64)cy * atom_tz) >> 14;
        i64 ry = atom_ty;
        i64 py = ( (i64)cp * ry - (i64)sp * rz) >> 14;
        i64 pz = ( (i64)sp * ry + (i64)cp * rz) >> 14;
        const i32 CAM_D_local = 4 * ONE;
        i32 denom = CAM_D_local - (i32)pz;
        if (denom < ONE / 8) denom = ONE / 8;
        mx += (i32)(((i64)rx * scale) / denom);
        my -= (i32)(((i64)py * scale) / denom);
    }

    /* Perspective projection.
     * Camera sits at (0, 0, +CAM_D) in Q16.16 model space, looking down -Z.
     * screen_x = mx + (vx * scale) / (CAM_D - vz)
     * screen_y = my - (vy * scale) / (CAM_D - vz)
     * CAM_D chosen so a unit cube fills a comfortable centre area. Larger
     * CAM_D → flatter perspective; smaller → more foreshortening. */
    const i32 CAM_D = 4 * ONE;   /* 4 units back from origin */

    proj_vertex_t projected[64];
    if (m->vertex_count > (i32)(sizeof(projected)/sizeof(projected[0]))) return;

    for (i32 i = 0; i < m->vertex_count; i++) {
        i32 vx = m->vertices[i].x;
        i32 vy = m->vertices[i].y;
        i32 vz = m->vertices[i].z;

        /* rotY (yaw): x' =  cy*x + sy*z ; z' = -sy*x + cy*z */
        i64 rx = ((i64)cy * vx + (i64)sy * vz) >> 14;
        i64 rz = (-(i64)sy * vx + (i64)cy * vz) >> 14;
        i64 ry = vy;

        /* rotX (pitch): y' = cp*y - sp*z ; z' =  sp*y + cp*z */
        i64 py = ( (i64)cp * ry - (i64)sp * rz) >> 14;
        i64 pz = ( (i64)sp * ry + (i64)cp * rz) >> 14;

        i32 mx_v = (i32)rx;
        i32 my_v = (i32)py;
        i32 mz_v = (i32)pz;

        /* Perspective divide. Keep everything Q16.16 while dividing so the
         * projected pixel stays integer at the end. Guard against
         * degenerate depth (should not happen for a unit cube with
         * CAM_D >= 3).  Nearer verts (larger mz_v) → bigger denom
         * reciprocal → bigger projected offset. */
        i32 denom = CAM_D - mz_v;
        if (denom < ONE / 8) denom = ONE / 8;   /* clamp very-near */
        i64 proj_x = ((i64)mx_v * scale) / denom;
        i64 proj_y = ((i64)my_v * scale) / denom;

        projected[i].sx = mx + (i32)proj_x;
        projected[i].sy = my - (i32)proj_y;
        /* Depth: map the vertex into world units, then normalise against
         * the scene's zfar so we can compare directly against the 8-bit
         * Z bmap. World depth = world_z - vz_world (positive mz_v =
         * closer to camera → smaller depth). world_size is the world
         * half-extent that model space's ±ONE covers.
         *
         *   vz_world     = (mz_v * world_size) / ONE
         *   world_depth  = world_z - vz_world
         *   pixel_depth  = clamp(world_depth * 255 / zfar, 0, 255)
         *
         * When z_far <= 0 or world_size <= 0 we degrade to a fixed
         * 128-mid depth so the model still renders (behaves like the
         * previous "Z-off" path). */
        i32 d;
        if (z_far > 0 && world_size > 0) {
            i64 vz_world    = ((i64)mz_v * world_size) >> 16;
            i64 world_depth = (i64)world_z - vz_world;
            i64 pix         = (world_depth * 255) / (i64)z_far;
            if (pix < 0)   pix = 0;
            if (pix > 255) pix = 255;
            d = (i32)pix;
        } else {
            d = 128 - ((i32)((i64)mz_v * 127 >> 16));
            if (d < 0)   d = 0;
            if (d > 255) d = 255;
        }
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

/* -------------------------------------------------------------------------
 * model_render_world — real in-game camera pipeline (dkev sub_420D80).
 * ------------------------------------------------------------------------- */
#define MRW_MAX_VERTS 2048
#define MRW_MAX_FACES 2048
#define MRW_SHADE_MID 32   /* GRD shade band (0..63) used until per-face lighting */

typedef struct { i32 sx, sy; i64 cz; i32 vis; } mrw_vert_t;

void model_render_world(const model_t *m,
                        const i32 *cam_r, const i32 *cam_t, i32 gscale,
                        i32 obj_wx, i32 obj_wy, i32 obj_wz, i32 facing,
                        i32 vshift, i32 flip_y,
                        i32 atom_tx, i32 atom_ty, i32 atom_tz,
                        const bmap_t *l_bmap, const bmap_t *z_bmap,
                        const u8 *pal, const u8 *grd) {
    if (!m || !state) return;
    u32 *fb   = state->graphic.front_buffer;
    i32  fb_w = state->graphic.width;
    i32  fb_h = state->graphic.height;
    if (!fb) return;
    if (m->vertex_count <= 0 || m->vertex_count > MRW_MAX_VERTS) return;
    if (m->face_count   <= 0 || m->face_count   > MRW_MAX_FACES) return;

    i32 cyf = cosine_table[facing & 0xFFF];
    i32 syf = cosine_table[(facing + 1024) & 0xFFF];

    /* Scale bone translation to the same world units as the vertices. OBJC
     * bone coords are raw OME; model_t vertices are OME<<1, so lift the bone
     * to model units (<<1) then apply the same vshift. */
    #define MRW_SCALE(v)  (vshift >= 0 ? ((v) >> vshift) : ((v) << (-vshift)))
    i32 sax = MRW_SCALE(atom_tx << 1);
    i32 say = MRW_SCALE(atom_ty << 1);
    i32 saz = MRW_SCALE(atom_tz << 1);
    if (flip_y) say = -say;

    static mrw_vert_t P[MRW_MAX_VERTS];
    for (i32 i = 0; i < m->vertex_count; i++) {
        /* model_t vertices are OME_raw<<1 (Q16.16-ish). vshift brings them to
         * world units (vshift=1 => OME units == world units 1:1). */
        i32 ox, oy, oz;
        if (vshift >= 0) {
            ox = m->vertices[i].x >> vshift;
            oy = m->vertices[i].y >> vshift;
            oz = m->vertices[i].z >> vshift;
        } else {
            ox = m->vertices[i].x << (-vshift);
            oy = m->vertices[i].y << (-vshift);
            oz = m->vertices[i].z << (-vshift);
        }
        if (flip_y) oy = -oy;

        /* Object facing rotation (yaw about world Y) + per-atom bone offset. */
        i32 rx = (i32)(( (i64)ox * cyf + (i64)oz * syf) >> 14) + sax;
        i32 rz = (i32)((-(i64)ox * syf + (i64)oz * cyf) >> 14) + saz;
        i32 ry = oy + say;

        /* Object -> world. */
        i32 wx = obj_wx + rx, wy = obj_wy + ry, wz = obj_wz + rz;

        /* World -> camera (Q14), R(row,col)=cam_r[row+col*3]; T added直接. */
        i64 cx = (( (i64)wx*cam_r[0] + (i64)wy*cam_r[3] + (i64)wz*cam_r[6] ) >> 14) + cam_t[0];
        i64 cy = (( (i64)wx*cam_r[1] + (i64)wy*cam_r[4] + (i64)wz*cam_r[7] ) >> 14) + cam_t[1];
        i64 cz = (( (i64)wx*cam_r[2] + (i64)wy*cam_r[5] + (i64)wz*cam_r[8] ) >> 14) + cam_t[2];

        i32 vis = (cz < -32768) ? 1 : 0;
        i64 czc = (cz < -32768) ? cz : -32768;
        i32 d = (i32)((-czc) >> 9); if (d < 1) d = 1;
        double s = ((double)gscale / 16384.0) / (double)d;
        P[i].sx = 320 + (i32)(cx * s + (cx < 0 ? -0.5 : 0.5));
        P[i].sy = 240 - (i32)(cy * s + (cy < 0 ? -0.5 : 0.5));
        P[i].cz = cz;
        P[i].vis = vis;
    }

    /* Painter's order: face key = sum of vertex camera depths (cz, negative =
     * far). Draw most-negative first (back-to-front). Simple insertion sort. */
    static i32 order[MRW_MAX_FACES];
    static i64 key[MRW_MAX_FACES];
    i32 nf = 0;
    for (i32 f = 0; f < m->face_count; f++) {
        const model_face_t *face = &m->faces[f];
        key[nf]   = P[face->i0].cz + P[face->i1].cz + P[face->i2].cz;
        order[nf] = f;
        i32 j = nf - 1;
        while (j >= 0 && key[j] > key[j+1]) {           /* ascending: far first */
            i64 tk = key[j]; key[j] = key[j+1]; key[j+1] = tk;
            i32 to = order[j]; order[j] = order[j+1]; order[j+1] = to;
            j--;
        }
        nf++;
    }

    if (getenv("DKE_MESHDBG")) {
        static i32 shown = 0;
        if (shown < 8) {
            shown++;
            i32 minx = 99999, maxx = -99999, miny = 99999, maxy = -99999, nv = 0;
            for (i32 i = 0; i < m->vertex_count; i++) {
                if (P[i].sx < minx) minx = P[i].sx;
                if (P[i].sx > maxx) maxx = P[i].sx;
                if (P[i].sy < miny) miny = P[i].sy;
                if (P[i].sy > maxy) maxy = P[i].sy;
                if (P[i].vis) nv++;
            }
            print("mesh: verts=%d vis=%d faces=%d bbox x[%d..%d] y[%d..%d] (%dx%d px)\n",
                  m->vertex_count, nv, m->face_count, minx, maxx, miny, maxy,
                  maxx - minx, maxy - miny);
        }
    }

    for (i32 oi = 0; oi < nf; oi++) {
        const model_face_t *face = &m->faces[order[oi]];
        const mrw_vert_t *a = &P[face->i0];
        const mrw_vert_t *b = &P[face->i1];
        const mrw_vert_t *c = &P[face->i2];
        if (!a->vis && !b->vis && !c->vis) continue;    /* all behind near plane */

        /* Backface cull via screen winding (Y flipped -> front faces have
         * negative signed area, matching model_render). */
        i64 cross_z = (i64)(b->sx - a->sx) * (i64)(c->sy - a->sy)
                    - (i64)(b->sy - a->sy) * (i64)(c->sx - a->sx);
        if (cross_z >= 0) continue;

        /* Face color: the material base index (MATR field[0]) into the scene
         * palette; the scene light map (l_bmap) shades it per pixel. Optional
         * GRD ramp could pick a shade band from the face normal. Falls back to
         * the raw material RGB when no palette is available. */
        u32 col = face->colour;
        if (pal && face->mat_base) {
            i32 idx = face->mat_base;
            if (grd) idx = grd[MRW_SHADE_MID * 256 + idx];   /* mid shade band */
            col = 0xFF000000u | ((u32)pal[3*idx+2] << 16)
                              | ((u32)pal[3*idx+1] << 8)
                              |  (u32)pal[3*idx+0];
        }

        fill_triangle(fb, fb_w, fb_h,
                      a->sx, a->sy, 128,
                      b->sx, b->sy, 128,
                      c->sx, c->sy, 128,
                      col, l_bmap, z_bmap);
    }
}

/* -------------------------------------------------------------------------
 * model_from_ome_atom — build a render-ready model_t from ome_atom_t.
 * See model.h for contract.
 * ------------------------------------------------------------------------- */

/* Scale raw .OME vertex coords to Q16.16: shift left 1 bit.
 * Max raw value ~30000 → ~60000 ≈ ONE (65536), keeping proportions intact. */
#define OME_VERT_SCALE(v) ((v) << 1)

/* Placeholder colour palette indexed by mat_id & 0x0F. */
static const u32 MAT_COLOURS[16] = {
    RGB_TO_XRGB(180, 160, 140), /* 0  default tan    */
    RGB_TO_XRGB(200, 180, 160), /* 1  skin / light   */
    RGB_TO_XRGB(100,  80,  60), /* 2  dark leather   */
    RGB_TO_XRGB(140, 120, 100), /* 3  leather (CUIR) */
    RGB_TO_XRGB(160, 160, 170), /* 4  steel (ACIER)  */
    RGB_TO_XRGB(110,  90,  70), /* 5  rust  (ROUIL)  */
    RGB_TO_XRGB( 80, 100,  60), /* 6  green cloth    */
    RGB_TO_XRGB(200, 200, 210), /* 7  silver         */
    RGB_TO_XRGB( 60,  60,  80), /* 8  dark metal     */
    RGB_TO_XRGB(220, 200, 180), /* 9  pale skin      */
    RGB_TO_XRGB(120,  80,  40), /* 10 dark leather   */
    RGB_TO_XRGB(170, 140, 110), /* 11 mid leather    */
    RGB_TO_XRGB(190, 170, 150), /* 12 light leather  */
    RGB_TO_XRGB(140, 130, 120), /* 13 grey cloth     */
    RGB_TO_XRGB(100, 110, 130), /* 14 blue-grey      */
    RGB_TO_XRGB(160,  50,  50), /* 15 red accent     */
};

model_t *model_from_ome_atom(const ome_atom_t *atom, const ome_matr_t *matr) {
    if (!atom || atom->vert_count <= 0 || atom->face_count <= 0) return NULL;

    /* Quads become 2 triangles; pre-compute max triangle count. */
    i32 max_tris = 0;
    for (i32 i = 0; i < atom->face_count; i++) {
        max_tris += (atom->faces[i].vcnt == 4) ? 2 : 1;
    }

    model_vec3_t *verts = (model_vec3_t *)malloc(
        (size_t)atom->vert_count * sizeof(model_vec3_t));
    model_face_t *faces = (model_face_t *)malloc(
        (size_t)max_tris * sizeof(model_face_t));
    model_t      *m     = (model_t      *)malloc(sizeof(model_t));

    if (!verts || !faces || !m) {
        free(verts); free(faces); free(m);
        return NULL;
    }

    for (i32 i = 0; i < atom->vert_count; i++) {
        verts[i].x = OME_VERT_SCALE(atom->verts[i].x);
        verts[i].y = OME_VERT_SCALE(atom->verts[i].y);
        verts[i].z = OME_VERT_SCALE(atom->verts[i].z);
    }

    if (getenv("DKE_MATDBG")) {
        static i32 shown = 0;
        if (shown++ < 3) {
            print("matdbg: mat_count=%d\n", matr ? matr->mat_count : -1);
            for (i32 k = 0; matr && k < matr->mat_count && k < 8; k++)
                print("  mat[%d] rgb=(%d,%d,%d) '%s'\n", k,
                      matr->mats[k].r, matr->mats[k].g, matr->mats[k].b,
                      matr->mats[k].name);
            for (i32 k = 0; k < atom->face_count && k < 6; k++)
                print("  face[%d] mat_id=%d\n", k, atom->faces[k].mat_id);
        }
    }

    i32 tri = 0;
    for (i32 i = 0; i < atom->face_count; i++) {
        const ome_face_t *f = &atom->faces[i];
        u32 colour;
        u8  mat_base = 0;
        if (matr && f->mat_id >= 0 && f->mat_id < matr->mat_count) {
            const ome_mat_t *m = &matr->mats[f->mat_id];
            colour = 0xFF000000u | ((u32)m->b << 16) | ((u32)m->g << 8) | (u32)m->r;
            mat_base = m->r;   /* MATR field[0] = base palette index */
        } else {
            i32 mat_idx = (f->mat_id >= 0) ? (f->mat_id & 0x0F) : 0;
            colour = MAT_COLOURS[mat_idx];
        }

        faces[tri].colour   = colour;
        faces[tri].i0       = f->idx[0];
        faces[tri].i1       = f->idx[1];
        faces[tri].i2       = f->idx[2];
        faces[tri].mat_base = mat_base;
        tri++;

        if (f->vcnt == 4) {
            faces[tri].colour   = colour;
            faces[tri].i0       = f->idx[0];
            faces[tri].i1       = f->idx[2];
            faces[tri].i2       = f->idx[3];
            faces[tri].mat_base = mat_base;
            tri++;
        }
    }

    m->vertices     = verts;
    m->vertex_count = atom->vert_count;
    m->faces        = faces;
    m->face_count   = tri;
    return m;
}

void model_free(model_t *m) {
    if (!m) return;
    free((void *)m->vertices);
    free((void *)m->faces);
    free(m);
}
