/*
 * lieu26_trig.c - Native hand-port of the LIEU26.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu26.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * Trigger 2 dispatches to triggers 3..9 (it is their only caller).
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu26_int.h"

static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig10(void);
static void trig12(void); static void trig13(void); static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* trigger table at 0x10019620 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 100073F0   1 10007ACE stub   2 10007AD9   3 10007E6C   4 100080E5
 *   5 10008316   6 10008411   7 100084E1   8 10008975   9 10008B9F
 *  10 10008C9A  11 10009054 stub  12 1000905F  13 100094A7
 *  14..40 stubs (100095BD..100096DB)  41 10007755
 */
static void (*const g_trigger[42])(void) = {
    l26_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig09, trig10, trig_noop, trig12, trig13, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10019620]). */
void l26_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - sub_100073F0 - level setup */
void l26_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(26)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
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
    CB->load_anim("226OUV01", 0);
    CB->load_anim("226TEL03", 1);
    CB->load_anim("226TELES", 2);
    CB->load_anim("226OUV03", 3);
    CB->load_anim("226BRUNO", 4);
    CB->ajoute_zone(10, 7163, -3326, 185, 105, 0);
    CB->ajoute_zone(35, 7165, -3377, 10, 10, 0);
    CB->ajoute_zone(2, 7445, -3272, 280, 250, 0);
    CB->ajoute_zone(3, 7314, -3393, 115, 120, 0);
    CB->ajoute_zone(4, 7410, -3427, 90, 150, 0);
    CB->ajoute_zone(5, 7504, -3449, 100, 135, 0);
    CB->ajoute_zone(6, 7615, -3424, 130, 130, 0);
    CB->ajoute_zone(7, 7575, -3106, 130, 150, 0);
    CB->ajoute_zone(8, 7434, -3101, 150, 125, 0);
    CB->ajoute_zone(9, 7298, -3126, 126, 143, 0);
    CB->ajoute_zone(12, 7787, -3102, 205, 140, 0);
    CB->ajoute_zone(13, 7056, -3269, 125, 150, 0);
    CB->pos_perso(ST(ST_HERO), 7073, -3267, 0xBDA8, 1, "ar", 1, 0);
    CB->bruitage("L26AMB1", 7445, -3272);
    /* 0x76D5 */
    if (ST(0xB38) == 0) {
        ST(0xB38) = 1;
        ST(0xE74) = 99;
        ST(0xC48) = 99;
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
}

/* trigger 41 - sub_10007755 */
static void trig41(void) {
    CB->divers(DV_MUSIC, 1, 1);
}

/* trig02 - sub_10007AD9 - action key: dispatch to the zone handlers */
static void trig02(void) {
    /* 0x7AE8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        /* 0x7B19 */
        if (CB->direction_zone(3) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
            trig03();
        } else if (CB->direction_zone(4) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 4)) {
            trig04();
        } else if (CB->direction_spec(0x9C40) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)) {
            trig05();
        } else if (CB->direction_zone(6) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 6)) {
            trig06();
        } else if (CB->direction_zone(7) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
            trig07();
        } else if (CB->direction_zone(8) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
            trig08();
        } else if (CB->direction_zone(9) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
            trig09();
        } else if (CB->direction_zone(2)) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x7CED */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x7CFF */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L26D01", P_ARKHAN2, "SG02DIA07", NULL);
                }
                /* 0x7D27 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP02ACT29");
                    ST(0x64) = CB->divers(DV_HP, ST(ST_HERO), -1);
                    CB->divers(DV_HP, ST(ST_HERO), (ST(0x64) - 10));
                }
                CB->animation("ar_100");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            } else if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x7DC2 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L26D02", P_ARKHAN3, "SG03DIA07", NULL);
                }
                /* 0x7DEA */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP03ACT29");
                    ST(0x64) = CB->divers(DV_HP, ST(ST_HERO), -1);
                    CB->divers(DV_HP, ST(ST_HERO), (ST(0x64) - 10));
                }
                CB->animation("ar_100");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
        }
    }
}

