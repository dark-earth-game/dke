#pragma once

/*
 * menu.h — Main menu state machine (00437EB7)
 *
 * Slice B pass 1: skeleton — static menu display, hover state, keyboard nav.
 * Interaction routing (mouse click → sub-menus) is pass 2.
 *
 * Original function: process_mainmenu_437EB7
 * ASM lines: 77596..80147 (~2551 lines)
 *
 * Key findings from asm analysis:
 *   - Menu items are IMAGE hitboxes, not text labels.
 *     Backdrop: DATAS>MENUS>MENUGEN.GIF (full 640x480).
 *     Menu overlay: DATAS>MENUS>MENU_%01d.CIF (language-specific).
 *     Cursor: DATAS>GLOBAL>IMAGES>CURS128.GIF.
 *   - Hitbox table var_4AE0[8]: 8 entries × 40 bytes each.
 *     Each entry: { x, y, w, h, action_code, ... }
 *   - Default hovered item = 6 (var_4AEC init).
 *   - Mouse state var_4C68: 0=up, 1=click, 2=doubleclick.
 *   - Selected action in var_C returned to caller.
 *   - Keyboard nav: ECX=0x25 (up), ECX=0x27 (down),
 *                   ECX=0x0D (enter), ECX=0x1B (esc).
 *   - Action codes: 1=new game, 2=quit, 3=load, 4=options,
 *                   5=save, 6=trailer, 7=credits.
 */

#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/graphics/graphic.h"

/* -------------------------------------------------------------------------
 * Enums
 * ------------------------------------------------------------------------- */

typedef enum {
    MENU_STATE_MAIN,       /* showing main menu */
    MENU_STATE_LOADING,    /* loading game data after selection */
    MENU_STATE_QUIT,       /* user selected quit — caller breaks loop */
    /* placeholders — extend as sub-menus land */
    MENU_STATE_NEWGAME,
    MENU_STATE_LOAD,
    MENU_STATE_OPTIONS,
    MENU_STATE_TRAILER,
} menu_state_e;

/*
 * menu_action_e — values returned by process_mainmenu to its caller.
 * Matches var_C / game_exit_code logic in original asm.
 *
 * Pass 2: renamed NEWGAME → NEW_GAME for consistency with task spec.
 * Values are unchanged — compatible with existing asm mapping.
 */
typedef enum {
    MENU_ACTION_NONE     = 0,
    MENU_ACTION_NEW_GAME = 1,   /* var_C == 1 → start new game (sub_4380F9) */
    MENU_ACTION_QUIT     = 2,   /* var_C == 2 → exit application */
    MENU_ACTION_LOAD     = 3,   /* var_C == 3 → load saved game (sub_437ADC) */
    MENU_ACTION_OPTIONS  = 4,   /* var_C == 4 → options sub-menu (sub_438CA5) */
    MENU_ACTION_SAVE     = 5,   /* var_C == 5 → save sub-menu */
    MENU_ACTION_TRAILER  = 6,   /* var_C == 6 → play trailer (sub_437DDD) */
    MENU_ACTION_CREDITS  = 7,   /* var_C == 7 → credits */
} menu_action_e;

/* -------------------------------------------------------------------------
 * Structs
 * ------------------------------------------------------------------------- */

/*
 * menu_item_t — one menu hitbox entry.
 *
 * Original asm: 40-byte struct in var_4AE0[8] / var_4B40[2].
 * Fields accessed at offsets +00 +04 +08 +0C +10 +14 in sub_43F7BD hit-test.
 * x/y/w/h are in screen pixels (640×480 viewport).
 * label is our port addition for fallback CP437 text rendering.
 */
typedef struct {
    const c8    *label;       /* display label (port addition) */
    i32          x;           /* hitbox left edge in screen pixels */
    i32          y;           /* hitbox top edge in screen pixels */
    i32          w;           /* hitbox width */
    i32          h;           /* hitbox height */
    menu_action_e action_code;
} menu_item_t;

/* Number of animated GENICO frames (GENICO_0.CIF .. GENICO_6.CIF) */
#define MENU_GENICO_FRAME_COUNT 7

/*
 * menu_t — live menu state.
 */
typedef struct {
    menu_state_e  state;
    i32           hovered_item;    /* -1 = none; else index into current item table */
    i32           selected_item;   /* -1 = none; else index triggering action */
    b8            dirty;           /* redraw flag — set by init/update */
    b8            pending_trailer; /* pass 2: set to trigger ls_play_movie next frame */

    /* loaded backdrop bmap + palette (fetched once from game_init) */
    bmap_t       *backdrop_bmap;
    u8           *backdrop_palette;

    /* Startup guard — Cocoa emits synthetic mouseDown events when the window
     * gains focus. Ignore mouse clicks until this many update() calls have
     * passed, so a spurious click at frame 0 doesn't trigger QUIT. */
    i32           startup_frames_remaining;

    /* Pass 3: cursor sprites loaded in menu_init.
     *
     * cursor_bmap    — CURS128.CIF: the OS mouse pointer sprite.
     *                  Drawn at (motion_x, motion_y) every frame.
     * genico_frames  — GENICO_0..6.CIF: 7-frame animated hover icon.
     *                  Cycled every 100 ms; drawn next to the hovered item.
     *
     * Either or both may be NULL if the CIF failed to load (graceful skip).
     * Both are freed in menu_release.
     */
    bmap_t       *cursor_bmap;                          /* CURS128.CIF */
    bmap_t       *genico_frames[MENU_GENICO_FRAME_COUNT]; /* GENICO_0..6.CIF */
} menu_t;

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */

/*
 * menu_init — initialise menu_t and fetch backdrop from game_init.
 * Call once after game_init completes, before the main loop.
 * Mirrors var_4AEC=6, var_C=0, var_4C68=0 init in process_mainmenu_437EB7.
 */
void menu_init(menu_t *menu);

/*
 * menu_update — read input, update hover/select state.
 * Call each frame before menu_render.
 * Reads sys->motion_x/y for hover; sys->actions[] for keyboard nav.
 * Sets menu->selected_item and logs — no click routing yet (pass 2).
 */
void menu_update(menu_t *menu, system_t *sys);

/*
 * menu_render — draw MENUGEN backdrop + text overlay into front_buffer.
 * Calls blit_bmap_to_framebuffer for backdrop then draw_text_at for labels.
 */
void menu_render(menu_t *menu, system_t *sys);

/*
 * process_mainmenu — 00437EB7 skeleton port.
 * Outer flow: init → update → render loop.
 * Returns menu_action_e on selection (caller decides next state).
 * Deep sub-tree is TODO stubs per function.
 */
i32 process_mainmenu(menu_t *menu);

/*
 * menu_release — free menu resources (click sfx, etc.).
 * Call once after the menu loop exits.
 */
void menu_release(void);
