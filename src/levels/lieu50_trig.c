/*
 * lieu50_trig.c - Native hand-port of the LIEU50.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu50.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu50_int.h"

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

/* dword_100223A0[42] - filled at INIT (before the reload-flag test) with the
 * 5-byte jmp thunks at 0x10001000+5*i; static here.
 *   idx handler        idx handler        idx handler
 *    0  10001200          14  10007545          28  10008B88
 *    1  10002602 stub     15  1000772C          29  10008C63 stub
 *    2  1000260D          16  10007830 stub     30  10008C6E
 *    3  1000601A          17  1000783B          31  10008EA2 stub
 *    4  100062A2 stub     18  10007B11          32  10008EAD stub
 *    5  100062AD stub     19  10007C15          33  10008EB8 stub
 *    6  100062B8 stub     20  10007D19          34  10008EC3 stub
 *    7  100062C3 stub     21  100082CB          35  10008ECE stub
 *    8  100062CE stub     22  100083CF stub     36  10008ED9 stub
 *    9  100062D9 stub     23  100083DA          37  10008EE4 stub
 *   10  100062E4          24  100088B6          38  10008EEF stub
 *   11  100066D4          25  10008990          39  10008EFA stub
 *   12  10006C32          26  10008A6A          40  10008F05 stub
 *   13  1000735E          27  10008B7D stub     41  10001746
 */
static void (*const g_trigger[42])(void) = {
    l50_trig00, trig_noop, trig02, trig03, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig10, trig11, trig12, trig13, trig14, trig15,
    trig_noop, trig17, trig18, trig19, trig20, trig21, trig_noop,
    trig23, trig24, trig25, trig26, trig_noop, trig28, trig_noop,
    trig30, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x100223A0]). */
void l50_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 - sub_10001200 - level setup: entry presets, zones, actor placement */
void l50_trig00(void) {
    if (ST(ST_START_POINT(12)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_LEVEL) = 11;
    }
    if (ST(ST_START_POINT(12)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xEFC) = 1;
        ST(ST_LEVEL) = 11;
    }
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
    ST(0x100) = 0;
    CB->bruitage("L12AMB1", 7553, -1342);
    CB->load_anim("L12OUV01", 0);
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        if (ST(0xC18) == 0) {
            ST(0xC18) = 1;
        }
        CB->pos_perso(P_KHORAD, 7385, -997, 0x7530, 1, "P83ARR01", 3, 0);
        CB->pos_perso(P_BANDOR, 7294, -1026, 0x6633, 1, "P07ATT01", 2, 0);
    }
    if (ST(0xEFC) != 1) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    if (ST(0xEFC) == 1) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD) {
        CB->pos_perso(P_BANDOR, 0, 0, 0, 2, "P07cad", 2, 0);
    }
    if (ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
        CB->pos_perso(P_KHORAD, 0, 0, 0, 2, "P83cad", 3, 0);
    }
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->load_anim("LEVIER", 1);
    }
}

