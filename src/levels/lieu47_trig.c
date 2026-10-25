/*
 * lieu47_trig.c - Native hand-port of the LIEU47.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick, events and tick body live in
 * lieu47.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * LIEU47 is room 11's script (L11 dialogue codes, the guard of the cellar
 * door); its slot 41 is a level-specific trigger.
 */

#include <stdio.h>

#include "lieu47_int.h"

static void trig_noop(void);   /* the 33 stub slots */
static void trig01(void); static void trig02(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig12(void);
static void trig13(void); static void trig41(void);

/* dword_10019A20[42] - filled at INIT with the 5-byte jmp thunks; static
 * here. Slots 0-2, 7-9, 12, 13 and 41 are level code; the others are empty
 * stubs (identical to L01 trig06). */
static void (*const g_trigger[42])(void) = {
    l47_trig00, trig01, trig02, trig_noop, trig_noop, trig_noop, trig_noop,
    trig07, trig08, trig09, trig_noop, trig_noop, trig12, trig13,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10019A20]). */
void l47_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* sub_10007120 (thunk sub_1000102D): the hero hands the guard an item. Its
 * __fastcall args (ecx = hero, edx = 32) and the 3 pushed stack args are
 * never read, so none are passed. */
static void pre_scene_guard(void) {
    CB->animation("SG02PRE80");
    CB->animation("G02PRE81");
    CB->animation("SP32PRE60");
    CB->animation("P32_65535");
    CB->animation("SG02PRE82");
    CB->animation("P02_65535");
    CB->animation("ar_10");
}

/* DLL .data sprintf buffers of trigger 12 (dword_10019820 / dword_10019620). */
static c8 g_buf_a[32];
static c8 g_buf_b[32];

/* trigger 0 - sub_10007183 (thunk sub_10001073; INIT default) */
void l47_trig00(void) {
    /* 0x7195 */
    if (ST(ST_START_POINT(11)) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 10;
        ST(0xAF8) = 0;
        ST(V_GUARD_FIGHT_DONE) = 0;
    }
    /* 0x71E0 */
    if (ST(ST_START_POINT(11)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 10;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    /* 0x722B */
    if (ST(ST_START_POINT(11)) == 3) {
        ST(ST_HERO) = P_ARKHAN3;
        ST(ST_LEVEL) = 10;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
    }
    ST(ST_START_POINT(11)) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    CB->bruitage("L11AMB1", -1, -1);
    CB->ajoute_zone(1, 5660, -1340, 90, 500, 1);
    CB->ajoute_zone(2, 7596, -1250, 115, 74, 0);
    CB->ajoute_zone(8, 6259, -1697, 245, 230, 0);
    CB->ajoute_zone(9, 6233, -991, 245, 230, 0);
    CB->ajoute_zone(10, 6268, -1707, 50, 50, 0);
    CB->ajoute_zone(11, 6269, -992, 50, 50, 0);
    CB->ajoute_zone(13, 6585, -1327, 95, 190, 1);
    /* 0x73B7 */
    if (ST(ST_LEVEL) == 10) {
        CB->pos_perso(ST(ST_HERO), 5750, -1340, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
    }
    /* 0x7405 */
    if (ST(ST_LEVEL) == 12) {
        CB->pos_perso(ST(ST_HERO), 7535, -1259, 0x9AA8, 1, "ar", 6, 0);
    }
    /* 0x7442 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0) {
        /* 0x7478 */
        if (ST(0xB98) == 0) {
            ST(0xB98) = 2;
        } else if (ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
            ST(0xB98) = 0;
        }
    }
    /* 0x74D1 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }
    /* 0x7537 */
    if (ST(V_GUARD_FIGHT_DONE) != 2) {
        CB->vire_col(7, 1);
        CB->ajoute_zone(7, 6333, -1300, 105, 140, 0);
        /* 0x7577 */
        if (ST(0xAF4) == 1) {
            CB->pos_perso(P_LORY, 4288, -2702, 0xBDD0, 1, "P15ARR01", 0x1A, 0);
            CB->pos_perso(P_RYLSADHAR20, 4454, -2713, 0x4266, 1, "P20ARR01", 0x1B, 0);
        }
    }
    /* 0x75D3 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->vire_col(7, 0);
        /* 0x75F2 */
        if (ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
            CB->pos_perso(P_GUARD, 6373, -1329, 0x3C62, 1, "P32ARR04", 0xC, 601);
        }
    }
    /* 0x762C */
    if (ST(ST_VIE(P_GUARD)) == VIE_DEAD) {
        CB->pos_perso(P_GUARD, 0, 0, 0, 2, "P32cad", 0xC, 0);
    }
    /* 0x765A */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->load_anim("LEVIER", 0);
    }
}

