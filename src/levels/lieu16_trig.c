/*
 * lieu16_trig.c — Native hand-port of the LIEU16.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu16.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu16_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table (and the triggers other triggers call).
 * ---------------------------------------------------------------------- */
static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig10(void);
static void trig12(void); static void trig14(void); static void trig15(void);
static void trig16(void); static void trig17(void); static void trig18(void);
static void trig19(void); static void trig21(void); static void trig22(void);
static void trig23(void); static void trig24(void); static void trig26(void);
static void trig28(void); static void trig29(void); static void trig30(void);
static void trig31(void); static void trig32(void); static void trig41(void);
static void trig_noop(void);   /* the 17 stub slots */

/* DLL buffers dword_10024AB0 / dword_100248B0 (sprintf targets). */
static c8 g_txt[256];
static c8 g_txt2[256];

/* dword_10024CB0[42] — filled at INIT (before the reload-flag test) with the
 * 5-byte jmp thunks; static here. A slot is "stub" when it is the empty
 * handler shared with LIEU01.
 *   idx handler        idx handler        idx handler
 *    0  100073F0         14  1000EECC         28  1000FF58
 *    1  1000AB16 stub    15  1000EFA2         29  10010036
 *    2  1000AB21         16  1000F15E         30  1001013D
 *    3  1000B08B         17  1000F230         31  1001046C
 *    4  1000D848         18  1000F413         32  100105AB
 *    5  1000E818         19  1000F4D6         33  1001069F stub
 *    6  1000EA04         20  1000F639 stub    34  100106AA stub
 *    7  1000EA83 stub    21  1000F644         35  100106B5 stub
 *    8  1000EA8E stub    22  1000F74B         36  100106C0 stub
 *    9  1000EA99 stub    23  1000FA90         37  100106CB stub
 *   10  1000EAA4         24  1000FB1B         38  100106D6 stub
 *   11  1000ECD3 stub    25  1000FD42 stub    39  100106E1 stub
 *   12  1000ECDE         26  1000FD4D         40  100106EC stub
 *   13  1000EEC1 stub    27  1000FF4D stub    41  10007EE8
 */
static void (*const g_trigger[42])(void) = {
    l16_trig00, trig_noop, trig02, trig03,
    trig04, trig05, trig06, trig_noop,
    trig_noop, trig_noop, trig10, trig_noop,
    trig12, trig_noop, trig14, trig15,
    trig16, trig17, trig18, trig19,
    trig_noop, trig21, trig22, trig23,
    trig24, trig_noop, trig26, trig_noop,
    trig28, trig29, trig30, trig31,
    trig32, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10024CB0]). */
