/*
 * music.c — in-game MOD music for the dkev.exe port.
 *
 * Mirrors the engine's built-in tracker (hand-written asm, dkev.exe):
 *   sub_48D940   load: read whole file (VirtualAlloc size+0x1000), sub_48DC60
 *   sub_48DC60   -> sub_48F0B6: build volume table (sub_48EC3B), detect format
 *                (sub_48E2F8: M.K. M!K! FLT4 OCTA FLT8 xCHN xxCH), parse
 *                (sub_48E3A8 header+patterns, sub_48E4A9 samples, sub_48E516
 *                player init), then snapshot 0xC77 bytes of player state
 *                (dword_4C582B..) to the module tail
 *   sub_48DA10   play: restore the snapshot (sub_48F0F5), lsys_play_cd(vol, fade)
 *   lsys_play_cd @0x48DCF0 (misnamed): order 0 / row 0 (sub_48E030), bpm 125,
 *                speed 6, volume, start DS buffer + mixer thread; fade-in if arg
 *   sub_48DE30   stop(ms): ms==0 -> stop now; else fade to 0 over ms then stop
 *   sub_48DFB0   master volume byte_4C586A, clamped 0..64
 *   sub_48E070   is-playing (byte_4C5875)
 *   sub_48DCA0   free (stop + VirtualFree)
 *   sub_48DED0 / sub_48DF40  pause / resume on WM_ACTIVATEAPP (sub_4804D0)
 *   sub_48E2A0   mixer thread: Sleep(20); sub_48E080 (fade step + 2 ticks)
 *   sub_48E5B7   tick: row (sub_48E5E2 -> sub_48E6EB per channel) or effects
 *                (sub_48E879), each followed by the channel mixer @0x4C5500
 *                (code in .data, self-modifying, nearest-sample, no interp.)
 *   sub_48EDBC / sub_48EE01  32-bit mix -> i16 stereo, with the 3:1 stereo
 *                crossfeed (byte_4C5847 = 1, set by sub_48E280(1) in ls_main)
 *
 * Quirks kept (they change what you hear): restart order read from header
 * byte 948 (always 0 in the game files), F00 = speed 0 freezes the song on
 * that row while byte_4C5875 stays 1 (41 of the 58 game MODs end with F00,
 * so they play once and go quiet; the rest loop to order 0), A00 reuses the last slide,
 * Axy applies both nibbles (down first), volume index = vol*master/64
 * capped at 63, an instrument without a note restores the *current*
 * sample's default volume, LRRL hard pan with 3:1 crossfeed.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MUSIC_STANDALONE
#include <stdint.h>
#include <stdbool.h>
typedef char c8; typedef bool b8;
typedef uint8_t u8; typedef uint16_t u16; typedef uint32_t u32; typedef uint64_t u64;
typedef int8_t i8; typedef int16_t i16; typedef int32_t i32; typedef int64_t i64;
#else
#include "libsys/common.h"
#include "music.h"
#endif

#define MUS_RATE      22050      /* primary buffer format (ls_main: 22050/2/16) */
#define MUS_CLOCK     0x369DE4   /* 3579364: period -> Hz numerator (sub_48ECBB) */
#define MUS_MAXCH     32
#define MUS_PER_MIN   0x1C
#define MUS_PER_MAX   0xD6D
#define MUS_NOSMP     31         /* [edi] = 0x7C: empty sample slot */
#define MUS_FADE_STEP (MUS_RATE / 50)   /* thread wakes every 20 ms */