/* trigger 41 - sub_10007672 */
static void trig41(void) {
    /* 0x7684 */
    if (ST(0x4E4) == 1 && ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = P_GUARD;
        CB->combat(P_GUARD, 0);
    }
}

/* trigger 1 - sub_10007FFE */
static void trig01(void) {
    ST(ST_LEVEL) = 11;
    /* 0x801C */
    if (ST(0x4E4) == 1 && ST(ST_SCRIPT_BUSY) == 1 && ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
        CB->dialogue("L11D45", P_GUARD);
    }
    /* 0x805F */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->change_piece(10, -1);
    }
    /* 0x8081 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 0) {
        CB->change_piece(10, 1);
    }
    /* 0x80B5 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 1) {
        CB->change_piece(10, 2);
    }
    /* 0x80E9 */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 2) {
        CB->change_piece(10, 3);
    }
    /* 0x811D */
    if (ST(ST_HERO) > 1 && ST(0xB4C) == 3) {
        CB->change_piece(51, -1);
    }
}

/* trigger 2 - sub_1000814A */
static void trig02(void) {
    /* 0x8159 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x6978)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x81B3 */
        if (ST(ST_VIE(P_GUARD)) == VIE_DEAD) {
            /* 0x81CD */
            if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x81DF */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x69BD, 1, "ar", 1, 0);
                    CB->animation("Sar_10");
                    CB->animation("SP02ATT77");
                }
                /* 0x825C */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x69BD, 1, "ar", 1, 0);
                    CB->animation("Sar_10");
                    CB->animation("SP03ATT77");
                }
            }
            /* 0x82E1 */
            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                /* 0x82F3 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x69BD, 1, "ar", 1, 0);
                    CB->animation("Sar_10");
                    CB->animation("SP02ATT76");
                }
                /* 0x8370 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), 0x69BD, 1, "ar", 1, 0);
                    CB->animation("Sar_10");
                    CB->animation("SP03ATT76");
                }
            }
            CB->evenement(1, 500);
            return;
        }
        /* 0x8402 */
        if (ST(ST_VIE(P_GUARD)) != VIE_DEAD && ST(0x78) == 0) {
            /* 0x8423 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                ST(0x78) = 1;
                lieu_action_register("L11D06", P_ARKHAN2, "SG02ATT01", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(0x78), 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8491 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                ST(0x78) = 1;
                lieu_action_register("L11D07", P_ARKHAN3, "SG03ATT01", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(0x78), 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 7 - sub_10008524 */
static void trig07(void) {
    /* 0x8533 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xC924)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x858D */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x859F */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG01OUV13");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD12", P_ARKHAN1, "SG01DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x85F0 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG01OUV53");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD15", P_ARKHAN1, "SG01DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x8641 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8653 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02OUV13");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD13", P_ARKHAN2, "SG02DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x86A4 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02OUV53");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD16", P_ARKHAN2, "SG02DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x86F5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8707 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03OUV13");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD14", P_ARKHAN3, "SG03DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x8758 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03OUV53");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                lieu_action_register("LCOD17", P_ARKHAN3, "SG03DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 8 - sub_100087A2 */
static void trig08(void) {
    /* 0x87B1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_GUARD_FIGHT_DONE) == 2 && CB->direction_zone(10)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x881D */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x882F */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT28");
            }
            /* 0x885E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0x887B */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x88C1 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x88D3 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT29");
            }
            /* 0x8902 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0x891F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT29");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 9 - sub_1000895E */
static void trig09(void) {
    /* 0x896D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_GUARD_FIGHT_DONE) == 2 && CB->direction_zone(11)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x89D9 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x89EB */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT28");
            }
            /* 0x8A1A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0x8A37 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x8A7D */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x8A8F */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT29");
            }
            /* 0x8ABE */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0x8ADB */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT29");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 12 - sub_10008B30 */
