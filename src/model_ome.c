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

static i32 read_i32_le(const u8 *p) {
    return (i32)((u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24));
}

/* ome_parse_matr — extract material colours from MATR chunk body. */
i32 ome_parse_matr(const u8 *raw, i32 raw_len, ome_matr_t *out) {
    if (!raw || !out || raw_len < 0x240 + 0x48) return 0;
    out->mat_count = 0;

    /* Records start at body offset 0x240 (past library header + linked-list
     * pointers). Each record is 72 bytes (0x48). */
    const i32 REC_OFF  = 0x240;
    const i32 REC_SIZE = 0x48;
    const i32 NAME_OFF = 0x14;  /* null-terminated name within record */

    for (i32 i = 0; i < OME_MATR_MAX_MATS; i++) {
        i32 rec = REC_OFF + i * REC_SIZE;
        if (rec + REC_SIZE > raw_len) break;

        i32 r = read_i32_le(raw + rec +  0) & 0xFF;
        i32 g = read_i32_le(raw + rec +  4) & 0xFF;
        i32 b = read_i32_le(raw + rec +  8) & 0xFF;

        /* Copy name (at most 11 chars to leave room for NUL). */
        const u8 *nptr = raw + rec + NAME_OFF;
        i32 nlen = 0;
        while (nlen < 11 && rec + NAME_OFF + nlen < raw_len &&
               nptr[nlen] >= 0x20 && nptr[nlen] < 0x7F) {
            out->mats[i].name[nlen] = (c8)nptr[nlen];
            nlen++;
        }
        out->mats[i].name[nlen] = '\0';

        /* Stop at first empty, all-zero record. */
        if (r == 0 && g == 0 && b == 0 && nlen == 0) break;

        out->mats[i].r = (u8)r;
        out->mats[i].g = (u8)g;
        out->mats[i].b = (u8)b;
        out->mat_count++;
    }

    if (out->mat_count > 0) {
        print("ome_parse_matr: %d materials parsed\n", out->mat_count);
    }
    return out->mat_count > 0 ? 1 : 0;
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

/* -------------------------------------------------------------------------
 * ATOMF body decoder
 * See model_ome.h for the empirical layout notes.
 * ------------------------------------------------------------------------- */

i32 ome_decode_atomf(const u8 *raw, i32 raw_len, ome_atom_t *out) {
    if (!raw || !out || raw_len < 16) return 0;
    out->verts      = NULL;
    out->vert_count = 0;
    out->faces      = NULL;
    out->face_count = 0;

    /* Skip 16-byte body header. */
    i32 pos = 16;

    /* Count vertex records: 32 bytes each, field[7] == 0 or 0x00200000. */
    i32 n_verts = 0;
    while (pos + 32 <= raw_len) {
        u32 flag = (u32)read_i32_le(raw + pos + 28);
        if (flag != 0 && flag != 0x00200000u) break;
        n_verts++;
        pos += 32;
    }

    i32 face_section_start = pos;

    /* Count faces with a greedy scan (mirrors tools/src/dump_ome.py).
     *
     * Layout: face 0 starts directly at face_section_start (no preceding ptr).
     * Faces 1+ are each preceded by a 4-byte heap ptr (ignored).
     * Stop when there is not enough space or the face type / vcnt is invalid.
     *
     * This correctly handles varying tail-byte counts (0, 4, or 8 trailing
     * bytes) across different .OME files without requiring exact divisibility. */
    i32 n_faces = 0;
    {
        i32 scan = face_section_start;
        i32 first_face = 1;
        while (scan + 96 <= raw_len) {
            if (!first_face) {
                if (scan + 4 + 96 > raw_len) break;
                scan += 4; /* skip 4-byte heap ptr before face 1+ */
            }
            first_face = 0;
            i32 ftype = read_i32_le(raw + scan);
            i32 vcnt  = read_i32_le(raw + scan + 4);
            if (ftype != 100 || vcnt < 3 || vcnt > 4) break;
            n_faces++;
            scan += 96;
        }
    }

    if (n_verts == 0 && n_faces == 0) {
        print("ome_decode_atomf: empty atom (raw_len=%d)\n", raw_len);
        return 1; /* success — caller may check counts */
    }

    /* Allocate. */
    if (n_verts > 0) {
        out->verts = (ome_vert_t *)malloc((size_t)n_verts * sizeof(ome_vert_t));
        if (!out->verts) { print("ome_decode_atomf: vert alloc failed\n"); return 0; }
    }
    if (n_faces > 0) {
        out->faces = (ome_face_t *)malloc((size_t)n_faces * sizeof(ome_face_t));
        if (!out->faces) {
            free(out->verts); out->verts = NULL;
            print("ome_decode_atomf: face alloc failed\n");
            return 0;
        }
    }

    /* Decode vertices. */
    pos = 16;
    for (i32 i = 0; i < n_verts; i++) {
        const u8 *r = raw + pos;
        out->verts[i].x  = read_i32_le(r +  0);
        out->verts[i].y  = read_i32_le(r +  4);
        out->verts[i].z  = read_i32_le(r +  8);
        out->verts[i].nx = read_i32_le(r + 12);
        out->verts[i].ny = read_i32_le(r + 16);
        out->verts[i].nz = read_i32_le(r + 20);
        pos += 32;
    }
    out->vert_count = n_verts;

    /* Decode faces — same greedy scan as the counting pass. */
    pos = face_section_start;
    for (i32 i = 0; i < n_faces; i++) {
        if (i > 0) {
            pos += 4; /* skip 4-byte heap ptr before face 1+ */
        }
        if (pos + 96 > raw_len) break;

        const u8 *r = raw + pos;
        i32 ftype = read_i32_le(r +  0);
        i32 vcnt  = read_i32_le(r +  4);

        if (ftype != 100 || vcnt < 3 || vcnt > 4) {
            print("ome_decode_atomf: face %d bad type=%d vcnt=%d — truncating\n",
                  i, ftype, vcnt);
            n_faces = i;
            break;
        }

        out->faces[i].vcnt   = (u8)vcnt;
        out->faces[i].idx[0] = (u8)read_i32_le(r +  8);
        out->faces[i].idx[1] = (u8)read_i32_le(r + 12);
        out->faces[i].idx[2] = (u8)read_i32_le(r + 16);
        out->faces[i].idx[3] = (vcnt == 4) ? (u8)read_i32_le(r + 20) : out->faces[i].idx[2];
        out->faces[i].nx     = read_i32_le(r + 24); /* Q14 */
        out->faces[i].ny     = read_i32_le(r + 28);
        out->faces[i].nz     = read_i32_le(r + 32);
        out->faces[i].mat_id = read_i32_le(r + 36);
        pos += 96;
    }
    out->face_count = n_faces;

    print("ome_decode_atomf: %d verts %d faces\n", out->vert_count, out->face_count);
    return 1;
}

void ome_atom_free(ome_atom_t *atom) {
    if (!atom) return;
    if (atom->verts) { free(atom->verts); atom->verts = NULL; }
    if (atom->faces) { free(atom->faces); atom->faces = NULL; }
    atom->vert_count = 0;
    atom->face_count = 0;
}
