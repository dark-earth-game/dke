# CRH (SEB) Movie Decoder

Canonical spec for the CRH format and the C port in `src/libsys/movie_decoder.c`.
Python reference: `tools/src/CRH_to_MP4.py`.

---

## File Format

### Header (54 bytes total)

| Offset | Size | Type  | Field           | Notes                                      |
|--------|------|-------|-----------------|--------------------------------------------|
| 0      | 2    | u16   | version         | LE                                         |
| 2      | 48   | u8[]  | audio_header    | see sub-table below                        |
| 50     | 2    | u16   | frame_count     | LE                                         |
| 52     | 2    | u16   | fps             | LE; treated as 15 if 0                     |

**Audio header sub-fields (relative to offset 2):**

| Sub-offset | Size | Field       |
|------------|------|-------------|
| 0          | 4    | sample_rate (u32 LE) |
| 14         | 2    | channels (u16 LE)    |
| rest       | —    | ignored              |

`payload_offset = 54` (first frame starts here).

---

### Per-frame layout

```
[A_BYTES = 2942]  audio block       — u16 seed + 7-bit delta PCM; skipped in port
[16]              video header
    u32 full      (total, unused in decode)
    u32 c0        component 0 compressed size
    u32 c1        component 1 compressed size
    u32 c2        component 2 compressed size
[c0]              plane 0 compressed bytes
[c1]              plane 1 compressed bytes
[c2]              plane 2 compressed bytes
[4]               tile_data_size (u32 LE)
[tile_data_size]  tile colour/mode table
```

---

## Component Compression (`decode_component` / Python `dec_comp_framepred`)

LSB bitstream (read bit 0 first within each byte).

| Bit(s)        | Meaning                                                              |
|---------------|----------------------------------------------------------------------|
| 1 bit: flag   | 0 = RLE fill, 1 = back-reference                                     |
| **flag=0**    | 5 bits val, 8 bits len → fill `len` bytes with `val`                |
| **flag=1**    | 1 bit sid (0=intra/dst, 1=inter/prev\_frame), 16 bits off, 8 bits len → copy `len` bytes from `src[off..off+len-1]` |

- `OFFSET_BITS = 16` (constant).
- `src` for sid=0 is the current (partially filled) destination plane — forward references allowed.
- `src` for sid=1 is the previous frame's plane (zeroed for frame 0).
- EOF mid-stream: break out of loop, leave remaining dst as-is.

Output: raw plane bytes, `width * height` bytes (indices, not colour values).

---

## Tile Data / RGB Reconstruction (`reconstruct_rgb`)

### Tile bitstream modes

Two modes detected at open time (`detect_dims`):

| Mode        | Constant              | Bitreader | First byte order |
|-------------|-----------------------|-----------|------------------|
| `field-rev` | `CRH_TILE_FIELD_REV=0`| BRlsb     | byte reversed: `tw = rbits(read8, 8)` |
| `byte-rev`  | `CRH_TILE_BYTE_REV=1` | BRmsb     | byte-reversed buffer: `tw = read8` |

`field-rev` is the preferred mode (tried first). Falls back to `byte-rev` if dim detection fails.

### Tile header (16 bits total)

| Bits | Field | Notes                                       |
|------|-------|---------------------------------------------|
| 8    | tw    | tile width; 0 → use 20                      |
| 8    | th    | tile height; 0 → use 20                     |

`tiles_x = max(1, w // tw)`, `tiles_y = max(1, h // th)`.

### Per-tile fields (18 bits per tile)

| Bits | field-rev read                      | byte-rev read         | Meaning             |
|------|-------------------------------------|-----------------------|---------------------|
| 2    | `rbits(read2, 2)`                   | `read2`               | mode m (0..3)       |
| 4    | `rbits(read4, 4) * 4`               | `read4 * 4`           | step1 s1            |
| 4    | `rbits(read4, 4) * 4`               | `read4 * 4`           | step2 s2            |
| 4    | `rbits(read4, 4) * 2`               | `read4 * 2`           | offset1 o1          |
| 4    | `rbits(read4, 4) * 2`               | `read4 * 2`           | offset2 o2          |

Total per tile: 2 + 4 + 4 + 4 + 4 = 18 bits. Used to detect nt (number of tiles) in `detect_dims`.

### Sampling (DEINT = even-odd)

For pixel at (x, y):
```
idx = 2 * ((y / 2) * w + x) + (y & 1)
a = c0[idx] & 31    (5-bit)
b = c1[idx] & 31
c = c2[idx] & 31
```

