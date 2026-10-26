/*
 * lieu51_trig.c — Native hand-port of the LIEU51.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu51.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "lieu51_int.h"

static void trig_noop(void);   /* the empty slots (identical to L01 trig06) */
static void trig01(void); static void trig02(void); static void trig03(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig12(void); static void trig23(void);
static void trig25(void); static void trig26(void); static void trig27(void);
static void trig28(void); static void trig31(void); static void trig38(void);
static void trig41(void);

/* dword_1001A730[42] — filled at INIT with the 5-byte jmp thunks; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000978F       28  10009C36
 *    1  10008A0F       15  1000979F       29  10009D6E
 *    2  10008A57       16  100097AF       30  10009D7E
 *    3  10008A9F       17  100097BF       31  10009D8E
 *    4  10008AD8 stub  18  100097CF       32  10009ED1
 *    5  10008AE3       19  100097DF       33  10009EE1
 *    6  10008FFD       20  100097EF       34  10009EF1
 *    7  10009036       21  100097FF       35  10009F01
 *    8  1000911B       22  1000980F       36  10009F11
 *    9  100091D9 stub  23  1000981F       37  10009F21
 *   10  100091E4 stub  24  10009874 stub  38  10009F31
 *   11  100091EF stub  25  1000987F       39  1000A013 stub
 *   12  100091FA       26  1000995A       40  1000A01E stub
 *   13  1000977F       27  10009A8A       41  10008013
 * Slots 13-22 forward to trig07, 29-30 to trig28, 32-34 to trig31 and
 * 35-37 to trig27 (16-byte `call` thunks, listed as stubs by the annotator).
 */
static void (*const g_trigger[42])(void) = {
    l51_trig00, trig01, trig02, trig03, trig_noop, trig05, trig06, trig07,
    trig08, trig_noop, trig_noop, trig_noop, trig12,
    trig07, trig07, trig07, trig07, trig07, trig07, trig07, trig07, trig07, trig07,
    trig23, trig_noop, trig25, trig26, trig27, trig28,
    trig28, trig28, trig31, trig31, trig31, trig31,
    trig27, trig27, trig27, trig38, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001A730]). */
