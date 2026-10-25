/*
 * lieu23_trig.c — Native hand-port of the LIEU23.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu23.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu23_int.h"

static void trig01(void); static void trig02(void); static void trig03(void);
static void trig09(void); static void trig10(void); static void trig11(void);
static void trig12(void); static void trig13(void); static void trig15(void);
static void trig16(void); static void trig41(void);
static void trig_noop(void);   /* the 31 stub slots */

/* dword_1001CBD0[42] — filled at INIT with the 5-byte jmp thunks; static
 * here. A slot is "stub" when it is the empty handler shared with LIEU01.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000B2CD stub  28  1000B6A2 stub
 *    1  1000A55F       15  1000B2D8       29  1000B6AD stub
 *    2  1000A728       16  1000B456       30  1000B6B8 stub
 *    3  1000A938       17  1000B629 stub  31  1000B6C3 stub
 *    4  1000AA77 stub  18  1000B634 stub  32  1000B6CE stub
 *    5  1000AA82 stub  19  1000B63F stub  33  1000B6D9 stub
 *    6  1000AA8D stub  20  1000B64A stub  34  1000B6E4 stub
 *    7  1000AA98 stub  21  1000B655 stub  35  1000B6EF stub
 *    8  1000AAA3 stub  22  1000B660 stub  36  1000B6FA stub
 *    9  1000AAAE       23  1000B66B stub  37  1000B705 stub
 *   10  1000ABA6       24  1000B676 stub  38  1000B710 stub
 *   11  1000AD2A       25  1000B681 stub  39  1000B71B stub
 *   12  1000AEAE       26  1000B68C stub  40  1000B726 stub
 *   13  1000B1A1       27  1000B697 stub  41  1000825A
 */
static void (*const g_trigger[42])(void) = {
    l23_trig00, trig01, trig02, trig03, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig09, trig10, trig11, trig12, trig13, trig_noop, trig15,
    trig16, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001CBD0]). */
