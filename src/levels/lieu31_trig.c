/*
 * lieu31_trig.c - Native hand-port of the LIEU31.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu31.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * Triggers 15..19 and 22 are copies of one handler that differ only in the
 * zone angle and the dialogue ids.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu31_int.h"

static void trig41(void); static void trig02(void);
static void trig03(void); static void trig04(void);
static void trig05(void); static void trig07(void);
static void trig08(void); static void trig12(void);
static void trig14(void); static void trig15(void);
static void trig16(void); static void trig17(void);
static void trig18(void); static void trig19(void);
static void trig20(void); static void trig21(void);
static void trig22(void); static void trig23(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* sprintf buffers of the DLL's .data (reset on every room load) */
static c8 g_buf24[512];   /* "OB24%c", dword_100219A0 */
static c8 g_buf77[512];   /* "OB77%c", dword_100217A0 */

/* trigger table at 0x10021BA0 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 100073F0   1 1000A608 stub   2 1000A613   3 1000BC9B   4 1000D1DE
 *   5 1000D52F   6 stub   7 1000D88B   8 1000DA06   9..11 stubs
 *  12 1000DB3C  13 stub  14 1000DC1F  15 1000DEB4  16 1000DFF7
 *  17 1000E13A  18 1000E27D  19 1000E3C0  20 1000E503  21 1000E6F3
 *  22 1000E8E3  23 1000EA26  24..40 stubs (1000ED31..1000EDE1)
 *  41 10008600
 */
static void (*const g_trigger[42])(void) = {
    l31_trig00, trig_noop, trig02, trig03, trig04, trig05, trig_noop, trig07,
    trig08, trig_noop, trig_noop, trig_noop, trig12, trig_noop, trig14,
    trig15, trig16, trig17, trig18, trig19, trig20, trig21, trig22, trig23,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10021BA0]). */
