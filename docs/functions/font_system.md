# Font System — ~405DAB / ~430BEA / ~405DC7 / ~405EDB / ~430D09

## Functions ported

| asm name | C name | addr (approx) | status |
|---|---|---|---|
| `init_font_system` | `init_font_system` | ~405DAB | partial |
| `init_font` | `init_font` | ~430BEA | partial |
| `set_position_coordinates` | `set_position_coordinates` | ~405DC7 | ported |
| `draw_text` | `draw_text` | ~405EDB | ported |
| — | `draw_text_at` | — | ported |
| `read_line_file` | `read_line_file` | ~430D09 | ported |

Source: `src/libsys/graphics/font.c/.h`

---

## init_font_system (~405DAB)

### Signature
```c
b8 init_font_system(void);
```

### asm summary

```asm
init_font_system proc near
    mov eax, font_file_handle   ; already loaded by sub_482560 / ls_set_video_mode
    mov font_handle, eax
    cmp font_handle, 0
    jnz loc_405DC2
    push offset aCannotOpenFntF ; "cannot open FNT file"
    push 4
    call log_message
    add esp, 8
loc_405DC2:
    pop edi / pop esi / pop ebx / leave / retn
init_font_system endp
```

### Port notes

In the original, `font_file_handle` is populated earlier (by `sub_482560` called
from `ls_set_video_mode`). In our port, `ls_set_video_mode` doesn't drive font
loading, so `init_font_system` allocates a `font_t` and calls `init_font("SYSFONT")`
directly. This is equivalent: the original `sub_482560` also calls `sub_482470` →
ultimately loads the SYSFONT font object.

Always returns `true` — original can only log a warning, never abort.

---

## init_font (~430BEA)

### Signature
```c
b8 init_font(font_t *out, const c8 *name);
```

### asm summary

```asm
init_font proc near
    mov [ebp+var_10], ecx       ; name arg
    push offset aSysfont        ; "SYSFONT"
    mov eax, [ebp+var_10]
    push eax
    call _strcmp
    jnz loc_430C09
    ; SYSFONT branch: copy font_file_handle → dword_508570, set dword_508AA4=1
    mov eax, font_file_handle
    mov dword_508570, eax
    mov dword_508AA4, 1
    jmp loc_430C25
loc_430C09:
    ; non-SYSFONT: call sub_482200(name, 8) to load <name>.FNT via ls_load_file
    mov edx, 8
    mov ecx, [ebp+var_10]
    call sub_482200             ; appends ".FNT", calls ls_load_file, parses header
    mov dword_508570, eax
    mov dword_508AA4, 0
loc_430C25:
    ; allocate bmap_t for glyph transfer: ls_init_bmap_ex(height*width, ...)
    ; then loop over all glyphs calling ls_bit_move to rearrange atlas rows
    ...
init_font endp
```

### Font file format (.CAR + .CIF)

`.CAR` — glyph atlas index (FONTE10.CAR, FONTE11.CAR in `datas/menus/`):
- 255 entries × 12 bytes = 3060 bytes + 4-byte trailing `band_height` dword.
- Each entry: `{ i32 atlas_x; i32 atlas_y; i32 glyph_width; }`
- `atlas_x/y` = pixel coordinates into the paired CIF atlas bitmap.
- `atlas_y` is a band offset (multiples of 32 pixels for FONTE11).
- Trailing dword: `0x1C` = 28 for FONTE11 (band height in pixels).
- Char encoding: entry [i] = character code i (0..254); printable ASCII starts at entry 32.

`.CIF` — compressed glyph atlas bitmap:
- Header: `[u32 version=1][u32 magic=0x32ef3356][u32 width=691][u32 height=224]`
- Body: proprietary compression (5352 bytes for a 691×224 = ~154KB raw atlas).
- Decode chain: `sub_482200` → `ls_load_file` → `sub_48B6F0` / `sub_48B660` (6×)
  → `sub_4822E0` → `sub_48BE30`.
- **Not yet ported** — these inner functions are not decoded.

### Port notes

- SYSFONT path: reads `.CAR` index successfully.
- Non-SYSFONT / CIF decode: **TODO** — `sub_482200`, `sub_4822E0`, `sub_48B660`,
  `sub_48BE30` require further asm analysis.
- `ls_bit_move` loop (the glyph rearrangement in `init_font`) copies each glyph
  from the atlas into a `bmap_t` — not ported; irrelevant until atlas is decoded.
