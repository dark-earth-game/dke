/*
 * lieu43_trig.c — Native hand-port of the LIEU43.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu43.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * The DLL globals (lever states) are declared in lieu43_int.h.
 */

#include "lieu43_int.h"

static void trig_noop(void);   /* the 31 stub slots */
static void trig02(void); static void trig05(void); static void trig06(void);
static void trig07(void); static void trig08(void); static void trig09(void);
static void trig10(void); static void trig12(void); static void trig13(void);
static void trig41(void);

/* dword_10019150[42] — filled at INIT with the 5-byte jmp thunks; static here.
 *   idx handler        idx handler        idx handler
 *    0  10001200       14  100031C5 stub  28  1000325F stub
 *    1  100021B7 stub  15  100031D0 stub  29  1000326A stub
 *    2  100021C2       16  100031DB stub  30  10003275 stub
 *    3  100024AF stub  17  100031E6 stub  31  10003280 stub
 *    4  100024BA stub  18  100031F1 stub  32  1000328B stub
 *    5  100024C5       19  100031FC stub  33  10003296 stub
 *    6  10002761       20  10003207 stub  34  100032A1 stub
 *    7  10002A00       21  10003212 stub  35  100032AC stub
 *    8  10002ABA       22  1000321D stub  36  100032B7 stub
 *    9  10002B51       23  10003228 stub  37  100032C2 stub
 *   10  10002BE8       24  10003233 stub  38  100032CD stub
 *   11  10002C7F stub  25  1000323E stub  39  100032D8 stub
 *   12  10002C8A       26  10003249 stub  40  100032E3 stub
 *   13  10002F29       27  10003254 stub  41  1000149B
 */
static void (*const g_trigger[42])(void) = {
    l43_trig00, trig_noop, trig02, trig_noop, trig_noop, trig05, trig06, trig07,
    trig08, trig09, trig10, trig_noop, trig12, trig13, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10019150]). */
void l43_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 — trig00_10001200 (thunk sub_10001073; INIT default) */
void l43_trig00(void) {
    /* 0x1206 */
    if (ST(ST_START_POINT(43)) == 1) {
        ST(ST_HERO) = P_ARKHAN3;
        ST(ST_LEVEL) = 44;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    ST(ST_START_POINT(43)) = 0;
    ST(0xEC) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 40;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x88) = 0;
    /* 0x12AB — the original tests ST_LEVEL == 44 (jump to the placement) and
     * then ST_LEVEL == 44 again (skip it), the second test falls through
     * to the placement too: it always runs. */
    CB->pos_perso(P_ARKHAN3, 3477, -1278, 0x2ED8, 1, "ar", 1, 0);
    CB->pos_perso(40, 2524, -717, 0xA481, 1, "P40GAR01", 2, 0);
    CB->load_anim("LEVIER1", 0);
    CB->load_anim("LEVIER2", 1);
    CB->load_anim("LEVIER2D", 2);
    CB->load_anim("LEVIER2G", 3);
    CB->load_anim("LEVIER1D", 4);
    CB->load_anim("LEVIER1G", 5);
    CB->bruitage("L43AMB1", 5888, -1443);
    CB->divers(DV_MUSIC, 1, 0);
    CB->ajoute_zone(5, 6320, -1894, 180, 70, 0);
    CB->ajoute_zone(6, 6312, -1215, 185, 135, 0);
    CB->ajoute_zone(7, 2218, -1540, 115, 165, 0);
    CB->ajoute_zone(8, 4917, -1996, 632, 700, 0);
    CB->ajoute_zone(9, 5569, -948, 423, 480, 0);
    CB->ajoute_zone(10, 5618, -1957, 432, 330, 0);
    CB->ajoute_zone(12, 6298, -1051, 135, 145, 0);
    CB->ajoute_zone(13, 6318, -1761, 160, 80, 0);
}

/* trigger 41 — trig41_1000149B */
static void trig41(void) {
    /* 0x14A1 */
    if (ST(0x504) == 0) {
        CB->cinematique("SC20");
        CB->divers(DV_RUN, 40, 1);
        ST(0x504) = 1;
        CB->combat(40, 0);
        ST(V_ENGAGED_PERSONA) = 40;
        CB->divers_empile(DV_MUSIC, 1, 1);
        CB->divers_empile(DV_SCREEN, 3, 0);
    }
}