/* trigger 02 - sub_1000260D - Bandor / Khorad: talk by stage (0xC1C), weapon reactions, loot, strikes */
static void trig02(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x1388) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 23)) {
        trig23();
        return;
    }
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_BANDOR) && ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(0xC1C) < 2) {
            i32 ang_bandor = CB->divers(DV_ANGLE_TO, P_BANDOR, ST(ST_HERO));
            i32 ang_khorad = CB->divers(DV_ANGLE_TO, P_KHORAD, ST(ST_HERO));
            if (ang_bandor < ang_khorad || ST(0xC1C) == 1) {
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
                ST(0x70) = 0;
            } else {
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_KHORAD);
                ST(0x70) = 1;
            }
        }
        if (ST(0xC1C) >= 2) {
            CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
            ST(0x70) = 0;
        }
        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP83ARR01_250");
        CB->animation("P83ARR01");
        CB->joindre(P_KHORAD, 0, 0, 0, ST(ST_HERO));
        if (ST(0xC1C) != 1) {
            CB->change_camera(-4, CAM_LOCKED);
        }
        if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
            if (ST(0xC1C) == 0) {
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON)) {
                    if (ST(0x690) == 0 || ST(0x68) == 1) {
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            if (ST(0x80C) == 0) {
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
                            if (ST(0x80C) == 1) {
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
                            if (ST(0x80C) == 2) {
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D19A", P_ARKHAN2, "SG02DIA02", NULL);
                                    lieu_action_register("L12D19B", P_ARKHAN2, "SG02DIA07", NULL);
                                }
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
                            if (ST(0x80C) == 3) {
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D200", P_ARKHAN2, "SG02DIA02", NULL);
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
                                if (ST(ST_SPECIAL_MODE) == 0) {
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
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            if (ST(0x80C) == 0) {
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
                            if (ST(0x80C) == 3 && ST(ST_SPECIAL_MODE) == 1) {
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
                            if (ST(0x80C) >= 1 && ST(ST_SPECIAL_MODE) == 0) {
                                ST(V_DLG_RANDOM) = CB->random(3);
                                CB->animation("SG00DIA08");
                                if (ST(V_DLG_RANDOM) == 0) {
                                    CB->dialogue("LCOD22A", P_ARKHAN3);
                                }
                                if (ST(V_DLG_RANDOM) == 1) {
                                    CB->dialogue("LCOD22B", P_ARKHAN3);
                                }
                                if (ST(V_DLG_RANDOM) == 2) {
                                    CB->dialogue("LCOD22C", P_ARKHAN3);
                                }
                                CB->animation("sar_10");
                                CB->animation("SP07ATT01");
                                CB->animation("SP83DIA51");
                                if (ST(V_DLG_RANDOM) < 2) {
                                    lieu_action_register("L12D17", P_KHORAD, "SP83ATT02", "SP83DIA54_10");
                                }
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
                    if (ST(0x690) == 1 && ST(0x68) == 0) {
                        ST(0x68) = 1;
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L12D32", P_ARKHAN2, "SG02DIA05", NULL);
                        }
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
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != OB_BANDOR_DIAG && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L12D121", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L12D122", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    ST(V_DLG_RANDOM) = CB->random(2);
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L12D123", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                    }
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
                if (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(0x68) = 1;
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    ST(0x68) = 1;
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L12D32", P_ARKHAN2, "SG02DIA05", NULL);
                    }
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
            if (ST(0xC1C) == 1) {
                if (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        if (ST(0x180) == 0 && ST(ST_LOCKED_CAMERA) != 2) {
                            CB->change_camera(2, CAM_LOCKED);
                        }
                        lieu_action_register("L12D40", P_ARKHAN2, "SG02DIA03", NULL);
                        lieu_action_register("L12D43", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                        lieu_v1_pre_scene(ST(ST_HERO), P_BANDOR, 70, 1, 2);
                        CB->link_objet(7, "OB70", 0x1B, 0);
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->joindre(P_KHORAD, 7356, -985, 0x8906, -1);
                        CB->joindre(P_BANDOR, 7294, -1026, 0x6633, -1);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xC1C) = 2;
                        if (ST(0x180) == 0) {
                            CB->change_camera(2, CAM_AUTO);
                        }
                        return;
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        if (ST(0x180) == 0 && ST(ST_LOCKED_CAMERA) != 2) {
                            CB->change_camera(2, CAM_LOCKED);
                        }
                        lieu_action_register("L12D42", P_ARKHAN3, "SG03DIA07", NULL);
                        lieu_action_register("L12D43", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                        lieu_v1_pre_scene(ST(ST_HERO), P_BANDOR, 70, 1, 2);
                        CB->link_objet(7, "OB70", 0x1B, 0);
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->joindre(P_KHORAD, 7356, -985, 0x8906, -1);
                        CB->joindre(P_BANDOR, 7294, -1026, 0x6633, -1);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xC1C) = 2;
                        if (ST(0x180) == 0) {
                            CB->change_camera(2, CAM_AUTO);
                        }
                        return;
                    }
                }
                if (ST(ST_PLAYER_WEAPON) != OB_BANDOR_DIAG) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->joindre(ST(ST_HERO), 0, 0, 0, P_BANDOR);
                        lieu_action_register("L12D40", P_ARKHAN2, "SG02DIA03", NULL);
                        lieu_action_register("L12D43", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
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
            if (ST(0xC1C) == 2) {
                if (ST(ST_HERO) == P_ARKHAN2 && ST(0x810) == 0) {
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
                if (ST(ST_HERO) == P_ARKHAN3) {
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
                    if (ST(0x810) >= 0 && ST(ST_SPECIAL_MODE) == 0) {
                        ST(V_DLG_RANDOM) = CB->random(3);
                        CB->animation("SG00DIA08");
                        if (ST(V_DLG_RANDOM) == 0) {
                            CB->dialogue("LCOD22A", P_ARKHAN3);
                        }
                        if (ST(V_DLG_RANDOM) == 1) {
                            CB->dialogue("LCOD22B", P_ARKHAN3);
                        }
                        if (ST(V_DLG_RANDOM) == 2) {
                            CB->dialogue("LCOD22C", P_ARKHAN3);
                        }
                        CB->animation("sar_10");
                        CB->animation("SP07ATT01");
                        CB->animation("SP83DIA51");
                        if (ST(V_DLG_RANDOM) < 2) {
                            lieu_action_register("L12D17", P_KHORAD, "SP83DIA51", "SP83ATT02_10");
                        }
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
            if (ST(0xC1C) == 3) {
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON)) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        if (ST(ST_SPECIAL_MODE) == 1) {
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
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L12D52A", P_ARKHAN2, "SG02DIA02", NULL);
                            CB->animation("SP07ATT01_10");
                            CB->animation("SP83ATT01_10");
                            lieu_action_register("L12D52B", P_ARKHAN2, "SG02DIA07", NULL);
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
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        if (ST(ST_SPECIAL_MODE) == 1) {
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
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            ST(V_DLG_RANDOM) = CB->random(3);
                            CB->animation("SG00DIA08");
                            if (ST(V_DLG_RANDOM) == 0) {
                                CB->dialogue("LCOD22A", P_ARKHAN3);
                            }
                            if (ST(V_DLG_RANDOM) == 1) {
                                CB->dialogue("LCOD22B", P_ARKHAN3);
                            }
                            if (ST(V_DLG_RANDOM) == 2) {
                                CB->dialogue("LCOD22C", P_ARKHAN3);
                            }
                            CB->animation("sar_10");
                            CB->animation("SP07ATT01");
                            CB->animation("SP83DIA51");
                            if (ST(V_DLG_RANDOM) < 2) {
                                lieu_action_register("L12D17", P_KHORAD, "SP83DIA51", "SP83ATT02_10");
                            }
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
                if (ST(ST_PLAYER_WEAPON) == OB_BOW_TO_KEY || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_1 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_3) {
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
                        if (ST(0xED0) == 5) {
                            ST(0xED0) = 5;
                        }
                        if (ST(0xED0) == 2) {
                            ST(0xED0) = 3;
                        }
                        if (ST(0xED0) == 1) {
                            ST(0xED0) = 2;
                        }
                        if (ST(0xED0) == 0) {
                            ST(0xED0) = 1;
                        }
                        if (ST(ST_PLAYER_WEAPON) != 0) {
                            CB->animation("SP02PRE73");
                            CB->animation("P02_65535");
                        }
                        CB->link_objet(7, "OB12", 0x1B, 1);
                        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
                        lieu_v1_pre_scene(P_ARKHAN2, P_BANDOR, 12, 1, 2);
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L12D58", P_ARKHAN2, "SG02DIA06", NULL);
                        }
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
                        if (ST(0xED0) == 5) {
                            ST(0xED0) = 5;
                        }
                        if (ST(0xED0) == 2) {
                            ST(0xED0) = 3;
                        }
                        if (ST(0xED0) == 1) {
                            ST(0xED0) = 2;
                        }
                        if (ST(0xED0) == 0) {
                            ST(0xED0) = 1;
                        }
                        CB->link_objet(7, "OB12", 0x1B, 1);
                        if (ST(ST_PLAYER_WEAPON) != 0) {
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                CB->animation("SP02PRE73");
                                CB->animation("ar");
                            }
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                CB->animation("SP03PRE73");
                                CB->animation("ar");
                            }
                        }
                        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
                        lieu_v1_pre_scene(P_ARKHAN3, P_BANDOR, 12, 1, 2);
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L12D60", P_ARKHAN3, "SG03DIA06", NULL);
                        }
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
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    CB->animation("SG00PRE80");
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("L12D125", P_ARKHAN2);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("L12D126", P_ARKHAN2);
                    }
                    ST(V_DLG_RANDOM) = CB->random(2);
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L12D127", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
                    }
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
            if (ST(0xC1C) == 4) {
                if (ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON) || CB->divers(DV_CAN_FIGHT, 0, 2) || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_2 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_1 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_3 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_KEY) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        if (ST(0x814) == 0) {
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L12D62A", P_ARKHAN2, "SG02DIA05", NULL);
                                lieu_action_register("L12D62B", P_ARKHAN2, "SG02DIA05", NULL);
                            }
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
                        if (ST(0x814) == 1) {
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
                            if (ST(0xB0C) == 0) {
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D72", P_ARKHAN2, "SG02DIA05", NULL);
                                }
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
                        if (ST(0x814) == 2) {
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                CB->animation("SG00DIA06");
                                CB->dialogue("L12D81A", P_ARKHAN2);
                                CB->dialogue("L12D81B", P_ARKHAN2);
                                CB->animation("sar_10");
                                lieu_action_register("L12D84", P_BANDOR, "SP07DIA55", "SP07ARR01_10");
                                lieu_action_register("L12D85A", P_KHORAD, "SP83DIA57", "SP83ARR01_10");
                                lieu_action_register("L12D85B", P_KHORAD, "SP83DIA53", "SP83ARR01_10");
                            }
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
                        if (ST(0x814) == 3) {
                            if (ST(0xC20) == 1 && ST(V_EVENT15_STEP) == 0) {
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L12D91A", P_ARKHAN2, "SG02DIA06", NULL);
                                    lieu_action_register("L12D91B", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L12D92", P_BANDOR, "SP07DIA58", "SP07ARR01_10");
                                    lieu_action_register("L12D93A", P_KHORAD, "SP83DIA56", "SP83ARR01_10");
                                    lieu_action_register("L12D93B", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
                                }
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
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L12D89A", P_ARKHAN2, "SG02DIA08", NULL);
                                    CB->animation("SP07DIA50_1000");
                                    lieu_action_register("L12D89B", P_ARKHAN2, "SG02DIA07", NULL);
                                    CB->animation("SP83ATT01_10");
                                    CB->animation("Sar_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->animation("P07ARR01_10");
                                    CB->change_camera(2, CAM_AUTO);
                                    CB->animation("P83ARR01_10");
                                    return;
                                }
                            }
                        }
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            ST(V_DLG_RANDOM) = CB->random(3);
                            CB->animation("SG00DIA08");
                            if (ST(V_DLG_RANDOM) == 0) {
                                CB->dialogue("LCOD22A", P_ARKHAN3);
                            }
                            if (ST(V_DLG_RANDOM) == 1) {
                                CB->dialogue("LCOD22B", P_ARKHAN3);
                            }
                            if (ST(V_DLG_RANDOM) == 2) {
                                CB->dialogue("LCOD22C", P_ARKHAN3);
                            }
                            CB->animation("sar_10");
                            CB->animation("SP07ATT01");
                            CB->animation("SP83DIA51");
                            if (ST(V_DLG_RANDOM) < 2) {
                                lieu_action_register("L12D17", P_KHORAD, "SP83DIA51", "SP83ATT02_10");
                            }
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
                        if (ST(0x814) == 1) {
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
                        if (ST(0x814) == 3) {
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
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L12D121", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L12D122", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    ST(V_DLG_RANDOM) = CB->random(2);
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L12D123", P_BANDOR, "SP07DIA53", "SP07ARR01_10");
                    }
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
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_BANDOR)) == VIE_DEAD) {
        if (ST(0x4C0) == 0) {
            CB->fouille(P_BANDOR);
        }
        if (ST(0x4C0) == 1 && ST(0x5A8) == 0) {
            CB->animation("SG00PRE08");
            CB->etat_objet("OB12", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            ST(0x5A8) = 1;
            return;
        }
    }
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        if (ST(0x818) == 0) {
            CB->dialogue("L12D105A", P_BANDOR);
            CB->dialogue("L12D105B", P_BANDOR);
            ST(0x818) = 1;
            return;
        }
        if (ST(0x818) == 1) {
            CB->dialogue("L12D107", P_BANDOR);
            ST(V_ENGAGED_PERSONA) = 0x30F;
            CB->combat(P_BANDOR, 0);
            CB->combat(P_KHORAD, 0);
            return;
        }
    }
}

/* trigger 03 - sub_1000601A - Khorad: talk, loot */
static void trig03(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_KHORAD) && ST(ST_VIE(P_KHORAD)) != VIE_DEAD && ST(0xC1C) == 1) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D40", P_ARKHAN2, "SG02DIA02", NULL);
            lieu_action_register("L12D41", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D42", P_ARKHAN3, "SG03DIA02", NULL);
            lieu_action_register("L12D41", P_KHORAD, "SP83DIA59", "SP83ARR01_10");
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
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
}

/* trigger 10 - sub_100062E4 - take OB70 (Master Builder diagram) */
static void trig10(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x37C6)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(0x690) == 0) {
            if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
                CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
                CB->animation("SP83ARR01_250");
                CB->animation("P83ARR01");
                CB->joindre(P_KHORAD, 0, 0, 0, ST(ST_HERO));
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
            if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02PRE06");
                    CB->etat_objet("OB70", -1, -1, -1, -1, -1, -1, -3, 4);
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x690) = 1;
                    return;
                }
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
        if (ST(0x690) == 1) {
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02PRE02");
                CB->dialogue("L12D28", P_ARKHAN2);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
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

/* trigger 11 - sub_100066D4 - zone 29 interaction (ACT28/29 anim; Bandor scene when holding OB70) */
static void trig11(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(29)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_PLAYER_WEAPON) != OB_BANDOR_DIAG || (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD)) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ACT28");
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT28");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT28");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ACT29");
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT29");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT29");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        if (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
            if (ST(0xC1C) == 0) {
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
            if (ST(0xC1C) >= 1) {
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L12D45", P_ARKHAN2, "SG02DIA03", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
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

/* trigger 12 - sub_10006C32 - zone 22 interaction (same scene as trigger 11, other side) */
static void trig12(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(22)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_PLAYER_WEAPON) != OB_BANDOR_DIAG || (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD)) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ACT28");
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT28");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT28");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ACT29");
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT29");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT29");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        if (ST(ST_PLAYER_WEAPON) == OB_BANDOR_DIAG && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
            if (ST(0xC1C) == 0) {
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
            if (ST(0xC1C) >= 1) {
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L12D45", P_ARKHAN2, "SG02DIA04", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
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
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && !CB->direction_spec(0x16) && CB->direction_spec(0xCB20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D103", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D104", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 13 - sub_1000735E - look (0xAFE2), zone 27 sign */
static void trig13(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xAFE2) && !CB->direction_zone(13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D97", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D98", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(27)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG00ATT01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 - sub_10007545 - look at 0x7273, zone 27 sign */
static void trig14(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x7273) && !CB->direction_zone(13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D99", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD77", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(27)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG00ATT01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 - sub_1000772C - zone 16: look */
static void trig15(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(16)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D101", P_ARKHAN2, "SG02PRE03", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D102", P_ARKHAN3, "SG03PRE03", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 17 - sub_1000783B - climb animation (P02/P03ATT76/77), timer 34, music */
static void trig17(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x8412)) {
        CB->bloque(BLOQUE_ALL);
        if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x8282, 1, "ar", 1, 0);
                CB->animation("Sar_10");
                CB->animation("P02ATT77");
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x8282, 1, "ar", 1, 0);
                CB->animation("Sar_10");
                CB->animation("P03ATT77");
            }
        }
        if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x8282, 1, "ar", 1, 0);
                CB->animation("Sar_10");
                CB->animation("P02ATT76");
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x8282, 1, "ar", 1, 0);
                CB->animation("Sar_10");
                CB->animation("P03ATT76");
            }
        }
        CB->evenement(34, 700);
        if (ST(0xEFC) != 1) {
            CB->divers(DV_MUSIC, 1, 4);
        }
        if (ST(0xEFC) == 1) {
            CB->divers(DV_MUSIC, 2, 4);
        }
    }
}

/* trigger 18 - sub_10007B11 - look (0x526B, L12D103/104) */
static void trig18(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x526B)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D103", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D104", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 19 - sub_10007C15 - look (0x526B, LCOD07/08) */
static void trig19(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x526B)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 20 - sub_10007D19 - shrine at 0x851: Bandor / Khorad react, sets 0x4BC / 0x4C0 */
static void trig20(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x851)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(0x4BC) == 0) {
            if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD || ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
                CB->joindre(ST(ST_HERO), 7686, -1028, 0xFFDE, -1);
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02PRE04_500");
                    CB->animation("G02PRE04_500");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG03PRE04_500");
                    CB->animation("G03PRE04_500");
                }
                CB->divers(DV_LOOK_AT_PLAYER, P_KHORAD, 0);
                CB->change_camera(3, CAM_LOCKED);
                if (ST(0xC1C) != 1) {
                    CB->joindre(P_BANDOR, 0, 0, 0, P_KHORAD);
                }
                if (ST(0xC1C) == 1) {
                    CB->joindre(P_BANDOR, 7294, -1026, 0x6633, -1);
                }
                CB->joindre(P_KHORAD, 0, 0, 0, P_BANDOR);
                lieu_action_register("L12D119", P_KHORAD, "SP83DIA54", "SP83ARR01_10");
                lieu_action_register("L12D120", P_BANDOR, "SP07DIA55", "SP07ARR01_10");
                CB->change_camera(4, CAM_LOCKED);
                CB->change_camera(4, CAM_AUTO);
                CB->divers_empile(DV_LOOK_AT_PLAYER, P_KHORAD, 1);
                if ((ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) != 0) && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    CB->animation("Sar_10");
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("P02ATT30");
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("P03ATT30");
                    }
                    ST(0x4BC) = 1;
                    ST(0x4C0) = 1;
                }
                if (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    CB->animation("Sar_10");
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT31");
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT31");
                    }
                    ST(0x4BC) = 1;
                    ST(0x4C0) = 1;
                }
                CB->animation("ar_10");
                CB->change_camera(3, CAM_LOCKED);
                CB->play_anim(0, 0);
                CB->bruitage("L12B01", -1, -1);
                CB->evenement(1, 500);
                return;
            }
            if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
                CB->joindre(ST(ST_HERO), 7686, -1028, 0xFFDE, -1);
                if ((ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) != 0) && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT30");
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT30");
                    }
                }
                if (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT31");
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT31");
                    }
                }
                ST(0x4BC) = 1;
                ST(0x4C0) = 1;
                CB->animation("ar");
                CB->change_camera(3, CAM_LOCKED);
                CB->play_anim(0, 0);
                CB->bruitage("L12B01", -1, -1);
                CB->evenement(1, 500);
                return;
            }
        }
        if (ST(0x4C0) == 1) {
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00ATT01");
                CB->dialogue("L12D117", P_ARKHAN2);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
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

/* trigger 21 - sub_100082CB - look (0xB7E7, LCOD07/08) */
static void trig21(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xB7E7)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 23 - sub_100083DA - chest at 0x13CE: OB21B / OB12 chain (also called from trigger 2) */
static void trig23(void) {
    if (!(ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x13CE)))
        return;
    ST(ST_SCRIPT_BUSY) = 1;
    CB->bloque(BLOQUE_ALL);
    /* 0x8437 - Bandor and Khorad both alive */
    if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        if (ST(0x4C0) == 1) {
            if (ST(0x184) == 0) {
                CB->animation("SG00PRE06");
                CB->etat_objet("OB21B", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x184) = 1;
                return;
            }
            if (ST(0x184) == 1) {
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD07", P_ARKHAN2);
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD08", P_ARKHAN3);
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        if (ST(0x4C0) == 0) {
            CB->animation("SG00PRE02");
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD07", P_ARKHAN2);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD08", P_ARKHAN3);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x85E9 - Bandor and Khorad both dead */
    if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD && ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
        if (ST(0x4C0) == 1) {
            if (ST(0xEDC) == 0) {
                if (ST(0x5A8) == 0) {
                    CB->animation("SG00PRE06");
                    CB->etat_objet("OB12", -1, -1, -1, -1, -1, -1, -3, 0);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x5A8) = 1;
                    return;
                }
                if (ST(0x5A8) == 1) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD07", P_ARKHAN2);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD08", P_ARKHAN3);
                    }
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            if (ST(0xEDC) == 1) {
                if (ST(0x184) == 0) {
                    CB->animation("SG00PRE06");
                    CB->etat_objet("OB21B", -1, -1, -1, -1, -1, -1, -3, 0);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x184) = 1;
                    return;
                }
                if (ST(0x184) == 1) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD07", P_ARKHAN2);
                    }
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
        if (ST(0x4C0) == 0) {
            CB->animation("SG00PRE02");
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD07", P_ARKHAN2);
            }
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

/* trigger 24 - sub_100088B6 - look (0xFADA, LCOD39/40) */
static void trig24(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xFADA)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE04");
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", P_ARKHAN2);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", P_ARKHAN3);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 25 - sub_10008990 - look (0x7683, LCOD39/40) */
static void trig25(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x7683)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE02");
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", P_ARKHAN2);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", P_ARKHAN3);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 26 - sub_10008A6A - look (0xCCA7, L12D103/104) */
static void trig26(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xCCA7)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L12D103", P_ARKHAN2, "SG02PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L12D104", P_ARKHAN3, "SG03PRE02", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 28 - sub_10008B88 - zone 13: look (LCOD29/30) */
static void trig28(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG00ATT01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 30 - sub_10008C6E - zone 31: ladder animation (ACT11/12/85/86) */
static void trig30(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(31)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT11");
                } else {
                    CB->animation("SP02ACT12");
                }
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT86");
                } else {
                    CB->animation("SP02ACT85");
                }
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT11");
                } else {
                    CB->animation("SP03ACT12");
                }
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
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

