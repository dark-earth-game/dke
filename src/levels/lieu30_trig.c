/*
 * lieu30_trig.c - Native hand-port of the LIEU30.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu30.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * Triggers 15..27 are thirteen copies of one handler that differ only in the
 * zone they test (zone_talk below).
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu30_int.h"

static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig10(void);
static void trig15(void); static void trig16(void); static void trig17(void);
static void trig18(void); static void trig19(void); static void trig20(void);
static void trig21(void); static void trig22(void); static void trig23(void);
static void trig24(void); static void trig25(void); static void trig26(void);
static void trig27(void); static void trig28(void); static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* trigger table at 0x1001C8C0 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 100073F0   1 100093E9 stub   2 100093F4   3 10009612   4 10009830
 *   5 10009917   6 10009ADF   7..9 stubs   10 1000A0BB   11..14 stubs
 *  15 1000A2F6  16 1000A48C  17 1000A622  18 1000A7B8  19 1000A94E
 *  20 1000AAE4  21 1000AC7A  22 1000AE10  23 1000AFA6  24 1000B13C
 *  25 1000B2D2  26 1000B468  27 1000B5FE  28 1000B794
 *  29..40 stubs (1000B86C..1000B8E5)  41 10007EF1
 */
static void (*const g_trigger[42])(void) = {
    l30_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig_noop,
    trig_noop, trig_noop, trig10, trig_noop, trig_noop, trig_noop, trig_noop,
    trig15, trig16, trig17, trig18, trig19, trig20, trig21, trig22, trig23,
    trig24, trig25, trig26, trig27, trig28, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001C8C0]). */
void l30_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* One ambient creature of trigger 0's block: the original evaluates
 * random(65535) first, then random(200). */
static void place_ambient(i32 x, i32 z) {
    i32 a = CB->random(65535);
    i32 y = CB->random(200) - 2000;
    CB->place_animal(5, x, y, z, a, 1000);
}

