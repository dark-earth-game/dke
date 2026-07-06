#include <stdio.h>
#include <string.h>
#include <time.h>

#include "menu.h"
#include "state.h"
#include "game_init.h"
#include "scene.h"
#include "libsys/common.h"
#include "libsys/system.h"
#include "libsys/graphics/graphic.h"
#include "libsys/graphics/font.h"
#include "libsys/movie.h"
#include "libsys/file_system.h"
#include "libsys/audio/wav.h"

/*
 * menu.c — Main menu skeleton (Slice B pass 1)
 *
 * 00437EB7  process_mainmenu_437EB7
 * ASM lines: 77596..80147 (~2551 lines of dkev.asm)
 *
 * Pass 1 scope: static menu display + hover + keyboard nav.
 * Pass 2 (TODO): mouse click routing → sub-menus, save/load slots, trailer.
 *
 * Colour constants — XRGB byte order: 0xFF000000 | (B<<16) | (G<<8) | R
 * matching blit_bmap_to_framebuffer palette expansion.
 */

/* -------------------------------------------------------------------------
 * Colour constants
 * RGB stored as 0xFFBBGGRR to match blit_bmap_to_framebuffer's
 * `0xFF000000 | (r<<16) | (g<<8) | b` expansion.
 * We pass u32 directly to draw_text_at which writes to front_buffer u32 XRGB.
 * front_buffer pixel = 0xFF000000 | (R<<16) | (G<<8) | B
 * So WHITE = 0xFFFFFFFF, YELLOW = 0xFFFFFF00.
 * ------------------------------------------------------------------------- */
#define COLOR_WHITE  0xFFFFFFFFu
#define COLOR_YELLOW 0xFFFFFF00u

/* -------------------------------------------------------------------------
 * SFX state — click sound loaded once in menu_init
 * ------------------------------------------------------------------------- */

/*
 * Click sound: originally INVENT1.WAV in the game's WAV/NORMAL/ directory.
 * If not found, we fall back to the first available WAV from a priority list,
 * then to the first .WAV in the NORMAL/ folder.
 */
static wav_t sfx_click = {0};

/* -------------------------------------------------------------------------
 * Menu item table — MAIN_MENU_ITEMS
 *
 * Positions decoded from asm init block (process_mainmenu_437EB7 prologue):
 *
 * var_4AE0 array — 8 entries, each 40 bytes.
 * The asm initialises items 1..7 (item 0 = "no item" sentinel at idx 0).
 * We map them index 0..6 here for C convenience.
 *
 * Coordinates extracted from asm (screen = 640×480):
 *   Item 1: x=0x4C=76,  y=0x4D=77,  w=0x6F=111, h=0x14=20   action=NEWGAME
 *   Item 2: x=0x4C=76,  y=0x4D=77,  w=0x70=112, h=0x89=137  action=LOAD
 *   Item 3: x=0x4C=76,  y=0x9A=154, w=0x6F=111, h=0xFC=252  action=OPTIONS
 *   Item 4: x=0x4C=76,  y=0xE7=231, w=0x1C5=453,h=0x14=20   action=SAVE
 *   Item 5: x=0x4C=76,  y=0x134=308,w=0x1C5=453,h=0x88=136  action=TRAILER
 *   Item 6: x=0x4C=76,  y=0x181=385,w=0x1C4=452,h=0xFB=251  action=QUIT
 *   Item 7: x=0x99=153, y=0x72=114, w=0xF3=243, h=0xC1=193  action=CREDITS
 *
 * Note: the raw coords from the asm overlap — they are sprite hitboxes for
 * a localised menu IMAGE (MENU_%01d.CIF), not independent text lines.
 * For the skeleton, we use a simplified centred column layout for legibility
 * since the real image rendering (sub_43F7BD hitbox test) is deferred.
 *
 * Simplified layout: centred at x=240, starting y=180, gap=40.
 * These will be replaced with exact bounding-box coords in pass 2 when the
 * full MENU_*.CIF image pipeline lands.
 * ------------------------------------------------------------------------- */