/* word_4C49B4: 16 finetune blocks x 86 periods ([0]=0xFFFF, [1..84] notes, [85]=0) */
static const u16 k_period[16 * 86] = {
    /* ft  0 */
    65535, 3424, 3232, 3048, 2880, 2712, 2560, 2416, 2280, 2152, 2032, 1920,
    1814, 1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016, 960,
    907, 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480,
    453, 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240,
    226, 214, 202, 190, 180, 169, 160, 151, 142, 134, 127, 120,
    113, 107, 101, 95, 90, 84, 80, 75, 71, 67, 63, 60,
    56, 53, 50, 47, 45, 42, 40, 37, 35, 33, 31, 30,
    28, 0,
    /* ft  1 */
    65535, 3400, 3208, 3028, 2860, 2700, 2544, 2404, 2268, 2140, 2020, 1908,
    1800, 1700, 1604, 1514, 1430, 1350, 1272, 1202, 1134, 1070, 1010, 954,
    900, 850, 802, 757, 715, 675, 636, 601, 567, 535, 505, 477,
    450, 425, 401, 378, 357, 337, 318, 300, 283, 267, 252, 238,
    225, 212, 200, 189, 178, 168, 159, 150, 141, 133, 126, 119,
    112, 106, 100, 94, 89, 84, 79, 75, 70, 66, 63, 59,
    56, 53, 50, 47, 44, 42, 39, 37, 35, 33, 31, 29,
    28, 0,
    /* ft  2 */
    65535, 3376, 3184, 3008, 2836, 2680, 2528, 2388, 2252, 2128, 2008, 1896,
    1788, 1688, 1592, 1504, 1418, 1340, 1264, 1194, 1126, 1064, 1004, 948,
    894, 844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474,
    447, 422, 398, 376, 354, 335, 316, 298, 281, 266, 251, 237,
    223, 211, 199, 188, 177, 167, 158, 149, 140, 133, 125, 118,
    111, 105, 99, 94, 88, 83, 79, 74, 70, 66, 62, 59,
    55, 52, 49, 47, 44, 41, 39, 37, 35, 33, 31, 29,
    27, 0,
    /* ft  3 */
    65535, 3352, 3164, 2984, 2816, 2660, 2512, 2368, 2236, 2112, 1992, 1880,
    1774, 1676, 1582, 1492, 1408, 1330, 1256, 1184, 1118, 1056, 996, 940,
    887, 838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470,
    443, 419, 395, 373, 352, 332, 314, 296, 279, 264, 249, 235,
    221, 209, 197, 186, 176, 166, 157, 148, 139, 132, 124, 117,
    110, 104, 98, 93, 88, 83, 78, 74, 69, 66, 62, 58,
    55, 52, 49, 46, 44, 41, 39, 37, 34, 33, 31, 29,
    27, 0,
    /* ft  4 */
    65535, 3328, 3140, 2964, 2796, 2640, 2492, 2352, 2220, 2096, 1976, 1868,
    1762, 1664, 1570, 1482, 1398, 1320, 1246, 1176, 1110, 1048, 988, 934,
    881, 832, 785, 741, 699, 660, 623, 588, 555, 524, 494, 467,
    440, 416, 392, 370, 349, 330, 311, 294, 277, 262, 247, 233,
    220, 208, 196, 185, 174, 165, 155, 147, 138, 131, 123, 116,
    110, 104, 98, 92, 87, 82, 77, 73, 69, 65, 61, 58,
    55, 52, 49, 46, 43, 41, 38, 36, 34, 32, 30, 29,
    27, 0,
    /* ft  5 */
    65535, 3304, 3116, 2944, 2776, 2620, 2476, 2336, 2204, 2080, 1964, 1852,
    1750, 1652, 1558, 1472, 1388, 1310, 1238, 1168, 1102, 1040, 982, 926,
    875, 826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463,
    437, 413, 389, 368, 347, 327, 309, 292, 275, 260, 245, 231,
    218, 206, 194, 184, 173, 163, 154, 146, 137, 130, 122, 115,
    109, 103, 97, 92, 86, 81, 77, 73, 68, 65, 61, 57,
    54, 51, 48, 46, 43, 40, 38, 36, 34, 32, 30, 28,
    27, 0,
    /* ft  6 */
    65535, 3280, 3096, 2920, 2756, 2604, 2456, 2320, 2188, 2064, 1948, 1840,
    1736, 1640, 1548, 1460, 1378, 1302, 1228, 1160, 1094, 1032, 974, 920,
    868, 820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460,
    434, 410, 387, 365, 344, 325, 307, 290, 273, 258, 243, 230,
    217, 205, 193, 182, 172, 162, 153, 145, 136, 129, 121, 115,
    108, 102, 96, 91, 86, 81, 76, 72, 68, 64, 60, 57,
    54, 51, 48, 45, 43, 40, 38, 36, 34, 32, 30, 28,
    27, 0,
    /* ft  7 */
    65535, 3256, 3072, 2900, 2736, 2584, 2440, 2300, 2172, 2052, 1936, 1828,
    1724, 1628, 1536, 1450, 1368, 1292, 1220, 1150, 1086, 1026, 968, 914,
    862, 814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457,
    431, 407, 384, 362, 342, 323, 305, 287, 271, 256, 242, 228,
    215, 203, 192, 181, 171, 161, 152, 143, 135, 128, 121, 114,
    107, 101, 96, 90, 85, 80, 76, 71, 67, 64, 60, 57,
    53, 50, 48, 45, 42, 40, 38, 35, 33, 32, 30, 28,
    26, 0,
    /* ft -8 */
    65535, 3628, 3424, 3232, 3048, 2880, 2712, 2560, 2416, 2280, 2152, 2032,
    1920, 1814, 1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016,
    960, 907, 856, 808, 762, 720, 678, 640, 604, 570, 538, 508,
    480, 453, 428, 404, 381, 360, 339, 320, 302, 285, 269, 254,
    240, 226, 214, 202, 190, 180, 169, 160, 151, 142, 134, 127,
    120, 113, 107, 101, 95, 90, 84, 80, 75, 71, 67, 63,
    60, 56, 53, 50, 47, 45, 42, 40, 37, 35, 33, 31,
    29, 0,
    /* ft -7 */
    65535, 3600, 3400, 3208, 3028, 2860, 2700, 2544, 2404, 2268, 2140, 2020,
    1908, 1800, 1700, 1604, 1514, 1430, 1350, 1272, 1202, 1134, 1070, 1010,
    954, 900, 850, 802, 757, 715, 675, 636, 601, 567, 535, 505,
    477, 450, 425, 401, 378, 357, 337, 318, 300, 283, 267, 252,
    239, 225, 212, 200, 189, 178, 168, 159, 150, 141, 133, 126,
    119, 112, 106, 100, 94, 89, 84, 79, 75, 70, 66, 63,
    59, 56, 53, 50, 47, 44, 42, 39, 37, 35, 33, 31,
    29, 0,
    /* ft -6 */
    65535, 3576, 3376, 3184, 3008, 2836, 2680, 2528, 2388, 2252, 2128, 2008,
    1896, 1788, 1688, 1592, 1504, 1418, 1340, 1264, 1194, 1126, 1064, 1004,
    948, 894, 844, 796, 752, 709, 670, 632, 597, 563, 532, 502,
    474, 447, 422, 398, 376, 354, 335, 316, 298, 281, 266, 251,
    237, 223, 211, 199, 188, 177, 167, 158, 149, 140, 133, 125,
    118, 111, 105, 99, 94, 88, 83, 79, 74, 70, 66, 62,
    59, 55, 52, 49, 47, 44, 41, 39, 37, 35, 33, 31,
    29, 0,
    /* ft -5 */
    65535, 3548, 3352, 3164, 2984, 2816, 2660, 2512, 2368, 2236, 2112, 1992,
    1880, 1774, 1676, 1582, 1492, 1408, 1330, 1256, 1184, 1118, 1056, 996,
    940, 887, 838, 791, 746, 704, 665, 628, 592, 559, 528, 498,
    470, 443, 419, 395, 373, 352, 332, 314, 296, 279, 264, 249,
    235, 221, 209, 197, 186, 176, 166, 157, 148, 139, 132, 124,
    118, 110, 104, 98, 93, 88, 83, 78, 74, 69, 66, 62,
    59, 55, 52, 49, 46, 44, 41, 39, 37, 34, 33, 31,
    29, 0,
    /* ft -4 */
    65535, 3524, 3328, 3140, 2964, 2796, 2640, 2492, 2352, 2220, 2096, 1976,
    1868, 1762, 1664, 1570, 1482, 1398, 1320, 1246, 1176, 1110, 1048, 988,
    934, 881, 832, 785, 741, 699, 660, 623, 588, 555, 524, 494,
    467, 440, 416, 392, 370, 349, 330, 311, 294, 277, 262, 247,
    233, 220, 208, 196, 185, 174, 165, 155, 147, 138, 131, 123,
    117, 110, 104, 98, 92, 87, 82, 77, 73, 69, 65, 61,
    58, 55, 52, 49, 46, 43, 41, 38, 36, 34, 32, 30,
    28, 0,
    /* ft -3 */
    65535, 3500, 3304, 3116, 2944, 2776, 2620, 2476, 2336, 2204, 2080, 1964,
    1852, 1750, 1652, 1558, 1472, 1388, 1310, 1238, 1168, 1102, 1040, 982,
    926, 875, 826, 779, 736, 694, 655, 619, 584, 551, 520, 491,
    463, 437, 413, 389, 368, 347, 327, 309, 292, 275, 260, 245,
    232, 218, 206, 194, 184, 173, 163, 154, 146, 137, 130, 122,
    116, 109, 103, 97, 92, 86, 81, 77, 73, 68, 65, 61,
    58, 54, 51, 48, 46, 43, 40, 38, 36, 34, 32, 30,
    28, 0,
    /* ft -2 */
    65535, 3472, 3280, 3096, 2920, 2756, 2604, 2456, 2320, 2188, 2064, 1948,
    1624, 1736, 1640, 1548, 1460, 1378, 1302, 1228, 1160, 1094, 1032, 974,
    812, 868, 820, 774, 730, 689, 651, 614, 580, 547, 516, 487,
    456, 434, 410, 387, 365, 344, 325, 307, 290, 273, 258, 243,
    228, 217, 205, 193, 182, 172, 162, 153, 145, 136, 129, 121,
    114, 108, 102, 96, 91, 86, 81, 76, 72, 68, 64, 60,
    57, 54, 51, 48, 45, 43, 40, 38, 36, 34, 32, 30,
    28, 0,
    /* ft -1 */
    65535, 3448, 3256, 3072, 2900, 2736, 2584, 2440, 2300, 2172, 2052, 1936,
    182, 1724, 1628, 1536, 1450, 1368, 1292, 1220, 1150, 1086, 1026, 968,
    914, 862, 814, 768, 725, 684, 646, 610, 575, 543, 513, 484,
    457, 431, 407, 384, 362, 342, 323, 305, 287, 271, 256, 242,
    228, 215, 203, 192, 181, 171, 161, 152, 143, 135, 128, 121,
    114, 107, 101, 96, 90, 85, 80, 76, 71, 67, 64, 60,
    57, 53, 50, 48, 45, 42, 40, 38, 35, 33, 32, 30,
    28, 0,
};
/* byte_4C4974: vibrato/tremolo half-sine */
static const u8 k_sine[64] = {
    0, 12, 25, 37, 49, 62, 74, 86, 97, 109, 120, 131, 142, 152, 162, 171,
    181, 189, 197, 205, 212, 219, 225, 231, 236, 241, 244, 248, 251, 253, 254, 255,
    255, 255, 254, 253, 251, 248, 244, 241, 236, 231, 225, 219, 212, 205, 197, 189,
    181, 171, 162, 152, 142, 131, 120, 109, 97, 86, 74, 62, 49, 37, 25, 0,
};
/* next bytes [255, 255, 96, 13, 160, 12, 232, 11] */

