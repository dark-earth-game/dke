/*
 * lieu22_trig.c — Native hand-port of the LIEU22.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu22.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu22_int.h"

static void trig10(void); static void trig13(void); static void trig14(void);
static void trig15(void); static void trig16(void); static void trig17(void);
static void trig18(void); static void trig19(void); static void trig20(void);
static void trig35(void); static void trig41(void);
static void trig_noop(void);   /* the 31 stub slots */

/* dword_1001A850[42] — filled at INIT with the 5-byte jmp thunks; static
 * here. A slot is "stub" when it is the empty handler shared with LIEU01.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  100087D7       28  10008EA5 stub
 *    1  100083F1 stub  15  1000883E       29  10008EB0 stub
 *    2  100083FC stub  16  100088A5       30  10008EBB stub
 *    3  10008407 stub  17  1000890C       31  10008EC6 stub
 *    4  10008412 stub  18  10008973       32  10008ED1 stub
 *    5  1000841D stub  19  100089DA       33  10008EDC stub
 *    6  10008428 stub  20  10008A41       34  10008EE7 stub
 *    7  10008433 stub  21  10008E58 stub  35  10008EF2
 *    8  1000843E stub  22  10008E63 stub  36  100099EE stub
 *    9  10008449 stub  23  10008E6E stub  37  100099F9 stub
 *   10  10008454       24  10008E79 stub  38  10009A04 stub
 *   11  100084C3 stub  25  10008E84 stub  39  10009A0F stub
 *   12  100084CE stub  26  10008E8F stub  40  10009A1A stub
 *   13  100084D9       27  10008E9A stub  41  10007920
 */
static void (*const g_trigger[42])(void) = {
    l22_trig00, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig10, trig_noop, trig_noop, trig13, trig14, trig15,
    trig16, trig17, trig18, trig19, trig20, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig35, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001A850]). */
void l22_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start-point presets, event resets, music, zones, actor placement */
void l22_trig00(void) {
    /* 0x73FB: ST_START_POINT(22) == 0 tests an empty block (no preset) */
    /* 0x7402 */
    if (ST(ST_START_POINT(22)) == 1) {
        ST(ST_LEVEL) = 21;
        ST(ST_HERO) = P_ARKHAN3;
        ST(V_GUARD_FIGHT_DONE) = 1;
        ST(0x330) = 1;
    }
    /* 0x745F */
    if (ST(ST_START_POINT(22)) == 2) {
        ST(V_GUARD_FIGHT_DONE) = 10;
        ST(0x32C) = 10;
        ST(0x330) = 10;
    }
    /* 0x749E */
    if (ST(ST_START_POINT(22)) == 3) {
        ST(ST_LEVEL) = 21;
        ST(ST_HERO) = P_ARKHAN2;
        ST(V_GUARD_FIGHT_DONE) = 0;
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
    /* 0x757F */
    if (ST(V_GUARD_FIGHT_DONE) == 0 || ST(V_GUARD_FIGHT_DONE) == 1) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x75B5 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    /* 0x75D9 */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->bruitage("L22AMB1", 4489, -6753);
    }
    CB->ajoute_zone(10, 3952, -5973, 150, 50, 1);
    /* 0x761E */
    if (ST(V_GUARD_FIGHT_DONE) == 1 && ST(0x32C) == 1 && ST(0x330) == 0) {
        CB->ajoute_zone(13, 4357, -6602, 180, 180, 0);
    }
    /* 0x7675 */
    if (ST(V_GUARD_FIGHT_DONE) == 1 && ST(0x330) == 1 && ST(0x32C) == 0) {
        CB->ajoute_zone(20, 4357, -6602, 180, 180, 0);
    }
    CB->ajoute_zone(14, 4374, -6754, 80, 120, 0);
    CB->ajoute_zone(15, 4439, -6856, 120, 80, 0);
    CB->ajoute_zone(16, 4559, -6856, 120, 80, 0);
    CB->ajoute_zone(17, 4592, -6759, 80, 120, 0);
    CB->ajoute_zone(18, 4544, -6650, 110, 85, 0);
    CB->ajoute_zone(19, 4433, -6657, 120, 80, 0);
    CB->ajoute_zone(21, 4475, -6754, 20, 20, 0);
    CB->ajoute_zone(22, 4475, -6754, 1000, 1000, 0);
    /* 0x77AA */
    if (ST(V_GUARD_FIGHT_DONE) == 1) {
        CB->ajoute_zone(11, 4531, -5943, 20, 20, 0);
        CB->ajoute_zone(12, 4489, -6461, 20, 20, 0);
    }
    /* 0x77EF */
    if (ST(ST_LEVEL) == 21) {
        CB->pos_perso(ST(ST_HERO), 3990, -5854, 0xC000, 1, "ar", 1, 0);
    }
    /* 0x782C */
    if (ST(V_GUARD_FIGHT_DONE) == 1) {
        /* 0x783E */
        if (ST(0x32C) == 1) {
            CB->pos_perso(P_RYLSADHAR21, 4393, -6641, 0x9E24, 1, "P20ARR01", 2, 0);
        }
        /* 0x7875 */
        if (ST(0x330) == 1) {
            CB->pos_perso(P_ZED27, 4393, -6641, 0x9E24, 1, "P27ARR01", 3, 0);
        }
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    /* 0x78E8 */
    if (ST(0xB88) == 0) {
        ST(0xB88) = 1;
        ST(0xE40) = 1;
    }
    ST(ST_START_POINT(22)) = 0;
}

/* trigger 41 — sub_10007920 — music cue (fight-done 0/1 -> track 1, 2 -> track 2) */
static void trig41(void) {
    /* 0x7926 */
    if (ST(V_GUARD_FIGHT_DONE) == 0 || ST(V_GUARD_FIGHT_DONE) == 1) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x795C */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 1);
    }
}

