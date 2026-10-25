/*
 * lieu48_trig.c — Native hand-port of the LIEU48.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu48.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include "lieu48_int.h"

static void trig_noop(void);   /* the stub slots */
static void trig01(void);
static void trig02(void);
static void trig04(void);
static void trig05(void);
static void trig09(void);
static void trig10(void);
static void trig11(void);
static void trig12(void);
static void trig13(void);
static void trig14(void);
static void trig15(void);
static void trig16(void);
static void trig17(void);
static void trig30(void);
static void trig41(void);

/* dword_1001E330[42] — filled at INIT with the 5-byte jmp thunks; static here.
 * Slots 3, 6-8, 18-29, 31-40 are empty stubs (identical to L01 trig06). */
static void (*const g_trigger[42])(void) = {
    l48_trig00, trig01, trig02, trig_noop, trig04,
    trig05, trig_noop, trig_noop, trig_noop, trig09,
    trig10, trig11, trig12, trig13, trig14,
    trig15, trig16, trig17, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig30, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001E330]). */
void l48_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 — sub_100073F0 (thunk sub_10001073; INIT default) */
void l48_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(48)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 49;
        ST(0xF10) = 2;
    }
    /* 0x743E */
    if (ST(ST_START_POINT(48)) == 2) {
        ST(ST_HERO) = P_ARKHAN3;
        ST(ST_LEVEL) = 19;
        ST(0xBBC) = 8;
    }
    /* 0x747A */
    if (ST(ST_START_POINT(48)) == 3) {
        ST(0xF10) = 1;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 19;
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x6C) = 0;
    ST(0x74) = 0;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    ST(0x80) = 0;
    ST(0x84) = 0;
    ST(0x88) = 0;
    ST(V_EVENT15_STEP) = 0;
    ST(0x90) = 0;
    ST(0x94) = 0;
    ST(0x98) = 0;
    ST(0xB8) = 0;
    ST(0xBC) = 0;
    ST(0xD4) = 0;
    ST(0xD8) = 0;
    ST(0xDC) = 0;
    ST(0xF0) = 0;
    ST(V_WEAPON_WARNED) = 0;
    ST(0x4C) = 0;
    ST(0x50) = 0;
    ST(0x54) = 0;
    ST(0x58) = 0;
    CB->select_pile(0, 1);
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    CB->vire_col(10, 1);
    CB->vire_col(16, 0);
    CB->ajoute_zone(1, 2080, 1930, 140, 80, 1);
    CB->ajoute_zone(2, 2040, 3950, 160, 90, 1);
    CB->ajoute_zone(4, 2420, 3700, 90, 350, 1);
    CB->ajoute_zone(9, 1923, 2046, 105, 135, 0);
    CB->ajoute_zone(14, 2020, 2770, 85, 1400, 0);
    CB->ajoute_zone(15, 2186, 2670, 85, 1245, 0);
    CB->ajoute_zone(16, 1880, 3550, 370, 410, 0);
    /* 0x7759 */
    if (ST(ST_LEVEL) != 30) {
        CB->ajoute_zone(17, 1637, 3790, 85, 200, 1);
    }
    CB->ajoute_zone(30, 1713, 4250, 190, 460, 1);
    CB->bruitage("L48AMB1", -1, -1);
    /* 0x77B9 */
    if (ST(ST_LEVEL) == 18) {
        ST(0x68) = 1;
        CB->pos_perso(ST(ST_HERO), 2100, 2016, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
    }
    /* 0x7813 */
    if (ST(ST_LEVEL) == 19) {
        CB->pos_perso(ST(ST_HERO), 2300, 3684, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
    }
    /* 0x7861 */
    if (ST(ST_LEVEL) == 30) {
        CB->pos_perso(ST(ST_HERO), 1484, 4085, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        CB->ajoute_zone(10, 1600, 3800, 100, 90, 1);
    }
    /* 0x78CA */
    if (ST(ST_LEVEL) == 49) {
        CB->pos_perso(ST(ST_HERO), 2026, 3830, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
    }
    /* 0x791B */
    if (ST(0xE8C) == 2) {
        ST(0xE8C) = 0xA;
    }
    /* 0x793C */
    if (ST(ST_VIE(P_MALROT7)) < 2 && ST(0xBBC) != 8 && ST(0x68) != 10) {
        /* 0x796F */
        if (ST(0xF10) < 2) {
            CB->pos_perso(P_MALROT7, 2281, 1751, 0x12C, 1, "P67ARR01", 0xD, 0);
            /* 0x79A6 */
            if (ST(0xF10) == 1) {
                ST(0x68) = 1;
                ST(0x6C) = 1;
            }
        }
        /* 0x79D0 */
        if (ST(0xF10) == 2) {
            CB->vire_col(-2, 0);
            ST(0x98) = 1;
            ST(0x68) = 1;
            ST(0x74) = 1;
            /* 0x7A13 */
            if (ST(ST_LEVEL) == 18) {
                CB->pos_perso(P_MALROT7, 2093, 2170, 0x8CA0, 1, "P67GAR02", 0xD, 0);
            }
            /* 0x7A47 */
            if (ST(ST_LEVEL) == 19 || ST(ST_LEVEL) == 49) {
                CB->pos_perso(P_MALROT7, 2026, 3630, CB->divers(DV_DESC_POS, P_MALROT7, 2), 1, "P67GAR02", 0xD, 0);
            }
            /* 0x7A98 */
            if (ST(ST_LEVEL) == 30) {
                ST(0xBC) = 1;
                CB->pos_perso(P_MALROT7, 2026, 3630, CB->divers(DV_DESC_POS, P_MALROT7, 2), 1, "P67GAR02", 0xD, 0);
            }
            CB->combat(P_MALROT7, 0);
            ST(V_ENGAGED_PERSONA) |= 0x6700;
        }
    }
    /* 0x7B05 */
    if (ST(ST_VIE(P_MALROT7)) >= 2 && ST(0xBBC) != 8 && ST(0xBBC) != 10) {
        CB->pos_perso(P_MALROT7, 0, 0, 0, 2, "P67cad", 0xD, 0);
    }
    /* 0x7B57 */
    if (ST(0xBBC) == 8 && ST(0xC30) == 0 && ST(0x68) != 10) {
        ST(0x68) = 1;
        CB->pos_perso(P_KALHI13, 2060, 3434, 0xFA0, 1, "P13GAR02", 0xC, 0);
        CB->pos_perso(P_MALROT3, 1963, 3563, 0xABE0, 1, "P63GAR02", 0xB, 0);
    }
    /* 0x7BE0 */
    if (ST(0xBBC) == 8 && ST(0xC30) == 1 && ST(0x68) != 10 && ST(ST_VIE(P_MALROT3)) == 1) {
        ST(0x68) = 1;
        CB->pos_perso(P_KALHI13, 2060, 3434, 0xFA0, 1, "P13GAR02", 0xC, 0);
        CB->pos_perso(P_MALROT3, 1963, 3563, 0xABE0, 1, "P63GAR02", 0xB, 0);
    }
    /* 0x7C7B */
    if (ST(0xBBC) == 8 && ST(0xC30) >= 2 && ST(0x68) != 10 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0x68) = 1;
        ST(ST_VIE(P_MALROT3)) = VIE_DEAD;
        CB->pos_perso(P_KALHI13, 0, 0, 0, 1, "P13ARR01", 0xC, 0);
        CB->link_objet(0xD, "OB73", 0x1B, 1);
        CB->pos_perso(P_MALROT3, 0, 0, 0, 2, "P63cad", 0xB, 0);
    }
    /* 0x7D27 */
    if (ST(0xBBC) == 8 && ST(0xC30) >= 2 && ST(0x68) != 10 && ST(ST_VIE(P_KALHI13)) == VIE_DEAD && ST(ST_VIE(P_MALROT3)) == 1) {
        ST(0x68) = 1;
        CB->pos_perso(P_MALROT3, 1963, 3563, 0xABE0, 1, "P63GAR02", 0xB, 0);
        CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13cad", 0xC, 0);
    }
    /* 0x7DCB */
    if (ST(0xBBC) == 9 && ST(ST_VIE(P_MALROT3)) == VIE_DEAD) {
        CB->pos_perso(P_MALROT3, 0, 0, 0, 2, "P63cad", 0xB, 0);
    }
    /* 0x7E0B */
    if (ST(0xBBC) == 20 && ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
        CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13cad", 0xC, 0);
    }
    CB->divers(DV_MUSIC, 1, 0);
    ST(ST_START_POINT(48)) = 0;
}

/* trigger 1 — sub_1000A00A */
static void trig01(void) {
    CB->select_pile(0, 2);
    ST(ST_LEVEL) = 48;
    CB->divers(DV_MUSIC, 1, 4);
    CB->retire_zone(1);
    /* 0xA04F */
    if ((ST(V_ENGAGED_PERSONA) & 0xFF) == 0x13 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xBBC) = 20;
        ST(0xC30) = 10;
        CB->bloque(BLOQUE_ALL);
        CB->combat(P_KALHI13, -1);
        ST(V_ENGAGED_PERSONA) &= 0xFF00;
        CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L48D103", P_KALHI13, "SP13DIA60", "SP13GAR27");
        lieu_action_register("L48D104", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
        CB->bloque(BLOQUE_ALL);
        CB->animation("Sar_10");
    }
    /* 0xA11D */
    if (ST(ST_VIE(P_MALROT3)) > 2 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xBBC) = 9;
        ST(0xC30) = 10;
        ST(ST_VIE(P_KALHI13)) = 4;
    }
    CB->animation("Sar_10");
    CB->change_piece(18, -1);
}

/* trigger 2 — sub_1000A182 */
static void trig02(void) {
    CB->select_pile(0, 2);
    ST(ST_LEVEL) = 48;
    CB->divers(DV_MUSIC, 1, 4);
    CB->retire_zone(2);
    /* 0xA1C7 */
    if ((ST(V_ENGAGED_PERSONA) & 0xFF) == 0x13 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xBBC) = 20;
        ST(0xC30) = 10;
        CB->bloque(BLOQUE_ALL);
        CB->combat(P_KALHI13, -1);
        ST(V_ENGAGED_PERSONA) &= 0xFF00;
        CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L48D103", P_KALHI13, "SP13DIA60", "SP13GAR27");
        lieu_action_register("L48D104", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
    }
    /* 0xA27F */
    if (ST(ST_VIE(P_MALROT3)) > 2 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xBBC) = 9;
        ST(0xC30) = 10;
        ST(ST_VIE(P_KALHI13)) = 4;
    }
    CB->animation("Sar_10");
    CB->change_piece(49, -1);
}

