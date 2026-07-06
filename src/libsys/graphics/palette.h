#pragma once

#include "../common.h"
#include "../file_system.h"

/*
 * Palette buffers — mirrors of original data-segment addresses.
 *
 *   alternative_palette_data  (0x004C64EC area — exact addr TBD from data seg)
 *   palette_target_buffer     (0x004C64EC area — exact addr TBD from data seg)
 *
 * Both are 34 dwords = 136 bytes, matching the rep movsd ecx=0x22 in sub_491450.
 */

/* 136-byte source palette buffer (alternative_palette_data in original BSS) */
extern u8 alternative_palette_data[136];

/* 136-byte destination palette buffer (palette_target_buffer in original BSS) */
extern u8 palette_target_buffer[136];

/*
 * color_table — 0x300-byte RGB palette entry table (~0x00401561).
 * Indexed by palette-index * 3; holds R/G/B triplets for 256 VGA palette entries.
 * Written by game_init phase 7 (MENUGEN.GIF palette memcpy) and read by
 * configure_game_palette / apply_palette_transformation.
 */
extern u8 color_table[0x300];

/*
 * word_509B40 — persona/inventory slot table (0x00509B40).
 * 98 persona entries × 8 i16 inventory-slot values = 0x310 words (0x620 bytes).
 * Populated by palette_postprocess_configure from PERSOS>INVENT.CTX / .TXT.
 */
extern i16 word_509B40[0x310];

/* 00491450 — copy alternative_palette_data → palette_target_buffer (34 dwords) */
void palette_copy_alternative(void);

/*
 * 00425D50 — palette_postprocess_configure (sub_425D50)
 *
 * Loads persona/inventory slot data into word_509B40 from a cached binary
 * (PERSOS>INVENT.CTX) or parses it from PERSOS>INVENT.TXT and writes the cache.
 * Called from game_init phase 8 (after MENUGEN.GIF palette wire-up).
 *
 * @param game_path_arg  Game root directory path (state->game_path).  May be
 *                       NULL; treated as "" (current directory).
 * @param is_using_hdd   Non-zero when game data is accessed from HDD (as
 *                       opposed to CD-ROM); controls which CTX file path is
 *                       built and whether a date-stamp check is performed.
 */
void palette_postprocess_configure(const c8 *game_path_arg, i32 is_using_hdd);
