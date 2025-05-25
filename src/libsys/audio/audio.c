#include "audio.h"

#include "../common.h"
#include "../system.h"


b8 ls_init_audio(audio_t *audio, i32 sample_rate, i32 channels, i32 bit_depth) {
    if (!audio || sample_rate <= 0 || channels <= 0 || bit_depth <= 0) {
        print("Invalid parameters for audio initialization.\n");
        return false;
    }

    // Initialize audio system here
    // This is a placeholder for actual audio initialization logic
    print("Audio system initialized with sample rate: %d, channels: %d, bit depth: %d\n",
          sample_rate, channels, bit_depth);

    // Set the audio parameters in the system
    audio->sample_rate = sample_rate;
    audio->channels = channels;
    audio->bit_depth = bit_depth;

    // TODO init

    return true;
}

void ls_release_audio(audio_t *audio) {
    if (!audio) {
        print("Invalid syste for audio release.\n");
        return;
    }
    
    // Reset audio parameters in the system
    audio->sample_rate = 0;
    audio->channels = 0;
    audio->bit_depth = 0;
    
    // TODO release

    print("Audio system released.\n");
}
