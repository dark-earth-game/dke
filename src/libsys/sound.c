#include "sound.h"
#include "common.h"

u16 g_audio_enabled = 0;

/*
 * lsys_init_sound_system — inner DirectSound init (0x0048502B)
 *
 * Original flow:
 *   1. Bail if hwndCallback == NULL.
 *   2. Read LIBSYS "Sound" from win.ini; bail if "FALSE".
 *   3. DirectSoundCreate → set cooperative level → CreateSoundBuffer (primary)
 *      → Play primary → SetFormat (22050 Hz, 16-bit stereo, 2 ch).
 *   4. Return 1 on success, 0 on any DirectSound failure.
 *
 * Port: DirectSound is Windows-only. Stub logs and returns 1 (success) so the
 * rest of ls_startup proceeds. The existing ls_init_audio() path in audio.c
 * owns the actual host-audio initialisation.
 *
 * TODO: implement real audio mixing via Core Audio (macOS) / ALSA (Linux).
 */
static b8 lsys_init_sound_system(void) {
    // TODO: native audio init (Core Audio / ALSA)
    print("lsys_init_sound_system: stub — returning success\n");
    return 1;
}

// 0x0047FDF8
b8 ls_init_sound_system(void) {
    // Original:
    //   call lsys_init_sound_system
    //   mov  word_55C77A, 0       ; active sound count = 0
    //   mov  audio_enabled, eax   ; store result
    //   retn
    b8 result = lsys_init_sound_system();
    /* word_55C77A (active sound count) reset to 0 — tracked by audio subsystem */
    g_audio_enabled = result;
    print("ls_init_sound_system: audio_enabled = %d\n", (i32)g_audio_enabled);
    return result;
}

// mirrors ls_shut_sound_system
void ls_shut_sound_system(void) {
    // Original: calls lsys_shut_sound_system → IDirectSound::Release
    // TODO: release native audio backend resources
    g_audio_enabled = 0;
    print("ls_shut_sound_system: audio disabled\n");
}
