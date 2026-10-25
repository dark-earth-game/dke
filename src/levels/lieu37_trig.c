/*
 * lieu37_trig.c — Native hand-port of the LIEU37.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu37.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu37_int.h"

static void trig01(void); static void trig02(void); static void trig03(void);
static void trig04(void); static void trig05(void); static void trig07(void);
static void trig08(void); static void trig10(void); static void trig11(void);
static void trig12(void); static void trig13(void); static void trig14(void);
static void trig15(void); static void trig17(void); static void trig41(void);
static void trig_noop(void);   /* the 27 stub slots */

/* dword_1001B800[42] — filled at INIT with the 5-byte jmp thunks; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000A051       28  1000A3B0 stub
 *    1  10008C18       15  1000A13A       29  1000A3BB stub
 *    2  10009014       16  1000A247 stub  30  1000A3C6 stub
 *    3  10009091       17  1000A252       31  1000A3D1 stub
 *    4  1000918D       18  1000A342 stub  32  1000A3DC stub
 *    5  100092A5       19  1000A34D stub  33  1000A3E7 stub
 *    6  10009711 stub  20  1000A358 stub  34  1000A3F2 stub
 *    7  1000971C       21  1000A363 stub  35  1000A3FD stub
 *    8  1000975A       22  1000A36E stub  36  1000A408 stub
 *    9  10009BC6 stub  23  1000A379 stub  37  1000A413 stub
 *   10  10009BD1       24  1000A384 stub  38  1000A41E stub
 *   11  10009CC1       25  1000A38F stub  39  1000A429 stub
 *   12  10009DB1       26  1000A39A stub  40  1000A434 stub
 *   13  10009EB5       27  1000A3A5 stub  41  10007BCA
 */
static void (*const g_trigger[42])(void) = {
    l37_trig00, trig01, trig02, trig03, trig04, trig05, trig_noop, trig07,
    trig08, trig_noop, trig10, trig11, trig12, trig13, trig14, trig15,
    trig_noop, trig17, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001B800]). */
