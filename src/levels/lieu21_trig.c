/*
 * lieu21_trig.c -- Native hand-port of the LIEU21.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu21.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu21_int.h"

static void trig02(void); static void trig03(void); static void trig04(void);
static void trig07(void); static void trig10(void); static void trig14(void);
static void trig15(void); static void trig16(void); static void trig17(void);
static void trig41(void);
static void trig_noop(void);   /* the 32 stub slots */

/* dword_1001F580[42] -- filled at INIT with the 5-byte jmp thunks; static
 * here. A slot is "stub" when it is the empty handler shared with LIEU01.
 *   idx handler        idx handler        idx handler
 *    0  10001200         14  1000630C         28  1000702F stub
 *    1  10004215 stub    15  10006D0D         29  1000703A stub
 *    2  10004220         16  10006E1F         30  10007045 stub
 *    3  1000513A         17  10006F24         31  10007050 stub
 *    4  100055C7         18  10006FC1 stub    32  1000705B stub
 *    5  10005A72 stub    19  10006FCC stub    33  10007066 stub
 *    6  10005A7D stub    20  10006FD7 stub    34  10007071 stub
 *    7  10005A88         21  10006FE2 stub    35  1000707C stub
 *    8  100061DA stub    22  10006FED stub    36  10007087 stub
 *    9  100061E5 stub    23  10006FF8 stub    37  10007092 stub
 *   10  100061F0         24  10007003 stub    38  1000709D stub
 *   11  100062EB stub    25  1000700E stub    39  100070A8 stub
 *   12  100062F6 stub    26  10007019 stub    40  100070B3 stub
 *   13  10006301 stub    27  10007024 stub    41  10001C8D
 */
static void (*const g_trigger[42])(void) = {
    l21_trig00, trig_noop, trig02, trig03,
    trig04, trig_noop, trig_noop, trig07,
    trig_noop, trig_noop, trig10, trig_noop,
    trig_noop, trig_noop, trig14, trig15,
    trig16, trig17, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001F580]). */
