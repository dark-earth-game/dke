/*
 * lieu35_trig.c - Native hand-port of the LIEU35.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu35.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * Most handlers are copies of one "press action at a spot" template
 * (busy, bloque, animation, random(3) of three dialogue pairs) that differ
 * only in the zone / angle they test and the animation they play.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu35_int.h"

static void trig01(void); static void trig02(void); static void trig04(void);
static void trig05(void); static void trig07(void); static void trig09(void);
static void trig11(void); static void trig13(void); static void trig15(void);
static void trig17(void); static void trig19(void); static void trig21(void);
static void trig23(void); static void trig24(void); static void trig25(void);
static void trig26(void); static void trig27(void); static void trig29(void);
static void trig30(void); static void trig31(void); static void trig34(void);
static void trig35(void); static void trig37(void); static void trig38(void);
static void trig39(void); static void trig40(void); static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* trigger table at 0x1001B7A0 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 100073F0   1 10008738   2 1000876E   3 stub       4 10008890
 *   5 10008992   6 stub       7 10008BB7   8 stub       9 10008D66
 *  10 stub      11 10008E40  12 stub      13 10008FEF  14 stub
 *  15 10009202  16 stub      17 100093ED  18 stub      19 100095ED
 *  20 stub      21 100097DF  22 stub      23 100099CE  24 10009B7D
 *  25 10009D2C  26 10009EA2  27 1000A046  28 stub      29 1000A1F5
 *  30 1000A399  31 1000A53D  32 stub      33 stub      34 1000A70A
 *  35 1000A7EC  36 stub      37 1000A99B  38 1000AB3F  39 1000ACE3
 *  40 1000AE87  41 10008050
 */
static void (*const g_trigger[42])(void) = {
    l35_trig00, trig01, trig02, trig_noop, trig04, trig05, trig_noop, trig07,
    trig_noop, trig09, trig_noop, trig11, trig_noop, trig13, trig_noop,
    trig15, trig_noop, trig17, trig_noop, trig19, trig_noop, trig21,
    trig_noop, trig23, trig24, trig25, trig26, trig27, trig_noop, trig29,
    trig30, trig31, trig_noop, trig_noop, trig34, trig35, trig_noop, trig37,
    trig38, trig39, trig40, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001B7A0]). */
void l35_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* One ambient creature of trigger 0's block: the original evaluates
 * random(65535) first, then random(50). */
static void place_ambient(i32 x, i32 z) {
    i32 a = CB->random(65535);
    i32 y = CB->random(50) - 3004;
    CB->place_animal(3, x, y, z, a, 1000);
}

/* trigger 1 - trig01_10008738 */
static void trig01(void) {
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 35;
    CB->change_piece(37, 0);
}

