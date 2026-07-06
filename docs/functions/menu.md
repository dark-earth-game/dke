# process_mainmenu — 00437EB7

## Signature

```c
i32 process_mainmenu(menu_t *menu);
void menu_init(menu_t *menu);
void menu_update(menu_t *menu, system_t *sys);
void menu_render(menu_t *menu, system_t *sys);
```

## asm summary

`process_mainmenu_437EB7` (ASM lines 77596..80147, ~2551 lines).

**Prologue (lines 77752..77889):**
- Allocates ~0x4D40 bytes of stack frame via `__alloca_probe`.
- Initialises two local hitbox tables:
  - `var_4AE0[8]`: 8 main-menu hitbox entries (40 bytes each = `[eax+eax*4]*8`).
  - `var_4B40[2]`: 2 secondary hitbox entries (ESC/Enter panel).
- Each hitbox entry has fields at offsets +00 (x), +04 (y), +08 (x+w), +0C (y+h), +10 (action), +14 (sprite_idx).
- Coordinates extracted (screen 640×480):
  - Item 1: x=76, y=77, w=111, h=20   → NEWGAME
  - Item 2: x=76, y=77, w=112, h=137  → LOAD
  - Item 3: x=76, y=154, w=111, h=252 → OPTIONS
  - Item 4: x=76, y=231, w=453, h=20  → SAVE
  - Item 5: x=76, y=308, w=453, h=136 → TRAILER
  - Item 6: x=76, y=385, w=452, h=251 → QUIT
  - Item 7: x=153, y=114, w=243, h=193 → CREDITS
- `var_4AEC = 6`: default hovered item index.
- `var_4C7C = 0xFFFFFFFE`: sentinel for "no state" (hitbox ptr field).

**Image loading (lines 77891..78240):**
- `select_language_version("DKEGAME.CFG")` — checks for save slots.
- `image_load_compressed("DATAS>MENUS>MENU_%01d.CIF")` → `var_4C78` — menu overlay image.
- `load_image_color_data(...)` → `var_4C84` — palette for menu.
- `image_load_compressed("DATAS>GLOBAL>IMAGES>CURS128.GIF")` → `var_4C80` — cursor sprite.
- `image_load_compressed("DATAS>MENUS>GENICO_%01d.GIF")` → `var_4` — animated icon.
- `find_closest_palette_colors` — palette remap for 8-bit mode.
- `ls_load_sound("INVENT1.WAV", 8)` → `var_4C6C` — click sound.

**Main loop (lines 78300..80147):**
- `loc_4389AE`: outer loop — checks `var_4C68==2` (double-click exit) or `var_C==7` (credits exit).
- `ls_update_event` + `process_input_command(0x12, 0x09)` — gamepad/joystick.
- `ls_get_mouse` → fills `var_798` (x), `var_48A0` (y), `var_4C68` (button).
- `draw_image(var_4C78, game_context)` — blit menu overlay.
- `sub_43F7BD(var_798, var_48A0, var_4C68, 8, var_4AE0, var_4AF0)` — hitbox test; returns hit item index.
- Hit result → `var_4AEC` (hovered item).
- `overlay_cursor_image(var_4, game_context, ...)` — blit animated cursor sprite.
- Input dispatch: ECX=0x25 up, ECX=0x27 down, ECX=0x0D enter, ECX=0x1B esc.
- Actions: `var_C = 1..7` routed by item, `game_exit_code = 1` for quit.

## Port notes