void l31_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - sub_100073F0 */
void l31_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(31)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 30;
        ST(0xBB0) = 0;
        ST(0x140) = 1;
    }
    /* 0x744D */
    if (ST(ST_START_POINT(31)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 30;
        ST(0xBB0) = 2;
        ST(0xF24) = 5;
    }
    /* 0x7498 */
    if (ST(ST_START_POINT(31)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xEE0) = 10;
        ST(ST_LEVEL) = 32;
    }
    /* 0x74D4 */
    if (ST(ST_START_POINT(31)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 34;
        ST(0xEF8) = 11;
    }
    ST(ST_START_POINT(31)) = 0;
    /* 0x751C */
    if (ST(ST_LEVEL) == 32 && ST(0xE8C) == 2) {
        /* 0x7540 */
        if (ST(0xEE0) == 1) {
            ST(0xEE0) = 10;
        }
        /* 0x7561 */
        if (ST(0xEE0) == 2) {
            /* no body */
        }
        /* 0x7567 */
        if (ST(0xEE0) == 30) {
            /* no body */
        }
    }
    CB->ajoute_zone(7, 2156, 6066, 305, 305, 1);
    CB->ajoute_zone(8, 4536, 7121, 290, 360, 1);
    CB->ajoute_zone(12, 4326, 5876, 65, 215, 1);
    CB->ajoute_zone(14, 3356, 5811, 160, 160, 0);
    CB->ajoute_zone(15, 3521, 5652, 190, 175, 0);
    CB->ajoute_zone(16, 3181, 5932, 190, 175, 0);
    CB->ajoute_zone(17, 3144, 6091, 185, 140, 0);
    CB->ajoute_zone(18, 3054, 6161, 185, 140, 0);
    CB->ajoute_zone(19, 3836, 5422, 455, 185, 0);
    CB->ajoute_zone(20, 4095, 6296, 243, 226, 0);
    CB->ajoute_zone(21, 3970, 6663, 230, 134, 0);
    CB->ajoute_zone(22, 2790, 6737, 184, 136, 0);
    CB->ajoute_zone(23, 3294, 6666, 225, 225, 0);
    CB->ajoute_zone(13, 3368, 6300, 20, 20, 0);
    /* 0x774C */
    if (ST(ST_LEVEL) == 30) {
        CB->pos_perso(ST(ST_HERO), 4189, 5817, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x779A */
    if (ST(ST_LEVEL) == 32) {
        CB->pos_perso(ST(ST_HERO), 4264, 7013, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x77E8 */
    if (ST(ST_LEVEL) == 34) {
        CB->pos_perso(ST(ST_HERO), 2379, 6190, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x7839 */
    if (ST(0xEE0) > 2) {
        /* 0x784B */
        if (ST(0xEE0) == 10) {
            /* 0x785A */
            if (ST(ST_LEVEL) == 30) {
                /* 0x786C */
                if (ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD) {
                    CB->pos_perso(P_KONKALITE8, 3834, 6153, 0x9E91, 1, "P78GAR03", 2, 1);
                } else {
                    CB->pos_perso(P_KONKALITE8, 0, 0, 0, 2, "P78CAD", 2, 1);
                }
                /* 0x78C4 */
                if (ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD) {
                    CB->pos_perso(P_KONKALITE10, 4021, 5634, 0xDE98, 1, "P80GAR03", 3, 1);
                } else {
                    CB->pos_perso(P_KONKALITE10, 0, 0, 0, 2, "P80CAD", 3, 1);
                }
            }
            /* 0x7919 */
            if (ST(ST_LEVEL) == 34) {
                /* 0x792B */
                if (ST(0xEF8) == 1 || ST(0xEF8) == 11) {
                    /* 0x794F */
                    if (ST(0xEF8) == 1) {
                        CB->pos_perso(P_BORK, 2580, 6286, 0x4E20, 1, "P30ARR03", 4, 1);
                    } else {
                        CB->pos_perso(P_BORK, 2580, 6286, 0x4E20, 1, "P30ARR03", 4, 1);
                        CB->pos_perso(P_SORDOS, 2326, 6265, 0xBB89, 1, "P23GAR03", -1, 1);
                    }
                } else {
                    /* 0x79DA */
                    if (ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD) {
                        CB->pos_perso(P_KONKALITE8, 2877, 6307, 0x4687, 1, "P78GAR03", 2, 1);
                    } else {
                        CB->pos_perso(P_KONKALITE8, 0, 0, 0, 2, "P78CAD", 2, 1);
                    }
                    /* 0x7A32 */
                    if (ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD) {
                        CB->pos_perso(P_KONKALITE10, 2731, 6436, 0x60E3, 1, "P80GAR03", 3, 1);
                    } else {
                        CB->pos_perso(P_KONKALITE10, 0, 0, 0, 2, "P80CAD", 3, 1);
                    }
                }
            }
            /* 0x7A87 */
            if (ST(ST_LEVEL) == 32) {
                /* 0x7A99 */
                if (ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD) {
                    CB->pos_perso(P_KONKALITE8, 3881, 6919, 0xBC1B, 1, "P78GAR03", 2, 1);
                } else {
                    CB->pos_perso(P_KONKALITE8, 0, 0, 0, 2, "P78CAD", 2, 1);
                }
                /* 0x7AF1 */
                if (ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD) {
                    CB->pos_perso(P_KONKALITE10, 3871, 6800, 0xCD6D, 1, "P80GAR03", 3, 1);
                } else {
                    CB->pos_perso(P_KONKALITE10, 0, 0, 0, 2, "P80CAD", 3, 1);
                }
            }
        }
        /* 0x7B49 */
        if (ST(0xEE0) == 20) {
            /* 0x7B58 */
            if (ST(ST_LEVEL) == 30) {
                CB->pos_perso(P_KONKALITE8, 0, 0, 0, 2, "P78CAD", 2, 0);
                CB->pos_perso(P_KONKALITE10, 0, 0, 0, 2, "P80CAD", 3, 0);
            }
            /* 0x7B9F */
            if (ST(ST_LEVEL) == 34) {
                /* 0x7BB1 */
                if (ST(0xEF8) == 1 || ST(0xEF8) == 11) {
                    /* 0x7BD5 */
                    if (ST(0xEF8) == 1) {
                        CB->pos_perso(P_BORK, 2580, 6286, 0x4E20, 1, "P30ARR03", 4, 1);
                    } else {
                        CB->pos_perso(P_BORK, 2580, 6286, 0x4E20, 1, "P30ARR03", 4, 1);
                        CB->pos_perso(P_SORDOS, 2326, 6265, 0xBB89, 1, "P23GAR03", -1, 1);
                    }
                } else {
                    CB->pos_perso(P_KONKALITE8, 0, 0, 0, 2, "P78CAD", 2, 0);
                    CB->pos_perso(P_KONKALITE10, 0, 0, 0, 2, "P80CAD", 3, 0);
                }
            }
            /* 0x7C95 */
            if (ST(ST_LEVEL) == 32) {
                CB->pos_perso(P_KONKALITE8, 0, 0, 0, 2, "P78CAD", 2, 0);
                CB->pos_perso(P_KONKALITE10, 0, 0, 0, 2, "P80CAD", 3, 0);
            }
        }
        /* 0x7CDF */
        if (ST(0xEE0) == 30) {
            /* 0x7CEE */
            if (ST(ST_LEVEL) == 30) {
                /* 0x7D00 */
                if (ST(ST_VIE(P_BORK)) != VIE_DEAD) {
                    CB->pos_perso(P_BORK, 3834, 6153, 0x9E91, 1, "P30GAR03", 4, 1);
                } else {
                    CB->pos_perso(P_BORK, 0, 0, 0, 2, "P30CAD", 4, 0);
                }
                /* 0x7D58 */
                if (ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
                    CB->pos_perso(P_KONKALITE11, 4021, 5634, 0xDE98, 1, "P81GAR03", 5, 1);
                } else {
                    CB->pos_perso(P_KONKALITE11, 0, 0, 0, 2, "P81CAD", 5, 0);
                }
            }
            /* 0x7DAD */
            if (ST(ST_LEVEL) == 32) {
                /* 0x7DBF */
                if (ST(ST_VIE(P_BORK)) != VIE_DEAD) {
                    CB->pos_perso(P_BORK, 3881, 6919, 0xBC1B, 1, "P30GAR03", 4, 1);
                } else {
                    CB->pos_perso(P_BORK, 0, 0, 0, 2, "P30CAD", 4, 0);
                }
                /* 0x7E17 */
                if (ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
                    CB->pos_perso(P_KONKALITE11, 3871, 6800, 0xCD6D, 1, "P81GAR03", 5, 1);
                } else {
                    CB->pos_perso(P_KONKALITE11, 0, 0, 0, 2, "P81CAD", 5, 0);
                }
            }
            /* 0x7E6C */
            if (ST(ST_LEVEL) == 34) {
                /* 0x7E7E */
                if (ST(ST_VIE(P_BORK)) != VIE_DEAD) {
                    CB->pos_perso(P_BORK, 2580, 6286, 0x4E20, 1, "P30GAR03", 4, 1);
                } else {
                    CB->pos_perso(P_BORK, 0, 0, 0, 2, "P30CAD", 4, 0);
                }
                /* 0x7ED6 */
                if (ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
                    CB->pos_perso(P_KONKALITE11, 2731, 6436, 0x60E3, 1, "P81GAR03", 5, 1);
                } else {
                    CB->pos_perso(P_KONKALITE11, 0, 0, 0, 2, "P81CAD", 5, 0);
                }
            }
        }
        /* 0x7F2E */
        if (ST(0xEE0) == 35) {
            CB->pos_perso(P_BORK, 0, 0, 0, 2, "P30CAD", 4, 0);
            CB->pos_perso(P_KONKALITE11, 0, 0, 0, 2, "P81CAD", 5, 0);
        }
    }
    /* 0x7F78 */
    if (ST(0xEE0) < 3) {
        /* 0x7F87 */
        if (ST(ST_LEVEL) == 34) {
            /* 0x7F99 */
            if (ST(0xEF8) == 1 || ST(0xEF8) == 11) {
                /* 0x7FBD */
                if (ST(0xEF8) == 1) {
                    CB->pos_perso(P_BORK, 2580, 6286, 0x4E20, 1, "P30ARR03", 4, 1);
                } else {
                    CB->pos_perso(P_BORK, 2580, 6286, 0x4E20, 1, "P30ARR03", 4, 1);
                    CB->pos_perso(P_SORDOS, 2326, 6265, 0xBB89, 1, "P23GAR03", -1, 1);
                }
            } else {
                /* 0x8048 */
                if (ST(0xEE8) < 8 || ST(0xEE8) == 99) {
                    CB->pos_perso(P_KONKALITE8, CB->divers(DV_DESC_POS, P_KONKALITE8, 0), CB->divers(DV_DESC_POS, P_KONKALITE8, 1), CB->divers(DV_DESC_POS, P_KONKALITE8, 2), 1, "P78ARR01", 2, 1);
                } else {
                    CB->pos_perso(P_KONKALITE8, 3062, 6786, 0x38A3, 2, "P78ACT04", 2, 0);
                }
                /* 0x80E6 */
                if (ST(0xEEC) < 8 || ST(0xEEC) == 99) {
                    CB->pos_perso(P_KONKALITE10, CB->divers(DV_DESC_POS, P_KONKALITE10, 0), CB->divers(DV_DESC_POS, P_KONKALITE10, 1), CB->divers(DV_DESC_POS, P_KONKALITE10, 2), 1, "P80ARR01", 3, 1);
                } else {
                    CB->pos_perso(P_KONKALITE10, CB->divers(DV_DESC_POS, P_KONKALITE10, 0), CB->divers(DV_DESC_POS, P_KONKALITE10, 1), CB->divers(DV_DESC_POS, P_KONKALITE10, 2), 2, "P80ACT04", 3, 0);
                }
            }
        }
        /* 0x81AC */
        if (ST(ST_LEVEL) == 30 || ST(ST_LEVEL) == 32) {
            /* 0x81CD */
            if (ST(0xBB0) != 2) {
                /* 0x81DF */
                if (ST(0xEE0) == 0) {
                    ST(0xEE0) = 1;
                    CB->pos_perso(P_KONKALITE8, 2712, 6441, 0x8EE8, 1, "P78DIA58", 2, 1);
                    CB->pos_perso(P_KONKALITE10, 2777, 6257, 0x138C, 1, "P80DIA53", 3, 1);
                } else {
                    /* 0x824F */
                    if (ST(0xEE8) < 8 || ST(0xEE8) == 99) {
                        CB->pos_perso(P_KONKALITE8, CB->divers(DV_DESC_POS, P_KONKALITE8, 0), CB->divers(DV_DESC_POS, P_KONKALITE8, 1), CB->divers(DV_DESC_POS, P_KONKALITE8, 2), 1, "P78ARR01", 2, 1);
                    } else {
                        CB->pos_perso(P_KONKALITE8, 3062, 6786, 0x38A3, 2, "P78ACT04", 2, 0);
                    }
                    /* 0x82ED */
                    if (ST(0xEEC) < 8 || ST(0xEEC) == 99) {
                        CB->pos_perso(P_KONKALITE10, CB->divers(DV_DESC_POS, P_KONKALITE10, 0), CB->divers(DV_DESC_POS, P_KONKALITE10, 1), CB->divers(DV_DESC_POS, P_KONKALITE10, 2), 1, "P80ARR01", 3, 1);
                    } else {
                        CB->pos_perso(P_KONKALITE10, CB->divers(DV_DESC_POS, P_KONKALITE10, 0), CB->divers(DV_DESC_POS, P_KONKALITE10, 1), CB->divers(DV_DESC_POS, P_KONKALITE10, 2), 2, "P80ACT04", 3, 0);
                    }
                }
            }
            /* 0x83B6 */
            if (ST(0xBB0) == 2) {
                ST(0xEE0) = 2;
                CB->pos_perso(P_BORK, 2621, 6262, 0xD576, 1, "P30ARR03", 4, 1);
                CB->pos_perso(P_KONKALITE11, 2902, 6748, 0x15C1, 1, "P81ARR01", 5, 1);
            }
        }
    }
    CB->bruitage("L31AMB1", -1, -1);
    /* 0x8430 */
    if (ST(ST_LEVEL) == 34) {
        CB->divers(DV_MUSIC, 1, 0);
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
    ST(0xB8) = 0;
    ST(0xBC) = 0;
    ST(0xD4) = 0;
    ST(0xE0) = 0;
    ST(0xFC) = 0;
    ST(0x100) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(0x118) = 0;
}

/* trigger 41 - sub_10008600 */
static void trig41(void) {
    /* 0x860F */
    if (ST(ST_LEVEL) == 34) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x8633 */
    if (ST(ST_VIE(P_BORK)) != VIE_DEAD && ST(ST_LEVEL) == 34 && ST(0xEF8) == 1) {
        CB->select_pile(1, 1);
        CB->change_camera(4, CAM_LOCKED);
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0, 400);
        CB->animation("SP30ATA20");
        CB->animation("P30ARR03");
        CB->select_pile(0, 1);
    }
    /* 0x86C1 */
    if (ST(ST_VIE(P_BORK)) != VIE_DEAD && ST(ST_LEVEL) == 34 && ST(0xEF8) == 11) {
        CB->select_pile(1, 1);
        CB->change_camera(4, CAM_LOCKED);
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0, 400);
        CB->animation("SP30ATA20");
        CB->animation("P30ARR03");
        lieu_action_register("L34D04A", P_SORDOS, "SP23DIA01", "SP23GAR03");
        CB->select_pile(0, 1);
    }
    /* 0x8768 */
    if (ST(0xEE0) == 10 && ST(0xEF8) != 1 && ST(0xEF8) != 11) {
        /* 0x879E */
        if (ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD) {
            CB->combat(P_KONKALITE8, 0);
        }
        /* 0x87BD */
        if (ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD) {
            CB->combat(P_KONKALITE10, 0);
        }
        ST(V_ENGAGED_PERSONA) = 0x1EC8;
    }
    /* 0x87E8 */
    if (ST(0xEE0) == 30 && ST(0xEF8) != 1 && ST(0xEF8) != 11) {
        /* 0x881E */
        if (ST(ST_VIE(P_BORK)) != VIE_DEAD) {
            CB->combat(P_BORK, 0);
        }
        /* 0x883D */
        if (ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
            CB->combat(P_KONKALITE11, 0);
        }
        ST(V_ENGAGED_PERSONA) = 0xC09;
    }
}

/* trigger 2 - sub_1000A613 */
static void trig02(void) {
    /* 0xA622 */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_perso(P_KONKALITE10) && !CB->direction_perso(P_KONKALITE8) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD) {
        trig03();
        return;
    }
    /* 0xA696 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_KONKALITE8) && ST(ST_VIE(P_KONKALITE8)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L31D53", P_KONKALITE8);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->combat(P_KONKALITE8, 0);
        CB->combat(P_KONKALITE10, 0);
        ST(V_ENGAGED_PERSONA) = 0x1EC8;
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA776 */
    if (ST(0x64) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD && ST(0xEE8) < 5 && ST(0xEEC) < 5) {
        ST(0x64) = 2;
        CB->select_pile(1, 1);
        /* 0xA7E9 */
        if (ST(ST_LOCKED_CAMERA) != 4) {
            CB->change_camera(4, CAM_LOCKED);
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(2, -1);
        /* 0xA826 */
        if (ST(0xEE8) == 2) {
            CB->dialogue("L31D01", P_KONKALITE8);
        } else {
            CB->stop_joindre(-P_KONKALITE8);
            lieu_action_register("L31D01", P_KONKALITE8, "SP78GAR26", "SP78GAR03_100");
            CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
        }
        CB->bloque(BLOQUE_NONE);
        /* 0xA88F */
        if (ST(0xEE8) != 99) {
            ST(0xEE8) = 5;
        }
        /* 0xA8B0 */
        if (ST(0xEEC) != 99) {
            ST(0xEEC) = 5;
        }
        CB->select_pile(0, 1);
    }
    /* 0xA8DE */
    if (ST(0xFC) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD && ST(0xEE8) == 99) {
        ST(0xFC) = 1;
        CB->select_pile(1, 1);
        CB->stop_joindre(-P_KONKALITE8);
        CB->animation("SP78GAR26");
        CB->animation("P78GAR03");
        CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
        CB->dialogue("L31D01", P_KONKALITE8);
        /* 0xA989 */
        if (ST(0xEE8) != 99) {
            ST(0xEE8) = 5;
        }
        CB->select_pile(0, 1);
    }
    /* 0xA9B4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_KONKALITE8) && ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAA24 */
        if (ST(ST_LOCKED_CAMERA) != 4 && ST(0xEEC) < 6) {
            CB->change_camera(4, CAM_LOCKED);
        }
        CB->bloque(BLOQUE_ALL);
        CB->joindre(ST(ST_HERO), 0, 0, 0, 78);
        CB->select_pile(1, 1);
        CB->joindre_perm(-P_KONKALITE8, ST(ST_HERO));
        CB->stop_joindre(P_KONKALITE8);
        CB->select_pile(0, 1);
        /* 0xAABA */
        if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON)) {
            /* 0xAAFD */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xAB0F */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L31D22", P_ARKHAN3, "SG00DIA03", NULL);
                    lieu_action_register("L31D25A", P_KONKALITE8, "SP78DIA50", "SP78ARR01_10");
                    lieu_action_register("L31D25B", P_KONKALITE8, "SP78DIA51", "SP78ARR01_10");
                }
                /* 0xAB69 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD22A", P_ARKHAN3);
                    lieu_action_register("L31D27", P_KONKALITE8, "SP78DIA53", "SP78ARR01_10");
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->combat(P_KONKALITE8, 0);
                ST(V_ENGAGED_PERSONA) = P_KONKALITE8;
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0xABEB */
                if (ST(0xEE8) <= 5 || ST(0xEE8) == 99) {
                    /* 0xAC0F */
                    if (ST(0xAD0) == 0) {
                        ST(0xAD0) = 1;
                        /* 0xAC30 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L31D02", P_ARKHAN2, "SG00DIA02", NULL);
                            lieu_action_register("L31D03A", P_KONKALITE8, "SP78GAR03", "SP78GAR03_100");
                            lieu_action_register("L31D03B", P_KONKALITE8, "SP78GAR03", "SP78GAR03_100");
                            /* 0xAC8A */
                            if (ST(0xEEC) > 5) {
                                lieu_action_register("L31D21C", P_KONKALITE8, "SP78DIA54", "SP78ARR01_10");
                            }
                            CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xACF4 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L31D04A", P_ARKHAN2, "SG00DIA01", NULL);
                            lieu_action_register("L31D04B", P_ARKHAN2, "SG00DIA01", NULL);
                            lieu_action_register("L31D05", P_KONKALITE8, "SP78GAR03", "SP78GAR03_100");
                            CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0xAD8A */
                    if (ST(0xAD0) == 1) {
                        /* 0xAD9C */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L31D06", P_ARKHAN2, "SG00DIA02", NULL);
                            lieu_action_register("L31D07", P_KONKALITE8, "SP78GAR03", "SP78GAR03_100");
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(V_ENGAGED_PERSONA), 78);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->combat(P_KONKALITE8, 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xAE27 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L31D08", P_ARKHAN2, "SG00DIA01", NULL);
                            lieu_action_register("L31D09", P_KONKALITE8, "SP78GAR03", "SP78GAR03_100");
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(V_ENGAGED_PERSONA), 78);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->combat(P_KONKALITE8, 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }
                /* 0xAEB2 */
                if (ST(0xEE8) > 5 && ST(0xEE8) < 9) {
                    /* 0xAED6 */
                    if (ST(0xAD0) == 0) {
                        ST(0xAD0) = 1;
                        ST(0xB1C) = 1;
                        lieu_action_register("L31D10", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L31D11A", P_KONKALITE8, "SP78DIA56", "SP78ARR01_10");
                        lieu_action_register("L31D11B", P_KONKALITE8, "SP78DIA57", "SP78ARR01_10");
                        lieu_action_register("L31D11C", P_KONKALITE8, "SP78DIA58", "SP78ARR01_10");
                        lieu_action_register("L31D11D", P_KONKALITE8, "SP78DIA59", "SP78ARR01_10");
                        lieu_action_register("L31D11E", P_KONKALITE8, "SP78DIA60", "SP78ARR01_10");
                        lieu_action_register("L31D11F", P_KONKALITE8, "SP78DIA61", "SP78ARR01_10");
                        lieu_action_register("L31D11G", P_KONKALITE8, "SP78DIA62", "SP78ARR01_10");
                        lieu_action_register("L31D12", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L31D13A", P_KONKALITE8, "SP78DIA63", "SP78ARR01_10");
                        lieu_action_register("L31D13B", P_KONKALITE8, "SP78DIA64", "SP78ARR01_10");
                        lieu_action_register("L31D13C", P_KONKALITE8, "SP78DIA65", "SP78ARR01_10");
                        lieu_action_register("L31D13D", P_KONKALITE8, "SP78DIA50", "SP78ARR01_10");
                        CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xB084 */
                    if (ST(0xAD0) == 1) {
                        ST(0xAD0) = 2;
                        lieu_action_register("L31D14", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L31D15A", P_KONKALITE8, "SP78DIA51", "SP78ARR01_10");
                        lieu_action_register("L31D15B", P_KONKALITE8, "SP78DIA53", "SP78ARR01_10");
                        lieu_action_register("L31D15C", P_KONKALITE8, "SP78DIA54", "SP78ARR01_10");
                        lieu_action_register("L31D15D", P_KONKALITE8, "SP78DIA56", "SP78ARR01_10");
                        lieu_action_register("L31D15E", P_KONKALITE8, "SP78DIA57", "SP78ARR01_10");
                        CB->dialogue("L31D16", P_ARKHAN2);
                        CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xB187 */
                    if (ST(0xAD0) == 2) {
                        ST(0xAD0) = 3;
                        lieu_action_register("L31D17", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L31D18A", P_KONKALITE8, "SP78DIA58", "SP78ARR01_10");
                        lieu_action_register("L31D18B", P_KONKALITE8, "SP78DIA59", "SP78ARR01_10");
                        lieu_action_register("L31D18C", P_KONKALITE8, "SP78DIA60", "SP78ARR01_10");
                        lieu_action_register("L31D18D", P_KONKALITE8, "SP78DIA61", "SP78ARR01_10");
                        lieu_action_register("L31D18E", P_KONKALITE8, "SP78DIA62", "SP78ARR01_10");
                        CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xB27A */
                    if (ST(0xAD0) == 3) {
                        /* 0xB28C */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L31D19", P_ARKHAN2, "SG00DIA02", NULL);
                        }
                        /* 0xB2B4 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L31D20", P_ARKHAN2, "SG00DIA02", NULL);
                        }
                        ST(V_DLG_RANDOM) = CB->random(2);
                        /* 0xB2F3 */
                        if (ST(V_DLG_RANDOM) == 0) {
                            lieu_action_register("L31D21A", P_KONKALITE8, "SP78DIA63", "SP78ARR01_10");
                        }
                        /* 0xB31E */
                        if (ST(V_DLG_RANDOM) == 1) {
                            lieu_action_register("L31D21B", P_KONKALITE8, "SP78DIA64", "SP78ARR01_10");
                        }
                        lieu_action_register("L31D21C", P_KONKALITE8, "SP78DIA56", "SP78ARR01_10");
                        CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
        }
        /* 0xB3A1 */
        if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
            /* 0xB3E4 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xB3F6 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L31D42", P_ARKHAN3, "SG03DIA03", NULL);
                }
                /* 0xB41E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD22B", P_ARKHAN3);
                }
                lieu_action_register("L31D43A", P_KONKALITE8, "P78DIA53", "SP78ARR01_10");
                lieu_action_register("L31D43B", P_KONKALITE8, "P78DIA53", "SP78ARR01_10");
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->combat(P_KONKALITE8, 0);
                ST(V_ENGAGED_PERSONA) = P_KONKALITE8;
                CB->bloque(BLOQUE_NONE);
                return;
            } else if (ST(ST_PLAYER_WEAPON) == 24) {
                /* 0xB4CB */
                if (ST(0xAD4) == 0) {
                    ST(0xAD4) = 1;
                    ST(0xAD0) = 0;
                    /* 0xB508 */
                    if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) != 0) {
                        ST(0x140 + 4 * ST(ST_USED_VARIANT))--;
                    }
                    /* 0xB532 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L31D40", P_ARKHAN2, "SG02DIA03", NULL);
                    }
                    /* 0xB55A */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L31D41", P_ARKHAN2, "SG02DIA03", NULL);
                    }
                    CB->animation("SP78GAR27");
                    lieu_action_register("L31D44", P_KONKALITE8, "P78DIA53", "SP78ARR01_10");
                    lieu_pre_scene(ST(ST_HERO), P_KONKALITE8, 24, 1, 2);
                    CB->animation("Sar_50");
                    ST(0xEE8) = 6;
                    CB->animation("SP78ACT01");
                    lieu_action_register("L31D45", P_KONKALITE8, "P78ARR01", "SP78ARR01_10");
                    lieu_pre_scene(ST(ST_HERO), P_KONKALITE8, 24, 1, 2);
                    CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                    /* 0xB63D */
                    if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) == 0) {
                        snprintf(g_buf24, sizeof(g_buf24), "OB24%c", (ST(ST_USED_VARIANT) + 65));
                        CB->etat_objet(g_buf24, -1, -1, -1, -1, -1, -1, -2, 0);
                        snprintf(g_buf77, sizeof(g_buf77), "OB77%c", (ST(ST_USED_VARIANT) + 65));
                        CB->etat_objet(g_buf77, -1, -1, -1, -1, -1, -1, -3, 0);
                        CB->animation("Sar_50");
                    }
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xB701 */
                if (ST(0xAD4) == 1) {
                    lieu_action_register("L31D46", P_ARKHAN2, "SG02DIA03", NULL);
                    lieu_action_register("L31D48", P_KONKALITE8, "SP78DIA53", "SP78ARR01_10");
                    lieu_pre_scene(ST(ST_HERO), P_KONKALITE8, 24, 1, 2);
                    CB->animation("Sar_50");
                    ST(0xEE8)++;
                    /* 0xB780 */
                    if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) != 0) {
                        ST(0x140 + 4 * ST(ST_USED_VARIANT))--;
                    }
                    /* 0xB7AA */
                    if (ST(0xEE8) < 8) {
                        CB->animation("SP78ACT01");
                        lieu_action_register("L31D47", P_KONKALITE8, "P78ARR01", "SP78ARR01_10");
                    }
                    /* 0xB7E0 */
                    if (ST(0xEE8) == 8) {
                        CB->animation("SP78ACT01");
                        lieu_action_register("L31D49", P_KONKALITE8, "P78ARR01", "SP78ARR01_10");
                        CB->animation("SP78ACT02");
                        CB->animation("P78ACT03");
                        CB->animation("P78CAD01");
                        /* 0xB844 */
                        if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) == 0) {
                            snprintf(g_buf24, sizeof(g_buf24), "OB24%c", (ST(ST_USED_VARIANT) + 65));
                            CB->link_objet(78, g_buf24, 27, 0);
                            snprintf(g_buf77, sizeof(g_buf77), "OB77%c", (ST(ST_USED_VARIANT) + 65));
                            CB->link_objet(78, g_buf77, 27, 1);
                            CB->animation("Sar_50");
                            CB->fouille(P_KONKALITE8);
                            CB->bloque(BLOQUE_ALL);
                            /* the original passes the (unspecified) results of these queued
                             * DiversEmpile calls as x/z/facing; here 0 (corpse view 2) */
                            CB->divers_empile(DV_DESC_POS, P_KONKALITE8, 2);
                            CB->divers_empile(DV_DESC_POS, P_KONKALITE8, 1);
                            CB->divers_empile(DV_DESC_POS, P_KONKALITE8, 0);
                            CB->pos_perso(P_KONKALITE8, 0, 0, 0, 2, "P78CAD01", -1, 0);
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(ST_VIE(P_KONKALITE8)), 2);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        CB->animation("Sar_50");
                        CB->fouille(P_KONKALITE8);
                        CB->bloque(BLOQUE_ALL);
                        /* as above: results of the queued calls are the x/z/facing */
                        CB->divers_empile(DV_DESC_POS, P_KONKALITE8, 2);
                        CB->divers_empile(DV_DESC_POS, P_KONKALITE8, 1);
                        CB->divers_empile(DV_DESC_POS, P_KONKALITE8, 0);
                        CB->pos_perso(P_KONKALITE8, 0, 0, 0, 2, "P78CAD01", 2, 0);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_VIE(P_KONKALITE8)), 2);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    lieu_pre_scene(ST(ST_HERO), P_KONKALITE8, 24, 1, 2);
                    CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                    /* 0xBA60 */
                    if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) == 0) {
                        snprintf(g_buf24, sizeof(g_buf24), "OB24%c", (ST(ST_USED_VARIANT) + 65));
                        CB->etat_objet(g_buf24, -1, -1, -1, -1, -1, -1, -2, 0);
                        snprintf(g_buf77, sizeof(g_buf77), "OB77%c", (ST(ST_USED_VARIANT) + 65));
                        CB->etat_objet(g_buf77, -1, -1, -1, -1, -1, -1, -3, 0);
                        CB->animation("Sar_50");
                    }
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            } else {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("L31D50", P_ARKHAN2, "SG00PRE81", NULL);
                lieu_action_register("L31D51", P_KONKALITE8, "SP78DIA56", "SP78ARR01_10");
                CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xBBA8 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE8)) == VIE_DEAD) {
        CB->fouille(P_KONKALITE8);
        return;
    }
    /* 0xBC06 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L31D53", P_KONKALITE8);
        CB->combat(P_KONKALITE8, 0);
        CB->combat(P_KONKALITE10, 0);
        ST(V_ENGAGED_PERSONA) = 0x1EC8;
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 3 - sub_1000BC9B */
static void trig03(void) {
    /* 0xBCAA */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_perso(P_KONKALITE8) && !CB->direction_perso(P_KONKALITE10) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) && ST(ST_VIE(P_KONKALITE8)) != VIE_DEAD) {
        trig02();
        return;
    }
    /* 0xBD1E */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_KONKALITE10) && ST(ST_VIE(P_KONKALITE10)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L31D82", P_KONKALITE10);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->combat(P_KONKALITE10, 0);
        CB->combat(P_KONKALITE8, 0);
        ST(V_ENGAGED_PERSONA) = 0x1EC8;
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xBDFE */
    if (ST(0x68) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD && ST(0xEE8) < 5 && ST(0xEEC) < 5) {
        ST(0x68) = 1;
        CB->select_pile(1, 1);
        /* 0xBE71 */
        if (ST(0xEE8) != 2) {
            CB->stop_joindre(-P_KONKALITE8);
            CB->animation("SP78GAR26");
            CB->animation("P78GAR03");
            CB->joindre_perm(P_KONKALITE10, ST(ST_HERO));
        }
        CB->select_pile(2, 1);
        /* 0xBECA */
        if (ST(ST_LOCKED_CAMERA) != 4) {
            CB->change_camera(4, CAM_LOCKED);
        }
        CB->evenement(2, -1);
        CB->joindre_perm(-P_KONKALITE10, ST(ST_HERO));
        CB->dialogue("L31D54", P_KONKALITE10);
        /* 0xBF22 */
        if (ST(0xEE8) != 99) {
            ST(0xEE8) = 5;
        }
        /* 0xBF43 */
        if (ST(0xEEC) != 99) {
            ST(0xEEC) = 5;
        }
        CB->select_pile(0, 1);
    }
    /* 0xBF71 */
    if (ST(0x100) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD && ST(0xEEC) == 99) {
        ST(0x100) = 1;
        CB->select_pile(1, 1);
        CB->stop_joindre(-P_KONKALITE10);
        CB->animation("SP80GAR26");
        CB->animation("P80GAR03");
        CB->joindre_perm(P_KONKALITE10, ST(ST_HERO));
        CB->dialogue("L31D54", P_KONKALITE10);
        /* 0xC01C */
        if (ST(0xEEC) != 99) {
            ST(0xEEC) = 5;
        }
        CB->select_pile(0, 1);
    }
    /* 0xC047 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_KONKALITE10) && ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(ST(ST_HERO), 0, 0, 0, 80);
        /* 0xC0DB */
        if (ST(ST_LOCKED_CAMERA) != 4 && ST(0xEE8) < 6) {
            CB->change_camera(4, CAM_LOCKED);
        }
        CB->select_pile(2, 1);
        CB->joindre_perm(-P_KONKALITE10, ST(ST_HERO));
        CB->stop_joindre(P_KONKALITE10);
        CB->select_pile(0, 1);
        /* 0xC14D */
        if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || ST(ST_PLAYER_WEAPON) == ST(0x118)) {
            /* 0xC193 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xC1A5 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L31D90", P_ARKHAN3, "SG00DIA03", NULL);
                    lieu_action_register("L31D70", P_KONKALITE10, "SP80DIA56", "SP80ARR01_10");
                }
                /* 0xC1E6 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD22C", P_ARKHAN3);
                    lieu_action_register("L31D71", P_KONKALITE10, "SP80DIA56", "SP80ARR01_10");
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->combat(P_KONKALITE10, 0);
                ST(V_ENGAGED_PERSONA) = P_KONKALITE10;
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                /* 0xC268 */
                if (ST(0xEEC) <= 5 || ST(0xEEC) == 99) {
                    /* 0xC28C */
                    if (ST(0xAD8) == 0) {
                        ST(0xAD8) = 1;
                        ST(0xB1C) = 1;
                        /* 0xC2BC */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L31D02", P_ARKHAN2, "SG00DIA03", NULL);
                            lieu_action_register("L31D55", P_KONKALITE10, "SP80GAR03", "SP80GAR03_10");
                            lieu_action_register("L31D56", P_ARKHAN2, "SG00DIA03", NULL);
                            CB->joindre_perm(P_KONKALITE10, ST(ST_HERO));
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xC352 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L31D04A", P_ARKHAN2, "SG00DIA08", NULL);
                            lieu_action_register("L31D04B", P_ARKHAN2, "SG00DIA08", NULL);
                            lieu_action_register("L31D57", P_KONKALITE10, "SP80GAR03", "SP80GAR03_10");
                            CB->joindre_perm(P_KONKALITE10, ST(ST_HERO));
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0xC3E8 */
                    if (ST(0xAD8) == 1) {
                        /* 0xC3FA */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L31D58", P_ARKHAN2, "SG00DIA03", NULL);
                            lieu_action_register("L31D59", P_KONKALITE10, "SP80GAR03", "SP80GAR03_10");
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(V_ENGAGED_PERSONA), 80);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->combat(P_KONKALITE10, 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xC485 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L31D08", P_ARKHAN2, "SG00DIA08", NULL);
                            lieu_action_register("L31D60", P_KONKALITE10, "SP80GAR03", "SP80GAR03_10");
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(V_ENGAGED_PERSONA), 80);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->combat(P_KONKALITE10, 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }
                /* 0xC510 */
                if (ST(0xEEC) > 5 && ST(0xEEC) < 8) {
                    /* 0xC534 */
                    if (ST(0xAD8) == 0) {
                        ST(0xAD8) = 1;
                        lieu_action_register("L31D10", P_ARKHAN2, "SG00DIA03", NULL);
                        lieu_action_register("L31D61A", P_KONKALITE10, "SP80DIA56", "SP80ARR01_10");
                        lieu_action_register("L31D61B", P_KONKALITE10, "SP80DIA53", "SP80ARR01_10");
                        lieu_action_register("L31D61C", P_KONKALITE10, "SP80DIA62", "SP80ARR01_10");
                        lieu_action_register("L31D61D", P_KONKALITE10, "SP80DIA50", "SP80ARR01_10");
                        lieu_action_register("L31D61E", P_KONKALITE10, "SP80DIA65", "SP80ARR01_10");
                        lieu_action_register("L31D61F", P_KONKALITE10, "SP80DIA57", "SP80ARR01_10");
                        lieu_action_register("L31D62", P_ARKHAN2, "SG00DIA03", NULL);
                        lieu_action_register("L31D63A", P_KONKALITE10, "SP80DIA54", "SP80ARR01_10");
                        lieu_action_register("L31D63B", P_KONKALITE10, "SP80DIA60", "SP80ARR01_10");
                        CB->joindre_perm(P_KONKALITE10, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xC688 */
                    if (ST(0xAD8) == 1) {
                        ST(0xAD8) = 2;
                        /* 0xC6A9 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L31D64", P_ARKHAN2, "SG00DIA03", NULL);
                        } else {
                            lieu_action_register("L31D65", P_ARKHAN2, "SG00DIA03", NULL);
                        }
                        lieu_action_register("L31D66A", P_KONKALITE10, "SP80DIA62", "SP80ARR01_10");
                        lieu_action_register("L31D66B", P_KONKALITE10, "SP80DIA63", "SP80ARR01_10");
                        lieu_action_register("L31D66C", P_KONKALITE10, "SP80DIA54", "SP80ARR01_10");
                        CB->joindre_perm(P_KONKALITE10, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xC776 */
                    if (ST(0xAD8) == 2) {
                        ST(0xAD8) = 3;
                        lieu_action_register("L31D17", P_ARKHAN2, "SG00DIA03", NULL);
                        lieu_action_register("L31D67", P_KONKALITE10, "SP80DIA60", "SP80ARR01_10");
                        CB->joindre_perm(P_KONKALITE10, ST(ST_HERO));
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xC805 */
                    if (ST(0xAD8) == 3) {
                        /* 0xC817 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L31D19", P_ARKHAN2, "SG00DIA03", NULL);
                        }
                        /* 0xC83F */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L31D68", P_ARKHAN2, "SG00DIA03", NULL);
                        }
                        lieu_action_register("L31D69", P_KONKALITE10, "SP80DIA65", "SP80ARR01_10");
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(V_ENGAGED_PERSONA), 80);
                        CB->bloque(BLOQUE_NONE);
                        CB->combat(P_KONKALITE10, 0);
                        return;
                    }
                }
            }
        }
        /* 0xC8CA */
        if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(0x118)) {
            /* 0xC910 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xC922 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L31D42", P_ARKHAN3, "SG03DIA03", NULL);
                }
                /* 0xC94A */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD22D", P_ARKHAN3);
                }
                lieu_action_register("L31D75A", P_KONKALITE10, "SP80DIA56", "SP80ARR01_10");
                lieu_action_register("L31D75B", P_KONKALITE10, "SP80DIA56", "SP80ARR01_10");
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->combat(P_KONKALITE10, 0);
                ST(V_ENGAGED_PERSONA) = P_KONKALITE10;
                CB->bloque(BLOQUE_NONE);
                return;
            } else if (ST(ST_PLAYER_WEAPON) == 24) {
                /* 0xC9F7 */
                if (ST(0xADC) == 0) {
                    ST(0xADC) = 1;
                    ST(0xAD8) = 0;
                    /* 0xCA34 */
                    if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) != 0) {
                        ST(0x140 + 4 * ST(ST_USED_VARIANT))--;
                    }
                    /* 0xCA5E */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L31D40", P_ARKHAN2, "SG02DIA03", NULL);
                    }
                    /* 0xCA86 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L31D41", P_ARKHAN2, "SG02DIA03", NULL);
                    }
                    CB->animation("SP80GAR27");
                    lieu_action_register("L31D76", P_KONKALITE10, "P80DIA53", "SP80ARR01_10");
                    lieu_pre_scene(ST(ST_HERO), P_KONKALITE10, 24, 1, 2);
                    CB->animation("Sar_50");
                    ST(0xEEC) = 6;
                    CB->animation("SP80ACT01");
                    lieu_action_register("L31D77", P_KONKALITE10, "P80ARR01", "SP80ARR01_10");
                    lieu_pre_scene(ST(ST_HERO), P_KONKALITE10, 24, 1, 2);
                    /* 0xCB53 */
                    if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) == 0) {
                        snprintf(g_buf24, sizeof(g_buf24), "OB24%c", (ST(ST_USED_VARIANT) + 65));
                        CB->etat_objet(g_buf24, -1, -1, -1, -1, -1, -1, -2, 0);
                        snprintf(g_buf77, sizeof(g_buf77), "OB77%c", (ST(ST_USED_VARIANT) + 65));
                        CB->etat_objet(g_buf77, -1, -1, -1, -1, -1, -1, -3, 0);
                        CB->animation("Sar_50");
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->joindre_perm(-P_KONKALITE10, ST(ST_HERO));
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xCC22 */
                if (ST(0xADC) == 1) {
                    lieu_action_register("L31D46", P_ARKHAN2, "SG02DIA03", NULL);
                    lieu_action_register("L31D72", P_KONKALITE10, "SP80DIA53", "SP80ARR01_10");
                    lieu_pre_scene(ST(ST_HERO), P_KONKALITE10, 24, 1, 2);
                    CB->animation("Sar_50");
                    ST(0xEEC)++;
                    /* 0xCCA1 */
                    if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) != 0) {
                        ST(0x140 + 4 * ST(ST_USED_VARIANT))--;
                    }
                    /* 0xCCCB */
                    if (ST(0xEEC) < 8) {
                        CB->animation("SP80ACT01");
                        lieu_action_register("L31D78", P_KONKALITE10, "P80ARR01", "SP80ARR01_10");
                    }
                    /* 0xCD01 */
                    if (ST(0xEEC) == 8) {
                        CB->animation("SP80ACT01");
                        lieu_action_register("L31D79", P_KONKALITE10, "P80ARR01", "SP80ARR01_10");
                        CB->animation("SP80ACT02");
                        CB->animation("P80ACT03");
                        CB->animation("P80CAD01");
                        /* 0xCD65 */
                        if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) == 0) {
                            snprintf(g_buf24, sizeof(g_buf24), "OB24%c", (ST(ST_USED_VARIANT) + 65));
                            CB->link_objet(80, g_buf24, 27, 0);
                            snprintf(g_buf77, sizeof(g_buf77), "OB77%c", (ST(ST_USED_VARIANT) + 65));
                            CB->link_objet(80, g_buf77, 27, 1);
                            CB->animation("Sar_50");
                            CB->fouille(P_KONKALITE10);
                            CB->bloque(BLOQUE_ALL);
                            /* as in trigger 2: x/z/facing are the results of the queued calls */
                            CB->divers_empile(DV_DESC_POS, P_KONKALITE10, 2);
                            CB->divers_empile(DV_DESC_POS, P_KONKALITE10, 1);
                            CB->divers_empile(DV_DESC_POS, P_KONKALITE10, 0);
                            CB->pos_perso(P_KONKALITE10, 0, 0, 0, 2, "P80CAD01", -1, 0);
                            CB->animation("Sar_50");
                            CB->modify_var(&ST(ST_VIE(P_KONKALITE10)), 2);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        CB->animation("Sar_50");
                        CB->fouille(P_KONKALITE10);
                        CB->bloque(BLOQUE_ALL);
                        /* as above */
                        CB->divers_empile(DV_DESC_POS, P_KONKALITE10, 2);
                        CB->divers_empile(DV_DESC_POS, P_KONKALITE10, 1);
                        CB->divers_empile(DV_DESC_POS, P_KONKALITE10, 0);
                        CB->pos_perso(P_KONKALITE10, 0, 0, 0, 2, "P80CAD01", 3, 0);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_VIE(P_KONKALITE10)), 2);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    lieu_pre_scene(ST(ST_HERO), P_KONKALITE10, 24, 1, 2);
                    CB->joindre_perm(P_KONKALITE10, ST(ST_HERO));
                    /* 0xCF81 */
                    if (ST(0x140 + 4 * ST(ST_USED_VARIANT)) == 0) {
                        snprintf(g_buf24, sizeof(g_buf24), "OB24%c", (ST(ST_USED_VARIANT) + 65));
                        CB->etat_objet(g_buf24, -1, -1, -1, -1, -1, -1, -2, 0);
                        snprintf(g_buf77, sizeof(g_buf77), "OB77%c", (ST(ST_USED_VARIANT) + 65));
                        CB->etat_objet(g_buf77, -1, -1, -1, -1, -1, -1, -3, 0);
                        CB->animation("Sar_50");
                    }
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(-P_KONKALITE10, ST(ST_HERO));
                    return;
                }
            } else {
                ST(0x118) = ST(ST_PLAYER_WEAPON);
                lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE81", NULL);
                lieu_action_register("L31D80", P_KONKALITE10, "SP80DIA56", "SP80ARR01_10");
                CB->joindre_perm(P_KONKALITE8, ST(ST_HERO));
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xD0E2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE10)) == VIE_DEAD) {
        CB->fouille(P_KONKALITE10);
        return;
    }
    /* 0xD140 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE10)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_KONKALITE10);
        CB->dialogue("L31D82", P_KONKALITE10);
        CB->animation("SP80GAR03_150");
        CB->combat(P_KONKALITE10, 0);
        ST(V_ENGAGED_PERSONA) = P_KONKALITE10;
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 4 - sub_1000D1DE */
static void trig04(void) {
    /* 0xD1F0 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_BORK) && ST(ST_VIE(P_BORK)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L31D506", P_BORK);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xD29B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_BORK)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD303 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
            CB->dialogue("LCOD20", P_ARKHAN2);
        }
        /* 0xD337 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        /* 0xD36B */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("L31D505", P_ARKHAN2);
        }
        /* 0xD39F */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD63", P_ARKHAN3);
        }
        CB->dialogue("L31D506", P_BORK);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->animation("ar");
        return;
    }
    /* 0xD400 - empty compare chains (every branch jumps to the same place) */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_BORK)) != VIE_DEAD) {
        /* no body */
    }
    /* 0xD44E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_BORK)) != VIE_DEAD) {
        /* no body */
    }
    /* 0xD49C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_BORK)) == VIE_DEAD) {
        CB->fouille(P_BORK);
        return;
    }
    /* 0xD4F4 */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_BORK)) != VIE_DEAD) {
        /* no body */
    }
}

