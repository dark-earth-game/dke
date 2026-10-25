/*
 * lieu27_trig.c - Native hand-port of the LIEU27.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu27.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu27_int.h"

static void trig02(void); static void trig03(void); static void trig06(void);
static void trig08(void); static void trig09(void); static void trig15(void);
static void trig16(void); static void trig17(void); static void trig18(void);
static void trig19(void); static void trig32(void); static void trig33(void);
static void trig34(void); static void trig35(void); static void trig36(void);
static void trig37(void); static void trig39(void); static void trig40(void);
static void trig41(void);
static void trig_noop(void);   /* the empty stub slots (shared L01 trig06 body) */

/* trigger table at 0x1001D850 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000+5*i; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000A641 stub  28  1000B4F0 stub
 *    1  10009547 stub  15  1000A64C       29  1000B4FB stub
 *    2  10009552       16  1000A883       30  1000B506 stub
 *    3  10009669       17  1000AAEE       31  1000B511 stub
 *    4  100097CA stub  18  1000ADAA       32  1000B51C
 *    5  100097D5 stub  19  1000B10F       33  1000B58B
 *    6  100097E0       20  1000B498 stub  34  1000B6DB
 *    7  10009929 stub  21  1000B4A3 stub  35  1000B7BD
 *    8  10009934       22  1000B4AE stub  36  1000B889
 *    9  1000A043       23  1000B4B9 stub  37  1000BAE2
 *   10  1000A615 stub  24  1000B4C4 stub  38  1000BD29 stub
 *   11  1000A620 stub  25  1000B4CF stub  39  1000BD34
 *   12  1000A62B stub  26  1000B4DA stub  40  1000BEFF
 *   13  1000A636 stub  27  1000B4E5 stub  41  100080D9
 */
static void (*const g_trigger[42])(void) = {
    l27_trig00, trig_noop, trig02, trig03, trig_noop, trig_noop, trig06, trig_noop,
    trig08, trig09, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig15,
    trig16, trig17, trig18, trig19, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig32, trig33, trig34, trig35, trig36, trig37, trig_noop,
    trig39, trig40, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001D850]). */
void l27_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 2 - trig02_10009552 */
static void trig02(void) {
    /* 0x9561 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xC000)) {
        CB->bloque(BLOQUE_ALL);
        g_dest_room = 28;
        /* 0x95AA */
        if (ST(0xB38) == 0) {
            CB->divers(DV_MUSIC, 1, 4);
        }
        /* 0x95CE */
        if (ST(0xB38) != 0) {
            CB->divers(DV_MUSIC, 2, 4);
        }
        /* 0x95F2 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            CB->evenement(0x25, 500);
        }
        /* 0x9614 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->evenement(0x25, 750);
        }
        /* 0x9636 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02OUV01");
        }
        /* 0x9653 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03OUV01");
        }
    }
}

/* trigger 3 - trig03_10009669 */
static void trig03(void) {
    /* 0x9678 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xC000)) {
        CB->bloque(BLOQUE_ALL);
        g_dest_room = 29;
        /* 0x96C1 */
        if (ST(0xB38) == 0) {
            CB->divers(DV_MUSIC, 1, 4);
        }
        /* 0x96E5 */
        if (ST(0xB38) != 0) {
            CB->divers(DV_MUSIC, 2, 4);
        }
        /* 0x9709 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            CB->evenement(0x26, 500);
        }
        /* 0x972B */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->evenement(0x26, 750);
        }
        /* 0x974D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02OUV01");
        }
        /* 0x976A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03OUV01");
        }
        return;
    }
    /* 0x978C */
    if (ST(0xB58) == 5 || ST(0xB58) == 10) {
        ST(0xB58) = 10;
        CB->change_piece(29, 0);
        return;
    }
}

