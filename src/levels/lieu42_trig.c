/*
 * lieu42_trig.c — Native hand-port of the LIEU42.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu42.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include "lieu42_int.h"

static void trig_noop(void);   /* the 24 stub slots */
static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig10(void); static void trig11(void);
static void trig12(void); static void trig13(void); static void trig14(void);
static void trig15(void); static void trig16(void); static void trig17(void);
static void trig18(void); static void trig20(void); static void trig21(void);
static void trig25(void); static void trig41(void);

/* dword_1001B920[42] — filled at INIT with the 5-byte jmp thunks; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000A500       28  1000ABDD stub
 *    1  10008951 stub  15  1000A5A1       29  1000ABE8 stub
 *    2  1000895C       16  1000A799       30  1000ABF3 stub
 *    3  10008CE7       17  1000A86E       31  1000ABFE stub
 *    4  1000903C       18  1000A90F       32  1000AC09 stub
 *    5  10009391       19  1000A9AD stub  33  1000AC14 stub
 *    6  1000971A       20  1000A9B8       34  1000AC1F stub
 *    7  10009AA3       21  1000AA6F       35  1000AC2A stub
 *    8  10009E79       22  1000AB82 stub  36  1000AC35 stub
 *    9  1000A173 stub  23  1000AB8D stub  37  1000AC40 stub
 *   10  1000A17E       24  1000AB98 stub  38  1000AC4B stub
 *   11  1000A27B       25  1000ABA3        39  1000AC56 stub
 *   12  1000A353       26  1000ABC7 stub  40  1000AC61 stub
 *   13  1000A42B       27  1000ABD2 stub  41  100078B4
 */
static void (*const g_trigger[42])(void) = {
    l42_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig_noop, trig10, trig11, trig12, trig13, trig14, trig15,
    trig16, trig17, trig18, trig_noop, trig20, trig21, trig_noop, trig_noop,
    trig_noop, trig25, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001B920]). */
void l42_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 — sub_100073F0 (thunk sub_10001073; INIT default) */
void l42_trig00(void) {
    CB->pos_perso(P_ARKHAN3, 1435, -1479, 0x3BC0, 1, "ar", 1, 0);
    CB->ajoute_zone(2, 1240, -1876, 140, 110, 0);
    CB->ajoute_zone(3, 1390, -1876, 140, 110, 0);
    CB->ajoute_zone(4, 1539, -1876, 140, 110, 0);
    CB->ajoute_zone(5, 1500, -1073, 160, 110, 0);
    CB->ajoute_zone(6, 1340, -1073, 160, 110, 0);
    CB->ajoute_zone(7, 1180, -1073, 160, 110, 0);
    CB->ajoute_zone(8, 1690, -1117, 170, 170, 0);
    CB->ajoute_zone(10, -11, -863, 315, 390, 0);
    CB->ajoute_zone(11, -683, -1653, 223, 230, 0);
    CB->ajoute_zone(12, -605, -1025, 290, 345, 0);
    CB->ajoute_zone(13, 725, -940, 320, 200, 0);
    CB->ajoute_zone(14, 692, -2041, 320, 150, 0);
    CB->ajoute_zone(15, -2796, -1288, 231, 515, 0);
    CB->ajoute_zone(16, 149, -1217, 405, 220, 0);
    CB->ajoute_zone(17, -1387, -2019, 300, 300, 0);
    CB->ajoute_zone(18, -1864, -807, 520, 360, 0);
    CB->ajoute_zone(20, 1703, -1459, 250, 300, 0);
    CB->ajoute_zone(21, -199, -905, 200, 215, 0);
    CB->etat_objet("IM88", 1725, -5835, -1253, 0x4000, 0xC000, 0, 0, 3);
    CB->divers(DV_MUSIC, 1, 0);
    CB->vire_col(30, 0);
    CB->vire_col(-7, 0);
    CB->vire_col(-8, 0);
    CB->select_pile(1, 1);
    CB->load_anim("242BAS01", 0);
    CB->load_anim("242BAS02", 1);
    CB->load_anim("242BAS03", 2);
    CB->load_anim("242BAS04", 3);
    CB->load_anim("242BAS05", 4);
    CB->load_anim("242BAS06", 5);
    CB->select_pile(0, 1);
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_EVENT_33) = 0;
    ST(V_EVENT_34) = 0;
    ST(V_EVENT_35) = 0;
    ST(V_EVENT_36) = 0;
    ST(V_EVENT_37) = 0;
    ST(V_EVENT_38) = 0;
    ST(V_EVENT_39) = 0;
    ST(V_EVENT_41) = 0;
    ST(0x64) = -1;
    ST(0x68) = -1;
    ST(0x6C) = -1;
    ST(0x70) = -1;
    ST(0x74) = -1;
    ST(0x78) = -1;
    ST(0x7C) = 0;
    ST(0x80) = 0;
    ST(0x84) = 0;
    ST(0x88) = 0;
    ST(0xB8) = 0;
    ST(0xE0) = 0;
    ST(0xF0) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
}

