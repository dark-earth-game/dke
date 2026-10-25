/*
 * lieu41_trig.c - Native hand-port of the LIEU41.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and events live in lieu41.c; this
 * file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu41_int.h"

static void trig03(void); static void trig04(void); static void trig05(void);
static void trig06(void); static void trig07(void); static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* trigger table at 0x10018100 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 100073F0   1 stub   2 stub   3 10007AB5   4 10007BBF   5 10007BEE
 *   6 10007C39   7 10007D0E   8..40 stub   41 10007591
 */
static void (*const g_trigger[42])(void) = {
    l41_trig00, trig_noop, trig_noop, trig03, trig04, trig05, trig06, trig07,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10018100]). */
void l41_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - sub_100073F0 (thunk sub_10001073): INIT default. */
void l41_trig00(void) {
    if (ST(ST_START_POINT(41)) == 1) {
        ST(ST_HERO) = 3;
        ST(ST_LEVEL) = 40;
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    if (ST(ST_LEVEL) == 40) {
        CB->pos_perso(3, 1583, -1494, 0x4000, 1, "ar", 1, 0);
    }
    CB->ajoute_zone(2, -30, -1470, 30, 30, 0);
    CB->ajoute_zone(3, -30, -1470, 200, 200, 0);
    CB->ajoute_zone(4, 1770, -1470, 110, 265, 1);
    CB->ajoute_zone(5, -1845, -1500, 120, 240, 1);
    CB->ajoute_zone(6, 640, -585, 885, 70, 1);
    CB->ajoute_zone(7, 640, -2350, 885, 70, 1);
    CB->divers(DV_MUSIC, 1, 0);
    CB->bruitage("L41AMB1", -1, -1);
}

/* trigger 41 - sub_10007591 */
static void trig41(void) {
    CB->divers_empile(DV_MUSIC, 1, 1);
    CB->bloque(BLOQUE_ALL);
    if (ST(0x8E8) == 0) {
        lieu_action_register("L41D02", P_ARKHAN3, "SG00DIA04", "Sar_10");
    }
    ST(0x8E8) = 1;
    CB->bloque(BLOQUE_NONE);
}

/* trigger 3 - sub_10007AB5 */
static void trig03(void) {
    if (CB->direction_zone(2) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, -3);
        if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
            CB->animation("SP03ACT71");
            lieu_action_register("L41D03", P_ARKHAN3, "P03ACT74", "SP03ACT77");
        } else {
            CB->animation("SP03ACT70");
            lieu_action_register("L41D03", P_ARKHAN3, "P03ACT74", "SP03ACT76");
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 4 - sub_10007BBF: exit to level 40 */
static void trig04(void) {
    CB->retire_zone(4);
    ST(ST_LEVEL) = 41;
    CB->change_piece(40, 0);
}

/* trigger 5 - sub_10007BEE: exit to level 42 */
static void trig05(void) {
    CB->divers(DV_MUSIC, 1, 4);
    CB->divers_empile(DV_SCREEN, 1, 0);
    ST(ST_LEVEL) = 41;
    CB->change_piece(42, -1);
}

/* trigger 6 - sub_10007C39: up key facing 0xA at the side zone 6 */
static void trig06(void) {
    if (ST(ST_UPDOWN_KEY) == 1 && CB->direction_spec(0xA)) {
        CB->bloque(BLOQUE_ALL);
        CB->retire_zone(6);
        if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
            lieu_action_register("LCOD68", P_ARKHAN3, "SP03ATT29", "Sar_10");
        } else {
            lieu_action_register("LCOD68", P_ARKHAN3, "SP03ATT28", "Sar_10");
        }
        if (ST(0x6C) == 0) {
            ST(0x6C) = 1;
            lieu_action_register("L41D09", P_ARKHAN3, "SG00DIA02", "Sar_10");
        }
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 7 - sub_10007D0E: up key facing 0x80E8 at the side zone 7 */
static void trig07(void) {
    if (ST(ST_UPDOWN_KEY) == 1 && CB->direction_spec(0x80E8)) {
        CB->bloque(BLOQUE_ALL);
        CB->retire_zone(7);
        if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
            lieu_action_register("LCOD68", P_ARKHAN3, "SP03ATT29", "Sar_10");
        } else {
            lieu_action_register("LCOD68", P_ARKHAN3, "SP03ATT28", "Sar_10");
        }
        if (ST(0x6C) == 0) {
            ST(0x6C) = 1;
            lieu_action_register("L41D09", P_ARKHAN3, "SG00DIA02", "Sar_10");
        }
        CB->bloque(BLOQUE_NONE);
        CB->evenement(1, 100);
    }
}
