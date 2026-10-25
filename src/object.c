/*
 * object.c — level props and items (see object.h).
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "libsys/graphics/cif.h"
#include "state.h"
#include "object.h"

static game_object_t g_obj[OBJ_SLOTS];          /* unk_4DAB60 */
static cif_image_t   g_atlas;                   /* dword_4FB1DC */
static i32           g_atlas_tried = 0;

/* The running AnimObjet (one at a time, globally). */
static struct {
    game_object_t *o;
    obc_node_t    *node;
    i32 axis, delta, ease, ms;
    i32 base;
    u32 start;
    i32 done;        /* finished, waiting for the action to collect it */
} g_anim;

static void atlas_load(void) {
    if (g_atlas_tried) return;
    g_atlas_tried = 1;
    c8 host[512];
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (ls_resolve_path(base, "OBJETS>OBJETS.CIF", host, (i32)sizeof(host)) &&
        cif_load(&g_atlas, host) && g_atlas.width == 256)
        ome_set_objets_atlas(g_atlas.pixels, g_atlas.height);
    else
        print("object: shared atlas OBJETS.CIF not loaded\n");
}

void object_reset(void) {
    for (i32 i = 0; i < OBJ_SLOTS; i++) {
        game_object_t *o = &g_obj[i];
        if (o->root) obc_free(o->root);
        ome_matr_free(&o->matr);
    }
    memset(g_obj, 0, sizeof(g_obj));
    memset(&g_anim, 0, sizeof(g_anim));
}

game_object_t *object_slot(i32 i) {
    return (i >= 0 && i < OBJ_SLOTS && g_obj[i].used) ? &g_obj[i] : NULL;
}

const void *object_poly(i32 i) {
    game_object_t *o = object_slot(i);
    return (o && o->has_poly && !(o->flags & OBJ_F_FREE)) ? o->poly : NULL;
}

i32 object_animating(const game_object_t *o) {
    return o && g_anim.o == o && !g_anim.done;
}

/* get_entity_character @ 0x475B15: match on the first 4 characters. */
static game_object_t *obj_lookup(const c8 *name) {
    for (i32 i = 0; i < OBJ_SLOTS; i++)
        if (g_obj[i].used && strncmp(g_obj[i].name, name, 4) == 0) return &g_obj[i];
    return NULL;
}

/* sub_475BB6 + sub_412979 / sub_472C5C: find or load. */
static game_object_t *obj_get(const c8 *name) {
    game_object_t *o = obj_lookup(name);
    if (o) return o;
    for (i32 i = 0; i < OBJ_SLOTS && !o; i++) if (!g_obj[i].used) o = &g_obj[i];
    if (!o) { print("object: no free slot for %s\n", name); return NULL; }
    memset(o, 0, sizeof(*o));
    snprintf(o->name, sizeof(o->name), "%s", name);
    o->is_prop = (name[0] == 'I' && name[1] == 'M');

    c8 vpath[64];
    if (o->is_prop)
        snprintf(vpath, sizeof(vpath), "DATAS>L%02d>%.4s.OME",
                 state ? state->current_level_id : 1, name);
    else
        snprintf(vpath, sizeof(vpath), "OBJETS>OB%.2s.OME", name + 2);
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    atlas_load();
    o->root = obc_load_ome(vpath, base);
    if (!o->root) { print("object: cannot load %s\n", vpath); return NULL; }
    c8 host[512];
    ome_file_t ome = {0};
    if (ls_resolve_path(base, vpath, host, (i32)sizeof(host)) && model_ome_open(host, &ome)) {
        ome_load_matr(ome.raw, ome.raw_size, &o->matr);
        model_ome_free(&ome);
    }
    o->used = 1;
    o->flags = 0;
    print("object: loaded %s (%s)\n", name, vpath);
    return o;
}

/* --- collision polygon (sol_limit layout) ---------------------------- */

#define P32(o, off) (*(i32 *)((o)->poly + (off)))

static void poly_finish(game_object_t *o, i32 n) {
    P32(o, 0x00) = 1;                          /* solid */
    P32(o, 0x1D4) = 1;
    P32(o, 0x1D8) = n;
    i32 minx = 0x7FFFFFFF, maxx = -0x7FFFFFFF, minz = 0x7FFFFFFF, maxz = -0x7FFFFFFF;
    for (i32 i = 0; i < n; i++) {
        i32 x = P32(o, 0x14 + i * 12), z = P32(o, 0x14 + i * 12 + 8);
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (z < minz) minz = z;
        if (z > maxz) maxz = z;
        /* Edge direction (Q10 unit) and length, as the level files store. */
        i32 j = (i + 1) % n;
        f64 dx = P32(o, 0x14 + j * 12) - x, dz = P32(o, 0x14 + j * 12 + 8) - z;
        f64 len = sqrt(dx * dx + dz * dz);
        P32(o, 0xD4 + i * 12)     = len > 0 ? (i32)(dx / len * 1024.0) : 0;
        P32(o, 0xD4 + i * 12 + 4) = 0;
        P32(o, 0xD4 + i * 12 + 8) = len > 0 ? (i32)(dz / len * 1024.0) : 0;
        P32(o, 0x194 + i * 4)     = (i32)len;
    }
    P32(o, 0x1DC) = minx; P32(o, 0x1E8) = maxx;
    P32(o, 0x1E4) = minz; P32(o, 0x1F0) = maxz;
    P32(o, 0x1F4) = (minx + maxx) / 2;
    P32(o, 0x1FC) = (minz + maxz) / 2;
    o->has_poly = 1;
}