- Fallback is installed whenever `atlas_pixels == NULL`.

---

## set_position_coordinates (~405DC7)

### Signature
```c
void set_position_coordinates(i32 x, i32 y);
```

### asm summary

```asm
set_position_coordinates proc near
    ; __fastcall: ECX=x, EDX=y (var_4 = x, var_8 = y after push)
    mov eax, [ebp+var_4]        ; x
    mov cursor_y_position, eax
    mov eax, cursor_y_position
    mov target_y_position, eax
    mov eax, [ebp+var_8]        ; y
    mov cursor_x_position, eax
    mov eax, cursor_x_position
    mov target_x_position, eax
    retn
```

### Port notes

Faithful port. Original `__fastcall` with ECX=x, EDX=y — note the register
assignment is counter-intuitive (ECX=first arg=x but stored via var_4 which
is labelled "y" in IDA because EDX=y was stored to var_8). The variable
assignments from asm confirm: `cursor_y_position = var_4 = ECX = first arg = x`?

Re-reading: `mov [ebp+var_8], edx` (EDX=second arg=y → var_8), `mov [ebp+var_4], ecx`
(ECX=first arg=x → var_4). Then `mov eax, [ebp+var_4]` → `cursor_y_position`.
So the original actually assigns **x to cursor_y** and **y to cursor_x** — or
the register convention is swapped. Port matches the asm literally.

---

## draw_text (~405EDB)

### Signature
```c
void draw_text(const c8 *text);
```

### asm summary

```asm
draw_text proc near
    mov [ebp+var_8], ecx        ; text ptr (__thiscall convention: ECX=this/arg)
    cmp font_handle, 0
    jnz loc_405EF8
    jmp loc_405FCC              ; skip if no font
loc_405EF8:
    cmp game_context, 0
    jnz loc_405F0A
    jmp loc_405FCC              ; skip if no game_context
loc_405F0A:
    cmp colour_mode_0_8bit_1_16bit, 0
    jnz loc_405F21
    call sub_47D330             ; 8-bit mode: lock palette
loc_405F21:
    mov [ebp+var_4], 0          ; i = 0
    ; loop:
    ;   call strlen(text), check i < len
    ;   call sub_4820B0(font_handle, game_context, char[i],
    ;                   target_y + i*8, target_x, 0)
    ;   i++
    ;   ...
    ; after loop:
    ;   call sub_47D310         ; 8-bit: unlock palette
    ;   call draw_image(game_context, graphics_context,
    ;                   target_y, target_x + 11,
    ;                   target_y + i*8, target_x,
    ;                   target_y, target_x)
loc_405FCC:
    retn
draw_text endp
```

### Port notes

- `game_context` and `graphics_context` globals are not yet available in the port.
  `draw_text_at` renders directly to `state->graphic.front_buffer` instead.
- `sub_4820B0` is the per-glyph blit from the font atlas into `game_context`.
  Not ported; replaced by `blit_fallback_glyph` / atlas blit stub.
- `draw_image` copies the `game_context` region into `graphics_context` (back-buffer
  to front). Not ported; front_buffer written directly.
- 8-bit palette lock/unlock (`sub_47D330`/`sub_47D310`): not applicable in 16-bit mode.
- The `i*8` stride in the loop (glyph column offset) suggests fixed-8-pixel x-advance
  for the SYSFONT at the current scale — matches the fallback 8×8 advance.

---

## read_line_file (~430D09)

### Signature
```c
i32 read_line_file(c8 *dst, FILE *fp);
```

### asm summary

```asm
read_line_file proc near
    ; reads bytes one-by-one into 0x100-byte stack buffer
    ; stops at '\n' (0x0A) or EOF
    ; copies buffer to dst via strcpy
    ; returns 1 on success, 0 on EOF
```

### Port notes

Faithful port. Buffer size 256 (0x100) matches original. Used by `read_and_init_sbt`
for subtitle line-by-line parsing.

---

## Parity

| function | build | launch | behavior | notes |
|---|---|---|---|---|
| `init_font_system` | yes | yes | partial | fires at startup, logs correctly |
| `init_font` | yes | yes | partial | .CAR read ok; CIF decode TODO |
| `set_position_coordinates` | yes | yes | ported | globals written correctly |
| `draw_text` | yes | yes | ported | fallback renders to front_buffer |
| `draw_text_at` | yes | yes | ported | fallback only; atlas path stubbed |
| `read_line_file` | yes | untested | ported | logic matches asm |
