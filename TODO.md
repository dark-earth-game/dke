# TODO

Kept in sync w/ `docs/ROADMAP.md`. High-level milestone view.

## Done

- [x] Disassemble the game (IDA 8.4 → `disassembly/dkev.asm`, 1404 procs)
- [x] Create a basic engine architecture (`src/libsys/` layers, `state_t`)
- [x] Create a game detection system (`src/game_detection.c`, locale enum)
- [x] Create a basic game data structure (`state_t`, `game_t` fields)
- [x] Create a basic game loop (`ls_main` w/ tick / event / blit / flip / fps)
- [x] Play the intro cutscenes (CRH decoder + CoreAudio streaming ported; `ls_play_movie` end-to-end w/ SC08.CRH sanity-verified)

## In progress

- [ ] Understand multiple file formats
  - CRH (movie) — spec'd + decoder ported.
  - CAR (font header) — layout known, `FONTE11.CAR` recognised.
  - CIF (font atlas) — TODO decode (`sub_482200` / `sub_4822E0` / `sub_48B660` / `sub_48BE30`).
  - CJR / OFF / CAN — TODO.
- [ ] Draw the first intro screens
  - Font subsystem ported (CP437 fallback active).
  - Intro cutscene wiring (list of movies to play) — TODO (task #10).

## Next up

- [ ] Wire intro cutscene sequence into `ls_main` (task #10).
- [ ] Port CIF atlas decoder → real fonts.
- [ ] Port `game_init_46BEEB` — HUD, weapons, entities bootstrap.
- [ ] Port level loader / entity spawn.
- [ ] Port rendering pipeline (`graphics_triangle_rasteriser`, `create_model_render_data`).

Refer to `docs/ROADMAP.md`, `docs/FUNCTION_MAP.md`, `docs/SUBSYSTEMS.md` for detail.
