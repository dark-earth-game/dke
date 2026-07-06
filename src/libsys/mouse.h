#pragma once

#include "common.h"

/*
 * Mouse subsystem
 *
 * Original: DirectInput / Win32 GetCursorPos + GetAsyncKeyState path.
 * Port: stub — platform layer (macos.m / linux.c) already delivers motion_x/y
 * and button state via system_t.actions[]. lsys_init_mouse_system only sets the
 * "mouse enabled" flag (word_55C77C = 1) in the original, so the port matches.
 *
 * TODO: native cursor clip / hide when needed (ls_set_movie_on path).
 */

/* mouse enabled flag — mirrors word_55C77C */
extern u16 g_mouse_enabled;

/* 0x00487BB0 — sets g_mouse_enabled = 1 */
void lsys_init_mouse_system(void);

/* mirrors lsys_shut_mouse_system — clears flag */
void lsys_shut_mouse_system(void);
