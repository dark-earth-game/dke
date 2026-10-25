/*
 * lieu11_trig.c — Native hand-port of the LIEU11.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu11.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdio.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu11_int.h"

static void trig01(void); static void trig02(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig12(void);
static void trig13(void); static void trig41(void);
static void trig_noop(void);   /* the 34 stub slots */

/* dword_1001AA10[42] — filled at INIT with the 5-byte jmp thunks; static here.
 *   idx handler        idx handler        idx handler
 *    0  10007463       14  1000A254 stub  28  1000A2EE stub
 *    1  10008345       15  1000A25F stub  29  1000A2F9 stub
 *    2  10008491       16  1000A26A stub  30  1000A304 stub
 *    3  1000883F stub  17  1000A275 stub  31  1000A30F stub
 *    4  1000884A stub  18  1000A280 stub  32  1000A31A stub
 *    5  10008855 stub  19  1000A28B stub  33  1000A325 stub
 *    6  10008860 stub  20  1000A296 stub  34  1000A330 stub
 *    7  1000886B       21  1000A2A1 stub  35  1000A33B stub
 *    8  10008AE9       22  1000A2AC stub  36  1000A346 stub
 *    9  10008CA5       23  1000A2B7 stub  37  1000A351 stub
 *   10  10008E61 stub  24  1000A2C2 stub  38  1000A35C stub
 *   11  10008E6C stub  25  1000A2CD stub  39  1000A367 stub
 *   12  10008E77       26  1000A2D8 stub  40  1000A372 stub
 *   13  1000A1FF       27  1000A2E3 stub  41  10007976
 */
static void (*const g_trigger[42])(void) = {
    l11_trig00, trig01, trig02, trig_noop, trig_noop, trig_noop, trig_noop, trig07,
    trig08, trig09, trig_noop, trig_noop, trig12, trig13, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001AA10]). */
void l11_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* sub_10007400 (thunk 0x1000102D) — fastcall(ecx=persona, edx=target, 3 stack
 * args, ret 0xC). The arguments are saved to locals and never used; the body
 * only preloads the animations of the guard scene. */
void l11_preload_anims(i32 persona, i32 target, i32 item, i32 a, i32 b) {
    (void)persona; (void)target; (void)item; (void)a; (void)b;
    CB->animation("SG02PRE80");
    CB->animation("G02PRE81");
    CB->animation("SP32PRE60");
    CB->animation("P32_65535");
    CB->animation("SG02PRE82");
    CB->animation("P02_65535");
    CB->animation("ar_10");
}

/* trigger 00 — sub_10007463 */
void l11_trig00(void) {
    /* 0x7475 */
    if (ST(ST_START_POINT(11)) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 10;
        ST(0xAF8) = 0;
        ST(V_GUARD_FIGHT_DONE) = 0;
    }
    /* 0x74C0 */
    if (ST(ST_START_POINT(11)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 10;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    /* 0x750B */
    if (ST(ST_START_POINT(11)) == 3) {
        ST(ST_HERO) = P_ARKHAN3;
        ST(ST_LEVEL) = 10;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    ST(ST_START_POINT(11)) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    CB->bruitage("L11AMB1", -1, -1);
    CB->ajoute_zone(1, 5660, -1340, 90, 500, 1);
    CB->ajoute_zone(2, 7596, -1250, 115, 74, 0);
    CB->ajoute_zone(8, 6259, -1697, 245, 230, 0);
    CB->ajoute_zone(9, 6233, -991, 245, 230, 0);
    CB->ajoute_zone(10, 6268, -1707, 50, 50, 0);
    CB->ajoute_zone(11, 6269, -992, 50, 50, 0);
    CB->ajoute_zone(13, 6585, -1327, 95, 190, 1);
    /* 0x7697 */
    if (ST(ST_LEVEL) == 10) {
        CB->pos_perso(ST(ST_HERO), 5750, -1340, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
    }
    /* 0x76E5 */
    if (ST(ST_LEVEL) == 12) {
        CB->pos_perso(ST(ST_HERO), 7535, -1259, 0x9AA8, 1, "ar", 6, 0);
    }
    /* 0x7722 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0) {
        /* 0x7758 */
        if (ST(0xB98) == 0) {
            ST(0xB98) = 2;
        } else if (ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
            ST(0xB98) = 0;
        }
    }
    /* 0x77B1 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }
    /* 0x7817 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->vire_col(7, 1);
        CB->ajoute_zone(7, 6333, -1300, 105, 140, 0);
        /* 0x7857 */
        if (ST(0xAF4) == 1) {
            CB->pos_perso(P_LORY, 4288, -2702, 0xBDD0, 1, "P15ARR01", 0x1A, 0);
            CB->pos_perso(P_RYLSADHAR20, 4454, -2713, 0x4266, 1, "P20ARR01", 0x1B, 0);
        }
    }
    /* 0x78B3 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->vire_col(7, 0);
        /* 0x78D2 */
        if (ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
            CB->pos_perso(P_GUARD, 6373, -1329, 0x3C62, 1, "P32ARR04", 0xC, 601);
            CB->divers_empile(DV_LOOK_AT_PLAYER, P_GUARD, 1);
        }
    }
    /* 0x791E */
    if (ST(ST_VIE(P_GUARD)) == VIE_DEAD) {
        CB->pos_perso(P_GUARD, 0, 0, 0, 2, "P32cad", 0xC, 0);
    }
    /* 0x794C */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->load_anim("LEVIER", 0);
    }
    CB->divers_empile(DV_SCREEN, 3, 0);
}

