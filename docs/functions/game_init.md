# game_init (0046BEEB)

**File**: `src/game_init.c` + `src/game_init.h`
**Signature**: `b8 game_init(state_t *state)`
**Called from**: `ls_main` after `init_all_systems(system)`, before intro cutscene.
**Original symbol**: `game_init_46BEEB`
**Asm lines**: 145447–145906 (`disassembly/dkev.asm`)

## Purpose

Big game-side bootstrap that runs once after LIBSYS is up. Sets up:
- HUD health / loading bitmaps
- Gouraud shading index buffer
- Colour palette from MENUGEN.GIF
- Weapon-slot and entity-slot tables
- Render bmaps (640×480, 320×240)
- Player entity and game-state globals
- Font subsystem (delegates to `init_font_system`)
- Weapons inventory display

## Phase list

| # | Phase | Ported? | Notes |
|---|-------|---------|-------|
| 1 | Debug banner "Init Scene ..." | inline | conditional on `debug_enabled` |
| 2 | HUD health bitmap (GINETTE.CIF/GIF or fallback bmap) | stub | `select_language_version` + `image_load_compressed` stubbed |
| 3 | LOADING.GIF compressed image load | stub | `image_load_compressed` stubbed |
| 4 | LOADING.GIF transparency pass (non-0xFF → 0) | inline | loop ported; skipped when bmap is NULL (stub returns NULL) |
| 5 | Two HUD bmap copies from GINETTE dims | inline | `ls_init_bmap_ex` calls wired with fallback sizes |
| 6 | Gouraud index buffer (0x4100 bytes) | inline | `allocate_memory(MEM_SLOT_HEALTH, 0x4100)` |
| 7 | MENUGEN.GIF palette load + input_state_table zero-init | stub | `load_image_color_data` + `ls_free_palette` stubbed |
| 8 | sub_425D50 (post-palette) | stub | purpose unknown |
| 9 | initialize_health_hud | stub | 2048-byte HUD buffer with numeric ramp |
| 10 | sub_45D395 | stub | |
| 11 | sub_421960 | stub | |
| 12 | sub_41F7C8 | stub | |
| 13 | sub_44D706 | stub | |
| 14 | sub_43040A | stub | |
| 15 | Weapon-slot table (4×31) | inline skeleton | loop structure preserved; pointer writes TODO (globals not in state_t) |
| 16 | Entity-slot table (98×31) | inline skeleton | loop structure preserved; pointer writes TODO |
| 17 | Zero-init global tables | inline skeleton | `g_dword_4ECD3C=0`; memset TODOs for unmapped globals |
| 18 | Debug banner "Init Divers ..." | inline | |
| 19 | FLAM.MAT material load | stub | `sub_45018B` stubbed; HDD/CD branch preserved |
| 20 | allocate_and_initialize_health_buffer | stub | |
| 21 | sub_42F88E | stub | |
| 22 | register_player_creature(1) | stub | |
| 23 | init_font_system | **ported** | full call to existing `init_font_system()` |
| 24 | sub_44D676 | stub | IDA label `__wsetargv` likely wrong |
| 25 | sub_48F2C0 (texture config, 0x3E8 entries) | stub | |
| 26 | sub_48F230 (entity buffer 640×480) | stub | |
| 27 | Render bmaps (colour-mode branch) | inline | `ls_init_bmap_ex` calls with correct dims/bpp; both branches |
| 28 | render_buffer2 (640×480) + dword_507474 (320×240) | inline | `ls_init_bmap_ex` calls |
| 29 | objets.gif load | stub | `image_load_compressed` stubbed |
| 30 | sub_4130AA (resource init) | stub | |
| 31 | initialize_resource + palette_data wire-up | stub | |
| 32 | game_state / game_death_state from dword_4B70E8 | inline skeleton | assignment preserved; dword_4B70E8 is NULL (global not ported) |
| 33 | sub_46493B (player entity setup) | stub | |
| 34 | sub_464F6D (player entity secondary init) | stub | |
| 35 | Entity/context palette cross-wiring | stub | pointer offsets documented in comment |
| 36 | system_get_gametime → player_entity+D5Ch | stub | |
| 37 | sub_48F1D0 ×2 (render surface) | stub | |
| 38 | set_entities_combat_mode | stub | |
| 39 | sub_426335 | stub | |
| 40 | sub_471C18 (objets.txt) | stub | |
| 41 | special_mode_active → dword_505DD8 | inline | direct assignment |
| 42 | 16-bit colour: sub_491ED0 + sub_491FD0 | stub | branch wired; callees stubbed |
| 43 | sub_45B207 | stub | |
| 44 | display_weapons_inventory | stub | |
| 45 | sub_4161B0 + dword_4B787C=0 | inline | dword_4B787C zero-write ported |

## Key data addresses

| asm symbol | addr | C name | role |
|------------|------|--------|------|
| dword_4DDE1C | 004DDE1C | `g_ginette_bmap` | GINETTE HUD source bmap |
| dword_4DE648 | 004DE648 | `g_health_bmap_a` | HUD bmap copy A |
| dword_4DE64C | 004DE64C | `g_health_bmap_b` | HUD bmap copy B |
| dword_4DE640 | 004DE640 | `g_gouraud_table` | Gouraud 0x4100-byte table |
| index_buffer | — | `g_index_buffer` | `g_gouraud_table` aligned + 0x100 |
| dword_4B71F8 | 004B71F8 | `g_menugen_palette` | MENUGEN colour palette |
| render_buffer1 | — | `g_render_buffer1` | Primary 640×480 render bmap |
| render_buffer2 | — | `g_render_buffer2` | Secondary 640×480 render bmap |
| debug_surface | — | `g_debug_surface` | Debug overlay bmap |
| dword_4B71FC | 004B71FC | `g_bmap_4B71FC` | Alt render bmap |
| dword_4B7200 | 004B7200 | `g_bmap_4B7200` | Alt render bmap 2 |
| dword_507474 | 00507474 | `g_bmap_507474` | Half-res 320×240 bmap |
| game_state | — | `g_game_state` | Active game state ptr |
| game_death_state | — | `g_game_death_state` | Death state ptr (= game_state initially) |
| player_entity | — | `g_player_entity` | Player entity handle |
| dword_4FB1E0 | 004FB1E0 | `g_flam_mat` | FLAM.MAT material handle |

## Verification

- **build**: compiles clean, zero warnings (`-Wall`).
- **launch**: game starts, logs all 45 phases, exits 143 on SIGTERM. No `Abort trap` / `Segmentation fault`.
- **log**: `grep game_init /tmp/dke.log` shows all 45 `game_init: ...` lines.
- **untested**: actual resource loads (all image/palette/entity sub-functions are stubs).
