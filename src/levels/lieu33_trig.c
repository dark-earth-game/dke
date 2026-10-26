/*
 * lieu33_trig.c - Native hand-port of the LIEU33.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and events live in lieu33.c; this
 * file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries its
 * original address. Handlers call the engine only through g_lieu_cb.
 * LIEU33 is an older build of the level-10 script (same rooms, zones and
 * dialogue codes), so the triggers follow lieu10_trig.c with the
 * differences of this version.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu33_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig01(void); static void trig02(void); static void trig03(void);
static void trig04(void); static void trig05(void); static void trig06(void);
static void trig07(void); static void trig09(void); static void trig11(void);
static void trig12(void); static void trig14(void); static void trig15(void);
static void trig16(void); static void trig17(void); static void trig22(void);
static void trig29(void); static void trig30(void); static void trig31(void);
static void trig32(void); static void trig33(void); static void trig34(void);
static void trig36(void); static void trig37(void); static void trig38(void);
static void trig39(void); static void trig40(void); static void trig41(void);
static void trig_noop(void);   /* the 14 stub slots */

/* dword_10025930[42] - filled at INIT with the 5-byte jmp thunks at
 * 0x10001000+5*i (in a shuffled order); static here.
 *   idx handler        idx handler        idx handler
 *    0  10005FB0       14  1000E4A6       28  1000F6D5 stub
 *    1  1000C95E       15  1000F04B       29  1000F6E0
 *    2  1000CC7D       16  1000F380       30  1000F775
 *    3  1000CF9C       17  1000F449       31  1000FDE8
 *    4  1000D26E       18  1000F512 stub  32  10010685
 *    5  1000D4F6       19  1000F51D stub  33  10010D99
 *    6  1000D5CA       20  1000F528 stub  34  10010E57
 *    7  1000D894       21  1000F533 stub  35  10011052 stub
 *    8  1000DC14 stub  22  1000F53E       36  1001105D
 *    9  1000DC1F       23  1000F69E stub  37  10011350
 *   10  1000DC45 stub  24  1000F6A9 stub  38  1001151E
 *   11  1000DC50       25  1000F6B4 stub  39  1001161E
 *   12  1000DD5D       26  1000F6BF stub  40  100117EC
 *   13  1000E49B stub  27  1000F6CA stub  41  10007C14
 */
static void (*const g_trigger[42])(void) = {
    l33_trig00, trig01, trig02, trig03, trig04, trig05, trig06, trig07,
    trig_noop, trig09, trig_noop, trig11, trig12, trig_noop, trig14, trig15,
    trig16, trig17, trig_noop, trig_noop, trig_noop, trig_noop, trig22,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig29,
    trig30, trig31, trig32, trig33, trig34, trig_noop, trig36, trig37,
    trig38, trig39, trig40, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10025930]). */
