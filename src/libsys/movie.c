#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "movie.h"
#include "movie_decoder.h"
#include "system.h"
#include "file_system.h"
#include "audio/audio.h"
#include "../state.h"

/* seb_* sub-allocator state (originals: _allocation_size, _number_allocations,
 * dword_508E30[]) */
#define SEB_MAX_ALLOCS  256
static void *seb_slots[SEB_MAX_ALLOCS];
static i32   seb_num_allocations = 0;
static u32   seb_allocation_size = 0;

/* movie palette / gradient tables built by ls_init_movie.
 *   byte_508B00[32 * 3]  — 32 RGB triples: (0, 2*i, 0), green ramp
 *   byte_508B60[14 * 14 * 3] — 14x14 grid of triples: (i*63/13, 0, j*63/13),
 *                              red/blue gradient LUT for movie colour mapping.
 */
u8 movie_green_ramp[32 * 3];
u8 movie_rb_grid[14 * 14 * 3];

/* movie-wide state (small subset of the original globals used by init/shut) */
b8 movie_is_ci8       = false;   /* _is_ci8            */
b8 movie_extern_sbt   = false;   /* _ls_extern_sbt     */
i32 movie_compression = 0;       /* dword_4B4628 (GetProfileString LIBSYS.COMPR) */

/* Heap slot returned by ls_get_heap(0x9EB10) — released by ls_shut_movie. */
static void *movie_heap_slot = NULL;

/* Callback slots — original stored two function pointers here. The port keeps
 * them as opaque placeholders until audio flip / decoder is wired. */
static void *movie_audio_flip_cb = NULL;

/* Big work buffers — sizes taken verbatim from asm. */
static void *movie_scratch_D6D8 = NULL;      /* dword_5089BC */
static void *movie_slice_9C40[20];           /* dword_508A50[0..19] */
static void *movie_zik_load = NULL;          /* _zik_load, size 0x15CC */

/* 0x00430A50
 * seb_get_memory(aligned, size)
 *   aligned != 0 -> reserve extra 0x10000 bytes and align result down to 64k boundary.
 *   aligned == 0 -> plain allocation.
 * Zero-fills result. Records raw base pointer in seb_slots for later release. */
static void *seb_get_memory(i32 aligned, i32 size) {
    i32 padding = aligned ? 0x10000 : 0;
    u8 *raw     = (u8 *)malloc((size_t)(size + padding));
    if (!raw) return NULL;
    /* aligned == 0 -> return raw. aligned != 0 -> round raw down to 64K
     * boundary, add 64K padding. Full-width mask on 64-bit hosts. */
    uintptr_t mask = aligned ? ~(uintptr_t)0xFFFFu : ~(uintptr_t)0u;
    u8 *aligned_ptr = (u8 *)(((uintptr_t)raw & mask) + (uintptr_t)padding);
    memset(aligned_ptr, 0, (size_t)size);
    if (seb_num_allocations < SEB_MAX_ALLOCS) {
        seb_slots[seb_num_allocations++] = raw;
    }
    seb_allocation_size += (u32)(size + padding);
    return aligned_ptr;
}

/* 00430A08 */
static void init_my_memory(void) {
    seb_allocation_size = 0;
    seb_num_allocations = 0;
}

/* 00430B00 */
static void seb_release_all(void) {
    for (i32 i = 0; i < seb_num_allocations; i++) {
        free(seb_slots[i]);
        seb_slots[i] = NULL;
    }
    seb_num_allocations = 0;
    seb_allocation_size = 0;
}

/* 00431EA0 */
void ls_init_movie(i32 a, i32 b) {
    (void)a;

    init_my_memory();
    movie_is_ci8     = false;
    movie_extern_sbt = false;

    /* Green ramp: 32 triples (0, 2*i, 0) */
    for (i32 i = 0; i < 32; i++) {
        movie_green_ramp[i * 3 + 0] = 0;
        movie_green_ramp[i * 3 + 1] = (u8)(i * 2);
        movie_green_ramp[i * 3 + 2] = 0;
    }

    /* 14x14 red/blue gradient LUT */
    for (i32 i = 0; i < 14; i++) {
        for (i32 j = 0; j < 14; j++) {
            i32 idx = (i * 14 + j) * 3;
            movie_rb_grid[idx + 0] = (u8)((i * 63) / 13);
            movie_rb_grid[idx + 1] = 0;
            movie_rb_grid[idx + 2] = (u8)((j * 63) / 13);
        }
    }

    /* Audio flip callback selection (asm: dword_508E10 = seb_traite_zik or
     * seb_traite_zik_flip). Not yet ported — record intent. */
    movie_audio_flip_cb = (void *)(uintptr_t)(b == 0 ? 1 : 2);

    movie_scratch_D6D8 = seb_get_memory(0, 0xD6D8);

    for (i32 i = 0; i < 20; i++) {
        movie_slice_9C40[i] = seb_get_memory(0, 0x9C40);
    }

    movie_zik_load = seb_get_memory(1, 0x15CC);

    /* ls_get_heap slot (0x9EB10 bytes) — decoder scratch. */
    movie_heap_slot = malloc(0x9EB10);
    if (movie_heap_slot) memset(movie_heap_slot, 0, 0x9EB10);

    /* cp_init — colour-pack / palette codec init. Not yet ported. */
    /* TODO: cp_init */

    /* Original reads win.ini [LIBSYS] COMPR=<TRUE|FALSE>. Default: FALSE -> 0. */
    movie_compression = 0;
}

