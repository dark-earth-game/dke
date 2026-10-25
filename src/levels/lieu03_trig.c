/*
 * lieu03_trig.c — Native hand-port of the LIEU03.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu03.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu03_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig01(void); static void trig02(void); static void trig03(void);
static void trig04(void); static void trig05(void); static void trig06(void);
static void trig07(void); static void trig09(void); static void trig12(void);
static void trig14(void); static void trig41(void);
static void trig_noop(void);   /* the 31 stub slots */

/* dword_1001DF10[42] — filled at INIT (0x1000DE.., before the reload-flag
 * test) with the 5-byte jmp thunks at 0x10001000+5*i; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000C56D       28  1000C7B7 stub
 *    1  1000995C       15  1000C728 stub  29  1000C7C2 stub
 *    2  10009B34       16  1000C733 stub  30  1000C7CD stub
 *    3  10009DBE       17  1000C73E stub  31  1000C7D8 stub
 *    4  10009FF3       18  1000C749 stub  32  1000C7E3 stub
 *    5  1000ABFF       19  1000C754 stub  33  1000C7EE stub
 *    6  1000B759       20  1000C75F stub  34  1000C7F9 stub
 *    7  1000B91D       21  1000C76A stub  35  1000C804 stub
 *    8  1000BBBE stub  22  1000C775 stub  36  1000C80F stub
 *    9  1000BBC9       23  1000C780 stub  37  1000C81A stub
 *   10  1000BF08 stub  24  1000C78B stub  38  1000C825 stub
 *   11  1000BF13 stub  25  1000C796 stub  39  1000C830 stub
 *   12  1000BF1E       26  1000C7A1 stub  40  1000C83B stub
 *   13  1000C562 stub  27  1000C7AC stub  41  10008051
 */
static void (*const g_trigger[42])(void) = {
    l03_trig00, trig01, trig02, trig03, trig04, trig05, trig06, trig07,
    trig_noop, trig09, trig_noop, trig_noop, trig12, trig_noop, trig14, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001DF10]). */