void l21_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 -- sub_10001200 (INIT default; start points 1..6) */
void l21_trig00(void) {
    /* 0x1212 */
    if (ST(ST_START_POINT(21)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 20;
        ST(V_GUARD_FIGHT_DONE) = 0;
    }
    /* 0x1260 */
    if (ST(ST_START_POINT(21)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 22;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xB88) = 1;
    }
    /* 0x12BA */
    if (ST(ST_START_POINT(21)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 20;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xB3C) = 2;
        ST(0x330) = 1;
    }
    /* 0x1323 */
    if (ST(ST_START_POINT(21)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xB3C) = 1;
        ST(0xB28) = 2;
        ST(0x32C) = 0;
        ST(0x330) = 0;
    }
    /* 0x1380 */
    if (ST(ST_START_POINT(21)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xB38) = 1;
        ST(0x32C) = 0;
        ST(0x330) = 0;
    }
    /* 0x13EC */
    if (ST(ST_START_POINT(21)) == 6) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(0xB3C) = 2;
        ST(0xF38) = 1;
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
    CB->ajoute_zone(10, 4632, -5774, 150, 90, 1);
    /* 0x150C */
    if (ST(0xB88) == 1 && ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->ajoute_zone(11, 4853, -5583, 20, 20, 0);
        CB->ajoute_zone(12, 4403, -6106, 20, 20, 0);
        CB->ajoute_zone(21, 4853, -5683, 20, 20, 0);
    }
    /* 0x1581 */
    if (ST(V_GUARD_FIGHT_DONE) == 0 && (ST(0x330) == 1 || ST(0x32C) == 1)) {
        CB->ajoute_zone(13, 3935, -6194, 20, 20, 0);
    }
    /* 0x15D2 */
    if (ST(V_GUARD_FIGHT_DONE) == 1 || ST(0xB3C) == 2) {
        CB->ajoute_zone(20, 3927, -6182, 50, 235, 1);
        CB->ajoute_zone(16, 3782, -6127, 120, 70, 1);
    }
    CB->ajoute_zone(14, 4480, -6550, 205, 160, 0);
    CB->ajoute_zone(15, 4475, -6756, 350, 350, 0);
    /* 0x166E */
    if (ST(ST_LEVEL) == 20) {
        ST(0xE0) = 0;
        CB->pos_perso(ST(ST_HERO), 4605, -5924, 0x7ADF, 1, "ar", 1, 0);
    }
    /* 0x16B7 */
    if (ST(ST_LEVEL) == 22) {
        ST(0xE0) = 1;
        CB->ajoute_zone(20, 3927, -6182, 50, 235, 1);
        CB->ajoute_zone(16, 3782, -6127, 120, 70, 1);
        CB->pos_perso(ST(ST_HERO), 3841, -6209, 0xB800, 1, "ar", 1, 0);
        /* 0x173C */
        if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xF38) == 1) {
            CB->pos_perso(ST(ST_HERO), 4011, -6209, 0xC923, 1, "ar", 1, 0);
        }
    }
    /* 0x1788 */
    if (ST(ST_LEVEL) != 20 && ST(ST_LEVEL) != 22) {
        CB->pos_perso(ST(ST_HERO), 4605, -5924, 0x7ADF, 1, "ar", 1, 0);
    }
    /* 0x17D4 */
    if (ST(V_GUARD_FIGHT_DONE) == 0 && (ST(0x330) == 1 || ST(0x32C) == 1)) {
        ST(V_GUARD_FIGHT_DONE) = 1;
        CB->pos_perso(P_THANANDAR25, 4359, -7173, 0xF57A, 1, "P25ARR01", 5, 0);
        CB->vire_col(-1, 0);
        CB->vire_col(-2, 0);
        CB->vire_col(-3, 0);
        CB->vire_col(-4, 0);
        CB->vire_col(-5, 0);
        CB->vire_col(-6, 0);
        CB->vire_col(-7, 0);
        /* 0x1899 */
        if (ST(0x32C) == 1) {
            CB->pos_perso(P_RYLSADHAR21, 4003, -6217, 0x319A, 1, "P20ARR01", -1, 0);
        }
        /* 0x18D0 */
        if (ST(0x330) == 1) {
            CB->pos_perso(P_ZED27, 4003, -6217, 0x319A, 1, "P27ARR01", -1, 0);
        }
    }
    /* 0x1907 */
    if (ST(V_GUARD_FIGHT_DONE) == 0 && ST(0xB3C) == 0 && ST(V_LCOD23_SEEN) != 2) {
        /* 0x193D */
        if (ST(0xE24) < 2) {
            CB->pos_perso(P_SUNSEER1, 4483, -7321, 0x850C, 1, "P18ARR01", 2, 1);
        }
        /* 0x1974 */
        if (ST(0xE24) == 2) {
            CB->pos_perso(P_SUNSEER1, 0, 0, 0, 2, "P18CAD", 2, 0);
        }
    }
    /* 0x19A2 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        /* 0x19B4 */
        if (ST(0xB88) == 1) {
            CB->pos_perso(P_LORY, 4190, -6192, 0x3502, 1, "P15ARR01", 3, 0);
            CB->pos_perso(P_PHEDORIA, 4127, -6107, 0x723B, 1, "P26ARR01", 4, 0);
            ST(0xB88) = 2;
        }
        /* 0x1A1F */
        if (ST(0xB38) != 0 && ST(0xB38) != 5) {
            /* 0x1A43 */
            if (ST(ST_VIE(P_SUNSEER2)) != VIE_DEAD) {
                CB->pos_perso(P_SUNSEER2, 4984, -6499, 0xA356, 1, "P19DIA51", 7, 1);
            } else {
                CB->pos_perso(P_SUNSEER2, CB->divers(DV_DESC_POS, P_SUNSEER2, 0), CB->divers(DV_DESC_POS, P_SUNSEER2, 1), CB->divers(DV_DESC_POS, P_SUNSEER2, 2), 2, "P19CAD", 7, 0);
            }
        }
    }
    /* 0x1ACF */
    if (ST(0xB3C) == 0) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x1AF3 */
    if ((ST(0xB3C) == 1 || ST(0xB3C) == 2) && ST(V_GUARD_FIGHT_DONE) != 1) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    /* 0x1B3B */
    if ((ST(0xB3C) == 1 || ST(0xB3C) == 2) && ST(V_GUARD_FIGHT_DONE) == 1) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    /* 0x1B83 */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->bruitage("L21AMB1", 4478, -6760);
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = CB->divers(DV_HP, ST(ST_HERO), -1);
    ST(0x78) = 0;
    ST(0x7C) = 0;
    ST(0x80) = 0;
    ST(0x88) = 0;
    ST(V_EVENT15_STEP) = 0;
    ST(0x90) = 0;
    ST(0xB8) = 0;
    ST(0xE0) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(ST_START_POINT(21)) = 0;
}

