/*
 * lieu54_trig.c - Native hand-port of the LIEU54.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick, events and tick body live in
 * lieu54.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * LIEU54 is another build of the room-14 script (L14 dialogue codes);
 * slots 1, 10, 14, 18, 20, 22, 26, 28, 32, 39 and 40 are empty stubs.
 */

#include <stdio.h>

#include "lieu54_int.h"

static void trig_noop(void);   /* the empty stub slots */
static void trig02(void);
static void trig03(void);
static void trig04(void);
static void trig05(void);
static void trig06(void);
static void trig07(void);
static void trig08(void);
static void trig09(void);
static void trig11(void);
static void trig12(void);
static void trig13(void);
static void trig15(void);
static void trig16(void);
static void trig17(void);
static void trig19(void);
static void trig21(void);
static void trig23(void);
static void trig24(void);
static void trig25(void);
static void trig27(void);
static void trig29(void);
static void trig30(void);
static void trig31(void);
static void trig33(void);
static void trig34(void);
static void trig35(void);
static void trig36(void);
static void trig37(void);
static void trig38(void);
static void trig41(void);

/* dword_10029BC0[42] - filled at INIT with the 5-byte jmp thunks; static
 * here. */
static void (*const g_trigger[42])(void) = {
    l54_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06,
    trig07, trig08, trig09, trig_noop, trig11, trig12, trig13,
    trig_noop, trig15, trig16, trig17, trig_noop, trig19, trig_noop,
    trig21, trig_noop, trig23, trig24, trig25, trig_noop, trig27,
    trig_noop, trig29, trig30, trig31, trig_noop, trig33, trig34,
    trig35, trig36, trig37, trig38, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10029BC0]). */
