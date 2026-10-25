/*
 * lieu25_trig.c - Native hand-port of the LIEU25.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu25.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu25_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig02(void); static void trig03(void); static void trig05(void);
static void trig08(void); static void trig09(void); static void trig10(void);
static void trig11(void); static void trig12(void); static void trig14(void);
static void trig15(void); static void trig17(void); static void trig20(void);
static void trig41(void);
static void trig_noop(void);   /* the 29 stub slots (shared L01 trig06 body) */

/* trigger table at 0x1001FBA0 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000+5*i; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000C5C8       28  1000D305 stub
 *    1  10009314 stub  15  1000CCD8       29  1000D310 stub
 *    2  1000931F       16  1000CFA6 stub  30  1000D31B stub
 *    3  1000A304       17  1000CFB1       31  1000D326 stub
 *    4  1000B2A1 stub  18  1000D141 stub  32  1000D331 stub
 *    5  1000B2AC       19  1000D14C stub  33  1000D33C stub
 *    6  1000B40B stub  20  1000D157       34  1000D347 stub
 *    7  1000B416 stub  21  1000D2B8 stub  35  1000D352 stub
 *    8  1000B421       22  1000D2C3 stub  36  1000D35D stub
 *    9  1000B8A4       23  1000D2CE stub  37  1000D368 stub
 *   10  1000BC63       24  1000D2D9 stub  38  1000D373 stub
 *   11  1000C187       25  1000D2E4 stub  39  1000D37E stub
 *   12  1000C3E6       26  1000D2EF stub  40  1000D389 stub
 *   13  1000C5BD stub  27  1000D2FA stub  41  10007DDA
 */
static void (*const g_trigger[42])(void) = {
    l25_trig00, trig_noop, trig02, trig03, trig_noop, trig05, trig_noop, trig_noop,
    trig08, trig09, trig10, trig11, trig12, trig_noop, trig14, trig15,
    trig_noop, trig17, trig_noop, trig_noop, trig20, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001FBA0]). */
void l25_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - sub_100073F0 - level setup: start points, music, zones, objects, actor placement */
void l25_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(25)) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(0xE78) = 6;
        ST(0x9C) = 3000;
        ST(ST_LEVEL) = 23;
    }
    /* 0x744D */
    if (ST(ST_START_POINT(25)) == 2) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(0xE78) = 7;
        ST(ST_LEVEL) = 23;
    }
    /* 0x7489 */
    if (ST(ST_START_POINT(25)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(0xE78) = 7;
        ST(ST_LEVEL) = 23;
    }
    /* 0x74D4 */
    if (ST(ST_START_POINT(25)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xE78) = 10;
        ST(ST_LEVEL) = 23;
    }
    /* 0x7510 */
    if (ST(ST_START_POINT(25)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xE78) = 9;
        ST(0xBD0) = 1;
        ST(ST_LEVEL) = 23;
    }
    /* 0x755B */
    if (ST(ST_START_POINT(25)) == 6) {
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xE78) = 9;
        ST(0xBD0) = 2;
        ST(ST_LEVEL) = 23;
        ST(0xD20) = 1;
    }
    /* 0x75C4 */
    if (ST(ST_START_POINT(25)) == 7) {
        ST(ST_LEVEL) = 23;
        ST(ST_VIE(P_RYLSADHAR20)) = VIE_DEAD;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xB10) = 1;
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
    /* 0x76C3 */
    if (ST(ST_VIE(P_RYLSADHAR20)) == VIE_DEAD || ST(0xBE4) == 1) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x76F9 */
    if (ST(0xB10) == 1 && ST(0xBE4) == 0) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    CB->load_anim("225OUV03", 0);
    /* 0x773C */
    if (ST(ST_HERO) > 1) {
        CB->load_anim("225OUV01", 1);
    }
    CB->ajoute_zone(5, 6575, -3670, 85, 100, 0);
    CB->ajoute_zone(14, 6969, -3355, 95, 95, 0);
    CB->ajoute_zone(8, 6926, -3667, 130, 215, 0);
    CB->ajoute_zone(9, 6962, -3513, 110, 95, 0);
    CB->ajoute_zone(10, 6932, -3821, 210, 210, 0);
    CB->ajoute_zone(11, 6680, -3859, 250, 250, 0);
    CB->ajoute_zone(12, 6926, -3081, 170, 230, 0);
    CB->ajoute_zone(13, 6898, -3921, 20, 20, 0);
    CB->ajoute_zone(20, 6681, -3778, 170, 190, 0);
    /* 0x786F */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->ajoute_zone(15, 6648, -3016, 275, 135, 0);
        CB->ajoute_zone(16, 6648, -2876, 20, 20, 0);
    }
    CB->ajoute_zone(7, 6771, -3418, 20, 20, 0);
    CB->ajoute_zone(18, 6777, -3066, 20, 20, 0);
    CB->ajoute_zone(19, 6656, -3131, 20, 20, 0);
    /* 0x790E */
    if (ST(0x5D0) == 0) {
        CB->etat_objet("OB22", 6985, -1447, -3629, 0x1E00, 0, 0, -1, 3);
    }
    /* 0x7944 */
    if (ST(ST_LEVEL) == 23) {
        CB->pos_perso(ST(ST_HERO), 6571, -3650, 0xD37F, 1, "ar", 1, 0);
    }
    /* 0x797E */
    if (ST(ST_LEVEL) == 26) {
        CB->pos_perso(ST(ST_HERO), 6960, -3215, 0x47A2, 1, "ar", 1, 0);
    }
    /* 0x79B8 */
    if (ST(ST_LEVEL) != 23 && ST(ST_LEVEL) != 26) {
        CB->pos_perso(ST(ST_HERO), 6571, -3650, 0xD37F, 1, "ar", 1, 0);
    }
    ST(0x9C) = ST(ST_DEBUG_PRINT) - ST(0xA0);
    /* 0x7A27 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        /* 0x7A39 */
        if (ST(0xE78) == 7) {
            CB->vire_col(8, 0);
            CB->pos_perso(P_RYLSADHAR20, 6670, -3000, 0x7530, 1, "P20ARR02", 2, 1);
        }
        /* 0x7A7D */
        if (ST(0xE78) == 5) {
            CB->pos_perso(P_RYLSADHAR20, 6672, -3617, 0xEDAC, 1, "P20ARR01", 2, 1);
        }
        /* 0x7AB4 */
        if (ST(0xE78) == 6) {
            /* 0x7AC9 */
            if (ST(0x9C) > 2000 && ST(0x9C) <= 4000) {
                CB->pos_perso(P_RYLSADHAR20, 6754, -3414, 0x59F, 1, "P20ARR01", 2, 1);
            }
            /* 0x7B18 */
            if (ST(0x9C) > 4000) {
                CB->vire_col(8, 0);
                CB->pos_perso(P_RYLSADHAR20, 6670, -3000, 0x7530, 1, "P20ARR02", 2, 1);
            }
        }
    } else {
        /* 0x7B61 */
        if (ST(0xD20) == 0) {
            CB->etat_objet("OB81", 6554, -1554, -2968, 0, 0xCC00, 0x4000, -1, 3);
            CB->ajoute_zone(17, 6608, -2980, 185, 210, 0);
        }
        /* 0x7BBE */
        if (ST(ST_VIE(P_RYLSADHAR20)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) == 0 && ST(0xBD0) != 2) {
            /* 0x7BF4 */
            if (ST(0xE78) == 7) {
                CB->vire_col(8, 0);
                CB->etat_objet("OB81", 6554, -1554, -2968, 0, 0xCC00, 0x4000, -1, 3);
                CB->ajoute_zone(17, 6608, -2980, 185, 210, 0);
                CB->pos_perso(P_RYLSADHAR20, 6670, -3000, 0x7530, 1, "P20ARR02", 2, 651);
                CB->pos_perso(P_KALHI13, 4539, -3629, 0xD984, 1, "P13ARR01", 3, 651);
            }
            /* 0x7CAE */
            if (ST(0xE78) == 9 && ST(0xBD0) == 1) {
                /* 0x7CD2 */
                if (ST(ST_VIE(P_KALHI13)) < 2) {
                    CB->pos_perso(P_KALHI13, 6881, -3289, 0x9C77, 1, "P13ARR02", 3, 651);
                } else {
                    CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13CAD", 3, 0);
                }
            }
        }
    }
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(0x118) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    ST(0x80) = 0;
    ST(0x88) = 0;
    ST(ST_START_POINT(25)) = 0;
}