void l03_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: entry presets, state reset, zones, objects, actor placement */
void l03_trig00(void) {
    /* 0x7402 */
    if (ST(0x96C) == 1) {
        ST(ST_LEVEL) = 4;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xB40) = 3;
        ST(ST_VIE(P_KALHI33)) = 0;
        ST(ST_VIE(P_KALHI13)) = 1;
    }
    /* 0x745C */
    if (ST(0x96C) == 2) {
        ST(ST_LEVEL) = 4;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xB40) = 3;
        ST(ST_VIE(P_KALHI13)) = 0;
        ST(ST_VIE(P_KALHI33)) = 1;
    }
    /* 0x74B6 */
    if (ST(0x96C) == 3) {
        ST(ST_LEVEL) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_DELIA_PHASE) = 8;
        ST(V_KALHI_ROUTE) = 1;
    }
    /* 0x7501 */
    if (ST(0x96C) == 4) {
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_LEVEL) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_KALHI_ROUTE) = 1;
    }
    ST(V_ENGAGED_PERSONA) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_LAST_WEAPON) = 0;
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
    ST(0xE0) = -1;
    ST(0xF0) = -1;
    ST(0xF4) = -1;
    ST(0xFC) = 0;
    CB->ajoute_zone(1, -1641, -1176, 80, 150, 1);
    CB->ajoute_zone(2, -1850, -2052, 190, 120, 0);
    CB->ajoute_zone(3, -1861, -554, 120, 60, 0);
    CB->ajoute_zone(6, -1641, -1563, 80, 150, 1);
    CB->ajoute_zone(7, -1943, -633, 125, 175, 0);
    CB->ajoute_zone(9, -1781, -713, 112, 304, 0);
    CB->ajoute_zone(11, -1983, -546, 70, 140, 0);
    CB->ajoute_zone(13, -1320, -1390, 100, 615, 0);
    CB->ajoute_zone(14, -1944, -1375, 125, 1245, 0);
    CB->ajoute_zone(15, -1958, -712, 5, 5, 0);
    CB->bruitage("L03AMB1", -1, -1);
    CB->etat_objet("IM02", -1925, 12, -2060, 0xC000, 0x4000, 0x8000, 0, 3);
    CB->etat_objet("IM67", -1794, 12, -481, 0xC000, 0x4000, 0, 0, 3);
    CB->place_animal(1, -1897, 12, -2017, 0xC350, 0x800);
    CB->place_animal(1, -1641, 12, -1176, 0x61A8, 0x800);
    /* 0x78C7 */
    if (ST(ST_LEVEL) == 45) {
        CB->pos_perso(ST(ST_HERO), -1859, -567, 0x7C9E, 1, "ar", 8, 0);
    }
    /* 0x7901 */
    if (ST(ST_LEVEL) == 2) {
        /* 0x7913 */
        if (ST(V_KALHI_ROUTE) == 1) {
            CB->pos_perso(ST(ST_HERO), -1738, -1160, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 8, 0);
        }
        /* 0x7964 */
        if (ST(V_KALHI_ROUTE) == 2) {
            CB->pos_perso(ST(ST_HERO), -1720, -1558, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 8, 0);
        }
    }
    /* 0x79B2 */
    if (ST(ST_LEVEL) == 4) {
        /* 0x79C4 */
        if (ST(0xB40) != 3 || (ST(0xB40) == 3 && ST(0xE9C) != 0)) {
            CB->pos_perso(ST(ST_HERO), -1845, -1890, 0xEE48, 1, "ar", 8, 0);
        }
        /* 0x7A25 */
        if (ST(0xB40) == 3 && ST(0xE9C) == 0) {
            /* 0x7A49 */
            if (ST(0xE58) == 1 && (ST(ST_VIE(P_KALHI13)) == VIE_DEAD || ST(ST_VIE(P_KALHI33)) == VIE_DEAD)) {
                CB->pos_perso(ST(ST_HERO), -1845, -1890, 0xEE48, 1, "ar", 8, 0);
            }
            /* 0x7AAA */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->pos_perso(ST(ST_HERO), -1845, -1890, 0xEE48, 1, "ar", 8, 0);
                CB->pos_perso(P_KALHI13, -1875, -2020, 0x3F4, 1, "P13ARR01", 4, 1);
                ST(0x7C) = 1;
                ST(0x78) = 1;
            }
            /* 0x7B24 */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->bloque(BLOQUE_ALL);
                CB->pos_perso(ST(ST_HERO), -1875, -2020, 0x3F4, 1, "ar", 8, 0);
                CB->pos_perso(P_KALHI33, -1845, -1900, 0xEE48, 1, "P33ARR01", 5, 1);
            }
        }
    }
    /* 0x7B91 */
    if (ST(0xE9C) == 1 && ST(ST_VIE(P_KALHI13)) == 1) {
        /* 0x7BB2 */
        if (ST(ST_LEVEL) == 45) {
            CB->pos_perso(P_KALHI13, -1875, -898, 0xFE43, 1, "P13ARR01", 4, 1);
        }
        /* 0x7BE6 */
        if (ST(ST_LEVEL) == 4) {
            CB->pos_perso(P_KALHI13, -1847, -1741, 0x7B05, 1, "P13ARR01", 4, 1);
        }
        ST(0x7C) = 1;
        ST(0x78) = 1;
    }
    /* 0x7C35 */
    if (ST(0xE9C) == 2) {
        /* 0x7C47 */
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            CB->pos_perso(P_KALHI13, -1930, -1400, 0xCB20, 1, "P13ARR03", 4, 1);
            ST(V_EVENT15_STEP) = 1;
        }
        /* 0x7C8D */
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            CB->pos_perso(P_KALHI33, -1930, -1400, 0xCB20, 1, "P33ARR03", 5, 1);
            ST(V_EVENT15_STEP) = 1;
        }
        /* 0x7CD3 */
        if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
            CB->pos_perso(P_KALHI13, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 2, "P13cad", 4, 0);
        }
        /* 0x7D35 */
        if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
            CB->pos_perso(P_KALHI33, CB->divers(DV_DESC_POS, P_KALHI33, 0), CB->divers(DV_DESC_POS, P_KALHI33, 1), CB->divers(DV_DESC_POS, P_KALHI33, 2), 2, "P33ARR03", 5, 1);
        }
    }
    /* 0x7D97 */
    if (ST(V_DELIA_PHASE) == 8) {
        ST(0x68) = 1;
        /* 0x7DB5 */
        if (ST(V_KALHI_ROUTE) == 1) {
            CB->pos_perso(P_DELIA, -1590, -1150, 0x32C8, 1, "P10ARR01", 0xC, 0);
        }
        /* 0x7DEC */
        if (ST(V_KALHI_ROUTE) == 2) {
            CB->pos_perso(P_DELIA, -1590, -1590, 0x32C8, 1, "P10ARR01", 0xC, 0);
        }
    }
    /* 0x7E23 */
    if (ST(V_DELIA_PHASE) == 9) {
        /* 0x7E32 */
        if (ST(ST_LEVEL) == 2) {
            CB->pos_perso(P_DELIA, -1940, -1330, 0x4074, 1, "P10ARR01", 0xC, 0);
        }
        /* 0x7E66 */
        if (ST(ST_LEVEL) != 2) {
            CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 1, "P10ARR01", 0xC, 0);
        }
    }
    /* 0x7EC8 */
    if (ST(ST_VIE(P_DELIA)) == VIE_DEAD && ST(V_DELIA_KILLED) == 3) {
        CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 2, "P10cad", 0xC, 0);
    }
    /* 0x7F3C */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0 && ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
        ST(0xB98) = 0;
    }
    /* 0x7FA5 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }
    ST(0x96C) = 0;
    /* 0x801A */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->load_anim("203EAU01", 0);
    }
    /* 0x8039 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->load_anim("203EAU02", 0);
    }
}

/* trigger 01 — sub_1000995C — zone 1: exit to room 2, route 1 */
static void trig01(void) {
    /* 0x996E */
    if (ST(V_DELIA_PHASE) == 9) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SAVED_HERO) = ST(ST_HERO);
        CB->retire_zone(1);
        CB->animation("Sar_10");
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 0xA);
        CB->animation("Sar_10");
        /* 0x99E0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L03D01", P_ARKHAN2);
        }
        /* 0x9A02 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L03D02", P_ARKHAN3);
        }
        CB->bloque(BLOQUE_NONE);
        CB->modify_var(&ST(V_KALHI_ROUTE), 1);
    }
    /* 0x9A43 */
    if (ST(0xE9C) == 1) {
        ST(0xE9C) = 3;
        /* 0x9A64 */
        if (ST(0xF3C) == 0 && ST(ST_VIE(P_KALHI13)) == 1) {
            ST(0xF3C) = 2;
        }
        /* 0x9A97 */
        if (ST(0xF3C) == 0 && ST(ST_VIE(P_KALHI33)) == 1) {
            /* 0x9AC6 */
            if (CB->divers(DV_DESC_POS, P_KALHI33, 1) < -1400) {
                ST(0xF3C) = 2;
            }
            /* 0x9AF2 */
            if (CB->divers(DV_DESC_POS, P_KALHI33, 1) > -1400) {
                ST(0xF3C) = 1;
            }
        }
    }
    ST(V_KALHI_ROUTE) = 1;
    ST(ST_LEVEL) = 3;
    CB->change_piece(2, 0);
}

