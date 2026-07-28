/*
 * game_init.c — game-side bootstrap  (0046BEEB)
 *
 * Ported from game_init_46BEEB in disassembly/dkev.asm (lines 145447–145906).
 * Reference IDA C: disassembly/functions/main/ls_main.c (ls_main body shows
 *   calling context and heap allocations that precede this call).
 *
 * Init phases (in order):
 *   1. Debug banner "Init Scene ..."
 *   2. HUD health bitmap  — GINETTE.CIF  (select_language_version path or fallback bmap)
 *   3. LOADING.GIF compressed image load
 *   4. Transparency pass on LOADING.GIF pixels (replace non-0xFF with 0)
 *   5. Two HUD bmap copies from GINETTE dims (dword_4DDE1C → 4DE648, 4DE64C)
 *   6. Gouraud-shading index buffer alloc (0x4100 bytes)
 *   7. MENUGEN.GIF palette load + init_input_state_table memset + palette wire-up
 *   8. sub_425D50 (unknown — after palette wire-up)
 *   9. initialize_health_hud
 *  10. sub_45D395
 *  11. sub_421960
 *  12. sub_41F7C8
 *  13. sub_44D706
 *  14. sub_43040A
 *  15. Weapon-slot table init (4 entries × 31 weapon slots — dword_4ECD50/4ECDD4/4ECD54)
 *  16. Entity-slot table init (0x62=98 entries × 31 slots — dword_4DF5D0/4DF654/4DF5D4)
 *  17. Various memset zero-inits for global tables
 *  18. Debug banner "Init Divers ..."
 *  19. FLAM.MAT material load (is_using_hdd branch)
 *  20. allocate_and_initialize_health_buffer
 *  21. sub_42F88E
 *  22. register_player_creature (ECX=1)
 *  23. init_font_system
 *  24. sub_44D676
 *  25. sub_48F2C0 — texture config init (3E8h entries)
 *  26. sub_48F230 — entity buffer setup (640×480)
 *  27. Render bmap creation: render_buffer1, dword_4B71FC, dword_4B7200, debug_surface
 *  28. create_image_buffer for render_buffer2 (640×480) and dword_507474 (320×240)
 *  29. datas>objets>objets.gif load
 *  30. sub_4130AA — resource initialiser
 *  31. initialize_resource + palette_data wire-up
 *  32. game_state / game_death_state assignment from dword_4B70E8
 *  33. sub_46493B — player entity setup
 *  34. sub_464F6D — player entity secondary init
 *  35. Entity/context cross-wiring (palette ptr into game_context / render_buffer1 / graphics_context)
 *  36. system_get_gametime → player_entity +D5Ch
 *  37. sub_48F1D0 — render surface setup ×2
 *  38. set_entities_combat_mode
 *  39. sub_426335
 *  40. sub_471C18 — load objets.txt
 *  41. special_mode_active → dword_505DD8
 *  42. 16-bit colour branch: sub_491ED0 + sub_491FD0
 *  43. sub_45B207
 *  44. display_weapons_inventory
 *  45. sub_4161B0 — final init call; dword_4B787C = 0
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "libsys/common.h"
#include "libsys/memory.h"
#include "libsys/file_system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/cif.h"
#include "libsys/graphics/palette.h"
#include "libsys/graphics/font.h"
#include "state.h"
#include "game_init.h"
#include "player.h"

/* -------------------------------------------------------------------------
 * File-local globals — mirrors of asm data symbols not yet in state_t.
 * Every entry has the original virtual address in a trailing comment.
 * ---------------------------------------------------------------------- */

/* HUD / image handles */
static void *g_ginette_bmap        = NULL;   /* dword_4DDE1C @ 004DDE1C */
static void *g_health_bmap_a       = NULL;   /* dword_4DE648 @ 004DE648 */
static void *g_health_bmap_b       = NULL;   /* dword_4DE64C @ 004DE64C */
static void *g_loading_bmap        = NULL;   /* renderer_context @ game_init asm */
static void *g_gouraud_table       = NULL;   /* dword_4DE640 @ 004DE640 */

/* Palette / colour handles */
static void *g_menugen_palette     = NULL;   /* dword_4B71F8 @ 004B71F8 */
static void *g_menugen_bmap        = NULL;   /* full 640x480 backdrop (port-only) */
static void *g_palette_data        = NULL;   /* palette_data (asm sym) */

/* Render / scratch bmaps */
static bmap_t *g_render_buffer1   = NULL;   /* render_buffer1 (asm sym) */
static bmap_t *g_render_buffer2   = NULL;   /* render_buffer2 (asm sym) */
static bmap_t *g_debug_surface    = NULL;   /* debug_surface (asm sym)  */
static bmap_t *g_bmap_4B71FC     = NULL;   /* dword_4B71FC @ 004B71FC */
static bmap_t *g_bmap_4B7200     = NULL;   /* dword_4B7200 @ 004B7200 */
static void  *g_bmap_507474      = NULL;   /* dword_507474 @ 00507474 */

/* Entity / game-state handles */
static void *g_game_state         = NULL;   /* game_state (asm sym)     */
static void *g_game_death_state   = NULL;   /* game_death_state (asm sym) */
/* g_player_entity moved to player.c — exposed via player.h */
static void *g_game_context       = NULL;   /* game_context (asm sym)   */
static void *g_entity_buffer_base = NULL;   /* entity_buffer_base (asm sym) */
static void *g_dword_4B70E8      = NULL;   /* dword_4B70E8 @ 004B70E8  */
static void *g_objets_gif         = NULL;   /* dword_4FB1DC @ 004FB1DC  */
static void *g_flam_mat           = NULL;   /* dword_4FB1E0 @ 004FB1E0  */

/* Weapon-slot / entity-slot zero-init tables (addresses only; we memset them) */
/* dword_4ECD50 @ 004ECD50 — weapon slot table head */
/* dword_4ECDD4 @ 004ECDD4 — weapon slot table tail */
/* dword_4ECD54 @ 004ECD54 — weapon slot entries    */
/* dword_4DF5D0 @ 004DF5D0 — entity slot table head */
/* dword_4DF654 @ 004DF654 — entity slot table tail */
/* dword_4DF5D4 @ 004DF5D4 — entity slot entries    */

/* Misc */
static i32 g_dword_4ECD3C        = 0;       /* dword_4ECD3C @ 004ECD3C  */
static i32 g_is_using_hdd        = 0;       /* is_using_hdd (asm sym)   */
static i32 g_special_mode_active = 0;       /* special_mode_active      */
static i32 g_dword_505DD8        = 0;       /* dword_505DD8 @ 00505DD8  */
static i32 g_dword_4B787C        = 0;       /* dword_4B787C @ 004B787C  */

/* Index buffer computed from gouraud table */
static void *g_index_buffer       = NULL;   /* index_buffer (asm sym)   */

/* Inverse-Z buffer (sub_421960) — 16KB from ui_health_buffer pool */
static void *g_inv_z_array        = NULL;   /* dword_510F30 @ 00510F30  */

/* Animation system (sub_41F7C8) */
static void *g_anim_buffer        = NULL;   /* dword_511744 — 54KB anim data  */
static void *g_pos_buffer         = NULL;   /* dword_511778 — 330KB positions */
static void *g_msg_buffer         = NULL;   /* dword_511740 — 12KB messages   */
static void *g_can_ptrs[4]        = {0};    /* dword_511730..51173C — CAN ptrs */
static i32   g_can_indices[4]     = {-1,-1,-1,-1};