/* Number of items in main menu */
#define MAIN_MENU_ITEM_COUNT 5

static const menu_item_t MAIN_MENU_ITEMS[MAIN_MENU_ITEM_COUNT] = {
    /* label        x    y    w    h    action               */
    { "NEW GAME",  240, 180, 200, 30,  MENU_ACTION_NEW_GAME },
    { "LOAD",      240, 220, 200, 30,  MENU_ACTION_LOAD     },
    { "OPTIONS",   240, 260, 200, 30,  MENU_ACTION_OPTIONS  },
    { "TRAILER",   240, 300, 200, 30,  MENU_ACTION_TRAILER  },
    { "QUIT",      240, 340, 200, 30,  MENU_ACTION_QUIT     },
};

/*
 * Default hovered item index (matches var_4AEC = 6 → zero-based idx 0 in
 * our simplified table; we map original idx 6 = QUIT → our idx 4).
 */
#define MENU_DEFAULT_ITEM 0

/* -------------------------------------------------------------------------
 * Internal helpers
 * ------------------------------------------------------------------------- */

/*
 * menu_item_hit — test if screen coordinate (mx, my) falls inside item hitbox.
 * Returns 1 on hit, 0 otherwise.
 */
static i32 menu_item_hit(const menu_item_t *item, i32 mx, i32 my) {
    return (mx >= item->x && mx < item->x + item->w &&
            my >= item->y && my < item->y + item->h) ? 1 : 0;
}

/*
 * menu_screenshot — dump front_buffer to timestamped PPM file.
 * Enables palette / colour parity verification.
 * Filename: screenshot_<seconds>.ppm in current working directory.
 * Format: P6 PPM (binary) with u32 XRGB bytes extracted to R,G,B triples.
 */
extern state_t *state;
static void menu_screenshot(system_t *sys) {
    (void)sys;  /* unused; accessing global state */
    if (!state) return;

    u32 *fb   = state->graphic.front_buffer;
    i32  w    = state->graphic.width;
    i32  h    = state->graphic.height;

    if (!fb || w <= 0 || h <= 0) {
        print("menu: screenshot failed — invalid framebuffer\n");
        return;
    }

    c8 fname[256];
    snprintf(fname, sizeof(fname), "screenshot_%ld.ppm", time(NULL));

    FILE *f = fopen(fname, "wb");
    if (!f) {
        print("menu: screenshot failed to open %s\n", fname);
        return;
    }

    /* Write PPM P6 header */
    fprintf(f, "P6\n%d %d\n255\n", w, h);

    /* Write pixels: u32 XRGB packing is 0xFF000000 | (B<<16) | (G<<8) | R.
     * Extract R (byte 0), G (byte 1), B (byte 2) and write in R,G,B order. */
    for (i32 y = 0; y < h; y++) {
        for (i32 x = 0; x < w; x++) {
            u32 pix = fb[y * w + x];
            u8 r = (u8)(pix & 0xFF);
            u8 g = (u8)((pix >> 8) & 0xFF);
            u8 b = (u8)((pix >> 16) & 0xFF);
            fputc(r, f);
            fputc(g, f);
            fputc(b, f);
        }
    }

    fclose(f);
    print("menu: screenshot saved %s (%dx%d)\n", fname, w, h);
}

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */

/*
 * menu_load_click_sfx — resolve and load the menu click WAV.
 *
 * Search order (game_path/wav/NORMAL/):
 *   1. INVENT1.WAV  — original game click sound
 *   2. MENU.WAV
 *   3. CLICK.WAV
 *   4. SELECT.WAV
 *   5. First .WAV found in directory (alphabetical order from ls)
 *
 * Logs which file was chosen. No-ops if none found.
 */