/* trigger 02 — sub_10009B34 — zone 2: door IM02 to room 4 */
static void trig02(void) {
    /* 0x9B43 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x7530)) {
        /* 0x9B77 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->animation("SG00OUV01");
            CB->animation("P00_65535");
            CB->anim_objet("IM02", 3, 0x3C, 0x5DC, 0);
            ST(ST_LEVEL) = 3;
            CB->change_piece(4, 0);
            return;
        }
        /* 0x9BED */
        if (ST(ST_HERO) >= 2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x9C16 */
            if (ST(0xE9C) == 1 && (ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1)) {
                ST(0x7C) = 0xA;
                ST(V_SAVED_HERO) = ST(ST_HERO);
                /* 0x9C77 */
                if (CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0)) {
                    CB->stop_joindre(-P_KALHI13);
                    CB->dialogue("L03D03", P_KALHI13);
                }
                /* 0x9CAC */
                if (CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0)) {
                    CB->stop_joindre(-P_KALHI33);
                    CB->dialogue("L03D03", P_KALHI33);
                }
            }
            /* 0x9CD9 */
            if (ST(V_DELIA_PHASE) == 9) {
                ST(V_SAVED_HERO) = ST(ST_HERO);
                CB->stop_joindre(-P_DELIA);
                /* 0x9D0D */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L03D01", P_ARKHAN2);
                }
                /* 0x9D2F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L03D02", P_ARKHAN3);
                }
            }
            CB->animation("SG00OUV01");
            CB->animation("P00_65535");
            CB->anim_objet("IM02", 3, 0x3C, 0x5DC, 0);
            ST(ST_LEVEL) = 3;
            /* 0x9D8C */
            if (ST(0xB40) < 4) {
                CB->change_piece(4, 0);
            } else {
                CB->change_piece(4, 1);
            }
            return;
        }
    }
}

/* trigger 03 — sub_10009DBE — zone 3: Kalhi13 shortcut into trigger 4; door IM67 to room 45 */
static void trig03(void) {
    /* 0x9DDE */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 4) && CB->direction_perso(P_KALHI13)) {
        trig04();
        return;
    }
    /* 0x9E0A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xA)) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9E62 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->animation("SG00OUV01");
            CB->animation("P00_65535");
            CB->anim_objet("IM67", 3, 0x3C, 0x5DC, 0);
            ST(ST_LEVEL) = 3;
            CB->change_piece(45, 0);
            return;
        }
        /* 0x9EC1 */
        if (ST(ST_HERO) >= 2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x9EEA */
            if (ST(0xE9C) == 1 && (ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1)) {
                ST(0x7C) = 0xA;
                ST(V_SAVED_HERO) = ST(ST_HERO);
                /* 0x9F4B */
                if (CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0)) {
                    CB->stop_joindre(-P_KALHI13);
                    CB->dialogue("L03D03", P_KALHI13);
                }
                /* 0x9F80 */
                if (CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0)) {
                    CB->stop_joindre(-P_KALHI33);
                    CB->dialogue("L03D03", P_KALHI13);   /* sic: 0x10009F96 passes 13 (not 33) in the Kalhi33 branch */
                }
            }
            CB->animation("SG00OUV01");
            CB->animation("P00_65535");
            CB->anim_objet("IM67", 3, 0x3C, 0x5DC, 0);
            ST(ST_LEVEL) = 3;
            CB->change_piece(45, 0);
            return;
        }
    }
}

