#include "audio.h"

#include "../common.h"


b8 ls_init_audio(audio_t *audio, i32 sample_rate, i32 channels, i32 bit_depth) {
    if (!audio || sample_rate <= 0 || channels <= 0 || bit_depth <= 0) {
        print("Invalid parameters for audio initialization.\n");
        return false;
    }

    audio->sample_rate = sample_rate;
    audio->channels    = channels;
    audio->bit_depth   = bit_depth;

    if (!ls_platform_audio_open(sample_rate, channels, bit_depth)) {
        print("ls_init_audio: platform audio open failed — continuing without audio.\n");
        /* non-fatal: stream functions will be no-ops */
    }

    print("Audio system initialized with sample rate: %d, channels: %d, bit depth: %d\n",
          sample_rate, channels, bit_depth);
    return true;
}

void ls_release_audio(audio_t *audio) {
    if (!audio) {
        print("Invalid audio for release.\n");
        return;
    }

    ls_platform_audio_close();

    audio->sample_rate = 0;
    audio->channels    = 0;
    audio->bit_depth   = 0;

    print("Audio system released.\n");
}

i32 ls_audio_push(audio_t *audio, const i16 *pcm, i32 bytes) {
    if (!audio || !pcm || bytes <= 0) return 0;
    return ls_platform_audio_push(pcm, bytes);
}

void ls_audio_stop_stream(audio_t *audio) {
    if (!audio) return;
    ls_platform_audio_close();
    /* Re-open so the audio_t is still valid for future pushes */
    if (audio->sample_rate > 0) {
        ls_platform_audio_open(audio->sample_rate, audio->channels, audio->bit_depth);
    }
}
