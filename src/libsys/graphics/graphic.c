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

// ls_free_bmap — release bmap pixel buffer and header
void ls_free_bmap(bmap_t *bmap) {
    if (!bmap) return;
    if (bmap->owns_buffer && bmap->pixels) {
        free_memory(bmap->pixels);
        bmap->pixels = NULL;
        bmap->first_pixel = NULL;
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