/* trigger 5 - sub_1000D52F */
static void trig05(void) {
    /* 0xD541 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KONKALITE11) && ST(ST_VIE(P_KONKALITE11)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L31D507", P_KONKALITE11);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xD5EC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD654 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
            CB->dialogue("LCOD20", P_ARKHAN2);
        }
        /* 0xD688 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            CB->dialogue("LCOD21", P_ARKHAN3);
        }
        /* 0xD6BC */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD61", P_ARKHAN2);
        }
        /* 0xD6F0 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("LCOD63", P_ARKHAN3);
        }
        CB->dialogue("L31D507", P_KONKALITE11);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->animation("ar");
        return;
    }
    /* 0xD756 - empty compare chains */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
        /* no body */
    }
    /* 0xD7A4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
        /* no body */
    }
    /* 0xD7F2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_KONKALITE11)) == VIE_DEAD) {
        CB->fouille(P_KONKALITE11);
        return;
    }
    /* 0xD845 */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE11)) != VIE_DEAD) {
        /* no body */
    }
}

/* trigger 7 - sub_1000D88B */
static void trig07(void) {
    /* 0xD89D */
    if (ST(0xEF8) == 1 && ST(ST_VIE(P_BORK)) == VIE_DEAD) {
        ST(ST_LEVEL) = 31;
        CB->change_piece(34, 0);
    }
    /* 0xD8DA */
    if ((ST(0xEF8) != 1 && ST(0xEF8) != 11) || ST(0xE8C) >= 1) {
        CB->divers(DV_MUSIC, 1, 4);
        /* 0xD922 */
        if (ST(0xEE0) == 1) {
            /* 0xD931 */
            if (ST(V_ENGAGED_PERSONA) != 0) {
                ST(0xEE0) = 10;
                ST(V_ENGAGED_PERSONA) = 0;
            }
            ST(ST_LEVEL) = 31;
            CB->divers(DV_CUT_SPEECH, 0, 0);
            CB->change_piece(34, 0);
            return;
        }
        /* 0xD98B */
        if (ST(0xEE0) == 2) {
            /* 0xD99A */
            if (ST(V_ENGAGED_PERSONA) != 0) {
                ST(0xEE0) = 30;
                ST(V_ENGAGED_PERSONA) = 0;
            }
            ST(ST_LEVEL) = 31;
            CB->divers(DV_CUT_SPEECH, 0, 0);
            CB->change_piece(34, 0);
            return;
        }
        ST(ST_LEVEL) = 31;
        CB->change_piece(34, 0);
    }
}