/* trigger 2 (Sunseer18) -- trig02_10004220 */
static void trig02(void) {
    /* 0x422F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SUNSEER1)) != VIE_DEAD && ST(0x85C) == 1 && CB->direction_perso(P_SUNSEER1)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_SUNSEER1);
        /* 0x42D9 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x42EB */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x42FD */
                if (ST(0x860) == 0) {
                    ST(0x860) = 1;
                    lieu_action_register("L21D01", P_ARKHAN1, "SP01ATT70", NULL);
                    CB->stop_joindre(-P_SUNSEER1);
                    CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("L21D02A", P_SUNSEER1, "SP18ATT70", "SP18ARR01_10");
                    lieu_action_register("L21D02B", P_SUNSEER1, "SP18DIA01", "SP18ARR01_10");
                    lieu_action_register("L21D02C", P_SUNSEER1, "SP18DIA50", "SP18ARR01_10");
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(P_SUNSEER1, ST(ST_HERO));
                    return;
                }
                /* 0x43E6 */
                if (ST(0x860) == 1) {
                    ST(0x860) = 2;
                    lieu_action_register("L21D03", P_ARKHAN1, "SG01DIA05", NULL);
                    CB->stop_joindre(-P_SUNSEER1);
                    CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("L21D04A", P_SUNSEER1, "SP18DIA53", "SP18ARR01_10");
                    lieu_action_register("L21D04B", P_SUNSEER1, "SP18DIA51", "SP18ARR01_10");
                    lieu_action_register("L21D04C", P_SUNSEER1, "SP18DIA54", "SP18ARR01_10");
                    lieu_action_register("L21D04D", P_SUNSEER1, "SP18DIA57", "SP18ARR01_10");
                    lieu_action_register("L21D04E", P_SUNSEER1, "SP18DIA56", "SP18ARR01_10");
                    lieu_action_register("L21D04F", P_SUNSEER1, "SP18DIA59", "SP18ARR01_10");
                    lieu_action_register("L21D04G", P_SUNSEER1, "SP18DIA58", "SP18ARR01_10");
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(P_SUNSEER1, ST(ST_HERO));
                    return;
                }
                /* 0x4533 */
                if (ST(0x860) == 2) {
                    lieu_action_register("L21D01", P_ARKHAN1, "SG01ATT01", NULL);
                    CB->stop_joindre(-P_SUNSEER1);
                    CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("L21D06", P_SUNSEER1, "SP18ATT70", "SP18ARR01_10");
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(P_SUNSEER1, ST(ST_HERO));
                    return;
                }
            }
            /* 0x45DB */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x45ED */
                if (ST(0x860) == 0) {
                    ST(0x860) = 1;
                    lieu_action_register("L21D07", P_ARKHAN1, "SP01ATT70", NULL);
                    CB->stop_joindre(-P_SUNSEER1);
                    CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("L21D08", P_SUNSEER1, "SP18ATT70", "SP18ARR01_10");
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(P_SUNSEER1, ST(ST_HERO));
                    return;
                }
                /* 0x46A4 */
                if (ST(0x860) == 1) {
                    ST(0x860) = 2;
                    lieu_action_register("L21D09", P_ARKHAN1, "SG01DIA05", NULL);
                    CB->stop_joindre(-P_SUNSEER1);
                    CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("L21D04A", P_SUNSEER1, "SP18DIA60", "SP18ARR01_10");
                    lieu_action_register("L21D04B", P_SUNSEER1, "SP18DIA50", "SP18ARR01_10");
                    lieu_action_register("L21D04C", P_SUNSEER1, "SP18DIA51", "SP18ARR01_10");
                    lieu_action_register("L21D04D", P_SUNSEER1, "SP18DIA54", "SP18ARR01_10");
                    lieu_action_register("L21D04E", P_SUNSEER1, "SP18DIA56", "SP18ARR01_10");
                    lieu_action_register("L21D04F", P_SUNSEER1, "SP18DIA59", "SP18ARR01_10");
                    lieu_action_register("L21D04G", P_SUNSEER1, "SP18DIA58", "SP18ARR01_10");
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(P_SUNSEER1, ST(ST_HERO));
                    return;
                }
                /* 0x47F1 */
                if (ST(0x860) == 2) {
                    lieu_action_register("L21D10", P_ARKHAN1, "SG01ATT01", NULL);
                    CB->stop_joindre(-P_SUNSEER1);
                    CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("L21D06", P_SUNSEER1, "SP18ATT70", "SP18ARR01_10");
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(P_SUNSEER1, ST(ST_HERO));
                    return;
                }
            }
        }
        /* 0x4899 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x48AB */
            if (ST(0x864) == 0) {
                ST(0x864) = 1;
                lieu_action_register("L21D11", P_ARKHAN2, "SP02ATT60", NULL);
                CB->dialogue("L21D12", P_SUNSEER1);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x491B */
            if (ST(0x864) == 1) {
                /* 0x492D */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L21D13A", P_ARKHAN2, "SG02DIA07", NULL);
                    lieu_action_register("L21D13B", P_ARKHAN2, "SG02DIA07", NULL);
                    lieu_action_register("L21D13C", P_ARKHAN2, "SG02DIA08", NULL);
                    lieu_action_register("L21D13D", P_ARKHAN2, "SG02DIA08", NULL);
                }
                /* 0x4997 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L21D14A", P_ARKHAN2, "SG02DIA07", NULL);
                    lieu_action_register("L21D14B", P_ARKHAN2, "SG02DIA08", NULL);
                    lieu_action_register("L21D14C", P_ARKHAN2, "SG02DIA08", NULL);
                }
                CB->stop_joindre(P_SUNSEER1);
                CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L21D15A", P_SUNSEER1, "SP18DIA01", "SP18ARR01_10");
                lieu_action_register("L21D15B", P_SUNSEER1, "SP18DIA50", "SP18ARR01_10");
                lieu_action_register("L21D15C", P_SUNSEER1, "SP18DIA60", "SP18ARR01_10");
                lieu_action_register("L21D15D", P_SUNSEER1, "SP18DIA54", "SP18ARR01_10");
                lieu_action_register("L21D15E", P_SUNSEER1, "SP18DIA56", "SP18ARR01_10");
                lieu_action_register("L21D16", P_ARKHAN2, "SG02DIA01", NULL);
                lieu_action_register("L21D72A", P_SUNSEER1, "SP18DIA56", "SP18ARR01_10");
                lieu_action_register("L21D72B", P_SUNSEER1, "SP18DIA59", "SP18ARR01_10");
                lieu_action_register("L21D72C", P_SUNSEER1, "SP18DIA53", "SP18ARR01_10");
                lieu_action_register("L21D72D", P_SUNSEER1, "SP18DIA58", "SP18ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                ST(0x864) = 2;
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x4B37 */
            if (ST(0x864) == 2) {
                CB->stop_joindre(-P_SUNSEER1);
                CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L21D17", P_ARKHAN2, "SG02DIA03", NULL);
                lieu_action_register("L21D18", P_SUNSEER1, "SP18ATT70", "SP18ARR01_10");
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x4BC9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x4BDB */
            if (ST(0x87C) == 0) {
                ST(0x87C) = 1;
                lieu_action_register("L21D100", P_ARKHAN3, "SP03ATT60", NULL);
                CB->dialogue("L21D101A", P_SUNSEER1);
                CB->dialogue("L21D101B", P_SUNSEER1);
                CB->dialogue("L21D18", P_SUNSEER1);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x4C6B */
            if (ST(0x87C) == 1) {
                /* 0x4C7D */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L21D102", P_ARKHAN3, "SG03DIA07", NULL);
                }
                /* 0x4CA5 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L21D103", P_ARKHAN3, "SG03DIA07", NULL);
                }
                CB->stop_joindre(P_SUNSEER1);
                CB->joindre(-P_SUNSEER1, 0, 0, 0, ST(ST_HERO));
                /* 0x4CF2 */
                if (ST(0x7C) == 0) {
                    lieu_action_register("L21D104", P_SUNSEER1, "SP18DIA50", "SP18ARR01_10");
                    CB->dialogue("L21D18", P_SUNSEER1);
                    ST(0x7C) = 1;
                } else {
                    lieu_action_register("L21D72D", P_SUNSEER1, "SP18DIA58", "SP18ARR01_10");
                    ST(0x7C) = 0;
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0x4D81 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_SUNSEER1)) != VIE_DEAD && CB->direction_perso(P_SUNSEER1)) {
        ST(V_CRI_USED) = 1;
        /* 0x4DF7 */
        if (ST(0xE0) != 99) {
            ST(0xE0) = 18;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(33, 200);
        return;
    }
    /* 0x4E35 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_SUNSEER1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SUNSEER1)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 0;
        /* 0x4E9D */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("L21D09", P_ARKHAN1);
        }
        /* 0x4EBF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x4ED1 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0x4EF3 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD61", P_ARKHAN2);
            }
        }
        /* 0x4F15 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x4F27 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN2);
            }
            /* 0x4F49 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD63", P_ARKHAN2);
            }
        }
        CB->dialogue("L21D18", P_SUNSEER1);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x4FA1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SUNSEER1)) == VIE_DEAD) {
        CB->fouille(P_SUNSEER1);
        return;
    }
    /* 0x4FFF */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_SUNSEER1)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        /* 0x504D */
        if (ST(0x868) == 0 && ST(ST_HERO) == P_ARKHAN1) {
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_SUNSEER1);
            CB->animation("SP18GAR01");
            CB->dialogue("L21D21", P_SUNSEER1);
            CB->joindre_perm(P_SUNSEER1, ST(ST_HERO));
            ST(0x868) = 1;
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x50D4 */
        if (ST(0x868) == 1 || ST(ST_HERO) > 1) {
            CB->stop_joindre(-P_SUNSEER1);
            CB->divers(DV_LOOK_AT_PLAYER, P_SUNSEER1, 0);
            CB->animation("SP18GAR01");
            ST(V_ENGAGED_PERSONA) = P_SUNSEER1;
            CB->combat(P_SUNSEER1, 1);
            return;
        }
    }
}

