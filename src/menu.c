#include <stdio.h>
#include <string.h>
#include <time.h>

#include "menu.h"
#include "state.h"
#include "actor.h"
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

/* sub_44CFF6 @ 0x44CFF6 — volume attenuation query.
 * Converts master volume (0..64) to DirectSound-style attenuation.
 * Returns: base_volume - (64 - master) * 32, or -10000 if muted/invalid. */
#define DSBVOLUME_MIN  (-10000)
#define VOLUME_MAX     64
static i32 g_master_volume = 64;  /* dword_4B60B8 @ 0x4B60B8 */

static i32 menu_get_volume(i32 base_volume) { /* sub_44CFF6 @ 0x44CFF6 */
    if (g_master_volume < 0 || g_master_volume > VOLUME_MAX) {
        print("Bad Volume %d\n", g_master_volume);
        return DSBVOLUME_MIN;
    }
    if (g_master_volume == 0) return DSBVOLUME_MIN;
    return base_volume - (VOLUME_MAX - g_master_volume) * 32;
}

/* sub_43F7BD @ 0x43F7BD — hitbox test from menu button table.
 * Tests mouse position against button entries. Returns 1-based index of hit
 * button, or 0 if no hit. Handles hover state toggle via indirect pointer. */
typedef struct {
    i32 src_x;       /* +0x00 — source sprite x */
    i32 src_y;       /* +0x04 — source sprite y */
    i32 width;       /* +0x08 — hitbox width */
    i32 height;      /* +0x0C — hitbox height */
    i32 dst_x;       /* +0x10 — hitbox screen x */
    i32 dst_y;       /* +0x14 — hitbox screen y */
    i32 *state_ptr;  /* +0x18 — indirect state variable */
    i32 hover_val;   /* +0x1C — value written on enter */
    i32 unhover_val; /* +0x20 — value written on leave (-1 = no action) */
    i32 is_hovered;  /* +0x24 — current hover flag */
} menu_button_entry_t;

