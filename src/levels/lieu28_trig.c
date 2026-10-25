/*
 * lieu28_trig.c - Native hand-port of the LIEU28.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu28.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * Slots 1, 3, 8(partly), 11, 13..40 are the shared L01 stub body.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu28_int.h"

static void trig02(void); static void trig04(void); static void trig05(void);
static void trig06(void); static void trig07(void); static void trig08(void);
static void trig09(void); static void trig10(void); static void trig12(void);
static void trig41(void);
static void trig_noop(void);

/* trigger table at 0x1001FCC0 (slots 1, 3, 11, 13..40 are empty stubs). */
static void (*const g_trigger[42])(void) = {
    l28_trig00, trig_noop, trig02, trig_noop, trig04, trig05, trig06, trig07,
    trig08, trig09, trig10, trig_noop, trig12, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001FCC0]). */
void l28_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - sub_100073F0 */
void l28_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(28)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 27;
        ST(0xEB0) = 1;
    }
    /* 0x743E */
    if (ST(ST_START_POINT(28)) == 2) {
        ST(0xB38) = 1;
        ST(ST_LEVEL) = 27;
    }
    /* 0x746B */
    if (ST(ST_START_POINT(28)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 27;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xB38) = 3;
    }
    ST(ST_START_POINT(28)) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    /* 0x74DD */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x7501 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 || ST(0xB38) == 1) {
        CB->divers(DV_MUSIC, 2, 0);
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
    CB->ajoute_zone(4, 8262, -5004, 247, 112, 0);
    CB->ajoute_zone(5, 8176, -4795, 90, 80, 0);
    CB->ajoute_zone(6, 8068, -4789, 95, 80, 0);
    CB->ajoute_zone(7, 7564, -4939, 330, 155, 0);
    CB->ajoute_zone(8, 7552, -5051, 180, 75, 0);
    CB->ajoute_zone(9, 8258, -4901, 235, 85, 0);
    CB->ajoute_zone(10, 7553, -4823, 110, 150, 0);
    CB->ajoute_zone(11, 7577, -4984, 50, 50, 0);
    CB->ajoute_zone(12, 7798, -4828, 100, 150, 0);
    CB->pos_perso(ST(ST_HERO), 7595, -4840, 0xB6B4, 1, "ar", 1, 0);
    /* 0x76DC */
    if (ST(ST_VIE(P_ARCHIVIST)) != VIE_DEAD) {
        CB->pos_perso(P_ARCHIVIST, 7890, -4827, 0xEA60, 1, "P05LIR01", 2, 500);
    }
    /* 0x7716 */
    if (ST(ST_VIE(P_ARCHIVIST)) == VIE_DEAD) {
        CB->pos_perso(P_ARCHIVIST, 0, 0, 0, 2, "P05cad", 2, 0);
    }
    /* 0x7744 */
    if (ST(0xB38) >= 1 && ST(ST_VIE(P_ARCHIVIST)) != VIE_DEAD) {
        CB->pos_perso(P_ARCHIVIST, 7976, -5015, 0x2691, 1, "P05ARR01", 2, 500);
    }
}