/* trigger 04 — sub_10009FF3 — Kalhi13 interaction (talk, fight, fouille) */
static void trig04(void) {
    /* 0xA002 */
    if (ST(ST_ACTION_KEY) == 1 && !CB->direction_perso(P_KALHI13)) {
        /* 0xA035 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 1)) {
            trig01();
            return;
        }
        /* 0xA05F */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
            trig02();
            return;
        }
        /* 0xA089 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
            trig03();
            return;
        }
        /* 0xA0B3 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 6)) {
            trig06();
            return;
        }
        /* 0xA0DD */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
            trig07();
            return;
        }
        /* 0xA107 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
            trig09();
            return;
        }
        /* 0xA131 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 14)) {
            trig14();
            return;
        }
    }
    /* 0xA14D */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KALHI13) && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L04D41", P_KALHI13, "SP13DIA59", "SP13ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA214 */
    if (CB->direction_perso(P_KALHI13)) {
        /* 0xA223 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
            CB->select_pile(0, 2);
            /* 0xA260 */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(0x7C) = 0x64;
                ST(0x78) = 2;
                ST(ST_SCRIPT_BUSY) = 1;
                CB->stop_joindre(-P_KALHI13);
                CB->joindre(P_KALHI13, 0, 0, 0, ST(ST_HERO));
                CB->joindre(-ST(ST_HERO), 0, 0, 0, 0xD);
                CB->animation("Sar_10");
                /* 0xA2F1 */
                if (ST(V_KALHI13_TALK_A) == 0) {
                    ST(V_KALHI13_TALK_A) = 1;
                    /* 0xA312 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L03D503", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0xA33A */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L03D504", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L03D505", P_KALHI13, "SP13DIA50", "SP13ARR01_10");
                    /* 0xA37B */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L03D506A", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L03D506B", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0xA3B9 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L03D507", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L03D508A", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                    lieu_action_register("L03D508B", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                    /* 0xA413 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L03D509", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0xA43B */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L03D510", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L03D511", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->modify_var(&ST(0x7C), 1);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xA4BB */
                if (ST(V_KALHI13_TALK_A) == 1) {
                    /* 0xA4CD */
                    if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                        /* 0xA522 */
                        if (ST(V_KALHI13_TALK_B) == 0 || ST(V_KALHI13_TALK_B) == 1) {
                            ST(V_KALHI13_TALK_B) = 2;
                            /* 0xA555 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L03D512", P_ARKHAN2, "SG00DIA07", NULL);
                            }
                            /* 0xA57D */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L03D513", P_ARKHAN3, "SG00DIA07", NULL);
                            }
                            lieu_action_register("L03D514", P_KALHI13, "SP13DIA56", "SP13ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x7C), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xA5FD */
                        if (ST(V_KALHI13_TALK_B) == 2) {
                            /* 0xA60F */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L03D515", P_ARKHAN2, "SG00DIA05", NULL);
                            }
                            /* 0xA637 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L03D516", P_ARKHAN3, "SG00DIA05", NULL);
                            }
                            lieu_action_register("L03D517", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x7C), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                    }
                    /* 0xA6B7 */
                    if (ST(ST_PLAYER_WEAPON) == 1) {
                        /* 0xA6C9 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L03D518", P_ARKHAN2, "SG00DIA02", NULL);
                            CB->dialogue("L03D519", P_ARKHAN2);
                        }
                        /* 0xA701 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->dialogue("L03D520", P_ARKHAN3);
                        }
                        lieu_pre_scene(ST(ST_HERO), P_KALHI13, 1, 6, 5);
                        CB->dialogue("L03D521", P_KALHI13);
                        CB->animation("SP13ARR01_50");
                        CB->modify_var(&ST(0x74), 1);
                        return;
                    }
                    /* 0xA772 */
                    if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 1 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0xA7DB */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        }
                        /* 0xA806 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                        }
                        lieu_action_register("L03D522", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x7C), 1);
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
            }
            /* 0xA89F */
            if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
                CB->fouille(P_KALHI13);
                return;
            }
        }
        /* 0xA8BE */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == P_KALHI13) {
            CB->bloque(BLOQUE_ALL);
            /* 0xA8F9 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xA90B */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L03D523", P_ARKHAN2);
                }
                /* 0xA92D */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L03D524", P_ARKHAN2);
                }
            }
            /* 0xA94F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xA961 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L03D525", P_ARKHAN3);
                }
                /* 0xA983 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("LCOD22A", P_ARKHAN3);
                }
            }
            CB->bloque(BLOQUE_NONE);
        }
        /* 0xA9AA */
        if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KALHI13)) == 1) {
            CB->select_pile(0, 2);
            /* 0xA9E7 */
            if (ST(V_PLAYER_STRIKE) == 2) {
                ST(0x7C) = 0x64;
                ST(V_PLAYER_STRIKE) = -1;
                CB->stop_joindre(-P_KALHI13);
                CB->dialogue("L04D43", P_KALHI13);
                ST(V_KALHI13_TALK_C) = 2;
                ST(V_ENGAGED_PERSONA) = P_KALHI13;
                CB->combat(P_KALHI13, 0);
                return;
            }
            CB->stop_joindre(-P_KALHI13);
            ST(0x7C) = 0x64;
            CB->bloque(BLOQUE_ALL);
            ST(V_PLAYER_STRIKE) = -1;
            CB->animation("SP13ESA15");
            CB->animation("P13_65535");
            /* 0xAA9D */
            if (ST(V_KALHI13_TALK_C) == 0) {
                ST(V_KALHI13_TALK_C) = 1;
                lieu_action_register("L04D41", P_KALHI13, "P13DIA59_10", "SP13ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x7C), 1);
                CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                CB->select_pile(0, 1);
                return;
            }
            /* 0xAB1B */
            if (ST(V_KALHI13_TALK_C) == 1) {
                ST(V_KALHI13_TALK_C) = 2;
                lieu_action_register("L04D42", P_KALHI13, "P13DIA59_10", "SP13ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x7C), 1);
                CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                CB->select_pile(0, 1);
                return;
            }
            /* 0xAB99 */
            if (ST(V_KALHI13_TALK_C) == 2) {
                ST(0xE9C) = 2;
                lieu_action_register("L04D43", P_KALHI13, "P13GAR26_10", "SP13GAR02_10");
                CB->bloque(BLOQUE_NONE);
                CB->combat(P_KALHI13, 0);
                ST(V_ENGAGED_PERSONA) = P_KALHI13;
                CB->select_pile(0, 1);
                return;
            }
        }
    }
}

