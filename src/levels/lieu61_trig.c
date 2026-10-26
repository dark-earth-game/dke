/*
 * lieu61_trig.c - Native hand-port of the LIEU61.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT and tick live in lieu61.c; this file
 * holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu61_int.h"

static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* trigger table at 0x10007140 - filled at op 1 with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 10001206 (thunk 10001064)   1..40 stub   41 100011DB (thunk 10001023)
 */
static void (*const g_trigger[42])(void) = {
    l61_trig00, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10007140]). */
void l61_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - sub_10001206 (thunk sub_10001064): INIT default. */
void l61_trig00(void) {
    CB->pos_perso(1, 399, -1741, 0x4000, 1, "ar", -1, 1);
    CB->pos_perso(27, -1, -1741, 0xC000, 1, "ar", 1, 1);
}

/* trigger 41 - sub_100011DB (thunk sub_10001023) */
static void trig41(void) {
    CB->combat(27, 2);
    CB->combat(1, 2);
}
