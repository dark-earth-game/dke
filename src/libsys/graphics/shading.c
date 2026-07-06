#include <math.h>

#include "../common.h"
#include "shading.h"

/* Global cosine LUT — mirrors "sine_table" from the original binary.
   4096 entries, each i16, scaled to Q14 fixed-point (max 16384).
   Original address range: sine_table[0..0xFFF] in .data segment. */
i16 cosine_table[TRIG_TABLE_SIZE];

// 0048F190
/* Initialise the 4096-entry cosine lookup table.
   Original name: sub_48F190 (IDA), preload_shading_table (project label).
   Called from graphics_init.

   ASM logic (verbatim mapping):
     si = 0
     loop (si < 0x1000):
       edi = (i32)si
       fild  edi                    ; push int as float
       fmul  0.001533980787885641   ; * (2*PI / 4096)
       fcos                         ; cos(edi * 2PI/4096)
       fmul  16384.0                ; scale to Q14
       __ftol -> ax                 ; truncate to i32, take low 16 bits
       sine_table[edi*2] = ax       ; store i16
       si++
   end loop */
void preload_shading_table(void) {
    /* dbl_4B0C00 = 2*PI/4096 = 0.001533980787885641
       dbl_4B0C08 = 16384.0 */
    static const f64 angle_step  = 0.001533980787885641; /* dbl_4B0C00 */
    static const f64 scale       = 16384.0;              /* dbl_4B0C08 */

    i32 i;
    for (i = 0; i < TRIG_TABLE_SIZE; i++) {
        /* fild -> fmul dbl_4B0C00 -> fcos -> fmul dbl_4B0C08 -> __ftol -> ax */
        cosine_table[i] = (i16)(cos((f64)i * angle_step) * scale);
    }
}
