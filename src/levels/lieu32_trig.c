/*
 * lieu32_trig.c - Native hand-port of the LIEU32.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu32.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * The 22 stub slots are the shared L01 trig06 body (an empty function).
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu32_int.h"

static void trig04(void);
static void trig05(void);
static void trig06(void);
static void trig07(void);
static void trig10(void);
static void trig11(void);
static void trig12(void);
static void trig13(void);
static void trig14(void);
static void trig15(void);
static void trig16(void);
static void trig19(void);
static void trig20(void);
static void trig21(void);
static void trig22(void);
static void trig23(void);
static void trig24(void);
static void trig30(void);
static void trig41(void);
static void trig_noop(void);   /* the stub slots */

/* trigger table at 0x10021910 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 100073F0   1..3 stubs   4 1000B546   5 1000B81C   6 1000BA85
 *   7 1000C49F   8,9 stubs   10 1000C628  11 1000CFD9  12 1000D192
 *  13 1000D1B6  14 1000D470  15 1000DE83  16 1000E089  17,18 stubs
 *  19 1000E2F5  20 1000E41A  21 1000E6A2  22 1000E78C  23 1000E9A3
 *  24 1000EA7E  25..29 stubs  30 1000EB84  31..40 stubs  41 10008347
 */
static void (*const g_trigger[42])(void) = {
    l32_trig00,
    trig_noop,
    trig_noop,
    trig_noop,
    trig04,
    trig05,
    trig06,
    trig07,
    trig_noop,
    trig_noop,
    trig10,
    trig11,
    trig12,
    trig13,
    trig14,
    trig15,
    trig16,
    trig_noop,
    trig_noop,
    trig19,
    trig20,
    trig21,
    trig22,
    trig23,
    trig24,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig30,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig_noop,
    trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10021910]). */