static void menu_load_click_sfx(const c8 *game_path) {
    /* Priority list of preferred filenames. */
    static const c8 *preferred[] = {
        "INVENT1.WAV", "MENU.WAV", "CLICK.WAV", "SELECT.WAV", NULL
    };

    c8 path[512];

    /* Try priority list first. */
    for (i32 i = 0; preferred[i]; i++) {
        snprintf(path, sizeof(path), "%s/wav/NORMAL/%s", game_path, preferred[i]);
        if (ls_exists(path)) {
            printf("menu: click sfx using '%s'\n", path);
            if (wav_load(&sfx_click, path)) return;
        }
    }

    /* Fall back: use first .WAV in NORMAL/ (alphabetical order).
     * We do a small open-dir loop using stdio popen to stay C99 / portable. */
    snprintf(path, sizeof(path), "%s/wav/NORMAL", game_path);

    /* Use popen ls to enumerate; C99 has no opendir portably without POSIX. */
    c8 cmd[600];
    snprintf(cmd, sizeof(cmd), "ls \"%s\"/*.WAV 2>/dev/null | head -1", path);

#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
    FILE *lspipe = popen(cmd, "r");
    if (lspipe) {
        c8 first_wav[512] = {0};
        if (fgets(first_wav, (i32)sizeof(first_wav), lspipe)) {
            /* Strip trailing newline */
            i32 len = (i32)strlen(first_wav);
            while (len > 0 && (first_wav[len-1] == '\n' || first_wav[len-1] == '\r')) {
                first_wav[--len] = '\0';
            }
            if (len > 0) {
                printf("menu: click sfx fallback to first WAV: '%s'\n", first_wav);
                pclose(lspipe);
                wav_load(&sfx_click, first_wav);
                return;
            }
        }
        pclose(lspipe);
    }
#endif

    printf("menu: no click sfx found in '%s/wav/NORMAL/'\n", game_path);
}

// 00437EB7 (init phase)
void menu_init(menu_t *menu) {
    menu->state           = MENU_STATE_MAIN;
    menu->hovered_item    = MENU_DEFAULT_ITEM;  /* var_4AEC = 6 → idx 0 default */
    menu->selected_item   = -1;
    menu->dirty           = 1;
    menu->pending_trailer = 0;
    menu->startup_frames_remaining = 30;  /* ignore Cocoa synth click ≈0.5s @60fps */

    /* Fetch backdrop loaded during game_init phase 3+7 */
    menu->backdrop_bmap    = NULL;
    menu->backdrop_palette = NULL;
    game_init_get_menu_backdrop(&menu->backdrop_bmap, &menu->backdrop_palette);

    /* Load click sound effect (INVENT1.WAV or best fallback). */
    extern state_t *state;
    if (state && state->game_path) {
        menu_load_click_sfx(state->game_path);
    }

    /* ---- Pass 3: load cursor sprites ---- */

    /* CURS128.CIF — main pointer cursor.
     * Virtual path mirrors asm string "DATAS>GLOBAL>IMAGES>CURS128.GIF". */
    menu->cursor_bmap = NULL;
    {
        bmap_t *cb = game_init_load_cursor("DATAS>GLOBAL>IMAGES>CURS128.GIF");
        if (cb) {
            menu->cursor_bmap = cb;
            print("menu: cursor loaded CURS128.CIF -> %dx%d\n",
                  (i32)cb->width, cb->height);
        } else {
            print("menu: CURS128.CIF load failed — cursor render will be skipped\n");
        }
    }

    /* GENICO_0..6.CIF — 7-frame animated hover icon.
     * Virtual path pattern mirrors asm "DATAS>MENUS>GENICO_%01d.GIF". */
    i32 genico_loaded = 0;
    for (i32 fi = 0; fi < MENU_GENICO_FRAME_COUNT; fi++) {
        c8 vpath[64];
        snprintf(vpath, sizeof(vpath), "DATAS>MENUS>GENICO_%d.GIF", fi);
        menu->genico_frames[fi] = game_init_load_cursor(vpath);
        if (menu->genico_frames[fi]) {
            genico_loaded++;
        } else {
            print("menu: GENICO_%d.CIF load failed\n", fi);
        }
    }
    print("menu: GENICO frames loaded %d/%d\n", genico_loaded, MENU_GENICO_FRAME_COUNT);

    print("menu: init state=MAIN hovered=%d backdrop_bmap=%p palette=%p\n",
          menu->hovered_item,
          (void *)menu->backdrop_bmap,
          (void *)menu->backdrop_palette);
}

