#include <string.h>

#include "palette.h"
#include "../common.h"

/* ---------------------------------------------------------------------------
 * Palette globals
 *
 * Both buffers are 34 dwords = 136 bytes, matching the rep movsd ecx=0x22
 * observed in sub_491450 and sub_4953B0.
 * --------------------------------------------------------------------------*/

/* Source buffer — original asm: "alternative_palette_data" */
u8 alternative_palette_data[136] = {0};

/* Destination buffer — original asm: "palette_target_buffer" */
u8 palette_target_buffer[136] = {0};

/* ---------------------------------------------------------------------------
 * sub_491450  (0x00491450)
 *
 *   push  esi
 *   mov   ecx, 22h          ; 34 dwords = 136 bytes
 *   push  edi
 *   mov   esi, offset alternative_palette_data
 *   mov   edi, offset palette_target_buffer
 *   rep   movsd
 *   pop   edi
 *   pop   esi
 *   retn
 *
 * Copies the alternative palette data into the active palette target buffer.
 * --------------------------------------------------------------------------*/
// 00491450
void palette_copy_alternative(void) {
    memcpy(palette_target_buffer, alternative_palette_data, sizeof(palette_target_buffer));
    print("palette_copy_alternative: copied %u bytes to palette_target_buffer\n",
          (u32)sizeof(palette_target_buffer));
}