/* trigger 25 — sub_1000ABA3 */
static void trig25(void) {
    ST(ST_LEVEL) = 42;
    CB->change_piece(44, 0);
}

/* trigger 21 — sub_1000AA6F */
static void trig21(void) {
    /* 0xAA7E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && (CB->direction_spec(0xDF94) || CB->direction_zone(21))) {
        CB->bloque(BLOQUE_ALL);
        /* 0xAADF */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", NULL);
        }
        /* 0xAB07 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", NULL);
        }
        /* 0xAB2F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", NULL);
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 2 — sub_1000895C */
static void trig02(void) {
    /* 0x896E */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x89E5 */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 0x4F1 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 0x523) {
            /* 0x8A1A */
            if (ST(0xE0) == 0) {
                ST(0xE0) = 1;
                /* 0x8A43 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT96");
                } else {
                    CB->animation("SP03ATT97");
                }
                /* 0x8A70 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L42D08", P_ARKHAN3);
                }
                /* 0x8A92 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L42D09", P_ARKHAN3);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0x8AE2 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D10", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x8B0A */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D11", P_ARKHAN3, "SG03DIA04", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        /* 0x8B60 */
        if (ST(ST_LOCKED_CAMERA) != 4) {
            CB->change_camera(4, CAM_LOCKED);
        }
        CB->pos_perso(ST(ST_HERO), 1205, -1880, 0x5E3, 1, "ar", 1, 0);
        CB->animation("Sar_100");
        /* 0x8BB5 */
        if (ST(0x64) == 1) {
            ST(0x64) = -1;
            ST(0x84) = 11;
            CB->load_anim("242BAI03", 6);
            CB->evenement(0, 1200);
            CB->evenement(2, 2300);
            CB->animation("P03ATT44");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(99, CAM_AUTO);
            return;
        }
        /* 0x8C50 */
        if (ST(0x64) == -1) {
            ST(0x64) = 1;
            ST(0x84) = -11;
            CB->load_anim("242LEV03", 6);
            CB->evenement(0, 800);
            CB->evenement(2, 1500);
            CB->animation("P03ATT43");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(99, CAM_AUTO);
            return;
        }
    }
}