/*
 * menu_release — free menu resources (call when done with the menu).
 * Frees click sfx and all cursor bmaps loaded in menu_init.
 *
 * NOTE: menu_t pointer is not available here (release is stateless).
 * Cursor bmaps must be freed by the caller after calling menu_release.
 * Use menu_release_cursors(menu) for cursor cleanup.
 */
void menu_release(void) {
    wav_free(&sfx_click);
}

/*
 * menu_release_cursors — free cursor bmaps owned by menu_t.
 * Called by process_mainmenu after the menu loop exits.
 */
static void menu_release_cursors(menu_t *menu) {
    if (!menu) return;
    if (menu->cursor_bmap) {
        if (menu->cursor_bmap->owns_buffer && menu->cursor_bmap->pixels)
            free(menu->cursor_bmap->pixels);
        if (menu->cursor_bmap->owns_palette && menu->cursor_bmap->palette)
            free(menu->cursor_bmap->palette);
        free(menu->cursor_bmap);
        menu->cursor_bmap = NULL;
    }
    for (i32 fi = 0; fi < MENU_GENICO_FRAME_COUNT; fi++) {
        bmap_t *gf = menu->genico_frames[fi];
        if (gf) {
            if (gf->owns_buffer && gf->pixels)
                free(gf->pixels);
            if (gf->owns_palette && gf->palette)
                free(gf->palette);
            free(gf);
            menu->genico_frames[fi] = NULL;
        }
    }
}

/*
 * menu_dispatch — central action router for both mouse and keyboard.
 * Called when selected_item is set. Routes action_code → state transitions
 * or pending flags. Resets selected_item to -1 to consume the event.
 * Pass 2: TRAILER and QUIT are wired; LOAD / OPTIONS / NEW_GAME are stubs.
 */
static void menu_dispatch(menu_t *menu) {
    if (menu->selected_item < 0 || menu->selected_item >= MAIN_MENU_ITEM_COUNT) {
        menu->selected_item = -1;
        return;
    }

    menu_action_e act = MAIN_MENU_ITEMS[menu->selected_item].action_code;
    const c8     *lbl = MAIN_MENU_ITEMS[menu->selected_item].label;
    print("menu: click %s action=%d\n", lbl, (i32)act);

    /* Play click feedback before dispatching (fire-and-forget). */
    sfx_play(&sfx_click);

    switch (act) {
        case MENU_ACTION_QUIT:
            /* Mirrors game_exit_code=1 + var_C=2 path in original asm */
            menu->state = MENU_STATE_QUIT;
            print("menu: QUIT selected, transitioning to QUIT state\n");
            break;

        case MENU_ACTION_TRAILER:
            /* sub_437DDD: triggers EADKE.CRH playback from the main menu.
             * Set pending flag — rendered next in menu_render to avoid
             * calling ls_play_movie from inside update. */
            menu->pending_trailer = 1;
            print("menu: TRAILER queued — will play EADKE.CRH\n");
            break;

        case MENU_ACTION_NEW_GAME:
            print("menu: starting new game (level=%d)\n", state->first_room);
            if (scene_load_level(state->first_room) == 0) {
                menu->state = MENU_STATE_LOADING;
                /* TODO: transition to game loop */
            } else {
                print("menu: scene_load_level failed\n");
            }
            break;

        case MENU_ACTION_LOAD:
            /* TODO: sub_437ADC — load-slot picker panel (SAVE.GIF, DKEGAME.CFG).
             * Stub: log and fall through. */
            print("menu: LOAD not yet implemented (sub_437ADC TODO)\n");
            break;

        case MENU_ACTION_OPTIONS:
            /* TODO: sub_438CA5 — options panel (brightness/contrast/shadow sliders).
             * Stub: log and fall through. */
            print("menu: OPTIONS not yet implemented (sub_438CA5 TODO)\n");
            break;

        default:
            print("menu: unhandled action=%d\n", (i32)act);
            break;
    }

    menu->selected_item = -1;  /* consume */
}

