#ifndef DKE_MUSIC_H
#define DKE_MUSIC_H

/*
 * music.h — the engine's built-in ProTracker player (see music.c), mixed
 * into the sfx.c stream at 22050 Hz stereo.
 */

#include "libsys/common.h"

b8   music_load(const c8 *path);               /* sub_48D940 */
void music_free(void);                         /* sub_48DCA0 */
b8   music_play(i32 vol, u32 fade_ms);         /* sub_48DA10 */
void music_stop(u32 fade_ms);                  /* sub_48DE30: 0 = at once */
void music_fade(u32 ms);
void music_volume(i32 v);                      /* sub_48DFB0, 0..64 */
b8   music_is_playing(void);                   /* sub_48E070 */
void music_pause(void);
void music_resume(void);
void music_render(i16 *out, u32 frames);
/* Add `frames` stereo frames into a 32-bit accumulator. */
void music_mix(i32 *acc, u32 frames);

/* dword_4B60B4: the options menu music volume, 0..64. */
extern i32 g_music_option;

#endif /* DKE_MUSIC_H */
