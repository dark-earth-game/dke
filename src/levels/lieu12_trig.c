/*
 * lieu12_trig.c - Native hand-port of the LIEU12.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu12.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu12_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig02(void); static void trig03(void); static void trig10(void);
static void trig11(void); static void trig12(void); static void trig13(void);
static void trig14(void); static void trig15(void); static void trig17(void);
static void trig18(void); static void trig19(void); static void trig20(void);
static void trig21(void); static void trig23(void); static void trig24(void);
static void trig25(void); static void trig26(void); static void trig28(void);
static void trig30(void); static void trig41(void);
static void trig_noop(void);   /* the 21 stub slots */

/* dword_100244D0[42] - filled at INIT (before the reload-flag test) with the
 * 5-byte jmp thunks at 0x10001000+5*i; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000E097       28  1000F847
 *    1  10008B3D stub  15  1000E27E       29  1000F922 stub
 *    2  10008B48       16  1000E382 stub  30  1000F92D
 *    3  1000C8BD       17  1000E38D       31  1000FB61 stub
 *    4  1000CD33 stub  18  1000E663       32  1000FB6C stub
 *    5  1000CD3E stub  19  1000E767       33  1000FB77 stub
 *    6  1000CD49 stub  20  1000E86B       34  1000FB82 stub
 *    7  1000CD54 stub  21  1000EE35       35  1000FB8D stub
 *    8  1000CD5F stub  22  1000EF39 stub  36  1000FB98 stub
 *    9  1000CD6A stub  23  1000EF44       37  1000FBA3 stub
 *   10  1000CD75       24  1000F575       38  1000FBAE stub
 *   11  1000D165       25  1000F64F       39  1000FBB9 stub
 *   12  1000D6C3       26  1000F729       40  1000FBC4 stub
 *   13  1000DDEF       27  1000F83C stub  41  100079FC
 */
static void (*const g_trigger[42])(void) = {
    l12_trig00, trig_noop, trig02, trig03, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig10, trig11, trig12, trig13, trig14, trig15,
    trig_noop, trig17, trig18, trig19, trig20, trig21, trig_noop,
    trig23, trig24, trig25, trig26, trig_noop, trig28, trig_noop,
    trig30, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x100244D0]). */
void l12_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 - sub_100073F0 (patch) - level setup: entry presets, zones, objects, actor placement */
void l12_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(12)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_LEVEL) = 11;
    }
    /* 0x743E */
    if (ST(ST_START_POINT(12)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xEFC) = 1;
        ST(ST_LEVEL) = 11;
    }
    /* 0x7477 */
    if (ST(ST_LEVEL) == 11) {
        CB->pos_perso(ST(ST_HERO), 7575, -1221, 0x2D82, 1, "ar", 1, 0);
    }
    CB->ajoute_zone(10, 7081, -1148, 250, 150, 0);
    CB->ajoute_zone(11, 7549, -1853, 130, 281, 0);
    CB->ajoute_zone(12, 7874, -1009, 205, 160, 0);
    CB->ajoute_zone(13, 7986, -1493, 95, 105, 0);
    CB->ajoute_zone(14, 7825, -1422, 145, 85, 0);
    CB->ajoute_zone(15, 8003, -1287, 235, 146, 0);
    CB->ajoute_zone(16, 8000, -1287, 50, 50, 0);
    CB->ajoute_zone(17, 7547, -1245, 120, 95, 0);
    CB->ajoute_zone(18, 7132, -1475, 95, 123, 0);
    CB->ajoute_zone(19, 7190, -1620, 94, 115, 0);
    CB->ajoute_zone(20, 7714, -993, 132, 81, 0);
    CB->ajoute_zone(21, 7930, -1613, 130, 120, 0);
    CB->ajoute_zone(22, 7860, -1012, 50, 50, 0);
    CB->ajoute_zone(23, 7219, -1004, 70, 80, 0);
    CB->ajoute_zone(24, 7526, -1042, 90, 84, 0);
    CB->ajoute_zone(25, 7505, -861, 140, 135, 0);
    CB->ajoute_zone(26, 8000, -1134, 210, 215, 0);
    CB->ajoute_zone(27, 7894, -1436, 50, 50, 0);
    CB->ajoute_zone(28, 7924, -1403, 115, 75, 0);
    CB->ajoute_zone(29, 7554, -1871, 15, 30, 0);
    CB->ajoute_zone(30, 7558, -1338, 230, 205, 0);
    CB->ajoute_zone(31, 7558, -1338, 65, 65, 0);
    CB->ajoute_zone(32, 7305, -1436, 20, 10, 0);
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
    ST(0x94) = 0;
    ST(0x100) = 0;
    CB->bruitage("L12AMB1", 7553, -1342);
    CB->load_anim("L12OUV01", 0);
    /* 0x7833 */
    if (ST(0x690) == 0) {
        CB->etat_objet("OB70", 7115, 1305, -1134, 0xF600, 0x4C00, 0x3200, -1, 4);
    }
    /* 0x7872 */
    if (ST(ST_VIE(96)) == 0) {
        CB->etat_objet("OB06", 7982, 1299, -1448, 0, 0x600, 0x600, -1, 3);
    }
    /* 0x78AE */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        /* 0x78C0 */
        if (ST(0xC18) == 0) {
            ST(0xC18) = 1;
        }
        CB->pos_perso(P_KHORAD, 7385, -997, 0x7530, 1, "P83ARR01", 3, 0);
        CB->pos_perso(P_BANDOR, 7294, -1026, 0x6633, 1, "P07ATT01", 2, 0);
    }
    /* 0x792B */
    if (ST(0xEFC) != 1) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x794F */
    if (ST(0xEFC) == 1) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    /* 0x7973 */
    if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD) {
        CB->pos_perso(P_BANDOR, 0, 0, 0, 2, "P07cad", 2, 0);
    }
    /* 0x79A1 */
    if (ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
        CB->pos_perso(P_KHORAD, 0, 0, 0, 2, "P83cad", 3, 0);
    }
    /* 0x79CF */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->load_anim("LEVIER", 1);
    }
}