/* trigger 2 - trig02_1000876E */
static void trig02(void) {
    /* 0x877D */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_spec(0x4E20)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->pos_perso(ST(ST_HERO), 4570, 5925, 0x4A38, 1, "ar", 6, 0);
        /* 0x87E4 */
        if (ST(ST_PLAYER_WEAPON) == 0) {
            /* 0x87F6 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02MAR89");
            }
            /* 0x8813 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03MAR89");
            }
        }
        /* 0x8830 */
        if (ST(ST_PLAYER_WEAPON) != 0) {
            /* 0x8842 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02MAR91");
            }
            /* 0x885F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03MAR91");
            }
        }
        CB->evenement(1, 5000);
    }
}

/* trigger 4 - trig04_10008890 */
static void trig04(void) {
    /* 0x889F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_MALROT6) && ST(0x4E0) == 0) {
        CB->fouille(P_MALROT6);
        ST(0x4E0) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8925 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L35D03", P_ARKHAN2, "SG00DIA01", "Sar_10");
        }
        /* 0x8950 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L35D04", P_ARKHAN3, "SG00DIA01", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 5 - trig05_10008992 */
static void trig05(void) {
    /* 0x89A1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && (CB->direction_spec(0x88B8) || CB->direction_spec(0xC350))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x8A25 */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->animation("SG00PRE02_10");
        }
        /* 0x8A42 */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->animation("SG00PRE03_10");
        }
        /* 0x8A5F */
        if (ST(V_DLG_RANDOM) == 2) {
            CB->animation("SG00PRE09_10");
        }
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x8A93 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0x8AA5 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0x8AC7 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0x8AE9 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0x8AFB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0x8B1D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0x8B3F */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0x8B51 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0x8B73 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 7 - trig07_10008BB7 */
static void trig07(void) {
    /* 0x8BC6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(8)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x8C42 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0x8C54 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0x8C76 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0x8C98 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0x8CAA */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0x8CCC */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0x8CEE */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0x8D00 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0x8D22 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 9 - trig09_10008D66 */
static void trig09(void) {
    /* 0x8D75 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(10)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE09_10");
        /* 0x8DDA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L35D07", P_ARKHAN2);
        }
        /* 0x8DFC */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L35D08", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 11 - trig11_10008E40 */
static void trig11(void) {
    /* 0x8E4F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(12)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x8ECB */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0x8EDD */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0x8EFF */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0x8F21 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0x8F33 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0x8F55 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0x8F77 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0x8F89 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0x8FAB */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 13 - trig13_10008FEF */
static void trig13(void) {
    /* 0x9002 */
    if (CB->direction_zone(14)) {
        /* 0x9011 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(14)) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            CB->animation("SG00PRE03_10");
            ST(V_DLG_RANDOM) = CB->random(3);
            /* 0x908D */
            if (ST(V_DLG_RANDOM) == 0) {
                /* 0x909F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L35D100", P_ARKHAN2);
                }
                /* 0x90C1 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L35D101", P_ARKHAN3);
                }
            }
            /* 0x90E3 */
            if (ST(V_DLG_RANDOM) == 1) {
                /* 0x90F5 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L35D102", P_ARKHAN2);
                }
                /* 0x9117 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L35D103", P_ARKHAN3);
                }
            }
            /* 0x9139 */
            if (ST(V_DLG_RANDOM) == 2) {
                /* 0x914B */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L35D104", P_ARKHAN2);
                }
                /* 0x916D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L35D105", P_ARKHAN3);
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x91C7 */
    if (CB->direction_zone(16) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 15)) {
        trig15();
    }
}

/* trigger 15 - trig15_10009202 */
static void trig15(void) {
    /* 0x9215 */
    if (CB->direction_zone(16) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0x9267 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x9279 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D09", P_ARKHAN2);
            }
            /* 0x929B */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D10", P_ARKHAN3);
            }
            CB->animation("sar_100");
        }
        /* 0x92C8 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x92DA */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x92F4 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP02ACT01");
                } else {
                    CB->animation("SP02ACT06");
                }
                CB->dialogue("L35D11", P_ARKHAN2);
            }
            /* 0x9331 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x934B */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT01");
                } else {
                    CB->animation("SP03ACT06");
                }
                CB->dialogue("L35D12", P_ARKHAN3);
            }
        }
        CB->animation("ar_100");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x93B2 */
    if (CB->direction_zone(14) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 13)) {
        trig13();
    }
}

/* trigger 17 - trig17_100093ED */
static void trig17(void) {
    /* 0x9400 */
    if (CB->direction_zone(18) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x9478 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0x948A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0x94AC */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0x94CE */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0x94E0 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0x9502 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0x9524 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0x9536 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0x9558 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x95B2 */
    if (CB->direction_zone(20) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 19)) {
        trig19();
    }
}

/* trigger 19 - trig19_100095ED */
static void trig19(void) {
    /* 0x9600 */
    if (CB->direction_zone(20) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x9678 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0x968A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0x96AC */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0x96CE */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0x96E0 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0x9702 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0x9724 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0x9736 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0x9758 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x97A4 */
    if (CB->direction_zone(18) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 17)) {
        trig17();
    }
}