- **Pass 1 scope**: menu state machine, hover, keyboard nav, text overlay. Hitbox image rendering deferred.
- **Pass 2 scope**: mouse click routing, action dispatch, trailer wire-up, keyboard key mapping, Cocoa Y-flip.
- **Simplified item table**: `MAIN_MENU_ITEMS[5]` uses centred column layout (x=240, y=180..340, w=200, h=30). Hitbox widened to 200px in pass 2. Real coordinates from `var_4AE0[8]` deferred to pass 3 (requires MENU_%01d.CIF pipeline).
- **Colours**: `COLOR_WHITE = 0xFFFFFFFF`, `COLOR_YELLOW = 0xFFFFFF00` as 32-bit XRGB.
- **Keyboard**: `sys->actions[38]`=up, `sys->actions[40]`=down, `sys->actions[13]`=enter, `sys->actions[27]`=esc. macOS keyCodes 126/125/36/53 mapped in `macos.m` (pass 2).
- **Mouse**: `sys->mouse_left == 2` (pressed-edge) triggers dispatch. Cocoa Y-coord flipped in both `mouseMoved` and `mouseDown`.
- **Action dispatch**: `menu_dispatch()` — single router for click+keyboard. QUIT transitions state; TRAILER sets `pending_trailer`; others are logged stubs.
- **Trailer**: `pending_trailer` flag consumed in `menu_render` → `ls_play_movie("HDISK:>EARTH>sc>EADKE.CRH")`.
- **Default hover**: item 0 (NEW GAME).
- **Backdrop**: `menu_render` owns `blit_bmap_to_framebuffer`.
- **Cursor overlay (pass 3)**: `menu_render` blits GENICO animated icon and CURS128 pointer each frame via `blit_bmap_masked_to_framebuffer` (index-0 transparent). Cursors stored in `menu_t.cursor_bmap` + `menu_t.genico_frames[7]`; loaded/freed by `menu_init`/`menu_release_cursors`.

## TODO stubs (pass 4)

| sub | intent |
|---|---|
| sub_43F7BD | real hitbox hit-test against var_4AE0[8] asm table (exact coords) |
| sub_4380F9 | new game bootstrap (Slice C) |
| sub_437ADC | load-slot picker panel (SAVE.GIF, DKEGAME.CFG) |
| sub_438CA5 | options panel (brightness/contrast/shadow sliders) |
| menu_process_gamma | pre-menu brightness slider |
| sub_4374B0 | sprite animator init (intro fade path, distinct from our direct-blit approach) |
| sub_4375DF | sprite animator tick (intro fade loop) |
| sub_4378EA | post-fade state reset |
| overlay_cursor_image | original asm wrapper — our port uses blit_bmap_masked_to_framebuffer directly |
| find_closest_palette_colors | palette remap for 8-bit colour mode |
| ls_load_sound / ls_play_sound | INVENT1.WAV click feedback |
| sub_44CFF6 | volume query |
| help_screen_context | secondary sound channel for UI sounds |
| var_4B40 table | save/credits panel hitbox entries |
| DKEGAME.CFG slot check | determine if Load/Save items are active |
| is_demo guard | disable certain items in demo mode |

## Parity

**Pass 1:**
- build: yes — `make dke` exit 0, no new warnings
- launch: yes — exit 143 on SIGTERM, no crash
- behavior: `menu: init state=MAIN hovered=0 backdrop_bmap=0x... palette=0x...` and `menu: render items=5 hovered=0 (NEW GAME)` in `/tmp/dke.log`. MENUGEN.GIF 640×480 backdrop blits every frame.

**Pass 2:**
- build: yes — `make dke` exit 0, no new warnings
- launch: yes — exit 143 on SIGTERM, no crash
- behavior: `menu: click NEW GAME action=1`, `menu: NEW GAME not yet implemented (sub_4380F9 TODO)` in `/tmp/dke.log`. Clicking TRAILER → `menu: TRAILER queued — will play EADKE.CRH` + `menu: playing trailer EADKE.CRH`. QUIT click/ESC → state=QUIT, main loop breaks.

**Pass 3 (cursor subitem):**
- build: yes — `make dke` exit 0, no new warnings
- launch: yes — exit 143 on SIGTERM, no crash
- behavior: `menu: cursor loaded CURS128.CIF -> 65x35`, `menu: GENICO frames loaded 7/7`, `menu: render cursor at (0,0) frame=N` present in `/tmp/dke.log`. All 7 GENICO frames loaded (720×116 each). CURS128 blits at (motion_x, motion_y); GENICO blits at (item.x-20, item.y+2) with index-0 transparency.
