/*
 * can.c — CAN character animation loader.
 * See can.h for the format specification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "can.h"

/* -------------------------------------------------------------------------
 * CAN file format constants
 * ------------------------------------------------------------------------- */

#define CAN_PTRTABLE_BYTES  0x1400   /* 640 × 8-byte entries */
#define CAN_PTRTABLE_COUNT  640
#define CAN_ENTRY_SIZE      8        /* hash(4) + offset(4) */

static i32 read_le_u32(const u8 *p) {
    return (i32)((u32)p[0] | ((u32)p[1]<<8) | ((u32)p[2]<<16) | ((u32)p[3]<<24));
}

i32 can_load_first(const c8 *host_path, can_anim_t *out) {
    if (!host_path || !out) return 0;
    memset(out, 0, sizeof(*out));

    FILE *f = ls_open(host_path, "rb");
    if (!f) {
        print("can_load_first: cannot open '%s'\n", host_path);
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long fsz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fsz < CAN_PTRTABLE_BYTES + CAN_HEADER_BYTES) {
        ls_close(f); return 0;
    }

    /* Read ptr table. */
    u8 *ptrtable = (u8 *)malloc((size_t)CAN_PTRTABLE_BYTES);
    if (!ptrtable) { ls_close(f); return 0; }
    if (fread(ptrtable, 1, CAN_PTRTABLE_BYTES, f) != CAN_PTRTABLE_BYTES) {
        free(ptrtable); ls_close(f); return 0;
    }

    /* Find first non-zero entry (offset must be within file). */
    i32 anim_offset = 0, anim_size = 0;
    for (i32 i = 0; i < CAN_PTRTABLE_COUNT; i++) {
        const u8 *e = ptrtable + i * CAN_ENTRY_SIZE;
        i32 hash = read_le_u32(e);
        i32 off  = read_le_u32(e + 4);
        if (hash == 0 || off <= 0 || off >= (i32)fsz) continue;
        /* size = next_offset - this_offset */
        i32 next_off = (i32)fsz;
        for (i32 j = i + 1; j < CAN_PTRTABLE_COUNT; j++) {
            const u8 *ne = ptrtable + j * CAN_ENTRY_SIZE;
            i32 nh = read_le_u32(ne);
            i32 no = read_le_u32(ne + 4);
            if (nh != 0 && no > off && no < (i32)fsz) {
                next_off = no; break;
            }
        }
        anim_offset = off;
        anim_size   = next_off - off;
        break;
    }
    free(ptrtable);

    if (anim_offset == 0 || anim_size < CAN_HEADER_BYTES) {
        ls_close(f); return 0;
    }

    /* Read animation block. */
    u8 *block = (u8 *)malloc((size_t)anim_size);
    if (!block) { ls_close(f); return 0; }
    fseek(f, anim_offset, SEEK_SET);
    if (fread(block, 1, (size_t)anim_size, f) != (size_t)anim_size) {
        free(block); ls_close(f); return 0;
    }
    ls_close(f);

    /* Parse animation header. */
    memcpy(out->name, block, 8);
    out->name[8] = '\0';
    u8 n_frames = block[0x0A];
    if (n_frames == 0 || n_frames > CAN_MAX_FRAMES) n_frames = CAN_MAX_FRAMES;
    out->n_frames = n_frames;

    /* Decode keyframes starting at offset 0x110. */
    for (i32 fi = 0; fi < n_frames; fi++) {
        i32 kf_off = CAN_HEADER_BYTES + fi * CAN_FRAME_BYTES;
        if (kf_off + CAN_FRAME_BYTES > anim_size) break;
        const u8 *kf = block + kf_off;

        can_frame_t *frame = &out->frames[fi];
        /* Header: i16 at [0] */
        frame->header = (i16)((u16)kf[0] | ((u16)kf[1] << 8));
        /* Per-atom rotations: 34 × 6 bytes starting at [2]. */
        for (i32 j = 0; j < CAN_MAX_ATOMSLOTS; j++) {
            const u8 *r = kf + 2 + j * 6;
            frame->atoms[j].rx = (u16)((u16)r[0] | ((u16)r[1] << 8));
            frame->atoms[j].ry = (u16)((u16)r[2] | ((u16)r[3] << 8));
            frame->atoms[j].rz = (u16)((u16)r[4] | ((u16)r[5] << 8));
        }
    }

    free(block);
    print("can_load_first: '%s' anim='%s' n_frames=%d\n",
          host_path, out->name, (i32)out->n_frames);
    return 1;
}

