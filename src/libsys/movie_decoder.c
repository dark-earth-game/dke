/*
 * movie_decoder.c — CRH (SEB) movie decoder
 *
 * C port of tools/src/CRH_to_MP4.py (Python reference by project team).
 * Semantics preserved verbatim; see movie_decoder.h for format spec.
 *
 * Audio streaming is deferred: each frame's A_BYTES block is fseek'd past.
 * TODO: wire decode_audio_block once SDL/CoreAudio streaming is available.
 */

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "movie_decoder.h"

/* -------------------------------------------------------------------------
 * Bit-reversal LUT (mirrors Python _REV)
 * _rev_byte[i] = reverse of the 8 bits of i
 * ------------------------------------------------------------------------- */
static u8 _rev_byte[256];
static b8 _rev_init = false;

static void init_rev_table(void) {
    if (_rev_init) return;
    for (i32 i = 0; i < 256; i++) {
        u8 v = (u8)i, r = 0;
        for (i32 b = 0; b < 8; b++) { r = (u8)((r << 1) | (v & 1)); v >>= 1; }
        _rev_byte[i] = r;
    }
    _rev_init = true;
}

/* rbits(v, n) — reverse n LSBs of v (Python rbits) */
static inline u32 rbits(u32 v, i32 n) {
    u32 r = 0;
    for (i32 i = 0; i < n; i++) { r = (r << 1) | (v & 1); v >>= 1; }
    return r;
}

/* five8(v) — expand 5-bit to 8-bit (Python five8) */
static inline u8 five8(u32 v) { return (u8)((v << 3) | (v >> 2)); }

/* clip5(x) — clamp to [0, 31] (Python clip5) */
static inline i32 clip5(i32 x) { return x < 0 ? 0 : (x > 31 ? 31 : x); }

/* -------------------------------------------------------------------------
 * LSB bit-reader (Python BRlsb)
 * Reads bits from LSB of each byte upward.
 * ------------------------------------------------------------------------- */
typedef struct {
    const u8 *buf;
    i32        n;    /* total bytes */
    i32        p;    /* byte position */
    i32        bp;   /* bit position within byte [0..7] */
} br_lsb_t;

static inline void brlsb_init(br_lsb_t *br, const u8 *buf, i32 n) {
    br->buf = buf; br->n = n; br->p = 0; br->bp = 0;
}

/* Returns -1 on EOF (mirrors EOFError in Python safe reads) */
static inline i32 brlsb_bit(br_lsb_t *br) {
    if (br->p >= br->n) return -1;
    i32 v = (br->buf[br->p] >> br->bp) & 1;
    if (++br->bp == 8) { br->bp = 0; br->p++; }
    return v;
}

static inline u32 brlsb_bits(br_lsb_t *br, i32 n) {
    u32 v = 0;
    for (i32 i = 0; i < n; i++) {
        i32 b = brlsb_bit(br);
        if (b < 0) break;   /* EOF — keep accumulated bits */
        v |= ((u32)b << i);
    }
    return v;
}

/* -------------------------------------------------------------------------
 * MSB bit-reader (Python BRmsb)
 * Reads bits from MSB of each byte downward.
 * Used for tile data in byte-rev mode (after byte-reversing tile data).
 * ------------------------------------------------------------------------- */
typedef struct {
    const u8 *buf;
    i32        n;
    i32        p;
    i32        bp;   /* bit position within byte [0..7], 0 = MSB side */
} br_msb_t;

static inline void brmsb_init(br_msb_t *br, const u8 *buf, i32 n) {
    br->buf = buf; br->n = n; br->p = 0; br->bp = 0;
}

static inline i32 brmsb_bit(br_msb_t *br) {
    if (br->p >= br->n) return -1;
    i32 v = (br->buf[br->p] >> (7 - br->bp)) & 1;
    if (++br->bp == 8) { br->bp = 0; br->p++; }
    return v;
}

static inline u32 brmsb_bits(br_msb_t *br, i32 n) {
    u32 v = 0;
    for (i32 i = 0; i < n; i++) {
        i32 b = brmsb_bit(br);
        if (b < 0) break;
        v = (v << 1) | (u32)b;
    }
    return v;
}

