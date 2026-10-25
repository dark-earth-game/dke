/*
 * lieu09_trig.c — Native hand-port of the LIEU09.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu09.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu09_int.h"

/* Global sprintf buffer (dword_1001A4A0); shared with lieu09.c in the DLL,
 * each file keeps its own scratch copy here since neither reads it back. */
static c8 g_txt[256];

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig01(void); static void trig02(void); static void trig03(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig10(void);
static void trig12(void); static void trig41(void);
static void trig_noop(void);   /* the 31 stub slots */

/* dword_1001A6A0[42] — filled at INIT with the 5-byte jmp thunks at
 * 0x10001000+5*i; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  10009D54 stub  28  10009DEE stub
 *    1  1000825D       15  10009D5F stub  29  10009DF9 stub
 *    2  1000833B       16  10009D6A stub  30  10009E04 stub
 *    3  1000884F       17  10009D75 stub  31  10009E0F stub
 *    4  10008953 stub  18  10009D80 stub  32  10009E1A stub
 *    5  1000895E       19  10009D8B stub  33  10009E25 stub
 *    6  10008AB1       20  10009D96 stub  34  10009E30 stub
 *    7  10008DBD       21  10009DA1 stub  35  10009E3B stub
 *    8  10009075       22  10009DAC stub  36  10009E46 stub
 *    9  10009665       23  10009DB7 stub  37  10009E51 stub
 *   10  1000991D       24  10009DC2 stub  38  10009E5C stub
 *   11  10009AA7 stub  25  10009DCD stub  39  10009E67 stub
 *   12  10009AB2       26  10009DD8 stub  40  10009E72 stub
 *   13  10009D49 stub  27  10009DE3 stub  41  1000786E
 */
static void (*const g_trigger[42])(void) = {
    l09_trig00, trig01, trig02, trig03, trig_noop, trig05, trig06, trig07,
    trig08, trig09, trig10, trig_noop, trig12, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001A6A0]). */
void l09_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start-point presets, state reset, zones, objects, actor placement */
void l09_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(9)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
    }
    /* 0x7423 */
    if (ST(ST_START_POINT(9)) == 2) {
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xC08) = 2;
    }
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
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(ST_START_POINT(9)) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x88) = 0;
    ST(V_EVENT15_STEP) = 0;
    CB->bruitage("L09AMB1", -1, -1);
    CB->ajoute_zone(1, 508, -574, 105, 65, 0);
    CB->ajoute_zone(2, 320, -132, 160, 160, 0);
    CB->ajoute_zone(5, 838, -489, 140, 85, 0);
    CB->ajoute_zone(6, 866, -130, 120, 50, 0);
    CB->ajoute_zone(7, 550, -101, 175, 165, 0);
    CB->ajoute_zone(8, 731, -176, 125, 70, 0);
    CB->ajoute_zone(9, 284, -519, 200, 250, 0);
    CB->ajoute_zone(10, 279, -373, 200, 200, 0);
    CB->ajoute_zone(11, 243, -50, 20, 20, 0);
    CB->ajoute_zone(12, 945, -280, 120, 325, 0);
    CB->etat_objet("IM71", 464, 13, -618, 0xC000, 0x4000, 0x8000, 0, 3);
    CB->etat_objet("IM23", 730, 38, -98, 0, 0x8000, 0, 0, 3);
    CB->pos_perso(ST(ST_HERO), 538, -512, 0x2D1, 1, "ar", 4, 0);
    /* 0x772D */
    if (ST(0xC08) == 2) {
        /* 0x773F */
        if (ST(ST_VIE(P_GUARDIAN8)) != VIE_DEAD) {
            CB->pos_perso(P_GUARDIAN8, 858, -305, 0x2231, 1, "P48ARR04", 3, 0);
        } else {
            CB->pos_perso(P_GUARDIAN8, 0, 0, 0, 2, "P48CAD", 3, 0);
        }
    }
    /* 0x778B */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0 && ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
        ST(0xB98) = 0;
    }
    /* 0x77F4 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }
    ST(V_WEAPON_WARNED) = 0;
}

/* trigger 41 — sub_1000786E — guardian 8 challenges the hero (cutscene) */
static void trig41(void) {
    /* 0x7880 */
    if (ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xC08) == 2 && ST(ST_VIE(P_GUARDIAN8)) != VIE_DEAD) {
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_GUARDIAN8);
        CB->change_camera(3, CAM_LOCKED);
        CB->joindre(-P_GUARDIAN8, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L09D31A", P_GUARDIAN8, "SP48GAR05", "SP48GAR05");
        CB->dialogue("L09D31B", P_GUARDIAN8);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        CB->change_camera(99, CAM_AUTO);
        CB->combat(P_GUARDIAN8, 0);
        CB->select_pile(0, 1);
    }
}