void l16_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start points, state reset, zones, objects, actor placement */
void l16_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(16)) == 1) {
        ST(0xEE4) = 0;
        ST(ST_LEVEL) = 19;
        ST(ST_HERO) = P_ARKHAN3;
    }
    /* 0x743E */
    if (ST(ST_START_POINT(16)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_DELIA_PHASE) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_LEVEL) = 19;
    }
    ST(ST_START_POINT(16)) = 0;
    /* 0x7498 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x74BC */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 0);
    }
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
    ST(0x98) = 0;
    ST(0xB8) = 0;
    ST(0xBC) = 0;
    ST(0xD4) = 0;
    ST(0xD8) = 0;
    ST(0xDC) = 0;
    ST(0xE0) = 0;
    ST(0xF0) = 0;
    ST(0xF4) = 0;
    ST(0xF8) = 0;
    ST(0xFC) = 0;
    ST(0x100) = 0;
    ST(0x114) = 0;
    ST(V_WEAPON_WARNED) = 0;
    ST(0x4C) = 0;
    ST(0x50) = 0;
    ST(0x54) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(0x118) = 0;
    CB->ajoute_zone(4, 3559, 3560, 125, 105, 1);
    CB->ajoute_zone(5, 3703, 3559, 155, 111, 1);
    CB->ajoute_zone(10, 3637, 3422, 271, 140, 0);
    CB->ajoute_zone(11, 3665, 3355, 50, 50, 0);
    CB->ajoute_zone(12, 4079, 3427, 555, 260, 0);
    CB->ajoute_zone(13, 4059, 3411, 50, 50, 0);
    CB->ajoute_zone(14, 3728, 3744, 20, 285, 1);
    CB->ajoute_zone(15, 3942, 3618, 115, 102, 1);
    CB->ajoute_zone(16, 3808, 3747, 70, 170, 1);
    CB->ajoute_zone(17, 3084, 3311, 235, 240, 0);
    CB->ajoute_zone(18, 3308, 3125, 140, 65, 1);
    CB->ajoute_zone(19, 2689, 3641, 65, 70, 0);
    CB->ajoute_zone(20, 3002, 3294, 50, 50, 0);
    CB->ajoute_zone(21, 2789, 3893, 135, 160, 0);
    CB->ajoute_zone(22, 3544, 3755, 188, 141, 0);
    CB->ajoute_zone(23, 2889, 3679, 110, 100, 0);
    CB->ajoute_zone(24, 3642, 3282, 275, 131, 0);
    CB->ajoute_zone(25, 2789, 3893, 50, 50, 0);
    CB->ajoute_zone(26, 3230, 3517, 305, 125, 0);
    CB->ajoute_zone(27, 2936, 3546, 75, 90, 0);
    CB->ajoute_zone(28, 3214, 3768, 365, 92, 0);
    CB->ajoute_zone(30, 3491, 3717, 110, 90, 0);
    CB->ajoute_zone(31, 3233, 3739, 180, 240, 0);
    CB->ajoute_zone(32, 2544, 3616, 110, 130, 0);
    CB->ajoute_zone(33, 3797, 3469, 25, 25, 0);
    /* 0x7999 */
    if (ST(0xBF0) == 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->retire_zone(18);
    }
    /* 0x79C8 */
    if (ST(0x788) == 0) {
        ST(0x148) = 3;
        CB->etat_objet("OB24C", 3576, -2210, 3360, 0xF800, 0xE400, 0, -1, 4);
    }
    /* 0x7A13 */
    if (ST(0x588) == 0) {
        CB->etat_objet("OB04", 3585, -2224, 3340, 0xF200, 0x5800, 0x3C00, -1, 4);
    }
    /* 0x7A52 */
    if (ST(0x534) == 0) {
        CB->etat_objet("OB44B", 3588, -2187, 3827, 0, 0x2200, 0x3200, -1, 4);
    }
    /* 0x7A8E */
    if (ST(0x67C) == 0) {
        CB->etat_objet("OB65", 3586, -2177, 3790, 0, 0xB200, 0xBC00, -1, 4);
    }
    CB->place_animal(1, 3350, -2199, 3371, 351471, 2048);
    /* 0x7AEB */
    if ((ST(ST_LEVEL) == 19 && ST(0xBF0) != 0) || ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->pos_perso(ST(ST_HERO), 3310, 3264, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        CB->vire_col(21, 0);
    }
    /* 0x7B6D */
    if (ST(0xBF0) == 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->vire_col(21, 0);
        CB->pos_perso(ST(ST_HERO), 3333, 3069, 0x2E1, 1, "ar", 1, 0);
        CB->pos_perso(P_MAOR, 3529, 3745, 0x7F4E, 1, "P17ARR01", 2, 1);
        CB->pos_perso(P_DELIA, 3220, 3761, 0x7CD4, 1, "P10ARR01", 3, 0);
    }
    /* 0x7C13 */
    if (ST(0x4D4) == 1 && ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_LEVEL) == 19) {
        CB->pos_perso(P_MAOR, 3502, 3472, 0x5D3A, 1, "P17ARR01", 2, 1);
    }
    /* 0x7C6B */
    if (ST(0x528) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(ST_LEVEL) == 19 && ST(ST_VIE(90)) != VIE_DEAD) {
        CB->pos_perso(90, 3356, 3323, 0x2994, 1, "P90GAR01", 6, 1);
    }
    /* 0x7CD5 */
    if (ST(0x770) >= 1 && ST(0x770) != 99) {
        ST(V_DELIA_PHASE) = 20;
        CB->pos_perso(P_DELIA, 3400, 3328, 0x4CAE, 2, "P10cad", 3, 0);
    }
    /* 0x7D2D */
    if (ST(0x770) == 2 || ST(ST_VIE(90)) == VIE_DEAD) {
        CB->pos_perso(90, 0, 0, 0, 2, "P90cad", 6, 0);
    }
    /* 0x7D6D */
    if (ST(0xBF0) >= 1 && ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(0x4D4) == 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->vire_col(21, 0);
        CB->pos_perso(P_MAOR, 3529, 3745, 0x7F4E, 1, "P17ARR01", 2, 1);
    }
    /* 0x7DE7 */
    if (ST(ST_VIE(P_MAOR)) == VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->pos_perso(P_MAOR, 0, 0, 0, 2, "P17cad", 2, 0);
    }
    /* 0x7E27 */
    if (ST(ST_VIE(P_DELIA)) == VIE_DEAD && ST(V_DELIA_KILLED) == 16) {
        CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10cad", 3, 0);
    }
    /* 0x7E67 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->vire_col(21, 0);
        CB->retire_zone(4);
        CB->retire_zone(14);
        CB->retire_zone(15);
        CB->retire_zone(16);
        CB->pos_perso(ST(ST_HERO), 3310, 3264, 0xF53C, 1, "ar", 1, 0);
    }
    CB->bruitage("L16AMB1", -1, -1);
}

/* trigger 2 — sub_1000AB21 — zone 2 dispatch: zones 16/18, Maor intro, talk and strike */
static void trig02(void) {
    /* 0xAB41 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 16)) {
        trig16();
        return;
    }
    /* 0xAB6B */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 18)) {
        trig18();
        return;
    }
    /* 0xAB87 */
    if (ST(0x100) == 0 && ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) == 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->select_pile(1, 2);
        CB->bloque(BLOQUE_ALL);
        CB->evenement(1, -1);
        ST(V_EVENT_1) = 0;
        ST(0x100) = 1;
        ST(0x74) = 1;
        ST(0x84) = 1;
        CB->stop_joindre(-P_MAOR);
        CB->animation("SP17ARR01_10");
        CB->joindre(P_MAOR, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L16D127", P_MAOR, "SP17DIA50", "SP17ARR01");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xAC81 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        /* 0xACC3 */
        if (ST(ST_VIE(P_MAOR)) == VIE_DEAD) {
            CB->fouille(P_MAOR);
        }
        /* 0xACE0 */
        if (ST(ST_VIE(P_MAOR)) != VIE_DEAD) {
            /* 0xACF2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->select_pile(1, 2);
                CB->bloque(BLOQUE_ALL);
                CB->evenement(1, -1);
                ST(V_EVENT_1) = 0;
                CB->animation("SP17ARR01_10");
                CB->stop_joindre(-P_MAOR);
                ST(0x84) = 1;
                CB->joindre(P_MAOR, 0, 0, 0, 2);
                /* 0xAD76 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L16D206", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0xAD9E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L16D207", P_ARKHAN2, "SG00DIA05", NULL);
                }
                lieu_action_register("L16D208", P_MAOR, "SP17DIA59", "SP17ARR01_10");
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xAE08 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->bloque(BLOQUE_ALL);
                CB->stop_joindre(-P_MAOR);
                CB->joindre(P_ARKHAN3, 0, 0, 0, 17);
                /* 0xAE43 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L16D209", P_ARKHAN3, "SG00DIA05", NULL);
                }
                /* 0xAE6B */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("LCOD22A", P_ARKHAN3, "SG00DIA05", NULL);
                }
                lieu_action_register("L16D210", P_MAOR, "SP17DIA59", "SP17ARR01_10");
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xAED2 */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MAOR)) != VIE_DEAD) {
        CB->select_pile(1, 2);
        ST(0x4D4) = 1;
        ST(V_ENGAGED_PERSONA) = P_MAOR;
        CB->combat(P_MAOR, 0);
        CB->select_pile(0, 1);
    }
    /* 0xAF56 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_HERO) != 4) {
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAFC5 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xAFD7 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0xAFF9 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD61", P_ARKHAN2);
            }
        }
        /* 0xB01B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xB02D */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
            /* 0xB04F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD63", P_ARKHAN3);
            }
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        return;
    }
}

/* trigger 3 — sub_1000B08B — Delia: conversation, strike, cry, search */
static void trig03(void) {
    /* 0xB09A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_DELIA)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) == 2 && CB->direction_perso(P_DELIA)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(ST(ST_HERO), 0, 0, 0, 10);
        CB->joindre(P_DELIA, 0, 0, 0, ST(ST_HERO));
        /* 0xB13F */
        if (ST(V_DELIA_PHASE) >= 1 && ST(V_DELIA_PHASE) != 10) {
            /* 0xB163 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xB175 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(0x118))) {
                    /* 0xB1CD */
                    if (ST(0x510) == 0) {
                        /* 0xB1DF */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L16D500", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D501", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                            lieu_action_register("L16D502", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D503", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                        }
                        /* 0xB24F */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L16D504A", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D504B", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D505", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                            lieu_action_register("L16D506", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D503", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(V_DELIA_PHASE) = 2;
                        ST(0x510) = 1;
                        return;
                    }
                    /* 0xB31C */
                    if (ST(0x510) == 1) {
                        /* 0xB32E */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L16D507", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D508", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                        }
                        /* 0xB36F */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L16D509", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D508", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                            /* 0xB3B0 */
                            if (ST(0x114) == 0) {
                                lieu_action_register("L16D510", P_ARKHAN2, "SG00DIA05", NULL);
                                ST(0x114) = 1;
                            }
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xB410 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    ST(0x118) = ST(ST_PLAYER_WEAPON);
                    /* 0xB450 */
                    if (ST(ST_PLAYER_WEAPON) == 56) {
                        lieu_action_register("L16D888", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        lieu_action_register("L16D889", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                        CB->animation("SG00PRE82");
                        CB->animation("ar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        CB->dialogue("L16D700", P_DELIA);
                        CB->animation("SG00PRE82");
                        CB->animation("ar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
            /* 0xB52A */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(0x510) == 0) {
                /* 0xB54E */
                if (ST(0x90) == 0) {
                    /* 0xB560 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L16D402", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    /* 0xB588 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("LCOD22B", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L16D409", P_DELIA, "SP10DIA59", "SP10ARR01_10");
                    ST(0x90) = 1;
                }
                /* 0xB5D8 */
                if (ST(0x90) == 1) {
                    /* 0xB5EA */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L16D407", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    /* 0xB612 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("LCOD22C", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L16D410", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                    ST(V_DELIA_HITS) = 1;
                    /* 0xB662 */
                    if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
                        CB->dialogue("L16D999", -1);
                    }
                    ST(0x90) = 0;
                }
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB6CE */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(0x510) != 0) {
                lieu_action_register("L16D511", P_ARKHAN3, "SG00DIA02", NULL);
                lieu_action_register("L16D512", P_DELIA, "SP10DIA59", "SP10ARR01_10");
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xB74A */
        if (ST(V_DELIA_PHASE) == 0) {
            /* 0xB75C */
            if (ST(ST_LOCKED_CAMERA) == 5) {
                CB->change_camera(4, CAM_LOCKED);
            }
            ST(0x4C) = 1;
            /* 0xB78A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xB79C */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(0x118))) {
                    /* 0xB7F4 */
                    if (ST(0x544) == 0) {
                        /* 0xB806 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L16D513A", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D513B", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D514A", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                            lieu_action_register("L16D514B", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                            lieu_action_register("L16D515C", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                        /* 0xB88C */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L16D516A", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D516B", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D514A", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                            lieu_action_register("L16D514B", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                            lieu_action_register("L16D517", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                        ST(0x544) = 1;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xB94A */
                    if (ST(0x544) == 1) {
                        /* 0xB95C */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L16D518", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                        /* 0xB984 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L16D519", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                        lieu_action_register("L16D520", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xB9EE */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    ST(0x118) = ST(ST_PLAYER_WEAPON);
                    /* 0xBA2E */
                    if (ST(ST_PLAYER_WEAPON) == 56) {
                        lieu_action_register("L16D888", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        lieu_action_register("L16D889", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                        CB->animation("SG00PRE82");
                        CB->animation("ar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        CB->dialogue("L16D700", P_DELIA);
                        CB->animation("SG00PRE82");
                        CB->animation("ar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
            /* 0xBB08 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xBB1A */
                if (ST(0x90) == 0) {
                    /* 0xBB2C */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L16D402", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    /* 0xBB54 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("LCOD22A", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L16D409", P_DELIA, "SP10DIA59", "SP10ARR01_10");
                    ST(0x90) = 1;
                    ST(V_DELIA_HITS) = 1;
                    /* 0xBBB3 */
                    if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
                        CB->dialogue("L16D999", -1);
                    }
                }
                /* 0xBBE7 */
                if (ST(0x90) == 1) {
                    /* 0xBBF9 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L16D407", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    /* 0xBC21 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("LCOD22D", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L16D410", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                    ST(0x90) = 0;
                    ST(V_DELIA_HITS) = 1;
                    /* 0xBC80 */
                    if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
                        CB->dialogue("L16D999", -1);
                    }
                }
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xBCDD */
        if (ST(V_DELIA_PHASE) == 10) {
            /* 0xBCEF */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L16D521", P_ARKHAN2, "SG00DIA05", NULL);
                lieu_action_register("L16D522", P_DELIA, "SP10DIA51", "SP10ARR01_10");
            }
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xBD56 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_DELIA)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_DELIA)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(ST(ST_HERO), 0, 0, 0, 10);
        CB->joindre(P_DELIA, 0, 0, 0, ST(ST_HERO));
        /* 0xBDF8 */
        if (ST(ST_LOCKED_CAMERA) == 5) {
            CB->change_camera(4, CAM_LOCKED);
        }
        ST(0x4C) = 1;
        /* 0xBE26 */
        if (ST(ST_VIE(P_MAOR)) != VIE_DEAD) {
            /* 0xBE38 */
            if (ST(0xEE4) == 0) {
                /* 0xBE4A */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xBE5C */
                    if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(0x118))) {
                        /* 0xBEB4 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            /* 0xBEC6 */
                            if (ST(0xAE8) == 0) {
                                lieu_action_register("L16D100", P_ARKHAN2, "SG00DIA02", NULL);
                                lieu_action_register("L16D101", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                                ST(0xAE8) = 1;
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xBF3F */
                            if (ST(0xAE8) == 1) {
                                lieu_action_register("L16D102A", P_ARKHAN2, "SG00DIA04", NULL);
                                lieu_action_register("L16D102B", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L16D102C", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L16D103", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                                ST(0xAE8) = 2;
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xBFE4 */
                            if (ST(0xAE8) == 2) {
                                lieu_action_register("L16D104", P_ARKHAN2, "SG00DIA02", NULL);
                                lieu_action_register("L16D105A", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                                lieu_action_register("L16D105B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                                lieu_action_register("L16D106", P_ARKHAN2, "SG00DIA06", NULL);
                                lieu_action_register("L16D107", P_DELIA, "SP10DIA53", "SP10ARR01_10");
                                ST(0xAE8) = 3;
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xC0A5 */
                            if (ST(0xAE8) == 3) {
                                /* 0xC0B7 */
                                if (ST(0xBC8) != 2) {
                                    lieu_action_register("L16D108", P_ARKHAN2, "SG00DIA05", NULL);
                                    lieu_action_register("L16D109A", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                                    lieu_action_register("L16D109B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                                    lieu_action_register("L16D110", P_ARKHAN2, "SG00DIA05", NULL);
                                    lieu_action_register("L16D111", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                                }
                                /* 0xC140 */
                                if (ST(0xBC8) == 2) {
                                    lieu_action_register("L16D112", P_ARKHAN2, "SG00DIA05", NULL);
                                    lieu_action_register("L16D113", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                                }
                                ST(0xAE8) = 4;
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xC1B9 */
                            if (ST(0xAE8) == 4) {
                                /* 0xC1CB */
                                if (ST(0xBC8) != 2) {
                                    ST(0xEE4) = 1;
                                    lieu_action_register("L16D114", P_ARKHAN2, "SG00DIA05", NULL);
                                    lieu_action_register("L16D115A", P_DELIA, "SP10DIA53", "SP10ARR01_10");
                                    lieu_action_register("L16D115B", P_DELIA, "SP10DIA53", "SP10ARR01_10");
                                    ST(0xAE8) = 5;
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                                /* 0xC26C */
                                if (ST(0xBC8) == 2) {
                                    lieu_action_register("L16D116", P_ARKHAN2, "SG00DIA05", NULL);
                                    lieu_action_register("L16D117", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            }
                        }
                        /* 0xC2D6 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            /* 0xC2E8 */
                            if (ST(0xAE8) == 0) {
                                ST(0xAE8) = 1;
                                lieu_action_register("L16D118", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L16D119", P_DELIA, "SP10DIA51", "SP10GAR01");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xC361 */
                            if (ST(0xAE8) >= 1) {
                                lieu_action_register("L16D120A", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L16D120B", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L16D121", P_DELIA, "SP10DIA51", "SP10GAR01");
                                /* 0xC3B8 */
                                if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
                                    CB->dialogue("L16D999", -1);
                                }
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                ST(V_DELIA_HITS) = 1;
                                ST(0x508) = 1;
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                        }
                    }
                    /* 0xC433 */
                    if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(0x118) = ST(ST_PLAYER_WEAPON);
                        /* 0xC473 */
                        if (ST(ST_PLAYER_WEAPON) == 56) {
                            lieu_action_register("L16D888", P_ARKHAN2, "SG00PRE80", "SG00PRE82");
                            lieu_action_register("L16D889", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                            CB->animation("SG00PRE82");
                            CB->animation("ar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        } else {
                            /* 0xC4F0 */
                            if (ST(0x98) == 0) {
                                lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                                lieu_action_register("L16D886A", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                                lieu_action_register("L16D886B", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                                CB->animation("SG00PRE82");
                                CB->animation("ar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x98) = 1;
                                return;
                            }
                            /* 0xC590 */
                            if (ST(0x98) == 1) {
                                lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                                lieu_action_register("L16D887", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                                CB->animation("SG00PRE82");
                                CB->animation("ar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                        }
                    }
                }
                /* 0xC608 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xC61A */
                    if (ST(0x90) == 0) {
                        /* 0xC62C */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L16D402", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                        /* 0xC654 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("LCOD22A", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                        lieu_action_register("L16D409", P_DELIA, "SP10DIA59", "SP10ARR01_10");
                        ST(0x68) = 1;
                        ST(V_DELIA_HITS) = 1;
                        /* 0xC6B0 */
                        if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
                            CB->dialogue("L16D999", -1);
                        }
                    }
                    /* 0xC6E4 */
                    if (ST(0x90) == 1) {
                        /* 0xC6F6 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L16D407", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                        /* 0xC71E */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("LCOD22B", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                        lieu_action_register("L16D410", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                        ST(0x90) = 0;
                        ST(V_DELIA_HITS) = 1;
                        /* 0xC77D */
                        if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
                            CB->dialogue("L16D999", -1);
                        }
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xC7DA */
            if (ST(0xEE4) != 0 && ST(ST_HERO) == P_ARKHAN2) {
                /* 0xC7FE */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L16D66", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L16D67", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                    /* 0xC83F */
                    if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
                        CB->dialogue("L16D999", -1);
                        ST(V_DELIA_HITS) = 1;
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xC8A0 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xC8B2 */
                    if (ST(V_DELIA_HITS) == 1) {
                        ST(V_DELIA_HITS) = 0;
                        ST(0x508) = 0;
                    }
                    /* 0xC8E2 */
                    if ((ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2)) && ST(0xEE4) == 1 && ST(0xAE8) == 5) {
                        lieu_action_register("L16D68", P_ARKHAN2, "SG00DIA04", NULL);
                        lieu_action_register("L16D69", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xC987 */
                    if (ST(ST_PLAYER_WEAPON) > 0 && ST(ST_PLAYER_WEAPON) != 20 && ST(ST_PLAYER_WEAPON) != 56 && ST(0xEE4) == 1 && ST(0xAE8) == 5) {
                        lieu_action_register("L16D70", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        lieu_action_register("L16D71", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                        CB->animation("SG00PRE82");
                        CB->animation("ar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xCA47 */
                    if (ST(ST_PLAYER_WEAPON) == 56 && ST(0xEE4) >= 1 && ST(0xAE8) >= 5) {
                        lieu_action_register("L16D888", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        lieu_action_register("L16D889", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                        CB->animation("SG00PRE82");
                        CB->animation("ar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xCAE3 */
                    if (ST(ST_PLAYER_WEAPON) == 20 && ST(0xEE4) == 1 && ST(0xAE8) == 5) {
                        ST(0xEE4) = 2;
                        ST(0xBC8) = 1;
                        CB->animation("SG00PRE80_1000");
                        lieu_action_register("L16D70", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L16D72", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                        lieu_pre_scene(P_ARKHAN2, P_DELIA, 20, 1, 3);
                        lieu_action_register("L16D74A", P_DELIA, "SP10DIA53", "SP10ARR01_10");
                        lieu_action_register("L16D74B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                        lieu_action_register("L16D74C", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                        lieu_action_register("L16D75", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                        lieu_action_register("L16D76", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L16D77", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xAE8) = 6;
                        return;
                    }
                    /* 0xCC51 */
                    if (ST(0xEE4) == 2 && ST(0xAE8) == 6 && ST(0xBC8) != 2) {
                        ST(0xEE4) = 3;
                        lieu_action_register("L16D78", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L16D79A", P_DELIA, "SP10DIA53", "SP10ARR01_10");
                        lieu_action_register("L16D79B", P_DELIA, "SP10DIA53", "SP10ARR01_10");
                        lieu_action_register("L16D80", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L16D81A", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                        lieu_action_register("L16D81B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                        lieu_action_register("L16D82", P_DELIA, "SP10DIA56", "SP10ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xAE8) = 7;
                        return;
                    }
                    /* 0xCD77 */
                    if ((ST(0xEE4) == 3 && ST(0xAE8) == 7 && ST(ST_PLAYER_WEAPON) == 0) || (ST(0xEE4) == 2 && ST(0xBC8) == 2)) {
                        lieu_action_register("L16D83", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L16D84", P_DELIA, "SP10DIA53", "SP10ARR01_10");
                        lieu_action_register("L16D85", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xCE42 */
                    if (ST(0xEE4) == 3 && ST(0xAE8) == 7 && ST(ST_PLAYER_WEAPON) > 0 && ST(ST_PLAYER_WEAPON) != 20 && ST(ST_PLAYER_WEAPON) != 56) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        lieu_action_register("L16D885A", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                        CB->animation("SG00PRE82");
                        CB->animation("ar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
        }
        /* 0xCF02 */
        if (ST(ST_VIE(P_MAOR)) == VIE_DEAD) {
            /* 0xCF14 */
            if (ST(0x540) == 0) {
                ST(0x540) = 1;
                /* 0xCF35 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L16D400", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0xCF6F */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L16D401", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0xCFA9 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L16D402", P_ARKHAN3, "SG00DIA02", NULL);
                }
                /* 0xCFE3 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("LCOD22C", P_ARKHAN3, "SG00DIA02", NULL);
                }
                lieu_action_register("L16D404A", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                lieu_action_register("L16D404B", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xD078 */
            if (ST(0x540) == 1) {
                ST(V_DLG_RANDOM) = CB->random(2);
                /* 0xD0A1 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L16D405", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0xD0DB */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L16D406", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0xD115 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L16D407", P_ARKHAN3, "SG00DIA02", NULL);
                }
                /* 0xD14F */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("LCOD22D", P_ARKHAN3, "SG00DIA02", NULL);
                }
                /* 0xD189 */
                if (ST(ST_HERO) != P_ARKHAN3 || ST(ST_SPECIAL_MODE) != 0) {
                    /* 0xD1AD */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L16D408", P_DELIA, "SP10DIA59", "SP10ARR01_10");
                    }
                    /* 0xD1D8 */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L16D409", P_DELIA, "SP10DIA59", "SP10ARR01_10");
                    }
                } else {
                    /* 0xD208 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L16D409", P_DELIA, "SP10DIA59", "SP10ARR01_10");
                    }
                    /* 0xD233 */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L16D410", P_DELIA, "SP10DIA51", "SP10ARR01_10");
                    }
                }
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xD284 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_MAOR && ST(ST_VIE(P_DELIA)) == 1 && CB->direction_perso(P_DELIA)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD2F0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xD302 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L16D513A", P_ARKHAN2, "SG00DIA05", NULL);
            }
            /* 0xD32A */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L16D516A", P_ARKHAN2, "SG00DIA05", NULL);
            }
        }
        /* 0xD352 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xD364 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L16D402", P_ARKHAN3, "SG00DIA02", NULL);
            }
            /* 0xD38C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD22A", P_ARKHAN3, "SG00DIA02", NULL);
            }
        }
        lieu_action_register("L16D408", P_DELIA, "SP10DIA59", "SP10ARR01_10");
        lieu_action_register("L16D404B", P_DELIA, "SP10DIA51", "SP10ARR01_10");
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xD40C */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (ST(V_ENGAGED_PERSONA) == 0 || ST(V_ENGAGED_PERSONA) == P_MAOR) && ST(ST_VIE(P_DELIA)) == 1 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_DELIA)) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->select_pile(1, 2);
        CB->stop_joindre(-P_DELIA);
        CB->bloque(BLOQUE_ALL);
        /* 0xD4B1 */
        if (ST(V_DELIA_HITS) == 1) {
            ST(V_DELIA_KILLED) = 16;
            ST(V_DELIA_PHASE) = 20;
            ST(ST_VIE(P_DELIA)) = VIE_DEAD;
            CB->animation("SP10IMP51");
            CB->animation("P10cad");
            CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10CAD", 3, 0);
            /* 0xD522 */
            if (ST(0x17C) == 1) {
                CB->vire_col(21, 0);
            }
            /* 0xD541 */
            if (ST(0x770) == 0) {
                ST(0x770) = 99;
            }
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
        /* 0xD57C */
        if (ST(V_DELIA_HITS) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            ST(V_DELIA_HITS) = 1;
            CB->animation("SP10ESA01");
            CB->animation("P10GAR01_50");
            CB->dialogue("L16D403", P_DELIA);
            CB->animation("P10GAR01");
            /* 0xD5DA */
            if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
                CB->dialogue("L16D999", -1);
            }
            CB->animation("SP10GAR01_10");
            CB->modify_var(&ST(0x6C), 1);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0xD655 */
    if (ST(V_PLAYER_STRIKE) == 2 && ST(ST_SCRIPT_BUSY) == 0 && (ST(V_ENGAGED_PERSONA) == 0 || ST(V_ENGAGED_PERSONA) == P_MAOR) && ST(ST_VIE(P_DELIA)) == 1 && CB->direction_perso(P_DELIA) && ST(ST_COMBAT_STANCE) == 1) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->combat(P_DELIA, 1);
        ST(V_ENGAGED_PERSONA) = P_DELIA;
        /* 0xD6F0 */
        if (ST(0x17C) == 1) {
            CB->vire_col(21, 0);
        }
        ST(0x70) = 1;
        ST(V_DELIA_HITS) = 1;
        CB->evenement(36, 2000);
    }
    /* 0xD737 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_DELIA)) == 1 && CB->direction_perso(P_DELIA)) {
        ST(V_CRI_USED) = 1;
        /* 0xD7BF */
        if (ST(0xF4) != 99) {
            ST(0xF4) = 10;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(33, 1000);
        return;
    }
    /* 0xD7FD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_DELIA)) == VIE_DEAD) {
        CB->fouille(P_DELIA);
        return;
    }
}

/* trigger 4 — sub_1000D848 — Maor: chase on zone 4 and the talk scene */
static void trig04(void) {
    /* 0xD85A */
    if (ST(0x84) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MAOR)) != VIE_DEAD && CB->divers(DV_IN_ZONE, ST(ST_HERO), 4) && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->select_pile(1, 2);
        CB->evenement(1, -1);
        ST(V_EVENT_1) = 0;
        ST(0x84) = 1;
        CB->animation("SP17ARR01_10");
        CB->stop_joindre(-P_MAOR);
        /* 0xD913 */
        if (ST(0xCF4) == 0) {
            CB->animation("SP17ARR01_10");
            CB->joindre(-P_MAOR, 3590, 3727, 0x7C93, -1);
        }
        /* 0xD94C */
        if (ST(0xCF4) == 2) {
            CB->animation("SP17ARR01_10");
            CB->joindre(-P_MAOR, 3590, 3727, 0x7C93, -1);
        }
        /* 0xD985 */
        if (ST(0xCF4) == 3) {
            CB->animation("SP17ARR01_10");
            CB->joindre(-P_MAOR, 3590, 3727, 0x7C93, -1);
        }
        CB->select_pile(0, 1);
        return;
    }
    /* 0xD9CD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MAOR)) != VIE_DEAD && CB->direction_spec(0xFBA5) && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(ST(ST_HERO), 0, 0, 0, 17);
        CB->joindre(P_MAOR, 0, 0, 0, ST(ST_HERO));
        ST(V_EVENT15_STEP) = 1;
        /* 0xDA90 */
        if (ST(ST_LOCKED_CAMERA) != 2) {
            CB->change_camera(2, CAM_LOCKED);
        }
        /* 0xDAB2 */
        if (ST(ST_VIE(P_MAOR)) != VIE_DEAD) {
            /* 0xDAC4 */
            if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON)) || CB->divers(DV_CAN_FIGHT, 0, 2)) {
                /* 0xDB19 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xDB2B */
                    if (ST(0xD00) == 0) {
                        /* 0xDB3D */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->dialogue("L16D300A", P_ARKHAN2);
                            CB->dialogue("L16D300B", P_ARKHAN2);
                            lieu_action_register("L16D301", P_MAOR, "SP17DIA56", "SP17ARR01_10");
                        }
                        /* 0xDB88 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L16D302A", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D302B", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L16D303A", P_MAOR, "SP17DIA60", "SP17ARR01_10");
                            lieu_action_register("L16D303B", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xD00) = 1;
                        return;
                    }
                    /* 0xDC30 */
                    if (ST(0xD00) == 1) {
                        /* 0xDC42 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L16D304A", P_ARKHAN2, "SG00DIA04", NULL);
                            lieu_action_register("L16D304B", P_ARKHAN2, "SG00DIA04", NULL);
                        }
                        /* 0xDC80 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L16D306A", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L16D306B", P_ARKHAN2, "SG00DIA07", NULL);
                        }
                        lieu_action_register("L16D305", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                        /* 0xDCD7 */
                        if (ST(0x788) == 0) {
                            lieu_action_register("L16D307A", P_MAOR, "SP17DIA53", "SP17ARR01_10");
                            lieu_action_register("L16D307B", P_MAOR, "SP17DIA53", "SP17ARR01_10");
                        }
                        /* 0xDD1B */
                        if (ST(0x788) != 0) {
                            lieu_action_register("L16D308A", P_MAOR, "SP17DIA53", "SP17ARR01_10");
                            lieu_action_register("L16D308B", P_MAOR, "SP17DIA53", "SP17ARR01_10");
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xD00) = 2;
                        return;
                    }
                    /* 0xDD97 */
                    if (ST(0xD00) == 2) {
                        /* 0xDDA9 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L16D309", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L16D310A", P_MAOR, "SP17DIA56", "SP17ARR01_10");
                            lieu_action_register("L16D310B", P_MAOR, "SP17DIA53", "SP17ARR01_10");
                            lieu_action_register("L16D310C", P_MAOR, "SP17DIA59", "SP17ARR01_10");
                        }
                        /* 0xDE1C */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L16D311A", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L16D311B", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L16D312A", P_MAOR, "SP17DIA59", "SP17ARR01_10");
                            lieu_action_register("L16D312B", P_MAOR, "SP17DIA57", "SP17ARR01_10");
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xD00) = 3;
                        return;
                    }
                    /* 0xDEC4 */
                    if (ST(0xD00) == 3) {
                        /* 0xDED6 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            /* 0xDEE8 */
                            if (ST(0xFC) == 0) {
                                lieu_action_register("L16D313", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L16D314", P_MAOR, "SP17DIA55", "SP17ARR01_10");
                                ST(0xFC) = 1;
                            } else {
                                lieu_action_register("L16D315", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L16D316A", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                                lieu_action_register("L16D316B", P_MAOR, "SP17DIA53", "SP17ARR01_10");
                                ST(0xFC) = 0;
                            }
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xDFBD */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L16D317A", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L16D317B", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L16D318A", P_MAOR, "SP17DIA59", "SP17ARR01_10");
                            lieu_action_register("L16D318B", P_MAOR, "SP17ATT50", "SP17ARR01_10");
                            CB->vire_col(7, 0);
                            ST(V_PLAYER_STRIKE) = -1;
                            ST(0x4D4) = 1;
                            CB->joindre(ST(ST_HERO), 3447, 3356, 0x30E1, -1);
                            CB->joindre(-P_MAOR, 3482, 3750, -1, -1);
                            CB->animation("P17ACT01");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->animation("P17ACT04");
                            CB->animation("P17ACT02");
                            CB->change_camera(0, CAM_LOCKED);
                            CB->change_camera(0, CAM_AUTO);
                            CB->animation("P17ARR01");
                            CB->bloque(BLOQUE_NONE);
                            CB->combat(P_MAOR, 0);
                            ST(V_ENGAGED_PERSONA) = P_MAOR;
                            CB->vire_col(7, 1);
                            return;
                        }
                    }
                }
                /* 0xE11D */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xE141 */
                    if (ST(0x518) == 0) {
                        lieu_action_register("L16D320", P_ARKHAN3, "SG00DIA07", NULL);
                        lieu_action_register("L16D321", P_MAOR, "SP17DIA57", "SP17ARR01_10");
                        lieu_action_register("L16D322", P_ARKHAN3, "SG00DIA07", NULL);
                        lieu_action_register("L16D323", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x518) = 1;
                        return;
                    }
                    /* 0xE1E9 */
                    if (ST(0x518) == 1) {
                        lieu_action_register("L16D324", P_ARKHAN3, "SG00DIA07", NULL);
                        lieu_action_register("L16D325A", P_MAOR, "SP17DIA59", "SP17ARR01_10");
                        /* 0xE22A */
                        if (ST(0x788) == 0) {
                            lieu_action_register("L16D326A", P_MAOR, "SP17DIA56", "SP17ARR01_10");
                            lieu_action_register("L16D326B", P_MAOR, "SP17DIA53", "SP17ARR01_10");
                        }
                        /* 0xE26E */
                        if (ST(0x788) != 0) {
                            lieu_action_register("L16D327A", P_MAOR, "SP17DIA56", "SP17ARR01_10");
                            lieu_action_register("L16D308B", P_MAOR, "SP17DIA53", "SP17ARR01_10");
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x518) = 2;
                        return;
                    }
                    /* 0xE2EA */
                    if (ST(0x518) == 2) {
                        /* 0xE2FC */
                        if (ST(0xFC) == 0) {
                            lieu_action_register("L16D328", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L16D329", P_MAOR, "SP17DIA55", "SP17ARR01_10");
                            ST(0xFC) = 1;
                        } else {
                            lieu_action_register("L16D330", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L16D331", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                            ST(0xFC) = 0;
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xE3B8 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                    /* 0xE3DC */
                    if (ST(0x518) == 0) {
                        lieu_action_register("L16D332", P_ARKHAN3, "SG00DIA07", NULL);
                        lieu_action_register("L16D333A", P_MAOR, "SP17ATT50", "SP17ARR01_10");
                        lieu_action_register("L16D333B", P_MAOR, "SP17ATT50", "SP17ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x518) = 1;
                        return;
                    }
                    /* 0xE46E */
                    if (ST(0x518) >= 1) {
                        lieu_action_register("L16D334", P_ARKHAN3, "SG00DIA07", NULL);
                        lieu_action_register("L16D318A", P_MAOR, "SP17DIA59", "SP17ARR01_10");
                        lieu_action_register("L16D318B", P_MAOR, "SP17ATT50", "SP17ARR01_10");
                        CB->vire_col(7, 0);
                        ST(V_PLAYER_STRIKE) = -1;
                        ST(0x4D4) = 1;
                        CB->joindre(ST(ST_HERO), 3447, 3356, 0x30E1, -1);
                        CB->joindre(-P_MAOR, 3482, 3750, -1, -1);
                        CB->animation("P17ACT01");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("P17ACT04");
                        CB->animation("P17ACT02");
                        CB->change_camera(0, CAM_LOCKED);
                        CB->change_camera(0, CAM_AUTO);
                        CB->animation("P17ARR01");
                        CB->bloque(BLOQUE_NONE);
                        CB->combat(P_MAOR, 0);
                        ST(V_ENGAGED_PERSONA) = P_MAOR;
                        CB->vire_col(7, 1);
                        return;
                    }
                }
            }
            /* 0xE5B8 */
            if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                /* 0xE5F5 */
                if (ST(ST_PLAYER_WEAPON) == 24) {
                    /* 0xE607 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L16D335", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L16D336", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                    }
                    /* 0xE648 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L16D337", P_ARKHAN3, "SG00DIA05", NULL);
                        lieu_action_register("L16D338", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xE6B2 */
                if (ST(ST_PLAYER_WEAPON) == 77) {
                    /* 0xE6C4 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L16D339", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L16D340", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                    }
                    /* 0xE705 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L16D341", P_ARKHAN3, "SG00DIA05", NULL);
                        lieu_action_register("L16D342", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xE774 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("LCOD18", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L16D343", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                }
                /* 0xE7B5 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("LCOD19", P_ARKHAN3, "SG00DIA05", NULL);
                    lieu_action_register("L16D346", P_MAOR, "SP17DIA58", "SP17ARR01_10");
                }
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 5 — sub_1000E818 — Maor chase on zone 5, talk, exit and search */
static void trig05(void) {
    /* 0xE82A */
    if ((ST(0x84) == 0 || ST(0x74) == 1) && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)) {
        CB->select_pile(1, 2);
        CB->stop_joindre(-P_MAOR);
        CB->evenement(1, -1);
        ST(V_EVENT_1) = 0;
        ST(0x84) = 1;
        ST(0x74) = 0;
        CB->animation("SP17ARR01_10");
        CB->stop_joindre(-P_MAOR);
        /* 0xE909 */
        if (ST(0xCF4) == 0) {
            CB->animation("SP17ARR01_10");
            CB->joindre(-P_MAOR, 3698, 3724, 0x756E, -1);
        }
        /* 0xE942 */
        if (ST(0xCF4) == 2) {
            CB->animation("SP17ARR01_10");
            CB->joindre(-P_MAOR, 3698, 3724, 0x756E, -1);
        }
        /* 0xE97B */
        if (ST(0xCF4) == 3) {
            CB->animation("SP17ARR01_10");
            CB->joindre(-P_MAOR, 3698, 3724, 0x756E, -1);
        }
        CB->select_pile(0, 1);
        return;
    }
    /* 0xE9C3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xFBA5)) {
        trig04();
    }
}

/* trigger 6 — sub_1000EA04 — search the Guardian 90 corpse */
static void trig06(void) {
    /* 0xEA24 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 18)) {
        trig18();
        return;
    }
    /* 0xEA3D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(90)) == VIE_DEAD) {
        CB->fouille(90);
    }
}

/* trigger 10 — sub_1000EAA4 — zone 11: OB24C chest */
static void trig10(void) {
    /* 0xEAB3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(11)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xEB1C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xEB2E */
            if (ST(0x788) == 0) {
                CB->animation("SG00PRE07");
                ST(0x148) = 3;
                CB->etat_objet("OB24C", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x788) = 1;
                return;
            }
            /* 0xEBA5 */
            if (ST(0x788) == 1) {
                CB->animation("SG00PRE03");
                CB->dialogue("LCOD79", P_ARKHAN2);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xEBFB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xEC0D */
            if (ST(0x788) == 0) {
                CB->animation("SG00PRE07");
                ST(0x148) = 3;
                CB->etat_objet("OB24C", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x788) = 1;
                return;
            }
            /* 0xEC84 */
            if (ST(0x788) == 1) {
                CB->animation("SG00PRE03");
                CB->dialogue("LCOD80", P_ARKHAN3);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 12 — sub_1000ECDE — zone 13: sign */
static void trig12(void) {
    /* 0xECED */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xED56 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xED68 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SP02ACT28");
                CB->animation("ar");
                CB->dialogue("L16D904", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xEDC9 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT29");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xEE0F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xEE21 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SP03ACT28");
                CB->animation("ar");
                CB->dialogue("L16D905", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xEE82 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT29");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 14 — sub_1000EECC — zone 14: Maor confrontation */
static void trig14(void) {
    /* 0xEEDE */
    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_COMBAT_STANCE) == 1 && ST(0x68) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MAOR)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(2, CAM_LOCKED);
        ST(0x68) = 1;
        CB->joindre(P_MAOR, 0, 0, 0, ST(ST_HERO));
        CB->dialogue("L16D04", P_MAOR);
        CB->animation("SP17ACT03");
        CB->animation("P17ACT01");
        CB->evenement(2, 1500);
    }
}

/* trigger 15 — sub_1000EFA2 — zone 15: talk to Maor or L16D07 scene */
static void trig15(void) {
    /* 0xEFB1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_MAOR) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
        trig02();
        return;
    }
    /* 0xF02D */
    if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x78) == 0) {
        CB->select_pile(1, 2);
        CB->bloque(BLOQUE_ALL);
        CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
        CB->evenement(1, -1);
        ST(V_EVENT_1) = 0;
        CB->animation("SP17ARR01_10");
        CB->stop_joindre(-P_MAOR);
        CB->joindre(ST(ST_HERO), 0, 0, 0, 17);
        CB->joindre(P_MAOR, 0, 0, 0, ST(ST_HERO));
        CB->dialogue("L16D07A", P_MAOR);
        CB->dialogue("L16D07B", P_MAOR);
        ST(0x78) = 1;
        CB->stop_joindre(ST(ST_HERO));
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 2);
    }
}

/* trigger 16 — sub_1000F15E — zone 16: Maor attacks (L16D08) */
static void trig16(void) {
    /* 0xF170 */
    if (ST(ST_VIE(P_MAOR)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x7C) == 0) {
        CB->select_pile(1, 1);
        CB->evenement(1, -1);
        ST(V_EVENT_1) = 0;
        CB->dialogue("L16D08", P_MAOR);
        ST(0x7C) = 1;
        ST(0x4D4) = 1;
        CB->combat(P_MAOR, 0);
        ST(V_ENGAGED_PERSONA) = P_MAOR;
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 2);
    }
}

/* trigger 17 — sub_1000F230 — zone 20: sign */
static void trig17(void) {
    /* 0xF23F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xF2A8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xF2BA */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SP02ACT28");
                CB->animation("ar");
                CB->dialogue("L16D904", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xF31B */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT29");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xF361 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xF373 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SP03ACT28");
                CB->animation("ar");
                CB->dialogue("L16D905", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xF3D4 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT29");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 18 — sub_1000F413 — zone 18: exit to room 19 */
static void trig18(void) {
    /* 0xF425 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0xF449 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    /* 0xF46D */
    if (ST(0x528) == 1 && ST(ST_VIE(90)) != VIE_DEAD && ST(ST_VIE(P_DELIA)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) == 2) {
        ST(0x770) = 1;
    }
    ST(ST_LEVEL) = 16;
    CB->change_piece(19, 0);
}

/* trigger 19 — sub_1000F4D6 — inscription L16D118/L16D119 */
static void trig19(void) {
    /* 0xF4E5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x69CF)) {
        ST(ST_SCRIPT_BUSY) = 0;
        CB->bloque(BLOQUE_ALL);
        /* 0xF54E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L16D118A", P_ARKHAN2, "SG00DIA05", NULL);
            /* 0xF576 */
            if (ST(ST_VIE(70)) != VIE_DEAD) {
                lieu_action_register("L16D118B", P_ARKHAN2, "SG00DIA05", NULL);
            }
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xF5C7 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L16D119A", P_ARKHAN3, "SG00DIA05", NULL);
            /* 0xF5EF */
            if (ST(ST_VIE(70)) != VIE_DEAD) {
                lieu_action_register("L16D119B", P_ARKHAN3, "SG00DIA05", NULL);
            }
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 21 — sub_1000F644 — zone 25: LCOD76/LCOD77 */
static void trig21(void) {
    /* 0xF653 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(25)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xF6BC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD76", P_ARKHAN2);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xF707 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD77", P_ARKHAN3);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 22 — sub_1000F74B — zone 30: OB44B / OB65 chest */
static void trig22(void) {
    /* 0xF76B */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 30) && CB->direction_spec(0x8513)) {
        trig30();
    }
    /* 0xF792 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xFE36)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xF7FB */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xF80D */
            if (ST(0x534) == 0) {
                CB->animation("SG00PRE06");
                CB->etat_objet("OB44B", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x534) = 1;
                return;
            }
            /* 0xF880 */
            if (ST(0x534) == 1) {
                CB->animation("SG00PRE06");
                CB->etat_objet("OB65", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x534) = 2;
                return;
            }
            /* 0xF8F3 */
            if (ST(0x534) == 2) {
                CB->animation("SG00PRE02");
                CB->dialogue("LCOD79", P_ARKHAN2);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xF949 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xF95B */
            if (ST(0x534) == 0) {
                CB->animation("SG00PRE06");
                CB->etat_objet("OB44B", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x534) = 1;
                return;
            }
            /* 0xF9CE */
            if (ST(0x534) == 1) {
                CB->animation("SG00PRE06");
                CB->etat_objet("OB65", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x534) = 2;
                return;
            }
            /* 0xFA41 */
            if (ST(0x534) == 2) {
                CB->animation("SG00PRE02");
                CB->dialogue("LCOD80", P_ARKHAN3);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 23 — sub_1000FA90 — open the door (SG00OUV01) */
static void trig23(void) {
    /* 0xFA9F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x2CFF)) {
        CB->bloque(BLOQUE_ALL);
        CB->vire_col(21, 0);
        CB->animation("SG00OUV01");
        CB->play_anim(0, 0);
        CB->animation("ar");
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 24 — sub_1000FB1B — zone 11: OB04 chest */
static void trig24(void) {
    /* 0xFB2A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(11)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xFB93 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xFBA5 */
            if (ST(0x588) == 0) {
                CB->animation("SG00PRE07");
                CB->etat_objet("OB04", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x588) = 1;
                return;
            }
            /* 0xFC18 */
            if (ST(0x588) == 1) {
                CB->animation("SG00PRE03");
                CB->dialogue("LCOD79", P_ARKHAN2);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xFC6E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xFC80 */
            if (ST(0x588) == 0) {
                CB->animation("SG00PRE07");
                CB->etat_objet("OB04", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x588) = 1;
                return;
            }
            /* 0xFCF3 */
            if (ST(0x588) == 1) {
                CB->animation("SG00PRE03");
                CB->dialogue("LCOD80", P_ARKHAN3);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 26 — sub_1000FD4D — sign L16D900/L16D901 */
static void trig26(void) {
    /* 0xFD5C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xF9CB)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xFDC5 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xFDD7 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L16D900", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xFE22 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT29");
                ST(0x80) = CB->divers(DV_HP, ST(ST_HERO), -1);
                CB->divers(DV_HP, ST(ST_HERO), (ST(0x80) - 10));
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xFEB1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xFEC3 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L16D901", P_ARKHAN3);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xFF0E */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT29");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 28 — sub_1000FF58 — sign L16D902/L16D903 */
static void trig28(void) {
    /* 0xFF67 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xFBD6)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xFFD0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L16D902", P_ARKHAN2);
        }
        /* 0xFFF2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L16D903", P_ARKHAN3);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 29 — sub_10010036 — sign LCOD10/LCOD11 */
static void trig29(void) {
    /* 0x45 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xD649)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD10", P_ARKHAN2);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xF9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD11", P_ARKHAN3);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 30 — sub_1001013D — gourd and flask stand */
static void trig30(void) {
    /* 0x14C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8513)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x1B5 */
        if (ST(ST_PLAYER_WEAPON) != 24 && ST(ST_PLAYER_WEAPON) != 77) {
            /* 0x1D9 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L16D701", P_ARKHAN2, "SG02DIA05", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x22A */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L16D702", P_ARKHAN3, "SG03DIA05", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x27B */
        if (ST(ST_PLAYER_WEAPON) == 24) {
            /* 0x28D */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L08D71", P_ARKHAN2, "SG00ATT01", NULL);
            }
            /* 0x2B5 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L08D72", P_ARKHAN3, "SG00ATT01", NULL);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x306 */
        if (ST(ST_PLAYER_WEAPON) == 77) {
            /* 0x318 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x32A */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SP02ACT03");
                }
                /* 0x347 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP02ACT33");
                }
            }
            /* 0x364 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x376 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SP03ACT03");
                }
                /* 0x393 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP03ACT33");
                }
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            ST(0x140 + 4 * ST(ST_USED_VARIANT)) = 3;
            snprintf(g_txt, sizeof(g_txt), "OB77%c", ST(ST_USED_VARIANT) + 65);
            snprintf(g_txt2, sizeof(g_txt2), "OB24%c", ST(ST_USED_VARIANT) + 65);
            CB->etat_objet(g_txt, -1, -1, -1, -1, -1, -1, -2, 3);
            CB->etat_objet(g_txt2, -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("Sar_10");
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 31 — sub_1001046C — ATT92 hit animation */
static void trig31(void) {
    /* 0x47B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x7530)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x4E4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ATT92");
            CB->animation("P02ATT92");
            CB->animation("P02ATT92");
            CB->animation("P02ATT92");
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x54B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ATT92");
            CB->animation("P03ATT92");
            CB->animation("P03ATT92");
            CB->animation("P03ATT92");
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 32 — sub_100105AB — LCOD07/LCOD08 */
static void trig32(void) {
    /* 0x5BA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x5381)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE02");
        CB->animation("ar");
        /* 0x639 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD07", P_ARKHAN2);
        }
        /* 0x65B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD08", P_ARKHAN3);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 41 — sub_10007EE8 — room entry: music, Maor/Delia intro scene */
static void trig41(void) {
    /* 0x7EFA */
    if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(0xBF0) >= 1) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x7F30 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 1);
    }
    /* 0x7F54 */
    if (ST(0xBF0) >= 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        ST(0xCF4) = 1;
    }
    /* 0x7F87 */
    if (ST(0xBF0) > 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->evenement(1, 1000);
    }
    /* 0x7FBB */
    if (ST(0xBF0) == 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->bloque(BLOQUE_ALL);
        CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
        CB->cinematique("SC11_500");
        CB->change_camera(2, CAM_LOCKED);
        CB->bruitage("L16AMB1", -1, -1);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->evenement(1, -1);
        CB->joindre(P_DELIA, 0, 0, 0, 17);
        lieu_action_register("L16D200", P_DELIA, "SP10DIA54", "SP10ARR01_10");
        CB->joindre(P_MAOR, 0, 0, 0, 10);
        lieu_action_register("L16D201A", P_MAOR, "SP17DIA58", "SP17ARR01_10");
        lieu_action_register("L16D201B", P_MAOR, "SP17DIA58", "SP17ARR01_10");
        lieu_action_register("L16D202", P_DELIA, "SP10DIA54", "SP10ARR01_10");
        CB->divers_empile(DV_MUSIC, 1, 1);
        CB->joindre(P_DELIA, 2558, 3811, 0xB5B5, -1);
        CB->evenement(0, 1000);
        CB->evenement(1, 500);
        ST(0xBF0) = 1;
    }
    /* 0x812C */
    if (ST(0x4D4) == 1 && ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_VIE(P_MAOR)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = P_MAOR;
        CB->combat(P_MAOR, 0);
    }
    /* 0x817B */
    if (ST(0x528) == 1 && ST(ST_VIE(90)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 90;
        CB->combat(90, 0);
    }
}