/* -------------------------------------------------------------------------
 * decode_component — port of Python dec_comp_framepred
 *
 * Decodes one compressed plane into dst (plane_size bytes).
 * prev_plane: previous frame's plane for inter back-refs (NULL on first frame).
 * data/data_len: compressed component bytes.
 *
 * Bitstream (LSB):
 *   flag=0 → 5-bit val, 8-bit len  → fill len bytes with val
 *   flag=1 → 1-bit sid, OFFSET_BITS off, 8-bit len
 *             sid=0: src=dst (intra), sid=1: src=prev_plane (inter)
 *             copy len bytes from src[off..off+len-1]
 * ------------------------------------------------------------------------- */
static void decode_component(const u8 *data, i32 data_len,
                              u8 *dst, i32 plane_size,
                              const u8 *prev_plane) {
    /* prev falls back to zeroed dst when no previous frame */
    br_lsb_t br;
    brlsb_init(&br, data, data_len);

    i32 pos = 0;
    while (pos < plane_size) {
        i32 flag = brlsb_bit(&br);
        if (flag < 0) break;   /* EOF */

        if (flag == 0) {
            /* RLE fill: 5-bit val, 8-bit len */
            u32 val = brlsb_bits(&br, 5);
            u32 ln  = brlsb_bits(&br, 8);
            i32 n   = (i32)ln;
            if (pos + n > plane_size) n = plane_size - pos;
            if (n > 0) { memset(dst + pos, (int)val, (size_t)n); pos += n; }
        } else {
            /* back-ref: 1-bit sid, OFFSET_BITS off, 8-bit len */
            u32 sid = brlsb_bits(&br, 1);
            u32 off = brlsb_bits(&br, CRH_OFFSET_BITS);
            u32 ln  = brlsb_bits(&br, 8);
            const u8 *src = (sid == 0) ? dst : prev_plane;
            i32 src_len   = plane_size; /* both dst and prev are plane_size bytes */
            for (i32 i = 0; i < (i32)ln; i++) {
                if (pos >= plane_size) break;
                i32 idx = (i32)off + i;
                u8 b = (src && idx >= 0 && idx < src_len) ? src[idx] : 0;
                dst[pos++] = b;
            }
        }
    }
}

/* -------------------------------------------------------------------------
 * reconstruct_rgb — port of Python reconstruct_rgb
 *
 * c0/c1/c2: decoded planes (plane_size bytes each)
 * tdata/tdata_len: raw tile data block
 * w, h: frame dimensions
 * tile_bits_mode: CRH_TILE_FIELD_REV or CRH_TILE_BYTE_REV
 * rgb_out: output buffer, w*h*3 bytes (R,G,B triples)
 *
 * Tile bitreader:
 *   field-rev: BRlsb, tw = rbits(8 bits, 8), th = rbits(8 bits, 8)
 *   byte-rev:  byte-reverse tdata, BRmsb, tw = 8 bits, th = 8 bits
 *
 * Per-tile fields (bits per tile, read in same mode):
 *   field-rev: m=rbits(2,2), s1=rbits(4,4)*4, s2=rbits(4,4)*4,
 *              o1=rbits(4,4)*2, o2=rbits(4,4)*2
 *   byte-rev:  m=2, s1=4*4, s2=4*4, o1=4*2, o2=4*2  (MSB reads)
 *
 * Sampler: even-odd (DEINT default)
 *   idx = 2*((y//2)*w + x) + (y & 1)
 *   result &= 31  (5-bit)
 *
 * Color from mode m:
 *   tab(step, off)[i] = i*step/32 + off - 16   (integer division)
 *   m=0: r=clip5(a+T2[c]), g=clip5(b+T1[c]), b5=c
 *   m=1: r=clip5(b+T1[c]), g=c,              b5=clip5(a+T2[c])
 *   m=2: r=c,              g=clip5(a+T2[c]), b5=clip5(b+T1[c])
 *   m=3: r=a,              g=b,              b5=c
 * Output: five8(r), five8(g), five8(b5)
 * ------------------------------------------------------------------------- */