/* sub_4771D1: the footprint turned by `yaw` about the object's pivot. */
static void poly_rotate(game_object_t *o, i32 yaw) {
    f64 a = (f64)(yaw & 0xFFFF) * 3.14159265358979 / 32768.0 * o->yaw_sign;
    f64 c = cos(a), s = sin(a);
    for (i32 i = 0; i < 4; i++) {
        f64 x = o->foot[i][0], z = o->foot[i][1];
        P32(o, 0x14 + i * 12)     = o->pos[0] + (i32)(x * c - z * s);
        P32(o, 0x14 + i * 12 + 4) = o->pos[1];
        P32(o, 0x14 + i * 12 + 8) = o->pos[2] + (i32)(x * s + z * c);
    }
    poly_finish(o, 4);
}

/* sub_47701F: the AABB footprint (taken with no yaw), then turned. The
 * turn direction is matched once against the mesh's own bounds. */
static void poly_build(game_object_t *o) {
    obc_node_t *r = o->root;
    i32 yaw = r->rot[2];
    i32 mn[3], mx[3];
    r->rot[2] = 0;
    obc_node_bounds(r, mn, mx);
    o->foot[0][0] = mn[0]; o->foot[0][1] = mn[2];
    o->foot[1][0] = mx[0]; o->foot[1][1] = mn[2];
    o->foot[2][0] = mx[0]; o->foot[2][1] = mx[2];
    o->foot[3][0] = mn[0]; o->foot[3][1] = mx[2];
    if (!o->yaw_sign) {
        i32 qn[3], qx[3];
        r->rot[2] = 0x4000;
        obc_node_bounds(r, qn, qx);
        /* A +90 degree turn with sign +1 maps x -> z. */
        f64 ep = fabs((f64)qn[2] - mn[0]) + fabs((f64)qx[2] - mx[0]);
        f64 em = fabs((f64)qn[2] + mx[0]) + fabs((f64)qx[2] + mn[0]);
        o->yaw_sign = (ep <= em) ? 1 : -1;
    }
    r->rot[2] = yaw;
    poly_rotate(o, yaw);
    if (getenv("DKE_OBJDBG")) print("object: %s footprint x[%d..%d] z[%d..%d] yaw 0x%X sign %d -> poly (%d,%d) (%d,%d) (%d,%d) (%d,%d)\n",
          o->name, mn[0], mx[0], mn[2], mx[2], yaw & 0xFFFF, o->yaw_sign,
          P32(o, 0x14), P32(o, 0x1C), P32(o, 0x20), P32(o, 0x28),
          P32(o, 0x2C), P32(o, 0x34), P32(o, 0x38), P32(o, 0x40));
}

/* --- EtatObjet --------------------------------------------------------- */

static void place(game_object_t *o, i32 x, i32 y, i32 z, i32 r0, i32 r1, i32 r2) {   /* sub_475F7A */
    o->pos[0] = x; o->pos[1] = y; o->pos[2] = z;
    o->rot[0] = r0; o->rot[1] = r1; o->rot[2] = r2;
    o->root->pos[0] = x; o->root->pos[1] = y; o->root->pos[2] = z;
    o->root->rot[0] = r0; o->root->rot[1] = r1; o->root->rot[2] = r2;
    if (o->is_prop) {
        poly_build(o);
        o->flags |= OBJ_F_PLACED | OBJ_F_CLOSED;               /* sub_412ACF */
    }
}