void l32_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 (level setup) - sub_100073F0 */
void l32_trig00(void) {
    if (ST(ST_START_POINT(32)) == 0) {
        /* no body */
    }
    /* 0x7402 */
    if (ST(ST_START_POINT(32)) == 1) {
        ST(0xE8) = 2;
        ST(0xE8C) = 1;
        ST(0xC38) = 1;
    }
    /* 0x7453 */
    if (ST(ST_START_POINT(32)) == 2) {
        ST(0xE8) = 2;
        ST(0xE8C) = 1;
        ST(0xC38) = 0;
        ST(0xEE0) = 1;
    }
    /* 0x74A1 */
    if (ST(ST_START_POINT(32)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xE8C) = 0;
        ST(0xF00) = 0;
        ST(0xBB0) = 2;
    }
    /* 0x74EF */
    if (ST(ST_START_POINT(32)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xF24) = 1;
    }
    /* 0x752E */
    if (ST(ST_START_POINT(32)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xF24) = 3;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    /* 0x756D */
    if (ST(ST_START_POINT(32)) == 6) {
        ST(0xBB0) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xF24) = 5;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    /* 0x75BB */
    if (ST(ST_START_POINT(32)) == 10) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(0xF00) = 1;
        ST(0xE8C) = 0;
        ST(0xF24) = 5;
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
    ST(ST_START_POINT(32)) = 0;
    CB->load_anim("232OUV01", 0);
    CB->load_anim("232OUV02", 1);
    CB->vire_col(14, 1);
    CB->ajoute_zone(12, 4418, 7035, 300, 185, 1);
    CB->ajoute_zone(10, 5313, 7762, 150, 150, 1);
    CB->ajoute_zone(19, 5597, 7666, 270, 270, 0);
    CB->ajoute_zone(20, 6589, 6469, 290, 245, 0);
    CB->ajoute_zone(21, 6221, 6178, 180, 170, 0);
    CB->ajoute_zone(22, 6361, 6169, 130, 170, 0);
    CB->ajoute_zone(23, 6501, 6280, 170, 170, 0);
    CB->ajoute_zone(24, 5418, 7418, 190, 155, 0);
    /* 0x7810 */
    if (ST(0xE8C) != 0 && ST(0xC3C) == 0) {
        CB->ajoute_zone(11, 5023, 6975, 314, 160, 0);
    } else {
        CB->ajoute_zone(30, 5023, 6962, 300, 200, 0);
    }
    CB->ajoute_zone(13, 5933, 6916, 145, 190, 0);
    /* 0x789C */
    if (ST(0xF24) == 0 || ST(0xF24) == 5) {
        /* 0x78C0 */
        if (ST(0xE8C) == 1) {
            /* 0x78D2 */
            if (ST(0xC38) == 0) {
                CB->ajoute_zone(7, 5226, 7139, 20, 20, 0);
                CB->ajoute_zone(8, 5646, 7309, 20, 20, 0);
                CB->ajoute_zone(9, 5352, 7582, 20, 20, 0);
            } else {
                CB->ajoute_zone(7, 4824, 7217, 20, 20, 0);
                CB->ajoute_zone(8, 5738, 7038, 20, 20, 0);
                CB->ajoute_zone(9, 5494, 7592, 20, 20, 0);
            }
        } else if (ST(0xF00) < 2) {
            CB->ajoute_zone(7, 5226, 7139, 20, 20, 0);
        }
    }
    /* 0x79BD */
    if (ST(0xE8C) == 1) {
        ST(ST_HERO) = ST(0xE8);
        CB->divers(DV_HP, ST(ST_HERO), 256);
        CB->divers(DV_INVENTORY, 1, 0);
        /* 0x7A10 */
        if (ST(ST_COMBAT_STANCE) == 1) {
            CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
        }
        CB->pos_perso(ST(ST_HERO), 5313, 7762, 0x89A5, 1, "P02ARR01", 1, 0);
    } else {
        ST(0xB70) = 1;
        CB->pos_perso(ST(ST_HERO), 4595, 7140, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x7AAF */
    if (ST(0xF24) == 0 || ST(0xF24) == 5) {
        /* 0x7AD3 */
        if (ST(0xE8C) == 1) {
            CB->pos_perso(P_SORDOS, 5177, 7173, 0xB2FD, 1, "P23ARR03", 2, 0);
            /* 0x7B0A */
            if (ST(0xC38) == 0) {
                CB->pos_perso(P_GORK, 5809, 6920, 0x2384, 1, "P31ARR01", 4, 1);
            } else {
                CB->pos_perso(P_KONKALITE11, 5809, 6920, 0x2384, 1, "P81ARR01", 0x10, 1);
            }
        }
        /* 0x7B6B */
        if (ST(0xE8C) == 2) {
            /* 0x7B7D */
            if (ST(0xC38) == 0) {
                /* 0x7B8F */
                if (ST(ST_VIE(P_GORK)) == VIE_DEAD) {
                    CB->pos_perso(P_GORK, 0, 0, 0, 2, "P31CAD", 4, 0);
                } else {
                    CB->pos_perso(P_GORK, 5208, 7114, 0x3753, 1, "P31GAR01", 4, 1);
                }
            } else if (ST(ST_VIE(P_KONKALITE11)) == VIE_DEAD) {
                CB->pos_perso(P_KONKALITE11, 0, 0, 0, 2, "P81CAD", 0x10, 0);
            } else {
                CB->pos_perso(P_KONKALITE11, 5208, 7114, 0x3753, 1, "P81GAR03", 0x10, 1);
            }
        }
        /* 0x7C44 */
        if ((ST(0xE8C) == 0 || ST(0xE8C) == 10) && ST(0xF24) != 5) {
            /* 0x7C7A */
            if (ST(0xF00) >= 10 && ST(0xF00) < 20) {
                CB->pos_perso(P_GORK, 0, 0, 0, 2, "P31CAD", 4, 0);
                CB->pos_perso(P_KONKALITE7, 0, 0, 0, 2, "P77CAD", 5, 0);
            }
            /* 0x7CD6 */
            if (ST(0xF00) >= 2 && ST(0xF00) < 10) {
                ST(0xF00) = 3;
                /* 0x7D09 */
                if (ST(ST_VIE(P_GORK)) != VIE_DEAD) {
                    CB->pos_perso(P_GORK, 5208, 7114, 0x3753, 1, "P31GAR01", 4, 1);
                } else {
                    CB->pos_perso(P_GORK, 0, 0, 0, 2, "P31CAD", 4, 0);
                }
                /* 0x7D61 */
                if (ST(ST_VIE(P_KONKALITE7)) != VIE_DEAD) {
                    CB->pos_perso(P_KONKALITE7, 5009, 7459, 0x6082, 1, "P77GAR03", 5, 1);
                } else {
                    CB->pos_perso(P_KONKALITE7, 0, 0, 0, 2, "P77CAD", 5, 0);
                }
            }
            /* 0x7DB9 */
            if (ST(0xF00) < 2) {
                ST(0xF00) = 1;
                CB->pos_perso(P_GORK, 5890, 6966, 0x22C4, 1, "P31ARR01", 4, 1);
                CB->pos_perso(P_KONKALITE7, 5346, 7531, 0x8C6B, 1, "P77ARR01", 5, 1);
            }
        }
    }
    /* 0x7E24 */
    if (ST(0xF24) == 1) {
        ST(0xF48) = 1;
        CB->pos_perso(P_DHORKAN95, 6165, 6479, 0xFB9E, 1, "P95ARR01", 6, 1);
        CB->vire_col(14, 0);
        CB->retire_zone(13);
    }
    /* 0x7E82 */
    if (ST(0xF24) == 3) {
        /* 0x7E94 */
        if (ST(0xF28) == 0) {
            CB->pos_perso(P_KONKALITE3, 5195, 6974, 0xBA, 1, "P73ATT01", 0xF, 0);
            CB->pos_perso(P_MALROT6, 5303, 7742, 0x89A5, 1, "P66ATT02", 0xE, 800);
        }
        /* 0x7EF3 */
        if (ST(0xF28) == 1) {
            CB->pos_perso(P_KONKALITE3, 5009, 7459, 0x6082, 1, "P73GAR03", 0xF, 0);
            /* 0x7F2A */
            if (ST(0xC28) != 10) {
                CB->pos_perso(P_MALROT6, 5303, 7742, 0x89A5, 1, "P66ATT02", 0xE, 800);
            } else {
                CB->pos_perso(P_MALROT6, 0, 0, 0, 2, "P66CAD", 0xE, 0);
            }
        }
        /* 0x7F85 */
        if (ST(0xF28) >= 2) {
            CB->pos_perso(P_KONKALITE3, 0, 0, 0, 2, "P73CAD", 0xF, 0);
            /* 0x7FB3 */
            if (ST(0xC28) != 10) {
                CB->pos_perso(P_MALROT6, 5303, 7742, 0x89A5, 1, "P66ATT02", 0xE, 800);
            } else {
                CB->pos_perso(P_MALROT6, 0, 0, 0, 2, "P66CAD", 0xE, 0);
            }
        }
    }
    /* 0x800E */
    if (ST(0xF24) == 5 && (ST(0xBB0) == 2 || ST(0xEF8) == 10) && ST(0xE8C) != 1) {
        /* 0x8056 */
        if (ST(0xD80) == 0) {
            /* 0x8068 */
            if (ST(ST_VIE(P_GORK)) != VIE_DEAD) {
                CB->pos_perso(P_GORK, 5890, 6966, 0x22C4, 1, "P31ARR01", 4, 1);
            } else {
                CB->pos_perso(P_GORK, 0, 0, 0, 2, "P31CAD", 4, 0);
            }
        } else if (ST(ST_VIE(P_GORK)) != VIE_DEAD) {
            CB->pos_perso(P_GORK, 5208, 7114, 0x3753, 1, "P31GAR01", 4, 1);
        } else {
            CB->pos_perso(P_GORK, 0, 0, 0, 2, "P31CAD", 4, 0);
        }
    }
    /* 0x811D */
    if (ST(0xF24) == 6) {
        CB->pos_perso(P_GORK, 0, 0, 0, 2, "P31CAD", 4, 0);
        /* 0x814B */
        if (ST(ST_VIE(29)) != VIE_DEAD) {
            CB->pos_perso(29, CB->divers(DV_DESC_POS, 29, 0), CB->divers(DV_DESC_POS, 29, 1), CB->divers(DV_DESC_POS, 29, 2), 1, "P29GAR01", 7, 0);
        } else {
            CB->pos_perso(29, 0, 0, 0, 2, "P29CAD", 7, 0);
        }
    }
    CB->bruitage("L32AMB1", -1, -1);
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
    ST(0x84) = 0;
    ST(0x88) = 0;
    ST(V_EVENT15_STEP) = 0;
    ST(0xE0) = 0;
    ST(0xF0) = 0;
    ST(0xFC) = 0;
    ST(0x100) = 0;
    ST(0x114) = 0;
    ST(V_WEAPON_WARNED) = 0;
    /* 0x82E2 */
    if (ST(0xE98) == 60) {
        ST(0xE98) = 65;
    }
    /* 0x8303 */
    if (ST(0xF48) == 1) {
        CB->vire_col(14, 0);
    }
    /* 0x8322 */
    if (ST(0xF24) != 1) {
        CB->vire_col(15, 0);
        CB->vire_col(-2, 0);
    }
}

/* trigger 4 - sub_1000B546 */
static void trig04(void) {
    /* 0xB558 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_GORK) && ST(ST_VIE(P_GORK)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L32D505", P_GORK, "SP31GAR01", "SP31ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xB60C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GORK)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        /* 0xB668 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("LCOD20", P_ARKHAN2, "SP02ATT51", NULL);
        }
        /* 0xB6A2 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
            lieu_action_register("L32D15", P_ARKHAN2, "SP02ATT51", NULL);
        }
        /* 0xB6DC */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("LCOD21", P_ARKHAN3, "SP03ATT51", NULL);
        }
        /* 0xB716 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
            lieu_action_register("L32D16", P_ARKHAN3, "SP03ATT51", NULL);
        }
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xB765 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_GORK)) != VIE_DEAD) {
        /* no body */
    }
    /* 0xB7AA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GORK)) == VIE_DEAD) {
        ST(0xF00) = 15;
        CB->fouille(P_GORK);
        return;
    }
}

/* trigger 5 - sub_1000B81C */
static void trig05(void) {
    /* 0xB82E */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KONKALITE7) && ST(ST_VIE(P_KONKALITE7)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L34D778", P_KONKALITE7);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xB8D9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE7)) != VIE_DEAD) {
        /* 0xB92A */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("LCOD20", P_ARKHAN2, "SP02ATT51", NULL);
        }
        /* 0xB964 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD61", P_ARKHAN2);
        }
        /* 0xB998 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("LCOD21", P_ARKHAN3, "SP03ATT51", NULL);
        }
        /* 0xB9D2 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD63", P_ARKHAN3);
        }
        CB->dialogue("L34D777", P_KONKALITE7);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xBA2B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE7)) == VIE_DEAD) {
        CB->fouille(P_KONKALITE7);
        return;
    }
}

/* trigger 6 - sub_1000BA85 */
static void trig06(void) {
    /* 0xBA94 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_DHORKAN95)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBAEE */
        if (ST(0x4F4) == 0) {
            ST(0x4F4) = 1;
            /* 0xBB0F */
            if (ST(ST_LOCKED_CAMERA) != 3) {
                CB->change_camera(3, CAM_LOCKED);
            }
            /* 0xBB31 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L32D40", P_ARKHAN2, "SG00DIA01", NULL);
            }
            /* 0xBB59 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L32D41", P_ARKHAN3, "SG00DIA01", NULL);
            }
            lieu_action_register("L32D42", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            /* 0xBB9A */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L32D20", P_ARKHAN2, "SG00DIA01", NULL);
            }
            /* 0xBBD4 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L32D21", P_ARKHAN2, "SG00DIA01", NULL);
            }
            /* 0xBC0E */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L32D22", P_ARKHAN3, "SG00DIA01", NULL);
            }
            /* 0xBC48 */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L32D23", P_ARKHAN3, "SG00DIA01", NULL);
            }
            lieu_action_register("L32D24A", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D24B", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            /* 0xBCB4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L32D25", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0xBCDC */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L32D26", P_ARKHAN3, "SG00DIA02", NULL);
            }
            lieu_action_register("L32D27A", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D27B", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D27C", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D27D", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D27E", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(99, CAM_AUTO);
            return;
        }
        /* 0xBDBA */
        if (ST(0x4F4) == 1) {
            ST(0x4F4) = 2;
            /* 0xBDDB */
            if (ST(ST_LOCKED_CAMERA) != 3) {
                CB->change_camera(3, CAM_LOCKED);
            }
            /* 0xBDFD */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L32D28", P_ARKHAN2, "SG00DIA03", NULL);
            }
            /* 0xBE37 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L32D29", P_ARKHAN2, "SG00DIA03", NULL);
            }
            /* 0xBE71 */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L32D30", P_ARKHAN3, "SG00DIA03", NULL);
            }
            /* 0xBEAB */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L32D31", P_ARKHAN3, "SG00DIA03", NULL);
            }
            lieu_action_register("L32D32A", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D32B", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D32C", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D32D", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D32E", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D32F", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D32G", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(99, CAM_AUTO);
            return;
        }
        /* 0xBFCD */
        if (ST(0x4F4) == 2) {
            ST(0x4F4) = 3;
            /* 0xBFEE */
            if (ST(ST_LOCKED_CAMERA) != 3) {
                CB->change_camera(3, CAM_LOCKED);
            }
            /* 0xC010 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L32D33A", P_ARKHAN2, "SG00DIA04", NULL);
                lieu_action_register("L32D33B", P_ARKHAN2, "SG00DIA05", NULL);
            }
            /* 0xC060 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L32D34A", P_ARKHAN2, "SG00DIA04", NULL);
                lieu_action_register("L32D34B", P_ARKHAN2, "SG00DIA05", NULL);
            }
            /* 0xC0B0 */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L32D35A", P_ARKHAN3, "SG00DIA04", NULL);
                lieu_action_register("L32D35B", P_ARKHAN3, "SG00DIA05", NULL);
            }
            /* 0xC100 */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L32D36A", P_ARKHAN3, "SG00DIA04", NULL);
                lieu_action_register("L32D36B", P_ARKHAN3, "SG00DIA05", NULL);
            }
            lieu_action_register("L32D37A", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D37B", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            lieu_action_register("L32D37C", P_DHORKAN95, "SP95DIA01", "SP95ARR01");
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(99, CAM_AUTO);
            return;
        }
        /* 0xC1D4 */
        if (ST(0x4F4) >= 3) {
            /* 0xC1E6 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L32D44", P_ARKHAN2, "SG00DIA06", NULL);
                /* 0xC20E */
                if (ST(0x4F4) == 3) {
                    ST(0x4F4) = 4;
                    /* 0xC22F */
                    if (ST(0xC6C) == 0) {
                        ST(0xC6C) = 1;
                        CB->fouille(P_DHORKAN11);
                        ST(0xED4) = 1;
                        /* 0xC26A */
                        if (ST(0xED0) == 5) {
                            ST(0xED0) = 5;
                        }
                        /* 0xC28B */
                        if (ST(0xED0) == 2) {
                            ST(0xED0) = 3;
                        }
                        /* 0xC2AC */
                        if (ST(0xED0) == 1) {
                            ST(0xED0) = 2;
                        }
                        /* 0xC2CD */
                        if (ST(0xED0) == 0) {
                            ST(0xED0) = 1;
                        }
                    }
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC317 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L32D46", P_ARKHAN3, "SG00DIA06", NULL);
                /* 0xC33F */
                if (ST(0x4F4) == 3) {
                    ST(0x4F4) = 4;
                    /* 0xC360 */
                    if (ST(0xC6C) == 0) {
                        ST(0xC6C) = 1;
                        CB->fouille(P_DHORKAN11);
                        ST(0xED4) = 1;
                        /* 0xC39B */
                        if (ST(0xED0) == 5) {
                            ST(0xED0) = 5;
                        }
                        /* 0xC3BC */
                        if (ST(0xED0) == 2) {
                            ST(0xED0) = 3;
                        }
                        /* 0xC3DD */
                        if (ST(0xED0) == 1) {
                            ST(0xED0) = 2;
                        }
                        /* 0xC3FE */
                        if (ST(0xED0) == 0) {
                            ST(0xED0) = 1;
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
    /* 0xC445 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_DHORKAN95)) {
        ST(V_PLAYER_STRIKE) = -1;
        /* 0xC485 */
        if (ST(0x4F4) != 3) {
            ST(0x4F4) = 3;
        }
    }
}

/* trigger 7 - sub_1000C49F */
static void trig07(void) {
    /* 0xC4AE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(29)) {
        /* 0xC4E2 */
        if (ST(ST_VIE(29)) != VIE_DEAD) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xC50B */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L32D50", P_ARKHAN2);
            }
            /* 0xC53F */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L32D51", P_ARKHAN2);
            }
            /* 0xC573 */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L32D52", P_ARKHAN3);
            }
            /* 0xC5A7 */
            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L32D53", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
            return;
        }
        CB->fouille(29);
        return;
    }
}

