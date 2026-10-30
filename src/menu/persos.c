/*
 * persos.c — sub_43F8C7 — characters (MENUPERS).
 *
 * Opened from the main menu (and from the in-game options). The screen is
 * the character settings page: it shows the hero (player_entity) standing in
 * the guard pose of g01arr01, turning on itself, over DATAS>MENUS>MENUPERS.GIF,
 * and sets two values kept in DKEGAME.CFG:
 *
 *   dword_4B7240  shadow detail under the characters (0x467875 in game):
 *                 1 = seven-bone silhouette (sub_433670), 2 = low-poly mesh
 *                 shadow (render_entity_bounds on atom LOD 1, dword_5317B4 = 1),
 *                 3 = full mesh shadow (LOD 0), 4 = automatic (per-actor
 *                 +0x94; the preview cycles 3, 2, 1).
 *   dword_4B7494  control scheme (g_control_scheme, combat_player.c).
 *
 * Six hit boxes, the 40-byte table at [ebp-0x4118] (PERSIMAG.GIF sprites):
 *
 *   0  shadow 3     (0x244,0xC3)  20x21   src (0,0)
 *   1  shadow 2     (0x244,0xE4)  20x21   src (0,0x21)
 *   2  shadow 1     (0x244,0x107) 20x21   src (0,0x44)
 *   3  shadow auto  (0x23E,0x128) 31x20   src (0x40,0xE)
 *   4  controls     (0x19,0x8B)   43x43   src (0x48,0x2E)  toggles, unhover 0
 *   5  exit         (0x235,0x172) 51x51   src (0x14,0x2E)
 *
 * Keys: Up / Down step the shadow level (Down from 1 goes to auto, Up from
 * auto to 1), Left / Right flip the control scheme, Esc / Enter / the right
 * button / the exit button leave.
 *
 * Camera (0x43FED3): identity rotation, eye 57300 above the ground and
 * 258300 in front of the hero, global scale 10240 / tan(0.5431); the hero's
 * root yaw (+0x5C) turns a full circle every 800 * 2pi ms.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu/screens.h"
#include "state.h"
#include "game_init.h"
#include "gamecfg.h"
#include "sfx.h"
#include "screen_fx.h"
#include "actor.h"
#include "can.h"
#include "obc.h"
#include "model.h"
#include "model_ome.h"
#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/file_system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/audio/wav.h"

extern i32 g_control_scheme;            /* dword_4B7494, combat_player.c */

typedef struct {
    i32 sx, sy, w, h;       /* +0 .. +C: sub-rect of PERSIMAG */
    i32 dx, dy;             /* +10, +14 */
    i32 *state;             /* +18 */
    i32 value;              /* +1C: written on click */
    i32 unhover;            /* +20: written on a second click, -1 = none */
    i32 hovered;            /* +24 */
} prs_btn_t;


static void prs_free_bmap(bmap_t *b) {
    if (!b) return;
    if (b->owns_buffer && b->pixels) free(b->pixels);
    if (b->owns_palette && b->palette) free(b->palette);
    free(b);
}

/* overlay_cursor_image: index 0 is transparent. */
static void blit_sub(u32 *fb, i32 fw, i32 fh, const bmap_t *src, const u8 *pal,
                     i32 sx, i32 sy, i32 sw, i32 sh, i32 dx, i32 dy) {
    if (!src || !src->pixels || !pal) return;
    for (i32 r = 0; r < sh; r++) {
        i32 y = dy + r, ys = sy + r;
        if (y < 0 || y >= fh || ys < 0 || ys >= src->height) continue;
        for (i32 c = 0; c < sw; c++) {
            i32 x = dx + c, xs = sx + c;
            if (x < 0 || x >= fw || xs < 0 || xs >= (i32)src->width) continue;
            u8 idx = src->pixels[ys * src->width + xs];
            if (idx == 0) continue;
            fb[y * fw + x] = 0xFF000000u | ((u32)pal[idx * 3 + 2] << 16) |
                             ((u32)pal[idx * 3 + 1] << 8) | pal[idx * 3];
        }
    }
}

