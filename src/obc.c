/*
 * obc.c — 3DITOR object hierarchy (.OBC) + atom geometry (.ATM) loader.
 *
 * See obc.h for the full list of asm references. Every routine below is a
 * direct transcription of the matching dkev.exe procedure; the original
 * address is quoted at each definition and at the non-obvious steps.
 *
 * Two things the original does that the port deliberately drops:
 *
 *   1. The ".COB" pre-parsed disk cache (sub_4538FB @ 0x4538FB writes it and
 *      sub_4232C3 reads it back). It only exists to avoid re-parsing from CD
 *      and is guarded by is_using_hdd; the port always parses the source.
 *   2. Per-face texture resolution. sub_453346 @ 0x453346 turns the face
 *      material id at +0x24 into a material pointer via sub_450BEB @ 0x450BEB
 *      and then biases the four UV pairs by that material's atlas rect. That
 *      lookup goes through the MAT "reverse LUT" packed into the high bytes of
 *      mat_lib_t::name, which mat_load() does not populate yet (it stops after
 *      the bitmap table). Faces therefore keep their raw material id and their
 *      unbiased UVs — see atm_fixup_faces().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "libsys/graphics/shading.h"
#include "obc.h"

/* =========================================================================
 * Fixed-point transform primitives
 * ====================================================================== */

/*
 * The original indexes a 4096-entry i16 table it calls "sine_table" which in
 * fact holds cos(i * 2*PI/4096) * 16384 — the port already builds exactly
 * that table as cosine_table (preload_shading_table @ 0x48F190).
 *
 * Angles are 16-bit BAM. The two index forms below are the ones the asm uses:
 *   "+ 8"      rounds to the nearest table slot   -> cos(angle)
 *   "- 0x3FF8" is the same rounding minus 90 deg  -> sin(angle)
 */
static inline i32 trig_cos(i32 a) { /* movsx sine_table[((a+8)&0xFFF0)>>3] */
    return cosine_table[(((u32)a + 8u) & 0xFFF0u) >> 4];
}
static inline i32 trig_sin(i32 a) { /* movsx sine_table[((a-0x3FF8)&0xFFF0)>>3] */
    return cosine_table[(((u32)a - 0x3FF8u) & 0xFFF0u) >> 4];
}

void fmat_rotation(fmat_t *out, i32 ax, i32 ay, i32 az) { /* 0x48FA20 */
    /* asm registers: var_4 = ca, var_8 = sa, esi = cb, edi = sb,
     *                edx  = cc, ebx  = sc                              */
    const i64 ca = trig_cos(ax & 0xFFFF), sa = trig_sin(ax & 0xFFFF);
    const i64 cb = trig_cos(ay & 0xFFFF), sb = trig_sin(ay & 0xFFFF);
    const i64 cc = trig_cos(az & 0xFFFF), sc = trig_sin(az & 0xFFFF);

    /* Intermediates, each already reduced back to Q14 by the asm. */
    const i64 cb_ca = (cb * ca) >> 14;   /* 48FB01 */
    const i64 sb_sa = (sb * sa) >> 14;   /* 48FB04 */
    const i64 cb_sa = (cb * sa) >> 14;   /* 48FB0C */
    const i64 sb_ca = (sb * ca) >> 14;   /* 48FB1C */

    out->m[0] = (i32)((cc * cb) >> 14);              /* [ecx+0x00] */
    out->m[1] = (i32)(((sc * cb_ca) >> 14) - sb_sa); /* [ecx+0x04] */
    out->m[2] = (i32)(((sc * cb_sa) >> 14) + sb_ca); /* [ecx+0x08] */
    out->m[3] = (i32)(-sc);                          /* [ecx+0x0C] */
    out->m[4] = (i32)((cc * ca) >> 14);              /* [ecx+0x10] */
    out->m[5] = (i32)((cc * sa) >> 14);              /* [ecx+0x14] */
    /* asm negates before the shift on these two — keep the same rounding. */
    out->m[6] = (i32)((-(cc * sb)) >> 14);           /* [ecx+0x18] */
    out->m[7] = (i32)(-(((sc * sb_ca) >> 14) + cb_sa)); /* [ecx+0x1C] */
    out->m[8] = (i32)(cb_ca - ((sc * sb_sa) >> 14)); /* [ecx+0x20] */

    out->t[0] = 0;
    out->t[1] = 0;
    out->t[2] = 0;
}

void fmat_position(fmat_t *out, i32 x, i32 y, i32 z) { /* 0x48FD60 */
    memset(out, 0, sizeof(*out));
    out->t[0] = x;
    out->t[1] = y;
    out->t[2] = z;
    /* asm: three stores at stride 0x10 -> m[0], m[4], m[8] = 1 */
    out->m[0] = 1;
    out->m[4] = 1;
    out->m[8] = 1;
}

void fmat_rotate_translation(const fmat_t *a, fmat_t *b) { /* 0x4903C0 */
    /* The asm splits each translation component into low/high 16-bit halves
     * purely to keep the products in 32 bits; a 64-bit multiply is the same
     * value, including the 0x2000 rounding bias. */
    const i64 t0 = b->t[0], t1 = b->t[1], t2 = b->t[2];
    i32 out[3];
    for (i32 j = 0; j < 3; j++) {
        const i64 sum = (i64)a->m[0 * 3 + j] * t0
                      + (i64)a->m[1 * 3 + j] * t1
                      + (i64)a->m[2 * 3 + j] * t2;
        out[j] = (i32)((sum + 0x2000) >> 14);
    }
    b->t[0] = out[0];
    b->t[1] = out[1];
    b->t[2] = out[2];
}

void fmat_premul_rotation(const fmat_t *r, fmat_t *m) { /* 0x4901E0 */
    /* asm copies m into a 12-dword local first, then writes back into m. */
    const fmat_t src = *m;
    for (i32 i = 0; i < 3; i++) {
        for (i32 j = 0; j < 3; j++) {
            const i64 sum = (i64)r->m[i * 3 + 0] * src.m[0 * 3 + j]
                          + (i64)r->m[i * 3 + 1] * src.m[1 * 3 + j]
                          + (i64)r->m[i * 3 + 2] * src.m[2 * 3 + j];
            m->m[i * 3 + j] = (i32)((sum + 0x2000) >> 14);
        }
    }
    /* Translation is deliberately untouched (the caller adds it separately). */
}

