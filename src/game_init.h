#pragma once

/*
 * game_init.h — game-side bootstrap (0046BEEB)
 *
 * game_init() runs after ls_startup() + init_all_systems() complete.
 * It wires up the HUD (health, GINETTE.CIF), weapons inventory,
 * entity tables, combat buffers, render bmaps, font, and palette.
 *
 * All work previously done in game_init_46BEEB (called from ls_main+66D).
 */

#include "libsys/common.h"
#include "libsys/graphics/graphic.h"
#include "state.h"

/* Combat / creature definition table entry (sub_471C18).
 * 100 entries × 20 bytes, loaded from DATAS>OBJETS>OBJETS.TXT.
 * Original addresses: combat_ability_table @ 004DB860. */
typedef struct {
    i32 nbobj;          /* +0x00: object count */
    i32 nbvie;          /* +0x04: health/lives */
    i32 ptvie;          /* +0x08: health points */
    i32 ptnrj;          /* +0x0C: energy points */
    u8  flags;          /* +0x10: combat ability bitmask */
    u8  poly_index;     /* +0x11: polygon object slot */
    u8  weapon_index;   /* +0x12: weapon slot */
    u8  linke;          /* +0x13: linked entity id */
} creature_entry_t;

/* Creature table accessors — hide the static arrays in game_init.c */
const creature_entry_t *game_init_get_creature(i32 id);
u8   game_init_get_ability(i32 creature_id, i32 slot);
void game_init_set_ability(i32 creature_id, i32 slot, u8 mask_clear, u8 mask_set);

/* 0046BEEB — game-side bootstrap. Runs after ls_startup + init_all_systems. */
b8 game_init(state_t *state);

/*
 * game_init_get_menu_backdrop — return loaded backdrop bmap and palette.
 *
 * Provides the LOADING.GIF bmap and MENUGEN.GIF palette loaded during
 * game_init phase 3 and phase 7 respectively.  Used by ls_main to blit
 * a visible menu backdrop into the front_buffer after game_init completes.
 *
 * @param out_bmap     receives pointer to g_loading_bmap (may be NULL if load failed)
 * @param out_palette  receives pointer to g_menugen_palette (768-byte RGB table)
 */
void game_init_get_menu_backdrop(bmap_t **out_bmap, u8 **out_palette);

/*
 * game_init_load_cursor — load a CIF cursor/sprite image by virtual path.
 *
 * Wraps image_load_compressed: resolves virtual path → host path, loads CIF,
 * returns a heap-owned bmap_t with embedded palette (bmap->palette, 768 bytes).
 * Returns NULL on failure (file missing or decode error); does not crash.
 *
 * Used by menu_init to load CURS128 and GENICO_* frames.
 * Caller is responsible for freeing the returned bmap (use free() on both
 * bmap->pixels and bmap->palette then bmap itself, or image_load_compressed_free
 * via the static helper in game_init.c).
 */
bmap_t *game_init_load_cursor(const c8 *virtual_path);