void l51_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start points, zones, objects, actor placement */
void l51_trig00(void) {
    /* 0x73FB */
    if (ST(ST_START_POINT(51)) == 1) {
        ST(ST_LEVEL) = 10;
        ST(ST_HERO) = P_ARKHAN2;
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x88) = 0;
    ST(V_EVENT15_STEP) = 0;
    ST(0xF0) = 0;
    ST(0xF4) = 0;
    ST(0xF8) = 0;
    ST(0xFC) = 0;
    ST(0x100) = 0;
    ST(0x114) = 0;
    ST(0x54) = 0;
    ST(0x58) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_EVENT_33) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    CB->bruitage("L51AMB1", 4380, -1243);
    CB->divers(DV_MUSIC, 1, 0);
    CB->ajoute_zone(1, 2550, -1100, 60, 350, 1);
    CB->ajoute_zone(2, 2550, -1622, 60, 350, 1);
    CB->ajoute_zone(3, 5170, -2620, 70, 180, 1);
    CB->ajoute_zone(5, 5110, -1233, 245, 200, 0);
    CB->ajoute_zone(6, 4475, -3420, 500, 70, 1);
    CB->ajoute_zone(7, 6267, -1394, 115, 475, 0);
    CB->ajoute_zone(8, 6174, -1719, 165, 165, 0);
    CB->ajoute_zone(11, 4462, -1322, 500, 500, 0);
    CB->ajoute_zone(12, 5350, -1123, 245, 300, 0);
    CB->ajoute_zone(23, 5308, -995, 160, 260, 0);
    CB->ajoute_zone(24, 5265, -1138, 20, 20, 0);
    CB->ajoute_zone(25, 4460, -103, 370, 90, 1);
    CB->ajoute_zone(26, 4460, -103, 370, 90, 0);
    CB->ajoute_zone(27, 3920, -370, 300, 311, 0);
    CB->ajoute_zone(28, 3680, -540, 165, 165, 0);
    CB->ajoute_zone(29, 3595, -644, 165, 165, 0);
    CB->ajoute_zone(30, 3485, -704, 165, 165, 0);
    CB->ajoute_zone(31, 5400, -505, 335, 215, 0);
    CB->ajoute_zone(32, 5270, -390, 335, 215, 0);
    CB->ajoute_zone(33, 5166, -292, 335, 215, 0);
    CB->ajoute_zone(34, 5014, -212, 335, 215, 0);
    CB->ajoute_zone(35, 3040, -1860, 315, 340, 0);
    CB->ajoute_zone(36, 3475, -2022, 630, 385, 0);
    CB->ajoute_zone(37, 4676, -3090, 260, 260, 0);
    CB->ajoute_zone(38, 3400, -850, 100, 150, 0);
    ST(0xB4C) = 3;
    /* 0x788E */
    if (ST(0xF30) < 2) {
        CB->etat_objet("OB48", 5196, -782, -1170, 0x2A00, 0x6E00, 0xD2F8, -1, 0);
    }
    /* 0x78CD */
    if (ST(0xF30) == 2) {
        CB->etat_objet("OB29A", 5196, -782, -1170, 0x2A00, 0x6E00, 0xD2F8, -1, 0);
    }
    /* 0x7909 */
    if (ST(ST_LEVEL) == 8) {
        /* 0x791B */
        if (ST(0xB54) == 3) {
            CB->pos_perso(ST(ST_HERO), 2712, -1622, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
        }
        /* 0x796C */
        if (ST(0xB54) == 4) {
            CB->pos_perso(ST(ST_HERO), 2712, -1100, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
        }
    }
    /* 0x79BA */
    if (ST(ST_LEVEL) == 10) {
        CB->bloque(BLOQUE_ALL);
        ST(0x64) = 1;
        ST(0x54) = CB->divers(DV_DESC_POS, ST(ST_HERO), 0);
        ST(0x58) = CB->divers(DV_DESC_POS, ST(ST_HERO), 1);
        /* sub_1000BA50 is abs() */
        ST(0xF0) = abs(2550 - ST(0x54));
        ST(0xF8) = abs(5170 - ST(0x54));
        ST(0xFC) = abs(5650 - ST(0x54));
        ST(0x100) = abs(4475 - ST(0x54));
        ST(0x114) = abs(4475 - ST(0x54));
        /* 0x7AC8: F8 = min(F8, 114) */
        if (ST(0x114) < ST(0xF8)) {
            ST(0xF8) = ST(0x114);
        }
        /* 0x7AF5 */
        if (ST(0xF0) < ST(0xFC) && ((ST(0xF0) <= ST(0x100) && ST(0xF0) <= ST(0xF8)) || (ST(0x58) > -2200 && ST(0x58) < -1100))) {
            /* 0x7B70 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), 3440, -1455, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "P02PUI01", 0xA, 0);
            }
            /* 0x7BC1 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), 3440, -1455, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "P03PUI01", 0xA, 0);
            }
        } else if (ST(0xF0) >= ST(0xF8) && ST(0xFC) >= ST(0xF8) && ST(0x100) >= ST(0xF8) && ST(0x58) < -1980) {
            /* 0x7C80 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), 4477, -2530, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "P02PUI01", 0xA, 0);
            }
            /* 0x7CD1 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), 4477, -2530, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "P03PUI01", 0xA, 0);
            }
        } else if (ST(0xF0) > ST(0xFC) && ((ST(0xFC) <= ST(0xF8) && ST(0xFC) <= ST(0x100)) || (ST(0x58) < -600 && ST(0x58) > -2100))) {
            /* 0x7DA2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), 5550, -1135, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "P02PUI01", 0xA, 0);
            }
            /* 0x7DF3 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), 5550, -1135, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "P03PUI01", 0xA, 0);
            }
        } else if ((ST(0xF0) >= ST(0x100) && ST(0x100) <= ST(0xF8) && ST(0xFC) >= ST(0x100)) || ST(0x58) > -500) {
            /* 0x7EB2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), 4250, -425, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "P02PUI01", 0xA, 0);
            }
            /* 0x7F03 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), 4250, -425, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "P03PUI01", 0xA, 0);
            }
        }
    }
    /* 0x7F54 */
    if (ST(ST_LEVEL) == 20) {
        CB->pos_perso(ST(ST_HERO), 4475, -3300, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
    }
    /* 0x7FA2 */
    if (ST(ST_LEVEL) == 23) {
        CB->pos_perso(ST(ST_HERO), 5070, -2655, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
    }
    ST(0xB54) = 0;
    ST(ST_START_POINT(51)) = 0;
}

/* trigger 41 — sub_10008013 */
static void trig41(void) {
    CB->divers_empile(DV_MUSIC, 1, 1);
    /* 0x802B */
    if (ST(0x64) == 1) {
        CB->divers_empile(DV_SCREEN, 3, 0);
        ST(0x64) = 0;
        /* 0x8058 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02PUI03");
            CB->dialogue("L51D17", P_ARKHAN2);
            CB->animation("P02_65535");
        }
        /* 0x8090 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03PUI03");
            CB->dialogue("L51D18", P_ARKHAN3);
            CB->animation("P03_65535");
        }
        CB->joindre(ST(ST_HERO), 0, 0, 0, -12);
        /* 0x80E1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L51D01A", P_ARKHAN2, "G00DIA01", NULL);
            lieu_action_register("L51D01B", P_ARKHAN2, "G00DIA01", NULL);
        }
        /* 0x811F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L51D02A", P_ARKHAN3, "G00DIA01", NULL);
            lieu_action_register("L51D02B", P_ARKHAN3, "G00DIA01", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->animation("Sar_10");
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 01 — sub_10008A0F — exit to room 8 (start 2) */
static void trig01(void) {
    ST(0xB54) = 2;
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 10;
    CB->change_piece(8, 2);
}

/* trigger 02 — sub_10008A57 — exit to room 8 (start 1) */
static void trig02(void) {
    ST(0xB54) = 1;
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 10;
    CB->change_piece(8, 2);
}

/* trigger 03 — sub_10008A9F — exit to room 23 */
static void trig03(void) {
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 10;
    CB->change_piece(23, 1);
}

/* trigger 05 — sub_10008AE3 */
static void trig05(void) {
    /* 0x8AE9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_zone(5) || CB->direction_spec(0))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x8B41 */
        if (ST(0xF30) < 2) {
            /* 0x8B53 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x8B65 */
                if (ST(0x530) == 1) {
                    /* 0x8B77 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L51D03", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0x8B9F */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L51D04", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                }
                /* 0x8BC7 */
                if (ST(0x530) == 0) {
                    ST(0x530) = 1;
                    /* 0x8BE8 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L51D100A", P_ARKHAN2, "SG00DIA01", NULL);
                        lieu_action_register("L51D100B", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0x8C26 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L51D101A", P_ARKHAN3, "SG00DIA01", NULL);
                        lieu_action_register("L51D101B", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                }
            }
            /* 0x8C64 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x8C7E */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    /* 0x8C90 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ACT85");
                    }
                    /* 0x8CAD */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ACT85");
                    }
                }
                /* 0x8CD2 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    /* 0x8CE4 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ACT86");
                    }
                    /* 0x8D01 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ACT86");
                    }
                }
                /* 0x8D1E */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L51D05", P_ARKHAN2, "G00DIA01", NULL);
                }
                /* 0x8D46 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L51D06", P_ARKHAN3, "G00DIA01", NULL);
                }
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
        /* 0x8D87 */
        if (ST(0xF30) == 2) {
            /* 0x8D99 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
                /* 0x8E14 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT35");
                    CB->animation("P02_65535");
                }
                /* 0x8E3C */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT35");
                    CB->animation("P03_65535");
                }
                /* 0x8E64 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L51D07", P_ARKHAN2, "G00DIA02", NULL);
                }
                /* 0x8E8C */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L51D08", P_ARKHAN3, "G00DIA02", NULL);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
            /* 0x8ECD */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
                CB->retire_zone(7);
                CB->retire_zone(8);
                CB->retire_zone(9);
                /* 0x8F69 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L51D09", P_ARKHAN2, "SG00DIA08", NULL);
                }
                /* 0x8F91 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L51D10", P_ARKHAN3, "SG00DIA08", NULL);
                }
                CB->joindre(-ST(ST_HERO), 0, 0, 0xCF08, -1);
                CB->animation("Sar_50");
                CB->modify_var(&ST(0x68), 1);
            }
        }
    }
}

/* trigger 06 — sub_10008FFD — exit to room 20 */
static void trig06(void) {
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 10;
    CB->change_piece(20, -1);
}

/* trigger 07 — sub_10009036 */
static void trig07(void) {
    /* 0x903C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xD5A4)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x90A2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L51D111", P_ARKHAN2, "SG00DIA01", "Sar_10");
        }
        /* 0x90CD */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L51D112", P_ARKHAN3, "SG00DIA01", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 08 — sub_1000911B */
static void trig08(void) {
    /* 0x9121 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xBED5)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9187/0x9199: the ST_HERO == P_ARKHAN2 / P_ARKHAN3 tests have empty bodies */
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 — sub_100091FA */
static void trig12(void) {
    /* 0x9200 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_zone(24) || CB->direction_zone(12) || (CB->direction_spec(0x4E20) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 23)))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
        /* 0x92F7 */
        if (ST(0xF30) < 2) {
            /* 0x9309 */
            if (ST(ST_PLAYER_WEAPON) != 51) {
                /* 0x931B */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x932D */
                    if (ST(0x530) == 1) {
                        /* 0x933F */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L51D03", P_ARKHAN2, "SG00DIA02", NULL);
                        }
                        /* 0x9367 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L51D04", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                    }
                    /* 0x938F */
                    if (ST(0x530) == 0) {
                        ST(0x530) = 1;
                        /* 0x93B0 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L51D100A", P_ARKHAN2, "SG00DIA01", NULL);
                            lieu_action_register("L51D100B", P_ARKHAN2, "SG00DIA02", NULL);
                        }
                        /* 0x93EE */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L51D101A", P_ARKHAN3, "SG00DIA01", NULL);
                            lieu_action_register("L51D101B", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                    }
                }
                /* 0x942C */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x9446 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        /* 0x9458 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SP02ACT85");
                        }
                        /* 0x9475 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SP03ACT85");
                        }
                    }
                    /* 0x949A */
                    if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        /* 0x94AC */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SP02ACT86");
                        }
                        /* 0x94C9 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SP03ACT86");
                        }
                    }
                    /* 0x94E6 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L51D05", P_ARKHAN2, "G00DIA01", NULL);
                    }
                    /* 0x950E */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L51D06", P_ARKHAN3, "G00DIA01", NULL);
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
            /* 0x954F */
            if (ST(ST_PLAYER_WEAPON) == 51) {
                ST(0xF30) = 2;
                CB->joindre(-ST(ST_HERO), 0, 0, 0, -13);
                /* 0x958D */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT34");
                    CB->animation("P02_65535");
                }
                /* 0x95B5 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT34");
                    CB->animation("P03_65535");
                }
                CB->divers_empile(DV_SCREEN, 5, 0);
                CB->etat_objet("OB48", -1, -1, -1, -1, -1, -1, -2, 0);
                CB->etat_objet("OB51", -1, -1, -1, -1, -1, -1, -2, 0);
                CB->etat_objet("OB29A", 5196, -782, -1170, 0x2A00, 0x6E00, 0xD2F8, -1, 2);
                CB->divers_empile(DV_SCREEN, 3, 0);
                /* 0x966A */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L51D13", P_ARKHAN2, "G00DIA01", "Sar_10");
                }
                /* 0x9695 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L51D14", P_ARKHAN3, "G00DIA01", "Sar_10");
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x96E9 */
        if (ST(0xF30) == 2) {
            /* 0x96FB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L51D15", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0x9726 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L51D16", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->animation("Sar_10");
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 23 — sub_1000981F — forwards to trigger 12 (thunk sub_10001019) */
static void trig23(void) {
    /* 0x9825 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4E20)) {
        trig12();
        return;
    }
}

/* trigger 25 — sub_1000987F */
static void trig25(void) {
    /* 0x9885 */
    if (ST(0x70) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x70) = 1;
        CB->retire_zone(25);
        CB->bloque(BLOQUE_ALL);
        /* 0x98E1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L51D111", P_ARKHAN2, "SG00DIA01", "Sar_10");
        }
        /* 0x990C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L51D112", P_ARKHAN3, "SG00DIA01", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 26 — sub_1000995A */
static void trig26(void) {
    /* 0x9960 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xA)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x99A8 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x99BA */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L51D113", P_ARKHAN2, "SG00DIA05", "Sar_10");
            }
            /* 0x99E5 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L51D114", P_ARKHAN3, "SG00DIA05", "Sar_10");
            }
        }
        /* 0x9A10 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x9A22 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0x9A3F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT29");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->animation("Sar_10");
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 27 — sub_10009A8A */
static void trig27(void) {
    /* 0x9A90 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 &&
        ((CB->divers(DV_IN_ZONE, ST(ST_HERO), 27) && CB->direction_zone(27)) ||
         (CB->divers(DV_IN_ZONE, ST(ST_HERO), 35) && CB->direction_zone(35)) ||
         (CB->divers(DV_IN_ZONE, ST(ST_HERO), 36) && CB->direction_zone(36)) ||
         (CB->divers(DV_IN_ZONE, ST(ST_HERO), 37) && CB->direction_zone(37)))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9B91 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00PRE09");
            CB->animation("P02_65535");
            lieu_action_register("L51D103", P_ARKHAN2, "SG00DIA01", "Sar_10");
        }
        /* 0x9BD2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00PRE09");
            CB->animation("P03_65535");
            lieu_action_register("L51D104", P_ARKHAN3, "SG00DIA01", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 28 — sub_10009C36 */
static void trig28(void) {
    /* 0x9C3C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x1F40)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9C84 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 28) || CB->divers(DV_IN_ZONE, ST(ST_HERO), 30)) {
            CB->animation("SG00PRE02");
        } else {
            CB->animation("SG00PRE03");
        }
        /* 0x9CDF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("P02_65535");
            lieu_action_register("L51D105", P_ARKHAN2, "SG00DIA05", "Sar_10");
        }
        /* 0x9D15 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("P03_65535");
            lieu_action_register("L51D106", P_ARKHAN3, "SG00DIA05", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 31 — sub_10009D8E */
static void trig31(void) {
    /* 0x9D94 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC738)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9DE0 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 31)) {
            CB->animation("SG00PRE02");
        }
        /* 0x9E0B */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 34)) {
            CB->animation("SG00PRE09");
        } else {
            CB->animation("SG00PRE03");
        }
        /* 0x9E48 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("P02_65535");
            lieu_action_register("L51D105", P_ARKHAN2, "SG00DIA05", "Sar_10");
        }
        /* 0x9E7E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("P03_65535");
            lieu_action_register("L51D106", P_ARKHAN3, "SG00DIA05", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 38 — sub_10009F31 */
static void trig38(void) {
    /* 0x9F37 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x5208)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x9F93 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0x9FBE */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        return;
    }
}