void fmat_transform_point(const fmat_t *m, i32 *v) { /* 0x48F424 */
    const i64 v0 = v[0], v1 = v[1], v2 = v[2];
    for (i32 j = 0; j < 3; j++) {
        /* shrd without a rounding bias, unlike the two routines above. */
        const i64 sum = v0 * m->m[0 * 3 + j]
                      + v1 * m->m[1 * 3 + j]
                      + v2 * m->m[2 * 3 + j];
        v[j] = (i32)(sum >> 14) + m->t[j];
    }
}

/* =========================================================================
 * Record reader — sub_452EA6 @ 0x452EA6
 *
 * Everything in a 3DITOR file is a length-prefixed record. The original is a
 * single routine with a 4-way switch on the record "kind":
 *   0 = face list, 1 = vertex list, 2 = hierarchy node, 3 = atom header.
 * Kinds 2 and 3 are single records with the length stored inline; kinds 0 and
 * 1 are bulk arrays whose element size comes from the file header.
 * ====================================================================== */

typedef struct {
    const u8 *base;
    const u8 *p;
    const u8 *end;
} cursor_t;

static i32 cur_left(const cursor_t *c) { return (i32)(c->end - c->p); }

/*
 * Single length-prefixed record (asm path at 0x452F6F with edx == 0).
 * Layout: [i32 size][size - 4 payload bytes]. The size field is read as a
 * BYTE ("mov cl, [eax]" @ 0x452F99), so records cannot exceed 255 bytes.
 * Returns the payload length, or -1 when the record runs past the file.
 */
static i32 read_record(cursor_t *c, u8 *buf, i32 buf_size) {
    if (cur_left(c) < 4) return -1;
    const i32 size = c->p[0];              /* byte, not dword — see above */
    const i32 payload = size - 4;
    c->p += 4;
    if (payload < 0 || payload > buf_size || cur_left(c) < payload) return -1;
    memcpy(buf, c->p, (size_t)payload);
    c->p += payload;
    return payload;
}

/*
 * Bulk array (asm path at 0x452EC8, edx = on-disk element size).
 * `count` elements of `elem_size` bytes are consumed from the cursor and the
 * first `copy_size` bytes of each are written into `dst` at `copy_size`
 * stride. Returns 0 when the array runs past the file.
 */
static i32 read_array(cursor_t *c, void *dst, i32 count,
                      i32 elem_size, i32 copy_size) {
    const i32 stride = elem_size & ~3;     /* asm: and eax, 0FFFFFFFCh */
    if (count <= 0 || stride < copy_size) return 0;

    i32 avail = cur_left(c) / stride;
    if (avail <= 0) return 0;
    if (avail > count) avail = count;

    u8 *out = (u8 *)dst;
    for (i32 i = 0; i < avail; i++) {
        memcpy(out, c->p + (size_t)i * stride, (size_t)copy_size);
        out += copy_size;
    }
    c->p += (size_t)avail * stride;
    return avail == count;
}

/* Read an i32 from a record payload addressed by its ON-DISK offset. */
static i32 rec_i32(const u8 *payload, i32 disk_off) {
    i32 v;
    memcpy(&v, payload + (disk_off - 4), sizeof(v));
    return v;
}

/* =========================================================================
 * ATM atom loader — sub_453346 @ 0x453346
 * ====================================================================== */

/* Traversal stack depth. The original's get_node_bounds frame allows exactly
 * 16 levels (76 bytes per level between var_4D0 and var_10); sub_436C56 uses
 * the same shape. */
#define OBC_BOUNDS_STACK 16

void (*atm_render_free)(void *render) = NULL;

static void atom_free(atm_atom_t *a) {
    if (!a) return;
    if (a->render && atm_render_free) atm_render_free(a->render);
    free(a->verts);
    free(a->faces);
    free(a);
}

static void free_atom_slot(atm_atom_t **slot) {
    atom_free(*slot);
    *slot = NULL;
}

static void atm_fixup_faces(atm_atom_t *atom) {
    /*
     * Original loop @ 0x453629..0x4538DC. For every face it:
     *   - resolves +0x24 (material id) to a material pointer via sub_450BEB,
     *     or 0 when the id is -1;
     *   - if the resolution failed but the "textured" bits (0x0C) are set,
     *     clears bit 3 so the rasteriser falls back to flat shading;
     *   - otherwise biases the four UV pairs at +0x3C..+0x58 by the material's
     *     atlas rect and pulls any u/v that landed exactly on 0x0100_0000 back
     *     by one texel.
     *
     * The material table needed for step one is not decoded yet (see the file
     * header comment), so the port performs only the parts that do not depend
     * on it: normalise "no material" to 0 and drop the textured bit when there
     * is nothing to sample.
     */
    for (i32 f = 0; f < atom->face_count; f++) {
        i32 *face = atom->faces + (size_t)f * ATM_FACE_STRIDE;

        /* The original replaces the id with a material pointer, NULL for
         * -1. The port keeps ids, so -1 stays as the "no material" marker
         * (the rasteriser skips those faces, as it skips NULL materials). */
        if (face[ATM_FACE_MAT] == -1 &&
            (face[ATM_FACE_FLAGS] & 0x0C) == 0x0C)
            face[ATM_FACE_FLAGS] &= ~0x08; /* 0x4536A9 */
    }
    atom->flags &= ~0x400;                 /* 0x4538DC */
}

/*
 * atm_parse_chunk — the body of sub_453346 @ 0x453346, operating on a chunk
 * already in memory. A standalone .ATM file is one such chunk at offset 0;
 * inside a .OME the same chunk layout is repeated once per atom.
 */