static i32 menu_hitbox_test(i32 mx, i32 my, i32 mouse_btn,
                            menu_button_entry_t *table, i32 count) {
    /* sub_43F7BD @ 0x43F7BD */
    for (i32 i = 0; i < count; i++) {
        menu_button_entry_t *btn = &table[i];
        if (mx < btn->dst_x || mx >= btn->dst_x + btn->width) continue;
        if (my < btn->dst_y || my >= btn->dst_y + btn->height) continue;
        if (mouse_btn != 0) {
            if (!btn->is_hovered ||
                (btn->state_ptr && *btn->state_ptr != btn->hover_val)) {
                btn->is_hovered = 1;
                if (btn->state_ptr) *btn->state_ptr = btn->hover_val;
            } else if (btn->unhover_val != -1) {
                btn->is_hovered = 0;
                if (btn->state_ptr) *btn->state_ptr = btn->unhover_val;
            }
        }
        return i + 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------
 * sub_4374B0 @ 0x4374B0 — fade scan: compute per-row first-nonzero pixel offset.
 *
 * Scans a bmap (480 rows × 640 cols) to find the position of the first
 * non-zero (non-transparent) pixel in each row.  Results stored in
 * row_offsets[480].  Also patches trailing zero pixels with index 1 so the
 * curtain-reveal wipe (sub_4375DF) has solid pixel data to copy.
 *
 * Parameters (thiscall):
 *   ECX = bmap pointer (source sprite)
 *   EDX = row_offsets array (i32[480])
 *   arg_0 = out_min (i32*) — smallest offset across all rows
 *   arg_4 = out_max (i32*) — largest offset across all rows
 *
 * Log message: "Fred : Trait couleur 0 pas bon ligne %d" when offset > 640.
 * ------------------------------------------------------------------------- */
static void fade_scan_offsets(bmap_t *bmap, i32 *row_offsets,
                              i32 *out_min, i32 *out_max) {
    /* sub_4374B0 @ 0x4374B0 */
    if (!bmap || !bmap->pixels || !row_offsets || !out_min || !out_max) return;

    *out_min = 640;
    *out_max = -1;

    i32 stride = bmap->stride > 0 ? bmap->stride : (i32)bmap->width;
    i32 h = bmap->height;
    if (h > 480) h = 480;

    for (i32 row = 0; row < h; row++) {
        u8 *rowp = bmap->pixels + row * stride;

        /* Find first non-zero pixel from left */
        i32 offset = 0;
        while (offset < 640 && rowp[offset] == 0) offset++;

        row_offsets[row] = offset;

        /* Patch remaining zero pixels after the first non-zero with index 1.
         * This fills the "transparent right curtain" so the reveal wipe
         * has solid pixel data everywhere past the boundary. */
        if (offset < 640) {
            for (i32 x = offset; x < 640 && x < stride; x++) {
                if (rowp[x] == 0) rowp[x] = 1;
            }
        }

        if (row_offsets[row] > 640) {
            print("Fred : Trait couleur 0 pas bon ligne %d\n", row);
        }

        /* Track min/max */
        if (row_offsets[row] < *out_min) *out_min = row_offsets[row];
        if (row_offsets[row] > *out_max) *out_max = row_offsets[row];
    }
}

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

    printf("menu: no click sfx found under '%s/wav/'\n", game_path);
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

            /* Intro-fade fill-in (mirrors sub_4374B0 side effect at asm 76664):
             * per row, from the first index-0 pixel to the row end, replace
             * every remaining index-0 with index 1.  Asm treats this as
             * "fill the transparent right-hand curtain with palette entry 1"
             * — after the reveal animation lands on fraction=1.0, the MENU
             * sprite is fully opaque and covers MENUGEN completely.  We
             * always render the post-fade state, so we do the fill once at
             * load time and then blit the overlay unmasked.  Without this,
             * transparent regions of MENU_<lang>.CIF let MENUGEN artwork
             * (red border strip on the right) leak through the frame. */
            if (ob->pixels && ob->width > 0 && ob->height > 0) {
                i32 filled_rows = 0;
                for (i32 row = 0; row < ob->height; row++) {
                    u8 *rowp = ob->pixels + row * (i32)ob->width;
                    /* Find first index-0 pixel from the left. */
                    i32 col = 0;
                    while (col < (i32)ob->width && rowp[col] != 0) col++;
                    if (col >= (i32)ob->width) continue;   /* row is opaque */
                    /* From here to end of row, promote 0 → 1. */
                    for (; col < (i32)ob->width; col++) {
                        if (rowp[col] == 0) rowp[col] = 1;
                    }
                    filled_rows++;
                }
                print("menu: overlay fill-in complete (%d rows patched)\n",
                      filled_rows);
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
            actor_new_game();
            if (scene_load_level(state->first_room) == 0) {
                menu->state = MENU_STATE_LOADING;
                /* TODO: transition to game loop */
            } else {
                print("menu: scene_load_level failed\n");
            }
            break;

        case MENU_ACTION_LOAD:
            /* sub_4379C4 @ 0x4379C4 — load-slot picker panel.
             * Loads SAVE.GIF backdrop, shows 2-button slot selector,
             * returns slot index (1-based). Needs draw_image + overlay_cursor_image. */
            print("menu: LOAD — sub_4379C4 (slot picker needs draw_image)\n");
            menu->state = MENU_STATE_MAIN;
            break;

        case MENU_ACTION_OPTIONS:
            /* sub_438CA5 (within 0x437EB7) — options panel.
             * 8-state keyboard nav grid (brightness/contrast/shadow/volume sliders).
             * Deeply interleaved with 10K+ instruction mega-function. */
            print("menu: OPTIONS — sub_438CA5 (needs mega-function refactor)\n");
            menu->state = MENU_STATE_MAIN;
            break;

        case MENU_ACTION_SAVE:
            /* sub_4379C4 @ 0x4379C4 — save-slot picker panel (same as LOAD). */
            print("menu: SAVE — sub_4379C4 (slot picker needs draw_image)\n");
            menu->state = MENU_STATE_MAIN;
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

    /* Blit MENU_<lang>.CIF overlay opaque — mirrors asm main-loop
     * draw_image(var_4C78 → game_context) which is a mode-0 (no colour key)
     * blit.  MENU sprite covers the full 640×480 after the intro fade
     * completes; MENUGEN is only visible during the reveal transition.
     * We patched the sprite's index-0 pixels to index 1 at load time (see
     * menu_init), so a non-masked blit produces the same steady-state look
     * as the original engine's post-fade state. */
    if (menu->overlay_bmap && menu->overlay_bmap->pixels) {
        const u8 *opal = menu->overlay_bmap->palette
                         ? menu->overlay_bmap->palette
                         : menu->backdrop_palette;
        if (opal) {
            blit_bmap_to_framebuffer(fb, fb_w, fb_h,
                                     menu->overlay_bmap, opal,
                                     0, 0);
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
     *   sub_43F7BD @ 0x43F7BD  — hitbox test: pure logic, implementable
     *   sub_44CFF6 @ 0x44CFF6  — volume query: trivial arithmetic
     *   sub_4374B0 @ 0x4374B0  — fade scan: per-row first-nonzero pixel
     *   sub_4375DF @ 0x4375DF  — fade tick: curtain-reveal wipe animation
     *   sub_4378EA @ 0x4378EA  — post-fade bounce: dampened sine oscillation
     *   sub_437ADC (within 0x4379C4) — load-slot picker: SAVE.GIF + 2-button UI
     *   sub_438CA5 (within 0x437EB7) — options panel: 8-state keyboard nav
     *   Remaining: palette remap, sub-menu tables, save detection, demo guards
     */

    /* Release cursor bmaps on QUIT (state transitions away from MAIN).
     * menu_release() handles the click sfx; cursors need explicit cleanup here
     * because menu_release() doesn't have a menu_t pointer. */
    if (menu->state == MENU_STATE_QUIT) {
        menu_release_cursors(menu);
    }

    return (i32)menu->state;
}