/* sub_43F7BD: hit test; a click writes the entry's value (or, clicked
 * again while it holds, its unhover value). Returns index + 1 or 0. */
static i32 prs_hit(prs_btn_t *t, i32 n, i32 mx, i32 my, i32 click) {
    for (i32 i = 0; i < n; i++) {
        prs_btn_t *b = &t[i];
        if (mx < b->dx || mx >= b->dx + b->w || my < b->dy || my >= b->dy + b->h) continue;
        if (click) {
            if (!b->hovered || *b->state != b->value) {
                b->hovered = 1;
                *b->state = b->value;
            } else if (b->unhover != -1) {
                b->hovered = 0;
                *b->state = b->unhover;
            }
        }
        return i + 1;
    }
    return 0;
}


static void prs_shot(void) {
    const c8 *path = getenv("DKE_MENUSHOT");
    if (!path) return;
    FILE *f = fopen(path, "wb");
    if (!f) return;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    fprintf(f, "P6\n%d %d\n255\n", fw, fh);
    for (i32 i = 0; i < fw * fh; i++) {
        u32 p = state->graphic.front_buffer[i];
        fputc((u8)p, f); fputc((u8)(p >> 8), f); fputc((u8)(p >> 16), f);
    }
    fclose(f);
}

/* ----------------------------------------------------------------------
 * 3D: projection, shadows, the hero
 * ---------------------------------------------------------------------- */

#define PRS_W 640
#define PRS_H 480

static i32 g_gscale;              /* global_timer_value: 10240 / tan(0.5431) */
static i32 g_eye[3];              /* -transformation_origin */
static u8  g_mask[PRS_W * PRS_H]; /* shadow coverage */

/* Camera space -> screen, as model_render_batch does (identity camera). */
static i32 prs_project(i64 cx, i64 cy, i64 cz, i32 *sx, i32 *sy) {
    if (cz >= -32768) return 0;
    i32 d = (i32)((-cz) >> 9);
    if (d < 1) d = 1;
    f64 sc = ((f64)g_gscale / 16384.0) / (f64)d;
    *sx = 320 + (i32)lround((f64)cx * sc);
    *sy = 240 - (i32)lround((f64)cy * sc);
    return 1;
}

/* sub_417E80 / sub_4184C0: flat triangle into the shadow mask. */
static void prs_fill_tri(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2) {
    i32 minx = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    i32 maxx = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    i32 miny = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    i32 maxy = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);
    if (minx < 0) minx = 0;
    if (miny < 0) miny = 0;
    if (maxx > PRS_W - 1) maxx = PRS_W - 1;
    if (maxy > PRS_H - 1) maxy = PRS_H - 1;
    i64 area = (i64)(x1 - x0) * (y2 - y0) - (i64)(y1 - y0) * (x2 - x0);
    if (area == 0) return;
    for (i32 y = miny; y <= maxy; y++)
        for (i32 x = minx; x <= maxx; x++) {
            i64 w0 = (i64)(x1 - x) * (y2 - y) - (i64)(y1 - y) * (x2 - x);
            i64 w1 = (i64)(x2 - x) * (y0 - y) - (i64)(y2 - y) * (x0 - x);
            i64 w2 = (i64)(x0 - x) * (y1 - y) - (i64)(y0 - y) * (x1 - x);
            if (area > 0 ? (w0 >= 0 && w1 >= 0 && w2 >= 0) : (w0 <= 0 && w1 <= 0 && w2 <= 0))
                g_mask[y * PRS_W + x] = 1;
        }
}

/* sub_433670: the cheap shadow, five triangles between seven bones
 * (ddpgau poidro ddpdro taldro poigau talgau tete) dropped on the floor. */
