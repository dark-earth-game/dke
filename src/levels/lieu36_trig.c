/*
 * lieu36_trig.c - Native hand-port of the LIEU36.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and the tick body live in
 * lieu36.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries its
 * original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu36_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig03(void); static void trig05(void); static void trig06(void);
static void trig07(void); static void trig09(void); static void trig10(void);
static void trig11(void); static void trig14(void); static void trig15(void);
static void trig16(void); static void trig17(void); static void trig41(void);
static void trig_noop(void);   /* the 30 stub slots */

/* dword_10020B70[42] - filled at INIT with the 5-byte jmp thunks at
 * 0x10001000+5*i (in a shuffled order); static here.
 *   idx handler        idx handler        idx handler
 *    0  10007547       14  1000D4B1       28  1000DF9B stub
 *    1  10009BFF stub  15  1000D610       29  1000DFA6 stub
 *    2  10009C0A       16  1000D964       30  1000DFB1 stub
 *    3  1000B3D1       17  1000DE1A       31  1000DFBC stub
 *    4  1000C0E1       18  1000DF2D stub  32  1000DFC7 stub
 *    5  1000CB3F       19  1000DF38 stub  33  1000DFD2 stub
 *    6  1000CD1C       20  1000DF43 stub  34  1000DFDD stub
 *    7  1000CECD       21  1000DF4E stub  35  1000DFE8 stub
 *    8  1000CFB2 stub  22  1000DF59 stub  36  1000DFF3 stub
 *    9  1000CFBD       23  1000DF64 stub  37  1000DFFE stub
 *   10  1000D12F       24  1000DF6F stub  38  1000E009 stub
 *   11  1000D28B       25  1000DF7A stub  39  1000E014 stub
 *   12  1000D49B stub  26  1000DF85 stub  40  1000E01F stub
 *   13  1000D4A6 stub  27  1000DF90 stub  41  10007D6C
 */
static void (*const g_trigger[42])(void) = {
    l36_trig00, trig_noop, l36_trig02, trig03, l36_trig04, trig05, trig06, trig07,
    trig_noop, trig09, trig10, trig11, trig_noop, trig_noop, trig14, trig15,
    trig16, trig17, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10020B70]). */
void l36_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* -------------------------------------------------------------------------
 * trig 0 — sub_10007547 (thunk sub_10001073); INIT default: story seed for the designers' start points, zones, persona placement
 * ---------------------------------------------------------------------- */