/* trigger 01 — sub_1000825D */
static void trig01(void) {
    /* 0x826C */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x829C */
        if (ST(ST_SPECIAL_MODE) == 1) {
            CB->evenement(1, 400);
        }
        /* 0x82BE */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->evenement(1, 650);
        }
        /* 0x82E0 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->animation("SG01OUV01");
        }
        /* 0x82FD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02OUV01");
        }
        /* 0x831A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03OUV01");
        }
        CB->animation("ar_50");
    }
}

/* trigger 02 — sub_1000833B */
static void trig02(void) {
    /* 0x834A */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_perso(P_GUARDIAN8)) {
        trig03();
        return;
    }
    /* 0x8396 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(11)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x83F0 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x8402 */
            if (ST(ST_PLAYER_WEAPON) == OB_LEAD_PLAQUE) {
                CB->pos_perso(ST(ST_HERO), 377, -147, 0x2AF8, 1, "ar", 4, 0);
                /* 0x843F */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01DON04");
                }
                /* 0x845C */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02DON04");
                }
                /* 0x8479 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03DON04");
                }
                ST(V_EVENT15_STEP) = ST(ST_USED_VARIANT);
                snprintf(g_txt, sizeof(g_txt), "OB33%c", (ST(ST_USED_VARIANT) + 65));
                CB->etat_objet(g_txt, -1, -1, -1, -1, -1, -1, -2, 3);
                /* 0x84EC */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01DON05");
                    CB->animation("P01PRE04");
                    CB->animation("P01PRE04");
                }
                /* 0x851F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02DON05");
                    CB->animation("P02PRE04");
                    CB->animation("P03PRE04");
                }
                /* 0x8552 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03DON05");
                    CB->animation("P03PRE04");
                    CB->animation("P03PRE04");
                }
                CB->animation("ar");
                CB->bruitage("L09B01", 320, -132);
                ST(V_EVENT_25) = 1;
                return;
            }
            /* 0x85B9 */
            if (ST(ST_PLAYER_WEAPON) != OB_LEAD_PLAQUE) {
                /* 0x85CB */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    lieu_action_register("L09D04A", P_ARKHAN1, "SG01DIA02", "SG01DIA01");
                    lieu_action_register("L09D04B", P_ARKHAN1, "SG01DIA01", NULL);
                }
                /* 0x860C */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L09D05", P_ARKHAN2, "SG02DIA07", NULL);
                }
                /* 0x8634 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L09D06", P_ARKHAN3, "SG03DIA07", NULL);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x867A */
        if (ST(ST_SPECIAL_MODE) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->pos_perso(ST(ST_HERO), 377, -147, 0x2AF8, 1, "ar", 4, 0);
            /* 0x86C3 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT06");
            }
            /* 0x86E0 */
            if (ST(ST_HERO) == P_ARKHAN2 && CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP02ACT01");
            }
            /* 0x8717 */
            if (ST(ST_HERO) == P_ARKHAN2 && !CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP02ACT06");
            }
            /* 0x874E */
            if (ST(ST_HERO) == P_ARKHAN3 && CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP03ACT01");
            }
            /* 0x8785 */
            if (ST(ST_HERO) == P_ARKHAN3 && !CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP03ACT06");
            }
            CB->animation("ar_50");
            /* 0x87C7 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("L09D01", P_ARKHAN1);
            }
            /* 0x87E9 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L09D02", P_ARKHAN2);
            }
            /* 0x880B */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L09D03", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 03 — sub_1000884F — guardian 8 (talk, or search the corpse) */
static void trig03(void) {
    /* 0x885E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN8)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        /* 0x88AB */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD20", P_ARKHAN2);
        }
        /* 0x88CD */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x88F9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN8)) == VIE_DEAD) {
        CB->fouille(P_GUARDIAN8);
        return;
    }
}