/* trigger 05 — sub_1000ABFF — Kalhi33 interaction (talk, fight, fouille) */
static void trig05(void) {
    /* 0xAC1F */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 1)) {
        trig01();
        return;
    }
    /* 0xAC49 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
        trig02();
        return;
    }
    /* 0xAC73 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
        return;
    }
    /* 0xAC9D */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 6)) {
        trig06();
        return;
    }
    /* 0xACC7 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        trig07();
        return;
    }
    /* 0xACF1 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
        trig09();
        return;
    }
    /* 0xAD0D */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KALHI33) && ST(ST_VIE(P_KALHI33)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L04D41", P_KALHI13, "SP13DIA59", "SP13ARR01_10");   /* speaker 13 in the Kalhi33 block, as in the original */
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xADD4 */
    if (CB->direction_perso(P_KALHI33)) {
        /* 0xADE3 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
            CB->select_pile(0, 2);
            /* 0xAE20 */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->select_pile(1, 1);
                CB->stop_joindre(-P_KALHI33);
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                CB->select_pile(0, 1);
                CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x21);
                CB->animation("Sar_10");
                /* 0xAEB6 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0xAF0B */
                    if (ST(V_KALHI13_TALK_B) == 0) {
                        ST(V_KALHI13_TALK_B) = 1;
                        /* 0xAF2C */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L03D512", P_ARKHAN2, "SG00DIA07", NULL);
                        }
                        /* 0xAF54 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L03D513", P_ARKHAN3, "SG00DIA07", NULL);
                        }
                        lieu_action_register("L03D530A", P_KALHI33, "SP33DIA56", "SP33ARR01_10");
                        lieu_action_register("L03D530B", P_KALHI33, "SP33DIA56", "SP33ARR01_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        /* 0xAFCA */
                        if (CB->divers(DV_DESC_POS, P_KALHI33, 1) < -1400) {
                            CB->joindre(P_KALHI33, -1400, -1620, -1, -1);
                        }
                        /* 0xB000 */
                        if (CB->divers(DV_DESC_POS, P_KALHI33, 1) > -1400) {
                            CB->joindre(P_KALHI33, -1400, -1180, -1, -1);
                        }
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                    /* 0xB045 */
                    if (ST(V_KALHI13_TALK_B) == 1) {
                        ST(V_KALHI13_TALK_B) = 2;
                        /* 0xB066 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L03D531A", P_ARKHAN2, "SG00DIA05", NULL);
                            /* 0xB08E */
                            if (ST(ST_VIE(70)) != VIE_DEAD) {
                                lieu_action_register("L03D531B", P_ARKHAN2, "SG00DIA05", NULL);
                            }
                        }
                        /* 0xB0B6 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L03D532", P_ARKHAN3, "SG00DIA05", NULL);
                        }
                        lieu_action_register("L03D533", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        /* 0xB113 */
                        if (CB->divers(DV_DESC_POS, P_KALHI33, 1) < -1400) {
                            CB->joindre(P_KALHI33, -1400, -1620, -1, -1);
                        }
                        /* 0xB149 */
                        if (CB->divers(DV_DESC_POS, P_KALHI33, 1) > -1400) {
                            CB->joindre(P_KALHI33, -1400, -1180, -1, -1);
                        }
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                    /* 0xB18E */
                    if (ST(V_KALHI13_TALK_B) == 2) {
                        /* 0xB1A0 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L03D534", P_ARKHAN2, "SG00DIA06", NULL);
                        }
                        /* 0xB1C8 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L03D535", P_ARKHAN3, "SG00DIA06", NULL);
                        }
                        lieu_action_register("L03D536", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        /* 0xB225 */
                        if (CB->divers(DV_DESC_POS, P_KALHI33, 1) < -1400) {
                            CB->joindre(P_KALHI33, -1400, -1620, -1, -1);
                        }
                        /* 0xB25B */
                        if (CB->divers(DV_DESC_POS, P_KALHI33, 1) > -1400) {
                            CB->joindre(P_KALHI33, -1400, -1180, -1, -1);
                        }
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
                /* 0xB2A0 */
                if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 1 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0xB309 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xB334 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L03D522", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    /* 0xB3AA */
                    if (CB->divers(DV_DESC_POS, P_KALHI33, 1) < -1400) {
                        CB->joindre(P_KALHI33, -1400, -1620, -1, -1);
                    }
                    /* 0xB3E0 */
                    if (CB->divers(DV_DESC_POS, P_KALHI33, 1) > -1400) {
                        CB->joindre(P_KALHI33, -1400, -1180, -1, -1);
                    }
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
            }
            /* 0xB425 */
            if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
                CB->fouille(P_KALHI33);
                return;
            }
        }
        /* 0xB444 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == P_KALHI33 && ST(ST_COMBAT_STANCE) == 0) {
            CB->bloque(BLOQUE_ALL);
            /* 0xB47F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xB491 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L03D523", P_ARKHAN2);
                }
                /* 0xB4B3 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L03D524", P_ARKHAN2);
                }
            }
            /* 0xB4D5 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xB4E7 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L03D525", P_ARKHAN3);
                }
                /* 0xB509 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("LCOD22A", P_ARKHAN3);
                }
            }
            CB->bloque(BLOQUE_NONE);
        }
        /* 0xB530 */
        if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KALHI33)) == 1) {
            CB->select_pile(0, 2);
            /* 0xB56D */
            if (ST(V_PLAYER_STRIKE) == 2) {
                ST(0x7C) = 0x64;
                ST(V_PLAYER_STRIKE) = -1;
                CB->stop_joindre(-P_KALHI33);
                CB->dialogue("L03D528", P_KALHI33);
                ST(V_KALHI13_TALK_C) = 2;
                ST(V_ENGAGED_PERSONA) = P_KALHI33;
                CB->combat(P_KALHI33, 0);
                return;
            }
            CB->stop_joindre(-P_KALHI33);
            ST(V_PLAYER_STRIKE) = -1;
            CB->bloque(BLOQUE_ALL);
            ST(0x7C) = 0x64;
            CB->animation("SP33ESA15");
            CB->animation("P33_65535");
            /* 0xB623 */
            if (ST(V_KALHI13_TALK_C) == 0) {
                ST(V_KALHI13_TALK_C) = 1;
                lieu_action_register("L03D526", P_KALHI33, "P33DIA59_10", "SP33ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x7C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xB68B */
            if (ST(V_KALHI13_TALK_C) == 1) {
                ST(V_KALHI13_TALK_C) = 2;
                lieu_action_register("L03D527", P_KALHI33, "P33DIA59_10", "SP33ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x7C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xB6F3 */
            if (ST(V_KALHI13_TALK_C) == 2) {
                ST(0xE9C) = 2;
                lieu_action_register("L03D528", P_KALHI33, "P33GAR26", "SP33GAR02_10");
                CB->bloque(BLOQUE_NONE);
                ST(V_ENGAGED_PERSONA) = P_KALHI33;
                CB->combat(P_KALHI33, 0);
                CB->select_pile(0, 1);
                return;
            }
        }
    }
}

/* trigger 06 — sub_1000B759 — zone 6: exit to room 2, route 2 */
static void trig06(void) {
    /* 0xB76B */
    if (ST(V_DELIA_PHASE) == 9) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SAVED_HERO) = ST(ST_HERO);
        CB->retire_zone(6);
        CB->animation("Sar_10");
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 0xA);
        CB->animation("Sar_10");
        /* 0xB7DD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L03D01", P_ARKHAN2);
        }
        /* 0xB7FF */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L03D02", P_ARKHAN3);
        }
        CB->animation("Sar_10");
    }
    /* 0xB82C */
    if (ST(0xE9C) == 1) {
        ST(0xE9C) = 3;
        /* 0xB84D */
        if (ST(0xF3C) == 0 && ST(ST_VIE(P_KALHI13)) == 1) {
            ST(0xF3C) = 2;
        }
        /* 0xB880 */
        if (ST(0xF3C) == 0 && ST(ST_VIE(P_KALHI33)) == 1) {
            /* 0xB8AF */
            if (CB->divers(DV_DESC_POS, P_KALHI33, 1) < -1400) {
                ST(0xF3C) = 2;
            }
            /* 0xB8DB */
            if (CB->divers(DV_DESC_POS, P_KALHI33, 1) > -1400) {
                ST(0xF3C) = 1;
            }
        }
    }
    ST(V_KALHI_ROUTE) = 2;
    ST(ST_LEVEL) = 3;
    CB->change_piece(2, 0);
}

