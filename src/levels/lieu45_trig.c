/*
 * lieu45_trig.c — Native hand-port of the LIEU45.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu45.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include "lieu45_int.h"

static void trig_noop(void);   /* the 34 stub slots (all identical to L01 trig06) */
static void trig01(void); static void trig02(void); static void trig03(void);
static void trig04(void); static void trig05(void); static void trig06(void);
static void trig08(void); static void trig10(void); static void trig11(void);
static void trig12(void); static void trig41(void);

/* dword_1001A610[42] — filled at INIT with the 5-byte jmp thunks; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  100098B9 stub  28  10009953 stub
 *    1  100081D8       15  100098C4 stub  29  1000995E stub
 *    2  100087CC       16  100098CF stub  30  10009969 stub
 *    3  100088DF       17  100098DA stub  31  10009974 stub
 *    4  10008CF0       18  100098E5 stub  32  1000997F stub
 *    5  10008FBC       19  100098F0 stub  33  1000998A stub
 *    6  1000928B       20  100098FB stub  34  10009995 stub
 *    7  100093B9 stub  21  10009906 stub  35  100099A0 stub
 *    8  100093C4       22  10009911 stub  36  100099AB stub
 *    9  1000959D stub  23  1000991C stub  37  100099B6 stub
 *   10  100095A8       24  10009927 stub  38  100099C1 stub
 *   11  1000973C       25  10009932 stub  39  100099CC stub
 *   12  10009763       26  1000993D stub  40  100099D7 stub
 *   13  100098AE stub  27  10009948 stub  41  10007A5C
 */
static void (*const g_trigger[42])(void) = {
    l45_trig00, trig01, trig02, trig03, trig04, trig05, trig06, trig_noop,
    trig08, trig_noop, trig10, trig11, trig12, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001A610]). */
void l45_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 — sub_100073F0 (thunk sub_10001073; INIT default) */
void l45_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(45)) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 3;
        ST(0x5B8) = 0;
    }
    /* 0x743E */
    if (ST(ST_START_POINT(45)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 3;
    }
    /* 0x746B */
    if (ST(ST_START_POINT(45)) == 3) {
        ST(V_DELIA_PHASE) = 9;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 3;
        ST(ST_VIE(P_DELIA)) = 1;
    }
    ST(V_ENGAGED_PERSONA) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    CB->ajoute_zone(11, -2287, -130, 100, 50, 1);
    CB->ajoute_zone(2, -1868, -387, 120, 80, 0);
    CB->ajoute_zone(3, -2033, -315, 100, 80, 0);
    CB->ajoute_zone(5, -1805, 100, 75, 230, 0);
    CB->ajoute_zone(6, -1805, -90, 80, 135, 0);
    CB->ajoute_zone(8, -1824, -211, 80, 100, 0);
    CB->ajoute_zone(9, -2037, -95, 90, 90, 0);
    CB->ajoute_zone(10, -1980, -105, 295, 290, 0);
    CB->ajoute_zone(12, -1805, -330, 70, 140, 0);
    CB->ajoute_zone(14, -2025, -420, 50, 50, 0);
    CB->etat_objet("IM66", -1790, 4, -463, 0xC000, 0x4000, 0, 0, 3);
    CB->etat_objet("IM23", -2030, 38, -393, 0, 0, 0, 0, 3);
    CB->bruitage("L45AMB1", -1, -1);
    /* 0x7699 */
    if (ST(ST_HERO) >= 2 && ST(0xB84) == 0) {
        ST(0xB84) = 1;
    }
    /* 0x76C9 */
    if (ST(ST_LEVEL) == 3) {
        CB->pos_perso(ST(ST_HERO), -1820, -366, 0x2CD, 1, "ar", 7, 0);
    }
    /* 0x7703 */
    if (ST(ST_LEVEL) == 5) {
        CB->pos_perso(ST(ST_HERO), -2284, -40, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 7, 0);
    }
    /* 0x7751 */
    if (ST(ST_VIE(P_DELIA)) != VIE_DEAD && (ST(V_DELIA_PHASE) == 9 || ST(V_DELIA_PHASE) == 10)) {
        ST(0x70) = 1;
        CB->pos_perso(P_DELIA, -1911, -373, 0x2CD, 1, "P10ARR01", 1, 0);
    }
    /* 0x77B8 */
    if (ST(V_DELIA_KILLED) == 45) {
        CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 2, "P10cad", 1, 0);
    }
    /* 0x781A */
    if (ST(0x5D8) == 0) {
        ST(0x140) = 3;
        CB->etat_objet("OB24A", -1744, 158, -216, 0x4000, 0, 0x53F8, -1, 3);
    }
    /* 0x7865 */
    if (ST(ST_HERO) >= 2) {
        /* 0x7877 */
        if (ST(0xB80) == 0) {
            CB->ajoute_zone(4, -2147, -232, 50, 150, 0);
            CB->etat_objet("OB08A", -2200, 20, -277, 0, 0, 0, -1, 0);
            CB->etat_objet("OB08B", -2200, 20, -244, 0, 0, 0, -1, 0);
            CB->etat_objet("OB08C", -2200, 20, -210, 0, 0, 0, -1, 0);
        }
        /* 0x790A */
        if (ST(0xB80) == 1) {
            CB->ajoute_zone(4, -2147, -232, 50, 150, 0);
            CB->etat_objet("OB08B", -2200, 20, -244, 0, 0, 0, -1, 0);
            CB->etat_objet("OB08C", -2200, 20, -210, 0, 0, 0, -1, 0);
        }
        /* 0x797C */
        if (ST(0xB80) == 2) {
            CB->ajoute_zone(4, -2147, -232, 50, 150, 0);
            CB->etat_objet("OB08C", -2200, 20, -210, 0, 0, 0, -1, 0);
        }
        /* 0x79CD */
        if (ST(0xB84) == 1) {
            CB->etat_objet("OB33A", -1750, 153, 95, 0, 0, 0, -1, 3);
            CB->etat_objet("OB33B", -1750, 153, 0, 0, 0, 0, -1, 3);
        }
        /* 0x7A21 */
        if (ST(0xB84) == 2) {
            CB->etat_objet("OB33B", -1750, 153, 0, 0, 0, 0, -1, 3);
        }
    }
    ST(ST_START_POINT(45)) = 0;
}