static atm_atom_t *atm_parse_chunk(const u8 *base, i32 size, i32 chunk_off,
                                   const c8 *dbg) {
    if (chunk_off < 0 || size - chunk_off <= 0x30) return NULL;

    cursor_t c = { base, base + chunk_off, base + size };

    /* 0x45342C — 0x30-byte header. The version digits are overwritten with
     * "----" before the compare so v1.x and v2.x both pass. */
    u8 hdr[0x30];
    memcpy(hdr, c.p, sizeof(hdr));
    c.p += sizeof(hdr);
    memcpy(hdr + 7, "----", 4);
    if (strncmp((const c8 *)hdr, "3DITOR ---- ATOM", 16) != 0) {
        print("atm: bad atom header in '%s'\n", dbg);
        return NULL;
    }

    /* Disk element sizes, patched into the path field by the exporter. */
    i32 disk_face_size, disk_vert_size;
    memcpy(&disk_face_size, hdr + 0x20, 4);   /* var_40 @ 0x453487 */
    memcpy(&disk_vert_size, hdr + 0x24, 4);   /* var_3C @ 0x45348D */

    atm_atom_t *atom = (atm_atom_t *)calloc(1, sizeof(*atom));
    if (!atom) return NULL;

    /* 0x4534CE — kind 3: one 0x40-byte atom header record. */
    u8 rec[0x40];
    if (read_record(&c, rec, (i32)sizeof(rec)) < 0x3C) {
        print("atm: truncated atom record in '%s'\n", dbg);
        free(atom);
        return NULL;
    }
    memcpy(atom->name, rec, 16);              /* +0x04 */
    atom->name[15]   = '\0';
    atom->flags      = rec_i32(rec, 0x14);
    atom->vert_count = rec_i32(rec, 0x1C);
    atom->vert_max   = rec_i32(rec, 0x20);
    atom->face_count = rec_i32(rec, 0x24);
    atom->face_max   = rec_i32(rec, 0x28);

    /* 0x4534D3 / 0x4534EF — the allocation counts are the larger of the two. */
    if ((u32)atom->face_count > (u32)atom->face_max) atom->face_max = atom->face_count;
    if ((u32)atom->vert_count > (u32)atom->vert_max) atom->vert_max = atom->vert_count;

    if (atom->vert_count <= 0 || atom->face_count <= 0 ||
        disk_vert_size < 4 || disk_face_size < 4) {
        print("atm: '%s' has no geometry (v=%d f=%d)\n",
              atom->name, atom->vert_count, atom->face_count);
        free(atom);
        return NULL;
    }

    /*
     * 0x453514 — vertices, then 0x453576 — faces.
     *
     * The arrays are allocated for the *max* slot counts (the runtime appends
     * to them; sub_436C56 @ 0x436C56 builds a proxy box atom the same way),
     * but only the live counts are actually stored in the file. The original
     * passes the max as the element count to sub_452EA6 and so reads past the
     * stored data — harmless for a standalone .ATM whose atom is the only
     * thing in the file, but inside a .OME it would run into the next chunk,
     * and for OBJETS/OB58.OME's "armeb" (vert_max 64 vs 12 stored) it would
     * run past EOF. Read the live counts; the surplus slots stay zeroed.
     */
    atom->verts = (i32 *)calloc((size_t)atom->vert_max * ATM_VERT_STRIDE, sizeof(i32));
    atom->faces = (i32 *)calloc((size_t)atom->face_max * ATM_FACE_STRIDE, sizeof(i32));
    if (!atom->verts || !atom->faces) {
        free(atom->verts); free(atom->faces); free(atom);
        return NULL;
    }

    if (!read_array(&c, atom->verts, atom->vert_count, disk_vert_size,
                    ATM_VERT_STRIDE * 4) ||
        !read_array(&c, atom->faces, atom->face_count, disk_face_size,
                    ATM_FACE_STRIDE * 4)) {
        print("atm: '%s' truncated geometry\n", dbg);
        free(atom->verts); free(atom->faces); free(atom);
        return NULL;
    }

    /* asm zeroes face+0x60 for every face as it copies (case 0 @ 0x453083). */
    for (i32 i = 0; i < atom->face_max; i++)
        atom->faces[(size_t)i * ATM_FACE_STRIDE + ATM_FACE_RT] = 0;

    atm_fixup_faces(atom);

    print("atm: '%s' — %d verts, %d faces\n",
          atom->name, atom->vert_count, atom->face_count);
    return atom;
}

/* atm_load — sub_453346 @ 0x453346, file front end. */
static atm_atom_t *atm_load(const c8 *vpath, const c8 *game_path) {
    c8 resolved[512];
    if (!ls_resolve_path(game_path, vpath, resolved, (i32)sizeof(resolved))) {
        print("atm_load: path overflow '%s'\n", vpath);
        return NULL;
    }

    const i32 size = ls_get_filesize(resolved);
    if (size <= 0x30) {
        /* asm: log_message(4, "Cannot open ATM file %s") @ 0x4533CA */
        print("atm_load: cannot open ATM file '%s'\n", resolved);
        return NULL;
    }

    u8 *raw = (u8 *)malloc((size_t)size);
    if (!raw) return NULL;

    FILE *f = ls_open(resolved, "rb");
    if (!f) { free(raw); return NULL; }
    ls_read_buffer(f, raw, size);
    ls_close(f);

    atm_atom_t *atom = atm_parse_chunk(raw, size, 0, resolved);
    free(raw);
    return atom;
}

/*
 * atm_vertex_bounds — sub_436AB0 @ 0x436AB0.
 *
 * Component-wise AABB over an atom's own vertices, in the atom's local space
 * (no transform, unlike get_node_bounds). Seeded from vertex 0 and refined
 * over [1, vert_count).
 *
 * Note the output order is the opposite of get_node_bounds: here the register
 * argument receives the maxima and the stack argument the minima.
 */
static void atm_vertex_bounds(const atm_atom_t *atom, i32 *out_max, i32 *out_min) {
    const i32 *v = atom->verts;

    i32 mn[3] = { v[ATM_VERT_X], v[ATM_VERT_Y], v[ATM_VERT_Z] };
    i32 mx[3] = { mn[0], mn[1], mn[2] };

    for (i32 i = 1; i < atom->vert_count; i++) {
        const i32 *p = atom->verts + (size_t)i * ATM_VERT_STRIDE;
        for (i32 k = 0; k < 3; k++) {
            if (p[k] < mn[k]) mn[k] = p[k];
            if (p[k] > mx[k]) mx[k] = p[k];
        }
    }

    for (i32 k = 0; k < 3; k++) {
        out_min[k] = mn[k];
        out_max[k] = mx[k];
    }
}

/*
 * atm_make_box — the box builder inside sub_436C56 @ 0x436CFF..0x437176.
 *
 * Builds an 8-vertex / 6-quad proxy atom spanning [min, max]. The engine
 * parks it in the node's *second* atom slot (node+0x24) and uses it for the
 * cheap tests that do not need the real mesh.
 */
