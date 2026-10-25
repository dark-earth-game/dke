/*
 * lieu13_trig.c — Native hand-port of the LIEU13.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu13.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu13_int.h"

static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig10(void);
static void trig11(void); static void trig12(void); static void trig13(void);
static void trig15(void); static void trig41(void);
static void trig_noop(void);   /* the 27 stub slots (1, 14, 16..40) */

/* dword_1001A870[42] — filled at INIT with the 5-byte jmp thunks at
 * 0x10001000..; static here. Slots 1, 14 and 16..40 point to the empty
 * stub (identical to LIEU01 trig06_1000D30E).
 *   idx handler        idx handler        idx handler
 *    0  100073F0        6  10008B51       12  10009BCB
 *    1  10007E49 stub   7  10009147       13  10009C9A
 *    2  10007E54        8  1000970B       14  10009FCF stub
 *    3  10007FDD        9  1000981E       15  10009FDA
 *    4  10008132       10  10009931       16..40 stubs
 *    5  100088B7       11  10009A79       41  1000779E
 */
static void (*const g_trigger[42])(void) = {
    l13_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig09, trig10, trig11, trig12, trig13, trig_noop, trig15,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001A870]). */
void l13_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 */
void l13_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(13)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 49;
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
    CB->bruitage("L13AMB1", -1, -1);
    CB->divers(DV_MUSIC, 1, 0);
    CB->load_anim("213SOU01", 0);
    CB->load_anim("L13A01", 1);
    CB->load_anim("L13A02", 2);
    CB->load_anim("213OUV01", 3);
    CB->load_anim("213RED", 5);
    CB->load_anim("213GRE", 6);
    CB->ajoute_zone(10, 2054, 4668, 115, 105, 0);
    CB->ajoute_zone(2, 1498, 4665, 143, 182, 0);
    CB->ajoute_zone(3, 1349, 4944, 305, 110, 0);
    CB->ajoute_zone(4, 1359, 4620, 125, 180, 0);
    CB->ajoute_zone(5, 1454, 5250, 150, 145, 0);
    CB->ajoute_zone(6, 1982, 5282, 160, 150, 0);
    CB->ajoute_zone(7, 2298, 4696, 170, 170, 0);
    CB->ajoute_zone(8, 1743, 4734, 140, 215, 0);
    CB->ajoute_zone(9, 1659, 5035, 165, 130, 0);
    CB->ajoute_zone(11, 1313, 4836, 370, 130, 0);
    CB->ajoute_zone(12, 1444, 5118, 150, 250, 0);
    CB->ajoute_zone(13, 1938, 5123, 165, 130, 0);
    CB->ajoute_zone(15, 1667, 4829, 150, 100, 0);
    /* 0x76F0 */
    if (ST(ST_LEVEL) == 49) {
        CB->pos_perso(ST(ST_HERO), 2029, 4756, 0x1047, 1, "ar", 1, 0);
    }
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(V_WEAPON_WARNED) = 0;
    /* 0x7784 */
    if (ST(0xC20) == 1) {
        ST(0xC20) = 2;
    }
}