/* -------------------------------------------------------------------------
 * can_name_hash — clip-name hash matching engine sub_414170. Uppercases the
 * name, subtracts '0', weights char 0 then chars 3..7 by 32^pos. Chars 1..2
 * (the persona number) are skipped, so hash("p00arr01")==hash("p12arr01")
 * (i.e. "ARR01" = arrival clip across any P<nn>.CAN).
 * ---------------------------------------------------------------------- */
static u32 can_name_hash(const c8 *name) {
    c8 u[16];
    i32 i;
    u32 h = 0, mult = 1;
    for (i = 0; i < 15 && name[i]; i++) u[i] = (c8)toupper((unsigned char)name[i]);
    u[i] = '\0';
    h += (u32)((u8)u[0] - '0') * mult; mult <<= 5;
    for (i = 3; i < 8 && u[i]; i++) { h += (u32)((u8)u[i] - '0') * mult; mult <<= 5; }
    return h;
}

/* -------------------------------------------------------------------------
 * can_load_named — load the clip whose name hashes to `clip_name` (e.g.
 * "p12arr01"); falls back to the first non-empty clip if no hash match.
 * Mirrors sub_41DE86's hash lookup. Returns 1 on success, 0 on failure.
 * ---------------------------------------------------------------------- */
i32 can_load_named(const c8 *host_path, const c8 *clip_name, can_anim_t *out) {
    if (!host_path || !out) return 0;
    memset(out, 0, sizeof(*out));

    FILE *f = ls_open(host_path, "rb");
    if (!f) { print("can_load_named: cannot open '%s'\n", host_path); return 0; }
    fseek(f, 0, SEEK_END);
    long fsz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fsz < CAN_PTRTABLE_BYTES + CAN_HEADER_BYTES) { ls_close(f); return 0; }

    u8 *ptrtable = (u8 *)malloc((size_t)CAN_PTRTABLE_BYTES);
    if (!ptrtable) { ls_close(f); return 0; }
    if (fread(ptrtable, 1, CAN_PTRTABLE_BYTES, f) != CAN_PTRTABLE_BYTES) {
        free(ptrtable); ls_close(f); return 0;
    }

    u32 want = clip_name ? can_name_hash(clip_name) : 0;

    /* Pass 1: hash match; Pass 2 (fallback): first non-zero entry. */
    i32 anim_offset = 0, anim_size = 0;
    for (i32 pass = 0; pass < 2 && anim_offset == 0; pass++) {
        for (i32 i = 0; i < CAN_PTRTABLE_COUNT; i++) {
            const u8 *e = ptrtable + i * CAN_ENTRY_SIZE;
            u32 hash = (u32)read_le_u32(e);
            i32 off  = read_le_u32(e + 4);
            if (hash == 0 || off <= 0 || off >= (i32)fsz) continue;
            if (pass == 0 && hash != want) continue;   /* pass 0: exact hash */
            i32 next_off = (i32)fsz;
            for (i32 j = i + 1; j < CAN_PTRTABLE_COUNT; j++) {
                const u8 *ne = ptrtable + j * CAN_ENTRY_SIZE;
                i32 nh = read_le_u32(ne);
                i32 no = read_le_u32(ne + 4);
                if (nh != 0 && no > off && no < (i32)fsz) { next_off = no; break; }
            }
            anim_offset = off;
            anim_size   = next_off - off;
            break;
        }
    }
    free(ptrtable);
    if (anim_offset == 0 || anim_size < CAN_HEADER_BYTES) { ls_close(f); return 0; }

    u8 *block = (u8 *)malloc((size_t)anim_size);
    if (!block) { ls_close(f); return 0; }
    fseek(f, anim_offset, SEEK_SET);
    if (fread(block, 1, (size_t)anim_size, f) != (size_t)anim_size) {
        free(block); ls_close(f); return 0;
    }
    ls_close(f);

    memcpy(out->name, block, 8);
    out->name[8] = '\0';
    u8 n_frames = block[0x0A];
    if (n_frames == 0 || n_frames > CAN_MAX_FRAMES) n_frames = CAN_MAX_FRAMES;
    out->n_frames = n_frames;
    for (i32 fi = 0; fi < n_frames; fi++) {
        i32 kf_off = CAN_HEADER_BYTES + fi * CAN_FRAME_BYTES;
        if (kf_off + CAN_FRAME_BYTES > anim_size) break;
        const u8 *kf = block + kf_off;
        can_frame_t *frame = &out->frames[fi];
        frame->header = (i16)((u16)kf[0] | ((u16)kf[1] << 8));
        for (i32 j = 0; j < CAN_MAX_ATOMSLOTS; j++) {
            const u8 *r = kf + 2 + j * 6;
            frame->atoms[j].rx = (u16)((u16)r[0] | ((u16)r[1] << 8));
            frame->atoms[j].ry = (u16)((u16)r[2] | ((u16)r[3] << 8));
            frame->atoms[j].rz = (u16)((u16)r[4] | ((u16)r[5] << 8));
        }
    }
    free(block);
    print("can_load_named: '%s' clip='%s' -> anim='%s' n_frames=%d\n",
          host_path, clip_name ? clip_name : "(first)", out->name, (i32)out->n_frames);
    return 1;
}

