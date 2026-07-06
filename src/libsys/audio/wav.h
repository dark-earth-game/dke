#pragma once

/*
 * wav.h — RIFF WAV loader + one-shot SFX playback.
 *
 * Supports PCM fmt tag = 1 only.
 * 8-bit unsigned PCM is converted to signed i16 on load.
 * sfx_play upmixes mono → stereo and resamples 11025 → 22050 before
 * pushing to ls_audio_push (which expects 22050 Hz stereo i16 LE).
 */

#include "../common.h"

typedef struct {
    u32   sample_rate;   /* original file sample rate (e.g. 11025 or 22050) */
    u16   channels;      /* 1 = mono, 2 = stereo */
    u16   bit_depth;     /* 8 or 16 */
    u32   num_samples;   /* per channel */
    i16  *samples;       /* interleaved i16 LE (decoded from 8-bit if needed) */
} wav_t;

/*
 * wav_load — parse a RIFF/WAVE PCM file and decode into wav_t.
 * path must be a host filesystem path (not a virtual LIBSYS path).
 * 8-bit unsigned PCM is converted: s16 = ((u8)b - 128) * 256.
 * Logs "wav_load: <path> -> Nch/RateHz/BDbit/samples=X" on success.
 * Returns true on success; out->samples is heap-allocated (use wav_free).
 * Returns false and leaves *out zeroed on failure (including missing file,
 * wrong RIFF magic, non-PCM fmt, or unsupported bit depth).
 */
b8   wav_load(wav_t *out, const c8 *path);

/*
 * wav_free — release the samples buffer. Safe to call on a zeroed wav_t.
 */
void wav_free(wav_t *w);

/*
 * sfx_play — fire-and-forget one-shot PCM playback via ls_audio_push.
 * Upmixes mono → stereo by sample duplication.
 * Point-doubles (nearest-neighbour) 11025 Hz → 22050 Hz when needed.
 * If sample_rate is neither 11025 nor 22050, pushes as-is (pitch-shifted).
 * Drops silently if state->audio_enabled is false or if the audio queue
 * is full (ls_audio_push returns 0 — drop semantics; no retry).
 * Safe to call with a zeroed or partially loaded wav_t (num_samples == 0
 * → early return).
 */
void sfx_play(const wav_t *w);