/* trigger 6 - trig06_100097E0 */
static void trig06(void) {
    /* 0x97EF */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        g_dest_room = 20;
        /* 0x9847 */
        if (ST(0xB38) == 0) {
            CB->divers_empile(DV_MUSIC, 1, 4);
        }
        /* 0x986B */
        if (ST(0xB38) != 0) {
            CB->divers_empile(DV_MUSIC, 2, 4);
        }
        /* 0x988F */
        if (ST(ST_SPECIAL_MODE) == 1) {
            CB->evenement(0, 400);
        }
        /* 0x98AE */
        if (ST(ST_SPECIAL_MODE) == 0) {
            CB->evenement(0, 700);
        }
        /* 0x98CD */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->animation("SG01OUV01");
        }
        /* 0x98EA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02OUV01");
        }
        /* 0x9907 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03OUV01");
        }
        ST(ST_LEVEL) = 27;
    }
}

/* trigger 8 - trig08_10009934 */
static void trig08(void) {
    /* 0x9943 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        /* 0x9A11: `if (ST(0x950) == 0) {}` - empty in the original, both paths continue */
        if (ST(0x950) == 0) {
        }
        /* 0x9A18 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x9A3C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L20D30", P_ARKHAN2, "SG02DIA05", NULL);
            }
            /* 0x9A64 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L20D31", P_ARKHAN2, "SG02DIA05", NULL);
            }
        }
        /* 0x9A8C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x9A9E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L20D32", P_ARKHAN3, "SG03DIA05", NULL);
            }
            /* 0x9AC6 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L20D33", P_ARKHAN3, "SG03DIA05", NULL);
            }
        }
        CB->dialogue("L20D34", P_LORY);
        /* 0x9AFB */
        if (ST(0x70) == 0) {
            CB->joindre(-P_LORY, 6468, -4952, 0xE87B, -1);
        }
        /* 0x9B26 */
        if (ST(0x70) == 1) {
            CB->joindre(-P_LORY, 6680, -4480, 0xC000, -1);
        }
        /* 0x9B51 */
        if (ST(0x70) == 2) {
            CB->joindre(-P_LORY, 7437, -4472, 0xBD8A, -1);
        }
        CB->select_pile(2, 1);
        /* 0x9B8C */
        if (ST(0x74) == 0) {
            CB->joindre(-P_PHEDORIA, 6468, -4952, 0xE87B, -1);
        }
        /* 0x9BB7 */
        if (ST(0x74) == 1) {
            CB->joindre(-P_PHEDORIA, 6680, -4824, 0xC000, -1);
        }
        /* 0x9BE2 */
        if (ST(0x74) == 2) {
            CB->joindre(-P_PHEDORIA, 7434, -4607, 0xFED8, -1);
        }
        ST(0x950) = 1;
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        return;
        /* 0x9C3E: unreachable in the original (the jmp at 0x9C39 skips it); kept for fidelity */
        if (ST(0x950) == 1) {
        }
        /* 0x9C4A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L20D35", P_ARKHAN2, "SG02DIA05", NULL);
        }
        /* 0x9C84 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L20D36", P_ARKHAN3, "SG03DIA05", NULL);
        }
        CB->dialogue("L20D37", P_LORY);
        /* 0x9CB9 */
        if (ST(0x70) == 0) {
            CB->joindre(-P_LORY, 6468, -4952, 0xE87B, -1);
        }
        /* 0x9CE4 */
        if (ST(0x70) == 1) {
            CB->joindre(-P_LORY, 6680, -4480, 0xC000, -1);
        }
        /* 0x9D0F */
        if (ST(0x70) == 2) {
            CB->joindre(-P_LORY, 7437, -4472, 0xBD8A, -1);
        }
        CB->select_pile(2, 1);
        /* 0x9D4A */
        if (ST(0x74) == 0) {
            CB->joindre(-P_PHEDORIA, 6468, -4952, 0xE87B, -1);
        }
        /* 0x9D75 */
        if (ST(0x74) == 1) {
            CB->joindre(-P_PHEDORIA, 6680, -4824, 0xC000, -1);
        }
        /* 0x9DA0 */
        if (ST(0x74) == 2) {
            CB->joindre(-P_PHEDORIA, 7434, -4607, 0xFED8, -1);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        return;
    }
    /* 0x9DF6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->evenement(2, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SG26FRA01");
        CB->evenement(0x22, 750);
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
    /* 0x9F26 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SG26FRA01");
        CB->evenement(0x22, 750);
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
}

/* trigger 9 - trig09_1000A043 */
static void trig09(void) {
    /* 0xA052 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        /* 0xA0EE: `if (ST(0x950) == 0) {}` - empty in the original, both paths continue */
        if (ST(0x950) == 0) {
        }
        /* 0xA0FA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA11E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L20D39", P_ARKHAN2, "SG02DIA05", NULL);
            }
            /* 0xA146 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L20D40", P_ARKHAN2, "SG02DIA05", NULL);
            }
        }
        /* 0xA16E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xA180 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L20D41", P_ARKHAN3, "SG03DIA05", NULL);
            }
            /* 0xA1A8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L20D42", P_ARKHAN3, "SG03DIA05", NULL);
            }
        }
        CB->dialogue("L20D43", P_PHEDORIA);
        /* 0xA1DD */
        if (ST(0x74) == 0) {
            CB->joindre(-P_PHEDORIA, 6468, -4952, 0xE87B, -1);
        }
        /* 0xA208 */
        if (ST(0x74) == 1) {
            CB->joindre(-P_PHEDORIA, 6680, -4824, 0xC000, -1);
        }
        /* 0xA233 */
        if (ST(0x74) == 2) {
            CB->joindre(-P_PHEDORIA, 7434, -4607, 0xFED8, -1);
        }
        ST(0x950) = 1;
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        return;
        /* 0xA28F: unreachable in the original (the jmp at 0xA28A skips it); kept for fidelity */
        if (ST(0x950) == 1) {
        }
        /* 0xA29B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L20D44", P_ARKHAN2, "SG02DIA05", NULL);
        }
        /* 0xA2D5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L20D45", P_ARKHAN3, "SG02DIA05", NULL);
        }
        CB->dialogue("L20D46", P_PHEDORIA);
        /* 0xA30A */
        if (ST(0x74) == 0) {
            CB->joindre(-P_PHEDORIA, 6468, -4952, 0xE87B, -1);
        }
        /* 0xA335 */
        if (ST(0x74) == 1) {
            CB->joindre(-P_PHEDORIA, 6680, -4824, 0xC000, -1);
        }
        /* 0xA360 */
        if (ST(0x74) == 2) {
            CB->joindre(-P_PHEDORIA, 7434, -4607, 0xFED8, -1);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        return;
    }
    /* 0xA3B6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        ST(0x874) = 5;
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SP26ATA17");
        CB->evenement(0x23, 500);
        CB->animation("P26GAR17");
        CB->select_pile(0, 1);
    }
    /* 0xA4E9 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(0x874) = 5;
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SP26ATA17");
        CB->evenement(0x23, 500);
        CB->animation("P26GAR17");
        CB->select_pile(0, 1);
    }
}

/* trigger 15 - trig15_1000A64C */
static void trig15(void) {
    /* 0xA65E */
    if (ST(ST_LOCKED_CAMERA) != 5 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(5, CAM_LOCKED);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA6CC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x8000)) {
        /* 0xA707 */
        if (CB->distance(1, 30) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xA730 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D01A", P_ARKHAN2);
                CB->dialogue("L27D01B", P_ARKHAN2);
                CB->dialogue("L27D01C", P_ARKHAN2);
            }
            /* 0xA772 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D02A", P_ARKHAN3);
                CB->dialogue("L27D02B", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA7D4 */
        if (CB->distance(1, 29) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xA7FD */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D03A", P_ARKHAN2);
                CB->dialogue("L27D03B", P_ARKHAN2);
            }
            /* 0xA82F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D04A", P_ARKHAN3);
                CB->dialogue("L27D04B", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 16 - trig16_1000A883 */
static void trig16(void) {
    /* 0xA895 */
    if (ST(ST_LOCKED_CAMERA) != 6 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(6, CAM_LOCKED);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA903 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x8000)) {
        /* 0xA93E */
        if (CB->distance(1, 28) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xA967 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D05A", P_ARKHAN2);
                CB->dialogue("L27D05B", P_ARKHAN2);
                /* 0xA999 */
                if (ST(0xB10) > 0) {
                    CB->dialogue("L27D05C", P_ARKHAN3);
                }
            }
            /* 0xA9BB */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D06A", P_ARKHAN3);
                CB->dialogue("L27D06B", P_ARKHAN3);
                /* 0xA9ED */
                if (ST(0xB10) > 0) {
                    CB->dialogue("L27D06C", P_ARKHAN3);
                }
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xAA3F */
        if (CB->distance(1, 27) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xAA68 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D07A", P_ARKHAN2);
                CB->dialogue("L27D07B", P_ARKHAN2);
            }
            /* 0xAA9A */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D08A", P_ARKHAN3);
                CB->dialogue("L27D08B", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 17 - trig17_1000AAEE */
static void trig17(void) {
    /* 0xAB00 */
    if (ST(ST_LOCKED_CAMERA) != 4 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(4, CAM_LOCKED);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xAB6E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        /* 0xABA6 */
        if (CB->distance(1, 26) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xABCF */
            if (ST(0xF34) == 0) {
                /* 0xABE1 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L27D09A", P_ARKHAN2);
                    CB->dialogue("L27D09B", P_ARKHAN2);
                }
                /* 0xAC13 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L27D10", P_ARKHAN3);
                }
            }
            /* 0xAC35 */
            if (ST(0xF34) == 1) {
                /* 0xAC47 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L27D11", P_ARKHAN2);
                }
                /* 0xAC69 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L27D12", P_ARKHAN3);
                }
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xACBB */
        if (CB->distance(1, 25) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xACE4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D13A", P_ARKHAN2);
                CB->dialogue("L27D13B", P_ARKHAN2);
                CB->dialogue("L27D13C", P_ARKHAN2);
                CB->dialogue("L27D13D", P_ARKHAN2);
            }
            /* 0xAD36 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D14A", P_ARKHAN3);
                CB->dialogue("L27D14B", P_ARKHAN3);
                CB->dialogue("L27D14C", P_ARKHAN3);
                CB->dialogue("L27D14D", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 18 - trig18_1000ADAA */
static void trig18(void) {
    /* 0xADBC */
    if (ST(ST_LOCKED_CAMERA) != 3 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(3, CAM_LOCKED);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xAE2A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        /* 0xAE62 */
        if (CB->distance(1, 24) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xAE8B */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D15A", P_ARKHAN2);
                CB->dialogue("L27D15B", P_ARKHAN2);
                CB->dialogue("L27D15C", P_ARKHAN2);
            }
            /* 0xAECD */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D16A", P_ARKHAN3);
                CB->dialogue("L27D16B", P_ARKHAN3);
                CB->dialogue("L27D16C", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xAF3F */
        if (CB->distance(1, 23) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xAF68 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D23A", P_ARKHAN2);
                CB->dialogue("L27D23B", P_ARKHAN2);
                CB->dialogue("L27D23C", P_ARKHAN2);
            }
            /* 0xAFAA */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D24A", P_ARKHAN3);
                CB->dialogue("L27D24B", P_ARKHAN3);
                CB->dialogue("L27D24C", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB01C */
        if (CB->distance(1, 22) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xB045 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D17A", P_ARKHAN2);
                CB->dialogue("L27D17B", P_ARKHAN2);
                /* 0xB077 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L27D17C", P_ARKHAN2);
                }
            }
            /* 0xB099 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D18A", P_ARKHAN3);
                CB->dialogue("L27D18B", P_ARKHAN3);
                /* 0xB0CB */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L27D18C", P_ARKHAN3);
                }
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 19 - trig19_1000B10F */
static void trig19(void) {
    /* 0xB121 */
    if (ST(ST_LOCKED_CAMERA) != 3 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(3, CAM_LOCKED);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xB18F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        /* 0xB1C7 */
        if (CB->distance(1, 22) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xB1F0 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D17A", P_ARKHAN2);
                CB->dialogue("L27D17B", P_ARKHAN2);
                /* 0xB222 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L27D17C", P_ARKHAN2);
                }
            }
            /* 0xB244 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D18A", P_ARKHAN3);
                CB->dialogue("L27D18B", P_ARKHAN3);
                /* 0xB276 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L27D18C", P_ARKHAN3);
                }
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB2C8 */
        if (CB->distance(1, 21) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xB2F1 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D19A", P_ARKHAN2);
                CB->dialogue("L27D19B", P_ARKHAN2);
                CB->dialogue("L27D19C", P_ARKHAN2);
                /* 0xB333 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L27D19D", P_ARKHAN2);
                }
            }
            /* 0xB355 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D20A", P_ARKHAN3);
                CB->dialogue("L27D20B", P_ARKHAN3);
                /* 0xB387 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L27D20C", P_ARKHAN3);
                }
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB3D9 */
        if (CB->distance(1, 20) < 50) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xB402 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L27D21A", P_ARKHAN2);
                CB->dialogue("L27D21B", P_ARKHAN2);
                CB->dialogue("L27D21C", P_ARKHAN2);
            }
            /* 0xB444 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L27D22A", P_ARKHAN3);
                CB->dialogue("L27D22B", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 32 - trig32_1000B51C */
static void trig32(void) {
    /* 0xB52E */
    if (ST(0xB38) == 0) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0xB552 */
    if (ST(0xB38) != 0) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 27;
    CB->change_piece(20, 1);
}

/* trigger 33 - trig33_1000B58B */
static void trig33(void) {
    /* 0xB59E */
    if (CB->direction_zone(33) && CB->direction_spec(0x88B8) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xB5F8 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xB60A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0xB627 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xB66D */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xB67F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0xB69C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT29");
            }
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 34 - trig34_1000B6DB */
static void trig34(void) {
    /* 0xB6EE */
    if (CB->direction_spec(0xB3B0) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xB735 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L27D25A", P_ARKHAN2);
            /* 0xB757 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L27D25B", P_ARKHAN2);
            }
        }
        /* 0xB779 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L27D26", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 35 - trig35_1000B7BD */
static void trig35(void) {
    /* 0xB7D0 */
    if (CB->direction_zone(35) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xB817 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0xB83F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 36 - trig36_1000B889 */
static void trig36(void) {
    /* 0xB898 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 40) && !CB->direction_perso(P_KONKALITE4)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xB919 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", NULL);
        }
        /* 0xB941 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", NULL);
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xB998 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_KONKALITE4)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->dialogue("L27D27", P_ARKHAN1);
        CB->dialogue("L27D28", P_KONKALITE4);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xBA3B */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KONKALITE4) && ST(ST_VIE(P_KONKALITE4)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L27D29", P_KONKALITE4);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 37 - trig37_1000BAE2 */
static void trig37(void) {
    /* 0xBAF1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(36)) != VIE_DEAD && CB->direction_perso(36)) {
        /* 0xBB55 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD20", P_ARKHAN2, "SP02ATT51", NULL);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xBB95 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD21", P_ARKHAN3, "SP03ATT51", NULL);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xBBD2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(36)) == VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        CB->retire_zone(37);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBC45 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00PRE09");
            /* 0xBC62 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD82", P_ARKHAN2);
            }
            /* 0xBC84 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD83", P_ARKHAN2);
            }
        }
        /* 0xBCA6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00PRE09");
            /* 0xBCC3 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD84", P_ARKHAN3);
            }
            /* 0xBCE5 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD85", P_ARKHAN3);
            }
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 39 - trig39_1000BD34 */
static void trig39(void) {
    /* 0xBD43 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBD9D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xBDAF */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xBDC9 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT11");
                } else {
                    CB->animation("SP02ACT12");
                }
            }
            /* 0xBDF6 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xBE10 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT86");
                } else {
                    CB->animation("SP02ACT85");
                }
            }
        }
        /* 0xBE3D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xBE4F */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xBE69 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT11");
                } else {
                    CB->animation("SP03ACT12");
                }
            }
            /* 0xBE96 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xBEB0 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT86");
                } else {
                    CB->animation("SP03ACT85");
                }
            }
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 40 - trig40_1000BEFF */
static void trig40(void) {
    /* 0xBF0E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xBF5C */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", NULL);
        }
        /* 0xBF84 */
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

/* trigger 41 - trig41_100080D9 */
static void trig41(void) {
    CB->bruitage("L27AMB1", -1, -1);
    /* 0x80FD */
    if (ST(0xC48) == 100) {
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(2, CAM_LOCKED);
        CB->animation("SP36ATA04");
        CB->animation("P36GAR01");
        CB->combat(36, 1);
        CB->change_camera(99, CAM_AUTO);
        ST(V_ENGAGED_PERSONA) = 36;
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x8174 */
    if (ST(V_EVENT_17) != 2) {
        /* 0x8186 */
        if (ST(0xB38) == 0) {
            CB->divers(DV_MUSIC, 1, 1);
        }
        /* 0x81AA */
        if (ST(0xB38) != 0) {
            CB->divers(DV_MUSIC, 2, 1);
        }
    }
    /* 0x81CE */
    if (ST(0xB58) == 5) {
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 6468, -4952, 0xE87B, -1);
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 6468, -4952, 0xE87B, -1);
        CB->select_pile(0, 1);
    }
    /* 0x8245 */
    if (ST(V_EVENT_17) == 2) {
        ST(V_EVENT_17) = 3;
        CB->cinematique("SC02_500");
        CB->change_camera(1, CAM_LOCKED);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->play_anim(0, 0);
        CB->combat(P_KONKALITE4, 4);
        CB->evenement(0x21, 2000);
        ST(0x88) = 1;
        ST(0x84) = ST(ST_DEBUG_PRINT);
    }
}
