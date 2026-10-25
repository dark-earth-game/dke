/*
 * lieu14_trig.c — Native hand-port of the LIEU14.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu14.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu14_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig02(void); static void trig03(void);
static void trig04(void); static void trig05(void);
static void trig06(void); static void trig07(void);
static void trig08(void); static void trig09(void);
static void trig11(void); static void trig12(void);
static void trig13(void); static void trig15(void);
static void trig16(void); static void trig17(void);
static void trig19(void); static void trig21(void);
static void trig23(void); static void trig24(void);
static void trig25(void); static void trig27(void);
static void trig29(void); static void trig30(void);
static void trig31(void); static void trig33(void);
static void trig34(void); static void trig35(void);
static void trig36(void); static void trig37(void);
static void trig38(void); static void trig41(void);
static void trig_noop(void);   /* the 11 stub slots */

/* dword_1002BE00[42] — filled at INIT with the 5-byte jmp thunks at
 * 0x10001000+5*i; static here.
 *   idx handler       idx handler       idx handler
 *    0  100073F0         14  1001172E stub      28  10013CE9 stub
 *    1  1000C4CA stub      15  10011739         29  10013CF4
 *    2  1000C4D5         16  10011814         30  10013E8F
 *    3  1000E149         17  10011A22         31  1001426A
 *    4  10010317         18  10012085 stub      32  1001438B stub
 *    5  10010673         19  10012090         33  10014396
 *    6  100107E4         20  1001219E stub      34  10014B70
 *    7  1001093F         21  100121A9         35  10014BAE
 *    8  10010AB3         22  100126A8 stub      36  10014CB2
 *    9  10011329         23  100126B3         37  10014D8D
 *   10  1001142D stub      24  1001289D         38  10014E68
 *   11  10011438         25  100139B0         39  10014FC8 stub
 *   12  1001150B         26  10013C16 stub      40  10014FD3 stub
 *   13  10011625         27  10013C21         41  10008C8E
 */
static void (*const g_trigger[42])(void) = {
    l14_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig09, trig_noop, trig11, trig12, trig13, trig_noop, trig15,
    trig16, trig17, trig_noop, trig19, trig_noop, trig21, trig_noop, trig23,
    trig24, trig25, trig_noop, trig27, trig_noop, trig29, trig30, trig31,
    trig_noop, trig33, trig34, trig35, trig36, trig37, trig38, trig_noop,
    trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1002BE00]). */