/* trigger 05 — sub_1000895E */
static void trig05(void) {
    /* 0x896D */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_perso(P_GUARDIAN8)) {
        trig03();
        return;
    }
    /* 0x89B9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8A22 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("LCOD06", P_ARKHAN1, "SG01PRE01", NULL);
        }
        /* 0x8A4A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG02PRE01", NULL);
        }
        /* 0x8A72 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN3, "SG03PRE01", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 06 — sub_10008AB1 */
static void trig06(void) {
    /* 0x8AC0 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_perso(P_GUARDIAN8)) {
        trig03();
        return;
    }
    /* 0x8B0C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xFFFF)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8B75 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x8B87 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D13", P_ARKHAN1, "SG01PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8BCD */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP01ACT32");
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8C1E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8C30 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D14", P_ARKHAN2, "SG02PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8C76 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x8C90 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT32");
                } else {
                    CB->animation("SP02ACT02");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8CF1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8D03 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D15", P_ARKHAN3, "SG03PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8D49 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x8D63 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT32");
                } else {
                    CB->animation("SP03ACT02");
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 07 — sub_10008DBD */
static void trig07(void) {
    /* 0x8DCC */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_perso(P_GUARDIAN8)) {
        trig03();
        return;
    }
    /* 0x8E18 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(7)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8E81 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x8E93 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D13", P_ARKHAN1, "SG01PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8ED9 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP01ACT32");
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8F2A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8F3C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D14", P_ARKHAN2, "SG02PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8F82 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT32");
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8FD3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8FE5 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D15", P_ARKHAN3, "SG03PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x902B */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT32");
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 08 — sub_10009075 — the door IM23 (keys, lead plaque slot) */
static void trig08(void) {
    /* 0x9084 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_perso(P_GUARDIAN8)) {
        trig03();
        return;
    }
    /* 0x90D0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), 738, -205, 0xFFFF, 1, "ar", 4, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9152 — a key that does not fit this door */
        if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY || ST(ST_PLAYER_WEAPON) == OB_KONK_KEY || ST(ST_PLAYER_WEAPON) == OB_SORDOS_KEY || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_KEY || ST(ST_PLAYER_WEAPON) == OB_STOCK_KEY || ST(ST_PLAYER_WEAPON) == OB_LARGE_KEY) {
            /* 0x91BE */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02OUV10");
                CB->animation("ar");
            }
            /* 0x91E6 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02OUV50");
                CB->animation("ar");
            }
            /* 0x920E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L04D44", P_ARKHAN2);
            }
            /* 0x9230 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L04D45", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x926F */
        if (ST(ST_PLAYER_WEAPON) == OB_BOGD_KEY && ST(ST_HERO) > 1) {
            /* 0x929F */
            if (ST(0xE44) <= 1) {
                /* 0x92B1 */
                if (ST(0xE44) == 0) {
                    CB->animation("SG02OUV10");
                    CB->animation("G02OUV11");
                    /* 0x92D9 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->evenement(2, 500);
                    }
                    /* 0x92FB */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->evenement(2, 350);
                    }
                    return;
                }
                /* 0x9322 */
                if (ST(0xE44) == 1) {
                    /* 0x9334 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SG02OUV04");
                        CB->animation("G02OUV08");
                        CB->anim_objet("IM23", 1, -54, 0x3E8, 0);
                    }
                    /* 0x9375 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SG02OUV44");
                        CB->animation("G02OUV48");
                        CB->anim_objet("IM23", 1, -54, 0x12C, 0x200);
                    }
                    CB->evenement(33, 1200);
                    return;
                }
            }
        } else {
            /* 0x93D3 */
            if (ST(0xE44) == 0) {
                CB->animation("SG02OUV04");
                CB->animation("G02OUV09");
                CB->animation("ar_100");
                /* 0x9406 */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    CB->dialogue("LCOD12", P_ARKHAN1);
                }
                /* 0x9428 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD13", P_ARKHAN2);
                }
                /* 0x944A */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD14", P_ARKHAN3);
                }
                CB->animation("Sar_100");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9495 */
            if (ST(0xE44) == 1 && ST(ST_HERO) > 1) {
                /* 0x94B9 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SG02OUV04");
                    CB->animation("G02OUV08");
                    CB->anim_objet("IM23", 1, -54, 0x3E8, 0);
                }
                /* 0x94FA */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SG02OUV44");
                    CB->animation("G02OUV48");
                    CB->anim_objet("IM23", 1, -54, 0x12C, 0x200);
                }
                CB->evenement(33, 1200);
                return;
            }
        }
        /* 0x9547 */
        if (ST(0xE44) == 2 && ST(ST_HERO) > 1) {
            CB->animation("SG02OUV04");
            /* 0x9582 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("G02OUV06");
                CB->anim_objet("IM23", 1, -54, 0x3E8, 0);
            }
            /* 0x95B8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("G02OUV46");
                CB->anim_objet("IM23", 1, -54, 0x12C, 0x200);
            }
            /* 0x95F1 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("LCOD09", P_ARKHAN1);
            }
            /* 0x9613 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD10", P_ARKHAN2);
            }
            /* 0x9635 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD11", P_ARKHAN3);
            }
            CB->evenement(35, 1200);
            return;
        }
    }
}

/* trigger 09 — sub_10009665 */
static void trig09(void) {
    /* 0x9674 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_perso(P_GUARDIAN8)) {
        trig03();
        return;
    }
    /* 0x96C0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(9)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9729 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x973B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D13", P_ARKHAN1, "SG01PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9781 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP01ACT32");
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x97D2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x97E4 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D14", P_ARKHAN2, "SG02PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x982A */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT32");
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x987B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x988D */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D15", P_ARKHAN3, "SG03PRE03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x98D3 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT32");
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 10 — sub_1000991D */
static void trig10(void) {
    /* 0x992C */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_perso(P_GUARDIAN8)) {
        trig03();
        return;
    }
    /* 0x9978 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(10)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x99E1 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L09D20", P_ARKHAN1, "SG01PRE02", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9A27 — no return here in the original: falls into the Arkhan3 test */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L09D22", P_ARKHAN2, "SG02PRE02", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
        /* 0x9A68 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L09D24", P_ARKHAN3, "SG03PRE02", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 12 — sub_10009AB2 */
static void trig12(void) {
    /* 0x9AC1 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && CB->direction_perso(P_GUARDIAN8)) {
        trig03();
        return;
    }
    /* 0x9B0D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xBCAD)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9B76 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x9B88 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D25", P_ARKHAN1, "SG01DIA02", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9BCE */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L09D26", P_ARKHAN1, "SG01DIA02", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x9C14 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x9C26 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D27", P_ARKHAN2, "SG02DIA02", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9C6C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L09D28", P_ARKHAN2, "SG02DIA02", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x9CB2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x9CC4 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L09D29", P_ARKHAN3, "SG03DIA02", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9D0A */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L09D30", P_ARKHAN3, "SG03DIA02", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}