/* ========================================================================
 * State
 * ======================================================================== */
typedef struct {            /* dword_4C5882/5902/5982/5A02/5A82/5B02 [32] */
    i8 *data;
    u32 end;                /* one-shot: length; looped: loop_start + loop_len */
    u32 loop_start, loop_len;
    b8  looped;             /* loop_len > 2 */
    u8  vol;                /* raw header byte (not clamped) */
    i32 ft;                 /* byte offset into k_period: finetune * 0xAC */
} mus_smp_t;

typedef struct { u8 note, smp, eff, prm; } mus_cell_t;  /* note = byte offset */

enum { T_NONE, T_ARP, T_PUP, T_PDN, T_TPORTA, T_VIB, T_TP_VS, T_VIB_VS,
       T_TREM, T_VSLIDE, T_RETRIG, T_CUT, T_DELAY };

typedef struct {            /* unk_4C5C82[32], stride 0x41 */
    i32 smp;                /* +00 */
    u16 porta;              /* +04 */
    u8  trem;               /* +06 */
    u32 pos, frac;          /* +07 / +0B (frac = 16.16 low part << 16) */
    u8  loop_row, loop_cnt, loop_flag;   /* +0F +10 +11 */
    u16 tp_target, tp_speed;             /* +12 +14 */
    u8  vib, vibpos;                     /* +16 +17 */
    i32 ft;                 /* +18 byte offset */
    u8  tick;               /* +1A per-tick handler */
    u8  vol, vol0;          /* +1F +20 */
    u16 period;             /* +22 */
    u32 step;               /* +24 16.16 */
    u16 arp[3], arp_idx;    /* +29 +2B +2D, +2F */
    u8  vs_down, vs_up;     /* +31 +32 */
    u8  pan;                /* +33 0 = left, 0x80 = right */
    u8  rt_cnt, rt;         /* +34 +35 */
    u8  cut;                /* +36 */
    u8  delay;              /* +37 */
    mus_cell_t dcell;       /* +39 */
} mus_ch_t;

