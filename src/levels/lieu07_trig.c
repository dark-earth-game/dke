/*
 * lieu07_trig.c — Native hand-port of the LIEU07.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu07.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu07_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig16(void);
static void trig17(void); static void trig18(void); static void trig19(void);
static void trig20(void); static void trig21(void); static void trig25(void);
static void trig41(void);
static void trig_noop(void);   /* the 29 stub slots */

/* dword_1001E1C0[42] — filled at INIT with the 5-byte jmp thunks at
 * 0x10001000+5*i; static here. Slots not listed are stubs (the shared
 * L01 trig06_1000D30E body: an empty handler).
 *   idx handler        idx handler
 *    0  100073F0       16  1000BAC5
 *    2  1000A73D       17  1000BC22
 *    3  1000ABF5       18  1000BD56
 *    4  1000B904       19  1000BE59
 *    5  1000B9FC       20  1000BF5C
 *    6  1000BA2F       21  1000C051
 *   25  1000C18C       41  10007E67
 */
static void (*const g_trigger[42])(void) = {
    l07_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig16, trig17, trig18, trig19, trig20, trig21, trig_noop,
    trig_noop, trig_noop, trig25, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001E1C0]). */
void l07_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start-point presets, zones, objects, actor placement */
void l07_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(7)) == 1) {
        ST(ST_LEVEL) = 2;
        ST(ST_HERO) = P_ARKHAN1;
        ST(0xAFC) = 4;
        ST(V_ZED_BRIEFING) = 3;
    }
    /* 0x744D */
    if (ST(ST_START_POINT(7)) == 2) {
        ST(ST_LEVEL) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(0xAFC) = 3;
    }
    /* 0x7498 */
    if (ST(ST_START_POINT(7)) == 3) {
        ST(ST_LEVEL) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(0xAFC) = 4;
    }
    /* 0x74F2 */
    if (ST(ST_START_POINT(7)) == 4) {
        ST(ST_LEVEL) = 2;
        ST(ST_HERO) = P_ARKHAN1;
        ST(0xAFC) = 4;
        ST(V_ZED_BRIEFING) = 3;
        ST(0xBDC) = 3;
    }
    ST(ST_START_POINT(7)) = 0;
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
    CB->vire_col(0x1F, 0);
    CB->vire_col(0x20, 0);
    CB->vire_col(0x21, 0);
    /* 0x7627 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->vire_col(0x1E, 0);
    }
    CB->ajoute_zone(5, 1514, -961, 210, 135, 1);
    CB->ajoute_zone(6, 1526, -1803, 245, 100, 1);
    CB->ajoute_zone(4, 111, -702, 255, 115, 0);
    CB->ajoute_zone(16, 1334, -1637, 250, 250, 0);
    /* 0x76C4 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->ajoute_zone(17, 1199, -1038, 205, 265, 0);
    }
    CB->ajoute_zone(18, -140, -983, 190, 175, 0);
    CB->ajoute_zone(19, -174, -851, 108, 88, 0);
    CB->ajoute_zone(20, 601, -702, 255, 115, 0);
    /* 0x7751 */
    if (ST(0x82C) == 0) {
        CB->ajoute_zone(21, 1389, -1412, 200, 160, 0);
    }
    CB->ajoute_zone(25, 1170, -1033, 490, 327, 0);
    CB->ajoute_zone(26, 939, -689, 25, 25, 0);
    /* 0x77C0 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->ajoute_zone(13, 190, -1735, 20, 20, 0);
        CB->ajoute_zone(15, 190, -1735, 680, 485, 0);
        CB->ajoute_zone(22, 190, -1735, 705, 515, 0);
        CB->ajoute_zone(39, 475, -978, 700, 350, 0);
        CB->ajoute_zone(40, 478, -978, 800, 450, 0);
    }
    /* 0x7871 */
    if (ST(0x82C) == 0) {
        CB->etat_objet("OB26A", 1466, 778, -1376, 0xA00, 0x4C00, 0x5C00, -1, 3);
    }
    /* 0x78AD */
    if (ST(ST_LEVEL) != 2) {
        CB->pos_perso(ST(ST_HERO), 1482, -1975, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x78FB */
    if (ST(ST_LEVEL) == 2) {
        /* 0x790D */
        if (ST(0xAFC) == 4) {
            CB->pos_perso(ST(ST_HERO), 1510, -789, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        }
        /* 0x795E */
        if (ST(0xAFC) == 3) {
            CB->pos_perso(ST(ST_HERO), 1482, -1975, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        }
    }
    /* 0x79AF */
    if (ST(ST_VIE(P_ZED27)) != VIE_DEAD && ST(ST_HERO) == P_ARKHAN1 && ST(0xAF4) == 0 && ST(V_ZED_BRIEFING) == 3) {
        /* 0x79F7 */
        if (ST(0xBDC) == 0) {
            CB->pos_perso(P_ZED27, 400, -978, 0x3574, 1, "P27ARR03", 2, 601);
            CB->evenement(1, 1000);
        }
        /* 0x7A41 */
        if (ST(0xBDC) == 2) {
            ST(0xBDC) = 0;
            CB->pos_perso(P_ZED27, 400, -978, 0x3574, 1, "P27ARR03", 2, 601);
            CB->evenement(1, 1000);
        }
        /* 0x7A9A */
        if (ST(0xBDC) == 3) {
            /* 0x7AAC */
            if (ST(0xAFC) == 4) {
                CB->pos_perso(ST(ST_HERO), 651, -739, 0x7F94, 1, "ar", 1, 0);
                CB->pos_perso(P_ZED27, 902, -694, 0x4000, 1, "P27ARR03", 2, 601);
            }
            /* 0x7B11 */
            if (ST(0xAFC) == 3) {
                CB->pos_perso(P_ZED27, 400, -978, 0x3574, 1, "P27ARR03", 2, 601);
                CB->evenement(1, 1000);
            }
        }
    }
    /* 0x7B5B */
    if (ST(V_GUARD_FIGHT_DONE) == 0 && ST(0xEB8) == 0) {
        /* 0x7B7F */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->pos_perso(P_PHEDORIA, 147, -1750, 0xBE0F, 1, "P26ARR03", 3, 1);
            CB->evenement(2, 500);
        } else if (ST(0x764) == 0) {
            /* 0x7BDD */
            if (ST(0xAFC) == 3) {
                CB->pos_perso(P_PHEDORIA, 1200, -1596, 0xB9E2, 1, "P26ARR03", 3, 601);
            }
            /* 0x7C17 */
            if (ST(0xAFC) == 4) {
                CB->pos_perso(P_PHEDORIA, 927, -1101, 0xCFDC, 1, "P26ARR03", 3, 601);
            }
        } else {
            CB->pos_perso(P_PHEDORIA, 1202, -1036, 0x45D5, 1, "P26GAR03", 3, 601);
            CB->evenement(0, 1500);
        }
    }
    CB->bruitage("L07AMB1", -1, -1);
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    ST(0x80) = 0;
    ST(0x88) = 0;
    ST(V_EVENT15_STEP) = 0;
    ST(0x90) = 0;
    ST(0x94) = 0;
    ST(0xB8) = 0;
    ST(0xBC) = 0;
    ST(0x100) = 0;
    ST(0x114) = 0;
    ST(V_WEAPON_WARNED) = 0;
    /* 0x7D9F */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0 && ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
        ST(0xB98) = 0;
    }
    /* 0x7E08 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }
}

/* trigger 41 — sub_10007E67 */
static void trig41(void) {
    /* 0x7E79 */
    if (ST(0xBDC) == 3 && ST(ST_HERO) == P_ARKHAN1 && ST(0xAFC) == 4) {
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-P_ZED27, 400, -978, -1, -1);
        CB->animation("SP27ARR01_50");
        CB->modify_var(&ST(0xBDC), 0);
        CB->bloque(BLOQUE_NONE);
        CB->evenement(1, 1000);
        CB->select_pile(0, 1);
    }
}

/* trigger 02 — sub_1000A73D */
static void trig02(void) {
    /* 0xA74C */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 18) && !CB->direction_perso(P_ZED27)) {
        trig18();
        return;
    }
    /* 0xA798 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 19) && !CB->direction_perso(P_ZED27)) {
        trig19();
        return;
    }
    /* 0xA7E4 */
    if (ST(ST_COMBAT_STANCE) == 1 && (ST(V_ENGAGED_PERSONA) == 0 || ST(V_ENGAGED_PERSONA) == 0x63 || ST(V_ENGAGED_PERSONA) == P_DELIA) && ST(ST_SCRIPT_BUSY) == 0 && ST(0xBDC) == 0) {
        /* 0xA844 */
        if (ST(0x8FC) == 1) {
            CB->select_pile(1, 2);
            CB->bloque(BLOQUE_ALL);
            CB->divers(DV_CUT_SPEECH, 0, 0);
            CB->stop_joindre(-P_ZED27);
            CB->animation("SP27ARR01");
            CB->joindre(-P_ZED27, 0, 0, 0, ST(ST_HERO));
            CB->animation("SP27ARR01");
            CB->dialogue("L07D07", P_ZED27);
            ST(V_ENGAGED_PERSONA) = P_ZED27;
            ST(0xE5C) = 5;
            ST(0xBC) = 0;
            CB->combat(P_ZED27, 2);
            CB->vire_col(0x20, 1);
            CB->vire_col(0x21, 1);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
        /* 0xA942 */
        if (ST(0x8FC) == 0) {
            CB->select_pile(1, 2);
            CB->bloque(BLOQUE_ALL);
            ST(0x8FC) = 1;
            CB->divers(DV_CUT_SPEECH, 0, 0);
            CB->stop_joindre(-P_ZED27);
            CB->joindre(P_ZED27, 0, 0, 0, ST(ST_HERO));
            CB->dialogue("L07D16", P_ZED27);
            ST(V_ENGAGED_PERSONA) = P_ZED27;
            ST(0xBC) = 0;
            ST(0xE5C) = 5;
            CB->vire_col(0x20, 1);
            CB->vire_col(0x21, 1);
            CB->combat(P_ZED27, 2);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0xAA36 */
    if (ST(ST_ACTION_KEY) == 1 && (ST(V_ENGAGED_PERSONA) == 0 || ST(V_ENGAGED_PERSONA) == 0x63 || ST(V_ENGAGED_PERSONA) == P_DELIA) && ST(ST_SCRIPT_BUSY) == 0 && ST(0xBDC) == 0 && ST(ST_HERO) == P_ARKHAN1) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x1B);
        CB->select_pile(1, 1);
        CB->stop_joindre(-P_ZED27);
        CB->evenement(0, -1);
        CB->joindre(-P_ZED27, 0, 0, 0, ST(ST_HERO));
        /* 0xAB21 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("L07D18", P_ARKHAN1, "G00DIA05", NULL);
            lieu_action_register("L07D19", P_ZED27, "SP27DIA56", "SP27ARR01");
        }
        /* 0xAB62 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            lieu_action_register("L07D20", P_ARKHAN1, "SG00DIA03", NULL);
            lieu_action_register("L07D21", P_ZED27, "SP27DIA56", "SP27ARR01");
        }
        CB->joindre_perm(P_ZED27, ST(ST_HERO));
        CB->evenement(0, 10000);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        return;
    }
}

/* trigger 03 — sub_1000ABF5 */
static void trig03(void) {
    /* 0xAC04 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 21) && !CB->direction_perso(P_PHEDORIA)) {
        trig21();
        return;
    }
    /* 0xAC53 */
    if (ST(ST_HERO) == P_ARKHAN1 && ST(ST_COMBAT_STANCE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 22)) {
        /* 0xACB2 */
        if (ST(0x758) == 0) {
            /* 0xACC4 */
            if (ST(ST_PLAYER_WEAPON) == 0x1A || ST(ST_PLAYER_WEAPON) == 0x26) {
                CB->select_pile(2, 1);
                CB->dialogue("L07D22", P_PHEDORIA);
                ST(V_ENGAGED_PERSONA) = P_PHEDORIA;
                ST(0xE60) = 3;
                ST(V_EVENT15_STEP) = 1;
                CB->combat(P_PHEDORIA, 2);
                CB->select_pile(0, 1);
                return;
            } else {
                CB->select_pile(2, 1);
                CB->dialogue("L07D23", P_PHEDORIA);
                ST(0x758) = 2;
                return;
            }
        }
        /* 0xAD8D */
        if (ST(0x758) == 1 && ST(0xE60) != 3) {
            /* 0xADB1 */
            if (ST(ST_PLAYER_WEAPON) == 0x1A || ST(ST_PLAYER_WEAPON) == 0x26) {
                CB->select_pile(2, 1);
                CB->dialogue("L07D24", P_PHEDORIA);
                ST(V_ENGAGED_PERSONA) = P_PHEDORIA;
                ST(0xE60) = 3;
                ST(V_EVENT15_STEP) = 1;
                CB->combat(P_PHEDORIA, 2);
                CB->select_pile(0, 1);
                return;
            } else {
                CB->select_pile(2, 1);
                CB->bloque(BLOQUE_ALL);
                CB->dialogue("L07D25", P_PHEDORIA);
                ST(0x758) = 2;
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xAE8A */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_PHEDORIA)) {
        /* 0xAECD */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x1A);
            CB->select_pile(2, 1);
            CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
            CB->select_pile(0, 1);
            /* 0xAF4D */
            if (ST(0x88) == 0) {
                ST(0x88) = 1;
                /* 0xAF6E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L07D26", P_ARKHAN1, "SG00DIA03", NULL);
                    lieu_action_register("L07D27", P_PHEDORIA, "SP26DIA59", "SP26GAR26");
                }
                /* 0xAFAF */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L07D28", P_ARKHAN1, "SG00DIA03", NULL);
                    lieu_action_register("L07D29", P_PHEDORIA, "SP26DIA59", "SP26GAR26");
                }
                CB->animation("P26GAR03");
            } else {
                /* 0xB000 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L07D26", P_ARKHAN1, "SG00DIA03", NULL);
                    CB->dialogue("L07D27", P_PHEDORIA);
                }
                /* 0xB038 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L07D28", P_ARKHAN1, "SG00DIA03", NULL);
                    CB->dialogue("L07D29", P_PHEDORIA);
                }
                CB->animation("SP26GAR03");
            }
            ST(0x758) = 1;
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB0B3 */
        if (ST(ST_HERO) == P_ARKHAN2 || ST(ST_HERO) == P_ARKHAN3) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xB0EE */
            if (ST(0x764) == 0) {
                CB->select_pile(2, 2);
                CB->evenement(0x21, -1);
                ST(V_EVENT_33) = 0;
                /* 0xB12F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L07D57", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xB157 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L07D58", P_ARKHAN3, "SG00DIA06", NULL);
                }
                /* 0xB17C */
                if (ST(0x78) == 1) {
                    CB->stop_joindre(P_PHEDORIA);
                    CB->animation("SP26GAR03");
                    lieu_action_register("L07D59A", P_PHEDORIA, "SP26GAR03", "SP26GAR27");
                    CB->animation("P26ARR03_25");
                    lieu_action_register("L07D59B", P_PHEDORIA, "P26DIA53", "SP26ARR03");
                }
                /* 0xB1DE */
                if (ST(0x78) == 0) {
                    CB->stop_joindre(P_PHEDORIA);
                    CB->animation("SP26GAR03");
                    lieu_action_register("L07D59A", P_PHEDORIA, "SP26GAR03", "SP26GAR27");
                    CB->animation("P26ARR03_25");
                    lieu_action_register("L07D59B", P_PHEDORIA, "P26DIA53", "SP26ARR03");
                }
                ST(0x764) = 1;
                CB->joindre_perm(P_PHEDORIA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xB29E */
            if (ST(0x764) == 1) {
                /* 0xB2B0 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->stop_joindre(-P_PHEDORIA);
                    /* 0xB2CD */
                    if (ST(0x75C) == 0) {
                        ST(0x75C) = 1;
                        lieu_action_register("L07D60A", P_ARKHAN2, "SG00DIA01", NULL);
                        lieu_action_register("L07D60B", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L07D61A", P_PHEDORIA, "SP26DIA50", "SP26ARR03");
                        lieu_action_register("L07D61B", P_PHEDORIA, "SP26DIA53", "SP26ARR03");
                        lieu_action_register("L07D61C", P_PHEDORIA, "SP26DIA54", "SP26ARR03");
                        CB->joindre_perm(P_PHEDORIA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xB3A4 */
                    if (ST(0x75C) == 1) {
                        CB->divers(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                        ST(0x75C) = 2;
                        lieu_action_register("L07D62A", P_ARKHAN2, "SG00DIA03", NULL);
                        lieu_action_register("L07D62B", P_ARKHAN2, "SG00DIA04", NULL);
                        lieu_action_register("L07D63", P_PHEDORIA, "SP26DIA51", "SP26ARR03");
                        lieu_action_register("L07D65", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L07D64A", P_PHEDORIA, "SP26DIA56", "SP26ARR03");
                        lieu_action_register("L07D64B", P_PHEDORIA, "SP26DIA59", "SP26ARR03");
                        lieu_action_register("L07D64C", P_PHEDORIA, "SP26DIA60", "SP26ARR03");
                        CB->joindre(P_PHEDORIA, 1202, -1036, 0x45D5, -1);
                        CB->animation("SP26GAR03");
                        CB->divers(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                        return;
                    }
                    /* 0xB4BB */
                    if (ST(0x75C) >= 2) {
                        lieu_action_register("L07D79", P_ARKHAN2, "SG00DIA06", NULL);
                        /* 0xB4E3 */
                        if (ST(0x80) == 0) {
                            CB->dialogue("L07D80A", P_PHEDORIA);
                            CB->dialogue("L07D80B", P_PHEDORIA);
                            ST(0x80) = 1;
                        } else {
                            CB->dialogue("L07D81", P_PHEDORIA);
                        }
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xB562 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->stop_joindre(-P_PHEDORIA);
                    /* 0xB57F */
                    if (ST(0x760) == 0) {
                        ST(0x760) = 1;
                        lieu_action_register("L07D70A", P_ARKHAN2, "SG00DIA01", NULL);
                        lieu_action_register("L07D70B", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L07D61A", P_PHEDORIA, "SP26DIA50", "SP26ARR03");
                        lieu_action_register("L07D61B", P_PHEDORIA, "SP26DIA53", "SP26ARR03");
                        lieu_action_register("L07D61C", P_PHEDORIA, "SP26DIA54", "SP26ARR03");
                        CB->joindre_perm(P_PHEDORIA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xB656 */
                    if (ST(0x760) == 1) {
                        CB->divers(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                        ST(0x760) = 2;
                        lieu_action_register("L07D72", P_ARKHAN2, "SG00DIA03", NULL);
                        lieu_action_register("L07D63", P_PHEDORIA, "SP26DIA51", "SP26ARR03");
                        lieu_action_register("L07D82", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L07D64A", P_PHEDORIA, "SP26DIA56", "SP26ARR03");
                        lieu_action_register("L07D64B", P_PHEDORIA, "SP26DIA59", "SP26ARR03");
                        lieu_action_register("L07D64C", P_PHEDORIA, "SP26DIA60", "SP26ARR03");
                        CB->joindre(P_PHEDORIA, 1202, -1036, 0x45D5, -1);
                        CB->animation("SP26GAR03");
                        CB->divers(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                        return;
                    }
                    /* 0xB757 */
                    if (ST(0x760) >= 2) {
                        CB->divers_empile(DV_LOOK_AT_PLAYER, P_PHEDORIA, 1);
                        lieu_action_register("L07D73", P_ARKHAN3, "SG00DIA06", NULL);
                        /* 0xB791 */
                        if (ST(0x80) == 0) {
                            CB->dialogue("L07D80A", P_PHEDORIA);
                            CB->dialogue("L07D80B", P_PHEDORIA);
                            ST(0x80) = 1;
                        } else {
                            CB->dialogue("L07D81", P_PHEDORIA);
                        }
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->divers_empile(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
        }
    }
    /* 0xB81F */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(V_EVENT15_STEP) == 0 && CB->direction_perso(P_PHEDORIA)) {
        ST(V_EVENT15_STEP) = 1;
    }
    /* 0xB862 */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_PHEDORIA)) {
        CB->select_pile(2, 2);
        ST(V_PLAYER_STRIKE) = -1;
        ST(0xE60) = 3;
        CB->dialogue("L07D30", P_PHEDORIA);
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = P_PHEDORIA;
        CB->combat(P_PHEDORIA, 2);
        CB->select_pile(0, 1);
        return;
    }
}

/* trigger 04 — sub_1000B904 */
static void trig04(void) {
    /* 0xB913 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(4)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xB96D */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L07D51", P_ARKHAN1, "SG00PRE03", NULL);
        }
        /* 0xB995 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L07D52", P_ARKHAN2, "SG00PRE03", NULL);
        }
        /* 0xB9BD */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L07D53", P_ARKHAN3, "SG00PRE03", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 05 — sub_1000B9FC */
static void trig05(void) {
    ST(0xAFC) = 2;
    ST(ST_LEVEL) = 7;
    CB->change_piece(2, 0);
}

/* trigger 06 — sub_1000BA2F */
static void trig06(void) {
    ST(0xAFC) = 1;
    ST(ST_LEVEL) = 7;
    CB->change_piece(2, 0);
}

/* trigger 16 — sub_1000BAC5 */
static void trig16(void) {
    /* 0xBAD4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(16)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBB2E */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0xBB40 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L07D33A", P_ARKHAN1, "SG00PRE01", NULL);
                CB->dialogue("L07D33B", P_ARKHAN1);
            }
            /* 0xBB78 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L07D34A", P_ARKHAN1, "SG00PRE01", NULL);
                CB->dialogue("L07D34B", P_ARKHAN1);
            }
            CB->animation("ar_50");
        }
        /* 0xBBBB */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L07D35", P_ARKHAN2, "SG00PRE01", NULL);
        }
        /* 0xBBE3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L07D37", P_ARKHAN3, "SG00PRE01", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 17 — sub_1000BC22 */
static void trig17(void) {
    /* 0xBC31 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBC8B */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L07D39A", P_ARKHAN1, "SG00PRE03", NULL);
            /* 0xBCB3 */
            if (ST(V_WEAPON_WARNED) == 0) {
                ST(V_WEAPON_WARNED) = 1;
                CB->dialogue("L07D39B", P_ARKHAN1);
            }
            CB->animation("ar_50");
        }
        /* 0xBCEF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L07D41", P_ARKHAN2, "SG00PRE03", NULL);
        }
        /* 0xBD17 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L07D43", P_ARKHAN3, "SG00PRE03", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 18 — sub_1000BD56 */
static void trig18(void) {
    /* 0xBD65 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBDBF */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L07D45", P_ARKHAN1, "SG00PRE01", NULL);
        }
        /* 0xBDE7 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L07D46", P_ARKHAN2, "SG00PRE01", NULL);
        }
        /* 0xBE0F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L07D47", P_ARKHAN3, "SG00PRE01", NULL);
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 19 — sub_1000BE59 */
static void trig19(void) {
    /* 0xBE68 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBEC2 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L07D48", P_ARKHAN1, "SG00PRE01", NULL);
        }
        /* 0xBEEA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L07D49", P_ARKHAN2, "SG00PRE01", NULL);
        }
        /* 0xBF12 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L07D50", P_ARKHAN3, "SG00PRE01", NULL);
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 20 — sub_1000BF5C */
static void trig20(void) {
    /* 0xBF6B */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBFC2 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L07D51", P_ARKHAN1, "SG00PRE03", NULL);
        }
        /* 0xBFEA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L07D52", P_ARKHAN2, "SG00PRE03", NULL);
        }
        /* 0xC012 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L07D53", P_ARKHAN3, "SG00PRE03", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 21 — sub_1000C051 */
static void trig21(void) {
    /* 0xC060 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(21)) {
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x82C) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->retire_zone(21);
        CB->joindre_obj("OB26A", 50);
        /* 0xC0E4 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->animation("SG00PRE07");
        }
        /* 0xC101 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00PRE07");
        }
        /* 0xC11E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00PRE07");
        }
        CB->etat_objet("OB26A", -1, -1, -1, -1, -1, -1, -3, 0);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 25 — sub_1000C18C */
static void trig25(void) {
    /* 0xC19B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_PHEDORIA) && CB->distance(1, 3) < 250 && ST(ST_HERO) > 1) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC213 */
        if (ST(0x764) == 0) {
            CB->select_pile(2, 2);
            CB->evenement(0x21, -1);
            ST(V_EVENT_33) = 0;
            /* 0xC254 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L07D57", P_ARKHAN2, "SG00DIA06", NULL);
            }
            /* 0xC27C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L07D58", P_ARKHAN3, "SG00DIA06", NULL);
            }
            /* 0xC2A1 */
            if (ST(0x78) == 1) {
                lieu_action_register("L07D59A", P_PHEDORIA, "SP26GAR03", "SP26GAR27");
                lieu_action_register("L07D59B", P_PHEDORIA, "SP26GAR03", "SP26GAR27");
                CB->animation("P26ARR03_50");
            }
            /* 0xC2ED */
            if (ST(0x78) == 0) {
                lieu_action_register("L07D59A", P_PHEDORIA, "SP26DIA53", "SP26ARR03");
                lieu_action_register("L07D59B", P_PHEDORIA, "SP26DIA53", "SP26ARR03");
            }
            ST(0x764) = 1;
            CB->joindre_perm(P_PHEDORIA, ST(ST_HERO));
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
        /* 0xC38C */
        if (ST(0x764) == 1) {
            /* 0xC39E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->stop_joindre(-P_PHEDORIA);
                /* 0xC3BB */
                if (ST(0x75C) == 0) {
                    ST(0x75C) = 1;
                    lieu_action_register("L07D60A", P_ARKHAN2, "SG00DIA01", NULL);
                    lieu_action_register("L07D60B", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L07D61A", P_PHEDORIA, "SP26DIA50", "SP26ARR03");
                    lieu_action_register("L07D61B", P_PHEDORIA, "SP26DIA53", "SP26ARR03");
                    lieu_action_register("L07D61C", P_PHEDORIA, "SP26DIA54", "SP26ARR03");
                    CB->joindre_perm(P_PHEDORIA, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xC492 */
                if (ST(0x75C) == 1) {
                    CB->divers(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                    ST(0x75C) = 2;
                    lieu_action_register("L07D62A", P_ARKHAN2, "SG00DIA03", NULL);
                    lieu_action_register("L07D62B", P_ARKHAN2, "SG00DIA04", NULL);
                    lieu_action_register("L07D63", P_PHEDORIA, "SP26DIA51", "SP26ARR03");
                    lieu_action_register("L07D65", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L07D64A", P_PHEDORIA, "SP26DIA56", "SP26ARR03");
                    lieu_action_register("L07D64B", P_PHEDORIA, "SP26DIA59", "SP26ARR03");
                    lieu_action_register("L07D64C", P_PHEDORIA, "SP26DIA60", "SP26ARR03");
                    CB->joindre(P_PHEDORIA, 1202, -1036, 0x45D5, -1);
                    CB->animation("SP26GAR03");
                    CB->divers(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                    return;
                }
                /* 0xC5A9 */
                if (ST(0x75C) >= 2) {
                    lieu_action_register("L07D79", P_ARKHAN2, "SG00DIA06", NULL);
                    /* 0xC5D1 */
                    if (ST(0x80) == 0) {
                        CB->dialogue("L07D80A", P_PHEDORIA);
                        CB->dialogue("L07D80B", P_PHEDORIA);
                        ST(0x80) = 1;
                    } else {
                        CB->dialogue("L07D81", P_PHEDORIA);
                    }
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xC650 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->stop_joindre(-P_PHEDORIA);
                /* 0xC66D */
                if (ST(0x760) == 0) {
                    ST(0x760) = 1;
                    lieu_action_register("L07D70A", P_ARKHAN2, "SG00DIA01", NULL);
                    lieu_action_register("L07D70B", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L07D61A", P_PHEDORIA, "SP26DIA50", "SP26ARR03");
                    lieu_action_register("L07D61B", P_PHEDORIA, "SP26DIA53", "SP26ARR03");
                    lieu_action_register("L07D61C", P_PHEDORIA, "SP26DIA54", "SP26ARR03");
                    CB->joindre_perm(P_PHEDORIA, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xC744 */
                if (ST(0x760) == 1) {
                    CB->divers(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                    ST(0x760) = 2;
                    lieu_action_register("L07D72", P_ARKHAN2, "SG00DIA03", NULL);
                    lieu_action_register("L07D63", P_PHEDORIA, "SP26DIA51", "SP26ARR03");
                    lieu_action_register("L07D82", P_ARKHAN2, "SG00DIA05", NULL);
                    lieu_action_register("L07D64A", P_PHEDORIA, "SP26DIA56", "SP26ARR03");
                    lieu_action_register("L07D64B", P_PHEDORIA, "SP26DIA59", "SP26ARR03");
                    lieu_action_register("L07D64C", P_PHEDORIA, "SP26DIA60", "SP26ARR03");
                    CB->joindre(P_PHEDORIA, 1202, -1036, 0x45D5, -1);
                    CB->animation("SP26GAR03");
                    CB->divers(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                    return;
                }
                /* 0xC845 */
                if (ST(0x760) >= 2) {
                    CB->divers_empile(DV_LOOK_AT_PLAYER, P_PHEDORIA, 1);
                    lieu_action_register("L07D73", P_ARKHAN3, "SG00DIA06", NULL);
                    /* 0xC87F */
                    if (ST(0x80) == 0) {
                        CB->dialogue("L07D80A", P_PHEDORIA);
                        CB->dialogue("L07D80B", P_PHEDORIA);
                        ST(0x80) = 1;
                    } else {
                        CB->dialogue("L07D81", P_PHEDORIA);
                    }
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->divers_empile(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
}