/* trigger 02 - sub_10008B48 - Bandor / Khorad: talk by stage (0xC1C), weapon reactions, loot, strikes */
static void trig02(void) {
    /* 0x8B57 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x2CEC) && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && CB->divers(DV_IN_ZONE, ST(ST_HERO), 23)) {
        trig23();
        return;
    }
    /* 0x8BE2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
        /* 0x8C33 */
        if (ST(0xC1C) < 2) {
            /* 0x8C46 */
            if (CB->direction_perso(P_BANDOR) || ST(0xC1C) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
                ST(0x70) = 0;
            } else if (CB->direction_perso(P_KHORAD)) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_KHORAD);
                ST(0x70) = 1;
            } else {
                return;
            }
        }
        /* 0x8D04 */
        if (ST(0xC1C) >= 2) {
            /* 0x8D17 */
            if (CB->direction_perso(P_BANDOR) || CB->direction_perso(P_KHORAD)) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
                ST(0x70) = 0;
            } else {
                return;
            }
        }
        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP83ARR01_250");
        CB->animation("P83ARR01");
        CB->joindre(P_KHORAD, 0, 0, 0, ST(ST_HERO));
        /* 0x8DD2 */
        if (ST(0xC1C) != 1) {
            CB->change_camera(-4, CAM_LOCKED);
        }
        /* 0x8DF4 */
        if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
            /* 0x8E06 */
            if (ST(0xC1C) == 0) {
                /* 0x8E18 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON)) {
                    /* 0x8E5B */
                    if (ST(0x690) == 0 || ST(0x68) == 1) {
                        /* 0x8E7C */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            /* 0x8E8E */
                            if (ST(0x80C) == 0) {
                                /* 0x8EA0 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D06A", P_ARKHAN2, "SG02DIA07", NULL);
                                    CB->animation("SP07ARR01_10");
                                    lieu_action_register("L12D06B", P_ARKHAN2, "SG02DIA05", NULL);
                                    CB->animation("SP83ARR01_400");
                                    lieu_action_register("L12D10", P_KHORAD, "P83DIA54", "SP83ARR01_10");
                                    lieu_action_register("L12D11", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                                    lieu_action_register("L12D12", P_KHORAD, "SP83DIA53", "SP83ARR01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x80C) = 1;
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                                /* 0x8F87 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L12D07", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L12D10", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
                                    lieu_action_register("L12D11", P_BANDOR, "SP07DIA50", "SP07ARR01_10");
                                    lieu_action_register("L12D12", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x80C) = 1;
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                            }
                            /* 0x9042 */
                            if (ST(0x80C) == 1) {
                                /* 0x9054 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D13A", P_ARKHAN2, "SG02DIA02", NULL);
                                    lieu_action_register("L12D13B", P_ARKHAN2, "SG02DIA04", NULL);
                                    lieu_action_register("L12D13C", P_ARKHAN2, "SG02DIA07", NULL);
                                    lieu_action_register("L12D24", P_BANDOR, "SP07DIA50", "SP07ARR01_10");
                                    lieu_action_register("L12D25", P_KHORAD, "SP83DIA53", "SP83ARR01_10");
                                    lieu_action_register("L12D26", P_BANDOR, "SP07DIA54", "SP07ARR01_10");
                                    lieu_action_register("L12D27", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x80C) = 2;
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                                /* 0x9154 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L12D14A", P_ARKHAN2, "SG02DIA02", NULL);
                                    CB->animation("SP07DIA51");
                                    CB->animation("SP83DIA50");
                                    lieu_action_register("L12D14B", P_ARKHAN2, "SG02DIA01", NULL);
                                    lieu_action_register("L12D14C", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L12D24", P_BANDOR, "SP07DIA50", "SP07ARR01_10");
                                    lieu_action_register("L12D25", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
                                    lieu_action_register("L12D26", P_BANDOR, "SP07DIA54", "SP07ARR01_10");
                                    lieu_action_register("L12D27", P_KHORAD, "SP83DIA59", "SP83ATT01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x80C) = 2;
                                    CB->animation("P83DIA51_750");
                                    CB->animation("P83ARR01_100");
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                            }
                            /* 0x9280 */
                            if (ST(0x80C) == 2) {
                                /* 0x9292 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D19A", P_ARKHAN2, "SG02DIA02", NULL);
                                    lieu_action_register("L12D19B", P_ARKHAN2, "SG02DIA07", NULL);
                                }
                                /* 0x92D0 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L12D20A", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L12D20B", P_ARKHAN2, "SG02DIA07", NULL);
                                }
                                lieu_action_register("L12D22", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                                lieu_action_register("L12D23A", P_KHORAD, "SP83DIA60", "SP83ARR01_10");
                                lieu_action_register("L12D23B", P_KHORAD, "SP83DIA63", "SP83ARR01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x80C) = 3;
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            }
                            /* 0x93A1 */
                            if (ST(0x80C) == 3) {
                                /* 0x93B3 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D200", P_ARKHAN2, "SG02DIA02", NULL);
                                    /* 0x93D8 */
                                    if (ST(0x70) == 0) {
                                        lieu_action_register("L12D202", P_BANDOR, "SP07DIA64", "SP07ARR01_10");
                                    } else {
                                        lieu_action_register("L12D203", P_KHORAD, "SP83DIA63", "SP83ARR01_10");
                                    }
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                                /* 0x945A */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    /* 0x9469 */
                                    if (ST(0x70) == 0) {
                                        CB->animation("SP83DIA51");
                                        CB->animation("SP07DIA50");
                                        lieu_action_register("LCOD22A", P_ARKHAN3, "SG02DIA07", NULL);
                                        lieu_action_register("L12D201", P_ARKHAN2, "SG02DIA07", NULL);
                                        lieu_action_register("L12D202", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                                        CB->animation("Sar_10");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        CB->change_camera(2, CAM_AUTO);
                                        CB->animation("SP83DIA50_750");
                                        CB->animation("P83ARR01_10");
                                    } else {
                                        CB->animation("SP07DIA50");
                                        CB->animation("SP83DIA51");
                                        lieu_action_register("LCOD22A", P_ARKHAN3, "SG02DIA07", NULL);
                                        lieu_action_register("L12D201", P_ARKHAN2, "SG02DIA05", NULL);
                                        lieu_action_register("L12D203", P_KHORAD, "SP83DIA50", "SP83ARR01_10");
                                        CB->animation("Sar_10");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        CB->change_camera(2, CAM_AUTO);
                                        CB->animation("P07ARR01_10");
                                    }
                                    return;
                                }
                            }
                        }
                        /* 0x95C4 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            /* 0x95D6 */
                            if (ST(0x80C) == 0) {
                                /* 0x95E8 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D08", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L12D10", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
                                    lieu_action_register("L12D11", P_BANDOR, "SP07DIA50", "SP07ARR01_10");
                                    lieu_action_register("L12D12", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x80C) = 1;
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                                /* 0x96A3 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    CB->animation("SP07ATT01_10");
                                    CB->animation("SP83ATT01_10");
                                    lieu_action_register("L12D09", P_ARKHAN3, "SG03DIA07", NULL);
                                    lieu_action_register("L12D10", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
                                    lieu_action_register("L12D11", P_BANDOR, "SP07DIA50", "SP07ARR01_10");
                                    lieu_action_register("L12D12", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x80C) = 1;
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                            }
                            /* 0x9774 */
                            if (ST(0x80C) == 1 && ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L12D15A", P_ARKHAN3, "SG03DIA07", NULL);
                                CB->animation("SP07DIA51");
                                CB->animation("SP83DIA50");
                                lieu_action_register("L12D15B", P_ARKHAN3, "SG03DIA05", NULL);
                                lieu_action_register("L12D24", P_BANDOR, "SP07DIA50", "SP07ARR01_10");
                                lieu_action_register("L12D25", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
                                lieu_action_register("L12D26", P_BANDOR, "SP07DIA54", "SP07ARR01_10");
                                lieu_action_register("L12D27", P_KHORAD, "SP83DIA59", "SP83ATT01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x80C) = 2;
                                CB->change_camera(2, CAM_AUTO);
                                CB->animation("P83DIA51_750");
                                CB->animation("P83ARR01_100");
                                return;
                            }
                            /* 0x989C */
                            if (ST(0x80C) == 2 && ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L12D21A", P_ARKHAN3, "SG03DIA02", NULL);
                                lieu_action_register("L12D21B", P_ARKHAN3, "SG03DIA07", NULL);
                                lieu_action_register("L12D22", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                                lieu_action_register("L12D23A", P_KHORAD, "SP83DIA60", "SP83ARR01_10");
                                lieu_action_register("L12D23B", P_KHORAD, "SP83DIA63", "SP83ARR01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x80C) = 3;
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            }
                            /* 0x997F */
                            if (ST(0x80C) == 3 && ST(ST_SPECIAL_MODE) == 1) {
                                /* 0x99A0 */
                                if (ST(0x70) == 0) {
                                    CB->animation("SP83DIA51");
                                    CB->animation("SP07DIA50");
                                    lieu_action_register("L12D300", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L12D202", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->change_camera(2, CAM_AUTO);
                                    CB->animation("SP83DIA50_750");
                                    CB->animation("P83ARR01_10");
                                } else {
                                    CB->animation("SP07DIA50");
                                    CB->animation("SP83DIA51");
                                    lieu_action_register("L12D300", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L12D203", P_KHORAD, "SP83DIA50", "SP83ARR01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->change_camera(2, CAM_AUTO);
                                    CB->animation("P07ARR01_10");
                                }
                                return;
                            }
                            /* 0x9ACF */
                            if (ST(0x80C) >= 1 && ST(ST_SPECIAL_MODE) == 0) {
                                ST(V_DLG_RANDOM) = CB->random(3);
                                CB->animation("SG00DIA08");
                                /* 0x9B15 */
                                if (ST(V_DLG_RANDOM) == 0) {
                                    CB->dialogue("LCOD22A", P_ARKHAN3);
                                }
                                /* 0x9B37 */
                                if (ST(V_DLG_RANDOM) == 1) {
                                    CB->dialogue("LCOD22B", P_ARKHAN3);
                                }
                                /* 0x9B59 */
                                if (ST(V_DLG_RANDOM) == 2) {
                                    CB->dialogue("LCOD22C", P_ARKHAN3);
                                }
                                CB->animation("sar_10");
                                CB->animation("SP07ATT01");
                                CB->animation("SP83DIA51");
                                /* 0x9B9C */
                                if (ST(V_DLG_RANDOM) < 2) {
                                    lieu_action_register("L12D17", P_KHORAD, "SP83ATT02", "SP83DIA54_10");
                                }
                                /* 0x9BC7 */
                                if (ST(0x88) == 0) {
                                    lieu_action_register("L12D18A", P_BANDOR, "SP07ATT50", "SP07DIA51_10");
                                    ST(0x88) = 1;
                                } else {
                                    lieu_action_register("L12D18B", P_BANDOR, "SP07ATT02", "SP07DIA51_10");
                                    ST(0x88) = 0;
                                }
                                CB->animation("SP83ATT01_10");
                                CB->animation("SP07ATT01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xCB0) = 2;
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            }
                        }
                    }
                    /* 0x9C8C */
                    if (ST(0x690) == 1 && ST(0x68) == 0) {
                        ST(0x68) = 1;
                        /* 0x9CB9 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L12D32", P_ARKHAN2, "SG02DIA05", NULL);
                        }
                        /* 0x9CE1 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L12D34", P_ARKHAN3, "SG03DIA05", NULL);
                        }
                        lieu_action_register("L12D33A", P_KHORAD, "SP83DIA63", "SP83ARR01_10");
                        lieu_action_register("L12D33B", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->change_camera(2, CAM_AUTO);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x9D74 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != OB_BANDOR_DIAG && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0x9DDD */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L12D121", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0x9E08 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L12D122", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    ST(V_DLG_RANDOM) = CB->random(2);
                    /* 0x9E4A */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L12D123", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                    }
                    /* 0x9E75 */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L12D124", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                    }
                    CB->animation("SG00PRE82");
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->change_camera(2, CAM_AUTO);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9EE4 */
                if (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(0x68) = 1;
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    ST(0x68) = 1;
                    /* 0x9F3C */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L12D32", P_ARKHAN2, "SG02DIA05", NULL);
                    }
                    /* 0x9F64 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L12D34", P_ARKHAN3, "SG03DIA05", NULL);
                    }
                    lieu_action_register("L12D33A", P_KHORAD, "SP83DIA63", "SP83ARR01_10");
                    lieu_action_register("L12D33B", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->change_camera(2, CAM_AUTO);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x9FF7 */
            if (ST(0xC1C) == 1) {
                /* 0xA009 */
                if (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG) {
                    /* 0xA01B */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0xA02D */
                        if (ST(0x180) == 0 && ST(ST_LOCKED_CAMERA) != 2) {
                            CB->change_camera(2, CAM_LOCKED);
                        }
                        lieu_action_register("L12D40", P_ARKHAN2, "SG02DIA03", NULL);
                        lieu_action_register("L12D43", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                        lieu_pre_scene(ST(ST_HERO), P_BANDOR, 70, 1, 2);
                        CB->link_objet(7, "OB70", 0x1B, 0);
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0xC1C), 2);
                        CB->evenement(2, 1000);
                    }
                    /* 0xA102 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0xA114 */
                        if (ST(0x180) == 0 && ST(ST_LOCKED_CAMERA) != 2) {
                            CB->change_camera(2, CAM_LOCKED);
                        }
                        lieu_action_register("L12D42", P_ARKHAN3, "SG03DIA07", NULL);
                        lieu_action_register("L12D43", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                        lieu_pre_scene(ST(ST_HERO), P_BANDOR, 70, 1, 2);
                        CB->link_objet(7, "OB70", 0x1B, 0);
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0xC1C), 2);
                        CB->evenement(2, 1000);
                    }
                }
                /* 0xA1E9 */
                if (ST(ST_PLAYER_WEAPON) != OB_BANDOR_DIAG) {
                    /* 0xA1FB */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
                        lieu_action_register("L12D40", P_ARKHAN2, "SG02DIA03", NULL);
                        lieu_action_register("L12D43", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xA27E */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
                        lieu_action_register("L12D42", P_ARKHAN3, "SG03DIA07", NULL);
                        lieu_action_register("L12D43", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
            /* 0xA301 */
            if (ST(0xC1C) == 2) {
                /* 0xA313 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(0x810) == 0) {
                    /* 0xA337 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L12D47A", P_ARKHAN2, "SG02DIA04", NULL);
                        lieu_action_register("L12D47B", P_ARKHAN2, "SG02DIA07", NULL);
                        lieu_action_register("L12D47C", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L12D50A", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                        lieu_action_register("L12D50B", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xC1C) = 3;
                        CB->change_camera(2, CAM_AUTO);
                        return;
                    }
                    /* 0xA405 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SP07ATT01_10");
                        lieu_action_register("L12D48", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L12D50A", P_BANDOR, "SP07ATT02", "SP07ARR01_10");
                        lieu_action_register("L12D50B", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xC1C) = 3;
                        CB->change_camera(2, CAM_AUTO);
                        return;
                    }
                }
                /* 0xA4B2 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xA4C4 */
                    if (ST(0x810) == 0 && ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SP07ATT01_10");
                        lieu_action_register("L12D49A", P_ARKHAN3, "SG03DIA02", NULL);
                        lieu_action_register("L12D49B", P_ARKHAN3, "SG03DIA07", NULL);
                        lieu_action_register("L12D50A", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                        lieu_action_register("L12D50B", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCB4) = 1;
                        ST(0xC1C) = 3;
                        CB->change_camera(2, CAM_AUTO);
                        return;
                    }
                    /* 0xA5A8 */
                    if (ST(0x810) >= 0 && ST(ST_SPECIAL_MODE) == 0) {
                        ST(V_DLG_RANDOM) = CB->random(3);
                        CB->animation("SG00DIA08");
                        /* 0xA5EE */
                        if (ST(V_DLG_RANDOM) == 0) {
                            CB->dialogue("LCOD22A", P_ARKHAN3);
                        }
                        /* 0xA610 */
                        if (ST(V_DLG_RANDOM) == 1) {
                            CB->dialogue("LCOD22B", P_ARKHAN3);
                        }
                        /* 0xA632 */
                        if (ST(V_DLG_RANDOM) == 2) {
                            CB->dialogue("LCOD22C", P_ARKHAN3);
                        }
                        CB->animation("sar_10");
                        CB->animation("SP07ATT01");
                        CB->animation("SP83DIA51");
                        /* 0xA675 */
                        if (ST(V_DLG_RANDOM) < 2) {
                            lieu_action_register("L12D17", P_KHORAD, "SP83DIA51", "SP83ATT02_10");
                        }
                        /* 0xA6A0 */
                        if (ST(0x88) == 0) {
                            lieu_action_register("L12D18A", P_BANDOR, "SP07ATT50", "SP07DIA51_10");
                            ST(0x88) = 1;
                        } else {
                            lieu_action_register("L12D18B", P_BANDOR, "SP07ATT02", "SP07DIA51_10");
                            ST(0x88) = 0;
                        }
                        CB->animation("SP83ATT01_10");
                        CB->animation("SP07ATT01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->change_camera(2, CAM_AUTO);
                        return;
                    }
                }
            }
            /* 0xA756 */
            if (ST(0xC1C) == 3) {
                /* 0xA768 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    /* 0xA791 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0xA7A3 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            /* 0xA7B5 */
                            if (ST(0x84) == 0) {
                                lieu_action_register("L12D51A", P_ARKHAN2, "SG02DIA02", NULL);
                                lieu_action_register("L12D51B", P_ARKHAN2, "SG02DIA07", NULL);
                                lieu_action_register("L12D54A", P_BANDOR, "SP07DIA55", "SP07ARR01_10");
                                lieu_action_register("L12D54B", P_BANDOR, "SP07DIA57", "SP07ARR01_10");
                                ST(0x84) = 1;
                            } else {
                                lieu_action_register("L12D200", P_ARKHAN2, "SG02DIA02", NULL);
                                lieu_action_register("L12D50A", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                                lieu_action_register("L12D50B", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                                ST(0x84) = 0;
                            }
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                        /* 0xA8C9 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L12D52A", P_ARKHAN2, "SG02DIA02", NULL);
                            CB->animation("SP07ATT01_10");
                            CB->animation("SP83ATT01_10");
                            lieu_action_register("L12D52B", P_ARKHAN2, "SG02DIA07", NULL);
                            /* 0xA91D */
                            if (ST(0x84) == 0) {
                                lieu_action_register("L12D54A", P_BANDOR, "SP07ATT02", "SP07ATT01_10");
                                lieu_action_register("L12D54B", P_BANDOR, "SP07DIA51", "SP07ATT01_10");
                                ST(0x84) = 1;
                            } else {
                                lieu_action_register("L12D50A", P_BANDOR, "SP07ATT02", "SP07ARR01_10");
                                lieu_action_register("L12D50B", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                                ST(0x84) = 0;
                            }
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                    }
                    /* 0xA9EF */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0xAA01 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            /* 0xAA13 */
                            if (ST(0x84) == 0) {
                                lieu_action_register("L12D53", P_ARKHAN3, "SG03DIA02", NULL);
                                lieu_action_register("L12D54A", P_BANDOR, "SP07ATT02", "SP07ATT01_10");
                                lieu_action_register("L12D54B", P_BANDOR, "SP07DIA51", "SP07ATT01_10");
                                ST(0x84) = 1;
                            } else {
                                lieu_action_register("L12D300", P_ARKHAN3, "SG03DIA02", NULL);
                                lieu_action_register("L12D50A", P_BANDOR, "SP07ATT02", "SP07ARR01_10");
                                lieu_action_register("L12D50B", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                                ST(0x84) = 0;
                            }
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                        /* 0xAB11 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            ST(V_DLG_RANDOM) = CB->random(3);
                            CB->animation("SG00DIA08");
                            /* 0xAB45 */
                            if (ST(V_DLG_RANDOM) == 0) {
                                CB->dialogue("LCOD22A", P_ARKHAN3);
                            }
                            /* 0xAB67 */
                            if (ST(V_DLG_RANDOM) == 1) {
                                CB->dialogue("LCOD22B", P_ARKHAN3);
                            }
                            /* 0xAB89 */
                            if (ST(V_DLG_RANDOM) == 2) {
                                CB->dialogue("LCOD22C", P_ARKHAN3);
                            }
                            CB->animation("sar_10");
                            CB->animation("SP07ATT01");
                            CB->animation("SP83DIA51");
                            /* 0xABCC */
                            if (ST(V_DLG_RANDOM) < 2) {
                                lieu_action_register("L12D17", P_KHORAD, "SP83DIA51", "SP83ATT02_10");
                            }
                            /* 0xABF7 */
                            if (ST(0x88) == 0) {
                                lieu_action_register("L12D18A", P_BANDOR, "SP07ATT50", "SP07DIA51_10");
                                ST(0x88) = 1;
                            } else {
                                lieu_action_register("L12D18B", P_BANDOR, "SP07ATT02", "SP07DIA51_10");
                                ST(0x88) = 0;
                            }
                            CB->animation("SP83ATT01_10");
                            CB->animation("SP07ATT01_10");
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                    }
                }
                /* 0xACAD */
                if (ST(ST_PLAYER_WEAPON) == OB_BOW_TO_KEY) {
                    /* 0xACBF */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SG00PRE80");
                        CB->animation("G00PRE81");
                        CB->dialogue("L12D55A", P_ARKHAN2);
                        CB->animation("SG00PRE82");
                        lieu_action_register("L12D55B", P_ARKHAN2, "G02DIA03", NULL);
                        lieu_action_register("L12D55C", P_ARKHAN2, "SG02DIA04", NULL);
                        lieu_action_register("L12D57A", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                        lieu_action_register("L12D57B", P_BANDOR, "SP07DIA57", "SP07ARR01_10");
                        lieu_action_register("L12D57C", P_BANDOR, "SP07DIA60", "SP07ARR01_10");
                        ST(0xEDC) = 1;
                        /* 0xAD88 */
                        if (ST(0xED0) == 5) {
                            ST(0xED0) = 5;
                        }
                        /* 0xADA9 */
                        if (ST(0xED0) == 2) {
                            ST(0xED0) = 3;
                        }
                        /* 0xADCA */
                        if (ST(0xED0) == 1) {
                            ST(0xED0) = 2;
                        }
                        /* 0xADEB */
                        if (ST(0xED0) == 0) {
                            ST(0xED0) = 1;
                        }
                        /* 0xAE0C */
                        if (ST(ST_PLAYER_WEAPON) != 0) {
                            CB->animation("SP02PRE73");
                            CB->animation("P02_65535");
                        }
                        CB->link_objet(7, "OB12", 0x1B, 1);
                        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
                        lieu_pre_scene(P_ARKHAN2, P_BANDOR, 12, 1, 2);
                        /* 0xAE7A */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L12D58", P_ARKHAN2, "SG02DIA06", NULL);
                        }
                        /* 0xAEA2 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L12D59A", P_ARKHAN2, "SG02DIA08", NULL);
                            lieu_action_register("L12D59B", P_ARKHAN2, "SG02DIA02", NULL);
                        }
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xC1C) = 4;
                        CB->change_camera(2, CAM_AUTO);
                        return;
                    }
                    /* 0xAF28 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SG00PRE80");
                        CB->animation("G00PRE81");
                        CB->dialogue("L12D56A", P_ARKHAN3);
                        CB->animation("SG00PRE82");
                        lieu_action_register("L12D56B", P_ARKHAN3, "G00DIA03", NULL);
                        lieu_action_register("L12D57A", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                        lieu_action_register("L12D57B", P_BANDOR, "SP07DIA57", "SP07ARR01_10");
                        lieu_action_register("L12D57C", P_BANDOR, "SP07DIA60", "SP07ARR01_10");
                        ST(0xEDC) = 1;
                        /* 0xAFDB */
                        if (ST(0xED0) == 5) {
                            ST(0xED0) = 5;
                        }
                        /* 0xAFFC */
                        if (ST(0xED0) == 2) {
                            ST(0xED0) = 3;
                        }
                        /* 0xB01D */
                        if (ST(0xED0) == 1) {
                            ST(0xED0) = 2;
                        }
                        /* 0xB03E */
                        if (ST(0xED0) == 0) {
                            ST(0xED0) = 1;
                        }
                        CB->link_objet(7, "OB12", 0x1B, 1);
                        /* 0xB073 */
                        if (ST(ST_PLAYER_WEAPON) != 0) {
                            /* 0xB085 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                CB->animation("SP02PRE73");
                                CB->animation("ar");
                            }
                            /* 0xB0AD */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                CB->animation("SP03PRE73");
                                CB->animation("ar");
                            }
                        }
                        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
                        lieu_pre_scene(P_ARKHAN3, P_BANDOR, 12, 1, 2);
                        /* 0xB107 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L12D60", P_ARKHAN3, "SG03DIA06", NULL);
                        }
                        /* 0xB12F */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L12D61", P_ARKHAN3, "SG03DIA01", NULL);
                        }
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xC1C) = 4;
                        CB->change_camera(2, CAM_AUTO);
                        return;
                    }
                } else {
                    CB->animation("SG00PRE80");
                    /* 0xB1AF */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("L12D125", P_ARKHAN2);
                    }
                    /* 0xB1D1 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("L12D126", P_ARKHAN2);
                    }
                    ST(V_DLG_RANDOM) = CB->random(2);
                    /* 0xB20A */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L12D127", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                    }
                    /* 0xB235 */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L12D127", P_BANDOR, "SP07DIA50", "SP07ARR01_10");
                    }
                    CB->animation("G00PRE82");
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->change_camera(2, CAM_AUTO);
                    return;
                }
            }
            /* 0xB2A4 */
            if (ST(0xC1C) == 4) {
                /* 0xB2B6 */
                if (ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON) || CB->divers(DV_CAN_FIGHT, 0, 2) || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_2 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_1 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_3 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_KEY) {
                    /* 0xB341 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0xB353 */
                        if (ST(0x814) == 0) {
                            /* 0xB365 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L12D62A", P_ARKHAN2, "SG02DIA05", NULL);
                                lieu_action_register("L12D62B", P_ARKHAN2, "SG02DIA05", NULL);
                            }
                            /* 0xB3A3 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L12D63A", P_ARKHAN2, "SG02DIA05", NULL);
                                lieu_action_register("L12D63B", P_ARKHAN2, "SG02DIA02", NULL);
                            }
                            lieu_action_register("L12D65", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                            lieu_action_register("L12D66", P_ARKHAN2, "SG02DIA04", NULL);
                            lieu_action_register("L12D68A", P_BANDOR, "SP07DIA57", "SP07ARR01_10");
                            lieu_action_register("L12D68B", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                            lieu_action_register("L12D68C", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0x814) = 1;
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                        /* 0xB4A3 */
                        if (ST(0x814) == 1) {
                            /* 0xB4B5 */
                            if (ST(0xB0C) == 1) {
                                lieu_action_register("L12D69", P_ARKHAN2, "SG02DIA07", NULL);
                                lieu_action_register("L12D71", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x814) = 2;
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            }
                            /* 0xB53E */
                            if (ST(0xB0C) == 0) {
                                /* 0xB550 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D72", P_ARKHAN2, "SG02DIA05", NULL);
                                }
                                /* 0xB578 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L12D73", P_ARKHAN2, "SG02DIA05", NULL);
                                }
                                lieu_action_register("L12D75A", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                                lieu_action_register("L12D75B", P_BANDOR, "SP07DIA57", "SP07ARR01_10");
                                lieu_action_register("L12D75C", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                                lieu_action_register("L12D76A", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                                lieu_action_register("L12D76B", P_KHORAD, "SP83DIA57", "SP83ARR01_10");
                                CB->animation("SG00DIA06");
                                CB->dialogue("L12D77A", P_ARKHAN2);
                                CB->dialogue("L12D77B", P_ARKHAN2);
                                CB->animation("sar_10");
                                lieu_action_register("L12D78", P_BANDOR, "SP07DIA54", "SP07ARR01_10");
                                lieu_action_register("L12D79", P_KHORAD, "SP83DIA63", "SP83ARR01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x814) = 2;
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            }
                        }
                        /* 0xB6CD */
                        if (ST(0x814) == 2) {
                            /* 0xB6DF */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                CB->animation("SG00DIA06");
                                CB->dialogue("L12D81A", P_ARKHAN2);
                                CB->dialogue("L12D81B", P_ARKHAN2);
                                CB->animation("sar_10");
                                lieu_action_register("L12D84", P_BANDOR, "SP07DIA55", "SP07ARR01_10");
                                lieu_action_register("L12D85A", P_KHORAD, "SP83DIA57", "SP83ARR01_10");
                                lieu_action_register("L12D85B", P_KHORAD, "SP83DIA53", "SP83ARR01_10");
                            }
                            /* 0xB772 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                CB->animation("SP07DIA51");
                                lieu_action_register("L12D82A", P_ARKHAN2, "SG02DIA06", NULL);
                                lieu_action_register("L12D82B", P_ARKHAN2, "SG02DIA02", NULL);
                                lieu_action_register("L12D84", P_BANDOR, "SP07DIA55", "SP07ARR01_10");
                                lieu_action_register("L12D85A", P_KHORAD, "SP83DIA57", "SP83ARR01_10");
                                lieu_action_register("L12D85B", P_KHORAD, "SP83DIA50", "SP83ARR01_10");
                            }
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0x814) = 3;
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                        /* 0xB84E */
                        if (ST(0x814) == 3) {
                            /* 0xB860 */
                            if (ST(0xC20) == 1 && ST(V_EVENT15_STEP) == 0) {
                                /* 0xB884 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D91A", P_ARKHAN2, "SG02DIA06", NULL);
                                    lieu_action_register("L12D91B", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L12D92", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                                    lieu_action_register("L12D93A", P_KHORAD, "SP83DIA56", "SP83ARR01_10");
                                    lieu_action_register("L12D93B", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
                                }
                                /* 0xB90D */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L12D94A", P_ARKHAN2, "SG02DIA06", NULL);
                                    lieu_action_register("L12D94B", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L12D92", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                                    lieu_action_register("L12D95", P_KHORAD, "SP83DIA60", "SP83ARR01_10");
                                }
                                ST(V_EVENT15_STEP) = 1;
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            } else {
                                /* 0xB9CA */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    CB->animation("SG00DIA07");
                                    CB->dialogue("L12D86A", P_ARKHAN2);
                                    CB->dialogue("L12D86B", P_ARKHAN2);
                                    CB->animation("Sar_10");
                                    lieu_action_register("L12D87", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                                    lieu_action_register("L12D88", P_KHORAD, "SP83DIA57", "SP83ARR01_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                                /* 0xBA72 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L12D89A", P_ARKHAN2, "SG02DIA08", NULL);
                                    CB->animation("SP07DIA50_1000");
                                    lieu_action_register("L12D89B", P_ARKHAN2, "SG02DIA07", NULL);
                                    CB->animation("SP83ATT01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->change_camera(2, CAM_AUTO);
                                    return;
                                }
                            }
                        }
                    }
                    /* 0xBAFF */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0xBB11 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            ST(V_DLG_RANDOM) = CB->random(3);
                            CB->animation("SG00DIA08");
                            /* 0xBB45 */
                            if (ST(V_DLG_RANDOM) == 0) {
                                CB->dialogue("LCOD22A", P_ARKHAN3);
                            }
                            /* 0xBB67 */
                            if (ST(V_DLG_RANDOM) == 1) {
                                CB->dialogue("LCOD22B", P_ARKHAN3);
                            }
                            /* 0xBB89 */
                            if (ST(V_DLG_RANDOM) == 2) {
                                CB->dialogue("LCOD22C", P_ARKHAN3);
                            }
                            CB->animation("sar_10");
                            CB->animation("SP07ATT01");
                            CB->animation("SP83DIA51");
                            /* 0xBBCC */
                            if (ST(V_DLG_RANDOM) < 2) {
                                lieu_action_register("L12D17", P_KHORAD, "SP83DIA51", "SP83ATT02_10");
                            }
                            /* 0xBBF7 */
                            if (ST(0x88) == 0) {
                                lieu_action_register("L12D18A", P_BANDOR, "SP07ATT50", "SP07DIA51_10");
                                ST(0x88) = 1;
                            } else {
                                lieu_action_register("L12D18B", P_BANDOR, "SP07ATT02", "SP07DIA51_10");
                                ST(0x88) = 0;
                            }
                            CB->animation("SP83ATT01_10");
                            CB->animation("SP07ATT01_10");
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                        /* 0xBCAD */
                        if (ST(0x814) == 0) {
                            lieu_action_register("L12D64A", P_ARKHAN3, "SG03DIA04", NULL);
                            lieu_action_register("L12D64B", P_ARKHAN3, "SG03DIA05", NULL);
                            lieu_action_register("L12D65", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                            lieu_action_register("L12D67", P_ARKHAN3, "SG03DIA07", NULL);
                            lieu_action_register("L12D68A", P_BANDOR, "SP07DIA57", "SP07ARR01_10");
                            lieu_action_register("L12D68B", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                            lieu_action_register("L12D68C", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0x814) = 1;
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                        /* 0xBDAD */
                        if (ST(0x814) == 1) {
                            /* 0xBDBF */
                            if (ST(0xB0C) == 1) {
                                lieu_action_register("L12D70", P_ARKHAN3, "SG03DIA07", NULL);
                                lieu_action_register("L12D71", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x814) = 2;
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            }
                            /* 0xBE48 */
                            if (ST(0xB0C) == 0) {
                                lieu_action_register("L12D74", P_ARKHAN3, "SG03DIA05", NULL);
                                lieu_action_register("L12D75A", P_BANDOR, "SP07DIA56", "SP07ARR01_10");
                                lieu_action_register("L12D75B", P_BANDOR, "SP07DIA57", "SP07ARR01_10");
                                lieu_action_register("L12D75C", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                                lieu_action_register("L12D76A", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                                lieu_action_register("L12D76B", P_KHORAD, "SP83DIA57", "SP83ARR01_10");
                                lieu_action_register("L12D80", P_ARKHAN3, "SG03DIA06", NULL);
                                lieu_action_register("L12D78", P_BANDOR, "SP07DIA54", "SP07ARR01_10");
                                lieu_action_register("L12D79", P_KHORAD, "SP83DIA63", "SP83ARR01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x814) = 2;
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            }
                        }
                        /* 0xBF7D */
                        if (ST(0x814) == 2) {
                            lieu_action_register("L12D83", P_ARKHAN3, "SG03DIA06", NULL);
                            lieu_action_register("L12D84", P_BANDOR, "SP07DIA55", "SP07ARR01_10");
                            lieu_action_register("L12D85A", P_KHORAD, "SP83DIA57", "SP83ARR01_10");
                            lieu_action_register("L12D85B", P_KHORAD, "SP83DIA53", "SP83ARR01_10");
                            CB->animation("Sar_10");
                            ST(0x814) = 3;
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->change_camera(2, CAM_AUTO);
                            return;
                        }
                        /* 0xC038 */
                        if (ST(0x814) == 3) {
                            /* 0xC04A */
                            if (ST(0xC20) == 1 && ST(V_EVENT15_STEP) == 0) {
                                ST(V_EVENT15_STEP) = 1;
                                lieu_action_register("L12D96A", P_ARKHAN3, "SG03DIA07", NULL);
                                lieu_action_register("L12D96B", P_ARKHAN3, "SG03DIA05", NULL);
                                lieu_action_register("L12D92", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                                lieu_action_register("L12D93A", P_KHORAD, "SP83DIA56", "SP83ARR01_10");
                                lieu_action_register("L12D93B", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            } else {
                                lieu_action_register("L12D90", P_ARKHAN3, "SG03DIA02", NULL);
                                lieu_action_register("L12D87", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                                lieu_action_register("L12D88", P_KHORAD, "SP83DIA57", "SP83ARR01_10");
                                CB->animation("Sar_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                CB->change_camera(2, CAM_AUTO);
                                return;
                            }
                        }
                    }
                } else {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0xC1CC */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L12D121", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xC1F7 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L12D122", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    ST(V_DLG_RANDOM) = CB->random(2);
                    /* 0xC239 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L12D123", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                    }
                    /* 0xC264 */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L12D124", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                    }
                    CB->animation("SG00PRE82");
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->change_camera(2, CAM_AUTO);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
    /* 0xC2D0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_BANDOR)) == VIE_DEAD && CB->direction_spec(0x1B58) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 23)) {
        trig23();
        return;
    }
    /* 0xC35B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_BANDOR)) == VIE_DEAD && CB->direction_perso(P_BANDOR)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC3D6 */
        if (ST(0xEDC) == 0) {
            /* 0xC3E8 */
            if ((ST(0x5A8) == 0 || ST(0x5A8) == 1) && ST(0x184) != 2) {
                CB->animation("SG00PRE08");
                CB->etat_objet("OB21B", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x184) = 1;
                CB->retire_zone(2);
                return;
            }
            /* 0xC48A */
            if (ST(0x184) == 2) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->animation("SG00PRE08");
                CB->etat_objet("OB12", -1, -1, -1, -1, -1, -1, -3, 0);
                ST(0xEDC) = 1;
                /* 0xC4EB */
                if (ST(0xED0) == 5) {
                    ST(0xED0) = 5;
                }
                /* 0xC50C */
                if (ST(0xED0) == 2) {
                    ST(0xED0) = 3;
                }
                /* 0xC52D */
                if (ST(0xED0) == 1) {
                    ST(0xED0) = 2;
                }
                /* 0xC54E */
                if (ST(0xED0) == 0) {
                    ST(0xED0) = 1;
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x184) = 2;
                CB->retire_zone(2);
                return;
            }
        }
        /* 0xC5B2 */
        if (ST(0xEDC) == 1) {
            CB->fouille(P_BANDOR);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            return;
        }
    }
    /* 0xC5E2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && CB->direction_spec(P_BANDOR)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC64E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xC660 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0xC682 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD61", P_ARKHAN2);
            }
        }
        /* 0xC6A4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xC6B6 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
            /* 0xC6D8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD63", P_ARKHAN3);
            }
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xC720 */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        /* 0xC76E */
        if (ST(0x818) == 0) {
            CB->bloque(BLOQUE_ALL);
            CB->dialogue("L12D105A", P_BANDOR);
            CB->dialogue("L12D105B", P_BANDOR);
            ST(0x818) = 1;
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xC7C7 */
        if (ST(0x818) == 1) {
            CB->bloque(BLOQUE_ALL);
            CB->dialogue("L12D107", P_BANDOR);
            ST(V_ENGAGED_PERSONA) = 0x30F;
            CB->combat(P_BANDOR, 0);
            CB->combat(P_KHORAD, 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xC824 */
    if (ST(V_PLAYER_STRIKE) == 2 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
        CB->select_pile(0, 2);
        CB->select_pile(0, 1);
        ST(V_PLAYER_STRIKE) = -1;
        ST(V_ENGAGED_PERSONA) = 0x30F;
        CB->combat(P_BANDOR, 0);
        CB->combat(P_KHORAD, 0);
        CB->select_pile(0, 1);
        return;
    }
}

/* trigger 03 - sub_1000C8BD - Khorad: talk, loot, strikes */
static void trig03(void) {
    /* 0xC8CC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_KHORAD) && ST(ST_VIE(P_KHORAD)) != VIE_DEAD && ST(0xC1C) == 1) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC94A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D40", P_ARKHAN2, "SG02DIA02", NULL);
            lieu_action_register("L12D41", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xC9B4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D42", P_ARKHAN3, "SG03DIA02", NULL);
            lieu_action_register("L12D41", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xCA1B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KHORAD)) == VIE_DEAD && CB->direction_perso(P_KHORAD)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xCA87 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02PRE09");
            CB->animation("ar_10");
            CB->dialogue("L12D110", P_ARKHAN2);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->retire_zone(3);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xCAF3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03PRE09");
            CB->animation("ar_10");
            CB->dialogue("L12D111", P_ARKHAN3);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->retire_zone(3);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xCB5C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(P_KHORAD)) != VIE_DEAD && CB->direction_spec(P_KHORAD)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xCBC8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xCBDA */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0xCBFC */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD61", P_ARKHAN2);
            }
        }
        /* 0xCC1E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xCC30 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
            /* 0xCC52 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD63", P_ARKHAN3);
            }
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xCC9A */
    if (ST(V_PLAYER_STRIKE) == 2 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        CB->select_pile(0, 2);
        CB->select_pile(0, 1);
        ST(V_PLAYER_STRIKE) = -1;
        ST(V_ENGAGED_PERSONA) = 0x30F;
        CB->combat(P_BANDOR, 0);
        CB->combat(P_KHORAD, 0);
        CB->select_pile(0, 1);
        return;
    }
}

/* trigger 10 - sub_1000CD75 - take OB70 (Master Builder diagram) */
static void trig10(void) {
    /* 0xCD84 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x37C6)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xCDDE */
        if (ST(0x690) == 0) {
            /* 0xCDF0 */
            if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
                CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
                CB->animation("SP83ARR01_250");
                CB->animation("P83ARR01");
                CB->joindre(P_KHORAD, 0, 0, 0, ST(ST_HERO));
                /* 0xCE64 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L12D30", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
                    lieu_action_register("L12D31", P_BANDOR, "SP07DIA60", "SP07ARR01_10");
                    CB->animation("SG02PRE06");
                    CB->etat_objet("OB70", -1, -1, -1, -1, -1, -1, -3, 4);
                    CB->animation("ar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x690) = 1;
                    return;
                }
                /* 0xCF09 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L12D30", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
                    lieu_action_register("L12D31", P_BANDOR, "SP07DIA60", "SP07ARR01_10");
                    CB->animation("SG03PRE06");
                    CB->etat_objet("OB70", -1, -1, -1, -1, -1, -1, -3, 4);
                    CB->animation("ar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x690) = 1;
                    return;
                }
            }
            /* 0xCFAE */
            if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
                /* 0xCFD2 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02PRE06");
                    CB->etat_objet("OB70", -1, -1, -1, -1, -1, -1, -3, 4);
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x690) = 1;
                    return;
                }
                /* 0xD045 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG03PRE06");
                    CB->etat_objet("OB70", -1, -1, -1, -1, -1, -1, -3, 4);
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x690) = 1;
                    return;
                }
            }
        }
        /* 0xD0B8 */
        if (ST(0x690) == 1) {
            /* 0xD0CA */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02PRE02");
                CB->dialogue("L12D28", P_ARKHAN2);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
            /* 0xD11B */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03PRE02");
                CB->dialogue("L12D29", P_ARKHAN3);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
        }
    }
}

/* trigger 11 - sub_1000D165 - zone 29 interaction (ACT28/29 anim; Bandor scene when holding OB70) */
static void trig11(void) {
    /* 0xD174 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(29)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD1CE */
        if (ST(ST_PLAYER_WEAPON) != OB_BANDOR_DIAG || (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD)) {
            /* 0xD216 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xD228 */
                if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ACT28");
                }
                /* 0xD257 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT28");
                }
                /* 0xD274 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT28");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xD2BA */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xD2CC */
                if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ACT29");
                }
                /* 0xD2FB */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT29");
                }
                /* 0xD318 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT29");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xD35E */
        if (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
            /* 0xD394 */
            if (ST(0xC1C) == 0) {
                /* 0xD3A6 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->joindre(P_ARKHAN2, 7598, -1748, 0x4399, -1);
                    CB->animation("SP02ATT10");
                    CB->dialogue("L12D35A", P_ARKHAN2);
                    CB->dialogue("L12D35B", P_ARKHAN2);
                    CB->animation("sar_10");
                    CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 0);
                    CB->change_camera(3, CAM_LOCKED);
                    CB->animation("SP07MAR01_200");
                    lieu_action_register("L12D38", P_BANDOR, "P07ATT50", "SP07MAR01_10");
                    CB->change_camera(2, CAM_LOCKED);
                    CB->animation("Sar_50");
                    CB->dialogue("L12D39", P_BANDOR);
                    CB->joindre(P_BANDOR, 7318, -1540, 0x8F3B, -1);
                    CB->change_camera(1, CAM_LOCKED);
                    lieu_action_register("L14D45", P_ARKHAN2, "SG02DIA03", NULL);
                    ST(0xC1C) = 1;
                    CB->evenement(0, 1400);
                    return;
                }
                /* 0xD4DE */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->joindre(P_ARKHAN3, 7603, -1785, 0x4268, -1);
                    CB->animation("SP03ATT10");
                    CB->dialogue("L12D36", P_ARKHAN3);
                    CB->dialogue("L12D37", P_ARKHAN3);
                    CB->animation("sar_10");
                    CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 0);
                    CB->change_camera(3, CAM_LOCKED);
                    CB->animation("SP07MAR01_200");
                    lieu_action_register("L12D38", P_BANDOR, "P07ATT50", "SP07MAR01_10");
                    CB->change_camera(2, CAM_LOCKED);
                    CB->animation("Sar_50");
                    CB->dialogue("L12D39", P_BANDOR);
                    CB->joindre(P_BANDOR, 7318, -1540, 0x8F3B, -1);
                    CB->change_camera(1, CAM_LOCKED);
                    lieu_action_register("L14D46", P_ARKHAN3, "SG03DIA02", NULL);
                    ST(0xC1C) = 1;
                    CB->evenement(0, 1400);
                    return;
                }
            }
            /* 0xD616 */
            if (ST(0xC1C) >= 1) {
                /* 0xD628 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L12D45", P_ARKHAN2, "SG02DIA03", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xD679 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L12D46", P_ARKHAN3, "SG03DIA03", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
}

/* trigger 12 - sub_1000D6C3 - zone 22 interaction (same scene as trigger 11, other side) */
static void trig12(void) {
    /* 0xD6D2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(22)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD72C */
        if (ST(ST_PLAYER_WEAPON) != OB_BANDOR_DIAG || (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD)) {
            /* 0xD774 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xD786 */
                if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ACT28");
                }
                /* 0xD7B5 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT28");
                }
                /* 0xD7D2 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT28");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xD818 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xD82A */
                if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ACT29");
                }
                /* 0xD859 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT29");
                }
                /* 0xD876 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT29");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xD8BC */
        if (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
            /* 0xD8F2 */
            if (ST(0xC1C) == 0) {
                /* 0xD904 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    ST(0x180) = 1;
                    CB->joindre(P_BANDOR, 7300, -1070, 0xC350, -1);
                    CB->joindre(P_ARKHAN2, 7773, -960, 0x8032, -1);
                    CB->joindre(P_KHORAD, 0, 0, 0, ST(ST_HERO));
                    CB->animation("SP02ATT10");
                    CB->dialogue("L12D35A", P_ARKHAN2);
                    CB->dialogue("L12D35B", P_ARKHAN2);
                    CB->animation("sar_10");
                    CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 0);
                    CB->change_camera(3, CAM_LOCKED);
                    CB->animation("SP07MAR01_200");
                    lieu_action_register("L12D38", P_BANDOR, "P07ATT50", "SP07MAR01_10");
                    CB->change_camera(2, CAM_LOCKED);
                    CB->animation("Sar_50");
                    CB->dialogue("L12D39", P_BANDOR);
                    CB->joindre(P_BANDOR, 7518, -1120, 0xC2D1, -1);
                    CB->change_camera(4, CAM_LOCKED);
                    CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
                    lieu_action_register("L14D45", P_ARKHAN2, "SG02DIA03", NULL);
                    ST(0xC1C) = 1;
                    CB->evenement(0, 1000);
                    return;
                }
                /* 0xDA9D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    ST(0x180) = 1;
                    CB->joindre(P_BANDOR, 7300, -1070, 0xC350, -1);
                    CB->joindre(P_ARKHAN2, 7773, -960, 0x8032, -1);
                    CB->joindre(P_KHORAD, 0, 0, 0, ST(ST_HERO));
                    CB->animation("SP03ATT10");
                    CB->dialogue("L12D36", P_ARKHAN3);
                    CB->dialogue("L12D37", P_ARKHAN3);
                    CB->animation("sar_10");
                    CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 0);
                    CB->change_camera(3, CAM_LOCKED);
                    CB->animation("SP07MAR01_200");
                    lieu_action_register("L12D38", P_BANDOR, "P07ATT50", "SP07MAR01_10");
                    CB->change_camera(2, CAM_LOCKED);
                    CB->animation("Sar_50");
                    CB->dialogue("L12D39", P_BANDOR);
                    CB->joindre(P_BANDOR, 7518, -1120, 0xC2D1, -1);
                    CB->change_camera(4, CAM_LOCKED);
                    CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
                    lieu_action_register("L14D46", P_ARKHAN3, "SG03DIA02", NULL);
                    ST(0xC1C) = 1;
                    CB->evenement(0, 1000);
                    return;
                }
            }
            /* 0xDC36 */
            if (ST(0xC1C) >= 1) {
                /* 0xDC48 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L12D45", P_ARKHAN2, "SG02DIA04", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xDC99 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L12D46", P_ARKHAN3, "SG03DIA04", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
    /* 0xDCE7 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && !CB->direction_spec(0x16) && CB->direction_spec(0xCB20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xDD54 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D103", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xDDA5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D104", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 13 - sub_1000DDEF - OB06 take / look, zone 27 sign */
static void trig13(void) {
    /* 0xDDFE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xAFE2) && !CB->direction_zone(13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xDE6B */
        if (ST(ST_VIE(96)) == 0) {
            ST(ST_VIE(96)) = 1;
            CB->animation("SG00PRE06");
            CB->etat_objet("OB06", -1, -1, -1, -1, -1, -1, -4, 4);
            CB->etat_objet("OB06", -1, -1, -1, -1, -1, -1, -4, 4);
            CB->etat_objet("OB06", -1, -1, -1, -1, -1, -1, -3, 4);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xDF1A */
        if (ST(ST_VIE(96)) == 1) {
            /* 0xDF2C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L12D97", P_ARKHAN2, "SG02PRE02", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xDF7D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L12D98", P_ARKHAN3, "SG03PRE02", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xDFCB */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(27)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE025 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0xE04D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 - sub_1000E097 - look at 0x7273, zone 27 sign */
static void trig14(void) {
    /* 0xE0A6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x7273) && !CB->direction_zone(13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE113 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D99", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xE164 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD77", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xE1B2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(27)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE20C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0xE234 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 - sub_1000E27E - zone 16: look */
static void trig15(void) {
    /* 0xE28D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(16)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE2E7 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D101", P_ARKHAN2, "SG02PRE03", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xE338 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D102", P_ARKHAN3, "SG03PRE03", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 17 - sub_1000E38D - climb animation (P02/P03ATT76/77), timer 34, music */
static void trig17(void) {
    /* 0xE39C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x8412)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xE3F2 */
        if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
            /* 0xE404 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x8282, 1, "ar", 1, 0);
                CB->animation("Sar_10");
                CB->animation("P02ATT77");
            }
            /* 0xE481 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x8282, 1, "ar", 1, 0);
                CB->animation("Sar_10");
                CB->animation("P03ATT77");
            }
        }
        /* 0xE506 */
        if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
            /* 0xE518 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x8282, 1, "ar", 1, 0);
                CB->animation("Sar_10");
                CB->animation("P02ATT76");
            }
            /* 0xE595 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x8282, 1, "ar", 1, 0);
                CB->animation("Sar_10");
                CB->animation("P03ATT76");
            }
        }
        CB->evenement(34, 400);
        /* 0xE622 */
        if (ST(0xEFC) != 1) {
            CB->divers(DV_MUSIC, 1, 4);
        }
        /* 0xE646 */
        if (ST(0xEFC) == 1) {
            CB->divers(DV_MUSIC, 2, 4);
        }
    }
}

/* trigger 18 - sub_1000E663 - look (0x526B, L12D103/104) */
static void trig18(void) {
    /* 0xE672 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x526B)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE6CC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D103", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xE71D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D104", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 19 - sub_1000E767 - look (0x526B, LCOD07/08) */
static void trig19(void) {
    /* 0xE776 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x526B)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE7D0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xE821 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 20 - sub_1000E86B - shrine at 0x851: Bandor / Khorad react, sets 0x4BC / 0x4C0 */
static void trig20(void) {
    /* 0xE87A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x851)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE8D4 */
        if (ST(0x4BC) == 0) {
            /* 0xE8E6 */
            if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD || ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
                CB->joindre(ST(ST_HERO), 7686, -1028, 0xFFDE, -1);
                /* 0xE92C */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02PRE04_500");
                    CB->animation("G02PRE04_500");
                }
                /* 0xE954 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG03PRE04_500");
                    CB->animation("G03PRE04_500");
                }
                CB->divers(DV_LOOK_AT_PLAYER, P_KHORAD, 0);
                CB->change_camera(3, CAM_LOCKED);
                /* 0xE99E */
                if (ST(0xC1C) != 1) {
                    CB->joindre(P_BANDOR, 0, 0, 0, P_KHORAD);
                }
                /* 0xE9C3 */
                if (ST(0xC1C) == 1) {
                    CB->joindre(P_BANDOR, 7294, -1026, 0x6633, -1);
                }
                CB->joindre(P_KHORAD, 0, 0, 0, P_BANDOR);
                lieu_action_register("L12D119", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                lieu_action_register("L12D120", P_BANDOR, "SP07DIA55", "SP07ARR01_10");
                CB->change_camera(4, CAM_LOCKED);
                CB->change_camera(4, CAM_AUTO);
                CB->divers_empile(DV_LOOK_AT_PLAYER, P_KHORAD, 1);
                /* 0xEA68 */
                if ((ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) != 0) && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    CB->animation("Sar_10");
                    /* 0xEAAE */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("P02ATT30");
                    }
                    /* 0xEACB */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("P03ATT30");
                    }
                    CB->modify_var(&ST(0x4BC), 1);
                    ST(0x4C0) = 1;
                }
                /* 0xEB0E */
                if (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    CB->animation("Sar_10");
                    /* 0xEB42 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT31");
                    }
                    /* 0xEB5F */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT31");
                    }
                    CB->modify_var(&ST(0x4BC), 1);
                    ST(0x4C0) = 1;
                }
                CB->animation("ar_10");
                CB->change_camera(3, CAM_LOCKED);
                CB->play_anim(0, 0);
                CB->bruitage("L12B01", -1, -1);
                CB->evenement(1, 500);
                return;
            }
            /* 0xEBEE */
            if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
                CB->joindre(ST(ST_HERO), 7686, -1028, 0xFFDE, -1);
                /* 0xEC34 */
                if ((ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) != 0) && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    /* 0xEC6F */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT30");
                    }
                    /* 0xEC8C */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT30");
                    }
                }
                /* 0xECA9 */
                if (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    /* 0xECD2 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT31");
                    }
                    /* 0xECEF */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT31");
                    }
                }
                CB->modify_var(&ST(0x4BC), 1);
                ST(0x4C0) = 1;
                CB->animation("ar");
                CB->change_camera(3, CAM_LOCKED);
                CB->play_anim(0, 0);
                CB->bruitage("L12B01", -1, -1);
                CB->evenement(1, 500);
                return;
            }
        }
        /* 0xED7E */
        if (ST(0x4C0) == 1) {
            /* 0xED90 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00ATT01");
                CB->dialogue("L12D117", P_ARKHAN2);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xEDE6 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG00ATT01");
                CB->dialogue("L12D118", P_ARKHAN3);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 21 - sub_1000EE35 - look (0xB7E7, LCOD07/08) */
static void trig21(void) {
    /* 0xEE44 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xB7E7)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xEE9E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xEEEF */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 23 - sub_1000EF44 - chest at 0x13CE: OB21B / OB12 chain (also called from trigger 2) */
static void trig23(void) {
    /* 0xEF53 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x13CE)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xEFAD */
        if (ST(0x4C0) == 1) {
            /* 0xEFBF */
            if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(0xEDC) == 1) {
                /* 0xEFE3 */
                if (ST(0x184) == 0) {
                    CB->animation("SG00PRE06");
                    CB->etat_objet("OB21B", -1, -1, -1, -1, -1, -1, -3, 0);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x184) = 1;
                    return;
                }
                /* 0xF056 */
                if (ST(0x184) == 1 || ST(0x184) == 2) {
                    /* 0xF07A */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD07", P_ARKHAN2);
                    }
                    /* 0xF09C */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD08", P_ARKHAN3);
                    }
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xF0E7 */
            if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(0xEDC) == 0) {
                /* 0xF10B */
                if (ST(0x5A8) == 0 && ST(0x184) != 2) {
                    CB->animation("SG00PRE06");
                    CB->etat_objet("OB12", -1, -1, -1, -1, -1, -1, -3, 0);
                    ST(0xEDC) = 1;
                    /* 0xF167 */
                    if (ST(0xED0) == 5) {
                        ST(0xED0) = 5;
                    }
                    /* 0xF188 */
                    if (ST(0xED0) == 2) {
                        ST(0xED0) = 3;
                    }
                    /* 0xF1A9 */
                    if (ST(0xED0) == 1) {
                        ST(0xED0) = 2;
                    }
                    /* 0xF1CA */
                    if (ST(0xED0) == 0) {
                        ST(0xED0) = 1;
                    }
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x5A8) = 1;
                    return;
                }
                /* 0xF223 */
                if (ST(0x5A8) == 1 || ST(0x184) == 2) {
                    /* 0xF247 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD07", P_ARKHAN2);
                    }
                    /* 0xF269 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD08", P_ARKHAN3);
                    }
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xF2B4 */
            if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(0xEDC) == 0) {
                /* 0xF2D8 */
                if (ST(0x184) == 0) {
                    CB->animation("SG00PRE06");
                    CB->etat_objet("OB21B", -1, -1, -1, -1, -1, -1, -3, 0);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x184) = 2;
                    return;
                }
                /* 0xF34B */
                if (ST(0x184) == 2) {
                    /* 0xF35D */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD07", P_ARKHAN2);
                    }
                    /* 0xF37F */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD08", P_ARKHAN3);
                    }
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xF3CA */
            if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(0xEDC) == 1) {
                /* 0xF3EE */
                if (ST(0x184) == 0) {
                    CB->animation("SG00PRE06");
                    CB->etat_objet("OB21B", -1, -1, -1, -1, -1, -1, -3, 0);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x184) = 1;
                    return;
                }
                /* 0xF461 */
                if (ST(0x184) == 1 || ST(0x184) == 2) {
                    /* 0xF485 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD07", P_ARKHAN2);
                    }
                    /* 0xF4A7 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD08", P_ARKHAN3);
                    }
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
        /* 0xF4F2 */
        if (ST(0x4C0) == 0) {
            CB->animation("SG00PRE02");
            /* 0xF50F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD07", P_ARKHAN2);
            }
            /* 0xF531 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD08", P_ARKHAN3);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 24 - sub_1000F575 - look (0xFADA, LCOD39/40) */
static void trig24(void) {
    /* 0xF584 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xFADA)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE04");
        /* 0xF5E9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", P_ARKHAN2);
        }
        /* 0xF60B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", P_ARKHAN3);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 25 - sub_1000F64F - look (0x7683, LCOD39/40) */
static void trig25(void) {
    /* 0xF65E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x7683)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE02");
        /* 0xF6C3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", P_ARKHAN2);
        }
        /* 0xF6E5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", P_ARKHAN3);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 26 - sub_1000F729 - look (0xCCA7, L12D103/104) */
static void trig26(void) {
    /* 0xF738 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xCCA7)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xF7A1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D103", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xF7F2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D104", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 28 - sub_1000F847 - zone 13: look (LCOD29/30) */
static void trig28(void) {
    /* 0xF856 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xF8B0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0xF8D8 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 30 - sub_1000F92D - zone 31: ladder animation (ACT11/12/85/86) */
static void trig30(void) {
    /* 0xF93C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(31)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        /* 0xF9FF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xFA11 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xFA2B */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT11");
                } else {
                    CB->animation("SP02ACT12");
                }
            }
            /* 0xFA58 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xFA72 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT86");
                } else {
                    CB->animation("SP02ACT85");
                }
            }
        }
        /* 0xFA9F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xFAB1 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xFACB */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT11");
                } else {
                    CB->animation("SP03ACT12");
                }
            }
            /* 0xFAF8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xFB12 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT86");
                } else {
                    CB->animation("SP03ACT85");
                }
            }
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 41 - sub_100079EA (patch) - start-up scene: Bandor and Khorad talk (variant by 0xC18); the screen divers_empile(16,3,0) now lives here, not in trigger 0 */
static void trig41(void) {
    /* 0x79FC */
    if (ST(0xC18) == 2 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 1);
        CB->divers(DV_LOOK_AT_PLAYER, P_KHORAD, 1);
        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
        CB->joindre(P_KHORAD, 0, 0, 0, ST(ST_HERO));
        ST(0x64) = CB->random(3);
        /* 0x7ABE */
        if (ST(0x64) == 0) {
            lieu_action_register("L12D03", P_BANDOR, "SP07DIA63", "SP07ARR01_10");
        }
        /* 0x7AE6 */
        if (ST(0x64) == 1) {
            lieu_action_register("L12D04", P_BANDOR, "SP07DIA63", "SP07ARR01_10");
        }
        /* 0x7B0E */
        if (ST(0x64) == 2) {
            lieu_action_register("L12D05", P_BANDOR, "SP07DIA63", "SP07ARR01_10");
        }
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7B41 */
    if (ST(0xC18) == 1) {
        CB->bloque(BLOQUE_ALL);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 1);
        CB->divers(DV_LOOK_AT_PLAYER, P_KHORAD, 1);
        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L12D01", P_BANDOR, "SP07ATT50", "SP07ARR01_10");
        lieu_action_register("L12D02", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
        CB->bloque(BLOQUE_NONE);
        ST(0xC18) = 2;
    }
    /* 0x7BFA (patch) */
    if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7C43 */
    if (ST(0xEFC) != 1) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x7C67 */
    if (ST(0xEFC) == 1) {
        CB->divers(DV_MUSIC, 2, 1);
    }
}