static atm_atom_t *atm_make_box(const i32 *mn, const i32 *mx) {
    atm_atom_t *box = (atm_atom_t *)calloc(1, sizeof(*box));
    if (!box) return NULL;

    box->vert_max = box->vert_count = 8;   /* 0x436D5B / 0x436D71 */
    box->face_max = box->face_count = 6;   /* 0x436F43 / 0x436F59 */
    box->verts = (i32 *)calloc(8 * ATM_VERT_STRIDE, sizeof(i32)); /* 0x100 */
    box->faces = (i32 *)calloc(6 * ATM_FACE_STRIDE, sizeof(i32)); /* 0x258 */
    if (!box->verts || !box->faces) {
        free(box->verts); free(box->faces); free(box);
        return NULL;
    }

    /* Corner order the face table below indexes into (0x436DB2..0x436F37). */
    static const u8 corner[8][3] = {
        /* x, y, z picked from max(1) or min(0) */
        { 1, 1, 1 },  /* v0 */
        { 1, 0, 1 },  /* v1 */
        { 0, 0, 1 },  /* v2 */
        { 0, 1, 1 },  /* v3 */
        { 1, 1, 0 },  /* v4 */
        { 1, 0, 0 },  /* v5 */
        { 0, 0, 0 },  /* v6 */
        { 0, 1, 0 },  /* v7 */
    };
    for (i32 i = 0; i < 8; i++) {
        i32 *v = box->verts + (size_t)i * ATM_VERT_STRIDE;
        for (i32 k = 0; k < 3; k++)
            v[k] = corner[i][k] ? mx[k] : mn[k];
    }

    /* Six quads, wound as the asm writes them (0x436FB6..0x437169). */
    static const u8 quad[6][4] = {
        { 0, 3, 2, 1 },   /* +Z */
        { 4, 5, 6, 7 },   /* -Z */
        { 1, 2, 6, 5 },   /* -Y */
        { 0, 4, 7, 3 },   /* +Y */
        { 0, 1, 5, 4 },   /* +X */
        { 3, 7, 6, 2 },   /* -X */
    };
    for (i32 i = 0; i < 6; i++) {
        i32 *f = box->faces + (size_t)i * ATM_FACE_STRIDE;
        f[ATM_FACE_SIZE]  = 0x64;      /* 0x436FC8 */
        f[ATM_FACE_VCNT]  = 4;         /* 0x437049 */
        f[0x28 / 4]       = 0xFF;      /* 0x436FE1 — remapped colour index */
        f[0x5C / 4]       = 0x2222;    /* 0x436FFB — packed RGB */
        f[0x38 / 4]       = 0;         /* 0x437015 */
        f[ATM_FACE_FLAGS] = 0;         /* 0x43702F */
        for (i32 k = 0; k < 4; k++)
            f[ATM_FACE_IDX + k] = quad[i][k];
    }

    memcpy(box->name, "box", 4);
    return box;
}

/*
 * obc_build_box_atoms — sub_436C56 @ 0x436C56.
 *
 * Walks the tree and gives every node that carries geometry a proxy box atom
 * in slot 1, then runs the original's weapon-chain sanity checks. Uses the
 * same park-the-sibling / descend-the-child stack shape as get_node_bounds.
 */
static void obc_build_box_atoms(obc_node_t *root) {
    obc_node_t *stack[OBC_BOUNDS_STACK];
    i32 depth = 0;
    obc_node_t *node = root;

    for (;;) {
        if (depth != 0) {                    /* loc_436C81 */
            depth--;
            node = stack[depth];
        }

        for (;;) {                            /* loc_436C9E */
            if (node->next && depth < OBC_BOUNDS_STACK)
                stack[depth++] = node->next;

            atm_atom_t *atom = node->atom[0];
            if (atom && atom->vert_count && atom->verts) {
                i32 mn[3], mx[3];
                atm_vertex_bounds(atom, mx, mn);
                atm_atom_t *box = atm_make_box(mn, mx);
                if (box) {
                    free_atom_slot(&node->atom[1]);
                    node->atom[1] = box;       /* 0x436D24 */
                }
            }

            node = node->child;                /* 0x437176 */
            if (!node) break;
        }

        if (depth == 0) break;
    }

    /* 0x43719C — weapon chain must read arme(a) -> armem -> arme. The
     * original only logs; keep that, it is a data-integrity check. */
    if (strncmp(root->name, "arme", 4) != 0) return;
    if (strcmp(root->name, "armea") != 0) {
        print("obc: armea is not here\n");
    } else if (!root->child) {
        print("obc: armea has not a son\n");
    } else if (strcmp(root->child->name, "armem") != 0) {
        print("obc: armem is not here\n");
    } else if (!root->child->child) {
        print("obc: armem has not a son\n");
    } else if (strcmp(root->child->child->name, "arme") != 0) {
        print("obc: arme is not here\n");
    }
}

/*
 * atm_path_from_name — sub_4531D6 @ 0x4531D6 (+ sub_4532C7 @ 0x4532C7).
 *
 * Atom names in the OBC name table have no extension. The original keeps at
 * most the last 8 characters, appends ".ATM" and prefixes the object's base
 * directory (byte_4DE330, built by setup_creature @ 0x44F2A0). Names that
 * already carry a '.' are used verbatim.
 */
static void atm_path_from_name(const c8 *name, const c8 *base_dir,
                               c8 *out, i32 out_size) {
    c8 leaf[32];
    if (strchr(name, '.')) {               /* sub_4532C7 returned non-zero */
        snprintf(leaf, sizeof(leaf), "%s", name);
    } else {
        const size_t len = strlen(name);
        const c8 *tail = (len > 8) ? name + (len - 8) : name;
        snprintf(leaf, sizeof(leaf), "%.8s.ATM", tail);
    }
    snprintf(out, (size_t)out_size, "%s%s", base_dir ? base_dir : "", leaf);
}

/* =========================================================================
 * OBC hierarchy loader — sub_4538FB @ 0x4538FB / sub_452B20 @ 0x452B20
 * ====================================================================== */

static void obc_free_node(obc_node_t *n) {
    if (!n) return;
    free(n->clefs);
    free(n);
}

/*
 * obc_parse_body — sub_452B20 @ 0x452B20.
 *
 * `chunk_off` is the offset of the "3DITOR ... OBJC" signature inside `base`.
 * It is 0 for a standalone .OBC file and the OBJC chunk offset inside a .OME;
 * the two carry byte-identical structures, and the name-table offset in the
 * header is chunk-relative in both cases.
 *
 * Atom names are handed to `resolve`, which either opens a sibling .ATM file
 * or picks the matching ATOM chunk out of the same buffer.
 */
typedef atm_atom_t *(*atom_resolver_fn)(const c8 *name, void *ctx);