/* trigger 10 — sub_10008454 — exit to level 21: music cue, then change_piece(21, 2) */
static void trig10(void) {
    /* 0x8466 */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0x848A */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 22;
    CB->change_piece(21, 2);
}

/* trigger 13 — sub_100084D9 — action key at the cabinet zone 13: hero comment, one per hero/mode */
static void trig13(void) {
    /* 0x84E8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x853E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8550 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x856A */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT71");
                    lieu_action_register("L22D12A", P_ARKHAN2, "P02ACT75", "SP02ACT75");
                    lieu_action_register("L22D12B", P_ARKHAN2, "SP02ACT75", "SP02ACT77");
                } else {
                    CB->animation("SP02ACT70");
                    lieu_action_register("L22D12A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L22D12B", P_ARKHAN2, "SP02ACT74", "SP02ACT76");
                }
            }
            /* 0x85FB */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x8615 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT73");
                    lieu_action_register("L22D13A", P_ARKHAN2, "P02ACT75", "SP02ACT75");
                    lieu_action_register("L22D13B", P_ARKHAN2, "SP02ACT75", "SP02ACT79");
                } else {
                    CB->animation("SP02ACT72");
                    lieu_action_register("L22D13A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                    lieu_action_register("L22D13B", P_ARKHAN2, "SP02ACT74", "SP02ACT78");
                }
            }
        }
        /* 0x86A6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x86B8 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x86D2 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT71");
                    lieu_action_register("L22D14", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                } else {
                    CB->animation("SP03ACT70");
                    lieu_action_register("L22D14", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                }
            }
            /* 0x8731 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x874B */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT73");
                    lieu_action_register("L22D15", P_ARKHAN3, "P03ACT74", "SP03ACT79");
                } else {
                    CB->animation("SP03ACT72");
                    lieu_action_register("L22D15", P_ARKHAN3, "P03ACT74", "SP03ACT78");
                }
            }
        }
        CB->retire_zone(13);
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 — sub_100087D7 — action key in the zone: opens the cabinet interaction (trigger 35) */
static void trig14(void) {
    /* 0x87E6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(21)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        trig35();
    }
}

/* trigger 15 — sub_1000883E — action key in the zone: opens the cabinet interaction (trigger 35) */
static void trig15(void) {
    /* 0x884D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(21)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        trig35();
    }
}

/* trigger 16 — sub_100088A5 — action key in the zone: opens the cabinet interaction (trigger 35) */
static void trig16(void) {
    /* 0x88B4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(21)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        trig35();
    }
}

/* trigger 17 — sub_1000890C — action key in the zone: opens the cabinet interaction (trigger 35) */
static void trig17(void) {
    /* 0x891B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(21)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        trig35();
    }
}

/* trigger 18 — sub_10008973 — action key in the zone: opens the cabinet interaction (trigger 35) */
static void trig18(void) {
    /* 0x8982 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(21)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        trig35();
    }
}

/* trigger 19 — sub_100089DA — action key in the zone: opens the cabinet interaction (trigger 35) */
static void trig19(void) {
    /* 0x89E9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(21)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        trig35();
    }
}

/* trigger 20 — sub_10008A41 — action key at zone 20: first press comments, second press takes OB58/OB35/OB73A */
static void trig20(void) {
    /* 0x8A50 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8AA3 */
        if (ST(0x64) == 0) {
            ST(0x64) = 1;
            /* 0x8AC1 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x8AD3 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8AED */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->animation("SP02ACT71");
                        lieu_action_register("L22D18A", P_ARKHAN2, "P02ACT75", "SP02ACT75");
                        lieu_action_register("L22D18B", P_ARKHAN2, "SP02ACT75", "SP02ACT77");
                    } else {
                        CB->animation("SP02ACT70");
                        lieu_action_register("L22D18A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L22D18B", P_ARKHAN2, "SP02ACT74", "SP02ACT76");
                    }
                }
                /* 0x8B7E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x8B98 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->animation("SP02ACT73");
                        lieu_action_register("L22D19A", P_ARKHAN2, "P02ACT75", "SP02ACT75");
                        lieu_action_register("L22D19B", P_ARKHAN2, "SP02ACT75", "SP02ACT79");
                    } else {
                        CB->animation("SP02ACT72");
                        lieu_action_register("L22D19A", P_ARKHAN2, "P02ACT74", "SP02ACT74");
                        lieu_action_register("L22D19B", P_ARKHAN2, "SP02ACT74", "SP02ACT78");
                    }
                }
            }
            /* 0x8C29 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x8C3B */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x8C55 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->animation("SP03ACT71");
                        lieu_action_register("L22D20", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                    } else {
                        CB->animation("SP03ACT70");
                        lieu_action_register("L22D20", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                    }
                }
                /* 0x8CB4 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x8CCE */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->animation("SP03ACT73");
                        lieu_action_register("L22D21", P_ARKHAN3, "P03ACT74", "SP03ACT79");
                    } else {
                        CB->animation("SP03ACT72");
                        lieu_action_register("L22D21", P_ARKHAN3, "P03ACT74", "SP03ACT78");
                    }
                }
            }
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x8D53 */
        if (ST(0x64) == 1) {
            ST(ST_SCRIPT_BUSY) = 0;
            CB->bloque(BLOQUE_ALL);
            CB->retire_zone(20);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x604) = 1;
            /* 0x8DA2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02PRE09");
            }
            /* 0x8DBF */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03PRE09");
            }
            CB->etat_objet("OB58", -1, -1, -1, -1, -1, -1, -4, 0);
            CB->etat_objet("OB35", -1, -1, -1, -1, -1, -1, -4, 0);
            CB->etat_objet("OB73A", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 35 — sub_10008EF2 — dialogue on the zone 21 object, by fight stage / hero / mode (called from triggers 14-19) */
static void trig35(void) {
    /* 0x8F04 */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        /* 0x8F16 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8F28 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x8F3A */
                if (ST(0xCE4) == 0) {
                    /* 0x8F54 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        ST(0xCE4) = 1;
                        CB->dialogue("L22D06A", P_ARKHAN2);
                        CB->dialogue("L22D06B", P_ARKHAN2);
                        CB->animation("SP02ACT59");
                        CB->evenement(2, 500);
                        CB->animation("G00CON01");
                        CB->dialogue("L22D06C", P_ARKHAN2);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        ST(0xCE4) = 1;
                        CB->dialogue("L22D06A", P_ARKHAN2);
                        CB->dialogue("L22D06B", P_ARKHAN2);
                        CB->animation("SP02ACT58");
                        CB->evenement(2, 500);
                        CB->animation("G00CON01");
                        CB->dialogue("L22D06C", P_ARKHAN2);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x9087 */
                if (ST(0xCE4) == 1) {
                    lieu_action_register("L22D07A", P_ARKHAN2, "SG02DIA04", NULL);
                    lieu_action_register("L22D07B", P_ARKHAN2, "SG02DIA04", NULL);
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x90EE */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x9100 */
                if (ST(0xCE4) == 0) {
                    /* 0x911A */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        ST(0xCE4) = 1;
                        CB->dialogue("L22D08A", P_ARKHAN2);
                        CB->dialogue("L22D08B", P_ARKHAN2);
                        CB->animation("SP02ACT60");
                        CB->evenement(2, 500);
                        CB->animation("G00CON01");
                        CB->dialogue("L22D06C", P_ARKHAN2);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        ST(0xCE4) = 1;
                        CB->dialogue("L22D08A", P_ARKHAN2);
                        CB->dialogue("L22D08B", P_ARKHAN2);
                        CB->animation("SP02ACT61");
                        CB->evenement(2, 500);
                        CB->animation("G00CON01");
                        CB->dialogue("L22D06C", P_ARKHAN2);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x924D */
                if (ST(0xCE4) == 1) {
                    CB->vire_col(3, 0);
                    CB->vire_col(5, 0);
                    /* 0x9281 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->dialogue("L22D16", P_ARKHAN2);
                        CB->animation("SP02ACT83");
                        CB->evenement(33, 100);
                        return;
                    } else {
                        CB->dialogue("L22D16", P_ARKHAN2);
                        CB->animation("SP02ACT82");
                        CB->evenement(33, 100);
                        return;
                    }
                }
            }
        }
        /* 0x92F8 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x930A */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x931C */
                if (ST(0xCE4) == 0) {
                    /* 0x9336 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        ST(0xCE4) = 1;
                        CB->dialogue("L22D09A", P_ARKHAN3);
                        CB->dialogue("L22D09B", P_ARKHAN3);
                        CB->animation("SP03ACT59");
                        CB->evenement(2, 500);
                        CB->animation("G00CON01");
                        CB->dialogue("L22D09C", P_ARKHAN2);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("ar");
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        ST(0xCE4) = 1;
                        CB->dialogue("L22D09A", P_ARKHAN3);
                        CB->dialogue("L22D09B", P_ARKHAN3);
                        CB->animation("SP03ACT58");
                        CB->evenement(2, 500);
                        CB->animation("ar_50");
                        CB->animation("G00CON01");
                        CB->dialogue("L22D09C", P_ARKHAN2);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("ar");
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x947F */
                if (ST(0xCE4) == 1) {
                    lieu_action_register("L22D10", P_ARKHAN3, "SG03DIA18", NULL);
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->animation("ar");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x94DB */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x94ED */
                if (ST(0xCE4) == 0) {
                    /* 0x9507 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        ST(0xCE4) = 1;
                        CB->dialogue("L22D11", P_ARKHAN3);
                        CB->animation("SP03ACT60");
                        CB->evenement(2, 500);
                        CB->animation("G00CON01");
                        CB->dialogue("L22D09C", P_ARKHAN2);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("ar");
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        ST(0xCE4) = 1;
                        CB->dialogue("L22D11", P_ARKHAN3);
                        CB->animation("SP03ACT61");
                        CB->evenement(2, 500);
                        CB->animation("G00CON01");
                        CB->dialogue("L22D09C", P_ARKHAN2);
                        CB->animation("ar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->animation("ar");
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0x9630 */
                if (ST(0xCE4) == 1) {
                    CB->vire_col(3, 0);
                    CB->vire_col(5, 0);
                    /* 0x9664 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->dialogue("L22D17", P_ARKHAN3);
                        CB->animation("SP03ACT83");
                        CB->evenement(33, 100);
                        return;
                    } else {
                        CB->dialogue("L22D17", P_ARKHAN2);
                        CB->animation("SP03ACT82");
                        CB->evenement(33, 100);
                        return;
                    }
                }
            }
        }
    }
    /* 0x96DB */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        /* 0x96ED */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x96FF */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x9719 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->dialogue("L22D02", P_ARKHAN2);
                    CB->animation("SP02ACT81");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                } else {
                    CB->dialogue("L22D02", P_ARKHAN2);
                    CB->animation("SP02ACT80");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x97B8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x97D2 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->dialogue("L22D03", P_ARKHAN2);
                    CB->animation("SP02ACT67");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                } else {
                    CB->dialogue("L22D03", P_ARKHAN2);
                    CB->animation("SP02ACT66");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
        /* 0x9871 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x9883 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x989D */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->dialogue("L22D04", P_ARKHAN3);
                    CB->animation("SP03ACT81");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                } else {
                    CB->dialogue("L22D04", P_ARKHAN3);
                    CB->animation("SP03ACT80");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x993C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x9956 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->dialogue("L22D05", P_ARKHAN3);
                    CB->animation("SP03ACT67");
                    CB->animation("ar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                    return;
                }
                CB->dialogue("L22D05", P_ARKHAN3);
                CB->animation("SP03ACT66");
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}
