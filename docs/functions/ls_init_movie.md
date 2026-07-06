# ls_init_movie — 00431EA0

## Signature

```c
void ls_init_movie(i32 a, i32 b);
```

`a` — original arg (edx). Currently unused in port; asm only checks it via the tables. `b` — audio flip callback selector (0 → `seb_traite_zik_flip`, non-zero → `seb_traite_zik`).

## asm summary

1. `init_my_memory` — zero seb allocator counters.
2. Zero flags: `_is_ci8 = 0`, `_ls_extern_sbt = 0`.
3. Loop 32× fills `byte_508B00[i*3]` with triples `(0, 2*i, 0)` — green ramp.
4. Loop 14×14 fills `byte_508B60[(14i+j)*3]` with `(i*63/13, 0, j*63/13)` — red/blue gradient.
5. If `graphics_context != 0`, copy fields at +4/+8/+1C into globals; select 1 vs 2 based on `dword_55C228`.
6. Pick `dword_508E10` = one of two `seb_traite_zik*` audio flip callbacks based on `b`.
7. `seb_get_memory(0, 0xD6D8)` → `dword_5089BC` (scratch).
8. 20× `seb_get_memory(0, 0x9C40)` → `dword_508A50[i]` (audio/decoder slices).
9. `seb_get_memory(1, 0x15CC)` → `_zik_load` (64K-aligned).
10. `ls_get_heap(0x9EB10)` → `dword_508564`.
11. `cp_init()` — colour-pack codec init (not yet ported).
12. `GetProfileString("LIBSYS", "COMPR", "FALSE")` → `dword_4B4628` compression flag.

## Port notes

- `seb_get_memory` alignment uses full-width mask on 64-bit hosts (`~(uintptr_t)0xFFFFu`). Initial port used `0xFFFF0000u` (32-bit constant) which truncated pointers and crashed — fixed.
- Globals in the asm (`dword_5089BC`, `dword_508A50[]`, `_zik_load`, `dword_508564`) are kept as file-local `static void *` in `movie.c`. Promote to header exports only when a follow-up port needs them.
- `cp_init` / `cp_uninit` / `ls_get_heap` / `ls_release_heap` are TODO stubs; movie heap slot is a `malloc(0x9EB10)` for now.
- Movie-side `COMPR` win.ini flag is defaulted to 0 (FALSE); asm reads it via `GetProfileStringA`. Wire to `state->win_ini` cache in a later pass if any decoder path branches on it.

## Parity

- build: yes (`make dke`, exit 0, no new warnings)
- launch: yes (SIGTERM exit 143 after 2s idle, no crash)
- behavior: allocations happen in the same order and sizes as asm. Not runtime-verified against a movie file until decoder is ported.