static obc_node_t *obc_parse_body(const u8 *base, i32 size, i32 chunk_off,
                                  atom_resolver_fn resolve, void *ctx,
                                  const c8 *dbg) {
    if (chunk_off < 0 || size - chunk_off <= 0x30) return NULL;

    cursor_t c = { base, base + chunk_off, base + size };

    /* 0x452B43 — 0x30-byte header, then three fields out of it. */
    u8 hdr[0x30];
    memcpy(hdr, c.p, sizeof(hdr));
    c.p += sizeof(hdr);

    i32 node_count, name_count, name_table_off;
    memcpy(&node_count,     hdr + 0x10, 4);  /* var_3C */
    memcpy(&name_count,     hdr + 0x20, 4);  /* var_2C -> dword_4D6F54 */
    memcpy(&name_table_off, hdr + 0x24, 4);  /* var_28 */

    if (strncmp((const c8 *)hdr, "3DITOR", 6) != 0)
        print("obc: '%s' has an unexpected signature\n", dbg);

    if (node_count <= 0 || name_count < 0 || name_table_off < 0 ||
        chunk_off + name_table_off + name_count * 16 > size) {
        /* asm: log_message(4, "Bad object to load") @ 0x452E5F */
        print("obc: bad object to load '%s' (nodes=%d names=%d)\n",
              dbg, node_count, name_count);
        return NULL;
    }

    obc_node_t **nodes = (obc_node_t **)calloc((size_t)node_count, sizeof(*nodes));
    /* Link indices are read from the record and resolved in a second pass. */
    i32 (*links)[7] = calloc((size_t)node_count, sizeof(*links));
    if (!nodes || !links) {
        free(nodes); free(links);
        return NULL;
    }

    /* 0x452B98 — one 0xB4-byte record per node. */
    i32 parsed = 0;
    for (; parsed < node_count; parsed++) {
        u8 rec[0xB4];
        const i32 payload = read_record(&c, rec, (i32)sizeof(rec));
        if (payload < 0xA0) {
            /* asm: log_message(4, "Bad sous-object to load") @ 0x452C2C */
            print("obc: bad sous-object to load (node %d)\n", parsed);
            break;
        }

        obc_node_t *n = (obc_node_t *)calloc(1, sizeof(*n));
        if (!n) break;

        memcpy(n->name, rec, 16);              /* +0x04 */
        n->name[15] = '\0';
        n->raw_14 = rec_i32(rec, 0x14);
        n->raw_18 = rec_i32(rec, 0x18);
        n->raw_1C = rec_i32(rec, 0x1C);
        for (i32 k = 0; k < 3; k++)
            links[parsed][k] = rec_i32(rec, 0x20 + k * 4);   /* atom slots */
        for (i32 k = 0; k < 4; k++)
            n->raw_2C[k] = rec_i32(rec, 0x2C + k * 4);
        for (i32 k = 0; k < 3; k++) {
            n->pos[k]    = rec_i32(rec, 0x3C + k * 4);
            n->raw_48[k] = rec_i32(rec, 0x48 + k * 4);
            n->rot[k]    = rec_i32(rec, 0x54 + k * 4);
        }
        for (i32 k = 0; k < 6; k++)
            n->raw_60[k] = rec_i32(rec, 0x60 + k * 4);
        links[parsed][3] = rec_i32(rec, 0x78);   /* parent  */
        links[parsed][4] = rec_i32(rec, 0x7C);   /* child   */
        links[parsed][5] = rec_i32(rec, 0x80);   /* prev    */
        links[parsed][6] = rec_i32(rec, 0x84);   /* next    */
        /* +0x88 is forced to 0 by the reader (0x45314C); calloc did that. */
        for (i32 k = 1; k < 5; k++)
            n->raw_88[k] = rec_i32(rec, 0x88 + k * 4);
        n->has_clefs = rec_i32(rec, 0x9C);

        nodes[parsed] = n;
    }

    if (parsed != node_count) {
        for (i32 i = 0; i < parsed; i++) obc_free_node(nodes[i]);
        free(nodes); free(links);
        return NULL;
    }

    /* 0x452C40 — name table: name_count 16-byte entries at name_table_off.
     * "tree" is the reserved "no geometry" marker. */
    atm_atom_t **atoms = NULL;
    if (name_count > 0) {
        atoms = (atm_atom_t **)calloc((size_t)name_count, sizeof(*atoms));
        if (!atoms) {
            for (i32 i = 0; i < node_count; i++) obc_free_node(nodes[i]);
            free(nodes); free(links);
            return NULL;
        }
        c.p = base + chunk_off + name_table_off;
        for (i32 i = 0; i < name_count; i++) {
            if (cur_left(&c) < 16) break;
            c8 name[17];
            memcpy(name, c.p, 16);
            name[16] = '\0';
            c.p += 16;

            if (strcmp(name, "tree") == 0) {   /* 0x452CD7 */
                atoms[i] = NULL;
                continue;
            }
            atoms[i] = resolve(name, ctx);
        }
    }

    /* 0x452D02 — resolve every index into a pointer; -1 becomes NULL. */
    for (i32 i = 0; i < node_count; i++) {
        obc_node_t *n = nodes[i];
        for (i32 k = 0; k < 3; k++) {
            const i32 idx = links[i][k];
            n->atom[k] = (idx >= 0 && idx < name_count) ? atoms[idx] : NULL;
        }
        obc_node_t **slot[4] = { &n->parent, &n->child, &n->prev, &n->next };
        for (i32 k = 0; k < 4; k++) {
            const i32 idx = links[i][3 + k];
            *slot[k] = (idx >= 0 && idx < node_count) ? nodes[idx] : NULL;
        }
    }

    obc_node_t *root = nodes[0];

    /*
     * Re-derive parent and previous-sibling from the child/next chains.
     *
     * The +0x78 and +0x80 the exporter wrote are stale: they survive edits to
     * the rig that the child/sibling chains do reflect. On PERSOS/P00.OME the
     * file claims "hangau" (left hip) hangs off "piedro" (the RIGHT foot) and
     * "epagau" (left shoulder) off "poidro" (the right wrist) — the whole left
     * side dangling from the end of the right limb. Across persos+objets, 99
     * of 208 files carry at least one such link, 258 in total.
     *
     * The engine never trusts them either: sub_4699D9 @ 0x4699D9 walks +0x7C /
     * +0x84 and writes both fields back before anything reads them. Doing it
     * here means no consumer of this loader can pick up the stale values.
     */
    for (i32 i = 0; i < node_count; i++) {
        obc_node_t *prev = NULL;
        for (obc_node_t *c = nodes[i]->child; c; c = c->next) {
            c->parent = nodes[i];
            c->prev   = prev;
            prev      = c;
        }
    }
    root->parent = NULL;
    root->prev   = NULL;

    free(atoms);
    free(links);
    free(nodes);

    /* 0x452E89 — proxy box atoms + weapon-chain checks. The original then
     * calls sub_491470 @ 0x491470, still unported. */
    obc_build_box_atoms(root);

    print("obc: '%s' — %d nodes, %d atom slots, root '%s'\n",
          dbg, node_count, name_count, root->name);
    return root;
}

