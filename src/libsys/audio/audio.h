#ifndef LIBSYS_AUDIO_H
#define LIBSYS_AUDIO_H

#include "../system.h"
#include "../common.h"


typedef struct {
    i32 sample_rate;
    i32 channels;
    i32 bit_depth;
} audio_t;


b8 ls_init_audio(audio_t *audio, i32 sample_rate, i32 channels, i32 bit_depth);
void ls_release_audio(audio_t *audio);

#endif // LIBSYS_AUDIO_H
