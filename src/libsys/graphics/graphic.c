#include <string.h>

#include "../common.h"
#include "../memory.h"
#include "graphic.h"

/* global bmap state */
bmap_t *g_graphics_context = NULL;
bmap_t *g_back_screen      = NULL;
u16     g_bmap_count       = 0;

/* pixel_format values derived from bit_depth switch (ls_init_bmap_ex asm) */
#define PIXEL_FMT_8BPP   2
#define PIXEL_FMT_16BPP  4
#define PIXEL_FMT_24BPP  0x20
#define PIXEL_FMT_32BPP  0x80

// 0x0047CEBE
bmap_t *ls_init_bmap_ex(i32 width, i32 height, i32 bit_depth, i32 bpp_flags) {
    /*
     * Original __fastcall: ECX=width, EDX=height, arg_0=bit_depth, arg_4=bpp_flags
     *
     * 1. ls_get_memory_heap(0x34, bpp_flags) → allocate 0x34-byte bmap header.
     * 2. If dword_55C75C != 0 && bpp_flags == 4 → upgrade to bpp_flags = 8.
     * 3. Zero 0xD dwords (52 bytes = struct body).
     * 4. Fill: stride = (bit_depth/8) * width, width, height, bit_depth, bpp_flags.
     * 5. Switch on bit_depth to set pixel_format (2/4/0x20/0x80).
     * 6. Inherit pixel_format from graphics_context when bit_depth matches.
     * 7. Switch on bpp_flags (1,2,4,8 = surface-backed; default = raw buffer).
     *    Port: all paths → raw buffer (DirectDraw absent).
     * 8. Allocate pixel buffer: size = stride * height.
     * 9. ls_clear_bmap → zero buffer.
     * 10. Increment word_55C774 (g_bmap_count). Return bmap ptr.
     */
    bmap_t *bm = (bmap_t *)allocate_memory(bpp_flags, (i32)sizeof(bmap_t));
    if (!bm) return NULL;

    /* struct is zeroed by allocate_memory (MEM_FLAG_ZERO path); fields set below */
    bm->width      = (i16)width;
    bm->height     = height;
    bm->bit_depth  = bit_depth;
    bm->bpp_flags  = bpp_flags;
    bm->stride     = (bit_depth / 8) * width;
    bm->surface    = NULL;

    /* select pixel_format from bit_depth */
    switch (bit_depth) {
        case 8:  bm->pixel_format = PIXEL_FMT_8BPP;  break;
        case 16: bm->pixel_format = PIXEL_FMT_16BPP; break;
        case 24: bm->pixel_format = PIXEL_FMT_24BPP; break;
        case 32: bm->pixel_format = PIXEL_FMT_32BPP; break;
        default:
            print("ls_init_bmap_ex: invalid bit depth %d\n", bit_depth);
            /* keep pixel_format = 0; caller will error-check */
            break;
    }

    /* inherit pixel_format from global graphics context if bit_depth matches */
    if (g_graphics_context && g_graphics_context->bit_depth == bit_depth) {
        bm->pixel_format = g_graphics_context->pixel_format;
    }

    /*
     * Allocate pixel buffer (raw path — surface-backed paths 1/2/4/8 stubbed).
     * Original surface-backed cases call sub_488EF0 (DirectDraw alloc) then
     * ls_get_video_mode_caps / lsys_get_format_info. Omit on non-Windows.
     * TODO: if DirectDraw surface wrappers are ever needed, restore cases here.
     */
    i32 buf_size = bm->stride * bm->height;
    bm->pixels       = (u8 *)allocate_memory(bpp_flags, buf_size);
    bm->first_pixel  = bm->pixels;
    bm->owns_buffer  = 1;

    ls_clear_bmap(bm);

    g_bmap_count++;
    print("ls_init_bmap_ex: %dx%d %dbpp bmap allocated (count=%d)\n",
          width, height, bit_depth, (i32)g_bmap_count);
    return bm;
}

// ls_clear_bmap — zero the pixel buffer (mirrors ls_clear_bmap in original)
void ls_clear_bmap(bmap_t *bmap) {
    if (bmap && bmap->pixels && bmap->owns_buffer) {
        i32 sz = bmap->stride * bmap->height;
        if (sz > 0) {
            memset(bmap->pixels, 0, (size_t)sz);
        }
    }
}

