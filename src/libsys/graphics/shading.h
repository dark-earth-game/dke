#pragma once

#include "../common.h"

/* 4096-entry cosine LUT, each entry scaled to fixed-point Q14 (max = 16384).
   Index i maps to cos(i * 2*PI/4096) * 16384, stored as i16.
   IDA names this buffer "sine_table"; it is a full-period cosine table.
   Callers can derive sine via a quarter-period phase shift:
       sin(i) = cosine_table[(i + 1024) & 0xFFF] */
#define TRIG_TABLE_SIZE 4096
extern i16 cosine_table[TRIG_TABLE_SIZE]; /* addr: mirrors sine_table @ ~4C5xxx */

void preload_shading_table(void);
