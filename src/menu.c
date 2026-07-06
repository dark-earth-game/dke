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

/* Number of items in main menu. Matches var_4AE0[8] entries used by asm — we
 * expose the 6 primary buttons (idx 0..5). Entries 6/7 (CREDITS panel) are
 * TODO — they require sub-menu wiring. */
#define MAIN_MENU_ITEM_COUNT 6

/* Real hitboxes + GENICO sub-rects decoded from process_mainmenu_437EB7
 * prologue (asm 77770..77834). Each var_4AE0[N] stores:
 *   +0 src_x, +4 src_y, +8 w, +C h, +10 dst_x, +14 dst_y, +1C action
 * Blit dst equals hitbox origin; GENICO icon (76×77 for buttons 0..5) is drawn
 * on top of the MENU_<lang>.CIF backdrop button at the same position. */
static const menu_item_t MAIN_MENU_ITEMS[MAIN_MENU_ITEM_COUNT] = {
    /* label        x    y    w   h  sx   sy  action               */
    { "NEW GAME",  111,  20,  76, 77,   0, 0, MENU_ACTION_NEW_GAME },
    { "QUIT",      112, 137,  76, 77,  77, 0, MENU_ACTION_QUIT     },
    { "LOAD",      111, 252,  76, 77, 154, 0, MENU_ACTION_LOAD     },
    { "OPTIONS",   453,  20,  76, 77, 231, 0, MENU_ACTION_OPTIONS  },
    { "SAVE",      453, 136,  76, 77, 308, 0, MENU_ACTION_SAVE     },
    { "TRAILER",   452, 251,  76, 77, 385, 0, MENU_ACTION_TRAILER  },
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
 * blit_bmap_subrect_masked — copy an (sw, sh) rect from src starting at
 * (sx, sy) to the framebuffer at (dx, dy), skipping pixels whose palette
 * index equals mask_idx.  Mirrors the overlay_cursor_image → ls_bit_move
 * path in the original asm, which takes a source sub-rectangle rather than
 * the whole bmap. The existing blit_bmap_masked_to_framebuffer draws the
 * entire src bmap; GENICO packs several icons into one sheet, so we need
 * per-item sub-rects.
 */
static void blit_bmap_subrect_masked(u32 *fb, i32 fb_w, i32 fb_h,
                                     const bmap_t *src, const u8 *palette,
                                     i32 sx, i32 sy, i32 sw, i32 sh,
                                     i32 dx, i32 dy, u8 mask_idx) {
    if (!fb || !src || !src->pixels || !palette) return;

    i32 src_w = (i32)src->width;
    i32 src_h = src->height;
    if (sx < 0) { sw += sx; dx -= sx; sx = 0; }
    if (sy < 0) { sh += sy; dy -= sy; sy = 0; }
    if (sx + sw > src_w) sw = src_w - sx;
    if (sy + sh > src_h) sh = src_h - sy;
    if (sw <= 0 || sh <= 0) return;

    if (dx < 0) { sw += dx; sx -= dx; dx = 0; }
    if (dy < 0) { sh += dy; sy -= dy; dy = 0; }
    if (dx + sw > fb_w) sw = fb_w - dx;
    if (dy + sh > fb_h) sh = fb_h - dy;
    if (sw <= 0 || sh <= 0) return;

    for (i32 row = 0; row < sh; row++) {
        const u8 *srow = src->pixels + (sy + row) * src_w + sx;
        u32      *drow = fb + (dy + row) * fb_w + dx;
        for (i32 col = 0; col < sw; col++) {
            u8 idx = srow[col];
            if (idx == mask_idx) continue;
            u8 r = palette[idx * 3 + 0];
            u8 g = palette[idx * 3 + 1];
            u8 b = palette[idx * 3 + 2];
            /* Byte order matches blit_bmap_to_framebuffer: byte0=R, byte1=G,
             * byte2=B — NSBitmapImageRep expects R in low byte. */
            drow[col] = 0xFF000000u | ((u32)b << 16) | ((u32)g << 8) | (u32)r;
        }
    }
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
 * Search order:
 *   1. wav/ANIMS/INVENT1.WAV — real game path confirmed from asm string "%sINVENT1.WAV"
 *                               and on-disk at bin/game/wav/ANIMS/INVENT1.WAV
 *   2. wav/NORMAL/INVENT1.WAV — secondary location searched in original engine
 *   3. wav/NORMAL/MENU.WAV
 *   4. wav/NORMAL/CLICK.WAV
 *   5. wav/NORMAL/SELECT.WAV
 *   6. First .WAV found in wav/NORMAL/ (alphabetical order from ls)
 *
 * Logs which file was chosen. No-ops if none found.
 */
static void menu_load_click_sfx(const c8 *game_path) {
    /* Priority list: subdir/filename pairs. */
    static const struct { const c8 *subdir; const c8 *fname; } candidates[] = {
        { "wav/ANIMS",  "INVENT1.WAV" },   /* real asm path — "%sINVENT1.WAV" in wav/ANIMS */
        { "wav/NORMAL", "INVENT1.WAV" },   /* secondary location */
        { "wav/NORMAL", "MENU.WAV"    },
        { "wav/NORMAL", "CLICK.WAV"   },
        { "wav/NORMAL", "SELECT.WAV"  },
        { NULL, NULL }
    };

    c8 path[512];

    /* Try priority list first. */
    for (i32 i = 0; candidates[i].subdir; i++) {
        snprintf(path, sizeof(path), "%s/%s/%s",
                 game_path, candidates[i].subdir, candidates[i].fname);
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

    /* GENICO_<language_id>.CIF — single hover-icon sprite sheet.
     * Asm: image_load_compressed(sprintf("DATAS>MENUS>GENICO_%01d.GIF", language_id)).
     * NOT animation frames — GENICO_0..6.CIF are language variants (0..6).
     * Icons for each menu button are sub-rects within the single sheet
     * (see MAIN_MENU_ITEMS.src_x/src_y). */
    menu->genico_bmap = NULL;
    {
        i32 lang = (state && state->language_id > 0) ? state->language_id : 1;
        c8 gpath[64];
        snprintf(gpath, sizeof(gpath), "DATAS>MENUS>GENICO_%d.GIF", lang);
        bmap_t *gb = game_init_load_cursor(gpath);
        if (gb) {
            menu->genico_bmap = gb;
            print("menu: GENICO loaded GENICO_%d.CIF -> %dx%d\n",
                  lang, (i32)gb->width, gb->height);
        } else {
            print("menu: GENICO_%d.CIF load failed — hover icon disabled\n", lang);
        }
    }

    /* ---- Pass 3 refinement: load language-specific menu overlay sprite ----
     *
     * Asm: image_load_compressed("DATAS>MENUS>MENU_%01d.CIF") → var_4C78.
     * Language id from state->language_id (0 = default/French, 1 = English UK).
     * Default to 1 if language_id is 0 so we get a real sprite.
     * Gracefully skip on failure — falls back to text overlay.
     */
    menu->overlay_bmap = NULL;
    {
        i32 lang = (state && state->language_id > 0) ? state->language_id : 1;
        c8 ovpath[64];
        snprintf(ovpath, sizeof(ovpath), "DATAS>MENUS>MENU_%d.GIF", lang);
        bmap_t *ob = game_init_load_cursor(ovpath);
        if (ob) {
            menu->overlay_bmap = ob;
            print("menu: overlay loaded MENU_%d.CIF -> %dx%d\n",
                  lang, (i32)ob->width, ob->height);

            /* Quick pixel-density scan: log row ranges where non-zero pixels are
             * dense — these correspond to menu-button text rows in the sprite.
             * Threshold: >10% of row width must be non-zero. */
            if (ob->pixels && ob->width > 0 && ob->height > 0) {
                i32 threshold = (i32)(ob->width / 10);
                i32 in_band = 0;
                i32 band_start = 0;
                for (i32 row = 0; row < ob->height; row++) {
                    i32 nz = 0;
                    const u8 *rowp = ob->pixels + row * (i32)ob->width;
                    for (i32 col = 0; col < (i32)ob->width; col++) {
                        if (rowp[col] != 0) nz++;
                    }
                    if (nz > threshold && !in_band) {
                        in_band = 1;
                        band_start = row;
                    } else if (nz <= threshold && in_band) {
                        in_band = 0;
                        print("menu: overlay band y=%d..%d (button region)\n",
                              band_start, row - 1);
                    }
                }
                if (in_band) {
                    print("menu: overlay band y=%d..%d (button region)\n",
                          band_start, ob->height - 1);
                }
            }
        } else {
            print("menu: overlay MENU_%d.CIF load failed — using text fallback\n", lang);
        }
    }

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
    if (menu->genico_bmap) {
        if (menu->genico_bmap->owns_buffer && menu->genico_bmap->pixels)
            free(menu->genico_bmap->pixels);
        if (menu->genico_bmap->owns_palette && menu->genico_bmap->palette)
            free(menu->genico_bmap->palette);
        free(menu->genico_bmap);
        menu->genico_bmap = NULL;
    }
    /* Free overlay sprite (pass 3 refinement) */
    if (menu->overlay_bmap) {
        if (menu->overlay_bmap->owns_buffer && menu->overlay_bmap->pixels)
            free(menu->overlay_bmap->pixels);
        if (menu->overlay_bmap->owns_palette && menu->overlay_bmap->palette)
            free(menu->overlay_bmap->palette);
        free(menu->overlay_bmap);
        menu->overlay_bmap = NULL;
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

    /* ---- Pass 3 refinement: blit language-specific menu overlay sprite ----
     *
     * Mirrors: draw_image(var_4C78, game_context) in the asm main loop.
     * The overlay is blitted with index-0 transparency over the MENUGEN backdrop.
     * When the overlay is present it IS the visual menu — suppress plain text. */
    if (menu->overlay_bmap && menu->overlay_bmap->pixels) {
        const u8 *opal = menu->overlay_bmap->palette
                         ? menu->overlay_bmap->palette
                         : menu->backdrop_palette;
        if (opal) {
            blit_bmap_masked_to_framebuffer(fb, fb_w, fb_h,
                                             menu->overlay_bmap, opal,
                                             0, 0, 0 /* index 0 = transparent */);
        }
    }

    /* Text overlay — rendered only when the real sprite overlay is absent.
     * When overlay_bmap is loaded, the sprite IS the text so we skip labels.
     * Highlighted item renders in YELLOW, others in WHITE (fallback only). */
    if (!menu->overlay_bmap) {
        for (i32 i = 0; i < MAIN_MENU_ITEM_COUNT; i++) {
            const menu_item_t *item = &MAIN_MENU_ITEMS[i];
            u32 colour = (i == menu->hovered_item) ? COLOR_YELLOW : COLOR_WHITE;
            draw_text_at(font_handle, item->x, item->y, item->label, colour);
        }
    }

    /* GENICO hover icon — blit the sub-rect that corresponds to the currently
     * hovered menu button. Mirrors asm process_mainmenu_437EB7+B?? →
     * overlay_cursor_image(genico, ctx, src_x, src_y, src_x+w, src_y+h,
     *                      dst_x, dst_y). Only the hovered item is drawn.
     * No animation — GENICO is a single sprite sheet, not a frame strip. */
    /* Each CIF carries its own 768-byte palette; since we render at 32bpp we
     * don't need find_closest_palette_colors — use the sprite's own palette
     * so its authored colours land in the framebuffer verbatim.  Fall back
     * to the backdrop palette only when the sprite lacks one. */
    if (menu->hovered_item >= 0 &&
        menu->hovered_item < MAIN_MENU_ITEM_COUNT &&
        menu->genico_bmap && menu->genico_bmap->pixels) {
        const menu_item_t *hitem = &MAIN_MENU_ITEMS[menu->hovered_item];
        const u8 *pal = menu->genico_bmap->palette
                        ? menu->genico_bmap->palette
                        : menu->backdrop_palette;
        if (pal) {
            blit_bmap_subrect_masked(fb, fb_w, fb_h,
                                     menu->genico_bmap, pal,
                                     hitem->src_x, hitem->src_y,
                                     hitem->w, hitem->h,
                                     hitem->x, hitem->y,
                                     0 /* index 0 = transparent */);
        }
    }

    /* Mouse cursor — CURS128.CIF is a 65×35 sheet holding TWO cursor shapes
     * side-by-side. Asm process_mainmenu_437EB7+2351/+237E picks the crop:
     *   var_4AF0 != 0 (hovering a menu item) → src rect (0, 0, 29, 32)
     *   var_4AF0 == 0 (idle)                 → src rect (30, 0, 32, 32)
     * Previously we blit the entire sheet, so both cursors rendered as one
     * garbled sprite next to the pointer.  Blit only the current shape. */
    i32 mx = sys->motion_x;
    i32 my = sys->motion_y;
    if (menu->cursor_bmap && menu->cursor_bmap->pixels) {
        const u8 *cpal = menu->cursor_bmap->palette
                         ? menu->cursor_bmap->palette
                         : menu->backdrop_palette;
        if (cpal) {
            i32 cur_sx, cur_sw;
            if (menu->hovered_item >= 0) {
                cur_sx = 0;  cur_sw = 29;   /* "over-item" cursor */
            } else {
                cur_sx = 30; cur_sw = 32;   /* "idle" cursor */
            }
            blit_bmap_subrect_masked(fb, fb_w, fb_h,
                                     menu->cursor_bmap, cpal,
                                     cur_sx, 0, cur_sw, 32,
                                     mx, my, 0 /* index 0 = transparent */);
        }
    }

    /* Log render once per dirty cycle (hover state change gate).
     * hovered_item may be -1 when the cursor sits outside every hitbox, so
     * guard the array access — printing (idx=-1, label=<none>) is fine. */
    if (menu->dirty) {
        const c8 *lbl = (menu->hovered_item >= 0 &&
                         menu->hovered_item < MAIN_MENU_ITEM_COUNT)
                        ? MAIN_MENU_ITEMS[menu->hovered_item].label
                        : "<none>";
        print("menu: render items=%d hovered=%d (%s)\n",
              MAIN_MENU_ITEM_COUNT, menu->hovered_item, lbl);
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