typedef struct {            /* everything the 0xC77-byte snapshot covers */
    u8  bpm, speed, count, row, pdelay, brk_row, brk;  /* 4C5878..587D, 5877 */
    u32 pos;                /* dword_4C587E */
    u8  pat;                /* byte_4C5876 */
    u32 tick_len;           /* dword_4C584A (frames) */
    mus_ch_t ch[MUS_MAXCH];
} mus_play_t;

static struct {
    b8  loaded;
    i32 nch;                /* dword_4C586F */
    u8  songlen, restart;   /* byte_4C5873, byte_4C5874 (header byte 948!) */
    u8  order[128];         /* byte_4C5B82 */
    i32 npat;
    mus_cell_t *cells;      /* npat * 64 * nch */
    mus_smp_t smp[32];
    i32 voltab[64][256];    /* sub_48EC3B */
    u32 step_base;          /* dword_4C54F8 = (MUS_CLOCK << 16) / rate */
    mus_play_t init, p;     /* snapshot + live */
    /* driver (sub_48DA10 .. sub_48E080) */
    b8  playing;            /* byte_4C5875 */
    b8  paused;             /* dword_4C47E8 */
    u8  master;             /* byte_4C586A */
    b8  fading;             /* byte_55F47C */
    i32 fade_target, fade_dur;          /* dword_55F474 / dword_55F478 */
    u32 fade_t0, clock_ms, fade_acc;    /* GetTickCount stand-in */
    i32 mix[2 * 2048];      /* dword_4C585A: one tick, stereo i32 */
    i16 out[2 * 2048];
    u32 out_len, out_pos;   /* frames */
} M = { .master = 0x40 };

/* ========================================================================
 * Helpers
 * ======================================================================== */
/* word_4C49B4 read at a byte offset (E5 stores an unscaled nibble there). */
static u16 period_at(i32 off) {
    if (off < 0 || off + 1 >= (i32)sizeof k_period) return 0;
    u32 w = (u32)off >> 1;
    if (!(off & 1)) return k_period[w];
    return (u16)((k_period[w] >> 8) | ((k_period[w + 1] & 0xFF) << 8));
}

/* sub_48E8AE: period -> byte offset of the first ft-0 entry <= period. */
static i32 note_of(u16 per) {
    if (!per) return 0;
    i32 i = 0;
    do i += 2; while (per < k_period[i >> 1] && i < 170);
    return i;
}

/* sub_48ECF4 */
static u32 per_step(u16 per, u32 keep) {
    per &= 0xFFF;
    return per ? M.step_base / per : keep;
}

/* sub_48ED08: frames per tick = rate*5 / (2*bpm), multiple of 4. */
static void set_bpm(mus_play_t *p, u8 bpm) {
    p->bpm = bpm;
    p->tick_len = bpm ? ((u32)MUS_RATE * 5 / (2u * bpm)) & ~3u : 0;
    if (p->tick_len > 2048) p->tick_len = 2048;
}

static void set_period(mus_ch_t *c, u16 per) { c->period = per; c->step = per_step(per, c->step); }

static void restart_smp(mus_ch_t *c) { c->pos = 0; c->frac = 0; }

/* ========================================================================
 * Effects
 * ======================================================================== */
static void vslide(mus_ch_t *c) {                            /* sub_48EBE0 */
    i32 v = (i32)c->vol - c->vs_down;
    if (v < 0) v = 0;
    v += c->vs_up;
    c->vol = (u8)(v > 64 ? 64 : v);
}

static void tporta(mus_ch_t *c) {                            /* sub_48EAFE */
    u32 t = c->tp_target, s = c->tp_speed, p = c->period;
    if (!t) return;
    if (p < t) { p += s; if (p > t) p = t; }
    else if (p > t) { p = (p < s) ? t : p - s; if (p < t) p = t; }
    set_period(c, (u16)p);
}

static void vibrato(mus_ch_t *c) {                           /* sub_48EB37 */
    u8 spd = (u8)((c->vib & 0xF0) >> 2), dep = c->vib & 0x0F;
    if (!spd || !dep) return;
    u8 old = c->vibpos;
    c->vibpos = (u8)(c->vibpos + spd);
    i32 d = (k_sine[(old >> 1) & 0x3F] * dep) >> 7;
    if (c->vibpos & 0x80) d = -d;
    d += c->period;
    if (d > MUS_PER_MAX) d = MUS_PER_MAX;
    if (d < MUS_PER_MIN) d = MUS_PER_MIN;
    c->step = per_step((u16)d, c->step);                      /* period kept */
}

static void tremolo(mus_ch_t *c) {                           /* sub_48EB9E */
    c->vibpos = (u8)(c->vibpos + ((c->trem & 0xF0) >> 2));    /* shares +17 */
    i32 d = (i8)(u8)(((k_sine[(c->vibpos >> 1) & 0x3F] * (c->trem & 0xF)) >> 2) >> 8);
    if (!(c->vibpos & 0x80)) d = -d;
    d += c->vol;                                              /* stored: drifts */
    c->vol = (u8)(d < 0 ? 0 : d > 64 ? 64 : d);
}

static void do_cell(mus_ch_t *c, const mus_cell_t *x, b8 delayed);

static void tick_fx(mus_ch_t *c) {                           /* [edi+1Ah] */
    switch (c->tick) {
    case T_ARP:                                              /* sub_48EA9B */
        c->arp_idx = (u16)(c->arp_idx + 2);
        if (c->arp_idx >= 6) c->arp_idx = 0;
        c->step = per_step(c->arp[c->arp_idx >> 1], c->step);
        break;
    case T_PUP: { i32 p = (i32)c->period - c->porta;         /* sub_48EABD */
        set_period(c, (u16)(p < MUS_PER_MIN ? MUS_PER_MIN : p)); break; }
    case T_PDN: { i32 p = (i32)c->period + c->porta;         /* sub_48EADD */
        set_period(c, (u16)(p > MUS_PER_MAX ? MUS_PER_MAX : p)); break; }
    case T_TPORTA: tporta(c); break;
    case T_VIB:    vibrato(c); break;
    case T_TP_VS:  vslide(c); tporta(c); break;              /* sub_48EB8D */
    case T_VIB_VS: vslide(c); vibrato(c); break;             /* sub_48EB97 */
    case T_TREM:   tremolo(c); break;
    case T_VSLIDE: vslide(c); break;
    case T_RETRIG:                                           /* sub_48EBF9 */
        if (c->rt_cnt && !--c->rt_cnt) { restart_smp(c); c->rt_cnt = c->rt; }
        break;
    case T_CUT:                                              /* sub_48EC1A */
        if (!--c->cut) { c->vol = 0; c->tick = T_NONE; }
        break;
    case T_DELAY:                                            /* sub_48EC2B */
        if (!--c->delay) { do_cell(c, &c->dcell, 1); c->tick = T_NONE; }
        break;
    }
}

