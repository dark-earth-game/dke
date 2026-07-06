# scene_load_level — 0046DFEC

## Signature

```c
i32 scene_load_level(i32 level_id);
```

Returns 0 on success, non-zero on error.

## asm summary

`scene_load_level_46DFEC` proc at `0x0046DFEC` (dkev.asm lines 147869–148134, ~265 lines).

- **Parameter**: `ECX` = `level_id` (stored as `var_4` on local frame).
- **Entry sequence**: `set_display_mode(0x1D)` → zero `dword_505C54` → `sprintf(game_temp_path, "DATAS>L%02d", level_id)` → `ls_is_path_exist(game_temp_path)`.
- **Folder missing branch**: `sub_469E34(1)` (fallback/error path); then continues.
- **Level 44 branch**: `cmp var_4, 0x2C` → `handle_data_l44_faux`.
- **Globals zeroed at entry**: `byte_511760=0`, `dword_505C54=0`, `dword_4EDC30/34/38=0`.
- **Globals set**: `byte_4B7280=1`, `world_state=level_id`.
- **player_entity**: `[+0xD04]=0`, `[+0xD08]=0`, `[+0xC64]=0x10000`, `[+0xC68]=0x4000`, `[+0x30]=0xFFFF4000`, `[+0x38]=0xFFFFC000`, `[+0xCEC/CF4/CF0/CF8]=0`, `[+0xC04]=0xFFFFFFFF`, `[+0x64]=0x80`.
- **world_controller**: if NULL → `memset(dword_543C60, 0, 0xF00)` + `sub_46D305` (allocates controller).
- **Asset loads** (via `sub_46D0EA`, TODO): `L%02d.DAT`, `IMAGES/`, `PAL/`, `ANIM2D/`, `IM%02d.OME`.
- **Tail**: `restore_display_mode(1, 0x1D)` → `sub_40636D` → `dword_505C58 = world_state` → `dword_4B2AB8 = 0`.

## Port notes

- Skeleton pass only: all heavy sub-functions (`sub_46D0EA`, `sub_46D305`, `set_display_mode`, `can_do_32k`, etc.) are no-op stubs with `// TODO: sub_XXXXXX` comments.
- `player_entity` is currently `NULL` (not yet exported from `game_init.c`); all offset writes are NULL-guarded.
- The `.INF` header read is a skeleton addition (not in this proc in the asm — owned by `sub_46D0EA`), included so the log shows file-access working before that sub is ported.
- `state->current_level_id` is updated on success (port addition).
- `world_state`, `world_controller`, and the `4EDC*` counters are file-local in `scene.c` (not yet wired to the global asm data segment addresses, which will be needed for entity/rendering code).

## Parity

- build: yes (`make dke` clean, no warnings)
- launch: yes (exit 143 on SIGTERM)
- behavior: NEW GAME click → `scene_load_level(1)` → 25 phase log lines → returns 0; no crash
- untested: actual level data loading (sub_46D0EA not yet ported)

## TODO stubs

| stub name | asm addr | purpose |
|---|---|---|
| `set_display_mode` | — | hardware display mode set |
| `stub_sub_469E34` | 0x00469E34 | missing-level fallback |
| `stub_handle_data_l44_faux` | — | level 44 special case |
| `stub_sub_46D305` | 0x0046D305 | world controller allocation |
| `stub_sub_46D0EA` | 0x0046D0EA | asset loader (DAT/IMAGES/PAL/ANIM2D/OME) |
| `stub_can_do_32k` | — | colour-mode capability check |
| `stub_sub_4640BC` | 0x004640BC | player entity world placement |
| `stub_sub_426D2C` | 0x00426D2C | unknown |
| `stub_process_color_palette` | — | per-entity palette processing |
| `stub_render_sprite_time_based_effect` | — | periodic animation effect |
| `stub_sub_45619D` | 0x0045619D | unknown |
| `stub_set_entities_combat_mode` | — | enable combat for all entities |
| `stub_sub_448F4D` | 0x00448F4D | unknown |
| `stub_ls_clear_bmap` | — | clear bmap pixel buffer |
| `stub_sub_45D27A` | 0x0045D27A | render buffer update |
| `stub_update_player_interface` | — | HUD/inventory render |
| `stub_sub_45E070` | 0x0045E070 | unknown |
| `stub_sub_47674F` | 0x0047674F | unknown |
| `stub_render_entity_with_ui` | — | render companion entities |
| `stub_apply_model_transform` | — | model transform matrix |
| `stub_system_get_gametime` | — | game timer read |
| `stub_restore_display_mode` | — | display mode restore |
| `stub_sub_40636D` | 0x0040636D | unknown tail call |