/* Sound sample tables (sub_44D706) */
static u8    g_sample_registry[3000] = {0}; /* byte_4FB450 @ 004FB450 */
static u8    g_sample_flags[200]     = {0}; /* dword_4FB380 @ 004FB380 */
static i32   g_samples_enabled       = 1;

/* Resource / sprite bmap used by initialize_resource_slots (sub_4130AA) */
static bmap_t *g_dword_4B297C    = NULL;   /* dword_4B297C @ 004B297C  */

/* Mouse state globals zeroed by initialize_mouse_and_animation (sub_45D395) */
static i32 g_dword_4D7F0C        = 0;      /* dword_4D7F0C @ 004D7F0C  */
static i32 g_dword_4D7F24        = 0;      /* dword_4D7F24 @ 004D7F24  */
static i32 g_dword_4D7F10        = 0;      /* dword_4D7F10 @ 004D7F10  */
static i32 g_dword_4D7F20        = 0;      /* dword_4D7F20 @ 004D7F20  */
static i32 g_dword_4D7F14        = 0;      /* dword_4D7F14 @ 004D7F14 (mouse range X) */
static i32 g_dword_4D7F18        = 0;      /* dword_4D7F18 @ 004D7F18 (mouse range Y) */
static bmap_t *g_dword_4D7F2C   = NULL;   /* dword_4D7F2C @ 004D7F2C (animation bmap) */
static void   *g_dword_4F4DF4   = NULL;   /* dword_4F4DF4 @ 004F4DF4 (INTFANI3 bmap) */

/*
 * unk_4CFE40 — font descriptor block for FONTE11 (used by game_finalize /
 * sub_4161B0 → sub_415FB0).  The block holds up to 0x260 font entries
 * (each 0x13 = 19 bytes wide, as in sub_415FB0 inner loop: edx += 0x13,
 * compare eax < offset unk_4D0A44).  Total: at least 0x260 * 0x13 = ~7K.
 * Sub_415FB0 writes the glyph bmap ptr at [unk_4CFE40+0], height 0x13 at
 * [+0BF8h], and walks unk_4CFFC4 .. unk_4D0A44 (stride 0x0Ch entries).
 * Conservative allocation: 0x2000 bytes (matches IDA data-segment layout).
 */
static u8 g_unk_4CFE40[0x2000]  = {0};    /* unk_4CFE40 @ 004CFE40  */

/* -------------------------------------------------------------------------
 * Forward declarations for sub-functions not yet ported.
 * Each has the original address in its comment.
 * ---------------------------------------------------------------------- */

/* HUD / image helpers */
static c8  *select_language_version(const c8 *virtual_path, c8 *out_buf, i32 out_size); /* 46BF17 area */
static void *image_load_compressed(const c8 *virtual_path, void *buf_ignored);           /* 46BF5F area */
static void  image_load_compressed_free(void *handle);
static void *load_image_color_data(const c8 *path, void *buf);                           /* 46C051 area */
static void  ls_free_palette(void *palette);                                              /* 46C076 area */

/* -------------------------------------------------------------------------
 * select_language_version — resolve a virtual path to a host path.
 *
 * Original asm @ 004815C4 (called near 46BF17):
 *   Uppercases input, calls ls_make_path → host path, then lsys_open.
 *   Returns 1 if file opens OK, 0 if FFFF (not found).
 *
 * Port strategy (single-language passthrough for UK/default):
 *   Use ls_resolve_path against state->game_path.
 *   Try path as-is first (already resolved → CIF on disk).
 *   No language-prefix substitution yet.
 *   Returns out_buf on success (file exists), NULL on failure.
 * ---------------------------------------------------------------------- */
static c8 *select_language_version(const c8 *virtual_path, c8 *out_buf, i32 out_size) {
    if (!virtual_path || !out_buf || out_size <= 0) return NULL;

    /* Resolve virtual path → host path */
    if (!ls_resolve_path(state->game_path, virtual_path, out_buf, out_size)) {
        print("select_language_version: path overflow for '%s'\n", virtual_path);
        return NULL;
    }

    /* Check file exists */
    if (ls_exists(out_buf)) {
        print("select_language_version: found '%s'\n", out_buf);
        return out_buf;
    }

    /* If path ends in .GIF (any case), try .CIF */
    i32 len = (i32)strlen(out_buf);
    if (len >= 4) {
        const c8 *ext = out_buf + len - 4;
        if ((ext[0] == '.' || ext[0] == '.') &&
            (toupper((u8)ext[1]) == 'G') &&
            (toupper((u8)ext[2]) == 'I') &&
            (toupper((u8)ext[3]) == 'F')) {
            /* swap extension to .CIF */
            out_buf[len-3] = 'C';
            out_buf[len-2] = 'I';
            out_buf[len-1] = 'F';
            if (ls_exists(out_buf)) {
                print("select_language_version: found (CIF fallback) '%s'\n", out_buf);
                return out_buf;
            }
            /* restore .GIF for accurate error message */
            out_buf[len-3] = 'G';
            out_buf[len-2] = 'I';
            out_buf[len-1] = 'F';
        }
    }

    print("select_language_version: not found '%s'\n", out_buf);
    return NULL;
}

/* -------------------------------------------------------------------------
 * image_load_compressed — load a CIF image file (accepts .GIF names,
 * retries with .CIF if not found).
 *
 * Original asm @ 004746C0: opens a cached header file, checks version byte,
 * reads width/height, then calls decompress_lzw_428E60 (our cif_load covers
 * the entire chain).  Falls back to ls_load_gif_ex when cache misses.
 *
 * Port: wraps cif_load.  Returns a heap-owned bmap_t on success, NULL on
 * failure.  buf_ignored is unused (original passes a memory slot; our port
 * allocates directly).
 * ---------------------------------------------------------------------- */
