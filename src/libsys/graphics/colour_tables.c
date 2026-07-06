#include <string.h>

#include "colour_tables.h"
#include "../common.h"

/*
 * Globals — BSS mirrors
 *
 *   primary_color_table   @ data segment (follows byte_4C0930 area)
 *   secondary_color_table @ immediately after primary_color_table
 *   color_transform_function @ set by initialize_color_mapping_tables
 */

u8 primary_color_table  [COLOR_TABLE_ROWS  * COLOR_TABLE_COLS] = {0};
u8 secondary_color_table[SEC_COLOR_TABLE_ROWS * COLOR_TABLE_COLS] = {0};

color_transform_fn color_transform_function = NULL;

/* ---------------------------------------------------------------------------
 * lookup_color_mapping  (00407C22)
 *
 * Used when colour_mode_0_8bit_1_16bit == 4 (555 RGB, no green-extra bit).
 *
 * Original asm (faithfully ported):
 *   edx = table_row << 5               (row index into 32-wide table)
 *   esi = primary_color_table + edx    (base of this row)
 *   r   = table[eax & 0x1F]            (red component,   bits 0..4)
 *   g   = table[(ecx>>5) & 0x1F]       (green component, bits 5..9)
 *   b   = table[(ecx>>10) & 0x1F]      (blue component,  bits 10..14)
 *   return (b << 10) | (g << 5) | r
 *
 * @param table_row    brightness/shade level (0..31)
 * @param packed_rgb16 555-encoded colour (R in bits 0-4, G 5-9, B 10-14)
 * --------------------------------------------------------------------------*/
// 00407C22  (DATA XREF: initialize_color_mapping_tables+9B)
u32 lookup_color_mapping(u32 table_row, u32 packed_rgb16) {
    u32 row_offset = (table_row & 0x1F) * COLOR_TABLE_COLS;
    const u8 *row = primary_color_table + row_offset;

    u32 rgb = packed_rgb16 & 0xFFFFu;
    u32 r = row[ rgb        & 0x1Fu];
    u32 g = row[(rgb >>  5) & 0x1Fu];
    u32 b = row[(rgb >> 10) & 0x1Fu];

    return (b << 10) | (g << 5) | r;
}

/* ---------------------------------------------------------------------------
 * transform_color_6bit  (00407C59)
 *
 * Used when colour_mode_0_8bit_1_16bit != 4 (565 RGB, green has 6 bits).
 *
 * Original asm (faithfully ported):
 *   row = primary_color_table + (table_row << 5)
 *   r   = row[rgb & 0x1F]
 *   g   = row[(rgb >> 5) & 0x3E >> 1]   i.e. bits 6..10, then >>1 → 5 bits
 *   b   = row[(rgb >> 11) & 0x1F]        (blue, bits 11-15)
 *   return (b << 11) | (g << 6) | r      (565 packing)
 *
 * Note: in the asm, `and eax, 3Eh; shr eax, 1` extracts bits [6..10] >> 1
 * which gives a 5-bit index into the same 32-entry row, preserving the
 * same LUT width.  The result is then packed with a 6-bit green shift.
 * --------------------------------------------------------------------------*/
// 00407C59  (DATA XREF: initialize_color_mapping_tables:loc_40B53D)
u32 transform_color_6bit(u32 table_row, u32 packed_rgb16) {
    u32 row_offset = (table_row & 0x1F) * COLOR_TABLE_COLS;
    const u8 *row = primary_color_table + row_offset;

    u32 rgb = packed_rgb16 & 0xFFFFu;
    u32 r   = row[ rgb         & 0x1Fu];
    /* green: bits [6..10] of rgb16 shifted right by 1 → 5-bit index */
    u32 g   = row[((rgb >> 5) & 0x3Eu) >> 1u];
    u32 b   = row[(rgb >> 11) & 0x1Fu];

    return (b << 11) | (g << 6) | r;
}