/* trigger 4 — sub_1000A2EF */
static void trig04(void) {
    CB->select_pile(0, 2);
    CB->retire_zone(4);
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 48;
    /* 0xA334 */
    if ((ST(V_ENGAGED_PERSONA) & 0xFF) == 0x13 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xBBC) = 20;
        ST(0xC30) = 10;
        CB->bloque(BLOQUE_ALL);
        CB->combat(P_KALHI13, -1);
        ST(V_ENGAGED_PERSONA) &= 0xFF00;
        CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L48D103", P_KALHI13, "SP13DIA60", "SP13GAR27");
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI13);
        lieu_action_register("L48D104", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
    }
    /* 0xA409 */
    if (ST(ST_VIE(P_MALROT3)) > 2 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xBBC) = 9;
        ST(0xC30) = 10;
        ST(ST_VIE(P_KALHI13)) = 4;
    }
    CB->animation("Sar_10");
    CB->change_piece(19, -1);
}

/* trigger 5 — sub_1000A46E: just calls trigger 2 */
static void trig05(void) {
    trig02();
}

/* trigger 9 — sub_1000A49F */
static void trig09(void) {
    /* 0xA4AE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x3E80)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE02");
        /* 0xA504 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD10", P_ARKHAN2);
        }
        /* 0xA526 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD11", P_ARKHAN3);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 10 — sub_1000A56A */
static void trig10(void) {
    /* 0xA579 */
    if (ST(ST_UPDOWN_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_spec(0xAFC8) || CB->direction_spec(0xD6D8))) {
        CB->select_pile(0, 2);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->vire_col(10, 0);
        CB->retire_zone(10);
        /* 0xA5FC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(ST(ST_HERO), 1530, 3800, 0xBB80, 1, "ar", 6, 0);
            /* 0xA641 */
            if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                CB->animation("SP02ATT23");
            }
            /* 0xA666 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                CB->animation("SP02ATT24");
            }
            CB->animation("P02_65535");
            CB->pos_perso(ST(ST_HERO), 1650, 3800, 0xBB80, 1, "ar", 6, 0);
        }
        /* 0xA6B9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(ST(ST_HERO), 1510, 3800, 0xBB80, 1, "ar", 6, 0);
            /* 0xA6FE */
            if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                CB->animation("SP03ATT23");
            }
            /* 0xA723 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                CB->animation("SP03ATT24");
            }
            CB->animation("P03_65535");
            CB->pos_perso(ST(ST_HERO), 1650, 3800, 0xBB80, 1, "ar", 6, 0);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->ajoute_zone(17, 1637, 3790, 85, 200, 1);
        CB->vire_col(10, 1);
        CB->bloque(BLOQUE_NONE);
        /* 0xA7C5 */
        if (((ST(0x64) >> 8) & 0xFF) == 0x63 && ST(ST_VIE(P_MALROT3)) == 1) {
            CB->combat(P_MALROT3, 0);
            ST(V_ENGAGED_PERSONA) |= 0x6300;
        }
        /* 0xA7FF */
        if ((ST(0x64) & 0xFF) == 0x13 && ST(ST_VIE(P_KALHI13)) == 1) {
            CB->combat(P_KALHI13, 0);
            ST(V_ENGAGED_PERSONA) |= 0x13;
        }
        /* 0xA836 */
        if (((ST(0x64) >> 8) & 0xFF) == 0x67 && ST(ST_VIE(P_MALROT7)) == 1) {
            /* 0xA863 */
            if (CB->distance(6, 13) < 300) {
                CB->pos_perso(P_MALROT7, 1771, 3746, 0x4536, 1, "P67GAR02", 0xD, 0);
            }
            CB->modify_var(&ST(0xBC), 0);
            CB->combat(P_MALROT7, 0);
            ST(V_ENGAGED_PERSONA) |= 0x6700;
        }
        CB->modify_var(&ST(0x64), 0);
        CB->change_camera(1, CAM_AUTO);
        CB->select_pile(0, 1);
    }
}