void l36_trig00(void) {
    /* 0x7559 */
    if (ST(ST_START_POINT(36)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 0;
    }
    /* 0x7589 */
    if (ST(ST_START_POINT(36)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
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
    ST(V_WEAPON_WARNED) = 0;
    ST(0xE0) = 0;
    ST(0xF0) = 0;
    ST(0xF4) = 0;
    ST(0xF8) = 0;
    ST(0x50) = 0;
    ST(0x54) = 0;
    l36_prev_8d4 = ST(0x8D4);
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_PLAYER_STRIKE) = -1;
    CB->bruitage("L36AMB1", -1, -1);
    CB->pos_perso(ST(ST_HERO), 2920, 5120, 0, 1, "ar", 1, 0);
    /* 0x7762 */
    if (ST(0xF50) == 0 && ST(V_GUARD_FIGHT_DONE) >= 2) {
        ST(0xF50) = 1;
    }
    /* 0x7795 */
    if (ST(V_GUARD_FIGHT_DONE) >= 2) {
        ST(ST_VIE(P_GUARDIAN20)) = 4;
        /* 0x77B6 */
        if (ST(ST_VIE(P_LODAR)) == 0) {
            ST(ST_VIE(P_LODAR)) = 1;
        }
        /* 0x77D7 */
        if (ST(ST_VIE(P_KONKALITE12)) == 0) {
            ST(ST_VIE(P_KONKALITE12)) = 1;
        }
    }
    CB->ajoute_zone(5, 2970, 5015, 280, 80, 1);
    CB->ajoute_zone(6, 3600, 6224, 135, 110, 0);
    CB->ajoute_zone(7, 3460, 6240, 165, 150, 0);
    CB->ajoute_zone(8, 3420, 6330, 30, 30, 0);
    CB->ajoute_zone(9, 4700, 6440, 140, 160, 0);
    CB->ajoute_zone(10, 4680, 6520, 160, 140, 0);
    CB->ajoute_zone(11, 5320, 6700, 200, 340, 0);
    CB->ajoute_zone(12, 5330, 6645, 30, 30, 0);
    CB->ajoute_zone(13, 5280, 6800, 30, 30, 0);
    CB->ajoute_zone(14, 6375, 6175, 230, 200, 0);
    CB->ajoute_zone(15, 4900, 6745, 645, 500, 0);
    CB->ajoute_zone(16, 4100, 6280, 660, 430, 0);
    CB->ajoute_zone(17, 2993, 5265, 150, 150, 0);
    CB->ajoute_zone(19, 6101, 6463, 150, 150, 0);
    /* 0x79AE */
    if (ST(ST_VIE(P_GUARDIAN20)) < 2) {
        CB->pos_perso(P_GUARDIAN20, 3720, 6154, 0x4E20, 1, "P20ARR01", 4, 0);
    }
    /* 0x79E5 */
    if (ST(ST_VIE(P_GUARDIAN20)) == VIE_DEAD) {
        CB->pos_perso(P_GUARDIAN20, 0, 0, 0, 2, "P20cad", 4, 0);
    }
    /* 0x7A13 */
    if (ST(V_GUARD_FIGHT_DONE) < 2 && ST(0xA50) == 0) {
        CB->pos_perso(P_GUARDIAN1, 3840, 5876, 0xC350, 1, "P41ARR02", 0x12, 0);
    }
    /* 0x7A5C */
    if (ST(V_GUARD_FIGHT_DONE) < 2 && ST(0xA50) == 1) {
        CB->pos_perso(85, 3840, 5876, 0xC350, 1, "P85ARR03", 0x12, 0);
    }
    /* 0x7AA5 */
    if (ST(ST_VIE(P_LODAR)) == 1) {
        /* 0x7AB7 */
        if (ST(0xF50) == 1) {
            CB->pos_perso(P_LODAR, 6365, 6330, 0x59D8, 1, "P86GAR02", 2, 0);
        }
        /* 0x7AEE */
        if (ST(0xF50) == 2) {
            ST(ST_VIE(P_LODAR)) = VIE_DEAD;
        }
        /* 0x7B0F */
        if (ST(0xF50) == 5) {
            /* 0x7B21 */
            if (ST(0xF54) == 3) {
                CB->pos_perso(P_LODAR, 3055, 5844, 0x7148, 1, "P86GAR02", 2, 0);
                CB->combat(P_LODAR, 0);
                ST(V_ENGAGED_PERSONA) = P_LODAR;
            } else {
                CB->pos_perso(P_LODAR, 4840, 6830, 0xFA0, 1, "P86ARR01", 2, 0);
            }
        }
    }
    /* 0x7B9B */
    if (ST(ST_VIE(P_LODAR)) == VIE_DEAD) {
        CB->pos_perso(P_LODAR, 0, 0, 0, 2, "P86cad", 2, 0);
    }
    /* 0x7BC9 */
    if (ST(ST_VIE(P_KONKALITE12)) == 1) {
        /* 0x7BDB */
        if (ST(0xF50) == 1) {
            CB->pos_perso(P_KONKALITE12, 6220, 6230, 0xD6D8, 1, "P82GAR03", 3, 0);
        }
        /* 0x7C12 */
        if (ST(0xF50) == 2) {
            CB->pos_perso(P_KONKALITE12, 6220, 6230, 0xD6D8, 1, "P82GAR03", 3, 0);
            CB->combat(P_KONKALITE12, 0);
            ST(V_ENGAGED_PERSONA) = P_KONKALITE12;
        }
        /* 0x7C62 */
        if (ST(0xF50) == 3) {
            /* 0x7C74 */
            if (ST(0xF54) == 2) {
                CB->pos_perso(P_KONKALITE12, 3055, 5844, 0x7148, 1, "P82GAR03", 3, 0);
                CB->combat(P_KONKALITE12, 0);
                ST(V_ENGAGED_PERSONA) = P_KONKALITE12;
            } else {
                CB->pos_perso(P_KONKALITE12, 3500, 6190, 0xAFC8, 1, "P82ARR01", 3, 0);
            }
        }
    }
    /* 0x7CEE */
    if (ST(ST_VIE(P_KONKALITE12)) == VIE_DEAD) {
        CB->pos_perso(P_KONKALITE12, 0, 0, 0, 2, "P82cad", 3, 0);
    }
    /* 0x7D1C */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x7D40 */
    if (ST(V_GUARD_FIGHT_DONE) != 0) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    ST(ST_START_POINT(36)) = 0;
}

/* -------------------------------------------------------------------------
 * trigger 2 — sub_10009C0A (thunk sub_100010BE): Lodar
 * ---------------------------------------------------------------------- */
void l36_trig02(void) {
    static c8 txt[256];     /* dword_10020970 */
    i32 joined;

    /* 0x9C1F */
    if (ST(ST_VIE(P_LODAR)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0 && ST(ST_ACTION_KEY) == 1) {
        /* 0x9C4F */
        if (ST(0xC14) == 1 && ST(ST_VIE(P_KONKALITE12)) == 1 && ST(V_ENGAGED_PERSONA) == 0) {
            ST(0xC14) = 2;
            ST(0xF50) = 8;
            CB->pos_perso(P_KONKALITE12, 3600, 6224, 0, 4, "P82ARR01", 3, 0);
        }
        /* 0x9CC2 */
        if (ST(0xC14) == 0) {
            ST(0xC14) = 3;
            CB->divers_empile(DV_PERSONA_ITEMS, P_LODAR, OB_HELMET);
        }
        CB->fouille(P_LODAR);
        return;
    }
    /* 0x9D06 */
    if (CB->direction_perso(P_LODAR)) {
        joined = CB->on_joindre(P_LODAR);
        /* 0x9D5D */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_VIE(P_LODAR)) < 2 && ST(V_ENGAGED_PERSONA) == P_LODAR && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0x9DC5 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x9DD7 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD20", P_ARKHAN2);
                }
                /* 0x9DF9 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD21", P_ARKHAN3);
                }
            }
            /* 0x9E1B */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x9E2D */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD61", P_ARKHAN2);
                }
                /* 0x9E4F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD63", P_ARKHAN3);
                }
            }
            CB->dialogue("L36D84", P_LODAR);
            CB->combat(P_LODAR, 0);
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x9EC1 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_PLAYER_STRIKE) <= 0) {
            snprintf(txt, sizeof(txt), "combat ACTION LODAR");
            CB->print_msg(txt, 0, 240);
            /* 0x9F24 */
            if (ST(ST_VIE(P_LODAR)) < 2 && joined != 0 && ST(V_ENGAGED_PERSONA) == 0) {
                CB->stop_joindre(P_LODAR);
            }
            /* 0x9F5A */
            if (ST(ST_VIE(P_LODAR)) < 2 && joined != 0 && ST(V_ENGAGED_PERSONA) != 0) {
                CB->select_pile(0, 2);
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                /* 0x9FA9 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD73", P_ARKHAN2);
                }
                /* 0x9FCB */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD74", P_ARKHAN3);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
            joined = CB->on_joindre(P_LODAR);
            /* 0xA026 */
            if (ST(ST_VIE(P_LODAR)) < 2 && ST(V_ENGAGED_PERSONA) != P_LODAR && joined == 0) {
                CB->select_pile(0, 2);
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                /* 0xA075 */
                if (ST(0xF50) > 1) {
                    /* 0xA084 */
                    if (ST(V_ENGAGED_PERSONA) == 0) {
                        CB->joindre(P_LODAR, 0, 0, 0, ST(ST_HERO));
                    }
                    CB->animation("SP86ARR01_10");
                }
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_LODAR);
                CB->animation("Sar_10");
                /* 0xA0E2 */
                if (ST(ST_VIE(P_KONKALITE12)) == 1) {
                    CB->divers(DV_CINEMATIC, 0, 0);
                    /* 0xA103 */
                    if (ST(ST_VIE(P_LODAR)) == 1) {
                        CB->animation("SP86GAR02_10");
                    }
                    CB->animation("SP82GAR03_10");
                }
                /* 0xA128 */
                if (ST(0x54) == 0) {
                    CB->animation("SP86GAR27");
                    ST(0x54) = 1;
                    CB->animation("P86_65535");
                }
                /* 0xA15C */
                if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) || (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0xA1DA */
                    if (ST(0xF50) == 1) {
                        CB->bloque(BLOQUE_ALL);
                        ST(ST_SCRIPT_BUSY) = 1;
                        /* 0xA200 */
                        if (ST(0x78) == 0) {
                            ST(0x78) = 1;
                            /* 0xA21E */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xA230 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D87", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA258 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D88", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                            }
                            /* 0xA280 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xA292 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D89", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA2BA */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D90", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                            }
                            CB->dialogue("L36D91A", P_LODAR);
                            CB->dialogue("L36D92A", P_KONKALITE12);
                            CB->dialogue("L36D92B", P_KONKALITE12);
                        } else {
                            ST(0x78) = 0;
                            /* 0xA323 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L36D78", P_ARKHAN2, "SG00DIA04", NULL);
                            }
                            /* 0xA34B */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L36D79", P_ARKHAN3, "SG00DIA04", NULL);
                            }
                            CB->dialogue("L36D99", P_LODAR);
                        }
                        /* 0xA380 */
                        if (ST(V_ENGAGED_PERSONA) == 0) {
                            CB->divers_empile(DV_CINEMATIC, 1, 2);
                            ST(0xF50) = 1;
                        }
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                    /* 0xA3E9 */
                    if (ST(0xF50) == 5) {
                        /* 0xA3FB */
                        if (ST(0x568) == 0) {
                            CB->bloque(BLOQUE_ALL);
                            ST(ST_SCRIPT_BUSY) = 1;
                            ST(0xC14) = 2;
                            ST(0x568) = 1;
                            ST(V_WEAPON_WARNED) = 1;
                            /* 0xA451 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xA463 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D93", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA48B */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D94", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                            }
                            /* 0xA4B3 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xA4C5 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D55", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA4ED */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D56", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                            }
                            lieu_action_register("L36D03", P_LODAR, "SP86DIA54", "SP86ARR01_10");
                            /* 0xA52E */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                CB->animation("SP02PRE73");
                            }
                            /* 0xA54B */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                CB->animation("SP03PRE73");
                            }
                            CB->animation("P86_65535");
                            CB->link_objet(P_LODAR, "OB84", 0x1B, 1);
                            ST(0xC14) = 3;
                            lieu_action_register("L36D57C", P_LODAR, "SP86DIA53", "SP86ARR01_10");
                            lieu_pre_scene(ST(ST_HERO), P_LODAR, OB_HELMET, 1, 2);
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xA600 */
                        if (ST(0x568) == 1) {
                            CB->bloque(BLOQUE_ALL);
                            ST(ST_SCRIPT_BUSY) = 1;
                            ST(0x568) = 2;
                            /* 0xA638 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xA64A */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D58", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA672 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D59", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                            }
                            /* 0xA69A */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xA6AC */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D60", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA6D4 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D61", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                            }
                            lieu_action_register("L36D62A", P_LODAR, "SP86DIA54", "SP86ARR01_10");
                            lieu_action_register("L36D62B", P_LODAR, "SP86DIA56", "SP86ARR01_10");
                            lieu_action_register("L36D62C", P_LODAR, "SP86DIA59", "SP86ARR01_10");
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xA77D */
                        if (ST(0x568) == 2) {
                            ST(0x568) = 3;
                            CB->bloque(BLOQUE_ALL);
                            ST(ST_SCRIPT_BUSY) = 1;
                            /* 0xA7B5 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xA7C7 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D63", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA7EF */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D64", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                lieu_action_register("L36D65A", P_LODAR, "SP86DIA56", "SP86ARR01_10");
                                lieu_action_register("L36D65B", P_LODAR, "SP86DIA59", "SP86ARR01_500");
                                lieu_action_register("L36D65C", P_LODAR, "P86DIA60", "SP86ARR01_10");
                                /* 0xA862 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D66", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA88A */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D67", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                lieu_action_register("L36D68A", P_LODAR, "SP86DIA50", "SP86ARR01_10");
                                lieu_action_register("L36D68B", P_LODAR, "SP86DIA53", "SP86ARR01_10");
                            }
                            /* 0xA8E4 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xA8F6 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D69", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xA91E */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D70", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                lieu_action_register("L36D71A", P_LODAR, "SP86DIA51", "SP86ARR01_10");
                                lieu_action_register("L36D71B", P_LODAR, "SP86DIA53", "SP86ARR01_10");
                                lieu_action_register("L36D71C", P_LODAR, "SP86DIA54", "SP86ARR01_10");
                            }
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xA9C7 */
                        if (ST(0x568) == 3) {
                            CB->bloque(BLOQUE_ALL);
                            ST(ST_SCRIPT_BUSY) = 1;
                            /* 0xA9F0 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xAA02 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D72", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xAA2A */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D73", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                /* 0xAA52 */
                                if (ST(0xF8) == 0) {
                                    ST(0xF8) = 1;
                                    lieu_action_register("L36D71B", P_LODAR, "SP86DIA53", "SP86ARR01_10");
                                    lieu_action_register("L36D71C", P_LODAR, "SP86DIA54", "SP86ARR01_10");
                                } else {
                                    lieu_action_register("L36D74A", P_LODAR, "SP86DIA56", "SP86ARR01_10");
                                    lieu_action_register("L36D74B", P_LODAR, "SP86DIA59", "SP86ARR01_10");
                                }
                            }
                            /* 0xAADC */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xAAEE */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D75", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xAB16 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D76", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                /* 0xAB3E */
                                if (ST(0xF8) == 0) {
                                    ST(0xF8) = 1;
                                    lieu_action_register("L36D77A", P_LODAR, "SP86DIA60", "SP86ARR01_10");
                                    lieu_action_register("L36D77B", P_LODAR, "SP86DIA51", "SP86ARR01_10");
                                } else {
                                    ST(0xF8) = 0;
                                    lieu_action_register("L36D71B", P_LODAR, "SP86DIA53", "SP86ARR01_10");
                                    lieu_action_register("L36D71C", P_LODAR, "SP86DIA54", "SP86ARR01_10");
                                }
                            }
                        }
                        CB->animation("Sar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
                /* 0xAC0D */
                if (ST(ST_PLAYER_WEAPON) == OB_HELMET) {
                    CB->bloque(BLOQUE_ALL);
                    ST(ST_SCRIPT_BUSY) = 1;
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0xAC4A */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L36D998", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xAC75 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L36D999", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L36D82A", P_LODAR, "SP86DIA56", "SP86ARR01_10");
                    lieu_action_register("L36D82B", P_LODAR, "SP86DIA59", "SP86ARR01_10");
                    lieu_action_register("L36D82C", P_LODAR, "SP86DIA60", "SP86ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xAD2C */
                if (ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_2 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_1 || ST(ST_PLAYER_WEAPON) == OB_CRYSTAL_3 || ST(ST_PLAYER_WEAPON) == OB_BOW_TO_KEY) {
                    CB->bloque(BLOQUE_ALL);
                    ST(ST_SCRIPT_BUSY) = 1;
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0xAD9F */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L36D124", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xADCA */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L36D125", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L36D83", P_LODAR, "SP86DIA51", "SP86ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("ar_10");
                    /* 0xAE24 */
                    if (ST(ST_VIE(P_KONKALITE12)) == 1) {
                        CB->divers_empile(DV_CINEMATIC, 1, 2);
                    }
                    /* 0xAE45 */
                    if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE12)) == VIE_DEAD) {
                        CB->joindre_perm(P_LODAR, ST(ST_HERO));
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                } else if (ST(ST_PLAYER_WEAPON) != 0) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    CB->bloque(BLOQUE_ALL);
                    ST(ST_SCRIPT_BUSY) = 1;
                    /* 0xAEEC */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xAF17 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    lieu_action_register("L36D81", P_LODAR, "SP86DIA53", "SP86ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("ar_10");
                    /* 0xAF71 */
                    if (ST(ST_VIE(P_KONKALITE12)) == 1) {
                        CB->divers_empile(DV_CINEMATIC, 1, 2);
                    }
                    /* 0xAF95 */
                    if (ST(ST_VIE(P_KONKALITE12)) != 1 && ST(V_ENGAGED_PERSONA) == 0) {
                        CB->joindre_perm(P_LODAR, ST(ST_HERO));
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
        snprintf(txt, sizeof(txt), "                    ");
        CB->print_msg(txt, 0, 240);
        /* 0xB084 */
        if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && (ST(V_ENGAGED_PERSONA) == 0 || ST(V_ENGAGED_PERSONA) == P_KONKALITE12) && ST(ST_VIE(P_LODAR)) == 1) {
            ST(V_PLAYER_STRIKE) = -1;
            CB->select_pile(0, 2);
            /* 0xB0EE */
            if (ST(ST_VIE(P_KONKALITE12)) == 1) {
                CB->divers_empile(DV_CINEMATIC, 0, 0);
                /* 0xB10C */
                if (ST(V_ENGAGED_PERSONA) == P_KONKALITE12) {
                    CB->combat(P_KONKALITE12, -1);
                }
                CB->animation("SP82GAR03_10");
            }
            CB->joindre_perm(P_LODAR, ST(ST_HERO));
            /* 0xB14F */
            if (ST(0x570) == 0) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->stop_joindre(-P_LODAR);
                ST(0x54) = 0;
                ST(0x570) = 1;
                CB->animation("SP86GAR02_10");
                lieu_action_register("L36D85", P_LODAR, "SP86DIA53", "SP86GAR02_10");
                CB->stop_joindre(P_LODAR);
                /* 0xB1CD */
                if (ST(ST_VIE(P_KONKALITE12)) == 1 && ST(V_ENGAGED_PERSONA) != P_KONKALITE12) {
                    CB->divers_empile(DV_CINEMATIC, 1, 2);
                }
                /* 0xB200 */
                if (ST(ST_VIE(P_KONKALITE12)) == 1 && ST(V_ENGAGED_PERSONA) == P_KONKALITE12) {
                    CB->combat(P_KONKALITE12, 0);
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xB264 */
            if (ST(0x570) >= 1) {
                ST(0xF54) = 3;
                ST(0x54) = 0;
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->stop_joindre(-P_LODAR);
                CB->animation("SP86GAR02_10");
                /* 0xB2BE */
                if (ST(0x570) == 1) {
                    CB->dialogue("L36D86", P_LODAR);
                    ST(0x570) = 2;
                }
                ST(V_ENGAGED_PERSONA) = P_LODAR;
                CB->combat(P_LODAR, 0);
                /* 0xB308 */
                if (ST(ST_VIE(P_KONKALITE12)) == 1) {
                    ST(V_EVENT_0) = 0;
                    CB->evenement(0, -1);
                    l36_sub_7400(P_KONKALITE12);
                    CB->modify_var(&ST(0xF4), 1);
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
        }
    }
    /* 0xB38E */
    if (CB->direction_perso(P_KONKALITE12) && !CB->direction_perso(P_LODAR) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
    }
}

/* -------------------------------------------------------------------------
 * trigger 3 — sub_1000B3D1 (thunk sub_100010F0): Konkalite
 * ---------------------------------------------------------------------- */
static void trig03(void) {
    /* 0xB3E4 */
    if (CB->direction_perso(P_KONKALITE12)) {
        /* 0xB3F6 */
        if (ST(ST_VIE(P_KONKALITE12)) == VIE_DEAD && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
            /* 0xB435 */
            if (ST(0xC14) == 1) {
                CB->divers(DV_PERSONA_ITEMS, P_KONKALITE12, OB_HELMET);
                ST(0xC14) = 3;
            }
            CB->fouille(P_KONKALITE12);
            return;
        }
        /* 0xB475 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_PLAYER_STRIKE) <= 0) {
            /* 0xB4A5 */
            if (ST(ST_VIE(P_KONKALITE12)) < 2 && ST(V_ENGAGED_PERSONA) == P_KONKALITE12 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
                CB->select_pile(0, 2);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->bloque(BLOQUE_ALL);
                /* 0xB508 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xB51A */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD20", P_ARKHAN2);
                    }
                    /* 0xB53C */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD21", P_ARKHAN3);
                    }
                }
                /* 0xB55E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0xB570 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD61", P_ARKHAN2);
                    }
                    /* 0xB592 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD63", P_ARKHAN3);
                    }
                }
                CB->dialogue("L36D200", P_KONKALITE12);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xB5FA */
            if (ST(ST_VIE(P_KONKALITE12)) < 2 && ST(V_ENGAGED_PERSONA) == P_LODAR) {
                CB->select_pile(0, 2);
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KONKALITE12);
                /* 0xB65C */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD73", P_ARKHAN2);
                }
                /* 0xB67E */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD74", P_ARKHAN3);
                }
                CB->dialogue("L36D105", P_KONKALITE12);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xB6E6 */
            if (ST(ST_VIE(P_KONKALITE12)) < 2 && ST(V_ENGAGED_PERSONA) == 0) {
                CB->select_pile(0, 2);
                CB->bloque(BLOQUE_ALL);
                CB->stop_joindre(-P_KONKALITE12);
                ST(ST_SCRIPT_BUSY) = 1;
                /* 0xB736 */
                if (ST(ST_VIE(P_LODAR)) == 1) {
                    CB->divers(DV_CINEMATIC, 0, 0);
                    CB->animation("SP86GAR02_10");
                    /* 0xB762 */
                    if (ST(ST_VIE(P_KONKALITE12)) < 2) {
                        CB->animation("SP82GAR03_10");
                    }
                }
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KONKALITE12);
                /* 0xB79C */
                if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) || (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0xB81A */
                    if (ST(0xF50) == 1) {
                        /* 0xB829 */
                        if (ST(0x7C) == 1) {
                            /* 0xB83B */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xB84D */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D103", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xB875 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D104", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                CB->dialogue("L36D105", P_KONKALITE12);
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->divers_empile(DV_CINEMATIC, 1, 2);
                                CB->bloque(BLOQUE_NONE);
                                CB->select_pile(0, 1);
                                return;
                            }
                            /* 0xB8EA */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xB8FC */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D106", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xB924 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D107", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                CB->dialogue("L36D108", P_KONKALITE12);
                                ST(V_ENGAGED_PERSONA) = P_KONKALITE12;
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->combat(P_KONKALITE12, 0);
                                ST(0xF54) = 2;
                                l36_sub_7400(P_LODAR);
                                CB->divers_empile(DV_CINEMATIC, 0, 0);
                                CB->bloque(BLOQUE_NONE);
                                CB->select_pile(0, 1);
                                return;
                            }
                        }
                        /* 0xB9C5 */
                        if (ST(0x7C) == 0) {
                            ST(0x7C) = 1;
                            /* 0xB9E3 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xB9F5 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D96", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xBA1D */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D97", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                CB->dialogue("L36D98", P_KONKALITE12);
                            }
                            /* 0xBA55 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xBA67 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D100", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xBA8F */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D101", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                CB->dialogue("L36D102", P_KONKALITE12);
                            }
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->divers_empile(DV_CINEMATIC, 1, 2);
                            CB->select_pile(0, 1);
                            return;
                        }
                    }
                    /* 0xBB04 */
                    if (ST(0xF50) == 2 || ST(0xF50) == 3) {
                        /* 0xBB25 */
                        if (ST(0x7C) == 0) {
                            ST(0x7C) = 1;
                            /* 0xBB43 */
                            if (ST(0xF50) == 2) {
                                /* 0xBB55 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D110", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xBB7D */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D111", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                lieu_action_register("L36D112", P_KONKALITE12, "SP82DIA53", "SP82ARR01_10");
                            }
                            /* 0xBBBE */
                            if (ST(0xF50) == 3) {
                                /* 0xBBD0 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L36D113", P_ARKHAN2, "SG00DIA04", NULL);
                                }
                                /* 0xBBF8 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L36D114", P_ARKHAN3, "SG00DIA04", NULL);
                                }
                                lieu_action_register("L36D115A", P_KONKALITE12, "SP82DIA53", "SP82ARR01_10");
                                lieu_action_register("L36D115B", P_KONKALITE12, "SP82DIA53", "SP82ARR01_10");
                            }
                            /* 0xBC4F */
                            if (ST(V_ENGAGED_PERSONA) == 0) {
                                CB->joindre_perm(P_KONKALITE12, ST(ST_HERO));
                            }
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xBC9F */
                        if (ST(0x7C) == 1) {
                            /* 0xBCB1 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L36D119", P_ARKHAN2, "SG00DIA04", NULL);
                            }
                            /* 0xBCD9 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L36D120", P_ARKHAN3, "SG00DIA04", NULL);
                            }
                            lieu_action_register("L36D121", P_KONKALITE12, "SP82DIA53", "SP82ARR01_10");
                            ST(V_ENGAGED_PERSONA) = P_KONKALITE12;
                            ST(0xF54) = 2;
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->combat(P_KONKALITE12, 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                    }
                } else {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0xBD86 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xBDB1 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0xBDDC */
                    if (ST(0xF50) == 1) {
                        CB->dialogue("L36D109", P_KONKALITE12);
                    }
                    /* 0xBDFE */
                    if (ST(0xF50) != 1) {
                        lieu_action_register("L36D109", P_KONKALITE12, "SP82DIA53", "SP82ARR01_10");
                    }
                    CB->animation("G00PRE82");
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    /* 0xBE50 */
                    if (ST(ST_VIE(P_LODAR)) == 1) {
                        CB->divers_empile(DV_CINEMATIC, 1, 2);
                    }
                    /* 0xBE74 */
                    if (ST(ST_VIE(P_LODAR)) != 1 && ST(V_ENGAGED_PERSONA) == 0) {
                        CB->joindre_perm(P_KONKALITE12, ST(ST_HERO));
                    }
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
            }
        }
        /* 0xBEC2 */
        if (ST(V_PLAYER_STRIKE) >= 1 && (ST(V_ENGAGED_PERSONA) == 0 || ST(V_ENGAGED_PERSONA) == P_LODAR) && ST(ST_VIE(P_KONKALITE12)) == 1) {
            CB->select_pile(0, 2);
            ST(V_PLAYER_STRIKE) = -1;
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            CB->animation("SP82ESA21");
            ST(0xF54) = 2;
            ST(0x70) = 1;
            CB->divers(DV_CINEMATIC, 0, 0);
            /* 0xBF66 */
            if (ST(V_ENGAGED_PERSONA) == 0) {
                CB->joindre_perm(P_KONKALITE12, ST(ST_HERO));
            }
            CB->animation("SP82GAR03_10");
            CB->stop_joindre(P_KONKALITE12);
            CB->animation("SP82GAR03_10");
            ST(V_ENGAGED_PERSONA) = P_KONKALITE12;
            CB->combat(P_KONKALITE12, 0);
            /* 0xBFC8 */
            if (ST(0xE0) == 0) {
                CB->dialogue("L36D200", P_KONKALITE12);
                ST(0xE0) = 1;
            }
            ST(V_EVENT_0) = 0;
            CB->evenement(0, -1);
            /* 0xC015 */
            if (ST(ST_VIE(P_LODAR)) != VIE_DEAD && ST(0x74) == 0) {
                CB->combat(P_LODAR, -1);
                l36_sub_7400(P_LODAR);
                CB->modify_var(&ST(0xF0), 1);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0xC09E */
    if (CB->direction_perso(P_LODAR) && !CB->direction_perso(P_KONKALITE12) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
        l36_trig02();
    }
}

/* -------------------------------------------------------------------------
 * trigger 4 — sub_1000C0E1 (thunk sub_10001082): Guardian 20
 * ---------------------------------------------------------------------- */
void l36_trig04(void) {
    /* 0xC0F0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_GUARDIAN20)) {
        /* 0xC124 */
        if (ST(ST_VIE(P_GUARDIAN20)) < 2 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_HERO) == P_ARKHAN2) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->stop_joindre(-P_GUARDIAN20);
            CB->joindre(-ST(ST_HERO), 0, 0, 0, P_GUARDIAN20);
            /* 0xC1A3 */
            if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) || (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                /* 0xC21E */
                if (ST(0x7C) == 2) {
                    /* 0xC230 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        ST(V_DLG_RANDOM) = CB->random(2);
                        /* 0xC259 */
                        if (ST(V_DLG_RANDOM) == 0) {
                            lieu_action_register("L36D149", P_ARKHAN2, "SG00DIA04", NULL);
                        }
                        /* 0xC281 */
                        if (ST(V_DLG_RANDOM) == 1) {
                            lieu_action_register("L36D150", P_ARKHAN2, "SG00DIA04", NULL);
                        }
                        /* 0xC2A9 */
                        if (ST(0x80) == 1) {
                            ST(V_DLG_RANDOM) = CB->random(2);
                            /* 0xC2D2 */
                            if (ST(V_DLG_RANDOM) == 0) {
                                CB->dialogue("L36D151A", P_GUARDIAN20);
                                CB->dialogue("L36D151B", P_GUARDIAN20);
                            }
                            /* 0xC304 */
                            if (ST(V_DLG_RANDOM) == 1) {
                                CB->dialogue("L36D251A", P_GUARDIAN20);
                                CB->dialogue("L36D251B", P_GUARDIAN20);
                            }
                        }
                        /* 0xC336 */
                        if (ST(0x80) == 0) {
                            ST(0x80) = 1;
                            ST(V_DLG_RANDOM) = CB->random(2);
                            /* 0xC36E */
                            if (ST(V_DLG_RANDOM) == 0) {
                                lieu_action_register("L36D151A", P_GUARDIAN20, "SP60GAR26", "SP60GAR03_10");
                                CB->dialogue("L36D151B", P_GUARDIAN20);
                            }
                            /* 0xC3A9 */
                            if (ST(V_DLG_RANDOM) == 1) {
                                lieu_action_register("L36D251A", P_GUARDIAN20, "SP60GAR26", "SP60GAR03_10");
                                CB->dialogue("L36D251B", P_GUARDIAN20);
                            }
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        /* 0xC3FA */
                        if (ST(V_ENGAGED_PERSONA) == 0) {
                            CB->joindre_perm(P_GUARDIAN20, ST(ST_HERO));
                        }
                        CB->select_pile(0, 1);
                    }
                    /* 0xC42F */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        ST(0x7C) = 1;
                    }
                }
                /* 0xC44A */
                if (ST(0x7C) == 1) {
                    /* 0xC45C */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        ST(0x7C) = 2;
                        /* 0xC47A */
                        if (ST(0x80) == 1) {
                            ST(0x80) = 0;
                            CB->animation("SP60GAR27");
                            CB->animation("P60ARR03_10");
                        }
                        lieu_action_register("L36D135A", P_ARKHAN2, "SG00DIA04", NULL);
                        lieu_action_register("L36D135B", P_ARKHAN2, "SG00DIA04", NULL);
                        lieu_action_register("L36D137A", P_GUARDIAN20, "SP60DIA54", "SP60ARR03_10");
                        lieu_action_register("L36D137B", P_GUARDIAN20, "SP60DIA54", "SP60ARR03_10");
                        lieu_action_register("L36D137C", P_GUARDIAN20, "SP60DIA54", "SP60ARR03_10");
                        lieu_action_register("L36D137D", P_GUARDIAN20, "SP60DIA54", "SP60ARR03_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        /* 0xC557 */
                        if (ST(V_ENGAGED_PERSONA) == 0) {
                            CB->joindre_perm(P_GUARDIAN20, ST(ST_HERO));
                        }
                        CB->select_pile(0, 1);
                    }
                    /* 0xC58C */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        ST(0xF54) = 1;
                        /* 0xC5AD */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L36D138A", P_ARKHAN2, "SG00DIA04", NULL);
                            lieu_action_register("L36D138B", P_ARKHAN2, "SG00DIA04", NULL);
                        }
                        /* 0xC5EB */
                        if (ST(0x80) == 0) {
                            lieu_action_register("L36D140", P_GUARDIAN20, "SP60GAR26", "SP60GAR03_10");
                        }
                        /* 0xC616 */
                        if (ST(0x80) == 1) {
                            CB->dialogue("L36D140", P_GUARDIAN20);
                        }
                        ST(V_ENGAGED_PERSONA) = P_GUARDIAN20;
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->combat(P_GUARDIAN20, 0);
                        CB->select_pile(0, 1);
                        ST(0x80) = 1;
                        return;
                    }
                }
                /* 0xC688 */
                if (ST(0x7C) == 0) {
                    ST(0x7C) = 1;
                    /* 0xC6A6 */
                    if (ST(ST_SPECIAL_MODE) == 1 && ST(V_ENGAGED_PERSONA) == 0) {
                        lieu_action_register("L36D123A", P_ARKHAN2, "SG00DIA04", NULL);
                        lieu_action_register("L36D123B", P_ARKHAN2, "SG00DIA04", NULL);
                        CB->dialogue("L36D136", P_GUARDIAN20);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        /* 0xC719 */
                        if (ST(V_ENGAGED_PERSONA) == 0) {
                            CB->joindre_perm(P_GUARDIAN20, ST(ST_HERO));
                        }
                        CB->select_pile(0, 1);
                        return;
                    }
                    /* 0xC753 */
                    if (ST(ST_SPECIAL_MODE) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
                        ST(0x80) = 1;
                        lieu_action_register("L36D126", P_ARKHAN2, "SG00DIA04", NULL);
                        CB->dialogue("L36D140", P_GUARDIAN20);
                        ST(V_ENGAGED_PERSONA) = P_GUARDIAN20;
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->combat(P_GUARDIAN20, 0);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
            } else {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                /* 0xC806 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                }
                /* 0xC831 */
                if (ST(0x80) == 0) {
                    lieu_action_register("L36D141A", P_GUARDIAN20, "SP60DIA54", "SP60ARR03_10");
                    lieu_action_register("L36D141B", P_GUARDIAN20, "SP60DIA54", "SP60ARR03_10");
                }
                /* 0xC875 */
                if (ST(0x80) == 1) {
                    CB->dialogue("L36D141A", P_GUARDIAN20);
                    CB->dialogue("L36D141B", P_GUARDIAN20);
                }
                CB->animation("G00PRE82");
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                /* 0xC8D3 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_GUARDIAN20, ST(ST_HERO));
                }
                CB->select_pile(0, 1);
                return;
            }
        }
        /* 0xC90D */
        if (ST(ST_VIE(P_GUARDIAN20)) < 2 && ST(V_ENGAGED_PERSONA) == P_GUARDIAN20 && ST(ST_SCRIPT_BUSY) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xC949 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xC95B */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD20", P_ARKHAN2);
                }
                /* 0xC97D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD21", P_ARKHAN3);
                }
            }
            /* 0xC99F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xC9B1 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("LCOD61", P_ARKHAN2);
                }
                /* 0xC9D3 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("LCOD63", P_ARKHAN3);
                }
            }
            CB->dialogue("L36D142", P_GUARDIAN20);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        }
        /* 0xCA16 */
        if (ST(ST_VIE(P_GUARDIAN20)) == VIE_DEAD) {
            CB->fouille(P_GUARDIAN20);
        }
    }
    /* 0xCA30 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARDIAN20)) == 1) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_GUARDIAN20);
        CB->animation("SP60ESA23");
        ST(0xF54) = 1;
        ST(V_PLAYER_STRIKE) = -1;
        ST(V_ENGAGED_PERSONA) = P_GUARDIAN20;
        /* 0xCAB8 */
        if (ST(0x80) == 0) {
            lieu_action_register("L36D143", P_GUARDIAN20, "SP60GAR26", "SP60GAR03_10");
        }
        /* 0xCAE3 */
        if (ST(0x80) == 1) {
            CB->dialogue("L36D143", P_GUARDIAN20);
            CB->animation("SP60GAR03_10");
        }
        CB->bloque(BLOQUE_NONE);
        CB->combat(P_GUARDIAN20, 0);
        ST(0x80) = 1;
        CB->select_pile(0, 1);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 5 — sub_1000CB3F (thunk sub_100010AA): start-area exit to room 49
 * ---------------------------------------------------------------------- */
static void trig05(void) {
    CB->retire_zone(5);
    /* 0xCB86 */
    if (ST(0xF50) == 2 && ST(0x68) == 1 && ST(0xC14) == 0) {
        ST(0xC14) = 1;
        ST(0xF50) = 3;
    } else if (ST(0xF50) == 3 && ST(0xF54) == 0 && ST(0x68) == 1) {
        ST(0xF50) = 8;
    } else if (ST(0xF50) == 1 && ST(0x68) == 1 && ST(0xC14) == 0) {
        ST(0xC14) = 1;
        ST(0xF50) = 2;
    } else if (ST(0xF50) == 2) {
        ST(0xC14) = 2;
        ST(0xF50) = 8;
    }
    /* 0xCCAB */
    if (ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(P_KONKALITE12)) == 1 && ST(ST_VIE(P_LODAR)) == 1) {
        ST(0xC14) = 1;
    }
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 36;
    CB->change_piece(49, -1);
    return;
}