// 00437EB7 (update phase — inner loop body)
void menu_update(menu_t *menu, system_t *sys) {
    if (menu->state != MENU_STATE_MAIN) {
        return;
    }

    i32 prev_hovered = menu->hovered_item;

    /* --- Mouse hover (var_798/var_48A0 → ls_get_mouse path) ---
     * Read motion_x/y from system_t (platform layer fills these).
     * Find which item (if any) the cursor overlaps.
     * Mirrors sub_43F7BD hitbox hit-test against var_4AE0 table.
     * Pass 2: approximate hitboxes — will be replaced with exact asm coords
     *         when MENU_%01d.CIF image pipeline lands. */
    i32 mx = sys->motion_x;
    i32 my = sys->motion_y;
    i32 hit = -1;
    for (i32 i = 0; i < MAIN_MENU_ITEM_COUNT; i++) {
        if (menu_item_hit(&MAIN_MENU_ITEMS[i], mx, my)) {
            hit = i;
            break;
        }
    }
    if (hit >= 0) {
        menu->hovered_item = hit;
        /* Log hover label for smoke-test verification */
        if (hit != prev_hovered) {
            print("menu: hover %s\n", MAIN_MENU_ITEMS[hit].label);
        }
    }

    /* --- Mouse click (var_4C68 path in original asm) ---
     * sys->mouse_left == 2: pressed-this-frame edge (set by mouseDown callback,
     * downgraded to 1 by ls_update_events on the next tick).
     * Original: var_4C68 == 1 or == 2 (click / double-click).
     * We treat edge (2) as "confirm" to avoid repeat triggering on held button. */
    if (menu->startup_frames_remaining > 0) {
        menu->startup_frames_remaining--;
        /* Consume any pending click edge during grace window. */
        if (sys->mouse_left == 2) sys->mouse_left = 0;
    } else if (sys->mouse_left == 2 && menu->hovered_item >= 0) {
        menu->selected_item = menu->hovered_item;
        menu_dispatch(menu);
        return;  /* dispatch may set QUIT — let caller check state */
    }

    /* --- Keyboard navigation (process_input_event path) ---
     * ECX=0x25 ('%') = up arrow (var_4AEC--)
     * ECX=0x27 (''') = down arrow (var_4AEC++)
     * ECX=0x0D ('\r') = Enter / confirm
     * ECX=0x1B = Esc → QUIT (mirrors var_C=2 path)
     * sys->actions[] is indexed by macOS keyCode mapped in handle_events.
     * Up/Down arrow + Return/Esc keycodes wired in pass 2 (macos.m). */

    /* Up (macOS keyCode 126 → actions[38]) */
    if (sys->actions[38] == 1) {
        sys->actions[38] = 0;
        menu->hovered_item--;
        if (menu->hovered_item < 0) {
            menu->hovered_item = MAIN_MENU_ITEM_COUNT - 1;
        }
        print("menu: nav UP hovered=%d (%s)\n",
              menu->hovered_item, MAIN_MENU_ITEMS[menu->hovered_item].label);
        menu->dirty = 1;
    }
    /* Down (macOS keyCode 125 → actions[40]) */
    if (sys->actions[40] == 1) {
        sys->actions[40] = 0;
        menu->hovered_item++;
        if (menu->hovered_item >= MAIN_MENU_ITEM_COUNT) {
            menu->hovered_item = 0;
        }
        print("menu: nav DOWN hovered=%d (%s)\n",
              menu->hovered_item, MAIN_MENU_ITEMS[menu->hovered_item].label);
        menu->dirty = 1;
    }
    /* Return/Enter (macOS keyCode 36 → actions[13]) — confirm selection */
    if (sys->actions[13] == 1) {
        sys->actions[13] = 0;
        menu->selected_item = menu->hovered_item;
        print("menu: ENTER on %s\n", MAIN_MENU_ITEMS[menu->hovered_item].label);
        menu_dispatch(menu);
        return;
    }
    /* Esc (macOS keyCode 53 → actions[27]) — QUIT (mirrors ECX=0x1B → game_exit_code) */
    if (sys->actions[27] == 1) {
        sys->actions[27] = 0;
        print("menu: ESC → QUIT\n");
        menu->state = MENU_STATE_QUIT;
        return;
    }

    /* F12 → screenshot */
    if (sys->actions[111] == 1) {
        sys->actions[111] = 0;   /* consume single-press */
        menu_screenshot(sys);
    }

    if (menu->hovered_item != prev_hovered) {
        menu->dirty = 1;
    }
}

