/*
 * model_ome.c — .OME chunk inventory pass.
 *
 * Walks the "3DITOR v?.? TAG" signatures inside a whole-file buffer and
 * classifies each chunk by its 4/5-char tag (MATR, OBJC, ATOMF). Chunk
 * bodies are NOT decoded here — that's the next slice, blocked on
 * pointer fix-ups. Callers get a list of (kind, offset, length, name)
 * tuples plus per-kind counts.
 *
 * Sample output for bin/game/persos/P00.OME:
 *   model_ome: bin/.../P00.OME size=124071 chunks=22
 *     +0x011c4 MATR  (36221 b)  "P00.MAT"
 *     +0x09f41 OBJC  ( 5848 b)  ""
 *     +0x0b619 ATOMF ( 5080 b)  ""
 *     ... etc
 *   model_ome: P00.OME → MATR=1 OBJC=1 ATOMF=20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "model_ome.h"

/* Longest signature we recognise: "3DITOR v2.0 ATOMF" = 17 chars. */
#define OME_SIG_PREFIX  "3DITOR v"
#define OME_SIG_MIN_LEN 16
#define OME_SIG_MAX_LEN 17

/*
 * classify_tag — map a chunk tag string to its enum kind.
 */
static ome_chunk_kind_e classify_tag(const c8 *tag) {
    if (!tag) return OME_CHUNK_UNKNOWN;
    if (strncmp(tag, "MATR",  4) == 0) return OME_CHUNK_MATR;
    if (strncmp(tag, "OBJC",  4) == 0) return OME_CHUNK_OBJC;
    if (strncmp(tag, "ATOMF", 5) == 0) return OME_CHUNK_ATOMF;
    return OME_CHUNK_UNKNOWN;
}

/*
 * find_next_signature — scan raw[start..end] for the next "3DITOR v"
 * marker and return its byte offset. Returns -1 when not found.
 */