/* 00432120 */
void ls_shut_movie(void) {
    /* TODO: cp_uninit */
    if (movie_heap_slot) {
        free(movie_heap_slot);
        movie_heap_slot = NULL;
    }
    seb_release_all();
    movie_scratch_D6D8 = NULL;
    for (i32 i = 0; i < 20; i++) movie_slice_9C40[i] = NULL;
    movie_zik_load = NULL;
}

/* 00432550
 * ls_play_movie — CRH decoder port (via CRH_to_MP4.py reference).
 *
 * Path resolution:
 *   - If path contains '/' or '\', use as-is (already absolute/rooted).
 *   - Otherwise prefix <state->game_path>/sc/  (Dark Earth stores cutscenes
 *     there; original used virtual "HDISK:>EARTH>sc>" prefix).
 *
 * Video: decoded frame → u32 XRGB (0xFF000000|R<<16|G<<8|B) → front_buffer.
 *   Frames smaller than display: copy top-left w×h region, rest unchanged.
 *   Frames larger than display: log and skip rendering (no crash).
 *
 * Audio: skipped (fseek past A_BYTES per frame). TODO: stream PCM.
 *
 * Frame timing: ls_tick / ls_delay to maintain movie fps.
 * Quit: ls_update_events; break on system->quit.
 *
 * Ported functions:
 *   init_decompress / read_entete / decompress_anim → crh_open + crh_next_frame
 *   cine_clear_movie_background → front_buffer zeroed after playback
 *   cp_restore_screen thread → not needed; blit+flip per frame is synchronous
 */