static void *image_load_compressed(const c8 *virtual_path, void *buf_ignored) {
    (void)buf_ignored;
    if (!virtual_path) return NULL;

    c8 resolved[512];

    /* 1. Resolve virtual path → host path */
    if (!ls_resolve_path(state->game_path, virtual_path, resolved, (i32)sizeof(resolved))) {
        print("image_load_compressed: path overflow '%s'\n", virtual_path);
        return NULL;
    }

    /* 2. Try cif_load on resolved path */
    cif_image_t img;
    memset(&img, 0, sizeof(img));
    b8 ok = cif_load(&img, resolved);

    /* 3. If failed AND path ends in .GIF, retry with .CIF */
    if (!ok) {
        i32 len = (i32)strlen(resolved);
        if (len >= 4) {
            const c8 *ext = resolved + len - 4;
            if ((ext[0] == '.') &&
                (toupper((u8)ext[1]) == 'G') &&
                (toupper((u8)ext[2]) == 'I') &&
                (toupper((u8)ext[3]) == 'F')) {
                resolved[len-3] = 'C';
                resolved[len-2] = 'I';
                resolved[len-1] = 'F';
                ok = cif_load(&img, resolved);
                if (!ok) {
                    /* restore for error message */
                    resolved[len-3] = 'G';
                    resolved[len-2] = 'I';
                    resolved[len-1] = 'F';
                }
            }
        }
    }

    if (!ok) {
        print("image_load_compressed: failed to load '%s'\n", resolved);
        return NULL;
    }

    /* 4. Allocate a bmap_t and populate from cif_image_t.
     *    The bmap_t pixel buffer and palette come from the cif decode (we take
     *    ownership of both so sprites can blit with their embedded palette). */
    bmap_t *bmap = (bmap_t *)malloc(sizeof(bmap_t));
    if (!bmap) {
        cif_free(&img);
        print("image_load_compressed: malloc failed\n");
        return NULL;
    }
    memset(bmap, 0, sizeof(bmap_t));

    bmap->width        = (i16)img.width;
    bmap->height       = img.height;
    bmap->bpp_flags    = 8;
    bmap->bit_depth    = 8;
    bmap->stride       = img.width;   /* 8-bit: 1 byte per pixel */
    bmap->pixels       = img.pixels;  /* transfer ownership */
    bmap->first_pixel  = img.pixels;
    bmap->owns_buffer  = 1;
    bmap->pixel_format = 2;           /* 8bpp indexed */
    /* Transfer palette ownership — callers can use bmap->palette for blitting
     * without a separate load_image_color_data call.
     * owns_palette = 1 so ls_free_bmap will release it. */
    bmap->palette      = img.palette; /* transfer ownership (768 bytes) */
    bmap->owns_palette = (img.palette != NULL) ? 1 : 0;
    img.palette        = NULL;        /* detach so cif_free won't double-free */

    print("image_load_compressed: loaded '%s' -> %dx%d palette=%p\n",
          resolved, img.width, img.height, (void *)bmap->palette);
    return (void *)bmap;
}

/* image_load_compressed_free — release a bmap_t returned by image_load_compressed.
 * TODO: call from any cleanup paths once those are ported. */
static void image_load_compressed_free(void *handle) __attribute__((unused));
static void image_load_compressed_free(void *handle) {
    if (!handle) return;
    bmap_t *bmap = (bmap_t *)handle;
    if (bmap->owns_buffer && bmap->pixels) {
        free(bmap->pixels);
        bmap->pixels      = NULL;
        bmap->first_pixel = NULL;
    }
    if (bmap->owns_palette && bmap->palette) {
        free(bmap->palette);
        bmap->palette = NULL;
    }
    free(bmap);
}

/* load_image_color_data — load image and extract palette/colour data.
 * Returns a heap-allocated 768-byte RGB palette buffer, NULL on failure.
 * The caller owns the returned buffer and must free it via ls_free_palette. */
static void *load_image_color_data(const c8 *virtual_path, void *buf) {
    (void)buf;
    if (!virtual_path) return NULL;

    c8 resolved[512];

    /* Resolve virtual path → host path */
    if (!ls_resolve_path(state->game_path, virtual_path, resolved, (i32)sizeof(resolved))) {
        print("load_image_color_data: path overflow '%s'\n", virtual_path);
        return NULL;
    }

    /* Try .GIF → .CIF extension swap if needed */
    cif_image_t img;
    memset(&img, 0, sizeof(img));
    b8 ok = cif_load(&img, resolved);
    if (!ok) {
        i32 len = (i32)strlen(resolved);
        if (len >= 4) {
            const c8 *ext = resolved + len - 4;
            if ((ext[0] == '.') &&
                (toupper((u8)ext[1]) == 'G') &&
                (toupper((u8)ext[2]) == 'I') &&
                (toupper((u8)ext[3]) == 'F')) {
                resolved[len-3] = 'C';
                resolved[len-2] = 'I';
                resolved[len-1] = 'F';
                ok = cif_load(&img, resolved);
            }
        }
    }

    if (!ok) {
        print("load_image_color_data: failed to load '%s'\n", resolved);
        return NULL;
    }

    /* Take ownership of palette; free pixels (palette-only path) */
    u8 *palette = img.palette;
    img.palette  = NULL; /* detach so cif_free won't double-free */
    if (img.pixels) {
        free(img.pixels);
        img.pixels = NULL;
    }

    /* Copy the 768-byte palette into color_table for rendering */
    if (palette) {
        memcpy(color_table, palette, 0x300);
        print("load_image_color_data: palette copied to color_table from '%s'\n", resolved);
    }

    return (void *)palette; /* caller owns; use ls_free_palette to release */
}

/* ls_free_palette — release a palette handle returned by load_image_color_data. */
static void ls_free_palette(void *palette) {
    if (palette) {
        free(palette);
    }
}

/* -------------------------------------------------------------------------
 * 004130AA — initialize_resource_slots
 *
 * Allocates a sprite/resource bmap (640 wide × 800 high, 8-bit) and stores
 * a pointer to the global color_table into [bmap+18h] (reserved_18 field,
 * used as a palette-pointer by rendering code).
 *
 * Original asm:
 *   mov  eax, ui_health_buffer
 *   push eax
 *   mov  edx, 320h          ; height = 0x320 = 800
 *   mov  ecx, 280h          ; width  = 0x280 = 640
 *   call create_image_buffer
 *   mov  dword_4B297C, eax
 *   mov  eax, dword_4B297C
 *   mov  dword ptr [eax+18h], offset color_table
 *   retn
 *
 * Note: create_image_buffer (ECX=width, EDX=height, push=slot) is a thin
 * wrapper around ls_init_bmap_ex; ported here as ls_init_bmap_ex with
 * bpp_flags = MEM_SLOT_HEALTH (matching push eax / ui_health_buffer).
 * ---------------------------------------------------------------------- */

// 004130AA
static void initialize_resource_slots(void) {
    g_dword_4B297C = ls_init_bmap_ex(
        0x280,           /* width  = 640 */
        0x320,           /* height = 800 */
        8,               /* bit_depth */
        MEM_SLOT_HEALTH  /* bpp_flags / memory slot (= ui_health_buffer) */
    );

    if (g_dword_4B297C) {
        /* Store color_table pointer in [bmap+18h] (reserved_18 / palette ptr) */
        g_dword_4B297C->reserved_18 = (i32)(intptr_t)color_table; /* 004130AA+20 */
        print("initialize_resource_slots: bmap 640×800 @ %p, color_table @ %p\n",
              (void *)g_dword_4B297C, (void *)color_table);
    } else {
        print("initialize_resource_slots: ls_init_bmap_ex failed\n");
    }
}