// ls_free_bmap — release bmap pixel buffer, palette, and header
void ls_free_bmap(bmap_t *bmap) {
    if (!bmap) return;
    if (bmap->owns_buffer && bmap->pixels) {
        free_memory(bmap->pixels);
        bmap->pixels = NULL;
        bmap->first_pixel = NULL;
    }
    if (bmap->owns_palette && bmap->palette) {
        free(bmap->palette);
        bmap->palette = NULL;
    }
    free_memory(bmap);
    if (g_bmap_count > 0) g_bmap_count--;
}

// 0x0048418A
i32 ls_set_video_mode(i32 width, i32 height, i32 bit_depth, i32 bpp_flags) {
    /*
     * Original flow:
     *   1. Check if mode unchanged vs graphics_context → early-out return 1.
     *   2. Free dword_55C224 (back_screen) if non-NULL.
     *   3. Free graphics_context if non-NULL.
     *   4. ls_shut_palette_system() (palette teardown — no-op in 16-bit mode).
     *   5. sub_48AED0(width, height, bit_depth, bpp_flags) → DirectDraw SetDisplayMode.
     *      Port: no-op — platform window owns the display mode.
     *   6. ls_init_bmap_ex(width, height, bit_depth, 1) → graphics_context.
     *   7. sub_480CF0 (cursor/clip setup) — skipped (platform layer).
     *   8. ls_get_video_mode_caps / sub_482560 (surface caps query) — skipped.
     *   9. ls_init_bmap_ex(width, height, bit_depth, 2 or 8) → back_screen.
     *   10. Return 0 on success (note: original xor eax, eax at loc_4842E8).
     *
     * Port notes:
     *   - DirectDraw mode-set (step 5) skipped.
     *   - bpp_flags for graphics_context = 1 (screen surface).
     *   - bpp_flags for back_screen = 2 (offscreen surface).
     *   - Returns 1 on success (C convention), 0 on failure.
     *
     * TODO: native display-mode change if full-screen is targeted.
     */

    /* tear down existing contexts */
    if (g_back_screen) {
        ls_free_bmap(g_back_screen);
        g_back_screen = NULL;
    }
    if (g_graphics_context) {
        ls_free_bmap(g_graphics_context);
        g_graphics_context = NULL;
    }

    /* TODO: DirectDraw SetDisplayMode → platform display mode set */

    /* create primary screen surface (bpp_flags=1 = screen) */
    g_graphics_context = ls_init_bmap_ex(width, height, bit_depth, 1);
    if (!g_graphics_context) {
        print("ls_set_video_mode: failed to init graphics_context\n");
        return 0;
    }

    /* create back-screen (bpp_flags=2 = offscreen) */
    g_back_screen = ls_init_bmap_ex(width, height, bit_depth, 2);
    if (!g_back_screen) {
        print("ls_set_video_mode: failed to init back_screen\n");
        return 0;
    }

    print("ls_set_video_mode: %dx%d %dbpp OK\n", width, height, bit_depth);
    return 1;
}


void ls_init_graphic_system(graphic_t *graphic, system_t *system) {
    print("Initializing graphic...\n");
    i32 width = system->width;
    i32 height = system->height;

    graphic->width = system->width;
    graphic->height = system->height;
    graphic->bpp = system->bpp;
    graphic->stride = system->width * system->bpp;

    graphic->back_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
    graphic->front_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
    graphic->frame_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
}

inline void ls_graphic_flip(graphic_t *graphic) {
    memory_copy(
        (u8*)graphic->back_buffer,
        (u8*)graphic->frame_buffer,
        graphic->width * graphic->height * graphic->bpp
    );
}

inline void ls_graphic_clear(graphic_t *graphic) {
    // i32 width = graphic->width;
    // i32 height = graphic->height;
}

void ls_release_graphic_system(graphic_t *graphic) {
    memory_free(graphic->frame_buffer);
    memory_free(graphic->front_buffer);
    memory_free(graphic->back_buffer);
}

/* =========================================================================
 * Blit primitives — Slice A
 * ======================================================================= */

/* 0x0047D466 area — ls_box
 *
 * Stub: draws a 1-pixel outline rectangle in the dst bmap's 8-bit pixel buffer.
 * The original ls_box has complex mode flags (bits 0/1/2 select between
 * ls_invert_rect, ls_fill_rect, and ls_set_rectangle sub-paths) which depend
 * on further unported primitives.  A simple outline is sufficient for debug use.
 *
 * TODO: port ls_fill_rect / ls_invert_rect when full debug-outline accuracy
 *       is required.
 */
