#pragma once

/*
 * gamecfg.h — DKEGAME.CFG, the player's settings, kept between sessions.
 *
 * Loaded by ls_main at startup (config_load_gamedke_cfg_45AE70) and saved
 * on exit and by the gamma picker (0x45B089). The main menu also uses its
 * absence to detect the first run (0x43837C): gamma picker, then the
 * EADKE and SC01 movies.
 */

#include "libsys/common.h"

/* Values the port has no consumer for yet are kept as read, so a file
 * written by the original game round-trips unchanged. */
typedef struct {
    i32 brightness;      /* rgb_brightness_slider_control (i8) */
    i32 contrast;        /* rgb_contrast_slider_control (i8) */
    i32 saturation;      /* rgb_saturation_slider_control (i8) */
    i32 shadows;         /* dword_4B7240: 1 silhouette, 2 low-poly, 3 full mesh, 4 auto */
    i32 render_mode;     /* dword_4B721C: index into dword_4B7228 -> render_mode */
    i32 subtitles;       /* byte_4B60AC: show dialogue subtitles */
    i32 voices;          /* byte_4B60B0: play dialogue voices */
    i32 movie_size;      /* dword_4B4770: 1..640, 640 when 0 or out of range */
} gamecfg_t;

extern gamecfg_t g_gamecfg;

b8   gamecfg_exists(void);  /* select_language_version("%sDKEGAME.CFG") */
void gamecfg_load(void);    /* 0x45AE70 */
void gamecfg_save(void);    /* 0x45B089 */
