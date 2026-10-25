/*
 * anim2d.c — pre-rendered backdrop animations (see anim2d.h).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "state.h"
#include "anim2d.h"

/* -------------------------------------------------------------------------
 * LZSS decompressor — direct port of sub_4780B4.
 *
 * Classic LZSS variant:
 *   Ring buffer: 4096 bytes (0x1000), initialized to 0x20 (space).
 *   Initial write position: 0xFEE (4078).
 *   Input starts with a 4-byte LE uncompressed-count (= remaining output
 *   bytes to produce); this is the loop counter, not stored in output.
 *   Flag byte: 8 bits, LSB first.
 *     bit = 1 → literal: copy one byte to output + ring buf, advance pos.
 *     bit = 0 → back-ref: read 2 bytes (lo, hi).
 *                  ring_offset  = lo | ((hi >> 4) << 8)   (12-bit)
 *                  run_length   = (hi & 0xF) + 3          (3..18)
 *                  Copy run_length bytes from ring[ring_offset..].
 * ------------------------------------------------------------------------- */

#define LZSS_BUFSIZE 0x1000
#define LZSS_INITPOS 0x0FEE
#define LZSS_FILL    0x20

i32 lzss_decompress(const u8 *src, i32 src_size, u8 *dst, i32 dst_size) {
    if (!src || src_size < 4 || !dst || dst_size <= 0) return 0;
    u8 ring[LZSS_BUFSIZE];
    memset(ring, LZSS_FILL, sizeof(ring));
    i32 count = (i32)((u32)src[0] | ((u32)src[1] << 8) | ((u32)src[2] << 16) | ((u32)src[3] << 24));
    if (count > dst_size) count = dst_size;
    const u8 *s = src + 4, *end = src + src_size;
    u8 *d = dst;
    i32 r = LZSS_INITPOS;
    u32 flags = 0;
    while (count > 0 && s < end) {
        flags >>= 1;
        if (!(flags & 0x100)) flags = (u32)*s++ | 0xFF00u;   /* 8 flags, LSB first */
        if (flags & 1) {                                      /* literal */
            if (s >= end) break;
            u8 c = *s++;
            *d++ = c; ring[r] = c; r = (r + 1) & 0xFFF;
            count--;
        } else {                                              /* back-reference */
            if (s + 1 >= end) break;
            i32 lo = *s++, hi = *s++;
            i32 pos = lo | ((hi & 0xF0) << 4), run = (hi & 0x0F) + 3;
            for (i32 k = 0; k < run && count > 0; k++) {
                u8 c = ring[(pos + k) & 0xFFF];
                *d++ = c; ring[r] = c; r = (r + 1) & 0xFFF;
                count--;
            }
        }
    }
    return (i32)(d - dst);
}

static i32 rd16(const u8 *p) { return (i16)((u32)p[0] | ((u32)p[1] << 8)); }
static i32 rd32(const u8 *p) { return (i32)((u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24)); }

i32 ani_load(const c8 *host_path, ani_t *out) {                /* sub_41333A */
    if (!host_path || !out) return 0;
    memset(out, 0, sizeof(*out));
    FILE *f = ls_open(host_path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 12 || sz > 16 * 1024 * 1024) { ls_close(f); return 0; }
    u8 *data = (u8 *)malloc((size_t)sz);
    if (!data || fread(data, 1, (size_t)sz, f) != (size_t)sz) { free(data); ls_close(f); return 0; }
    ls_close(f);

    i32 n = data[sz - 1];
    if (n > ANI_MAX_FRAMES) n = ANI_MAX_FRAMES;
    out->fps = n; out->mode = 0; out->overlay = 0; out->depth = -5000000;
    if (data[sz - 2] == 1) {                                     /* extended footer */
        out->mode    = data[sz - 12];
        out->overlay = data[sz - 11];
        i32 dk = rd32(data + sz - 10);
        out->depth   = dk ? dk : -5000000;                       /* 0x4137B1 */
        out->fps     = data[sz - 6] ? data[sz - 6] : n;
    }
    i32 pos = 0;
    for (i32 i = 0; i < n; i++) {
        if (pos + 16 > sz) break;
        ani_frame_t *fr = &out->frames[i];
        i32 x1 = rd16(data + pos), y1 = rd16(data + pos + 2);
        i32 x2 = rd16(data + pos + 4), y2 = rd16(data + pos + 6);
        i32 us = rd32(data + pos + 8), cs = rd32(data + pos + 12);
        pos += 16;
        if (cs < 0 || pos + cs > sz) break;
        i32 w = x2 - x1, h = y2 - y1;                            /* 0x413617 */
        if (w > 0 && h > 0 && us >= w * h) {
            u8 *raw = (u8 *)malloc((size_t)us);
            if (raw) {
                if (us == cs) memcpy(raw, data + pos, (size_t)us);
                else          lzss_decompress(data + pos, cs, raw, us);
                /* 0x4136CD: h rows of w bytes, row stride w. */
                fr->pixels = (u8 *)realloc(raw, (size_t)(w * h));
                if (!fr->pixels) fr->pixels = raw;
                fr->x = (i16)x1; fr->y = (i16)y1; fr->w = (i16)w; fr->h = (i16)h;
            }
        }
        pos += cs;
        out->n_frames = i + 1;
    }
    free(data);
    return out->n_frames > 0;
}