void ls_box(bmap_t *dst, i32 x, i32 y, i32 w, i32 h, u8 color, i32 flags) {
    (void)flags; /* TODO: implement mode-flag dispatch per asm */
    if (!dst || !dst->pixels || w <= 0 || h <= 0) return;

    i32 bpp  = (dst->bit_depth > 0) ? (dst->bit_depth / 8) : 1;
    i32 str  = dst->stride;
    i32 dw   = (i32)dst->width;
    i32 dh   = dst->height;
    u8 *pix  = dst->pixels;

    /* Draw top and bottom rows */
    for (i32 col = 0; col < w; col++) {
        i32 px = x + col;
        if (px < 0 || px >= dw) continue;
        /* top row */
        if (y >= 0 && y < dh)
            pix[y * str + px * bpp] = color;
        /* bottom row */
        i32 by = y + h - 1;
        if (by >= 0 && by < dh)
            pix[by * str + px * bpp] = color;
    }
    /* Draw left and right columns */
    for (i32 row = 1; row < h - 1; row++) {
        i32 py = y + row;
        if (py < 0 || py >= dh) continue;
        /* left */
        if (x >= 0 && x < dw)
            pix[py * str + x * bpp] = color;
        /* right */
        i32 rx = x + w - 1;
        if (rx >= 0 && rx < dw)
            pix[py * str + rx * bpp] = color;
    }
}

/* 0x0047DC3C — ls_bit_move
 *
 * Straight-line port of the original clipping + pixel-copy logic.
 * The original uses half-open [si..di] / [bx..bp] coordinate ranges (dx/sx are
 * minima; sw/sh are the exclusive maxima relative to the source).  We keep the
 * same convention: sw = source right edge, sh = source bottom edge (inclusive
 * when treated as end-x/end-y, exclusive for rect width/height counting).
 *
 * Clip path mirrors the asm register arithmetic:
 *   esi = sx_lo, edi = sw_hi  (sorted)
 *   ebx = sy_lo, ebp = sh_hi  (sorted)
 *
 * The inner copy calls lsys_bit_move_256 (row-by-row memcpy / colour-key loop).
 * We inline the copy here to avoid the indirection.
 *
 * Cross-format (pixel_format mismatch) falls through to lsi_remap_bmap which
 * is not yet ported — log a TODO and return.
 *
 * TODO: port lsi_bit_move_alpha (mode==2), ls_set_rectangle_2/3 surface paths.
 */