/* trigger 3 — sub_10008CE7 */
static void trig03(void) {
    /* 0x8CF9 */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8D70 */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 0x57D && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 0x5AF) {
            /* 0x8DA5 */
            if (ST(0xE0) == 0) {
                ST(0xE0) = 1;
                /* 0x8DCE */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT96");
                } else {
                    CB->animation("SP03ATT97");
                }
                /* 0x8DFB */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D08", P_ARKHAN3, "G03DIA04", NULL);
                }
                /* 0x8E23 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D09", P_ARKHAN3, "G03DIA04", NULL);
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0x8E79 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D10", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x8EA1 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D11", P_ARKHAN3, "SG03DIA04", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        CB->pos_perso(ST(ST_HERO), 1340, -1880, 0x5E3, 1, "ar", 1, 0);
        CB->animation("Sar_100");
        /* 0x8F2A */
        if (ST(0x68) == 1) {
            ST(0x68) = -1;
            ST(0x84) = 12;
            CB->load_anim("242BAI02", 6);
            CB->evenement(0, 1200);
            CB->evenement(2, 2500);
            CB->animation("P03ATT44");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x8FB5 */
        if (ST(0x68) == -1) {
            ST(0x68) = 1;
            ST(0x84) = -12;
            CB->load_anim("242LEV02", 6);
            CB->evenement(0, 800);
            CB->evenement(2, 1700);
            CB->animation("P03ATT43");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 4 — sub_1000903C */
static void trig04(void) {
    /* 0x904E */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x90C5 */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 0x61D && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 0x645) {
            /* 0x90FA */
            if (ST(0xE0) == 0) {
                ST(0xE0) = 1;
                /* 0x9123 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT96");
                } else {
                    CB->animation("SP03ATT97");
                }
                /* 0x9150 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D08", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x9178 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D09", P_ARKHAN3, "SG03DIA04", NULL);
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0x91CE */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D10", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x91F6 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D11", P_ARKHAN3, "SG03DIA04", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        CB->pos_perso(ST(ST_HERO), 1500, -1880, 0x5E3, 1, "ar", 1, 0);
        CB->animation("Sar_100");
        /* 0x927F */
        if (ST(0x6C) == 1) {
            ST(0x6C) = -1;
            ST(0x84) = 13;
            CB->load_anim("242BAI01", 6);
            CB->evenement(0, 1200);
            CB->evenement(2, 2300);
            CB->animation("P03ATT44");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x930A */
        if (ST(0x6C) == -1) {
            ST(0x6C) = 1;
            ST(0x84) = -13;
            CB->load_anim("242LEV01", 6);
            CB->evenement(0, 800);
            CB->evenement(2, 1500);
            CB->animation("P03ATT43");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 5 — sub_10009391 */
static void trig05(void) {
    /* 0x93A3 */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        /* 0x93F5 */
        if (ST(0xB8) == 1) {
            ST(0xB8) = 1000;
            CB->change_camera(99, CAM_AUTO);
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x944E */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 0x5F0 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 0x62C) {
            /* 0x9483 */
            if (ST(0xE0) == 0) {
                ST(0xE0) = 1;
                /* 0x94AC */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT96");
                } else {
                    CB->animation("SP03ATT97");
                }
                /* 0x94D9 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D08", P_ARKHAN3, "G03DIA04", NULL);
                }
                /* 0x9501 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D09", P_ARKHAN3, "G03DIA04", NULL);
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0x9557 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D10", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x957F */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D11", P_ARKHAN3, "SG03DIA04", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        CB->pos_perso(ST(ST_HERO), 1505, -1080, 0x815D, 1, "ar", 1, 0);
        CB->animation("Sar_100");
        /* 0x9608 */
        if (ST(0x78) == 1) {
            ST(0x78) = -1;
            ST(0x84) = 16;
            CB->load_anim("242BAI06", 6);
            CB->evenement(0, 1200);
            CB->evenement(2, 2300);
            CB->animation("P03ATT44");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9693 */
        if (ST(0x78) == -1) {
            ST(0x78) = 1;
            ST(0x84) = -16;
            CB->load_anim("242LEV06", 6);
            CB->evenement(0, 800);
            CB->evenement(2, 2000);
            CB->animation("P03ATT43");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 6 — sub_1000971A */
static void trig06(void) {
    /* 0x972C */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        /* 0x977E */
        if (ST(0xB8) == 1) {
            ST(0xB8) = 1000;
            CB->change_camera(99, CAM_AUTO);
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x97D7 */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 0x555 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 0x58C) {
            /* 0x980C */
            if (ST(0xE0) == 0) {
                ST(0xE0) = 1;
                /* 0x9835 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT96");
                } else {
                    CB->animation("SP03ATT97");
                }
                /* 0x9862 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D08", P_ARKHAN3, "G03DIA04", NULL);
                }
                /* 0x988A */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D09", P_ARKHAN3, "G03DIA04", NULL);
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0x98E0 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D10", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x9908 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D11", P_ARKHAN3, "SG03DIA04", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        CB->pos_perso(ST(ST_HERO), 1333, -1071, 0x898F, 1, "ar", 1, 0);
        CB->animation("Sar_100");
        /* 0x9991 */
        if (ST(0x74) == 1) {
            ST(0x74) = -1;
            ST(0x84) = 15;
            CB->load_anim("242BAI05", 6);
            CB->evenement(0, 1200);
            CB->evenement(2, 2300);
            CB->animation("P03ATT44");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9A1C */
        if (ST(0x74) == -1) {
            ST(0x74) = 1;
            ST(0x84) = -15;
            CB->load_anim("242LEV05", 6);
            CB->evenement(0, 800);
            CB->evenement(2, 1500);
            CB->animation("P03ATT43");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 7 — sub_10009AA3 */
static void trig07(void) {
    /* 0x9AB5 */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        /* 0x9B07 */
        if (ST(0xB8) == 1) {
            ST(0xB8) = 1000;
            CB->change_camera(99, CAM_AUTO);
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9B60 */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 0x4BA && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 0x4EC) {
            /* 0x9B95 */
            if (ST(0xE0) == 0) {
                ST(0xE0) = 1;
                /* 0x9BBE */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT96");
                } else {
                    CB->animation("SP03ATT97");
                }
                /* 0x9BEB */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D08", P_ARKHAN3, "G03DIA04", NULL);
                }
                /* 0x9C13 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D09", P_ARKHAN3, "G03DIA04", NULL);
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0x9C69 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L42D10", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x9C91 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L42D11", P_ARKHAN3, "SG03DIA04", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        /* 0x9CE7 */
        if (ST(ST_LOCKED_CAMERA) != 5) {
            CB->change_camera(5, CAM_LOCKED);
        }
        CB->pos_perso(ST(ST_HERO), 1184, -1071, 0x8672, 1, "ar", 1, 0);
        CB->animation("Sar_100");
        /* 0x9D3C */
        if (ST(0x70) == 1) {
            ST(0x70) = -1;
            ST(0x84) = 14;
            CB->load_anim("242BAI04", 6);
            CB->evenement(0, 1200);
            CB->evenement(2, 2300);
            CB->animation("P03ATT44");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(99, CAM_AUTO);
            return;
        }
        /* 0x9DD7 */
        if (ST(0x70) == -1) {
            ST(0x70) = 1;
            ST(0x84) = -14;
            CB->load_anim("242LEV04", 6);
            CB->animation("ar_50");
            CB->evenement(0, 800);
            CB->evenement(2, 1500);
            CB->animation("P03ATT43");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(99, CAM_AUTO);
            return;
        }
    }
}

/* trigger 8 — sub_10009E79 */
static void trig08(void) {
    /* 0x9E88 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8D60)) {
        /* 0x9ECB */
        if (ST(0x56C) == 0) {
            /* 0x9EDA */
            if (ST(0x64) == 1 && ST(0x68) == 1 && ST(0x6C) == -1 && ST(0x70) == 1 && ST(0x74) == -1 && ST(0x78) == 1) {
                ST(0x56C) = 1;
                CB->bloque(BLOQUE_ALL);
                CB->retire_zone(20);
                CB->ajoute_zone(25, 1762, -1476, 45, 100, 1);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(-ST(ST_HERO), 1664, -1075, 0x80F2, -1);
                CB->animation("Sar_100");
                CB->evenement(1, 500);
                CB->animation("P03ATT45");
                CB->animation("ar_50");
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(-ST(ST_HERO), 1664, -1075, 0x80F2, -1);
                CB->animation("Sar_100");
                CB->evenement(1, 500);
                CB->animation("P03ATT45");
                CB->animation("ar_50");
                ST(V_DLG_RANDOM) = CB->random(3);
                /* 0xA071 */
                if (ST(V_DLG_RANDOM) == 0) {
                    lieu_action_register("L42D07A", P_ARKHAN3, "SG03DIA03", NULL);
                }
                /* 0xA099 */
                if (ST(V_DLG_RANDOM) == 1) {
                    lieu_action_register("L42D07B", P_ARKHAN3, "SG03DIA03", NULL);
                }
                /* 0xA0C1 */
                if (ST(V_DLG_RANDOM) == 2) {
                    lieu_action_register("L42D07C", P_ARKHAN3, "SG03DIA03", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xA112 */
        if (ST(0x56C) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            lieu_action_register("L42D06", P_ARKHAN3, "SG03DIA01", NULL);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 10 — sub_1000A17E */
static void trig10(void) {
    /* 0xA18D */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 21) && (CB->direction_zone(21) || CB->direction_spec(0))) {
        trig21();
        return;
    }
    /* 0xA1E9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(10)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D12", P_ARKHAN3, "SG03DIA05", NULL);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 11 — sub_1000A27B */
static void trig11(void) {
    /* 0xA28A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(11)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D13A", P_ARKHAN3, "SG03DIA05", NULL);
        /* 0xA2FA */
        if (ST(0xF0) == 0) {
            ST(0xF0) = 1;
            lieu_action_register("L42D13B", P_ARKHAN3, "SG03DIA05", NULL);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 — sub_1000A353 */
static void trig12(void) {
    /* 0xA362 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(12)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D13A", P_ARKHAN3, "SG03DIA05", NULL);
        /* 0xA3D2 */
        if (ST(0xF0) == 0) {
            ST(0xF0) = 1;
            lieu_action_register("L42D13B", P_ARKHAN3, "SG03DIA05", NULL);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 13 — sub_1000A42B */
static void trig13(void) {
    /* 0xA43A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D13A", P_ARKHAN3, "SG03DIA05", NULL);
        /* 0xA4A7 */
        if (ST(0xF0) == 0) {
            ST(0xF0) = 1;
            lieu_action_register("L42D13B", P_ARKHAN3, "SG03DIA05", NULL);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 — sub_1000A500 */
static void trig14(void) {
    /* 0xA50F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D12", P_ARKHAN3, "SG03DIA05", NULL);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 — sub_1000A5A1 */
static void trig15(void) {
    /* 0xA5B0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA60A */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xA624 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP03ACT70");
                lieu_action_register("L42D03", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                CB->animation("SP03ACT71");
                lieu_action_register("L42D03", P_ARKHAN3, "P03ACT75", "SP03ACT77");
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xA6D5 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xA6EF */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP03ACT72");
                lieu_action_register("L42D04", P_ARKHAN3, "P03ACT74", "SP03ACT78");
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
                return;
            }
            CB->animation("SP03ACT73");
            lieu_action_register("L42D04", P_ARKHAN3, "P03ACT75", "SP03ACT79");
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 16 — sub_1000A799 */
static void trig16(void) {
    /* 0xA7A8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D13A", P_ARKHAN3, "SG03DIA05", NULL);
        /* 0xA815 */
        if (ST(0xF0) == 0) {
            ST(0xF0) = 1;
            lieu_action_register("L42D13B", P_ARKHAN3, "SG03DIA05", NULL);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 17 — sub_1000A86E */
static void trig17(void) {
    /* 0xA87D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(17)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D12", P_ARKHAN3, "SG03DIA05", NULL);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 18 — sub_1000A90F */
static void trig18(void) {
    /* 0xA91E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D12", P_ARKHAN3, "SG03DIA05", NULL);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 20 — sub_1000A9B8 */
static void trig20(void) {
    /* 0xA9C7 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xC000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D14", P_ARKHAN3, "SG03DIA05", NULL);
        lieu_action_register("L42D15", P_ARKHAN3, "SG03DIA05", NULL);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 41 — sub_100078B4 */
static void trig41(void) {
    CB->divers(DV_MUSIC, 1, 1);
    /* 0x78D8 */
    if (ST(0x8B0) == 0) {
        ST(0x8B0) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(ST(ST_HERO), 1205, -1489, -1, -1);
        CB->cinematique("SC31_500");
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->bruitage("L42AMB1", -2796, -1288);
        CB->animation("Sar_50");
        CB->joindre(-ST(ST_HERO), 0, 0, 0xB3C4, -1);
        ST(ST_SCRIPT_BUSY) = 1;
        lieu_action_register("L42D01", P_ARKHAN3, "SG03DIA03", NULL);
        CB->animation("Sar_50");
        CB->joindre(ST(ST_HERO), 0, 0, 0x4000, -1);
        CB->animation("Sar_50");
        lieu_action_register("L42D05", P_ARKHAN3, "SG03DIA02", NULL);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}