void l23_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start-point presets, state resets, zones, actor placement (INIT default) */
void l23_trig00(void) {
    /* 29698 */
    if (ST(ST_START_POINT(23)) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 10;
        ST(0xE78) = 1;
        ST(0xBA8) = 0;
        ST(0x9C) = 2000;
    }
    /* 29788 */
    if (ST(ST_START_POINT(23)) == 2) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 10;
        ST(0xBA8) = 0;
        ST(0xE78) = 1;
        ST(0x9C) = 10000;
    }
    /* 29878 */
    if (ST(ST_START_POINT(23)) == 3) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 25;
        ST(0xE78) = 10;
    }
    /* 29938 */
    if (ST(ST_START_POINT(23)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 24;
        ST(0xBA8) = 1;
        ST(0xBB4) = 1;
    }
    /* 30013 */
    if (ST(ST_START_POINT(23)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 25;
        ST(0xBA8) = 2;
        ST(0xBB4) = 1;
    }
    /* 30088 */
    if (ST(ST_START_POINT(23)) == 6) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 24;
        ST(ST_VIE(P_THANANDAR24)) = VIE_DEAD;
        ST(0xBA8) = 2;
        ST(0xBB4) = 1;
    }
    /* 30178 */
    if (ST(ST_START_POINT(23)) == 7) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 24;
        ST(0xA5C) = 1;
        ST(ST_VIE(P_THANANDAR24)) = 1;
        ST(0xBA8) = 4;
    }
    /* 30268 */
    if (ST(ST_START_POINT(23)) == 9) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 10;
        ST(0xA5C) = 1;
        ST(ST_VIE(P_THANANDAR24)) = 1;
        ST(0xBA8) = 6;
    }
    /* 30358 */
    if (ST(ST_START_POINT(23)) == 10) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 24;
        ST(0xA5C) = 1;
        ST(ST_VIE(P_THANANDAR24)) = VIE_DEAD;
        ST(0xBA8) = 6;
    }
    /* 30448 */
    if (ST(ST_START_POINT(23)) == 11) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 10;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xBA8) = 6;
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
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    CB->ajoute_zone(1, 5105, -2647, 80, 150, 1);
    CB->ajoute_zone(2, 6464, -3652, 90, 100, 0);
    CB->ajoute_zone(9, 6460, -3055, 80, 105, 0);
    CB->ajoute_zone(15, 6420, -3364, 200, 190, 0);
    CB->ajoute_zone(16, 5745, -3144, 600, 600, 0);
    CB->ajoute_zone(17, 5745, -3144, 50, 50, 0);
    CB->ajoute_zone(3, 6070, -3896, 100, 90, 0);
    CB->vire_col(12, 0);
    /* 31160 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB4C) == 3 && ST(0xF30) == 0) {
        CB->etat_objet("OB51", 5810, -1551, -2852, 0, 0xA7F8, 0x11F8, -1, 0);
        CB->ajoute_zone(13, 5890, -2860, 230, 235, 0);
    }
    CB->bruitage("L23AMB1", -1, -1);
    /* 31307 */
    if (ST(0xB10) != 0 && ST(0xE74) == 0) {
        ST(0xE74) = 1;
    }
    /* 31358 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0) {
        /* 31412 */
        if (ST(0xB98) == 0) {
            ST(0xB98) = 2;
        } else if (ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
            ST(0xB98) = 0;
        }
    }
    /* 31501 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }
    /* 31600 */
    if (ST(ST_LEVEL) == 10) {
        CB->pos_perso(ST(ST_HERO), 5268, -2605, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
    }
    /* 31678 */
    if (ST(ST_LEVEL) == 24) {
        /* 31696 */
        if (ST(ST_HERO) >= 2 && ST(0xBB4) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->pos_perso(P_PHEDORIA, 6300, -3400, 0xCE4, 1, "P26ARR01", 5, 0);
            CB->pos_perso(ST(ST_HERO), 6135, -3850, 0, 1, "ar", 6, 0);
        } else {
            CB->pos_perso(ST(ST_HERO), 6142, -3821, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        }
    }
    /* 31886 */
    if (ST(ST_LEVEL) == 25) {
        CB->pos_perso(ST(ST_HERO), 6425, -3642, 0x2E12, 1, "ar", 6, 0);
        /* 31947 */
        if (ST(0xBB4) == 1 && ST(ST_HERO) >= 2) {
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x84) = 1;
            CB->pos_perso(P_KALHI13, 5983, -2823, 0x7530, 1, "P13ARR01", 0xE, 0);
            CB->pos_perso(P_PHEDORIA, 6214, -2965, 0x7530, 1, "P26ARR01", 5, 0);
        }
    }
    /* 32084 */
    if (ST(0xE78) == 1 && ST(ST_HERO) == P_ARKHAN1) {
        ST(0xE78) = 2;
        ST(0xE7C) = 3;
        CB->ajoute_zone(7, 6721, -3716, 100, 100, 1);
        CB->pos_perso(P_PHEDORIA, 5000, 3000, 0xC350, 1, "P26ARR03", 5, 0);
        ST(0x9C) = (ST(ST_DEBUG_PRINT) - ST(0xA0));
        /* 32252 */
        if (ST(0x9C) <= 5000) {
            CB->pos_perso(P_RYLSADHAR20, 6100, -3000, 0x9664, 1, "P20ARR01", 4, 0);
        }
        /* 32310 */
        if (ST(0x9C) > 5000 && ST(0x9C) <= 10000) {
            CB->pos_perso(P_RYLSADHAR20, 6450, -3640, 0xC350, 1, "P20ARR01", 4, 0);
        }
        /* 32389 */
        if (ST(0x9C) > 10000) {
            ST(0xE78) = 7;
        }
    }
    /* 32422 */
    if (ST(0xE78) == 10 && ST(ST_HERO) == P_ARKHAN1) {
        CB->vire_col(13, 0);
        CB->retire_zone(2);
        CB->pos_perso(P_ZED84, 6306, -3774, 0xE4C5, 1, "P84ARR01", 7, 0);
        CB->link_objet(0x54, "OB68", 0x1D, 1);
    }
    /* 32539 */
    if (ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->ajoute_zone(12, 6320, -3330, 250, 250, 0);
        /* 32608 */
        if (ST(0x4EC) == 0) {
            CB->pos_perso(P_GUARDIAN13, 6327, -3493, 0xFAE6, 1, "P53GAR03", 0xA, 0);
            CB->pos_perso(P_GUARDIAN14, 6099, -3367, 0xC501, 1, "P54GAR02", 0xB, 0);
        }
        /* 32700 */
        if (ST(0x4EC) == 1) {
            CB->pos_perso(P_GUARDIAN13, 6059, -3793, 0x37E, 1, "P53ARR03", 0xA, 0);
            CB->pos_perso(P_GUARDIAN14, 6393, -3605, 0x3A35, 1, "P54ARR02", 0xB, 0);
        }
        /* 32792 */
        if (ST(0x4EC) == 2) {
            /* 32810 */
            if (ST(ST_VIE(P_GUARDIAN13)) == VIE_DEAD) {
                CB->pos_perso(P_GUARDIAN13, CB->divers(DV_DESC_POS, P_GUARDIAN13, 0), CB->divers(DV_DESC_POS, P_GUARDIAN13, 1), CB->divers(DV_DESC_POS, P_GUARDIAN13, 2), 2, "P53cad", 0xA, 0);
            } else {
                CB->pos_perso(P_GUARDIAN13, 6059, -3793, 0x37E, 1, "P53GAR03", 0xA, 0);
            }
            /* 32950 */
            if (ST(ST_VIE(P_GUARDIAN14)) == VIE_DEAD) {
                CB->pos_perso(P_GUARDIAN14, CB->divers(DV_DESC_POS, P_GUARDIAN14, 0), CB->divers(DV_DESC_POS, P_GUARDIAN14, 1), CB->divers(DV_DESC_POS, P_GUARDIAN14, 2), 2, "P54cad", 0xB, 0);
            } else {
                CB->pos_perso(P_GUARDIAN14, 6393, -3605, 0x3A35, 1, "P54GAR02", 0xB, 0);
            }
        }
        /* 33090 */
        if (ST(0x4EC) == 3) {
            CB->pos_perso(P_GUARDIAN13, CB->divers(DV_DESC_POS, P_GUARDIAN13, 0), CB->divers(DV_DESC_POS, P_GUARDIAN13, 1), CB->divers(DV_DESC_POS, P_GUARDIAN13, 2), 2, "P53cad", 0xA, 0);
            CB->pos_perso(P_GUARDIAN14, CB->divers(DV_DESC_POS, P_GUARDIAN14, 0), CB->divers(DV_DESC_POS, P_GUARDIAN14, 1), CB->divers(DV_DESC_POS, P_GUARDIAN14, 2), 2, "P54cad", 0xB, 0);
        }
    }
    /* 33268 */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->load_anim("223OUV02", 0);
        CB->load_anim("223OUV03", 1);
    }
    /* 33315 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->load_anim("223OUV05", 0);
        CB->load_anim("223OUV06", 1);
    }
    ST(ST_START_POINT(23)) = 0;
}

/* trigger 41 — sub_1000825A */
static void trig41(void) {
    /* 33388 */
    if (ST(0xE78) == 2 && ST(ST_HERO) == P_ARKHAN1) {
        CB->joindre(-P_RYLSADHAR20, 6450, -3640, 0xC350, -1);
        CB->modify_var(&ST(0x64), 1);
    }
    /* 33472 */
    if (ST(0xE78) == 10 && ST(ST_HERO) == P_ARKHAN1) {
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-ST(ST_HERO), 6288, -3485, 0x2083, -1);
        lieu_action_register("L23D23", P_ZED84, "SP84DIA54", "SP84ARR02_10");
        CB->divers(DV_RUN, P_ZED84, 1);
        CB->joindre(-P_ZED84, 6576, -3668, 0xD6D8, -1);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 84);
        CB->modify_var(&ST(0x68), 1);
    }
    /* 33677 */
    if (ST(ST_HERO) >= 2 && ST(0xBB4) == 1) {
        CB->bloque(BLOQUE_ALL);
        CB->divers(DV_RUN, P_PHEDORIA, 1);
        /* 33742 */
        if (ST(0x84) == 0) {
            CB->joindre(-P_PHEDORIA, 6200, -3570, 0x5AC8, -1);
        }
        /* 33788 */
        if (ST(0x84) == 1) {
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 26);
            CB->joindre(-P_PHEDORIA, 6266, -3267, 0x9088, -1);
        }
        CB->modify_var(&ST(0x78), 1);
    }
    /* 33883 */
    if (ST(0x4EC) == 0 && ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(2, CAM_LOCKED);
        ST(0x4EC) = 1;
        CB->animation("SP53GAR27");
        lieu_action_register("L23D29", P_GUARDIAN13, "P53DIA59", "SP53ARR03_10");
        CB->animation("SP54GAR27");
        lieu_action_register("L23D30", P_GUARDIAN14, "P54DIA61", "SP54ARR02_10");
        CB->joindre(P_GUARDIAN13, 6211, -3424, -1, -1);
        CB->animation("SP53ARR03_50");
        CB->joindre(P_GUARDIAN14, 0, 0, 0, 53);
        CB->joindre(P_GUARDIAN13, 0, 0, 0, 54);
        lieu_action_register("L23D31", P_GUARDIAN13, "SP53DIA59", "SP53ARR03_10");
        lieu_action_register("L23D32", P_GUARDIAN14, "SP54DIA60", "SP54ARR02_10");
        CB->joindre(P_GUARDIAN14, 6059, -3793, 0x37E, -1);
        CB->joindre(P_GUARDIAN13, 6393, -3605, 0x3A35, -1);
        CB->change_camera(0, CAM_LOCKED);
        CB->change_camera(99, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
    }
}


