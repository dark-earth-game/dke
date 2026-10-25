/*
 * lieu46_trig.c — Native hand-port of the LIEU46.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu46.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include "lieu46_int.h"

static void trig_noop(void);   /* the 37 stub slots */
static void trig01(void); static void trig02(void); static void trig03(void);
static void trig04(void);

/* dword_100184B0[42] — filled at INIT with the 5-byte jmp thunks; static here.
 * Slots 0-4 are level code; slots 5-41 are empty stubs (trig05..trig41 are
 * identical to L01 trig06). */
static void (*const g_trigger[42])(void) = {
    l46_trig00, trig01, trig02, trig03, trig04,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x100184B0]). */
void l46_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 — sub_100073F0 (thunk sub_10001073; INIT default) */
void l46_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(46)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 6;
        ST(0xB4C) = 3;
        ST(0x32C) = 0x1C;
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0xE0) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_53) = 0;
    CB->ajoute_zone(1, 3030, 660, 120, 125, 1);
    CB->ajoute_zone(2, 1780, -392, 110, 100, 0);
    CB->bruitage("L46AMB1", -1, -1);
    /* 0x750A */
    if (ST(ST_LEVEL) == 14) {
        CB->vire_col(-1, 0);
    }
    /* 0x7526 */
    if (ST(ST_LEVEL) == 6) {
        CB->bloque(BLOQUE_ALL);
        CB->vire_col(-1, 0);
        CB->pos_perso(ST(ST_HERO), 1780, -394, 0x3A98, 1, "ar", 6, 0);
        ST(V_EVENT_53) = 1;
        CB->modify_var(&ST(V_EVENT_2), 1);
        CB->evenement(53, 3800);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x75B6 */
    if (ST(ST_LEVEL) == 14) {
        CB->pos_perso(ST(ST_HERO), 3021, 515, 0x9C40, 1, "ar", 6, 0);
    }
    /* 0x75F3 */
    if (ST(0x32C) == 0 && ST(0x330) == 0) {
        CB->pos_perso(P_RYLSADHAR21, 1820, -150, 0x9470, 1, "P21ARR01", 3, 0);
    }
    /* 0x763C */
    if (ST(0x330) != 0) {
        CB->pos_perso(P_RYLSADHAR21, 1820, -150, 0x9470, 1, "P21cad", 3, 0);
    }
    /* 0x7673 */
    if (ST(0xB4C) <= 2) {
        CB->vire_col(0xC, 0);
    } else {
        CB->ajoute_zone(4, 2778, -251, 210, 200, 1);
    }
}

/* trigger 1 — sub_10007DF6 */
static void trig01(void) {
    ST(ST_LEVEL) = 46;
    CB->change_piece(14, -1);
}

/* trigger 2 — sub_10007E1D */
static void trig02(void) {
    /* 0x7E2C */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_spec(0x3E80)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->pos_perso(ST(ST_HERO), 1780, -394, 0x3A98, 1, "ar", 6, 0);
        /* 0x7E93 */
        if (ST(ST_PLAYER_WEAPON) == 0) {
            /* 0x7EA5 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02MAR89");
            }
            /* 0x7EC2 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03MAR89");
            }
        }
        /* 0x7EDF */
        if (ST(ST_PLAYER_WEAPON) != 0) {
            /* 0x7EF1 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02MAR91");
            }
            /* 0x7F0E */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03MAR91");
            }
        }
        CB->evenement(1, 5000);
    }
}

/* trigger 3 — sub_10007F34 */
static void trig03(void) {
    /* 0x7F43 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        /* 0x7F7F */
        if (ST(0x64) == 0) {
            ST(0x64) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x7FB4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x7FC6 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x7FE0 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02ACT71");
                        lieu_action_register("L22D12A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L22D12B", P_ARKHAN2, "SP02ACT74", "SP02ACT77");
                    } else {
                        CB->animation("SP02ACT70");
                        lieu_action_register("L22D12A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L22D12B", P_ARKHAN2, "SP02ACT74", "SP02ACT76");
                    }
                }
                /* 0x8071 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x808B */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02ACT73");
                        lieu_action_register("L22D13A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L22D13B", P_ARKHAN2, "SP02ACT74", "SP02ACT79");
                    } else {
                        CB->animation("SP02ACT72");
                        lieu_action_register("L22D13A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L22D13B", P_ARKHAN2, "SP02ACT74", "SP02ACT78");
                    }
                }
            }
            /* 0x811C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x812E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8148 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03ACT71");
                        lieu_action_register("L22D14", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                    } else {
                        CB->animation("SP03ACT70");
                        lieu_action_register("L22D14", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                    }
                }
                /* 0x81A7 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x81C1 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03ACT73");
                        lieu_action_register("L22D15", P_ARKHAN3, "P03ACT74", "SP03ACT79");
                    } else {
                        CB->animation("SP03ACT72");
                        lieu_action_register("L22D15", P_ARKHAN3, "P03ACT74", "SP03ACT78");
                    }
                }
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x8246 */
        if (ST(0x64) == 1) {
            CB->fouille(P_RYLSADHAR21);
            return;
        }
    }
}

/* trigger 4 — sub_10008261 */
static void trig04(void) {
    /* 0x8270 */
    if (ST(ST_SCRIPT_BUSY) == 0 && ST(0xE0) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0xE0) = 1;
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x82BA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD70", P_ARKHAN2, "SG00DIA04", NULL);
        }
        /* 0x82E2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD71", P_ARKHAN3, "SG00DIA04", NULL);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}
