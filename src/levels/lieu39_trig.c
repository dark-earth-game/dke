/*
 * lieu39_trig.c - Native hand-port of the LIEU39.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and events live in lieu39.c; this
 * file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu39_int.h"

static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* trigger table at 0x1001E6C0 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 10001200   1 stub       2 10002086   3 10005201   4 10005234
 *   5 10005291   6 10005369   7 10005447   8 10005525   9..40 stub
 *  41 10001530
 */
static void (*const g_trigger[42])(void) = {
    l39_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001E6C0]). */
void l39_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - trig00_10001200 */
void l39_trig00(void) {
    /* 0x1212 */
    if (ST(ST_START_POINT(39)) == 1) {
        ST(ST_LEVEL) = 38;
        ST(0xD5C) = 1;
        ST(ST_HERO) = P_ARKHAN3;
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
    ST(ST_START_POINT(39)) = 0;
    CB->ajoute_zone(3, 333, 1573, 40, 60, 1);
    CB->ajoute_zone(4, 1326, 731, 40, 165, 1);
    CB->ajoute_zone(5, 1181, 1223, 167, 143, 0);
    CB->ajoute_zone(6, 1587, 1043, 195, 130, 0);
    CB->ajoute_zone(7, 1454, 1002, 163, 117, 0);
    CB->ajoute_zone(8, 1422, 1330, 190, 190, 0);
    /* 0x13BC */
    if (ST(0xD5C) == 1) {
        CB->pos_perso(ST(ST_HERO), 1196, 827, 0xF439, 1, "ar", 1, 0);
    }
    /* 0x13F9 */
    if (ST(0xD5C) == 2) {
        CB->pos_perso(ST(ST_HERO), 395, 1585, 0xC6E6, 1, "ar", 1, 0);
    }
    /* 0x1436 */
    if (ST(ST_VIE(P_LEONA)) != VIE_DEAD) {
        CB->pos_perso(P_LEONA, 1398, 1033, 0xA832, 1, "P14ATT01", 2, 1);
    } else {
        CB->pos_perso(P_LEONA, CB->divers(DV_DESC_POS, P_LEONA, 0), CB->divers(DV_DESC_POS, P_LEONA, 1), CB->divers(DV_DESC_POS, P_LEONA, 2), 2, "P14CAD", 2, 1);
    }
    CB->bruitage("L39AMB1", -1, -1);
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x88) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
}