/* -------------------------------------------------------------------------
 * trigger 6 — sub_1000CD1C (thunk sub_100010E6)
 * ---------------------------------------------------------------------- */
static void trig06(void) {
    /* 0xCD2B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xCD91 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xCDA3 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D32", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xCDCE */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L36D33", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
        }
        /* 0xCDF9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xCE0B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D34", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
            /* 0xCE36 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xCE50 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT85");
                }
                /* 0xCE75 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT86");
                }
                CB->animation("P03_65535");
                lieu_action_register("L36D35", P_ARKHAN3, "G00DIA04", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 7 — sub_1000CECD (thunk sub_1000101E)
 * ---------------------------------------------------------------------- */
static void trig07(void) {
    /* 0xCEDC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(8)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xCF45 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L36D36", P_ARKHAN2, "SG00DIA04", "Sar_10");
        }
        /* 0xCF70 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L36D37", P_ARKHAN3, "SG00DIA04", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 9 — sub_1000CFBD (thunk sub_10001078)
 * ---------------------------------------------------------------------- */
static void trig09(void) {
    /* 0xCFCC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && (CB->direction_spec(0x7530) || CB->direction_spec(0xC350))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD048 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xD05A */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D38", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xD085 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L36D39", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
        }
        /* 0xD0B0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xD0C2 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D40", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
            /* 0xD0ED */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L36D41", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 10 — sub_1000D12F (thunk sub_1000109B)
 * ---------------------------------------------------------------------- */
static void trig10(void) {
    /* 0xD13E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD1A4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xD1B6 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D38", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xD1E1 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L36D39", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
        }
        /* 0xD20C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xD21E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D40", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
            /* 0xD249 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L36D41", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 11 — sub_1000D28B (thunk sub_10001028)
 * ---------------------------------------------------------------------- */
static void trig11(void) {
    /* 0xD29A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD2EE */
        if (CB->direction_spec(0) || CB->direction_spec(0x3A98)) {
            CB->joindre(-ST(ST_HERO), 0, 0, 0, -13);
        } else {
            CB->joindre(-ST(ST_HERO), 0, 0, 0, -14);
        }
        /* 0xD352 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xD364 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D46", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xD38F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xD3A9 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT85");
                }
                /* 0xD3CE */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT86");
                }
            }
        }
        /* 0xD3EB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xD3FD */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D48", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
            /* 0xD428 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xD442 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT85");
                }
                /* 0xD467 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT86");
                }
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 14 — sub_1000D4B1 (thunk sub_100010D7)
 * ---------------------------------------------------------------------- */
