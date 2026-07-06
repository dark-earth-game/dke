# CIF Decoder

Port of `tools/src/dump_cif.py` → `src/libsys/graphics/cif.c/.h`.

## File format spec

```
Offset  Size  Field
0x00     4    version    u32 LE (always 1 in practice)
0x04     4    signature  4 raw bytes (not validated by decoder)
0x08     4    width      u32 LE — pixel columns
0x0C     4    height     u32 LE — pixel rows
0x10   768    palette    256 × RGB triplets (8 bits per channel, not shifted)
0x310   var   compressed LZW pixel data
```

## LZW variant

| Parameter        | Value                        |
|------------------|------------------------------|
| Initial code size| 9 bits                       |
| Clear code       | 256                          |
| End code         | 257                          |
| First dict code  | 258                          |
| Max code size    | 12 bits                      |
| Max dict entries | 4096                         |
| Bit packing      | LSB-first (same as GIF)      |
| First code       | Must be clear code (256)     |
| Code growth      | When next_code > max_code    |
| Multiple clears  | Supported (dict reset)       |

The decoder outputs 8-bit palette indices (one byte per pixel, row-major top-left origin).

## Port mapping

| Python (`dump_cif.py`)           | C (`cif.c`)                  |
|----------------------------------|------------------------------|
| `decompress_lzw()`               | `decompress_lzw()` (static)  |
| `dict = {i: bytes([i]) ...}`     | `lzw_entry_t dict[]` chain   |
| `read_bits()` closure            | `bitreader_t` + `bitreader_read()` |
| `output.extend(entry)`           | `EMIT_CHAIN()` macro         |
| `dictionary[old_code]+bytes([first])`| `dict[next_code].prefix/suffix` |
| `parse_cif_file()` header read   | `cif_load()` header section  |
| `np.frombuffer(palette_data)`    | `fread(palette, 1, 768, fp)` |
| `f.read()` compressed data       | `fread(comp, 1, comp_size)`  |
| partial decode: `+ bytes([0]*…)` | `memset(pixels, 0, …)` pre-fill |

Key difference: Python uses a heap `dict` mapping `code → bytes` (copies the full string per entry). The C port uses a compact prefix-chain table (`dict[code].prefix = parent, .suffix = last_byte`) and walks the chain backwards into a reversal buffer to emit bytes in correct order. This avoids any per-step heap allocation.

## Palette note

`dump_cif.py` has a commented-out `palette >> 2` shift for the font prefix `F` files. Both Python and the C port leave the palette values unshifted (raw 0–255 values). The `adjust_palette` path in the Python tool is a viewer-only display adjustment and is not part of the decoding spec.

## Functions

| C name        | file            | status   | notes                                    |
|---------------|-----------------|----------|------------------------------------------|
| `cif_load`    | graphics/cif.c  | ported   | open + read header/palette/compressed, LZW decode |
| `cif_free`    | graphics/cif.c  | ported   | frees pixels + palette buffers           |
| `cif_to_xrgb` | graphics/cif.c  | ported   | expands 8-bit indexed to 32-bit XRGB     |

## Verified samples

| File                         | Expected       | Actual         |
|------------------------------|----------------|----------------|
| biganims/DIAB_0.CIF          | 350×112, 8bpp  | 350×112, 8bpp  |
| DATAS/MENUS/FONTE11.CIF      | 691×224, 8bpp  | 691×224, 8bpp  |

Verified via `make dke` + 3-second smoke run. Log output:

```
cif_load: …/FONTE11.CIF -> 691x224 bpp=8
init_font: CIF atlas loaded 691x224, atlas-based rendering enabled
init_font_system: font system ready (fallback=0)
cif_load: …/DIAB_0.CIF -> 350x112 bpp=8
CIF smoke: 350x112 bpp=8
```

## Font atlas wire-up

`init_font()` (`font.c`) now:
1. Reads `FONTE11.CAR` glyph index (255 entries of `{atlas_x, atlas_y, glyph_width}`).
2. Calls `cif_load()` on `FONTE11.CIF`.
3. On success: stores `atlas_pixels`, `atlas_width`, `atlas_height` in `font_t`; sets `use_fallback = false`.
4. On failure: falls back to embedded CP437 8×8 bitmap (unchanged behaviour).

`draw_text_at()` atlas blitting (inner pixel loop) remains a TODO — the `glyph_entries + atlas_pixels` branch is now reachable but the blit itself is a separate task.

## Parity

- **build**: zero warnings with `-Wall`
- **launch**: exit 143 (SIGTERM after 3 s) — no crash
- **behavior**: FONTE11.CIF 691×224 matches Python decoder; DIAB_0.CIF 350×112 matches Python decoder