/* trigger 10 - sub_1000C628 */
static void trig10(void) {
    /* 0xC63A */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_GORK) && CB->distance(1, 4) < 250 && (ST(0x6C) == 3 || ST(0x6C) == 0)) {
        CB->select_pile(1, 2);
        CB->evenement(2, -1);
        ST(V_EVENT_2) = 0;
        CB->bloque(BLOQUE_ALL);
        CB->select_pile(2, 2);
        CB->stop_joindre(-P_GORK);
        CB->joindre(P_GORK, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(1, 1);
        ST(V_CRI_USED) = 1;
        CB->evenement(37, 800);
        CB->select_pile(0, 1);
        return;
    }
    /* 0xC769 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x7918)) {
        /* 0xC7AC */
        if (ST(0xC38) == 0) {
            /* 0xC7C7 */
            if (CB->distance(1, 4) > 300 && ST(0x6C) != 3) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                /* 0xC7FF */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->animation("SP02ATT05");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("Sar_50");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xC845 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0xC854 */
                    if (ST(0x70) == 0) {
                        ST(0x70) = 1;
                        CB->animation("SP02ATT06");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("Sar_50");
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xC8A3 */
                    if (ST(0x70) == 1) {
                        CB->select_pile(1, 2);
                        ST(0xB70) = 2;
                        CB->stop_joindre(-P_GORK);
                        CB->joindre(P_GORK, 0, 0, 0, ST(ST_HERO));
                        CB->select_pile(0, 1);
                        ST(0x70) = 2;
                        ST(V_ENGAGED_PERSONA) = P_GORK;
                        ST(0x6C) = 0;
                        CB->evenement(2, -1);
                        CB->animation("SP02ATT07");
                        CB->evenement(33, 900);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("Sar_50");
                        CB->bloque(BLOQUE_NONE);
                        CB->combat(P_GORK, 0);
                        CB->change_camera(99, CAM_AUTO);
                        return;
                    }
                }
            } else {
                CB->select_pile(1, 2);
                CB->evenement(2, -1);
                ST(V_EVENT_2) = 0;
                CB->bloque(BLOQUE_ALL);
                CB->select_pile(2, 2);
                CB->stop_joindre(-P_GORK);
                CB->joindre(P_GORK, 0, 0, 0, ST(ST_HERO));
                CB->select_pile(1, 1);
                ST(ST_SCRIPT_BUSY) = 1;
                /* 0xCA31 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xCA43 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->dialogue("L32D500", P_ARKHAN2);
                    }
                    /* 0xCA65 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->dialogue("L32D501", P_ARKHAN2);
                    }
                }
                /* 0xCA87 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xCA99 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->dialogue("L32D502", P_ARKHAN3);
                    }
                    /* 0xCABB */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->dialogue("LCOD22C", P_ARKHAN3);
                    }
                }
                ST(V_DLG_RANDOM) = CB->random(3);
                /* 0xCAF4 */
                if (ST(V_DLG_RANDOM) == 0) {
                    lieu_action_register("L32D503", P_GORK, "SP31GAR01", "SP31ARR01_10");
                }
                /* 0xCB1F */
                if (ST(V_DLG_RANDOM) == 1) {
                    lieu_action_register("L32D504", P_GORK, "SP31GAR01", "SP31ARR01_10");
                }
                /* 0xCB4A */
                if (ST(V_DLG_RANDOM) == 2) {
                    lieu_action_register("L32D505", P_GORK, "SP31GAR01", "SP31ARR01_10");
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->evenement(2, 500);
                return;
            }
            return;
        }
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0xCBD4 */
        if (CB->distance(1, 16) > 400) {
            /* 0xCBE6 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->stop_joindre(-ST(ST_HERO));
                CB->animation("SP02ATT05");
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xCC58 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                /* 0xCC7E */
                if (ST(0x70) == 0) {
                    ST(0x70) = 1;
                    CB->animation("SP02ATT06");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("Sar_50");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xCCCD */
                if (ST(0x70) == 1) {
                    CB->select_pile(1, 2);
                    ST(0xB70) = 2;
                    CB->stop_joindre(-P_KONKALITE11);
                    CB->joindre(P_KONKALITE11, 0, 0, 0, ST(ST_HERO));
                    CB->select_pile(0, 1);
                    ST(0x70) = 2;
                    CB->animation("SP02ATT07");
                    CB->evenement(33, 900);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("Sar_50");
                    CB->bloque(BLOQUE_NONE);
                    CB->combat(P_KONKALITE11, 0);
                    CB->change_camera(99, CAM_AUTO);
                    return;
                }
            }
            return;
        }
        CB->select_pile(1, 2);
        CB->evenement(35, -1);
        ST(V_EVENT_35) = 0;
        CB->bloque(BLOQUE_ALL);
        CB->select_pile(2, 2);
        CB->stop_joindre(-P_KONKALITE11);
        CB->joindre(P_KONKALITE11, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(1, 1);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xCE33 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xCE45 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L32D506", P_ARKHAN2);
                lieu_action_register("L32D509A", P_KONKALITE11, "SP81DIA56", "SP81ARR01_10");
                lieu_action_register("L32D509B", P_KONKALITE11, "SP81DIA56", "SP81ARR01_10");
            }
            /* 0xCE99 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L32D507", P_ARKHAN2);
                lieu_action_register("L32D510A", P_KONKALITE11, "SP81DIA56", "SP81ARR01_10");
                lieu_action_register("L32D510B", P_KONKALITE11, "SP81DIA56", "SP81ARR01_10");
            }
        }
        /* 0xCEED */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xCEFF */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L32D508", P_ARKHAN3);
                lieu_action_register("L32D509A", P_KONKALITE11, "SP81DIA56", "SP81ARR01_10");
                lieu_action_register("L32D509B", P_KONKALITE11, "SP81DIA56", "SP81ARR01_10");
            }
            /* 0xCF53 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD22B", P_ARKHAN3);
                lieu_action_register("L32D511A", P_KONKALITE11, "SP81DIA56", "SP81ARR01_10");
                lieu_action_register("L32D511B", P_KONKALITE11, "SP81DIA56", "SP81ARR01_10");
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->evenement(35, 500);
        return;
    }
}

/* trigger 11 - sub_1000CFD9 */
static void trig11(void) {
    /* 0xCFE8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0xC3C) = 1;
        CB->retire_zone(11);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        CB->animation("SG01PRE09");
        CB->animation("ar_10");
        /* 0xD0DB */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ATT92");
            CB->dialogue("LCOD92", P_ARKHAN2);
        }
        /* 0xD108 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ATT92");
            CB->dialogue("LCOD93", P_ARKHAN3);
        }
        CB->ajoute_zone(30, 5023, 6962, 300, 200, 0);
        CB->animation("Sar_50");
        CB->divers_empile(DV_INVENTORY, 0, 0);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 - sub_1000D192 */