/* -------------------------------------------------------------------------
 * 0045D395 — initialize_mouse_and_animation
 *
 * Sets the mouse cursor type and input range, zeroes six mouse-state globals,
 * loads the INTFANI3.GIF cursor animation bmap, then creates a small image
 * buffer matching the animation frame dimensions + 1.
 *
 * Original asm:
 *   xor  edx, edx; xor ecx, ecx
 *   call ls_set_mouse          ; hide/reset mouse cursor (ecx=0, edx=0)
 *   push 1E0h; push 280h
 *   xor  edx, edx; xor ecx, ecx
 *   call ls_set_mouse_range    ; set range to 640×480 (push h=0x1E0, w=0x280)
 *   mov  dword_4D7F0C, 0
 *   mov  dword_4D7F24, 0
 *   mov  dword_4D7F10, 0
 *   mov  dword_4D7F20, 0
 *   mov  dword_4D7F14, 32h    ; mouse X min = 50
 *   mov  dword_4D7F18, 62h    ; mouse Y min = 98
 *   mov  ecx, offset "DATAS>GLOBAL>IMAGES>INTFANI3.GIF"
 *   mov  edx, ui_health_buffer
 *   call image_load_compressed
 *   mov  dword_4F4DF4, eax
 *   push 8
 *   mov  edx, dword_4D7F18    ; height = dword_4D7F18 + 1 = 0x63
 *   mov  ecx, dword_4D7F14    ; width  = dword_4D7F14 + 1 = 0x33
 *   inc  edx; inc ecx
 *   call create_image_buffer
 *   mov  dword_4D7F2C, eax
 *   retn
 *
 * Unported: ls_set_mouse, ls_set_mouse_range (mouse platform layer not yet
 * implemented); image_load_compressed (image loader not yet ported).
 * ---------------------------------------------------------------------- */

// 0045D395
static void initialize_mouse_and_animation(void) {
    /* TODO: ls_set_mouse(ecx=0, edx=0) @ 45D395+9 — hide/reset cursor */
    /* TODO: ls_set_mouse_range(0, 0, 0x280, 0x1E0) @ 45D395+16 — set 640×480 range */

    /* Zero mouse-state globals */
    g_dword_4D7F0C = 0;  /* 45D39B */
    g_dword_4D7F24 = 0;  /* 45D3A1 */
    g_dword_4D7F10 = 0;  /* 45D3A7 */
    g_dword_4D7F20 = 0;  /* 45D3AD */
    g_dword_4D7F14 = 0x32; /* 45D3B3 — mouse X offset = 50 */
    g_dword_4D7F18 = 0x62; /* 45D3B9 — mouse Y offset = 98 */

    /* TODO: image_load_compressed @ 45D3BF —
     *   ecx = "DATAS>GLOBAL>IMAGES>INTFANI3.GIF"
     *   edx = ui_health_buffer
     *   result → dword_4F4DF4
     */
    g_dword_4F4DF4 = image_load_compressed(
        "DATAS>GLOBAL>IMAGES>INTFANI3.GIF",
        NULL /* ui_health_buffer — asm global, not yet threaded here */
    ); /* 45D3C9 */

    /* Allocate animation frame bmap: width = 4D7F14+1, height = 4D7F18+1 */
    g_dword_4D7F2C = ls_init_bmap_ex(
        g_dword_4D7F14 + 1,  /* ecx = dword_4D7F14+1 = 0x33 = 51 */
        g_dword_4D7F18 + 1,  /* edx = dword_4D7F18+1 = 0x63 = 99 */
        8,                   /* bit_depth (push 8) */
        MEM_SLOT_HEALTH      /* bpp_flags (= ui_health_buffer slot) */
    ); /* 45D3D2 */

    print("initialize_mouse_and_animation: mouse range 640×480, "
          "anim bmap %dx%d @ %p\n",
          g_dword_4D7F14 + 1, g_dword_4D7F18 + 1,
          (void *)g_dword_4D7F2C);
}

/* -------------------------------------------------------------------------
 * 00415FB0 — sub_415FB0  (font descriptor builder)
 *
 * Original asm body:
 *   1. Appends ".CAR" to base_name → ls_open → ls_read 0xBF4 bytes into
 *      [desc+4] (glyph metrics table) → ls_read 4 trailing bytes → ls_close.
 *   2. Appends ".GIF" to base_name → image_load_compressed(path, ui_health_buffer)
 *      → bmap ptr stored at [desc+0] → height 0x13 stored at [desc+0BF8h].
 *   3. Zeroes word_4D0A40 (0x976 words + stosw ≈ 0x4ED words = ~2.5 KB).
 *   4. Walks unk_4CFFC4..unk_4D0A44 (stride 0x0C, 0x260 entries) to populate
 *      per-glyph bitmask into word_4D0A40 from atlas pixel data.
 *      (The bitmask loop is preserved as a TODO — requires real pixel layout.)
 *
 * Port: steps 1–3 fully ported.  Step 4 glyph bitmask walk is a TODO stub
 * (requires the word_4D0A40 global and unk_4CFFC4 glyph-entry layout).
 *
 * desc points to g_unk_4CFE40 (passed as edx from game_finalize).
 * base_name = "DATAS>MENUS>FONTE11".
 * ---------------------------------------------------------------------- */

// 00415FB0
static void sub_415FB0(const c8 *base_name, u8 *desc) {
    c8 path[512];
    c8 resolved[512];

    /* ---- Step 1: load .CAR glyph metrics into [desc+4] ---- */
    /* Build "DATAS>MENUS>FONTE11.CAR" */
    snprintf(path, sizeof(path), "%s.CAR", base_name);
    if (ls_resolve_path(state->game_path, path, resolved, (i32)sizeof(resolved))) {
        FILE *car_fp = ls_open(resolved, "rb");
        if (car_fp) {
            /* Read 0xBF4 bytes = 255 glyph entries × 12 bytes each (3×i32) */
            ls_read_buffer(car_fp, desc + 4, 0xBF4);
            /* Read trailing 4 bytes (band_height stored at the end of .CAR) */
            u8 trail[4] = {0};
            ls_read_buffer(car_fp, trail, 4);
            ls_close(car_fp);
            print("sub_415FB0: FONTE11.CAR loaded (0xBF4 glyph bytes)\n");
        } else {
            print("sub_415FB0: FONTE11.CAR not found at '%s'\n", resolved);
        }
    }

    /* ---- Step 2: load .GIF atlas via image_load_compressed ---- */
    /* Build "DATAS>MENUS>FONTE11.GIF" */
    snprintf(path, sizeof(path), "%s.GIF", base_name);
    void *atlas_bmap = image_load_compressed(path, NULL /* ui_health_buffer */);

    /* Store bmap ptr at [desc+0] and height 0x13 at [desc+0BF8h] */
    memcpy(desc + 0,      &atlas_bmap, sizeof(void *));
    i32 glyph_height = 0x13;
    memcpy(desc + 0xBF8,  &glyph_height, sizeof(i32));

    if (atlas_bmap) {
        bmap_t *bm = (bmap_t *)atlas_bmap;
        print("sub_415FB0: FONTE11 atlas -> %dx%d, glyph_height=0x13\n",
              (i32)bm->width, bm->height);
    } else {
        /* Fallback: if the real atlas failed, use the already-loaded font_handle
         * atlas (init_font_system loaded FONTE11.CIF via cif_load). */
        if (font_handle && font_handle->atlas_pixels) {
            /* Wrap the existing atlas into a stack-local bmap_t for the desc ptr.
             * NOTE: This is a shallow reference — do NOT free via image_load_compressed_free. */
            static bmap_t s_atlas_bmap;
            memset(&s_atlas_bmap, 0, sizeof(s_atlas_bmap));
            s_atlas_bmap.width       = (i16)font_handle->atlas_width;
            s_atlas_bmap.height      = font_handle->atlas_height;
            s_atlas_bmap.bpp_flags   = 8;
            s_atlas_bmap.bit_depth   = 8;
            s_atlas_bmap.stride      = font_handle->atlas_width;
            s_atlas_bmap.pixels      = font_handle->atlas_pixels;
            s_atlas_bmap.first_pixel = font_handle->atlas_pixels;
            s_atlas_bmap.owns_buffer = 0; /* do not free — owned by font_handle */
            void *fallback_ptr = (void *)&s_atlas_bmap;
            memcpy(desc + 0, &fallback_ptr, sizeof(void *));
            print("sub_415FB0: using FONTE11 atlas from font_handle (%dx%d)\n",
                  font_handle->atlas_width, font_handle->atlas_height);
        } else {
            print("sub_415FB0: FONTE11 atlas unavailable — font descriptor bmap is NULL\n");
        }
    }

    /* ---- Step 3: zero word_4D0A40 (≈0x4ED words) ---- */
    /* word_4D0A40 is a global bitmask table, not yet mapped in our port.
     * Original: xor eax,eax; mov ecx,976h; rep stosd; stosw
     * This is a ~2.5 KB zero-init; safe to skip until the table is mapped. */
    /* TODO: memset(word_4D0A40, 0, 0x976*4 + 2) when word_4D0A40 lands in state */

    /* ---- Step 4: glyph bitmask walk ---- */
    /* TODO: walk unk_4CFFC4..unk_4D0A44 (stride 0Ch, 0x260 entries), populate
     * word_4D0A40[edx*2] from atlas pixel data (sub_415FB0+15E..+1BAh).
     * Requires word_4D0A40 global and unk_4CFFC4 layout — deferred. */

    print("sub_415FB0: font descriptor built (using FONTE11 atlas)\n");
}

