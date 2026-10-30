#pragma once

/*
 * screens.h — the main menu's sub-screens (process_mainmenu_437EB7).
 *
 * Each one is modal, like the original: it loads its own images, runs its
 * own loop (ls_update_events, draw into state->graphic.front_buffer,
 * ls_blit + ls_flip) and returns when the player leaves it. The caller
 * redraws the menu afterwards.
 */

#include "libsys/common.h"

i32  menu_files_run(void);      /* sub_43E038: save / load; 1 = a game was loaded */
void menu_persos_run(void);     /* sub_43F8C7: characters */
void menu_sound_run(void);      /* sub_440B92: volume */
void menu_cine_run(void);       /* sub_441E43: replay seen movies */
void menu_dial_run(void);       /* sub_46A4D5: replay seen dialogues */
