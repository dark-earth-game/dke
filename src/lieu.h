#ifndef DKE_LIEU_H
#define DKE_LIEU_H

#include "libsys/common.h"
#include "libsys/system.h"

/* Max recorded calls per level (LIEU01 = 57; safety margin) */
#define LIEU_MAX_RECORDS  256

/* An arg is either an i32 or an interned c-string.
 * When kind == LIEU_ARG_STR, `str` points into arena_strings[]. */
typedef enum {
    LIEU_ARG_NONE = 0,
    LIEU_ARG_I32,
    LIEU_ARG_STR,
} lieu_arg_kind_e;

typedef struct {
    lieu_arg_kind_e kind;
    i32             i32v;
    const c8       *str;
} lieu_arg_t;

typedef struct {
    i32          slot;        /* 0x00..0x9C, 4-byte aligned */
    lieu_arg_t   ecx;
    lieu_arg_t   edx;
    lieu_arg_t   stack[4];
    i32          stack_count;
} lieu_record_t;

typedef struct {
    lieu_record_t records[LIEU_MAX_RECORDS];
    i32           count;
    /* String arena: c-strings from all records are copied here contiguously. */
    c8           *arena;
    i32           arena_size;
    i32           arena_used;
} lieu_script_t;

/*
 * lieu_load_script — read bin/game/lieu/LIEU%02d.script into `out`.
 * Uses sys->state->game_path + "lieu/LIEU%02d.script".
 * Returns 1 on success, 0 on missing file / parse fail.
 */
i32 lieu_load_script(system_t *sys, i32 level_id, lieu_script_t *out);

/* Free arena. Safe to call on zeroed struct. */
void lieu_free_script(lieu_script_t *s);

/*
 * lieu_replay — walk `s->records`, dispatch each to its native stub.
 * All stubs currently log via print() only; no engine effect yet.
 */
void lieu_replay(system_t *sys, const lieu_script_t *s);

#endif /* DKE_LIEU_H */