/* ---------------------------------------------------------------------------
 * initialize_color_mapping_tables  (00407D49)
 *
 * Called from ls_main+138 when colour_mode forces 16-bit.
 *
 * Algorithm (two nested loops, outer = brightness level ebx 0..31):
 *
 * Primary table — for each brightness row `b` (0..31):
 *   var4 = (b * 60) >> 8                  (= b * 60 / 256, range 0..7)
 *   step = b - var4                        (rounding slope)
 *   acc  = 0
 *   for i in 0..31:
 *       primary_color_table[b*32 + i] = (u8)(var4 + (acc >> 5))
 *       acc += step
 *   (the asm uses a Bresenham-style fixed-point accumulator)
 *
 * Secondary table — for each brightness row `b` (0..31):
 *   step = 31 - b
 *   acc  = 0
 *   for i in 0..32:   (33 entries — ecx starts at 0x21)
 *       secondary_color_table[b*32 + i] = (u8)(b + (acc >> 5))
 *       acc += step
 *
 * After both tables are filled, installs the appropriate colour-transform
 * function pointer.  We use state->colour_mode to replicate the original
 * colour_mode_0_8bit_1_16bit check:
 *   colour_mode_0_8bit_1_16bit == 4 → lookup_color_mapping  (555)
 *   otherwise                       → transform_color_6bit   (565)
 *
 * Our port forces 16-bit (colour_mode==1 in state_t → 565 mode),
 * so color_transform_function will be set to transform_color_6bit.
 * --------------------------------------------------------------------------*/
// 00407D49
void initialize_color_mapping_tables(void) {
    i32 var8 = 0;   /* accumulates b * 0x3C across outer iterations */

    for (i32 b = 0; b < COLOR_TABLE_ROWS; b++) {
        /* --- Primary table row b ----------------------------------------- */
        /*
         * eax = var8; cdq; and edx, 0xFF → edx = 0 (var8 always >= 0)
         * add eax, edx; sar eax, 8 → eax = var8 >> 8
         */
        i32 var4 = var8 >> 8;               /* (b * 60) >> 8 */
        i32 step = b - var4;                /* Bresenham step */
        i32 acc  = 0;

        u8 *prow = primary_color_table + b; /* base pointer at column b,
                                               stride 32 in the inner loop */
        for (i32 i = 0; i < COLOR_TABLE_COLS; i++) {
            /*
             * asm:
             *   eax = acc; cdq; and edx, 1Fh; add acc, step; add eax, edx
             *   sar eax, 5; dl = var4; dl += al
             *   store to [esi - 0x20]   (esi is pre-advanced by 0x20)
             *
             * The edx extraction (and edx, 1Fh after cdq) picks up the
             * low 5 bits of eax BEFORE the step is added.  When acc >= 0
             * this is just acc & 0x1F.  After sar eax, 5 we get (acc+edx)>>5.
             * For non-negative values this equals acc >> 5 (Bresenham rounding).
             */
            i32 edx = acc & 0x1F;
            i32 eax = (acc + edx) >> 5;
            *prow = (u8)(var4 + eax);
            prow += COLOR_TABLE_COLS;   /* advance stride */
            acc  += step;
        }

        /* --- Secondary table row b --------------------------------------- */
        /*
         * asm:
         *   ebp = 31 - b   (step)
         *   ecx = 0x21     (33 iterations)
         *   acc = 0
         *   for i 0..32:
         *       store (b + (acc>>5)) then acc += step
         */
        i32 step2 = 31 - b;
        i32 acc2  = 0;
        /* secondary_color_table is indexed [b + i*32] same as primary */
        u8 *srow = secondary_color_table + b;
        for (i32 i = 0; i < SEC_COLOR_TABLE_ROWS; i++) {
            i32 edx2 = acc2 & 0x1F;
            i32 eax2 = (acc2 + edx2) >> 5;
            *srow = (u8)(b + eax2);
            srow += COLOR_TABLE_COLS;
            acc2 += step2;
        }

        var8 += 0x3C; /* 60 */
    }

    /*
     * Select transform function.
     * Original check: colour_mode_0_8bit_1_16bit == 4 → 555 mode.
     * In our port state->colour_mode is 1 for 16-bit (565), so we always
     * install transform_color_6bit here.  If a caller explicitly needs 555
     * mode they can override color_transform_function after this call.
     *
     * Mirrors:
     *   colour_mode == 4 → lookup_color_mapping  (555)
     *   else             → transform_color_6bit   (565)
     */
    /* Our port forces 565 (colour_mode != 4) */
    color_transform_function = transform_color_6bit;

    print("initialize_color_mapping_tables: tables built, transform=565\n");
}