/* trigger 8 - sub_1000DA06 */
static void trig08(void) {
    CB->retire_zone(8);
    /* 0xDA23 */
    if (ST(0xEE0) == 1) {
        /* 0xDA32 */
        if (ST(V_ENGAGED_PERSONA) != 0) {
            ST(0xEE0) = 10;
            ST(V_ENGAGED_PERSONA) = 0;
        }
        ST(ST_LEVEL) = 31;
        CB->divers(DV_CUT_SPEECH, 0, 0);
        CB->change_piece(32, 0);
        return;
    }
    /* 0xDA8C */
    if (ST(0xEE0) == 2) {
        /* 0xDA9B */
        if (ST(V_ENGAGED_PERSONA) != 0) {
            ST(0xEE0) = 30;
            ST(V_ENGAGED_PERSONA) = 0;
        }
        ST(ST_LEVEL) = 31;
        CB->divers(DV_CUT_SPEECH, 0, 0);
        CB->change_piece(32, 0);
        return;
    }
    ST(ST_LEVEL) = 31;
    CB->divers(DV_CUT_SPEECH, 0, 0);
    CB->change_piece(32, 0);
    return;
}

/* trigger 12 - sub_1000DB3C */
static void trig12(void) {
    /* 0xDB4E */
    if (ST(0xEE0) == 1) {
        /* 0xDB5D */
        if (ST(V_ENGAGED_PERSONA) != 0) {
            ST(0xEE0) = 10;
            ST(V_ENGAGED_PERSONA) = 0;
        }
        ST(ST_LEVEL) = 31;
        CB->change_piece(30, 0);
        return;
    }
    /* 0xDBA8 */
    if (ST(0xEE0) == 2) {
        /* 0xDBB7 */
        if (ST(V_ENGAGED_PERSONA) != 0) {
            ST(0xEE0) = 30;
            ST(V_ENGAGED_PERSONA) = 0;
        }
        ST(ST_LEVEL) = 31;
        CB->change_piece(30, 0);
        return;
    }
    ST(ST_LEVEL) = 31;
    CB->change_piece(30, 0);
}