### Lookup tables

```
tab(step, off)[i] = i * step / 32 + off - 16   for i in [0..31]
T1[v] = v * s1 / 32 + o1 - 16
T2[v] = v * s2 / 32 + o2 - 16
```

### Color reconstruction by mode m

| m | R                 | G                 | B                  |
|---|-------------------|-------------------|--------------------|
| 0 | `clip5(a + T2[c])`| `clip5(b + T1[c])`| `c`                |
| 1 | `clip5(b + T1[c])`| `c`               | `clip5(a + T2[c])` |
| 2 | `c`               | `clip5(a + T2[c])`| `clip5(b + T1[c])` |
| 3 | `a`               | `b`               | `c`                |

`clip5(x) = clamp(x, 0, 31)`.

### Pixel expand

`five8(v) = (v << 3) | (v >> 2)` — expands 5-bit to 8-bit.

Output: `rgb_out[p..p+2] = { five8(R), five8(G), five8(B) }`.

---

## Dimension Detection (`detect_dims`)

1. Open file, seek to `payload_offset`.
2. Skip `A_BYTES` (audio), read 16-byte video header → get c0, c1, c2.
3. Seek to tile data: `pos + c0 + c1 + c2`, read u32 `tsize`, read `tsize` bytes.
4. For each mode in `{field-rev, byte-rev}`:
   - Read tw, th from tile data header.
   - `rem_bits = tsize * 8 - 16`.
   - If `rem_bits <= 0` or `rem_bits % 18 != 0`: skip.
   - `nt = rem_bits / 18`.
   - Enumerate factor pairs `(tx, ty)` of `nt` where `tx * ty == nt`.
   - `w = tw * tx`, `h = th * ty`. Accept if `32 <= w <= 4096`, `32 <= h <= 2160`.
   - Score: `(|w/h - 4/3|, -w*h)` — prefer 4:3, then largest area.
5. Return best `(w, h, mode)`. Fall back to `(320, 200, field-rev)` on failure.

---

## Audio (deferred)

Each frame's `A_BYTES = 2942` bytes: `u16` seed (per channel) + 7-bit delta encoded PCM.
- `sign7(v) = v - 128 if (v & 0x40) else v` — converts 7-bit signed.
- Step: `FINE_STEP = 16`, `COARSE_STEP = 512`. LSB=0 → coarse, LSB=1 → fine.
- Port: currently `fseek(fp, A_BYTES, SEEK_CUR)` per frame.
- TODO: implement `decode_audio_block` from Python ref, stream via CoreAudio/SDL.

---

## Port Mapping (Python → C)

| Python function       | C equivalent                         | Location                          |
|-----------------------|--------------------------------------|-----------------------------------|
| `parse_header`        | `crh_open` (header part)             | `movie_decoder.c`                 |
| `detect_dims`         | `detect_dims` (static)               | `movie_decoder.c`                 |
| `dec_comp_framepred`  | `decode_component` (static)          | `movie_decoder.c`                 |
| `reconstruct_rgb`     | `reconstruct_rgb` (static)           | `movie_decoder.c`                 |
| `BRlsb`               | `br_lsb_t` + `brlsb_*` inlines      | `movie_decoder.c`                 |
| `BRmsb`               | `br_msb_t` + `brmsb_*` inlines      | `movie_decoder.c`                 |
| `five8`, `clip5`, `rbits` | static inline functions          | `movie_decoder.c`                 |
| `convert` loop        | `ls_play_movie`                      | `movie.c`                         |
| `decode_audio_block`  | (not ported)                         | TODO                              |

---

## Constants

| Name           | Value | Source          |
|----------------|-------|-----------------|
| `CRH_A_BYTES`  | 2942  | Python `A_BYTES` |
| `CRH_OFFSET_BITS` | 16 | Python `OFFSET_BITS` |
| `CRH_TILE_FIELD_REV` | 0 | Python `TILE_BITS_DEF = "field-rev"` |
| `CRH_TILE_BYTE_REV`  | 1 | Python `"byte-rev"` |

DEINT is always `even-odd` (only sampler implemented).

---

## Parity

- **build**: clean `-Wall` with `-std=c99`.
- **launch**: game starts without crash even without a movie call.
- **behavior**: `crh_open` correctly parses SC08.CRH header (ver, dims, fps) — confirmed by printf output.
- **untested**: actual pixel output vs Python MP4 reference (requires visual comparison tool).