static void prs_shadow_bones(entity_t *e) {
    static const i32 slot[7] = { 0x14, 0x0E, 0x15, 0x13, 0x10, 0x12, 0x11 };
    static const i32 tri[5][3] = { {0,1,5}, {1,2,4}, {2,3,4}, {1,4,5}, {1,4,6} };
    i32 sx[7], sy[7];
    for (i32 i = 0; i < 7; i++) {
        obc_node_t *n = EBONE(e, slot[i]);
        if (!n) return;
        if (!prs_project((i64)n->raw_88[2] - g_eye[0], (i64)E32(e, 0x8C) - g_eye[1],
                         (i64)n->raw_88[4] - g_eye[2], &sx[i], &sy[i])) return;
    }
    for (i32 t = 0; t < 5; t++)
        prs_fill_tri(sx[tri[t][0]], sy[tri[t][0]], sx[tri[t][1]], sy[tri[t][1]],
                     sx[tri[t][2]], sy[tri[t][2]]);
}

/* render_entity_bounds "rshadow": every face of the LOD `lod` atoms
 * projected from the light point (var_4128) onto the floor plane. */
static void prs_shadow_mesh(entity_t *e, i32 lod, const i32 light[3]) {
    obc_node_t *root = EROOT(e);
    if (!root) return;
    f64 ground = (f64)E32(e, 0x8C);
    fmat_t cur, stack_m[16];
    obc_node_t *stack_n[16];
    i32 sp = 0;
    fmat_rotation(&cur, 0, 0, 0);
    cur.t[0] = cur.t[1] = cur.t[2] = 0;
    obc_node_t *n = root;
    static i32 px[4096], py[4096];
    static u8 ok[4096];
    for (;;) {
        while (n) {
            if (n->next && n != root && sp < 16) { stack_n[sp] = n->next; stack_m[sp] = cur; sp++; }
            fmat_t rot, pos;
            fmat_rotation(&rot, n->rot[0], n->rot[1], n->rot[2]);
            fmat_position(&pos, n->pos[0], n->pos[1], n->pos[2]);
            fmat_rotate_translation(&cur, &pos);
            fmat_premul_rotation(&rot, &cur);
            cur.t[0] += pos.t[0]; cur.t[1] += pos.t[1]; cur.t[2] += pos.t[2];
            atm_atom_t *a = n->atom[lod] ? n->atom[lod] : n->atom[0];
            if (a && a->verts && a->faces && a->vert_count <= 4096) {
                const i32 *M = cur.m;
                for (i32 i = 0; i < a->vert_count; i++) {
                    const i32 *v = &a->verts[i * ATM_VERT_STRIDE];
                    i64 vx = v[ATM_VERT_X], vy = v[ATM_VERT_Y], vz = v[ATM_VERT_Z];
                    f64 wx = (f64)(((vx * M[0] + vy * M[3] + vz * M[6]) >> 14) + cur.t[0]);
                    f64 wy = (f64)(((vx * M[1] + vy * M[4] + vz * M[7]) >> 14) + cur.t[1]);
                    f64 wz = (f64)(((vx * M[2] + vy * M[5] + vz * M[8]) >> 14) + cur.t[2]);
                    f64 dy = wy - light[1];
                    f64 k = dy != 0 ? (ground - light[1]) / dy : 1.0;
                    f64 gx = light[0] + (wx - light[0]) * k;
                    f64 gz = light[2] + (wz - light[2]) * k;
                    ok[i] = (u8)prs_project((i64)gx - g_eye[0], (i64)ground - g_eye[1],
                                            (i64)gz - g_eye[2], &px[i], &py[i]);
                }
                for (i32 f = 0; f < a->face_count; f++) {
                    const i32 *fc = &a->faces[f * ATM_FACE_STRIDE];
                    i32 vc = fc[ATM_FACE_VCNT];
                    const i32 *ix = &fc[ATM_FACE_IDX];
                    if (vc < 3 || vc > 4) continue;
                    i32 bad = 0;
                    for (i32 k = 0; k < vc; k++)
                        if (ix[k] < 0 || ix[k] >= a->vert_count || !ok[ix[k]]) bad = 1;
                    if (bad) continue;
                    prs_fill_tri(px[ix[0]], py[ix[0]], px[ix[1]], py[ix[1]], px[ix[2]], py[ix[2]]);
                    if (vc == 4)
                        prs_fill_tri(px[ix[0]], py[ix[0]], px[ix[2]], py[ix[2]], px[ix[3]], py[ix[3]]);
                }
            }
            n = n->child;
        }
        if (sp == 0) break;
        sp--; n = stack_n[sp]; cur = stack_m[sp];
    }
}

