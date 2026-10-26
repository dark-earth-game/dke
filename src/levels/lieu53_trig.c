/*
 * lieu53_trig.c - Native hand-port of the LIEU53.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick, events and tick body live in
 * lieu53.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * LIEU53 is the L13 room script (dialogue codes L13Dnn).
 */

#include <stdio.h>

#include "lieu53_int.h"

static void trig_noop(void);   /* the 27 stub slots */
static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig10(void);
static void trig11(void); static void trig12(void); static void trig13(void);
static void trig15(void); static void trig41(void);

/* dword_10019820[42] - filled at INIT with the 5-byte jmp thunks; static
 * here. Slots 0, 2-13, 15 and 41 are level code; the others are empty
 * stubs (identical to L01 trig06). */
static void (*const g_trigger[42])(void) = {
    l53_trig00, trig_noop, trig02, trig03, trig04, trig05,
    trig06, trig07, trig08, trig09, trig10, trig11,
    trig12, trig13, trig_noop, trig15, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10019820]). */
void l53_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - trig00_100065C0 (thunk sub_1000106E; INIT default) */
void l53_trig00(void) {
    /* 0x65D2 */
    if (ST(0x994) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 49;
    }
    CB->select_pile(2, 2);
    CB->select_pile(1, 2);
    CB->select_pile(0, 2);
    CB->select_pile(0, 1);
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
    /* 0x68EB */
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
    /* 0x697F */
    if (ST(0xC20) == 1) {
        ST(0xC20) = 2;
    }
}

/* trig41 - trig41_10006999 */
static void trig41(void) {
    CB->play_anim(5, 1);
    CB->divers(DV_MUSIC, 1, 1);
    /* 0x69CD */
    if (ST(0xEFC) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_WEAPON_WARNED) = 1;
        ST(0xEFC) = 1;
        /* 0x6A08 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L13D01", P_ARKHAN2, "SG00DIA04", NULL);
        }
        /* 0x6A30 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L13D02", P_ARKHAN3, "SG00DIA04", NULL);
        }
        CB->bloque(BLOQUE_NONE);
    }
}

/* trig02 - trig02_10007086 */
static void trig02(void) {
    /* 0x7095 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xC350)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x70EF */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7109 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D11A", P_ARKHAN2, "SP02ATT94", NULL);
            } else {
                lieu_action_register("L13D11A", P_ARKHAN2, "SP02ATT95", NULL);
            }
            /* 0x714C */
            if (ST(ST_LOCKED_CAMERA) == 3) {
                CB->dialogue("L13D11B", P_ARKHAN2);
            }
        }
        /* 0x716E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7188 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D12A", P_ARKHAN3, "SP03ATT94", NULL);
            } else {
                lieu_action_register("L13D12A", P_ARKHAN3, "SP03ATT95", NULL);
            }
            /* 0x71CB */
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