/* trigger 41 — sub_1000779E */
static void trig41(void) {
    CB->play_anim(5, 1);
    CB->divers(DV_MUSIC, 1, 1);
    /* 0x77D2 */
    if (ST(0xEFC) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_WEAPON_WARNED) = 1;
        ST(0xEFC) = 1;
        /* 0x780D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L13D01", P_ARKHAN2, "SG00DIA04", NULL);
        }
        /* 0x7835 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L13D02", P_ARKHAN3, "SG00DIA04", NULL);
        }
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 02 — sub_10007E54 */
static void trig02(void) {
    /* 0x7E63 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xC350)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7EBD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7ED7 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D11A", P_ARKHAN2, "SP02ATT94", NULL);
            } else {
                lieu_action_register("L13D11A", P_ARKHAN2, "SP02ATT95", NULL);
            }
            /* 0x7F1A */
            if (ST(ST_LOCKED_CAMERA) == 3) {
                CB->dialogue("L13D11B", P_ARKHAN2);
            }
        }
        /* 0x7F3C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7F56 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D12A", P_ARKHAN3, "SP03ATT94", NULL);
            } else {
                lieu_action_register("L13D12A", P_ARKHAN3, "SP03ATT95", NULL);
            }
            /* 0x7F99 */
            if (ST(ST_LOCKED_CAMERA) == 3) {
                CB->dialogue("L13D12B", P_ARKHAN3);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 03 — sub_10007FDD */
static void trig03(void) {
    /* 0x7FEC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(3)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8046 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8060 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D13A", P_ARKHAN2, "SP02ATT94", NULL);
            } else {
                lieu_action_register("L13D13A", P_ARKHAN2, "SP02ATT95", NULL);
            }
            CB->dialogue("L13D13B", P_ARKHAN2);
        }
        /* 0x80B3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x80CD */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D14", P_ARKHAN3, "SP03ATT94", NULL);
            } else {
                lieu_action_register("L13D14", P_ARKHAN3, "SP03ATT95", NULL);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 04 — sub_10008132 */
static void trig04(void) {
    /* 0x8141 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x819B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x81B5 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x81C7 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    ST(0x64) = 1;
                    /* 0x81E5 */
                    if (ST(0xC8C) < 2) {
                        ST(0xC8C)++;
                        CB->evenement(1, 800);
                        CB->animation("SP02ACT98");
                        CB->animation("ar_50");
                        /* 0x8237 */
                        if (CB->divers(DV_HP, ST(ST_HERO), -1) > 35) {
                            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) - 30));
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        CB->dialogue("L13D15", P_ARKHAN2);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x82D7 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    ST(0x64) = 2;
                    CB->evenement(1, 400);
                    CB->animation("SP02ACT97");
                    CB->animation("ar_50");
                    /* 0x831B */
                    if (ST(V_WEAPON_WARNED) == 1) {
                        ST(V_WEAPON_WARNED) = 0;
                        CB->dialogue("L13D23", P_ARKHAN2);
                    }
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x837D */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x838F */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    ST(0x64) = 1;
                    /* 0x83AD */
                    if (ST(0xC8C) < 2) {
                        ST(0xC8C)++;
                        CB->evenement(1, 500);
                        CB->animation("SP02ACT94");
                        CB->animation("ar_50");
                        /* 0x83FF */
                        if (CB->divers(DV_HP, ST(ST_HERO), -1) > 35) {
                            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) - 30));
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        CB->dialogue("L13D15", P_ARKHAN2);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x849F */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    ST(0x64) = 2;
                    CB->evenement(1, 400);
                    CB->animation("SP02ACT93");
                    CB->animation("ar_50");
                    /* 0x84E3 */
                    if (ST(V_WEAPON_WARNED) == 1) {
                        ST(V_WEAPON_WARNED) = 0;
                        CB->dialogue("L13D23", P_ARKHAN2);
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
        /* 0x8532 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x854C */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x855E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    ST(0x64) = 1;
                    /* 0x857C */
                    if (ST(0xC8C) < 2) {
                        ST(0xC8C)++;
                        CB->evenement(1, 500);
                        CB->animation("SP03ACT98");
                        CB->animation("ar_50");
                        /* 0x85CE */
                        if (CB->divers(DV_HP, ST(ST_HERO), -1) > 35) {
                            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) - 30));
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        CB->dialogue("L13D16", P_ARKHAN3);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x866E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    ST(0x64) = 2;
                    CB->evenement(1, 400);
                    CB->animation("SP03ACT97");
                    /* 0x86A7 */
                    if (ST(V_WEAPON_WARNED) == 1) {
                        ST(V_WEAPON_WARNED) = 0;
                        CB->dialogue("L13D24", P_ARKHAN2);
                    }
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x8709 */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x871B */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    ST(0x64) = 1;
                    /* 0x8739 */
                    if (ST(0xC8C) < 2) {
                        ST(0xC8C)++;
                        CB->evenement(1, 500);
                        CB->animation("SP03ACT94");
                        CB->animation("ar_50");
                        /* 0x878B */
                        if (CB->divers(DV_HP, ST(ST_HERO), -1) > 35) {
                            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) - 30));
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        CB->dialogue("L13D16", P_ARKHAN3);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x882B */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    ST(0x64) = 2;
                    CB->evenement(1, 400);
                    CB->animation("SP03ACT93");
                    /* 0x8864 */
                    if (ST(V_WEAPON_WARNED) == 1) {
                        ST(V_WEAPON_WARNED) = 0;
                        CB->dialogue("L13D24", P_ARKHAN2);
                    }
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
}