/* dword_511790 / dword_5317A0 (arg 0x12): darken the backdrop under the
 * mask through the shade ramp. */
static void prs_shadow_apply(u32 *fb, const bmap_t *bg, const u8 *pal, const u8 *grd) {
    for (i32 i = 0; i < PRS_W * PRS_H; i++) {
        if (!g_mask[i]) continue;
        u8 idx = bg->pixels[i];
        if (grd) idx = grd[idx * 64 + 0x12];
        u32 c = 0xFF000000u | ((u32)pal[idx * 3 + 2] << 16) | ((u32)pal[idx * 3 + 1] << 8) | pal[idx * 3];
        if (!grd) c = 0xFF000000u | ((c >> 1) & 0x7F7F7F);
        fb[i] = c;
    }
}

static i32 prs_load_matr(i32 num, ome_matr_t *out);

/* create_entity_texture: the hero's textured, GRD-shaded atoms. */
static void prs_draw_hero(entity_t *e, const ome_matr_t *matr, const u8 *pal, const u8 *grd) {
    obc_node_t *root = EROOT(e);
    if (!root) return;
    static model_draw_t items[96];
    i32 n_items = 0;
    fmat_t cur, stack_m[16];
    obc_node_t *stack_n[16];
    i32 sp = 0;
    fmat_rotation(&cur, 0, 0, 0);                       /* parameter_table = identity */
    cur.t[0] = -g_eye[0]; cur.t[1] = -g_eye[1]; cur.t[2] = -g_eye[2];
    obc_node_t *n = root;
    for (;;) {
        while (n) {
            if (n->next && n != root && sp < 16) { stack_n[sp] = n->next; stack_m[sp] = cur; sp++; }
            fmat_t rot, pos;
            fmat_rotation(&rot, n->rot[0], n->rot[1], n->rot[2]);
            fmat_position(&pos, n->pos[0], n->pos[1], n->pos[2]);
            fmat_rotate_translation(&cur, &pos);
            fmat_premul_rotation(&rot, &cur);
            cur.t[0] += pos.t[0]; cur.t[1] += pos.t[1]; cur.t[2] += pos.t[2];
            const model_t *m = n->atom[0] ? model_from_atm_atom(n->atom[0], matr) : NULL;
            if (m && n_items < 96) {
                model_draw_t *it = &items[n_items++];
                memset(it, 0, sizeof(*it));
                it->m = m;
                for (i32 k = 0; k < 9; k++) it->m3[k] = cur.m[k];
                for (i32 k = 0; k < 3; k++) it->t[k] = cur.t[k];
                it->zval = -1;
                /* sub_4358D3: light from the viewer side, 45 degrees down;
                 * strength from +0x64 (set to 0x80 at 0x4400DB). */
                it->light[0] = 0; it->light[1] = -11585; it->light[2] = -11585;
                it->light_s = E32(e, 0x64) + 0x20;
            }
            n = n->child;
        }
        if (sp == 0) break;
        sp--; n = stack_n[sp]; cur = stack_m[sp];
    }
    model_render_batch(items, n_items, g_gscale, NULL, NULL, pal, grd, 0, 0, 0);
}

