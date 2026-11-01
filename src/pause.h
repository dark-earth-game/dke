#ifndef DKE_PAUSE_H
#define DKE_PAUSE_H

/*
 * pause.h — the P key pause screen and the game-clock compensation the engine
 * applies around every modal screen (see pause.c).
 */

#include "libsys/common.h"

/* Cheat state typed on the pause screen (display_pause_screen 0x40A2B0). */
extern i32 g_easy_mode_enabled;        /* easy_mode_enabled @ 0x4B2170: 1 = on, |2 = "oksave" */
extern i32 g_limit_override_flag;      /* limit_override_flag @ 0x4B70CC */
extern i32 g_debug_display_flag;       /* debug_display_flag @ 0x546340 */
extern i32 g_character_model_override; /* character_model_override @ 0x505D74 (write-only in the EXE) */

/* set_display_mode 0x4063FE: a modal screen opens; `ctx` pairs it with the
 * matching thaw. Nested calls keep the outermost stamp. */
void game_time_freeze(i32 ctx);
/* restore_display_mode 0x4064C3: the screen closed, shift every game timer by
 * the time it was open. `refresh_clock` = ECX (re-read current_game_time). */
void game_time_thaw(i32 refresh_clock, i32 ctx);
/* update_game_timing_state 0x4067EA: the same, but for a screen that ended
 * with a loaded game: slot and object clocks are rebased instead of shifted. */
void game_time_thaw_reload(i32 refresh_clock, i32 ctx);

/* The P key branch of process_input_command (0x407159): pause sounds, show
 * the pause screen, resume. Blocks until P is pressed again. */
void pause_game(void);

#endif /* DKE_PAUSE_H */