static void reconstruct_rgb(const u8 *c0, const u8 *c1, const u8 *c2,
                             const u8 *tdata, i32 tdata_len,
                             i32 w, i32 h, i32 tile_bits_mode,
                             u8 *rgb_out) {
    /* --- set up tile bitreader and read tw, th --- */
    u32 tw, th;

    /* byte-reversed tile data buffer (only allocated if needed) */
    u8 *tdata_rev = NULL;

    /* union-style: use one of the two readers */
    br_lsb_t brl;
    br_msb_t brm;

    if (tile_bits_mode == CRH_TILE_BYTE_REV) {
        /* byte-reverse tdata into temp buffer (Python byterev) */
        tdata_rev = (u8 *)malloc((size_t)tdata_len);
        if (!tdata_rev) {
            memset(rgb_out, 0, (size_t)(w * h * 3));
            return;
        }
        for (i32 i = 0; i < tdata_len; i++) tdata_rev[i] = _rev_byte[tdata[i]];
        brmsb_init(&brm, tdata_rev, tdata_len);
        tw = brmsb_bits(&brm, 8);
        th = brmsb_bits(&brm, 8);
        if (tw == 0) tw = 20;
        if (th == 0) th = 20;
    } else {
        /* field-rev: LSB reader, bits reversed (rbits) */
        brlsb_init(&brl, tdata, tdata_len);
        tw = rbits(brlsb_bits(&brl, 8), 8);
        th = rbits(brlsb_bits(&brl, 8), 8);
        if (tw == 0) tw = 20;
        if (th == 0) th = 20;
    }

    i32 tiles_x = (i32)tw > 0 ? (w / (i32)tw) : 1;
    i32 tiles_y = (i32)th > 0 ? (h / (i32)th) : 1;
    if (tiles_x < 1) tiles_x = 1;
    if (tiles_y < 1) tiles_y = 1;

    i32 row = w * 3;
    memset(rgb_out, 0, (size_t)(h * row));

    /* --- macro helpers to read one tile's fields --- */
    /* Returns m, s1, s2, o1, o2 packed into out params */
#define READ_FIELDS_FREV(m_,s1_,s2_,o1_,o2_) do { \
    (m_)  = (i32)rbits(brlsb_bits(&brl, 2), 2); \
    (s1_) = (i32)rbits(brlsb_bits(&brl, 4), 4) * 4; \
    (s2_) = (i32)rbits(brlsb_bits(&brl, 4), 4) * 4; \
    (o1_) = (i32)rbits(brlsb_bits(&brl, 4), 4) * 2; \
    (o2_) = (i32)rbits(brlsb_bits(&brl, 4), 4) * 2; \
} while(0)