/* trigger 07 — sub_1000B91D — zone 7: hero remark at object zone 11 (L03D04..L03D08) */
static void trig07(void) {
    /* 0xB92C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(11)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xB991 */
        if (CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0) && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
            CB->stop_joindre(-P_KALHI13);
        }
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xB9CC */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0xB9DE */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L03D04A", P_ARKHAN1, "SG00ATT01", NULL);
                lieu_action_register("L03D04B", P_ARKHAN1, "SG00ATT01", NULL);
            }
            /* 0xBA1C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L03D05", P_ARKHAN1, "SP01ACT32", NULL);
            }
        }
        /* 0xBA44 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xBA56 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L03D06", P_ARKHAN2, "SG00ATT01", NULL);
            }
            /* 0xBA7E */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xBA98 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("L03D07", P_ARKHAN2, "SP02ACT85", NULL);
                }
                /* 0xBAC8 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("L03D07", P_ARKHAN2, "SP02ACT86", NULL);
                }
            }
        }
        /* 0xBAF0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xBB02 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L03D08", P_ARKHAN3, "SG00ATT01", NULL);
            }
            /* 0xBB2A */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xBB44 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("LCOD22B", P_ARKHAN3, "SP03ACT85", NULL);
                }
                /* 0xBB74 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("LCOD22B", P_ARKHAN3, "SP03ACT86", NULL);
                }
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 09 — sub_1000BBC9 — zone 9: action key runs trigger 3; hero remark (as trigger 7) */
static void trig09(void) {
    /* 0xBBD8 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_spec(0)) {
        trig03();
    }
    /* 0xBC1F */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ((CB->direction_zone(9) && CB->divers(DV_DESC_POS, ST(ST_HERO), 2) > 0x8E94) || (CB->direction_spec(0xC000) && ST(ST_LOCKED_CAMERA) == 1))) {
        CB->bloque(BLOQUE_ALL);
        /* 0xBCDB */
        if (CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0) && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
            CB->stop_joindre(-P_KALHI13);
        }
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBD16 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0xBD28 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L03D04A", P_ARKHAN1, "SG00ATT01", NULL);
                lieu_action_register("L03D04B", P_ARKHAN1, "SG00ATT01", NULL);
            }
            /* 0xBD66 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L03D05", P_ARKHAN1, "SP01ACT32", NULL);
            }
        }
        /* 0xBD8E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xBDA0 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L03D06", P_ARKHAN2, "SG00ATT01", NULL);
            }
            /* 0xBDC8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xBDE2 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("L03D07", P_ARKHAN2, "SP02ACT85", NULL);
                }
                /* 0xBE12 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("L03D07", P_ARKHAN2, "SP02ACT86", NULL);
                }
            }
        }
        /* 0xBE3A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xBE4C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L03D08", P_ARKHAN3, "SG00ATT01", NULL);
            }
            /* 0xBE74 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xBE8E */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("LCOD22B", P_ARKHAN3, "SP03ACT85", NULL);
                }
                /* 0xBEBE */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("LCOD22B", P_ARKHAN3, "SP03ACT86", NULL);
                }
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 — sub_1000BF1E — Delia interaction (talk, hit, fouille) */
static void trig12(void) {
    /* 0xBF2D */
    if (ST(ST_ACTION_KEY) == 1 && !CB->direction_perso(P_DELIA)) {
        /* 0xBF60 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 1)) {
            trig01();
            return;
        }
        /* 0xBF8A */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
            trig02();
            return;
        }
        /* 0xBFB4 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
            trig03();
            return;
        }
        /* 0xBFDE */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 6)) {
            trig06();
            return;
        }
        /* 0xC008 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
            trig07();
            return;
        }
        /* 0xC032 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
            trig09();
            return;
        }
        /* 0xC05C */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 14)) {
            trig14();
            return;
        }
    }
    /* 0xC079 */
    if (CB->direction_perso(P_DELIA)) {
        /* 0xC088 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
            /* 0xC0B8 */
            if (ST(ST_VIE(P_DELIA)) < 2) {
                ST(ST_SCRIPT_BUSY) = 1;
                CB->bloque(BLOQUE_ALL);
                CB->stop_joindre(-P_DELIA);
                ST(0x68) = 0x14;
                /* 0xC0F8 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0xC14D */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L03D10", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0xC175 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L03D11", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L03D12", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x68), 1);
                }
                /* 0xC1E3 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0xC23A */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xC265 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L03D13", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                    CB->animation("G00PRE82");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x68), 1);
                }
            }
            /* 0xC2E1 */
            if (ST(ST_VIE(P_DELIA)) == VIE_DEAD) {
                CB->fouille(P_DELIA);
                return;
            }
        }
        /* 0xC300 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == P_DELIA && ST(ST_COMBAT_STANCE) == 0) {
            /* 0xC330 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L03D523", P_ARKHAN2);
            }
            /* 0xC352 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L03D16", P_ARKHAN3);
            }
        }
        /* 0xC371 */
        if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_DELIA)) == 1) {
            ST(0x68) = 0xA;
            CB->stop_joindre(-P_DELIA);
            CB->bloque(BLOQUE_ALL);
            ST(V_PLAYER_STRIKE) = -1;
            /* 0xC3E1 */
            if (ST(V_DELIA_HITS) == 1) {
                ST(ST_VIE(P_DELIA)) = VIE_DEAD;
                ST(V_DELIA_PHASE) = 0x14;
                ST(V_DELIA_KILLED) = 3;
                CB->animation("SP10IMP51");
                CB->animation("P10cad_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC443 */
            if (ST(V_DELIA_HITS) == 0) {
                ST(V_DELIA_HITS) = 1;
                CB->animation("SP10ESA01");
                CB->animation("P10GAR01_10");
                CB->dialogue("L03D09", P_DELIA);
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x68), 1);
                return;
            }
        }
        return;
    }
    /* 0xC4BE */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 1)) {
        trig01();
    }
    /* 0xC4E3 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
        trig02();
    }
    /* 0xC508 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
    }
    /* 0xC52D */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 6)) {
        trig06();
    }
    /* 0xC552 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        trig07();
    }
}

/* trigger 14 — sub_1000C56D — zone 14: hero remark at spec 0x4000 (L03D17..L03D19) */
static void trig14(void) {
    /* 0xC57C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xC5E1 */
        if (CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0) && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
            CB->stop_joindre(-P_KALHI13);
        }
        ST(0x94) = ST(ST_LOCKED_CAMERA);
        /* 0xC627 */
        if (ST(0x94) == 0) {
            CB->change_camera(1, CAM_LOCKED);
        }
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC655 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L03D17A", P_ARKHAN1, "SG00DIA04", "Sar_10");
            lieu_action_register("L03D17B", P_ARKHAN1, "SG00DIA07", "Sar_10");
        }
        /* 0xC699 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L03D18", P_ARKHAN2, "SG00DIA04", "Sar_10");
        }
        /* 0xC6C4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L03D19", P_ARKHAN3, "SG00DIA04", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        /* 0xC700 */
        if (ST(0x94) == 0) {
            CB->change_camera(99, CAM_AUTO);
        }
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 41 — sub_10008051 — hero switch: replay the greeting lines */
static void trig41(void) {
    /* 0x806E */
    if (ST(V_SAVED_HERO) != ST(ST_HERO) && (ST(V_DELIA_PHASE) == 9 || (ST(0xE9C) == 1 && (ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1)))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x80DF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x80F1 */
            if (ST(V_DELIA_PHASE) == 9) {
                CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD48", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            }
            /* 0x8139 */
            if (ST(0xE9C) == 1 && ST(ST_VIE(P_KALHI13)) == 1) {
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD49", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
            }
            /* 0x8193 */
            if (ST(0xE9C) == 1 && ST(ST_VIE(P_KALHI33)) == 1) {
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD49", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
            }
            /* 0x81ED */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD50", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0x8215 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD51", P_ARKHAN2, "SG00DIA02", NULL);
            }
        }
        /* 0x823D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x824F */
            if (ST(V_DELIA_PHASE) == 9) {
                CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD44", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            }
            /* 0x8297 */
            if (ST(0xE9C) == 1 && ST(ST_VIE(P_KALHI13)) == 1) {
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD45", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
            }
            /* 0x82F1 */
            if (ST(0xE9C) == 1 && ST(ST_VIE(P_KALHI33)) == 1) {
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD45", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
            }
            /* 0x834B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD46", P_ARKHAN3, "SG00DIA08", NULL);
            }
            /* 0x8373 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD47", P_ARKHAN3, "SG00DIA08", NULL);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}