/* -------------------------------------------------------------------------
 * Front ends
 * ---------------------------------------------------------------------- */

/* Sibling-.ATM resolver — the .OBC path (CD install, sub_4531D6 @ 0x4531D6). */
typedef struct {
    const c8 *base_dir;
    const c8 *game_path;
} atm_file_ctx_t;

static atm_atom_t *resolve_atom_file(const c8 *name, void *ctx) {
    const atm_file_ctx_t *fc = (const atm_file_ctx_t *)ctx;
    c8 vpath[512];
    atm_path_from_name(name, fc->base_dir, vpath, (i32)sizeof(vpath));
    return atm_load(vpath, fc->game_path);
}

/* Read a whole file into a fresh buffer. Returns NULL and leaves *out_size
 * untouched on failure. */
static u8 *slurp(const c8 *vpath, const c8 *game_path, i32 *out_size,
                 c8 *resolved, i32 resolved_size) {
    if (!ls_resolve_path(game_path, vpath, resolved, resolved_size)) {
        print("obc: path overflow '%s'\n", vpath);
        return NULL;
    }
    const i32 size = ls_get_filesize(resolved);
    if (size <= 0x30) {
        print("obc: cannot open '%s'\n", resolved);
        return NULL;
    }
    u8 *raw = (u8 *)malloc((size_t)size);
    if (!raw) return NULL;

    FILE *f = ls_open(resolved, "rb");
    if (!f) { free(raw); return NULL; }
    ls_read_buffer(f, raw, size);
    ls_close(f);

    *out_size = size;
    return raw;
}

obc_node_t *obc_load(const c8 *obc_vpath, const c8 *base_dir,
                     const c8 *game_path) {
    if (!obc_vpath) return NULL;

    c8 resolved[512];
    i32 size = 0;
    /* asm: log_message(4, "Cannot open OBC file %s") @ 0x453BC7 */
    u8 *raw = slurp(obc_vpath, game_path, &size, resolved, (i32)sizeof(resolved));
    if (!raw) return NULL;

    atm_file_ctx_t fc = { base_dir, game_path };
    obc_node_t *root = obc_parse_body(raw, size, 0, resolve_atom_file, &fc,
                                      resolved);
    free(raw);
    return root;
}

/* -------------------------------------------------------------------------
 * .OME front end — sub_44F454 @ 0x44F454
 *
 * On an HDD install setup_creature @ 0x44F2A0 never touches the .OBC/.ATM
 * pair: it hands the .OME to sub_44F454, which strips the "CTX3" wrapper and
 * lets sub_44F92D @ 0x44F92D / sub_4228E0 @ 0x4228E0 build the same node tree
 * out of the chunks packed into that one file.
 *
 * The chunk bodies are byte-identical to the standalone files — verified
 * against PERSOS/P00.OME, whose OBJC chunk carries 29 nodes and a 20-entry
 * name table matching its 20 ATOM chunks one for one — so the port reuses
 * obc_parse_body / atm_parse_chunk and only has to locate the chunks.
 * ---------------------------------------------------------------------- */

#define OME_MAX_ATOM_CHUNKS 64

typedef struct {
    const u8 *base;
    i32       size;
    i32       offs[OME_MAX_ATOM_CHUNKS];
    c8        names[OME_MAX_ATOM_CHUNKS][17];
    i32       count;
} ome_atom_index_t;

static atm_atom_t *resolve_atom_ome(const c8 *name, void *ctx) {
    ome_atom_index_t *ix = (ome_atom_index_t *)ctx;
    for (i32 i = 0; i < ix->count; i++) {
        if (strcmp(ix->names[i], name) == 0)
            return atm_parse_chunk(ix->base, ix->size, ix->offs[i], name);
    }
    print("obc_ome: no ATOM chunk named '%s'\n", name);
    return NULL;
}

obc_node_t *obc_load_ome(const c8 *ome_vpath, const c8 *game_path) {
    if (!ome_vpath) return NULL;

    c8 resolved[512];
    i32 size = 0;
    /* asm: log_message(4, "Cannot open OME file %s") @ 0x44F4EF */
    u8 *raw = slurp(ome_vpath, game_path, &size, resolved, (i32)sizeof(resolved));
    if (!raw) return NULL;

    /* 0x44F4FE — "CTX3" container: the 3DITOR payload starts at +[raw+8]. */
    i32 body = 0;
    if (memcmp(raw, "CTX3", 4) == 0) {
        memcpy(&body, raw + 8, 4);
        if (body < 0 || body >= size) body = 0;
    }

    /* Index the chunks. Chunk spans are not stored anywhere, so locate each
     * signature rather than trying to walk the file sequentially. */
    ome_atom_index_t ix = { raw, size, { 0 }, { { 0 } }, 0 };
    i32 objc_off = -1;
    for (i32 i = body; i + 0x44 <= size; i++) {
        if (memcmp(raw + i, "3DITOR", 6) != 0) continue;
        if (memcmp(raw + i + 12, "OBJC", 4) == 0) {
            if (objc_off < 0) objc_off = i;
        } else if (memcmp(raw + i + 12, "ATOM", 4) == 0) {
            if (ix.count < OME_MAX_ATOM_CHUNKS) {
                ix.offs[ix.count] = i;
                memcpy(ix.names[ix.count], raw + i + 0x34, 16);
                ix.names[ix.count][16] = '\0';
                ix.count++;
            }
        }
    }

    if (objc_off < 0) {
        print("obc_ome: '%s' has no OBJC chunk\n", resolved);
        free(raw);
        return NULL;
    }

    obc_node_t *root = obc_parse_body(raw, size, objc_off,
                                      resolve_atom_ome, &ix, resolved);
    free(raw);
    return root;
}

#define OBC_FREE_STACK 512

void obc_free(obc_node_t *root) {
    if (!root) return;

    /*
     * The tree is a plain child/sibling structure; walk it iteratively so a
     * deep skeleton cannot blow the host stack. Atoms from the name table are
     * shared between nodes, so they have to be freed exactly once — when the
     * seen table is full we leak rather than risk a double free.
     */
    obc_node_t *stack[OBC_FREE_STACK];
    atm_atom_t *seen[OBC_FREE_STACK];
    i32 depth = 0, nseen = 0;

    stack[depth++] = root;
    while (depth > 0) {
        obc_node_t *n = stack[--depth];
        for (i32 k = 0; k < 3; k++) {
            atm_atom_t *a = n->atom[k];
            if (!a) continue;
            i32 dup = 0;
            for (i32 i = 0; i < nseen; i++) if (seen[i] == a) { dup = 1; break; }
            if (dup) continue;
            if (nseen >= OBC_FREE_STACK) continue;   /* leak, do not double free */
            seen[nseen++] = a;
            atom_free(a);
        }
        if (n->child && depth < OBC_FREE_STACK) stack[depth++] = n->child;
        if (n->next  && depth < OBC_FREE_STACK) stack[depth++] = n->next;
        obc_free_node(n);
    }
}