void object_etat(const c8 *name, i32 x, i32 y, i32 z,
                 i32 r0, i32 r1, i32 r2, i32 a6, i32 a7) {      /* case 6 */
    if (!name || !name[0]) return;
    c8 nm[16];
    snprintf(nm, sizeof(nm), "%s", name[0] == 'S' ? name + 1 : name);
    game_object_t *o = (a6 <= -3) ? obj_lookup(nm) : obj_get(nm);
    if (!o) return;
    if (a6 >= -1) o->mode = a7;
    if (o->is_prop) {
        place(o, x, y, z, r0, r1, r2);
        return;
    }
    i32 v = nm[4] ? nm[4] - 'A' : 0;
    if (v < 0 || v >= OBJ_VARIANTS) v = 0;
    switch (a6) {
    case -1: case 0:                                            /* place */
        o->var_on[v] = 1;
        o->var_pos[v][0] = x; o->var_pos[v][1] = y; o->var_pos[v][2] = z;
        o->var_rot[v][0] = r0; o->var_rot[v][1] = r1; o->var_rot[v][2] = r2;
        place(o, x, y, z, r0, r1, r2);
        o->flags |= OBJ_F_PLACED;
        break;
    case -6:                                                    /* sub_472B47 */
        o->var_on[v] = 0;
        break;
    case 1: case 2:
        break;
    default:
        print("object: EtatObjet %s state %d (inventory) not ported\n", nm, a6);
        break;
    }
}

void object_colobjet(const c8 *name, i32 x, i32 z, i32 w, i32 d) {   /* case 7 */
    game_object_t *o = name ? obj_lookup(name[0] == 'S' ? name + 1 : name) : NULL;
    if (!o) return;
    if (!x && !z && !w && !d) { o->has_poly = 0; memset(o->poly, 0, sizeof(o->poly)); return; }
    i32 fy = o->pos[1];
    i32 c[4][2] = { { x - w / 2, z - d / 2 }, { x + w / 2, z - d / 2 },
                    { x + w / 2, z + d / 2 }, { x - w / 2, z + d / 2 } };
    for (i32 i = 0; i < 4; i++) {
        P32(o, 0x14 + i * 12) = c[i][0];
        P32(o, 0x14 + i * 12 + 4) = fy;
        P32(o, 0x14 + i * 12 + 8) = c[i][1];
    }
    o->rot[2] = 0;
    poly_finish(o, 4);
}

/* --- AnimObjet (case 0x13, per frame sub_412B94) ------------------------- */

i32 object_anim(const c8 *name, i32 axis, i32 degrees, i32 ease, i32 ms, u32 now_ms) {
    game_object_t *o = name ? obj_lookup(name) : NULL;
    if (!o || !o->root) return 1;
    if (g_anim.o) {
        if (g_anim.o == o && g_anim.done) { memset(&g_anim, 0, sizeof(g_anim)); return 1; }
        return 0;                                    /* another one is running */
    }
    i32 ax = abs(axis);
    if (ax < 1 || ax > 3) return 1;
    g_anim.o = o;
    g_anim.node = o->root->child ? o->root->child : o->root;
    g_anim.axis = ax;
    g_anim.delta = degrees * 182;                    /* degrees -> BAM */
    g_anim.ease = ease;
    g_anim.ms = ms > 0 ? ms : 1;
    g_anim.base = g_anim.node->rot[ax - 1];          /* dword_4CFE24 */
    g_anim.start = now_ms;
    g_anim.done = 0;
    if (getenv("DKE_OBJDBG")) print("object: AnimObjet %s axis %d %+d deg over %d ms\n", o->name, axis, degrees, ms);
    return 0;
}

void object_tick(u32 now_ms) {
    if (!g_anim.o || g_anim.done) return;
    f64 t = (f64)(now_ms - g_anim.start) / g_anim.ms;
    i32 ang;
    if (t >= 1.0) {
        ang = g_anim.base + g_anim.delta;
    } else {
        f64 k = g_anim.ease ? pow(t, g_anim.ease / 256.0) : t;
        ang = g_anim.base + (i32)(g_anim.delta * k);
    }
    g_anim.node->rot[g_anim.axis - 1] = ang & 0xFFFF;
    game_object_t *o = g_anim.o;
    if (!o->root->child && g_anim.axis == 3) {
        o->rot[2] = ang & 0xFFFF;
        if (o->has_poly && o->is_prop) poly_rotate(o, o->rot[2]);
    }
    if (t >= 1.0) {
        /* Toggle open / closed. */
        if (o->flags & OBJ_F_CLOSED) { o->flags &= ~OBJ_F_CLOSED; o->flags |= OBJ_F_OPEN; }
        else                         { o->flags &= ~OBJ_F_OPEN;   o->flags |= OBJ_F_CLOSED; }
        g_anim.done = 1;
    }
}

i32 object_pos_xz(const c8 *name, i32 *x, i32 *z) {
    game_object_t *o = name ? obj_lookup(name) : NULL;
    if (!o) return 0;
    if (o->has_poly) {
        *x = P32(o, 0x1F4);
        *z = P32(o, 0x1FC);
    } else {
        *x = o->pos[0];
        *z = o->pos[2];
    }
    return 1;
}

void object_world_remove(const c8 *name) {                     /* sub_472B47 */
    game_object_t *o = name ? obj_lookup(name) : NULL;
    if (!o || o->is_prop) return;
    i32 v = name[4] ? name[4] - 'A' : 0;
    if (v >= 0 && v < OBJ_VARIANTS) o->var_on[v] = 0;
}