/* Common head of the four lever triggers (5, 6, 12, 13): the lever's side
 * animation and the "ar_10" return. `a` and `b` are the two paused anim
 * slots, `p` the played one; the pause_anim/play_anim order is the original's. */
static void lever_head(i32 a, i32 b, i32 p, const char *ar) {
    CB->select_pile(1, 1);
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
        CB->animation("SP03POU02");
    }
    if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
        CB->animation("SP03POU01");
    }
    CB->pause_anim(a, 4, -1);
    CB->pause_anim(b, 4, -1);
    CB->play_anim(p, 0);
    CB->animation(ar);
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->select_pile(2, 2);
}

/* trigger 2 — trig02_100021C2 */
static void trig02(void) {
    /* 0x21C8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(0xEC) == 0 && ST(ST_SCRIPT_BUSY) == 0
        && ST(V_ENGAGED_PERSONA) == 40 && ST(ST_COMBAT_STANCE) == 0) {
        /* 0x2216 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 5) && CB->direction_spec(0xE5C3)) {
            trig05();
            return;
        }
        /* 0x2253 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13) && CB->direction_spec(0x9C73)) {
            trig13();
            return;
        }
        /* 0x2290 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 6) && CB->direction_spec(0xF464)) {
            trig06();
            return;
        }
        /* 0x22CD */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 12) && CB->direction_spec(0xAB9E)) {
            trig12();
            return;
        }
    }
    /* 0x230A */
    if (ST(ST_ACTION_KEY) == 1 && ST(0xEC) == 0 && ST(ST_SCRIPT_BUSY) == 0
        && ST(V_ENGAGED_PERSONA) == 40 && ST(ST_COMBAT_STANCE) == 0
        && ST(ST_VIE(40)) != 2 && CB->direction_perso(40)) {
        /* 0x237D */
        if (!CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)
            || !CB->divers(DV_IN_ZONE, ST(ST_HERO), 13)
            || !CB->divers(DV_IN_ZONE, ST(ST_HERO), 6)
            || !CB->divers(DV_IN_ZONE, ST(ST_HERO), 12)) {
            /* 0x23FD */
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x2414 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L43D01", 3);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x245F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L43D02", 3);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 5 — trig05_100024C5 (left lever, facing 0xE5C3) */
static void trig05(void) {
    /* 0x24CB */
    if (ST(ST_ACTION_KEY) == 1 && ST(0xEC) == 0 && ST(ST_SCRIPT_BUSY) == 0
        && ST(V_ENGAGED_PERSONA) == 40 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xE5C3)) {
        lever_head(0, 5, 4, "ar_10");
        /* 0x25FF */
        if (g_l43_left_init == 0) {
            g_l43_left_init = 1;
            g_l43_left_pos = 0;
            G_A50 = 1;
            g_l43_cur_left = 7;
            CB->load_anim("TLM1", 7);
            CB->evenement(0, 500);
        } else if (g_l43_left_pos == 1) {
            /* 0x2656 */
            g_l43_left_pos = 0;
            G_A58 = 1;
            g_l43_cur_left = 7;
            CB->load_anim("TLM1", 7);
            CB->evenement(0, 500);
            CB->load_anim("TLU0", 9);
            CB->evenement(1, 1000);
        } else if (g_l43_left_pos == 0) {
            /* 0x26D0 */
            g_l43_left_pos = -1;
            G_A5C = 1;
            g_l43_cur_left = 8;
            CB->load_anim("TLD1", 8);
            CB->evenement(0, 500);
            CB->load_anim("TLM0", 10);
            CB->evenement(1, 1000);
        }
        /* 0x274A */
        CB->select_pile(0, 1);
    }
}