void ani_free(ani_t *ani) {
    if (!ani) return;
    for (i32 i = 0; i < ANI_MAX_FRAMES; i++) free(ani->frames[i].pixels);
    memset(ani, 0, sizeof(*ani));
}

/* -------------------------------------------------------------------------
 * Slots (anim_state_531BE0, 31 x 0x944)
 * ------------------------------------------------------------------------- */

typedef struct {
    ani_t ani;
    i32   active;    /* +0x70  */
    i32   frame;     /* +0x91C */
    u32   last;      /* +0x920 */
    i32   enabled;   /* +0x93C */
    i32   loop;      /* +0x940 */
} anim2d_slot_t;

static anim2d_slot_t g_slots[ANIM2D_SLOTS];
static c8  g_named[20][16];          /* byte_505AA0 */
static i32 g_named_n = 0;            /* dword_505A90 */

static void slot_clear(anim2d_slot_t *s) {
    ani_free(&s->ani);
    memset(s, 0, sizeof(*s));
}

static i32 slot_load(anim2d_slot_t *s, const c8 *vpath) {
    slot_clear(s);
    c8 host[512];
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, vpath, host, (i32)sizeof(host))) return 0;
    if (!ani_load(host, &s->ani)) return 0;
    s->active = 1;
    s->last = state ? state->current_game_time : 0;
    return 1;
}

void anim2d_load_camera(i32 level, i32 camera) {               /* sub_4130FB */
    for (i32 i = 0; i < ANIM2D_SLOTS; i++) slot_clear(&g_slots[i]);
    /* Prefix 'N' (night) is used only when N%02d.DAT exists; level files
     * ship 'L' only, so the day set is loaded. */
    for (i32 i = 0; i < ANIM2D_SLOTS; i++) {
        c8 vpath[64];
        snprintf(vpath, sizeof(vpath), "DATAS>L%02d>ANIM2D>L%02dC%1dA%1d.ANI",
                 level, level, camera, i + 1);
        if (!slot_load(&g_slots[i], vpath)) break;
        g_slots[i].enabled = 1;                                 /* +0x93C */
        g_slots[i].loop = 1;                                    /* +0x940 */
    }
    /* sub_44313F: the named anims come back in slots 10.., idle. */
    for (i32 k = 0; k < g_named_n && 10 + k < ANIM2D_SLOTS; k++) {
        if (!anim2d_load_slot(10 + k, level, g_named[k], 0)) continue;
        g_slots[10 + k].frame = 0;
        g_slots[10 + k].enabled = 0;
    }
}

i32 anim2d_register_named(i32 level, const c8 *name) {
    if (!name) return -1;
    i32 k = 0;
    while (k < g_named_n && strcmp(g_named[k], name) != 0) k++;
    if (k == g_named_n) {
        if (g_named_n >= 20) { print("anim2d: too many anim2d\n"); return -1; }
        snprintf(g_named[g_named_n++], sizeof(g_named[0]), "%s", name);
    }
    anim2d_load_slot(30, level, name, 2);
    return k;
}