/* trigger 11 — sub_1000B3FA */
static void trig11(void) {
    /* 0xB409 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_VIE(P_MALROT3)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->etat_objet("SOB73", -1, -1, -1, -1, -1, -1, -4, 0);
        CB->fouille(P_MALROT3);
        return;
    }
    /* 0xB47A */
    if (CB->direction_perso(P_MALROT3)) {
        /* 0xB489 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_MALROT3)) == 1) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_MALROT3);
            CB->joindre(-ST(ST_HERO), 0, 0, 0, P_MALROT3);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xB517 */
            if (ST(0xC30) == 1) {
                ST(V_EVENT_0) = 0;
                CB->evenement(0, -1);
            }
            /* 0xB545 */
            if (ST(0xC30) == 2) {
                CB->divers_empile(DV_CINEMATIC, 0, 0);
                CB->animation("SP63GAR02_10");
                CB->animation("SP13GAR02_10");
            }
            /* 0xB57C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xB58E */
                if (ST(0x94) == 0) {
                    ST(0x94) = 1;
                    /* 0xB5AF */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L48D33A", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L48D33B", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0xB5ED */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L48D34", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L48D35", P_MALROT3, "SP63DIA54", "SP63GAR02_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    /* 0xB647 */
                    if (ST(0xC30) == 1) {
                        CB->evenement(0, 3000);
                    }
                    /* 0xB666 */
                    if (ST(0xC30) == 2) {
                        CB->divers_empile(DV_CINEMATIC, 1, 0);
                    }
                    return;
                }
                /* 0xB68F */
                if (ST(0x94) == 1) {
                    /* 0xB6A1 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L48D36A", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L48D36B", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0xB6DF */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L48D37", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L48D38", P_MALROT3, "SP63DIA54", "SP63GAR02_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    /* 0xB739 */
                    if (ST(0xC30) == 1) {
                        CB->evenement(0, 3000);
                    }
                    /* 0xB758 */
                    if (ST(0xC30) == 2) {
                        CB->divers_empile(DV_CINEMATIC, 1, 0);
                    }
                    return;
                }
            }
            /* 0xB781 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                ST(0xBBC) = 9;
                ST(0xC30) = 100;
                ST(V_WEAPON_WARNED) = 1;
                /* 0xB7C0 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L48D39", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0xB7E8 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L48D40", P_ARKHAN3, "SG00DIA02", NULL);
                }
                CB->joindre(-P_MALROT3, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L48D41", P_MALROT3, "SP63DIA60", "SP63GAR02_10");
                CB->animation("Sar_10");
                CB->animation("P63GAR27");
                CB->animation("G00PRE80");
                CB->etat_objet("SOB73", -1, -1, -1, -1, -1, -1, -4, 0);
                lieu_action_register("L48D42", P_MALROT3, "SP63DIA54", "SP63GAR02_10");
                CB->divers(DV_RUN, P_MALROT3, 1);
                CB->joindre(-P_MALROT3, 2032, 4140, 0x1B58, -1);
                CB->modify_var(&ST(0x84), 1);
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI13);
                lieu_action_register("L48D56", P_KALHI13, "SP13DIA54", "SP13GAR02_10");
                CB->divers(DV_RUN, P_KALHI13, 1);
                CB->change_camera(-1, CAM_LOCKED);
                CB->joindre(-P_KALHI13, 2670, 3650, 0x1068, -1);
                CB->modify_var(&ST(0x88), 1);
                return;
            }
        }
        /* 0xB98D */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) != 0 && ST(ST_SCRIPT_BUSY) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xB9E3 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xB9F5 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD20", P_ARKHAN2);
                }
                /* 0xBA17 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD21", P_ARKHAN3);
                }
            }
            /* 0xBA39 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xBA4B */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD61", P_ARKHAN2);
                }
                /* 0xBA6D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD63", P_ARKHAN3);
                }
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
    /* 0xBAB0 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_VIE(P_MALROT3)) == 1 && ((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) == 0 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && ST(0x54) == 0 && ST(ST_VIE(P_MALROT3)) == 1) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_MALROT3);
        ST(V_PLAYER_STRIKE) = -1;
        /* 0xBB53 */
        if (ST(0xC30) == 2) {
            CB->divers_empile(DV_CINEMATIC, 0, 0);
            CB->animation("SP63GAR02_10");
            CB->animation("SP13GAR02_10");
        }
        /* 0xBB8A */
        if (ST(0xC30) == 1) {
            ST(0xC30) = 2;
            ST(V_EVENT_0) = 0;
            CB->evenement(0, -1);
        }
        CB->animation("SP63ESA14");
        CB->animation("P63GAR02_10");
        CB->dialogue("L48D57", P_MALROT3);
        CB->animation("Sar_10");
        CB->animation("SP63GAR02_10");
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_CINEMATIC, 1, 0);
        CB->combat(P_MALROT3, 0);
        ST(V_ENGAGED_PERSONA) |= 0x6300;
        return;
    }
}