/* trigger 21 - trig21_100097DF */
static void trig21(void) {
    /* 0x97EE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(22)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9848 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x985A */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L35D15", P_ARKHAN2, "SG00ATT01", NULL);
            }
            /* 0x9882 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x989C */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("L35D16", P_ARKHAN2, "SP02ACT85", "Sar_10");
                }
                /* 0x98CF */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("L35D16", P_ARKHAN2, "SP02ACT86", "Sar_10");
                }
            }
        }
        /* 0x98FA */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x990C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L35D17", P_ARKHAN3, "SG00ATT01", NULL);
            }
            /* 0x9934 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x994E */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("L35D18", P_ARKHAN3, "SP03ACT85", "Sar_10");
                }
                /* 0x9981 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    lieu_action_register("L35D18", P_ARKHAN3, "SP03ACT86", "Sar_10");
                }
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 23 - trig23_100099CE */
static void trig23(void) {
    /* 0x99DD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x4650)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE08_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x9A59 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0x9A6B */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0x9A8D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0x9AAF */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0x9AC1 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0x9AE3 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0x9B05 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0x9B17 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0x9B39 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 24 - trig24_10009B7D */
static void trig24(void) {
    /* 0x9B8C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xD2F0)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE08_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x9C08 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0x9C1A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0x9C3C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0x9C5E */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0x9C70 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0x9C92 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0x9CB4 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0x9CC6 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0x9CE8 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 25 - trig25_10009D2C */
static void trig25(void) {
    /* 0x9D3B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xBB80)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9D86 */
        if (ST(0x698) == 1) {
            CB->animation("SG00PRE09_10");
            /* 0x9DA3 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D19", P_ARKHAN2);
            }
            /* 0x9DC5 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D20", P_ARKHAN3);
            }
            CB->animation("Sar_50");
        }
        /* 0x9DF2 */
        if (ST(0x698) == 0) {
            ST(0x698) = 1;
            CB->animation("SG00PRE08_10");
            /* 0x9E1E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D21", P_ARKHAN2);
            }
            /* 0x9E40 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D22", P_ARKHAN3);
            }
            CB->etat_objet("OB72", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("Sar_50");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 26 - trig26_10009EA2 */
static void trig26(void) {
    /* 0x9EB1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xDAC0)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x9F2D */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0x9F3F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0x9F61 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0x9F83 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0x9F95 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0x9FB7 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0x9FD9 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0x9FEB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xA00D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 27 - trig27_1000A046 */
static void trig27(void) {
    /* 0xA055 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(28)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE09_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0xA0D1 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0xA0E3 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0xA105 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0xA127 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0xA139 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0xA15B */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0xA17D */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0xA18F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xA1B1 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 29 - trig29_1000A1F5 */
static void trig29(void) {
    /* 0xA204 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xD2F0)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE02_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0xA280 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0xA292 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0xA2B4 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0xA2D6 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0xA2E8 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0xA30A */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0xA32C */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0xA33E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xA360 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 30 - trig30_1000A399 */
static void trig30(void) {
    /* 0xA3A8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xCB20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0xA424 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0xA436 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0xA458 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0xA47A */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0xA48C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0xA4AE */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0xA4D0 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0xA4E2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xA504 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 31 - trig31_1000A53D */
static void trig31(void) {
    /* 0xA54C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && (CB->direction_zone(32) || CB->direction_spec(0x3A98))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0xA5DB */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0xA5ED */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0xA60F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0xA631 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0xA643 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0xA665 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0xA687 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0xA699 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xA6BB */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 34 - trig34_1000A70A */
static void trig34(void) {
    /* 0xA719 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && (CB->direction_spec(0x5DC0) || CB->direction_spec(0x3A98))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE02_10");
        /* 0xA791 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L35D23", P_ARKHAN2);
        }
        /* 0xA7B3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L35D24", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 35 - trig35_1000A7EC */
static void trig35(void) {
    /* 0xA7FB */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(36)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0xA877 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0xA889 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0xA8AB */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0xA8CD */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0xA8DF */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0xA901 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0xA923 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0xA935 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xA957 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 37 - trig37_1000A99B */
static void trig37(void) {
    /* 0xA9AA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xCB20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0xAA26 */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0xAA38 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0xAA5A */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0xAA7C */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0xAA8E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0xAAB0 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0xAAD2 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0xAAE4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xAB06 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 38 - trig38_1000AB3F */
static void trig38(void) {
    /* 0xAB4E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xCB20)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0xABCA */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0xABDC */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0xABFE */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0xAC20 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0xAC32 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0xAC54 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0xAC76 */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0xAC88 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xACAA */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 39 - trig39_1000ACE3 */
static void trig39(void) {
    /* 0xACF2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xAFC8)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SG00PRE03_10");
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0xAD6E */
        if (ST(V_DLG_RANDOM) == 0) {
            /* 0xAD80 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D100", P_ARKHAN2);
            }
            /* 0xADA2 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D101", P_ARKHAN3);
            }
        }
        /* 0xADC4 */
        if (ST(V_DLG_RANDOM) == 1) {
            /* 0xADD6 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D102", P_ARKHAN2);
            }
            /* 0xADF8 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D103", P_ARKHAN3);
            }
        }
        /* 0xAE1A */
        if (ST(V_DLG_RANDOM) == 2) {
            /* 0xAE2C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L35D104", P_ARKHAN2);
            }
            /* 0xAE4E */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L35D105", P_ARKHAN3);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 40 - trig40_1000AE87 */
static void trig40(void) {
    /* 0xAE96 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(92)) == VIE_DEAD && CB->direction_perso(92)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("Sar_10");
        /* 0xAF0D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L35D25", P_ARKHAN2);
        }
        /* 0xAF2F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L35D26", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 41 - trig41_10008050 */
static void trig41(void) {
    CB->divers_empile(DV_MUSIC, 1, 1);
    /* 0x8074 */
    if (ST(V_EVENT_53) == 1) {
        ST(V_EVENT_53) = 2;
        CB->evenement(0, 750);
        /* 0x80A2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x80B4 */
            if (ST(ST_PLAYER_WEAPON) == 0) {
                CB->animation("SP02MAR90");
            }
            /* 0x80D1 */
            if (ST(ST_PLAYER_WEAPON) != 0) {
                CB->animation("SP02MAR92");
            }
            CB->vire_col(-23, 0);
            CB->evenement(53, 9000);
        }
        /* 0x810B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x811D */
            if (ST(ST_PLAYER_WEAPON) == 0) {
                CB->animation("SP03MAR90");
            }
            /* 0x813A */
            if (ST(ST_PLAYER_WEAPON) != 0) {
                CB->animation("SP03MAR92");
            }
            CB->vire_col(-23, 0);
            CB->evenement(53, 6250);
        }
    }
}

/* trigger 0 */
void l35_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(35)) == 1) {
        ST(V_HUD_TIMER_MS) = 600000;
        ST(0xC28) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 30;
    }
    /* 0x744D */
    if (ST(ST_START_POINT(35)) == 2) {
        ST(V_HUD_TIMER_MS) = 600000;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 37;
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(0xF40) = 1200000;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_53) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_EVENT_33) = 0;
    ST(V_EVENT_34) = 0;
    ST(V_EVENT_35) = 0;
    ST(V_EVENT_36) = 0;
    ST(V_EVENT_37) = 0;
    ST(V_EVENT_38) = 0;
    ST(V_EVENT_39) = 0;
    ST(V_EVENT_41) = 0;
    CB->bruitage("L35AMB1", -1, -1);
    /* 0x75A3 */
    if (ST(ST_LEVEL) == 30) {
        CB->divers_empile(DV_SCREEN, 1, 0);
    }
    CB->ajoute_zone(1, 2155, 10667, 350, 275, 1);
    CB->ajoute_zone(2, 4560, 5867, 100, 100, 0);
    CB->ajoute_zone(5, 4780, 5700, 250, 300, 0);
    CB->ajoute_zone(7, 4422, 5970, 250, 250, 0);
    CB->ajoute_zone(8, 4422, 5970, 40, 40, 0);
    CB->ajoute_zone(9, 4554, 6222, 250, 250, 0);
    CB->ajoute_zone(10, 4554, 6222, 40, 40, 0);
    CB->ajoute_zone(11, 4314, 6670, 200, 200, 0);
    CB->ajoute_zone(12, 4314, 6670, 40, 40, 0);
    CB->ajoute_zone(13, 3677, 7055, 200, 200, 0);
    CB->ajoute_zone(14, 3677, 7055, 40, 40, 0);
    CB->ajoute_zone(15, 3840, 7290, 400, 400, 0);
    CB->ajoute_zone(16, 3820, 7290, 50, 50, 0);
    CB->ajoute_zone(17, 3373, 7200, 300, 180, 0);
    CB->ajoute_zone(18, 3773, 7200, 40, 40, 0);
    CB->ajoute_zone(19, 3363, 7320, 160, 160, 0);
    CB->ajoute_zone(20, 3363, 7320, 40, 40, 0);
    CB->ajoute_zone(21, 3270, 7565, 230, 200, 0);
    CB->ajoute_zone(22, 3270, 7565, 40, 40, 0);
    CB->ajoute_zone(23, 3320, 8010, 155, 185, 0);
    CB->ajoute_zone(25, 3450, 8175, 160, 230, 0);
    CB->ajoute_zone(27, 3254, 8300, 170, 200, 0);
    CB->ajoute_zone(28, 3254, 8300, 40, 80, 0);
    CB->ajoute_zone(24, 3430, 8900, 180, 340, 0);
    CB->ajoute_zone(29, 3433, 8563, 160, 160, 0);
    CB->ajoute_zone(31, 3263, 8583, 245, 235, 0);
    CB->ajoute_zone(32, 3263, 8583, 40, 70, 0);
    CB->ajoute_zone(34, 3200, 7300, 180, 185, 0);
    CB->ajoute_zone(35, 3233, 9280, 215, 230, 0);
    CB->ajoute_zone(36, 3233, 9280, 40, 40, 0);
    CB->ajoute_zone(37, 3376, 9506, 130, 335, 0);
    CB->ajoute_zone(38, 3300, 9654, 170, 270, 0);
    CB->ajoute_zone(39, 2694, 10175, 200, 200, 0);
    CB->ajoute_zone(30, 2534, 10370, 185, 145, 0);
    CB->ajoute_zone(26, 2710, 10370, 145, 140, 0);
    CB->evenement(EV_HUD_TIMER, 10000);
    /* 0x7A0F */
    if (ST(ST_LEVEL) == 30) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7A38 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(ST(ST_HERO), 4555, 5890, 0x59D8, 1, "P02MAR90", 6, 0);
        }
        /* 0x7A75 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(ST(ST_HERO), 4555, 5890, 0x59D8, 1, "P03MAR90", 6, 0);
        }
        /* 0x7AB2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(98, 4555, 5890, 0x59D8, 1, "SP02MAR90", 6, 0);
        }
        /* 0x7AE9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(99, 4555, 5890, 0x59D8, 1, "SP03MAR90", 6, 0);
        }
        ST(V_EVENT_53) = 1;
    }
    /* 0x7B2C */
    if (ST(ST_LEVEL) == 37) {
        CB->vire_col(-23, 0);
        /* 0x7B4B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(ST(ST_HERO), 2355, 10551, 0xA93F, 1, "ar", 6, 0);
        }
        /* 0x7B88 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(ST(ST_HERO), 2355, 10551, 0xA93F, 1, "ar", 6, 0);
        }
        /* 0x7BC8 */
        if (ST(V_SOB86_ACTIVE) != 666) {
            /* 0x7BDA */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->pos_perso(98, 2355, 10551, 0xA93F, 1, "ar", 6, 0);
            }
            /* 0x7C11 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->pos_perso(99, 2355, 10551, 0xA93F, 1, "ar", 6, 0);
            }
        }
    }
    /* 0x7C48 */
    if (ST(ST_VIE(92)) < 2) {
        /* 0x7C5A */
        if (ST(0x908) == 0) {
            CB->pos_perso(92, 3130, 75600, 0x9858, 1, "p92mar01", 0x28, 600);
        } else {
            CB->pos_perso(92, 3130, 7560, 0x9858, 1, "p92mar01", 0x28, 600);
        }
    }
    /* 0x7CC1 */
    if (ST(ST_VIE(92)) == VIE_DEAD) {
        {
            /* the original pushes the z lookup first, then y, then x */
            i32 z = CB->divers(DV_DESC_POS, 92, 2);
            i32 y = CB->divers(DV_DESC_POS, 92, 1);
            i32 x = CB->divers(DV_DESC_POS, 92, 0);
            CB->pos_perso(92, x, y, z, 2, "p92cad", 0x28, 400);
        }
    }
    /* 0x7D26 */
    if (ST(0xC28) >= 2 || ST(0xF24) == 3) {
        ST(0xC28) = 3;
        CB->pos_perso(P_MALROT6, 3900, 7450, 0x60E0, 1, "P66ATT01", 4, 0);
        CB->etat_objet("OB90", 3900, -3300, 7500, 0, 0, 0, 0, 2);
    }
    place_ambient(3430, 7437);
    place_ambient(3430, 7437);
    place_ambient(3430, 7437);
    place_ambient(3430, 7437);
    place_ambient(3430, 7437);
    place_ambient(3430, 7437);
    place_ambient(3430, 7437);
    place_ambient(3430, 7437);
    place_ambient(3430, 7437);
    place_ambient(4616, 5900);
    place_ambient(4616, 5874);
    place_ambient(4616, 5874);
    CB->divers(DV_MUSIC, 1, 0);
    ST(ST_START_POINT(35)) = 0;
}
