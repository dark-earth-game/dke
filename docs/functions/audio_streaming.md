# Audio Streaming Pipeline

Pipeline added in M4. Streams CRH cutscene audio via CoreAudio on macOS;
stubs on Linux/Windows.

## Pipeline: CRH frame → PCM → CoreAudio

```
crh_next_frame_ex(r, rgb_out, audio_raw)
        │
        │  audio_raw: raw CRH_A_BYTES (2942) bytes
        ▼
crh_decode_audio(audio_raw, CRH_A_BYTES, channels, audio_pcm)
        │
        │  audio_pcm: interleaved i16 LE PCM
        │  frames = 1 (seed) + (CRH_A_BYTES - 2*ch) / ch
        ▼
ls_audio_push(audio, audio_pcm, pcm_bytes)
        │
        │  forwards to platform hook
        ▼
ls_platform_audio_push(pcm, bytes)          [macos.m]
        │
        │  grabs free AudioQueueBuffer (waits ≤2 ms)
        │  memcpy → AudioQueueEnqueueBuffer
        ▼
CoreAudio AudioQueue (hardware output)
```

## CRH audio codec (crh_decode_audio)

Port of Python `decode_audio_block` in `tools/src/CRH_to_MP4.py`.

- **Seeds**: first `channels × 2` bytes of the block are i16 LE signed integers,
  one per channel. Emitted as the first output sample per channel.
- **Remainder**: bytes interleaved by channel (ch0, ch1, ch0, ch1, …).
  For each byte `b`:
  - `step  = (b & 1) ? 16 : 512`   (LSB=1 → FINE, LSB=0 → COARSE)
  - `sv    = b >> 1`
  - `delta = sign7(sv) * step`  where `sign7(v) = (v & 0x40) ? v-128 : v`
  - `prev[ch] = clamp(prev[ch] + delta, -32768, 32767)`
  - Emit `prev[ch]` as i16.
- Output is interleaved: `[ch0, ch1, ch0, ch1, …]`
- Output sample count: `channels * (1 + (CRH_A_BYTES - 2*channels) / channels)`

## CoreAudio backend (macos.m)

- `AudioQueueNewOutput` with `AudioStreamBasicDescription`:
  - Format: `kAudioFormatLinearPCM`, signed integer, packed, little-endian.
  - Filled from `crh_reader_t.sample_rate` / `.channels` / bit_depth=16.
- 4 `AudioQueueBuffer`s of 4096 bytes each; pre-filled with silence on open.
- Free-list guarded by `pthread_mutex_t` + `pthread_cond_t`.
- `aq_callback` (CoreAudio thread): returns finished buffer to free-list, signals condvar.
- `ls_platform_audio_push`: locks mutex, waits ≤2 ms for free buffer, copies PCM,
  enqueues. Returns 0 (drop) if no buffer becomes available.
- `ls_platform_audio_close`: `AudioQueueStop(sync)` + `AudioQueueDispose`.
- Linked with `-framework AudioToolbox` (Darwin block in Makefile only).

## Files

| File | Role |
|---|---|
| `src/libsys/movie_decoder.c` | `crh_decode_audio`, `crh_next_frame_ex` |
| `src/libsys/movie_decoder.h` | Prototypes + docblock |
| `src/libsys/audio/audio.c` | `ls_audio_push`, `ls_audio_stop_stream`; calls platform hooks |
| `src/libsys/audio/audio.h` | Public API + platform hook prototypes |
| `src/libsys/platforms/macos.m` | CoreAudio implementation |
| `src/libsys/platforms/linux.c` | Stub (ALSA deferred) |
| `src/libsys/platforms/windows.c` | Stub (WASAPI deferred) |
| `src/libsys/movie.c` | Wiring: `crh_next_frame_ex` + `crh_decode_audio` + `ls_audio_push` per frame |
| `src/state.c` | `state->audio_enabled = true` in `state_init` |
| `Makefile` | `-framework AudioToolbox` in Darwin `LDFLAGS` |