/* trigger 2 - sub_10008352 */
static void trig02(void) {
    /* 0x8361 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x470) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 12)) {
        trig12();
        return;
    }
    /* 0x83DA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_ARCHIVIST)) {
        /* 0x842C */
        if (ST(ST_VIE(P_ARCHIVIST)) != VIE_DEAD) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x6C) = 0;
            CB->select_pile(1, 1);
            CB->joindre(-P_ARCHIVIST, 0, 0, 0, ST(ST_HERO));
            CB->select_pile(0, 1);
            CB->joindre(ST(ST_HERO), 0, 0, 0, 5);
            /* 0x84B4 */
            if (ST(0xB38) == 0) {
                /* 0x84C6 */
                if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 2)) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0x852D */
                    if (ST(0xEB0) == 0 || ST(0xEB0) == 3) {
                        /* 0x8551 */
                        if (ST(0x4C4) == 0) {
                            /* 0x8563 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0x8575 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D72A", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D72B", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D73A", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                    lieu_action_register("L28D73B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D73C", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D73D", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D74", P_ARKHAN2, "SG02DIA02", NULL);
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 1;
                                    return;
                                }
                                /* 0x8665 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L28D75", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D76A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D76B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 1;
                                    return;
                                }
                            }
                            /* 0x86F7 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0x8709 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D77", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D76A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D76B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 1;
                                    return;
                                }
                                /* 0x879B */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("LCOD22A", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            }
                        }
                        /* 0x881E */
                        if (ST(0x4C4) == 1) {
                            /* 0x8830 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0x8842 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D26", P_ARKHAN2, "SG02DIA02", NULL);
                                    lieu_action_register("L28D27A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D27B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D28", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D29A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D29B", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                    lieu_action_register("L28D29C", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 2;
                                    return;
                                }
                                /* 0x8935 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    ST(0x64) = 4;
                                    lieu_action_register("L28D30", P_ARKHAN2, "SG02DIA02", NULL);
                                    lieu_action_register("L28D27A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D27B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D31", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D29A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D29B", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                    lieu_action_register("L28D29C", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 2;
                                    return;
                                }
                            }
                            /* 0x8A34 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0x8A46 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    ST(0x64) = 4;
                                    lieu_action_register("L28D32", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D27A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D27B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D33", P_ARKHAN3, "SG03DIA05", NULL);
                                    lieu_action_register("L28D29A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D29B", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                    lieu_action_register("L28D29C", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 2;
                                    return;
                                }
                                /* 0x8B45 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("LCOD22C", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            }
                        }
                        /* 0x8BC8 */
                        if (ST(0x4C4) == 2) {
                            /* 0x8BDA */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0x8BEC */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D34", P_ARKHAN2, "SG02DIA06", NULL);
                                    lieu_action_register("L28D35A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D35B", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    lieu_action_register("L28D35C", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                    lieu_action_register("L28D36A", P_ARKHAN2, "SG02DIA03", NULL);
                                    lieu_action_register("L28D36B", P_ARKHAN2, "SG02DIA03", NULL);
                                    lieu_action_register("L28D37A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D37B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D38", P_ARKHAN2, "SG02DIA03", NULL);
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 3;
                                    return;
                                }
                                /* 0x8D0B */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L28D39", P_ARKHAN2, "SG02DIA07", NULL);
                                    lieu_action_register("L28D35A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D35B", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    lieu_action_register("L28D35C", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                    lieu_action_register("L28D40", P_ARKHAN2, "SG02DIA02", NULL);
                                    lieu_action_register("L28D37A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D37B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D41", P_ARKHAN2, "SG02DIA02", NULL);
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 3;
                                    return;
                                }
                            }
                            /* 0x8E14 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0x8E26 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D42", P_ARKHAN3, "SG03DIA07", NULL);
                                    lieu_action_register("L28D35A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D35B", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    lieu_action_register("L28D35C", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                    lieu_action_register("L28D43", P_ARKHAN3, "SG03DIA05", NULL);
                                    lieu_action_register("L28D37A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D37B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D44", P_ARKHAN3, "SG03DIA02", NULL);
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 3;
                                    return;
                                }
                                /* 0x8F2F */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("LCOD22D", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            }
                        }
                        /* 0x8FB2 */
                        if (ST(0x4C4) == 3) {
                            /* 0x8FC4 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0x8FD6 */
                                if (ST(0xEBC) == 0) {
                                    /* 0x8FE8 */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L28D45", P_ARKHAN2, "SG02DIA06", NULL);
                                        lieu_action_register("L28D46", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                    /* 0x9052 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        ST(0xEBC) = 1;
                                        CB->evenement(19, 60000);
                                        lieu_action_register("L28D47", P_ARKHAN2, "SG02DIA05", NULL);
                                        lieu_action_register("L28D48A", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                        lieu_action_register("L28D48B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                        lieu_action_register("L28D48C", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                        lieu_action_register("L28D48D", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                        lieu_action_register("L28D49", P_ARKHAN2, "SG02DIA02", NULL);
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                }
                                /* 0x913C */
                                if (ST(0xEBC) == 1 || ST(0xEBC) == 3) {
                                    /* 0x9160 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("L28D62", P_ARKHAN2, "SG02DIA05", NULL);
                                        lieu_action_register("L28D61", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                    /* 0x91CA */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L28D45", P_ARKHAN2, "SG02DIA06", NULL);
                                        lieu_action_register("L28D46", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                }
                                /* 0x9234 */
                                if (ST(0xEBC) == 2) {
                                    ST(0xEBC) = 3;
                                    lieu_action_register("L28D52", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D53A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D53B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D53C", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                    lieu_action_register("L28D53D", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D54", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D55A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D55B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D55C", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                    lieu_action_register("L28D55D", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D56A", P_ARKHAN2, "SG02DIA02", NULL);
                                    /* 0x935F */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L28D56B", P_ARKHAN2, "SG02DIA02", NULL);
                                    }
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            }
                            /* 0x93B0 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0x93C2 */
                                if (ST(0xEBC) == 0) {
                                    /* 0x93D4 */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        ST(0xEBC) = 1;
                                        CB->evenement(19, 60000);
                                        lieu_action_register("L28D50", P_ARKHAN3, "SG03DIA05", NULL);
                                        lieu_action_register("L28D51A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                        lieu_action_register("L28D51B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                        lieu_action_register("L28D48D", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                    /* 0x948F */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("LCOD22B", P_ARKHAN3, "SG03DIA02", NULL);
                                        lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                        lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                }
                                /* 0x9512 */
                                if (ST(0xEBC) == 1 || ST(0xEBC) == 3) {
                                    /* 0x9536 */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L28D63", P_ARKHAN3, "SG03DIA05", NULL);
                                        lieu_action_register("L28D61", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                    /* 0x95A0 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("LCOD22A", P_ARKHAN3, "SG03DIA02", NULL);
                                        lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                        lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                }
                                /* 0x9623 */
                                if (ST(0xEBC) == 2) {
                                    /* 0x9635 */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        ST(0xEBC) = 3;
                                        lieu_action_register("L28D57", P_ARKHAN3, "SG03DIA05", NULL);
                                        lieu_action_register("L28D53A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                        lieu_action_register("L28D53B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                        lieu_action_register("L28D53C", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                        lieu_action_register("L28D53D", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                        lieu_action_register("L28D58", P_ARKHAN3, "SG03DIA05", NULL);
                                        lieu_action_register("L28D55A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                        lieu_action_register("L28D55B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                        lieu_action_register("L28D55C", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                        lieu_action_register("L28D55D", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                        lieu_action_register("L28D59", P_ARKHAN3, "SG03DIA02", NULL);
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                    /* 0x9789 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("LCOD22D", P_ARKHAN3, "SG03DIA02", NULL);
                                        lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                        lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                        CB->animation("Sar_20");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                }
                            }
                        }
                    }
                    /* 0x980C */
                    if (ST(0xEB0) == 1) {
                        /* 0x981E */
                        if (ST(0x4C4) == 0) {
                            /* 0x9830 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0x9842 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D72A", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D72B", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D73A", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                    lieu_action_register("L28D73B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D73C", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D73D", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D74", P_ARKHAN2, "SG02DIA02", NULL);
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 1;
                                    return;
                                }
                                /* 0x9932 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L28D75", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D76A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D76B", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 1;
                                    return;
                                }
                            }
                            /* 0x99C4 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0x99D6 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D77", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D76A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D76B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0x4C4) = 1;
                                    return;
                                }
                                /* 0x9A68 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("LCOD22A", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            }
                        }
                        /* 0x9AEB */
                        if (ST(0x4C4) >= 1) {
                            /* 0x9AFD */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                ST(0xEB0) = 2;
                                /* 0x9B1E */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D12", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D13A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                                    lieu_action_register("L28D13B", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                                    lieu_action_register("L28D13C", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D14", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D15A", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                    lieu_action_register("L28D15B", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                                /* 0x9C02 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L28D16", P_ARKHAN2, "SG02DIA05", NULL);
                                    lieu_action_register("L28D17A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D17B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D18", P_ARKHAN2, "SG02DIA07", NULL);
                                    lieu_action_register("L28D15A", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                    lieu_action_register("L28D15B", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            }
                            /* 0x9CCD */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0x9CDF */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L28D19", P_ARKHAN3, "SG03DIA05", NULL);
                                    lieu_action_register("L28D17A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D17B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    lieu_action_register("L28D20", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D21A", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                    lieu_action_register("L28D21B", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    ST(0xEB0) = 2;
                                    return;
                                }
                                /* 0x9DB9 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("LCOD22A", P_ARKHAN3, "SG03DIA02", NULL);
                                    lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                    lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                    CB->animation("Sar_20");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            }
                        }
                    }
                    /* 0x9E3C */
                    if (ST(0xEB0) == 2) {
                        /* 0x9E4E */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            /* 0x9E60 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L28D23A", P_ARKHAN2, "SG02DIA06", NULL);
                                lieu_action_register("L28D23B", P_ARKHAN2, "SG02DIA06", NULL);
                                lieu_action_register("L28D24A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                lieu_action_register("L28D24B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xEB0) = 3;
                                return;
                            }
                            /* 0x9F08 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L28D25", P_ARKHAN2, "SG02DIA07", NULL);
                                lieu_action_register("L28D24A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                lieu_action_register("L28D24B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xEB0) = 3;
                                return;
                            }
                        }
                        /* 0x9F9A */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            /* 0x9FAC */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L28D19", P_ARKHAN3, "SG03DIA05", NULL);
                                lieu_action_register("L28D17A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                lieu_action_register("L28D17B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                lieu_action_register("L28D20", P_ARKHAN3, "SG03DIA02", NULL);
                                lieu_action_register("L28D21A", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                                lieu_action_register("L28D21B", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                ST(0xEB0) = 3;
                                return;
                            }
                            /* 0xA086 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("LCOD22C", P_ARKHAN3, "SG03DIA02", NULL);
                                lieu_action_register("L28D22A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                                lieu_action_register("L28D22B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                                CB->animation("Sar_20");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                        }
                    }
                }
                /* 0xA109 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                    /* 0xA14C */
                    if (ST(ST_PLAYER_WEAPON) == OB_METAL_PLATE) {
                        ST(0xB28) = 1;
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0xA181 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L28D64", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L28D65A", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                            lieu_action_register("L28D65B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            lieu_action_register("L28D65C", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D65D", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                            lieu_action_register("L28D65E", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D65F", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xA268 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L28D66", P_ARKHAN3, "SG03DIA05", NULL);
                            lieu_action_register("L28D65A", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                            lieu_action_register("L28D65B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            lieu_action_register("L28D65C", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D65D", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                            lieu_action_register("L28D65E", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D65F", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0xA34F */
                    if (ST(ST_PLAYER_WEAPON) == OB_PARCHMENT) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0xA375 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L28D217", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L28D218A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                            lieu_action_register("L28D218B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D218C", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xA411 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L28D66", P_ARKHAN3, "SG03DIA05", NULL);
                            lieu_action_register("L28D218A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                            lieu_action_register("L28D218B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D218C", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0xA4AD */
                    if (ST(ST_PLAYER_WEAPON) == OB_MIRROR) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0xA4D3 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L28D113", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L28D114A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                            lieu_action_register("L28D114B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D114C", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xA56F */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("LCOD19", P_ARKHAN3, "SG03DIA05", NULL);
                            lieu_action_register("L28D114A", P_ARCHIVIST, "SP05DIA56", "SP05ARR01_10");
                            lieu_action_register("L28D114B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D114C", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    } else {
                        /* 0xA610 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                            lieu_action_register("L28D67", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L28D68", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xA68E */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                            lieu_action_register("L28D66", P_ARKHAN3, "SG03DIA05", NULL);
                            lieu_action_register("L28D68", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }
            }
            /* 0xA70C */
            if (ST(0xB38) == 1) {
                /* 0xA71E */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xA730 */
                    if (ST(0xD30) == 0) {
                        /* 0xA742 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L28D104", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L28D105A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D105B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D105C", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            lieu_action_register("L28D105D", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                            lieu_action_register("L28D107A", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L28D107B", P_ARKHAN2, "SG02DIA05", NULL);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xD30) = 1;
                            return;
                        }
                        /* 0xA832 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L28D110", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L28D105A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D105B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                            lieu_action_register("L28D105C", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                            lieu_action_register("L28D105D", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                            lieu_action_register("L28D111", P_ARKHAN2, "SG02DIA05", NULL);
                            CB->animation("Sar_20");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            ST(0xD30) = 1;
                            return;
                        }
                    }
                    /* 0xA90C */
                    if (ST(0xD30) == 1) {
                        /* 0xA91E */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L28D108", P_ARKHAN2, "SG02DIA07", NULL);
                        }
                        /* 0xA946 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L28D112", P_ARKHAN2, "SG02DIA07", NULL);
                        }
                        lieu_action_register("L28D109A", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                        lieu_action_register("L28D109B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                        lieu_action_register("L28D109C", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xA9E2 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xA9F4 */
                    if (ST(0xD30) == 0) {
                        lieu_action_register("L28D115", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L28D105A", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                        lieu_action_register("L28D105B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                        lieu_action_register("L28D105C", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                        lieu_action_register("L28D105D", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                        lieu_action_register("L28D107", P_ARKHAN2, "SG02DIA05", NULL);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(0xD30) = 1;
                        return;
                    }
                    /* 0xAACE */
                    if (ST(0xD30) == 1) {
                        lieu_action_register("L28D116A", P_ARKHAN2, "SG02DIA07", NULL);
                        /* 0xAAF6 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L28D116B", P_ARKHAN2, "SG02DIA07", NULL);
                        }
                        lieu_action_register("L28D109A", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                        lieu_action_register("L28D109B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                        lieu_action_register("L28D109C", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
            /* 0xAB92 */
            if (ST(0xB38) >= 2) {
                /* 0xABA4 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xABB6 */
                    if (ST(0x190) == 0) {
                        ST(0x190) = 1;
                        lieu_action_register("L28D118A", P_ARKHAN2, "SG02DIA07", NULL);
                        lieu_action_register("L28D118B", P_ARKHAN2, "SG02DIA07", NULL);
                        lieu_action_register("L28D119A", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                        lieu_action_register("L28D119B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xAC5E */
                    if (ST(0x190) == 1) {
                        lieu_action_register("L28D121", P_ARKHAN2, "SG02DIA07", NULL);
                        lieu_action_register("L28D122", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xACC8 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xACDA */
                    if (ST(0x190) == 0) {
                        ST(0x190) = 1;
                        lieu_action_register("L28D123A", P_ARKHAN3, "SG02DIA07", NULL);
                        lieu_action_register("L28D123B", P_ARKHAN3, "SG02DIA07", NULL);
                        lieu_action_register("L28D119A", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                        lieu_action_register("L28D119B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xAD82 */
                    if (ST(0x190) == 1) {
                        lieu_action_register("L28D124", P_ARKHAN3, "SG02DIA07", NULL);
                        lieu_action_register("L28D122", P_ARCHIVIST, "SP05DIA50", "SP05ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
        }
        /* 0xADEC */
        if (ST(ST_VIE(P_ARCHIVIST)) == VIE_DEAD) {
            CB->fouille(P_ARCHIVIST);
        }
    }
    /* 0xAE06 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == P_ARCHIVIST && CB->direction_perso(P_ARCHIVIST) && ST(ST_VIE(P_ARCHIVIST)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAE81 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD20", P_ARKHAN2);
        }
        /* 0xAEA3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xAEEB */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->select_pile(1, 1);
        ST(V_ENGAGED_PERSONA) = P_ARCHIVIST;
        CB->combat(P_ARCHIVIST, 1);
        CB->select_pile(0, 1);
    }
}

/* trigger 4 - sub_1000AF64 */
static void trig04(void) {
    /* 0xAF73 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xC000) && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAFDC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xAFEB */
            if (ST(0x68) == 0) {
                ST(0x68) = 1;
                CB->animation("SG01PRE06");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB03A */
            if (ST(0x68) == 1) {
                CB->animation("SG01PRE02");
                CB->animation("ar");
                CB->dialogue("LCOD10", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xB09B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xB0AA */
            if (ST(0x68) == 0) {
                CB->animation("SG01PRE06");
                CB->animation("ar");
                ST(0x68) = 1;
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB104 */
            if (ST(0x68) == 1) {
                CB->animation("SG01PRE02");
                CB->animation("ar");
                CB->dialogue("LCOD11", P_ARKHAN3);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 5 - sub_1000B15E */
static void trig05(void) {
    /* 0xB16D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0) && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->pos_perso(ST(ST_HERO), 8167, -4799, 0xFADD, 1, "ar", 1, 0);
        /* 0xB1FE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ACT08");
            CB->dialogue("L28D71A", P_ARKHAN2);
            CB->dialogue("L28D71B", P_ARKHAN2);
            CB->dialogue("L28D71C", P_ARKHAN2);
            CB->dialogue("L28D71D", P_ARKHAN2);
            CB->dialogue("L28D71E", P_ARKHAN2);
            CB->dialogue("L28D71F", P_ARKHAN2);
            CB->dialogue("L28D71G", P_ARKHAN2);
            CB->dialogue("L28D71H", P_ARKHAN2);
            CB->dialogue("L28D71I", P_ARKHAN2);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB2D4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ACT08");
            CB->dialogue("L28D171A", P_ARKHAN3);
            CB->dialogue("L28D171B", P_ARKHAN3);
            CB->dialogue("L28D171C", P_ARKHAN3);
            CB->dialogue("L28D171D", P_ARKHAN3);
            CB->dialogue("L28D171E", P_ARKHAN3);
            CB->dialogue("L28D171F", P_ARKHAN3);
            CB->dialogue("L28D171G", P_ARKHAN3);
            CB->dialogue("L28D171H", P_ARKHAN3);
            CB->dialogue("L28D171I", P_ARKHAN3);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 6 - sub_1000B3A3 */
static void trig06(void) {
    /* 0xB3B2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xF497) && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xB41B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xB42D */
            if (ST(V_LCOD23_SEEN) == 0 || (ST(V_LCOD23_SEEN) == 1 && ST(ST_PLAYER_WEAPON) != OB_PARCHMENT && ST(0x84) == 0)) {
                CB->pos_perso(ST(ST_HERO), 8062, -4805, 0xF497, 1, "ar", 1, 0);
                /* 0xB4A8 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT10");
                }
                /* 0xB4CD */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT99");
                }
                CB->animation("ar");
                /* 0xB4F5 */
                if (ST(0x8DC) == 0) {
                    CB->dialogue("L28D82", P_ARKHAN2);
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(0x8DC), 1);
                }
                /* 0xB539 */
                if (ST(0x8DC) == 1) {
                    CB->dialogue("L28D83", P_ARKHAN2);
                }
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB584 */
            if (ST(V_LCOD23_SEEN) == 1 && (ST(ST_PLAYER_WEAPON) == OB_PARCHMENT || ST(0x84) == 1)) {
                CB->pos_perso(ST(ST_HERO), 8062, -4805, 0xF497, 1, "ar", 1, 0);
                /* 0xB5E5 */
                if (ST(ST_PLAYER_WEAPON) != OB_PARCHMENT) {
                    CB->dialogue("LCOD23C", P_ARKHAN2);
                } else {
                    lieu_action_register("LCOD23C", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                }
                CB->dialogue("LCOD23D", P_ARKHAN2);
                CB->dialogue("LCOD23E", P_ARKHAN2);
                ST(V_LCOD23_SEEN) = 2;
                /* 0xB65C */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ATT90");
                }
                /* 0xB681 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ATT91");
                }
                CB->load_anim("L28A01", 0);
                CB->evenement(1, 5000);
                CB->animation("ar");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB6E4 */
            if (ST(V_LCOD23_SEEN) == 2) {
                CB->animation("SG02ATT01");
                CB->dialogue("LCOD79", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xB73A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xB74C */
            if (ST(V_LCOD23_SEEN) == 0 || (ST(V_LCOD23_SEEN) == 1 && ST(ST_PLAYER_WEAPON) != OB_PARCHMENT)) {
                CB->pos_perso(ST(ST_HERO), 8062, -4805, 0xF497, 1, "ar", 1, 0);
                /* 0xB7B5 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT10");
                }
                /* 0xB7DA */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT99");
                }
                CB->animation("ar");
                CB->dialogue("L28D97", P_ARKHAN3);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB83B */
            if (ST(V_LCOD23_SEEN) == 1 && (ST(ST_PLAYER_WEAPON) == OB_PARCHMENT || ST(0x84) == 1)) {
                CB->pos_perso(ST(ST_HERO), 8062, -4805, 0xF497, 1, "ar", 1, 0);
                /* 0xB89C */
                if (ST(ST_PLAYER_WEAPON) != OB_PARCHMENT) {
                    CB->dialogue("LCOD24A", P_ARKHAN3);
                    CB->dialogue("LCOD24D", P_ARKHAN3);
                    CB->dialogue("LCOD24E", P_ARKHAN3);
                } else {
                    lieu_action_register("LCOD24A", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                    CB->dialogue("LCOD24D", P_ARKHAN3);
                    CB->dialogue("LCOD24E", P_ARKHAN3);
                }
                ST(V_LCOD23_SEEN) = 2;
                /* 0xB933 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT90");
                }
                /* 0xB958 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT91");
                }
                CB->load_anim("L28A01", 0);
                CB->evenement(1, 5000);
                CB->animation("ar");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xB9BB */
            if (ST(V_LCOD23_SEEN) == 2) {
                CB->animation("SG03ATT01");
                CB->dialogue("L28D99", P_ARKHAN3);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 7 - sub_1000BA0A */
static void trig07(void) {
    /* 0xBA19 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(11) && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xBA82 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xBA94 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xBAA6 */
                if (ST(0x5C8) == 0) {
                    CB->dialogue("L28D88", P_ARKHAN2);
                    CB->animation("SG02PRE06");
                    CB->etat_objet("OB20A", -1, -1, -1, -1, -1, -1, -3, 0);
                    CB->animation("Sar_20");
                    ST(0x5C8) = 1;
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xBB29 */
                if (ST(0x5C8) == 1) {
                    CB->animation("SG02ATT01");
                    CB->dialogue("LCOD79", P_ARKHAN2);
                    CB->animation("ar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xBB7F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02ATT01");
                CB->dialogue("L28D87", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xBBD5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xBBE7 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xBBF9 */
                if (ST(0x5C8) == 0) {
                    CB->dialogue("L28D90", P_ARKHAN3);
                    CB->animation("SG01PRE06");
                    CB->etat_objet("OB20A", -1, -1, -1, -1, -1, -1, -3, 0);
                    CB->animation("Sar_20");
                    ST(0x5C8) = 1;
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xBC7C */
                if (ST(0x5C8) == 1) {
                    CB->animation("SG01ATT02");
                    CB->dialogue("LCOD80", P_ARKHAN3);
                    CB->animation("ar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xBCD2 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG01ATT02");
                CB->dialogue("L28D92", P_ARKHAN3);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 8 - sub_1000BD21 - only probes direction_spec(0); no effect */
static void trig08(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->direction_spec(0);
    }
}

/* trigger 9 - sub_1000BD5A */
static void trig09(void) {
    /* 0xBD69 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xC000) && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBDD2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L28D69", P_ARKHAN2);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xBE1D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L28D70", P_ARKHAN3);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 10 - sub_1000BE61 */
static void trig10(void) {
    /* 0xBE70 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000)) {
        CB->select_pile(1, 2);
        CB->bloque(BLOQUE_ALL);
        CB->load_anim("228OUV03", 1);
        CB->vire_col(9, 0);
        CB->animation("SG00OUV01");
        /* 0xBEF6 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            CB->evenement(33, 400);
        }
        /* 0xBF18 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->evenement(33, 600);
        }
    }
}

/* trigger 12 - sub_1000BF3E */
static void trig12(void) {
    /* 0xBF4D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x1388)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBFA7 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xBFB9 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD97", P_ARKHAN2);
            }
            /* 0xBFDB */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD98", P_ARKHAN2);
            }
        }
        /* 0xBFFD */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD99", P_ARKHAN3);
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 41 - sub_10007789 */
static void trig41(void) {
    /* 0x779B */
    if (ST(ST_VIE(P_ARCHIVIST)) != VIE_DEAD && ST(0xB38) >= 1 && ST(0x774) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0x774) = 1;
        lieu_action_register("L28D100", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
        /* 0x7804 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L28D101", P_ARKHAN2, "SG02DIA05", NULL);
        }
        /* 0x782C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L28D102", P_ARKHAN3, "SG03DIA05", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        lieu_action_register("L28D103A", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
        lieu_action_register("L28D103B", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
    }
    /* 0x788E */
    if (ST(0x548) == 0 && ST(0xB38) == 0) {
        ST(0x548) = 1;
        CB->joindre(ST(ST_HERO), 0, 0, 0, 5);
        CB->joindre(P_ARCHIVIST, 0, 0, 0, ST(ST_HERO));
        CB->bloque(BLOQUE_ALL);
        /* 0x7902 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7914 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L28D01", P_ARKHAN2, "SG02DIA02", NULL);
                lieu_action_register("L28D02", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                lieu_action_register("L28D03A", P_ARKHAN2, "SG02DIA07", NULL);
                lieu_action_register("L28D03B", P_ARKHAN2, "SG02DIA07", NULL);
                lieu_action_register("L28D04A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                lieu_action_register("L28D04B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
            }
            /* 0x79B3 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L28D05", P_ARKHAN2, "SG02DIA02", NULL);
                lieu_action_register("L28D02", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                lieu_action_register("L28D06", P_ARKHAN2, "SG02DIA02", NULL);
                lieu_action_register("L28D04A", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
                lieu_action_register("L28D04B", P_ARCHIVIST, "SP05DIA53", "SP05ARR01_10");
            }
        }
        /* 0x7A3C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7A4E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L28D07", P_ARKHAN3, "SG03DIA02", NULL);
                lieu_action_register("L28D02", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
                lieu_action_register("L28D08", P_ARKHAN3, "SG03DIA07", NULL);
                lieu_action_register("L28D09", P_ARCHIVIST, "SP05DIA51", "SP05ARR01_10");
            }
            /* 0x7ABE */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("LCOD22B", P_ARKHAN3, "SG03DIA02", NULL);
                lieu_action_register("L28D11A", P_ARCHIVIST, "SP05DIA58", "SP05ARR01_10");
                lieu_action_register("L28D11B", P_ARCHIVIST, "SP05DIA57", "SP05ARR01_10");
            }
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->joindre(P_ARCHIVIST, 7890, -4827, 0xEA60, -1);
        CB->animation("SP05LIR01");
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x7B68 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x7B8C */
    if (ST(V_GUARD_FIGHT_DONE) == 2 || ST(0xB38) == 1) {
        CB->divers(DV_MUSIC, 2, 1);
    }
}