static void trig12(void) {
    ST(ST_LEVEL) = 32;
    CB->change_piece(31, 0);
}

/* trigger 13 - sub_1000D1B6 */
static void trig13(void) {
    /* 0xD1C5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(13) && ST(0xF48) == 0) {
        /* 0xD21A */
        if (ST(ST_PLAYER_WEAPON) == OB_KONK_KEY) {
            /* 0xD22C */
            if (ST(0xF24) == 5 && ST(0x330) == 0) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                ST(0xF48) = 1;
                ST(0xF24) = 6;
                ST(0x32C) = 1;
                /* 0xD294 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02OUV15");
                }
                /* 0xD2B1 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG03OUV15");
                }
                CB->animation("ar_50");
                CB->ajoute_zone(17, 5155, 7724, 145, 160, 0);
                CB->play_anim(1, 0);
                CB->vire_col(14, 0);
                CB->evenement(36, 2500);
                return;
            } else {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                ST(0xF48) = 1;
                /* 0xD354 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG02OUV15");
                }
                /* 0xD371 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG03OUV15");
                }
                CB->animation("ar_50");
                CB->play_anim(1, 0);
                CB->vire_col(14, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD3F8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD13", P_ARKHAN2, "SG00OUV13", "SG00DIA02");
        }
        /* 0xD423 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD14", P_ARKHAN3, "SG00OUV13", "SG00DIA02");
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 - sub_1000D470 */
static void trig14(void) {
    /* 0xD47F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(14) && ST(0xC28) != 10 && ST(ST_VIE(P_MALROT6)) != VIE_DEAD) {
        /* 0xD4D7 */
        if (ST(0xF28) == 1) {
            CB->select_pile(2, 1);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT6, 1);
            CB->dialogue("L32D102", P_MALROT6);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0xD54A */
        if (ST(0xF28) == 2 && ST(V_ENGAGED_PERSONA) == 0) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->select_pile(2, 1);
            /* 0xD592 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xD5A4 */
                if (ST(ST_PLAYER_WEAPON) != OB_AX67 && ST(ST_PLAYER_WEAPON) != OB_STILETTO && ST(ST_PLAYER_WEAPON) != OB_DAGGER63 && ST(ST_PLAYER_WEAPON) != 5 && ST(ST_PLAYER_WEAPON) != OB_HAMMER && ST(ST_PLAYER_WEAPON) != OB_DAGGER91 && ST(ST_PLAYER_WEAPON) != OB_LONG_DAGGER && ST(V_WEAPON_WARNED) == 0) {
                    CB->joindre(-ST(ST_HERO), 5354, 7576, 0xFEC4, -1);
                    /* 0xD65A */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L32D200", P_ARKHAN2, "SG00DIA01", NULL);
                        lieu_action_register("L32D201", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                        lieu_action_register("L32D202", P_ARKHAN2, "SG00OUV13", NULL);
                        lieu_action_register("L32D203", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    }
                    /* 0xD6CA */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L32D204", P_ARKHAN3, "SG00DIA01", NULL);
                        lieu_action_register("L32D201", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                        lieu_action_register("L32D205", P_ARKHAN3, "SG00OUV13", NULL);
                        lieu_action_register("L32D203", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    }
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    ST(V_WEAPON_WARNED) = 1;
                    return;
                }
                /* 0xD77F */
                if (ST(ST_PLAYER_WEAPON) != OB_AX67 && ST(ST_PLAYER_WEAPON) != OB_STILETTO && ST(ST_PLAYER_WEAPON) != OB_DAGGER63 && ST(ST_PLAYER_WEAPON) != 5 && ST(ST_PLAYER_WEAPON) != OB_HAMMER && ST(ST_PLAYER_WEAPON) != OB_DAGGER91 && ST(ST_PLAYER_WEAPON) != OB_LONG_DAGGER && ST(V_WEAPON_WARNED) == 1) {
                    CB->joindre(-ST(ST_HERO), 5354, 7576, 0xFEC4, -1);
                    /* 0xD835 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L32D206", P_ARKHAN2, "SG00DIA01", NULL);
                    }
                    /* 0xD85D */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L32D207", P_ARKHAN3, "SG00DIA01", NULL);
                    }
                    ST(V_DLG_RANDOM) = CB->random(2);
                    /* 0xD89C */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L32D208", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    }
                    /* 0xD8C7 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L32D209", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    }
                    CB->animation("SG00OUV13");
                    CB->animation("P03_65535");
                    /* 0xD908 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L32D210", P_ARKHAN2, "SG00DIA01", NULL);
                    }
                    /* 0xD930 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L32D211", P_ARKHAN3, "SG00DIA01", NULL);
                    }
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                } else {
                    ST(0xF24) = 4;
                    ST(0xC28) = 1;
                    ST(0x70) = 2;
                    ST(0xF0) = 99;
                    ST(0xB70) = 2;
                    CB->joindre(-ST(ST_HERO), 5354, 7576, 0xFEC4, -1);
                    CB->change_camera(0, CAM_LOCKED);
                    /* 0xDA0E */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L32D212", P_ARKHAN2, "SG00DIA01", "SP02ATT32");
                    }
                    /* 0xDA39 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L32D213", P_ARKHAN3, "SG00DIA01", "SP03ATT32");
                    }
                    CB->evenement(33, 1000);
                    CB->animation("ar_50");
                    CB->joindre(-ST(ST_HERO), 5510, 7510, 0x2FB0, -1);
                    lieu_action_register("L32D214", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    CB->animation("SP66ATT04");
                    CB->animation("P66ARR01");
                    CB->joindre(P_MALROT6, 5453, 7519, 0xABE0, -1);
                    CB->animation("SP66ARR01");
                    CB->joindre(P_MALROT6, 0, 0, 0, ST(ST_HERO));
                    CB->divers(DV_RUN, P_MALROT6, 1);
                    lieu_action_register("L32D217A", P_MALROT6, "SP66DIA56", "SP66DIA61");
                    lieu_action_register("L32D217B", P_MALROT6, "SP66DIA61", "SP66ARR01");
                    lieu_action_register("L32D217C", P_MALROT6, "SP66DIA61", "SP66ARR01");
                    CB->joindre(P_MALROT6, 4174, 6960, -1, -1);
                    CB->change_camera(99, CAM_AUTO);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(0xF0), 0);
                    CB->select_pile(0, 1);
                    return;
                }
            }
            /* 0xDBCF */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xDBE1 */
                if (ST(0x114) == 0) {
                    /* 0xDBF3 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L32D218", P_ARKHAN2, "SG00DIA01", NULL);
                    }
                    /* 0xDC1B */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L32D219", P_ARKHAN3, "SG00DIA01", NULL);
                    }
                    lieu_action_register("L32D220A", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    lieu_action_register("L32D220B", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    ST(0x114) = 1;
                } else {
                    /* 0xDC89 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L32D221", P_ARKHAN2, "SG00DIA01", NULL);
                    }
                    /* 0xDCB1 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L32D222", P_ARKHAN3, "SG00DIA01", NULL);
                    }
                    /* 0xDCD9 */
                    if (ST(0x114) == 1) {
                        lieu_action_register("L32D223", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    }
                    /* 0xDD04 */
                    if (ST(0x114) > 1) {
                        lieu_action_register("L32D224", P_MALROT6, "SP66DIA54", "SP66ATT02_100");
                    }
                    ST(0x114)++;
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
        }
    }
    /* 0xDD6D */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(14) && ST(ST_VIE(P_MALROT6)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDDE5 */
        if (ST(0x88) == 0) {
            ST(0x88) = 1;
            CB->dialogue("L32D512", P_MALROT6);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xDE3F */
        if (ST(0x88) == 1) {
            CB->dialogue("L32D513", P_MALROT6);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 15 - sub_1000DE83 */
static void trig15(void) {
    /* 0xDE92 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE3)) != VIE_DEAD) {
        /* 0xDEE3 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("LCOD20", P_ARKHAN2, "SP02ATT51", NULL);
        }
        /* 0xDF1D */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD61", P_ARKHAN2);
        }
        /* 0xDF51 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("LCOD21", P_ARKHAN3, "SP03ATT51", NULL);
        }
        /* 0xDF8B */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD63", P_ARKHAN3);
        }
        CB->dialogue("L32D888", P_KONKALITE3);
        return;
    }
    /* 0xDFD1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_KONKALITE3)) != VIE_DEAD) {
        CB->dialogue("L32D889", P_KONKALITE3);
    }
    /* 0xE02F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE3)) == VIE_DEAD) {
        CB->fouille(P_KONKALITE3);
        return;
    }
}

/* trigger 16 - sub_1000E089 */
static void trig16(void) {
    /* 0xE09B */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KONKALITE11) && ST(ST_VIE(P_KONKALITE11)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L32D515", P_KONKALITE11);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xE146 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
        /* 0xE197 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("LCOD20", P_ARKHAN2, "SP02ATT51", NULL);
        }
        /* 0xE1D1 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD61", P_ARKHAN2);
        }
        /* 0xE205 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("LCOD21", P_ARKHAN3, "SP03ATT51", NULL);
        }
        /* 0xE23F */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD63", P_ARKHAN3);
        }
        CB->dialogue("L32D514", P_KONKALITE11);
        return;
    }
    /* 0xE285 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE11)) == VIE_DEAD) {
        CB->fouille(P_KONKALITE11);
        return;
    }
}

/* trigger 19 - sub_1000E2F5 */
static void trig19(void) {
    /* 0xE304 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(19) && CB->distance(1, 19) < 150) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE388 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L32D60A", P_ARKHAN2);
            lieu_action_register("L32D60B", P_ARKHAN2, "SG02DIA02", NULL);
        }
        /* 0xE3C0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L32D61A", P_ARKHAN3);
            lieu_action_register("L32D61B", P_ARKHAN3, "SG03DIA02", NULL);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 20 - sub_1000E41A */
static void trig20(void) {
    /* 0xE429 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(20) && ST(0x604) == 0 && (ST(ST_VIE(29)) == VIE_DEAD || CB->divers(DV_PERSONA_PRESENT, 29, 0))) {
        CB->bloque(BLOQUE_ALL);
        /* 0xE4B5 */
        if (ST(ST_VIE(29)) != VIE_DEAD) {
            CB->combat(29, -1);
            CB->animation("SP29GAR01");
        }
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x604) = 1;
        /* 0xE4FD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02PRE08");
        }
        /* 0xE51A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03PRE08");
        }
        CB->etat_objet("OB58", -1, -1, -1, -1, -1, -1, -4, 0);
        CB->etat_objet("OB35", -1, -1, -1, -1, -1, -1, -4, 0);
        CB->etat_objet("OB73A", -1, -1, -1, -1, -1, -1, -3, 0);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->combat(29, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xE5C4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(20) && ST(0x604) == 1) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE630 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L32D58", P_ARKHAN2, "SG02PRE09", NULL);
        }
        /* 0xE658 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L32D59", P_ARKHAN3, "SG03PRE09", NULL);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 21 - sub_1000E6A2 */
static void trig21(void) {
    /* 0xE6B1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(21)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE71A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L32D58", P_ARKHAN2, "SG02PRE09", NULL);
        }
        /* 0xE742 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L32D59", P_ARKHAN3, "SG03PRE09", NULL);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 22 - sub_1000E78C */
static void trig22(void) {
    /* 0xE79B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x88B8)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE804 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xE816 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD07", P_ARKHAN2, "SG00DIA07", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xE85C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xE876 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
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
        /* 0xE8D7 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xE8E9 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD08", P_ARKHAN3, "SG00DIA08", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xE92F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xE949 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
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

/* trigger 23 - sub_1000E9A3 */
static void trig23(void) {
    /* 0xE9B2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x9C40)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xEA0C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG02PRE03", NULL);
        }
        /* 0xEA34 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG03PRE03", NULL);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 24 - sub_1000EA7E */
static void trig24(void) {
    /* 0xEA8D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(24)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xEAE7 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L32D56", P_ARKHAN2);
        }
        /* 0xEB09 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L32D57", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 30 - sub_1000EB84 */
static void trig30(void) {
    /* 0xEB93 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(30)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xEBFC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L32D54", P_ARKHAN2);
        }
        /* 0xEC1E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L32D55", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 41 - sub_10008347 */
static void trig41(void) {
    /* 0x8359 */
    if (ST(0xE8C) == 2 && ST(0xC38) == 0 && ST(ST_VIE(P_GORK)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = P_GORK;
        CB->combat(P_GORK, 0);
    }
    /* 0x83A8 */
    if (ST(0xE8C) == 2 && ST(0xC38) == 1 && ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = P_KONKALITE11;
        CB->combat(P_KONKALITE11, 0);
    }
    /* 0x83F7 */
    if (ST(0xE8C) == 1) {
        ST(0xE8C) = 2;
        CB->bloque(BLOQUE_ALL);
        /* 0x8423 */
        if (ST(0xC38) == 0) {
            ST(0x64) = 20;
            CB->select_pile(2, 1);
            CB->joindre(-P_GORK, 5292, 7089, 0x2A66, -1);
            CB->select_pile(1, 1);
            CB->joindre(-P_SORDOS, 0, 0, 0, P_GORK);
            lieu_action_register("L32D08", P_SORDOS, "SP23DIA54", "SP23ARR03");
            CB->evenement(0, 2000);
            CB->select_pile(0, 1);
        } else {
            CB->select_pile(2, 1);
            CB->joindre(-P_KONKALITE11, 5292, 7089, 0x2A66, -1);
            CB->select_pile(1, 1);
            CB->joindre(-P_SORDOS, 0, 0, 0, P_KONKALITE11);
            lieu_action_register("L32D70", P_SORDOS, "SP23DIA53", "SP23ARR03");
            CB->evenement(0, 2000);
            CB->select_pile(0, 1);
        }
    }
    /* 0x854A */
    if (ST(0xF00) == 3) {
        ST(V_ENGAGED_PERSONA) = 3177;   /* P_GORK * 100 + P_KONKALITE7 */
        CB->select_pile(2, 1);
        CB->combat(P_KONKALITE7, 0);
        CB->select_pile(1, 1);
        CB->combat(P_GORK, 0);
        CB->select_pile(0, 1);
    }
    /* 0x85AF */
    if (ST(0xF28) == 1) {
        ST(V_ENGAGED_PERSONA) = P_KONKALITE3;
        CB->combat(P_KONKALITE3, 0);
    }
    /* 0x85DA */
    if (ST(0xF24) == 5 && ST(0xD80) == 1 && (ST(0xBB0) == 2 || ST(0xEF8) == 10) && ST(ST_VIE(P_GORK)) != VIE_DEAD && !CB->divers(DV_PERSONA_PRESENT, P_SORDOS, 0)) {
        ST(V_ENGAGED_PERSONA) = P_GORK;
        CB->combat(P_GORK, 0);
    }
    /* 0x8667 */
    if (ST(0xF24) == 5 && ST(0xD80) == 0 && (ST(0xBB0) == 2 || ST(0xEF8) == 10) && ST(ST_VIE(P_GORK)) != VIE_DEAD && !CB->divers(DV_PERSONA_PRESENT, P_SORDOS, 0)) {
        ST(0xD80) = 1;
        ST(V_ENGAGED_PERSONA) = P_GORK;
        CB->combat(P_GORK, 0);
        CB->dialogue("L32D12B", P_GORK);
    }
    /* 0x8713 */
    if (ST(0xF24) == 6 && ST(ST_VIE(29)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 29;
        CB->combat(29, 0);
    }
}