/* trigger 14 - sub_1000DC1F */
static void trig14(void) {
    /* 0xDC2E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x56CE)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDCF1 */
        if (ST(0x59C) == 0) {
            ST(0x59C) = 1;
            CB->animation("SG00PRE07");
            CB->etat_objet("OB09", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        CB->animation("SG00ATT01");
        /* 0xDD74 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            ST(V_DLG_RANDOM) = CB->random(3);
            /* 0xDD9D */
            if (ST(V_DLG_RANDOM) == 0) {
                CB->dialogue("LCOD10", P_ARKHAN2);
            }
            /* 0xDDBF */
            if (ST(V_DLG_RANDOM) == 1) {
                CB->dialogue("LCOD76", P_ARKHAN2);
            }
            /* 0xDDE1 */
            if (ST(V_DLG_RANDOM) == 2) {
                CB->dialogue("LCOD07", P_ARKHAN2);
            }
        }
        /* 0xDE03 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            ST(V_DLG_RANDOM) = CB->random(3);
            /* 0xDE2C */
            if (ST(V_DLG_RANDOM) == 0) {
                CB->dialogue("LCOD11", P_ARKHAN3);
            }
            /* 0xDE4E */
            if (ST(V_DLG_RANDOM) == 1) {
                CB->dialogue("LCOD77", P_ARKHAN3);
            }
            /* 0xDE70 */
            if (ST(V_DLG_RANDOM) == 2) {
                CB->dialogue("LCOD08", P_ARKHAN3);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 - sub_1000DEB4 */
static void trig15(void) {
    /* 0xDEC3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x6286)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0xDF91 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L31D900", P_ARKHAN2);
        }
        /* 0xDFB3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L31D901", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 - sub_1000DFF7 */
static void trig16(void) {
    /* 0xE006 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x61A8)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0xE0D4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L31D900", P_ARKHAN2);
        }
        /* 0xE0F6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L31D901", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 17 - sub_1000E13A */
static void trig17(void) {
    /* 0xE149 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x6E3E)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0xE217 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L31D900", P_ARKHAN2);
        }
        /* 0xE239 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L31D901", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 18 - sub_1000E27D */
static void trig18(void) {
    /* 0xE28C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x88B8)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0xE35A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L31D900", P_ARKHAN2);
        }
        /* 0xE37C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L31D901", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 19 - sub_1000E3C0 */
static void trig19(void) {
    /* 0xE3CF */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0xE49D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L31D900", P_ARKHAN2);
        }
        /* 0xE4BF */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L31D901", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 20 - sub_1000E503 */
static void trig20(void) {
    /* 0xE515 */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->divers(DV_DESC_POS, ST(ST_HERO), 2) > 36000 && CB->divers(DV_DESC_POS, ST(ST_HERO), 2) < 61500) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0xE625 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xE637 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L31D902", P_ARKHAN2);
            }
            /* 0xE659 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L31D904", P_ARKHAN3);
            }
        }
        /* 0xE67B */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xE68D */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L31D903", P_ARKHAN2);
            }
            /* 0xE6AF */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L31D905", P_ARKHAN3);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 21 - sub_1000E6F3 */
static void trig21(void) {
    /* 0xE705 */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->divers(DV_DESC_POS, ST(ST_HERO), 2) > 40000 && CB->divers(DV_DESC_POS, ST(ST_HERO), 2) < 64000) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0xE815 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xE827 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L31D902", P_ARKHAN2);
            }
            /* 0xE849 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L31D904", P_ARKHAN3);
            }
        }
        /* 0xE86B */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xE87D */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L31D903", P_ARKHAN2);
            }
            /* 0xE89F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L31D905", P_ARKHAN3);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 22 - sub_1000E8E3 */
static void trig22(void) {
    /* 0xE8F2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x33F4)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00ATT01");
        /* 0xE9C0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L31D906", P_ARKHAN2);
        }
        /* 0xE9E2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L31D907", P_ARKHAN3);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 23 - sub_1000EA26 */
static void trig23(void) {
    /* 0xEA35 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(23)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xEB07 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xEB19 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xEB33 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT71");
                    lieu_action_register("LCOD76", P_ARKHAN2, "P02ACT75", "SP02ACT77");
                } else {
                    CB->animation("SP02ACT70");
                    lieu_action_register("LCOD76", P_ARKHAN2, "P02ACT74", "SP02ACT76");
                }
            }
            /* 0xEB92 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xEBAC */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT73");
                    lieu_action_register("LCOD76", P_ARKHAN2, "P02ACT75", "SP02ACT79");
                } else {
                    CB->animation("SP02ACT72");
                    lieu_action_register("LCOD76", P_ARKHAN2, "P02ACT74", "SP02ACT78");
                }
            }
        }
        /* 0xEC0B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xEC1D */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xEC37 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT71");
                    lieu_action_register("LCOD77", P_ARKHAN3, "P03ACT75", "SP03ACT77");
                } else {
                    CB->animation("SP03ACT70");
                    lieu_action_register("LCOD77", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                }
            }
            /* 0xEC96 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xECB0 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT73");
                    lieu_action_register("LCOD77", P_ARKHAN3, "P03ACT75", "SP03ACT79");
                } else {
                    CB->animation("SP03ACT72");
                    lieu_action_register("LCOD77", P_ARKHAN3, "P03ACT74", "SP03ACT78");
                }
            }
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}