static void trig14(void) {
    /* 0xD4C0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xA410)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD529 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xD53B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L36D49", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xD566 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD70", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
        }
        /* 0xD591 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xD5A3 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD71", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
            /* 0xD5CE */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD71", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 15 — sub_1000D610 (thunk sub_100010EB)
 * ---------------------------------------------------------------------- */
static void trig15(void) {
    /* 0xD61F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_LOCKED_CAMERA) == 4 && CB->direction_spec(0x88B8) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD6DA */
        if (ST(V_GUARD_FIGHT_DONE) < 2) {
            /* 0xD6EC */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L36D24", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xD717 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L36D25", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        /* 0xD742 */
        if (ST(V_GUARD_FIGHT_DONE) >= 2) {
            /* 0xD754 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L36D26", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xD77F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L36D27", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xD7C5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_LOCKED_CAMERA) == 4 && CB->direction_spec(0) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD87D */
        if (ST(V_GUARD_FIGHT_DONE) < 2) {
            /* 0xD88F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L36D28", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xD8BA */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L36D29", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        /* 0xD8E5 */
        if (ST(V_GUARD_FIGHT_DONE) >= 2) {
            /* 0xD8F7 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L36D30", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xD922 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L36D27", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 16 — sub_1000D964 (thunk sub_10001096)
 * ---------------------------------------------------------------------- */
static void trig16(void) {
    /* 0xD973 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_LOCKED_CAMERA) == 2 && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDA1B */
        if (ST(V_GUARD_FIGHT_DONE) < 2) {
            /* 0xDA2D */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xDA3F */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L36D09", P_ARKHAN2, "SG00DIA04", "Sar_10");
                }
                /* 0xDA6A */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L36D10", P_ARKHAN2, "SG00DIA04", "Sar_10");
                }
            }
            /* 0xDA95 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xDAA7 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L36D11", P_ARKHAN3, "SG00DIA04", "Sar_10");
                }
                /* 0xDAD2 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L36D12", P_ARKHAN3, "SG00DIA04", "Sar_10");
                }
            }
        }
        /* 0xDAFD */
        if (ST(V_GUARD_FIGHT_DONE) >= 2) {
            /* 0xDB0F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xDB21 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L36D13", P_ARKHAN2, "SG00DIA04", "Sar_10");
                }
                /* 0xDB4C */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L36D14", P_ARKHAN2, "SG00DIA04", "Sar_10");
                }
            }
            /* 0xDB77 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xDB89 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L36D16A", P_ARKHAN3, "SG00DIA04", "Sar_10");
                }
                lieu_action_register("L36D16B", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xDBE8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_LOCKED_CAMERA) == 2 && CB->direction_spec(0) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDCA0 */
        if (ST(V_GUARD_FIGHT_DONE) < 2) {
            /* 0xDCB2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L36D17", P_ARKHAN2, "SG00DIA04", "Sar_10");
                lieu_action_register("L36D18", P_ARKHAN2, "SG00DIA04", "Sar_10");
            }
            /* 0xDCF6 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L36D19", P_ARKHAN3, "SG00DIA04", "Sar_10");
            }
        }
        /* 0xDD21 */
        if (ST(V_GUARD_FIGHT_DONE) >= 2) {
            /* 0xDD33 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xDD45 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L36D20", P_ARKHAN2, "SG00DIA04", "Sar_10");
                }
                /* 0xDD70 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L36D21", P_ARKHAN2, "SG00DIA04", "Sar_10");
                }
            }
            /* 0xDD9B */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xDDAD */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L36D22", P_ARKHAN3, "SG00DIA04", "Sar_10");
                }
                /* 0xDDD8 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L36D23", P_ARKHAN3, "SG00DIA04", "Sar_10");
                }
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * trigger 17 — sub_1000DE1A (thunk sub_100010B4): save point
 * ---------------------------------------------------------------------- */
static void trig17(void) {
    /* 0xDE29 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xEA60) && ST(ST_COMBAT_STANCE) == 0) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0xDE84 */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xDEAF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xDEDA */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->select_pile(0, 1);
    }
}

/* -------------------------------------------------------------------------
 * trigger 41 — sub_10007D6C (thunk sub_10001000)
 * ---------------------------------------------------------------------- */
static void trig41(void) {
    /* 0x7D7E */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x7DA2 */
    if (ST(V_GUARD_FIGHT_DONE) != 0) {
        CB->divers(DV_MUSIC, 2, 1);
    }
    /* 0x7DC6 */
    if (ST(0xF50) == 5 && ST(V_ENGAGED_PERSONA) == 0) {
        CB->joindre_perm(P_LODAR, ST(ST_HERO));
    }
    /* 0x7DFD */
    if (ST(0xF54) == 1 && ST(ST_VIE(P_GUARDIAN20)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) < 2) {
        CB->combat(P_GUARDIAN20, 0);
        ST(0x80) = 1;
        ST(V_ENGAGED_PERSONA) = P_GUARDIAN20;
    }
    ST(V_PLAYER_STRIKE) = -1;
    /* 0x7E67 */
    if (ST(ST_VIE(P_LODAR)) == 1) {
        CB->stop_joindre(P_LODAR);
    }
}
