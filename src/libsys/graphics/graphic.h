#pragma once

#include "../common.h"
#include "../system.h"
#include "../memory.h"

typedef struct {
    i32 width;
    i32 height;
    i32 bpp;
    i32 stride;
    u32 *back_buffer;
    u32 *front_buffer;
    u32 *frame_buffer;
} graphic_t;

/*
 * bmap_t — engine bitmap / surface descriptor (0x34 bytes, matches original alloc).
 *
 * Offsets derived from ls_init_bmap_ex asm:
 *   +00  stride       (width * bpp/8)
 *   +04  width
 *   +08  height
 *   +0C  pixels       (allocated pixel buffer; NULL if surface-backed)
 *   +10  bpp_flags    (palette depth argument passed in)
 *   +14  reserved_14
 *   +18  reserved_18
 *   +1C  bit_depth    (colour depth in bits: 8/16/24/32)
 *   +20  owns_buffer  (1 = we allocated pixels, 0 = surface-backed)
 *   +24  reserved_24
 *   +28  first_pixel  (ptr copy of pixels — mirrors original [esi+28h] = [esi])
 *   +2C  pixel_format (mask / pixel-format enum; 2=8bpp,4=16bpp,0x20=24bpp,0x80=32bpp)
 *   +30  surface      (platform surface / DirectDraw ptr — NULL in our port)
 */
typedef struct {
    i32  stride;        /* +00 */
    i16  width;         /* +04 */
    i16  _pad_06;
    i32  height;        /* +08 */
    u8  *pixels;        /* +0C */
    i32  bpp_flags;     /* +10 */
    i32  reserved_14;   /* +14 */
    i32  reserved_18;   /* +18 */
    i32  bit_depth;     /* +1C */
    i32  owns_buffer;   /* +20 */
    i32  reserved_24;   /* +24 */
    u8  *first_pixel;   /* +28 */
    i32  pixel_format;  /* +2C */
    void *surface;      /* +30 */
} bmap_t;

/* global graphics context — mirrors graphics_context data symbol */
extern bmap_t *g_graphics_context;

/* global back-screen bmap — mirrors dword_55C224 */
extern bmap_t *g_back_screen;

/* bmap instance counter — mirrors word_55C774 */
extern u16 g_bmap_count;

void ls_init_graphic_system(graphic_t *graphic, system_t *system);
void ls_graphic_flip(graphic_t *graphic);
void ls_graphic_clear(graphic_t *graphic);
void ls_release_graphic_system(graphic_t *graphic);

/*
 * ls_init_bmap_ex — allocate and initialise a bmap_t (0x0047CEBE).
 *
 * Original __fastcall convention (ECX=width, EDX=height, arg_0=bit_depth, arg_4=bpp_flags):
 *   Allocates 0x34-byte header from heap, zeros it, fills layout fields,
 *   selects pixel_format from bit_depth, allocates pixel buffer, calls ls_clear_bmap.
 *
 * Port: allocate_memory(resource_type, size) replaces ls_get_memory_heap.
 *   resource_type = bpp_flags (matches original ECX arg to ls_get_memory_heap).
 *   Surface-backed paths (cases 1,2,4,8 for bpp_flags) are stubbed to the
 *   default path (raw buffer alloc) since DirectDraw is absent.
 *
 * Returns pointer to initialised bmap_t, or NULL on allocation failure.
 */
bmap_t *ls_init_bmap_ex(i32 width, i32 height, i32 bit_depth, i32 bpp_flags);

/* ls_free_bmap — release a bmap_t and its pixel buffer */
void ls_free_bmap(bmap_t *bmap);

/* ls_clear_bmap — zero the pixel buffer */
void ls_clear_bmap(bmap_t *bmap);

/*
 * ls_set_video_mode — set display resolution / colour depth (0x0048418A).
 *
 * Original: tears down old graphics_context + back_screen bmaps, calls
 *   sub_48AED0 (DirectDraw mode set), re-creates graphics_context + back_screen
 *   via ls_init_bmap_ex.
 *
 * Port: DirectDraw mode-set is a no-op (platform layer owns the window).
 *   Creates/recreates g_graphics_context and g_back_screen with the given dimensions.
 *   Returns 1 on success, 0 on failure.
 *
 * Parameters match the original __fastcall+stack convention:
 *   ECX = width, EDX = height, arg_0 = bit_depth, arg_4 = bpp_flags
 */
i32 ls_set_video_mode(i32 width, i32 height, i32 bit_depth, i32 bpp_flags);

/* -------------------------------------------------------------------------
 * Blit primitives (Slice A)
 * ------------------------------------------------------------------------- */

/*
 * ls_box — draw a debug outline rectangle (0x0047D466 area).
 *
 * Ported as stub: the original has complex fill/invert sub-paths depending on
 * flag bits in arg_10.  For now draws a 1-pixel solid outline in the dst bmap
 * pixel buffer (colour byte 0xFF), sufficient for debug purposes.
 *
 * TODO: port ls_fill_rect / ls_invert_rect sub-paths when needed.
 */
void ls_box(bmap_t *dst, i32 x, i32 y, i32 w, i32 h, u8 color, i32 flags);

/*
 * ls_bit_move — rectangular blit from src bmap to dst bmap (0x0047DC3C).
 *
 * Clips source and destination rectangles, then copies pixel rows.
 * mode 0 = opaque copy; mode 1 = colour-key (skip zero pixels);
 * mode 2 = alpha-blend (TODO stub).
 */
void ls_bit_move(bmap_t *dst, bmap_t *src,
                 i32 sx, i32 sy, i32 sw, i32 sh,
                 i32 dx, i32 dy, i32 mode);

/*
 * draw_image — thin wrapper around ls_bit_move (0x00453FA3).
 *
 * Calls ls_box for debug outline when debug surface is active,
 * then calls ls_bit_move with mode=0.
 */
void draw_image(bmap_t *dst, bmap_t *src,
                i32 sx, i32 sy, i32 sw, i32 sh,
                i32 dx, i32 dy);

/*
 * blit_bmap_to_framebuffer — expand 8bpp indexed bmap into 32-bit XRGB front_buffer.
 *
 * Palette entry i -> 0xFF000000 | (r<<16) | (g<<8) | b from palette[i*3+0..2].
 * Clips to framebuffer bounds.  Palette must be 768 bytes (256 x RGB triplets).
 *
 * @param fb       u32 XRGB framebuffer (row-major, fb_w x fb_h)
 * @param fb_w     framebuffer width in pixels
 * @param fb_h     framebuffer height in pixels
 * @param src      8-bit indexed source bmap
 * @param palette  768-byte RGB palette (color_table layout)
 * @param dx       destination X offset in framebuffer
 * @param dy       destination Y offset in framebuffer
 */
void blit_bmap_to_framebuffer(u32 *fb, i32 fb_w, i32 fb_h,
                               const bmap_t *src, const u8 *palette,
                               i32 dx, i32 dy);
