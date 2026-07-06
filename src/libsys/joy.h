#pragma once

#include "common.h"

/*
 * Joystick subsystem
 *
 * Original: iterates up to ~10 joystick slots (unk_558BB0 … unk_55953C, stride 0x198),
 *   calls joyGetPosEx / joyConfigChanged / joyGetDevCapsA (winmm.h) to enumerate
 *   and cache connected joysticks.
 * Port: winmm is Windows-only and joystick input is not needed for M1 targets.
 *   Stub — logs and returns immediately.
 *
 * TODO: platform joystick enumeration (SDL_GameController / IOKit / evdev)
 *   when joystick input is implemented.
 */

/* 0x004811A0 — enumerate and cache connected joysticks */
void lsys_init_joy_system(void);