void ls_play_movie(const c8 *path) {
    if (!path || !path[0]) {
        printf("ls_play_movie: null/empty path\n");
        return;
    }

    /* Build resolved path. Accepts:
     *   "HDISK:>EARTH>sc>SC08.CRH" (LIBSYS virtual — original engine style)
     *   "sc/SC08.CRH"              (relative from game_path)
     *   "SC08.CRH"                 (bare name — assume in sc/)
     *   absolute host path         (pass through)
     */
    c8 resolved[512];
    const c8 *gp = (state && state->game_path) ? state->game_path : ".";
    b8 is_virtual = (strchr(path, ':') && strchr(path, '>')) != 0;
    b8 has_sep    = false;
    for (const c8 *p = path; *p; p++) {
        if (*p == '/' || *p == '\\') { has_sep = true; break; }
    }

    if (is_virtual || path[0] == '/' || has_sep) {
        if (!ls_resolve_path(gp, path, resolved, (i32)sizeof(resolved))) {
            printf("ls_play_movie: path overflow: %s\n", path);
            return;
        }
    } else {
        /* Bare filename — assume cutscene folder. */
        snprintf(resolved, sizeof(resolved), "%s/sc/%s", gp, path);
    }

    printf("ls_play_movie: opening %s\n", resolved);

    crh_reader_t r;
    if (!crh_open(&r, resolved)) {
        printf("ls_play_movie: crh_open failed for %s\n", resolved);
        return;
    }

    /* Allocate decode buffers */
    u8  *rgb_buf  = (u8 *)malloc((size_t)(r.width * r.height * 3));
    u32 *pix_buf  = (u32 *)malloc((size_t)(r.width * r.height) * sizeof(u32));

    /* Audio: raw CRH_A_BYTES block + decoded i16 PCM */
    b8   do_audio      = (state && state->audio_enabled);
    i32  ach           = (r.channels > 0 && r.channels <= 2) ? (i32)r.channels : 2;
    /* samples_per_ch = (CRH_A_BYTES - 2*ach) / ach  (seed bytes excluded) */
    i32  samples_per_ch = (CRH_A_BYTES - 2 * ach) / ach;
    /* pcm_frames = 1 (seed) + samples_per_ch; output = frames * channels i16 */
    i32  pcm_buf_samples = (1 + samples_per_ch) * ach;
    u8  *audio_raw  = do_audio ? (u8 *)malloc(CRH_A_BYTES) : NULL;
    i16 *audio_pcm  = do_audio ? (i16 *)malloc((size_t)pcm_buf_samples * sizeof(i16)) : NULL;

    if (do_audio && (!audio_raw || !audio_pcm)) {
        printf("ls_play_movie: audio buffer alloc failed — disabling audio for this movie\n");
        free(audio_raw); free(audio_pcm);
        audio_raw = NULL; audio_pcm = NULL;
        do_audio = false;
    }

    if (!rgb_buf || !pix_buf) {
        printf("ls_play_movie: out of memory (rgb=%p pix=%p)\n", (void*)rgb_buf, (void*)pix_buf);
        free(rgb_buf); free(pix_buf); free(audio_raw); free(audio_pcm);
        crh_close(&r);
        return;
    }

    system_t *sys      = state ? &state->system : NULL;
    graphic_t *graphic = state ? &state->graphic : NULL;
    audio_t   *audio   = state ? &state->audio   : NULL;
    u32 frame_ms       = (r.fps > 0) ? (1000u / r.fps) : 66u;

    /* Clear letterbox bars once at start of playback. Scale loop overwrites
     * the fitted region every frame; the black bars stay untouched. */
    if (graphic && graphic->front_buffer) {
        memset(graphic->front_buffer, 0,
               (size_t)graphic->width * graphic->height * sizeof(u32));
    }

    while (crh_next_frame_ex(&r, rgb_buf, do_audio ? audio_raw : NULL)) {
        /* Decode and push audio for this frame */
        if (do_audio && audio && audio_raw && audio_pcm) {
            i32 pcm_bytes = crh_decode_audio(audio_raw, CRH_A_BYTES, ach, audio_pcm);
            if (pcm_bytes > 0) {
                ls_audio_push(audio, audio_pcm, pcm_bytes);
            }
        }
        /* Convert RGB triples to u32 XRGB */
        i32 n_pixels = r.width * r.height;
        for (i32 i = 0; i < n_pixels; i++) {
            u8 rr = rgb_buf[i * 3 + 0];
            u8 gg = rgb_buf[i * 3 + 1];
            u8 bb = rgb_buf[i * 3 + 2];
            /* NSBitmapImageRep expects byte[0]=R on little-endian */
            pix_buf[i] = 0xFF000000u | ((u32)bb << 16) | ((u32)gg << 8) | (u32)rr;
        }

        /* Scale movie into front_buffer preserving aspect ratio.
         * Nearest-neighbour up/downscale, centred. For 320x200 in 640x480
         * this picks the largest integer factor that fits both dims (2×),
         * yielding a 640x400 image centred with 40px vertical margin. */
        if (graphic && graphic->front_buffer && r.width > 0 && r.height > 0) {
            i32 fb_w = graphic->width;
            i32 fb_h = graphic->height;

            /* Fractional fit — pick min of fb_w/w and fb_h/h in fixed 16.16. */
            i32 sx_q16 = (fb_w << 16) / r.width;
            i32 sy_q16 = (fb_h << 16) / r.height;
            i32 s_q16  = sx_q16 < sy_q16 ? sx_q16 : sy_q16;
            i32 out_w  = (r.width  * s_q16) >> 16;
            i32 out_h  = (r.height * s_q16) >> 16;
            i32 off_x  = (fb_w - out_w) / 2;
            /* Extra 15px top padding — image sits lower in the window than
             * a strict centred fit, matching the original engine's letterbox
             * layout where movies are biased slightly below centre. */
            i32 off_y  = (fb_h - out_h) / 2 + 15;
            if (off_y + out_h > fb_h) off_y = fb_h - out_h;
            if (off_y < 0)            off_y = 0;
            i32 istep  = (1 << 16) * r.width  / (out_w > 0 ? out_w : 1);
            i32 jstep  = (1 << 16) * r.height / (out_h > 0 ? out_h : 1);

            for (i32 dy = 0; dy < out_h; dy++) {
                i32 sy = (dy * jstep) >> 16;
                if (sy >= r.height) sy = r.height - 1;
                u32 *src_row = pix_buf + sy * r.width;
                u32 *dst_row = graphic->front_buffer + (off_y + dy) * fb_w + off_x;
                i32 sx_frac = 0;
                for (i32 dx = 0; dx < out_w; dx++) {
                    i32 sx = sx_frac >> 16;
                    if (sx >= r.width) sx = r.width - 1;
                    dst_row[dx] = src_row[sx];
                    sx_frac += istep;
                }
            }
        }

        /* Blit + flip */
        if (sys) {
            ls_blit(sys, graphic ? graphic->front_buffer : NULL);
            ls_flip(sys);
            ls_update_events(sys);
            if (sys->quit) break;
            /* ESC skips the movie — consume the edge so the menu doesn't
             * also see it as a QUIT. */
            if (sys->actions[27] == 1) {
                sys->actions[27] = 0;
                printf("ls_play_movie: skipped by ESC at frame %u/%u\n",
                       (u32)r.current_frame, (u32)r.frame_count);
                break;
            }
        }

        /* Frame timing */
        u32 now = ls_tick();
        static u32 frame_start = 0;
        if (frame_start == 0) frame_start = now;
        u32 elapsed = now - frame_start;
        if (elapsed < frame_ms) ls_delay(frame_ms - elapsed);
        frame_start = ls_tick();
    }

    /* cine_clear_movie_background: zero front_buffer after playback */
    if (graphic && graphic->front_buffer) {
        memset(graphic->front_buffer, 0,
               (size_t)(graphic->width * graphic->height) * sizeof(u32));
    }

    free(rgb_buf);
    free(pix_buf);
    free(audio_raw);
    free(audio_pcm);
    crh_close(&r);
    printf("ls_play_movie: done (%u frames)\n", (u32)r.current_frame);
}
