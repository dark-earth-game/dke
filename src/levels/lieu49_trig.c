/*
 * lieu49_trig.c — Native hand-port of the LIEU49.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu49.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include "lieu49_int.h"

static void trig_noop(void);   /* the 27 stub slots (all identical to L01 trig06) */
static void trig02(void); static void trig03(void); static void trig04(void);
static void trig09(void); static void trig11(void); static void trig13(void);
static void trig14(void); static void trig15(void); static void trig16(void);
static void trig17(void); static void trig18(void); static void trig19(void);
static void trig20(void); static void trig41(void);

/* dword_1001BB50[42] — filled at INIT with the 5-byte jmp thunks; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000A715       28  1000B03C stub
 *    1  10009968 stub  15  1000A8A0       29  1000B047 stub
 *    2  10009973       16  1000AA60       30  1000B052 stub
 *    3  10009B57       17  1000AB83       31  1000B05D stub
 *    4  1000A03E       18  1000AC8A       32  1000B068 stub
 *    5  1000A1A4 stub  19  1000AD7E       33  1000B073 stub
 *    6  1000A1AF stub  20  1000AE9B       34  1000B07E stub
 *    7  1000A1BA stub  21  1000AFEF stub  35  1000B089 stub
 *    8  1000A1C5 stub  22  1000AFFA stub  36  1000B094 stub
 *    9  1000A1D0       23  1000B005 stub  37  1000B09F stub
 *   10  1000A35F stub  24  1000B010 stub  38  1000B0AA stub
 *   11  1000A36A       25  1000B01B stub  39  1000B0B5 stub
 *   12  1000A453 stub  26  1000B026 stub  40  1000B0C0 stub
 *   13  1000A45E       27  1000B031 stub  41  10007C3E
 */
static void (*const g_trigger[42])(void) = {
    l49_trig00, trig_noop, trig02, trig03, trig04, trig_noop, trig_noop, trig_noop,
    trig_noop, trig09, trig_noop, trig11, trig_noop, trig13, trig14, trig15,
    trig16, trig17, trig18, trig19, trig20, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001BB50]). */
