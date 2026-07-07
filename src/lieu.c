/*
 * lieu.c — LIEU script replay module.
 *
 * Reads pre-extracted .script text files (one per level) and dispatches
 * each recorded callback to a log-only C stub.  No x86 emulation.
 *
 * Script files are produced by tools/src/lieu_extract.py and stored at
 * bin/game/lieu/LIEU%02d.script.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/file_system.h"
#include "libsys/system.h"
#include "state.h"
#include "lieu.h"

/* -------------------------------------------------------------------------
 * Internal helpers
 * ---------------------------------------------------------------------- */

/* Skip leading whitespace; return pointer past it. */
static const c8 *skip_ws(const c8 *p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

/*
 * intern_str — copy `src` (len bytes, no null terminator required) into the
 * arena, append '\0', and return a pointer to the interned copy.
 * Returns NULL if the arena is exhausted (should not happen with generous
 * pre-allocation).
 */
static const c8 *intern_str(lieu_script_t *s, const c8 *src, i32 len) {
    if (s->arena_used + len + 1 > s->arena_size) return NULL;
    c8 *dst = s->arena + s->arena_used;
    memcpy(dst, src, (size_t)len);
    dst[len] = '\0';
    s->arena_used += len + 1;
    return dst;
}

/*
 * parse_arg — parse one VALUE token at *pp.
 * Advances *pp past the parsed token.
 * Returns 1 on success, 0 on failure.
 */
static i32 parse_arg(const c8 **pp, lieu_script_t *s, lieu_arg_t *out) {
    const c8 *p = *pp;
    if (p[0] == '0' && p[1] == 'x') {
        /* HEX integer: 0x... */
        c8 *end = NULL;
        unsigned long uv = strtoul(p, &end, 16);
        if (end == p) return 0;
        out->kind = LIEU_ARG_I32;
        out->i32v = (i32)uv;     /* sign via two's complement reinterpret */
        out->str  = NULL;
        *pp = end;
        return 1;
    } else if (p[0] == '\'') {
        /* Quoted string: '...' */
        p++;  /* skip opening quote */
        const c8 *start = p;
        while (*p && *p != '\'') p++;
        if (*p != '\'') return 0;
        i32 len = (i32)(p - start);
        const c8 *interned = intern_str(s, start, len);
        if (!interned) return 0;
        out->kind = LIEU_ARG_STR;
        out->i32v = 0;
        out->str  = interned;
        *pp = p + 1;  /* skip closing quote */
        return 1;
    }
    return 0;
}

/*
 * parse_line — parse one LIEU script line into a lieu_record_t.
 * Line grammar (simplified):
 *   NAME "(" HEX_OFF ")" WS+ FIELD (WS+ FIELD)*
 *   FIELD := KEY "=" VALUE
 * Returns 1 on success, 0 if the line should be skipped.
 */
static i32 parse_line(const c8 *line, lieu_script_t *s, lieu_record_t *rec) {
    memset(rec, 0, sizeof(*rec));

    const c8 *p = skip_ws(line);

    /* NAME — skip past identifier to the opening paren */
    while (*p && *p != '(' && *p != ' ' && *p != '\t') p++;
    if (*p != '(') return 0;
    /* (name_start..p) is the name — we don't need to store it, slot is enough */

    /* HEX_OFF inside parens */
    p++;  /* skip '(' */
    c8 *end = NULL;
    unsigned long slot = strtoul(p, &end, 16);
    if (end == p || *end != ')') return 0;
    rec->slot = (i32)slot;
    p = end + 1;  /* skip ')' */

    p = skip_ws(p);

    /* ecx=VALUE */
    if (strncmp(p, "ecx=", 4) != 0) return 0;
    p += 4;
    if (!parse_arg(&p, s, &rec->ecx)) return 0;

    p = skip_ws(p);

    /* edx=VALUE */
    if (strncmp(p, "edx=", 4) != 0) return 0;
    p += 4;
    if (!parse_arg(&p, s, &rec->edx)) return 0;

    /* Stack fields: 0..4 of "i32=..." or "str=..." */
    rec->stack_count = 0;
    while (rec->stack_count < 4) {
        p = skip_ws(p);
        if (*p == '\0' || *p == '\n' || *p == '\r') break;

        if (strncmp(p, "i32=", 4) == 0) {
            p += 4;
            if (!parse_arg(&p, s, &rec->stack[rec->stack_count])) break;
            rec->stack_count++;
        } else if (strncmp(p, "str=", 4) == 0) {
            p += 4;
            if (!parse_arg(&p, s, &rec->stack[rec->stack_count])) break;
            rec->stack_count++;
        } else {
            break;
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

void lieu_free_script(lieu_script_t *s) {
    if (!s) return;
    if (s->arena) {
        free(s->arena);
    }
    memset(s, 0, sizeof(*s));
}

i32 lieu_load_script(system_t *sys, i32 level_id, lieu_script_t *out) {
    (void)sys;  /* sys unused; we access state global for game_path */

    memset(out, 0, sizeof(*out));

    /* Build path: <game_path>/lieu/LIEU%02d.script */
    c8 virt[64];
    snprintf(virt, sizeof(virt), "lieu/LIEU%02d.script", level_id);
    const c8 *base = (state && state->game_path) ? state->game_path : ".";
    c8 host[512];
    if (!ls_resolve_path(base, virt, host, (i32)sizeof(host))) return 0;

    FILE *f = ls_open(host, "r");
    if (!f) return 0;

    /* Read whole file into a temporary buffer. */
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (fsize <= 0) { ls_close(f); return 0; }

    c8 *buf = (c8 *)malloc((size_t)fsize + 1);
    if (!buf) { ls_close(f); return 0; }
    size_t nread = fread(buf, 1, (size_t)fsize, f);
    ls_close(f);
    buf[nread] = '\0';

    /* Allocate arena with generous headroom (file size is upper bound on
     * total string data since strings come from the file). */
    out->arena_size = (i32)nread + 1;
    out->arena = (c8 *)malloc((size_t)out->arena_size);
    if (!out->arena) { free(buf); return 0; }
    out->arena_used = 0;

    /* Parse line by line. */
    c8 *line = buf;
    while (*line && out->count < LIEU_MAX_RECORDS) {
        /* Find end of line. */
        c8 *eol = line;
        while (*eol && *eol != '\n') eol++;
        c8 saved = *eol;
        *eol = '\0';

        lieu_record_t rec;
        if (parse_line(line, out, &rec)) {
            out->records[out->count++] = rec;
        }

        *eol = saved;
        line = (*eol == '\n') ? eol + 1 : eol;
    }

    free(buf);
    return 1;
}

/* -------------------------------------------------------------------------
 * Logging helpers
 * ---------------------------------------------------------------------- */

static void log_arg(const lieu_arg_t *a) {
    if (a->kind == LIEU_ARG_STR) {
        print("'%s'", a->str ? a->str : "");
    } else {
        print("0x%x", (u32)a->i32v);
    }
}

static void log_stack(const lieu_record_t *r) {
    print("stack=[");
    for (i32 k = 0; k < r->stack_count; k++) {
        if (k) print(", ");
        log_arg(&r->stack[k]);
    }
    print("]");
}

/* -------------------------------------------------------------------------
 * Replay dispatch
 * ---------------------------------------------------------------------- */

void lieu_replay(system_t *sys, const lieu_script_t *s) {
    (void)sys;

    for (i32 i = 0; i < s->count; i++) {
        const lieu_record_t *r = &s->records[i];
        switch (r->slot) {

        case 0x00:
            print("lieu: [%d] slot=0x00 NamedObject ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x04:
            print("lieu: [%d] slot=0x04 NamedObjectFlag ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x08:
            print("lieu: [%d] slot=0x08 NamedObjectTwoInts ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x0C:
            print("lieu: [%d] slot=0x0C Cinematique ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x10:
            print("lieu: [%d] slot=0x10 KillPerso ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x14:
            print("lieu: [%d] slot=0x14 NamedObjectStrPair ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x18:
            print("lieu: [%d] slot=0x18 NamedObjectStr ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x1C:
            print("lieu: [%d] slot=0x1C CB_1C ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x24:
            print("lieu: [%d] slot=0x24 CB_24 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x2C:
            print("lieu: [%d] slot=0x2C CB_2C ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x30:
            print("lieu: [%d] slot=0x30 CB_30 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x34:
            print("lieu: [%d] slot=0x34 CB_34 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x50:
            print("lieu: [%d] slot=0x50 CB_50 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x5C:
            print("lieu: [%d] slot=0x5C CB_5C ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x74:
            print("lieu: [%d] slot=0x74 CB_74 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x78:
            print("lieu: [%d] slot=0x78 CB_78 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x80:
            print("lieu: [%d] slot=0x80 CB_80 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x84:
            print("lieu: [%d] slot=0x84 CB_84 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x88:
            print("lieu: [%d] slot=0x88 CB_88 ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x8C:
            print("lieu: [%d] slot=0x8C CB_8C ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        case 0x9C:
            print("lieu: [%d] slot=0x9C CB_9C ecx=", i);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;

        default:
            print("lieu: [%d] slot=0x%02x CB_?? ecx=", i, (u32)r->slot);
            log_arg(&r->ecx); print(" edx="); log_arg(&r->edx);
            print(" "); log_stack(r); print("\n");
            break;
        }
    }
}