void l54_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - sub_10001D60 (thunk sub_1000106E; INIT default) */
void l54_trig00(void) {
    /* 0x1D72 */
    if (ST(ST_START_POINT(14)) == 1) {
        ST(0xBF4) = 0;
        ST(0xAF8) = 1;
        /* 0x1DA2: `if (V_GUARD_FIGHT_DONE != 2) {}` - empty in the DLL */
        ST(0xE9C) = 10;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_KALHI13)) = 1;
        /* 0x1DE3 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
        ST(0xEB8) = 2;
    }
    /* 0x1E2E */
    if (ST(ST_START_POINT(14)) == 2) {
        ST(0xBF4) = 0;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(ST_HERO) = P_ARKHAN2;
        /* 0x1E79 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
    }
    /* 0x1EB5 */
    if (ST(ST_START_POINT(14)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xBE8) = 1;
        /* 0x1EE2 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
    }
    /* 0x1F1E */
    if (ST(ST_START_POINT(14)) == 4) {
        ST(0xBF4) = 0;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xBE8) = 1;
        ST(ST_LEVEL) = 19;
        ST(V_DELIA_PHASE) = 3;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    /* 0x1F87 */
    if (ST(ST_START_POINT(14)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xC44) = 1;
        /* 0x1FC3 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
        ST(ST_VIE(P_MALROT1)) = VIE_DEAD;
        ST(ST_VIE(P_MALROT4)) = VIE_DEAD;
    }
    /* 0x201D */
    if (ST(ST_START_POINT(14)) == 6) {
        ST(0xBF4) = 0;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_DELIA_PHASE) = 1;
        /* 0x2077 */
        if (ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
            ST(ST_LEVEL) = 10;
        }
    }
    /* 0x20B3 */
    if (ST(ST_START_POINT(14)) == 102) {
        ST(ST_LEVEL) = 10;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_BOGDARAN)) = 1;
        ST(ST_VIE(P_DHORKAN11)) = 1;
        ST(ST_VIE(P_KALHI12)) = 1;
        ST(ST_VIE(P_KALHI13)) = 1;
        ST(ST_VIE(P_LORY)) = 1;
        ST(ST_VIE(P_PROVIDER)) = 1;
        ST(ST_VIE(P_SUNSEER1)) = 1;
        ST(ST_VIE(P_RYLSADHAR20)) = 1;
        ST(ST_VIE(P_THANANDAR24)) = 1;
        ST(ST_VIE(P_PHEDORIA)) = 1;
        ST(ST_VIE(P_KALHI33)) = 1;
        ST(ST_VIE(P_GUARDIAN1)) = 1;
        ST(ST_VIE(P_GUARDIAN2)) = VIE_DEAD;
        ST(ST_VIE(P_GUARDIAN3)) = 1;
        ST(ST_VIE(P_GUARDIAN4)) = 1;
        ST(ST_VIE(P_GUARDIAN10)) = 1;
        ST(ST_VIE(P_GUARDIAN15)) = 1;
        ST(ST_VIE(P_GUARDIAN16)) = VIE_DEAD;
        ST(ST_VIE(P_GUARDIAN17)) = 1;
        ST(ST_VIE(P_MALROT4)) = 1;
        ST(ST_VIE(P_KONKALITE4)) = 4;
        ST(ST_VIE(P_ZED84)) = 1;
        ST(0x338) = 1;
        ST(0x348) = 1;
        ST(0x358) = 2;
        ST(V_EVENT_16) = 1;
        ST(V_EVENT_17) = 3;
        ST(0x5B8) = 2;
        ST(V_GATE_OB25_OPEN) = 1;
        ST(V_OB38A_STATE) = 2;
        ST(V_KALHI12_TALK_NIGHT) = 1;
        ST(0x718) = 1;
        ST(V_G17_TALK_H23) = 2;
        ST(0x73C) = 1;
        ST(0x740) = 2;
        ST(0x7A4) = 2;
        ST(V_KALHI13_TALK_B) = 2;
        ST(0x830) = 1;
        ST(0x83C) = 1;
        ST(0x848) = 13857;
        ST(V_KALHI13_TALK_A) = 1;
        ST(0x948) = 1;
        ST(0xA5C) = 2;
        ST(V_KALHI12_D34_DONE) = 666;
        ST(0xAE4) = 1;
        ST(V_INTRO_STATE) = 1;
        ST(0xAF4) = 3;
        ST(0xAF8) = 1;
        ST(V_KALHI_ROUTE) = 2;
        ST(0xB40) = 3;
        ST(0xB44) = 2;
        ST(0xB54) = 3;
        ST(0xB84) = 1;
        ST(0xB98) = 4;
        ST(V_ZED_BRIEFING) = 3;
        ST(0xBA8) = 6;
        ST(0xBD0) = 2;
        ST(0xBDC) = 2;
        ST(0xBE8) = 1;
        ST(0xCB0) = 4;
        ST(0xD0C) = 1;
        ST(0xD20) = 1;
        ST(0xD24) = 2;
        ST(0xE1C) = 1;
        ST(0xE3C) = 1;
        ST(0xE78) = 9;
        ST(0xE7C) = 3;
        ST(0xE90) = 100;
        ST(0xE9C) = 40;
        ST(0xEA0) = 1;
        ST(0xEB0) = 1;
        ST(V_LEFT_HOME) = 1;
        ST(0xEF0) = 1;
        ST(0xF14) = 2;
        ST(0xF3C) = 2;
    }
    ST(ST_START_POINT(14)) = 0;
    CB->select_pile(2, 2);
    CB->select_pile(1, 2);
    CB->select_pile(0, 2);
    CB->select_pile(0, 1);
    /* 0x258E */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x25B2 */
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
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    CB->vire_col(27, 0);
    CB->vire_col(28, 0);
    /* 0x2797 */
    if (ST(0x6CC) == 0) {
        CB->etat_objet("OB85", 6400, -2300, 1283, 0x4200, 0xE00, 0, -1, 4);
    }
    /* 0x27D3 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->etat_objet("OBCHAR", 3322, -2268, 962, 0xBE00, 0x4200, 0x6200, -1, 4);
        CB->col_objet("OBCHAR", 3322, 962, 100, 100);
    }
    /* 0x282B */
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
    CB->ajoute_zone(25, 5086, 2004, 195, 415, 0);
    CB->ajoute_zone(26, 5110, 2025, 50, 50, 0);
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
    /* 0x2C58 */
    if (ST(0xBF4) == 0) {
        /* 0x2C6A */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->load_anim("214MOT01", 0);
            CB->load_anim("214ELI02", 1);
            CB->bruitage("L14AMB2", -1, -1);
        }
        /* 0x2CAB */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->load_anim("114MOT01", 2);
            CB->load_anim("114ELI02", 3);
            CB->bruitage("L14AMB1", -1, -1);
        }
    }
    /* 0x2CEF */
    if (ST(0xBF4) == 1) {
        /* 0x2D01 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->bruitage("L14AMB2", -1, -1);
        }
        /* 0x2D25 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->bruitage("L14AMB1", -1, -1);
        }
    }
    /* 0x2D49 */
    if (ST(0xBA8) == 5) {
        ST(0xBA8) = 6;
    }
    /* 0x2D6A */
    if (ST(0xBA8) == 6 && ST(ST_LEVEL) == 10 && ST(0xE14) == 1) {
        ST(0xE14) = 2;
    }
    /* 0x2DAC */
    if (ST(ST_VIE(P_THANANDAR24)) == VIE_DEAD) {
        ST(ST_VIE(P_THANANDAR24)) = 4;
    }
    /* 0x2DCD */
    if (ST(0xB38) == 2) {
        ST(0xB38) = 3;
    }
    /* 0x2DEE */
    if (ST(ST_VIE(P_DHORKAN11)) == VIE_DEAD) {
        ST(0xF24) = 1;
    }
    /* 0x2E0F */
    if (ST(0xF28) == 3) {
        ST(0xF28) = 4;
    }
    /* 0x2E30 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->ajoute_zone(28, 5728, 1135, 50, 50, 0);
        CB->ajoute_zone(29, 5739, 1120, 235, 285, 0);
    }
    /* 0x2E7B */
    if (ST(ST_LEVEL) == 10) {
        /* 0x2E8D */
        if (ST(0xE9C) != 10) {
            CB->pos_perso(ST(ST_HERO), 4338, 828, 0x387C, 1, "ar", 1, 0);
        }
        /* 0x2ECA */
        if (ST(0xE9C) == 10) {
            CB->pos_perso(ST(ST_HERO), 4573, 562, 0x518, 1, "ar", 1, 0);
        }
    }
    /* 0x2F04 */
    if (ST(ST_LEVEL) == 15) {
        CB->pos_perso(ST(ST_HERO), 5330, 1008, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x2F52 */
    if (ST(ST_LEVEL) == 19) {
        CB->pos_perso(ST(ST_HERO), 3835, 1950, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x2FA0 */
    if (ST(ST_LEVEL) == 46) {
        CB->pos_perso(ST(ST_HERO), 3062, 923, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        CB->vire_col(25, 0);
        ST(0x188) = 1;
    }
    /* 0x300A */
    if (ST(ST_LEVEL) != 10 && ST(ST_LEVEL) != 15 && ST(ST_LEVEL) != 19 && ST(ST_LEVEL) != 46) {
        CB->pos_perso(ST(ST_HERO), 4315, 1050, 0, 1, "ar", 1, 0);
    }
    /* 0x3071 */
    if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        CB->pos_perso(P_PROVIDER, 3267, 915, 0xCE17, 1, "P16ARR01", 3, 600);
    }
    /* 0x30BD */
    if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(0xBE8) >= 1 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
        CB->pos_perso(P_MALROT4, 5592, 1439, 0x2D9D, 1, "P64ARR01", 2, 1);
    }
    /* 0x3118 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4 && ST(0x170) != 1 && ST(0xC44) == 0) {
        CB->pos_perso(P_MALROT4, 5592, 1439, 0x2D9D, 1, "P64ARR01", 2, 1);
    }
    /* 0x31A9 */
    if (ST(0xC44) == 1 && ST(ST_VIE(35)) != VIE_DEAD) {
        /* 0x31CA */
        if (ST(ST_LEVEL) == 19) {
            CB->pos_perso(0x23, 3925, 1289, 0xC350, 1, "P35GAR01", 4, 1);
            CB->combat(35, 0);
            ST(V_ENGAGED_PERSONA) = 35;
        }
        /* 0x3217 */
        if (ST(ST_LEVEL) == 46) {
            CB->pos_perso(0x23, 3714, 1178, 0x40C4, 1, "P35GAR01", 4, 1);
            CB->combat(35, 0);
            ST(V_ENGAGED_PERSONA) = 35;
        }
        /* 0x3264 */
        if (ST(ST_LEVEL) == 10) {
            CB->pos_perso(0x23, 4907, 1319, 0x38F0, 1, "P35GAR01", 4, 1);
            CB->combat(35, 0);
            ST(V_ENGAGED_PERSONA) = 35;
        }
        /* 0x32B1 */
        if (ST(ST_LEVEL) == 15) {
            CB->pos_perso(0x23, 5167, 1399, 0x86A8, 1, "P35GAR01", 4, 1);
            CB->combat(35, 0);
            ST(V_ENGAGED_PERSONA) = 35;
        }
    }
    /* 0x3301 */
    if (ST(0x4CC) == 1 && ST(0xC44) == 0) {
        /* 0x3325 */
        if (ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4) {
            /* 0x336A */
            if (ST(ST_LEVEL) == 19) {
                CB->pos_perso(P_MALROT4, 3779, 1582, 0x48A, 1, "P64ARR01", 2, 1);
            }
            /* 0x339E */
            if (ST(ST_LEVEL) == 10) {
                CB->pos_perso(P_MALROT4, 4566, 1406, 0x8146, 1, "P64ARR01", 2, 1);
            }
            /* 0x33D2 */
            if (ST(ST_LEVEL) == 15) {
                CB->pos_perso(P_MALROT4, 5677, 1396, 0x6CD3, 1, "P64ARR01", 2, 1);
            }
            /* 0x3406 */
            if (ST(ST_LEVEL) == 46) {
                CB->pos_perso(P_MALROT4, 3583, 1023, 0x3765, 1, "P64ARR01", 2, 1);
            }
        }
        /* 0x343D */
        if (ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) == 2) {
            /* 0x345E */
            if (ST(ST_LEVEL) == 19) {
                CB->pos_perso(P_MALROT1, 4055, 1626, 0x15D8, 1, "P61ARR01", 0x18, 0);
            }
            /* 0x3492 */
            if (ST(ST_LEVEL) == 10) {
                CB->pos_perso(P_MALROT1, 4242, 1413, 0x9008, 1, "P61ARR01", 0x18, 0);
            }
            /* 0x34C6 */
            if (ST(ST_LEVEL) == 15) {
                CB->pos_perso(P_MALROT1, 5835, 1365, 0x682F, 1, "P61ARR01", 0x18, 0);
            }
            /* 0x34FA */
            if (ST(ST_LEVEL) == 46) {
                CB->pos_perso(P_MALROT1, 3580, 1205, 0x4D91, 1, "P61ARR01", 0x18, 0);
            }
        }
    }
    /* 0x3531 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xC44) == 0 && ST(0x4CC) == 0) {
        CB->pos_perso(P_MALROT1, 3649, 1268, 0xE98, 1, "P61ATT04", 0x18, 0);
        ST(V_EVENT_2) = 1;
        CB->retire_zone(19);
    }
    /* 0x35A6 */
    if (ST(0xE9C) == 10 && ST(0xBE8) == 0 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
        CB->vire_col(26, 0);
        CB->evenement(40, 300000);
        ST(0xE9C) = 40;
        CB->pos_perso(P_KALHI13, 4321, 745, 0xB0B1, 1, "P13ARR01", 4, 0);
    }
    /* 0x362D */
    if (ST(ST_VIE(P_MALROT4)) == VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2 && ST(0xC44) == 0) {
        /* 0x3663 */
        if (ST(0xCB8) == 0) {
            ST(0xCB8) = 1;
        }
        CB->pos_perso(P_MALROT4, 0, 0, 0, 2, "P64cad", 2, 0);
        ST(0x4CC) = 0;
    }
    /* 0x36AF */
    if (ST(ST_VIE(P_PROVIDER)) == VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->pos_perso(P_PROVIDER, 0, 0, 0, 2, "P16cad", 3, 0);
        CB->ajoute_zone(38, 3315, 945, 275, 280, 0);
        CB->ajoute_zone(39, 3318, 957, 50, 50, 0);
    }
    /* 0x372B */
    if (ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
        CB->pos_perso(P_MALROT1, 0, 0, 0, 2, "P61cad", 0x18, 0);
    }
    /* 0x3759 */
    if (ST(ST_VIE(35)) == VIE_DEAD) {
        CB->pos_perso(0x23, 0, 0, 0, 2, "P35cad", 4, 0);
    }
    /* 0x3787 */
    if (ST(0x188) == 1) {
        CB->vire_col(25, 0);
    }
    /* 0x37A6 */
    if (ST(V_DELIA_PHASE) == 3 && ST(ST_VIE(P_DELIA)) != VIE_DEAD) {
        CB->pos_perso(P_DELIA, 3756, 2073, 0x7BC3, 1, "P10ARR01", 0x21, 0);
        /* 0x37EF */
        if (ST(0xB4C) == 0) {
            ST(0xB4C) = 1;
        }
    }
    /* 0x3810 */
    if (ST(V_DELIA_PHASE) == 4) {
        CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 1, "P10ARR01", 0x21, 0);
    }
    /* 0x3872 */
    if (ST(ST_VIE(P_DELIA)) == VIE_DEAD && ST(V_DELIA_KILLED) == 14 && ST(0xC44) == 0 && ST(0x170) == 1) {
        CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 2, "P10cad", 0x21, 0);
    }
    /* 0x390A */
    if (ST(0xEB8) == 2 && ST(0x538) == 0) {
        ST(0x538) = 1;
    }
    /* 0x393D */
    if (ST(0xE24) == 2) {
        ST(0xE24) = 3;
    }
    /* 0x395E */
    if (ST(0xEF8) == 2 || ST(0xEF8) == 3) {
        ST(0xEF8) = 10;
        ST(0xF24) = 5;
    }
    /* 0x39A0 */
    if (ST(0xB10) != 0 && ST(0xE74) == 0) {
        ST(0xE74) = 1;
    }

}