static void row_e(mus_ch_t *c, u8 prm) {                     /* loc_48E859 */
    u8 x = prm & 0xF;
    mus_play_t *p = &M.p;
    c->tick = T_NONE;
    switch (prm >> 4) {
    case 0x1: { i32 v = (i32)c->period - x;                  /* sub_48EA02 */
        set_period(c, (u16)(v < MUS_PER_MIN ? MUS_PER_MIN : v)); break; }
    case 0x2: { i32 v = (i32)c->period + x;                  /* sub_48EA1E */
        set_period(c, (u16)(v > MUS_PER_MAX ? MUS_PER_MAX : v)); break; }
    case 0x5: c->ft = (i32)((i8)(x << 4) >> 4); break;       /* sub_48EA3B quirk */
    case 0x6:                                                /* sub_48EA48 */
        if (!x) c->loop_row = p->row;
        else if (!c->loop_cnt) { c->loop_flag = 1; c->loop_cnt = x; }
        else { c->loop_flag = 1; c->loop_cnt--; }
        break;
    case 0x9: if (x) c->rt = x; c->rt_cnt = x; c->tick = T_RETRIG; break;
    case 0xA: { i32 v = c->vol + x; c->vol = (u8)(v > 64 ? 64 : v); break; }
    case 0xB: { i32 v = c->vol - x; c->vol = (u8)(v < 0 ? 0 : v); break; }
    case 0xC: c->cut = (u8)(x + 1); c->tick = T_CUT; break;
    case 0xD: c->tick = T_DELAY; break;     /* only reached after the delay */
    case 0xE: p->pdelay = x; break;
    }
}

/* sub_48E6EB (delayed = entry at loc_48E725, after an EDx countdown). */
static void do_cell(mus_ch_t *c, const mus_cell_t *x, b8 delayed) {
    static const u8 k_tick[16] = { T_ARP, T_PUP, T_PDN, T_TPORTA, T_VIB, T_TP_VS,
        T_VIB_VS, T_TREM, T_NONE, T_NONE, T_VSLIDE, T_NONE, T_NONE, T_NONE, T_NONE, T_NONE };
    mus_play_t *p = &M.p;
    u8 eff = x->eff, prm = x->prm;
    i32 note = x->note, s = (i32)x->smp - 1;

    if (!delayed && eff == 0xE && (prm >> 4) == 0xD && (prm & 0xF)) {
        c->delay = prm & 0xF; c->dcell = *x; c->tick = T_DELAY;
        return;
    }
    if (eff == 3 || eff == 5) {
        if (c->period) {
            if (note) c->tp_target = period_at(note + c->ft);
            if (s >= 0 && s < 31) {
                if (s != c->smp) { c->smp = s; restart_smp(c); }
                c->ft = M.smp[s].ft; c->vol = c->vol0 = M.smp[s].vol;
            }
        }
    } else {
        b8 go = 0;
        if (s >= 0 && s < 31) {
            if (note) { c->smp = s; restart_smp(c); }
            /* no note: the *current* sample's defaults (quirk) */
            c->ft = M.smp[c->smp].ft; c->vol = c->vol0 = M.smp[c->smp].vol;
            go = 1;
        } else if (note) { restart_smp(c); go = 1; }
        if (go) {
            if (eff == 0xE && (prm >> 4) == 5) c->ft = (i32)((i8)(prm << 4) >> 4);
            if (note) set_period(c, period_at(note + c->ft));
        }
    }

    c->tick = k_tick[eff];
    switch (eff) {
    case 0x0:                                                /* sub_48E8C8 */
        if (!prm) { c->tick = T_NONE; break; }
        {
            i32 b = note_of(c->period), n1 = b + 2 * (prm >> 4), n2 = b + 2 * (prm & 0xF);
            c->arp[0] = c->period;
            c->arp[1] = period_at((n1 > 0xA6 ? 0xA6 : n1) + c->ft);
            c->arp[2] = period_at((n2 > 0xA6 ? 0xA6 : n2) + c->ft);
        }
        break;
    case 0x1: case 0x2: c->porta = prm; break;               /* sub_48E922 */
    case 0x3: if (prm) c->tp_speed = prm; break;             /* sub_48E927 */
    case 0x4:                                                /* sub_48E930 */
        if (prm) { u8 v = c->vib;
            if (prm & 0xF0) v = (u8)((v & 0x0F) | (prm & 0xF0));
            if (prm & 0x0F) v = (u8)((v & 0xF0) | (prm & 0x0F));
            c->vib = v; }
        break;
    case 0x7:                                                /* sub_48E966 */
        if (prm) { u8 v = c->trem;
            if (prm & 0xF0) v = (u8)((v & 0x0F) | (prm & 0xF0));
            if (prm & 0x0F) v = (u8)((v & 0xF0) | (prm & 0x0F));
            c->trem = v; }
        break;
    case 0xA: if (!prm) break; /* fall through: A00 keeps the old slide */
    case 0x5: case 0x6:                                      /* sub_48E957 */
        c->vs_down = prm & 0xF; c->vs_up = prm >> 4; break;
    case 0x9:                                                /* sub_48E989 */
        if (c->smp < 31) {
            u32 o = (u32)prm << 8;
            c->pos = o > M.smp[c->smp].end ? M.smp[c->smp].end : o;
        }
        break;
    case 0xB: p->pos = (u32)prm - 1; p->brk_row = 0; p->brk = 1; break;  /* sub_48E9AD */
    case 0xC: c->vol = prm > 64 ? 64 : prm; break;           /* sub_48E9C3 */
    case 0xD: p->brk_row = (u8)((prm >> 4) * 10 + (prm & 0xF)); p->brk = 1; break;
    case 0xE: row_e(c, prm); break;
    case 0xF:                                                /* sub_48E9EA */
        if (prm <= 0x1F) { p->speed = prm; p->count = prm; }  /* F00: freeze */
        else set_bpm(p, prm);
        break;
    }
}

