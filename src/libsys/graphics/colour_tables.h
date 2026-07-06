#pragma once

#include "../common.h"

/*
 * initialize_color_mapping_tables  (00407D49 — see ls_main+138)
 *
 * Fills two lookup tables used by the 16-bit hicolor blitter:
 *
 *   primary_color_table   [32][32]  u8  — shading LUT indexed by
 *                                         [brightness_level][colour_component]
 *   secondary_color_table [32][33]  u8  — complementary shading LUT
 *
 * After building the tables the function stores a pointer to either
 * lookup_color_mapping or transform_color_6bit in color_transform_function,
 * selected by colour_mode_0_8bit_1_16bit:
 *   == 4 → lookup_color_mapping   (5-5-5 RGB mapping)
 *   != 4 → transform_color_6bit   (5-6-5 RGB mapping)
 *
 * Exposed globals allow the blitter (graphic.c) to call
 * color_transform_function(table_row, packed_rgb16).
 */

/* 32 rows × 32 columns */
#define COLOR_TABLE_ROWS    32
#define COLOR_TABLE_COLS    32
/* secondary table has 33 rows × 32 columns */
#define SEC_COLOR_TABLE_ROWS 33

extern u8 primary_color_table  [COLOR_TABLE_ROWS * COLOR_TABLE_COLS];
extern u8 secondary_color_table[SEC_COLOR_TABLE_ROWS * COLOR_TABLE_COLS];

/*
 * Function-pointer type: (table_row_index, packed_rgb16) → mapped_rgb16.
 * Matches calling convention of lookup_color_mapping / transform_color_6bit.
 */
typedef u32 (*color_transform_fn)(u32 table_row, u32 packed_rgb16);
extern color_transform_fn color_transform_function;

/* 00407D49 — build primary_color_table and secondary_color_table */
void initialize_color_mapping_tables(void);

/*
 * lookup_color_mapping     — colour_mode == 4 (5-5-5 RGB)
 * transform_color_6bit     — colour_mode != 4 (5-6-5 RGB)
 */
u32 lookup_color_mapping(u32 table_row, u32 packed_rgb16);
u32 transform_color_6bit(u32 table_row, u32 packed_rgb16);