/* trigger 41 - sub_10001746 - start-up scene: Bandor and Khorad talk (variant by 0xC18) */
static void trig41(void) {
    if (ST(0xC18) == 2 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 1);
        CB->divers(DV_LOOK_AT_PLAYER, P_KHORAD, 1);
        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
        CB->joindre(P_KHORAD, 0, 0, 0, ST(ST_HERO));
        ST(0x64) = CB->random(3);
        if (ST(0x64) == 0) {
            lieu_action_register("L12D03", P_BANDOR, "SP07DIA63", "SP07ARR01_10");
        }
        if (ST(0x64) == 1) {
            lieu_action_register("L12D04", P_BANDOR, "SP07DIA63", "SP07ARR01_10");
        }
        if (ST(0x64) == 2) {
            lieu_action_register("L12D05", P_BANDOR, "SP07DIA63", "SP07ARR01_10");
        }
        CB->bloque(BLOQUE_NONE);
    }
    if (ST(0xC18) == 1) {
        CB->bloque(BLOQUE_ALL);
        CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 1);
        CB->divers(DV_LOOK_AT_PLAYER, P_KHORAD, 1);
        CB->joindre(P_BANDOR, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L12D01", P_BANDOR, "SP07ATT50", "SP07ARR01_10");
        lieu_action_register("L12D02", P_KHORAD, "SP83DIA51", "SP83ARR01_10");
        CB->bloque(BLOQUE_NONE);
        ST(0xC18) = 2;
    }
    if (ST(0x690) == 0) {
        CB->etat_objet("OB70", 7115, 1305, -1134, 0xF600, 0x4C00, 0x3200, -1, 4);
    }
    if (ST(0xEFC) != 1) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    if (ST(0xEFC) == 1) {
        CB->divers(DV_MUSIC, 2, 1);
    }
}

