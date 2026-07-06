# preload_shading_table — 0048F190

## Signature
```c
void preload_shading_table(void);
```

## asm summary

`sub_48F190` (IDA), called from `graphics_init+39` and `graphics_init+58`.

```asm
sub_48F190 proc near
    sub esp, 4          ; local dword slot (var_4)
    push esi
    push edi
    xor si, si          ; i = 0

loc_48F198:             ; loop while si < 0x1000
    movsx edi, si
    inc si
    mov [esp+var_4], edi
    fild [esp+var_4]    ; (float)i
    fmul dbl_4B0C00     ; * 0.001533980787885641  (= 2*PI/4096)
    fcos                ; cos(i * 2PI/4096)
    fmul dbl_4B0C08     ; * 16384.0
    call __ftol         ; truncate to i32 (result in eax, low 16 in ax)
    mov sine_table[edi*2], ax   ; store i16
    cmp si, 1000h
    jl short loc_48F198
    ...
    retn
sub_48F190 endp
```

Data constants (`.rdata`):
- `dbl_4B0C00 dq 0.001533980787885641` — angle increment = `2*PI / 4096`
- `dbl_4B0C08 dq 16384.0` — Q14 scale factor

Output: `sine_table` — global `i16[4096]` in `.data` segment.

## Port notes

- The project label `preload_shading_table` is a misnomer. The function builds a
  **full-period cosine lookup table** (4096 entries, Q14 fixed-point, range ±16384).
  IDA's label `sine_table` is accurate in the sense that cos(i) = sin(i + π/2),
  so the same table serves as a sine table with a 1024-entry phase offset.
- The C global is named `cosine_table` (in `shading.h/c`) to be explicit; the
  `extern` symbol matches IDA's `sine_table` address conceptually.
- `__ftol` truncates toward zero, matching C's `(i16)` cast from `f64`.
- `fild` sign-extends the 32-bit integer to float; `(f64)i` in C is identical.
- Loop runs 4096 times: `si` starts at 0, incremented before the branch, so
  iterations cover `edi = 0..4095` (i.e. `si` goes 1..0x1000, branch exits when
  `si == 0x1000`). Equivalent C: `for (i = 0; i < 4096; i++)`.

## Parity
- build: yes — compiles cleanly with `-Wall`
- launch: untested (no runtime verification of LUT values yet)
- behavior: LUT math verified by inspection: `cos(0 * 2PI/4096) * 16384 = 16384`,
  `cos(1024 * 2PI/4096) * 16384 = cos(PI/2) * 16384 ≈ 0` — matches expectations
  for a standard trig table. Exact bit-identity with original `__ftol` truncation
  vs C cast not yet checked.