/* trigger 41 - sub_10007DDA - room entry: music, Rylsadhar, Kalhi13 and hero greetings */
static void trig41(void) {
    /* 0x7DEC */
    if (ST(ST_VIE(P_RYLSADHAR20)) == VIE_DEAD && ST(0xB10) != 5) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x7E22 */
    if (ST(0xB10) == 5) {
        CB->divers(DV_MUSIC, 2, 1);
    }
    /* 0x7E46 */
    if (ST(ST_HERO) == P_ARKHAN1 && ST(0xE78) < 7) {
        CB->joindre(P_RYLSADHAR20, 6670, -3050, 0x608, -1);
    }
    /* 0x7E86 */
    if (ST(ST_HERO) > 1 && ST(0xE78) == 9 && ST(0xBD0) != 2 && ST(ST_VIE(P_KALHI13)) == 1 && CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0) && ST(0xE58) == 0) {
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(1, CAM_LOCKED);
        CB->joindre_perm(-P_KALHI13, ST(ST_HERO));
        /* 0x7F2B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L25D106", P_KALHI13, "SP13DIA50", "SP13ARR02_10");
        }
        /* 0x7F56 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD45", P_KALHI13, "SP13DIA50", "SP13ARR02_10");
        }
        CB->change_camera(99, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7F99 */
    if (ST(ST_HERO) > 1 && ST(0xBD0) == 2 && ST(0xE0C) == 0) {
        ST(0xE0C) = 1;
        /* 0x7FDE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("L25D200A", P_ARKHAN2, "SG00DIA01", NULL);
            /* 0x8011 */
            if (ST(0xB40) < 2) {
                lieu_action_register("L25D200B", P_ARKHAN2, "SG00DIA06", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        /* 0x8041 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("L25D201A", P_ARKHAN3, "SG00DIA01", NULL);
            /* 0x8074 */
            if (ST(0xB40) < 2) {
                lieu_action_register("L25D201B", P_ARKHAN3, "SG00DIA06", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
    }
}

/* trigger 2 - sub_1000931F - Rylsadhar: first contact, talk, shout, search, strike */
static void trig02(void) {
    /* 0x9331 */
    if (ST(0xD04) == 0 && ST(ST_VIE(P_RYLSADHAR20)) != VIE_DEAD) {
        /* 0x9355 */
        if (ST(ST_HERO) == P_ARKHAN1 && ST(0xE78) == 7 && ST(0xD08) > 0) {
            CB->divers(DV_LOOK_AT_PLAYER, P_RYLSADHAR20, 1);
            ST(0x70) = 1;
        }
        /* 0x93A9 */
        if (ST(ST_HERO) > 1 && ST(ST_LOCKED_CAMERA) == 2) {
            ST(0xD04) = 1;
            CB->bloque(BLOQUE_ALL);
            CB->joindre(ST(ST_HERO), 0, 0, 0, 20);
            CB->select_pile(1, 1);
            CB->divers(DV_LOOK_AT_PLAYER, P_RYLSADHAR20, 1);
            ST(0x70) = 1;
            /* 0x942E */
            if (ST(0xD0C) == 0) {
                CB->animation("SP20ATT02");
            }
            CB->animation("P20LEV02");
            CB->animation("P20ARR01_100");
            CB->joindre(-P_RYLSADHAR20, 0, 0, 0, ST(ST_HERO));
            /* 0x947E */
            if (ST(0xD0C) == 0) {
                lieu_action_register("L25D07A", P_RYLSADHAR20, "SP20DIA58", "SP20ARR01_100");
                lieu_action_register("L25D07B", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01_100");
                CB->modify_var(&ST(0xD0C), 1);
            }
            /* 0x94D6 */
            if (ST(ST_COMBAT_STANCE) == 1 && ST(0x64) == 0) {
                ST(0x64) = 1;
                lieu_action_register("L25D27", P_RYLSADHAR20, "SP20ATT51", "SP20ARR01_100");
            }
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0x9536 */
    if ((ST(ST_HERO) == P_ARKHAN2 || ST(ST_HERO) == P_ARKHAN3) && ST(ST_VIE(P_RYLSADHAR20)) != VIE_DEAD && ST(0xD0C) == 1 && ST(ST_COMBAT_STANCE) == 1 && ST(0x64) == 0) {
        ST(0x64) = 1;
        lieu_action_register("L25D27", P_RYLSADHAR20, "SP20ATT51", "SP20ARR01_100");
    }
    /* 0x95BE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_RYLSADHAR20)) != VIE_DEAD && ST(0xE78) == 7 && CB->direction_perso(P_RYLSADHAR20)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x964B */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x965D */
            if (ST(0xD08) == 0) {
                ST(0xD08) = 1;
                /* 0x967E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L25D01A", P_ARKHAN1, "SG01DIA06", NULL);
                    lieu_action_register("L25D01B", P_ARKHAN1, "SG01DIA06", NULL);
                }
                /* 0x96BC */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L25D106A", P_ARKHAN1, "SG01DIA06", NULL);
                    lieu_action_register("L25D106B", P_ARKHAN1, "SG01DIA06", NULL);
                }
                lieu_action_register("L25D02A", P_RYLSADHAR20, "SP20ATT01", "SP20ARR02");
                lieu_action_register("L25D02B", P_RYLSADHAR20, "SP20ATT01", "SP20ARR02");
                CB->divers(DV_LOOK_AT_PLAYER, P_RYLSADHAR20, 1);
                ST(0x70) = 1;
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9773 */
            if (ST(0xD08) == 1 && (ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON) || CB->divers(DV_CAN_FIGHT, 0, 2))) {
                /* 0x97C8 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L25D03", P_ARKHAN1, "SG01DIA07", NULL);
                }
                /* 0x97F0 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L25D06", P_ARKHAN1, "SG01DIA07", NULL);
                }
                lieu_action_register("L25D04A", P_RYLSADHAR20, "SP20ATT01", "SP20ARR02");
                lieu_action_register("L25D04B", P_RYLSADHAR20, "SP20ATT01", "SP20ARR02");
                lieu_action_register("L25D04C", P_RYLSADHAR20, "SP20ATT01", "SP20ARR02");
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x988C */
            if (ST(0xD08) == 1 && ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON) && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("L25D104", P_ARKHAN1, "SG00PRE81", NULL);
                lieu_action_register("L25D105A", P_RYLSADHAR20, "SP20ATT01", "SP20ARR02");
                lieu_action_register("L25D105B", P_RYLSADHAR20, "SP20ATT01", "SP20ARR02");
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x9966 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x9978 */
            if (ST(0x958) == 0) {
                ST(0x958) = 1;
            }
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 20);
            CB->select_pile(1, 1);
            CB->joindre(-P_RYLSADHAR20, 0, 0, 0, ST(ST_HERO));
            CB->select_pile(0, 1);
            /* 0x99F0 */
            if (ST(ST_LOCKED_CAMERA) != 2) {
                CB->change_camera(2, CAM_LOCKED);
            }
            ST(0xEB0) = 1;
            /* 0x9A21 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L25D08", P_ARKHAN2, "SG02DIA08", NULL);
                lieu_action_register("L25D09A", P_RYLSADHAR20, "SP20DIA50", "SP20ARR01");
                lieu_action_register("L25D09B", P_RYLSADHAR20, "SP20DIA55", "SP20ARR01");
                lieu_action_register("L25D09C", P_RYLSADHAR20, "SP20DIA53", "SP20ARR01");
                lieu_action_register("L25D09D", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01");
                lieu_action_register("L25D10A", P_ARKHAN2, "SG02DIA06", NULL);
                lieu_action_register("L25D10B", P_ARKHAN2, "SG02DIA06", NULL);
                lieu_action_register("L25D11", P_RYLSADHAR20, "SP20DIA60", "SP20ARR01");
                lieu_action_register("L25D12A", P_ARKHAN2, "SG02DIA07", NULL);
                lieu_action_register("L25D12B", P_ARKHAN2, "SG02DIA07", NULL);
                lieu_action_register("L25D13A", P_RYLSADHAR20, "SP20DIA61", "SP20ARR01");
                lieu_action_register("L25D13B", P_RYLSADHAR20, "SP20DIA57", "SP20ARR01");
                lieu_action_register("L25D13C", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01");
                lieu_action_register("L25D13D", P_RYLSADHAR20, "SP20DIA50", "SP20ARR01");
                lieu_action_register("L25D13E", P_RYLSADHAR20, "SP20DIA51", "SP20ARR01");
                lieu_action_register("L25D14", P_ARKHAN2, "SG02DIA02", NULL);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(0x78), 1);
                return;
            }
            /* 0x9BF1 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L25D19", P_ARKHAN2, "SG02DIA08", NULL);
                lieu_action_register("L25D09A", P_RYLSADHAR20, "SP20DIA50", "SP20ARR01");
                lieu_action_register("L25D09B", P_RYLSADHAR20, "SP20DIA55", "SP20ARR01");
                lieu_action_register("L25D09C", P_RYLSADHAR20, "SP20DIA53", "SP20ARR01");
                lieu_action_register("L25D09D", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01");
                lieu_action_register("L25D20A", P_ARKHAN2, "SG02DIA06", NULL);
                lieu_action_register("L25D20B", P_ARKHAN2, "SG02DIA06", NULL);
                lieu_action_register("L25D11", P_RYLSADHAR20, "SP20DIA60", "SP20ARR01");
                lieu_action_register("L25D12A", P_ARKHAN2, "SG02DIA07", NULL);
                lieu_action_register("L25D12B", P_ARKHAN2, "SG02DIA07", NULL);
                lieu_action_register("L25D13A", P_RYLSADHAR20, "SP20DIA61", "SP20ARR01");
                lieu_action_register("L25D13B", P_RYLSADHAR20, "SP20DIA57", "SP20ARR01");
                lieu_action_register("L25D13C", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01");
                lieu_action_register("L25D13D", P_RYLSADHAR20, "SP20DIA50", "SP20ARR01");
                lieu_action_register("L25D13E", P_RYLSADHAR20, "SP20DIA51", "SP20ARR01");
                lieu_action_register("L25D21", P_ARKHAN2, "SG02DIA02", NULL);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(0x78), 1);
                return;
            }
        }
        /* 0x9DC1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x9DD3 */
            if (ST(0x958) == 0) {
                ST(0x958) = 1;
            }
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 20);
            CB->select_pile(1, 1);
            CB->joindre(-P_RYLSADHAR20, 0, 0, 0, ST(ST_HERO));
            CB->select_pile(0, 1);
            /* 0x9E4B */
            if (ST(ST_LOCKED_CAMERA) != 2) {
                CB->change_camera(2, CAM_LOCKED);
            }
            ST(0xEB0) = 1;
            /* 0x9E7C */
            if (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L25D22", P_ARKHAN3, "SG03DIA08", NULL);
                lieu_action_register("L25D09A", P_RYLSADHAR20, "SP20DIA50", "SP20ARR01");
                lieu_action_register("L25D09B", P_RYLSADHAR20, "SP20DIA55", "SP20ARR01");
                lieu_action_register("L25D09C", P_RYLSADHAR20, "SP20DIA53", "SP20ARR01");
                lieu_action_register("L25D09D", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01");
                lieu_action_register("L25D23A", P_ARKHAN3, "SG03DIA06", NULL);
                lieu_action_register("L25D23B", P_ARKHAN3, "SG03DIA06", NULL);
                lieu_action_register("L25D11", P_RYLSADHAR20, "SP20DIA60", "SP20ARR01");
                lieu_action_register("L25D24", P_ARKHAN3, "SG03DIA07", NULL);
                lieu_action_register("L25D13A", P_RYLSADHAR20, "SP20DIA61", "SP20ARR01");
                lieu_action_register("L25D13B", P_RYLSADHAR20, "SP20DIA57", "SP20ARR01");
                lieu_action_register("L25D13C", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01");
                lieu_action_register("L25D13D", P_RYLSADHAR20, "SP20DIA50", "SP20ARR01");
                lieu_action_register("L25D13E", P_RYLSADHAR20, "SP20DIA51", "SP20ARR01");
                lieu_action_register("L25D25", P_ARKHAN3, "SG03DIA02", NULL);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(0x78), 1);
                return;
            }
        }
    }
    /* 0xA048 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_RYLSADHAR20) && ST(ST_VIE(P_RYLSADHAR20)) != VIE_DEAD) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xA0C9 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("L25D04A", P_RYLSADHAR20, "SP20ATT01", "SP20ARR02");
        }
        /* 0xA0F4 */
        if (ST(ST_HERO) > 1) {
            lieu_action_register("L25D27", P_RYLSADHAR20, "SP20ATT51", "SP20ARR01_100");
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA148 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_RYLSADHAR20)) == VIE_DEAD) {
        CB->fouille(P_RYLSADHAR20);
        return;
    }
    /* 0xA1A6 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_RYLSADHAR20)) != VIE_DEAD) {
        /* 0xA1E8 */
        if (ST(ST_HERO) > 1 && ST(0xD0C) != 0) {
            CB->select_pile(0, 2);
            CB->divers(DV_LOOK_AT_PLAYER, P_RYLSADHAR20, 0);
            ST(0xD0C) = 0;
            /* 0xA237 */
            if (ST(V_PLAYER_STRIKE) == 1) {
                CB->evenement(0x26, 375);
            }
            /* 0xA256 */
            if (ST(V_PLAYER_STRIKE) == 2) {
                CB->evenement(0x26, 1500);
            }
            ST(V_PLAYER_STRIKE) = -1;
            ST(0xBB4) = 1;
        }
        /* 0xA293 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->select_pile(0, 2);
            CB->divers(DV_LOOK_AT_PLAYER, P_RYLSADHAR20, 0);
            ST(V_PLAYER_STRIKE) = -1;
            CB->evenement(0x27, 375);
            CB->bloque(BLOQUE_ALL);
            ST(0x68) = 10;
            ST(0xE78) = 10;
            return;
        }
    }
}

/* trigger 3 - sub_1000A304 - zones 17/5 then Kalhi13: guard stance, shout, talk, search, strike */
static void trig03(void) {
    /* 0xA324 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 17) && CB->direction_zone(17)) {
        trig17();
    }
    /* 0xA35C */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 5) && CB->direction_zone(5)) {
        trig05();
    }
    /* 0xA386 */
    if (ST(ST_VIE(P_KALHI13)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) == 0) {
        /* 0xA3A7 */
        if (ST(0x80) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KALHI13)) {
            CB->bloque(BLOQUE_ALL);
            ST(0x80) = 1;
            CB->stop_joindre(-P_KALHI13);
            lieu_action_register("L25D85", P_KALHI13, "SP13GAR26", "SP13GAR02");
            CB->joindre_perm(P_KALHI13, ST(ST_HERO));
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA43C */
        if (ST(0x80) == 1 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_KALHI13)) {
            CB->bloque(BLOQUE_ALL);
            ST(0x80) = 0;
            CB->stop_joindre(-P_KALHI13);
            lieu_action_register("L25D86", P_KALHI13, "SP13GAR27", "SP13ARR01");
            CB->joindre_perm(P_KALHI13, ST(ST_HERO));
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xA4D1 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KALHI13) && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L04D41", P_KALHI13, "SP13DIA59", "SP13ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA594 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD && CB->direction_perso(P_KALHI13)) {
        CB->bloque(BLOQUE_ALL);
        CB->divers(DV_LOOK_AT_PLAYER, P_KALHI13, 1);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 13);
        CB->select_pile(2, 1);
        CB->stop_joindre(-P_KALHI13);
        CB->select_pile(0, 1);
        /* 0xA666 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA678 */
            if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                /* 0xA6CD */
                if (ST(0xD24) == 2) {
                    /* 0xA6DF */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L25D62", P_ARKHAN2, "SG02DIA03", NULL);
                    }
                    /* 0xA707 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L25D64", P_ARKHAN2, "SG02DIA03", NULL);
                    }
                    lieu_action_register("L25D63A", P_KALHI13, "SP13DIA53", "SP13ARR02");
                    lieu_action_register("L25D63B", P_KALHI13, "SP13DIA50", "SP13ARR02");
                }
                /* 0xA761 */
                if (ST(0xD24) == 1) {
                    ST(0xD24) = 2;
                    /* 0xA782 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L25D58", P_ARKHAN2, "SG02DIA05", NULL);
                    }
                    /* 0xA7AA */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L25D61", P_ARKHAN2, "SG02DIA05", NULL);
                    }
                    lieu_action_register("L25D59A", P_KALHI13, "SP13DIA50", "SP13ARR02");
                    lieu_action_register("L25D59B", P_KALHI13, "SP13DIA51", "SP13ARR02");
                    lieu_action_register("L25D59C", P_KALHI13, "SP13DIA60", "SP13ARR02");
                    lieu_action_register("L25D59D", P_KALHI13, "SP13DIA64", "SP13ARR02");
                }
                /* 0xA836 */
                if (ST(0xD24) == 0) {
                    ST(0xD24) = 1;
                    /* 0xA857 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L25D56A", P_ARKHAN2, "SG02DIA01", NULL);
                        lieu_action_register("L25D56B", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L25D56C", P_ARKHAN2, "SG02DIA03", NULL);
                        lieu_action_register("L25D56D", P_ARKHAN2, "SG02DIA04", NULL);
                    }
                    /* 0xA8C1 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L25D60A", P_ARKHAN2, "SG02DIA04", NULL);
                        lieu_action_register("L25D60B", P_ARKHAN2, "SG02DIA03", NULL);
                        lieu_action_register("L25D60C", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L25D60D", P_ARKHAN2, "SG02DIA01", NULL);
                    }
                    lieu_action_register("L25D57A", P_KALHI13, "SP13DIA63", "SP13ARR02");
                    lieu_action_register("L25D57B", P_KALHI13, "SP13DIA62", "SP13ARR02");
                    lieu_action_register("L25D57C", P_KALHI13, "SP13DIA61", "SP13ARR02");
                    lieu_action_register("L25D57D", P_KALHI13, "SP13DIA60", "SP13ARR02");
                    lieu_action_register("L25D57E", P_KALHI13, "SP13DIA64", "SP13ARR02");
                }
                CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0xA9EC */
                if (ST(ST_PLAYER_WEAPON) == 81) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    lieu_action_register("L25D94A", P_ARKHAN2, "SG02PRE81", NULL);
                    CB->dialogue("L25D94B", P_ARKHAN2);
                    lieu_action_register("L25D95", P_KALHI13, "SP13DIA65", "SP13ARR02");
                    CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xAA90 */
                if (ST(ST_PLAYER_WEAPON) != 81 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    lieu_action_register("LCOD18", P_ARKHAN2, "SG02PRE81", NULL);
                    lieu_action_register("L25D101A", P_KALHI13, "SP13DIA56", "SP13ARR02");
                    lieu_action_register("L25D101B", P_KALHI13, "SP13DIA57", "SP13ARR02");
                    lieu_action_register("L25D101C", P_KALHI13, "SP13DIA58", "SP13ARR02");
                    CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
        /* 0xAB6D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xAB7F */
            if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                /* 0xABD4 */
                if (ST(0xD28) == 2) {
                    lieu_action_register("L25D66", P_ARKHAN3, "SG03DIA03", NULL);
                    lieu_action_register("L25D63A", P_KALHI13, "SP13DIA50", "SP13ARR02");
                    lieu_action_register("L25D63B", P_KALHI13, "SP13DIA51", "SP13ARR02");
                }
                /* 0xAC2E */
                if (ST(0xD28) == 1) {
                    ST(0xD28) = 2;
                    lieu_action_register("L25D67", P_ARKHAN3, "SG03DIA03", NULL);
                    lieu_action_register("L25D59A", P_KALHI13, "SP13DIA53", "SP13ARR02");
                    lieu_action_register("L25D59B", P_KALHI13, "SP13DIA54", "SP13ARR02");
                    lieu_action_register("L25D59C", P_KALHI13, "SP13DIA55", "SP13ARR02");
                    lieu_action_register("L25D59D", P_KALHI13, "SP13DIA56", "SP13ARR02");
                }
                /* 0xACC9 */
                if (ST(0xD28) == 0) {
                    ST(0xD28) = 1;
                    lieu_action_register("L25D68A", P_ARKHAN3, "SG03DIA07", NULL);
                    lieu_action_register("L25D68B", P_ARKHAN3, "SG03DIA05", NULL);
                    lieu_action_register("L25D68C", P_ARKHAN3, "SG03DIA06", NULL);
                    lieu_action_register("L25D57A", P_KALHI13, "SP13DIA60", "SP13ARR02");
                    lieu_action_register("L25D57B", P_KALHI13, "SP13DIA61", "SP13ARR02");
                    lieu_action_register("L25D57C", P_KALHI13, "SP13DIA62", "SP13ARR02");
                    lieu_action_register("L25D57D", P_KALHI13, "SP13DIA63", "SP13ARR02");
                    lieu_action_register("L25D57E", P_KALHI13, "SP13DIA64", "SP13ARR02");
                }
                CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0xADED */
                if (ST(ST_PLAYER_WEAPON) == 81) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    lieu_action_register("L25D102A", P_ARKHAN3, "SG02PRE81", NULL);
                    lieu_action_register("L25D102B", P_ARKHAN3, "SG02PRE81", NULL);
                    lieu_action_register("L25D95", P_KALHI13, "SP13DIA53", "SP13ARR02");
                    CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xAE97 */
                if (ST(ST_PLAYER_WEAPON) != 81 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    lieu_action_register("LCOD19", P_ARKHAN3, "SG02PRE81", NULL);
                    lieu_action_register("L25D101A", P_KALHI13, "SP13DIA53", "SP13ARR02");
                    lieu_action_register("L25D101B", P_KALHI13, "SP13DIA50", "SP13ARR02");
                    lieu_action_register("L25D101C", P_KALHI13, "SP13DIA56", "SP13ARR02");
                    CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
    /* 0xAF71 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        /* 0xAFCD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xAFDF */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L04D37", P_ARKHAN2);
            }
            /* 0xB001 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L04D38", P_ARKHAN2);
            }
        }
        /* 0xB023 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xB035 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L04D39", P_ARKHAN3);
            }
            /* 0xB057 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L04D40", P_ARKHAN3);
            }
        }
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xB07E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KALHI13)) == VIE_DEAD && CB->direction_perso(P_KALHI13)) {
        CB->fouille(P_KALHI13);
        return;
    }
    /* 0xB0EF */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_KALHI13)) != VIE_DEAD && CB->direction_perso(P_KALHI13)) {
        /* 0xB141 */
        if (ST(V_PLAYER_STRIKE) == 2) {
            ST(V_PLAYER_STRIKE) = -1;
            CB->stop_joindre(-P_KALHI13);
            CB->dialogue("L04D43", P_KALHI13);
            ST(V_KALHI13_TALK_C) = 2;
            ST(V_ENGAGED_PERSONA) = P_KALHI13;
            CB->combat(P_KALHI13, 0);
            return;
        }
        ST(V_PLAYER_STRIKE) = -1;
        CB->stop_joindre(-P_KALHI13);
        CB->bloque(BLOQUE_ALL);
        /* 0xB1C9 */
        if (ST(V_KALHI13_TALK_C) == 0) {
            lieu_action_register("L04D41", P_KALHI13, "SP13IMP02", "SP13GAR02_50");
            ST(V_KALHI13_TALK_C) = 1;
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB210 */
        if (ST(V_KALHI13_TALK_C) == 1) {
            lieu_action_register("L04D42", P_KALHI13, "SP13IMP02", "SP13GAR02_50");
            ST(V_KALHI13_TALK_C) = 2;
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB257 */
        if (ST(V_KALHI13_TALK_C) == 2) {
            lieu_action_register("L04D43", P_KALHI13, "SP13IMP02", "SP13GAR02_50");
            ST(V_ENGAGED_PERSONA) = P_KALHI13;
            CB->combat(P_KALHI13, 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 5 - sub_1000B2AC - door (zone 9 side) with the 0x4000 facing */
static void trig05(void) {
    /* 0xB2BB */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000)) {
        CB->vire_col(9, 0);
        CB->bloque(BLOQUE_ALL);
        /* 0xB316 */
        if (ST(ST_VIE(P_RYLSADHAR20)) == VIE_DEAD || ST(0xBE4) == 1) {
            CB->divers(DV_MUSIC, 1, 4);
        }
        /* 0xB34C */
        if (ST(0xB10) == 1 && ST(0xBE4) == 0) {
            CB->divers(DV_MUSIC, 2, 4);
        }
        ST(ST_SCRIPT_BUSY) = 1;
        CB->pos_perso(ST(ST_HERO), 6570, -3655, 0x4000, 1, "ar", 1, 0);
        CB->animation("Sar_50");
        CB->animation("G01OUV01");
        /* 0xB3CF */
        if (ST(ST_SPECIAL_MODE) == 1) {
            CB->evenement(0, 200);
        }
        /* 0xB3EE */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->evenement(0, 600);
        }
        return;
    }
}

/* trigger 8 - sub_1000B421 - zone 8 object: look / pick up OB22 */
static void trig08(void) {
    /* 0xB430 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(8)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xB4F3 */
        if (ST(0x5D0) == 1) {
            /* 0xB505 */
            if (ST(ST_HERO) == P_ARKHAN1 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
                ST(0x6C) = CB->random(3) + 1;
                /* 0xB54D */
                if (ST(0x6C) == 1) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D28", P_ARKHAN1, "SG01PRE00", NULL);
                }
                /* 0xB57E */
                if (ST(0x6C) == 2) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D29", P_ARKHAN1, "SG01PRE01", NULL);
                }
                /* 0xB5AF */
                if (ST(0x6C) == 3) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D30", P_ARKHAN1, "SG01PRE03", NULL);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB601 */
            if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
                ST(0x6C) = CB->random(3) + 1;
                /* 0xB649 */
                if (ST(0x6C) == 1) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D31", P_ARKHAN1, "SG02PRE00", NULL);
                }
                /* 0xB67A */
                if (ST(0x6C) == 2) {
                    ST(0x6C) = 0;
                    lieu_action_register("LCOD10", P_ARKHAN1, "SG02PRE01", NULL);
                }
                /* 0xB6AB */
                if (ST(0x6C) == 3) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D33", P_ARKHAN1, "SG02PRE03", NULL);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB6FD */
            if (ST(ST_HERO) == P_ARKHAN3 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
                ST(0x6C) = CB->random(3) + 1;
                /* 0xB745 */
                if (ST(0x6C) == 1) {
                    ST(0x6C) = 0;
                    lieu_action_register("LCOD08", P_ARKHAN1, "SG03PRE00", NULL);
                }
                /* 0xB776 */
                if (ST(0x6C) == 2) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D35", P_ARKHAN1, "SG03PRE01", NULL);
                }
                /* 0xB7A7 */
                if (ST(0x6C) == 3) {
                    ST(0x6C) = 0;
                    lieu_action_register("LCOD11", P_ARKHAN1, "SG03PRE03", NULL);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        ST(0x5D0) = 1;
        /* 0xB80D */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->animation("SG01PRE06");
        }
        /* 0xB82A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02PRE06");
        }
        /* 0xB847 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03PRE06");
        }
        CB->etat_objet("OB22", -1, -1, -1, -1, -1, -1, -3, 0);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 9 - sub_1000B8A4 - zone 8 look lines */
static void trig09(void) {
    /* 0xB8B3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(8)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xB976 */
        if (ST(ST_HERO) == P_ARKHAN1 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
            ST(0x6C) = CB->random(3) + 1;
            /* 0xB9BE */
            if (ST(0x6C) == 1) {
                ST(0x6C) = 0;
                lieu_action_register("L25D28", P_ARKHAN1, "SG01PRE00", NULL);
            }
            /* 0xB9EF */
            if (ST(0x6C) == 2) {
                ST(0x6C) = 0;
                lieu_action_register("L25D29", P_ARKHAN1, "SG01PRE01", NULL);
            }
            /* 0xBA20 */
            if (ST(0x6C) == 3) {
                ST(0x6C) = 0;
                lieu_action_register("L25D30", P_ARKHAN1, "SG01PRE03", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xBA72 */
        if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
            ST(0x6C) = CB->random(3) + 1;
            /* 0xBABA */
            if (ST(0x6C) == 1) {
                ST(0x6C) = 0;
                lieu_action_register("L25D31", P_ARKHAN1, "SG02PRE00", NULL);
            }
            /* 0xBAEB */
            if (ST(0x6C) == 2) {
                ST(0x6C) = 0;
                lieu_action_register("LCOD10", P_ARKHAN1, "SG02PRE01", NULL);
            }
            /* 0xBB1C */
            if (ST(0x6C) == 3) {
                ST(0x6C) = 0;
                lieu_action_register("L25D33", P_ARKHAN1, "SG02PRE03", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xBB6E */
        if (ST(ST_HERO) == P_ARKHAN3 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
            ST(0x6C) = CB->random(3) + 1;
            /* 0xBBB6 */
            if (ST(0x6C) == 1) {
                ST(0x6C) = 0;
                lieu_action_register("LCOD08", P_ARKHAN1, "SG03PRE00", NULL);
            }
            /* 0xBBE7 */
            if (ST(0x6C) == 2) {
                ST(0x6C) = 0;
                lieu_action_register("L25D35", P_ARKHAN1, "SG03PRE01", NULL);
            }
            /* 0xBC18 */
            if (ST(0x6C) == 3) {
                ST(0x6C) = 0;
                lieu_action_register("LCOD11", P_ARKHAN1, "SG03PRE03", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 10 - sub_1000BC63 - zones 8 and 13 look lines */
static void trig10(void) {
    /* 0xBC72 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        /* 0xBCA3 */
        if (CB->direction_zone(8)) {
            CB->bloque(BLOQUE_ALL);
            CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xBD35 */
            if (ST(ST_HERO) == P_ARKHAN1 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
                ST(0x6C) = CB->random(3) + 1;
                /* 0xBD7D */
                if (ST(0x6C) == 1) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D28", P_ARKHAN1, "SG01PRE00", NULL);
                }
                /* 0xBDAE */
                if (ST(0x6C) == 2) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D29", P_ARKHAN1, "SG01PRE01", NULL);
                }
                /* 0xBDDF */
                if (ST(0x6C) == 3) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D30", P_ARKHAN1, "SG01PRE03", NULL);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xBE31 */
            if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
                ST(0x6C) = CB->random(3) + 1;
                /* 0xBE79 */
                if (ST(0x6C) == 1) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D31", P_ARKHAN1, "SG02PRE00", NULL);
                }
                /* 0xBEAA */
                if (ST(0x6C) == 2) {
                    ST(0x6C) = 0;
                    lieu_action_register("LCOD10", P_ARKHAN1, "SG02PRE01", NULL);
                }
                /* 0xBEDB */
                if (ST(0x6C) == 3) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D33", P_ARKHAN1, "SG02PRE03", NULL);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xBF2D */
            if (ST(ST_HERO) == P_ARKHAN3 && (ST(ST_SPECIAL_MODE) == 1 || ST(ST_SPECIAL_MODE) == 0)) {
                ST(0x6C) = CB->random(3) + 1;
                /* 0xBF75 */
                if (ST(0x6C) == 1) {
                    ST(0x6C) = 0;
                    lieu_action_register("LCOD08", P_ARKHAN1, "SG03PRE00", NULL);
                }
                /* 0xBFA6 */
                if (ST(0x6C) == 2) {
                    ST(0x6C) = 0;
                    lieu_action_register("L25D35", P_ARKHAN1, "SG03PRE01", NULL);
                }
                /* 0xBFD7 */
                if (ST(0x6C) == 3) {
                    ST(0x6C) = 0;
                    lieu_action_register("LCOD11", P_ARKHAN1, "SG03PRE03", NULL);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xC02A */
        if (CB->direction_zone(13)) {
            CB->bloque(BLOQUE_ALL);
            CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xC0BC */
            if (ST(ST_HERO) == P_ARKHAN1) {
                lieu_action_register("LCOD06", P_ARKHAN1, "SG01PRE09", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC102 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD07", P_ARKHAN1, "SG02PRE09", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC148 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD08", P_ARKHAN1, "SG03PRE09", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 11 - sub_1000C187 - zone 20, zone 11 look lines */
static void trig11(void) {
    /* 0xC196 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 20) && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(20)) {
        trig20();
        return;
    }
    /* 0xC200 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(11)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC2C3 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0xC2D5 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L25D43", P_ARKHAN1, "SG01DIA03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC31B */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L25D44", P_ARKHAN1, "SG01DIA03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xC361 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD76", P_ARKHAN2, "SG02DIA03", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xC3A7 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD77", P_ARKHAN3, "SG00DIA03", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 12 - sub_1000C3E6 - zone 12 look lines */
static void trig12(void) {
    /* 0xC3F5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(12)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC4B8 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0xC4CA */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L25D49", P_ARKHAN1, "SG01DIA02", NULL);
            }
            /* 0xC4F2 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L25D50", P_ARKHAN1, "SG01DIA02", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xC538 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD26", P_ARKHAN2, "SG02DIA02", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xC57E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD27", P_ARKHAN3, "SG03DIA02", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 14 - sub_1000C5C8 - crystal door: weapon / key checks */
static void trig14(void) {
    /* 0xC5D7 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xC350)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC640 */
        if (ST(0xB10) == 0) {
            /* 0xC652 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                lieu_action_register("L25D76", P_ARKHAN1, "SG01DIA02", NULL);
            }
            /* 0xC67A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L25D77", P_ARKHAN2, "SG02DIA02", NULL);
                /* 0xC6A2 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    CB->dialogue("L25D81", P_ARKHAN2);
                }
            }
            /* 0xC6DB */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L25D78", P_ARKHAN3, "SG03DIA02", NULL);
                /* 0xC703 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                    CB->dialogue("L25D84", P_ARKHAN3);
                }
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xC75A */
        if (ST(0xB10) == 1 || ST(0xB10) == 2) {
            /* 0xC77E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(ST(ST_HERO), 6959, -3318, 0xBBDF, 1, "ar", 1, 0);
                /* 0xC7BB */
                if (ST(ST_PLAYER_WEAPON) == 75) {
                    /* 0xC7CD */
                    if (ST(ST_VIE(P_RYLSADHAR20)) == VIE_DEAD || ST(0xBE4) == 1) {
                        CB->divers(DV_MUSIC, 1, 4);
                    }
                    /* 0xC803 */
                    if (ST(0xB10) == 1 && ST(0xBE4) == 0) {
                        CB->divers(DV_MUSIC, 2, 4);
                    }
                    /* 0xC839 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->evenement(1, 1500);
                        CB->animation("SP02ACT57");
                    }
                    /* 0xC866 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->evenement(1, 1500);
                        CB->animation("SP02ACT56");
                    }
                    return;
                }
                /* 0xC898 */
                if (ST(ST_PLAYER_WEAPON) == OB_BOW_TO_KEY || ST(ST_PLAYER_WEAPON) == 12 || ST(ST_PLAYER_WEAPON) == 18 || ST(ST_PLAYER_WEAPON) == 19) {
                    /* 0xC8E0 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SG00OUV14");
                        CB->animation("ar");
                        CB->dialogue("L25D79", P_ARKHAN2);
                    }
                    /* 0xC918 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SG00OUV14");
                        CB->animation("ar");
                        CB->dialogue("L25D80", P_ARKHAN2);
                    }
                }
                /* 0xC950 */
                if (ST(ST_PLAYER_WEAPON) != OB_BOW_TO_KEY && ST(ST_PLAYER_WEAPON) != 12 && ST(ST_PLAYER_WEAPON) != 18 && ST(ST_PLAYER_WEAPON) != 19 && ST(ST_PLAYER_WEAPON) != 75) {
                    /* 0xC9AA */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SG00OUV14");
                        CB->animation("ar_50");
                        CB->dialogue("L25D81", P_ARKHAN2);
                    }
                    /* 0xC9E2 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SG00OUV14");
                        CB->animation("ar_50");
                        CB->dialogue("L25D80", P_ARKHAN2);
                    }
                }
            }
            /* 0xCA1A */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(ST(ST_HERO), 6966, -3322, 0xC444, 1, "ar", 1, 0);
                /* 0xCA57 */
                if (ST(ST_PLAYER_WEAPON) == 75) {
                    /* 0xCA69 */
                    if (ST(ST_VIE(P_RYLSADHAR20)) == VIE_DEAD || ST(0xBE4) == 1) {
                        CB->divers(DV_MUSIC, 1, 4);
                    }
                    /* 0xCA9F */
                    if (ST(0xB10) == 1 && ST(0xBE4) == 0) {
                        CB->divers(DV_MUSIC, 2, 4);
                    }
                    /* 0xCAD5 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->evenement(1, 1500);
                        CB->animation("SP03ACT57");
                    }
                    /* 0xCB02 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->evenement(1, 1500);
                        CB->animation("SP03ACT56");
                    }
                    return;
                }
                /* 0xCB34 */
                if (ST(ST_PLAYER_WEAPON) == OB_BOW_TO_KEY || ST(ST_PLAYER_WEAPON) == 12 || ST(ST_PLAYER_WEAPON) == 18 || ST(ST_PLAYER_WEAPON) == 19) {
                    /* 0xCB7C */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SG00OUV14");
                        CB->animation("ar");
                        CB->dialogue("L25D82", P_ARKHAN3);
                    }
                    /* 0xCBB4 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SG00OUV14");
                        CB->animation("ar");
                        CB->dialogue("L25D83", P_ARKHAN3);
                    }
                }
                /* 0xCBEC */
                if (ST(ST_PLAYER_WEAPON) != OB_BOW_TO_KEY && ST(ST_PLAYER_WEAPON) != 12 && ST(ST_PLAYER_WEAPON) != 18 && ST(ST_PLAYER_WEAPON) != 19 && ST(ST_PLAYER_WEAPON) != 75) {
                    /* 0xCC46 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SG00OUV14");
                        CB->animation("ar");
                        CB->dialogue("L25D84", P_ARKHAN3);
                    }
                    /* 0xCC7E */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SG00OUV14");
                        CB->animation("ar");
                        CB->dialogue("L25D83", P_ARKHAN3);
                    }
                }
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 15 - sub_1000CCD8 - zone 17 then zone 16 examine */
static void trig15(void) {
    /* 0xCCE7 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 17) && CB->direction_zone(17)) {
        trig17();
        return;
    }
    /* 0xCD33 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(16)) {
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xCE05 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xCE17 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP00ACT26");
                CB->animation("ar");
                CB->dialogue("LCOD29", P_ARKHAN2);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xCE78 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP00ACT26");
                CB->animation("ar");
                CB->dialogue("LCOD30", P_ARKHAN3);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xCED9 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xCEEB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP00ACT27");
                CB->animation("ar");
                CB->dialogue("L25D74", P_ARKHAN2);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xCF4C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP00ACT27");
                CB->animation("ar");
                CB->dialogue("L25D75", P_ARKHAN3);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 17 - sub_1000CFB1 - zone 17: take OB81 (parchment) */
static void trig17(void) {
    /* 0xCFC0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(17)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD01A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            ST(0xD20) = 1;
            CB->retire_zone(17);
            lieu_action_register("L25D87", P_ARKHAN1, "Sar", "SG02PRE08");
            CB->etat_objet("OB81", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->animation("ar_50");
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xD0B1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            ST(0xD20) = 1;
            CB->retire_zone(17);
            lieu_action_register("L25D88", P_ARKHAN1, "Sar", "SG03PRE08");
            CB->etat_objet("OB81", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->animation("ar_50");
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 20 - sub_1000D157 - zone 20 look lines */
static void trig20(void) {
    /* 0xD166 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(20)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD229 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("LCOD28", P_ARKHAN1, "SG01DIA02", NULL);
        }
        /* 0xD251 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN1, "SG02DIA02", NULL);
        }
        /* 0xD279 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN1, "SG03DIA02", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