void l49_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start points, zones, objects, actor placement */
void l49_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(49)) == 1) {
        ST(ST_LEVEL) = 48;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_KALHI13)) = 1;
        ST(0xBBC) = 2;
    }
    /* 0x744D */
    if (ST(ST_START_POINT(49)) == 2) {
        ST(ST_LEVEL) = 48;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    /* 0x7489 */
    if (ST(ST_START_POINT(49)) == 3) {
        ST(ST_LEVEL) = 48;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xBBC) = 2;
    }
    ST(ST_START_POINT(49)) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    ST(0x80) = 0;
    ST(0x84) = 0;
    ST(0x88) = 0;
    ST(V_EVENT15_STEP) = 0;
    ST(0x90) = 0;
    CB->ajoute_zone(9, 2563, 3595, 185, 65, 1);
    CB->ajoute_zone(11, 2645, 4078, 281, 286, 0);
    CB->ajoute_zone(12, 2628, 4112, 50, 50, 0);
    CB->ajoute_zone(13, 2520, 4676, 155, 120, 0);
    CB->ajoute_zone(14, 2638, 4618, 70, 110, 0);
    CB->ajoute_zone(15, 2891, 4386, 135, 95, 0);
    CB->ajoute_zone(16, 2728, 5060, 70, 95, 1);
    CB->ajoute_zone(17, 3407, 4411, 200, 190, 0);
    CB->ajoute_zone(18, 3153, 5236, 205, 220, 0);
    CB->ajoute_zone(19, 2974, 4949, 262, 130, 0);
    CB->ajoute_zone(20, 2899, 5071, 65, 85, 0);
    /* 0x7702 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->bruitage("L49AMB1", -1, -1);
    }
    /* 0x7726 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->bruitage("L49AMB2", -1, -1);
    }
    /* 0x7747 */
    if (ST(ST_LEVEL) == 13) {
        CB->pos_perso(ST(ST_HERO), 2533, 4606, 0x7530, 1, "ar", 1, 0);
    }
    /* 0x7781 */
    if (ST(ST_LEVEL) == 48) {
        CB->pos_perso(ST(ST_HERO), 2497, 3695, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x77CF */
    if (ST(ST_LEVEL) == 36) {
        CB->pos_perso(ST(ST_HERO), 2649, 5060, 0x2B29, 1, "ar", 1, 0);
    }
    /* 0x780C */
    if (ST(0x6FC) == 0) {
        CB->etat_objet("OB97", 2881, -2143, 4995, 0, 0x3A00, 0xFC00, -1, 4);
    }
    /* 0x7848 */
    if (ST(0xBBC) == 2 && ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD && ST(ST_VIE(P_MALROT8)) != VIE_DEAD) {
        CB->pos_perso(P_KALHI13, 2619, 4327, 0x3909, 1, "P13GAR02", 3, 0);
        CB->divers_empile(DV_HP, P_KALHI13, 200);
        CB->pos_perso(P_MALROT8, 2506, 4330, 0xB959, 1, "P68GAR02", 2, 0);
    }
    /* 0x78EF */
    if (ST(0xC2C) == 1) {
        CB->divers(DV_CINEMATIC, 0, 0);
        ST(ST_VIE(P_KALHI13)) = VIE_DEAD;
        ST(0xC2C) = 2;
    }
    /* 0x792E */
    if (ST(0xC2C) == 3 && ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_VIE(P_MALROT8)) != VIE_DEAD) {
        /* 0x7961 */
        if (ST(ST_LEVEL) == 48) {
            CB->pos_perso(P_MALROT8, 2434, 4248, 0x7E11, 1, "P68GAR02", 2, 0);
        }
        /* 0x7995 */
        if (ST(ST_LEVEL) == 36) {
            CB->pos_perso(P_MALROT8, 2890, 5128, 0x265A, 1, "P68GAR02", 2, 0);
        }
    }
    /* 0x79CC */
    if (ST(0x4DC) == 1 && ST(ST_VIE(P_DHORKAN11)) != VIE_DEAD && ST(ST_LEVEL) == 48) {
        CB->pos_perso(P_DHORKAN11, 2475, 4257, 0x7D51, 1, "P11GAR02", 4, 0);
    }
    /* 0x7A21 */
    if (ST(ST_LEVEL) == 36 && ST(0x4DC) == 1) {
        ST(0x4DC) = 2;
        /* 0x7A54 */
        if (ST(0xF78) == 0) {
            ST(0xF24) = 1;
        }
    }
    /* 0x7A75 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0x4DC) == 0 && ST(ST_LEVEL) != 36 && ST(ST_VIE(P_DHORKAN11)) != VIE_DEAD) {
        CB->pos_perso(P_DHORKAN11, 2620, 4630, 0xEBB3, 1, "P11ATT05", 4, 0);
    }
    /* 0x7ADF */
    if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD && ST(0xC2C) >= 2 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13cad", 3, 0);
    }
    /* 0x7B31 */
    if (ST(ST_VIE(P_MALROT8)) == VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->pos_perso(P_MALROT8, 0, 0, 0, 2, "P68cad", 2, 0);
    }
    /* 0x7B71 */
    if (ST(ST_VIE(P_DHORKAN11)) == VIE_DEAD && ST(0x4DC) == 2 && ST(0xF24) != 1 && ST(0xF24) != 2) {
        CB->pos_perso(P_DHORKAN11, 0, 0, 0, 2, "P11CAD", 4, 0);
    }
    /* 0x7BD5 */
    if (ST(0xE98) == 0x3C) {
        ST(0xE98) = 0x41;
    }
    /* 0x7BF6 */
    if (ST(0x4DC) == 3) {
        ST(0x4DC) = 2;
        /* 0x7C17 */
        if (ST(0xF78) == 0) {
            ST(0xF24) = 1;
        }
    }
    CB->load_anim("249PORTE", 0);
}