#define READ_FIELDS_BREV(m_,s1_,s2_,o1_,o2_) do { \
    (m_)  = (i32)brmsb_bits(&brm, 2); \
    (s1_) = (i32)brmsb_bits(&brm, 4) * 4; \
    (s2_) = (i32)brmsb_bits(&brm, 4) * 4; \
    (o1_) = (i32)brmsb_bits(&brm, 4) * 2; \
    (o2_) = (i32)brmsb_bits(&brm, 4) * 2; \
} while(0)

    /* Python: tab(step, off) = [int(i*step/32 + off - 16) for i in range(32)] */
    /* We compute on the fly: tab[i] = i*step/32 + off - 16 */

    i32 tile_idx = 0;
    /* first tile fields read outside loop (Python: first=fields(); it=0) */
    i32 first_m, first_s1, first_s2, first_o1, first_o2;
    if (tile_bits_mode == CRH_TILE_BYTE_REV) {
        READ_FIELDS_BREV(first_m, first_s1, first_s2, first_o1, first_o2);
    } else {
        READ_FIELDS_FREV(first_m, first_s1, first_s2, first_o1, first_o2);
    }

    for (i32 ty = 0; ty < tiles_y; ty++) {
        for (i32 tx = 0; tx < tiles_x; tx++) {
            i32 m, s1, s2, o1, o2;
            if (tile_idx == 0) {
                m = first_m; s1 = first_s1; s2 = first_s2;
                o1 = first_o1; o2 = first_o2;
            } else {
                if (tile_bits_mode == CRH_TILE_BYTE_REV) {
                    READ_FIELDS_BREV(m, s1, s2, o1, o2);
                } else {
                    READ_FIELDS_FREV(m, s1, s2, o1, o2);
                }
            }
            tile_idx++;

            i32 x0 = tx * (i32)tw;
            i32 y0 = ty * (i32)th;

            for (i32 yy = 0; yy < (i32)th; yy++) {
                i32 gy = y0 + yy;
                if (gy >= h) break;
                i32 dstrow = gy * row;

                for (i32 xx = 0; xx < (i32)tw; xx++) {
                    i32 gx = x0 + xx;
                    if (gx >= w) break;

                    /* sampler_even_odd: idx = 2*((y//2)*w + x) + (y & 1) */
                    i32 idx0 = 2 * ((gy / 2) * w + gx) + (gy & 1);

                    /* safety clamp */
                    i32 plane_size = w * h;
                    i32 a5 = (idx0 >= 0 && idx0 < plane_size) ? (c0[idx0] & 31) : 0;
                    i32 b5 = (idx0 >= 0 && idx0 < plane_size) ? (c1[idx0] & 31) : 0;
                    i32 c5 = (idx0 >= 0 && idx0 < plane_size) ? (c2[idx0] & 31) : 0;

                    /* tab(step, off)[i] = i*step/32 + off - 16 */
                    /* T1[c5], T2[c5] */
                    i32 T1_c = c5 * s1 / 32 + o1 - 16;
                    i32 T2_c = c5 * s2 / 32 + o2 - 16;

                    i32 r5, g5, bl5;
                    switch (m) {
                        case 0: r5 = clip5(a5 + T2_c); g5 = clip5(b5 + T1_c); bl5 = c5;             break;
                        case 1: r5 = clip5(b5 + T1_c); g5 = c5;               bl5 = clip5(a5 + T2_c); break;
                        case 2: r5 = c5;               g5 = clip5(a5 + T2_c); bl5 = clip5(b5 + T1_c); break;
                        default: r5 = a5;              g5 = b5;               bl5 = c5;             break;
                    }

                    i32 p = dstrow + gx * 3;
                    rgb_out[p + 0] = five8((u32)r5);
                    rgb_out[p + 1] = five8((u32)g5);
                    rgb_out[p + 2] = five8((u32)bl5);
                }
            }
        }
    }

#undef READ_FIELDS_FREV
#undef READ_FIELDS_BREV

    if (tdata_rev) free(tdata_rev);
}

/* -------------------------------------------------------------------------
 * detect_dims — port of Python detect_dims
 *
 * Reads tile data from first frame, tries field-rev then byte-rev mode.
 * Picks (w, h) that gives closest 4:3 and largest area.
 * Sets r->width, r->height, r->tile_bits_mode.
 * Falls back to 320x200 field-rev on any read error.
 * ------------------------------------------------------------------------- */