/* trigger 01 -- sub_1000A55F */
static void trig01(void) {
    ST(ST_LEVEL) = 23;
    /* 42365 */
    if (ST(ST_HERO) == P_ARKHAN1 && ST(0xE78) < 7) {
        ST(0xE78) = 7;
    }
    /* 42416 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0x4EC) == 1 && ST(V_ENGAGED_PERSONA) != 0) {
        ST(0x4EC) = 2;
        ST(0x4F0) = 0;
    }
    /* 42497 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0x4EC) == 2 && ST(V_ENGAGED_PERSONA) != 0) {
        ST(0x4F0) = 0;
    }
    /* 42563 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->change_piece(10, 0);
    }
    /* 42594 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 0) {
        CB->change_piece(10, 1);
    }
    /* 42646 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 1) {
        CB->change_piece(10, 2);
    }
    /* 42698 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 2) {
        CB->change_piece(10, 3);
    }
    /* 42750 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 3) {
        CB->change_piece(51, 0);
    }
}

/* trigger 02 -- sub_1000A728 */
static void trig02(void) {
    /* 42807 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xC350)) {
        /* 42874 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 42892 */
            if (ST(0xE78) == 2) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->animation("SG00OUV12");
                CB->dialogue("L23D16", P_RYLSADHAR20);
                /* 42960 */
                if (ST(ST_PLAYER_WEAPON) != OB_AX26 && ST(ST_PLAYER_WEAPON) != OB_SABER38) {
                    CB->animation("SP01MAR29");
                }
                /* 43007 */
                if (ST(ST_PLAYER_WEAPON) == OB_AX26 || ST(ST_PLAYER_WEAPON) == OB_SABER38) {
                    CB->animation("SP01MAR30");
                }
                CB->animation("ar_10");
                CB->bloque(BLOQUE_NONE);
            }
            /* 43073 */
            if (ST(0xE78) > 2) {
                CB->bloque(BLOQUE_ALL);
                CB->load_anim("223OUV02", 0);
                ST(ST_SCRIPT_BUSY) = 1;
                /* 43127 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->evenement(0, 500);
                }
                /* 43158 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->evenement(0, 700);
                }
                CB->animation("SG00OUV01");
            }
        }
        /* 43200 */
        if (ST(ST_HERO) >= 2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->vire_col(13, 0);
            CB->animation("SG00OUV01");
            /* 43265 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->evenement(0, 200);
            }
            /* 43296 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->evenement(0, 500);
            }
        }
    }
}

/* trigger 03 -- sub_1000A938 */
static void trig03(void) {
    /* 43335 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        /* 43383 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->animation("SG00OUV00");
            CB->dialogue("L23D15", P_ARKHAN1);
            CB->bruitage("L23B01", -2177, -2352);
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 43513 */
        if (ST(ST_HERO) >= 2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->vire_col(14, 0);
            CB->animation("SG00OUV01");
            /* 43578 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->evenement(27, 200);
            }
            /* 43612 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->evenement(27, 500);
            }
        }
    }
}

/* trigger 09 -- sub_1000AAAE */
static void trig09(void) {
    /* 43709 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xB798)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 43799 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L23D25", P_ARKHAN1, "SG00ATT01", NULL);
        }
        /* 43839 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L23D26", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 43879 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L23D27", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 10 -- sub_1000ABA6 */
static void trig10(void) {
    /* 43961 */
    if (CB->direction_perso(P_GUARDIAN13)) {
        /* 43976 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_GUARDIAN13)) == VIE_DEAD) {
            CB->fouille(P_GUARDIAN13);
            return;
        }
        /* 44055 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN13)) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 44129 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 44147 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("LCOD20", P_ARKHAN2);
                }
                /* 44181 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD61", P_ARKHAN2);
                }
            }
            /* 44215 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 44233 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("LCOD21", P_ARKHAN3);
                }
                /* 44267 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD63", P_ARKHAN3);
                }
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
}

/* trigger 11 -- sub_1000AD2A */
static void trig11(void) {
    /* 44349 */
    if (CB->direction_perso(P_GUARDIAN14)) {
        /* 44364 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_GUARDIAN14)) == VIE_DEAD) {
            CB->fouille(P_GUARDIAN14);
            return;
        }
        /* 44443 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN14)) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 44517 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 44535 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("LCOD20", P_ARKHAN2);
                }
                /* 44569 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD62", P_ARKHAN2);
                }
            }
            /* 44603 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 44621 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("LCOD21", P_ARKHAN3);
                }
                /* 44655 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD63", P_ARKHAN3);
                }
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
}

/* trigger 12 -- sub_1000AEAE */
static void trig12(void) {
    /* 44737 */
    if (CB->direction_zone(12) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 44823 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 44841 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 44867 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT71");
                    lieu_action_register("L23D38A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L23D38B", P_ARKHAN2, "SP02ACT74", "SP02ACT77");
                } else {
                    CB->animation("SP02ACT70");
                    lieu_action_register("L23D38A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L23D38B", P_ARKHAN2, "SP02ACT74", "SP02ACT76");
                }
            }
            /* 45012 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 45038 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT71");
                    lieu_action_register("L23D39A", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                    lieu_action_register("L23D39B", P_ARKHAN3, "SP03ACT74", "SP03ACT77");
                } else {
                    CB->animation("SP03ACT70");
                    lieu_action_register("L23D39A", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                    lieu_action_register("L23D39B", P_ARKHAN3, "SP03ACT74", "SP03ACT76");
                }
            }
        }
        /* 45183 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 45209 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 45227 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT85");
                }
                /* 45256 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT85");
                }
            }
            /* 45293 */
            if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 45311 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT86");
                }
                /* 45340 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT86");
                }
            }
            /* 45369 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L23D40", P_ARKHAN2, "G00DIA08", "Sar_10");
            }
            /* 45412 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L23D41", P_ARKHAN3, "G00DIA08", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 13 -- sub_1000B1A1 */
static void trig13(void) {
    /* 45488 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && (CB->direction_spec(0x1B58) || CB->direction_spec(0x32C8))) {
        ST(0xF30) = 1;
        ST(ST_SCRIPT_BUSY) = 1;
        CB->retire_zone(13);
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE08");
        CB->etat_objet("OB51", -1, -1, -1, -1, -1, -1, -3, 0);
        /* 45664 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L23D42", P_ARKHAN2, "G00DIA05", "Sar_10");
        }
        /* 45707 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L23D43", P_ARKHAN3, "G00DIA05", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 -- sub_1000B2D8 */
static void trig15(void) {
    /* 45799 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC350)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 45889 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 45907 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                lieu_action_register("L23D47", P_ARKHAN1, "SG00DIA04", NULL);
            }
            /* 45947 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L23D48", P_ARKHAN2, "SG00DIA04", NULL);
            }
            /* 45987 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L23D49", P_ARKHAN3, "SG00DIA04", NULL);
            }
        }
        /* 46027 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 46045 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT29");
            }
            /* 46074 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 46103 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT29");
            }
        }
        CB->animation("ar_200");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 -- sub_1000B456 */
static void trig16(void) {
    /* 46184 */
    if (ST(0xB4C) >= 2) {
        return;
    }
    /* 46204 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(17) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 5) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 13)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 46358 */
        if (ST(ST_PLAYER_WEAPON) == OB_OBJ_BEFORE40 || ST(ST_PLAYER_WEAPON) == OB_OBJ_BEFORE41) {
            /* 46394 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD86", P_ARKHAN2, "SG00DIA06", NULL);
            }
            /* 46434 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD87", P_ARKHAN3, "SG00DIA06", NULL);
            }
        } else {
            /* 46479 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                lieu_action_register("L23D50", P_ARKHAN1, "SG01DIA02", NULL);
            }
            /* 46519 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L23D51", P_ARKHAN2, "SG02DIA02", NULL);
            }
            /* 46559 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L23D52", P_ARKHAN3, "SG03DIA02", NULL);
            }
        }
        CB->animation("Sar_200");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}