/* trigger 02 — sub_10009973 */
static void trig02(void) {
    /* 0x9982 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_MALROT8)) == VIE_DEAD) {
        /* 0x99D3 */
        if (ST(0x90C) == 0) {
            CB->divers_empile(DV_GIVE_ITEM, OB_SM_TH_AMMO, 1);
            ST(0x90C) = 1;
        }
        CB->fouille(P_MALROT8);
    }
    /* 0x9A0E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_MALROT8 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_MALROT8)) != VIE_DEAD && CB->direction_perso(P_MALROT8)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9A89 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x9A9B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0x9ABD */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD62", P_ARKHAN2);
            }
        }
        /* 0x9ADF */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x9AF1 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
            /* 0x9B13 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD63", P_ARKHAN3);
            }
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 03 — sub_10009B57 */
static void trig03(void) {
    /* 0x9B66 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_MALROT8 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9BCE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L49D29", P_ARKHAN2);
        }
        /* 0x9BF0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L49D30", P_ARKHAN3);
        }
        CB->dialogue("L49D31", P_KALHI13);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x9C48 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9CB0 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x9CC2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x9CDC */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT71");
                    lieu_action_register("L49D47A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L49D47B", P_ARKHAN2, "SP02ACT74", "SP02ACT77");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("ar_10");
                    CB->bloque(BLOQUE_NONE);
                } else {
                    CB->animation("SP02ACT70");
                    lieu_action_register("L49D47A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L49D47B", P_ARKHAN2, "SP02ACT74", "SP02ACT76");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("ar_10");
                    CB->bloque(BLOQUE_NONE);
                }
            }
            /* 0x9DB5 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x9DCF */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT71");
                    lieu_action_register("L49D48", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("ar_10");
                    CB->bloque(BLOQUE_NONE);
                } else {
                    CB->animation("SP03ACT70");
                    lieu_action_register("L49D48", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("ar_10");
                    CB->bloque(BLOQUE_NONE);
                }
            }
        }
        /* 0x9E76 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->fouille(P_KALHI13);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        }
    }
    /* 0x9EAC */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_MALROT8 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD && CB->direction_perso(P_KALHI13)) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->stop_joindre(-P_KALHI13);
        CB->bloque(BLOQUE_ALL);
        /* 0x9F23 */
        if (ST(0x53C) == 2) {
            CB->animation("SP13IMP52");
            CB->animation("P13CAD");
            CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13CAD", 3, 0);
            ST(ST_VIE(P_KALHI13)) = VIE_DEAD;
            CB->animation("Sar_10");
            CB->bloque(BLOQUE_NONE);
            ST(0x53C) = 3;
            return;
        }
        /* 0x9F9D */
        if (ST(0x53C) == 1) {
            ST(0x53C) = 2;
            CB->animation("SP13ESA15");
            CB->animation("P13GAR02_10");
            CB->dialogue("L10D107", P_KALHI13);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9FF1 */
        if (ST(0x53C) == 0) {
            ST(0x53C) = 1;
            CB->animation("SP13ESA15");
            CB->animation("P13GAR02_10");
            CB->dialogue("L10D106", P_KALHI13);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 04 — sub_1000A03E */
static void trig04(void) {
    /* 0xA04D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_DHORKAN11)) == VIE_DEAD && ST(0xC6C) == 0) {
        ST(0xC6C) = 1;
        ST(0xED4) = 1;
        /* 0xA0CE */
        if (ST(0xED0) == 5) {
            ST(0xED0) = 5;
        }
        /* 0xA0EF */
        if (ST(0xED0) == 2) {
            ST(0xED0) = 3;
        }
        /* 0xA110 */
        if (ST(0xED0) == 1) {
            ST(0xED0) = 2;
        }
        /* 0xA131 */
        if (ST(0xED0) == 0) {
            ST(0xED0) = 1;
        }
        CB->fouille(P_DHORKAN11);
    }
    /* 0xA15A — condition chain whose both outcomes continue at 0xA19F (no effect) */
}

/* trigger 09 — sub_1000A1D0 */
static void trig09(void) {
    /* 0xA1E2 */
    if ((ST(0xBBC) == 2 && ST(V_GUARD_FIGHT_DONE) != 2) || ST(0xBBC) == 10) {
        /* 0xA218 */
        if (ST(ST_VIE(P_MALROT8)) != VIE_DEAD) {
            ST(0xC2C) = 1;
        }
        /* 0xA239 */
        if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
            ST(0xC2C) = 3;
        }
    }
    /* 0xA26C */
    if (ST(0xBBC) == 3 && ST(0x52C) == 0) {
        ST(0x52C) = 1;
    }
    /* 0xA29F */
    if (ST(0xC2C) == 2 && ST(V_ENGAGED_PERSONA) == P_MALROT8) {
        ST(0xC2C) = 3;
    }
    /* 0xA2CF */
    if (ST(0x4DC) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->dialogue("L49D46", P_DHORKAN11);
        ST(ST_LEVEL) = 49;
        CB->change_piece(48, -1);
        CB->bloque(BLOQUE_NONE);
    }
    ST(ST_LEVEL) = 49;
    /* 0xA344 */
    if (ST(ST_SCRIPT_BUSY) == 0) {
        CB->change_piece(48, -1);
    }
}

