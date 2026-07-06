# draw_image / ls_bit_move / ls_box / blit_bmap_to_framebuffer — Slice A blit pipeline

## Addresses

| function | addr |
|---|---|
| `draw_image` | 0x00453FA3 |
| `ls_bit_move` | 0x0047DC3C |
| `ls_box` | 0x0047D466 area |
| `ls_bit_move_256` | 0x0047D671 (inner pixel loop, inlined) |
| `blit_bmap_to_framebuffer` | new — no asm addr |

Source: `src/libsys/graphics/graphic.c`

---

## draw_image

### Signature
```c
void draw_image(bmap_t *dst, bmap_t *src,
                i32 sx, i32 sy, i32 sw, i32 sh,
                i32 dx, i32 dy);
```

### asm summary
`__fastcall` ECX=dst, EDX=src; stack args: sx(arg_0), sy(arg_4), sw(arg_8),
sh(arg_C), dx(arg_10), dy(arg_14).

Flow:
1. `test global_state_flags, 2` — if debug mode and dst == debug_surface,
   compute outline rect dimensions and call `ls_box(dst, dx, dy, w, h, 0xFF, 0)`.
2. `cmp dword_4B69BC, 0` — if non-zero and dst == graphics_context, skip blit
   (used to suppress redundant blits to primary surface).
3. Push all 8 args + mode=0 and call `ls_bit_move`.

### Port notes
- `global_state_flags` and `debug_surface` identity check not yet wired (those
  globals live in game_init and are not yet exposed externally).  The ls_box
  call is therefore skipped in this port; add when `debug_surface` is published.
- `dword_4B69BC` guard likewise skipped (no direct surface handle available).
- Mode is always 0 (opaque) from draw_image.

---

## ls_bit_move

### Signature
```c
void ls_bit_move(bmap_t *dst, bmap_t *src,
                 i32 sx, i32 sy, i32 sw, i32 sh,
                 i32 dx, i32 dy, i32 mode);
```

### asm summary
`__fastcall` ECX=src, EDX=dst; stack: sx=arg_0, sy=arg_4, sw=arg_8, sh=arg_C,
dx=arg_10, dy=arg_14, mode=arg_18.

Clip path (register arithmetic):
- Sort [sx..sw] and [sy..sh] so esi<=edi, ebx<=ebp (XOR-swap pattern).
- Multiple early-exit checks: dx/dy outside dst bounds, sx/sy outside src bounds.
- Adjust sx/dx if either < 0, adjust sw/sh against src width/height.
- Clamp final copy rect against dst extents.

Inner dispatch (after clipping):
- `mode == 2` → call `lsi_bit_move_alpha` (not ported, TODO).
- `byte_4D7F30 != 0` → call `ls_set_rectangle_2` (surface path, TODO).
- `byte_4D7F34 != 0` → call `ls_set_rectangle_3` (surface path, TODO).
- `pixel_format` mismatch → call `lsi_remap_bmap` (colour-space convert, TODO).
- Same format → call `ls_lock_bmap` (if bpp_flags > 8), then `ls_set_rectangle`
  to build rect structs, then `lsys_bit_move_256` (opaque) or fallback with mode.

`lsys_bit_move_256` inner loop:
- mode==0: `rep movsd` + `rep movsb` row-by-row memcpy.
- mode!=0: byte loop skipping zero pixels (colour-key).

### Port notes
- `ls_lock_bmap` / `ls_unlock_bmap` (DirectDraw surface lock) skipped — raw buffer path only.
- `ls_set_rectangle` helper builds a rect struct; inlined as direct pointer arithmetic.
- `lsys_bit_move_256` inlined as `memcpy` (mode 0) or byte-loop (mode 1).
- Cross-format `lsi_remap_bmap` path logs a TODO and returns early.
- Byte-per-pixel computed from `src->bit_depth / 8`.

---

## ls_box

### Signature
```c
void ls_box(bmap_t *dst, i32 x, i32 y, i32 w, i32 h, u8 color, i32 flags);
```

### asm summary
`__fastcall` ECX=dst, EDX=dst (duplicate); stack: x=arg_0, y=arg_4, w=arg_8,
h=arg_C, color=arg_10(low byte), flags=arg_10(bits).

Original branches on `arg_10` bits:
- bit 0 + bit 1 + bit 2: `ls_invert_rect` calls (3-pixel inset double outline).
- bit 0 + bit 1 (no bit 2): `ls_fill_rect` calls (solid filled border).
- bit 0 only + bit 2: single `ls_invert_rect`.
- bit 0 only (no bit 2): single `ls_fill_rect`.
- else: `loc_47D557` (no-op or further path not decoded).

### Port notes
Status: **stub**.  A simple 1-pixel outline is drawn using direct pixel writes
into `dst->pixels`.  `ls_fill_rect` and `ls_invert_rect` depend on further
unported DirectDraw rect-fill primitives.  The stub is sufficient for debug use
(visually shows source rect boundaries).

TODO: port `ls_fill_rect` / `ls_invert_rect` if accurate debug-outline rendering
is required.

---

## blit_bmap_to_framebuffer

### Signature
```c
void blit_bmap_to_framebuffer(u32 *fb, i32 fb_w, i32 fb_h,
                               const bmap_t *src, const u8 *palette,
                               i32 dx, i32 dy);
```

### Purpose
No direct asm counterpart.  New function for Slice A that bridges the 8bpp
indexed `bmap_t` world with the 32-bit XRGB `graphic_t.front_buffer` used by
`ls_blit`.

Palette lookup: `palette[idx*3+0..2]` → `0xFF000000 | (R<<16) | (G<<8) | B`.
Clips source to framebuffer bounds.

### Wire-up
Called from `ls_main` main loop after intro CRH completes.  `game_init_get_menu_backdrop`
returns `g_loading_bmap` (512×64 LOADING.GIF) and `color_table` (MENUGEN.GIF
palette).  Backdrop is centred in the 640×480 framebuffer.

---

## Parity

- **build**: yes — `make dke` exits 0, no new warnings.
- **launch**: yes — exit 143 on SIGTERM, no crash.
- **behavior**: confirmed via `/tmp/dke2.log`:
  - `game_init_get_menu_backdrop: bmap=0x... palette=0x...`
  - `ls_main: menu backdrop ready 512x64, will blit each frame`
  - `blit_bmap_to_framebuffer: 512x64 src -> fb offset (64,208) copy 512x64`
  (fires every main-loop iteration after intro finishes)
- `draw_image` / `ls_bit_move` invocation log: would fire when called from
  menu/entity render paths (not yet wired); confirmed compileable and linked.
- **untested**: `ls_box` stub not yet triggered by current code paths.