/* trigger 41 — sub_10007A5C */
static void trig41(void) {
    /* 0x7A79 */
    if (ST(V_SAVED_HERO) != ST(ST_HERO) && ST(V_DELIA_PHASE) == 10) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7AB4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7AC6 */
            if (ST(V_DELIA_PHASE) == 10) {
                CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD48", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            }
            /* 0x7B0E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD50", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0x7B36 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD51", P_ARKHAN3, "SG00DIA02", NULL);
            }
        }
        /* 0x7B5E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7B70 */
            if (ST(V_DELIA_PHASE) == 10) {
                CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD44", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            }
            /* 0x7BB8 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD46", P_ARKHAN2, "SG00DIA08", NULL);
            }
            /* 0x7BE0 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD47", P_ARKHAN3, "SG00DIA08", NULL);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 1 — sub_100081D8 */
static void trig01(void) {
    /* 0x81EB */
    if (CB->direction_perso(P_DELIA)) {
        /* 0x81FA */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
            /* 0x822A */
            if (ST(ST_VIE(P_DELIA)) < 2) {
                ST(0x70) = 10;
                CB->stop_joindre(-P_DELIA);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->bloque(BLOQUE_ALL);
                /* 0x826A */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0x82BF */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L45D15", P_ARKHAN2, "SG00DIA02", "Sar_10");
                    }
                    /* 0x82EA */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L45D16", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L45D17", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                }
                /* 0x834C */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0x83A3 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0x83CE */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L02D104", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                }
            }
            /* 0x8455 */
            if (ST(ST_VIE(P_DELIA)) == VIE_DEAD) {
                CB->fouille(P_DELIA);
            }
        }
        /* 0x8472 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_DELIA && ST(ST_VIE(P_DELIA)) == 1) {
            CB->bloque(BLOQUE_ALL);
            /* 0x84BC */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x84CE */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L45D01", P_ARKHAN2);
                }
                /* 0x84F0 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L45D02", P_ARKHAN2);
                }
            }
            /* 0x8512 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x8524 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L45D03", P_ARKHAN3);
                }
                /* 0x8546 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD22A", P_ARKHAN3);
                }
            }
            CB->bloque(BLOQUE_NONE);
        }
        /* 0x8570 */
        if (ST(V_PLAYER_STRIKE) > 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_DELIA)) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x70) = 10;
            ST(V_PLAYER_STRIKE) = -1;
            CB->select_pile(1, 2);
            CB->evenement(0, -1);
            CB->stop_joindre(-P_DELIA);
            ST(V_ENGAGED_PERSONA) = P_DELIA;
            CB->combat(P_DELIA, 0);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x863D */
        if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_DELIA)) == 1) {
            CB->select_pile(1, 2);
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x70) = 10;
            ST(V_PLAYER_STRIKE) = -1;
            CB->evenement(0, -1);
            CB->stop_joindre(-P_DELIA);
            /* 0x86DF */
            if (ST(V_DELIA_HITS) == 1) {
                CB->evenement(0, -1);
                ST(V_ENGAGED_PERSONA) = P_DELIA;
                CB->combat(P_DELIA, 0);
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->select_pile(0, 1);
                return;
            }
            /* 0x8742 */
            if (ST(V_DELIA_HITS) == 0) {
                ST(V_DELIA_HITS) = 1;
                CB->animation("SP10ESA01");
                CB->animation("P10GAR01_10");
                CB->dialogue("L45D04", P_DELIA);
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x70), 1);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->select_pile(0, 1);
                return;
            }
        }
    }
}