void l33_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 - sub_10005FB0 */
void l33_trig00(void) {
    i32 ra, ry;   /* place_animal: random(65535) is evaluated before random(100) */

    /* 0x5FC2 */
    if (ST(ST_START_POINT(10)) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 8;
        ST(0xB54) = 3;
        ST(0xAF4) = 0;
        ST(0xEC8) = 0;
    }
    /* 0x601C */
    if (ST(ST_START_POINT(10)) == 2) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 8;
        ST(0xB54) = 3;
        ST(0xAF4) = 0;
        ST(0xEC8) = 1;
    }
    /* 0x6076 */
    if (ST(ST_START_POINT(10)) == 3) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 8;
        ST(0xB54) = 3;
        ST(0xAF4) = 2;
        ST(0xEC8) = 0;
    }
    /* 0x60D0 */
    if (ST(ST_START_POINT(10)) == 4) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 8;
        ST(0xB54) = 3;
        ST(0xAF4) = 2;
        ST(0xEC8) = 1;
    }
    /* 0x612A */
    if (ST(ST_START_POINT(10)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 23;
        ST(V_DELIA_PHASE) = 0;
        ST(0xAF8) = 1;
    }
    /* 0x6175 */
    if (ST(ST_START_POINT(10)) == 6) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_LEVEL) = 14;
        ST(ST_VIE(P_GUARDIAN15)) = 4;
        ST(V_DELIA_PHASE) = 5;
    }
    /* 0x61CF */
    if (ST(ST_START_POINT(10)) == 7) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 8;
        ST(ST_VIE(P_KALHI13)) = 1;
        ST(ST_VIE(P_KALHI33)) = 0;
        ST(0xE9C) = 7;
        ST(0xB54) = 3;
        ST(V_DELIA_PHASE) = 0;
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = ST(0x12C);
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
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_EVENT_33) = 0;
    ST(V_EVENT_34) = 0;
    ST(V_EVENT_35) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
    /* 0x6465 */
    if (ST(0xC08) == 3) {
        ST(0xC08) = 10;
    }
    CB->select_pile(2, 2);
    CB->select_pile(1, 2);
    CB->select_pile(0, 2);
    /* 0x64B3 */
    if (ST(0xB40) == 2 && ST(V_KALHI13_TALK_C) == 3) {
        ST(ST_VIE(P_KALHI13)) = 4;
        ST(ST_VIE(P_KALHI33)) = 4;
    }
    /* 0x64F5 */
    if (ST(0xB40) == 4 && ST(0x6DC) == 4) {
        ST(ST_VIE(P_GUARDIAN25)) = 4;
        ST(0x6DC) = 5;
    }
    /* 0x6537 */
    if (ST(0xBA8) == 4) {
        ST(0xBA8) = 5;
    }
    /* 0x6558 */
    if (ST(ST_VIE(P_GUARDIAN15)) == VIE_DEAD && ST(V_GUARD_FIGHT_DONE) >= 2) {
        ST(ST_VIE(P_GUARDIAN15)) = 4;
    }
    CB->ajoute_zone(1, 2550, -1100, 60, 450, 1);
    CB->ajoute_zone(2, 2550, -1622, 60, 450, 1);
    CB->ajoute_zone(3, 5170, -2620, 70, 180, 1);
    CB->ajoute_zone(4, 5654, -1350, 70, 500, 1);
    CB->ajoute_zone(6, 4475, -3420, 800, 70, 1);
    CB->ajoute_zone(14, 4380, -1243, 140, 140, 0);
    CB->ajoute_zone(15, 4400, -1422, 140, 140, 0);
    CB->ajoute_zone(16, 4595, -1416, 140, 140, 0);
    CB->ajoute_zone(17, 4590, -1250, 140, 140, 0);
    CB->ajoute_zone(18, 4462, -1322, 60, 60, 0);
    CB->ajoute_zone(11, 3406, -900, 150, 150, 0);
    /* 0x66E1 */
    if (ST(0xB4C) <= 1) {
        CB->bruitage("L10AMB1", 4380, -1243);
    }
    CB->evenement(0, -1);
    CB->evenement(1, -1);
    CB->evenement(2, -1);
    CB->evenement(30, -1);
    CB->evenement(32, -1);
    CB->evenement(33, -1);
    CB->evenement(34, -1);
    CB->evenement(35, -1);
    CB->evenement(36, -1);
    CB->evenement(37, -1);
    CB->evenement(38, -1);
    CB->evenement(39, -1);
    /* 0x67C2 */
    if (ST(ST_LEVEL) == 8) {
        /* 0x67D4 */
        if (ST(0xB54) == 3) {
            CB->pos_perso(ST(ST_HERO), 2712, -1622, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
        }
        /* 0x6825 */
        if (ST(0xB54) == 4) {
            CB->pos_perso(ST(ST_HERO), 2712, -1100, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
        }
    }
    /* 0x6873 */
    if (ST(ST_LEVEL) == 11) {
        CB->pos_perso(ST(ST_HERO), 5495, -1300, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
    }
    /* 0x68C1 */
    if (ST(ST_LEVEL) == 14) {
        CB->pos_perso(ST(ST_HERO), 4450, 800, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
    }
    /* 0x690F */
    if (ST(ST_LEVEL) == 20) {
        CB->pos_perso(ST(ST_HERO), 4475, -3300, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
    }
    /* 0x695D */
    if (ST(ST_LEVEL) == 23) {
        CB->pos_perso(ST(ST_HERO), 5040, -2655, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 0xA, 0);
        /* 0x69AE */
        if (ST(0xAF8) != 0 && ST(0xF14) == 0 && ST(0xE58) == 0) {
            CB->change_camera(4, CAM_LOCKED);
            CB->bloque(BLOQUE_ALL);
            ST(0xF14) = 1;
            CB->free_anim(-10);
            CB->load_anim("GAR20", 0);
            CB->load_anim("GAR20par", 1);
            CB->load_anim("GAR45", 2);
            CB->load_anim("GAR45par", 3);
            CB->load_anim("GAR46", 4);
            CB->load_anim("GAR46par", 5);
            CB->load_anim("GARban", 6);
            CB->load_anim("GARbanpa", 7);
            CB->load_anim("Archi", 8);
            CB->load_anim("Archipar", 9);
            CB->pos_perso(P_SUNSEER1, 4490, -2860, 0x640, 1, "P18ARR01", 0x22, 0);
            CB->pos_perso(P_GUARDIAN15, 4500, -1980, 0x79B8, 1, "P55ARR04", 0x21, 0);
            ST(0xE58) = 10;
        }
    }
    /* 0x6B0F */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->ajoute_zone(29, 4476, 422, 325, 80, 1);
        /* 0x6B3F */
        if (ST(0xAF4) == 0 && ST(0xEC8) == 0) {
            CB->pos_perso(P_LORY, 4300, -2300, 0xB798, 1, "P15ARR01", 0x1A, 0);
            CB->pos_perso(P_RYLSADHAR20, 4720, -3270, 0xFA0, 1, "P20ARR01", 0x1B, 0);
            CB->link_objet(P_RYLSADHAR20, "OB02", 0x1B, 1);
            CB->ajoute_zone(19, 4420, -2720, 100, 100, 1);
            CB->ajoute_zone(20, 4647, -3520, 100, 100, 1);
            CB->ajoute_zone(21, 5320, -2640, 70, 70, 1);
        }
        /* 0x6C12 */
        if (ST(0xAF4) == 1 && ST(0xEC8) == 0) {
            CB->pos_perso(P_LORY, 4380, -2760, 0xD228, 1, "P15ARR01", 0x1A, 0);
            CB->pos_perso(P_RYLSADHAR20, 4500, -2720, 0x2EE0, 1, "P20ARR01", 0x1B, 0);
            CB->link_objet(P_RYLSADHAR20, "OB02", 0x1B, 1);
            CB->ajoute_zone(19, 4420, -2720, 100, 100, 1);
            CB->ajoute_zone(20, 4647, -3520, 100, 100, 1);
            CB->ajoute_zone(21, 5320, -2640, 70, 70, 1);
        }
        /* 0x6CE5 */
        if (ST(0xAF4) >= 2 && ST(0xEC8) == 0) {
            ST(0x7C) = 1;
            CB->pos_perso(P_ZED84, 4540, 304, 0, 1, "P84ARR01", 0xC, 0);
            CB->link_objet(P_ZED84, "OB68", 0x1D, 1);
        }
        /* 0x6D4B */
        if (ST(0xEC8) == 1) {
            CB->ajoute_zone(24, 2900, -1130, 10, 10, 0);
            CB->ajoute_zone(25, 2888, -1493, 10, 10, 0);
            /* 0x6D93 */
            if (ST(0xAF4) >= 3) {
                CB->pos_perso(P_ZED84, 3333, -1300, 0x3C8C, 1, "P84ARR01", 0xC, 0);
            }
            /* 0x6DCA */
            if (ST(0xAF4) < 3) {
                CB->pos_perso(P_ZED84, 1979, -1148, 0xBD5F, 1, "P84ARR01", 0xC, 0);
            }
            CB->link_objet(P_ZED84, "OB68", 0x1D, 1);
        }
    }
    /* 0x6E15 */
    if (ST(ST_HERO) > 1) {
        CB->ajoute_zone(7, 4312, 754, 120, 100, 0);
    }
    /* 0x6E42 */
    if (ST(ST_HERO) >= 2) {
        /* 0x6E54 */
        if (ST(0xE9C) == 7 && ST(0xEF0) == 0) {
            ST(0xEF4) = 0;
            /* 0x6E87 */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                ST(0x6C) = 1;
                ST(0x74) = 1;
                /* 0x6EB1 */
                if (ST(0xB54) == 3) {
                    CB->pos_perso(P_KALHI13, 2500, -1490, 0xBB80, 1, "P13ARR01", 0x1F, 0);
                }
                /* 0x6EE8 */
                if (ST(0xB54) == 4) {
                    CB->pos_perso(P_KALHI13, 2500, -1150, 0xBB80, 1, "P13ARR01", 0x1F, 0);
                }
            } else if (ST(0xB54) != 3 && ST(0xB54) != 4) {
                CB->pos_perso(P_KALHI13, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 1, "P13ARR03", 0x1F, 0);
            }
            /* 0x6F98 */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->ajoute_zone(35, 4240, -780, 100, 100, 1);
                /* 0x6FC5 */
                if (ST(0xB54) == 3) {
                    CB->pos_perso(P_KALHI33, 2912, -1622, 0x7530, 1, "P33ARR01", 0x20, 0);
                }
                /* 0x6FFC */
                if (ST(0xB54) == 4) {
                    CB->pos_perso(P_KALHI33, 2912, -1100, 0x7530, 1, "P33ARR01", 0x20, 0);
                }
            }
        }
        /* 0x7033 */
        if (ST(0xE9C) == 7 && ST(0xEF0) == 1) {
            /* 0x7057 */
            if (ST(0xEF4) == 0) {
                ST(0x80) = 1;
                /* 0x7078 */
                if (ST(ST_VIE(P_KALHI13)) == 1) {
                    CB->pos_perso(P_KALHI13, 4240, -800, CB->divers(DV_DESC_POS, P_KALHI13, 2), 1, "P13ARR01", 0x1F, 0);
                }
                /* 0x70BD */
                if (ST(ST_VIE(P_KALHI33)) == 1) {
                    CB->pos_perso(P_KALHI33, 4240, -800, CB->divers(DV_DESC_POS, P_KALHI33, 2), 1, "P33ARR01", 0x20, 0);
                }
            }
            /* 0x7102 */
            if (ST(0xEF4) == 1) {
                ST(0xE58) = 1;
                /* 0x7123 */
                if (ST(ST_VIE(P_KALHI13)) == 1) {
                    ST(ST_VIE(P_KALHI13)) = VIE_DEAD;
                    CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13cad", 0x1F, 0);
                }
                /* 0x7160 */
                if (ST(ST_VIE(P_KALHI33)) == 1) {
                    ST(ST_VIE(P_KALHI33)) = VIE_DEAD;
                    CB->pos_perso(P_KALHI33, 0, 0, 0, 2, "P33cad", 0x20, 0);
                }
            }
            /* 0x719D */
            if (ST(0xEF4) == 2 || ST(0xEF4) == 3) {
                ST(0xE0) = 4;
                /* 0x71D0 */
                if (ST(ST_VIE(P_KALHI13)) == 1) {
                    ST(0x94) = 9;
                    CB->pos_perso(P_KALHI13, 4312, 754, 0x4650, 1, "P13ARR01", 0x1F, 0);
                }
                /* 0x7216 */
                if (ST(ST_VIE(P_KALHI33)) == 1) {
                    ST(0x94) = 9;
                    CB->pos_perso(P_KALHI33, 4312, 754, 0x4650, 1, "P33ARR01", 0x20, 0);
                }
            }
        }
        /* 0x725C */
        if (ST(0xE9C) == 8) {
            ST(0x84) = 1;
            /* 0x727D */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->pos_perso(P_KALHI13, 2920, -1200, 0xC544, 1, "P13ARR03", 0x1F, 0);
            }
            /* 0x72B4 */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->pos_perso(P_KALHI33, 2920, -1200, 0xC544, 1, "P33ARR03", 0x20, 0);
            }
            /* 0x72EB */
            if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
                CB->pos_perso(P_KALHI13, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 2, "P13cad", 0x1F, 0);
            }
            /* 0x734D */
            if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
                CB->pos_perso(P_KALHI33, CB->divers(DV_DESC_POS, P_KALHI33, 0), CB->divers(DV_DESC_POS, P_KALHI33, 1), CB->divers(DV_DESC_POS, P_KALHI33, 2), 2, "P33cad", 0x20, 0);
            }
        }
        /* 0x73AF */
        if (ST(ST_VIE(P_GUARDIAN15)) <= 1) {
            /* 0x73C1 */
            if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD || ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
                ST(0xD8) = 1;
                ST(0xE58) = 1;
                /* 0x7400 */
                if (ST(ST_LEVEL) == 8) {
                    CB->pos_perso(P_GUARDIAN15, 3333, -1300, 0x3C8C, 1, "P55GAR05", 0x21, 0);
                }
                /* 0x7434 */
                if (ST(ST_LEVEL) == 20) {
                    CB->pos_perso(P_GUARDIAN15, 4494, -2634, 0x7148, 1, "P55GAR05", 0x21, 0);
                }
                /* 0x7468 */
                if (ST(ST_LEVEL) == 23) {
                    CB->pos_perso(P_GUARDIAN15, 4494, -2634, 0xBB80, 1, "P55GAR05", 0x21, 0);
                }
                /* 0x749C */
                if (ST(ST_LEVEL) == 11) {
                    CB->pos_perso(P_GUARDIAN15, 5036, -1254, 0xAFC8, 1, "P55GAR05", 0x21, 0);
                }
            } else {
                /* 0x74D8 */
                if (ST(0xEF4) == 2 || ST(0xEF4) == 3) {
                    CB->pos_perso(P_GUARDIAN15, 4420, -140, 0x7530, 1, "P55ARR04", 0x21, 0);
                } else {
                    CB->pos_perso(P_GUARDIAN15, 4480, 275, 0x88B8, 1, "P55ARR04", 0x21, 0);
                }
                ST(0xE58) = 0;
            }
        }
        /* 0x755A */
        if (ST(ST_VIE(P_GUARDIAN15)) == VIE_DEAD) {
            CB->pos_perso(P_GUARDIAN15, CB->divers(DV_DESC_POS, P_GUARDIAN15, 0), CB->divers(DV_DESC_POS, P_GUARDIAN15, 1), CB->divers(DV_DESC_POS, P_GUARDIAN15, 2), 2, "P55cad", 0x21, 0);
        }
        /* 0x75BC */
        if (ST(V_DELIA_PHASE) == 5) {
            CB->ajoute_zone(5, 4472, 400, 335, 90, 1);
            CB->ajoute_zone(9, 4472, 150, 335, 90, 1);
            CB->pos_perso(P_DELIA, 4600, 600, 0, 1, "P10ARR01", 0x1E, 0);
            /* 0x762C */
            if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD && ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
                ST(ST_VIE(P_KALHI13)) = 4;
                ST(ST_VIE(P_KALHI33)) = 4;
            }
        }
        /* 0x766E */
        if (ST(V_DELIA_PHASE) == 6) {
            CB->ajoute_zone(5, 4472, 400, 335, 90, 1);
            /* 0x769B */
            if (ST(ST_LEVEL) != 14) {
                ST(0x6C) = 1;
            }
            CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 1, "P10ARR01", 0x1E, 0);
            /* 0x7706 */
            if (ST(ST_LEVEL) == 23) {
                CB->pos_perso(P_DELIA, 4590, -2637, 0xD1C1, 1, "P10ARR01", 0x1E, 0);
            }
        }
        /* 0x773D */
        if (ST(0xB4C) > 0) {
            /* 0x774F */
            if (ST(0xB4C) == 1) {
                CB->load_anim("ouvgril", 0);
                CB->load_anim("grouvc00", 1);
                CB->load_anim("grouvc01", 2);
                CB->load_anim("grouvc05", 3);
            }
            /* 0x779E */
            if (ST(0x4E8) == 0) {
                ST(ST_VIE(85)) = 1;
            }
            /* 0x77BF */
            if (ST(ST_VIE(85)) == 1) {
                CB->pos_perso(85, 4405, -646, 0x9470, 1, "P85cad39", 0x22, 0);
            }
            /* 0x77F6 */
            if (ST(ST_VIE(85)) == VIE_DEAD) {
                CB->pos_perso(85, 4405, -646, 0x9470, 2, "P85cad38", 0x22, 0);
            }
            CB->ajoute_zone(36, 4727, -500, 310, 310, 0);
            CB->ajoute_zone(37, 4560, -726, 200, 200, 0);
            CB->ajoute_zone(38, 4750, -755, 200, 200, 0);
            CB->ajoute_zone(39, 5053, -600, 200, 200, 0);
            CB->ajoute_zone(40, 4500, -200, 200, 200, 0);
        }
    }
    /* 0x78D2 */
    if (ST(V_DELIA_KILLED) == 10) {
        CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 2, "P10cad", 0x1E, 0);
    }
    /* 0x7934 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->ajoute_zone(22, 4470, 340, 340, 110, 0);
        CB->vire_col(10, 0);
    }
    /* 0x7971 */
    if (ST(ST_HERO) > 1) {
        CB->vire_col(9, 0);
    }
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 3000, ry, -1200, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 3550, ry, -1300, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 4000, ry, -1400, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 4500, ry, -1500, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 5000, ry, -1600, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 5500, ry, -1700, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 2500, ry, -1800, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 3000, ry, -2000, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 3500, ry, -2200, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 4000, ry, -2400, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 4500, ry, -2600, ra, 1000);
    ra = CB->random(65535);
    ry = CB->random(100) - 100;
    CB->place_animal(4, 5000, ry, -2800, ra, 1000);
    ST(ST_START_POINT(10)) = 0;
}

