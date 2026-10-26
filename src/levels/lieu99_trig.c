/*
 * lieu99_trig.c — Native hand-port of the LIEU99.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu99.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 *
 * LIEU99 is a developer test room: trigger 0 does not read
 * ST_START_POINT(99) (0xAEC). It reads the scene-phase slot ST(0x100),
 * clears it first, so only the phase-0 block is reachable from INIT.
 */

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu99_int.h"

static void trig41(void);
static void trig_noop(void);   /* the 40 stub slots */

/* dword_100182F0[42] — filled at INIT with the 5-byte jmp thunks; static
 * here. Slots 1..40 are empty stubs (trig01_10008491 ... trig40_1000863E,
 * `push ebp; mov ebp,esp; ... ret`).
 *   0  100073F0 (l99_trig00)     1..40  stubs     41  1000766A (trig41)
 */
static void (*const g_trigger[42])(void) = {
    l99_trig00,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x100182F0]). */
void l99_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — test-room setup: clear the phase, then place the actors of the phase */
void l99_trig00(void) {
    CB->vire_col(0x20, 0);
    CB->vire_col(0x21, 0);
    ST(0x100) = 0;
    /* 0x7424 */
    if (ST(0x100) == 0) {
        CB->divers(DV_SCREEN, 1, 0);
        CB->divers(DV_MUSIC, 1, 0);
        CB->change_camera(7, CAM_LOCKED);
        CB->pos_perso(2, 1506, -699, 0x4000, 1, "ar", 1, 0);
        CB->pos_perso(P_MAOR, 1150, -1018, 0x3A3C, 1, "P17ARR01", 2, 0);
        CB->pos_perso(P_BOGDARAN, 968, -1070, 0xC0A5, 1, "P08ARR01", 3, 0);
    }
    /* 0x74D4 */
    if (ST(0x100) == 1) {
        CB->divers(DV_MUSIC, 2, 0);
        CB->change_camera(4, CAM_LOCKED);
        CB->pos_perso(2, 112, -3133, 0xC000, 1, "ar", 1, 0);
        CB->pos_perso(P_PHEDORIA, 302, -1775, 0x4000, 1, "P26GAR03", 2, 0);
        CB->pos_perso(P_ZED27, 21, -1764, 0xC000, 1, "P27GAR02", 3, 0);
    }
    /* 0x7577 */
    if (ST(0x100) == 2) {
        CB->divers(DV_MUSIC, 3, 0);
        CB->change_camera(3, CAM_LOCKED);
        CB->pos_perso(2, 800, -1556, 0xC499, 1, "ar", 1, 0);
        CB->divers_empile(DV_USE_ITEM, 24, 0);
        CB->pos_perso(P_LEONA, 1230, -1717, 0, 1, "P14ARR01", 3, 0);
        CB->pos_perso(P_MALROT2, 1287, -1524, 0x8585, 1, "P62ATT02", 2, 0);
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
}

/* trigger 41 — sub_1000766A — phase-change zone: run the entry sequence of the current phase */
static void trig41(void) {
    CB->bloque(BLOQUE_ALL);
    /* 0x767B */
    if (ST(0x100) == 0) {
        CB->divers_empile(DV_MUSIC, 1, 1);
        CB->joindre(-ST(ST_HERO), 946, -931, 0xAE1E, -1);
        CB->animation("Sar_50");
        CB->select_pile(2, 2);
        ST(0x70) = 1;
        CB->cinematique("SC01_500");
        CB->divers_empile(DV_SCREEN, 3, 0);
        lieu_action_register("L05D110A", P_BOGDARAN, "SP08DIA60", "SP08ARR03_20");
        lieu_action_register("L05D110B", P_BOGDARAN, "SP08DIA56", "SP08ARR03_20");
        CB->animation("SP08ARR03");
        CB->joindre(-P_BOGDARAN, 839, -1573, -1, -1);
        CB->select_pile(0, 1);
    }
    /* 0x776C */
    if (ST(0x100) == 1) {
        CB->divers_empile(DV_MUSIC, 2, 1);
        CB->divers_empile(DV_CINEMATIC, 1, 2);
    }
    /* 0x77A2 */
    if (ST(0x100) == 2) {
        CB->divers_empile(DV_MUSIC, 3, 1);
        CB->joindre(ST(ST_HERO), 1115, -1556, -1, -1);
        CB->select_pile(0, 1);
    }
}