/* =========================================================================
 * Bounds + control points
 * ====================================================================== */

void obc_node_bounds(const obc_node_t *root, i32 *out_min, i32 *out_max) {
    /* 0x436669 — minima start at 0x7FFFFFFF, maxima at 0x80000001. */
    i32 mn[3] = { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF };
    i32 mx[3] = { (i32)0x80000001, (i32)0x80000001, (i32)0x80000001 };

    if (!root) {
        for (i32 k = 0; k < 3; k++) { out_min[k] = mn[k]; out_max[k] = mx[k]; }
        return;
    }

    struct { fmat_t mat; const obc_node_t *node; } stack[OBC_BOUNDS_STACK];
    i32 depth = 0;

    /* 0x4366C4 — identity in Q14. */
    fmat_t cur;
    memset(&cur, 0, sizeof(cur));
    cur.m[0] = cur.m[4] = cur.m[8] = 0x4000;

    const obc_node_t *node = root;

    for (;;) {
        if (depth != 0) {                    /* loc_4366E6 */
            depth--;
            cur  = stack[depth].mat;
            node = stack[depth].node;
        }

        for (;;) {                            /* loc_43673D */
            /* Park the sibling branch with a copy of the current transform. */
            if (node->next && depth < OBC_BOUNDS_STACK) {
                stack[depth].node = node->next;
                stack[depth].mat  = cur;
                depth++;
            }

            /* 0x4367AF — local rotation, then local translation rotated by
             * the transform accumulated down to this node's parent. */
            fmat_t rot, pos;
            fmat_rotation(&rot, node->rot[0], node->rot[1], node->rot[2]);
            fmat_position(&pos, node->pos[0], node->pos[1], node->pos[2]);
            fmat_rotate_translation(&cur, &pos);   /* 0x43680B */
            fmat_premul_rotation(&rot, &cur);      /* 0x43681C */
            cur.t[0] += pos.t[0];                  /* 0x436821 */
            cur.t[1] += pos.t[1];
            cur.t[2] += pos.t[2];

            const fmat_t world = cur;              /* 0x436855 — var_508 */

            const atm_atom_t *atom = node->atom[0];
            if (atom && atom->verts && atom->faces) {
                for (i32 fi = 0; fi < atom->face_count; fi++) {
                    const i32 *face = atom->faces + (size_t)fi * ATM_FACE_STRIDE;
                    const i32 vcnt = face[ATM_FACE_VCNT];
                    for (i32 j = 0; j < vcnt && j < 4; j++) {
                        const i32 vi = face[ATM_FACE_IDX + j];
                        if (vi < 0 || vi >= atom->vert_max) continue;
                        const i32 *src = atom->verts + (size_t)vi * ATM_VERT_STRIDE;

                        i32 v[3] = { src[ATM_VERT_X], src[ATM_VERT_Y], src[ATM_VERT_Z] };
                        fmat_transform_point(&world, v);

                        for (i32 k = 0; k < 3; k++) {
                            if (v[k] < mn[k]) mn[k] = v[k];
                            if (v[k] > mx[k]) mx[k] = v[k];
                        }
                    }
                }
            }

            node = node->child;                /* 0x436A01 / 0x43687C */
            if (!node) break;
        }

        if (depth == 0) break;                 /* 0x436A1D */
    }

    /* 0x436A2A — results are relative to the root node's own pivot. */
    for (i32 k = 0; k < 3; k++) {
        out_min[k] = mn[k] - root->pos[k];
        out_max[k] = mx[k] - root->pos[k];
    }
}

/* Node/bone name table @ 0x4B2BA0 — 34 entries, stride 7. */
const c8 *const obc_node_type_names[OBC_NODE_TYPE_COUNT] = {
    "tree",   "basdos", "hangau", "handro", "gendro",
    "piedro", "gengau", "piegau", "mildos", "haudos",
    "bascou", "epagau", "epadro", "coudro", "poidro",
    "cougau", "poigau", "tete",   "talgau", "taldro",
    "ddpgau", "ddpdro", "acces0", "acces1", "acces2",
    "acces3", "acces4", "pacdro", "pacgau", "hacdro",
    "hacgau", "arme",   "armea",  "armem"
};

i32 obc_node_type(const c8 *name) { /* get_node_type @ 0x41DE03 */
    for (i32 i = 0; i < OBC_NODE_TYPE_COUNT; i++)
        if (strcmp(obc_node_type_names[i], name) == 0) return i;
    if (strcmp(name, "armeb") == 0) return OBC_NODE_TYPE_ARMEB;
    return -1;
}

/*
 * Per-bone control-point budget, from the 34-case switch at 0x4043BD.
 *
 * The switch runs on `type - 1` and dispatches through the byte table at
 * 0x40442D; every entry not listed here maps to the default slot (0x11) and
 * gets no control points at all — including "tree" (type 0, so type-1 wraps
 * negative), "mildos" and "bascou", i.e. the torso and neck.
 *
 * `scale` is the fraction of (longest axis + shortest axis) each sphere's
 * diameter covers.
 */
typedef struct { i32 count; f64 scale; } cp_spec_t;

static cp_spec_t cp_spec_for_type(i32 type) {
    switch (type - 1) {
    case 0:  return (cp_spec_t){ 1, 0.7 };  /* basdos — lower spine   */
    case 1:  return (cp_spec_t){ 1, 0.3 };  /* hangau — left hip      */
    case 2:  return (cp_spec_t){ 1, 0.3 };  /* handro — right hip     */
    case 3:  return (cp_spec_t){ 2, 0.4 };  /* gendro — right knee    */
    case 4:  return (cp_spec_t){ 2, 0.5 };  /* piedro — right foot    */
    case 5:  return (cp_spec_t){ 2, 0.4 };  /* gengau — left knee     */
    case 6:  return (cp_spec_t){ 2, 0.5 };  /* piegau — left foot     */
    case 8:  return (cp_spec_t){ 1, 0.6 };  /* haudos — upper torso   */
    case 10: return (cp_spec_t){ 2, 0.4 };  /* epagau — left shoulder */
    case 11: return (cp_spec_t){ 2, 0.4 };  /* epadro — right shoulder*/
    case 12: return (cp_spec_t){ 2, 0.5 };  /* coudro — right elbow   */
    case 13: return (cp_spec_t){ 1, 0.8 };  /* poidro — right wrist   */
    case 14: return (cp_spec_t){ 1, 0.5 };  /* cougau — left elbow    */
    case 15: return (cp_spec_t){ 1, 0.8 };  /* poigau — left wrist    */
    case 16: return (cp_spec_t){ 1, 0.7 };  /* tete   — head          */
    case 30: return (cp_spec_t){ 3, 0.3 };  /* arme   — weapon        */
    case 33: return (cp_spec_t){ 4, 0.3 };  /* armem  — weapon mount  */
    default: return (cp_spec_t){ 0, 0.0 };
    }
}