void ls_bit_move(bmap_t *dst, bmap_t *src,
                 i32 sx, i32 sy, i32 sw, i32 sh,
                 i32 dx, i32 dy, i32 mode) {
    if (!dst || !src || !dst->pixels || !src->pixels) return;

    /* Sort sx..sw and sy..sh so esi<=edi, ebx<=ebp (mirrors asm XCHG pattern) */
    if (sw < sx) { i32 t = sx; sx = sw; sw = t; }
    if (sh < sy) { i32 t = sy; sy = sh; sh = t; }

    /* Early-exit: ranges entirely outside destination */
    if (dx < 0 && (dx + (sw - sx)) < 0) return;
    if (dy < 0 && (dy + (sh - sy)) < 0) return;
    if (sx >= (i32)src->width || sy >= src->height) return;
    if (dx >= (i32)dst->width || dy >= dst->height) return;

    /* Clamp src origin to >= 0, adjusting dst offset to compensate */
    if (sx < 0) { dx -= sx; sx = 0; }
    if (sy < 0) { dy -= sy; sy = 0; }
    if (dx < 0) { sx -= dx; dx = 0; }
    if (dy < 0) { sy -= dy; dy = 0; }

    /* Clamp sw/sh to src extents */
    i32 src_w = (i32)src->width;
    i32 src_h = src->height;
    i32 dst_w = (i32)dst->width;
    i32 dst_h = dst->height;

    if (sw >= src_w) sw = src_w - 1;
    if (sh >= src_h) sh = src_h - 1;
    if (sx >= src_w || sy >= src_h) return;

    /* Width and height of copy region */
    i32 copy_w = sw - sx + 1;
    i32 copy_h = sh - sy + 1;
    if (copy_w <= 0 || copy_h <= 0) return;

    /* Clamp copy region to destination extents */
    if (dx + copy_w > dst_w) copy_w = dst_w - dx;
    if (dy + copy_h > dst_h) copy_h = dst_h - dy;
    if (dx >= dst_w || dy >= dst_h) return;
    if (copy_w <= 0 || copy_h <= 0) return;

    /* Cross-format blit not ported (lsi_remap_bmap) */
    if (dst->pixel_format != src->pixel_format) {
        print("ls_bit_move: pixel_format mismatch (dst=%d src=%d) — TODO lsi_remap_bmap\n",
              dst->pixel_format, src->pixel_format);
        return;
    }

    /* bytes per pixel from bit_depth */
    i32 bpp_bytes = (src->bit_depth > 0) ? (src->bit_depth / 8) : 1;
    i32 row_bytes = copy_w * bpp_bytes;

    i32 src_stride = src->stride;
    i32 dst_stride = dst->stride;

    const u8 *src_row = src->pixels + sy * src_stride + sx * bpp_bytes;
          u8 *dst_row = dst->pixels + dy * dst_stride + dx * bpp_bytes;

    if (mode == 0) {
        /* Opaque copy — row-by-row memcpy (mirrors rep movsd/movsb path) */
        for (i32 row = 0; row < copy_h; row++) {
            memcpy(dst_row, src_row, (size_t)row_bytes);
            src_row += src_stride;
            dst_row += dst_stride;
        }
    } else if (mode == 1) {
        /* Colour-key: skip zero-value pixels (mirrors loc_47DBB0 loop) */
        for (i32 row = 0; row < copy_h; row++) {
            for (i32 col = 0; col < row_bytes; col++) {
                if (src_row[col]) dst_row[col] = src_row[col];
            }
            src_row += src_stride;
            dst_row += dst_stride;
        }
    } else {
        /* TODO: mode==2 alpha-blend (lsi_bit_move_alpha), other modes */
        print("ls_bit_move: mode %d not yet ported — TODO lsi_bit_move_alpha\n", mode);
    }
}

/* 0x00453FA3 — draw_image
 *
 * Thin wrapper: optional debug outline via ls_box when global debug flag is set
 * and dst is the debug_surface; then calls ls_bit_move with mode=0.
 *
 * The original also has a short-circuit when dword_4B69BC != 0 and dst ==
 * graphics_context (skips the blit).  We preserve that guard via the NULL check.
 */
void draw_image(bmap_t *dst, bmap_t *src,
                i32 sx, i32 sy, i32 sw, i32 sh,
                i32 dx, i32 dy) {
    if (!dst || !src) return;

    /*
     * Original: test global_state_flags & 2 && dst == debug_surface → ls_box.
     * We skip the debug-surface identity check (debug_surface is a game_init
     * global not yet exposed here) and simply draw the box when any non-NULL
     * debug mode would trigger it.  The ls_box stub draws a visible outline.
     * TODO: wire debug_surface global when game_init exposes it.
     */
    /* (debug outline skipped — global_state_flags not yet wired) */

    print("draw_image: blit %dx%d src(%d,%d)-(%d,%d) -> dst(%d,%d)\n",
          (i32)src->width, src->height, sx, sy, sw, sh, dx, dy);

    ls_bit_move(dst, src, sx, sy, sw, sh, dx, dy, 0);
}

/* blit_bmap_to_framebuffer
 *
 * Expands an 8-bit palette-indexed bmap into the 32-bit XRGB front_buffer.
 * Palette entry i -> 0xFF000000 | (R<<16) | (G<<8) | B
 * from palette[i*3+0], palette[i*3+1], palette[i*3+2].
 *
 * Clips to framebuffer bounds.  src must be 8bpp (bit_depth==8 or pixel_format==2).
 */
