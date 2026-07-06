# Conventions

## Naming

- Original engine prefix `ls_*` / `lsys_*` = LIBSYS runtime. Keep prefix.
- Game-specific functions: no prefix (`load_configuration`, `preload_shading_table`).
- Struct types: lowercase `_t` suffix (`state_t`, `graphic_t`, `audio_t`).
- Enums: `TYPE_e` (`game_type_e`).
- Renamed IDA symbols (e.g. `sub_487E30`) → give a semantic name **only** when purpose is confirmed. Otherwise keep as `sub_XXXXXX` in a comment.

## Address anchors

Every ported function keeps its original virtual address as a preceding comment:

```c
// 0046091B
void load_configuration(state_t *state) { ... }
```

This makes cross-referencing `dkev.asm` trivial (`grep 0046091B disassembly/dkev.asm`).

## Types

Use fixed-width from `libsys/common.h`: `u8/u16/u32/u64`, `i8/i16/i32/i64`, `f32/f64`, `c8`, `b8`. Not `int`/`char`/`short` (except in ported IDA output that will be normalized).

## asm → C workflow

1. Locate the proc in `disassembly/dkev.asm` and capture the IDA C output if useful.
2. Draft the C in a scratch file under `docs/functions/<name>.md` first if non-trivial (>50 lines asm).
3. Port to `src/…` under the correct subsystem file.
4. Add `// XXXXXXXX` address anchor.
5. Update `docs/FUNCTION_MAP.md`.
6. Verify: build (`make dke`), run (`make run`), spot-check behavior matches asm intent.

## Parity checks

Not every port can be diffed. Document what was verified in the function's entry:

- **build**: compiles cleanly with `-Wall`.
- **launch**: game still starts.
- **behavior**: side effect matches asm (e.g. buffer allocated with same size, correct file opened, correct string blitted).
- **untested**: honesty flag — no way to verify yet.

## Docs discipline

- One source of truth per fact — don't duplicate between `ARCHITECTURE.md` and `SUBSYSTEMS.md`. Cross-link instead.
- Keep `FUNCTION_MAP.md` sorted by address.
- Delete stale entries; don't tombstone them.
