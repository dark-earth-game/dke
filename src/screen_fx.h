#ifndef DKE_SCREEN_FX_H
#define DKE_SCREEN_FX_H

#include "libsys/common.h"

/* dword_4B69BC: nothing reaches the screen while set (game logic runs). */
extern i32 g_screen_keep_black;

/* sub_42E0E0: blocking cloudy dissolve of the shown frame to black over
 * duration_ms, with the "trans" sound. */
void screen_dissolve(i32 duration_ms);

/* Present the front buffer now (blit + flip + event pump). */
void fx_present(void);

#endif /* DKE_SCREEN_FX_H */