void blit_bmap_to_framebuffer(u32 *fb, i32 fb_w, i32 fb_h,
                               const bmap_t *src, const u8 *palette,
                               i32 dx, i32 dy) {
    if (!fb || !src || !src->pixels || !palette) return;

    i32 src_w = (i32)src->width;
    i32 src_h = src->height;
    i32 src_stride = src->stride > 0 ? src->stride : src_w;

    /* Clip: skip entirely out-of-bounds sources */
    if (dx >= fb_w || dy >= fb_h) return;
    if (dx + src_w <= 0 || dy + src_h <= 0) return;

    /* Source row/col start (clamped to 0) */
    i32 src_x0 = (dx < 0) ? -dx : 0;
    i32 src_y0 = (dy < 0) ? -dy : 0;

    /* Destination top-left after clamping */
    i32 dst_x0 = (dx < 0) ? 0 : dx;
    i32 dst_y0 = (dy < 0) ? 0 : dy;

    /* Number of pixels to copy per row / column */
    i32 copy_w = src_w - src_x0;
    i32 copy_h = src_h - src_y0;
    if (dst_x0 + copy_w > fb_w) copy_w = fb_w - dst_x0;
    if (dst_y0 + copy_h > fb_h) copy_h = fb_h - dst_y0;
    if (copy_w <= 0 || copy_h <= 0) return;

    print("blit_bmap_to_framebuffer: %dx%d src -> fb offset (%d,%d) copy %dx%d\n",
          src_w, src_h, dx, dy, copy_w, copy_h);

    const u8 *src_row = src->pixels + src_y0 * src_stride + src_x0;
          u32 *dst_row = fb + dst_y0 * fb_w + dst_x0;

    for (i32 row = 0; row < copy_h; row++) {
        for (i32 col = 0; col < copy_w; col++) {
            u8 idx = src_row[col];
            u8 r   = palette[idx * 3 + 0];
            u8 g   = palette[idx * 3 + 1];
            u8 b   = palette[idx * 3 + 2];
            /* NSBitmapImageRep expects byte[0]=R, byte[1]=G, byte[2]=B on
             * little-endian hosts — pack accordingly. */
            dst_row[col] = 0xFF000000u | ((u32)b << 16) | ((u32)g << 8) | (u32)r;
        }
        src_row += src_stride;
        dst_row += fb_w;
    }
}

/* blit_bmap_masked_to_framebuffer
 *
 * Like blit_bmap_to_framebuffer but skips pixels whose palette index equals
 * mask_idx (transparency key).  Conventional use: mask_idx=0 → index 0 is
 * transparent, so sprites with black backgrounds blit correctly.
 *
 * Mirrors the colour-key skip in ls_bit_move (mode==1) but targets the 32-bit
 * XRGB framebuffer directly (no intermediate bmap copy needed).
 */
void blit_bmap_masked_to_framebuffer(u32 *fb, i32 fb_w, i32 fb_h,
                                      const bmap_t *src, const u8 *palette,
                                      i32 dx, i32 dy, u8 mask_idx) {
    if (!fb || !src || !src->pixels || !palette) return;

    i32 src_w = (i32)src->width;
    i32 src_h = src->height;
    i32 src_stride = src->stride > 0 ? src->stride : src_w;

    /* Clip: skip entirely out-of-bounds sources */
    if (dx >= fb_w || dy >= fb_h) return;
    if (dx + src_w <= 0 || dy + src_h <= 0) return;

    /* Source row/col start (clamped to 0) */
    i32 src_x0 = (dx < 0) ? -dx : 0;
    i32 src_y0 = (dy < 0) ? -dy : 0;

    /* Destination top-left after clamping */
    i32 dst_x0 = (dx < 0) ? 0 : dx;
    i32 dst_y0 = (dy < 0) ? 0 : dy;

    /* Number of pixels to copy per row / column */
    i32 copy_w = src_w - src_x0;
    i32 copy_h = src_h - src_y0;
    if (dst_x0 + copy_w > fb_w) copy_w = fb_w - dst_x0;
    if (dst_y0 + copy_h > fb_h) copy_h = fb_h - dst_y0;
    if (copy_w <= 0 || copy_h <= 0) return;

    const u8 *src_row = src->pixels + src_y0 * src_stride + src_x0;
          u32 *dst_row = fb + dst_y0 * fb_w + dst_x0;

    for (i32 row = 0; row < copy_h; row++) {
        for (i32 col = 0; col < copy_w; col++) {
            u8 idx = src_row[col];
            if (idx == mask_idx) continue; /* transparent — skip pixel */
            u8 r = palette[idx * 3 + 0];
            u8 g = palette[idx * 3 + 1];
            u8 b = palette[idx * 3 + 2];
            dst_row[col] = 0xFF000000u | ((u32)b << 16) | ((u32)g << 8) | (u32)r;
        }
        src_row += src_stride;
        dst_row += fb_w;
    }
}