/* trig03 - trig03_1000720F */
static void trig03(void) {
    /* 0x721E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(3)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7278 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7292 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D13A", P_ARKHAN2, "SP02ATT94", NULL);
            } else {
                lieu_action_register("L13D13A", P_ARKHAN2, "SP02ATT95", NULL);
            }
            CB->dialogue("L13D13B", P_ARKHAN2);
        }
        /* 0x72E5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x72FF */
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

/* trig04 - trig04_10007364 */
static void trig04(void) {
    /* 0x7373 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x73CD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x73E7 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x73F9 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    ST(0x64) = 1;
                    /* 0x7417 */
                    if (ST(0xC8C) < 2) {
                        ST(0xC8C)++;
                        CB->evenement(1, 800);
                        CB->animation("SP02ACT98");
                        CB->animation("ar_50");
                        /* 0x7469 */
                        if (CB->divers(DV_HP, ST(ST_HERO), -1) > 35) {
                            CB->divers_empile(DV_HP, ST(ST_HERO), CB->divers(DV_HP, ST(ST_HERO), -1) - 30);
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
                /* 0x7509 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    ST(0x64) = 2;
                    CB->evenement(1, 400);
                    CB->animation("SP02ACT97");
                    CB->animation("ar_50");
                    /* 0x754D */
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
            /* 0x75AF */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x75C1 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    ST(0x64) = 1;
                    /* 0x75DF */
                    if (ST(0xC8C) < 2) {
                        ST(0xC8C)++;
                        CB->evenement(1, 500);
                        CB->animation("SP02ACT94");
                        CB->animation("ar_50");
                        /* 0x7631 */
                        if (CB->divers(DV_HP, ST(ST_HERO), -1) > 35) {
                            CB->divers_empile(DV_HP, ST(ST_HERO), CB->divers(DV_HP, ST(ST_HERO), -1) - 30);
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
                /* 0x76D1 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    ST(0x64) = 2;
                    CB->evenement(1, 400);
                    CB->animation("SP02ACT93");
                    CB->animation("ar_50");
                    /* 0x7715 */
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
        /* 0x7764 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x777E */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x7790 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    ST(0x64) = 1;
                    /* 0x77AE */
                    if (ST(0xC8C) < 2) {
                        ST(0xC8C)++;
                        CB->evenement(1, 500);
                        CB->animation("SP03ACT98");
                        CB->animation("ar_50");
                        /* 0x7800 */
                        if (CB->divers(DV_HP, ST(ST_HERO), -1) > 35) {
                            CB->divers_empile(DV_HP, ST(ST_HERO), CB->divers(DV_HP, ST(ST_HERO), -1) - 30);
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
                /* 0x78A0 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    ST(0x64) = 2;
                    CB->evenement(1, 400);
                    CB->animation("SP03ACT97");
                    /* 0x78D9 */
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
            /* 0x793B */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x794D */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    ST(0x64) = 1;
                    /* 0x796B */
                    if (ST(0xC8C) < 2) {
                        ST(0xC8C)++;
                        CB->evenement(1, 500);
                        CB->animation("SP03ACT94");
                        CB->animation("ar_50");
                        /* 0x79BD */
                        if (CB->divers(DV_HP, ST(ST_HERO), -1) > 35) {
                            CB->divers_empile(DV_HP, ST(ST_HERO), CB->divers(DV_HP, ST(ST_HERO), -1) - 30);
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
                /* 0x7A5D */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    ST(0x64) = 2;
                    CB->evenement(1, 400);
                    CB->animation("SP03ACT93");
                    /* 0x7A96 */
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

/* trig05 - trig05_10007AE9 */
static void trig05(void) {
    /* 0x7AF8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(5)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7B52 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7B64 */
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
        /* 0x7C5F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7C71 */
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

/* trig06 - trig06_10007D83 */
static void trig06(void) {
    /* 0x7D92 */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_spec(0x6E3B) && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7DEC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7E06 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x7E18 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x7E2A */
                    if (ST(0xC90) == 1 && ST(0xC94) == 0) {
                        ST(0xC94) = 1;
                        CB->evenement(0, 3000);
                        lieu_action_register("L13D17", P_ARKHAN2, "SP02ACT96", NULL);
                        return;
                    }
                    /* 0x7E85 */
                    if (ST(0xC90) == 1 && ST(0xC94) > 0) {
                        ST(0xC94) = 2;
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                    /* 0x7ED5 */
                    if (ST(0xC90) == 0) {
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                }
                /* 0x7F04 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->animation("SP02ACT95");
                    CB->dialogue("LCOD26", P_ARKHAN2);
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x7F62 */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x7F74 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x7F86 */
                    if (ST(0xC90) == 1 && ST(0xC94) == 0) {
                        ST(0xC94) = 1;
                        CB->evenement(0, 3000);
                        lieu_action_register("L13D17", P_ARKHAN2, "SP02ACT92", NULL);
                        return;
                    }
                    /* 0x7FE1 */
                    if (ST(0xC90) == 1 && ST(0xC94) > 0) {
                        ST(0xC94) = 2;
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                    /* 0x8031 */
                    if (ST(0xC90) == 0) {
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                }
                /* 0x8060 */
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
        /* 0x80B6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x80D0 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x80E2 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x80F4 */
                    if (ST(0xC90) == 1 && ST(0xC94) == 0) {
                        ST(0xC94) = 1;
                        CB->evenement(0, 3000);
                        lieu_action_register("L13D18", P_ARKHAN3, "SP03ACT96", NULL);
                        return;
                    }
                    /* 0x814F */
                    if (ST(0xC90) == 1 && ST(0xC94) > 0) {
                        ST(0xC94) = 2;
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                    /* 0x819F */
                    if (ST(0xC90) == 0) {
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                }
                /* 0x81CE */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD27", P_ARKHAN3);
                    CB->animation("SP03ACT95");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x822C */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x823E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8250 */
                    if (ST(0xC90) == 1 && ST(0xC94) == 0) {
                        ST(0xC94) = 1;
                        CB->evenement(0, 3000);
                        lieu_action_register("L13D18", P_ARKHAN3, "SP03ACT92", NULL);
                        return;
                    }
                    /* 0x82AB */
                    if (ST(0xC90) == 1 && ST(0xC94) > 0) {
                        ST(0xC94) = 2;
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                    /* 0x82FB */
                    if (ST(0xC90) == 0) {
                        CB->animation("SG00PRE02");
                        CB->evenement(0, 2000);
                        return;
                    }
                }
                /* 0x832A */
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

/* trig07 - trig07_10008379 */
static void trig07(void) {
    /* 0x8388 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xA028)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x83F1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8403 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x841D */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    /* 0x842F */
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
            /* 0x8635 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x864F */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT05");
                }
                /* 0x8674 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT31");
                }
                lieu_action_register("L13D19", P_ARKHAN2, "SG00DIA03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x86C5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x86D7 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x86F1 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    /* 0x8703 */
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
            /* 0x88FE */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L13D20", P_ARKHAN2, "SG00DIA03", NULL);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trig08 - trig08_1000893D */
static void trig08(void) {
    /* 0x894C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x37D1)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x89A6 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D03", P_ARKHAN2);
        }
        /* 0x89C8 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D04", P_ARKHAN3);
        }
        /* 0x89EA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D05", P_ARKHAN2);
        }
        /* 0x8A0C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D06", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trig09 - trig09_10008A50 */
static void trig09(void) {
    /* 0x8A5F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(9)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8AB9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D03", P_ARKHAN2);
        }
        /* 0x8ADB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D04", P_ARKHAN3);
        }
        /* 0x8AFD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D05", P_ARKHAN2);
        }
        /* 0x8B1F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D06", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trig10 - trig10_10008B63 */
static void trig10(void) {
    /* 0x8B72 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        CB->divers(DV_MUSIC, 1, 4);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("Sar_20");
        CB->pos_perso(ST(ST_HERO), 2054, 4668, 0x8214, 1, "ar", 1, 0);
        CB->evenement(2, 500);
        /* 0x8C24 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8C3E */
            if (!CB->divers(DV_IN_ZONE, P_ARKHAN2, 9)) {
                CB->animation("SP02ATT82");
            } else {
                CB->animation("SP02ATT83");
            }
        }
        /* 0x8C6B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8C85 */
            if (!CB->divers(DV_IN_ZONE, P_ARKHAN2, 9)) {
                CB->animation("SP03ATT82");
                return;
            }
            CB->animation("SP03ATT83");
        }
    }
}

/* trig11 - trig11_10008CAB */
static void trig11(void) {
    /* 0x8CBA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8D11 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8D2B */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D13A", P_ARKHAN2, "SP02ATT94", NULL);
            } else {
                lieu_action_register("L13D13A", P_ARKHAN2, "SP02ATT95", NULL);
            }
            CB->dialogue("L13D13B", P_ARKHAN2);
        }
        /* 0x8D7E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8D98 */
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

/* trig12 - trig12_10008DFD */
static void trig12(void) {
    /* 0x8E0C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xBB80)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8E66 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L13D21", P_ARKHAN2);
        }
        /* 0x8E88 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L13D22", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trig13 - trig13_10008ECC */
static void trig13(void) {
    /* 0x8EDB */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(13)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8F35 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8F4F */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP02ACT70");
                /* 0x8F69 */
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
                /* 0x9000 */
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
        /* 0x908A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x90A4 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                CB->animation("SP03ACT70");
                /* 0x90BE */
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
                /* 0x9155 */
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

/* trig15 - trig15_1000920C */
static void trig15(void) {
    /* 0x921B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9275 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x928F */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D11A", P_ARKHAN2, "SP02ATT94", NULL);
            } else {
                lieu_action_register("L13D11A", P_ARKHAN2, "SP02ATT95", NULL);
            }
            /* 0x92D2 */
            if (ST(ST_LOCKED_CAMERA) == 3) {
                CB->dialogue("L13D11B", P_ARKHAN2);
            }
        }
        /* 0x92F4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x930E */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                lieu_action_register("L13D12A", P_ARKHAN3, "SP03ATT94", NULL);
            } else {
                lieu_action_register("L13D12A", P_ARKHAN3, "SP03ATT95", NULL);
            }
            /* 0x9351 */
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