static void obc_build_control_points(obc_node_t *root) { /* 0x4040B8 */
    obc_node_t *stack[OBC_BOUNDS_STACK];
    i32 depth = 0;
    obc_node_t *node = root;

    for (;;) {
        if (depth != 0) {                     /* loc_4040D7 */
            depth--;
            node = stack[depth];
        }

        for (;;) {                             /* loc_4040F4 */
            if (node->next && depth < OBC_BOUNDS_STACK)
                stack[depth++] = node->next;

            i32 cp_count = 0;
            f64 scale = 0.0;
            /*
             * 0x40412A — a node that already carries control points is left
             * alone. The original then falls into the "arme" test below with
             * a *stale* type still in its stack slot; the port initialises it
             * per node instead, which is the only sane reading.
             */
            i32 type = -1;
            if (node->has_clefs == 0) {
                type = obc_node_type(node->name);
                const cp_spec_t spec = cp_spec_for_type(type);
                cp_count = spec.count;
                scale    = spec.scale;
            }

            /* 0x40445C — a weapon node delegates to the deepest node of its
             * chain, unless that chain starts with "armeb". */
            if (type == OBC_NODE_TYPE_ARME && node->child && node->child->atom[0] &&
                obc_node_type(node->child->name) != OBC_NODE_TYPE_ARMEB) {
                do {
                    node = node->child;
                } while (node->child);
                cp_count = 1;
                scale    = 1.0;
            }

            atm_atom_t *atom = node->atom[0];  /* 0x4044E6 */
            if (cp_count > 0 && atom && atom->verts && atom->vert_count > 0) {
                i32 mn[3], mx[3];
                atm_vertex_bounds(atom, mx, mn);   /* inlined at 0x40451F */

                i32 d[3];
                for (i32 k = 0; k < 3; k++) d[k] = abs(mx[k] - mn[k]);

                /* 0x40473E — longest axis (1=X, 2=Y, 3=Z), unsigned compares. */
                i32 axis, len;
                if ((u32)d[1] >= (u32)d[0]) { axis = 2; len = d[1]; }
                else                        { axis = 1; len = d[0]; }
                if ((u32)len < (u32)d[2])   { axis = 3; len = d[2]; }

                const u32 step = (u32)len / (u32)cp_count;   /* 0x4047AB */

                /* 0x4047B4 — despite the name IDA gave it, this is the
                 * *shortest* extent, used to fatten the spheres. */
                i32 girth = ((u32)d[1] <= (u32)d[0]) ? d[1] : d[0];
                if ((u32)girth > (u32)d[2]) girth = d[2];

                /* 0x404817 — radius = (longest + shortest) * scale / 2 */
                const i32 radius =
                    (i32)((f64)((u32)len + (u32)girth) * scale / 2.0);

                /* 0x404842 — first centre: mid-span on the two minor axes,
                 * half a step in from the far end on the primary one. */
                i32 c[3];
                for (i32 k = 0; k < 3; k++)
                    c[k] = (axis == k + 1) ? mx[k] - (i32)(step >> 1)
                                           : (mx[k] + mn[k]) / 2;

                node->has_clefs = cp_count;        /* 0x404933 */

                if (!node->clefs) {                /* 0x404939 */
                    node->clefs = (obc_clefs_t *)calloc((size_t)cp_count,
                                                        sizeof(obc_clefs_t));
                    if (node->clefs) {
                        for (i32 i = 0; i < cp_count; i++) {
                            node->clefs[i].radius    = radius;
                            node->clefs[i].centre[0] = c[0];
                            node->clefs[i].centre[1] = c[1];
                            node->clefs[i].centre[2] = c[2];
                            /* 0x404A6F — walk back along the primary axis. */
                            c[axis - 1] -= (i32)step;
                        }
                    }
                }
            }

            node = node->child;                    /* 0x404AD8 */
            if (!node) break;
        }

        if (depth == 0) break;                     /* 0x404AF4 */
    }
}

void obc_process_hierarchy(obc_node_t *entity_root) { /* 0x404C26 */
    if (!entity_root) return;
    /* Skip the entity's own transform node when it has children. */
    obc_build_control_points(entity_root->child ? entity_root->child
                                                : entity_root);
}

void obc_create_control_point(obc_node_t *root) { /* 0x404D92 */
    if (!root) return;

    i32 mn[3], mx[3];
    obc_node_bounds(root, mn, mx);

    const i32 dx = abs(mx[0] - mn[0]);     /* 0x404DD2 */
    const i32 dy = abs(mx[1] - mn[1]);
    const i32 dz = abs(mx[2] - mn[2]);

    /* 0x404E13 — largest extent, then halved (scaling_constant = 2.0). */
    f64 d = (f64)dx;
    if ((f64)dy >= (f64)dx) d = (f64)dy;
    if (d < (f64)dz) d = (f64)dz;
    d /= 2.0;

    obc_clefs_t *clefs = (obc_clefs_t *)calloc(1, sizeof(*clefs));
    if (!clefs) return;

    free(root->clefs);
    root->has_clefs = 1;                   /* 0x404E6D */
    root->clefs     = clefs;               /* 0x404E87 — "obj->Clefs", 0x1C */

    clefs->radius    = (i32)d;             /* 0x404EB6 — __ftol truncates */
    clefs->centre[0] = (mn[0] + mx[0]) / 2;
    clefs->centre[1] = (mn[1] + mx[1]) / 2;
    clefs->centre[2] = (mn[2] + mx[2]) / 2;

    print("obc: control point '%s' r=%d centre=(%d,%d,%d)\n",
          root->name, clefs->radius,
          clefs->centre[0], clefs->centre[1], clefs->centre[2]);
}
