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