static i32 find_next_signature(const u8 *raw, i32 start, i32 end) {
    const i32 pre_len = (i32)sizeof(OME_SIG_PREFIX) - 1;
    for (i32 i = start; i + pre_len < end; i++) {
        if (raw[i] == '3' &&
            memcmp(raw + i, OME_SIG_PREFIX, (size_t)pre_len) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * read_tag_at — extract the space-delimited 4/5-char TAG right after
 * "3DITOR v?.? ". Writes up to 8 chars (incl. NUL) into out.
 */
static void read_tag_at(const u8 *raw, i32 sig_off, c8 *out, i32 out_cap) {
    /* After "3DITOR v" comes "1.3 " or "2.0 " (4 chars incl. trailing space). */
    i32 t = sig_off + 8 /*strlen("3DITOR v")*/ + 4;
    i32 j = 0;
    while (j < out_cap - 1 && t < INT32_MAX) {
        u8 c = raw[t];
        if (c < 'A' || c > 'Z') break;
        out[j++] = (c8)c;
        t++;
    }
    out[j] = '\0';
}

/*
 * read_name_at — extract the 16-byte filename field that sits at
 * sig_off + 0x20 (past sig + dashes). Truncated to out_cap. Copies
 * up to first NUL / non-printable byte.
 */
static void read_name_at(const u8 *raw, i32 sig_off, i32 file_size,
                          c8 *out, i32 out_cap) {
    i32 name_off = sig_off + 0x20;
    i32 j = 0;
    for (; j < 15 && name_off + j < file_size && j < out_cap - 1; j++) {
        u8 c = raw[name_off + j];
        if (c == 0 || c < 0x20 || c > 0x7E) break;
        out[j] = (c8)c;
    }
    out[j] = '\0';
}

i32 model_ome_open(const c8 *host_path, ome_file_t *out) {
    if (!out || !host_path) return 0;
    model_ome_free(out);

    FILE *f = ls_open(host_path, "rb");
    if (!f) {
        print("model_ome: cannot open '%s'\n", host_path);
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0 || sz > 8 * 1024 * 1024) {   /* cap at 8 MB */
        print("model_ome: '%s' size %ld out of range\n", host_path, sz);
        ls_close(f);
        return 0;
    }
    u8 *raw = (u8 *)malloc((size_t)sz);
    if (!raw) { ls_close(f); return 0; }
    if (fread(raw, 1, (size_t)sz, f) != (size_t)sz) {
        free(raw);
        ls_close(f);
        return 0;
    }
    ls_close(f);

    /* First pass: count signatures. */
    i32 file_size = (i32)sz;
    i32 count = 0;
    for (i32 pos = 0; pos < file_size; ) {
        i32 next = find_next_signature(raw, pos, file_size);
        if (next < 0) break;
        count++;
        pos = next + OME_SIG_MIN_LEN;
    }
    if (count == 0) {
        print("model_ome: no 3DITOR signatures in '%s'\n", host_path);
        free(raw);
        return 0;
    }

    ome_chunk_t *chunks = (ome_chunk_t *)calloc((size_t)count, sizeof(*chunks));
    if (!chunks) { free(raw); return 0; }

    /* Second pass: fill chunk table. */
    i32 idx = 0;
    for (i32 pos = 0; pos < file_size && idx < count; ) {
        i32 sig = find_next_signature(raw, pos, file_size);
        if (sig < 0) break;
        c8 tag[8];
        read_tag_at(raw, sig, tag, sizeof(tag));
        chunks[idx].kind   = classify_tag(tag);
        chunks[idx].offset = sig;
        read_name_at(raw, sig, file_size,
                     chunks[idx].name, (i32)sizeof(chunks[idx].name));
        idx++;
        pos = sig + OME_SIG_MIN_LEN;
    }
    /* Chunk lengths = distance to next chunk (or EOF for last one). */
    for (i32 i = 0; i < idx; i++) {
        i32 next_off = (i + 1 < idx) ? chunks[i + 1].offset : file_size;
        chunks[i].length = next_off - chunks[i].offset;
    }

    out->chunks      = chunks;
    out->chunk_count = idx;
    out->raw         = raw;
    out->raw_size    = file_size;
    out->matr_count  = 0;
    out->objc_count  = 0;
    out->atomf_count = 0;
    for (i32 i = 0; i < idx; i++) {
        switch (chunks[i].kind) {
            case OME_CHUNK_MATR:  out->matr_count++;  break;
            case OME_CHUNK_OBJC:  out->objc_count++;  break;
            case OME_CHUNK_ATOMF: out->atomf_count++; break;
            default: break;
        }
    }

    /* Report inventory. */
    print("model_ome: '%s' size=%d chunks=%d (MATR=%d OBJC=%d ATOMF=%d)\n",
          host_path, file_size, out->chunk_count,
          out->matr_count, out->objc_count, out->atomf_count);
    for (i32 i = 0; i < idx && i < 16; i++) {
        static const c8 *KIND_NAME[] = {"MATR", "OBJC", "ATOMF", "?"};
        i32 k = (i32)chunks[i].kind;
        if (k < 0 || k > 3) k = 3;
        print("model_ome:   [%02d] +0x%05x %-5s (%d b) name='%s'\n",
              i, chunks[i].offset, KIND_NAME[k],
              chunks[i].length, chunks[i].name);
    }
    if (idx > 16) {
        print("model_ome:   ... %d more chunks omitted from listing\n", idx - 16);
    }
    return 1;
}

void model_ome_free(ome_file_t *f) {
    if (!f) return;
    if (f->chunks) { free(f->chunks); f->chunks = NULL; }
    if (f->raw)    { free(f->raw);    f->raw    = NULL; }
    f->chunk_count = 0;
    f->raw_size    = 0;
    f->matr_count  = 0;
    f->objc_count  = 0;
    f->atomf_count = 0;
}