/* trigger 12 — sub_1000A8EE */
static void trig12(void) {
    /* 0xA900 */
    if (ST(0xBBC) == 9) {
        return;
    }
    /* 0xA917 */
    if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        /* 0xA956 */
        if (ST(0xC30) == 3) {
            CB->etat_objet("SOB73", -1, -1, -1, -1, -1, -1, -4, 0);
        }
        CB->fouille(P_KALHI13);
        return;
    }
    /* 0xA997 */
    if (CB->direction_perso(P_KALHI13)) {
        /* 0xA9A6 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_KALHI13)) == 1) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_KALHI13);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI13);
            /* 0xAA34 */
            if (ST(0xC30) == 1) {
                ST(V_EVENT_0) = 0;
                CB->evenement(0, -1);
            }
            /* 0xAA62 */
            if (ST(0xC30) == 2) {
                CB->divers_empile(DV_CINEMATIC, 0, 0);
                /* 0xAA83 */
                if (ST(ST_VIE(P_MALROT3)) == 1) {
                    CB->animation("SP63GAR02_10");
                }
                CB->animation("SP13GAR02_10");
            }
            /* 0xAAAB */
            if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (!CB->divers(DV_CAN_FIGHT, 0, 2) && ST(0x7C) == 1)) {
                /* 0xAAFA */
                if (ST(0xC30) == 1) {
                    /* 0xAB0C */
                    if (ST(0x90) == 0) {
                        ST(0x90) = 1;
                        /* 0xAB2D */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            /* 0xAB3F */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L48D17", P_ARKHAN2, "SG00DIA02", NULL);
                            }
                            /* 0xAB67 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L48D18", P_ARKHAN2, "SG00DIA02", NULL);
                            }
                        }
                        /* 0xAB8F */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            /* 0xABA1 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L48D19", P_ARKHAN3, "SG00DIA02", NULL);
                            }
                            /* 0xABC9 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L48D20", P_ARKHAN3, "SG00DIA02", NULL);
                            }
                        }
                        lieu_action_register("L48D21A", P_KALHI13, "SP13DIA54", "SP13GAR02_10");
                        lieu_action_register("L48D21B", P_KALHI13, "SP13DIA54", "SP13GAR02_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->evenement(0, 3000);
                        return;
                    }
                    /* 0xAC4E */
                    if (ST(0x90) == 1) {
                        /* 0xAC60 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            /* 0xAC72 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L48D22", P_ARKHAN2, "SG00DIA02", NULL);
                            }
                            /* 0xAC9A */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L48D23", P_ARKHAN2, "SG00DIA02", NULL);
                            }
                        }
                        /* 0xACC2 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            /* 0xACD4 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L48D24", P_ARKHAN3, "SG00DIA02", NULL);
                            }
                            /* 0xACFC */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L48D25", P_ARKHAN3, "SG00DIA02", NULL);
                            }
                        }
                        lieu_action_register("L48D26", P_KALHI13, "SP13DIA54", "SP13GAR02_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->evenement(0, 3000);
                        return;
                    }
                }
                /* 0xAD68 */
                if (ST(0xC30) == 2) {
                    CB->divers_empile(DV_CINEMATIC, 0, 0);
                    /* 0xAD89 */
                    if (ST(ST_VIE(P_MALROT3)) == 1) {
                        CB->animation("SP63GAR02_10");
                    }
                    CB->animation("SP13GAR02_10");
                    /* 0xADB1 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0xADC3 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L48D27", P_ARKHAN2, "SG00DIA02", NULL);
                        }
                        /* 0xADEB */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L48D28", P_ARKHAN2, "SG00DIA02", NULL);
                        }
                    }
                    /* 0xAE13 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0xAE25 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L48D29", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                        /* 0xAE4D */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L48D30", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                    }
                    CB->dialogue("L48D31", P_KALHI13);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->divers_empile(DV_CINEMATIC, 1, 0);
                    return;
                }
            }
            /* 0xAEB5 */
            if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(0x7C) == 0) {
                ST(0x7C) = 1;
                /* 0xAEF9 */
                if (ST(0xC30) == 2) {
                    CB->divers_empile(DV_CINEMATIC, 0, 0);
                    /* 0xAF1A */
                    if (ST(ST_VIE(P_MALROT3)) == 1) {
                        CB->animation("SP63GAR02_10");
                    }
                    CB->animation("SP13GAR02_10");
                }
                /* 0xAF42 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81_10");
                }
                /* 0xAF6D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81_10");
                }
                ST(V_DLG_RANDOM) = CB->random(3);
                CB->dialogue("L48D32", P_KALHI13);
                CB->animation("SG00PRE82_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                /* 0xAFE3 */
                if (ST(0xC30) == 2) {
                    CB->divers_empile(DV_CINEMATIC, 1, 0);
                }
                return;
            }
        }
        /* 0xB009 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && (ST(V_ENGAGED_PERSONA) & 0xFF) == 0x13 && ST(ST_VIE(P_KALHI13)) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xB071 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L48D100", P_ARKHAN2);
            }
            /* 0xB093 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L48D101", P_ARKHAN3);
            }
            CB->bloque(BLOQUE_NONE);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->animation("Sar_10");
        }
        /* 0xB0D6 */
        if (ST(V_PLAYER_STRIKE) >= 1 && (ST(V_ENGAGED_PERSONA) == 0 || ((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) != 0) && ST(ST_VIE(P_KALHI13)) == 1 && ST(0xBBC) != 9 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 12)) {
            CB->select_pile(0, 2);
            ST(V_EVENT_0) = 0;
            CB->bloque(BLOQUE_ALL);
            ST(V_PLAYER_STRIKE) = -1;
            /* 0xB17D */
            if (ST(0xC30) == 1) {
                ST(V_EVENT_0) = 0;
                CB->evenement(0, -1);
            }
            /* 0xB1AB */
            if (ST(0xC30) == 2) {
                CB->divers_empile(DV_CINEMATIC, 0, 0);
                /* 0xB1CC */
                if (ST(ST_VIE(P_MALROT3)) == 1) {
                    CB->animation("SP63GAR02_10");
                }
                CB->animation("SP13GAR02_10");
            }
            /* 0xB1F1 */
            if (((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) == 0x63) {
                CB->divers_empile(DV_CINEMATIC, 0, 0);
                CB->combat(P_MALROT3, -1);
                ST(V_ENGAGED_PERSONA) &= 0xFF;
                CB->animation("SP13GAR02_10");
                /* 0xB239 */
                if (ST(ST_VIE(P_MALROT3)) == 1) {
                    CB->animation("SP63GAR02_10");
                }
                CB->animation("Sar_10");
            }
            /* 0xB25E */
            if (ST(V_ENGAGED_PERSONA) == 0) {
                CB->animation("SP13ESA15");
                CB->animation("P13GAR02_10");
                CB->animation("Sar_10");
            }
            CB->joindre_perm(-P_KALHI13, ST(ST_HERO));
            lieu_action_register("L48D102", P_KALHI13, "SP13DIA60", "SP13GAR02_10");
            /* 0xB2C0 */
            if (ST(ST_VIE(P_MALROT3)) == 1 && (ST(V_ENGAGED_PERSONA) & 0xFF) != 0x13) {
                CB->bloque(BLOQUE_ALL);
                CB->divers_empile(DV_CINEMATIC, 0, 0);
                CB->combat(P_MALROT3, -1);
                ST(V_ENGAGED_PERSONA) &= 0xFF;
                lieu_action_register("L48D105", P_MALROT3, "SP63DIA54", NULL);
                CB->animation("P63ARR01_10");
                CB->divers(DV_RUN, P_MALROT3, 1);
                CB->joindre(-P_MALROT3, 2032, 4140, 0x1B58, -1);
                CB->modify_var(&ST(0x84), 1);
                CB->combat(P_KALHI13, 0);
                ST(V_ENGAGED_PERSONA) |= 0x13;
                ST(0x54) = 1;
                CB->evenement(2, 3000);
            }
            /* 0xB3AF */
            if (ST(ST_VIE(P_MALROT3)) != 1 && ST(ST_VIE(P_KALHI13)) == 1) {
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                CB->combat(P_KALHI13, 0);
                ST(V_ENGAGED_PERSONA) |= 0x13;
            }
            return;
        }
    }
}

