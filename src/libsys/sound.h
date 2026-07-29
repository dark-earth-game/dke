#pragma once

#include "common.h"

/*
 * Sound subsystem
 *
 * Original: DirectSound — DirectSoundCreate, IDirectSound::CreateSoundBuffer,
 *   IDirectSoundBuffer::Play (primary), IDirectSoundBuffer::SetFormat
 *   (22050 Hz, 16-bit stereo). Also reads LIBSYS "Sound" key from win.ini.
 * Port: stub — DirectSound is Windows-only. The existing ls_init_audio()
 *   (libsys/audio/audio.c) already sets up the host audio context.
 *   ls_init_sound_system() delegates to it and sets the audio_enabled flag.
 *
 * TODO: wire a real platform audio backend (Core Audio / ALSA) when
 *   sample playback is needed.
 */

/* audio enabled flag — mirrors audio_enabled in original data segment */
extern u16 g_audio_enabled;

/*
 * 0x0047FDF8 — thin wrapper: calls lsys_init_sound_system, clears
 *   word_55C77A (active sound count), stores result in audio_enabled.
 */
b8 ls_init_sound_system(void);

/* mirrors ls_shut_sound_system */
void ls_shut_sound_system(void);

/* 0x0047FF70 — free a sound handle (stop + release DirectSound buffer).
 * ecx = sound handle pointer. No-op if audio disabled or handle NULL. */
void ls_free_sound(void *sound_handle);

/* sub_44D1C5 @ 0x44D1C5 — clear active sound table and free all handles.
 * Called during level transitions (from sub_46D305 world controller load). */
void sound_table_cleanup(void);