/* trigger 01 — sub_10008345 */
static void trig01(void) {
    ST(ST_LEVEL) = 11;
    /* 0x8363 */
    if (ST(0x4E4) == 1 && ST(ST_SCRIPT_BUSY) == 1 && ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
        CB->dialogue("L11D45", P_GUARD);
    }
    /* 0x83A6 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->change_piece(10, -1);
    }
    /* 0x83C8 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 0) {
        CB->change_piece(10, 1);
    }
    /* 0x83FC */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 1) {
        CB->change_piece(10, 2);
    }
    /* 0x8430 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 2) {
        CB->change_piece(10, 3);
    }
    /* 0x8464 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 3) {
        CB->change_piece(51, -1);
    }
}

/* trigger 02 — sub_10008491 */
static void trig02(void) {
    /* 0x84A0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x6978)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x84FA */
        if (ST(ST_VIE(P_GUARD)) == VIE_DEAD) {
            /* 0x8514 */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x8526 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x69BD, 1, "ar", 1, 0);
                    CB->animation("Sar_10");
                    CB->animation("SP02ATT77");
                }
                /* 0x85A3 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x69BD, 1, "ar", 1, 0);
                    CB->animation("Sar_10");
                    CB->animation("SP03ATT77");
                }
            }
            /* 0x8628 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x863A */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x69BD, 1, "ar", 1, 0);
                    CB->animation("Sar_10");
                    CB->animation("SP02ATT76");
                }
                /* 0x86B7 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x69BD, 1, "ar", 1, 0);
                    CB->animation("Sar_10");
                    CB->animation("SP03ATT76");
                }
            }
            CB->evenement(1, 500);
            return;
        }
        /* 0x8749 */
        if (ST(ST_VIE(P_GUARD)) != VIE_DEAD && ST(0x78) == 0) {
            /* 0x876A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                ST(0x78) = 1;
                lieu_action_register("L11D06", P_ARKHAN2, "SG02ATT01", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(0x78), 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x87D8 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                ST(0x78) = 1;
                lieu_action_register("L11D07", P_ARKHAN3, "SG03ATT01", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(0x78), 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 07 — sub_1000886B */
static void trig07(void) {
    /* 0x887A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xC924)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x88D4 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x88E6 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG01OUV13");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD12", P_ARKHAN1, "SG01DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8937 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG01OUV53");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD15", P_ARKHAN1, "SG01DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8988 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x899A */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02OUV13");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD13", P_ARKHAN2, "SG02DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x89EB */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02OUV53");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD16", P_ARKHAN2, "SG02DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8A3C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8A4E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03OUV13");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD14", P_ARKHAN3, "SG03DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8A9F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03OUV53");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD17", P_ARKHAN3, "SG03DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 08 — sub_10008AE9 */
static void trig08(void) {
    /* 0x8AF8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_GUARD_FIGHT_DONE) == 2 && CB->direction_zone(10)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x8B64 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x8B76 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT28");
            }
            /* 0x8BA5 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0x8BC2 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x8C08 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x8C1A */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT29");
            }
            /* 0x8C49 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0x8C66 */
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

/* trigger 09 — sub_10008CA5 */
static void trig09(void) {
    /* 0x8CB4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_GUARD_FIGHT_DONE) == 2 && CB->direction_zone(11)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x8D20 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x8D32 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT28");
            }
            /* 0x8D61 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0x8D7E */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x8DC4 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x8DD6 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT29");
            }
            /* 0x8E05 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0x8E22 */
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