void l14_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — trig00_100073F0 — level setup: start-point presets, state reset, zones, objects, actor placement per entry room */
void l14_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(14)) == 1) {
        ST(0xBF4) = 0;
        ST(0xAF8) = 1;
        /* 0x7432 */
        /* 0x7432: empty in the DLL (both branches jump to 0x743D) */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
        }
        ST(0xE9C) = 10;
        ST(ST_HERO) = P_ARKHAN3;
        ST(ST_VIE(P_KALHI13)) = 1;
        /* 0x7473 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
        ST(0xEB8) = 2;
    }
    /* 0x74BE */
    if (ST(ST_START_POINT(14)) == 2) {
        ST(0xBF4) = 0;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(ST_HERO) = P_ARKHAN2;
        /* 0x7509 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
    }
    /* 0x7545 */
    if (ST(ST_START_POINT(14)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xBE8) = 1;
        /* 0x7572 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
    }
    /* 0x75AE */
    if (ST(ST_START_POINT(14)) == 4) {
        ST(0xBF4) = 0;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xBE8) = 1;
        ST(ST_LEVEL) = 19;
        ST(V_DELIA_PHASE) = 3;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0x188) = 1;
    }
    /* 0x7626 */
    if (ST(ST_START_POINT(14)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xC44) = 1;
        ST(ST_LEVEL) = 19;
        ST(ST_VIE(P_MALROT1)) = VIE_DEAD;
        ST(ST_VIE(P_MALROT4)) = VIE_DEAD;
    }
    /* 0x768F */
    if (ST(ST_START_POINT(14)) == 6) {
        ST(0xBF4) = 0;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_DELIA_PHASE) = 1;
        /* 0x76E9 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
    }
    ST(ST_START_POINT(14)) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    /* 0x774C */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x7770 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        ST(ST_VIE(P_GUARDIAN15)) = VIE_DEAD;
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
    ST(0x58) = 0;
    CB->vire_col(0x1B, 0);
    CB->vire_col(0x1C, 0);
    /* 0x7955 */
    if (ST(0x6CC) == 0) {
        CB->etat_objet("OB85", 6400, -2300, 1283, 0x4200, 0xE00, 0, -1, 4);
    }
    /* 0x7991 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->etat_objet("OBCHAR", 3322, -2268, 962, 0xBE00, 0x4200, 0x6200, -1, 4);
        CB->col_objet("OBCHAR", 3322, 962, 100, 100);
    }
    /* 0x79E9 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->etat_objet("OBCHAR", 3343, -2262, 959, 0xC600, 0xFE00, 0xE00, -1, 4);
        CB->col_objet("OBCHAR", 3322, 962, 100, 100);
        CB->ajoute_zone(38, 3315, 945, 275, 280, 0);
        CB->ajoute_zone(39, 3318, 957, 50, 50, 0);
    }
    CB->ajoute_zone(5, 3932, 2220, 85, 315, 1);
    CB->ajoute_zone(6, 5285, 855, 145, 70, 1);
    CB->ajoute_zone(7, 3044, 670, 105, 115, 1);
    CB->ajoute_zone(8, 4363, 816, 250, 225, 0);
    CB->ajoute_zone(9, 3096, 1157, 470, 135, 0);
    CB->ajoute_zone(10, 3175, 1115, 75, 60, 0);
    CB->ajoute_zone(11, 4400, 1800, 115, 165, 0);
    CB->ajoute_zone(12, 5499, 1976, 319, 405, 0);
    CB->ajoute_zone(13, 5968, 1916, 581, 523, 0);
    CB->ajoute_zone(14, 5950, 1886, 103, 98, 0);
    CB->ajoute_zone(15, 5261, 1970, 139, 360, 0);
    CB->ajoute_zone(16, 6425, 1327, 355, 125, 0);
    CB->ajoute_zone(17, 3752, 965, 410, 305, 0);
    CB->ajoute_zone(18, 3752, 965, 80, 55, 0);
    CB->ajoute_zone(19, 3491, 1349, 354, 379, 0);
    CB->ajoute_zone(20, 3472, 1320, 45, 50, 0);
    CB->ajoute_zone(21, 6110, 1017, 120, 155, 0);
    CB->ajoute_zone(23, 3246, 1036, 140, 85, 0);
    CB->ajoute_zone(25, 5083, 2000, 180, 370, 0);
    CB->ajoute_zone(26, 5097, 1955, 5, 5, 0);
    CB->ajoute_zone(27, 4843, 1894, 262, 160, 0);
    CB->ajoute_zone(30, 5982, 985, 105, 255, 0);
    CB->ajoute_zone(31, 5697, 789, 130, 330, 0);
    CB->ajoute_zone(35, 6362, 1191, 275, 105, 0);
    CB->ajoute_zone(36, 4280, 1792, 115, 160, 0);
    CB->ajoute_zone(37, 4519, 1798, 110, 160, 0);
    CB->place_animal(4, 5000, 0, 1400, 32768, 1024);
    CB->place_animal(4, 5000, 100, 1400, 65000, 1024);
    CB->place_animal(4, 5000, -100, 1400, 16000, 2048);
    CB->place_animal(1, 4000, 0, 1000, 0, 1024);
    /* 0x7E16 */
    if (ST(0xBF4) == 0) {
        /* 0x7E28 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->load_anim("214MOT01", 0);
            CB->load_anim("214ELI02", 1);
            CB->bruitage("L14AMB2", -1, -1);
        }
        /* 0x7E69 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->load_anim("114MOT01", 2);
            CB->load_anim("114ELI02", 3);
            CB->bruitage("L14AMB1", -1, -1);
        }
    }
    /* 0x7EAD */
    if (ST(0xBF4) == 1) {
        /* 0x7EBF */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->load_anim("214MOT01", 0);
            CB->load_anim("214ELI02", 1);
            CB->bruitage("L14AMB2", -1, -1);
        }
        /* 0x7F00 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->load_anim("114MOT01", 2);
            CB->load_anim("114ELI02", 3);
            CB->bruitage("L14AMB1", -1, -1);
        }
    }
    /* 0x7F44 */
    if (ST(0xBA8) == 5) {
        ST(0xBA8) = 6;
    }
    /* 0x7F65 */
    if (ST(0xBA8) == 6 && ST(ST_LEVEL) == 10 && ST(0xE14) == 1) {
        ST(0xE14) = 2;
    }
    /* 0x7FA7 */
    if (ST(ST_VIE(P_THANANDAR24)) == VIE_DEAD) {
        ST(ST_VIE(P_THANANDAR24)) = 4;
    }
    /* 0x7FC8 */
    if (ST(0xB38) == 2) {
        ST(0xB38) = 3;
    }
    /* 0x7FE9 */
    if (ST(ST_VIE(P_DHORKAN11)) == VIE_DEAD && ST(0xF78) == 0) {
        ST(0xF24) = 1;
    }
    /* 0x801C */
    if (ST(0xF28) == 3) {
        ST(0xF28) = 4;
    }
    /* 0x803D */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->ajoute_zone(28, 5728, 1135, 50, 50, 0);
        CB->ajoute_zone(29, 5739, 1120, 235, 285, 0);
    }
    /* 0x8088 */
    if (ST(ST_LEVEL) == 10) {
        /* 0x809A */
        if (ST(0xE9C) != 10) {
            CB->pos_perso(ST(ST_HERO), 4445, 852, 0x41A0, 1, "ar", 1, 0);
        }
        /* 0x80D7 */
        if (ST(0xE9C) == 10) {
            CB->pos_perso(ST(ST_HERO), 4573, 562, 0x518, 1, "ar", 1, 0);
        }
    }
    /* 0x8111 */
    if (ST(ST_LEVEL) == 15) {
        CB->pos_perso(ST(ST_HERO), 5330, 1008, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x815F */
    if (ST(ST_LEVEL) == 19) {
        CB->pos_perso(ST(ST_HERO), 3835, 1950, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x81AD */
    if (ST(ST_LEVEL) == 46) {
        CB->pos_perso(ST(ST_HERO), 3062, 923, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(0x188) = 1;
    }
    /* 0x820A */
    if (ST(ST_LEVEL) != 10 && ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
        CB->pos_perso(ST(ST_HERO), 4315, 1050, 0, 1, "ar", 1, 0);
    }
    /* 0x8271 */
    if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        CB->pos_perso(P_PROVIDER, 3267, 915, 0xCE17, 1, "P16ARR01", 3, 600);
    }
    /* 0x82BD */
    if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(0xBE8) >= 1 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
        CB->pos_perso(P_MALROT4, 5592, 1439, 0x2D9D, 1, "P64ARR01", 2, 1);
    }
    /* 0x8318 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4 && ST(0x170) != 1 && ST(0xC44) == 0) {
        CB->pos_perso(P_MALROT4, 5592, 1439, 0x2D9D, 1, "P64ARR01", 2, 1);
    }
    /* 0x83A9 */
    if (ST(0xC44) == 1 && ST(ST_VIE(35)) != VIE_DEAD) {
        CB->ajoute_zone(40, 4520, 701, 605, 613, 0);
        /* 0x83EE */
        if (ST(0x16C) == 0) {
            CB->pos_perso(35, 4626, 763, 0x16B, 1, "P35GAR01", 4, 1);
        }
        /* 0x8425 */
        if (ST(0x16C) == 1) {
            /* 0x8434 */
            if (ST(ST_LEVEL) == 19) {
                CB->pos_perso(35, 3925, 1289, 0xC350, 1, "P35GAR01", 4, 1);
                CB->combat(35, 0);
                ST(V_ENGAGED_PERSONA) = 35;
            }
            /* 0x8481 */
            if (ST(ST_LEVEL) == 46) {
                CB->pos_perso(35, 3714, 1178, 0x40C4, 1, "P35GAR01", 4, 1);
                CB->combat(35, 0);
                ST(V_ENGAGED_PERSONA) = 35;
            }
            /* 0x84CE */
            if (ST(ST_LEVEL) == 10) {
                CB->pos_perso(35, 4907, 1319, 0x38F0, 1, "P35GAR01", 4, 1);
                CB->combat(35, 0);
                ST(V_ENGAGED_PERSONA) = 35;
            }
            /* 0x851B */
            if (ST(ST_LEVEL) == 15) {
                CB->pos_perso(35, 5167, 1399, 0x86A8, 1, "P35GAR01", 4, 1);
                CB->combat(35, 0);
                ST(V_ENGAGED_PERSONA) = 35;
            }
        }
    }
    /* 0x856B */
    if (ST(0x4CC) == 1 && ST(0xC44) == 0) {
        /* 0x858F */
        if (ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4) {
            /* 0x85D4 */
            if (ST(ST_LEVEL) == 19) {
                CB->pos_perso(P_MALROT4, 3779, 1582, 0x48A, 1, "P64ARR01", 2, 1);
            }
            /* 0x8608 */
            if (ST(ST_LEVEL) == 10) {
                CB->pos_perso(P_MALROT4, 4566, 1406, 0x8146, 1, "P64ARR01", 2, 1);
            }
            /* 0x863C */
            if (ST(ST_LEVEL) == 15) {
                CB->pos_perso(P_MALROT4, 5677, 1396, 0x6CD3, 1, "P64ARR01", 2, 1);
            }
            /* 0x8670 */
            if (ST(ST_LEVEL) == 46) {
                CB->pos_perso(P_MALROT4, 3583, 1023, 0x3765, 1, "P64ARR01", 2, 1);
            }
        }
        /* 0x86A7 */
        if (ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) == 2) {
            /* 0x86C8 */
            if (ST(ST_LEVEL) == 19) {
                CB->pos_perso(P_MALROT1, 4055, 1626, 0x15D8, 1, "P61ARR01", 0x18, 0);
            }
            /* 0x86FC */
            if (ST(ST_LEVEL) == 10) {
                CB->pos_perso(P_MALROT1, 4242, 1413, 0x9008, 1, "P61ARR01", 0x18, 0);
            }
            /* 0x8730 */
            if (ST(ST_LEVEL) == 15) {
                CB->pos_perso(P_MALROT1, 5835, 1365, 0x682F, 1, "P61ARR01", 0x18, 0);
            }
            /* 0x8764 */
            if (ST(ST_LEVEL) == 46) {
                CB->pos_perso(P_MALROT1, 3580, 1205, 0x4D91, 1, "P61ARR01", 0x18, 0);
            }
        }
    }
    /* 0x879B */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xC44) == 0 && ST(0x4CC) == 0) {
        CB->pos_perso(P_MALROT1, 3649, 1268, 0xE98, 1, "P61ATT04", 0x18, 0);
        CB->evenement(2, 2000);
        CB->retire_zone(19);
    }
    /* 0x8811 */
    if (ST(0xE9C) == 10 && ST(0xBE8) == 0 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
        CB->vire_col(0x1A, 0);
        CB->evenement(0x28, 300000);
        ST(0xE9C) = 40;
        CB->pos_perso(P_KALHI13, 4321, 745, 0xB0B1, 1, "P13ARR01", 4, 0);
    }
    /* 0x8898 */
    if (ST(ST_VIE(P_MALROT4)) == VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2 && ST(0xC44) == 0) {
        /* 0x88CE */
        if (ST(0xCB8) == 0) {
            ST(0xCB8) = 1;
        }
        ST(0x4CC) = 0;
        CB->pos_perso(P_MALROT4, 0, 0, 0, 2, "P64cad", 2, 0);
    }
    /* 0x891A */
    if (ST(ST_VIE(P_PROVIDER)) == VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->pos_perso(P_PROVIDER, 0, 0, 0, 2, "P16cad", 3, 0);
        CB->ajoute_zone(38, 3315, 945, 275, 280, 0);
        CB->ajoute_zone(39, 3318, 957, 50, 50, 0);
    }
    /* 0x8996 */
    if (ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
        CB->pos_perso(P_MALROT1, 0, 0, 0, 2, "P61cad", 0x18, 0);
    }
    /* 0x89C4 */
    if (ST(ST_VIE(35)) == VIE_DEAD) {
        CB->pos_perso(35, 0, 0, 0, 2, "P35cad", 4, 0);
    }
    /* 0x89F2 */
    if (ST(0x188) == 1) {
        CB->vire_col(0x19, 0);
    }
    /* 0x8A11 */
    if (ST(V_DELIA_PHASE) == 3 && ST(ST_VIE(P_DELIA)) != VIE_DEAD) {
        CB->pos_perso(P_DELIA, 3756, 2073, 0x7BC3, 1, "P10ARR01", 0x21, 0);
        /* 0x8A5A */
        if (ST(0xB4C) == 0) {
            ST(0xB4C) = 1;
        }
    }
    /* 0x8A7B */
    if (ST(V_DELIA_PHASE) == 4) {
        CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 1, "P10ARR01", 0x21, 0);
    }
    /* 0x8ADD */
    if (ST(ST_VIE(P_DELIA)) == VIE_DEAD && ST(V_DELIA_KILLED) == 14 && ST(0xC44) == 0 && ST(0x170) == 1) {
        CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 2, "P10cad", 0x21, 0);
    }
    /* 0x8B75 */
    if (ST(0xEB8) == 2 && ST(0x538) == 0) {
        ST(0x538) = 1;
    }
    /* 0x8BA8 */
    if (ST(0xE24) == 2) {
        ST(0xE24) = 3;
    }
    /* 0x8BC9 */
    if (ST(0xEF8) == 2 || ST(0xEF8) == 3) {
        ST(0xEF8) = 10;
        /* 0x8BFC */
        if (ST(ST_VIE(29)) != VIE_DEAD) {
            ST(0xF24) = 5;
        }
    }
    /* 0x8C1D */
    if (ST(0xB10) != 0 && ST(0xE74) == 0) {
        ST(0xE74) = 1;
    }
    /* 0x8C50 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(ST_VIE(29)) == VIE_DEAD && ST(0xF24) == 6) {
        ST(0xF24) = 3;
    }
}

/* trigger 02 — trig02_1000C4D5 — zone 2: Malrot4 (first meeting, action-key conversations, strike/fight start) */
static void trig02(void) {
    /* 0xC4E7 */
    if (ST(0xCB4) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
        CB->select_pile(1, 2);
        ST(0xCB4) = 1;
        CB->evenement(1, -1);
        ST(V_EVENT_1) = 0;
        CB->animation("Sar_10");
        CB->stop_joindre(-P_MALROT4);
        CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
        CB->joindre(-P_MALROT4, 0, 0, 0, ST(ST_HERO));
        CB->modify_var(&ST(0xCB8), 1);
        CB->select_pile(0, 1);
    }
    /* 0xC5BE */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(0xCB8) == 1 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_MALROT4)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xC639 */
        if (ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
            CB->stop_joindre(-P_MALROT4);
            CB->joindre(P_MALROT4, 0, 0, 0, ST(ST_HERO));
            CB->joindre(ST(ST_HERO), 0, 0, 0, P_MALROT4);
            /* 0xC68C */
            if (ST(V_GUARD_FIGHT_DONE) != 2) {
                /* 0xC69E */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xC6B0 */
                    if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                        /* 0xC6EE */
                        if (ST(0xCC0) == 0) {
                            /* 0xC700 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L14D05", P_ARKHAN2, "SG00DIA02", NULL);
                                lieu_action_register("L14D07A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D07B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D177", P_ARKHAN2, "SG00DIA02", NULL);
                                lieu_action_register("L14D178A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D178B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D178C", P_MALROT4, "SP64ATT01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xCC0) = 1;
                                return;
                            }
                            /* 0xC7F3 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L14D08", P_ARKHAN2, "SG00DIA02", NULL);
                                lieu_action_register("L14D09A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D09B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D179A", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L14D179B", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L14D180", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D178A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D178B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D178C", P_MALROT4, "SP64ATT01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xCC0) = 1;
                                return;
                            }
                        }
                        /* 0xC915 */
                        if (ST(0xCC0) == 1) {
                            /* 0xC927 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L14D10", P_ARKHAN2, "SG00DIA06", NULL);
                                lieu_action_register("L14D11A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D11B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D11C", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xCC0) = 2;
                                return;
                            }
                            /* 0xC9D2 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L14D12A", P_ARKHAN2, "SG00DIA05", NULL);
                                lieu_action_register("L14D12B", P_ARKHAN2, "SG00DIA05", NULL);
                                lieu_action_register("L14D12C", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L14D13A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D13B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0x4CC) = 1;
                                CB->combat(P_MALROT4, 0);
                                ST(V_ENGAGED_PERSONA) = P_MALROT4;
                                return;
                            }
                        }
                        /* 0xCAA9 */
                        if (ST(0xCC0) == 2 && ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D34A", P_ARKHAN2, "SG00DIA02", NULL);
                            lieu_action_register("L14D34B", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L14D35A", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D35B", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D35C", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D35D", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xCC0) = 3;
                            return;
                        }
                        /* 0xCB95 */
                        if (ST(0xCC0) == 3 && ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D36A", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L14D37A", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D37B", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D36B", P_ARKHAN2, "SG00DIA02", NULL);
                            lieu_action_register("L14D37C", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xCC0) = 4;
                            return;
                        }
                        /* 0xCC68 */
                        if (ST(0xCC0) == 4 && ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D38", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L14D39A", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D39B", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D39C", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D181", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L14D182A", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D182B", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D182C", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            lieu_action_register("L14D183", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L14D184", P_MALROT4, "SP64DIA02", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xCC0) = 5;
                            return;
                        }
                        /* 0xCDB5 */
                        if (ST(0xCC0) >= 5 && ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D40", P_ARKHAN2, "SG00DIA02", NULL);
                            lieu_action_register("L14D41A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            /* 0xCE08 */
                            if (ST(0xCC0) == 5) {
                                lieu_action_register("L14D41B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                ST(0xCC0) = 6;
                            }
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xCE6B */
                        if (ST(0xCC0) >= 2 && ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D42", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L14D43", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0x4CC) = 1;
                            CB->combat(P_MALROT4, 0);
                            ST(V_ENGAGED_PERSONA) = P_MALROT4;
                            return;
                        }
                    }
                    /* 0xCF14 */
                    if (CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        /* 0xCF26 */
                        if (ST(0xCBC) == 0) {
                            lieu_action_register("L14D00", P_ARKHAN2, "SG00DIA02", NULL);
                            lieu_action_register("L14D01A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D01B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xCBC) = 1;
                            return;
                        }
                        /* 0xCFB8 */
                        if (ST(0xCBC) == 1) {
                            /* 0xCFCA */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L14D04", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L14D06", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xCBC) = 2;
                                return;
                            }
                            /* 0xD043 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L14D02", P_ARKHAN2, "SG00DIA05", NULL);
                                lieu_action_register("L14D03", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                ST(0x4CC) = 1;
                                CB->combat(P_MALROT4, 0);
                                ST(V_ENGAGED_PERSONA) = P_MALROT4;
                                CB->bloque(BLOQUE_NONE);
                                ST(0xCBC) = 2;
                                return;
                            }
                        }
                        /* 0xD0E4 */
                        if (ST(0xCBC) == 2) {
                            /* 0xD0F6 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L14D13", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                ST(V_EVENT_1) = 0;
                                ST(V_ENGAGED_PERSONA) = P_MALROT4;
                                ST(0x4CC) = 1;
                                CB->combat(P_MALROT4, 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xD181 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L14D02", P_ARKHAN2, "SG00DIA05", NULL);
                                lieu_action_register("L14D03", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                ST(0x4CC) = 1;
                                ST(V_ENGAGED_PERSONA) = P_MALROT4;
                                CB->combat(P_MALROT4, 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                        }
                    }
                    /* 0xD213 */
                    if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        lieu_action_register("L14D44", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D45A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        /* 0xD27F */
                        if (ST(0xF8) == 0) {
                            ST(0xF8) = 1;
                            lieu_action_register("L14D45B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xD2E2 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xD2F4 */
                    if (ST(0xDEC) == 0) {
                        /* 0xD306 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D18", P_ARKHAN3, "SG00DIA07", NULL);
                            lieu_action_register("L14D19A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D19B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xDEC) = 1;
                            return;
                        }
                        /* 0xD398 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->dialogue("LCOD22A", P_ARKHAN3);
                            lieu_action_register("L14D15", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xDEC) = 1;
                            return;
                        }
                    }
                    /* 0xD40B */
                    if (ST(0xDEC) == 1) {
                        /* 0xD41D */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D20", P_ARKHAN3, "SG00DIA07", NULL);
                            lieu_action_register("L14D21A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D21B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        }
                        /* 0xD477 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->dialogue("LCOD22B", P_ARKHAN3);
                            lieu_action_register("L14D185", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x4CC) = 1;
                        CB->combat(P_MALROT4, 0);
                        ST(V_ENGAGED_PERSONA) = P_MALROT4;
                        return;
                    }
                }
            }
            /* 0xD503 */
            if (ST(V_GUARD_FIGHT_DONE) == 2) {
                /* 0xD515 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xD527 */
                    if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                        /* 0xD57C */
                        if (ST(0xDDC) == 0) {
                            /* 0xD58E */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L14D05", P_ARKHAN2, "SG00DIA02", NULL);
                                lieu_action_register("L14D206A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D206B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D207A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D207B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xDDC) = 1;
                                return;
                            }
                            /* 0xD652 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L14D214", P_ARKHAN2, "SG00DIA05", NULL);
                                lieu_action_register("L14D223", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D207A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                lieu_action_register("L14D207B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xDDC) = 1;
                                return;
                            }
                        }
                        /* 0xD6FD */
                        if (ST(0xDDC) == 1 && ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D208", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L14D209", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D210A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D210B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D210C", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D210D", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D213", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xDDC) = 2;
                            return;
                        }
                        /* 0xD805 */
                        if (ST(0xDDC) == 2 && ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D211", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L14D212", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xD881 */
                        if (ST(0xDDC) >= 1 && ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D215", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L14D03", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0x4CC) = 1;
                            CB->combat(P_MALROT4, 0);
                            ST(V_ENGAGED_PERSONA) = P_MALROT4;
                            /* 0xD920 */
                            if (ST(ST_VIE(P_MALROT1)) == 1) {
                                CB->select_pile(2, 2);
                                CB->evenement(2, -1);
                                ST(V_EVENT_1) = 0;
                                ST(V_ENGAGED_PERSONA) = 0x1814;
                                CB->combat(P_MALROT1, 0);
                                CB->select_pile(0, 1);
                            }
                            return;
                        }
                    }
                    /* 0xD98C */
                    if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        lieu_action_register("L14D44", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D45A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        lieu_action_register("L14D45B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xDA3A */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xDA4C */
                    if (ST(0xDF4) == 0 && ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L14D217", P_ARKHAN3, "SG00DIA02", NULL);
                        lieu_action_register("L14D218A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        lieu_action_register("L14D218B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xDF4) = 1;
                        return;
                    }
                    /* 0xDAF0 */
                    if (ST(0xDF4) == 1 && ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L14D219", P_ARKHAN3, "SG00DIA07", NULL);
                        lieu_action_register("L14D220", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xDB6C */
                    if (ST(0xDF4) >= 0 && ST(ST_SPECIAL_MODE) == 0) {
                        CB->dialogue("LCOD22A", P_ARKHAN3);
                        lieu_action_register("L14D185", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x4CC) = 1;
                        CB->combat(P_MALROT4, 0);
                        ST(V_ENGAGED_PERSONA) = P_MALROT4;
                        /* 0xDC05 */
                        if (ST(ST_VIE(P_MALROT1)) == 1) {
                            CB->select_pile(2, 2);
                            CB->evenement(2, -1);
                            ST(V_EVENT_1) = 0;
                            ST(V_ENGAGED_PERSONA) = 0x1814;
                            CB->combat(P_MALROT1, 0);
                            CB->select_pile(0, 1);
                        }
                        return;
                    }
                }
            }
        }
        /* 0xDC71 */
        if (ST(ST_VIE(P_MALROT4)) == VIE_DEAD) {
            /* 0xDC83 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00PRE09");
                CB->animation("ar_10");
                CB->dialogue("L14D165", P_ARKHAN2);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->retire_zone(2);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xDCEF */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG00PRE09");
                CB->animation("ar_10");
                CB->dialogue("L14D166", P_ARKHAN3);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->retire_zone(2);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xDD58 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) != P_MALROT4 && ST(V_ENGAGED_PERSONA) != 0x1910 && ST(V_ENGAGED_PERSONA) != 0x1814 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0) {
        /* 0xDDBB */
        if (ST(V_ENGAGED_PERSONA) == 0) {
            CB->select_pile(1, 1);
            CB->evenement(1, -1);
            ST(V_PLAYER_STRIKE) = -1;
            ST(V_EVENT_1) = 0;
            ST(V_ENGAGED_PERSONA) = P_MALROT4;
            ST(0x4CC) = 1;
            CB->combat(P_MALROT4, 0);
            CB->select_pile(0, 1);
            /* 0xDE3D */
            if (ST(ST_VIE(P_MALROT1)) == 1) {
                CB->select_pile(2, 2);
                CB->evenement(2, -1);
                ST(V_EVENT_1) = 0;
                ST(V_ENGAGED_PERSONA) = 0x1814;
                CB->combat(P_MALROT1, 0);
                CB->select_pile(0, 1);
            }
        }
        /* 0xDEA1 */
        if (ST(V_ENGAGED_PERSONA) == P_PROVIDER) {
            CB->select_pile(1, 1);
            CB->stop_joindre(-P_MALROT4);
            CB->evenement(1, -1);
            ST(V_PLAYER_STRIKE) = -1;
            ST(V_EVENT_1) = 0;
            ST(V_ENGAGED_PERSONA) = 0x1910;
            CB->combat(P_MALROT4, 0);
            ST(0x4CC) = 1;
            CB->select_pile(0, 1);
        }
        /* 0xDF2B */
        if (ST(V_ENGAGED_PERSONA) == P_MALROT1) {
            CB->select_pile(1, 1);
            CB->stop_joindre(-P_MALROT4);
            CB->evenement(1, -1);
            ST(V_PLAYER_STRIKE) = -1;
            ST(V_EVENT_1) = 0;
            ST(V_ENGAGED_PERSONA) = 0x1814;
            CB->combat(P_MALROT4, 0);
            ST(0x4CC) = 1;
            CB->select_pile(0, 1);
        }
    }
    /* 0xDFB5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE01D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD20", P_ARKHAN2);
        }
        /* 0xE03F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xE08A */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && CB->direction_perso(P_MALROT4)) {
        ST(V_CRI_USED) = 1;
        ST(0x4CC) = 1;
        /* 0xE10F */
        if (ST(0xDC) != 99) {
            ST(0xDC) = 64;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0x21, 1000);
        return;
    }
}

/* trigger 03 — trig03_1000E149 */
static void trig03(void) {
    /* 0xE158 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x63A) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && !CB->direction_perso(P_PROVIDER)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xE1D9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xE204 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xE253 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_PROVIDER)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_PROVIDER);
        /* 0xE2C7 */
        if (ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
            CB->joindre(P_PROVIDER, 0, 0, 0, ST(ST_HERO));
            CB->joindre(ST(ST_HERO), 0, 0, 0, P_PROVIDER);
            /* 0xE30F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xE321 */
                if (ST(ST_PLAYER_WEAPON) == 0 && ST(0x538) != 1) {
                    /* 0xE345 */
                    if (ST(0xCCC) == 0) {
                        /* 0xE357 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D49", P_ARKHAN2, "SG00DIA02", NULL);
                        }
                        /* 0xE37F */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D59", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                        lieu_action_register("L14D50A", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        lieu_action_register("L14D50B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D50C", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        /* 0xE3F2 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D186", P_ARKHAN2, "SG00DIA03", NULL);
                        }
                        /* 0xE41A */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D187", P_ARKHAN3, "SG00DIA03", NULL);
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCCC) = 1;
                        return;
                    }
                    /* 0xE47A */
                    if (ST(0xCCC) == 1) {
                        /* 0xE48C */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D51", P_ARKHAN2, "SG00DIA07", NULL);
                        }
                        /* 0xE4B4 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D60", P_ARKHAN2, "SG00DIA07", NULL);
                        }
                        lieu_action_register("L14D52A", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D52B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xBE0) = 1;
                        ST(0xCCC) = 2;
                        return;
                    }
                    /* 0xE555 */
                    if (ST(0xCCC) == 2) {
                        /* 0xE567 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D53", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                        /* 0xE58F */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D61", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                        lieu_action_register("L14D54A", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D54B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        lieu_action_register("L14D54C", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        lieu_action_register("L14D54D", P_PROVIDER, "SP16DIA53", "SP16ARR01");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCCC) = 3;
                        return;
                    }
                    /* 0xE653 */
                    if (ST(0xCCC) == 3) {
                        lieu_action_register("L14D55", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D56A", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D56B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCCC) = 4;
                        return;
                    }
                    /* 0xE6E5 */
                    if (ST(0xCCC) == 4) {
                        /* 0xE6F7 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D57", P_ARKHAN2, "SG00DIA03", NULL);
                            lieu_action_register("L14D58", P_PROVIDER, "SP16DIA54", "SP16ARR01");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xE761 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D63", P_ARKHAN2, "SG00DIA03", NULL);
                            lieu_action_register("L14D188A", P_PROVIDER, "SP16DIA59", "SP16ARR01");
                            lieu_action_register("L14D188B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }
                /* 0xE7E4 */
                if (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    /* 0xE80D */
                    if (ST(0xCC4) == 0) {
                        lieu_action_register("L14D28", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D29", P_PROVIDER, "SP16DIA60", "SP16ARR01");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCC4) = 1;
                        return;
                    }
                    /* 0xE886 */
                    if (ST(0xCC4) == 1) {
                        lieu_action_register("L14D30", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D31A", P_PROVIDER, "SP16DIA60", "SP16ARR01");
                        lieu_action_register("L14D31B", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xE909 */
                if (ST(ST_PLAYER_WEAPON) != 0 && ST(0xBE0) == 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    lieu_action_register("L14D77", P_ARKHAN2, "SG00DIA04", NULL);
                    lieu_action_register("L14D78A", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                    lieu_action_register("L14D78B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                    /* 0xE98C */
                    if (ST(0xCCC) == 0) {
                        ST(0xCCC) = 1;
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xE9D6 */
                if (ST(ST_PLAYER_WEAPON) != 0 && ST(0xBE0) == 1 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    ST(0xCCC) = 4;
                    /* 0xEA20 */
                    if (ST(ST_PLAYER_WEAPON) == 20) {
                        lieu_action_register("L14D74A", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D74B", P_ARKHAN2, "SG00DIA07", NULL);
                        lieu_action_register("L14D152A", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(0x10, "OB99", 0x1B, 1);
                        lieu_action_register("L14D152B", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        lieu_action_register("L14D152D", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D152E", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA02", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 20, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0xEB34 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0xEB77 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xEBBD */
                    if (ST(ST_PLAYER_WEAPON) == 22) {
                        lieu_action_register("L14D82", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D83A", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(0x10, "OB44C", 0x1B, 1);
                        lieu_action_register("L14D83B", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA02", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 22, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0xEC89 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0xECCC */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xED12 */
                    if (ST(ST_PLAYER_WEAPON) == 42) {
                        lieu_action_register("L14D85", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D86", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(0x10, "OB43C", 0x1B, 1);
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA02", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 42, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0xEDC5 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0xEE08 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xEE4E */
                    if (ST(ST_PLAYER_WEAPON) == 95) {
                        lieu_action_register("L14D225", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(0x10, "OB36", 0x1B, 1);
                        lieu_action_register("L14D226A", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        lieu_action_register("L14D226B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 95, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0xEF1A */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0xEF5D */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xEFA3 */
                    if (ST(ST_PLAYER_WEAPON) == 4) {
                        lieu_action_register("L14D227", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D228A", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(0x10, "OB13", 0x1B, 1);
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        lieu_action_register("L14D228B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 4, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0xF06F */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0xF0B2 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xF0F8 */
                    if (ST(ST_PLAYER_WEAPON) == 94) {
                        lieu_action_register("L14D229", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D230A", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(0x10, "OB34", 0x1B, 1);
                        lieu_action_register("L14D230B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 94, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0xF1C4 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0xF207 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xF24D */
                    if (ST(ST_PLAYER_WEAPON) == 65) {
                        lieu_action_register("L14D231", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D232", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(0x10, "OB64C", 0x1B, 1);
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 65, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0xF300 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0xF343 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xF389 */
                    if (ST(ST_PLAYER_WEAPON) == 31) {
                        lieu_action_register("L14D233", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D234", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        ST(0x154) = 3;
                        CB->link_objet(0x10, "OB24F", 0x1B, 1);
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 31, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0xF44B */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0xF48E */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xF4D4 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != 22 && ST(ST_PLAYER_WEAPON) != 4 && ST(ST_PLAYER_WEAPON) != 42 && ST(ST_PLAYER_WEAPON) != 31 && ST(ST_PLAYER_WEAPON) != 20 && ST(ST_PLAYER_WEAPON) != 62 && ST(ST_PLAYER_WEAPON) != 94 && ST(ST_PLAYER_WEAPON) != 95) {
                    ST(0xCCC) = 4;
                    lieu_action_register("L14D157", P_ARKHAN2, "SG00DIA05", NULL);
                    ST(V_DLG_RANDOM) = CB->random(3);
                    /* 0xF5C9 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L14D191", P_PROVIDER, "SP16DIA59", "SP16ARR01");
                    }
                    /* 0xF5F4 */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L14D192", P_PROVIDER, "SP16DIA60", "SP16ARR01");
                    }
                    /* 0xF61F */
                    if (ST(V_DLG_RANDOM) == 2) {
                        lieu_action_register("L14D193", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xF673 */
                if (ST(0x538) == 1) {
                    /* 0xF685 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L14D243", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D244A", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        lieu_action_register("L14D244B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        lieu_action_register("L14D245", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D246", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D247", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x538) = 0;
                        return;
                    }
                    /* 0xF75C */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L14D248", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D244A", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        lieu_action_register("L14D244B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        lieu_action_register("L14D249", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D250", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D251", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D252", P_PROVIDER, "SP16DIA59", "SP16ARR01");
                        lieu_action_register("L14D253", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x538) = 0;
                        return;
                    }
                }
            }
            /* 0xF862 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xF874 */
                if (ST(0xDF0) == 0 && ST(0x538) != 1 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L14D64", P_ARKHAN3, "SG00DIA07", NULL);
                    lieu_action_register("L14D65", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xDF0) = 1;
                    return;
                }
                /* 0xF911 */
                if (ST(0xDF0) == 1 && ST(0x538) != 1 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L14D66", P_ARKHAN3, "SG00DIA04", NULL);
                    lieu_action_register("L14D67A", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                    CB->animation("SP16PRE01");
                    CB->animation("P16ARR01");
                    CB->link_objet(0x10, "OB99", 0x1B, 1);
                    /* 0xF9A0 */
                    if (ST(ST_PLAYER_WEAPON) != 0) {
                        /* 0xF9B2 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SP02PRE73");
                            CB->animation("ar");
                        }
                        /* 0xF9DA */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SP03PRE73");
                            CB->animation("ar");
                        }
                    }
                    lieu_pre_scene(P_ARKHAN2, P_PROVIDER, 99, 1, 3);
                    lieu_action_register("L14D67B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                    lieu_action_register("L14D67C", P_PROVIDER, "SP16DIA64", "SP16ARR01");
                    lieu_action_register("L14D67D", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xDF0) = 2;
                    return;
                }
                /* 0xFA9A */
                if (ST(0xDF0) == 2 && ST(0x538) != 1 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L14D68", P_ARKHAN3, "SG00DIA07", NULL);
                    lieu_action_register("L14D69", P_PROVIDER, "SP16DIA64", "SP16ARR01");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xFB28 */
                if (ST(0xDF0) >= 0 && ST(ST_SPECIAL_MODE) == 0 && ST(0x538) >= 0) {
                    CB->dialogue("LCOD22C", P_ARKHAN3);
                    lieu_action_register("L14D48", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->combat(P_PROVIDER, 1);
                    ST(0x4C8) = 1;
                    ST(0x4CC) = 1;
                    ST(V_ENGAGED_PERSONA) = P_PROVIDER;
                    /* 0xFBDD */
                    if (ST(ST_VIE(P_MALROT4)) == 1 && ST(V_GUARD_FIGHT_DONE) != 2) {
                        CB->select_pile(1, 1);
                        CB->evenement(1, -1);
                        CB->animation("Sar_10");
                        ST(V_EVENT_1) = 0;
                        CB->stop_joindre(-P_MALROT4);
                        CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
                        CB->animation("P64ARR01_10");
                        CB->modify_var(&ST(0xCB8), 1);
                        ST(V_ENGAGED_PERSONA) = 0x1910;
                        CB->combat(P_MALROT4, 0);
                        CB->select_pile(0, 1);
                    }
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xFCAD */
                if (ST(0x538) == 1) {
                    /* 0xFCBF */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L14D254", P_ARKHAN3, "SG00DIA05", NULL);
                        lieu_action_register("L14D244A", P_PROVIDER, "SP16DIA60", "SP16ARR01");
                        lieu_action_register("L14D244B", P_PROVIDER, "SP16DIA59", "SP16ARR01");
                        lieu_action_register("L14D255", P_ARKHAN3, "SG00DIA05", NULL);
                        lieu_action_register("L14D250", P_PROVIDER, "SP16DIA60", "SP16ARR01");
                        lieu_action_register("L14D256", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D252", P_PROVIDER, "SP16DIA59", "SP16ARR01");
                        lieu_action_register("L14D257", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x538) = 0;
                        return;
                    }
                    /* 0xFDC5 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->dialogue("LCOD22C", P_ARKHAN3);
                        lieu_action_register("L14D48", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->combat(P_PROVIDER, 1);
                        ST(0x4C8) = 1;
                        ST(0x4CC) = 1;
                        ST(V_ENGAGED_PERSONA) = P_PROVIDER;
                        /* 0xFE56 */
                        if (ST(ST_VIE(P_MALROT4)) == 1 && ST(V_GUARD_FIGHT_DONE) != 2) {
                            CB->select_pile(1, 1);
                            CB->evenement(1, -1);
                            CB->animation("Sar_10");
                            ST(V_EVENT_1) = 0;
                            CB->stop_joindre(-P_MALROT4);
                            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
                            CB->animation("P64ARR01_10");
                            CB->modify_var(&ST(0xCB8), 1);
                            ST(V_ENGAGED_PERSONA) = 0x1814;
                            CB->combat(P_MALROT4, 0);
                            CB->select_pile(0, 1);
                        }
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
        }
        /* 0xFF26 */
        if (ST(ST_VIE(P_PROVIDER)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
            /* 0xFF47 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->fouille(P_PROVIDER);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            }
            /* 0xFF75 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->fouille(P_PROVIDER);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            }
        }
    }
    /* 0xFFA0 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) != P_PROVIDER && ST(V_ENGAGED_PERSONA) != 0x1910 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(0x4C8) = 1;
        /* 0xC */
        if (ST(V_ENGAGED_PERSONA) == 0) {
            ST(V_ENGAGED_PERSONA) = P_PROVIDER;
            CB->combat(P_PROVIDER, 0);
        }
        /* 0x34 */
        if (ST(V_ENGAGED_PERSONA) == P_MALROT4) {
            ST(V_ENGAGED_PERSONA) = 0x1910;
            CB->combat(P_PROVIDER, 0);
        }
    }
    /* 0x5C */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == P_MALROT4 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xB5 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L14D149", P_ARKHAN2, "SG00DIA05", NULL);
            lieu_action_register("L14D150", P_PROVIDER, "SP16DIA60", "SP16ARR01");
        }
        /* 0xF6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L14D189", P_ARKHAN3, "SG00DIA05", NULL);
            lieu_action_register("L14D190", P_PROVIDER, "SP16DIA60", "SP16ARR01");
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x15D */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == P_PROVIDER && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x1B6 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x1C8 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0x1EA */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L14D194", P_ARKHAN2);
            }
            CB->dialogue("L14D195", P_PROVIDER);
        }
        /* 0x21C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x267 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD && CB->direction_perso(P_PROVIDER)) {
        ST(V_CRI_USED) = 1;
        /* 0x2DD */
        if (ST(0xE0) != 99) {
            ST(0xE0) = 16;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0x22, 1000);
        return;
    }
}

/* trigger 04 — trig04_10010317 — zone 4: forwards to trigger 5 inside zone 5, else the save spot (LCOD53) */
static void trig04(void) {
    /* 0x337 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)) {
        trig05();
        return;
    }
    /* 0x350 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x63A) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && !CB->direction_perso(0x23)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x3D1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0x3FC */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x44B */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 8) && !CB->direction_perso(0x23)) {
        trig08();
        return;
    }
    /* 0x497 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 35 && ST(ST_VIE(35)) != VIE_DEAD && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(0x23)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x512 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD62", P_ARKHAN2);
        }
        /* 0x534 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD22A", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x57C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(35)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(0x23)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->retire_zone(4);
        CB->animation("SG00PRE09");
        CB->animation("ar_10");
        /* 0x618 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD76", P_ARKHAN2);
        }
        /* 0x63A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD85", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 05 — trig05_10010673 */
static void trig05(void) {
    /* 0x685 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0x6A9 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    /* 0x6CD */
    if (ST(V_DELIA_PHASE) == 4 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(V_SAVED_HERO) = ST(ST_HERO);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x71C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L03D01", P_ARKHAN2);
        }
        /* 0x73E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L03D02", P_ARKHAN3);
        }
        ST(ST_LEVEL) = 14;
        CB->change_piece(19, 0);
        return;
    }
    /* 0x77E */
    if (ST(0xE34) == 1) {
        ST(0xE34) = 0;
    }
    /* 0x79F */
    if (ST(0xBE8) == 3) {
        ST(0xBE8) = 4;
    }
    ST(ST_LEVEL) = 14;
    /* 0x7C9 */
    if (ST(ST_SCRIPT_BUSY) == 0) {
        CB->change_piece(19, -1);
    }
}

/* trigger 06 — trig06_100107E4 */
static void trig06(void) {
    /* 0x7F6 */
    if (ST(V_DELIA_PHASE) == 4 && ST(0xD8) == 0) {
        ST(V_SAVED_HERO) = ST(ST_HERO);
        ST(0xD8) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x857 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L03D01", P_ARKHAN2);
        }
        /* 0x879 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L03D02", P_ARKHAN3);
        }
        ST(ST_LEVEL) = 14;
        CB->change_piece(15, -1);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x8C4 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0x8E8 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 14;
    ST(V_EVENT_1) = 0;
    /* 0x924 */
    if (ST(ST_SCRIPT_BUSY) == 0) {
        CB->change_piece(15, -1);
    }
}

/* trigger 07 — trig07_1001093F */
static void trig07(void) {
    /* 0x951 */
    if (ST(0xE34) == 1) {
        ST(0xE34) = 0;
    }
    /* 0x972 */
    if (ST(V_DELIA_PHASE) == 4 && ST(0x114) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0x114) = 1;
        ST(V_SAVED_HERO) = ST(ST_HERO);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9D3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L03D01", P_ARKHAN2);
        }
        /* 0x9F5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L03D02", P_ARKHAN3);
        }
        ST(ST_LEVEL) = 14;
        CB->change_piece(46, -1);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xA3B */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0xA5F */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 14;
    /* 0xA8C */
    if (ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->change_piece(46, -1);
    }
}

/* trigger 08 — trig08_10010AB3 */
static void trig08(void) {
    /* 0xACD */
    if (!CB->divers(DV_PERSONA_PRESENT, 35, 0)) {
        /* 0xADC */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (ST(V_ENGAGED_PERSONA) == 0 || ST(V_ENGAGED_PERSONA) == P_PROVIDER) && CB->direction_spec(0x494F)) {
            CB->bloque(BLOQUE_ALL);
            CB->evenement(2, -1);
            /* 0xB51 */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0xB63 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02lev07");
                }
                /* 0xB80 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03lev07");
                }
            }
            /* 0xBA5 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0xBB7 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02lev07");
                }
                /* 0xBD4 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03lev07");
                }
            }
            CB->animation("ar");
            /* 0xBFC */
            if (ST(0xBBC) == 1) {
                ST(0xBBC) = 2;
            }
            /* 0xC1D */
            if (ST(0xBBC) == 4 && ST(ST_VIE(P_MALROT5)) != VIE_DEAD) {
                ST(0xBBC) = 5;
            }
            /* 0xC50 */
            if (ST(0xBBC) == 4 && ST(ST_VIE(P_MALROT5)) == VIE_DEAD) {
                ST(0xBBC) = 8;
            }
            /* 0xC83 */
            if (ST(0xBBC) == 7) {
                ST(0xBBC) = 8;
            }
            /* 0xCA4 */
            if (ST(ST_VIE(P_DANRYS)) == VIE_DEAD && ST(V_KALHI13_TALK_C) == 3 && ST(0xC34) == 1) {
                ST(0xBBC) = 9;
            }
            /* 0xCE9 */
            if (ST(0xE34) == 1) {
                ST(0xE34) = 0;
            }
            /* 0xD0A */
            if (ST(0xBE8) == 3) {
                ST(0xBE8) = 4;
            }
            /* 0xD2B */
            if (ST(V_GUARD_FIGHT_DONE) != 2) {
                CB->divers(DV_MUSIC, 1, 4);
            }
            /* 0xD4F */
            if (ST(V_GUARD_FIGHT_DONE) == 2) {
                CB->divers(DV_MUSIC, 2, 4);
            }
            /* 0xD73 */
            if (ST(V_DELIA_PHASE) == 4) {
                ST(V_DELIA_PHASE) = 5;
            }
            /* 0xD94 */
            if (ST(ST_VIE(P_KONKALITE9)) == VIE_DEAD) {
                ST(ST_VIE(P_KONKALITE9)) = 4;
            }
            ST(ST_LEVEL) = 14;
            /* 0xDC1 */
            if (ST(0xB4C) == 0) {
                CB->change_piece(10, 1);
            }
            /* 0xDE3 */
            if (ST(0xB4C) == 1) {
                CB->change_piece(10, 2);
            }
            /* 0xE05 */
            if (ST(0xB4C) == 2) {
                CB->change_piece(10, 3);
            }
        }
        /* 0xE24 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(V_ENGAGED_PERSONA) != P_PROVIDER && CB->direction_spec(0x494F)) {
            CB->bloque(BLOQUE_ALL);
            /* 0xE81 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD32", P_ARKHAN2);
            }
            /* 0xEA3 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD33", P_ARKHAN3);
            }
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        return;
    }
    /* 0xEF0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (ST(ST_VIE(35)) == VIE_DEAD || (ST(ST_VIE(35)) != VIE_DEAD && !CB->divers(DV_IN_ZONE, 35, 40))) && CB->direction_spec(0x494F)) {
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        /* 0xF85 */
        if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
            /* 0xF97 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02lev07");
            }
            /* 0xFB4 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03lev07");
            }
        }
        /* 0xFD9 */
        if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
            /* 0xFEB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02lev07");
            }
            /* 0x1008 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03lev07");
            }
        }
        CB->animation("ar");
        /* 0x1030 */
        if (ST(0xBBC) == 1) {
            ST(0xBBC) = 2;
        }
        /* 0x1051 */
        if (ST(0xBBC) == 4 && ST(ST_VIE(P_MALROT5)) != VIE_DEAD) {
            ST(0xBBC) = 5;
        }
        /* 0x1084 */
        if (ST(0xBBC) == 4 && ST(ST_VIE(P_MALROT5)) == VIE_DEAD) {
            ST(0xBBC) = 8;
        }
        /* 0x10B7 */
        if (ST(0xBBC) == 7) {
            ST(0xBBC) = 8;
        }
        /* 0x10D8 */
        if (ST(ST_VIE(P_DANRYS)) == VIE_DEAD && ST(V_KALHI13_TALK_C) == 3 && ST(0xC34) == 1) {
            ST(0xBBC) = 9;
        }
        /* 0x111D */
        if (ST(0xE34) == 1) {
            ST(0xE34) = 0;
        }
        /* 0x113E */
        if (ST(0xBE8) == 3) {
            ST(0xBE8) = 4;
        }
        /* 0x115F */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
            CB->divers(DV_MUSIC, 1, 4);
        }
        /* 0x1183 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->divers(DV_MUSIC, 2, 4);
        }
        /* 0x11A7 */
        if (ST(V_DELIA_PHASE) == 4) {
            ST(V_DELIA_PHASE) = 5;
        }
        /* 0x11C8 */
        if (ST(ST_VIE(P_KONKALITE9)) == VIE_DEAD) {
            ST(ST_VIE(P_KONKALITE9)) = 4;
        }
        ST(ST_LEVEL) = 14;
        /* 0x11F5 */
        if (ST(0xB4C) == 0) {
            CB->change_piece(10, 1);
        }
        /* 0x1217 */
        if (ST(0xB4C) == 1) {
            CB->change_piece(10, 2);
        }
        /* 0x1239 */
        if (ST(0xB4C) == 2) {
            CB->change_piece(10, 3);
        }
    }
    /* 0x1258 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(35)) != VIE_DEAD && CB->divers(DV_IN_ZONE, 35, 40) && CB->direction_spec(0x494F)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x12C3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD32", P_ARKHAN2);
        }
        /* 0x12E5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD33", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 09 — trig09_10011329 */
static void trig09(void) {
    /* 0x1338 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(9)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x1392 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L14D105", P_ARKHAN2, "SG01ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x13E3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L14D106", P_ARKHAN3, "SG00ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 11 — trig11_10011438 */
static void trig11(void) {
    /* 0x1447 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x63A)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x1495 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0x14C0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 — trig12_1001150B */
static void trig12(void) {
    /* 0x151A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x10CC)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x1574 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00PRE03");
            lieu_action_register("L14D95", P_ARKHAN2, "SG01PRE03", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x15D0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00PRE03");
            lieu_action_register("L14D96", P_ARKHAN3, "SG00ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 13 — trig13_10011625 */
static void trig13(void) {
    /* 0x1634 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(14)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x168E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L14D97", P_ARKHAN2, "SG00ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x16DF */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD22B", P_ARKHAN3);
            CB->animation("SP03ACT32");
            CB->animation("ar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 15 — trig15_10011739 */
static void trig15(void) {
    /* 0x1748 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xED8)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x17A2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0x17CA */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN2, "SG00ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 — trig16_10011814 */
static void trig16(void) {
    /* 0x1823 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xB3B0)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x187D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x188F */
            if (ST(0x6CC) == 0) {
                CB->animation("SG00PRE08");
                CB->etat_objet("OB85", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x6CC) = 1;
                return;
            }
            /* 0x1902 */
            if (ST(0x6CC) == 1) {
                lieu_action_register("LCOD10", P_ARKHAN2, "SG00DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x1953 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x1965 */
            if (ST(0x6CC) == 0) {
                CB->animation("SG00PRE08");
                CB->etat_objet("OB85", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x6CC) = 1;
                return;
            }
            /* 0x19D8 */
            if (ST(0x6CC) == 1) {
                lieu_action_register("LCOD11", P_ARKHAN3, "SG00DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 17 — trig17_10011A22 */
static void trig17(void) {
    /* 0x1A31 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(18)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x1A9A */
        if (ST(V_GUARD_FIGHT_DONE) != 2 || (ST(0xC44) == 1 && ST(V_GUARD_FIGHT_DONE) == 2)) {
            /* 0x1AD0 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L14D100", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x1B1B */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L14D101", P_ARKHAN3);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x1B66 */
        if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xC44) != 1) {
            /* 0x1B8A */
            if (ST(0xBEC) != 2) {
                /* 0x1B9C */
                if (ST(ST_VIE(P_MALROT1)) == 1 && ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L14D137", P_MALROT1, "SP61ATT51", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x1C02 */
                if (ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
                    /* 0x1C14 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0x1C26 */
                        if (ST(0x174) == 0) {
                            CB->animation("SG00PRE08");
                            CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            ST(0x174) = 1;
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x1C99 */
                        if (ST(0x174) == 1) {
                            CB->dialogue("L14D100", P_ARKHAN2);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0x1CE4 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0x1CF6 */
                        if (ST(0x174) == 0) {
                            CB->animation("SG00PRE08");
                            CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            ST(0x174) = 1;
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x1D69 */
                        if (ST(0x174) == 1) {
                            CB->dialogue("L14D101", P_ARKHAN3);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }
            }
            /* 0x1DB4 */
            if (ST(0xBEC) == 2) {
                /* 0x1DC6 */
                if (ST(ST_VIE(P_MALROT1)) == 1 && ST(ST_HERO) == P_ARKHAN2) {
                    /* 0x1DEA */
                    if (ST(0x174) == 0) {
                        lieu_action_register("L14D138A", P_MALROT1, "SP61ATT51", "SP61ARR01_10");
                        lieu_action_register("L14D138B", P_MALROT1, "SP61ATT51", "SP61ARR01_10");
                        CB->animation("SG00PRE08");
                        CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        ST(0x174) = 1;
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x1E8F */
                    if (ST(0x174) == 1) {
                        CB->dialogue("L14D100", P_ARKHAN2);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x1EDA */
                if (ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
                    /* 0x1EEC */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0x1EFE */
                        if (ST(0x174) == 0) {
                            CB->animation("SG00PRE08");
                            CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            ST(0x174) = 1;
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x1F71 */
                        if (ST(0x174) == 1) {
                            CB->dialogue("L14D100", P_ARKHAN2);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0x1FBC */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0x1FCE */
                        if (ST(0x174) == 0) {
                            CB->animation("SG00PRE08");
                            CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            ST(0x174) = 1;
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x2041 */
                        if (ST(0x174) == 1) {
                            CB->dialogue("L14D101", P_ARKHAN3);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }
            }
        }
    }
}

/* trigger 19 — trig19_10012090 */
static void trig19(void) {
    /* 0x209F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x20F9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG01PRE02");
            CB->dialogue("L14D89", P_ARKHAN2);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x214F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG01PRE02");
            CB->dialogue("L14D90", P_ARKHAN3);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 21 — trig21_100121A9 */
static void trig21(void) {
    /* 0x21B8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x74CC)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x2212 */
        if (ST(0xBF4) == 0) {
            /* 0x2224 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L14D265", P_ARKHAN2, "SG00DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x2275 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L14D266", P_ARKHAN3, "SG00DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x22C6 */
        if (ST(0xBF4) == 1) {
            /* 0x22D8 */
            if (ST(0x560) == 0) {
                /* 0x22EA */
                if (ST(ST_PLAYER_WEAPON) != 97) {
                    /* 0x22FC */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L14D267", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x2324 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L14D268", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x2375 */
                if (ST(ST_PLAYER_WEAPON) == 97) {
                    /* 0x2387 */
                    if (ST(0x6FC) == 1) {
                        /* 0x2399 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SP02ATT41");
                            CB->animation("ar_20");
                            lieu_action_register("L14D269", P_ARKHAN2, "SG00DIA05", NULL);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0x560) = 1;
                            ST(0x6FC) = 2;
                            return;
                        }
                        /* 0x2413 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SP03ATT41");
                            CB->animation("ar_20");
                            lieu_action_register("L14D270", P_ARKHAN3, "SG00DIA05", NULL);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0x560) = 1;
                            ST(0x6FC) = 2;
                            return;
                        }
                    }
                    /* 0x248D */
                    if (ST(0x6FC) == 2) {
                        /* 0x249F */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->dialogue("LCOD42", P_ARKHAN2);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x24EA */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->dialogue("LCOD43", P_ARKHAN3);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }
            }
            /* 0x2535 */
            if (ST(0x560) == 1) {
                /* 0x2547 */
                if (ST(ST_PLAYER_WEAPON) == 97) {
                    /* 0x2559 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L14D271", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x25AA */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L14D272", P_ARKHAN3, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x25FB */
                if (ST(ST_PLAYER_WEAPON) != 97) {
                    /* 0x260D */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L14D273", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x265E */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L14D274", P_ARKHAN3, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
        }
    }
}

/* trigger 23 — trig23_100126B3 */
static void trig23(void) {
    /* 0x26C2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(10)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x271C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x272E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D104A", P_ARKHAN2, "SG00ATT01", NULL);
                lieu_action_register("L14D104B", P_ARKHAN2, "SG00ATT01", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x2795 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT84");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->dialogue("L14D196", P_ARKHAN2);
                CB->animation("ar_20");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x27EB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x27FD */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D107", P_ARKHAN3, "SG00ATT01", NULL);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x284E */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT84");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->dialogue("L14D197", P_ARKHAN3);
                CB->animation("ar_20");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 24 — trig24_1001289D */
static void trig24(void) {
    /* 0x28AC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x63A) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && !CB->direction_perso(P_MALROT1)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x292D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0x2958 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x29A7 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->select_pile(2, 2);
        CB->evenement(2, -1);
        ST(V_EVENT_2) = 0;
        CB->stop_joindre(-P_MALROT1);
        CB->joindre(P_MALROT1, 0, 0, 0, ST(ST_HERO));
        CB->joindre(ST(ST_HERO), 0, 0, 0, P_MALROT1);
        ST(0x98) = 1;
        /* 0x2A8E */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            /* 0x2AA0 */
            if ((ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2)) && ST(ST_HERO) == P_ARKHAN2) {
                /* 0x2ADB */
                if (ST(0xCF0) == 0) {
                    /* 0x2AED */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L14D113", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D114A", P_MALROT1, "SP61DIA58", "SP61ARR01_10");
                        lieu_action_register("L14D114B", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCF0) = 1;
                        CB->select_pile(0, 1);
                        return;
                    }
                    /* 0x2B8C */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L14D109", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D110A", P_MALROT1, "SP61DIA56", "SP61ARR01_10");
                        lieu_action_register("L14D110B", P_MALROT1, "SP61DIA61", "SP61ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCF0) = 1;
                        CB->select_pile(0, 1);
                        return;
                    }
                }
                /* 0x2C2B */
                if (ST(0xCF0) == 1) {
                    /* 0x2C3D */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L14D115", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D116A", P_MALROT1, "SP61DIA58", "SP61ARR01_10");
                        lieu_action_register("L14D116B", P_MALROT1, "SP61DIA61", "SP61ARR01_10");
                        lieu_action_register("L14D116C", P_MALROT1, "SP61DIA60", "SP61ARR01_10");
                        lieu_action_register("L14D116D", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        ST(0xCF0) = 2;
                        return;
                    }
                    /* 0x2D0E */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L14D111", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D112", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                        /* 0x2D4F */
                        if (ST(ST_VIE(P_MALROT4)) == VIE_DEAD) {
                            CB->stop_joindre(P_MALROT1);
                            ST(0x4CC) = 1;
                            CB->evenement(2, -1);
                            ST(V_EVENT_2) = 0;
                            ST(V_ENGAGED_PERSONA) = P_MALROT1;
                            CB->combat(P_MALROT1, 0);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0x2DE9 */
                        if (ST(ST_VIE(P_MALROT4)) == 1 && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4) {
                            CB->select_pile(1, 1);
                            CB->stop_joindre(P_MALROT1);
                            ST(0x4CC) = 1;
                            CB->evenement(2, -1);
                            ST(V_EVENT_2) = 0;
                            ST(V_ENGAGED_PERSONA) = P_MALROT1;
                            CB->evenement(1, -1);
                            CB->animation("Sar_10");
                            ST(V_EVENT_1) = 0;
                            CB->stop_joindre(-P_MALROT4);
                            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
                            CB->animation("P64ARR01_10");
                            CB->modify_var(&ST(0xCB8), 1);
                            CB->select_pile(0, 1);
                            CB->select_pile(2, 1);
                            ST(V_ENGAGED_PERSONA) = 0x193D;
                            CB->combat(P_MALROT4, 0);
                            CB->combat(P_MALROT1, 0);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                    }
                }
                /* 0x2F68 */
                if (ST(0xCF0) == 2 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L14D118A", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D118B", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D118C", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D119A", P_MALROT1, "SP61DIA56", "SP61ARR01_10");
                    lieu_action_register("L14D119B", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xCF0) = 3;
                    ST(0xBEC) = 1;
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0x3054 */
                if (ST(0xCF0) == 3 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L14D121", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D122A", P_MALROT1, "SP61DIA58", "SP61ARR01_10");
                    lieu_action_register("L14D122B", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                    lieu_action_register("L14D198A", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D198B", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D198C", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D199", P_MALROT1, "SP61DIA61", "SP61ARR01_10");
                    lieu_action_register("L14D198D", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D198E", P_ARKHAN2, "SG00DIA02", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xCF0) = 4;
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0x318C */
                if (ST(0xCF0) == 4 && ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x31B0 */
                    if (ST(0xF4) == 0) {
                        lieu_action_register("L14D200", P_ARKHAN2, "SG00DIA02", NULL);
                        ST(0xF4) = 1;
                    } else {
                        lieu_action_register("L14D242", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L14D201", P_MALROT1, "SP61DIA58", "SP61ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xCF0) = 4;
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0x3255 */
                if (ST(0xCF0) >= 1 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L14D277", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D278", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->evenement(0, 250);
                    CB->select_pile(0, 1);
                    return;
                }
            }
            /* 0x32EB */
            if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != 24 && ST(ST_HERO) == P_ARKHAN2) {
                /* 0x3338 */
                if (ST(0xBEC) >= 1) {
                    lieu_action_register("LCOD18", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D130", P_MALROT1, "SP61DIA64", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0x33AF */
                if (ST(0xBEC) == 0) {
                    lieu_action_register("L14D129", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D131", P_MALROT1, "SP61DIA64", "SP61ARR01_10");
                    lieu_action_register("L14D116D", P_MALROT1, "SP61DIA63", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
            }
            /* 0x343F */
            if (ST(ST_PLAYER_WEAPON) == 24) {
                /* 0x3451 */
                if (ST(0xBEC) == 0) {
                    lieu_action_register("L14D144", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L14D133", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0x34C8 */
                if (ST(0xBEC) == 1) {
                    lieu_action_register("L14D144", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L14D135", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                    lieu_pre_scene(P_ARKHAN2, P_MALROT1, 24, 1, 24);
                    CB->animation("P61ACT01");
                    CB->animation("P61ARR01");
                    CB->dialogue("L14D175A", P_MALROT1);
                    CB->dialogue("L14D175B", P_MALROT1);
                    lieu_pre_scene(P_ARKHAN2, P_MALROT1, 24, 1, 24);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xBEC) = 2;
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0x35A3 */
                if (ST(0xBEC) == 2) {
                    lieu_action_register("L14D134", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L14D136", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
            }
        }
    }
    /* 0x3617 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
        CB->fouille(P_MALROT1);
        return;
    }
    /* 0x3657 */
    if (ST(V_PLAYER_STRIKE) >= 1 && (ST(V_ENGAGED_PERSONA) != P_MALROT4 || ST(V_ENGAGED_PERSONA) != 0x1814) && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        /* 0x36B4 */
        if (ST(V_ENGAGED_PERSONA) == 0) {
            CB->select_pile(2, 2);
            CB->stop_joindre(P_MALROT1);
            ST(0x4CC) = 1;
            CB->evenement(2, -1);
            ST(V_EVENT_2) = 0;
            ST(V_ENGAGED_PERSONA) = P_MALROT1;
            CB->combat(P_MALROT1, 0);
            CB->select_pile(0, 1);
            CB->bloque(BLOQUE_NONE);
            /* 0x373D */
            if (ST(ST_VIE(P_MALROT4)) == 1 && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4) {
                CB->select_pile(1, 1);
                CB->evenement(1, -1);
                CB->animation("Sar_10");
                ST(V_EVENT_1) = 0;
                CB->stop_joindre(-P_MALROT4);
                CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
                CB->animation("P64ARR01_10");
                CB->modify_var(&ST(0xCB8), 1);
                ST(V_ENGAGED_PERSONA) = 0x193D;
                CB->combat(P_MALROT4, 0);
                CB->select_pile(0, 1);
            }
        }
    }
    /* 0x3821 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x3889 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD20", P_ARKHAN2);
        }
        /* 0x38AB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x38F1 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD && CB->direction_perso(P_MALROT1)) {
        ST(V_CRI_USED) = 1;
        ST(0x4CC) = 1;
        /* 0x3976 */
        if (ST(0xF0) != 99) {
            ST(0xF0) = 61;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0x23, 1000);
        return;
    }
}

/* trigger 25 — trig25_100139B0 */
static void trig25(void) {
    /* 0x39BF */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(26)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        /* 0x3A82 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x3A94 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D162", P_ARKHAN2, "SG00DIA01", NULL);
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x3AE5 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT55");
                ST(0x80) = CB->divers(DV_HP, ST(ST_HERO), -1);
                CB->divers(DV_HP, ST(ST_HERO), (ST(0x80) - 10));
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x3B74 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x3B86 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D279", P_ARKHAN3, "SG00DIA01", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x3BD7 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT55");
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 27 — trig27_10013C21 */
static void trig27(void) {
    /* 0x3C30 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x3C77 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0x3C9F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN2, "SG00ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 29 — trig29_10013CF4 */
static void trig29(void) {
    /* 0x3D03 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(28)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x3D4E */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x3D60 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT28");
            }
            /* 0x3D8F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0x3DAC */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x3DF2 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x3E04 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT29");
            }
            /* 0x3E33 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0x3E50 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT29");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 30 — trig30_10013E8F */
static void trig30(void) {
    /* 0x3E9E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x7D70)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x3EE9 */
        if (ST(0xBF4) == 0) {
            /* 0x3EFB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L14D258", P_ARKHAN2, "SG00DIA05", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x3F4C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L14D259", P_ARKHAN3, "SG00DIA05", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x3F9D */
        if (ST(0xBF4) == 1) {
            /* 0x3FAF */
            if (ST(0x560) == 0) {
                /* 0x3FC1 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L14D260", P_ARKHAN2, "SG00DIA05", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x4012 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L14D261", P_ARKHAN3, "SG00DIA05", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x4063 */
            if (ST(0x560) == 1) {
                /* 0x4075 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->select_pile(1, 2);
                    CB->evenement(0x24, 550);
                    CB->select_pile(0, 1);
                    CB->animation("SP02ATT42");
                    CB->animation("ar_10");
                    /* 0x40CA */
                    if (ST(V_GUARD_FIGHT_DONE) == 2) {
                        CB->bruitage("L14AMB4", -1, -1);
                    }
                    /* 0x40EE */
                    if (ST(V_GUARD_FIGHT_DONE) != 2) {
                        CB->bruitage("L14AMB3", -1, -1);
                    }
                    ST(0x100) = 0;
                    ST(0xFC) = 0;
                    ST(0xBF4) = 0;
                    lieu_action_register("L14D262", P_ARKHAN2, "SG00DIA05", NULL);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x4173 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->select_pile(1, 2);
                    CB->evenement(0x24, 450);
                    CB->select_pile(0, 1);
                    CB->animation("SP03ATT42");
                    CB->animation("ar_10");
                    /* 0x41C8 */
                    if (ST(V_GUARD_FIGHT_DONE) == 2) {
                        CB->bruitage("L14AMB4", -1, -1);
                    }
                    /* 0x41EC */
                    if (ST(V_GUARD_FIGHT_DONE) != 2) {
                        CB->bruitage("L14AMB3", -1, -1);
                    }
                    ST(0xFC) = 0;
                    ST(0x100) = 0;
                    ST(0xBF4) = 0;
                    lieu_action_register("L14D264", P_ARKHAN3, "SG00DIA05", NULL);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
}

/* trigger 31 — trig31_1001426A */
static void trig31(void) {
    /* 0x4279 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x8F62)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x42C4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L14D275A", P_ARKHAN2, "SG00DIA05", NULL);
            lieu_action_register("L14D275B", P_ARKHAN2, "SG00DIA05", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x432B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L14D276A", P_ARKHAN3, "SG00DIA05", NULL);
            lieu_action_register("L14D276B", P_ARKHAN3, "SG00DIA05", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 33 — trig33_10014396 */
static void trig33(void) {
    /* 0x43A5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x63A) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && !CB->direction_perso(P_DELIA)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x4426 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0x4451 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x44A4 */
    if (CB->direction_perso(P_DELIA)) {
        /* 0x44B3 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
            /* 0x44E3 */
            if (ST(ST_VIE(P_DELIA)) < 2) {
                ST(0x70) = 10;
                CB->stop_joindre(-P_DELIA);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->bloque(BLOQUE_ALL);
                /* 0x4523 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0x4578 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L14D238", P_ARKHAN2, "SG00DIA02", "Sar_10");
                    }
                    /* 0x45A3 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L14D239", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L14D240", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                }
                /* 0x4611 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0x4668 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0x4693 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L14D241A", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                    lieu_action_register("L14D241B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                }
            }
            /* 0x4733 */
            if (ST(ST_VIE(P_DELIA)) == VIE_DEAD) {
                CB->fouille(P_DELIA);
            }
        }
        /* 0x474D */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_DELIA) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x4794 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L10D93", P_ARKHAN2);
            }
            /* 0x47B6 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L10D94", P_ARKHAN3);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
        /* 0x47F9 */
        if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_DELIA)) == 1) {
            ST(V_PLAYER_STRIKE) = -1;
            CB->select_pile(1, 2);
            CB->stop_joindre(-P_DELIA);
            CB->bloque(BLOQUE_ALL);
            /* 0x486D */
            if (ST(V_DELIA_HITS) == 1) {
                ST(ST_SCRIPT_BUSY) = 1;
                ST(0x170) = 1;
                ST(V_DELIA_KILLED) = 14;
                ST(V_DELIA_PHASE) = 20;
                ST(ST_VIE(P_DELIA)) = VIE_DEAD;
                CB->animation("SP10IMP51");
                CB->animation("P10cad");
                CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10CAD", 0x21, 0);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
            /* 0x492F */
            if (ST(V_DELIA_HITS) == 0) {
                ST(ST_SCRIPT_BUSY) = 1;
                ST(V_DELIA_HITS) = 1;
                CB->animation("SP10ESA01");
                CB->animation("P10ARR02_50");
                CB->dialogue("L10D95", P_DELIA);
                CB->animation("P10ARR02_50");
                CB->modify_var(&ST(0x6C), 1);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
        }
        /* 0x49C9 */
        if (ST(V_PLAYER_STRIKE) == 2 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_DELIA)) == 1 && CB->direction_perso(P_DELIA) && ST(ST_COMBAT_STANCE) == 1) {
            ST(V_PLAYER_STRIKE) = -1;
            CB->combat(P_DELIA, 1);
            ST(V_ENGAGED_PERSONA) = P_DELIA;
        }
        return;
    }
    trig_noop();   /* trig10_1001142D: empty stub */
    /* 0x4A60 */
    if (CB->direction_zone(12) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 4)) {
        trig04();
    }
    /* 0x4A98 */
    if (CB->direction_zone(13) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
    }
    /* 0x4AD0 */
    if (CB->direction_zone(6) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)) {
        trig05();
    }
    /* 0x4B08 */
    if (CB->direction_zone(2) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
        trig02();
    }
    /* 0x4B40 */
    if (CB->direction_zone(14) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
    }
}

/* trigger 34 — trig34_10014B70 — exit: remember the hero, enter room 46 */
static void trig34(void) {
    ST(V_SAVED_HERO) = ST(ST_HERO);
    ST(ST_LEVEL) = 14;
    CB->change_piece(46, -1);
}

/* trigger 35 — trig35_10014BAE */
static void trig35(void) {
    /* 0x4BBD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xBAC9)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x4C17 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00DIA05", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x4C68 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN3, "SG00DIA05", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 36 — trig36_10014CB2 */
static void trig36(void) {
    /* 0x4CC1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xFF0D)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x4D1B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0x4D43 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN2, "SG00ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 37 — trig37_10014D8D */
static void trig37(void) {
    /* 0x4D9C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xFD13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x4DF6 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0x4E1E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN2, "SG00ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 38 — trig38_10014E68 */
static void trig38(void) {
    /* 0x4E77 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(39)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x4ED1 */
        if (ST(0x178) == 0) {
            CB->animation("SG00PRE08");
            CB->etat_objet("OB34A", -1, -1, -1, -1, -1, -1, -3, 4);
            ST(0x178) = 1;
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x4F44 */
        if (ST(0x178) == 1) {
            /* 0x4F56 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD10", P_ARKHAN2, "SG00DIA05", NULL);
            }
            /* 0x4F7E */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD11", P_ARKHAN3, "SG00DIA05", NULL);
            }
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 41 — trig41_10008C8E — entry hook (music, Delia hand-over, who engages whom) */
static void trig41(void) {
    /* 0x8CA0 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers_empile(DV_MUSIC, 1, 1);
    } else {
        CB->divers_empile(DV_MUSIC, 2, 1);
    }
    /* 0x8CE6 */
    if (ST(V_SAVED_HERO) != ST(ST_HERO) && ST(V_DELIA_PHASE) == 4 && ST(0xC44) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8D33 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
            lieu_action_register("LCOD48", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            /* 0x8D7B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD50", P_ARKHAN2, "SG00DIA05", NULL);
            }
            /* 0x8DA3 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD51", P_ARKHAN2, "SG00DIA05", NULL);
            }
        }
        /* 0x8DCB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
            lieu_action_register("LCOD44", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            /* 0x8E13 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD46", P_ARKHAN3, "SG00DIA05", NULL);
            }
            /* 0x8E3B */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD47", P_ARKHAN3, "SG00DIA05", NULL);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x8E7C */
    if (ST(0xBE8) == 0 && ST(0xC44) == 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->bloque(BLOQUE_ALL);
        /* 0x8EBD */
        if (ST(ST_VIE(P_KALHI13)) == 1 && ST(ST_VIE(P_DANRYS)) != VIE_DEAD) {
            CB->divers(DV_RUN, P_KALHI13, 1);
            CB->dialogue("L14D108", P_KALHI13);
            ST(0x7C) = 1;
            ST(0xBE8) = 1;
        }
    }
    /* 0x8F1E */
    if ((ST(V_DELIA_PHASE) == 3 || ST(V_DELIA_PHASE) == 4) && ST(0xD4) == 0) {
        ST(V_DELIA_PHASE) = 4;
        ST(0xD4) = 1;
    }
    /* 0x8F72 */
    if (ST(0x4CC) == 1 && ST(0xC44) == 0) {
        /* 0x8F96 */
        if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
            ST(V_ENGAGED_PERSONA) = P_MALROT4;
            CB->combat(P_MALROT4, 0);
        }
        /* 0x8FD3 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            /* 0x8FE5 */
            if (ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4 && ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
                ST(V_ENGAGED_PERSONA) = P_MALROT4;
                CB->combat(P_MALROT4, 0);
            }
            /* 0x9058 */
            if ((ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(ST_VIE(P_MALROT4)) == VIE_DEAD && ST(ST_VIE(97)) == 1) || (ST(ST_VIE(P_MALROT4)) != VIE_DEAD && (ST(0x170) == 1 || ST(V_DELIA_PHASE) == 3 || ST(V_DELIA_PHASE) == 4))) {
                ST(V_ENGAGED_PERSONA) = P_MALROT1;
                CB->combat(P_MALROT1, 0);
            }
            /* 0x90EF */
            if (ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4) {
                ST(V_ENGAGED_PERSONA) = 0x1814;
                CB->combat(P_MALROT4, 0);
                CB->combat(P_MALROT1, 0);
            }
        }
    }
}