/* trigger 0 (level setup) - sub_100073F0 */
void l30_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(30)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xAF8) = 1;
    }
    /* 0x7450 */
    if (ST(ST_START_POINT(30)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(0xE98) = 65;
        ST(ST_VIE(P_KONKALITE2)) = VIE_DEAD;
    }
    /* 0x74B9 */
    if (ST(ST_START_POINT(30)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 35;
        ST(V_SOB86_ACTIVE) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(0xE98) = 65;
        ST(ST_VIE(P_KONKALITE2)) = VIE_DEAD;
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
    CB->divers(DV_MUSIC, 1, 0);
    CB->vire_col(39, 0);
    CB->ajoute_zone(4, 4192, 5850, 105, 160, 1);
    CB->ajoute_zone(5, 1300, 4100, 50, 770, 1);
    CB->ajoute_zone(6, 4502, 5835, 132, 165, 0);
    CB->ajoute_zone(15, 1500, 4410, 190, 208, 0);
    CB->ajoute_zone(16, 1714, 4466, 250, 170, 0);
    CB->ajoute_zone(17, 2033, 4583, 273, 217, 0);
    CB->ajoute_zone(18, 1939, 4106, 310, 205, 0);
    CB->ajoute_zone(19, 3027, 4116, 260, 204, 0);
    CB->ajoute_zone(20, 2638, 3974, 236, 183, 0);
    CB->ajoute_zone(21, 2846, 3701, 230, 221, 0);
    CB->ajoute_zone(22, 3728, 3697, 325, 245, 0);
    CB->ajoute_zone(23, 3182, 3500, 220, 240, 0);
    CB->ajoute_zone(24, 4926, 3969, 200, 180, 0);
    CB->ajoute_zone(25, 4705, 3459, 190, 155, 0);
    CB->ajoute_zone(26, 5335, 4158, 250, 230, 0);
    CB->ajoute_zone(27, 4983, 4225, 225, 220, 0);
    CB->ajoute_zone(28, 4829, 5180, 145, 145, 0);
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    /* 0x7835 */
    if (ST(ST_LEVEL) == 31) {
        CB->pos_perso(ST(ST_HERO), 4380, 5898, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x7883 */
    if (ST(ST_LEVEL) == 48) {
        CB->pos_perso(ST(ST_HERO), 1442, 4164, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x78D1 */
    if (ST(ST_LEVEL) == 35) {
        /* 0x78E3 */
        if (ST(0xC58) == 1) {
            ST(0xC58) = 0;
            ST(V_HUD_TIMER_MS) = 0;
        }
        CB->pos_perso(ST(ST_HERO), 4462, 5820, 0x4EB4, 1, "ar", 1, 0);
        /* 0x793E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(98, 4462, 5820, 0x4EB4, 1, "ar", 1, 0);
        }
        /* 0x7975 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(99, 4462, 5820, 0x4EB4, 1, "ar", 1, 0);
        }
    }
    /* 0x79AC */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xE98) != 65) {
        ST(0xE98) = 65;
        ST(ST_VIE(P_KONKALITE2)) = VIE_DEAD;
        ST(ST_VIE(P_KONKALITE5)) = VIE_DEAD;
        ST(ST_VIE(94)) = VIE_DEAD;
    }
    /* 0x7A0C */
    if (ST(0xE98) == 60) {
        CB->pos_perso(P_KONKALITE2, 0, 0, 0, 2, "P72CAD", 0xA, 0);
    }
    /* 0x7A3A */
    if (ST(0xE98) == 0 || ST(0xE98) == 1) {
        ST(0xE98) = 1;
        CB->pos_perso(P_KONKALITE5, 2792, 3942, 0x131B, 1, "P75ARR01", 2, 0);
        CB->pos_perso(94, 5207, 3824, 0x1628, 1, "P94ARR03", 3, 0);
    } else {
        /* 0x7ABC */
        if (ST(0xE98) == 2 && ST(ST_LEVEL) == 48) {
            /* 0x7ADD */
            if (ST(ST_VIE(P_KONKALITE5)) != VIE_DEAD) {
                ST(V_ENGAGED_PERSONA) = P_KONKALITE5;
                CB->pos_perso(P_KONKALITE5, 1668, 4398, 0x5FB0, 1, "P75GAR03", 2, 0);
            }
            /* 0x7B20 */
            if (ST(ST_VIE(94)) != VIE_DEAD) {
                /* 0x7B2F */
                if (ST(V_ENGAGED_PERSONA) == P_KONKALITE5) {
                    ST(V_ENGAGED_PERSONA) = 0x1DAA;
                } else {
                    ST(V_ENGAGED_PERSONA) = 94;
                }
                CB->pos_perso(94, 1645, 4186, 0x44CA, 1, "P94GAR03", 3, 0);
            }
        }
        /* 0x7B83 */
        if ((ST(0xE98) == 20 || ST(0xE98) == 21 || ST(0xE98) == 22 || (ST(ST_VIE(P_KONKALITE2)) == VIE_DEAD && ST(0xE98) != 65)) && ST(ST_LEVEL) != 48) {
            ST(0x8C4) = 2;
            CB->pos_perso(P_KONKALITE2, 0, 0, 0, 2, "P72CAD", 0xA, 0);
        }
        /* 0x7C17 */
        if (ST(0xE98) == 56 && ST(0xDB8) == 1) {
            CB->pos_perso(P_KONKALITE2, 0, 0, 0, 1, "P72GAR03", 0xA, 0);
        }
    }
    CB->bruitage("L30AMB1", -1, -1);
    place_ambient(1429, 4162);
    place_ambient(1978, 4324);
    place_ambient(2751, 4095);
    place_ambient(3630, 3495);
    place_ambient(5160, 4265);
    place_ambient(5080, 3904);
    place_ambient(4366, 3597);
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
    ST(0xB8) = 0;
    ST(0xBC) = 0;
    ST(0xE0) = 0;
    ST(0x100) = 0;
    /* 0x7EB6 */
    if (ST(0x4DC) == 1 && ST(0xF24) != 1) {
        ST(0x4DC) = 3;
    }
    ST(ST_START_POINT(30)) = 0;
}

/* trigger 41 - sub_10007EF1 */
static void trig41(void) {
    CB->divers(DV_MUSIC, 1, 1);
    /* 0x7F15 */
    if (ST(0xE98) == 2 && ST(ST_LEVEL) == 48) {
        /* 0x7F36 */
        if (ST(ST_VIE(P_KONKALITE5)) != VIE_DEAD) {
            CB->combat(P_KONKALITE5, 0);
        }
        /* 0x7F55 */
        if (ST(ST_VIE(94)) != VIE_DEAD) {
            CB->combat(94, 0);
        }
    }
    /* 0x7F74 */
    if (ST(0xE98) == 56 && ST(ST_LEVEL) == 48 && ST(0xDB8) == 1 && ST(ST_VIE(P_KONKALITE2)) != VIE_DEAD) {
        CB->combat(P_KONKALITE2, 1);
    }
    /* 0x7FC9 */
    if (ST(V_SOB86_ACTIVE) == 1) {
        CB->bloque(BLOQUE_ALL);
        ST(V_SOB86_ACTIVE) = 0;
        /* 0x7FF5 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L30D16", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x801D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L30D17", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->divers_empile(DV_SCREEN, 5, 0);
        /* 0x8062 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(P_ARKHAN2, CB->divers(DV_DESC_POS, P_ARKHAN2, 0), CB->divers(DV_DESC_POS, P_ARKHAN2, 1), CB->divers(DV_DESC_POS, P_ARKHAN2, 2), 0, "ar", 1, 0);
        }
        /* 0x80C4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(P_ARKHAN3, CB->divers(DV_DESC_POS, P_ARKHAN3, 0), CB->divers(DV_DESC_POS, P_ARKHAN3, 1), CB->divers(DV_DESC_POS, P_ARKHAN3, 2), 0, "ar", 1, 0);
        }
        CB->divers_empile(DV_SCREEN, 3, 0);
        /* 0x8138 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD668", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x8160 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD669", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 2 (Konkalite5 talk / search) - sub_100093F4 */
static void trig02(void) {
    /* 0x9403 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE5)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x946B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x947D */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0x949F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD62", P_ARKHAN2);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x94EA */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x94FC */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
            /* 0x951E */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD63", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x955D - empty conditional in the original (every branch of the compare chain
     * jumps to the same address): ACTION_KEY == 1 && SCRIPT_BUSY == 0 &&
     * ENGAGED_PERSONA != 0 && COMBAT_STANCE == 1 && VIE != VIE_DEAD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_KONKALITE5)) != VIE_DEAD) {
        /* no body */
    }
    /* 0x9566 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_KONKALITE5)) == VIE_DEAD && CB->direction_zone(2)) {
        CB->fouille(P_KONKALITE5);
        return;
    }
}

/* trigger 3 (Konkalite 94 talk / search) - sub_10009612 */
static void trig03(void) {
    /* 0x9621 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(94)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9689 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x969B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
            /* 0x96BD */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD61", P_ARKHAN2);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9708 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x971A */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD21", P_ARKHAN3);
            }
            /* 0x973C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD63", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x977B - empty conditional in the original (every branch of the compare chain
     * jumps to the same address): ACTION_KEY == 1 && SCRIPT_BUSY == 0 &&
     * ENGAGED_PERSONA != 0 && COMBAT_STANCE == 1 && VIE != VIE_DEAD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(94)) != VIE_DEAD) {
        /* no body */
    }
    /* 0x9784 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(94)) == VIE_DEAD && CB->direction_zone(3)) {
        CB->fouille(94);
        return;
    }
}

/* trigger 4 (exit to level 31) - sub_10009830 */
static void trig04(void) {
    ST(ST_LEVEL) = 30;
    /* 0x984E */
    if (ST(0xE98) == 1) {
        /* 0x9860 */
        if (ST(0x914) != 2 && ST(0x904) != 2) {
            ST(0xE98) = 20;
            ST(0xDBC) = 1;
        } else if (ST(0x914) != 2) {
            ST(0xE98) = 21;
        } else {
            ST(0xE98) = 22;
        }
        ST(0xDBC) = 1;
    }
    /* 0x98EB */
    if (ST(0xE98) == 56) {
        ST(0xE98) = 60;
    }
    CB->change_piece(31, 0);
    return;
}

/* trigger 5 (exit to level 48) - sub_10009917 */
static void trig05(void) {
    CB->retire_zone(5);
    /* 0x9934 */
    if (ST(0xE98) > 0 && ST(0xE98) < 60) {
        /* 0x9958 */
        if (ST(0xE98) == 56 && ST(ST_VIE(P_KONKALITE2)) != VIE_DEAD) {
            CB->divers(DV_MUSIC, 1, 4);
            ST(ST_LEVEL) = 30;
            ST(V_ENGAGED_PERSONA) = 0;
            CB->change_piece(48, 0);
            return;
        }
        /* 0x99B8 */
        if (ST(0xE98) == 56 && ST(ST_VIE(P_KONKALITE2)) == VIE_DEAD) {
            CB->divers(DV_MUSIC, 1, 4);
            ST(0xE98) = 60;
            ST(ST_LEVEL) = 30;
            ST(V_ENGAGED_PERSONA) = 0;
            CB->change_piece(48, 0);
            return;
        }
        /* 0x9A27 */
        if (ST(ST_VIE(P_KONKALITE5)) != VIE_DEAD || ST(ST_VIE(94)) != VIE_DEAD) {
            CB->divers(DV_MUSIC, 1, 4);
            /* 0x9A5A */
            if (ST(V_ENGAGED_PERSONA) != 0) {
                ST(0xE98) = 2;
                ST(V_ENGAGED_PERSONA) = 0;
            }
            ST(ST_LEVEL) = 30;
            ST(V_ENGAGED_PERSONA) = 0;
            CB->change_piece(48, 0);
            return;
        }
        return;
    }
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 30;
    CB->change_piece(48, 0);
    return;
}

/* trigger 6 (exit zone, Konkalite2 / 31D83) - sub_10009ADF */
static void trig06(void) {
    /* 0x9AEE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_KONKALITE2)) == VIE_DEAD && CB->direction_spec(0xCF08) && ST(ST_COMBAT_STANCE) == 0) {
        /* 0x9B43 */
        if (ST(V_SOB86_ACTIVE) == 1) {
            CB->vire_col(36, 0);
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x9B79 */
            if (ST(V_HUD_TIMER_MS) == 0) {
                ST(V_SOB86_ACTIVE) = 0;
                CB->vire_col(36, 1);
                /* 0x9BAA */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L31D83A", P_ARKHAN2);
                    CB->dialogue("L31D83B", P_ARKHAN2);
                    /* 0x9BDC */
                    if (ST(0xC50) == 0) {
                        CB->dialogue("L31D83C", P_ARKHAN2);
                    }
                }
                /* 0x9BFE */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L31D84A", P_ARKHAN3);
                    CB->dialogue("L31D84B", P_ARKHAN3);
                    /* 0x9C30 */
                    if (ST(0xC50) == 0) {
                        CB->dialogue("L31D84C", P_ARKHAN3);
                    }
                }
                CB->animation("Sar_50");
                CB->divers_empile(DV_SCREEN, 5, 0);
                /* 0x9C6F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(P_ARKHAN2, CB->divers(DV_DESC_POS, P_ARKHAN2, 0), CB->divers(DV_DESC_POS, P_ARKHAN2, 1), CB->divers(DV_DESC_POS, P_ARKHAN2, 2), 0, "ar", 1, 0);
                }
                /* 0x9CD1 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(P_ARKHAN3, CB->divers(DV_DESC_POS, P_ARKHAN3, 0), CB->divers(DV_DESC_POS, P_ARKHAN3, 1), CB->divers(DV_DESC_POS, P_ARKHAN3, 2), 0, "ar", 1, 0);
                }
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            ST(ST_LEVEL) = 30;
            /* 0x9D7A */
            if (ST(0xE98) == 1) {
                /* 0x9D8C */
                if (ST(0x914) != 2 && ST(0x904) != 2) {
                    ST(0xE98) = 20;
                    ST(0xDBC) = 1;
                } else if (ST(0x914) != 2) {
                    ST(0xE98) = 21;
                } else {
                    ST(0xE98) = 22;
                }
                ST(0xDBC) = 1;
            }
            /* 0x9E17 */
            if (ST(0xE98) == 56) {
                ST(0xE98) = 60;
            }
            /* 0x9E40 */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x9E52 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02PRE73");
                }
                /* 0x9E6F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03PRE73");
                }
                CB->animation("ar");
            }
            CB->joindre(ST(ST_HERO), 4555, 5830, 0x4EB6, -1);
            CB->animation("Sar_50");
            /* 0x9EC4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(98, 4555, 5830, 0x4EB6, 1, "ar", 1, 0);
                CB->animation("SP02MAR90");
                CB->animation("P02MAR90");
                ST(0x100) = 1;
            }
            /* 0x9F20 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(99, 4555, 5830, 0x4EB6, 1, "ar", 1, 0);
                CB->animation("SP03MAR90");
                CB->animation("P03MAR90");
                ST(0x100) = 1;
            }
            return;
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9F98 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
            CB->dialogue("L31D85A", P_ARKHAN2);
            CB->dialogue("L31D85B", P_ARKHAN2);
        }
        /* 0x9FDC */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("L31D86", P_ARKHAN2);
        }
        /* 0xA010 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            CB->dialogue("L31D87", P_ARKHAN3);
        }
        /* 0xA044 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("L31D88", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 10 (Konkalite2 talk / search) - sub_1000A0BB */
static void trig10(void) {
    /* 0xA0CA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE2)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA132 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA144 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L30D11", P_ARKHAN2);
            }
            /* 0xA166 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L30D12", P_ARKHAN2);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA1B1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xA1C3 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L30D13", P_ARKHAN3);
            }
            /* 0xA1E5 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD22A", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xA224 - empty conditional in the original (every branch of the compare chain
     * jumps to the same address): ACTION_KEY == 1 && SCRIPT_BUSY == 0 &&
     * ENGAGED_PERSONA != 0 && COMBAT_STANCE == 1 && VIE != VIE_DEAD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_KONKALITE2)) != VIE_DEAD) {
        /* no body */
    }
    /* 0xA22D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_KONKALITE2)) == VIE_DEAD && CB->direction_zone(10)) {
        CB->fouille(P_KONKALITE2);
        return;
    }
}

/* triggers 15..27 - the shared body of sub_1000A2F6 .. sub_1000B5FE (each
 * copy tests its own zone number). */
static void zone_talk(i32 zone) {
    /* 0xA305 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(zone)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA35F */
        if (ST(V_EVENT15_STEP) == 0) {
            ST(V_EVENT15_STEP) = 1;
            /* 0xA380 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L30D07", P_ARKHAN2, "SG02PRE03", NULL);
            }
            /* 0xA3A8 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L30D08", P_ARKHAN3, "SG03PRE03", NULL);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA3F9 */
        if (ST(V_EVENT15_STEP) == 1) {
            ST(V_EVENT15_STEP) = 0;
            /* 0xA41A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L30D09", P_ARKHAN2, "SG02PRE01", NULL);
            }
            /* 0xA442 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L30D10", P_ARKHAN3, "SG03PRE01", NULL);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 15 - sub_1000A2F6 */
static void trig15(void) {
    zone_talk(15);
}

/* trigger 16 - sub_1000A48C */
static void trig16(void) {
    zone_talk(16);
}

/* trigger 17 - sub_1000A622 */
static void trig17(void) {
    zone_talk(17);
}

/* trigger 18 - sub_1000A7B8 */
static void trig18(void) {
    zone_talk(18);
}

/* trigger 19 - sub_1000A94E */
static void trig19(void) {
    zone_talk(19);
}

/* trigger 20 - sub_1000AAE4 */
static void trig20(void) {
    zone_talk(20);
}

/* trigger 21 - sub_1000AC7A */
static void trig21(void) {
    zone_talk(21);
}

/* trigger 22 - sub_1000AE10 */
static void trig22(void) {
    zone_talk(22);
}

/* trigger 23 - sub_1000AFA6 */
static void trig23(void) {
    zone_talk(23);
}

/* trigger 24 - sub_1000B13C */
static void trig24(void) {
    zone_talk(24);
}

/* trigger 25 - sub_1000B2D2 */
static void trig25(void) {
    zone_talk(25);
}

/* trigger 26 - sub_1000B468 */
static void trig26(void) {
    zone_talk(26);
}

/* trigger 27 - sub_1000B5FE */
static void trig27(void) {
    zone_talk(27);
}

/* trigger 28 (save point) - sub_1000B794 */
static void trig28(void) {
    /* 0xB7A3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x88B8)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xB7F1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", NULL);
        }
        /* 0xB819 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", NULL);
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