/* trigger 12 — sub_10008E77 */
static void trig12(void) {
    /* 0x8E86 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARD)) != VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x8EEE */
        if (ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
            /* 0x8F00 */
            if (ST(ST_PLAYER_WEAPON) != OB_MEAT && ST(ST_PLAYER_WEAPON) != OB_FULL_GOURD && ST(ST_PLAYER_WEAPON) != OB_VEGETABLE && ST(ST_PLAYER_WEAPON) != OB_ROAST_BIRD && ST(ST_PLAYER_WEAPON) != OB_ROOT && ST(ST_PLAYER_WEAPON) != OB_FISH) {
                /* 0x8F6C */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->joindre(ST(ST_HERO), 0, 0, 0, P_GUARD);
                }
                CB->joindre(P_GUARD, 0, 0, 0, ST(ST_HERO));
                /* 0x8FB4 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8FC6 */
                    if (ST(0xC9C) == 0) {
                        lieu_action_register("L11D08", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L11D09", P_GUARD, "SP32DIA59", "SP32ARR04_10");
                        ST(0xC9C) = 1;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x903F */
                    if (ST(0xC9C) == 1) {
                        lieu_action_register("L11D10A", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D10B", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D11A", P_GUARD, "SP32DIA60", "SP32ARR04_10");
                        lieu_action_register("L11D11B", P_GUARD, "SP32DIA60", "SP32ARR04_10");
                        ST(0xC9C) = 2;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x90E7 */
                    if (ST(0xC9C) == 2) {
                        lieu_action_register("L11D12", P_ARKHAN2, "SG02DIA04", NULL);
                        lieu_action_register("L11D13A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        lieu_action_register("L11D13B", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        CB->animation("SP32GAR05");
                        ST(0xC9C) = 3;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9184 */
                    if (ST(0xC9C) == 3) {
                        lieu_action_register("L11D14", P_ARKHAN2, "SG02DIA04", NULL);
                        CB->dialogue("L11D14A", P_GUARD);
                        CB->dialogue("L11D14B", P_GUARD);
                        ST(0xC9C) = 4;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9204 */
                    if (ST(0xC9C) == 4) {
                        lieu_action_register("L11D15A", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D15B", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D16A", P_GUARD, "SP32DIA56", "SP32ARR04_10");
                        lieu_action_register("L11D16B", P_GUARD, "SP32DIA56", "SP32ARR04_10");
                        ST(0xC9C) = 5;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x92AC */
                    if (ST(0xC9C) == 5 && ST(0xEA4) == 0) {
                        lieu_action_register("L11D17A", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D17B", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D17C", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D18A", P_GUARD, "SP32DIA62", "SP32ARR04_10");
                        lieu_action_register("L11D18B", P_GUARD, "SP32DIA62", "SP32ARR04_10");
                        lieu_action_register("L11D19", P_ARKHAN2, "SG02DIA05", NULL);
                        ST(0xEA4) = 1;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9392 */
                    if (ST(0xC9C) == 5 && ST(0xEA4) == 1 && (ST(ST_PLAYER_WEAPON) != OB_MEAT || ST(ST_PLAYER_WEAPON) != OB_FISH)) {
                        lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9432 */
                    if (ST(0xC9C) == 5 && ST(0xEA4) == 2) {
                        lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        lieu_action_register("L11D21B", P_GUARD, "SP32DIA56", "SP32ARR04_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x94C7 */
                    if (ST(0xC9C) == 5 && ST(0xEA4) == 3) {
                        lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x9543 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x9555 */
                    if (ST(0xC9C) == 0) {
                        lieu_action_register("L11D22", P_ARKHAN2, "SG02DIA06", NULL);
                        CB->dialogue("L11D23A", P_GUARD);
                        CB->dialogue("L11D23B", P_GUARD);
                        ST(0xC9C) = 1;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x95D5 */
                    if (ST(0xC9C) >= 1) {
                        lieu_action_register("L11D24A", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L11D24B", P_ARKHAN2, "SG02DIA02", NULL);
                        CB->dialogue("L11D25", P_GUARD);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(V_ENGAGED_PERSONA) = P_GUARD;
                        CB->combat(P_GUARD, 0);
                        ST(0x4E4) = 1;
                        return;
                    }
                }
            }
            /* 0x9674 */
            if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_PLAYER_WEAPON) == OB_VEGETABLE || ST(ST_PLAYER_WEAPON) == OB_POTATO || ST(ST_PLAYER_WEAPON) == OB_ROOT || ST(ST_PLAYER_WEAPON) == OB_FISH || ST(ST_PLAYER_WEAPON) == OB_MEAT || ST(ST_PLAYER_WEAPON) == OB_RAT || ST(ST_PLAYER_WEAPON) == OB_ROAST_BIRD) && ST(0xEA4) == 0) {
                lieu_action_register("L11D26", P_ARKHAN2, "SG02DIA02", NULL);
                /* 0x972C */
                if (ST(ST_PLAYER_WEAPON) == OB_RAT) {
                    CB->dialogue("L11D27", P_GUARD);
                } else {
                    CB->dialogue("L11D28", P_GUARD);
                }
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x978C */
            if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_PLAYER_WEAPON) == OB_VEGETABLE || ST(ST_PLAYER_WEAPON) == OB_POTATO || ST(ST_PLAYER_WEAPON) == OB_ROOT || ST(ST_PLAYER_WEAPON) == OB_FISH || ST(ST_PLAYER_WEAPON) == OB_MEAT || ST(ST_PLAYER_WEAPON) == OB_ROAST_BIRD) && ST(0xEA4) != 0) {
                /* 0x981C */
                if (ST(0xEA4) == 1) {
                    lieu_action_register("L11D29", P_ARKHAN2, "SG02DIA02", NULL);
                    CB->dialogue("L11D30", P_GUARD);
                    /* 0x9854 */
                    if (ST(ST_PLAYER_WEAPON) == OB_VEGETABLE) {
                        l11_preload_anims(ST(ST_HERO), P_GUARD, OB_VEGETABLE, 6, 12);
                    }
                    /* 0x9881 */
                    if (ST(ST_PLAYER_WEAPON) == OB_POTATO) {
                        l11_preload_anims(ST(ST_HERO), P_GUARD, OB_POTATO, 6, 12);
                    }
                    /* 0x98AE */
                    if (ST(ST_PLAYER_WEAPON) == OB_ROOT) {
                        l11_preload_anims(ST(ST_HERO), P_GUARD, OB_ROOT, 6, 12);
                    }
                    /* 0x98DB */
                    if (ST(ST_PLAYER_WEAPON) == OB_FISH) {
                        l11_preload_anims(ST(ST_HERO), P_GUARD, OB_FISH, 6, 12);
                        ST(0xEA4) = 2;
                    }
                    /* 0x9917 */
                    if (ST(ST_PLAYER_WEAPON) == OB_MEAT) {
                        l11_preload_anims(ST(ST_HERO), P_GUARD, OB_MEAT, 6, 12);
                        ST(0xEA4) = 2;
                    }
                    /* 0x9953 */
                    if (ST(ST_PLAYER_WEAPON) == OB_ROAST_BIRD) {
                        l11_preload_anims(ST(ST_HERO), P_GUARD, OB_ROAST_BIRD, 6, 12);
                    }
                    CB->animation("P32ACT02");
                    CB->animation("P32ARR04");
                    CB->animation("Sar_20");
                    ST(V_DLG_RANDOM) = CB->random(4);
                    /* 0x99B8 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L11D31", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    /* 0x99E3 */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L11D32", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    /* 0x9A0E */
                    if (ST(V_DLG_RANDOM) == 2) {
                        lieu_action_register("L11D33", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    /* 0x9A39 */
                    if (ST(V_DLG_RANDOM) == 3) {
                        lieu_action_register("L11D34", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    /* 0x9A64 */
                    if (ST(0xEA4) == 2) {
                        lieu_action_register("L11D35", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9AAD */
                if (ST(0xEA4) >= 2) {
                    lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                    lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                    lieu_action_register("L11D21B", P_GUARD, "SP32DIA56", "SP32ARR04_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x9B30 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_PLAYER_WEAPON) == OB_RAT && ST(0xEA4) == 1) {
                lieu_action_register("L11D36", P_ARKHAN2, "SG02DIA02", NULL);
                CB->dialogue("L11D37A", P_GUARD);
                CB->dialogue("L11D37B", P_GUARD);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x4E4) = 1;
                ST(V_ENGAGED_PERSONA) = P_GUARD;
                CB->combat(P_GUARD, 0);
                return;
            }
            /* 0x9BE2 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_PLAYER_WEAPON) == OB_FULL_GOURD && ST(0xEA4) == 2) {
                snprintf(g_txt, sizeof(g_txt), "OB24%c", (char)(ST(ST_USED_VARIANT) + 65));
                snprintf(g_txt2, sizeof(g_txt2), "OB77%c", (char)(ST(ST_USED_VARIANT) + 65));
                lieu_action_register("L11D38", P_ARKHAN2, "SG02DIA02", NULL);
                CB->dialogue("L11D39", P_GUARD);
                l11_preload_anims(ST(ST_HERO), P_GUARD, OB_FULL_GOURD, 6, 12);
                ST(0x140 + 4 * ST(ST_USED_VARIANT)) = 0;
                CB->link_objet(P_GUARD, g_txt, 27, 1);
                CB->animation("P32ACT03");
                CB->link_objet(P_GUARD, g_txt, 27, 0);
                CB->link_objet(P_GUARD, g_txt2, 27, 1);
                CB->animation("P32CADB");
                CB->pos_perso(P_GUARD, 0, 0, 0, 2, "P32CADB", 0xC, 0);
                ST(0xEA4) = 3;
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9D52 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_PLAYER_WEAPON) == OB_FLASK77 && ST(0xEA4) == 2) {
                lieu_action_register("L11D38", P_ARKHAN2, "SG02DIA02", NULL);
                CB->dialogue("L11D39", P_GUARD);
                l11_preload_anims(ST(ST_HERO), P_GUARD, OB_FULL_GOURD, 6, 12);
                snprintf(g_txt, sizeof(g_txt), "OB77%c", (char)(ST(ST_USED_VARIANT) + 65));
                CB->link_objet(P_GUARD, g_txt, 27, 1);
                CB->animation("P32ACT04");
                CB->dialogue("L11D40A", P_GUARD);
                CB->dialogue("L11D40B", P_GUARD);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x4E4) = 1;
                ST(V_ENGAGED_PERSONA) = P_GUARD;
                CB->combat(P_GUARD, 0);
                return;
            }
            /* 0x9E6F */
            if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_PLAYER_WEAPON) == OB_FULL_GOURD || ST(ST_PLAYER_WEAPON) == OB_FLASK77)) {
                /* 0x9EA5 */
                if (ST(0xEA4) != 2 && ST(0xEA4) != 0) {
                    lieu_action_register("L11D38", P_ARKHAN2, "SG02DIA02", NULL);
                    CB->dialogue("L11D42A", P_GUARD);
                    CB->dialogue("L11D42B", P_GUARD);
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9F28 */
                if (ST(0xEA4) == 0) {
                    lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                    lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
    /* 0x9F8F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARD)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
        CB->vire_col(7, 0);
        CB->bloque(BLOQUE_ALL);
        /* 0x9FF8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->fouille(P_GUARD);
            /* 0xA015 */
            if (ST(0x60C) == 0) {
                lieu_action_register("L11D43", P_ARKHAN2, "SG02DIA05", NULL);
                ST(0x60C) = 1;
            }
        }
        /* 0xA04C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->fouille(P_GUARD);
            /* 0xA069 */
            if (ST(0x60C) == 0) {
                lieu_action_register("L11D44", P_ARKHAN3, "SG03DIA05", NULL);
                ST(0x60C) = 1;
            }
        }
    }
    /* 0xA09D */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(ST_SCRIPT_BUSY) = 1;
        CB->vire_col(7, 0);
        CB->dialogue("L11D02", P_GUARD);
        ST(0x4E4) = 1;
        ST(V_ENGAGED_PERSONA) = P_GUARD;
        CB->combat(P_GUARD, 0);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        return;
    }
    /* 0xA152 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_GUARD)) != VIE_DEAD && CB->direction_perso(P_GUARD)) {
        ST(V_CRI_USED) = 1;
        /* 0xA1C8 */
        if (ST(0x80) != 99) {
            ST(0x80) = 32;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0, 1000);
        return;
    }
}

/* trigger 13 — sub_1000A1FF */
static void trig13(void) {
    /* 0xA20E */
    if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = P_GUARD;
        CB->dialogue("L11D23B", P_GUARD);
        CB->combat(P_GUARD, 0);
    }
}

/* trigger 41 — sub_10007976 */
static void trig41(void) {
    /* 0x7988 */
    if (ST(0x4E4) == 1 && ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = P_GUARD;
        CB->combat(P_GUARD, 0);
    }
}

