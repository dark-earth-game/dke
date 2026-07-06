# player_entity_init / player_entity_init_secondary / player_input_update

Source: `src/player.c` + `src/player.h`

## Addresses

| C name | asm addr | asm lines |
|---|---|---|
| `player_entity_init` | 0046493B | 136208..136567 (359 lines) |
| `player_entity_init_secondary` | 00464F6D | 136575..136796 (221 lines) |
| `player_input_update` | — (new) | — |

## player_entity_init (0046493B)

### Purpose

Allocates the player entity struct, zero-inits it, and sets all default fields that the asm writes immediately after `_memset`. Called from `game_init` phase 33.

### Allocation

```c
i32 entity_size = 0x0EA0; /* 3744 bytes */
void *ent = calloc(1, entity_size);
```

Asm:
```asm
mov ecx, 0EA0h
call allocate_memory        ; eax = entity ptr
push 0EA0h; push 0; push eax
call _memset
```

### Default fields set (asm-faithful)

All from `sub_46493B` player path (ECX < 0x62):

| offset | value | asm source |
|---|---|---|
| +0x000 | 0 | `mov [eax+10h],0; mov ecx,[eax+10h]; mov [eax],ecx` |
| +0x004 | 0 | mirror of +0x14 |
| +0x008 | 0x27F | mirror of +0x18 |
| +0x00C | 0x1DF | mirror of +0x1C |
| +0x010 | 0 | position_x |
| +0x014 | 0 | position_y |
| +0x018 | 0x27F | bound_right (639) |
| +0x01C | 0x1DF | bound_bottom (479) |
| +0x030 | 0xFFFF4000 | `mov dword ptr [eax+30h],0FFFF4000h` |
| +0x038 | 0xFFFFC000 | `mov dword ptr [eax+38h],0FFFFC000h` |
| +0x064 | 0x50 | `mov dword ptr [eax+64h],50h` |
| +0x070 | 1 | flag |
| +0x074 | 1 | flag |
| +0x078 | 0 | screen-clip left |
| +0x07C | 0 | screen-clip top |
| +0x080 | 0x27F | screen-clip right |
| +0x084 | 0x1DF | screen-clip bottom |
| +0x090 | 2 | entity type |
| +0x094 | 2 | secondary type |
| +0xC04 | 0xFFFFFFFF | `mov dword ptr [eax+0C04h],0FFFFFFFFh` |
| +0xC64 | 0x10000 | `mov dword ptr [eax+0C64h],10000h` |
| +0xC68 | 0x4000 | `mov dword ptr [eax+0C68h],4000h` |
| +0xD34 | current_game_time | `mov eax,current_game_time; mov [ecx+0D34h],eax` |
| +0xD64 | 0 | anim state |
| +0xD68 | 0 | anim state |
| +0xD80 | 0 | squad flag (0 = player, no NPC squad) |
| +0xE94 | 0xFFFF | `mov dword ptr [eax+0E94h],0FFFFh` |

### Stubbed nested sub-calls (>100 asm lines each)

- `sub_462BCC` — creature descriptor / persona file load
- `setup_creature` — mesh + animation init
- `process_scene_hierarchy_with_memory` — scene hierarchy builder
- `create_single_control_point` — root control point creation
- `sub_4699D9` — anim state init from `[entity+0x50h]`
- `sub_41F6BA` — entity registration into global table

### Output

`g_player_entity` (extern `void *`) set to the allocated pointer.

### Parity checks

- **build**: clean
- **launch**: no crash; `g_player_entity = 0x...` non-NULL in log
- **behavior**: `player_entity_init: allocated 3744 bytes` fires in log
- **untested**: field values — can't diff against running exe until entity rendering lands

---

## player_entity_init_secondary (00464F6D)

### Purpose

Second-phase init: wires +0xCD8 dialogue buffer, reads weapon type from slot table, and loads the player's starting weapon entity. Called from `game_init` phase 34 after `player_entity_init`.

### Key asm logic

1. `if entity == player_entity` → `[entity+0xCD8] = &byte_4F60E0`
2. `if entity == companion_entity` → `[entity+0xCD8] = &byte_4F60E0 + 0x1000`
3. `if entity == secondary_companion_entity` → `[entity+0xCD8] = &byte_4F60E0 + 0x2000`
4. Read weapon type: `cl = [entity+0xACh][0x97]` → `var_C`
5. If `var_C == 0`: skip weapon init (no weapon)
6. Player branch (`var_94 = 0`): `sub_4648DC` + `sprintf "O%c%02d"` + `sub_475BB6` → weapon entity load, attach via `sub_472C5C` / `sub_473756` / `sub_41F6BA`
7. If player: `dword_505DDC = var_C; sub_471B80`

### Stubbed nested sub-calls

- `sub_4648DC` — weapon class decode
- `sub_475BB6` — object lookup by name (`"O%c%02d"` format)
- `sub_472C5C` — weapon entity init
- `sub_473756` — weapon entity attach to player
- `sub_41F6BA` — entity registration
- `sub_471B80` — post-init weapon hook

### Parity checks

- **build**: clean
- **launch**: no crash; all TODO stubs fire in log
- **untested**: actual weapon attachment — no weapon entities loaded yet

---

## player_input_update

### Purpose

Per-frame WASD + mouse poll. Resets all output fields each call; sets only active directions.

### Mapping

| action | key | output |
|---|---|---|
| W held | `sys->actions[119]` | `out->velocity_z = -1.0f` (forward) |
| S held | `sys->actions[115]` | `out->velocity_z = +1.0f` (backward) |
| A held | `sys->actions[97]`  | `out->velocity_x = -1.0f` (strafe left) |
| D held | `sys->actions[100]` | `out->velocity_x = +1.0f` (strafe right) |
| mouse_left == 2 | pressed-edge | `out->want_action = 1` |

### Integration

Called from `ls_main` when `menu.state == MENU_STATE_LOADING`:

```c
static player_input_t pinput;
player_input_update(&pinput, system);
```

### Parity checks

- **build**: clean
- **launch**: no crash; log line fires on WASD press in LOADING state
- **untested**: integration with entity movement (entity update pipeline not yet ported)