void l37_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 — sub_100073F0 (thunk sub_10001073; INIT default) */
void l37_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(37)) == 1) {
        ST(0x550) = 0;
        ST(ST_LEVEL) = 35;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_SOB86_ACTIVE) = 1;
    }
    /* 0x744D */
    if (ST(ST_START_POINT(37)) == 2) {
        ST(0x550) = 0;
        ST(ST_LEVEL) = 35;
        ST(ST_HERO) = P_ARKHAN3;
    }
    ST(ST_START_POINT(37)) = 0;
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
    ST(0x90) = 0;
    ST(0x94) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_CRI_USED) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(ST_PLAYER_WEAPON) = 71;
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
    CB->ajoute_zone(1, -1126, 13620, 430, 140, 1);
    CB->ajoute_zone(2, -495, 15080, 295, 525, 0);
    CB->ajoute_zone(3, -1543, 16087, 100, 335, 1);
    CB->ajoute_zone(4, -1074, 15808, 90, 665, 1);
    CB->ajoute_zone(7, -490, 14792, 285, 140, 1);
    CB->ajoute_zone(10, -1760, 14380, 320, 175, 0);
    CB->ajoute_zone(11, -1650, 14550, 200, 225, 0);
    CB->ajoute_zone(12, -1895, 15050, 220, 200, 0);
    CB->ajoute_zone(13, -1525, 15500, 210, 220, 0);
    CB->ajoute_zone(14, -1340, 15070, 200, 220, 0);
    CB->ajoute_zone(15, -1950, 15460, 140, 220, 0);
    CB->ajoute_zone(16, -2000, 15440, 40, 40, 0);
    CB->ajoute_zone(17, -1744, 14830, 240, 240, 0);
    CB->bruitage("L37AMB1", -1, -1);
    /* 0x77DD */
    if (ST(ST_LEVEL) == 35 || ST(ST_LEVEL) == 37) {
        ST(0x64) = 1;
        CB->pos_perso(ST(ST_HERO), -1130, 13710, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        /* 0x7849 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(98, -1130, 13710, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        }
        /* 0x7894 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(99, -1130, 13710, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        }
    }
    /* 0x78DC */
    if (ST(ST_LEVEL) == 38) {
        ST(0x64) = 2;
        CB->pos_perso(ST(ST_HERO), -500, 15080, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
    }
    /* 0x7939 */
    if (ST(ST_VIE(39)) < 2) {
        /* 0x7948 */
        if (ST(ST_LEVEL) == 35) {
            /* 0x795A */
            if (ST(0xE2C) != 1) {
                CB->pos_perso(39, -1950, 14146, 0xE290, 1, "P39GAR01", 8, 0);
            }
            /* 0x7991 */
            if (ST(0xE2C) == 1) {
                CB->pos_perso(39, -1387, 14676, 0x8120, 1, "P39GAR01", 8, 0);
            }
        }
        /* 0x79C5 */
        if (ST(ST_LEVEL) == 38) {
            CB->pos_perso(39, -766, 15739, 0x9688, 1, "P39GAR01", 8, 0);
            /* 0x7A07 */
            if (CB->divers(DV_DESC_POS, 39, 0) > -1300) {
                CB->vire_col(-8, 0);
            }
        }
    }
    /* 0x7A26 */
    if (ST(ST_VIE(39)) == VIE_DEAD) {
        CB->vire_col(-8, 0);
        CB->pos_perso(39, CB->divers(DV_DESC_POS, 39, 0), CB->divers(DV_DESC_POS, 39, 1), CB->divers(DV_DESC_POS, 39, 2), 2, "P39cad", 8, 0);
    }
    /* 0x7A95 */
    if (ST(ST_VIE(38)) < 2) {
        /* 0x7AA4 */
        if (ST(ST_LEVEL) == 35) {
            CB->pos_perso(38, -1702, 15480, 0x4650, 1, "P38ACT01", 5, 0);
        }
        /* 0x7AD8 */
        if (ST(ST_LEVEL) == 38) {
            CB->pos_perso(38, -449, 15695, 0x73A0, 1, "P38GAR01", 5, 0);
            ST(V_ENGAGED_PERSONA) = 38;
        }
    }
    /* 0x7B1B */
    if (ST(ST_VIE(38)) == VIE_DEAD) {
        CB->pos_perso(38, CB->divers(DV_DESC_POS, 38, 0), CB->divers(DV_DESC_POS, 38, 1), CB->divers(DV_DESC_POS, 38, 2), 2, "P38cad", 5, 0);
    }
    ST(ST_START_POINT(37)) = 0;
    /* 0x7B8C */
    if (ST(ST_VIE(P_DELIA)) == VIE_DEAD || ST(V_DELIA_PHASE) >= 10) {
        ST(0xC44) = 1;
    }
    CB->divers(DV_MUSIC, 1, 0);
}

/* trigger 41 — sub_10007BCA */
static void trig41(void) {
    CB->divers(DV_MUSIC, 1, 1);
    /* 0x7BEE */
    if (ST(0x550) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0x550) = 1;
        /* 0x7C1A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L37D01A", P_ARKHAN2, "SG00DIA01", NULL);
            lieu_action_register("L37D01B", P_ARKHAN2, "SG00DIA05", NULL);
        }
        /* 0x7C58 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L37D02A", P_ARKHAN3, "SG00DIA01", NULL);
            lieu_action_register("L37D02B", P_ARKHAN3, "SG00DIA05", NULL);
        }
    }
    /* 0x7C96 */
    if (ST(V_SOB86_ACTIVE) == 1) {
        ST(V_SOB86_ACTIVE) = 0;
        CB->bloque(BLOQUE_ALL);
        /* 0x7CC2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L37D03", P_ARKHAN2, "SG00DIA01", NULL);
        }
        /* 0x7CEA */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L37D04", P_ARKHAN3, "SG00DIA01", NULL);
        }
        CB->divers_empile(DV_SCREEN, 5, 0);
        /* 0x7D24 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(P_ARKHAN2, CB->divers(DV_DESC_POS, P_ARKHAN2, 0), CB->divers(DV_DESC_POS, P_ARKHAN2, 1), CB->divers(DV_DESC_POS, P_ARKHAN2, 2), 0, "ar", 6, 0);
        }
        /* 0x7D86 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(P_ARKHAN3, CB->divers(DV_DESC_POS, P_ARKHAN3, 0), CB->divers(DV_DESC_POS, P_ARKHAN3, 1), CB->divers(DV_DESC_POS, P_ARKHAN3, 2), 0, "ar", 6, 0);
        }
        CB->divers_empile(DV_SCREEN, 3, 0);
        /* 0x7DFA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD668", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x7E22 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD669", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7E52 */
    if (ST(0xE2C) == 1) {
        /* 0x7E64 */
        if (ST(ST_VIE(39)) != VIE_DEAD && ST(ST_VIE(38)) != VIE_DEAD) {
            ST(V_ENGAGED_PERSONA) = 3938;
            CB->combat(38, 0);
            CB->combat(39, 0);
        }
        /* 0x7EAE */
        if (ST(ST_VIE(39)) == VIE_DEAD && ST(ST_VIE(38)) != VIE_DEAD) {
            ST(V_ENGAGED_PERSONA) = 38;
            CB->combat(38, 0);
        }
        /* 0x7EEB */
        if (ST(ST_VIE(39)) != VIE_DEAD && ST(ST_VIE(38)) == VIE_DEAD) {
            ST(V_ENGAGED_PERSONA) = 39;
            CB->combat(39, 0);
        }
    }
}

/* trigger 1 — sub_10008C18 */
static void trig01(void) {
    /* 0x8C2A */
    if (ST(V_SOB86_ACTIVE) == 1 && ST(0x94) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0x94) = 1;
        /* 0x8C6B */
        if (ST(V_HUD_TIMER_MS) < 600000) {
            ST(V_HUD_TIMER_MS) = 300000;
        }
        /* 0x8C8C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->divers(DV_MUSIC, 1, 4);
            ST(ST_LEVEL) = 37;
            /* 0x8CC4 */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP02PRE73");
                CB->animation("P02_65535");
            }
            CB->bloque(BLOQUE_NONE);
            CB->change_piece(35, 0);
            return;
        }
        /* 0x8D06 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->divers(DV_MUSIC, 1, 4);
            ST(ST_LEVEL) = 37;
            /* 0x8D3E */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP03PRE73");
                CB->animation("P03_65535");
            }
            CB->bloque(BLOQUE_NONE);
            CB->change_piece(35, 0);
            return;
        }
    }
    /* 0x8D80 */
    if (ST(V_SOB86_ACTIVE) == 0 && ST(0x80) == 0) {
        ST(0x80) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8DC7 */
        if (ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(38)) != VIE_DEAD) {
            CB->combat(38, -1);
            CB->animation("SP38GAR01_10");
            /* 0x8E11 */
            if (CB->divers(DV_DESC_POS, 38, 1) < 13900) {
                CB->pos_perso(38, -1220, 13915, CB->divers(DV_DESC_POS, 38, 2), 1, "P38GAR01", 5, 0);
            }
        }
        /* 0x8E53 */
        if (ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(39)) != VIE_DEAD) {
            CB->combat(39, -1);
            CB->animation("SP39GAR01_10");
            /* 0x8E9D */
            if (CB->divers(DV_DESC_POS, 39, 1) < 13900) {
                CB->pos_perso(39, -1060, 13915, CB->divers(DV_DESC_POS, 39, 2), 1, "P39GAR01", 8, 0);
            }
        }
        /* 0x8EE2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L37D11", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x8F0A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L37D12", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        /* 0x8F4E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->joindre(-P_ARKHAN2, -1140, 13750, 0, -1);
        }
        /* 0x8F79 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->joindre(-P_ARKHAN3, -1140, 13750, 0, -1);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(0x84), 2);
        return;
    }
    /* 0x8FCB */
    if (ST(V_SOB86_ACTIVE) == 0 && ST(0x80) == 2) {
        ST(V_SOB86_ACTIVE) = 666;
        CB->bloque(BLOQUE_ALL);
        CB->change_piece(35, 0);
        return;
    }
}

