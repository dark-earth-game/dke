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

#include "libsys/common.h"
#include "libsys/memory.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/font.h"
#include "state.h"
#include "game_init.h"

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
static void *g_player_entity      = NULL;   /* player_entity (asm sym)  */
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

/* -------------------------------------------------------------------------
 * Forward declarations for sub-functions not yet ported.
 * Each has the original address in its comment.
 * ---------------------------------------------------------------------- */

/* HUD / image helpers */
static void *select_language_version(const c8 *path);            /* no direct addr; see 46BF17 */
static void *image_load_compressed(const c8 *path, void *buf);   /* no direct addr; see 46BF5F */
static void *load_image_color_data(const c8 *path, void *buf);   /* no direct addr; see 46C051 */
static void  ls_free_palette(void *palette);                      /* no direct addr; see 46C076 */

/* -------------------------------------------------------------------------
 * Stub implementations of sub-functions not yet ported
 * ---------------------------------------------------------------------- */

/* select_language_version — test if a language-specific CIF variant exists.
 * Returns non-NULL if the file exists for the current language. */
static void *select_language_version(const c8 *path) {
    (void)path;
    /* TODO: sub_XXXXXX @ 46BF17 vicinity — check language CIF path */
    return NULL; /* fallback: use base bmap path */
}

/* image_load_compressed — load a compressed GIF/image into a memory buffer.
 * Returns a bmap_t-like handle on success, NULL on failure. */
static void *image_load_compressed(const c8 *path, void *buf) {
    (void)path; (void)buf;
    /* TODO: sub_XXXXXX @ 46BF5F vicinity — load compressed image */
    return NULL;
}

/* load_image_color_data — load image and extract palette/colour data.
 * Returns a palette handle. */
static void *load_image_color_data(const c8 *path, void *buf) {
    (void)path; (void)buf;
    /* TODO: sub_XXXXXX @ 46C051 vicinity — load image colour data */
    return NULL;
}

/* ls_free_palette — release a palette handle. */
static void ls_free_palette(void *palette) {
    (void)palette;
    /* TODO: sub_XXXXXX @ 46C076 vicinity — free palette */
}

/* -------------------------------------------------------------------------
 * 0046BEEB — game-side bootstrap
 * ---------------------------------------------------------------------- */

b8 game_init(state_t *state) {

    /* Suppress warnings for stub globals that will be written by future
     * ported sub-functions; they are declared here so their asm addresses
     * are documented and names are reserved for cross-reference. */
    (void)g_palette_data;
    (void)g_player_entity;
    (void)g_game_context;
    (void)g_entity_buffer_base;

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
        void *lang_variant = select_language_version("DATAS>MENUS>GINETTE.CIF"); /* 46BF17 */
        if (lang_variant) {
            /* TODO: image_load_compressed @ 46BF43 */
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

    /* TODO: image_load_compressed @ 46BF5F */
    g_loading_bmap = image_load_compressed(
        "DATAS>MENUS>LOADING.GIF",
        NULL /* _ls_mem_buffer — asm global, not yet in state_t */); /* _ls_mem_buffer */

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
        /* TODO: load_image_color_data @ 46C051 */
        void *menugen_palette = load_image_color_data(
            "DATAS>MENUS>MENUGEN.GIF",
            NULL /* _ls_mem_buffer — asm global, not yet in state_t */);         /* _ls_mem_buffer */

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

        /* TODO: update_palette_with_default_colors @ 46C088 */
        /* TODO: ls_build_palette_colour @ 46C08E */
    }

    /* ------------------------------------------------------------------ */
    /* Phase 8 — sub_425D50 (unknown — after palette wire-up)             */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_425D50 (palette post-process)\n");
    /* TODO: sub_425D50 @ 004625D50 — purpose unknown, called after palette */

    /* ------------------------------------------------------------------ */
    /* Phase 9 — initialize_health_hud                                     */
    /* ------------------------------------------------------------------ */
    print("game_init: initializing health HUD\n");
    /* TODO: initialize_health_hud @ 0046C092 — allocates 2048-byte HUD buffer */

    /* ------------------------------------------------------------------ */
    /* Phase 10 — sub_45D395                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_45D395\n");
    /* TODO: sub_45D395 @ 0045D395 */

    /* ------------------------------------------------------------------ */
    /* Phase 11 — sub_421960                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_421960\n");
    /* TODO: sub_421960 @ 00421960 */

    /* ------------------------------------------------------------------ */
    /* Phase 12 — sub_41F7C8                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_41F7C8\n");
    /* TODO: sub_41F7C8 @ 0041F7C8 */

    /* ------------------------------------------------------------------ */
    /* Phase 13 — sub_44D706                                               */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_44D706\n");
    /* TODO: sub_44D706 @ 0044D706 */

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
    print("game_init: sub_44D676\n");
    /* TODO: sub_44D676 @ 0044D676 — labelled __wsetargv in IDA (doubtful) */

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
    /* TODO: image_load_compressed @ 46C445 — "datas>objets>objets.gif" */
    g_objets_gif = image_load_compressed(
        "datas>objets>objets.gif",
        state->game.ui_health_buffer);

    /* ------------------------------------------------------------------ */
    /* Phase 30 — sub_4130AA — resource/sprite table initialiser           */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_4130AA resource init\n");
    /* TODO: sub_4130AA @ 004130AA — ECX=ui_health_buffer slot            */

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
    /* TODO: sub_46493B @ 0046493B — ECX=dword_4B70E8, EDX=ui_weapon_slot_buffer;
     *       result → player_entity                                        */

    /* ------------------------------------------------------------------ */
    /* Phase 34 — sub_464F6D — player entity secondary init                */
    /* ------------------------------------------------------------------ */
    print("game_init: sub_464F6D player entity secondary init\n");
    /* TODO: sub_464F6D @ 00464F6D — ECX=player_entity                    */

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
    /* TODO: sub_4161B0 @ 004161B0 */
    g_dword_4B787C = 0;                       /* 46C57E */

    print("game_init: done\n");
    return true;
}
