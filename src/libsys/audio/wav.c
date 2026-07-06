#include "wav.h"
#include "audio.h"

#include "../common.h"
#include "../file_system.h"
#include "../../state.h"   /* state_t, state->audio_enabled, state->audio */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Internal helpers — little-endian reads from a byte buffer
 * ------------------------------------------------------------------------- */

static u16 read_u16_le(const u8 *p) {
    return (u16)((u32)p[0] | ((u32)p[1] << 8));
}

static u32 read_u32_le(const u8 *p) {
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

/* -------------------------------------------------------------------------
 * wav_load
 * ------------------------------------------------------------------------- */

b8 wav_load(wav_t *out, const c8 *path) {
    if (!out || !path) return false;
    memset(out, 0, sizeof(*out));

    FILE *f = ls_open(path, "rb");
    if (!f) {
        print("wav_load: cannot open '%s'\n", path);
        return false;
    }

    /* Read entire file into memory for simple sequential parsing. */
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size < 44) {   /* minimum valid RIFF WAV is 44 bytes */
        print("wav_load: '%s' too small (%ld bytes)\n", path, file_size);
        ls_close(f);
        return false;
    }

    u8 *buf = (u8 *)malloc((size_t)file_size);
    if (!buf) {
        print("wav_load: malloc failed for %ld bytes\n", file_size);
        ls_close(f);
        return false;
    }

    size_t nread = fread(buf, 1, (size_t)file_size, f);
    ls_close(f);

    if ((long)nread < file_size) {
        print("wav_load: short read on '%s'\n", path);
        free(buf);
        return false;
    }

    /* --- RIFF header (12 bytes) --- */
    if (buf[0] != 'R' || buf[1] != 'I' || buf[2] != 'F' || buf[3] != 'F') {
        print("wav_load: '%s' missing RIFF magic\n", path);
        free(buf);
        return false;
    }
    if (buf[8] != 'W' || buf[9] != 'A' || buf[10] != 'V' || buf[11] != 'E') {
        print("wav_load: '%s' missing WAVE fourcc\n", path);
        free(buf);
        return false;
    }

    /* --- Scan chunks for 'fmt ' and 'data' --- */
    u16 fmt_tag     = 0;
    u16 channels    = 0;
    u32 sample_rate = 0;
    u16 bit_depth   = 0;
    b8  have_fmt    = false;

    const u8 *data_ptr  = NULL;
    u32       data_size = 0;

    i32 pos = 12;   /* start of first chunk */
    while (pos + 8 <= (i32)file_size) {
        u8  id[4];
        memcpy(id, buf + pos, 4);
        u32 chunk_size = read_u32_le(buf + pos + 4);
        i32 payload_start = pos + 8;

        if ((long)(payload_start + chunk_size) > file_size) {
            /* Clamp to avoid overread — tolerate trailing garbage */
            chunk_size = (u32)(file_size - payload_start);
        }

        if (id[0]=='f' && id[1]=='m' && id[2]=='t' && id[3]==' ') {
            /* fmt  chunk: need at least 16 bytes */
            if (chunk_size < 16) {
                print("wav_load: '%s' fmt chunk too small (%u)\n", path, chunk_size);
                free(buf);
                return false;
            }
            fmt_tag     = read_u16_le(buf + payload_start + 0);
            channels    = read_u16_le(buf + payload_start + 2);
            sample_rate = read_u32_le(buf + payload_start + 4);
            /* byte_rate = [8..11], block_align = [12..13] — not needed */
            bit_depth   = read_u16_le(buf + payload_start + 14);
            have_fmt    = true;

        } else if (id[0]=='d' && id[1]=='a' && id[2]=='t' && id[3]=='a') {
            data_ptr  = buf + payload_start;
            data_size = chunk_size;
        }

        /* Advance: chunk_size is always even (RIFF pads odd chunks) */
        pos = payload_start + (i32)((chunk_size + 1) & ~1u);
    }

    if (!have_fmt) {
        print("wav_load: '%s' no fmt chunk found\n", path);
        free(buf);
        return false;
    }
    if (!data_ptr || data_size == 0) {
        print("wav_load: '%s' no data chunk found\n", path);
        free(buf);
        return false;
    }
    if (fmt_tag != 1) {
        print("wav_load: '%s' unsupported fmt tag %u (only PCM=1 supported)\n",
              path, (u32)fmt_tag);
        free(buf);
        return false;
    }
    if (channels == 0 || channels > 2) {
        print("wav_load: '%s' unsupported channel count %u\n", path, (u32)channels);
        free(buf);
        return false;
    }
    if (bit_depth != 8 && bit_depth != 16) {
        print("wav_load: '%s' unsupported bit depth %u\n", path, (u32)bit_depth);
        free(buf);
        return false;
    }

    /* Compute number of samples (per channel). */
    u32 bytes_per_sample = (u32)channels * ((u32)bit_depth / 8u);
    u32 num_samples      = (bytes_per_sample > 0) ? (data_size / bytes_per_sample) : 0;

    if (num_samples == 0) {
        print("wav_load: '%s' empty data chunk\n", path);
        free(buf);
        return false;
    }

    /* Allocate output i16 sample buffer (always interleaved, per-channel). */
    u32 total_samples = num_samples * (u32)channels;
    i16 *samples = (i16 *)malloc(total_samples * sizeof(i16));
    if (!samples) {
        print("wav_load: malloc for samples failed (%u samples)\n", total_samples);
        free(buf);
        return false;
    }

    if (bit_depth == 16) {
        /* Copy directly — already i16 LE. */
        memcpy(samples, data_ptr, total_samples * sizeof(i16));
    } else {
        /* 8-bit unsigned PCM → signed i16: s16 = ((u8)b - 128) * 256 */
        const u8 *src = data_ptr;
        for (u32 i = 0; i < total_samples; i++) {
            samples[i] = (i16)(((i32)src[i] - 128) * 256);
        }
    }

    free(buf);

    out->sample_rate = sample_rate;
    out->channels    = channels;
    out->bit_depth   = 16;   /* always i16 in output */
    out->num_samples = num_samples;
    out->samples     = samples;

    printf("wav_load: %s -> %uch/%uHz/%ubit/samples=%u\n",
           path, (u32)channels, sample_rate, (u32)bit_depth, num_samples);

    return true;
}