// 00437EB7 (render phase — draw_image + text overlay)
void menu_render(menu_t *menu, system_t *sys) {
    /* --- Trailer playback (sub_437DDD path) ---
     * pending_trailer is set by menu_dispatch on TRAILER click/enter.
     * We handle it here rather than in update to ensure the last menu frame
     * has been blitted before the movie takes over the framebuffer.
     * After movie ends, force dirty=1 so the backdrop redraws cleanly. */
    if (menu->pending_trailer) {
        print("menu: playing trailer EADKE.CRH\n");
        ls_play_movie("HDISK:>EARTH>sc>EADKE.CRH");
        menu->pending_trailer = 0;
        menu->dirty = 1;
        /* Re-enter the normal render path to restore the backdrop immediately */
    }

    /* Backdrop blit — mirrors:
     *   draw_image(game_context, var_4C78, 0,0, 640,480, 0,0)
     *   followed by draw_image(game_context, graphics_context, ...)
     * We replicate the Slice A per-frame blit here so ls_main can delegate. */
    extern state_t *state;  /* defined in main.c */

    u32 *fb   = state->graphic.front_buffer;
    i32  fb_w = state->graphic.width;
    i32  fb_h = state->graphic.height;

    if (menu->backdrop_bmap && menu->backdrop_palette) {
        i32 bmp_w = (i32)menu->backdrop_bmap->width;
        i32 bmp_h = menu->backdrop_bmap->height;
        i32 off_x = (fb_w > bmp_w) ? (fb_w - bmp_w) / 2 : 0;
        i32 off_y = (fb_h > bmp_h) ? (fb_h - bmp_h) / 2 : 0;
        blit_bmap_to_framebuffer(fb, fb_w, fb_h,
                                 menu->backdrop_bmap, menu->backdrop_palette,
                                 off_x, off_y);
    }

    /* Text overlay — iterate MAIN_MENU_ITEMS, draw each label.
     * Highlighted item renders in YELLOW, others in WHITE.
     * TODO: replace with sprite blit from MENU_%01d.CIF when image pipeline
     *       lands (sub_4374B0 / sub_4375DF sprite animator). */
    for (i32 i = 0; i < MAIN_MENU_ITEM_COUNT; i++) {
        const menu_item_t *item = &MAIN_MENU_ITEMS[i];
        u32 colour = (i == menu->hovered_item) ? COLOR_YELLOW : COLOR_WHITE;
        draw_text_at(font_handle, item->x, item->y, item->label, colour);
    }

    /* ---- Pass 3: cursor sprite rendering ---- */

    /* Animated GENICO hover icon — draw next to currently hovered item.
     * Frame advances every 100 ms via sys->timer.tick.
     * Positioned at approx (item.x - 20, item.y + 2) — 20 px left of text
     * so the icon sits beside the label without overlapping it.
     * Only renders when hovered_item >= 0 and at least frame 0 loaded. */
    i32 frame_idx = (i32)((sys->timer.tick / 100u) % (u32)MENU_GENICO_FRAME_COUNT);
    if (menu->hovered_item >= 0 && menu->hovered_item < MAIN_MENU_ITEM_COUNT) {
        bmap_t *gf = menu->genico_frames[frame_idx];
        if (gf && gf->pixels) {
            const u8 *pal = gf->palette ? gf->palette : menu->backdrop_palette;
            if (pal) {
                const menu_item_t *hitem = &MAIN_MENU_ITEMS[menu->hovered_item];
                i32 gx = hitem->x - 20;   /* 16–20 px left of text baseline */
                i32 gy = hitem->y + 2;    /* align with text top + 2 px */
                blit_bmap_masked_to_framebuffer(fb, fb_w, fb_h,
                                                gf, pal,
                                                gx, gy, 0 /* index 0 = transparent */);
                print("menu: render genico frame=%d at (%d,%d)\n", frame_idx, gx, gy);
            }
        }
    }

    /* Raw mouse cursor — draw CURS128 at (motion_x, motion_y).
     * Hotspot: top-left = (0,0) for a pointer arrow shape.
     * Skips pixel index 0 (transparent background). */
    i32 mx = sys->motion_x;
    i32 my = sys->motion_y;
    if (menu->cursor_bmap && menu->cursor_bmap->pixels) {
        const u8 *cpal = menu->cursor_bmap->palette
                         ? menu->cursor_bmap->palette
                         : menu->backdrop_palette;
        if (cpal) {
            blit_bmap_masked_to_framebuffer(fb, fb_w, fb_h,
                                             menu->cursor_bmap, cpal,
                                             mx, my, 0 /* index 0 = transparent */);
            print("menu: render cursor at (%d,%d) frame=%d\n", mx, my, frame_idx);
        }
    }

    /* Log render once per dirty cycle */
    if (menu->dirty) {
        print("menu: render items=%d hovered=%d (%s)\n",
              MAIN_MENU_ITEM_COUNT, menu->hovered_item,
              MAIN_MENU_ITEMS[menu->hovered_item].label);
        menu->dirty = 0;
    }
}