/* trig03 - sub_10007E6C - zone 3 notice board */
static void trig03(void) {
    /* 0x7E7B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x584A)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), 7340, -3359, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7F14 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ACT08");
            CB->dialogue("L26D03", P_ARKHAN2);
            CB->dialogue("L26D03A", P_ARKHAN2);
            CB->dialogue("L26D03B", P_ARKHAN2);
            CB->dialogue("L26D03C", P_ARKHAN2);
            CB->dialogue("L26D03D", P_ARKHAN2);
            CB->dialogue("L26D03E", P_ARKHAN2);
            CB->dialogue("L26D03F", P_ARKHAN2);
            CB->dialogue("L26D03G", P_ARKHAN2);
            CB->dialogue("L26D03H", P_ARKHAN2);
            lieu_action_register("L26D03I", P_ARKHAN2, "SG00DIA05", NULL);
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        } else if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ACT08");
            CB->dialogue("L26D04", P_ARKHAN3);
            CB->dialogue("L26D04A", P_ARKHAN3);
            CB->dialogue("L26D04B", P_ARKHAN3);
            CB->dialogue("L26D04C", P_ARKHAN3);
            CB->dialogue("L26D04D", P_ARKHAN3);
            CB->dialogue("L26D04E", P_ARKHAN3);
            CB->dialogue("L26D04F", P_ARKHAN3);
            CB->dialogue("L26D04G", P_ARKHAN3);
            CB->dialogue("L26D04H", P_ARKHAN3);
            lieu_action_register("L26D04I", P_ARKHAN3, "SG00DIA05", NULL);
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
}

/* trig04 - sub_100080E5 - zone 4 */
static void trig04(void) {
    /* 0x80F4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x814E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8160 */
            if (ST(0x630) == 0) {
                ST(0x630) = 1;
                CB->animation("SG00PRE02");
                CB->dialogue("L26D05", P_ARKHAN2);
                lieu_action_register("L26D06", P_ARKHAN2, "SG00PRE08", NULL);
                CB->etat_objet("OB46", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x81EE */
            if (ST(0x630) == 1) {
                lieu_action_register("LCOD10", P_ARKHAN2, "SG02PRE01", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8234 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8246 */
            if (ST(0x630) == 0) {
                ST(0x630) = 1;
                lieu_action_register("L26D07", P_ARKHAN3, "SG00PRE02", "SG00PRE09");
                CB->dialogue("L26D08", P_ARKHAN3);
                CB->etat_objet("OB46", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x82D7 */
            if (ST(0x630) == 1) {
                lieu_action_register("LCOD11", P_ARKHAN3, "SG03PRE01", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trig05 - sub_10008316 - zone 5 */
static void trig05(void) {
    /* 0x8325 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x837F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00DIA06");
            CB->dialogue("L26D09", P_ARKHAN2);
            lieu_action_register("L26D10", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x83C2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00ATT01");
            CB->dialogue("L26D11", P_ARKHAN3);
        }
        CB->animation("sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trig06 - sub_10008411 - zone 6 */
static void trig06(void) {
    /* 0x8420 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xA196)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x847A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L26D12", P_ARKHAN2, "SG00PRE02", NULL);
        }
        /* 0x84A2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L26D13", P_ARKHAN3, "SG00PRE02", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trig07 - sub_100084E1 - zone 7 */
static void trig07(void) {
    /* 0x84F0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xD903)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x854A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x855C */
            if (ST(0x808) == 0) {
                /* 0x856E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L26D14A", P_ARKHAN2, "SG02PRE01", NULL);
                    lieu_action_register("L26D14B", P_ARKHAN2, "SG02PRE01", NULL);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x85D5 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->pos_perso(ST(ST_HERO), 7553, -3128, 0xD3EF, 1, "ar", 1, 0);
                    CB->evenement(1, 500);
                    CB->animation("SP02ACT06");
                    ST(0x808) = 1;
                    CB->dialogue("L26D15", P_ARKHAN2);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->change_camera(99, CAM_AUTO);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x8685 */
            if (ST(0x808) == 1) {
                ST(0x808) = 2;
                ST(0xB4C) = 1;
                lieu_action_register("L26D16", P_ARKHAN2, "SG02PRE02", NULL);
                CB->etat_objet("OB96", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8712 */
            if (ST(0x808) == 2) {
                lieu_action_register("LCOD79", P_ARKHAN2, "SG02DIA07", NULL);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8763 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8775 */
            if (ST(0x808) == 0) {
                /* 0x8787 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L26D19A", P_ARKHAN3, "SG03PRE01", NULL);
                    lieu_action_register("L26D19B", P_ARKHAN3, "SG03PRE01", NULL);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x87EE */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->pos_perso(ST(ST_HERO), 7553, -3128, 0xD3EF, 1, "ar", 1, 0);
                    CB->evenement(1, 500);
                    CB->animation("SP03ACT06");
                    ST(0x808) = 1;
                    CB->dialogue("L26D20", P_ARKHAN3);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->change_camera(99, CAM_AUTO);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x889E */
            if (ST(0x808) == 1) {
                ST(0x808) = 2;
                ST(0xB4C) = 1;
                lieu_action_register("L26D21", P_ARKHAN3, "SG03PRE02", NULL);
                CB->etat_objet("OB96", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x892B */
            if (ST(0x808) == 2) {
                lieu_action_register("LCOD80", P_ARKHAN3, "SG03DIA07", NULL);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trig08 - sub_10008975 - zone 8 */
static void trig08(void) {
    /* 0x8984 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x625)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x89DE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x89F0 */
            if (ST(0xD18) == 0) {
                ST(0xD18) = 1;
                lieu_action_register("L26D23", P_ARKHAN2, "SG00DIA05", "SG00PRE06");
                CB->etat_objet("OB52", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8A71 */
            if (ST(0xD18) == 1) {
                lieu_action_register("LCOD70", P_ARKHAN2, "SG00PRE02", NULL);
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8AC2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8AD4 */
            if (ST(0xD18) == 0) {
                ST(0xD18) = 1;
                lieu_action_register("L26D25", P_ARKHAN3, "SG00DIA05", "SG00PRE06");
                CB->etat_objet("OB52", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8B55 */
            if (ST(0xD18) == 1) {
                lieu_action_register("LCOD80", P_ARKHAN3, "SG03PRE02", NULL);
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trig09 - sub_10008B9F - zone 9 */
static void trig09(void) {
    /* 0x8BAE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x22F8)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8C08 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00DIA06");
            CB->dialogue("L26D09", P_ARKHAN2);
            lieu_action_register("L26D10", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x8C4B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00ATT01");
            CB->dialogue("L26D11", P_ARKHAN3);
        }
        CB->animation("sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trig10 - sub_10008C9A - zone 10 */
static void trig10(void) {
    /* 0x8CA9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(35)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8D6C */
        if (ST(0x50C) == 0) {
            ST(0x50C) = 1;
            /* 0x8D8D */
            if (ST(0xD18) == 0) {
                /* 0x8D9F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L26D27A", P_ARKHAN2, "SG00DIA08", NULL);
                    lieu_action_register("L26D27B", P_ARKHAN2, "SG00DIA08", NULL);
                }
                /* 0x8DDD */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L26D28A", P_ARKHAN3, "SG00DIA08", NULL);
                    lieu_action_register("L26D28B", P_ARKHAN3, "SG00DIA08", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            } else if (ST(0xD18) == 1) {
                CB->evenement(0, 500);
                CB->pos_perso(ST(ST_HERO), 7106, -3309, 0x7D38, 1, "ar", 1, 0);
                CB->animation("SP02ACT21");
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            } else if (ST(0x50C) == 1) {
                /* 0x8ECC */
                if (ST(0xD18) == 0) {
                    /* 0x8EDE */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L26D300", P_ARKHAN2, "SG00DIA08", NULL);
                        lieu_action_register("L26D27B", P_ARKHAN2, "SG00DIA08", NULL);
                    }
                    /* 0x8F1C */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L26D301", P_ARKHAN3, "SG00DIA08", NULL);
                        lieu_action_register("L26D28B", P_ARKHAN3, "SG00DIA08", NULL);
                    }
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                } else if (ST(0xD18) == 1) {
                    CB->evenement(0, 500);
                    CB->pos_perso(ST(ST_HERO), 7106, -3309, 0x7D38, 1, "ar", 1, 0);
                    CB->animation("SP03ACT21");
                    /* 0x8FD8 */
                    if (ST(0xD14) == 0) {
                        /* 0x8FEA */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L26D32", P_ARKHAN2, "G00DIA08", NULL);
                        }
                        /* 0x9012 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L26D33", P_ARKHAN3, "G00DIA08", NULL);
                        }
                    }
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                }
            }
        } else if (ST(0x50C) == 1) {
            /* 0x8ECC */
            if (ST(0xD18) == 0) {
                /* 0x8EDE */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L26D300", P_ARKHAN2, "SG00DIA08", NULL);
                    lieu_action_register("L26D27B", P_ARKHAN2, "SG00DIA08", NULL);
                }
                /* 0x8F1C */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L26D301", P_ARKHAN3, "SG00DIA08", NULL);
                    lieu_action_register("L26D28B", P_ARKHAN3, "SG00DIA08", NULL);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            } else if (ST(0xD18) == 1) {
                CB->evenement(0, 500);
                CB->pos_perso(ST(ST_HERO), 7106, -3309, 0x7D38, 1, "ar", 1, 0);
                CB->animation("SP03ACT21");
                /* 0x8FD8 */
                if (ST(0xD14) == 0) {
                    /* 0x8FEA */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L26D32", P_ARKHAN2, "G00DIA08", NULL);
                    }
                    /* 0x9012 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L26D33", P_ARKHAN3, "G00DIA08", NULL);
                    }
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            }
        }
    }
}

/* trig12 - sub_1000905F - zone 12 */
static void trig12(void) {
    /* 0x906E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(12)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x90C8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x90DA */
            if (ST(0x808) == 0) {
                /* 0x90EC */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L26D34", P_ARKHAN2, "SG02DIA07", NULL);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x913D */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->pos_perso(ST(ST_HERO), 7794, -3149, 0x1718, 1, "ar", 1, 0);
                    CB->evenement(1, 500);
                    CB->animation("SP02ACT06");
                    ST(0x808) = 1;
                    CB->dialogue("L26D15", P_ARKHAN2);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x91DD */
            if (ST(0x808) == 1) {
                ST(0x808) = 2;
                ST(0xB4C) = 1;
                lieu_action_register("L26D16", P_ARKHAN2, "SG02PRE02", NULL);
                CB->etat_objet("OB96", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x926A */
            if (ST(0x808) == 2) {
                lieu_action_register("LCOD79", P_ARKHAN2, "SG02DIA07", NULL);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x92BB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x92CD */
            if (ST(0x808) == 0) {
                /* 0x92DF */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L26D35", P_ARKHAN3, "SG03DIA07", NULL);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9330 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->pos_perso(ST(ST_HERO), 7794, -3149, 0x1718, 1, "ar", 1, 0);
                    CB->evenement(1, 500);
                    CB->animation("SP02ACT06");
                    ST(0x808) = 1;
                    CB->dialogue("L26D20", P_ARKHAN3);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x93D0 */
            if (ST(0x808) == 1) {
                ST(0x808) = 2;
                ST(0xB4C) = 1;
                lieu_action_register("L26D21", P_ARKHAN3, "SG02PRE02", NULL);
                CB->etat_objet("OB96", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x945D */
            if (ST(0x808) == 2) {
                lieu_action_register("LCOD80", P_ARKHAN3, "SG03DIA07", NULL);
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trig13 - sub_100094A7 - zone 13 */
static void trig13(void) {
    /* 0x94B6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x3A98) && ST(0xD14) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9522 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L26D36", P_ARKHAN2, "SG02DIA07", NULL);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9573 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L26D38", P_ARKHAN3, "SG03DIA07", NULL);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

