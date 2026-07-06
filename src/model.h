#pragma once

/*
 * model.h — Placeholder 3D model + software renderer (Slice F pass 1)
 *
 * We can't parse .OME (Object Mesh Editor, "3DITOR v1.3" format) yet — the
 * file layout is nested/chunked with material tags (CUIR, ACIER, ROUIL, ...)
 * and needs its own reverse-engineering slice. This module ships a hard-coded
 * cube instead so the render pipeline (rotation → projection → per-pixel Z
 * depth-test against the scene's Z bmap → L light-map shading) can land and
 * be tested independently of the .OME loader.
 *
 * Once model_load_ome exists, it will populate the same model_t struct, so
 * the renderer stays unchanged.
 */

#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/graphics/graphic.h"

typedef struct {
    i32 x, y, z;   /* Q16.16 fixed point in model space (-1..1 ≈ ±0x10000) */
} model_vec3_t;

typedef struct {
    /* Face colour as pre-multiplied XRGB (already 0xFFRRGGBB packed to
     * match blit_bmap_to_framebuffer byte order: byte0=R, byte1=G,
     * byte2=B, byte3=alpha). Renderer modulates by L before writing. */
    u32 colour;
    /* Indices into vertices[]. */
    u8  i0, i1, i2, _pad;
} model_face_t;

typedef struct {
    const model_vec3_t *vertices;
    i32                 vertex_count;
    const model_face_t *faces;
    i32                 face_count;
} model_t;

/*
 * model_test_cube — a unit cube centered at origin, 8 vertices + 12 faces.
 * Face colours picked to make orientation obvious (red/green/blue axes).
 */
const model_t *model_test_cube(void);

/*
 * model_render — draw a model into the front buffer.
 *
 * Rotation is applied on the Y axis using tick-driven angle (spins the cube
 * so the render is visibly alive).  Vertices are projected orthographically
 * for this pass: (mx, my) is the screen-space centre where the model lands,
 * scale converts model units (Q16.16) into pixels.
 *
 * For each visible pixel, samples the scene depth (z_bmap, 640×480 u8) and
 * only writes when the model's projected depth is closer (smaller u8) than
 * the background depth — that gives the 2.5D "characters go behind the
 * console" effect Dark Earth uses.  Samples the light map (l_bmap, 320×240
 * u8 grayscale) and modulates the face colour so the model picks up scene
 * lighting.
 *
 * Wireframe pass 1: draws each face as three Bresenham line segments.
 * Triangle-fill rasteriser lands in pass 2.
 */
void model_render(const model_t   *m,
                  system_t        *sys,
                  i32              mx,        /* screen centre X */
                  i32              my,        /* screen centre Y */
                  i32              scale,     /* focal length in pixels */
                  u32              tick,      /* animation clock ms */
                  i32              world_z,   /* model centre world depth (units) */
                  i32              world_size,/* model half-extent world units */
                  i32              z_far,     /* scene camera zfar (units) */
                  const bmap_t    *l_bmap,    /* optional light map, may be NULL */
                  const bmap_t    *z_bmap);   /* optional depth bmap, may be NULL */
