#include <string.h>
#include "sound.h"
#include "common.h"

u16 g_audio_enabled = 0;

/* Active sound handle table (sub_44D1C5).
 * byte_4FB270[128] — sound state flags, dword_4FB2F0[16] — handle pointers,
 * dword_4FB370 — active count. */
static u8   g_sound_table[128]  = {0};  /* byte_4FB270 @ 0x4FB270 */
static void *g_sound_handles[16] = {0}; /* dword_4FB2F0 @ 0x4FB2F0 */
static i32  g_sound_count        = 0;   /* dword_4FB370 @ 0x4FB370 */

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

/* ls_free_sound @ 0x47FF70 — stop and release a sound handle.
 * Original: ls_stop_sound → lsys_free_sound (IDirectSoundBuffer::Release)
 * → ls_release (free memory). Decrements active sound count.
 * Stub: no real DirectSound backend — just log. */
void ls_free_sound(void *sound_handle) {
    /* ls_free_sound @ 0x47FF70 */
    if (!g_audio_enabled || !sound_handle) return;
    print("ls_free_sound: releasing handle %p (stub)\n", sound_handle);
}

/* sub_44D1C5 @ 0x44D1C5 — clear active sound table and free all handles.
 * Called during level transitions to release sounds from previous level. */
void sound_table_cleanup(void) {
    /* sub_44D1C5 @ 0x44D1C5 */
    memset(g_sound_table, 0, sizeof(g_sound_table));
    g_sound_count = 0;

    for (i32 i = 15; i >= 0; i--) {
        if (g_sound_handles[i]) {
            ls_free_sound(g_sound_handles[i]);
            g_sound_handles[i] = NULL;
        }
    }
}