/* -------------------------------------------------------------------------
 * 004161B0 — game_finalize
 *
 * Original asm (entire body):
 *   mov  edx, offset unk_4CFE40
 *   mov  ecx, offset aDatasMenusFont ; "DATAS>MENUS>FONTE11"
 *   jmp  sub_415FB0
 *
 * Side-effect preserved here: g_dword_4B787C = 0 is done by game_init after
 * this call (phase 45).
 * ---------------------------------------------------------------------- */

// 004161B0
static void game_finalize(void) {
    /* ecx = "DATAS>MENUS>FONTE11", edx = &g_unk_4CFE40 */
    sub_415FB0("DATAS>MENUS>FONTE11", g_unk_4CFE40);
}

/* -------------------------------------------------------------------------
 * 0046BEEB — game-side bootstrap
 * ---------------------------------------------------------------------- */

b8 game_init(state_t *state) {

    /* Suppress warnings for stub globals that will be written by future
     * ported sub-functions; they are declared here so their asm addresses
     * are documented and names are reserved for cross-reference. */
    (void)g_palette_data;
    /* g_player_entity is now in player.c (extern via player.h) */
    (void)g_game_context;
    (void)g_entity_buffer_base;
    /* New globals populated by ported functions — suppress read-before-use
     * warnings from -Wall until callers of these values are ported. */
    (void)g_dword_4B297C;
    (void)g_dword_4D7F0C;
    (void)g_dword_4D7F24;
    (void)g_dword_4D7F10;
    (void)g_dword_4D7F20;
    (void)g_dword_4D7F2C;
    (void)g_dword_4F4DF4;
    (void)g_unk_4CFE40;

    /* ------------------------------------------------------------------ */
    /* Phase 1 — debug banner                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: starting\n");

    if (state->debug_enabled) {
        set_position_coordinates(0, 32);   /* 0046BEEB+10 */
        draw_text("Init Scene ...");       /* offset aInitScene */
    }

    /* ------------------------------------------------------------------ */
    /* Phase 2 — HUD health bitmap (GINETTE.CIF / fallback bmap)           */
    /* ------------------------------------------------------------------ */
    print("game_init: loading HUD health bitmap\n");

    {
        /* Check whether the localised CIF variant exists.                  *
         * Original: select_language_version("DATAS>MENUS>GINETTE.CIF")     *
         *   → non-zero: load "DATAS>MENUS>GINETTE.GIF" compressed          *
         *   → zero:     create a blank bmap (0x14 × 0x4B, 8-bit)           */
        c8 ginette_resolved[512];
        c8 *lang_variant = select_language_version(
            "DATAS>MENUS>GINETTE.CIF", ginette_resolved, (i32)sizeof(ginette_resolved)); /* 46BF17 */
        if (lang_variant) {
            g_ginette_bmap = image_load_compressed(
                "DATAS>MENUS>GINETTE.GIF",
                state->game.ui_health_buffer); /* ui_health_buffer slot */
        } else {
            /* fallback — allocate 0x14 wide × 0x4B high, 8-bit bmap */
            g_ginette_bmap = ls_init_bmap_ex(
                0x14, 0x4B, 8, 8);             /* 46BF43 */
        }
    }

    /* ------------------------------------------------------------------ */
    /* Phase 3 — LOADING.GIF compressed image load                         */
    /* ------------------------------------------------------------------ */
    print("game_init: loading LOADING.GIF\n");

    g_loading_bmap = image_load_compressed(
        "DATAS>MENUS>LOADING.GIF",
        NULL /* _ls_mem_buffer — asm global, not yet in state_t */); /* _ls_mem_buffer */
    if (g_loading_bmap) {
        bmap_t *_bm = (bmap_t *)g_loading_bmap;
        print("game_init: LOADING.GIF -> %dx%d\n", (i32)_bm->width, _bm->height);
    }

    /* ------------------------------------------------------------------ */
    /* Phase 4 — Transparency pass on LOADING.GIF pixels                   */
    /* Original: walk pixel array, set non-0xFF entries to 0               */
    /* ------------------------------------------------------------------ */
    print("game_init: applying LOADING.GIF transparency pass\n");

    if (g_loading_bmap) {
        /* renderer_context is a bmap_t; pixel count = width * height       *
         * bmap layout: +4=width i32, +8=height i32, +0Ch=pixels u8*        */
        bmap_t *bmap = (bmap_t *)g_loading_bmap;
        if (bmap->pixels) {
            i32 pixel_count = (i32)bmap->width * bmap->height;
            for (i32 i = 0; i < pixel_count; i++) {
                if (bmap->pixels[i] != 0xFF) {
                    bmap->pixels[i] = 0;    /* 46BFCE */
                }
            }
        }
    }

    /* ------------------------------------------------------------------ */
    /* Phase 5 — Two HUD bmap copies from GINETTE dims                     */
    /* ------------------------------------------------------------------ */
    print("game_init: allocating HUD bmap copies\n");

    if (g_ginette_bmap) {
        bmap_t *src = (bmap_t *)g_ginette_bmap;
        /* dword_4DE648 — first copy */
        g_health_bmap_a = ls_init_bmap_ex(
            (i32)src->width, src->height, 8, 8);  /* 46BFCE */
        /* dword_4DE64C — second copy */
        g_health_bmap_b = ls_init_bmap_ex(
            (i32)src->width, src->height, 8, 8);  /* 46BFCE+16 */
    } else {
        /* Fallback: allocate with minimal size so the rest of init proceeds */
        g_health_bmap_a = ls_init_bmap_ex(0x14, 0x4B, 8, 8);
        g_health_bmap_b = ls_init_bmap_ex(0x14, 0x4B, 8, 8);
    }

    /* ------------------------------------------------------------------ */
    /* Phase 6 — Gouraud-shading index buffer (0x4100 bytes)               */
    /* ------------------------------------------------------------------ */
    print("game_init: allocating gouraud index buffer\n");

    /* allocate_memory(ui_health_buffer_slot, 0x4100) @ 46BFCE+3C          */
    g_gouraud_table = allocate_memory(
        MEM_SLOT_HEALTH, 0x4100);              /* sub_4130AA @ 4130AA */

    if (g_gouraud_table) {
        /* index_buffer = (g_gouraud_table & 0xFFFFFF00) + 0x100           */
        g_index_buffer = (void *)(
            ((uintptr_t)g_gouraud_table & (uintptr_t)0xFFFFFF00u)
            + (uintptr_t)0x100);               /* 46C051 */
    }

    /* ------------------------------------------------------------------ */
    /* Phase 7 — MENUGEN.GIF palette load + input-state table zero-init    */
    /* ------------------------------------------------------------------ */
    print("game_init: loading MENUGEN.GIF palette\n");

    {
        /* load_image_color_data → colour palette from MENUGEN.GIF          */
        void *menugen_palette = load_image_color_data(
            "DATAS>MENUS>MENUGEN.GIF",
            NULL /* _ls_mem_buffer — asm global, not yet in state_t */);         /* _ls_mem_buffer */
        if (menugen_palette) {
            print("game_init: MENUGEN.GIF palette loaded -> %p\n", menugen_palette);
        }

        /* memcpy 0x300 bytes from palette into unk_4DDF30                  *
         * memset unk_4DDF30 first 0x180 bytes to 0                         *
         * These are file-local statics we don't track; skip inline.        */
        /* TODO: memcpy/memset to unk_4DDF30 @ 46C05F-46C071 */

        /* Release temporary palette */
        ls_free_palette(menugen_palette);       /* 46C076 */

        /* memset input_state_table[0x100] to 0 @ 46C07B */
        /* TODO: input_state_table global not yet in state_t */

        /* Second pass: load MENUGEN.GIF palette into dword_4B71F8          */
        /* TODO: load_image_color_data @ 46C083 */
        g_menugen_palette = load_image_color_data(
            "DATAS>MENUS>MENUGEN.GIF",
            state->game.ui_health_buffer);      /* ui_health_buffer */

        /* Port-only extra: keep the full MENUGEN bmap around so we can use
         * it as the menu backdrop until the real main-menu renderer lands. */
        g_menugen_bmap = image_load_compressed("DATAS>MENUS>MENUGEN.GIF", NULL);
        if (g_menugen_bmap) {
            bmap_t *bm = (bmap_t *)g_menugen_bmap;
            print("game_init: MENUGEN backdrop bmap loaded -> %dx%d\n",
                  (i32)bm->width, bm->height);
        }

        /* TODO: update_palette_with_default_colors @ 46C088 */
        /* TODO: ls_build_palette_colour @ 46C08E */
    }

    /* ------------------------------------------------------------------ */
    /* Phase 8 — sub_425D50 (palette post-process / persona inventory)    */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_425D50 (palette post-process)\n");
    palette_postprocess_configure(state->game_path, g_is_using_hdd); /* sub_425D50 */

    /* ------------------------------------------------------------------ */
    /* Phase 9 — initialize_health_hud                                     */
    /* ------------------------------------------------------------------ */
    print("game_init: initializing health HUD\n");
    /* TODO: initialize_health_hud @ 0046C092 — allocates 2048-byte HUD buffer */

    /* ------------------------------------------------------------------ */
    /* Phase 10 — sub_45D395 (mouse + animation init)                     */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_45D395\n");
    initialize_mouse_and_animation(); /* sub_45D395 */

    /* ------------------------------------------------------------------ */
    /* Phase 11 — sub_421960                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_421960 (inverse-Z buffer)\n");
    g_inv_z_array = allocate_memory(0, 0x4000);
    if (g_inv_z_array) memset(g_inv_z_array, 0, 0x4000);

    /* ------------------------------------------------------------------ */
    /* Phase 12 — sub_41F7C8                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_41F7C8 (animation system init)\n");
    g_anim_buffer = allocate_memory(0, 54 * 1024);
    g_pos_buffer  = allocate_memory(0, 330 * 1024);
    g_msg_buffer  = allocate_memory(0, 12 * 1024);
    if (g_anim_buffer) memset(g_anim_buffer, 0, 54 * 1024);
    if (g_pos_buffer)  memset(g_pos_buffer,  0, 330 * 1024);
    if (g_msg_buffer)  memset(g_msg_buffer,  0, 12 * 1024);
    for (i32 i = 0; i < 4; i++) {
        g_can_ptrs[i] = allocate_memory(0, 5 * 1024);
        g_can_indices[i] = -1;
    }

    /* ------------------------------------------------------------------ */
    /* Phase 13 — sub_44D706                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_44D706 (sound sample tables)\n");
    if (g_samples_enabled) {
        memset(g_sample_registry, 0, sizeof(g_sample_registry));
        memset(g_sample_flags, 0, sizeof(g_sample_flags));
    }

    /* ------------------------------------------------------------------ */
    /* Phase 14 — sub_43040A                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_43040A\n");
    /* TODO: sub_43040A @ 0043040A */

    /* ------------------------------------------------------------------ */
    /* Phase 15 — weapon-slot table init (4 entries × 31 slots)            */
    /* Original loop: i in [0,4), j in [0,0x1F)                            */
    /*   dword_4ECD50[stride] = 0       (head sentinel)                    */
    /*   dword_4ECDD4[stride] = 0x3FF   (tail sentinel)                    */
    /*   dword_4ECD54[stride + j*4] = 0xFFFFFFFF (empty slot marker)       */
    /*                                                                      */
    /* Stride formula from asm: i*(1+i*4*2)*8 = i * (1+8i) * 8            */
    /* Simplified: stride = ((i + i*4) + i)*2 << 3 = i*11*8 = i*88         */
    /* ------------------------------------------------------------------  */
    print("game_init: init weapon-slot tables\n");

    /*
     * NOTE: dword_4ECD50/4ECDD4/4ECD54 are raw asm globals not yet in
     * state_t. We record the init intent here as inline comments and skip
     * the actual pointer writes since those addresses aren't mapped.
     * When those globals land in state_t the writes below should be
     * uncommented.
     */
    for (i32 i = 0; i < 4; i++) {
        /* stride per asm: ((i + i*4)*2 + i*2)*4 = see 46C11B–46C194 */
        /* TODO: dword_4ECD50[stride] = 0;        @ 004ECD50 */
        /* TODO: dword_4ECDD4[stride] = 0x3FF;    @ 004ECDD4 */
        for (i32 j = 0; j < 0x1F; j++) {
            /* TODO: dword_4ECD54[stride + j*4] = 0xFFFFFFFF; @ 004ECD54 */
        }
    }

    /* ------------------------------------------------------------------ */
    /* Phase 16 — entity-slot table init (0x62=98 entries × 31 slots)     */
    /* Same structure as weapon-slot but 98 entries, diff stride calc.      */
    /* stride = (i*0x11) << 5  (from asm: shl eax,4; add ecx; shl,5)      */
    /* ------------------------------------------------------------------  */
    print("game_init: init entity-slot tables\n");

    for (i32 i = 0; i < 0x62; i++) {
        /* TODO: dword_4DF5D0[stride] = 0;        @ 004DF5D0 */
        /* TODO: dword_4DF654[stride] = 0;        @ 004DF654 */
        for (i32 j = 0; j < 0x1F; j++) {
            /* TODO: dword_4DF5D4[stride + j*4] = 0xFFFFFFFF; @ 004DF5D4 */
        }
    }

    /* ------------------------------------------------------------------ */
    /* Phase 17 — misc global zero-inits (memset calls)                    */
    /* ------------------------------------------------------------------ */
    print("game_init: zero-init global tables\n");

    g_dword_4ECD3C = 0;                        /* dword_4ECD3C = 0 @ 46C21E */

    /* TODO: memset(dword_4EC6A0, 0, 0x188) @ 004EC6A0 */
    /* TODO: memset(companion_entity, 0, 8)             */
    /* TODO: memset(dword_4EE0F0, 0, 0x6BD0)            */

    /* dword_543B20 = 0; dword_543B1C = dword_543B20; */
    /* TODO: dword_543B20 / dword_543B1C @ 00543B20/1C */

    /* TODO: memset(unk_4DAB60, 0, 0x860)               */
    /* TODO: memset(dword_4DCDA0, 0xFF, 0x9D8)          */
    /* TODO: memset(dword_4DC2E0, 0xFF, 0x9D8)          */
    /* TODO: memset(byte_4DCCC0, 0, 0xD2)               */

    /* ------------------------------------------------------------------ */
    /* Phase 18 — debug banner "Init Divers ..."                           */
    /* ------------------------------------------------------------------ */
    if (state->debug_enabled) {
        set_position_coordinates(0, 32);       /* 46C2E8 */
        draw_text("Init Divers ...");
    }

    /* ------------------------------------------------------------------ */
    /* Phase 19 — FLAM.MAT material load                                   */
    /* ------------------------------------------------------------------ */
    print("game_init: loading FLAM.MAT material\n");

    {
        const c8 *flam_path = g_is_using_hdd
            ? "DATAS>OBJETS>FLAM.MAT"          /* 46C2F4 — HDD path */
            : "OBJETS>FLAM>FLAM.MAT";          /* 46C30F — CD path  */

        /* TODO: sub_45018B @ 0045018B — material/resource loader */
        g_flam_mat = NULL; /* sub_45018B(flam_path, ui_health_buffer) */
        (void)flam_path;
    }

    /* ------------------------------------------------------------------ */
    /* Phase 20 — allocate_and_initialize_health_buffer                    */
    /* ------------------------------------------------------------------ */
    print("game_init: allocate_and_initialize_health_buffer\n");
    /* TODO: allocate_and_initialize_health_buffer @ 46C324 */

    /* ------------------------------------------------------------------ */
    /* Phase 21 — sub_42F88E                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_42F88E\n");
    /* TODO: sub_42F88E @ 0042F88E */

    /* ------------------------------------------------------------------ */
    /* Phase 22 — register_player_creature (ECX=1)                         */
    /* ------------------------------------------------------------------ */
    print("game_init: register_player_creature\n");
    /* TODO: register_player_creature @ 0046C32C — ECX=1 configures serpent/fish/bird */

    /* ------------------------------------------------------------------ */
    /* Phase 23 — init_font_system                                          */
    /* ------------------------------------------------------------------ */
    print("game_init: init_font_system\n");
    init_font_system();                        /* 0046C330 */

    /* ------------------------------------------------------------------ */
    /* Phase 24 — sub_44D676                                               */
    /* ------------------------------------------------------------------ */
    /* Phase 24 — sub_44D676: confirmed empty stub in original (push/pop/ret) */

    /* ------------------------------------------------------------------ */
    /* Phase 25 — sub_48F2C0 — texture config init (3E8h = 1000 entries)   */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_48F2C0 texture config init\n");
    /* TODO: sub_48F2C0 @ 0048F2C0 — ECX=&texture_config, EDX=0x3E8, push 0x3E8 */

    /* ------------------------------------------------------------------ */
    /* Phase 26 — sub_48F230 — entity buffer setup (640×480)               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_48F230 entity buffer setup\n");
    /* TODO: sub_48F230 @ 0048F230 — ECX=dword_56A5E0, EDX=&entity_buffer_base,
     *   push 0, 0, 0x280, 0x1E0, dword_4B7228[dword_4B721C*4]            */

    /* ------------------------------------------------------------------ */
    /* Phase 27 — render bmap creation                                     */
    /* Colour-mode branch: 0 → create_image_buffer; non-0 → ls_init_bmap_ex */
    /* ------------------------------------------------------------------ */
    print("game_init: creating render bmaps\n");

    if (state->colour_mode == 0) {
        /* 8-bit palette mode */
        /* render_buffer1 = create_image_buffer(0x280, 0x1E0, 8) @ 46C3D2 */
        g_render_buffer1 = ls_init_bmap_ex(0x280, 0x1E0, 8, 8);
        /* TODO: create_image_buffer is a thin alias; ls_init_bmap_ex suffices */

        /* debug_surface = create_image_buffer(0x280, 0x1E0, 8) @ 46C3DC */
        g_debug_surface = ls_init_bmap_ex(0x280, 0x1E0, 8, 8);
        g_bmap_4B71FC   = g_debug_surface;    /* dword_4B71FC = debug_surface */
    } else {
        /* 16-bit colour mode (our active path) */
        /* render_buffer1 = ls_init_bmap_ex(0x280, 0x1E0, 16, 0x10) @ 46C3C9 */
        g_render_buffer1 = ls_init_bmap_ex(0x280, 0x1E0, 16, 0x10); /* 46C3C9 */

        /* dword_4B71FC  = ls_init_bmap_ex(0x280, 0x1E0, 8, 8)   @ 46C3D9 */
        g_bmap_4B71FC   = ls_init_bmap_ex(0x280, 0x1E0,  8,  8); /* 46C3D9 */

        /* dword_4B7200  = ls_init_bmap_ex(0x280, 0x1E0, 16, 0x10) @ 46C3E9 */
        g_bmap_4B7200   = ls_init_bmap_ex(0x280, 0x1E0, 16, 0x10); /* 46C3E9 */

        /* debug_surface = ls_init_bmap_ex(0x280, 0x1E0, 16, 0x10) @ 46C3F9 */
        g_debug_surface = ls_init_bmap_ex(0x280, 0x1E0, 16, 0x10); /* 46C3F9 */
    }

    /* ------------------------------------------------------------------ */
    /* Phase 28 — render_buffer2 (640×480) and dword_507474 (320×240)      */
    /* ------------------------------------------------------------------ */
    print("game_init: creating render_buffer2 / half-res buffer\n");

    /* render_buffer2 = create_image_buffer(ui_health_buffer, 0x280, 0x1E0) @ 46C429 */
    g_render_buffer2 = ls_init_bmap_ex(0x280, 0x1E0, 8, 8); /* 46C429 */

    /* dword_507474 = create_image_buffer(ui_health_buffer, 0x140, 0xF0) @ 46C437 */
    g_bmap_507474 = ls_init_bmap_ex(0x140, 0x0F0, 8, 8);    /* 46C437 */

    /* ------------------------------------------------------------------ */
    /* Phase 29 — datas>objets>objets.gif load                             */
    /* ------------------------------------------------------------------ */
    print("game_init: loading objets.gif\n");
    g_objets_gif = image_load_compressed(
        "datas>objets>objets.gif",
        state->game.ui_health_buffer);
    if (g_objets_gif) {
        bmap_t *_bm = (bmap_t *)g_objets_gif;
        print("game_init: objets.gif -> %dx%d\n", (i32)_bm->width, _bm->height);
    }

    /* ------------------------------------------------------------------ */
    /* Phase 30 — sub_4130AA — resource/sprite table initialiser           */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_4130AA resource init\n");
    initialize_resource_slots(); /* sub_4130AA */

    /* ------------------------------------------------------------------ */
    /* Phase 31 — initialize_resource + palette_data wire-up               */
    /* ------------------------------------------------------------------ */
    print("game_init: initialize_resource + palette wire-up\n");
    /* TODO: initialize_resource @ 0046C453 — ECX=ui_health_buffer slot;
     *       result → palette_data                                         */
    /* g_palette_data = initialize_resource(state->game.ui_health_buffer); */

    /* ------------------------------------------------------------------ */
    /* Phase 32 — game_state / game_death_state from dword_4B70E8          */
    /* ------------------------------------------------------------------ */
    print("game_init: wiring game_state / game_death_state\n");
    /* dword_4B70E8 is a global entity table not yet in state_t.           *
     * TODO: wire dword_4B70E8 → game_state → game_death_state @ 46C460   */
    g_game_state       = g_dword_4B70E8;
    g_game_death_state = g_game_state;

    /* ------------------------------------------------------------------ */
    /* Phase 33 — sub_46493B — player entity setup                         */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_46493B player entity setup\n");
    player_entity_init();

    /* ------------------------------------------------------------------ */
    /* Phase 34 — sub_464F6D — player entity secondary init                */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_464F6D player entity secondary init\n");
    player_entity_init_secondary();

    /* ------------------------------------------------------------------ */
    /* Phase 35 — entity/context palette cross-wiring                      */
    /* player_entity+B0h = dword_4B70E8                                    */
    /* game_context = entity_buffer_base                                    */
    /* game_context+18h = palette_data                                     */
    /* render_buffer1+18h = palette_data                                   */
    /* graphics_context+18h = palette_data                                 */
    /* ------------------------------------------------------------------ */
    print("game_init: entity/context cross-wiring\n");
    /* TODO: pointer writes @ 46C484–46C4B1 — waiting for entity types    */

    /* ------------------------------------------------------------------ */
    /* Phase 36 — system_get_gametime → player_entity +D5Ch               */
    /* ------------------------------------------------------------------ */
    print("game_init: system_get_gametime → player_entity\n");
    /* TODO: system_get_gametime @ 46C4B4 — ECX=player_entity, [ecx+D5Ch]=eax */

    /* ------------------------------------------------------------------ */
    /* Phase 37 — sub_48F1D0 — render surface setup ×2                    */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_48F1D0 render surface setup\n");
    /* TODO: sub_48F1D0 @ 0048F1D0 ×2:
     *   push 0Fh; ECX=dword_4B71FC; EDX=&render_surface2        @ 46C4BA
     *   push 0Fh; ECX=debug_surface; EDX=&unk_4EDC00            @ 46C4C6 */

    /* ------------------------------------------------------------------ */
    /* Phase 38 — set_entities_combat_mode                                 */
    /* ------------------------------------------------------------------ */
    print("game_init: set_entities_combat_mode\n");
    /* TODO: set_entities_combat_mode @ 46C4D2 */

    /* ------------------------------------------------------------------ */
    /* Phase 39 — sub_426335                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_426335\n");
    /* TODO: sub_426335 @ 00426335 */

    /* ------------------------------------------------------------------ */
    /* Phase 40 — sub_471C18 — load objets.txt                             */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_471C18 loading objets.txt\n");
    /* TODO: sub_471C18 @ 00471C18 — ECX="objets>objets.txt" */

    /* ------------------------------------------------------------------ */
    /* Phase 41 — special_mode_active → dword_505DD8                      */
    /* ------------------------------------------------------------------ */
    g_dword_505DD8 = g_special_mode_active;   /* 46C4E4 */

    /* ------------------------------------------------------------------ */
    /* Phase 42 — 16-bit colour branch: sub_491ED0 + sub_491FD0            */
    /* ------------------------------------------------------------------ */
    if (state->colour_mode != 0) {
        print("game_init: sub_491ED0 + sub_491FD0 (16-bit colour init)\n");
        /* TODO: sub_491ED0 @ 00491ED0 */
        /* TODO: sub_491FD0 @ 00491FD0 — push 0x3C; ECX=dword_4B71F8;
         *       EDX=graphics_context[+2Ch]                               */
    }

    /* ------------------------------------------------------------------ */
    /* Phase 43 — sub_45B207                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_45B207\n");
    /* TODO: sub_45B207 @ 0045B207 */

    /* ------------------------------------------------------------------ */
    /* Phase 44 — display_weapons_inventory                                */
    /* ------------------------------------------------------------------ */
    print("game_init: display_weapons_inventory\n");
    /* TODO: display_weapons_inventory @ 0046C57F — displays ammo/items in UI */

    /* ------------------------------------------------------------------ */
    /* Phase 45 — sub_4161B0 + dword_4B787C = 0 (tail cleanup)            */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_4161B0 (final init)\n");
    game_finalize();                           /* sub_4161B0 @ 004161B0 */
    g_dword_4B787C = 0;                       /* 46C57E */

    print("game_init: done\n");
    return true;
}

