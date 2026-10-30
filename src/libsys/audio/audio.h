#ifndef LIBSYS_AUDIO_H
#define LIBSYS_AUDIO_H

#include "../system.h"
#include "../common.h"


typedef struct {
    i32 sample_rate;
    i32 channels;
    i32 bit_depth;
} audio_t;


b8   ls_init_audio(audio_t *audio, i32 sample_rate, i32 channels, i32 bit_depth);
void ls_release_audio(audio_t *audio);

/*
 * ls_audio_push — push interleaved i16 LE PCM into the streaming queue.
 * bytes = frames * channels * 2. Non-blocking; forwards directly to platform.
 * Returns bytes actually queued (0 if platform buffer is full or audio not open).
 */
i32  ls_audio_push(audio_t *audio, const i16 *pcm, i32 bytes);

/*
 * ls_audio_queued — bytes pushed but not yet played, or -1 when the
 * platform cannot tell (callers then pace by the wall clock).
 */
i32  ls_audio_queued(audio_t *audio);

/*
 * ls_audio_stop_stream — drain and stop the audio stream.
 */
void ls_audio_stop_stream(audio_t *audio);

/* -----------------------------------------------------------------------
 * Platform backend hooks — implemented in platforms/macos.m (CoreAudio),
 * platforms/linux.c (stub), platforms/windows.c (stub).
 * Called from audio.c; must NOT be called directly by game code.
 * ----------------------------------------------------------------------- */
b8   ls_platform_audio_open(i32 sample_rate, i32 channels, i32 bit_depth);
i32  ls_platform_audio_push(const i16 *pcm, i32 bytes);
i32  ls_platform_audio_queued(void);
void ls_platform_audio_close(void);

#endif // LIBSYS_AUDIO_H
