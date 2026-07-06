#pragma once

#include "common.h"

/*
 * CD subsystem
 *
 * Original: mciSendStringA("open cdaudio wait") + "set cdaudio time format milliseconds".
 *   Stores result in dword_55C784 (1 = CD present, 0 = absent).
 * Port: this is a patched no-CD build. Stub sets g_cd_present = 0 (no CD).
 *   Returns 0 to signal "no CD" just like the original does when mciSendStringA fails.
 *
 * TODO: if CD audio is ever needed, wire platform MCI equivalent or
 *   an audio stream fallback.
 */

/* CD present flag — mirrors dword_55C784 */
extern u32 g_cd_present;

/* 0x00487770 — open cdaudio via MCI; returns 1 if CD found, 0 if not */
i32 lsys_init_cd_system(void);