static void trig12(void) {
    /* 0x8B3F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARD)) != VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x8BA7 */
        if (ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
            /* 0x8BB9 */
            if (ST(ST_PLAYER_WEAPON) != OB_MEAT && ST(ST_PLAYER_WEAPON) != OB_FULL_GOURD && ST(ST_PLAYER_WEAPON) != OB_VEGETABLE && ST(ST_PLAYER_WEAPON) != OB_ROAST_BIRD && ST(ST_PLAYER_WEAPON) != OB_ROOT && ST(ST_PLAYER_WEAPON) != OB_FISH) {
                /* 0x8C25 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->joindre(ST(ST_HERO), 0, 0, 0, 0x20);
                }
                CB->joindre(P_GUARD, 0, 0, 0, ST(ST_HERO));
                /* 0x8C6D */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8C7F */
                    if (ST(0xC9C) == 0) {
                        lieu_action_register("L11D08", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L11D09", P_GUARD, "SP32DIA59", "SP32ARR04_10");
                        ST(0xC9C) = 1;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x8CF8 */
                    if (ST(0xC9C) == 1) {
                        lieu_action_register("L11D10A", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D10B", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D11A", P_GUARD, "SP32DIA60", "SP32ARR04_10");
                        lieu_action_register("L11D11B", P_GUARD, "SP32DIA60", "SP32ARR04_10");
                        ST(0xC9C) = 2;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x8DA0 */
                    if (ST(0xC9C) == 2) {
                        lieu_action_register("L11D12", P_ARKHAN2, "SG02DIA04", NULL);
                        lieu_action_register("L11D13A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        lieu_action_register("L11D13B", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        CB->animation("SP32GAR05");
                        ST(0xC9C) = 3;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x8E3D */
                    if (ST(0xC9C) == 3) {
                        lieu_action_register("L11D14", P_ARKHAN2, "SG02DIA04", NULL);
                        CB->dialogue("L11D14A", P_GUARD);
                        CB->dialogue("L11D14B", P_GUARD);
                        ST(0xC9C) = 4;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x8EBD */
                    if (ST(0xC9C) == 4) {
                        lieu_action_register("L11D15A", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D15B", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D16A", P_GUARD, "SP32DIA56", "SP32ARR04_10");
                        lieu_action_register("L11D16B", P_GUARD, "SP32DIA56", "SP32ARR04_10");
                        ST(0xC9C) = 5;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x8F65 */
                    if (ST(0xC9C) == 5 && ST(0xEA4) == 0) {
                        lieu_action_register("L11D17A", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D17B", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D17C", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D18A", P_GUARD, "SP32DIA62", "SP32ARR04_10");
                        lieu_action_register("L11D18B", P_GUARD, "SP32DIA62", "SP32ARR04_10");
                        lieu_action_register("L11D19", P_ARKHAN2, "SG02DIA05", NULL);
                        ST(0xEA4) = 1;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x904B */
                    if (ST(0xC9C) == 5 && ST(0xEA4) == 1 && (ST(ST_PLAYER_WEAPON) != OB_MEAT || ST(ST_PLAYER_WEAPON) != OB_FISH)) {
                        lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x90EB */
                    if (ST(0xC9C) == 5 && ST(0xEA4) == 2) {
                        lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        lieu_action_register("L11D21B", P_GUARD, "SP32DIA56", "SP32ARR04_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9180 */
                    if (ST(0xC9C) == 5 && ST(0xEA4) == 3) {
                        lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                        lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x91FC */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x920E */
                    if (ST(0xC9C) == 0) {
                        lieu_action_register("L11D22", P_ARKHAN2, "SG02DIA06", NULL);
                        CB->dialogue("L11D23A", P_GUARD);
                        CB->dialogue("L11D23B", P_GUARD);
                        ST(0xC9C) = 1;
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x928E */
                    if (ST(0xC9C) >= 1) {
                        lieu_action_register("L11D24A", P_ARKHAN2, "SG02DIA02", NULL);
                        lieu_action_register("L11D24B", P_ARKHAN2, "SG02DIA02", NULL);
                        CB->dialogue("L11D25", P_GUARD);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        ST(V_ENGAGED_PERSONA) = P_GUARD;
                        CB->combat(P_GUARD, 0);
                        ST(0x4E4) = 1;
                        return;
                    }
                }
            }
            /* 0x932D */
            if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_PLAYER_WEAPON) == OB_VEGETABLE || ST(ST_PLAYER_WEAPON) == OB_POTATO || ST(ST_PLAYER_WEAPON) == OB_ROOT || ST(ST_PLAYER_WEAPON) == OB_FISH || ST(ST_PLAYER_WEAPON) == OB_MEAT || ST(ST_PLAYER_WEAPON) == OB_RAT || ST(ST_PLAYER_WEAPON) == OB_ROAST_BIRD) && ST(0xEA4) == 0) {
                lieu_action_register("L11D26", P_ARKHAN2, "SG02DIA02", NULL);
                /* 0x93E5 */
                if (ST(ST_PLAYER_WEAPON) == OB_RAT) {
                    CB->dialogue("L11D27", P_GUARD);
                } else {
                    CB->dialogue("L11D28", P_GUARD);
                }
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9445 */
            if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_PLAYER_WEAPON) == OB_VEGETABLE || ST(ST_PLAYER_WEAPON) == OB_POTATO || ST(ST_PLAYER_WEAPON) == OB_ROOT || ST(ST_PLAYER_WEAPON) == OB_FISH || ST(ST_PLAYER_WEAPON) == OB_MEAT || ST(ST_PLAYER_WEAPON) == OB_ROAST_BIRD) && ST(0xEA4) != 0) {
                /* 0x94D5 */
                if (ST(0xEA4) == 1) {
                    lieu_action_register("L11D29", P_ARKHAN2, "SG02DIA02", NULL);
                    CB->dialogue("L11D30", P_GUARD);
                    /* 0x950D */
                    if (ST(ST_PLAYER_WEAPON) == OB_VEGETABLE) {
                        pre_scene_guard();
                    }
                    /* 0x953A */
                    if (ST(ST_PLAYER_WEAPON) == OB_POTATO) {
                        pre_scene_guard();
                    }
                    /* 0x9567 */
                    if (ST(ST_PLAYER_WEAPON) == OB_ROOT) {
                        pre_scene_guard();
                    }
                    /* 0x9594 */
                    if (ST(ST_PLAYER_WEAPON) == OB_FISH) {
                        pre_scene_guard();
                        ST(0xEA4) = 2;
                    }
                    /* 0x95D0 */
                    if (ST(ST_PLAYER_WEAPON) == OB_MEAT) {
                        pre_scene_guard();
                        ST(0xEA4) = 2;
                    }
                    /* 0x960C */
                    if (ST(ST_PLAYER_WEAPON) == OB_ROAST_BIRD) {
                        pre_scene_guard();
                    }
                    CB->animation("P32ACT02");
                    CB->animation("P32ARR04");
                    CB->animation("Sar_20");
                    ST(V_DLG_RANDOM) = CB->random(4);
                    /* 0x9671 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L11D31", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    /* 0x969C */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L11D32", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    /* 0x96C7 */
                    if (ST(V_DLG_RANDOM) == 2) {
                        lieu_action_register("L11D33", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    /* 0x96F2 */
                    if (ST(V_DLG_RANDOM) == 3) {
                        lieu_action_register("L11D34", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    /* 0x971D */
                    if (ST(0xEA4) == 2) {
                        lieu_action_register("L11D35", P_GUARD, "SP32DIA53", "SP32ARR04_10");
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9766 */
                if (ST(0xEA4) >= 2) {
                    lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                    lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                    lieu_action_register("L11D21B", P_GUARD, "SP32DIA56", "SP32ARR04_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x97E9 */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_PLAYER_WEAPON) == OB_RAT && ST(0xEA4) == 1) {
                lieu_action_register("L11D36", P_ARKHAN2, "SG02DIA02", NULL);
                CB->dialogue("L11D37A", P_GUARD);
                CB->dialogue("L11D37B", P_GUARD);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x4E4) = 1;
                ST(V_ENGAGED_PERSONA) = P_GUARD;
                CB->combat(P_GUARD, 0);
                return;
            }
            /* 0x989B */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_PLAYER_WEAPON) == OB_FULL_GOURD && ST(0xEA4) == 2) {
                snprintf(g_buf_a, sizeof(g_buf_a), "OB24%c", ST(ST_USED_VARIANT) + 'A');
                snprintf(g_buf_b, sizeof(g_buf_b), "OB77%c", ST(ST_USED_VARIANT) + 'A');
                lieu_action_register("L11D38", P_ARKHAN2, "SG02DIA02", NULL);
                CB->dialogue("L11D39", P_GUARD);
                pre_scene_guard();
                ST(0x140 + 4 * ST(ST_USED_VARIANT)) = 0;
                CB->link_objet(P_GUARD, g_buf_a, 0x1B, 1);
                CB->animation("P32ACT03");
                CB->link_objet(P_GUARD, g_buf_a, 0x1B, 0);
                CB->link_objet(P_GUARD, g_buf_b, 0x1B, 1);
                CB->animation("P32CADB");
                CB->pos_perso(P_GUARD, 0, 0, 0, 2, "P32CADB", 0xC, 0);
                ST(0xEA4) = 3;
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9A0B */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_PLAYER_WEAPON) == OB_FLASK77 && ST(0xEA4) == 2) {
                lieu_action_register("L11D38", P_ARKHAN2, "SG02DIA02", NULL);
                CB->dialogue("L11D39", P_GUARD);
                pre_scene_guard();
                snprintf(g_buf_a, sizeof(g_buf_a), "OB77%c", ST(ST_USED_VARIANT) + 'A');
                CB->link_objet(P_GUARD, g_buf_a, 0x1B, 1);
                CB->animation("P32ACT04");
                CB->dialogue("L11D40A", P_GUARD);
                CB->dialogue("L11D40B", P_GUARD);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0x4E4) = 1;
                ST(V_ENGAGED_PERSONA) = P_GUARD;
                CB->combat(P_GUARD, 0);
                return;
            }
            /* 0x9B28 */
            if (ST(ST_HERO) == P_ARKHAN2 && (ST(ST_PLAYER_WEAPON) == OB_FULL_GOURD || ST(ST_PLAYER_WEAPON) == OB_FLASK77)) {
                /* 0x9B5E */
                if (ST(0xEA4) != 2 && ST(0xEA4) != 0) {
                    lieu_action_register("L11D38", P_ARKHAN2, "SG02DIA02", NULL);
                    CB->dialogue("L11D42A", P_GUARD);
                    CB->dialogue("L11D42B", P_GUARD);
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9BE1 */
                if (ST(0xEA4) == 0) {
                    lieu_action_register("L11D20", P_ARKHAN2, "SG02DIA05", NULL);
                    lieu_action_register("L11D21A", P_GUARD, "SP32DIA61", "SP32ARR04_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
    /* 0x9C48 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARD)) == VIE_DEAD && ST(ST_COMBAT_STANCE) == 0) {
        CB->vire_col(7, 0);
        CB->bloque(BLOQUE_ALL);
        /* 0x9CB1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->fouille(P_GUARD);
            lieu_action_register("L11D43", P_ARKHAN2, "SG02DIA05", NULL);
        }
        /* 0x9CE4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->fouille(P_GUARD);
            lieu_action_register("L11D44", P_ARKHAN3, "SG03DIA05", NULL);
        }
    }
    /* 0x9D14 */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(ST_SCRIPT_BUSY) = 1;
        CB->vire_col(7, 0);
        CB->dialogue("L11D02", P_GUARD);
        ST(0x4E4) = 1;
        ST(V_ENGAGED_PERSONA) = P_GUARD;
        CB->combat(P_GUARD, 0);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        return;
    }
    /* 0x9DC9 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_GUARD)) != VIE_DEAD && CB->direction_perso(P_GUARD)) {
        ST(V_CRI_USED) = 1;
        /* 0x9E3F */
        if (ST(0x80) != 99) {
            ST(0x80) = 32;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0, 1000);
        return;
    }
}

/* trigger 13 - sub_10009E76 */
static void trig13(void) {
    /* 0x9E85 */
    if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_GUARD)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = P_GUARD;
        CB->dialogue("L11D23B", P_GUARD);
        CB->combat(P_GUARD, 0);
    }
}