/* trigger 2 — sub_10009014 */
static void trig02(void) {
    /* 0x9023 */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)) {
        trig05();
        return;
    }
    /* 0x905C */
    if (ST(ST_ACTION_KEY) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
        return;
    }
}

/* trigger 3 — sub_10009091 */
static void trig03(void) {
    /* 0x90A0 */
    if (ST(ST_UPDOWN_KEY) == 1 && ST(0x64) == 1) {
        ST(0x64) = 2;
        CB->pos_perso(ST(ST_HERO), -877, 15874, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        CB->animation("Sar_10");
        /* 0x9114 */
        if (ST(V_ENGAGED_PERSONA) != 0) {
            /* 0x9126 */
            if (ST(ST_VIE(38)) != VIE_DEAD) {
                CB->pos_perso(38, -1034, 16000, 0x7148, 1, "P38GAR01", 5, 0);
            }
            /* 0x915D */
            if (ST(ST_VIE(39)) != VIE_DEAD) {
                CB->pos_perso(39, -1034, 15808, 0x7148, 1, "P39GAR01", 8, 0);
            }
        }
    }
}

/* trigger 4 — sub_1000918D */
static void trig04(void) {
    /* 0x919C */
    if (ST(ST_UPDOWN_KEY) == 1 && ST(0x64) == 2) {
        ST(0x64) = 1;
        CB->pos_perso(ST(ST_HERO), -1900, 15887, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        CB->animation("Sar_10");
        /* 0x9210 */
        if (ST(V_ENGAGED_PERSONA) != 0) {
            /* 0x9222 */
            if (ST(ST_VIE(38)) != VIE_DEAD) {
                CB->pos_perso(38, -1890, 16022, CB->divers(DV_DESC_POS, 38, 2), 1, "P38GAR01", 5, 0);
            }
            /* 0x9267 */
            if (ST(ST_VIE(39)) != VIE_DEAD) {
                CB->pos_perso(39, -2034, 16030, CB->divers(DV_DESC_POS, 39, 2), 1, "P39GAR01", 8, 0);
            }
        }
    }
}

/* trigger 5 — sub_100092A5 */
static void trig05(void) {
    /* 0x92B8 */
    if (CB->direction_perso(38) && ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0) {
        /* 0x92E8 */
        if (ST(ST_VIE(38)) == 1) {
            /* 0x92F7 */
            if (ST(0x7C) == 2 || ST(0x70) == 1) {
                CB->bloque(BLOQUE_ALL);
                /* 0x9323 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L37D13A", P_ARKHAN2, "SG00DIA02", NULL);
                    /* 0x935A */
                    if (ST(0x7C) == 2) {
                        lieu_action_register("L37D13B", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                }
                /* 0x9382 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L37D14", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0x93BC */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L37D15A", P_ARKHAN3, "SG00DIA02", NULL);
                    /* 0x93F3 */
                    if (ST(0x7C) == 2) {
                        lieu_action_register("L37D15B", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                }
                /* 0x941B */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L37D16", P_ARKHAN3, "SG00DIA02", NULL);
                }
                CB->bloque(BLOQUE_NONE);
            } else {
                CB->bloque(BLOQUE_ALL);
                /* 0x946D */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L37D17", P_ARKHAN2);
                }
                /* 0x948F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L37D18", P_ARKHAN3);
                }
                CB->bloque(BLOQUE_NONE);
            }
        }
        /* 0x94B9 */
        if (ST(ST_VIE(38)) == VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_CRI_USED) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0x9512 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x9524 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0x953E */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02ACT71");
                        lieu_action_register("L37D19A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L37D19B", P_ARKHAN2, "SP02ACT74", "SP02ACT74");
                    } else {
                        CB->animation("SP02ACT70");
                        lieu_action_register("L37D19A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L37D19B", P_ARKHAN2, "SP02ACT74", "SP02ACT74");
                    }
                }
                /* 0x95CF */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0x95E9 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03ACT71");
                        lieu_action_register("L37D20A", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                        lieu_action_register("L37D20B", P_ARKHAN3, "SP03ACT74", "SP03ACT74");
                    } else {
                        CB->animation("SP03ACT70");
                        lieu_action_register("L37D20A", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                        lieu_action_register("L37D20B", P_ARKHAN3, "SP03ACT74", "SP03ACT74");
                    }
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
            /* 0x969E */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x96B0 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L37D21", P_ARKHAN2);
                }
                /* 0x96D2 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L37D22", P_ARKHAN3);
                }
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
        }
    }
}

/* trigger 7 — sub_1000971C */
static void trig07(void) {
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 37;
    CB->bloque(BLOQUE_NONE);
    CB->change_piece(38, 0);
}

/* trigger 8 — sub_1000975A */
static void trig08(void) {
    /* 0x9769 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(39)) {
        /* 0x979D */
        if (ST(ST_VIE(39)) == 1) {
            /* 0x97AC */
            if (ST(0x7C) == 2 || ST(0x70) == 1) {
                CB->bloque(BLOQUE_ALL);
                /* 0x97D8 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L37D13A", P_ARKHAN2, "SG00DIA02", NULL);
                    /* 0x980F */
                    if (ST(0x70) == 2) {
                        lieu_action_register("L37D23", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                }
                /* 0x9837 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L37D14", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0x9871 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L37D15A", P_ARKHAN3, "SG00DIA02", NULL);
                    /* 0x98A8 */
                    if (ST(0x70) == 2) {
                        lieu_action_register("L37D24", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                }
                /* 0x98D0 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L37D16", P_ARKHAN3, "SG00DIA02", NULL);
                }
                CB->bloque(BLOQUE_NONE);
            } else {
                CB->bloque(BLOQUE_ALL);
                /* 0x9922 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L37D17", P_ARKHAN2);
                }
                /* 0x9944 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L37D18", P_ARKHAN3);
                }
                CB->bloque(BLOQUE_NONE);
            }
        }
        /* 0x996E */
        if (ST(ST_VIE(39)) == VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_CRI_USED) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0x99C7 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x99D9 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0x99F3 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02ACT71");
                        lieu_action_register("L37D25A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L37D25B", P_ARKHAN2, "SP02ACT74", "SP02ACT74");
                    } else {
                        CB->animation("SP02ACT70");
                        lieu_action_register("L37D25A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L37D25B", P_ARKHAN2, "SP02ACT74", "SP02ACT74");
                    }
                }
                /* 0x9A84 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0x9A9E */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03ACT71");
                        lieu_action_register("L37D26A", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                        lieu_action_register("L37D26B", P_ARKHAN3, "SP03ACT74", "SP03ACT74");
                    } else {
                        CB->animation("SP03ACT70");
                        lieu_action_register("L37D26A", P_ARKHAN3, "P03ACT74", "SP03ACT74");
                        lieu_action_register("L37D26B", P_ARKHAN3, "SP03ACT74", "SP03ACT74");
                    }
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
            /* 0x9B53 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x9B65 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L37D21", P_ARKHAN2);
                }
                /* 0x9B87 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L37D22", P_ARKHAN3);
                }
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
        }
    }
}

/* trigger 10 — sub_10009BD1 */
static void trig10(void) {
    /* 0x9BE0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x2198)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE09_10");
        /* 0x9C66 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L37D27", P_ARKHAN2);
        }
        /* 0x9C88 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L37D28", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 11 — sub_10009CC1 */
static void trig11(void) {
    /* 0x9CD0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x2EE0)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE09_10");
        /* 0x9D56 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L37D29", P_ARKHAN2);
        }
        /* 0x9D78 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD60", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 — sub_10009DB1 */
static void trig12(void) {
    /* 0x9DC0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 0 && (CB->direction_spec(0x2198) || CB->direction_spec(0x55F0))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9E4E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L37D31", P_ARKHAN2, "SG00DIA01", NULL);
        }
        /* 0x9E76 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L37D32", P_ARKHAN3, "SG00DIA01", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 13 — sub_10009EB5 */
static void trig13(void) {
    /* 0x9EC4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9F1D */
        if (ST(0x624) > 0) {
            /* 0x9F2F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L37D33A", P_ARKHAN2, "SG00PRE09", NULL);
                lieu_action_register("L37D33B", P_ARKHAN2, "SG00DIA01", NULL);
            }
            /* 0x9F6D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L37D34", P_ARKHAN3, "SG00PRE09", NULL);
            }
        }
        /* 0x9F95 */
        if (ST(0x624) == 0) {
            ST(0x624) = 1;
            CB->animation("SG00PRE08_10");
            /* 0x9FC1 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L37D35", P_ARKHAN2, "SG00PRE09", NULL);
            }
            /* 0x9FE9 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L37D36", P_ARKHAN3, "SG00PRE09", NULL);
            }
            CB->etat_objet("OB43B", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("ar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 — sub_1000A051 */
static void trig14(void) {
    /* 0xA060 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE01");
        /* 0xA0D3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L37D37", P_ARKHAN2, "G00DIA06", NULL);
        }
        /* 0xA0FB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L37D38", P_ARKHAN3, "G00DIA06", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 — sub_1000A13A */
static void trig15(void) {
    /* 0xA149 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(16)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, -17);
        CB->animation("SG00PRE09_10");
        /* 0xA1EC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD10", P_ARKHAN2);
        }
        /* 0xA20E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD08", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 17 — sub_1000A252 */
static void trig17(void) {
    /* 0xA261 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4650)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE09_10");
        /* 0xA2E7 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L37D27", P_ARKHAN2);
        }
        /* 0xA309 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L37D28", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}