/* trigger 2 - trig02_10006B7D */
static void trig02(void) {
    /* 0x6B8F */
    if (ST(0xCB4) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        ST(0xCB4) = 1;
        CB->evenement(1, -1);
        CB->animation("Sar_10");
        ST(V_EVENT_1) = 1;
        CB->stop_joindre(-P_MALROT4);
        CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
        CB->joindre(-P_MALROT4, 0, 0, 0, ST(ST_HERO));
        CB->modify_var(&ST(0xCB8), 1);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0x6C79 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(0xCB8) == 1 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_MALROT4)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x6CF4 */
        if (ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
            CB->stop_joindre(-P_MALROT4);
            CB->joindre(P_MALROT4, 0, 0, 0, ST(ST_HERO));
            CB->joindre(ST(ST_HERO), 0, 0, 0, 0x40);
            /* 0x6D47 */
            if (ST(V_GUARD_FIGHT_DONE) != 2) {
                /* 0x6D59 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0x6D6B */
                    if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                        /* 0x6DA9 */
                        if (ST(0xCC0) == 0) {
                            /* 0x6DBB */
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
                            /* 0x6EAE */
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
                        /* 0x6FD0 */
                        if (ST(0xCC0) == 1) {
                            /* 0x6FE2 */
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
                            /* 0x708D */
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
                        /* 0x7164 */
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
                        /* 0x7250 */
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
                        /* 0x7323 */
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
                        /* 0x7470 */
                        if (ST(0xCC0) >= 5 && ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D40", P_ARKHAN2, "SG00DIA02", NULL);
                            lieu_action_register("L14D41A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            /* 0x74C3 */
                            if (ST(0xCC0) == 5) {
                                lieu_action_register("L14D41B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                ST(0xCC0) = 6;
                            }
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                        }
                        /* 0x7521 */
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
                    /* 0x75CA */
                    if (CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        /* 0x75DC */
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
                        /* 0x766E */
                        if (ST(0xCBC) == 1) {
                            /* 0x7680 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L14D04", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L14D06", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xCBC) = 2;
                                return;
                            }
                            /* 0x76F9 */
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
                        /* 0x779A */
                        if (ST(0xCBC) == 2) {
                            /* 0x77AC */
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
                            /* 0x7837 */
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
                    /* 0x78C9 */
                    if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        lieu_action_register("L14D44", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D45A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        /* 0x7935 */
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
                /* 0x7998 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0x79AA */
                    if (ST(0xDEC) == 0) {
                        /* 0x79BC */
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
                        /* 0x7A4E */
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
                    /* 0x7AC1 */
                    if (ST(0xDEC) == 1) {
                        /* 0x7AD3 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D20", P_ARKHAN3, "SG00DIA07", NULL);
                            lieu_action_register("L14D21A", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            lieu_action_register("L14D21B", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        }
                        /* 0x7B2D */
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
            /* 0x7BB9 */
            if (ST(V_GUARD_FIGHT_DONE) == 2) {
                /* 0x7BCB */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0x7BDD */
                    if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                        /* 0x7C32 */
                        if (ST(0xDDC) == 0) {
                            /* 0x7C44 */
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
                            /* 0x7D08 */
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
                        /* 0x7DB3 */
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
                        /* 0x7EBB */
                        if (ST(0xDDC) == 2 && ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D211", P_ARKHAN2, "SG00DIA05", NULL);
                            lieu_action_register("L14D212", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x7F37 */
                        if (ST(0xDDC) >= 1 && ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D215", P_ARKHAN2, "SG00DIA07", NULL);
                            lieu_action_register("L14D03", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0x4CC) = 1;
                            CB->combat(P_MALROT4, 0);
                            ST(V_ENGAGED_PERSONA) = P_MALROT4;
                            return;
                        }
                    }
                    /* 0x7FDB */
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
                /* 0x8089 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0x809B */
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
                    /* 0x813F */
                    if (ST(0xDF4) == 1 && ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L14D219", P_ARKHAN3, "SG00DIA07", NULL);
                        lieu_action_register("L14D220", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x81BB */
                    if (ST(0xDF4) >= 0 && ST(ST_SPECIAL_MODE) == 0) {
                        CB->dialogue("LCOD22A", P_ARKHAN3);
                        lieu_action_register("L14D185", P_MALROT4, "SP64DIA01", "SP64ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0x4CC) = 1;
                        CB->combat(P_MALROT4, 0);
                        ST(V_ENGAGED_PERSONA) = P_MALROT4;
                        return;
                    }
                }
            }
        }
        /* 0x8259 */
        if (ST(ST_VIE(P_MALROT4)) == VIE_DEAD) {
            /* 0x826B */
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
            /* 0x82D7 */
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
    /* 0x8340 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) != P_MALROT4 && ST(V_ENGAGED_PERSONA) != 0x1910 && ST(V_ENGAGED_PERSONA) != 0x1814 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0) {
        /* 0x83A3 */
        if (ST(V_ENGAGED_PERSONA) == 0) {
            CB->select_pile(1, 1);
            CB->evenement(1, -1);
            ST(V_PLAYER_STRIKE) = -1;
            ST(V_EVENT_1) = 0;
            ST(V_ENGAGED_PERSONA) = P_MALROT4;
            ST(0x4CC) = 1;
            CB->combat(P_MALROT4, 0);
            CB->select_pile(0, 1);
            /* 0x8425 */
            if (ST(ST_VIE(P_MALROT1)) == 1) {
                CB->select_pile(2, 2);
                CB->evenement(2, -1);
                ST(V_EVENT_1) = 0;
                ST(V_ENGAGED_PERSONA) = 0x1814;
                CB->combat(P_MALROT1, 0);
                CB->select_pile(0, 1);
            }
        }
        /* 0x8489 */
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
        /* 0x8513 */
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
    /* 0x859D */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x85FA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD20", P_ARKHAN2);
        }
        /* 0x861C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        return;
    }
    /* 0x865F */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && CB->direction_perso(P_MALROT4)) {
        ST(V_CRI_USED) = 1;
        ST(0x4CC) = 1;
        /* 0x86E4 */
        if (ST(0xDC) != 99) {
            ST(0xDC) = 64;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(33, 1000);
        return;
    }

}

/* trigger 3 - trig03_1000871E */
static void trig03(void) {
    /* 0x872D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_PROVIDER)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_PROVIDER);
        /* 0x87A1 */
        if (ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
            CB->joindre(P_PROVIDER, 0, 0, 0, ST(ST_HERO));
            CB->joindre(ST(ST_HERO), 0, 0, 0, 0x10);
            /* 0x87E9 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x87FB */
                if (ST(ST_PLAYER_WEAPON) == 0 && ST(0x538) != 1) {
                    /* 0x881F */
                    if (ST(0xCCC) == 0) {
                        /* 0x8831 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D49", P_ARKHAN2, "SG00DIA02", NULL);
                        }
                        /* 0x8859 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D59", P_ARKHAN3, "SG00DIA02", NULL);
                        }
                        lieu_action_register("L14D50A", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        lieu_action_register("L14D50B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D50C", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        /* 0x88CC */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D186", P_ARKHAN2, "SG00DIA03", NULL);
                        }
                        /* 0x88F4 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L14D187", P_ARKHAN3, "SG00DIA03", NULL);
                        }
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCCC) = 1;
                        return;
                    }
                    /* 0x8954 */
                    if (ST(0xCCC) == 1) {
                        /* 0x8966 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D51", P_ARKHAN2, "SG00DIA07", NULL);
                        }
                        /* 0x898E */
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
                    /* 0x8A2F */
                    if (ST(0xCCC) == 2) {
                        /* 0x8A41 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D53", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                        /* 0x8A69 */
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
                    /* 0x8B2D */
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
                    /* 0x8BBF */
                    if (ST(0xCCC) == 4) {
                        /* 0x8BD1 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L14D57", P_ARKHAN2, "SG00DIA03", NULL);
                            lieu_action_register("L14D58", P_PROVIDER, "SP16DIA54", "SP16ARR01");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x8C3B */
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
                /* 0x8CBE */
                if (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    /* 0x8CE7 */
                    if (ST(0xCC4) == 0) {
                        lieu_action_register("L14D28", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D29", P_PROVIDER, "SP16DIA60", "SP16ARR01");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xCC4) = 1;
                        return;
                    }
                    /* 0x8D60 */
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
                /* 0x8DE3 */
                if (ST(ST_PLAYER_WEAPON) != 0 && ST(0xBE0) == 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    lieu_action_register("L14D77", P_ARKHAN2, "SG00DIA04", NULL);
                    lieu_action_register("L14D78A", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                    lieu_action_register("L14D78B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                    /* 0x8E66 */
                    if (ST(0xCCC) == 0) {
                        ST(0xCCC) = 1;
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x8EB0 */
                if (ST(ST_PLAYER_WEAPON) != 0 && ST(0xBE0) == 1 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    ST(0xCCC) = 4;
                    /* 0x8EFA */
                    if (ST(ST_PLAYER_WEAPON) == OB_MIRROR) {
                        lieu_action_register("L14D74A", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D74B", P_ARKHAN2, "SG00DIA07", NULL);
                        lieu_action_register("L14D152A", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(16, "OB99", 27, 1);
                        lieu_action_register("L14D152B", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        lieu_action_register("L14D152D", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D152E", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA02", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        l54_pre_scene(P_ARKHAN2, P_PROVIDER, 20, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0x900E */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0x9051 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9097 */
                    if (ST(ST_PLAYER_WEAPON) == OB_FLAGON22) {
                        lieu_action_register("L14D82", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D83A", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(16, "OB44C", 27, 1);
                        lieu_action_register("L14D83B", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA02", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        l54_pre_scene(P_ARKHAN2, P_PROVIDER, 22, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0x9163 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0x91A6 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x91EC */
                    if (ST(ST_PLAYER_WEAPON) == OB_FLAGON42) {
                        lieu_action_register("L14D85", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D86", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(16, "OB43C", 27, 1);
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA02", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        l54_pre_scene(P_ARKHAN2, P_PROVIDER, 42, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0x929F */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0x92E2 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9328 */
                    if (ST(ST_PLAYER_WEAPON) == OB_MORTAR) {
                        lieu_action_register("L14D225", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(16, "OB36", 27, 1);
                        lieu_action_register("L14D226A", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        lieu_action_register("L14D226B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        l54_pre_scene(P_ARKHAN2, P_PROVIDER, 95, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0x93F4 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0x9437 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x947D */
                    if (ST(ST_PLAYER_WEAPON) == OB_DISH) {
                        lieu_action_register("L14D227", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D228A", P_PROVIDER, "SP16DIA56", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(16, "OB13", 27, 1);
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        lieu_action_register("L14D228B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        l54_pre_scene(P_ARKHAN2, P_PROVIDER, 4, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0x9549 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0x958C */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x95D2 */
                    if (ST(ST_PLAYER_WEAPON) == OB_PESTLE) {
                        lieu_action_register("L14D229", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D230A", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(16, "OB34", 27, 1);
                        lieu_action_register("L14D230B", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        l54_pre_scene(P_ARKHAN2, P_PROVIDER, 94, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0x969E */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0x96E1 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9727 */
                    if (ST(ST_PLAYER_WEAPON) == OB_BOWL) {
                        lieu_action_register("L14D231", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D232", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        CB->link_objet(16, "OB64C", 27, 1);
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        l54_pre_scene(P_ARKHAN2, P_PROVIDER, 65, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0x97DA */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0x981D */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9863 */
                    if (ST(ST_PLAYER_WEAPON) == OB_POTION) {
                        lieu_action_register("L14D233", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L14D234", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                        CB->animation("SP16PRE01");
                        CB->animation("P16ARR01");
                        ST(0x154) = 3;
                        CB->link_objet(16, "OB24F", 27, 1);
                        lieu_action_register("L14D76A", P_ARKHAN2, "SG00DIA07", NULL);
                        CB->divers(DV_FLAG_4B2B98, 1, 0);
                        l54_pre_scene(P_ARKHAN2, P_PROVIDER, 31, 1, 3);
                        CB->animation("SP16PRE73");
                        /* 0x9925 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("ar");
                            CB->dialogue("L14D76B", P_ARKHAN2);
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        /* 0x9968 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("P16ARR01");
                            CB->animation("Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x99AE */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != OB_FLAGON22 && ST(ST_PLAYER_WEAPON) != OB_DISH && ST(ST_PLAYER_WEAPON) != OB_FLAGON42 && ST(ST_PLAYER_WEAPON) != OB_POTION && ST(ST_PLAYER_WEAPON) != OB_MIRROR && ST(ST_PLAYER_WEAPON) != OB_HALBERD62 && ST(ST_PLAYER_WEAPON) != OB_PESTLE && ST(ST_PLAYER_WEAPON) != OB_MORTAR) {
                    ST(0xCCC) = 4;
                    lieu_action_register("L14D157", P_ARKHAN2, "SG00DIA05", NULL);
                    ST(V_DLG_RANDOM) = CB->random(3);
                    /* 0x9AA3 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L14D191", P_PROVIDER, "SP16DIA59", "SP16ARR01");
                    }
                    /* 0x9ACE */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L14D192", P_PROVIDER, "SP16DIA60", "SP16ARR01");
                    }
                    /* 0x9AF9 */
                    if (ST(V_DLG_RANDOM) == 2) {
                        lieu_action_register("L14D193", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9B4D */
                if (ST(0x538) == 1) {
                    /* 0x9B5F */
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
                    /* 0x9C36 */
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
            /* 0x9D3C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x9D4E */
                if (ST(0xDF0) == 0 && ST(0x538) != 1 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L14D64", P_ARKHAN3, "SG00DIA07", NULL);
                    lieu_action_register("L14D65", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xDF0) = 1;
                    return;
                }
                /* 0x9DEB */
                if (ST(0xDF0) == 1 && ST(0x538) != 1 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L14D66", P_ARKHAN3, "SG00DIA04", NULL);
                    lieu_action_register("L14D67A", P_PROVIDER, "SP16DIA58", "SP16ARR01");
                    CB->animation("SP16PRE01");
                    CB->animation("P16ARR01");
                    CB->link_objet(16, "OB99", 27, 1);
                    /* 0x9E7A */
                    if (ST(ST_PLAYER_WEAPON) != 0) {
                        /* 0x9E8C */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SP02PRE73");
                            CB->animation("ar");
                        }
                        /* 0x9EB4 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SP03PRE73");
                            CB->animation("ar");
                        }
                    }
                    l54_pre_scene(P_ARKHAN2, P_PROVIDER, 99, 1, 3);
                    lieu_action_register("L14D67B", P_PROVIDER, "SP16DIA57", "SP16ARR01");
                    lieu_action_register("L14D67C", P_PROVIDER, "SP16DIA64", "SP16ARR01");
                    lieu_action_register("L14D67D", P_PROVIDER, "SP16DIA62", "SP16ARR01");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xDF0) = 2;
                    return;
                }
                /* 0x9F74 */
                if (ST(0xDF0) == 2 && ST(0x538) != 1 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L14D68", P_ARKHAN3, "SG00DIA07", NULL);
                    lieu_action_register("L14D69", P_PROVIDER, "SP16DIA64", "SP16ARR01");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xA002 */
                if (ST(0xDF0) >= 0 && ST(ST_SPECIAL_MODE) == 0 && ST(0x538) >= 0) {
                    CB->dialogue("LCOD22C", P_ARKHAN3);
                    lieu_action_register("L14D48", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->combat(P_PROVIDER, 0);
                    ST(V_ENGAGED_PERSONA) = P_PROVIDER;
                    return;
                }
                /* 0xA0A3 */
                if (ST(0x538) == 1) {
                    /* 0xA0B5 */
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
                    /* 0xA1BB */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->dialogue("LCOD22C", P_ARKHAN3);
                        lieu_action_register("L14D48", P_PROVIDER, "SP16DIA61", "SP16ARR01");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->combat(P_PROVIDER, 0);
                        ST(V_ENGAGED_PERSONA) = P_PROVIDER;
                        return;
                    }
                }
            }
        }
        /* 0xA238 */
        if (ST(ST_VIE(P_PROVIDER)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
            /* 0xA259 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->fouille(P_PROVIDER);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            }
            /* 0xA287 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->fouille(P_PROVIDER);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            }
        }
    }
    /* 0xA2B2 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) != P_PROVIDER && ST(V_ENGAGED_PERSONA) != 0x1910 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        /* 0xA30F */
        if (ST(V_ENGAGED_PERSONA) == 0) {
            ST(V_ENGAGED_PERSONA) = P_PROVIDER;
            CB->combat(P_PROVIDER, 0);
        }
        /* 0xA337 */
        if (ST(V_ENGAGED_PERSONA) == P_MALROT4) {
            ST(V_ENGAGED_PERSONA) = 0x1910;
            CB->combat(P_PROVIDER, 0);
        }
    }
    /* 0xA35F */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == P_MALROT4 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xA3B8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L14D149", P_ARKHAN2, "SG00DIA05", NULL);
            lieu_action_register("L14D150", P_PROVIDER, "SP16DIA60", "SP16ARR01");
        }
        /* 0xA3F9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L14D189", P_ARKHAN3, "SG00DIA05", NULL);
            lieu_action_register("L14D190", P_PROVIDER, "SP16DIA60", "SP16ARR01");
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA460 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == P_PROVIDER && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xA4B9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA4CB */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0xA4ED */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L14D194", P_ARKHAN2);
            }
            CB->dialogue("L14D195", P_PROVIDER);
        }
        /* 0xA51F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA56A */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD && CB->direction_perso(P_PROVIDER)) {
        ST(V_CRI_USED) = 1;
        /* 0xA5E0 */
        if (ST(0xE0) != 99) {
            ST(0xE0) = 16;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(34, 1000);
        return;
    }

}

/* trigger 4 - trig04_1000A61A */
static void trig04(void) {
    /* 0xA629 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(35)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->fouille(35);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

}

/* trigger 5 - trig05_1000A6B3 */
static void trig05(void) {
    /* 0xA6C5 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0xA6E9 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    /* 0xA70D */
    if (ST(V_DELIA_PHASE) == 4 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(V_SAVED_HERO) = ST(ST_HERO);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA75C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L03D01", P_ARKHAN2);
        }
        /* 0xA77E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L03D02", P_ARKHAN3);
        }
        ST(ST_LEVEL) = 14;
        CB->change_piece(19, 0);
        return;
    }
    /* 0xA7BE */
    if (ST(0xE34) == 1) {
        ST(0xE34) = 0;
    }
    /* 0xA7DF */
    if (ST(0xBE8) == 3) {
        ST(0xBE8) = 4;
    }
    ST(ST_LEVEL) = 14;
    /* 0xA809 */
    if (ST(ST_SCRIPT_BUSY) == 0) {
        CB->change_piece(19, -1);
    }

}

/* trigger 6 - trig06_1000A824 */
static void trig06(void) {
    /* 0xA836 */
    if (ST(V_DELIA_PHASE) == 4 && ST(0xD8) == 0) {
        ST(V_SAVED_HERO) = ST(ST_HERO);
        ST(0xD8) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA897 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L03D01", P_ARKHAN2);
        }
        /* 0xA8B9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L03D02", P_ARKHAN3);
        }
        ST(ST_LEVEL) = 14;
        CB->change_piece(15, -1);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA904 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0xA928 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 14;
    /* 0xA955 */
    if (ST(ST_SCRIPT_BUSY) == 0) {
        CB->change_piece(15, -1);
    }

}

/* trigger 7 - trig07_1000A970 */
static void trig07(void) {
    /* 0xA982 */
    if (ST(0xE34) == 1) {
        ST(0xE34) = 0;
    }
    /* 0xA9A3 */
    if (ST(V_DELIA_PHASE) == 4 && ST(0x114) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0x114) = 1;
        ST(V_SAVED_HERO) = ST(ST_HERO);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(ST(ST_HERO), 0, 0, 0, 0xA);
        /* 0xAA1D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L03D01", P_ARKHAN2);
        }
        /* 0xAA3F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L03D02", P_ARKHAN3);
        }
        ST(ST_LEVEL) = 14;
        CB->change_piece(46, -1);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xAA85 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0xAAA9 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 14;
    /* 0xAAD6 */
    if (ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->change_piece(46, -1);
    }

}

/* trigger 8 - trig08_1000AAFD */
static void trig08(void) {
    /* 0xAB0C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (ST(V_ENGAGED_PERSONA) == 0 || ST(V_ENGAGED_PERSONA) == 0x23) && CB->direction_spec(0x494F)) {
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        /* 0xAB81 */
        if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
            /* 0xAB93 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02lev07");
            }
            /* 0xABB0 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03lev07");
            }
        }
        /* 0xABD5 */
        if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
            /* 0xABE7 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02lev07");
            }
            /* 0xAC04 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03lev07");
            }
        }
        CB->animation("ar");
        CB->bruitage("L10B01", 0, 0);
        /* 0xAC3B */
        if (ST(0xBBC) == 1) {
            ST(0xBBC) = 2;
        }
        /* 0xAC5C */
        if (ST(0xBBC) == 4 && ST(ST_VIE(P_MALROT5)) != VIE_DEAD) {
            ST(0xBBC) = 5;
        }
        /* 0xAC8F */
        if (ST(0xBBC) == 4 && ST(ST_VIE(P_MALROT5)) == VIE_DEAD) {
            ST(0xBBC) = 8;
        }
        /* 0xACC2 */
        if (ST(0xBBC) == 7) {
            ST(0xBBC) = 8;
        }
        /* 0xACE3 */
        if (ST(0xE34) == 1) {
            ST(0xE34) = 0;
        }
        /* 0xAD04 */
        if (ST(0xBE8) == 3) {
            ST(0xBE8) = 4;
        }
        /* 0xAD25 */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
            CB->divers(DV_MUSIC, 1, 4);
        }
        /* 0xAD49 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->divers(DV_MUSIC, 2, 4);
        }
        /* 0xAD6D */
        if (ST(V_DELIA_PHASE) == 4) {
            ST(V_DELIA_PHASE) = 5;
        }
        /* 0xAD8E */
        if (ST(ST_VIE(P_KONKALITE9)) == VIE_DEAD) {
            ST(ST_VIE(P_KONKALITE9)) = 4;
        }
        ST(ST_LEVEL) = 14;
        /* 0xADBB */
        if (ST(0xB4C) == 0) {
            CB->change_piece(10, 1);
        }
        /* 0xADDD */
        if (ST(0xB4C) == 1) {
            CB->change_piece(10, 2);
        }
        /* 0xADFF */
        if (ST(0xB4C) == 2) {
            CB->change_piece(10, 3);
        }
    }

}

/* trigger 9 - trig09_1000AE1A */
static void trig09(void) {
    /* 0xAE29 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(9)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xAE92 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L14D105", P_ARKHAN2, "SG01ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xAEE3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L14D106", P_ARKHAN3, "SG00ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }

}

/* trigger 11 - trig11_1000AF38 */
static void trig11(void) {
    /* 0xAF47 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x63A)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xAF95 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xAFC0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

}

/* trigger 12 - trig12_1000B00B */
static void trig12(void) {
    /* 0xB01A */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x10CC)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xB083 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00PRE03");
            lieu_action_register("L14D95", P_ARKHAN2, "SG01PRE03", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB0DF */
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

/* trigger 13 - trig13_1000B134 */
static void trig13(void) {
    /* 0xB143 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(14)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xB1AC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L14D97", P_ARKHAN2, "SG00ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB1FD */
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

/* trigger 15 - trig15_1000B257 */
static void trig15(void) {
    /* 0xB266 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xED8)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xB2CF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0xB2F7 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN2, "SG00ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

}

/* trigger 16 - trig16_1000B341 */
static void trig16(void) {
    /* 0xB350 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xB3B0)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xB3B9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xB3CB */
            if (ST(0x6CC) == 0) {
                CB->animation("SG00PRE08");
                CB->etat_objet("OB85", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x6CC) = 1;
                return;
            }
            /* 0xB43E */
            if (ST(0x6CC) == 1) {
                lieu_action_register("LCOD10", P_ARKHAN2, "SG00DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xB48F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xB4A1 */
            if (ST(0x6CC) == 0) {
                CB->animation("SG00PRE08");
                CB->etat_objet("OB85", -1, -1, -1, -1, -1, -1, -3, 4);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x6CC) = 1;
                return;
            }
            /* 0xB514 */
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

/* trigger 17 - trig17_1000B55E */
static void trig17(void) {
    /* 0xB56D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(18)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xB5D6 */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
            /* 0xB5E8 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L14D100", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB633 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L14D101", P_ARKHAN3);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xB67E */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            /* 0xB690 */
            if (ST(0xBEC) != 2) {
                /* 0xB6A2 */
                if (ST(ST_VIE(P_MALROT1)) == 1 && ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L14D137", P_MALROT1, "SP61ATT51", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xB708 */
                if (ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
                    /* 0xB71A */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0xB72C */
                        if (ST(0x174) == 0) {
                            CB->animation("SG00PRE08");
                            CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            ST(0x174) = 1;
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xB79F */
                        if (ST(0x174) == 1) {
                            CB->dialogue("L14D100", P_ARKHAN2);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0xB7EA */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0xB7FC */
                        if (ST(0x174) == 0) {
                            CB->animation("SG00PRE08");
                            CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            ST(0x174) = 1;
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xB86F */
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
            /* 0xB8BA */
            if (ST(0xBEC) == 2) {
                /* 0xB8CC */
                if (ST(ST_VIE(P_MALROT1)) == 1 && ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xB8F0 */
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
                    /* 0xB995 */
                    if (ST(0x174) == 1) {
                        CB->dialogue("L14D100", P_ARKHAN2);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xB9E0 */
                if (ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
                    /* 0xB9F2 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0xBA04 */
                        if (ST(0x174) == 0) {
                            CB->animation("SG00PRE08");
                            CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            ST(0x174) = 1;
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xBA77 */
                        if (ST(0x174) == 1) {
                            CB->dialogue("L14D100", P_ARKHAN2);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0xBAC2 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0xBAD4 */
                        if (ST(0x174) == 0) {
                            CB->animation("SG00PRE08");
                            CB->etat_objet("OB64C", -1, -1, -1, -1, -1, -1, -3, 4);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            ST(0x174) = 1;
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xBB47 */
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

/* trigger 19 - trig19_1000BB96 */
static void trig19(void) {
    /* 0xBBA5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xBC0E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG01PRE02");
            CB->dialogue("L14D89", P_ARKHAN2);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xBC64 */
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

/* trigger 21 - trig21_1000BCBE */
static void trig21(void) {
    /* 0xBCCD */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x74CC)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xBD36 */
        if (ST(0xBF4) == 0) {
            /* 0xBD48 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L14D265", P_ARKHAN2, "SG00DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xBD99 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L14D266", P_ARKHAN3, "SG00DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xBDEA */
        if (ST(0xBF4) == 1) {
            /* 0xBDFC */
            if (ST(0x560) == 0) {
                /* 0xBE0E */
                if (ST(ST_PLAYER_WEAPON) != OB_JERRYCAN) {
                    /* 0xBE20 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L14D267", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0xBE48 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L14D268", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xBE99 */
                if (ST(ST_PLAYER_WEAPON) == OB_JERRYCAN) {
                    /* 0xBEAB */
                    if (ST(0x6FC) == 1) {
                        /* 0xBEBD */
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
                        /* 0xBF37 */
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
                    /* 0xBFB1 */
                    if (ST(0x6FC) == 2) {
                        /* 0xBFC3 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->dialogue("LCOD42", P_ARKHAN2);
                            CB->animation("ar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xC00E */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->dialogue("LCOD43", P_ARKHAN3);
                            CB->animation("ar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }
            }
            /* 0xC059 */
            if (ST(0x560) == 1) {
                /* 0xC06B */
                if (ST(ST_PLAYER_WEAPON) == OB_JERRYCAN) {
                    /* 0xC07D */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L14D271", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xC0CE */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L14D272", P_ARKHAN3, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xC11F */
                if (ST(ST_PLAYER_WEAPON) != OB_JERRYCAN) {
                    /* 0xC131 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L14D273", P_ARKHAN2, "SG00DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xC182 */
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

/* trigger 23 - trig23_1000C1D7 */
static void trig23(void) {
    /* 0xC1E6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(10)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xC24F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xC261 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D104A", P_ARKHAN2, "SG00ATT01", NULL);
                lieu_action_register("L14D104B", P_ARKHAN2, "SG00ATT01", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC2C8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT84");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->dialogue("L14D196", P_ARKHAN2);
                CB->animation("ar_20");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xC31E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xC330 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D107", P_ARKHAN3, "SG00ATT01", NULL);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC381 */
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

/* trigger 24 - trig24_1000C3D0 */
static void trig24(void) {
    /* 0xC3DF */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->select_pile(2, 2);
        ST(V_EVENT_2) = 0;
        CB->stop_joindre(-P_MALROT1);
        CB->joindre(P_MALROT1, 0, 0, 0, ST(ST_HERO));
        ST(0x98) = 1;
        /* 0xC49D */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            /* 0xC4AF */
            if (ST(ST_PLAYER_WEAPON) == 0 && ST(ST_HERO) == P_ARKHAN2) {
                /* 0xC4D3 */
                if (ST(0xCF0) == 0) {
                    /* 0xC4E5 */
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
                    /* 0xC584 */
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
                /* 0xC623 */
                if (ST(0xCF0) == 1) {
                    /* 0xC635 */
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
                    /* 0xC706 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L14D111", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D112", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->evenement(0, 250);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
                /* 0xC78A */
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
                /* 0xC876 */
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
                /* 0xC9AE */
                if (ST(0xCF0) == 4 && ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xC9D2 */
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
                /* 0xCA77 */
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
            /* 0xCB0D */
            if (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_HERO) == P_ARKHAN2) {
                /* 0xCB48 */
                if (ST(0x84) == 0) {
                    lieu_action_register("L14D123", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D124", P_MALROT1, "SP61GAR26", "SP61GAR02");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x84) = 1;
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xCBCE */
                if (ST(0x84) == 1) {
                    /* 0xCBE0 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L14D125A", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D125B", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D126", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                    /* 0xCC6D */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L14D127", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L14D278", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->evenement(0, 250);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
            }
            /* 0xCCF1 */
            if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != OB_FULL_GOURD && ST(ST_HERO) == P_ARKHAN2) {
                /* 0xCD3E */
                if (ST(0xBEC) == 1) {
                    lieu_action_register("LCOD18", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L14D130", P_MALROT1, "SP61DIA64", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xCDB5 */
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
            /* 0xCE45 */
            if (ST(ST_PLAYER_WEAPON) == OB_FULL_GOURD) {
                /* 0xCE57 */
                if (ST(0xBEC) == 0) {
                    lieu_action_register("L14D144", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L14D133", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xCECE */
                if (ST(0xBEC) == 1) {
                    lieu_action_register("L14D144", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L14D135", P_MALROT1, "SP61DIA59", "SP61ARR01_10");
                    l54_pre_scene(P_ARKHAN2, P_MALROT1, 24, 1, 24);
                    CB->animation("P61ACT01");
                    CB->animation("P61ARR01");
                    CB->dialogue("L14D175A", P_MALROT1);
                    CB->dialogue("L14D175B", P_MALROT1);
                    l54_pre_scene(P_ARKHAN2, P_MALROT1, 24, 1, 24);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0xBEC) = 2;
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xCFA9 */
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
    /* 0xD01D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
        CB->fouille(P_MALROT1);
        return;
    }
    /* 0xD05D */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) != P_MALROT4 && ST(V_ENGAGED_PERSONA) != 0x1814 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        /* 0xD0BA */
        if (ST(V_ENGAGED_PERSONA) == 0) {
            ST(V_ENGAGED_PERSONA) = P_MALROT1;
            CB->combat(P_MALROT1, 0);
            CB->select_pile(2, 1);
            CB->stop_joindre(P_MALROT1);
            ST(0x4CC) = 1;
            CB->evenement(2, -1);
            ST(V_EVENT_2) = 0;
            CB->select_pile(0, 1);
            /* 0xD13B */
            if (ST(ST_VIE(P_MALROT4)) == 1 && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4) {
                CB->select_pile(1, 1);
                CB->evenement(1, -1);
                CB->animation("Sar_10");
                ST(V_EVENT_1) = 0;
                CB->stop_joindre(-P_MALROT4);
                CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
                CB->joindre(-P_MALROT4, 0, 0, 0, ST(ST_HERO));
                CB->modify_var(&ST(0xCB8), 1);
                ST(V_ENGAGED_PERSONA) = 0x1814;
                CB->combat(P_MALROT4, 0);
                CB->select_pile(0, 1);
            }
        }
    }
    /* 0xD231 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD299 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD20", P_ARKHAN2);
        }
        /* 0xD2BB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xD301 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD && CB->direction_perso(P_MALROT1)) {
        ST(V_CRI_USED) = 1;
        ST(0x4CC) = 1;
        /* 0xD386 */
        if (ST(0xF0) != 99) {
            ST(0xF0) = 61;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(35, 1000);
        return;
    }

}

/* trigger 25 - trig25_1000D3C0 */
static void trig25(void) {
    /* 0xD3CF */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(26)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD438 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xD44A */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D162", P_ARKHAN2, "SG00DIA01", NULL);
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xD49B */
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
        /* 0xD52A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xD53C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D279", P_ARKHAN3, "SG00DIA01", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xD58D */
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

/* trigger 27 - trig27_1000D5D7 */
static void trig27(void) {
    /* 0xD5E6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD63C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0xD664 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN2, "SG00ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

}

/* trigger 29 - trig29_1000D6B9 */
static void trig29(void) {
    /* 0xD6C8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(28)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD722 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xD734 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT28");
            }
            /* 0xD763 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0xD780 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xD7C6 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xD7D8 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT29");
            }
            /* 0xD807 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0xD824 */
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

/* trigger 30 - trig30_1000D863 */
static void trig30(void) {
    /* 0xD872 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x7D70)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD8CC */
        if (ST(0xBF4) == 0) {
            /* 0xD8DE */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L14D258", P_ARKHAN2, "SG00DIA05", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xD92F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L14D259", P_ARKHAN3, "SG00DIA05", NULL);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xD980 */
        if (ST(0xBF4) == 1) {
            /* 0xD992 */
            if (ST(0x560) == 0) {
                /* 0xD9A4 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L14D260", P_ARKHAN2, "SG00DIA05", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xD9F5 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L14D261", P_ARKHAN3, "SG00DIA05", NULL);
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xDA46 */
            if (ST(0x560) == 1) {
                /* 0xDA58 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->select_pile(1, 2);
                    CB->evenement(36, 550);
                    CB->select_pile(0, 2);
                    CB->animation("SP02ATT42");
                    CB->animation("ar_10");
                    /* 0xDAAD */
                    if (ST(V_GUARD_FIGHT_DONE) == 2) {
                        CB->bruitage("L14AMB4", -1, -1);
                    }
                    /* 0xDAD1 */
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
                /* 0xDB56 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->select_pile(1, 2);
                    CB->evenement(36, 450);
                    CB->select_pile(0, 2);
                    CB->animation("SP03ATT42");
                    CB->animation("ar_10");
                    /* 0xDBAB */
                    if (ST(V_GUARD_FIGHT_DONE) == 2) {
                        CB->bruitage("L14AMB4", -1, -1);
                    }
                    /* 0xDBCF */
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

/* trigger 31 - trig31_1000DC4D */
static void trig31(void) {
    /* 0xDC5C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x8F62)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDCB6 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L14D275A", P_ARKHAN2, "SG00DIA05", NULL);
            lieu_action_register("L14D275B", P_ARKHAN2, "SG00DIA05", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xDD1D */
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

/* trigger 33 - trig33_1000DD88 */
static void trig33(void) {
    /* 0xDD9B */
    if (CB->direction_perso(P_DELIA)) {
        /* 0xDDAA */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
            /* 0xDDDA */
            if (ST(ST_VIE(P_DELIA)) < 2) {
                ST(0x70) = 10;
                CB->stop_joindre(-P_DELIA);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->bloque(BLOQUE_ALL);
                /* 0xDE1A */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0xDE6F */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L14D238", P_ARKHAN2, "SG00DIA02", "Sar_10");
                    }
                    /* 0xDE9A */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L14D239", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L14D240", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                }
                /* 0xDF08 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0xDF5F */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xDF8A */
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
            /* 0xE02A */
            if (ST(ST_VIE(P_DELIA)) == VIE_DEAD) {
                CB->fouille(P_DELIA);
            }
        }
        /* 0xE044 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_DELIA) {
            /* 0xE074 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L10D93", P_ARKHAN2);
            }
            /* 0xE096 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L10D94", P_ARKHAN3);
            }
        }
        /* 0xE0B5 */
        if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_DELIA)) == 1) {
            ST(V_PLAYER_STRIKE) = -1;
            CB->select_pile(1, 2);
            CB->stop_joindre(-P_DELIA);
            CB->bloque(BLOQUE_ALL);
            /* 0xE129 */
            if (ST(V_DELIA_HITS) == 1) {
                ST(0x170) = 1;
                ST(V_DELIA_KILLED) = 14;
                ST(V_DELIA_PHASE) = 20;
                ST(ST_VIE(P_DELIA)) = VIE_DEAD;
                CB->animation("SP10IMP51");
                CB->animation("P10cad");
                CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10CAD", 0x21, 0);
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xE1CE */
            if (ST(V_DELIA_HITS) == 0) {
                ST(V_DELIA_HITS) = 1;
                CB->animation("SP10ESA01");
                CB->animation("P10ARR02_50");
                CB->dialogue("L10D95", P_DELIA);
                CB->animation("P10ARR02_50");
                CB->modify_var(&ST(0x6C), 1);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
        }
        return;
    }
    trig_noop();
    /* 0xE259 */
    if (CB->direction_zone(12) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 4)) {
        trig04();
    }
    /* 0xE291 */
    if (CB->direction_zone(13) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
    }
    /* 0xE2C9 */
    if (CB->direction_zone(6) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)) {
        trig05();
    }
    /* 0xE301 */
    if (CB->direction_zone(2) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
        trig02();
    }
    /* 0xE339 */
    if (CB->direction_zone(14) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
    }

}

/* trigger 34 - trig34_1000E369 */
static void trig34(void) {
    ST(V_SAVED_HERO) = ST(ST_HERO);
    ST(ST_LEVEL) = 14;
    CB->change_piece(46, -1);

}

/* trigger 35 - trig35_1000E3A7 */
static void trig35(void) {
    /* 0xE3B6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xBAC9)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE41F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00DIA05", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xE470 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN3, "SG00DIA05", NULL);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }

}

/* trigger 36 - trig36_1000E4BA */
static void trig36(void) {
    /* 0xE4C9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xFF0D)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE532 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0xE55A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN2, "SG00ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

}

/* trigger 37 - trig37_1000E5A4 */
static void trig37(void) {
    /* 0xE5B3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xFD13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE61C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0xE644 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN2, "SG00ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

}

/* trigger 38 - trig38_1000E68E */
static void trig38(void) {
    /* 0xE69D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(39)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xE706 */
        if (ST(0x178) == 0) {
            CB->animation("SG00PRE08");
            CB->etat_objet("OB34A", -1, -1, -1, -1, -1, -1, -3, 4);
            ST(0x178) = 1;
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xE779 */
        if (ST(0x178) == 1) {
            lieu_action_register("LCOD10", P_ARKHAN2, "SG00DIA05", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 41 - trig41_100039CC */
static void trig41(void) {
    /* 0x39E9 */
    if (ST(V_SAVED_HERO) != ST(ST_HERO) && ST(V_DELIA_PHASE) == 4 && ST(0xC44) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x3A36 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
            lieu_action_register("LCOD48", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            /* 0x3A7E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD50", P_ARKHAN2, "SG00DIA05", NULL);
            }
            /* 0x3AA6 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD51", P_ARKHAN3, "SG00DIA05", NULL);
            }
        }
        /* 0x3ACE */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
            lieu_action_register("LCOD44", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            /* 0x3B16 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD46", P_ARKHAN2, "SG00DIA05", NULL);
            }
            /* 0x3B3E */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD47", P_ARKHAN3, "SG00DIA05", NULL);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x3B7F */
    if ((ST(0xBE8) >= 1 || ST(V_GUARD_FIGHT_DONE) == 2) && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4 && ST(0xC44) == 0 && ST(0x170) != 1) {
        ST(0xCB0) = 1;
        CB->evenement(1, 1000);
    }
    /* 0x3C1C */
    if (ST(0xBE8) == 0 && ST(0xC44) == 0 && ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->bloque(BLOQUE_ALL);
        /* 0x3C5D */
        if (ST(ST_VIE(P_KALHI13)) == 1 && ST(ST_VIE(P_DANRYS)) != VIE_DEAD) {
            CB->divers(DV_RUN, P_KALHI13, 1);
            CB->dialogue("L14D108", P_KALHI13);
            ST(0x7C) = 1;
            ST(0xBE8) = 1;
        }
    }
    /* 0x3CBE */
    if ((ST(V_DELIA_PHASE) == 3 || ST(V_DELIA_PHASE) == 4) && ST(0xD4) == 0) {
        ST(V_DELIA_PHASE) = 4;
        ST(0xD4) = 1;
    }
    /* 0x3D12 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers_empile(DV_MUSIC, 1, 1);
    } else {
        CB->divers_empile(DV_MUSIC, 2, 1);
    }
    /* 0x3D4D */
    if (ST(0x4CC) == 1 && ST(0xC44) == 0) {
        /* 0x3D71 */
        if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
            ST(V_ENGAGED_PERSONA) = P_MALROT4;
            CB->combat(P_MALROT4, 0);
        }
        /* 0x3DAE */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            /* 0x3DC0 */
            if (ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4 && ST(ST_VIE(P_MALROT1)) == VIE_DEAD) {
                ST(V_ENGAGED_PERSONA) = P_MALROT4;
                CB->combat(P_MALROT4, 0);
            }
            /* 0x3E33 */
            if ((ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(ST_VIE(P_MALROT4)) == VIE_DEAD) || (ST(ST_VIE(P_MALROT4)) != VIE_DEAD && (ST(0x170) == 1 || ST(V_DELIA_PHASE) == 3 || ST(V_DELIA_PHASE) == 4))) {
                ST(V_ENGAGED_PERSONA) = P_MALROT1;
                CB->combat(P_MALROT1, 0);
            }
            /* 0x3EB8 */
            if (ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4) {
                ST(V_ENGAGED_PERSONA) = 0x1814;
                CB->combat(P_MALROT4, 0);
                CB->combat(P_MALROT1, 0);
            }
        }
    }
    /* 0x3F38 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers_empile(DV_SCREEN, 3, 0);
    }

}