/* trigger 3 (Lory, Phedoria) -- trig03_1000513A */
static void trig03(void) {
    /* 0x5149 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x51C4 */
        if (ST(0x870) == 0) {
            ST(0x870) = 1;
            /* 0x51E5 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x51F7 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L20D30", P_ARKHAN2, "SG02DIA05", NULL);
                }
                /* 0x521F */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L20D31", P_ARKHAN2, "SG02DIA05", NULL);
                }
            }
            /* 0x5247 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x5259 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L20D32", P_ARKHAN3, "SG02DIA05", NULL);
                }
                /* 0x5281 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L20D33", P_ARKHAN3, "SG02DIA05", NULL);
                }
            }
            CB->dialogue("L20D34", P_LORY);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x52E2 */
        if (ST(0x870) == 1) {
            /* 0x52F4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L20D35", P_ARKHAN2, "SG02DIA05", NULL);
            }
            /* 0x531C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L20D36", P_ARKHAN3, "SG02DIA05", NULL);
            }
            CB->dialogue("L20D37", P_LORY);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x537A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->evenement(2, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SG26FRA01");
        CB->evenement(34, 750);
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
    /* 0x54AA */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SG26FRA01");
        CB->evenement(34, 750);
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
}

/* trigger 4 -- trig04_100055C7 */
static void trig04(void) {
    /* 0x55D6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x5651 */
        if (ST(0x874) == 0) {
            ST(0x874) = 1;
            /* 0x5672 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x5684 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L20D39", P_ARKHAN2, "SG02DIA05", NULL);
                }
                /* 0x56AC */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L20D40", P_ARKHAN2, "SG02DIA05", NULL);
                }
            }
            /* 0x56D4 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x56E6 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L20D41", P_ARKHAN3, "SG03DIA05", NULL);
                }
                /* 0x570E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L20D42", P_ARKHAN3, "SG03DIA05", NULL);
                }
            }
            CB->dialogue("L20D43", P_PHEDORIA);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x576F */
        if (ST(0x874) == 1) {
            /* 0x5781 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L20D44", P_ARKHAN2, "SG02DIA05", NULL);
            }
            /* 0x57A9 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L20D45", P_ARKHAN3, "SG03DIA05", NULL);
            }
            CB->dialogue("L20D46", P_PHEDORIA);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x5807 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(0x874) = 5;
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SP26ATA17");
        CB->evenement(35, 500);
        CB->animation("P26GAR17");
        CB->select_pile(0, 1);
    }
    /* 0x5946 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(0x874) = 5;
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SP26ATA17");
        CB->evenement(35, 500);
        CB->animation("P26GAR17");
        CB->select_pile(0, 1);
    }
}

/* trigger 7 (Sunseer19) -- trig07_10005A88 */
static void trig07(void) {
    /* 0x5A97 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SUNSEER2)) != VIE_DEAD && CB->direction_perso(P_SUNSEER2)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_SUNSEER2);
        /* 0x5B28 */
        if (ST(0x884) == 0) {
            ST(0x884) = 1;
            /* 0x5B49 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L21D74", P_ARKHAN2, "SP02ATT60", NULL);
            }
            /* 0x5B71 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L21D75", P_ARKHAN3, "SP03ATT60", NULL);
            }
            CB->dialogue("L21D76", P_SUNSEER2);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x5BD2 */
        if (ST(0x884) == 1) {
            ST(0x884) = 2;
            /* 0x5BF3 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L21D77", P_ARKHAN2, "SG02DIA07", NULL);
            }
            /* 0x5C2D */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L21D78", P_ARKHAN2, "SG02DIA07", NULL);
            }
            /* 0x5C67 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L21D79", P_ARKHAN3, "SG03DIA07", NULL);
            }
            CB->stop_joindre(P_SUNSEER2);
            lieu_action_register("L21D80A", P_SUNSEER2, "SP19DIA53", "SP19ARR01");
            lieu_action_register("L21D80B", P_SUNSEER2, "SP19DIA54", "SP19ARR01");
            lieu_action_register("L21D80C", P_SUNSEER2, "SP19DIA54", "SP19ARR01");
            lieu_action_register("L21D80D", P_SUNSEER2, "SP19DIA54", "SP19ARR01");
            /* 0x5CFE */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x5D10 */
                if (ST(0xB38) == 1) {
                    lieu_action_register("L21D81", P_ARKHAN2, "SG02DIA01", NULL);
                }
                /* 0x5D38 */
                if (ST(0xB38) == 2) {
                    lieu_action_register("L21D82", P_ARKHAN2, "SG02DIA01", NULL);
                }
            }
            /* 0x5D60 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L21D83", P_ARKHAN3, "SG03DIA01", NULL);
            }
            lieu_action_register("L21D84A", P_SUNSEER2, "SP19DIA50", "SP19ARR01");
            lieu_action_register("L21D84B", P_SUNSEER2, "SP19ATT02", "SP19ATT04");
            lieu_action_register("L21D84C", P_SUNSEER2, "SP19ATT03", "SP19ATT04");
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x5DFC */
        if (ST(0x884) == 2) {
            CB->bloque(BLOQUE_ALL);
            /* 0x5E19 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L21D85", P_ARKHAN2, "SG02DIA07", NULL);
            }
            /* 0x5E41 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L21D86", P_ARKHAN3, "SG03DIA07", NULL);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x5E8D: action key + stance 1 + persona alive: empty body in the DLL (every
     * exit of the condition chain jumps to 0x5EDB) */
    /* 0x5EDB */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SUNSEER2)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x5F45 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x5F57 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0x5F79 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD61", P_ARKHAN2);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x5FC4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x5FD6 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
            /* 0x5FF8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD63", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x6043 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_SUNSEER2)) != VIE_DEAD && CB->direction_perso(P_SUNSEER2)) {
        ST(V_CRI_USED) = 1;
        /* 0x60B3 */
        if (ST(0xE0) != 99) {
            ST(0xE0) = 19;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(33, 1000);
        return;
    }
    /* 0x60FA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SUNSEER2)) == VIE_DEAD) {
        CB->fouille(P_SUNSEER2);
        return;
    }
    /* 0x6153 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_SUNSEER2)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->stop_joindre(P_SUNSEER2);
        CB->animation("SP19GAR01");
        ST(V_ENGAGED_PERSONA) = P_SUNSEER2;
        CB->combat(P_SUNSEER2, 1);
    }
}

/* trigger 10 (exit to room 20) -- trig10_100061F0 */
static void trig10(void) {
    /* 0x6202 */
    if (ST(V_GUARD_FIGHT_DONE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && CB->divers(DV_PERSONA_PRESENT, P_THANANDAR25, 0)) {
        return;
    }
    ST(V_MUSIC_TRACK) = -1;
    /* 0x6251 */
    if (ST(0xB3C) == 0) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0x6275 */
    if (ST(0xB3C) == 1 || ST(0xB3C) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    /* 0x62A8 */
    if (ST(0x64) == 1) {
        ST(0xA8) = -666;
    }
    ST(ST_LEVEL) = 21;
    CB->evenement(EV_MUSIC_LOOP_A, -1);
    CB->change_piece(20, 0);
}

/* trigger 14 -- trig14_1000630C */
static void trig14(void) {
    /* 0x631B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x6375 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->joindre(-ST(ST_HERO), 4473, -6525, 0x833D, -1);
            CB->animation("SG01PRE09");
            CB->animation("ar_50");
            CB->dialogue("L21D48", P_ARKHAN1);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        CB->joindre(-ST(ST_HERO), 4473, -6525, 0x833D, -1);
        /* 0x6422 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x6434 */
            if (ST(V_GUARD_FIGHT_DONE) == 0) {
                /* 0x6446 */
                if (ST(0xB3C) == 0) {
                    /* 0x6458 */
                    if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY) {
                        /* 0x646A */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("SP02ACT62");
                        }
                        /* 0x6487 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("SP02ACT63");
                        }
                        /* 0x64A4 */
                        if (ST(0xB28) == 0) {
                            CB->dialogue("L21D57", P_ARKHAN2);
                        }
                        /* 0x64C6 */
                        if (ST(0xB28) == 1) {
                            CB->dialogue("L21D58", P_ARKHAN2);
                        }
                        /* 0x64E8 */
                        if (ST(0xB28) == 2) {
                            CB->dialogue("L21D59", P_ARKHAN2);
                        }
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        CB->animation("SG02PRE09");
                        CB->animation("ar_50");
                        CB->dialogue("L21D60", P_ARKHAN2);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x6587 */
                if (ST(0xB3C) == 1) {
                    /* 0x6599 */
                    if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY) {
                        /* 0x65AB */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("SP02ACT62");
                        }
                        /* 0x65C8 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("SP02ACT63");
                        }
                        ST(0xB3C) = 2;
                        CB->evenement(1, 2000);
                        CB->dialogue("L21D61", P_ARKHAN2);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        return;
                    } else {
                        lieu_action_register("L21D62", P_ARKHAN2, "SG02PRE09", NULL);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x6679 */
                if (ST(0xB3C) == 2) {
                    /* 0x668B */
                    if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY) {
                        /* 0x669D */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("SP02ACT62");
                        }
                        /* 0x66BA */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("SP02ACT63");
                        }
                        ST(0xB3C) = 1;
                        CB->animation("ar_50");
                        CB->bruitage("L21AMB4", 3825, -6073);
                        CB->animation("ar");
                        CB->bruitage("L21AMB3", 4475, -6756);
                        CB->dialogue("L21D63", P_ARKHAN2);
                        CB->change_piece(-21, 1);
                        CB->retire_zone(16);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("L21D62", P_ARKHAN2, "SG02PRE09", NULL);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            } else if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY) {
                /* 0x67D5 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SP02ACT62");
                }
                /* 0x67F2 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP02ACT63");
                }
                CB->animation("ar_50");
                CB->dialogue("LCOD39", P_ARKHAN2);
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                lieu_action_register("L21D62", P_ARKHAN2, "SG02PRE09", NULL);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x6897 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x68A9 */
            if (ST(V_GUARD_FIGHT_DONE) == 0) {
                /* 0x68BB */
                if (ST(0xB3C) == 0) {
                    /* 0x68CD */
                    if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY) {
                        /* 0x68DF */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("SP03ACT62");
                        }
                        /* 0x68FC */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("SP03ACT63");
                        }
                        /* 0x6919 */
                        if (ST(0xB28) == 0) {
                            CB->dialogue("L21D64", P_ARKHAN3);
                        }
                        /* 0x693B */
                        if (ST(0xB28) == 1) {
                            CB->dialogue("L21D65", P_ARKHAN3);
                        }
                        /* 0x695D */
                        if (ST(0xB28) == 2) {
                            CB->dialogue("L21D66", P_ARKHAN3);
                        }
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        CB->animation("SG03PRE09");
                        CB->animation("ar_50");
                        CB->dialogue("L21D67", P_ARKHAN3);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x69FC */
                if (ST(0xB3C) == 1) {
                    /* 0x6A0E */
                    if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY) {
                        /* 0x6A20 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("SP03ACT62");
                        }
                        /* 0x6A3D */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("SP03ACT63");
                        }
                        ST(0xB3C) = 2;
                        CB->evenement(1, 2000);
                        CB->dialogue("L21D68", P_ARKHAN3);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("L21D69", P_ARKHAN3, "SG03PRE09", NULL);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x6AF6 */
                if (ST(0xB3C) == 2) {
                    /* 0x6B08 */
                    if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY) {
                        /* 0x6B1A */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            CB->animation("SP03ACT62");
                        }
                        /* 0x6B37 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            CB->animation("SP03ACT63");
                        }
                        ST(0xB3C) = 1;
                        CB->animation("ar_50");
                        CB->bruitage("L21AMB4", 3825, -6073);
                        CB->animation("ar");
                        CB->bruitage("L21AMB3", 4475, -6756);
                        CB->dialogue("L21D70", P_ARKHAN3);
                        CB->change_piece(-21, 1);
                        CB->retire_zone(16);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("L21D69", P_ARKHAN3, "SG03PRE09", NULL);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                return;
            }
            /* 0x6C40 */
            if (ST(ST_PLAYER_WEAPON) == OB_RED_KEY) {
                /* 0x6C52 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SP03ACT62");
                }
                /* 0x6C6F */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP03ACT63");
                }
                CB->animation("ar_50");
                CB->dialogue("LCOD40", P_ARKHAN3);
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            lieu_action_register("L21D69", P_ARKHAN3, "SG03PRE09", NULL);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 15 -- trig15_10006D0D */
static void trig15(void) {
    /* 0x6D1C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(15)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x6D85 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L21D49", P_ARKHAN1, "SG01DIA02", NULL);
        }
        /* 0x6DAD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L21D50", P_ARKHAN2, "SG02DIA02", NULL);
        }
        /* 0x6DD5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L21D51", P_ARKHAN3, "SG03DIA02", NULL);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 (exit to room 22) -- trig16_10006E1F */
static void trig16(void) {
    ST(V_MUSIC_TRACK) = -1;
    /* 0x6E40 */
    if (ST(0xB3C) == 0) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0x6E64 */
    if (ST(0xB3C) == 1 || ST(0xB3C) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 21;
    CB->evenement(EV_MUSIC_LOOP_A, -1);
    /* 0x6EB6 */
    if (ST(0x32C) != 5 && ST(0x330) != 5) {
        CB->change_piece(22, 0);
    }
    /* 0x6EE7 */
    if (ST(0x32C) == 5) {
        CB->change_piece(22, 1);
    }
    /* 0x6F09 */
    if (ST(0x330) == 5) {
        CB->change_piece(22, 2);
    }
}

/* trigger 17 (exit to room 22) -- trig17_10006F24 */
static void trig17(void) {
    ST(V_MUSIC_TRACK) = -1;
    /* 0x6F45 */
    if (ST(0xB3C) == 0) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0x6F69 */
    if (ST(0xB3C) == 1 || ST(0xB3C) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 21;
    CB->evenement(EV_MUSIC_LOOP_A, -1);
    CB->change_piece(22, 0);
}

/* trigger 41 -- trig41_10001C8D */
static void trig41(void) {
    /* 0x1C9F */
    if (ST(0xE0) == 1 && ST(V_GUARD_FIGHT_DONE) != 2 && ST(0xF38) != 1) {
        CB->change_camera(7, CAM_LOCKED);
    }
    /* 0x1CE5 */
    if (ST(0xB3C) == 0) {
        CB->divers(DV_MUSIC, 1, 1);
        ST(V_MUSIC_TRACK) = 1;
        CB->evenement(EV_MUSIC_LOOP_A, 45000);
        ST(0xB8) = ST(ST_GAME_TIME);
    }
    /* 0x1D3F */
    if ((ST(0xB3C) == 1 || ST(0xB3C) == 2) && ST(V_GUARD_FIGHT_DONE) != 1) {
        ST(0xB8) = ST(ST_GAME_TIME);
        CB->divers(DV_MUSIC, 2, 1);
        ST(V_MUSIC_TRACK) = 2;
        CB->evenement(EV_MUSIC_LOOP_A, 30000);
    }
    /* 0x1DBD */
    if ((ST(0xB3C) == 1 || ST(0xB3C) == 2) && ST(V_GUARD_FIGHT_DONE) == 1) {
        CB->divers(DV_MUSIC, 2, 1);
    }
    /* 0x1E05 */
    if (ST(0x878) == 0 && ST(V_GUARD_FIGHT_DONE) == 1 && (ST(0x330) == 1 || ST(0x32C) == 1)) {
        CB->select_pile(0, 1);
        /* 0x1E57 */
        if (ST(ST_COMBAT_STANCE) == 1) {
            CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
        }
        CB->bloque(BLOQUE_ALL);
        ST(0x878) = 1;
        ST(0xB3C) = 2;
        CB->select_pile(1, 1);
        CB->change_camera(6, CAM_LOCKED);
        CB->bruitage("L21AMB4", 3825, -6073);
        CB->divers_empile(DV_BIG_ANIM, 1, 0);
        CB->evenement(36, 11500);
        CB->evenement(38, 6500);
        CB->select_pile(0, 1);
    }
    /* 0x1F15 */
    if (ST(0xB88) == 2 && ST(V_GUARD_FIGHT_DONE) == 2) {
        ST(0xB3C) = 2;
        ST(0xB88) = 3;
        ST(0xB58) = 1;
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        /* 0x1F7E */
        if (ST(ST_COMBAT_STANCE) == 1 && CB->divers(DV_CAN_FIGHT, 9, 2)) {
            CB->divers(DV_COMBAT_STANCE, 0, 0);
        }
        CB->joindre(P_LORY, 4150, -6221, 0x4060, -1);
        /* 0x1FD5 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L21D202", P_ARKHAN2, "SG00DIA01", NULL);
        }
        /* 0x1FFD */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L21D203", P_ARKHAN3, "SG00DIA01", NULL);
        }
        lieu_action_register("L21D26A", P_LORY, "SP15DIA50", "SP15ARR01");
        lieu_action_register("L21D26B", P_LORY, "SP15DIA51", "SP15ARR01");
        lieu_action_register("L21D26C", P_LORY, "SP15DIA53", "SP15ARR01");
        lieu_action_register("L21D26D", P_LORY, "SP15DIA54", "SP15ARR01");
        CB->evenement(2, 1000);
        lieu_action_register("L21D26E", P_LORY, "SP15DIA55", "SP15ARR01");
        CB->select_pile(0, 1);
    }
}
