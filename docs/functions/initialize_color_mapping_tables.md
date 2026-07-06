# initialize_color_mapping_tables

| field | value |
|---|---|
| original address | 00407D49 |
| asm name | `initialize_color_mapping_tables` |
| C name | `initialize_color_mapping_tables` |
| file | `src/libsys/graphics/colour_tables.c` |
| status | `ported` |
| called from | `ls_main+138` (16-bit colour path) → our port: `init_all_systems` |
| parity | build ✓, launch ✓, behavior: untested (no blitter exercised yet) |

## Purpose

Builds two lookup tables used by the 16-bit hicolor blitter to apply per-channel
brightness/shading transformations without per-pixel arithmetic:

- `primary_color_table[32][32]` — for each brightness level `b` (row), maps
  a raw 5-bit colour component `i` to a shaded value.
- `secondary_color_table[33][32]` — complementary table; maps `i` to the
  remaining complement at brightness level `b`.

After building the tables the function installs one of two colour-transform
function pointers into `color_transform_function`:

| original check | function | RGB packing |
|---|---|---|
| `colour_mode_0_8bit_1_16bit == 4` | `lookup_color_mapping` | 5-5-5 (555) |
| anything else | `transform_color_6bit` | 5-6-5 (565) |

Our port always installs `transform_color_6bit` because `state->colour_mode`
forces 16-bit 565 mode.

## Algorithm

### Outer loop (`b` = 0..31)

```
var8 starts at 0, increments by 60 each iteration (b * 60 at iteration b).
var4 = var8 >> 8                     -- scaled brightness floor
step = b - var4                      -- Bresenham slope
```

### Primary inner loop (`i` = 0..31)

Bresenham fixed-point accumulator in 5-bit fractional units:

```c
acc = 0;
for i in 0..31:
    edx = acc & 0x1F;
    primary_color_table[b*32 + i] = (u8)(var4 + ((acc + edx) >> 5));
    acc += step;
```

Effective: `primary[b][i] ≈ var4 + round(i * step / 32)`.

### Secondary inner loop (`i` = 0..32)  — 33 entries

```c
step2 = 31 - b;
acc2  = 0;
for i in 0..32:
    secondary_color_table[b*32 + i] = (u8)(b + ((acc2 + (acc2 & 0x1F)) >> 5));
    acc2 += step2;
```

## Transform functions

### `lookup_color_mapping` (555 mode)

```
row = primary_color_table + (table_row << 5)
r = row[rgb & 0x1F]
g = row[(rgb >> 5) & 0x1F]
b = row[(rgb >> 10) & 0x1F]
return (b << 10) | (g << 5) | r
```

### `transform_color_6bit` (565 mode)

```
row = primary_color_table + (table_row << 5)
r = row[rgb & 0x1F]
g = row[((rgb >> 5) & 0x3E) >> 1]   -- 5-bit index from 6-bit green field
b = row[(rgb >> 11) & 0x1F]
return (b << 11) | (g << 6) | r
```

## Globals

| C name | original addr | type | notes |
|---|---|---|---|
| `primary_color_table` | BSS after byte_4C0930 | `u8[32*32]` | 1024 bytes |
| `secondary_color_table` | immediately after primary | `u8[33*32]` | 1056 bytes |
| `color_transform_function` | set by this function | `color_transform_fn` | ptr to 555 or 565 transform |

## TODOs

- Verify table values against a live Win32 run or reference dump.
- Wire `color_transform_function` into the blitter when `apply_palette_transformation` is ported.