/* -------------------------------------------------------------------------
 * game_init_load_cursor — load a single CIF cursor image.
 *
 * Wraps image_load_compressed with explicit path resolution and GIF→CIF
 * fallback.  Returns a heap-owned bmap_t (with embedded palette) on success,
 * NULL on failure.  The caller owns the result and must free it with
 * image_load_compressed_free / free().
 *
 * Used by menu_init to load CURS128.CIF and GENICO_%01d.CIF frames.
 * ---------------------------------------------------------------------- */
bmap_t *game_init_load_cursor(const c8 *virtual_path) {
    return (bmap_t *)image_load_compressed(virtual_path, NULL);
}

/* -------------------------------------------------------------------------
 * game_init_get_menu_backdrop — accessor for backdrop bmap + palette.
 *
 * Returns pointers to the LOADING.GIF bmap (g_loading_bmap) and the
 * MENUGEN.GIF palette (g_menugen_palette / color_table).  Either or both
 * may be NULL if their respective load phases failed.
 * ---------------------------------------------------------------------- */
void game_init_get_menu_backdrop(bmap_t **out_bmap, u8 **out_palette) {
    /* Prefer full-screen MENUGEN backdrop (640×480); fall back to LOADING
     * banner (512×64) if MENUGEN failed to load. */
    void *bmap = g_menugen_bmap ? g_menugen_bmap : g_loading_bmap;
    if (out_bmap) {
        *out_bmap = (bmap_t *)bmap;
    }
    if (out_palette) {
        /* color_table is always available (zeroed at startup, written in phase 7) */
        *out_palette = color_table;
    }
    print("game_init_get_menu_backdrop: bmap=%p palette=%p\n",
          (void *)bmap, (void *)color_table);
}