/* -------------------------------------------------------------------------
 * wav_free
 * ------------------------------------------------------------------------- */

void wav_free(wav_t *w) {
    if (!w) return;
    if (w->samples) {
        free(w->samples);
        w->samples = NULL;
    }
    w->num_samples = 0;
}

/* -------------------------------------------------------------------------
 * sfx_play — upmix + resample + push to audio queue
 *
 * Target: 22050 Hz stereo i16 LE (matches CoreAudio queue opened by
 * ls_platform_audio_open in macos.m).
 * ------------------------------------------------------------------------- */

void sfx_play(const wav_t *w) {
    if (!w || !w->samples || w->num_samples == 0) return;

    /* Check global audio_enabled via the state_t extern. */
    extern state_t *state;
    if (!state || !state->audio_enabled) return;

    /* Determine resample factor: 1× (already 22050) or 2× (11025 → 22050). */
    u32 resample_factor = 1;
    if (w->sample_rate == 11025) {
        resample_factor = 2;
    } else if (w->sample_rate == 22050) {
        resample_factor = 1;
    } else {
        /* Unsupported rate — push as-is with stereo upmix; pitch will shift. */
        printf("sfx_play: warning — unsupported sample rate %u, pushing as-is\n",
               w->sample_rate);
    }

    /* Output: always stereo, resampled.
     * out_samples = num_samples * resample_factor * 2 (stereo interleaved) */
    u32 out_count = w->num_samples * resample_factor * 2u;
    i16 *out = (i16 *)malloc(out_count * sizeof(i16));
    if (!out) {
        print("sfx_play: malloc failed (%u samples)\n", out_count);
        return;
    }

    const i16 *src = w->samples;
    i16 *dst = out;

    if (w->channels == 1) {
        /* Mono → stereo + optional 2× repeat (nearest-neighbour) */
        for (u32 s = 0; s < w->num_samples; s++) {
            i16 smp = src[s];
            for (u32 r = 0; r < resample_factor; r++) {
                *dst++ = smp;   /* L */
                *dst++ = smp;   /* R */
            }
        }
    } else {
        /* Stereo + optional 2× repeat */
        for (u32 s = 0; s < w->num_samples; s++) {
            i16 l = src[s * 2 + 0];
            i16 r = src[s * 2 + 1];
            for (u32 rep = 0; rep < resample_factor; rep++) {
                *dst++ = l;
                *dst++ = r;
            }
        }
    }

    i32 bytes = (i32)(out_count * sizeof(i16));
    i32 pushed = ls_audio_push(&state->audio, out, bytes);
    if (pushed > 0) {
        print("menu: click sfx played (%u bytes pushed)\n", (u32)pushed);
    } else {
        print("sfx_play: audio queue full or disabled — drop\n");
    }

    free(out);
}