/* PERSOS>Pnn.OME material library + texture atlas. Kept for the session:
 * the atoms cache their render mesh (model_from_atm_atom) with pointers
 * into it. */
static struct { i32 num, ok; ome_matr_t m; } s_matr[4];
static const ome_matr_t *prs_matr(i32 num) {
    for (i32 i = 0; i < 4; i++)
        if (s_matr[i].ok && s_matr[i].num == num) return &s_matr[i].m;
    for (i32 i = 0; i < 4; i++)
        if (!s_matr[i].ok) {
            s_matr[i].num = num;
            s_matr[i].ok = prs_load_matr(num, &s_matr[i].m);
            return s_matr[i].ok ? &s_matr[i].m : NULL;
        }
    return NULL;
}

static i32 prs_load_matr(i32 num, ome_matr_t *out) {
    c8 vp[32], host[512];
    snprintf(vp, sizeof(vp), "PERSOS>P%02d.OME", num);
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    ome_file_t ome = {0};
    i32 ok = 0;
    if (ls_resolve_path(base, vp, host, (i32)sizeof(host)) && model_ome_open(host, &ome)) {
        ok = ome_load_matr(ome.raw, ome.raw_size, out);
        model_ome_free(&ome);
    }
    return ok;
}

static i32 prs_load_grd(u8 *grd) {
    c8 host[512];
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, "DATAS>MENUS>MENUPERS.GRD", host, (i32)sizeof(host))) return 0;
    FILE *f = ls_open(host, "rb");
    if (!f) return 0;
    i32 ok = fread(grd, 1, 0x4000, f) == 0x4000;
    ls_close(f);
    return ok;
}

/* sub_463A24 from an identity matrix: world position of every node. */
static void prs_node_positions(entity_t *e) {
    fmat_t id;
    fmat_rotation(&id, 0, 0, 0);
    id.t[0] = id.t[1] = id.t[2] = 0;
    actor_node_positions(EROOT(e), &id);
}

/* ---------------------------------------------------------------------- */