/* trigger 01 - sub_1000C95E */
static void trig01(void) {
    CB->select_pile(0, 2);
    CB->divers_empile(DV_MUSIC, 1, 4);
    CB->retire_zone(1);
    /* 0xC99A */
    if (ST(0xE48) == 0 && ST(0xB4C) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(V_DELIA_PHASE) == 20 && (ST(V_DELIA_KILLED) == 16 || ST(V_DELIA_KILLED) == 19 || ST(V_DELIA_KILLED) == 14 || ST(V_DELIA_KILLED) == 10)) {
        ST(0xE48) = 1;
    }
    /* 0xCA39 */
    if (ST(0xEC8) != 0 && ST(0x338) == 0 && ST(0x348) == 0) {
        ST(0x64) = 1;
    } else {
        /* 0xCA80 */
        if (ST(V_DELIA_PHASE) == 5) {
            ST(V_DELIA_PHASE) = 6;
        }
        /* 0xCAA1 */
        if (ST(0xE9C) == 7 && ST(0xF0) == 0 && ST(0xEF4) == 0) {
            CB->bloque(BLOQUE_ALL);
            ST(V_SAVED_HERO) = ST(ST_HERO);
            ST(0xF0) = 1;
            ST(0x6C) = 100;
            /* 0xCB14 */
            if (ST(ST_VIE(P_KALHI13)) == 1 && CB->distance(10, 31) < 1000) {
                CB->stop_joindre(-P_KALHI13);
                lieu_action_register("L10D109A", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                lieu_action_register("L10D109B", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
            }
            /* 0xCB7E */
            if (ST(ST_VIE(P_KALHI33)) == 1 && CB->distance(10, 32) < 1000) {
                CB->stop_joindre(-P_KALHI33);
                lieu_action_register("L10D109A", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
                lieu_action_register("L10D109B", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
            }
            CB->bloque(BLOQUE_NONE);
        }
        ST(0xB54) = 2;
        ST(ST_LEVEL) = 10;
        /* 0xCC0B */
        if (ST(V_DELIA_PHASE) == 6 || ST(0xF20) == 1 || ST(0xE48) != 0) {
            CB->change_piece(8, 1);
        } else {
            CB->change_piece(8, 0);
        }
        ST(0x12C) = ST(0x6C);
    }
    CB->select_pile(0, 1);
}

/* trigger 02 - sub_1000CC7D */
static void trig02(void) {
    CB->select_pile(0, 2);
    CB->divers_empile(DV_MUSIC, 1, 4);
    CB->retire_zone(2);
    /* 0xCCB9 */
    if (ST(0xE48) == 0 && ST(0xB4C) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(V_DELIA_PHASE) == 20 && (ST(V_DELIA_KILLED) == 16 || ST(V_DELIA_KILLED) == 19 || ST(V_DELIA_KILLED) == 14 || ST(V_DELIA_KILLED) == 10)) {
        ST(0xE48) = 1;
    }
    /* 0xCD58 */
    if (ST(0xEC8) != 0 && ST(0x338) == 0 && ST(0x348) == 0) {
        ST(0x64) = 1;
    } else {
        /* 0xCD9F */
        if (ST(V_DELIA_PHASE) == 5) {
            ST(V_DELIA_PHASE) = 6;
        }
        /* 0xCDC0 */
        if (ST(0xE9C) == 7 && ST(0xF0) == 0 && ST(0xEF4) == 0) {
            CB->bloque(BLOQUE_ALL);
            ST(V_SAVED_HERO) = ST(ST_HERO);
            ST(0xF0) = 1;
            ST(0x6C) = 100;
            /* 0xCE33 */
            if (ST(ST_VIE(P_KALHI13)) == 1 && CB->distance(10, 31) < 1000) {
                CB->stop_joindre(-P_KALHI13);
                lieu_action_register("L10D109A", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                lieu_action_register("L10D109B", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
            }
            /* 0xCE9D */
            if (ST(ST_VIE(P_KALHI33)) == 1 && CB->distance(10, 32) < 1000) {
                CB->stop_joindre(-P_KALHI33);
                lieu_action_register("L10D109A", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
                lieu_action_register("L10D109B", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
            }
            CB->bloque(BLOQUE_NONE);
        }
        ST(0xB54) = 1;
        ST(ST_LEVEL) = 10;
        /* 0xCF2A */
        if (ST(V_DELIA_PHASE) == 6 || ST(0xF20) == 1 || ST(0xE48) != 0) {
            CB->change_piece(8, 1);
        } else {
            CB->change_piece(8, 0);
        }
        ST(0x12C) = ST(0x6C);
    }
    CB->select_pile(0, 1);
}

/* trigger 03 - sub_1000CF9C */
static void trig03(void) {
    CB->select_pile(0, 2);
    CB->divers(DV_MUSIC, 1, 4);
    CB->retire_zone(3);
    /* 0xCFD8 */
    if (ST(V_LEFT_HOME) == 0) {
        ST(V_LEFT_HOME) = 1;
    }
    /* 0xCFF9 */
    if (ST(V_DELIA_PHASE) == 6) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SAVED_HERO) = ST(ST_HERO);
        ST(0x6C) = 10;
        CB->stop_joindre(-P_DELIA);
        CB->animation("SP10ARR01_10");
        /* 0xD04F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L10D02A", P_ARKHAN2);
        }
        /* 0xD071 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L10D02B", P_ARKHAN3);
        }
        lieu_action_register("L10D03A", P_DELIA, "SP10DIA53", "SP10ARR01_10");
        lieu_action_register("L10D03B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xD0CD */
    if (ST(0xE9C) == 7 && ST(0xF0) == 0 && ST(0xEF4) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SAVED_HERO) = ST(ST_HERO);
        ST(0xF0) = 1;
        ST(0x6C) = 10;
        /* 0xD140 */
        if (ST(ST_VIE(P_KALHI13)) == 1 && CB->distance(10, 31) < 1000) {
            CB->stop_joindre(-P_KALHI13);
            lieu_action_register("L10D109A", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
            lieu_action_register("L10D109B", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
        }
        /* 0xD1AA */
        if (ST(ST_VIE(P_KALHI33)) == 1 && CB->distance(10, 32) < 1000) {
            CB->stop_joindre(-P_KALHI33);
            lieu_action_register("L10D109A", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
            lieu_action_register("L10D109B", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
        }
        CB->bloque(BLOQUE_NONE);
    }
    ST(ST_LEVEL) = 10;
    /* 0xD228 */
    if (ST(0xB4C) < 3) {
        CB->change_piece(23, 0);
    } else {
        CB->change_piece(23, 1);
    }
    ST(0x12C) = ST(0x6C);
}

/* trigger 04 - sub_1000D26E */
static void trig04(void) {
    CB->select_pile(0, 2);
    CB->divers(DV_MUSIC, 1, 4);
    CB->retire_zone(4);
    /* 0xD2AA */
    if (ST(V_DELIA_PHASE) == 6) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SAVED_HERO) = ST(ST_HERO);
        CB->stop_joindre(-P_DELIA);
        ST(0x6C) = 10;
        CB->animation("SP10ARR01_10");
        /* 0xD300 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L10D02A", P_ARKHAN2);
        }
        /* 0xD322 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L10D02B", P_ARKHAN3);
        }
        lieu_action_register("L10D03A", P_DELIA, "SP10DIA53", "SP10ARR01_10");
        lieu_action_register("L10D03B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xD37E */
    if (ST(0xE9C) == 7 && ST(0xF0) == 0 && ST(0xEF4) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SAVED_HERO) = ST(ST_HERO);
        ST(0xF0) = 1;
        ST(0x6C) = 10;
        /* 0xD3F1 */
        if (ST(ST_VIE(P_KALHI13)) == 1 && CB->distance(10, 31) < 1000) {
            CB->stop_joindre(-P_KALHI13);
            lieu_action_register("L10D109A", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
            lieu_action_register("L10D109B", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
        }
        /* 0xD45B */
        if (ST(ST_VIE(P_KALHI33)) == 1 && CB->distance(10, 32) < 1000) {
            CB->stop_joindre(-P_KALHI33);
            lieu_action_register("L10D109A", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
            lieu_action_register("L10D109B", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
        }
        CB->bloque(BLOQUE_NONE);
    }
    ST(ST_LEVEL) = 10;
    CB->change_piece(11, -1);
    ST(0x12C) = ST(0x6C);
}

/* trigger 05 - sub_1000D4F6 */
static void trig05(void) {
    /* 0xD508 */
    if (ST(V_DELIA_PHASE) == 6 && ST(0x6C) == 1) {
        ST(0x6C) = 10;
        CB->stop_joindre(-P_DELIA);
        CB->animation("SP10ARR01_10");
        /* 0xD54B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L10D02A", P_ARKHAN2);
        }
        /* 0xD56D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L10D02B", P_ARKHAN3);
        }
        lieu_action_register("L10D03A", P_DELIA, "SP10DIA53", "SP10ARR01_10");
        lieu_action_register("L10D03B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
        CB->retire_zone(3);
    }
}

/* trigger 06 - sub_1000D5CA */
static void trig06(void) {
    CB->select_pile(0, 2);
    CB->divers(DV_MUSIC, 1, 4);
    CB->retire_zone(6);
    /* 0xD606 */
    if (ST(0xBD0) == 1) {
        ST(0xBD0) = 2;
    }
    /* 0xD627 */
    if (ST(V_LEFT_HOME) == 0) {
        ST(V_LEFT_HOME) = 1;
    }
    /* 0xD648 */
    if (ST(V_DELIA_PHASE) == 6) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SAVED_HERO) = ST(ST_HERO);
        CB->stop_joindre(-P_DELIA);
        ST(0x6C) = 10;
        CB->animation("SP10ARR01_10");
        /* 0xD69E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L10D02A", P_ARKHAN2);
        }
        /* 0xD6C0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L10D02B", P_ARKHAN3);
        }
        lieu_action_register("L10D03A", P_DELIA, "SP10DIA53", "SP10ARR01_10");
        lieu_action_register("L10D03B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xD71C */
    if (ST(0xE9C) == 7 && ST(0xF0) == 0 && ST(0xEF4) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SAVED_HERO) = ST(ST_HERO);
        ST(0xF0) = 1;
        ST(0x6C) = 10;
        /* 0xD78F */
        if (ST(ST_VIE(P_KALHI13)) == 1 && CB->distance(10, 31) < 1000) {
            CB->stop_joindre(-P_KALHI13);
            lieu_action_register("L10D109A", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
            lieu_action_register("L10D109B", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
        }
        /* 0xD7F9 */
        if (ST(ST_VIE(P_KALHI33)) == 1 && CB->distance(10, 32) < 1000) {
            CB->stop_joindre(-P_KALHI33);
            lieu_action_register("L10D109A", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
            lieu_action_register("L10D109B", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
        }
        CB->bloque(BLOQUE_NONE);
    }
    ST(ST_LEVEL) = 10;
    ST(0x12C) = ST(0x6C);
    CB->change_piece(20, -1);
}

/* trigger 07 - sub_1000D894 */
static void trig07(void) {
    /* 0xD8A3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(0x50) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x4650)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD914 */
        if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
            /* 0xD926 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02lev07");
            }
            /* 0xD943 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03lev07");
            }
        }
        /* 0xD968 */
        if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
            /* 0xD97A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02lev06");
            }
            /* 0xD997 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03lev06");
            }
        }
        /* 0xD9B4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("P02_65535");
        }
        /* 0xD9D1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("P03_65535");
        }
        CB->bruitage("L10B01", 4310, 1040);
        /* 0xDA03 */
        if (ST(V_DELIA_PHASE) == 6 && ST(0x6C) != 10) {
            ST(V_SAVED_HERO) = ST(ST_HERO);
            /* 0xDA3B */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L10D02A", P_ARKHAN2);
            }
            /* 0xDA5D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L10D02B", P_ARKHAN3);
            }
            lieu_action_register("L10D03A", P_DELIA, "SP10DIA53", "SP10ARR01_10");
            lieu_action_register("L10D03B", P_DELIA, "SP10DIA54", "SP10ARR01_10");
        }
        CB->animation("Sar_10");
        /* 0xDABC */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
            CB->divers_empile(DV_BIG_ANIM, 2, 0);
        }
        /* 0xDAE0 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->divers_empile(DV_BIG_ANIM, 0, 0);
        }
        CB->divers_empile(DV_MUSIC, 1, 4);
        ST(ST_LEVEL) = 10;
        CB->bloque(BLOQUE_NONE);
        ST(0x12C) = ST(0x6C);
        CB->change_piece(14, -1);
    }
    /* 0xDB48 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x50) == 1 && CB->direction_spec(0x4650)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xDBA2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L11D06", P_ARKHAN2, "SG02ATT01", NULL);
        }
        /* 0xDBCA */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L11D07", P_ARKHAN3, "SG03ATT01", NULL);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 09 - sub_1000DC1F */
static void trig09(void) {
    /* 0xDC2E */
    if (ST(0x6C) != 1) {
        ST(0x6C) = 1;
    }
}

/* trigger 11 - sub_1000DC50 */
static void trig11(void) {
    /* 0xDC5F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x5208)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xDCBC */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xDCE7 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xDD12 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 12 - sub_1000DD5D */
static void trig12(void) {
    /* 0xDD70 */
    if (CB->direction_perso(P_ZED84)) {
        /* 0xDD7F */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->stop_joindre(-P_ZED84);
            CB->animation("SP84ARR01_10");
            CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
            /* 0xDDEA */
            if (ST(0x7D4) == 0) {
                ST(0x7D4) = 1;
                /* 0xDE0B */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L10D16", P_ARKHAN1, "SG00DIA04", NULL);
                }
                /* 0xDE33 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L10D18A", P_ARKHAN1, "SG00DIA04", NULL);
                    lieu_action_register("L10D18B", P_ARKHAN1, "SG00DIA04", NULL);
                    lieu_action_register("L10D19", P_ZED84, "SP84DIA59", "SP84ARR01_10");
                }
                lieu_action_register("L10D17A", P_ZED84, "SP84DIA54", "SP84ARR01_10");
                lieu_action_register("L10D17B", P_ZED84, "SP84DIA54", "SP84ARR01_10");
                CB->animation("Sar_10");
                /* 0xDEC4 */
                if (ST(0x7C) == 0) {
                    CB->joindre(-P_ZED84, 0, 0, 0xA, -1);
                    ST(V_DLG_RANDOM) = CB->random(3);
                    /* 0xDF00 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L10D70", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                    /* 0xDF2B */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L10D71", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                    /* 0xDF56 */
                    if (ST(V_DLG_RANDOM) == 2) {
                        lieu_action_register("L10D72", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xDF9F */
            if (ST(0x7D4) == 1) {
                ST(0x7D4) = 2;
                /* 0xDFC0 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L10D20", P_ARKHAN1, "SG00DIA04", NULL);
                }
                /* 0xDFE8 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L10D22", P_ARKHAN1, "SG00DIA04", NULL);
                }
                lieu_action_register("L10D21A", P_ZED84, "SP84DIA54", "SP84ARR01_10");
                lieu_action_register("L10D21B", P_ZED84, "SP84DIA56", "SP84ARR01_10");
                lieu_action_register("L10D21C", P_ZED84, "SP84DIA55", "SP84ARR01_10");
                CB->animation("Sar_10");
                /* 0xE063 */
                if (ST(0x7C) == 0) {
                    CB->joindre(-P_ZED84, 0, 0, 0xA, -1);
                    ST(V_DLG_RANDOM) = CB->random(3);
                    /* 0xE09F */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L10D70", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                    /* 0xE0CA */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L10D71", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                    /* 0xE0F5 */
                    if (ST(V_DLG_RANDOM) == 2) {
                        lieu_action_register("L10D72", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xE13E */
            if (ST(0x7D4) == 2) {
                /* 0xE150 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L10D24A", P_ARKHAN1, "SG00DIA04", NULL);
                    lieu_action_register("L10D24B", P_ARKHAN1, "SG00DIA02", NULL);
                }
                /* 0xE18E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L10D26A", P_ARKHAN1, "SG00DIA04", NULL);
                    lieu_action_register("L10D26B", P_ARKHAN1, "SG00DIA02", NULL);
                }
                lieu_action_register("L10D25A", P_ZED84, "SP84DIA54", "SP84ARR01_10");
                lieu_action_register("L10D25B", P_ZED84, "SP84DIA54", "SP84ARR01_10");
                lieu_action_register("L10D25C", P_ZED84, "SP84DIA56", "SP84ARR01_10");
                CB->animation("Sar_10");
                /* 0xE21F */
                if (ST(0x7C) == 0) {
                    CB->joindre(-P_ZED84, 0, 0, 0xA, -1);
                    ST(V_DLG_RANDOM) = CB->random(3);
                    /* 0xE25B */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L10D70", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                    /* 0xE286 */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L10D71", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                    /* 0xE2B1 */
                    if (ST(V_DLG_RANDOM) == 2) {
                        lieu_action_register("L10D72", P_ZED84, "SP84ACT06", "SP84ARR01_10");
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xE2F7 */
        if (ST(V_PLAYER_STRIKE) == 1 && ST(V_ENGAGED_PERSONA) == 0) {
            ST(V_PLAYER_STRIKE) = -1;
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_ZED84);
            ST(0x7C) = 10;
            CB->animation("SP84ESA13");
            CB->animation("P84_65535");
            CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
            /* 0xE379 */
            if (ST(0x7F4) == 0) {
                ST(0x7F4) = 1;
                lieu_action_register("L10D27", P_ZED84, "SP84DIA59", "SP84ARR02_10");
                CB->modify_var(&ST(0x7C), 2);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xE3D4 */
            if (ST(0x7F4) == 1) {
                ST(0x7F4) = 2;
                /* 0xE3FC */
                if (CB->distance(32, 13) > 125) {
                    lieu_action_register("L10D01B", P_ZED84, "SP84DIA60", "SP84ACT01");
                    CB->evenement(2, 1600);
                } else {
                    lieu_action_register("L10D01B", P_ZED84, "SP84DIA60", "SP84ACT03");
                    CB->evenement(1, 200);
                    return;
                }
            }
        }
    }
    /* 0xE46B */
    if (CB->direction_zone(22) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 22)) {
        trig22();
    }
}

/* trigger 14 - sub_1000E4A6 */
static void trig14(void) {
    /* 0xE4B5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(18)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE500 */
        if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY || ST(ST_PLAYER_WEAPON) == OB_KONK_KEY || ST(ST_PLAYER_WEAPON) == OB_SORDOS_KEY || ST(ST_PLAYER_WEAPON) == OB_BOGD_KEY || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_KEY || ST(ST_PLAYER_WEAPON) == OB_STOCK_KEY) {
            /* 0xE56C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xE57E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xE598 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02ACT71");
                        lieu_action_register("L10D88", P_ARKHAN2, "P02ACT74", "SP02ACT77");
                    } else {
                        CB->animation("SP02ACT70");
                        lieu_action_register("L10D88", P_ARKHAN2, "P02ACT74", "SP02ACT76");
                    }
                }
                /* 0xE5F7 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0xE611 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02ACT73");
                        lieu_action_register("L10D88", P_ARKHAN2, "P02ACT74", "SP02ACT79");
                    } else {
                        CB->animation("SP02ACT72");
                        lieu_action_register("L10D88", P_ARKHAN2, "SP02ACT74", "SP02ACT78");
                    }
                }
            }
            /* 0xE670 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xE682 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xE69C */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03ACT71");
                        lieu_action_register("L10D89", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                    } else {
                        CB->animation("SP03ACT70");
                        lieu_action_register("L10D89", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                    }
                }
                /* 0xE6FB */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0xE715 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03ACT73");
                        lieu_action_register("L10D89", P_ARKHAN3, "P03ACT74", "SP03ACT79");
                    } else {
                        CB->animation("SP03ACT72");
                        lieu_action_register("L10D89", P_ARKHAN3, "P03ACT74", "SP03ACT78");
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xE79D */
        if (ST(ST_PLAYER_WEAPON) == OB_LARGE_KEY) {
            ST(ST_PLAYER_WEAPON) = 0;
            ST(0xB4C) = 2;
            ST(0xDC) = 1;
            /* 0xE7DC */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xE7EE */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SP02ACT70");
                }
                /* 0xE80B */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP02ACT72");
                }
                CB->animation("P02ATT33");
                CB->etat_objet("SOB96", -1, -1, -1, -1, -1, -1, -2, 0);
                /* 0xE851 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("P02ACT76");
                }
                /* 0xE86E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("P02ACT78");
                }
                lieu_action_register("L10D90", P_ARKHAN2, "G00DIA01", "Sar_10");
            }
            /* 0xE8A4 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xE8B6 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SP03ACT70");
                }
                /* 0xE8D3 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP03ACT72");
                }
                CB->animation("P03ATT33");
                CB->etat_objet("SOB96", -1, -1, -1, -1, -1, -1, -2, 0);
                /* 0xE919 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("P03ACT76");
                }
                /* 0xE936 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("P03ACT78");
                }
                lieu_action_register("L10D91", P_ARKHAN3, "G00DIA01", "Sar_10");
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xE98A */
        if (ST(ST_PLAYER_WEAPON) == OB_OBJ_BEFORE40 || ST(ST_PLAYER_WEAPON) == OB_OBJ_BEFORE41) {
            /* 0xE9AE */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xE9C0 */
                if (ST(0xB4C) == 1) {
                    /* 0xE9D2 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SP02ACT70");
                    }
                    /* 0xE9EF */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SP02ACT72");
                    }
                    lieu_action_register("L10D84", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    /* 0xEA25 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SP02ACT76");
                    }
                    /* 0xEA42 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SP02ACT78");
                    }
                }
                /* 0xEA5F */
                if (ST(0xB4C) == 2) {
                    CB->retire_zone(1);
                    CB->retire_zone(2);
                    CB->retire_zone(3);
                    CB->retire_zone(4);
                    CB->retire_zone(5);
                    CB->retire_zone(6);
                    CB->animation("SP02ATT27");
                    /* 0xEABE */
                    if (ST(ST_PLAYER_WEAPON) == OB_OBJ_BEFORE41) {
                        ST(V_EVENT_12) = 1;
                        CB->etat_objet("SOB41", -1, -1, -1, -1, -1, -1, -2, 0);
                        CB->link_objet(ST(ST_HERO), "OB41", 0x1B, 0);
                        CB->modify_var(&ST(ST_PLAYER_WEAPON), 0);
                        CB->evenement(12, 3000);
                    }
                    /* 0xEB3B */
                    if (ST(ST_PLAYER_WEAPON) == OB_OBJ_BEFORE40) {
                        ST(V_EVENT_11) = 1;
                        CB->etat_objet("SOB40", -1, -1, -1, -1, -1, -1, -2, 0);
                        CB->link_objet(ST(ST_HERO), "OB40", 0x1B, 0);
                        CB->modify_var(&ST(ST_PLAYER_WEAPON), 0);
                        CB->evenement(11, 6000);
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xEBE1 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xEBF3 */
                if (ST(0xB4C) == 1) {
                    /* 0xEC05 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SP03ACT70");
                    }
                    /* 0xEC22 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SP03ACT72");
                    }
                    lieu_action_register("L10D85", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                    /* 0xEC58 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SP03ACT76");
                    }
                    /* 0xEC75 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SP03ACT78");
                    }
                }
                /* 0xEC92 */
                if (ST(0xB4C) == 2) {
                    CB->animation("SP03ATT27");
                    /* 0xECAF */
                    if (ST(ST_PLAYER_WEAPON) == OB_OBJ_BEFORE41) {
                        ST(V_EVENT_12) = 1;
                        CB->etat_objet("SOB41", -1, -1, -1, -1, -1, -1, -2, 0);
                        CB->link_objet(ST(ST_HERO), "OB41", 0x1B, 0);
                        CB->modify_var(&ST(ST_PLAYER_WEAPON), 0);
                        CB->evenement(12, 4000);
                    }
                    /* 0xED2C */
                    if (ST(ST_PLAYER_WEAPON) == OB_OBJ_BEFORE40) {
                        ST(V_EVENT_11) = 1;
                        CB->etat_objet("SOB40", -1, -1, -1, -1, -1, -1, -2, 0);
                        CB->link_objet(ST(ST_HERO), "OB40", 0x1B, 0);
                        CB->modify_var(&ST(ST_PLAYER_WEAPON), 0);
                        CB->evenement(11, 8000);
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        /* 0xEDD7 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xEDE9 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT30");
                lieu_action_register("L10D04A", P_ARKHAN1, "G00DIA07", NULL);
            }
            /* 0xEE1C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xEE36 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT04");
                }
                /* 0xEE5B */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT30");
                }
                lieu_action_register("L10D04B", P_ARKHAN2, "G00DIA07", NULL);
            }
            /* 0xEE8E */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xEEA8 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT04");
                }
                /* 0xEECD */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT30");
                }
                lieu_action_register("L10D04C", P_ARKHAN3, "G00DIA07", NULL);
            }
        }
        /* 0xEF00 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xEF12 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT31");
                lieu_action_register("L10D666", P_ARKHAN1, "G00DIA07", NULL);
            }
            /* 0xEF45 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xEF5F */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT05");
                }
                /* 0xEF84 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT31");
                }
                lieu_action_register("L10D667", P_ARKHAN2, "G00DIA07", NULL);
            }
            /* 0xEFB7 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xEFD1 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT05");
                }
                /* 0xEFF6 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT31");
                }
                lieu_action_register("L10D668", P_ARKHAN3, "G00DIA07", NULL);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->animation("ar_10");
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 15 - sub_1000F04B */
static void trig15(void) {
    /* 0xF05A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(18)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xF0A5 */
        if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY || ST(ST_PLAYER_WEAPON) == OB_KONK_KEY || ST(ST_PLAYER_WEAPON) == OB_SORDOS_KEY || ST(ST_PLAYER_WEAPON) == OB_BOGD_KEY || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_KEY || ST(ST_PLAYER_WEAPON) == OB_STOCK_KEY || ST(ST_PLAYER_WEAPON) == OB_LARGE_KEY) {
            /* 0xF123 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xF135 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xF14F */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02ACT71");
                        lieu_action_register("L10D86", P_ARKHAN2, "P02ACT74", "SP02ACT77");
                    } else {
                        CB->animation("SP02ACT70");
                        lieu_action_register("L10D86", P_ARKHAN2, "P02ACT74", "SP02ACT76");
                    }
                }
                /* 0xF1AE */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0xF1C8 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02ACT73");
                        lieu_action_register("L10D86", P_ARKHAN2, "P02ACT74", "SP02ACT79");
                    } else {
                        CB->animation("SP02ACT72");
                        lieu_action_register("L10D86", P_ARKHAN2, "SP02ACT74", "SP02ACT78");
                    }
                }
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xF250 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xF262 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xF27C */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03ACT71");
                        lieu_action_register("L10D87", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                    } else {
                        CB->animation("SP03ACT70");
                        lieu_action_register("L10D87", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                    }
                }
                /* 0xF2DB */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0xF2F5 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03ACT73");
                        lieu_action_register("L10D87", P_ARKHAN3, "P03ACT74", "SP03ACT79");
                    } else {
                        CB->animation("SP03ACT72");
                        lieu_action_register("L10D87", P_ARKHAN3, "P03ACT74", "SP03ACT78");
                    }
                }
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        trig14();
    }
}

/* trigger 16 - sub_1000F380 */
static void trig16(void) {
    /* 0xF38F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(18)) {
        /* 0xF3C3 */
        if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY || ST(ST_PLAYER_WEAPON) == OB_KONK_KEY || ST(ST_PLAYER_WEAPON) == OB_SORDOS_KEY || ST(ST_PLAYER_WEAPON) == OB_BOGD_KEY || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_KEY || ST(ST_PLAYER_WEAPON) == OB_STOCK_KEY || ST(ST_PLAYER_WEAPON) == OB_LARGE_KEY) {
            trig15();
            return;
        }
        trig14();
    }
}

/* trigger 17 - sub_1000F449 */
static void trig17(void) {
    /* 0xF458 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(18)) {
        /* 0xF48C */
        if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY || ST(ST_PLAYER_WEAPON) == OB_KONK_KEY || ST(ST_PLAYER_WEAPON) == OB_SORDOS_KEY || ST(ST_PLAYER_WEAPON) == OB_BOGD_KEY || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_KEY || ST(ST_PLAYER_WEAPON) == OB_STOCK_KEY || ST(ST_PLAYER_WEAPON) == OB_LARGE_KEY) {
            trig15();
            return;
        }
        trig14();
    }
}

/* trigger 22 - sub_1000F53E */
static void trig22(void) {
    /* 0xF54D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0) && ST(ST_HERO) == P_ARKHAN1) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xF5A7 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(2);
            /* 0xF5D0 */
            if (ST(V_DLG_RANDOM) == 0) {
                lieu_action_register("L10D67", P_ARKHAN1, "SP01ATT08", NULL);
            }
            /* 0xF5F8 */
            if (ST(V_DLG_RANDOM) == 1) {
                lieu_action_register("L10D68", P_ARKHAN1, "SP01ATT08", NULL);
            }
        }
        /* 0xF620 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->animation("SP01ATT08_1000");
            CB->animation("ar_10");
            CB->dialogue("L10D69A", P_ARKHAN1);
            lieu_action_register("L10D69B", P_ARKHAN1, "SP01ATT10", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->ajoute_zone(29, 4476, 422, 325, 80, 1);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 29 - sub_1000F6E0 */
static void trig29(void) {
    /* 0xF6EF */
    if (ST(ST_UPDOWN_KEY) == 1 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(V_EVENT_34) = 0;
        CB->evenement(34, -1);
        CB->retire_zone(29);
        lieu_action_register("LCOD66", P_ARKHAN1, "SP01ATT28", "ar_10");
        CB->bloque(BLOQUE_NONE);
        CB->evenement(34, 6000);
    }
}

/* trigger 30 - sub_1000F775 */
static void trig30(void) {
    /* 0xF788 */
    if (CB->direction_perso(P_DELIA)) {
        /* 0xF797 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
            /* 0xF7C7 */
            if (ST(ST_VIE(P_DELIA)) < 2) {
                ST(ST_SCRIPT_BUSY) = 1;
                CB->bloque(BLOQUE_ALL);
                /* 0xF7F0 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 0)) {
                    /* 0xF819 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L10D13A", P_ARKHAN2, "SG00DIA02", "Sar");
                        lieu_action_register("L10D13B", P_ARKHAN2, "SG00DIA02", "Sar");
                    }
                    /* 0xF85D */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L10D13C", P_ARKHAN3, "SG00DIA02", "Sar");
                        lieu_action_register("L10D13D", P_ARKHAN3, "SG00DIA02", "Sar");
                    }
                    lieu_action_register("L10D14", P_DELIA, "SP10DIA50", "SP10ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                }
                /* 0xF8D3 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 0)) {
                    /* 0xF8FC */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xF927 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L10D92", P_DELIA, "SP10DIA54", "SP10ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                }
            }
            /* 0xF99A */
            if (ST(ST_VIE(P_DELIA)) == VIE_DEAD) {
                CB->fouille(P_DELIA);
            }
        }
        /* 0xF9B4 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_DELIA) {
            /* 0xF9E4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L10D93", P_ARKHAN2);
            }
            /* 0xFA06 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L10D94", P_ARKHAN3);
            }
        }
        /* 0xFA25 */
        if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_DELIA)) == 1) {
            ST(0x6C) = 10;
            CB->stop_joindre(-P_DELIA);
            CB->bloque(BLOQUE_ALL);
            ST(V_PLAYER_STRIKE) = -1;
            /* 0xFA95 */
            if (ST(V_DELIA_HITS) == 1) {
                ST(V_DELIA_PHASE) = 20;
                ST(V_DELIA_KILLED) = 10;
                ST(ST_VIE(P_DELIA)) = VIE_DEAD;
                CB->animation("SP10IMP51");
                CB->animation("P10cad_10");
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xFB02 */
            if (ST(V_DELIA_HITS) == 0) {
                ST(V_DELIA_HITS) = 1;
                CB->animation("SP10ESA01");
                CB->animation("P10GAR01_10");
                CB->dialogue("L10D95", P_DELIA);
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x6C), 1);
                return;
            }
        }
    } else if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 1)) {
        trig01();
    }
    /* 0xFBAD */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 4)) {
        trig04();
    }
    /* 0xFBD2 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)) {
        trig05();
    }
    /* 0xFBF7 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 6)) {
        trig06();
    }
    /* 0xFC1C */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig_noop();
    }
    /* 0xFC41 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
        trig09();
    }
    /* 0xFC66 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13)) {
        trig_noop();
    }
    /* 0xFC8B */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 15)) {
        trig15();
    }
    /* 0xFCB0 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 17)) {
        trig17();
    }
    /* 0xFCD5 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 18)) {
        trig_noop();
    }
    /* 0xFCFA */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 19)) {
        trig_noop();
    }
    /* 0xFD1F */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 31)) {
        trig31();
    }
    /* 0xFD44 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 32)) {
        trig32();
    }
    /* 0xFD69 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 33)) {
        trig33();
    }
    /* 0xFD8E */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 34)) {
        trig34();
    }
    /* 0xFDB3 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 35)) {
        trig_noop();
    }
    /* 0xFDD8 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 38)) {
        trig38();
    }
}

/* trigger 31 - sub_1000FDE8 */
static void trig31(void) {
    /* 0xFDFB */
    if (CB->direction_perso(P_KALHI13)) {
        /* 0xFE0A */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x50) == 0) {
            /* 0xFE49 */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(0x6C) = 100;
                ST(0x74) = 2;
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI13);
                /* 0xFEC4 */
                if (ST(0x7FC) == 0) {
                    ST(0x7FC) = 1;
                    /* 0xFEE5 */
                    if (ST(0x98) == 1) {
                        /* 0xFEF7 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L10D58", P_ARKHAN2, "SG00DIA05", "Sar");
                        }
                        /* 0xFF22 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L10D59", P_ARKHAN3, "SG00DIA05", "Sar");
                        }
                        lieu_action_register("L10D60", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                    }
                    /* 0xFF66 */
                    if (ST(0x98) == 0) {
                        ST(0x98) = 1;
                        /* 0xFF87 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L10D61A", P_ARKHAN2, "SG00DIA05", "Sar");
                        }
                        lieu_action_register("L10D61B", P_ARKHAN2, "SG00DIA05", "Sar");
                        /* 0xFFCB */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L10D62", P_ARKHAN3, "SG00DIA05", "Sar");
                        }
                        lieu_action_register("L10D63", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->modify_var(&ST(0x6C), 1);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x10041 */
                if (ST(0x7FC) == 1) {
                    /* 0x10053 */
                    if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) || (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(V_LAST_WEAPON) == ST(ST_PLAYER_WEAPON))) {
                        /* 0x100D1 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L10D65", P_ARKHAN2, "SG00DIA06", "Sar_10");
                        }
                        /* 0x100FC */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L10D66", P_ARKHAN3, "SG00DIA06", "Sar_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x6C), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x10159 */
                    if (ST(ST_PLAYER_WEAPON) != 1 && ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(V_LAST_WEAPON) != ST(ST_PLAYER_WEAPON)) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0x101C2 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L10D100", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        }
                        /* 0x101ED */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L10D101", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                        }
                        lieu_action_register("L10D102", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x6C), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x10279 */
                    if (ST(ST_PLAYER_WEAPON) == 1) {
                        /* 0x1028B */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L03D518", P_ARKHAN2, "SG00DIA02", NULL);
                            CB->dialogue("L03D519", P_ARKHAN2);
                        }
                        /* 0x102C3 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->dialogue("L03D520", P_ARKHAN3);
                        }
                        lieu_v1_pre_scene(ST(ST_HERO), P_KALHI13, 1, 6, 5);
                        CB->dialogue("L03D521", P_KALHI13);
                        CB->modify_var(&ST(0xF4), 1);
                        return;
                    }
                }
            }
            /* 0x1032C */
            if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
                CB->fouille(P_KALHI13);
            }
        }
        /* 0x10346 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == P_KALHI13) {
            /* 0x10376 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L10D93", P_ARKHAN2);
            }
            /* 0x10398 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L10D94", P_ARKHAN3);
            }
        }
        /* 0x103B7 */
        if (ST(V_PLAYER_STRIKE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KALHI13)) == 1) {
            CB->select_pile(0, 2);
            CB->stop_joindre(-P_KALHI13);
            ST(0x6C) = 100;
            CB->bloque(BLOQUE_ALL);
            ST(V_ENGAGED_PERSONA) = P_KALHI13;
            ST(V_PLAYER_STRIKE) = -1;
            CB->animation("SP13ESA15");
            CB->animation("P13_65535");
            /* 0x10447 */
            if (ST(V_KALHI13_TALK_C) == 0) {
                ST(V_KALHI13_TALK_C) = 1;
                lieu_action_register("L10D106", P_KALHI13, "P13DIA59_10", "SP13ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(V_ENGAGED_PERSONA), 0);
                CB->modify_var(&ST(0x6C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0x104C0 */
            if (ST(V_KALHI13_TALK_C) == 1) {
                ST(V_KALHI13_TALK_C) = 2;
                lieu_action_register("L10D107", P_KALHI13, "P13DIA59_10", "SP13ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(V_ENGAGED_PERSONA), 0);
                CB->modify_var(&ST(0x6C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0x10539 */
            if (ST(V_KALHI13_TALK_C) == 2) {
                ST(0xE9C) = 8;
                ST(V_KALHI13_TALK_C) = 3;
                lieu_action_register("L10D108", P_KALHI13, "P13GAR26_10", "SP13GAR02_10");
                ST(0xA84) = 3;
                CB->bloque(BLOQUE_NONE);
                CB->combat(P_KALHI13, 0);
                CB->select_pile(0, 1);
                return;
            }
        }
        /* 0x105B5 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x50) == 1 && ST(ST_VIE(P_KALHI13)) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0x1060E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L11D06", P_ARKHAN2, "SG02ATT01", NULL);
            }
            /* 0x10636 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L11D07", P_ARKHAN3, "SG03ATT01", NULL);
            }
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 32 - sub_10010685 */
static void trig32(void) {
    /* 0x10698 */
    if (CB->direction_perso(P_KALHI33)) {
        /* 0x106A7 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(0x50) == 0) {
            /* 0x106E6 */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(0x6C) = 100;
                ST(0x74) = 2;
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_KALHI33);
                /* 0x1075D */
                if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) || (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(V_LAST_WEAPON) == ST(ST_PLAYER_WEAPON))) {
                    /* 0x107ED */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L10D44", P_ARKHAN2, "SG00DIA05", "Sar");
                    }
                    /* 0x10818 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L10D45", P_ARKHAN3, "SG00DIA05", "Sar");
                    }
                    lieu_action_register("L10D46", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x6C), 1);
                    return;
                }
                /* 0x1088E */
                if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 1 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(V_LAST_WEAPON) != ST(ST_PLAYER_WEAPON)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0x108F7 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L10D100", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0x10922 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L10D101", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L10D102", P_KALHI33, "SP13DIA54", "SP13ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x6C), 1);
                    return;
                }
                /* 0x109AE */
                if (ST(ST_PLAYER_WEAPON) == 1) {
                    /* 0x109C0 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L10D130A", P_ARKHAN2, "SG00DIA02", NULL);
                        CB->dialogue("L10D130B", P_ARKHAN2);
                    }
                    /* 0x109F8 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("L10D131", P_ARKHAN3);
                    }
                    lieu_v1_pre_scene(ST(ST_HERO), P_KALHI13, 1, 6, 5);
                    CB->dialogue("L10D132", P_KALHI13);
                    CB->modify_var(&ST(0xF4), 1);
                    return;
                }
            }
            /* 0x10A61 */
            if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
                CB->fouille(P_KALHI33);
            }
        }
        /* 0x10A7B */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == P_KALHI33) {
            /* 0x10AAB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L10D93", P_ARKHAN2);
            }
            /* 0x10ACD */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L10D94", P_ARKHAN3);
            }
        }
        /* 0x10AEC */
        if (ST(V_PLAYER_STRIKE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KALHI33)) == 1) {
            CB->stop_joindre(-P_KALHI33);
            ST(V_PLAYER_STRIKE) = -1;
            CB->bloque(BLOQUE_ALL);
            ST(0x6C) = 100;
            ST(V_ENGAGED_PERSONA) = P_KALHI33;
            CB->animation("SP33ESA15");
            CB->animation("P33_65535");
            /* 0x10B6F */
            if (ST(V_KALHI13_TALK_C) == 0) {
                ST(V_KALHI13_TALK_C) = 1;
                lieu_action_register("L10D106", P_KALHI33, "P33DIA59_10", "SP33ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(V_ENGAGED_PERSONA), 0);
                CB->modify_var(&ST(0x6C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0x10BE8 */
            if (ST(V_KALHI13_TALK_C) == 1) {
                ST(V_KALHI13_TALK_C) = 2;
                lieu_action_register("L10D107", P_KALHI33, "P33DIA59_10", "SP33ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(V_ENGAGED_PERSONA), 0);
                CB->modify_var(&ST(0x6C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0x10C61 */
            if (ST(V_KALHI13_TALK_C) == 2) {
                ST(V_KALHI13_TALK_C) = 3;
                ST(0xE9C) = 8;
                lieu_action_register("L10D108", P_KALHI33, "P33GAR26", "SP33GAR02_10");
                CB->bloque(BLOQUE_NONE);
                CB->combat(P_KALHI33, 0);
                CB->select_pile(0, 1);
                return;
            }
        }
        /* 0x10CCE */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x50) == 1 && ST(ST_VIE(P_KALHI33)) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0x10D27 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L11D06", P_ARKHAN2, "SG02ATT01", NULL);
            }
            /* 0x10D4F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L11D07", P_ARKHAN3, "SG03ATT01", NULL);
            }
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
}

/* trigger 33 - sub_10010D99 */
static void trig33(void) {
    /* 0x10DA8 */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_perso(P_GUARDIAN15)) {
        /* 0x10DCA */
        if (ST(V_ENGAGED_PERSONA) == P_GUARDIAN15 && ST(ST_VIE(P_GUARDIAN15)) == 1) {
            /* 0x10DEE */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0x10E10 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
        }
        /* 0x10E2F */
        if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARDIAN15)) == VIE_DEAD) {
            CB->fouille(P_GUARDIAN15);
        }
    }
}

/* trigger 34 - sub_10010E57 */
static void trig34(void) {
    /* 0x10E6A */
    if (CB->direction_perso(85) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        /* 0x10EA9 */
        if (ST(ST_VIE(85)) < 2) {
            ST(0x4E8) = 1;
            ST(ST_VIE(85)) = VIE_DEAD;
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0x10EF0 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L10D110", P_ARKHAN2, "SG00DIA02", NULL);
                CB->dialogue("L10D112A", 85);
                CB->dialogue("L10D112B", 85);
            }
            /* 0x10F38 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L10D111", P_ARKHAN3, "SG00DIA02", NULL);
                CB->dialogue("L10D113A", 85);
                CB->dialogue("L10D113B", 85);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->pos_perso(85, 4405, -646, 0x9470, 2, "P85cad38", 0x22, 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x10FC3 */
        if (ST(ST_VIE(85)) == VIE_DEAD) {
            CB->fouille(85);
        }
    }
    /* 0x10FDD */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(85)) == 1) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(ST_VIE(85)) = VIE_DEAD;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SP85cad38_10");
        CB->animation("Sar_10");
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 36 - sub_1001105D */
static void trig36(void) {
    /* 0x11070 */
    if (CB->direction_zone(36) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x110C6 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x110D8 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x110F2 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT71");
                    lieu_action_register("L10D114A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L10D114B", P_ARKHAN2, "SP02ACT74", "SP02ACT74");
                } else {
                    CB->animation("SP02ACT70");
                    lieu_action_register("L10D114A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L10D114B", P_ARKHAN2, "SP02ACT74", "SP02ACT74");
                }
            }
            /* 0x11183 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x1119D */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT71");
                    lieu_action_register("L10D115A", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                    lieu_action_register("L10D115B", P_ARKHAN3, "SP03ACT74", "SP03ACT74");
                } else {
                    CB->animation("SP03ACT70");
                    lieu_action_register("L10D115A", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                    lieu_action_register("L10D115B", P_ARKHAN3, "SP03ACT74", "SP03ACT74");
                }
            }
        }
        /* 0x1122E */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x11248 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x1125A */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT85");
                }
                /* 0x11277 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT85");
                }
            }
            /* 0x1129C */
            if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x112AE */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT86");
                }
                /* 0x112CB */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT86");
                }
            }
            /* 0x112E8 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L10D116", P_ARKHAN2, "SG00DIA08", "Sar_10");
            }
            /* 0x11313 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L10D117", P_ARKHAN3, "SG00DIA08", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 37 - sub_10011350 */