/* trigger 2 — sub_100087CC */
static void trig02(void) {
    /* 0x87DB */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_spec(0x8000) && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8826 */
        if (ST(V_DELIA_PHASE) == 10) {
            ST(V_SAVED_HERO) = ST(ST_HERO);
            /* 0x884F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L45D05", P_ARKHAN2);
            }
            /* 0x8871 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L45D06", P_ARKHAN3);
            }
        }
        CB->animation("SG00OUV01");
        CB->animation("P00_65535");
        CB->anim_objet("IM66", 3, -60, 1500, 0);
        ST(ST_LEVEL) = 45;
        CB->bloque(BLOQUE_NONE);
        CB->change_piece(3, -1);
    }
}

/* trigger 3 — sub_100088DF */
static void trig03(void) {
    /* 0x88EE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(14)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8939 */
        if (ST(ST_PLAYER_WEAPON) != 16) {
            /* 0x894B */
            if (ST(0x5B8) == 0 || ST(0x5B8) == 1) {
                /* 0x896F */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SG01OUV04");
                    CB->animation("G01OUV09");
                    CB->dialogue("LCOD12", P_ARKHAN1);
                }
                /* 0x89A7 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02OUV04");
                    CB->animation("G02OUV09");
                    CB->dialogue("LCOD13", P_ARKHAN2);
                }
                /* 0x89DF */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG03OUV04");
                    CB->animation("G03OUV09");
                    CB->dialogue("LCOD14", P_ARKHAN3);
                }
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8A40 */
            if (ST(0x5B8) == 2) {
                /* 0x8A52 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02OUV04");
                    CB->animation("G02OUV06");
                    CB->anim_objet("IM23", 1, -50, 1000, 0);
                    CB->animation("G02OUV07");
                }
                /* 0x8A9E */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG03OUV04");
                    CB->animation("G03OUV06");
                    CB->anim_objet("IM23", 1, -50, 1000, 0);
                    CB->animation("G03OUV07");
                }
                CB->evenement(2, 800);
                return;
            }
        }
        /* 0x8AFF */
        if (ST(ST_PLAYER_WEAPON) == 16) {
            /* 0x8B11 */
            if (ST(0x5B8) == 1) {
                ST(0x5B8) = 2;
                /* 0x8B32 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02OUV10");
                    CB->animation("G02OUV11");
                    CB->animation("P02_65535");
                    CB->anim_objet("IM23", 1, -54, 1000, 0);
                }
                /* 0x8B7E */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG02OUV10");
                    CB->animation("G03OUV11");
                    CB->animation("P03_65535");
                    CB->anim_objet("IM23", 1, -54, 1000, 0);
                }
                CB->animation("G00OUV05");
                CB->anim_objet("IM23", 1, 54, 550, 0x200);
                CB->animation("ar_20");
                CB->etat_objet("OB01", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8C38 */
            if (ST(0x5B8) == 2) {
                /* 0x8C4A */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02OUV04");
                    CB->animation("G02OUV06");
                    CB->anim_objet("IM23", 1, -50, 1000, 0);
                    CB->animation("G02OUV07");
                }
                /* 0x8C96 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG03OUV04");
                    CB->animation("G03OUV06");
                    CB->anim_objet("IM23", 1, -50, 1000, 0);
                    CB->animation("G03OUV07");
                }
                CB->evenement(2, 800);
                return;
            }
        }
    }
}

/* trigger 4 — sub_10008CF0 */
static void trig04(void) {
    /* 0x8CFF */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        /* 0x8D21 */
        if (CB->direction_spec(0x4650) && ST(ST_HERO) > 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x8D5C */
            if (ST(V_EVENT_9) == 0) {
                ST(V_EVENT_9) = 1;
                CB->evenement(9, 1200000);
            }
            /* 0x8D8D */
            if (ST(0xB80) == 0) {
                ST(0xB80) = 1;
                CB->animation("SG00PRE08");
                CB->bruitage("L45B03", -2017, -340);
                CB->etat_objet("OB08A", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8E15 */
            if (ST(0xB80) == 1) {
                ST(0xB80) = 2;
                CB->animation("SG00PRE08");
                CB->bruitage("L45B03", -2037, -340);
                CB->etat_objet("OB08B", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8E9D */
            if (ST(0xB80) == 2) {
                ST(0xB80) = 3;
                CB->animation("SG00PRE08");
                CB->bruitage("L45B03", -2057, -340);
                CB->etat_objet("OB08C", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8F25 */
            if (ST(0xB80) == 3) {
                /* 0x8F37 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L45D07", P_ARKHAN2, "SP02ATT60", "Sar_10");
                }
                /* 0x8F62 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L45D08", P_ARKHAN3, "SP03ATT60", "Sar_10");
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
            return;
        }
        /* 0x8FAC */
        if (CB->direction_zone(9)) {
            trig10();   /* call sub_1000109B -> sub_100095A8 */
        }
    }
}

/* trigger 5 — sub_10008FBC */
static void trig05(void) {
    /* 0x8FCB */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC738)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9016 */
        if (ST(0xB84) == 0) {
            ST(V_DLG_RANDOM) = CB->random(2);
            /* 0x903F */
            if (ST(V_DLG_RANDOM) == 0) {
                CB->animation("SG00PRE00");
            }
            /* 0x905C */
            if (ST(V_DLG_RANDOM) == 1) {
                CB->animation("SG00PRE01");
            }
            /* 0x9079 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("LCOD06", P_ARKHAN1);
            }
            /* 0x909B */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD07", P_ARKHAN2);
            }
            /* 0x90BD */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD08", P_ARKHAN3);
            }
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9108 */
        if (ST(0xB84) == 1) {
            ST(0xB84) = 2;
            CB->animation("SG00PRE06");
            CB->bruitage("L45B04", -1820, -20);
            CB->etat_objet("OB33A", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x918D */
        if (ST(0xB84) == 2) {
            ST(0xB84) = 3;
            CB->animation("SG00PRE06");
            CB->bruitage("L45B04", -1820, -40);
            CB->etat_objet("OB33B", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9212 */
        if (ST(0xB84) == 3) {
            /* 0x9224 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L45D200", P_ARKHAN2, "SG00DIA01", NULL);
            }
            /* 0x924C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L45D201", P_ARKHAN3, "SG00DIA01", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 6 — sub_1000928B */
static void trig06(void) {
    /* 0x929A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC738)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(V_DLG_RANDOM) = CB->random(2);
        /* 0x92FC */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->animation("SG00PRE00");
        }
        /* 0x9319 */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->animation("SG00PRE01");
        }
        /* 0x9336 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD06", P_ARKHAN1);
        }
        /* 0x9358 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD07", P_ARKHAN2);
        }
        /* 0x937A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD08", P_ARKHAN3);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 8 — sub_100093C4 */
static void trig08(void) {
    /* 0x93D3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC738)) {
        /* 0x9407 */
        if (ST(0x5D8) == 0) {
            ST(0x5D8) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->animation("SG00PRE05");
            ST(0x140) = 3;
            CB->etat_objet("OB24A", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x94A0 */
        if (ST(0x5D8) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(V_DLG_RANDOM) = CB->random(2);
            /* 0x94E0 */
            if (ST(V_DLG_RANDOM) == 0) {
                CB->animation("SG00PRE00");
            }
            /* 0x94FD */
            if (ST(V_DLG_RANDOM) == 1) {
                CB->animation("SG00PRE01");
            }
            /* 0x951A */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("LCOD06", P_ARKHAN1);
            }
            /* 0x953C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD07", P_ARKHAN2);
            }
            /* 0x955E */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD08", P_ARKHAN3);
            }
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
}

/* trigger 10 — sub_100095A8 (also called from trigger 4) */
static void trig10(void) {
    /* 0x95B7 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(9)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, -10);
        /* 0x961F */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x9631 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("L45D09A", P_ARKHAN1);
                CB->dialogue("L45D09B", P_ARKHAN1);
            }
            /* 0x9663 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L45D10", P_ARKHAN2);
            }
            /* 0x9685 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L45D11", P_ARKHAN3);
            }
        }
        /* 0x96A7 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x96B9 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("L45D12", P_ARKHAN1);
            }
            /* 0x96DB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L45D13", P_ARKHAN2);
            }
            /* 0x96FD */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L45D14", P_ARKHAN3);
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 11 — sub_1000973C */
static void trig11(void) {
    ST(ST_LEVEL) = 45;
    CB->change_piece(5, -1);
}

/* trigger 12 — sub_10009763 */
static void trig12(void) {
    /* 0x9772 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC738)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x97D4 */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->animation("SG00PRE00");
        }
        /* 0x97F1 */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->animation("SG00PRE01");
        }
        /* 0x980E */
        if (ST(V_DLG_RANDOM) == 2) {
            CB->animation("SG00PRE02");
        }
        /* 0x982B */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD06", P_ARKHAN1);
        }
        /* 0x984D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD07", P_ARKHAN2);
        }
        /* 0x986F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD08", P_ARKHAN3);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}