void menu_persos_run(void) {
    if (!state || !state->graphic.front_buffer) return;
    i32 fw = state->graphic.width, fh = state->graphic.height;
    u32 *fb = state->graphic.front_buffer;
    system_t *sys = &state->system;
    if (fw != PRS_W || fh != PRS_H) return;

    bmap_t *bg   = game_init_load_cursor("DATAS>MENUS>MENUPERS.GIF");
    bmap_t *img  = game_init_load_cursor("DATAS>MENUS>PERSIMAG.GIF");
    bmap_t *curs = game_init_load_cursor("DATAS>GLOBAL>IMAGES>CURS128.GIF");
    if (!bg || !img || !bg->palette) {
        prs_free_bmap(bg); prs_free_bmap(img); prs_free_bmap(curs);
        return;
    }
    const u8 *pal  = bg->palette;                         /* PERSIMAG uses color_table too */
    const u8 *cpal = (curs && curs->palette) ? curs->palette : pal;
    static u8 grd[0x4000];
    i32 grd_ok = prs_load_grd(grd);                       /* MENUPERS.GRD -> palette_buffer */

    /* The hero: player_entity, or (main menu before a game) a stand-in. */
    entity_t *e = g_player_entity;
    i32 own = 0;
    if (!e) {
        e = actor_create(actor_hero_id());                /* port: no player actor yet */
        own = e != NULL;
    }
    const ome_matr_t *matr = NULL;
    obc_node_t *root = e ? EROOT(e) : NULL;
    i32 save_y = 0, save_yaw = 0, save_light = 0, swapped = 0;
    entity_t *snap = NULL;
    if (root) {
        matr = prs_matr(own ? actor_hero_id() : E32(e, 0xB0));
        save_yaw = root->rot[2];                          /* var_412C */
        save_y = root->pos[1];                            /* var_4010 */
        save_light = E32(e, 0x64);
        /* 0x43FC8D: Arkhan standing idle takes the guard pose g01arr01, its
         * first keyframe, with the right heel (taldro) put on the floor. */
        can_rec_t *cur = (can_rec_t *)EPTR(e, 0xD50);
        /* Port: at the title menu the player exists but was never placed
         * (no clip); pose it like the idle case and put it back as found. */
        if (!own && !cur) {
            snap = (entity_t *)malloc(sizeof(entity_t));
            if (snap) *snap = *e;
        }
        if (own || snap || (E32(e, 0xC40) == 0 && cur && !strncmp((const c8 *)cur->hdr, "p00arr01", 8))) {
            can_rec_t *rec = can_get_anim("g01arr01");
            if (rec) {
                swapped = 1;
                actor_play_anim(e, rec, 0);
                if (rec->frame[0]) actor_set_pose(e, rec->frame[0]);
                prs_node_positions(e);
                obc_node_t *heel = EBONE(e, 0x13);
                if (heel) root->pos[1] += E32(e, 0x8C) - heel->raw_88[3];
            }
        }
        E32(e, 0x64) = 0x80;
    }

    g_gscale = (i32)(10240.0 / tan(0.5431464632206353));   /* sub_49FAB1 */
    if (root) {
        g_eye[0] = root->pos[0];
        g_eye[1] = E32(e, 0x8C) + 57300;                  /* dbl_4B0638 */
        g_eye[2] = root->pos[2] + 258300;                 /* dbl_4B0640 */
    }

    if (g_gamecfg.shadows == 0) g_gamecfg.shadows = 1;   /* 0x440136 */
    i32 *shadow = &g_gamecfg.shadows;
    i32 exit_flag = 0;                                    /* var_411C */
    prs_btn_t btn[6] = {
        { 0x00, 0x00, 0x14, 0x15, 0x244, 0x0C3, shadow, 3, -1, 0 },
        { 0x00, 0x21, 0x14, 0x15, 0x244, 0x0E4, shadow, 2, -1, 0 },
        { 0x00, 0x44, 0x14, 0x15, 0x244, 0x107, shadow, 1, -1, 0 },
        { 0x3C, 0x0E, 0x1F, 0x14, 0x23E, 0x128, shadow, 4, -1, 0 },
        { 0x48, 0x2E, 0x2B, 0x2B, 0x019, 0x08B, &g_control_scheme, 1, 0, 0 },
        { 0x14, 0x2E, 0x33, 0x33, 0x235, 0x172, &exit_flag, 1, 0, 0 },
    };

    sys->motion_x = 0x208; sys->motion_y = 0xE6;          /* ls_set_mouse */
    i32 prev[256];
    memcpy(prev, sys->actions, sizeof(prev));
    #define EDGE(k) (sys->actions[k] && !prev[k])
    u32 opened = ls_tick();
    i32 hit = 0;

    while (!sys->quit) {
        ls_update_events(sys);
        sfx_pump();
        u32 now = ls_tick();
        i32 k_esc = EDGE(27), k_enter = EDGE(13), k_up = EDGE(38), k_dn = EDGE(40);
        i32 k_lr = EDGE(37) || EDGE(39);
        i32 click = sys->mouse_left == 2;
        memcpy(prev, sys->actions, sizeof(prev));
        if (getenv("DKE_FRAMES") && now - opened > 1500) k_esc = 1;   /* headless */
        if (k_esc || k_enter || exit_flag || sys->mouse_right) break;  /* 0x44017F */

        /* 0x4401C6: the hero turns, a full circle per 800 * 2pi ms. */
        if (root) root->rot[2] = (i32)((f64)(now - opened) / 800.0 * 65535.0 / 6.283185307179586);

        i32 old_shadow = *shadow, old_scheme = g_control_scheme;
        hit = prs_hit(btn, 6, sys->motion_x, sys->motion_y, click);
        if (g_control_scheme != old_scheme) sfx_ui_click(0);   /* 0x44025C */
        if (k_lr) { sfx_ui_click(0); g_control_scheme ^= 1; }  /* 0x4402C5 */
        if (*shadow != old_shadow) sfx_ui_click(0);           /* 0x440322 */
        if (k_up) {                                               /* 0x44037A */
            if (*shadow < 3) { sfx_ui_click(0); (*shadow)++; }
            else if (*shadow == 4) { sfx_ui_click(0); *shadow = 1; }
        }
        if (k_dn) {                                               /* 0x440403 */
            if (*shadow > 1 && *shadow != 4) { sfx_ui_click(0); (*shadow)--; }
            else if (*shadow != 4) { sfx_ui_click(0); *shadow = 4; }
        }

        blit_bmap_to_framebuffer(fb, fw, fh, bg, pal, 0, 0);
        i32 level = *shadow;
        if (level == 4) {                                         /* 0x440499 */
            level = 4 - ((i32)(now % 600) * 3 / 600 + 1);
            blit_sub(fb, fw, fh, img, pal, 0x40, 0x0E, 0x1F, 0x14, 0x23E, 0x128);
        }
        memset(g_mask, 0, sizeof(g_mask));
        if (level == 1) {                                         /* 0x440507 */
            blit_sub(fb, fw, fh, img, pal, 0x00, 0x44, 0x14, 0x15, 0x244, 0x107);
            if (root) { prs_node_positions(e); prs_shadow_bones(e); }
        } else if (level == 2 || level == 3) {                    /* 0x44053C / 0x4405FF */
            if (level == 2) blit_sub(fb, fw, fh, img, pal, 0x00, 0x21, 0x14, 0x15, 0x244, 0x0E4);
            else            blit_sub(fb, fw, fh, img, pal, 0x00, 0x00, 0x14, 0x15, 0x244, 0x0C3);
            if (root) {
                /* var_4128: the light, behind and far above the hero. */
                i32 light[3] = { root->pos[0] - 258300, root->pos[1] + 0x124F80, root->pos[2] - 498300 };
                prs_shadow_mesh(e, level == 2 ? 1 : 0, light);
            }
        }
        prs_shadow_apply(fb, bg, pal, grd_ok ? grd : NULL);
        if (root && matr) prs_draw_hero(e, matr, pal, grd_ok ? grd : NULL);   /* 0x4406F3 */
        if (g_control_scheme)                                     /* 0x44071E */
            blit_sub(fb, fw, fh, img, pal, 0x48, 0x2E, 0x2B, 0x2B, 0x19, 0x8B);
        if (curs) {                                               /* 0x440794 */
            if (hit == 0) blit_sub(fb, fw, fh, curs, cpal, 0x1E, 0, 0x20, 0x20, sys->motion_x, sys->motion_y);
            else          blit_sub(fb, fw, fh, curs, cpal, 0, 0, 0x1D, 0x20, sys->motion_x, sys->motion_y);
        }
        prs_shot();
        fx_present();
        ls_delay(10);
    }
    #undef EDGE

    /* 0x44081C: click, the exit button held down for one frame. */
    sfx_ui_click(0);
    blit_sub(fb, fw, fh, img, pal, 0x14, 0x2E, 0x33, 0x33, 0x235, 0x172);
    fx_present();

    if (root) {
        if (snap) {
            *e = *snap;
            free(snap);
        } else if (swapped && !own) {                             /* 0x4409D2 */
            can_rec_t *rec = can_get_anim("p00arr01");
            if (rec) actor_play_anim(e, rec, 0);
            root->pos[1] = save_y;
        }
        root->rot[2] = save_yaw;                                  /* 0x440A75 */
        E32(e, 0x64) = save_light;
    }
    if (own) actor_free(e);
    prs_free_bmap(curs);
    prs_free_bmap(img);
    prs_free_bmap(bg);
}