/* ========================================================================
 * Mixer (@0x4C5500): one channel, one tick, nearest sample.
 * ======================================================================== */
static void mix_ch(mus_ch_t *c) {
    if (c->smp >= 31) return;
    const mus_smp_t *s = &M.smp[c->smp];
    if (s->end <= 2 || c->pos > s->end) return;
    u32 v = ((u32)c->vol * M.master * 4) >> 8;
    if (v > 63) v = 63;
    const i32 *tab = M.voltab[v];
    i32 *o = M.mix + (c->pan > 0x40 ? 1 : 0);
    u32 pos = c->pos, frac = c->frac, si = c->step >> 16, sf = c->step << 16;
    for (u32 f = 0; f < M.p.tick_len; f++, o += 2) {
        if (pos >= s->end) {
            if (!s->looped) break;               /* zero pad past the end */
            while (pos >= s->end) pos -= s->loop_len;
        }
        *o += tab[(u8)s->data[pos]];
        u32 nf = frac + sf;
        pos += si + (nf < frac);
        frac = nf;
    }
    c->pos = pos; c->frac = frac;
}

/* sub_48E5B7 / sub_48E5E2 / sub_48E879 */
static void player_tick(void) {
    mus_play_t *p = &M.p;
    i32 n = M.nch;
    if (--p->count == 0 && p->speed) {
        p->count = p->speed;
        for (i32 i = 0; i < n; i++) {
            mus_ch_t *c = &p->ch[i];
            if (c->loop_flag == 1 && c->loop_cnt) { c->loop_flag = 0; p->row = c->loop_row; }
        }
        if (p->pdelay) { p->pdelay--; return; }  /* tick not mixed (quirk) */
        if (p->brk || p->row > 63) {
            p->brk = 0;
            p->row = p->brk_row > 63 ? 63 : p->brk_row;   /* engine: no clamp */
            p->brk_row = 0;
            p->pos++;
            for (i32 i = 0; i < n; i++) p->ch[i].loop_row = 0;
            if ((u8)p->pos >= M.songlen) p->pos = M.restart;
            p->pat = M.order[(u8)p->pos & 0x7F];
        }
        const mus_cell_t *row = M.cells + ((size_t)p->pat * 64 + p->row) * (size_t)n;
        if (p->pat >= M.npat) row = NULL;
        for (i32 i = 0; i < n; i++) {
            static const mus_cell_t empty;
            do_cell(&p->ch[i], row ? &row[i] : &empty, 0);
            mix_ch(&p->ch[i]);
        }
        p->row++;
    } else {
        for (i32 i = 0; i < n; i++) { tick_fx(&p->ch[i]); mix_ch(&p->ch[i]); }
    }
}

/* sub_48EE01, 16-bit stereo + crossfeed path (loc_48F010). */
static void render_tick(void) {
    u32 n = M.p.tick_len;
    memset(M.mix, 0, n * 2 * sizeof(i32));
    player_tick();
    for (u32 f = 0; f < n; f++) {
        i32 l = M.mix[f * 2], r = M.mix[f * 2 + 1];
        M.out[f * 2]     = (i16)((3 * l + r) >> 2);
        M.out[f * 2 + 1] = (i16)((3 * r + l) >> 2);
    }
    M.out_len = n; M.out_pos = 0;
}

/* ========================================================================
 * Driver (sub_48DA10 .. sub_48E080)
 * ======================================================================== */
i32 g_music_option = 0x40;                    /* dword_4B60B4 */

void music_volume(i32 v) { M.master = (u8)(v < 0 ? 0 : v > 64 ? 64 : v); }   /* sub_48DFB0 */
b8   music_is_playing(void) { return M.playing; }                            /* sub_48E070 */

/* sub_48DE30 */
void music_stop(u32 fade_ms) {
    if (!M.playing) return;
    if (fade_ms) {
        M.fade_t0 = M.clock_ms; M.fade_dur = (i32)fade_ms;
        M.fade_target = 0; M.fading = 1;
        return;
    }
    M.playing = 0;
}
void music_fade(u32 ms) { music_stop(ms); }

void music_pause(void)  { if (M.playing && !M.paused) { M.playing = 0; M.paused = 1; } } /* sub_48DED0 */
void music_resume(void) { if (M.paused) { M.playing = 1; M.paused = 0; } }               /* sub_48DF40 */

/* sub_48DA10 + lsys_play_cd(vol, fade_ms) */
b8 music_play(i32 vol, u32 fade_ms) {
    if (!M.loaded || M.playing) return 0;
    M.p = M.init;                               /* sub_48F0F5 snapshot */
    M.p.row = 0; M.p.pos = 0; M.p.pat = M.order[0];   /* sub_48E030(0) */
    M.p.bpm = 0x7D; M.p.speed = 6;              /* tick_len stays snapshot's */
    M.fading = 0;
    music_volume(vol);
    if (fade_ms) {
        M.fade_t0 = M.clock_ms; M.fade_dur = (i32)fade_ms;
        M.fade_target = vol; M.fading = 1;
        music_volume(0);
    }
    M.paused = 0; M.playing = 1;
    M.out_len = M.out_pos = 0;
    return 1;
}