/* trigger 11 — sub_1000A36A */
static void trig11(void) {
    /* 0xA379 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(12)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE01");
        /* 0xA3ED */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD07", P_ARKHAN2);
        }
        /* 0xA40F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD08", P_ARKHAN3);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 13 — sub_1000A45E */
static void trig13(void) {
    /* 0xA46D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xEF89)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA4D6 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA4E8 */
            if (ST(0xC20) == 0) {
                /* 0xA4FA */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SG00OUV12");
                    CB->dialogue("L49D32A", P_ARKHAN2);
                    CB->dialogue("L49D32B", P_ARKHAN2);
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xA560 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SG00OUV12");
                    CB->dialogue("L49D33", P_ARKHAN2);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xA5B6 */
            if (ST(0xC20) >= 1) {
                CB->dialogue("LCOD16", P_ARKHAN2);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xA601 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xA613 */
            if (ST(0xC20) == 0) {
                /* 0xA625 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SG00OUV12");
                    CB->dialogue("L49D34", P_ARKHAN3);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xA67B */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SG00OUV12");
                    CB->dialogue("L49D35", P_ARKHAN3);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xA6D1 */
            if (ST(0xC20) >= 1) {
                CB->dialogue("LCOD17", P_ARKHAN3);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 14 — sub_1000A715 */
static void trig14(void) {
    /* 0xA724 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xEF2F)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xA78D */
        if (ST(ST_PLAYER_WEAPON) != OB_PLATE_BEFORE) {
            /* 0xA79F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L49D36", P_ARKHAN2);
            }
            /* 0xA7C1 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L49D37", P_ARKHAN3);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA80C */
        if (ST(ST_PLAYER_WEAPON) == OB_PLATE_BEFORE) {
            /* 0xA81E */
            if (ST(0xC20) == 0) {
                ST(0xC20) = 1;
            }
            CB->joindre(ST(ST_HERO), 0, 0, 0xDF55, -1);
            /* 0xA85B */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ATT93");
            }
            /* 0xA878 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ATT93");
            }
            CB->evenement(0, 1400);
            return;
        }
    }
}

/* trigger 15 — sub_1000A8A0 */
static void trig15(void) {
    /* 0xA8AF */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xF525)) {
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00OUV12");
        /* 0xA908 */
        if (ST(0xBBC) < 2 || ST(0xBBC) >= 3 || (ST(0xBBC) == 2 && ST(V_GUARD_FIGHT_DONE) == 2)) {
            /* 0xA950 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("P02_65535");
                /* 0xA96A */
                if (ST(0x74) == 0) {
                    CB->dialogue("LCOD16", P_ARKHAN2);
                    ST(0x74) = 1;
                } else {
                    CB->dialogue("L49D44", P_ARKHAN2);
                }
            }
            /* 0xA9AD */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("P03_65535");
                /* 0xA9C7 */
                if (ST(0x74) == 0) {
                    CB->dialogue("LCOD17", P_ARKHAN3);
                    ST(0x74) = 1;
                } else {
                    CB->dialogue("L49D45", P_ARKHAN3);
                }
            }
        }
        /* 0xAA0A */
        if (ST(0xBBC) == 2 && ST(V_GUARD_FIGHT_DONE) != 2) {
            CB->dialogue("L49D31", P_KALHI13);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 — sub_1000AA60 */
static void trig16(void) {
    /* 0xAA72 */
    if ((ST(0xBBC) == 2 && ST(V_GUARD_FIGHT_DONE) != 2) || ST(0xBBC) == 10) {
        /* 0xAAA8 */
        if (ST(ST_VIE(P_MALROT8)) != VIE_DEAD) {
            ST(0xC2C) = 3;
            ST(ST_VIE(P_KALHI13)) = VIE_DEAD;
        }
        /* 0xAAD8 */
        if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
            ST(0xC2C) = 3;
        }
    }
    /* 0xAB0B */
    if (ST(0xBBC) == 3 && ST(0x52C) == 0) {
        ST(0x52C) = 1;
    }
    /* 0xAB3E */
    if (ST(0xC2C) == 2 && ST(V_ENGAGED_PERSONA) == P_MALROT8) {
        ST(0xC2C) = 3;
    }
    ST(ST_LEVEL) = 49;
    CB->change_piece(36, -1);
}

/* trigger 17 — sub_1000AB83 */
static void trig17(void) {
    /* 0xAB92 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x71D7)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xABFB */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD10", P_ARKHAN2);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xAC46 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD11", P_ARKHAN3);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 18 — sub_1000AC8A */
static void trig18(void) {
    /* 0xAC99 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xACEF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD07", P_ARKHAN2);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xAD3A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD08", P_ARKHAN3);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 19 — sub_1000AD7E */
static void trig19(void) {
    /* 0xAD8D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x745B)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xADF6 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00PRE03");
            CB->dialogue("L49D49", P_ARKHAN2);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xAE4C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00PRE03");
            CB->dialogue("L49D50", P_ARKHAN3);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 20 — sub_1000AE9B */
static void trig20(void) {
    /* 0xAEAA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x7D3B)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAF04 */
        if (ST(0x6FC) == 0) {
            CB->animation("SG00PRE08");
            CB->etat_objet("OB97", -1, -1, -1, -1, -1, -1, -3, 4);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            ST(0x6FC) = 1;
            return;
        }
        /* 0xAF77 */
        if (ST(0x6FC) == 1) {
            /* 0xAF89 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD79", P_ARKHAN2);
            }
            /* 0xAFAB */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD80", P_ARKHAN3);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 41 — sub_10007C3E */
static void trig41(void) {
    /* 0x7C50 */
    if (ST(0xBBC) == 2 && ST(0xC2C) != 1 && ST(0xC2C) != 2 && ST(0xC2C) != 3 && ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        CB->divers_empile(DV_PERSONA_ITEMS, P_KALHI13, OB_SM_THBOLT);
        CB->divers_empile(DV_PERSONA_ITEMS, P_MALROT8, OB_SM_TH_AMMO);
        CB->dialogue("L49D01", P_KALHI13);
        CB->dialogue("L49D02A", P_MALROT8);
        CB->divers_empile(DV_CINEMATIC, 1, 0);
        CB->animation("Sar_50");
        CB->evenement(0x22, 2000);
    }
    /* 0x7D38 */
    if (ST(0xC2C) == 2 && ST(ST_VIE(P_MALROT8)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L49D03A", P_MALROT8, "SP68GAR02", "SP68GAR02_10");
        CB->joindre(P_MALROT8, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L49D03B", P_MALROT8, "SP68GAR02", "SP68GAR02_10");
        lieu_action_register("L49D03C", P_MALROT8, "SP68GAR02", "SP68GAR02_10");
        CB->combat(P_MALROT8, 0);
        ST(V_ENGAGED_PERSONA) = P_MALROT8;
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7E02 */
    if (ST(0xC2C) == 3 && ST(ST_VIE(P_MALROT8)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->joindre(P_MALROT8, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L49D04", P_MALROT8, "SP68GAR02", "SP68GAR02_10_10");
        CB->combat(P_MALROT8, 0);
        ST(V_ENGAGED_PERSONA) = P_MALROT8;
    }
    /* 0x7E87 */
    if (ST(0x4DC) == 1 && ST(ST_VIE(P_DHORKAN11)) != VIE_DEAD) {
        ST(V_DLG_RANDOM) = CB->random(2);
        /* 0x7EC2 */
        if (ST(V_DLG_RANDOM) == 0) {
            lieu_action_register("L49D05", P_DHORKAN11, "SP11GAR02", "SP11GAR02_10");
        }
        /* 0x7EED */
        if (ST(V_DLG_RANDOM) == 1) {
            lieu_action_register("L49D06A", P_DHORKAN11, "SP11GAR02", "SP11GAR02_10");
            lieu_action_register("L49D06B", P_DHORKAN11, "SP11GAR02", "SP11GAR02_10");
        }
        CB->combat(P_DHORKAN11, 0);
        ST(V_ENGAGED_PERSONA) = P_DHORKAN11;
    }
}