/* trigger 13 — sub_1000BC34 */
static void trig13(void) {
    /* 0xBC43 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_VIE(P_MALROT7)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->fouille(P_MALROT7);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xBCCD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_MALROT7)) == 1 && CB->direction_perso(P_MALROT7) && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBD39 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xBD4B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0xBD6D */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD61", P_ARKHAN2);
            }
        }
        /* 0xBD8F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xBDA1 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
            /* 0xBDC3 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD22D", P_ARKHAN3);
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xBE06 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_VIE(P_MALROT7)) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(0xF10) != 0) {
        CB->select_pile(0, 2);
        ST(V_PLAYER_STRIKE) = -1;
        ST(0xF10) = 2;
        ST(V_ENGAGED_PERSONA) |= 0x6700;
        CB->combat(P_MALROT7, 0);
        CB->select_pile(0, 1);
    }
}

/* trigger 14 — sub_1000BE92 */
static void trig14(void) {
    /* 0xBEA1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x4A38)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xBF0A */
        if (ST(ST_PLAYER_WEAPON) != 40 && ST(ST_PLAYER_WEAPON) != 41) {
            /* 0xBF2E */
            if (ST(0xDC) == 1) {
                /* 0xBF40 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L48D668", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0xBF68 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L48D669", P_ARKHAN3, "SG00DIA05", NULL);
                }
            }
            /* 0xBF90 */
            if (ST(0xDC) == 0) {
                ST(0xDC) = 1;
                /* 0xBFB1 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L48D666", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xBFD9 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L48D667", P_ARKHAN3, "SG00DIA06", NULL);
                }
            }
        } else {
            /* 0xC006 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD86", P_ARKHAN2, "SG00DIA06", NULL);
            }
            /* 0xC02E */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD87", P_ARKHAN3, "SG00DIA06", NULL);
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 — sub_1000C078 */
static void trig15(void) {
    /* 0xC087 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xCB20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xC0F0 */
        if (ST(ST_PLAYER_WEAPON) != 40 && ST(ST_PLAYER_WEAPON) != 41) {
            /* 0xC114 */
            if (ST(0xDC) == 1) {
                /* 0xC126 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L48D668", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0xC14E */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L48D669", P_ARKHAN3, "SG00DIA05", NULL);
                }
            }
            /* 0xC176 */
            if (ST(0xDC) == 0) {
                ST(0xDC) = 1;
                /* 0xC197 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L48D666", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xC1BF */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L48D667", P_ARKHAN3, "SG00DIA06", NULL);
                }
            }
        } else {
            /* 0xC1EC */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD86", P_ARKHAN2, "SG00DIA06", NULL);
            }
            /* 0xC214 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD87", P_ARKHAN3, "SG00DIA06", NULL);
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 — sub_1000C25E */
static void trig16(void) {
    /* 0xC26D */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_spec(0x4A38) || CB->direction_spec(0x59D8))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xC2E9 */
        if (ST(ST_PLAYER_WEAPON) != 40 && ST(ST_PLAYER_WEAPON) != 41) {
            /* 0xC30D */
            if (ST(0xDC) == 1) {
                /* 0xC31F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L48D668", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0xC347 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L48D669", P_ARKHAN3, "SG00DIA05", NULL);
                }
            }
            /* 0xC36F */
            if (ST(0xDC) == 0) {
                ST(0xDC) = 1;
                /* 0xC390 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L48D666", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xC3B8 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L48D667", P_ARKHAN3, "SG00DIA06", NULL);
                }
            }
        } else {
            /* 0xC3E5 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD86", P_ARKHAN2, "SG00DIA06", NULL);
            }
            /* 0xC40D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD87", P_ARKHAN3, "SG00DIA06", NULL);
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 17 — sub_1000C457 */
static void trig17(void) {
    /* 0xC466 */
    if (ST(ST_UPDOWN_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_spec(0x6D60) || CB->direction_spec(0x2EE0))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xC4C4 */
        if (ST(0xF0C) == 0) {
            ST(0xF0C) = 1;
            CB->vire_col(10, 0);
        }
        CB->change_camera(1, CAM_LOCKED);
        CB->evenement(1, 1300);
        CB->pos_perso(ST(ST_HERO), 1637, 3790, 0x4A38, 1, "ar", 6, 0);
        /* 0xC53D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xC557 */
            if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                CB->animation("SP02ATT21");
            }
            /* 0xC57C */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                CB->animation("SP02ATT22");
            }
            CB->animation("P02_65535");
        }
        /* 0xC5A4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xC5BE */
            if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                CB->animation("SP03ATT21");
            }
            /* 0xC5E3 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                CB->animation("SP03ATT22");
            }
            CB->animation("P03_65535");
        }
        CB->vire_col(10, 1);
        CB->animation("Sar_10");
        CB->retire_zone(17);
        CB->ajoute_zone(10, 1600, 3800, 100, 90, 1);
        /* 0xC649 */
        if (((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) == 0x67 && ST(ST_VIE(P_MALROT7)) == 1) {
            ST(0x64) = ST(V_ENGAGED_PERSONA);
            CB->combat(P_MALROT7, -1);
            ST(V_ENGAGED_PERSONA) &= 0xFF;
            CB->stop_joindre(-P_MALROT7);
            CB->animation("SP67GAR02_10");
            ST(0xBC) = 1;
            CB->joindre_perm(P_MALROT7, ST(ST_HERO));
        }
        /* 0xC6D2 */
        if ((ST(V_ENGAGED_PERSONA) & 0xFF) == 0x13 && ST(ST_VIE(P_KALHI13)) == 1) {
            ST(0x64) = ST(V_ENGAGED_PERSONA);
            CB->combat(P_KALHI13, -1);
            ST(V_ENGAGED_PERSONA) &= 0xFF00;
            CB->animation("SP13GAR02_10");
            CB->animation("Sar_10");
        }
        /* 0xC736 */
        if (((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) == 0x63 && ST(ST_VIE(P_MALROT3)) == 1) {
            ST(0x64) = ST(V_ENGAGED_PERSONA);
            CB->combat(P_MALROT3, -1);
            ST(V_ENGAGED_PERSONA) &= 0xFF;
            CB->animation("SP63GAR02_10");
            CB->animation("Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 30 — sub_1000C833 */
static void trig30(void) {
    CB->select_pile(0, 2);
    ST(ST_LEVEL) = 48;
    CB->retire_zone(30);
    CB->divers(DV_MUSIC, 1, 4);
    /* 0xC878 */
    if (((ST(V_ENGAGED_PERSONA) & 0xFF) == 0x13 && ST(ST_VIE(P_KALHI13)) == 1) || ((ST(0x64) & 0xFF) == 0x13 && ST(ST_VIE(P_KALHI13)) == 1)) {
        ST(0xBBC) = 20;
        ST(0xC30) = 10;
        CB->bloque(BLOQUE_ALL);
        CB->combat(P_KALHI13, -1);
        ST(V_ENGAGED_PERSONA) &= 0xFF00;
        CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L48D103", P_KALHI13, "SP13DIA60", "SP13GAR27");
        lieu_action_register("L48D104", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xC959 */
    if (ST(ST_VIE(P_MALROT3)) > 2 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xBBC) = 9;
        ST(0xC30) = 10;
        ST(ST_VIE(P_KALHI13)) = 4;
    }
    CB->animation("Sar_10");
    CB->change_piece(30, -1);
}

/* trigger 41 — sub_10007E65 */
static void trig41(void) {
    CB->divers_empile(DV_MUSIC, 1, 1);
    /* 0x7E89 */
    if (ST(0xE98) == 2 && ST(ST_VIE(P_KONKALITE5)) != VIE_DEAD && ST(ST_VIE(94)) != VIE_DEAD && ST(0xC5C) == 0) {
        CB->dialogue("L48D01", P_KONKALITE5);
        ST(0xC5C) = 1;
    }
    /* 0x7EF0 */
    if (ST(0xBBC) == 8 && ST(0xC30) == 2 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xC30) = 3;
    }
    /* 0x7F35 */
    if (ST(0xBBC) == 8 && ST(0xC30) == 1 && ST(ST_VIE(P_MALROT3)) == 1 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xC30) = 2;
        CB->divers(DV_CINEMATIC, 1, 2);
    }
    /* 0x7F9E */
    if (ST(0xBBC) == 8 && ST(0xC30) == 0 && ST(ST_VIE(P_MALROT3)) == 1 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0xC30) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-P_KALHI13, 2060, 3434, 0xFA0, -1);
        lieu_action_register("L48D04", P_KALHI13, "SP13DIA60", "SP13GAR02_10");
        lieu_action_register("L48D05", P_MALROT3, "SP63DIA60", "SP63GAR02_10");
        CB->evenement(0, 5000);
        CB->bloque(BLOQUE_NONE);
    }
}
