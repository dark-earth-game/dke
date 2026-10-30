/*
 * gamecfg.c — DKEGAME.CFG (config_load_gamedke_cfg_45AE70 / 0x45B089).
 *
 * 56 bytes, little-endian, no header:
 *   +0x00 i32 brightness       +0x04 i32 contrast      +0x08 i32 saturation
 *   +0x0C i32 shadows (dword_4B7240)  +0x10 i32 dword_4B721C  +0x14 i32 music volume (dword_4B60B4)
 *   +0x18 i32 sfx volume (dword_4B60B8)                 +0x1C i32 subtitles (byte_4B60AC)
 *   +0x20 i32 reverse stereo (byte_4B60A8)              +0x24 i32 voices (byte_4B60B0)
 *   +0x28 u8[8] movies seen (byte_507350)               +0x30 i32 movie size (dword_4B4770)
 *   +0x34 i32 control scheme (dword_4B7494)
 * The loader keeps the current value of any field the file is too short for.
 */

#include <stdio.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "state.h"
#include "gamecfg.h"
#include "sfx.h"
#include "music.h"
#include "lieu_cb.h"

extern state_t *state;
extern i32 g_control_scheme;            /* dword_4B7494, combat_player.c */

gamecfg_t g_gamecfg = { .subtitles = 1, .voices = 1, .movie_size = 0x280 };

static void gamecfg_path(c8 *out, i32 cap) {
    snprintf(out, (size_t)cap, "%s/DKEGAME.CFG", state && state->game_path ? state->game_path : ".");
}

b8 gamecfg_exists(void) {
    c8 path[512];
    gamecfg_path(path, (i32)sizeof(path));
    return ls_exists(path);
}

static b8 rd(FILE *f, i32 *v) { return fread(v, 4, 1, f) == 1; }

void gamecfg_load(void) {
    c8 path[512];
    gamecfg_path(path, (i32)sizeof(path));
    FILE *f = fopen(path, "rb");
    if (!f) return;
    gamecfg_t *c = &g_gamecfg;
    i32 v;
    if (rd(f, &v)) c->brightness = (i8)v;
    if (rd(f, &v)) c->contrast   = (i8)v;
    if (rd(f, &v)) c->saturation = (i8)v;
    if (rd(f, &v)) c->shadows = v;
    if (rd(f, &v)) c->render_mode = v;
    if (rd(f, &v)) g_music_option = v;
    if (rd(f, &v)) sfx_set_master(v);
    if (rd(f, &v)) c->subtitles = (i8)v;
    if (rd(f, &v)) sfx_set_reverse_stereo((i8)v != 0);
    if (rd(f, &v)) c->voices = (i8)v;
    if (fread(g_movie_seen, 1, sizeof(g_movie_seen), f) != sizeof(g_movie_seen)) { fclose(f); return; }
    if (rd(f, &v)) c->movie_size = (v == 0 || v > 0x280) ? 0x280 : v;
    if (rd(f, &v)) g_control_scheme = v;
    fclose(f);
    print("gamecfg: loaded %s (music %d, sfx %d)\n", path, g_music_option, sfx_get_master());
}

void gamecfg_save(void) {
    c8 path[512];
    gamecfg_path(path, (i32)sizeof(path));
    FILE *f = fopen(path, "wb");
    if (!f) { print("gamecfg: cannot write %s\n", path); return; }
    const gamecfg_t *c = &g_gamecfg;
    i32 v[10] = {
        c->brightness, c->contrast, c->saturation, c->shadows, c->render_mode,
        g_music_option, sfx_get_master(), c->subtitles, sfx_get_reverse_stereo(), c->voices,
    };
    fwrite(v, 4, 10, f);
    fwrite(g_movie_seen, 1, sizeof(g_movie_seen), f);
    fwrite(&c->movie_size, 4, 1, f);
    fwrite(&g_control_scheme, 4, 1, f);
    fclose(f);
}