/* trigger 41 - trig41_10001530 */
static void trig41(void) {
    /* 0x1542 */
    if (ST(0xD88) == 0) {
        ST(0xD88) = 1;
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        CB->animation("SP14ATT01_250");
        CB->animation("P14ARR01");
        CB->joindre(-P_LEONA, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L39D01A", P_LEONA, "SP14DIA56", "SP14ARR01");
        lieu_action_register("L39D01B", P_LEONA, "SP14DIA50", "SP14ARR01");
        CB->bloque(BLOQUE_NONE);
        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
        CB->select_pile(0, 1);
        return;
    }
    /* 0x1613 */
    if (ST(ST_VIE(P_LEONA)) != VIE_DEAD) {
        ST(0x88) = 1;
        CB->evenement(1, 1000);
    }
}

/* trigger 2 - trig02_10002086 */
static void trig02(void) {
    /* 0x2095 */
    if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_LEONA)) != VIE_DEAD) {
        /* 0x20B6 */
        if (ST(0x64) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_LEONA)) {
            ST(0x64) = 1;
            CB->select_pile(1, 1);
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_LEONA);
            CB->animation("SP14GAR26");
            lieu_action_register("L39D04", P_LEONA, "P14GAR03", "SP14GAR03");
            CB->joindre_perm(P_LEONA, ST(ST_HERO));
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x216D */
        if (ST(0x64) == 1 && ST(ST_COMBAT_STANCE) == 0) {
            ST(0x64) = 0;
            CB->select_pile(1, 1);
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_LEONA);
            CB->animation("SP14GAR27");
            lieu_action_register("L39D05", P_LEONA, "P14ARR01", "SP14ARR01");
            CB->joindre_perm(P_LEONA, ST(ST_HERO));
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0x2214 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_LEONA) && ST(ST_VIE(P_LEONA)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L39D04", P_LEONA, "SP14GAR03", "SP14GAR03");
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x22D7 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x32C8) && !CB->direction_perso(P_LEONA) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x2358 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", NULL);
        }
        /* 0x2380 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", NULL);
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x23D7 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_LEONA)) != VIE_DEAD && CB->direction_perso(P_LEONA)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(ST(ST_HERO), 0, 0, 0, P_LEONA);
        CB->select_pile(1, 1);
        CB->stop_joindre(-P_LEONA);
        CB->joindre(P_LEONA, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(0, 1);
        /* 0x24B0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x24C2 */
            if (ST(0xD88) == 1) {
                /* 0x24D4 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L39D06A", P_ARKHAN2, "SG02DIA01", NULL);
                    lieu_action_register("L39D06B", P_ARKHAN2, "SG02DIA04", NULL);
                }
                /* 0x2512 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L39D07A", P_ARKHAN2, "SG02DIA01", NULL);
                    lieu_action_register("L39D07B", P_ARKHAN2, "SG02DIA04", NULL);
                }
                lieu_action_register("L39D08A", P_LEONA, "SP14DIA50", "SP14ARR01");
                lieu_action_register("L39D08B", P_LEONA, "SP14DIA51", "SP14ARR01");
                lieu_action_register("L39D08C", P_LEONA, "SP14DIA57", "SP14ARR01");
                /* 0x259B */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L39D09A", P_ARKHAN2, "SG02DIA02", NULL);
                    lieu_action_register("L39D09B", P_ARKHAN2, "SG02DIA01", NULL);
                }
                /* 0x25D9 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L39D10A", P_ARKHAN2, "SG02DIA02", NULL);
                    lieu_action_register("L39D10B", P_ARKHAN2, "SG02DIA01", NULL);
                }
                lieu_action_register("L39D11A", P_LEONA, "SP14DIA54", "SP14ARR01");
                lieu_action_register("L39D11B", P_LEONA, "SP14DIA55", "SP14ARR01");
                lieu_action_register("L39D11C", P_LEONA, "SP14DIA58", "SP14ARR01");
                ST(0xD88) = 2;
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x26B0 */
            if (ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON) || CB->divers(DV_CAN_FIGHT, 0, 2)) {
                /* 0x26F3 */
                if (ST(0xD88) == 2) {
                    /* 0x2705 */
                    if (ST(0xD90) == 0) {
                        /* 0x2717 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D18A", P_ARKHAN2, "SG02DIA02", NULL);
                            lieu_action_register("L39D18B", P_ARKHAN2, "SG02DIA03", NULL);
                        }
                        /* 0x2755 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D19A", P_ARKHAN2, "SG02DIA02", NULL);
                            lieu_action_register("L39D19B", P_ARKHAN2, "SG02DIA03", NULL);
                        }
                        lieu_action_register("L39D20A", P_LEONA, "SP14DIA58", "SP14ARR01");
                        lieu_action_register("L39D20B", P_LEONA, "SP14DIA57", "SP14ARR01");
                        lieu_action_register("L39D21A", P_ARKHAN2, "SG02DIA04", NULL);
                        lieu_action_register("L39D21B", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L39D22", P_LEONA, "SP14DIA55", "SP14ARR01");
                        lieu_action_register("L39D23A", P_ARKHAN2, "SG02DIA06", NULL);
                        lieu_action_register("L39D23B", P_ARKHAN2, "SG02DIA07", NULL);
                        lieu_action_register("L39D23C", P_ARKHAN2, "SG02DIA01", NULL);
                        /* 0x284C */
                        if (ST(0x32C) != 0) {
                            lieu_action_register("L39D24", P_ARKHAN2, "SG02DIA04", NULL);
                        }
                        /* 0x2874 */
                        if (ST(0x330) != 0) {
                            lieu_action_register("L39D25", P_ARKHAN2, "SG02DIA04", NULL);
                        }
                        lieu_action_register("L39D26A", P_ARKHAN2, "SG02DIA08", NULL);
                        lieu_action_register("L39D26B", P_ARKHAN2, "SG02DIA09", NULL);
                        lieu_action_register("L39D26C", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L39D26D", P_ARKHAN2, "SG02DIA04", NULL);
                        ST(0xD90) = 1;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x2942 */
                    if (ST(0xD90) == 1) {
                        /* 0x2954 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D33A", P_ARKHAN2, "SG02DIA07", NULL);
                            lieu_action_register("L39D33B", P_ARKHAN2, "SG02DIA06", NULL);
                            /* 0x2992 */
                            if (ST(0xB10) != 2) {
                                lieu_action_register("L39D34A", P_ARKHAN2, "SG02DIA01", NULL);
                                lieu_action_register("L39D34B", P_ARKHAN2, "SG02DIA04", NULL);
                            }
                        }
                        /* 0x29D0 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D35A", P_ARKHAN2, "SG02DIA07", NULL);
                            lieu_action_register("L39D35B", P_ARKHAN2, "SG02DIA06", NULL);
                            /* 0x2A0E */
                            if (ST(0xB10) != 2) {
                                lieu_action_register("L39D36A", P_ARKHAN2, "SG02DIA01", NULL);
                                lieu_action_register("L39D36B", P_ARKHAN2, "SG02DIA04", NULL);
                            }
                        }
                        lieu_action_register("L39D37A", P_LEONA, "SP14DIA54", "SP14ARR01");
                        lieu_action_register("L39D37B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D37C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        ST(0xD90) = 2;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x2AE5 */
                    if (ST(0xD90) == 2) {
                        lieu_action_register("L39D40A", P_ARKHAN2, "SG02DIA08", NULL);
                        lieu_action_register("L39D41A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D41B", P_LEONA, "SP14DIA51", "SP14ARR01");
                        lieu_action_register("L39D41C", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D41D", P_LEONA, "SP14DIA54", "SP14ARR01");
                        lieu_action_register("L39D41E", P_LEONA, "SP14DIA55", "SP14ARR01");
                        ST(0xD90) = 3;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x2BD8 */
                    if (ST(0xD90) == 3) {
                        /* 0x2BEA */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D43A", P_ARKHAN2, "SG02DIA07", NULL);
                            lieu_action_register("L39D43B", P_ARKHAN2, "SG02DIA03", NULL);
                        }
                        /* 0x2C28 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D44A", P_ARKHAN2, "SG02DIA07", NULL);
                            lieu_action_register("L39D44B", P_ARKHAN2, "SG02DIA03", NULL);
                        }
                        lieu_action_register("L39D45A", P_LEONA, "SP14DIA58", "SP14ARR01");
                        lieu_action_register("L39D45B", P_LEONA, "SP14DIA57", "SP14ARR01");
                        lieu_action_register("L39D45C", P_LEONA, "SP14DIA56", "SP14ARR01");
                        lieu_action_register("L39D45D", P_LEONA, "SP14DIA55", "SP14ARR01");
                        lieu_action_register("L39D45E", P_LEONA, "SP14DIA54", "SP14ARR01");
                        lieu_action_register("L39D45F", P_LEONA, "SP14DIA50", "SP14ARR01");
                        ST(0xD90) = 4;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x2D4A */
                    if (ST(0xD90) == 4) {
                        lieu_action_register("L39D46A", P_ARKHAN2, "SG02DIA09", NULL);
                        lieu_action_register("L39D46B", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L39D47A", P_LEONA, "SP14DIA55", "SP14ARR01");
                        lieu_action_register("L39D47B", P_LEONA, "SP14DIA56", "SP14ARR01");
                        lieu_action_register("L39D47C", P_LEONA, "SP14DIA54", "SP14ARR01");
                        lieu_action_register("L39D47D", P_LEONA, "SP14DIA58", "SP14ARR01");
                        lieu_action_register("L39D47E", P_LEONA, "SP14DIA57", "SP14ARR01");
                        lieu_action_register("L39D47F", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D47G", P_LEONA, "SP14DIA51", "SP14ARR01");
                        lieu_action_register("L39D47H", P_LEONA, "SP14DIA53", "SP14ARR01");
                        /* 0x2E50 */
                        if (ST(0x32C) != 0) {
                            lieu_action_register("L39D48", P_ARKHAN2, "SG02DIA03", NULL);
                        }
                        /* 0x2E78 */
                        if (ST(0x330) != 0) {
                            lieu_action_register("L39D49", P_ARKHAN2, "SG02DIA03", NULL);
                        }
                        lieu_action_register("L39D50", P_ARKHAN2, "SG02DIA04", NULL);
                        ST(0xD90) = 5;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x2F04 */
                    if (ST(0xD90) == 5) {
                        /* 0x2F16 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D55A", P_ARKHAN2, "SG02DIA01", NULL);
                            lieu_action_register("L39D55B", P_ARKHAN2, "SG02DIA06", NULL);
                        }
                        /* 0x2F54 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D56A", P_ARKHAN2, "SG02DIA08", NULL);
                            lieu_action_register("L39D56B", P_ARKHAN2, "SG02DIA04", NULL);
                        }
                        lieu_action_register("L39D57A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D57B", P_LEONA, "SP14DIA51", "SP14ARR01");
                        lieu_action_register("L39D57C", P_LEONA, "SP14DIA54", "SP14ARR01");
                        lieu_action_register("L39D57D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D57E", P_LEONA, "SP14DIA55", "SP14ARR01");
                        lieu_action_register("L39D58", P_ARKHAN2, "SG02DIA09", NULL);
                        lieu_action_register("L39D59A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D59B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D59C", P_LEONA, "SP14DIA58", "SP14ARR01");
                        /* 0x3070 */
                        if (ST(0x32C) != 0) {
                            lieu_action_register("L39D60", P_LEONA, "SP14DIA51", "SP14ARR01");
                        }
                        /* 0x309B */
                        if (ST(0x330) != 0) {
                            lieu_action_register("L39D61", P_LEONA, "SP14DIA51", "SP14ARR01");
                        }
                        ST(0xD90) = 6;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x3114 */
                    if (ST(0xD90) == 6) {
                        /* 0x3126 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D64A", P_ARKHAN2, "SG02DIA03", NULL);
                            lieu_action_register("L39D64B", P_ARKHAN2, "SG02DIA04", NULL);
                        }
                        /* 0x3164 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D65A", P_ARKHAN2, "SG02DIA03", NULL);
                            lieu_action_register("L39D65B", P_ARKHAN2, "SG02DIA04", NULL);
                        }
                        lieu_action_register("L39D66A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D66B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D66C", P_LEONA, "SP14DIA51", "SP14ARR01");
                        lieu_action_register("L39D66D", P_LEONA, "SP14DIA55", "SP14ARR01");
                        lieu_action_register("L39D66E", P_LEONA, "SP14DIA57", "SP14ARR01");
                        lieu_action_register("L39D66F", P_LEONA, "SP14DIA58", "SP14ARR01");
                        lieu_action_register("L39D66G", P_LEONA, "SP14DIA54", "SP14ARR01");
                        lieu_action_register("L39D66H", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D67A", P_ARKHAN2, "SG02DIA08", NULL);
                        lieu_action_register("L39D67B", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L39D68", P_LEONA, "SP14DIA50", "SP14ARR01");
                        /* 0x32AF */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D69A", P_ARKHAN2, "SG02DIA03", NULL);
                            lieu_action_register("L39D69B", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L39D69C", P_ARKHAN2, "SG02DIA07", NULL);
                        }
                        /* 0x3303 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D70A", P_ARKHAN2, "SG02DIA03", NULL);
                            lieu_action_register("L39D70B", P_ARKHAN2, "SG02DIA05", NULL);
                            lieu_action_register("L39D70C", P_ARKHAN2, "SG02DIA07", NULL);
                        }
                        lieu_action_register("L39D71A", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D71B", P_LEONA, "SP14DIA58", "SP14ARR01");
                        ST(0xD90) = 7;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x33D7 */
                    if (ST(0xD90) == 7) {
                        /* 0x33E9 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D75A", P_ARKHAN2, "SG02DIA09", NULL);
                            lieu_action_register("L39D75B", P_ARKHAN2, "SG02DIA04", NULL);
                        }
                        /* 0x3427 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D76", P_ARKHAN2, "SG02DIA04", NULL);
                        }
                        lieu_action_register("L39D77A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D77B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D77C", P_LEONA, "SP14DIA51", "SP14ARR01");
                        lieu_action_register("L39D77D", P_LEONA, "SP14DIA55", "SP14ARR01");
                        lieu_action_register("L39D78", P_ARKHAN2, "SG02DIA08", NULL);
                        lieu_action_register("L39D79A", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D79B", P_LEONA, "SP14DIA50", "SP14ARR01");
                        ST(0xD90) = 8;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x3549 */
                    if (ST(0xD90) == 8) {
                        /* 0x355B */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D82", P_ARKHAN2, "SG02DIA06", NULL);
                        }
                        /* 0x3583 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D83", P_ARKHAN2, "SG02DIA05", NULL);
                        }
                        lieu_action_register("L39D84A", P_LEONA, "SP14DIA58", "SP14ARR01");
                        lieu_action_register("L39D84B", P_LEONA, "SP14DIA57", "SP14ARR01");
                        lieu_action_register("L39D84C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D84D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D84E", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D84F", P_LEONA, "SP14DIA51", "SP14ARR01");
                        lieu_action_register("L39D84G", P_LEONA, "SP14DIA55", "SP14ARR01");
                        lieu_action_register("L39D84H", P_LEONA, "SP14DIA54", "SP14ARR01");
                        lieu_action_register("L39D84I", P_LEONA, "SP14DIA53", "SP14ARR01");
                        ST(0xD90) = 9;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x36DA */
                    if (ST(0xD90) == 9) {
                        /* 0x36EC */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L39D86A", P_ARKHAN2, "SG02DIA01", NULL);
                            lieu_action_register("L39D86B", P_ARKHAN2, "SG02DIA06", NULL);
                        }
                        /* 0x372A */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L39D87A", P_ARKHAN2, "SG02DIA01", NULL);
                            lieu_action_register("L39D87B", P_ARKHAN2, "SG02DIA02", NULL);
                            lieu_action_register("L39D87C", P_ARKHAN2, "SG02DIA06", NULL);
                        }
                        ST(0x68) = CB->random(3);
                        /* 0x378F */
                        if (ST(0x68) == 0) {
                            lieu_action_register("L39D88", P_LEONA, "SP14DIA50", "SP14ARR01");
                        }
                        /* 0x37B7 */
                        if (ST(0x68) == 1) {
                            lieu_action_register("L39D89", P_LEONA, "SP14DIA50", "SP14ARR01");
                        }
                        /* 0x37DF */
                        if (ST(0x68) == 2) {
                            lieu_action_register("L39D90", P_LEONA, "SP14DIA50", "SP14ARR01");
                        }
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            } else if (ST(0xD88) != 2) {
                lieu_action_register("L39D95", P_ARKHAN2, "SG02PRE80", "SG02PRE81");
                lieu_action_register("L39D96", P_LEONA, "SP14DIA50", "SP14ARR01");
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else if (ST(ST_PLAYER_WEAPON) == 2 || ST(ST_PLAYER_WEAPON) == 12 || ST(ST_PLAYER_WEAPON) == 18 || ST(ST_PLAYER_WEAPON) == 19) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("L39D97", P_ARKHAN2, "SG02PRE80", "SG02PRE81");
                lieu_action_register("L39D98", P_LEONA, "SP14DIA50", "SP14ARR01");
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else if (ST(ST_PLAYER_WEAPON) == 45) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("L39D99", P_ARKHAN2, "SG02PRE80", "SG02PRE81");
                lieu_action_register("L39D100", P_LEONA, "SP14DIA50", "SP14ARR01");
                /* 0x3A00 */
                if (ST(0xB10) != 2) {
                    lieu_action_register("L39D101", P_LEONA, "SP14DIA50", "SP14ARR01");
                }
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("L39D102", P_ARKHAN2, "SG02PRE80", "SG02PRE81");
                lieu_action_register("L39D103", P_LEONA, "SP14DIA50", "SP14ARR01");
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x3AF4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x3B06 */
            if (ST(0xD88) == 1) {
                /* 0x3B18 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L39D12", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x3B40 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L39D13", P_ARKHAN3, "SG03DIA04", NULL);
                }
                lieu_action_register("L39D014A", P_LEONA, "SP14DIA50", "SP14ARR01");
                lieu_action_register("L39D014B", P_LEONA, "SP14DIA53", "SP14ARR01");
                lieu_action_register("L39D014C", P_LEONA, "SP14DIA55", "SP14ARR01");
                /* 0x3BB3 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L39D15", P_ARKHAN3, "SG03DIA04", NULL);
                }
                /* 0x3BDB */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L39D16", P_ARKHAN3, "SG03DIA04", NULL);
                }
                lieu_action_register("L39D17A", P_LEONA, "SP14DIA58", "SP14ARR01");
                lieu_action_register("L39D17B", P_LEONA, "SP14DIA57", "SP14ARR01");
                ST(0xD88) = 2;
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x3C83 */
            if (ST(ST_PLAYER_WEAPON) == 0 || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON) || CB->divers(DV_CAN_FIGHT, 0, 2)) {
                /* 0x3CC6 */
                if (ST(0xD88) == 2 && ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x3CE7 */
                    if (ST(0x6C) == 1) {
                        ST(0x6C) = 0;
                        CB->animation("SP14GAR27");
                        CB->animation("SP14ARR01");
                    }
                    /* 0x3D1B */
                    if (ST(0xD94) == 0) {
                        lieu_action_register("L39D27", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D20A", P_LEONA, "SP14DIA51", "SP14ARR01");
                        lieu_action_register("L39D20B", P_LEONA, "SP14DIA54", "SP14ARR01");
                        lieu_action_register("L39D28A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D28B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D28C", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D22", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D29A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D29B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D29C", P_ARKHAN3, "SG03DIA04", NULL);
                        /* 0x3E12 */
                        if (ST(0x32C) != 0) {
                            lieu_action_register("L39D30", P_ARKHAN3, "SG03DIA04", NULL);
                        }
                        /* 0x3E3A */
                        if (ST(0x330) != 0) {
                            lieu_action_register("L39D31", P_ARKHAN3, "SG03DIA04", NULL);
                        }
                        lieu_action_register("L39D32A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D32B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D32C", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D32D", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D32E", P_ARKHAN3, "SG03DIA04", NULL);
                        ST(0xD94) = 1;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x3F1E */
                    if (ST(0xD94) == 1) {
                        lieu_action_register("L39D38A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D38B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D38C", P_ARKHAN3, "SG03DIA04", NULL);
                        /* 0x3F72 */
                        if (ST(0xB10) != 2) {
                            lieu_action_register("L39D39A", P_ARKHAN3, "SG03DIA04", NULL);
                            lieu_action_register("L39D39B", P_ARKHAN3, "SG03DIA04", NULL);
                        }
                        lieu_action_register("L39D37A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D37B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D37C", P_LEONA, "SP14DIA53", "SP14ARR01");
                        ST(0xD94) = 2;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x4049 */
                    if (ST(0xD94) == 2) {
                        lieu_action_register("L39D42A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D41A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D41B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D41C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D41D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D41E", P_LEONA, "SP14DIA53", "SP14ARR01");
                        ST(0xD94) = 3;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x413C */
                    if (ST(0xD94) == 3) {
                        lieu_action_register("L39D109", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D45A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D45B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D45C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D45D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D45E", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D45F", P_LEONA, "SP14DIA53", "SP14ARR01");
                        ST(0xD94) = 4;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x4248 */
                    if (ST(0xD94) == 4) {
                        lieu_action_register("L39D51A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D51B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D47A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D47B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D47C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D47D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D47E", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D47F", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D47G", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D47H", P_LEONA, "SP14DIA53", "SP14ARR01");
                        /* 0x434E */
                        if (ST(0x32C) != 0) {
                            lieu_action_register("L39D52", P_ARKHAN3, "SG03DIA04", NULL);
                        }
                        /* 0x4376 */
                        if (ST(0x330) != 0) {
                            lieu_action_register("L39D53", P_ARKHAN3, "SG03DIA04", NULL);
                        }
                        lieu_action_register("L39D54", P_ARKHAN3, "SG03DIA04", NULL);
                        ST(0xD94) = 5;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x4402 */
                    if (ST(0xD94) == 5) {
                        lieu_action_register("L39D62A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D62B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D57A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D57B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D57C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D57D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D57E", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D63", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D59A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D59B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D59C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        /* 0x451E */
                        if (ST(0x32C) != 0) {
                            lieu_action_register("L39D60", P_LEONA, "SP14DIA50", "SP14ARR01");
                        }
                        /* 0x4549 */
                        if (ST(0x330) != 0) {
                            lieu_action_register("L39D61", P_LEONA, "SP14DIA50", "SP14ARR01");
                        }
                        ST(0xD94) = 6;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x45C2 */
                    if (ST(0xD94) == 6) {
                        lieu_action_register("L39D72", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D66A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D66B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D66C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D66D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D66E", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D66F", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D66G", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D66H", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D73A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D73B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D68", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D74A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D74B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D71A", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D71B", P_LEONA, "SP14DIA50", "SP14ARR01");
                        ST(0xD94) = 7;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x47A3 */
                    if (ST(0xD94) == 7) {
                        lieu_action_register("L39D80A", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D80B", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D77A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D77B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D77C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D77D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D81", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D79A", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D79B", P_LEONA, "SP14DIA50", "SP14ARR01");
                        ST(0xD94) = 8;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x48DB */
                    if (ST(0xD94) == 8) {
                        lieu_action_register("L39D85", P_ARKHAN3, "SG03DIA04", NULL);
                        lieu_action_register("L39D84A", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D84B", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D84C", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D84D", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D84E", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D84F", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D84G", P_LEONA, "SP14DIA53", "SP14ARR01");
                        lieu_action_register("L39D84H", P_LEONA, "SP14DIA50", "SP14ARR01");
                        lieu_action_register("L39D84I", P_LEONA, "SP14DIA53", "SP14ARR01");
                        ST(0xD94) = 9;
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x4A32 */
                    if (ST(0xD94) == 9) {
                        lieu_action_register("LCOD65", P_ARKHAN3, "SG03DIA04", NULL);
                        ST(0x68) = CB->random(3);
                        /* 0x4A6B */
                        if (ST(0x68) == 0) {
                            lieu_action_register("L39D88", P_LEONA, "SP14DIA50", "SP14ARR01");
                        }
                        /* 0x4A93 */
                        if (ST(0x68) == 1) {
                            lieu_action_register("L39D89", P_LEONA, "SP14DIA50", "SP14ARR01");
                        }
                        /* 0x4ABB */
                        if (ST(0x68) == 2) {
                            lieu_action_register("L39D90", P_LEONA, "SP14DIA50", "SP14ARR01");
                        }
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x4B25 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x4B34 */
                    if (ST(0x6C) == 0) {
                        ST(0x6C) = 1;
                        lieu_action_register("L39D92", P_ARKHAN3, "SG03DIA04", NULL);
                        CB->animation("SP14GAR26");
                        lieu_action_register("L39D93", P_LEONA, "P14GAR03", "SP14GAR03");
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x4BC8 */
                    if (ST(0x6C) == 1) {
                        ST(0x6C) = 0;
                        lieu_action_register("LCOD22B", P_ARKHAN3, "SG03DIA04", NULL);
                        CB->animation("SP14GAR27");
                        lieu_action_register("L39D94", P_LEONA, "P14ARR01", "SP14ARR01");
                        CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            } else if (ST(0xD88) != 2) {
                lieu_action_register("L39D104", P_ARKHAN3, "SG03PRE80", "SG03PRE81");
                lieu_action_register("L39D96", P_LEONA, "SP14DIA50", "SP14ARR01");
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else if (ST(ST_PLAYER_WEAPON) == 2 || ST(ST_PLAYER_WEAPON) == 12 || ST(ST_PLAYER_WEAPON) == 18 || ST(ST_PLAYER_WEAPON) == 19) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("L39D105", P_ARKHAN3, "SG03PRE80", "SG03PRE81");
                lieu_action_register("L39D98", P_LEONA, "SP14DIA50", "SP14ARR01");
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else if (ST(ST_PLAYER_WEAPON) == 45) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("L39D106", P_ARKHAN3, "SG03PRE80", "SG03PRE81");
                lieu_action_register("L39D100", P_LEONA, "SP14DIA50", "SP14ARR01");
                /* 0x4E16 */
                if (ST(0xB10) != 2) {
                    lieu_action_register("L39D101", P_LEONA, "SP14DIA50", "SP14ARR01");
                }
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("L39D104", P_ARKHAN3, "SG03PRE80", "SG03PRE81");
                lieu_action_register("L39D103", P_LEONA, "SP14DIA50", "SP14ARR01");
                CB->joindre_perm(-P_LEONA, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0x4F0A */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_LEONA)) != VIE_DEAD && CB->direction_perso(P_LEONA)) {
        ST(V_CRI_KEY) = 0;
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0, 1000);
        return;
    }
    /* 0x4FA9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_LEONA)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        /* 0x5005 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x5017 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD62", P_ARKHAN2);
            }
            /* 0x5039 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            }
        }
        /* 0x505B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x506D */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD22A", P_ARKHAN3);
            }
            /* 0x508F */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD63", P_ARKHAN3);
            }
        }
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x50B6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_LEONA)) == VIE_DEAD) {
        CB->fouille(P_LEONA);
        return;
    }
    /* 0x5114 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_LEONA)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(V_ENGAGED_PERSONA) = P_LEONA;
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_LEONA);
        /* 0x5191 */
        if (ST(0x6C) == 0) {
            CB->animation("SP14GAR26");
        }
        lieu_action_register("L39D108", P_LEONA, "SP14GAR03", "SP14GAR03");
        CB->combat(P_LEONA, 4);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
}

/* trigger 3 - trig03_10005201 */
static void trig03(void) {
    ST(0xD84) = 2;
    ST(ST_LEVEL) = 39;
    CB->change_piece(38, 0);
}

/* trigger 4 - trig04_10005234 */
static void trig04(void) {
    /* 0x5243 */
    if (ST(V_ENGAGED_PERSONA) != 0) {
        ST(V_ENGAGED_PERSONA) = 0;
        ST(0xD88) = 10;
    }
    ST(0xD84) = 1;
    ST(ST_LEVEL) = 39;
    CB->change_piece(38, 0);
}

/* trigger 5 - trig05_10005291 */
static void trig05(void) {
    /* 0x52A0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x32C8)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x52EE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", NULL);
        }
        /* 0x5316 */
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

/* trigger 6 - trig06_10005369 */
static void trig06(void) {
    /* 0x5378 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x9C40) && ST(V_ENGAGED_PERSONA) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x53E1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD07", P_ARKHAN2);
        }
        /* 0x5403 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD08", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 7 - trig07_10005447 */
static void trig07(void) {
    /* 0x5456 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(7) && ST(V_ENGAGED_PERSONA) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x54BF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD07", P_ARKHAN2);
        }
        /* 0x54E1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD08", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 8 - trig08_10005525 */
static void trig08(void) {
    /* 0x5534 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xEA60) && ST(V_ENGAGED_PERSONA) == 0) {
        /* 0x5586 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG02DIA02", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x55CC */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG03DIA02", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}