static void detect_dims(crh_reader_t *r) {
    /* defaults */
    r->width  = 320;
    r->height = 200;
    r->tile_bits_mode = CRH_TILE_FIELD_REV;

    if (!r->fp) return;

    /* seek to first frame, skip audio block, read video header */
    if (fseek(r->fp, (long)r->payload_offset, SEEK_SET) != 0) return;
    if (fseek(r->fp, CRH_A_BYTES, SEEK_CUR) != 0) return;

    u8 h16[16];
    if (fread(h16, 1, 16, r->fp) != 16) return;

    /* u32 full, c0, c1, c2 (LE) */
    u32 c0 = (u32)h16[4] | ((u32)h16[5]<<8) | ((u32)h16[6]<<16) | ((u32)h16[7]<<24);
    u32 c1 = (u32)h16[8] | ((u32)h16[9]<<8) | ((u32)h16[10]<<16) | ((u32)h16[11]<<24);
    u32 c2 = (u32)h16[12] | ((u32)h16[13]<<8) | ((u32)h16[14]<<16) | ((u32)h16[15]<<24);

    /* seek to tile data */
    long comp0_pos = ftell(r->fp);
    if (comp0_pos < 0) return;
    long tpos = comp0_pos + (long)c0 + (long)c1 + (long)c2;
    if (fseek(r->fp, tpos, SEEK_SET) != 0) return;

    u8 tsz_buf[4];
    if (fread(tsz_buf, 1, 4, r->fp) != 4) return;
    u32 tsize = (u32)tsz_buf[0] | ((u32)tsz_buf[1]<<8) | ((u32)tsz_buf[2]<<16) | ((u32)tsz_buf[3]<<24);
    if (tsize == 0 || tsize > 50000000u) return;

    u8 *tdata = (u8 *)malloc(tsize);
    if (!tdata) return;
    if (fread(tdata, 1, tsize, r->fp) != (size_t)tsize) { free(tdata); return; }

    /* try each mode: 0=field-rev, 1=byte-rev */
    for (i32 mode = 0; mode < 2; mode++) {
        u32 tw, th;

        if (mode == CRH_TILE_FIELD_REV) {
            br_lsb_t br;
            brlsb_init(&br, tdata, (i32)tsize);
            tw = rbits(brlsb_bits(&br, 8), 8);
            th = rbits(brlsb_bits(&br, 8), 8);
        } else {
            /* byte-rev */
            u8 *rev = (u8 *)malloc(tsize);
            if (!rev) continue;
            for (u32 i = 0; i < tsize; i++) rev[i] = _rev_byte[tdata[i]];
            br_msb_t br;
            brmsb_init(&br, rev, (i32)tsize);
            tw = brmsb_bits(&br, 8);
            th = brmsb_bits(&br, 8);
            free(rev);
        }

        if (tw == 0 || th == 0) continue;

        /* rem_bits = tsize*8 - 16 (header bits); each tile uses 18 bits */
        i32 rem_bits = (i32)tsize * 8 - 16;
        if (rem_bits <= 0 || rem_bits % 18 != 0) continue;
        i32 nt = rem_bits / 18;

        /* enumerate factor pairs of nt, pick best 4:3 */
        f64 best_score = 1e18;
        i32 best_w = 0, best_h = 0;

        for (i32 x = 1; (i64)x * x <= (i64)nt; x++) {
            if (nt % x != 0) continue;
            i32 pairs[2][2] = { {x, nt/x}, {nt/x, x} };
            for (i32 pi = 0; pi < 2; pi++) {
                i32 tx = pairs[pi][0], ty = pairs[pi][1];
                i32 fw = (i32)tw * tx;
                i32 fh = (i32)th * ty;
                if (fw < 32 || fw > 4096 || fh < 32 || fh > 2160) continue;
                f64 ar_diff = (f64)fw / (f64)fh - (4.0 / 3.0);
                if (ar_diff < 0) ar_diff = -ar_diff;
                /* score = (ar_diff, -w*h) — lower is better */
                f64 score = ar_diff * 1e9 - (f64)(fw * fh);
                if (best_w == 0 || score < best_score) {
                    best_score = score;
                    best_w = fw;
                    best_h = fh;
                }
            }
        }

        if (best_w > 0) {
            r->width  = best_w;
            r->height = best_h;
            r->tile_bits_mode = mode;
            free(tdata);
            return;
        }
    }

    free(tdata);
    /* fall through to defaults already set */
}

/* -------------------------------------------------------------------------
 * crh_open — port of Python parse_header + detect_dims
 * ------------------------------------------------------------------------- */