// 00437EB7
i32 process_mainmenu(menu_t *menu) {
    /*
     * Pass 2 — action dispatch wired.
     * Pass 3 — cursor sprites loaded and rendered.
     * Returns the current menu state as i32 (0=MAIN, 2=QUIT) so ls_main
     * can break the loop.  Selected_item is consumed by menu_dispatch in
     * menu_update, so we only report state here.
     *
     * Deep tree TODOs (pass 3 remainder):
     *   TODO: sub_43F7BD       — exact hitbox test from var_4AE0[8] asm table
     *   TODO: find_closest_palette_colors — palette remap for 8-bit mode
     *   TODO: ls_lock_bmap / ls_unlock_bmap — 8-bit pixel remap loop (480×640)
     *   TODO: sub_44CFF6       — volume query
     *   TODO: help_screen_context — secondary sound channel
     *   TODO: var_4B40 table   — sub-menu hitbox entries (SAVE/CREDITS panel)
     *   TODO: select_language_version + DKEGAME.CFG — save slot existence check
     *   TODO: game_exit_code   — global action code communicated to ls_main
     *   TODO: is_demo / dword_4B4768 / dword_4B4854 — demo-mode guards
     *   TODO: sub_4380F9       — new game bootstrap (Slice C)
     *   TODO: sub_437ADC       — load-slot picker panel
     *   TODO: sub_438CA5       — options panel (brightness/contrast/shadow)
     *   TODO: menu_process_gamma — pre-menu brightness slider
     *   TODO: sub_4374B0       — sprite animation init (intro fade path)
     *   TODO: sub_4375DF       — sprite animation tick (intro fade loop)
     *   TODO: sub_4378EA       — post-fade state reset
     */

    /* Release cursor bmaps on QUIT (state transitions away from MAIN).
     * menu_release() handles the click sfx; cursors need explicit cleanup here
     * because menu_release() doesn't have a menu_t pointer. */
    if (menu->state == MENU_STATE_QUIT) {
        menu_release_cursors(menu);
    }

    return (i32)menu->state;
}