/* sub_44313F: byte_505AA0[n] = name, then name.ANI into slot 10 + n,
 * frame 0, mode 0, not shown (+0x91C / +0x934 / +0x93C = 0). */
void anim2d_load_named(i32 level, i32 n, const c8 *name) {
    if (n < 0 || n >= 20 || 10 + n >= ANIM2D_SLOTS || !name) return;
    snprintf(g_named[n], sizeof(g_named[0]), "%s", name);
    if (n >= g_named_n) g_named_n = n + 1;
    if (!anim2d_load_slot(10 + n, level, name, 0)) return;
    g_slots[10 + n].frame = 0;
    g_slots[10 + n].enabled = 0;
}

i32 anim2d_load_slot(i32 slot, i32 level, const c8 *name, i32 mode) {
    if (slot < 0 || slot >= ANIM2D_SLOTS || !name) return 0;
    c8 vpath[96];
    snprintf(vpath, sizeof(vpath), "DATAS>L%02d>ANIM2D>%s.ANI", level, name);
    if (!slot_load(&g_slots[slot], vpath)) return 0;
    g_slots[slot].ani.mode = mode;
    g_slots[slot].enabled = 1;
    return 1;
}

void anim2d_start(i32 slot, i32 loop) {                        /* case 35 */
    if (slot < 0 || slot >= ANIM2D_SLOTS) return;
    anim2d_slot_t *s = &g_slots[slot];
    s->frame = 0; s->enabled = 1; s->ani.mode = 0; s->loop = loop;
}

void anim2d_set_frame(i32 slot, i32 frame, i32 hold) {         /* case 36 */
    if (slot < 0 || slot >= ANIM2D_SLOTS) return;
    anim2d_slot_t *s = &g_slots[slot];
    if (hold == -1) { s->enabled = 0; return; }
    s->frame = (s->ani.n_frames > 0 && frame >= s->ani.n_frames) ? s->ani.n_frames - 1 : frame;
    s->enabled = 1;
    if (hold == 0) s->ani.mode = 2;
}

static void blit_frame(u32 *fb, i32 fb_w, i32 fb_h, const u8 *pal,
                       const ani_frame_t *fr, i32 keyed) {
    if (!fr->pixels) return;
    for (i32 y = 0; y < fr->h; y++) {
        i32 sy = fr->y + y;
        if ((u32)sy >= (u32)fb_h) continue;
        const u8 *row = fr->pixels + (size_t)y * fr->w;
        for (i32 x = 0; x < fr->w; x++) {
            i32 sx = fr->x + x;
            if ((u32)sx >= (u32)fb_w) continue;
            u8 c = row[x];
            if (keyed && c == 0) continue;
            fb[sy * fb_w + sx] = 0xFF000000u | ((u32)pal[3 * c + 2] << 16)
                                             | ((u32)pal[3 * c + 1] << 8) | pal[3 * c];
        }
    }
}

void anim2d_draw(u32 *fb, i32 fb_w, i32 fb_h, const u8 *pal, u32 now_ms) {   /* sub_413857 */
    if (!fb || !pal) return;
    for (i32 i = 0; i < ANIM2D_SLOTS; i++) {
        anim2d_slot_t *s = &g_slots[i];
        if (!s->active || !s->enabled || s->ani.n_frames <= 0) continue;
        if (s->frame >= s->ani.n_frames) s->frame = s->ani.n_frames - 1;
        /* The frame drawn is the one current before the advance. */
        blit_frame(fb, fb_w, fb_h, pal, &s->ani.frames[s->frame], s->ani.overlay);
        if (s->ani.fps > 0 && now_ms - s->last > (u32)(1000 / s->ani.fps)) {
            if (s->frame == s->ani.n_frames - 1) {
                if (s->ani.mode == 0 && s->loop == 1) s->frame = 0;
            } else if (s->ani.mode != 2) {
                s->frame++;
            }
            s->last = now_ms;
        }
    }
}

void anim2d_release(void) {
    for (i32 i = 0; i < ANIM2D_SLOTS; i++) slot_clear(&g_slots[i]);
    g_named_n = 0;    /* sub_448F4D clears the list per room */
}