/* =========================================================================
 * Engine animation cache — get_animation_id @ 0x41E26B, sub_41DE86 @ 0x41DE86
 * ========================================================================= */

#include "state.h"

static can_rec_t g_anim_cache[CAN_REC_SLOTS];   /* dword_511744 */
static i32       g_anim_cursor = 0;             /* dword_4B2B9C */

/* sub_41DE86 keeps the 0x1400-byte offset tables of the last four CAN files
 * it opened (dword_511730 tables, dword_511780 file numbers, dword_511750
 * last-use times). */
static u8  g_can_table[4][CAN_PTRTABLE_BYTES];
static i32 g_can_table_num[4] = { -1, -1, -1, -1 };
static u32 g_can_table_used[4];
static u32 g_can_clock = 0;

static void anim_rec_clear(can_rec_t *r) {
    for (i32 i = 0; i < CAN_MAX_FRAMES; i++) {
        free(r->frame[i]); r->frame[i] = NULL;
        free(r->mes[i]);   r->mes[i]   = NULL;
    }
    memset(r->hdr, 0, sizeof(r->hdr));
}

void can_cache_reset(void) {
    for (i32 i = 0; i < CAN_REC_SLOTS; i++) anim_rec_clear(&g_anim_cache[i]);
    g_anim_cursor = 0;
}