/* sub_48E080 head: runs every 20 ms of output. */
static void fade_step(void) {
    if (!M.fading) return;
    i32 t = (i32)(M.clock_ms - M.fade_t0);
    if (M.fade_target == 0) music_volume(M.master - (i32)((i64)t * M.master / M.fade_dur));
    else                    music_volume((i32)((i64)t * M.fade_target / M.fade_dur));
    if (t > M.fade_dur) {
        M.fading = 0;
        if (M.fade_target == 0) music_stop(0);
    }
}

/* Writes frames of i16 stereo (silence when stopped). */
void music_render(i16 *out, u32 frames) {
    while (frames) {
        if (!M.playing) { memset(out, 0, frames * 4); return; }
        if (M.out_pos >= M.out_len) render_tick();
        u32 n = M.out_len - M.out_pos;
        if (n > frames) n = frames;
        if (n > MUS_FADE_STEP - M.fade_acc) n = MUS_FADE_STEP - M.fade_acc;
        memcpy(out, M.out + M.out_pos * 2, n * 4);
        M.out_pos += n; out += n * 2; frames -= n;
        if ((M.fade_acc += n) >= MUS_FADE_STEP) {
            M.fade_acc = 0; M.clock_ms += 20;
            fade_step();
        }
    }
}

/* For sfx.c mix(): adds into the i32 accumulator before the clamp. */
void music_mix(i32 *acc, u32 frames) {
    i16 buf[2 * 1024];
    while (frames && M.playing) {
        u32 n = frames > 1024 ? 1024 : frames;
        music_render(buf, n);
        for (u32 i = 0; i < n * 2; i++) acc[i] += buf[i];
        acc += n * 2; frames -= n;
    }
}

/* ========================================================================
 * Loader (sub_48D940 / sub_48E2F8 / sub_48E3A8 / sub_48E4A9 / sub_48E516)
 * ======================================================================== */
void music_free(void) {                                      /* sub_48DCA0 */
    music_stop(0);
    free(M.cells); M.cells = NULL;
    for (i32 i = 0; i < 32; i++) { free(M.smp[i].data); }
    memset(M.smp, 0, sizeof M.smp);
    M.loaded = 0;
}

static i32 mod_channels(const u8 *t) {                       /* sub_48E2F8 */
    if (!memcmp(t, "M.K.", 4) || !memcmp(t, "M!K!", 4) ||
        !memcmp(t, "FLT4", 4) || !memcmp(t, "OCTA", 4)) return 4;
    if (!memcmp(t, "FLT8", 4)) return 8;
    if (t[1] == 'C' && t[2] == 'H' && t[3] == 'N') return t[0] - '0';
    if (t[2] == 'C' && t[3] == 'H') return (t[0] - '0') * 10 + (t[1] - '0');
    return 0;
}

