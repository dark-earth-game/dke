#pragma once

/*
 * anim2d.h — pre-rendered backdrop animations ("anim2D": torches, flames,
 * swinging lanterns), DATAS>Lnn>ANIM2D>*.ANI.
 *
 * ANI file (loader sub_41333A @ 0x41333A):
 *   frames back to back from offset 0, each:
 *     i16 x1, y1, x2, y2     screen rect on the 640x480 backdrop; x2/y2 are
 *                            exclusive: w = x2 - x1, h = y2 - y1
 *     u32 usize, u32 csize   usize = (w+1)*(h+1) as authored; the engine
 *                            keeps h rows of w bytes and ignores the rest
 *     csize bytes            raw when usize == csize, else LZSS (sub_4780B4)
 *   pixels index the backdrop palette; every frame is a full patch of its
 *   rect. Footer (last 12 bytes, extended when byte[-2] == 1):
 *     [-1] frame count (<= 60)    [-6] fps (0 = frame count)
 *     [-10..-7] i32 sort key (0 = -5000000, behind every actor)
 *     [-11] overlay: 1 = colour 0 transparent, 0 = opaque
 *     [-12] mode: 0 loop, 1 play once, 2 frozen
 *
 * Runtime (anim_state_531BE0): 31 slots. A camera load (sub_4130FB) fills
 * slots 0.. with "%c%02dC%1dA%1d.ANI" (prefix 'L', level, camera, i+1) up to
 * the first missing file; slots 10.. hold script-named anims, slot 30 the
 * one-shot ones (named '2...' objects, animation event 0x1A).
 */

#include "libsys/common.h"

#define ANI_MAX_FRAMES  60
#define ANIM2D_SLOTS    31

typedef struct {
    i16 x, y, w, h;          /* screen rect */
    u8 *pixels;              /* w * h palette indices, heap-owned */
} ani_frame_t;

typedef struct {
    ani_frame_t frames[ANI_MAX_FRAMES];
    i32         n_frames;
    i32         fps;         /* +0x930 */
    i32         mode;        /* +0x934: 0 loop, 1 once, 2 frozen */
    i32         overlay;     /* +0x92C */
    i32         depth;       /* +0x938 -> entity +0x98 */
} ani_t;

/* ani_load — decode an ANI file. Returns 1 on success; free with ani_free. */
i32  ani_load(const c8 *host_path, ani_t *out);
void ani_free(ani_t *ani);

/* lzss_decompress — sub_4780B4 (first 4 bytes = output count). Returns the
 * number of bytes written. */
i32 lzss_decompress(const u8 *src, i32 src_size, u8 *dst, i32 dst_size);

/* sub_4130FB @ 0x4130FB: reload the camera's animations. */
void anim2d_load_camera(i32 level, i32 camera);

/* Load one ANI by file name (without extension) into `slot`, enabled with
 * the given mode (script-named anims, animation events). */
i32  anim2d_load_slot(i32 slot, i32 level, const c8 *name, i32 mode);

/* sub_44453E: a script animation named '2...' joins the named list
 * (byte_505AA0, max 20; slot 10 + index after the next camera load) and is
 * shown at once in slot 30, frozen on frame 0. Returns the list index. */
i32  anim2d_register_named(i32 level, const c8 *name);

/* LoadAnim (sub_44313F): named slot n <- name.ANI, loaded but not shown. */
void anim2d_load_named(i32 level, i32 n, const c8 *name);

/* Script control (sub_44A58B cases 35 / 36). */
void anim2d_start(i32 slot, i32 loop);
void anim2d_set_frame(i32 slot, i32 frame, i32 hold);

/* sub_413857 @ 0x413857: advance (by the clip's fps against now_ms) and
 * blit every enabled slot onto the 32-bit framebuffer. */
void anim2d_draw(u32 *fb, i32 fb_w, i32 fb_h, const u8 *pal, u32 now_ms);

void anim2d_release(void);