/* trigger 6 — trig06_10002761 (right lever, facing 0xF464) */
static void trig06(void) {
    /* 0x276C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0xEC) == 0
        && ST(V_ENGAGED_PERSONA) == 40 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xF464)) {
        lever_head(1, 3, 2, "ar_10");
        /* 0x289E */
        if (g_l43_right_init == 0) {
            g_l43_right_init = 1;
            g_l43_right_pos = 0;
            G_A64 = 1;
            g_l43_cur_right = 13;
            CB->load_anim("TRM1", 13);
            CB->evenement(0, 500);
        } else if (g_l43_right_pos == -1) {
            /* 0x28F5 */
            g_l43_right_pos = 0;
            G_A74 = 1;
            g_l43_cur_right = 13;
            CB->load_anim("TRM1", 13);
            CB->evenement(0, 500);
            CB->load_anim("TRD0", 17);
            CB->evenement(1, 1000);
        } else if (g_l43_right_pos == 0) {
            /* 0x296F */
            g_l43_right_pos = 1;
            G_A68 = 1;
            g_l43_cur_right = 12;
            CB->load_anim("TRU1", 12);
            CB->evenement(0, 500);
            CB->load_anim("TRM0", 16);
            CB->evenement(1, 1000);
        }
        /* 0x29E9 */
        CB->select_pile(0, 1);
    }
}

/* trigger 7 — trig07_10002A00 */
static void trig07(void) {
    /* 0x2A06 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 40
        && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x427B)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->dialogue("L43D03A", 3);
        CB->dialogue("L43D03B", 3);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 8 — trig08_10002ABA */
static void trig08(void) {
    /* 0x2AC0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 40
        && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->dialogue("L43D04", 3);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 9 — trig09_10002B51 */
static void trig09(void) {
    /* 0x2B57 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 40
        && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->dialogue("L43D04", 3);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 10 — trig10_10002BE8 */
static void trig10(void) {
    /* 0x2BEE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 40
        && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->dialogue("L43D04", 3);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 12 — trig12_10002C8A (right lever, facing 0xAB9E) */
static void trig12(void) {
    /* 0x2C90 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0xEC) == 0
        && ST(V_ENGAGED_PERSONA) == 40 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xAB9E)) {
        lever_head(1, 2, 3, "ar_10");
        /* 0x2DC7 */
        if (g_l43_right_init == 0) {
            g_l43_right_init = 1;
            g_l43_right_pos = 0;
            G_A64 = 1;
            g_l43_cur_right = 13;
            CB->load_anim("TRM1", 13);
            CB->evenement(0, 500);
        } else if (g_l43_right_pos == 1) {
            /* 0x2E1E */
            g_l43_right_pos = 0;
            G_A6C = 1;
            g_l43_cur_right = 13;
            CB->load_anim("TRM1", 13);
            CB->evenement(0, 500);
            CB->load_anim("TRU0", 15);
            CB->evenement(1, 1000);
        } else if (g_l43_right_pos == 0) {
            /* 0x2E98 */
            g_l43_right_pos = -1;
            G_A70 = 1;
            g_l43_cur_right = 14;
            CB->load_anim("TRD1", 14);
            CB->evenement(0, 500);
            CB->load_anim("TRM0", 16);
            CB->evenement(1, 1000);
        }
        /* 0x2F12 */
        CB->select_pile(0, 1);
    }
}

/* trigger 13 — trig13_10002F29 (left lever, facing 0x9C73) */
static void trig13(void) {
    /* 0x2F2F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0xEC) == 0
        && ST(V_ENGAGED_PERSONA) == 40 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x9C73)) {
        lever_head(0, 4, 5, "ar_10");
        /* 0x3063 */
        if (g_l43_left_init == 0) {
            g_l43_left_init = 1;
            g_l43_left_pos = 0;
            G_A50 = 1;
            g_l43_cur_left = 7;
            CB->load_anim("TLM1", 7);
            CB->evenement(0, 500);
        } else if (g_l43_left_pos == -1) {
            /* 0x30BA */
            g_l43_left_pos = 0;
            G_A60 = 1;
            g_l43_cur_left = 7;
            CB->load_anim("TLM1", 7);
            CB->evenement(0, 500);
            CB->load_anim("TLD0", 11);
            CB->evenement(1, 1000);
        } else if (g_l43_left_pos == 0) {
            /* 0x3134 */
            g_l43_left_pos = 1;
            G_A54 = 1;
            g_l43_cur_left = 6;
            CB->load_anim("TLU1", 6);
            CB->evenement(0, 500);
            CB->load_anim("TLM0", 10);
            CB->evenement(1, 1000);
        }
        /* 0x31AE */
        CB->select_pile(0, 1);
    }
}