static void trig37(void) {
    /* 0x11363 */
    if (CB->direction_zone(37) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x113B9 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x113CB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x113E5 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT71");
                    lieu_action_register("L10D118A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L10D118B", P_ARKHAN2, "SP02ACT74", "SP02ACT77");
                } else {
                    CB->animation("SP02ACT70");
                    lieu_action_register("L10D118A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L10D118B", P_ARKHAN2, "SP02ACT74", "SP02ACT76");
                }
            }
            /* 0x11476 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x11490 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT71");
                    lieu_action_register("L10D119", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                } else {
                    CB->animation("SP03ACT70");
                    lieu_action_register("L10D119", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                }
            }
        }
        /* 0x114EF */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->animation("SG00PRE09");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 38 - sub_1001151E */
static void trig38(void) {
    /* 0x11531 */
    if (CB->direction_zone(38) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x11587 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x11599 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L10D120", P_ARKHAN2, "SG00DIA08", "Sar_10");
            }
            /* 0x115C4 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L10D121", P_ARKHAN3, "SG00DIA08", "Sar_10");
            }
        }
        /* 0x115EF */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->animation("SG00PRE09");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 39 - sub_1001161E */
static void trig39(void) {
    /* 0x11631 */
    if (CB->direction_zone(39) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x11687 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x11699 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x116B3 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT71");
                    lieu_action_register("L10D122A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L10D122B", P_ARKHAN2, "SP02ACT74", "SP02ACT77");
                } else {
                    CB->animation("SP02ACT70");
                    lieu_action_register("L10D122A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L10D122B", P_ARKHAN2, "SP02ACT74", "SP02ACT76");
                }
            }
            /* 0x11744 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x1175E */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT71");
                    lieu_action_register("L10D123", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                } else {
                    CB->animation("SP03ACT70");
                    lieu_action_register("L10D123", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                }
            }
        }
        /* 0x117BD */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->animation("SG00PRE09");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 40 - sub_100117EC */