b8 music_load(const c8 *path) {
    music_free();
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    u8 *d = sz > 1084 ? malloc((size_t)sz) : NULL;
    if (!d || fread(d, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(d); return 0; }
    fclose(f);

    i32 nch = mod_channels(d + 1080);
    if (nch <= 0 || nch > MUS_MAXCH) { free(d); return 0; }
    M.nch = nch;

    /* sub_48EC3B: voltab[v][s] = (s * (1024*v / ceil(nch/2))) >> 8 */
    i32 div = (nch + 1) >> 1;
    for (i32 v = 0; v < 64; v++) {
        i32 k = (i32)((u32)(4 * v) * 0x6400u / 100u / (u32)div);
        for (i32 s = 0; s < 256; s++) M.voltab[v][s] = ((i8)s * k) >> 8;
    }
    M.step_base = (u32)(((u64)MUS_CLOCK << 16) / MUS_RATE);   /* sub_48ECBB */

    /* sub_48E3A8: samples, order, patterns */
    for (i32 i = 0; i < 31; i++) {
        const u8 *h = d + 20 + 30 * i + 22;
        u32 len = ((u32)h[0] << 8 | h[1]) * 2, ls = ((u32)h[4] << 8 | h[5]) * 2,
            ll  = ((u32)h[6] << 8 | h[7]) * 2;
        if (len <= 2) continue;
        mus_smp_t *s = &M.smp[i];
        s->vol = h[3]; s->ft = (h[2] & 0xF) * 0xAC;
        s->end = len; s->loop_start = ls; s->loop_len = ll;
        s->looped = ll > 2;
        if (s->looped) s->end = ls + ll;
    }
    M.songlen = d[950];
    M.restart = d[948];             /* engine reads +0x3B4, not +0x3B7 (quirk) */
    u8 mx = d[952];
    for (i32 i = 0; i < 127; i++) { M.order[i] = d[952 + i]; if (d[952 + i] > mx) mx = d[952 + i]; }
    M.order[127] = 0;
    M.npat = mx + 1;
    size_t ncell = (size_t)M.npat * 64 * (size_t)nch;
    size_t pat_end = 1084 + ncell * 4;
    if (pat_end > (size_t)sz) { free(d); return 0; }
    M.cells = malloc(ncell * sizeof(mus_cell_t));
    for (size_t i = 0; i < ncell; i++) {
        const u8 *b = d + 1084 + i * 4;
        M.cells[i].note = (u8)note_of((u16)(((b[0] & 0xF) << 8) | b[1]));
        M.cells[i].smp  = (u8)((b[0] & 0xF0) | (b[2] >> 4));
        M.cells[i].eff  = b[2] & 0xF;
        M.cells[i].prm  = b[3];
    }
    /* sub_48E4A9 / sub_48ED3B: sample data back to back after the patterns,
     * each copied with a 32-byte pad (loop start copy or zeros). */
    size_t at = pat_end;
    for (i32 i = 0; i < 31; i++) {
        const u8 *h = d + 20 + 30 * i + 22;
        u32 len = ((u32)h[0] << 8 | h[1]) * 2;
        if (len <= 2) continue;         /* engine does not skip its bytes either */
        mus_smp_t *s = &M.smp[i];
        s->data = calloc(len + 32, 1);
        size_t avail = at < (size_t)sz ? (size_t)sz - at : 0;
        memcpy(s->data, d + at, len < avail ? len : avail);
        at += len;
        if (s->end > len) s->end = len;  /* safety (engine trusts the header) */
        if (s->looped && s->loop_len > s->end) s->looped = 0;
    }
    free(d);

    /* sub_48E516: player init -> snapshot */
    mus_play_t *p = &M.init;
    memset(p, 0, sizeof *p);
    p->speed = 6; p->count = 6;
    set_bpm(p, 0x7D);
    p->pat = M.order[0];
    for (i32 i = 0; i < nch; i++) {
        mus_ch_t *c = &p->ch[i];
        c->smp = MUS_NOSMP;
        c->pan = ((i & 3) == 1 || (i & 3) == 2) ? 0x80 : 0;   /* L R R L */
    }
    M.loaded = 1;
    return 1;
}

/* ========================================================================
 * Standalone test: cc -DMUSIC_STANDALONE -O2 music_draft.c -o mus
 *   ./mus in.MOD out.wav seconds
 * ======================================================================== */
#ifdef MUSIC_STANDALONE
static void wr32(FILE *f, u32 v) { fputc(v & 255, f); fputc(v >> 8 & 255, f); fputc(v >> 16 & 255, f); fputc(v >> 24, f); }
static void wr16(FILE *f, u32 v) { fputc(v & 255, f); fputc(v >> 8 & 255, f); }
int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "usage: %s in.mod out.wav seconds [fade_at_s]\n", argv[0]); return 1; }
    if (!music_load(argv[1])) { fprintf(stderr, "load failed\n"); return 1; }
    u32 total = (u32)(atof(argv[3]) * MUS_RATE);
    u32 fade_at = argc > 4 ? (u32)(atof(argv[4]) * MUS_RATE) : 0;
    music_play(0x40, 0);
    FILE *f = fopen(argv[2], "wb");
    fwrite("RIFF", 1, 4, f); wr32(f, 36 + total * 4); fwrite("WAVEfmt ", 1, 8, f);
    wr32(f, 16); wr16(f, 1); wr16(f, 2); wr32(f, MUS_RATE); wr32(f, MUS_RATE * 4); wr16(f, 4); wr16(f, 16);
    fwrite("data", 1, 4, f); wr32(f, total * 4);
    i16 buf[2 * 1000];
    u32 done = 0, last_row = 0xFFFF, last_pos = 0xFFFF;
    i32 peak = 0; u64 sq = 0; u32 freeze_at = 0;
    while (done < total) {
        u32 n = total - done > 1000 ? 1000 : total - done;
        if (fade_at && done <= fade_at && done + n > fade_at) music_fade(1000);
        music_render(buf, n);
        for (u32 i = 0; i < n * 2; i++) { i32 a = buf[i] < 0 ? -buf[i] : buf[i]; if (a > peak) peak = a; sq += (u64)((i64)buf[i] * buf[i]); }
        fwrite(buf, 4, n, f);
        if (M.p.pos != last_pos || M.p.row != last_row) {
            if (M.p.speed == 0 && !freeze_at) freeze_at = done;
            last_pos = M.p.pos; last_row = M.p.row;
        }
        done += n;
    }
    fclose(f);
    printf("frames %u (%.2fs) peak %d rms %.1f  speed0(frozen) at %.2fs  playing=%d master=%d\n",
           total, total / (double)MUS_RATE, peak, __builtin_sqrt((double)sq / (total * 2.0)),
           freeze_at / (double)MUS_RATE, M.playing, M.master);
    music_free();
    return 0;
}
#endif

/* ========================================================================
 * Integration notes (port)
 * ========================================================================
 * sfx.c
 *   mix():        after the voice loop, before the clamp:  music_mix(acc, frames);
 *                 (music is its own DS buffer at 0 dB in the original, so it
 *                 adds 1:1 to the voices' i16 scale)
 *   mixer_pump(): treat music as a voice:  any = any || music_is_playing();
 *   Music then streams through the same ls_audio_push FIFO at 22050 Hz.
 *
 * lieu_cb.c DIVERS op 24 (sub_446E19 @0x447F9D):
 *   if (!state->music_enabled) return 0;        // [DKE] MOD=TRUE in win.ini
 *   path = "%sDATAS/MOD/L%02dMUS%02d.MOD" (root, world_state, a)
 *   switch (b & 0xFF) {
 *   case 0: if (g_mod && !strcmp(path, g_mod_name)) break;      // already loaded
 *           if (g_mod) { music_stop(0); music_free(); }
 *           g_mod = file_exists(path) && music_load(path);      // select_language_version
 *           if (g_mod) strcpy(g_mod_name, path);                // name kept after free
 *           break;                                              // load only, no play
 *   case 1: if (g_mod && !music_is_playing() && opt_music_vol) {
 *               music_play(0x40, 0); music_volume(opt_music_vol); }
 *           break;
 *   case 2: if (g_mod) { music_free(); g_mod = 0; } break;     // falls into 3: no-op
 *   case 3: if (g_mod) music_volume(((b >> 8) * opt_music_vol) / 64); break; // C div
 *   case 4: if (g_mod) music_fade(1000); break;
 *   }
 *   then the shared default exit (def_448E78).
 *
 * Other callers of sub_48DE30(0) (hard stop, no resume):
 *   sub_444BF7 (cutscene movie player, before ls_play_movie), audio shutdown,
 *   sub_48DCA0 (free). Options menu sub_440B92: music slider dword_4B60B4
 *   0..64 (default 0x40, toggle key -> 0 / 63), applied live with
 *   music_volume() when a module is loaded; saved in game.cfg
 *   (config_load_gamedke_cfg_45AE70). WM_ACTIVATEAPP: music_pause()/resume().
 */
