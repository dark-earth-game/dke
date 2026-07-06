/*
 * can.c — CAN character animation loader.
 * See can.h for the format specification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