b8 crh_open(crh_reader_t *r, const c8 *path) {
    init_rev_table();
    memset(r, 0, sizeof(*r));

    r->fp = fopen(path, "rb");
    if (!r->fp) {
        printf("crh_open: cannot open %s\n", path);
        return false;
    }

    /* --- parse 52-byte header --- */
    u8 hdr[54];
    if (fread(hdr, 1, 54, r->fp) != 54) {
        printf("crh_open: short header in %s\n", path);
        fclose(r->fp); r->fp = NULL;
        return false;
    }

    r->version     = (u16)(hdr[0] | (hdr[1] << 8));
    /* audio header starts at offset 2 (48 bytes) */
    r->sample_rate = (u32)hdr[2] | ((u32)hdr[3]<<8) | ((u32)hdr[4]<<16) | ((u32)hdr[5]<<24);
    r->channels    = (u16)(hdr[16] | (hdr[17] << 8));  /* offset 2+14=16, 2+15=17 */
    r->frame_count = (u16)(hdr[50] | (hdr[51] << 8));
    r->fps         = (u16)(hdr[52] | (hdr[53] << 8));
    if (r->fps == 0) r->fps = 15;

    r->payload_offset = (i64)ftell(r->fp);   /* = 54 */
    r->current_frame  = 0;

    /* --- detect frame dimensions --- */
    detect_dims(r);   /* sets width, height, tile_bits_mode */

    r->plane_size = r->width * r->height;
    if (r->plane_size <= 0) {
        printf("crh_open: invalid dimensions %dx%d\n", r->width, r->height);
        fclose(r->fp); r->fp = NULL;
        return false;
    }

    /* --- allocate inter-prediction buffers (zeroed = first frame has no prev) --- */
    for (i32 i = 0; i < 3; i++) {
        r->prev_plane[i] = (u8 *)calloc((size_t)r->plane_size, 1);
        if (!r->prev_plane[i]) {
            printf("crh_open: out of memory for prev_plane[%d]\n", i);
            crh_close(r);
            return false;
        }
    }

    /* seek back to payload start for crh_next_frame */
    fseek(r->fp, (long)r->payload_offset, SEEK_SET);

    printf("crh_open: %s ver=%u %dx%d %ufps %u frames tile_mode=%s\n",
           path, r->version, r->width, r->height, r->fps, r->frame_count,
           r->tile_bits_mode == CRH_TILE_FIELD_REV ? "field-rev" : "byte-rev");
    return true;
}

/* -------------------------------------------------------------------------
 * crh_decode_audio — port of Python decode_audio_block
 * Returns bytes written to dst.
 * ------------------------------------------------------------------------- */
i32 crh_decode_audio(const u8 *raw, i32 raw_len, i32 channels, i16 *dst) {
    if (!raw || !dst || channels <= 0 || raw_len < 2 * channels) return 0;

    /* Read channel seed values (i16 LE) from head */
    i32 prev[2] = {0, 0};  /* supports up to 2 channels */
    i32 ch_clamp = channels > 2 ? 2 : channels;
    for (i32 ch = 0; ch < ch_clamp; ch++) {
        i32 byte_pos = ch * 2;
        i16 seed = (i16)((u16)raw[byte_pos] | ((u16)raw[byte_pos + 1] << 8));
        prev[ch] = (i32)seed;
    }

    /* Emit seed samples (interleaved) */
    i32 out_idx = 0;
    for (i32 ch = 0; ch < ch_clamp; ch++) {
        dst[out_idx++] = (i16)prev[ch];
    }

    /* Decode remaining bytes — interleaved per channel */
    i32 data_start = ch_clamp * 2;
    i32 data_len   = raw_len - data_start;
    i32 samples_per_ch = (data_len > 0) ? (data_len / ch_clamp) : 0;

    for (i32 s = 0; s < samples_per_ch; s++) {
        for (i32 ch = 0; ch < ch_clamp; ch++) {
            i32 byte_pos = data_start + s * ch_clamp + ch;
            if (byte_pos >= raw_len) break;
            u8 b = raw[byte_pos];
            /* step: LSB=1 → FINE_STEP=16, LSB=0 → COARSE_STEP=512 */
            i32 step  = (b & 1) ? 16 : 512;
            /* sign7: interpret bits [7:1] as 7-bit signed */
            i32 sv    = (i32)(b >> 1);
            i32 delta = ((sv & 0x40) ? sv - 128 : sv) * step;
            i32 val   = prev[ch] + delta;
            /* clamp to i16 range */
            if (val >  32767) val =  32767;
            if (val < -32768) val = -32768;
            prev[ch] = val;
            dst[out_idx++] = (i16)val;
        }
    }

    return out_idx * (i32)sizeof(i16);
}

/* -------------------------------------------------------------------------
 * crh_next_frame_ex — decode one frame; optionally capture raw audio block
 * ------------------------------------------------------------------------- */