/* sub_41DE86 — load clip `name` from "ANM>P<name[1..2]>.CAN" into `r`. */
static i32 can_load_rec(const c8 *name, can_rec_t *r) {
    c8 num[3] = { name[1], name[2], 0 };
    i32 file_num = atoi(num);

    c8 vpath[32], host[512];
    snprintf(vpath, sizeof(vpath), "ANM>P%s.CAN", num);
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    if (!ls_resolve_path(base, vpath, host, (i32)sizeof(host))) return 0;
    FILE *f = ls_open(host, "rb");
    if (!f) return 0;

    /* Offset table: reuse a cached copy, else read it into the least
     * recently used of the four slots. */
    g_can_clock++;
    i32 t;
    for (t = 0; t < 4; t++)
        if (g_can_table_num[t] == file_num) break;
    if (t < 4) {
        g_can_table_used[t] = g_can_clock;
    } else {
        t = 0;
        for (i32 i = 1; i < 4; i++)
            if (g_can_table_used[i] < g_can_table_used[t]) t = i;
        if (fread(g_can_table[t], 1, CAN_PTRTABLE_BYTES, f) != CAN_PTRTABLE_BYTES) {
            ls_close(f);
            return 0;
        }
        g_can_table_num[t]  = file_num;
        g_can_table_used[t] = g_can_clock;
    }

    /* Entry search by sub_414170 hash; the size is the gap to the NEXT
     * table entry (asm: [e+0x0C] - [e+4]). */
    const u32 want = can_name_hash(name);
    const u8 *tab  = g_can_table[t];
    i32 off = -1, size = 0;
    for (i32 i = 0; i < CAN_PTRTABLE_COUNT; i++) {
        const u8 *e = tab + i * CAN_ENTRY_SIZE;
        if ((u32)read_le_u32(e) != want) continue;
        off  = read_le_u32(e + 4);
        size = (i + 1 < CAN_PTRTABLE_COUNT) ? read_le_u32(e + 12) - off : 0;
        break;
    }
    if (off < 0 || size < CAN_REC_HDR) { ls_close(f); return 0; }

    u8 *block = (u8 *)malloc((size_t)size);
    if (!block) { ls_close(f); return 0; }
    fseek(f, off, SEEK_SET);
    i32 got = (i32)fread(block, 1, (size_t)size, f);
    ls_close(f);
    if (got < CAN_REC_HDR) { free(block); return 0; }

    memcpy(r->hdr, block, CAN_REC_HDR);
    const u8 *p = block + CAN_REC_HDR;
    i32 nf = CAN_NFRAMES(r), nm = CAN_NMES(r);
    if (nf < 0 || nf > CAN_MAX_FRAMES) nf = 0;
    if (nm < 0 || nm > CAN_MAX_FRAMES) nm = 0;
    for (i32 i = 0; i < nf; i++, p += CAN_FRAME_BYTES) {
        r->frame[i] = (i16 *)calloc(1, CAN_FRAME_BYTES);
        if (r->frame[i] && p + CAN_FRAME_BYTES <= block + got)
            memcpy(r->frame[i], p, CAN_FRAME_BYTES);
    }
    for (i32 i = 0; i < nm; i++, p += CAN_MES_BYTES) {
        r->mes[i] = (u8 *)calloc(1, CAN_MES_BYTES);
        if (r->mes[i] && p + CAN_MES_BYTES <= block + got)
            memcpy(r->mes[i], p, CAN_MES_BYTES);
    }
    free(block);
    return 1;
}

can_rec_t *can_get_anim(const c8 *name_in) {
    c8 name[16];
    snprintf(name, sizeof(name), "%s", (name_in && name_in[0]) ? name_in : "g01arr01");

    /* Hit: a record more than 100 requests old is moved to the cursor so it
     * survives the next evictions; the old slot is wiped (41E3EA). */
    for (i32 i = 0; i < CAN_REC_SLOTS; i++) {
        can_rec_t *r = &g_anim_cache[i];
        if (memcmp(r->hdr, name, 4) != 0 || strcmp((const c8 *)r->hdr, name) != 0)
            continue;
        i32 age = g_anim_cursor - i;
        if (age < 0) age += CAN_REC_SLOTS;
        if (age < 100) return r;
        can_rec_t *dst = &g_anim_cache[g_anim_cursor];
        if (dst != r) {
            anim_rec_clear(dst);
            *dst = *r;
            memset(r, 0, sizeof(*r));
        }
        if (++g_anim_cursor == CAN_REC_SLOTS) g_anim_cursor = 0;
        return dst;
    }

    /* Miss: free the oldest occupied record at least 110 requests back, then
     * take the slot under the cursor (41E44D..41E683). */
    for (i32 j = 110; j < CAN_REC_SLOTS; j++) {
        i32 idx = g_anim_cursor - j;
        if (idx < 0) idx += CAN_REC_SLOTS;
        if (g_anim_cache[idx].hdr[0]) { anim_rec_clear(&g_anim_cache[idx]); break; }
    }
    can_rec_t *r = &g_anim_cache[g_anim_cursor];
    anim_rec_clear(r);
    if (++g_anim_cursor == CAN_REC_SLOTS) g_anim_cursor = 0;

    if (name[0] != 'p' && name[0] != 'g') {
        print("can: Animation bizarre : %s ne commence pas par p ou g\n", name);
        return NULL;
    }
    if (!can_load_rec(name, r)) {
        print("can: Anim %s pas trouve -> g01arr01\n", name);
        anim_rec_clear(r);
        if (!can_load_rec("g01arr01", r)) return NULL;
    }
    return r;
}