static void trig40(void) {
    /* 0x117FF */
    if (CB->direction_zone(40) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x11855 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x11867 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L10D124", P_ARKHAN2, "SG00DIA08", "Sar_10");
            }
            /* 0x11892 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L10D125", P_ARKHAN3, "SG00DIA08", "Sar_10");
            }
        }
        /* 0x118BD */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->animation("SG00PRE09");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 41 - sub_10007C14 */
static void trig41(void) {
    /* 0x7C26 */
    if (ST(V_DELIA_PHASE) < 5 || (ST(V_DELIA_PHASE) == 20 && ST(0xBE4) == 0)) {
        CB->divers(DV_MUSIC, 1, 0);
        CB->divers_empile(DV_MUSIC, 1, 1);
    }
    /* 0x7C8B */
    if (ST(V_SAVED_HERO) != ST(ST_HERO) && (ST(V_DELIA_PHASE) == 6 || (ST(0xE9C) == 7 && ST(0xEF0) != 0 && (ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1) && ST(0xEF4) < 2))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7D20 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7D32 */
            if (ST(V_DELIA_PHASE) == 6) {
                CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD48", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            }
            /* 0x7D7A */
            if (ST(0xE9C) == 7 && ST(ST_VIE(P_KALHI13)) == 1) {
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD49", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
            }
            /* 0x7DD4 */
            if (ST(0xE9C) == 7 && ST(ST_VIE(P_KALHI33)) == 1) {
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD49", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
            }
            /* 0x7E2E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD50", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0x7E56 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD51", P_ARKHAN3, "SG00DIA02", NULL);
            }
        }
        /* 0x7E7E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7E90 */
            if (ST(V_DELIA_PHASE) == 6) {
                CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD44", P_DELIA, "SP10DIA54", "SP10ARR01_10");
            }
            /* 0x7ED8 */
            if (ST(0xE9C) == 7 && ST(ST_VIE(P_KALHI13)) == 1) {
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD45", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
            }
            /* 0x7F32 */
            if (ST(0xE9C) == 7 && ST(ST_VIE(P_KALHI33)) == 1) {
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("LCOD45", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
            }
            /* 0x7F8C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD46", P_ARKHAN2, "SG00DIA08", NULL);
            }
            /* 0x7FB4 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD47", P_ARKHAN3, "SG00DIA08", NULL);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7FF5 */
    if (ST(0xAF4) == 0 && ST(0xEC8) == 0) {
        CB->change_camera(9, CAM_LOCKED);
    }
    /* 0x8029 */
    if (ST(0xEC8) == 0 && ST(0xAF4) == 0 && ST(ST_HERO) == P_ARKHAN1) {
        CB->joindre(P_RYLSADHAR20, 4500, -2720, 0x2EE0, -1);
        CB->joindre(P_LORY, 4380, -2760, 0xD228, -1);
        CB->modify_var(&ST(0x70), 1);
    }
    /* 0x80AB */
    if (ST(0xAF4) < 3 && ST(0xEC8) == 1 && ST(ST_HERO) == P_ARKHAN1) {
        CB->select_pile(2, 1);
        CB->divers(DV_RUN, P_ZED84, 1);
        CB->joindre(-P_ZED84, 2900, -1130, -1, -1);
        CB->select_pile(0, 1);
    }
    /* 0x8129 */
    if (ST(V_DELIA_PHASE) == 5) {
        ST(V_DELIA_PHASE) = 6;
        CB->joindre(-P_DELIA, 4472, 400, 0x88B8, -1);
    }
    /* 0x8166 */
    if (ST(0xE9C) == 7 && ST(ST_VIE(P_KALHI33)) == 1 && ST(0x80) == 0 && ST(0x948) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0x948) = 1;
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L10D64A", P_KALHI33, "SP33DIA56", "SP33ARR01_10");
        lieu_action_register("L10D64B", P_KALHI33, "SP33DIA56", "SP33ARR01_10");
        lieu_action_register("L10D64C", P_KALHI33, "SP33DIA59", "SP33ARR01_10");
        CB->modify_var(&ST(0x6C), 1);
        CB->modify_var(&ST(0x74), 1);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x8265 */
    if (ST(0xF14) == 1) {
        CB->change_camera(4, CAM_AUTO);
        ST(0xF14) = 2;
        CB->play_anim(0, 1);
        CB->play_anim(2, 1);
        CB->play_anim(4, 1);
        CB->play_anim(6, 1);
        CB->play_anim(8, 1);
        lieu_action_register("L10D78A", P_SUNSEER1, "SP18DIA59", "SP18ARR01_10");
        lieu_action_register("L10D78B", P_SUNSEER1, "SP18DIA60", "SP18ARR01_10");
        lieu_action_register("L10D78C", P_SUNSEER1, "SP18DIA58", "SP18ARR01_10");
        lieu_action_register("L10D78D", P_SUNSEER1, "SP18DIA54", "SP18ARR01_10");
        lieu_action_register("L10D78E", P_SUNSEER1, "SP18DIA59", "SP18ARR01_10");
        lieu_action_register("L10D78F", P_SUNSEER1, "SP18DIA56", "SP18ARR01_10");
        lieu_action_register("L10D78G", P_SUNSEER1, "SP18DIA60", "SP18ARR01_10");
        lieu_action_register("L10D79", P_GUARDIAN15, "SP55ATT72", "SP55ARR04_10");
        CB->evenement(0, 20);
    }
}