/* trigger 05 — sub_100088B7 */
static void trig05(void) {
    /* 0x88C6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(5)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8920 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8932 */
            if (ST(0x5D4) == 0) {
                ST(0x5D4) = 1;
                CB->animation("SG00PRE07");
                CB->animation("P00_65535");
                CB->etat_objet("OB72", -1, -1, -1, -1, -1, -1, -4, 0);
                CB->etat_objet("OB23", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->joindre(-ST(ST_HERO), 0, 0, 0xAD3E, -1);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 42);
                return;
            } else {
                lieu_action_register("LCOD80", P_ARKHAN2, "SP02ATT60", NULL);
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8A2D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8A3F */
            if (ST(0x5D4) == 0) {
                ST(0x5D4) = 1;
                CB->animation("SG00PRE07");
                CB->etat_objet("OB72", -1, -1, -1, -1, -1, -1, -4, 0);
                CB->etat_objet("OB23", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_50");
                CB->joindre(-ST(ST_HERO), 0, 0, 0xAD3E, -1);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 42);
                return;
            } else {
                lieu_action_register("LCOD80", P_ARKHAN3, "SP03ATT60", NULL);
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 06 — sub_10008B51 */
static void trig06(void) {
    /* 0x8B60 */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_spec(0x6E3B) && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8BBA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8BD4 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x8BE6 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8BF8 */
                    if (ST(0xC90) == 1 && ST(0xD54) == 0) {
                        ST(0xD54) = 1;
                        CB->evenement(0, 3000);
                        lieu_action_register("L13D17", P_ARKHAN2, "SP02ACT96", NULL);
                        return;
                    }
                    /* 0x8C53 */
                    if (ST(0xC90) == 1 && ST(0xD54) > 0) {
                        ST(0xD54) = 2;
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                    /* 0x8CA3 */
                    if (ST(0xC90) == 0) {
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                }
                /* 0x8CD2 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP02ACT95");
                    CB->dialogue("LCOD26", P_ARKHAN2);
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x8D30 */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x8D42 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8D54 */
                    if (ST(0xC90) == 1 && ST(0xD54) == 0) {
                        ST(0xD54) = 1;
                        CB->evenement(0, 3000);
                        lieu_action_register("L13D17", P_ARKHAN2, "SP02ACT92", NULL);
                        return;
                    }
                    /* 0x8DAF */
                    if (ST(0xC90) == 1 && ST(0xD54) > 0) {
                        ST(0xD54) = 2;
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                    /* 0x8DFF */
                    if (ST(0xC90) == 0) {
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                }
                /* 0x8E2E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD26", P_ARKHAN2);
                    CB->animation("SP02ACT91");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
        /* 0x8E84 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8E9E */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x8EB0 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8EC2 */
                    if (ST(0xC90) == 1 && ST(0xD54) == 0) {
                        ST(0xD54) = 1;
                        CB->evenement(0, 3000);
                        lieu_action_register("L13D18", P_ARKHAN3, "SP03ACT96", NULL);
                        return;
                    }
                    /* 0x8F1D */
                    if (ST(0xC90) == 1 && ST(0xD54) > 0) {
                        ST(0xD54) = 2;
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                    /* 0x8F6D */
                    if (ST(0xC90) == 0) {
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                }
                /* 0x8F9C */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD27", P_ARKHAN3);
                    CB->animation("SP03ACT95");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x8FFA */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x900C */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x901E */
                    if (ST(0xC90) == 1 && ST(0xD54) == 0) {
                        ST(0xD54) = 1;
                        CB->evenement(0, 3000);
                        lieu_action_register("L13D18", P_ARKHAN3, "SP03ACT92", NULL);
                        return;
                    }
                    /* 0x9079 */
                    if (ST(0xC90) == 1 && ST(0xD54) > 0) {
                        ST(0xD54) = 2;
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                    /* 0x90C9 */
                    if (ST(0xC90) == 0) {
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                }
                /* 0x90F8 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD27", P_ARKHAN3);
                    CB->animation("SP03ACT91");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
}

/* trigger 07 — sub_10009147 */
static void trig07(void) {
    /* 0x9156 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xA028)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x91BF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x91D1 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x91EB */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    /* 0x91FD */
                    if (ST(0x61C) == 0) {
                        ST(0xB7C) = 1;
                        CB->pos_perso(ST(ST_HERO), 2243, 4750, 0xADD0, 1, "ar", 1, 0);
                        CB->animation("SP02ACT89");
                        CB->play_anim(0, 0);
                        CB->etat_objet("OB41", -1, -1, -1, -1, -1, -1, -3, 0);
                        ST(0x61C) = 1;
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("ar");
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("LCOD79", P_ARKHAN2, "SP02ATT60", NULL);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                } else if (ST(0x61C) == 0) {
                    ST(0xB7C) = 1;
                    CB->pos_perso(ST(ST_HERO), 2243, 4750, 0xADD0, 1, "ar", 1, 0);
                    CB->animation("SP02ACT90");
                    CB->play_anim(0, 0);
                    CB->etat_objet("OB41", -1, -1, -1, -1, -1, -1, -3, 0);
                    ST(0x61C) = 1;
                    CB->animation("P02ACT88");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("ar");
                    CB->bloque(BLOQUE_NONE);
                    return;
                } else {
                    lieu_action_register("LCOD79", P_ARKHAN2, "SP02ATT60", NULL);
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x9403 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x941D */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT05");
                }
                /* 0x9442 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT31");
                }
                lieu_action_register("L13D19", P_ARKHAN2, "SG00DIA03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x9493 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x94A5 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x94BF */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    /* 0x94D1 */
                    if (ST(0x61C) == 0) {
                        ST(0xB7C) = 1;
                        CB->pos_perso(ST(ST_HERO), 2243, 4750, 0xADD0, 1, "ar", 1, 0);
                        CB->animation("SP03ACT89");
                        CB->play_anim(0, 0);
                        CB->etat_objet("OB41", -1, -1, -1, -1, -1, -1, -3, 0);
                        ST(0x61C) = 1;
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("ar");
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("LCOD80", P_ARKHAN3, "SP03ATT60", NULL);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                } else if (ST(0x61C) == 0) {
                    ST(0xB7C) = 1;
                    CB->pos_perso(ST(ST_HERO), 2243, 4750, 0xADD0, 1, "ar", 1, 0);
                    CB->animation("SP03ACT90");
                    CB->play_anim(0, 0);
                    CB->etat_objet("OB41", -1, -1, -1, -1, -1, -1, -3, 0);
                    ST(0x61C) = 1;
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("ar");
                    CB->bloque(BLOQUE_NONE);
                    return;
                } else {
                    lieu_action_register("LCOD80", P_ARKHAN3, "SP03ATT60", NULL);
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x96CC */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L13D20", P_ARKHAN2, "SG00DIA03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 08 — sub_1000970B */
static void trig08(void) {
    /* 0x971A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x37D1)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9774 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D03", P_ARKHAN2);
        }
        /* 0x9796 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D04", P_ARKHAN3);
        }
        /* 0x97B8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D05", P_ARKHAN2);
        }
        /* 0x97DA */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D06", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 09 — sub_1000981E */
static void trig09(void) {
    /* 0x982D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(9)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9887 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D03", P_ARKHAN2);
        }
        /* 0x98A9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D04", P_ARKHAN3);
        }
        /* 0x98CB */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D05", P_ARKHAN2);
        }
        /* 0x98ED */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D06", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 10 — sub_10009931 */
static void trig10(void) {
    /* 0x9940 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        CB->divers(DV_MUSIC, 1, 4);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("Sar_20");
        CB->pos_perso(ST(ST_HERO), 2054, 4668, 0x8214, 1, "ar", 1, 0);
        CB->evenement(2, 500);
        /* 0x99F2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x9A0C */
            if (!CB->divers(DV_IN_ZONE, P_ARKHAN2, 9)) {
                CB->animation("SP02ATT82");
            } else {
                CB->animation("SP02ATT83");
            }
        }
        /* 0x9A39 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x9A53 */
            if (!CB->divers(DV_IN_ZONE, P_ARKHAN2, 9)) {
                CB->animation("SP03ATT82");
                return;
            }
            CB->animation("SP03ATT83");
        }
    }
}

/* trigger 11 — sub_10009A79 */
static void trig11(void) {
    /* 0x9A88 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9ADF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x9AF9 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D13A", P_ARKHAN2, "SP02ATT94", NULL);
            } else {
                lieu_action_register("L13D13A", P_ARKHAN2, "SP02ATT95", NULL);
            }
            CB->dialogue("L13D13B", P_ARKHAN2);
        }
        /* 0x9B4C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x9B66 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D14", P_ARKHAN3, "SP03ATT94", NULL);
            } else {
                lieu_action_register("L13D14", P_ARKHAN3, "SP03ATT95", NULL);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 — sub_10009BCB */
static void trig12(void) {
    /* 0x9BDA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xBB80)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9C34 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D21", P_ARKHAN2);
        }
        /* 0x9C56 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D22", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 13 — sub_10009C9A */
static void trig13(void) {
    /* 0x9CA9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(13)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9D03 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x9D1D */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP02ACT70");
                /* 0x9D37 */
                if (ST(0x6C) == 0) {
                    ST(0x6C) = 1;
                    lieu_action_register("L13D03", P_ARKHAN2, "P02ACT74", "SP02ACT76");
                } else {
                    ST(0x6C) = 0;
                    lieu_action_register("L13D05", P_ARKHAN2, "P02ACT74", "SP02ACT76");
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                CB->animation("SP02ACT71");
                /* 0x9DCE */
                if (ST(0x6C) == 0) {
                    ST(0x6C) = 1;
                    lieu_action_register("L13D03", P_ARKHAN2, "P02ACT75", "SP02ACT77");
                } else {
                    ST(0x6C) = 0;
                    lieu_action_register("L13D05", P_ARKHAN2, "P02ACT75", "SP02ACT77");
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x9E58 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x9E72 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP03ACT70");
                /* 0x9E8C */
                if (ST(0x6C) == 0) {
                    ST(0x6C) = 1;
                    lieu_action_register("L13D04", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                } else {
                    ST(0x6C) = 0;
                    lieu_action_register("L13D06", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                CB->animation("SP03ACT71");
                /* 0x9F23 */
                if (ST(0x6C) == 0) {
                    ST(0x6C) = 1;
                    lieu_action_register("L13D04", P_ARKHAN3, "P03ACT75", "SP03ACT77");
                } else {
                    ST(0x6C) = 0;
                    lieu_action_register("L13D06", P_ARKHAN3, "P03ACT75", "SP03ACT77");
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 — sub_10009FDA */
static void trig15(void) {
    /* 0x9FE9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA043 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA05D */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D11A", P_ARKHAN2, "SP02ATT94", NULL);
            } else {
                lieu_action_register("L13D11A", P_ARKHAN2, "SP02ATT95", NULL);
            }
            /* 0xA0A0 */
            if (ST(ST_LOCKED_CAMERA) == 3) {
                CB->dialogue("L13D11B", P_ARKHAN2);
            }
        }
        /* 0xA0C2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xA0DC */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D12A", P_ARKHAN3, "SP03ATT94", NULL);
            } else {
                lieu_action_register("L13D12A", P_ARKHAN3, "SP03ATT95", NULL);
            }
            /* 0xA11F */
            if (ST(ST_LOCKED_CAMERA) == 3) {
                CB->dialogue("L13D12B", P_ARKHAN3);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}
