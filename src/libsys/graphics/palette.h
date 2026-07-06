#pragma once

#include "../common.h"

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

/* 00491450 — copy alternative_palette_data → palette_target_buffer (34 dwords) */
void palette_copy_alternative(void);