b8 crh_next_frame_ex(crh_reader_t *r, u8 *rgb_out, u8 *audio_out) {
    if (!r->fp || !rgb_out) return false;
    if (r->current_frame >= r->frame_count) return false;

    /* Read or skip the audio block */
    if (audio_out) {
        if (fread(audio_out, 1, CRH_A_BYTES, r->fp) != CRH_A_BYTES) return false;
    } else {
        if (fseek(r->fp, CRH_A_BYTES, SEEK_CUR) != 0) return false;
    }

    /* Read 16-byte video header */
    u8 h16[16];
    if (fread(h16, 1, 16, r->fp) != 16) return false;

    /* u32 full, c0, c1, c2 (little-endian) — full is unused in decode */
    u32 csz[3];
    csz[0] = (u32)h16[4]  | ((u32)h16[5]<<8)  | ((u32)h16[6]<<16)  | ((u32)h16[7]<<24);
    csz[1] = (u32)h16[8]  | ((u32)h16[9]<<8)  | ((u32)h16[10]<<16) | ((u32)h16[11]<<24);
    csz[2] = (u32)h16[12] | ((u32)h16[13]<<8) | ((u32)h16[14]<<16) | ((u32)h16[15]<<24);

    /* Sanity check component sizes */
    u32 total_comp = csz[0] + csz[1] + csz[2];
    if (total_comp > 64*1024*1024u) {
        printf("crh_next_frame: absurd component sizes at frame %u\n", r->current_frame);
        return false;
    }

    /* Allocate & read compressed components */
    u8 *comp[3] = {NULL, NULL, NULL};
    b8 ok = true;

    for (i32 i = 0; i < 3 && ok; i++) {
        comp[i] = (u8 *)malloc(csz[i] ? csz[i] : 1);
        if (!comp[i]) { ok = false; break; }
        if (csz[i] > 0 && fread(comp[i], 1, csz[i], r->fp) != csz[i]) { ok = false; break; }
    }

    /* Read tile data size + bytes */
    u8 *tdata = NULL;
    u32 tsize = 0;

    if (ok) {
        u8 tsz_buf[4];
        if (fread(tsz_buf, 1, 4, r->fp) != 4) { ok = false; }
        else {
            tsize = (u32)tsz_buf[0] | ((u32)tsz_buf[1]<<8) | ((u32)tsz_buf[2]<<16) | ((u32)tsz_buf[3]<<24);
            if (tsize > 50000000u) { ok = false; }
            else {
                tdata = (u8 *)malloc(tsize ? tsize : 1);
                if (!tdata) { ok = false; }
                else if (tsize > 0 && fread(tdata, 1, tsize, r->fp) != tsize) { ok = false; }
            }
        }
    }

    if (ok) {
        /* Decode each component into tmp buffers */
        u8 *decoded[3] = {NULL, NULL, NULL};
        for (i32 i = 0; i < 3 && ok; i++) {
            decoded[i] = (u8 *)malloc((size_t)r->plane_size);
            if (!decoded[i]) { ok = false; break; }
            /* pass prev_plane (which is zeroed for frame 0) */
            decode_component(comp[i], (i32)csz[i],
                             decoded[i], r->plane_size,
                             r->prev_plane[i]);
        }

        if (ok) {
            /* reconstruct RGB */
            reconstruct_rgb(decoded[0], decoded[1], decoded[2],
                            tdata, (i32)tsize,
                            r->width, r->height, r->tile_bits_mode,
                            rgb_out);

            /* save planes as previous frame for next call */
            for (i32 i = 0; i < 3; i++) {
                memcpy(r->prev_plane[i], decoded[i], (size_t)r->plane_size);
            }
        }

        for (i32 i = 0; i < 3; i++) { if (decoded[i]) free(decoded[i]); }
    }

    for (i32 i = 0; i < 3; i++) { if (comp[i]) free(comp[i]); }
    if (tdata) free(tdata);

    if (ok) r->current_frame++;
    return ok;
}

/* -------------------------------------------------------------------------
 * crh_next_frame — wrapper: skip audio block, decode video
 * ------------------------------------------------------------------------- */
b8 crh_next_frame(crh_reader_t *r, u8 *rgb_out) {
    return crh_next_frame_ex(r, rgb_out, NULL);
}

/* -------------------------------------------------------------------------
 * crh_close
 * ------------------------------------------------------------------------- */
void crh_close(crh_reader_t *r) {
    if (!r) return;
    if (r->fp) { fclose(r->fp); r->fp = NULL; }
    for (i32 i = 0; i < 3; i++) {
        if (r->prev_plane[i]) { free(r->prev_plane[i]); r->prev_plane[i] = NULL; }
    }
    r->plane_size    = 0;
    r->current_frame = 0;
}
