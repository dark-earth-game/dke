/*
 * lieu20_trig.c — Native hand-port of the LIEU20.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu20.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu20_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table (and the triggers other triggers call).
 * ---------------------------------------------------------------------- */
static void trig02(void); static void trig03(void); static void trig04(void);
static void trig06(void); static void trig07(void); static void trig10(void);
static void trig11(void); static void trig12(void); static void trig13(void);
static void trig14(void); static void trig19(void); static void trig26(void);
static void trig27(void); static void trig35(void); static void trig36(void);
static void trig37(void); static void trig38(void); static void trig39(void);
static void trig41(void);
static void trig_noop(void);   /* the 22 stub slots */

/* dword_10023FB0[42] — filled at INIT (before the reload-flag test) with the
 * 5-byte jmp thunks; static here. A slot is "stub" when it is the empty
 * handler shared with LIEU01.
 *   idx handler        idx handler        idx handler
 *    0  100073F0         14  1000EE58         28  100100D4 stub
 *    1  1000B604 stub    15  1000F530 stub    29  100100DF stub
 *    2  1000B60F         16  1000F53B stub    30  100100EA stub
 *    3  1000C0C6         17  1000F546 stub    31  100100F5 stub
 *    4  1000C56F         18  1000F551 stub    32  10010100 stub
 *    5  1000CA36 stub    19  1000F55C         33  1001010B stub
 *    6  1000CA41         20  1000F580 stub    34  10010116 stub
 *    7  1000D46D         21  1000F58B stub    35  10010121
 *    8  1000DD31 stub    22  1000F596 stub    36  1001031D
 *    9  1000DD3C stub    23  1000F5A1 stub    37  1001069A
 *   10  1000DD47         24  1000F5AC stub    38  1001070B
 *   11  1000E0CD         25  1000F5B7 stub    39  1001077C
 *   12  1000E34B         26  1000F5C2         40  100107ED stub
 *   13  1000E744         27  1000FF09         41  100085DD
 */
static void (*const g_trigger[42])(void) = {
    l20_trig00, trig_noop, trig02, trig03,
    trig04, trig_noop, trig06, trig07,
    trig_noop, trig_noop, trig10, trig11,
    trig12, trig13, trig14, trig_noop,
    trig_noop, trig_noop, trig_noop, trig19,
    trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig26, trig27,
    trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig35,
    trig36, trig37, trig38, trig39,
    trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10023FB0]). */
void l20_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — INIT default: start points, state reset, zones, objects, actor placement */
void l20_trig00(void) {
    /* 0x73FB: start point 0 -> empty branch */
    if (ST(ST_START_POINT(20)) == 0) {
    }
    /* 0x7402 */
    if (ST(ST_START_POINT(20)) == 1) {
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xAF8) = 0;
        ST(ST_HERO) = P_ARKHAN1;
        CB->vire_col(0x17, 1);
        ST(0xE7C) = 3;
        ST(ST_LEVEL) = 10;
    }
    /* 0x747E */
    if (ST(ST_START_POINT(20)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xB58) = 1;
        ST(ST_LEVEL) = 21;
    }
    /* 0x74D8 */
    if (ST(ST_START_POINT(20)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(0x330) = 1;
        ST(0xE7C) = 3;
    }
    /* 0x7526 */
    if (ST(ST_START_POINT(20)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(0xB28) = 1;
    }
    /* 0x7565 */
    if (ST(ST_START_POINT(20)) == 6) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xB3C) = 2;
        ST(0xE74) = 1;
        ST(ST_VIE(P_GUARDIAN10)) = VIE_DEAD;
    }
    /* 0x75D1 */
    if (ST(ST_START_POINT(20)) == 7) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xE74) = 2;
        ST(0xB3C) = 2;
    }
    /* 0x762E */
    if (ST(ST_START_POINT(20)) == 8) {
        ST(ST_LEVEL) = 21;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 1;
        ST(0x330) = 1;
        ST(0xE7C) = 3;
        ST(0xF64) = 0;
    }
    ST(ST_START_POINT(20)) = 0;
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
    CB->bruitage("L20AMB1", -1, -1);
    /* 0x776C */
    if (ST(V_GUARD_FIGHT_DONE) == 0 || ST(0xF38) == 0) {
        CB->load_anim("220OUV01", 0);
    }
    /* 0x779D */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xF38) == 1) {
        CB->load_anim("220OUV03", 0);
    }
    /* 0x77CE */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->load_anim("220OUV02", 1);
    }
    /* 0x77F0 */
    if (ST(0xB3C) == 0) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x7814 */
    if (ST(0xB3C) == 1) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    CB->ajoute_zone(10, 4493, -3394, 442, 80, 1);
    CB->ajoute_zone(11, 4586, -5912, 140, 80, 1);
    /* 0x7874 */
    if (ST(0xAF8) == 0 && ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->ajoute_zone(20, 5528, -4541, 450, 450, 0);
        CB->ajoute_zone(12, 5600, -4513, 80, 150, 0);
    }
    /* 0x78D7 */
    if (ST(0xAF8) == 1) {
        CB->ajoute_zone(27, 5593, -4749, 105, 195, 0);
        CB->ajoute_zone(12, 5600, -4513, 80, 150, 0);
    }
    CB->ajoute_zone(13, 3928, -4466, 100, 100, 0);
    CB->ajoute_zone(14, 4500, -5083, 475, 140, 0);
    CB->ajoute_zone(35, 4482, -4653, 1000, 1000, 0);
    CB->ajoute_zone(36, 3655, -5477, 400, 400, 0);
    CB->ajoute_zone(37, 5305, -5477, 400, 400, 0);
    CB->ajoute_zone(38, 3655, -3832, 400, 400, 0);
    CB->ajoute_zone(39, 5305, -3832, 400, 400, 0);
    /* 0x7A06 */
    if (ST(0xAF8) == 0 && ST(V_GUARD_FIGHT_DONE) == 0 && ST(0xE7C) == 1) {
        CB->ajoute_zone(15, 5754, -4621, 30, 30, 0);
        CB->ajoute_zone(16, 5679, -4528, 30, 30, 0);
    }
    /* 0x7A72 */
    if (ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB58) == 1) {
        CB->ajoute_zone(17, 5594, -4523, 20, 20, 0);
        CB->ajoute_zone(18, 5747, -4560, 20, 20, 0);
    }
    /* 0x7ADE */
    if ((ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(0x32C) == 1 && ST(0x330) == 1) || ST(0xB28) == 2) {
        CB->etat_objet("OB03B", 3970, -330, -4480, 0x6600, 0xE800, 0x4800, 0, 6);
    }
    /* 0x7B62 */
    if (ST(ST_LEVEL) == 10) {
        CB->pos_perso(ST(ST_HERO), 4620, -3532, 0x7EE8, 1, "ar", 1, 0);
    }
    /* 0x7B9C */
    if (ST(ST_LEVEL) == 21) {
        CB->pos_perso(ST(ST_HERO), 4647, -5740, 0xF6E0, 1, "ar", 1, 0);
    }
    /* 0x7BD6 */
    if (ST(ST_LEVEL) == 27) {
        CB->pos_perso(ST(ST_HERO), 5552, -4496, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x7C24 */
    if (ST(ST_LEVEL) != 10 && ST(ST_LEVEL) != 21 && ST(ST_LEVEL) != 27) {
        CB->pos_perso(ST(ST_HERO), 4620, -3532, 0x7EE8, 1, "ar", 1, 0);
    }
    /* 0x7C7F */
    if (ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 1 && ST(0xF64) == 0) {
        CB->pos_perso(ST(ST_HERO), 4660, -5617, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    /* 0x7CF4 */
    if (ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB58) == 1) {
        CB->pos_perso(ST(ST_HERO), 4600, -5765, 0xEF6A, 1, "ar", 1, 0);
    }
    /* 0x7D55 */
    if (ST(0xAF8) == 0 && ST(V_GUARD_FIGHT_DONE) == 0) {
        /* 0x7D79 */
        if (ST(V_EVENT_17) != 3) {
            CB->pos_perso(P_GUARDIAN1, 5596, -4730, 0x1457, 1, "P41ARR02", 2, 1);
            CB->vire_col(0x17, 1);
        }
        /* 0x7DC0 */
        if (ST(V_EVENT_17) == 3) {
            CB->pos_perso(P_ZED84, 5048, -4868, 0xD78A, 1, "P84ARR01", 5, 0);
            CB->link_objet(P_ZED84, "OB68", 0x1D, 1);
        }
        /* 0x7E0B */
        if (ST(0xE7C) == 1) {
            ST(0xA4) = (ST(ST_DEBUG_PRINT) - ST(0xA8));
            CB->vire_col(0x17, 0);
            ST(0xE7C) = 2;
            ST(0xE78) = 7;
            /* 0x7E6B */
            if (ST(0xA4) >= 100 && ST(0xA4) < 6000) {
                CB->pos_perso(P_LORY, 5263, -4291, 0xA8F3, 1, "P15ARR01", 3, 1);
            }
            /* 0x7EBA */
            if (ST(0xA4) >= 6000) {
                CB->vire_col(0x17, 1);
                ST(0xE7C) = 3;
            }
        }
    }
    /* 0x7EEB */
    if (ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 1 && ST(0xF64) == 0) {
        CB->pos_perso(P_THANANDAR25, 5048, -4868, 0xA, 1, "P25ARR01", -1, 0);
    }
    /* 0x7F43 */
    if (ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB58) == 1) {
        /* 0x7F7C */
        if (ST(0xA8) != -666) {
            ST(0xA4) = (ST(ST_DEBUG_PRINT) - ST(0xA8));
        } else {
            ST(0xA4) = 666;
        }
        /* 0x7FC5 */
        if (ST(0xA4) > 0 && ST(0xA4) < 10000) {
            CB->pos_perso(P_LORY, 4770, -5626, 0xDDA6, 1, "P15ARR01", 3, 0);
            CB->pos_perso(P_PHEDORIA, 4617, -5598, 0xC7C6, 1, "P26ARR01", 4, 0);
        }
        /* 0x8039 */
        if (ST(0xA4) >= 10000) {
            ST(0xB58) = 10;
        }
    }
    /* 0x805A */
    if (ST(0xE74) == 1 && ST(ST_VIE(P_GUARDIAN10)) == VIE_DEAD) {
        ST(0xE74) = 111;
    }
    /* 0x808D */
    if (ST(0xAF8) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xE74) != 0 && ST(0xE74) != 99) {
        /* 0x80D5 */
        if (ST(0xE74) == 1 || ST(0xE74) == 10) {
            /* 0x80F9 */
            if (ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
                CB->pos_perso(P_GUARDIAN9, 5456, -4425, 0x68E0, 1, "P49DIA53", 6, 1);
            } else {
                CB->pos_perso(P_GUARDIAN9, 0, 0, 0, 2, "P49CAD", 6, 0);
            }
            /* 0x8151 */
            if (ST(ST_VIE(P_GUARDIAN10)) != VIE_DEAD) {
                CB->pos_perso(P_GUARDIAN10, 5417, -4573, 0xF052, 1, "P50DIA57", 7, 1);
            } else {
                CB->pos_perso(P_GUARDIAN10, 0, 0, 0, 2, "P50CAD", 7, 0);
            }
        }
        /* 0x81A9 */
        if (ST(0xE74) == 11) {
            CB->pos_perso(P_GUARDIAN9, 0, 0, 0, 2, "P49CAD", 6, 0);
            CB->pos_perso(P_GUARDIAN10, 0, 0, 0, 2, "P50CAD", 7, 0);
        }
        /* 0x81F3 */
        if (ST(0xE74) == 111 || ST(0xE74) == 112) {
            /* 0x8217 */
            if (ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
                CB->pos_perso(P_GUARDIAN9, 5484, -4521, 0x3DEB, 1, "P49ARR02", 0x1A, 1);
            } else {
                CB->pos_perso(P_GUARDIAN9, 0, 0, 0, 2, "P49CAD", 0x1A, 0);
            }
        }
        /* 0x826F */
        if (ST(0xE74) == 115) {
            CB->pos_perso(P_GUARDIAN9, 0, 0, 0, 2, "P49CAD", 0x1A, 0);
        }
        /* 0x829D */
        if (ST(0xE74) == 2 || ST(0xE74) == 20) {
            /* 0x82C1 */
            if (ST(ST_VIE(P_GUARDIAN11)) != VIE_DEAD) {
                CB->pos_perso(P_GUARDIAN11, 5075, -4696, 0xBC76, 1, "P51ARR03", 8, 1);
            } else {
                CB->pos_perso(P_GUARDIAN11, 0, 0, 0, 2, "P51CAD", 8, 0);
            }
            /* 0x8319 */
            if (ST(ST_VIE(52)) != VIE_DEAD) {
                CB->pos_perso(52, 5273, -4703, 0x3BF1, 1, "P52ARR04", 9, 1);
            } else {
                CB->pos_perso(52, 0, 0, 0, 2, "P52CAD", 9, 0);
            }
        }
        /* 0x8371 */
        if (ST(0xE74) == 21) {
            CB->pos_perso(P_GUARDIAN11, 0, 0, 0, 2, "P51CAD", 8, 0);
            CB->pos_perso(52, 0, 0, 0, 2, "P52CAD", 9, 0);
        }
    }
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    ST(0x88) = 0;
    ST(0x90) = 0;
    ST(0x94) = 0;
    ST(0xB8) = 0;
    ST(0xBC) = 0;
    ST(0xD4) = 0;
    ST(0xE0) = 0;
    ST(0xF0) = 0;
    ST(0xFC) = 0;
    ST(0x100) = 0;
    ST(0x114) = ST(ST_DEBUG_PRINT);
    ST(V_WEAPON_WARNED) = 0;
    ST(0x4C) = 0;
    /* 0x84EF */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0) {
        /* 0x8525 */
        if (ST(0xB98) == 0) {
            ST(0xB98) = 2;
        } else if (ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
            ST(0xB98) = 0;
        }
    }
    /* 0x857E */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }
}

/* trigger 02 — sub_1000B60F — Guardian 1 (door guard): cry, talk, strike */
static void trig02(void) {
    /* 0xB621 */
    if (ST(0x830) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        CB->select_pile(2, 1);
        CB->joindre_perm(P_GUARDIAN1, ST(ST_HERO));
        ST(0x830) = 1;
        CB->select_pile(0, 1);
    }
    /* 0xB684 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_GUARDIAN1) && ST(ST_VIE(P_GUARDIAN1)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_GUARDIAN1);
        lieu_action_register("L20D28", P_GUARDIAN1, "SP41DIA59", "SP41ARR02");
        CB->joindre_perm(P_GUARDIAN1, ST(ST_HERO));
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xB768 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN1)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_GUARDIAN1);
        /* 0xB7ED */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0xB7FF */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xB811 */
                if (ST(0x834) == 0) {
                    ST(0x834) = 1;
                    CB->evenement(1, -1);
                    lieu_action_register("L20D15A", P_ARKHAN1, "SP01ATT74", NULL);
                    CB->stop_joindre(-P_GUARDIAN1);
                    lieu_action_register("L20D16A", P_GUARDIAN1, "SP41DIA56", "SP41ARR02_100");
                    lieu_action_register("L20D16B", P_GUARDIAN1, "SP41DIA56", "SP41ARR02_100");
                    lieu_action_register("L20D15B", P_ARKHAN1, "SG01DIA03", NULL);
                    lieu_action_register("L20D16C", P_GUARDIAN1, "SP41DIA53", "SP41ARR02_100");
                    CB->joindre_perm(P_GUARDIAN1, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(0xBC), ST(ST_DEBUG_PRINT));
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xB920 */
                if (ST(0x834) == 1) {
                    ST(0x834) = 2;
                    CB->evenement(1, -1);
                    lieu_action_register("L20D17A", P_ARKHAN1, "SG01DIA06", "SG01DIA06");
                    lieu_action_register("L20D17B", P_ARKHAN1, "SG01DIA06", NULL);
                    CB->stop_joindre(-P_GUARDIAN1);
                    lieu_action_register("L20D18A", P_GUARDIAN1, "SP41DIA56", "SP41DIA56");
                    lieu_action_register("L20D18B", P_GUARDIAN1, "SP41DIA54", "SP41DIA54");
                    lieu_action_register("L20D18C", P_GUARDIAN1, "SP41DIA53", "SP41DIA54");
                    lieu_action_register("L20D18D", P_GUARDIAN1, "SP41DIA54", "SP41ARR02");
                    lieu_action_register("L20D18E", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    CB->joindre_perm(P_GUARDIAN1, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(0xBC), ST(ST_DEBUG_PRINT));
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xBA64 */
                if (ST(0x834) >= 2) {
                    ST(0x834)++;
                    lieu_action_register("L20D19", P_ARKHAN1, "SG01DIA06", NULL);
                    CB->evenement(1, -1);
                    CB->stop_joindre(-P_GUARDIAN1);
                    /* 0xBAB2 */
                    if (ST(0x834) == 3) {
                        lieu_action_register("L20D20A", P_GUARDIAN1, "SP41DIA54", "SP41DIA56");
                        lieu_action_register("L20D20B", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    } else {
                        lieu_action_register("L20D21", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    }
                    CB->joindre_perm(P_GUARDIAN1, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(0xBC), ST(ST_DEBUG_PRINT));
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xBB70 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xBB82 */
                if (ST(0x834) == 0) {
                    ST(0x834) = 1;
                    CB->evenement(1, -1);
                    lieu_action_register("L20D22", P_ARKHAN1, "SG01DIA06", NULL);
                    CB->stop_joindre(-P_GUARDIAN1);
                    lieu_action_register("L20D23A", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    lieu_action_register("L20D23B", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    CB->joindre_perm(-P_GUARDIAN1, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(0xBC), ST(ST_DEBUG_PRINT));
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xBC62 */
                if (ST(0x834) == 1) {
                    ST(0x834) = 2;
                }
                /* 0xBC83 */
                if (ST(0x834) >= 2 && ST(0x834) < 666) {
                    ST(0x834) = 666;
                    lieu_action_register("L20D25", P_ARKHAN1, "SG01DIA06", NULL);
                    CB->evenement(1, -1);
                    CB->stop_joindre(-P_GUARDIAN1);
                    lieu_action_register("L20D26A", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    lieu_action_register("L20D26B", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    lieu_action_register("L20D26C", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    lieu_action_register("L20D26D", P_GUARDIAN1, "SP41DIA56", "SP41ARR02");
                    lieu_action_register("L20D26E", P_GUARDIAN1, "SP41DIA56", "SP41ARR02_100");
                    CB->joindre_perm(-P_GUARDIAN1, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(0xBC), ST(ST_DEBUG_PRINT));
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xBDC6 */
                if (ST(0x834) >= 666) {
                    ST(0x834)++;
                    lieu_action_register("L20D27", P_ARKHAN1, "SG01DIA06", NULL);
                    CB->evenement(1, -1);
                    CB->stop_joindre(-P_GUARDIAN1);
                    /* 0xBE17 */
                    if (ST(0x834) == 666) {
                        lieu_action_register("L20D20A", P_GUARDIAN1, "SP41DIA56", "SP41ARR02_10");
                        lieu_action_register("L20D20B", P_GUARDIAN1, "SP41DIA56", "SP41ARR02_10");
                    } else {
                        lieu_action_register("L20D21", P_GUARDIAN1, "SP41DIA56", "SP41ARR02_10");
                    }
                    CB->joindre_perm(-P_GUARDIAN1, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(0xBC), ST(ST_DEBUG_PRINT));
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }
    }
    /* 0xBED2 */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_GUARDIAN1)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_GUARDIAN1);
        /* 0xBF36 */
        if (ST(0x844) == 0) {
            CB->select_pile(1, 1);
            ST(0x844) = 1;
            CB->joindre(P_GUARDIAN1, 0, 0, 0, ST(ST_HERO));
            lieu_action_register("L20D28", P_GUARDIAN1, "SP41DIA59", "SP41ARR02");
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
        /* 0xBFB7 */
        if (ST(0x844) == 1) {
            ST(0x844) = 2;
            CB->select_pile(1, 1);
            CB->pos_perso(P_ZED84, 4307, -3270, 0x9E57, 1, "P84ARR02", 5, 0);
            CB->link_objet(P_ZED84, "OB68", 0x1D, 1);
            CB->animation("SP41GAR26");
            lieu_action_register("L20D29", P_GUARDIAN1, "P41GAR02", "SP41GAR02");
            /* 0xC045 */
            if (ST(ST_PLAYER_WEAPON) == 0) {
                CB->animation("SP01GAR09");
            }
            /* 0xC062 */
            if (ST(ST_PLAYER_WEAPON) == 26) {
                CB->animation("SP01GAR17");
            }
            /* 0xC07F */
            if (ST(ST_PLAYER_WEAPON) == 38) {
                CB->animation("SP01GAR13");
            }
            CB->animation("ar_50");
            CB->modify_var(&ST(V_ENGAGED_PERSONA), 41);
            CB->select_pile(0, 1);
            return;
        }
    }
}

/* trigger 03 — sub_1000C0C6 — Lory: talk and fight */
static void trig03(void) {
    /* 0xC0D5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC150 */
        if (ST(0x870) == 0) {
            ST(0x870) = 1;
            /* 0xC171 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xC183 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L20D30", P_ARKHAN2, "SG02DIA05", NULL);
                }
                /* 0xC1AB */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L20D31", P_ARKHAN2, "SG02DIA05", NULL);
                }
            }
            /* 0xC1D3 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xC1E5 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L20D32", P_ARKHAN3, "SG03DIA05", NULL);
                }
                /* 0xC20D */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L20D33", P_ARKHAN3, "SG03DIA05", NULL);
                }
            }
            CB->dialogue("L20D34", P_LORY);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xC26E */
        if (ST(0x870) == 1) {
            /* 0xC280 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L20D35", P_ARKHAN2, "SG02DIA05", NULL);
            }
            /* 0xC2A8 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L20D36", P_ARKHAN3, "SG03DIA05", NULL);
            }
            CB->dialogue("L20D37", P_LORY);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xC306 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0xB58) = 0;
        ST(0x90) = 0;
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SG26FRA01");
        CB->evenement(35, 750);
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
    /* 0xC444 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_LORY)) != VIE_DEAD && CB->direction_perso(P_LORY)) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        ST(0xB58) = 0;
        ST(0x90) = 0;
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SG26FRA01");
        CB->evenement(35, 750);
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
}

/* trigger 04 — sub_1000C56F — Phedoria: talk and fight */
static void trig04(void) {
    /* 0xC57E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC5F9 */
        if (ST(0x874) == 0) {
            ST(0x874) = 1;
            /* 0xC61A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xC62C */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L20D39", P_ARKHAN2, "SG02DIA05", NULL);
                }
                /* 0xC654 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L20D40", P_ARKHAN2, "SG02DIA05", NULL);
                }
            }
            /* 0xC67C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xC68E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L20D41", P_ARKHAN3, "SG03DIA05", NULL);
                }
                /* 0xC6B6 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L20D42", P_ARKHAN3, "SG03DIA05", NULL);
                }
            }
            CB->dialogue("L20D43", P_PHEDORIA);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xC717 */
        if (ST(0x874) == 1) {
            /* 0xC729 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L20D44", P_ARKHAN2, "SG02DIA05", NULL);
            }
            /* 0xC751 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L20D45", P_ARKHAN3, "SG02DIA05", NULL);
            }
            CB->dialogue("L20D46", P_PHEDORIA);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xC7AF */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(0x874) = 5;
        CB->bloque(BLOQUE_ALL);
        ST(0xB58) = 0;
        ST(0x90) = 0;
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SP26ATA17");
        CB->evenement(36, 500);
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
    /* 0xC8FC */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD && CB->direction_perso(P_PHEDORIA)) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(0x874) = 5;
        CB->bloque(BLOQUE_ALL);
        ST(0xB58) = 0;
        ST(0x90) = 0;
        CB->select_pile(1, 1);
        CB->joindre(-P_LORY, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(2, 1);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP26GAR03");
        CB->dialogue("L20D38", P_PHEDORIA);
        CB->animation("SP26ATA17");
        CB->evenement(36, 500);
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
}

/* trigger 06 — sub_1000CA41 — Guardian 9: fight start, talk, search, strike */
static void trig06(void) {
    /* 0xCA50 */
    if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 4950;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(P_GUARDIAN10);
        CB->stop_joindre(P_GUARDIAN9);
        CB->animation("SP49GAR26");
        lieu_action_register("L20D112", P_GUARDIAN9, "SP49GAR02", "SP49GAR02");
        CB->combat(P_GUARDIAN9, 0);
        CB->animation("SP50GAR26");
        CB->combat(P_GUARDIAN10, 0);
        ST(V_ENGAGED_PERSONA) = 4950;
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xCB2B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_GUARDIAN9);
        /* 0xCBB0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xCBC2 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xCBD4 */
                if (ST(0x880) == 0) {
                    ST(0xE74) = 10;
                    ST(0x880) = 1;
                    CB->evenement(37, -1);
                    ST(0x78) = 0;
                    lieu_action_register("L20D100", P_ARKHAN2, "SG02DIA02", NULL);
                    CB->stop_joindre(P_GUARDIAN9);
                    CB->animation("SP49GAR27");
                    lieu_action_register("L20D101A", P_GUARDIAN9, "P49DIA51", "SP49ARR02");
                    lieu_action_register("L20D101B", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    lieu_action_register("L20D101C", P_GUARDIAN9, "SP49DIA54", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->stop_joindre(P_GUARDIAN10);
                    CB->animation("SP50GAR27");
                    CB->animation("P50ARR03");
                    CB->joindre_perm(-P_GUARDIAN10, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xCD0D */
                if (ST(0x880) == 1) {
                    ST(0x880) = 2;
                    CB->stop_joindre(P_GUARDIAN9);
                    lieu_action_register("L20D102", P_ARKHAN2, "SG02DIA02", NULL);
                    lieu_action_register("L20D103", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xCDA7 */
                if (ST(0x880) == 2) {
                    CB->stop_joindre(P_GUARDIAN9);
                    lieu_action_register("LCOD64", P_ARKHAN2, "SG02DIA02", NULL);
                    lieu_action_register("L20D105", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xCE32 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->joindre(-P_GUARDIAN9, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L20D106", P_ARKHAN2, "SG02DIA02", NULL);
                lieu_action_register("L20D107", P_GUARDIAN9, "SP49GAR26", "SP49GAR02");
                CB->stop_joindre(P_GUARDIAN9);
                CB->combat(P_GUARDIAN9, 0);
                CB->stop_joindre(P_GUARDIAN10);
                CB->combat(P_GUARDIAN10, 0);
                ST(V_ENGAGED_PERSONA) = 4950;
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xCEF5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xCF07 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xCF19 */
                if (ST(0x850) == 0) {
                    ST(0xE74) = 10;
                    ST(0x850) = 1;
                    CB->evenement(37, -1);
                    ST(0x78) = 0;
                    lieu_action_register("L20D108", P_ARKHAN3, "SG03DIA02", NULL);
                    CB->stop_joindre(P_GUARDIAN9);
                    CB->animation("SP49GAR27");
                    lieu_action_register("L20D101A", P_GUARDIAN9, "P49DIA51", "SP49ARR02");
                    lieu_action_register("L20D101B", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    lieu_action_register("L20D101C", P_GUARDIAN9, "SP49DIA54", "SP49ARR02");
                    lieu_action_register("L20D103", P_GUARDIAN9, "SP49DIA55", "SP49ARR02");
                    CB->stop_joindre(P_GUARDIAN10);
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("SP50GAR27");
                    CB->animation("P50ARR03");
                    CB->joindre_perm(-P_GUARDIAN10, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xD06B */
                if (ST(0x850) == 1) {
                    ST(0x850) = 2;
                    CB->joindre(-P_GUARDIAN9, 0, 0, 0, ST(ST_HERO));
                    CB->stop_joindre(P_GUARDIAN9);
                    lieu_action_register("LCOD65", P_ARKHAN3, "SG03DIA02", NULL);
                    lieu_action_register("L20D111", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xD122 */
                if (ST(0x850) == 2) {
                    CB->joindre(-P_GUARDIAN9, 0, 0, 0, ST(ST_HERO));
                    CB->stop_joindre(P_GUARDIAN9);
                    lieu_action_register("LCOD65", P_ARKHAN3, "SG03DIA02", NULL);
                    lieu_action_register("L20D105", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xD1CA */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->joindre(-P_GUARDIAN9, 0, 0, 0, ST(ST_HERO));
                CB->animation("SP49GAR26");
                lieu_action_register("L20D112", P_GUARDIAN9, "SP49GAR02", "SP49GAR02");
                CB->stop_joindre(P_GUARDIAN9);
                CB->combat(P_GUARDIAN9, 0);
                CB->stop_joindre(P_GUARDIAN10);
                CB->combat(P_GUARDIAN10, 0);
                ST(V_ENGAGED_PERSONA) = 4950;
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xD27F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
        /* 0xD2D0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            lieu_action_register("LCOD20", P_ARKHAN2, "SP02ATT51", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xD32D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            lieu_action_register("LCOD21", P_ARKHAN3, "SP03ATT51", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xD387 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN9)) == VIE_DEAD) {
        CB->fouille(P_GUARDIAN9);
        return;
    }
    /* 0xD3E5 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->stop_joindre(P_GUARDIAN9);
        CB->combat(P_GUARDIAN9, 0);
        CB->stop_joindre(P_GUARDIAN10);
        CB->combat(P_GUARDIAN10, 0);
        ST(V_ENGAGED_PERSONA) = 4950;
        return;
    }
}

/* trigger 07 — sub_1000D46D — Guardian 10: fight start, talk, search, strike */
static void trig07(void) {
    /* 0xD47C */
    if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_GUARDIAN10)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 4950;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(P_GUARDIAN10);
        CB->stop_joindre(P_GUARDIAN9);
        CB->animation("SP49GAR26");
        lieu_action_register("L20D112", P_GUARDIAN9, "SP49GAR02", "SP49GAR02");
        CB->combat(P_GUARDIAN9, 0);
        CB->animation("SP50GAR26");
        CB->combat(P_GUARDIAN10, 0);
        ST(V_ENGAGED_PERSONA) = 4950;
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xD557 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN10)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_GUARDIAN10);
        /* 0xD5DC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xD5EE */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xD600 */
                if (ST(0x880) == 0) {
                    ST(0xE74) = 10;
                    ST(0x880) = 1;
                    CB->evenement(37, -1);
                    ST(0x78) = 0;
                    lieu_action_register("L20D113", P_ARKHAN2, "SG02DIA02", NULL);
                    CB->stop_joindre(P_GUARDIAN10);
                    CB->animation("SP50GAR27");
                    lieu_action_register("L20D114", P_GUARDIAN10, "P50DIA53", "SP50ARR03");
                    CB->joindre_perm(-P_GUARDIAN10, ST(ST_HERO));
                    CB->stop_joindre(P_GUARDIAN9);
                    CB->animation("SP49GAR27");
                    CB->animation("P49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xD707 */
                if (ST(0x880) == 1) {
                    ST(0x880) = 2;
                    CB->stop_joindre(P_GUARDIAN10);
                    lieu_action_register("L20D102", P_ARKHAN2, "SG02DIA02", NULL);
                    lieu_action_register("L20D115A", P_GUARDIAN10, "SP50DIA53", "SP50ARR03");
                    lieu_action_register("L20D115B", P_GUARDIAN10, "SP50DIA53", "SP50ARR03");
                    CB->joindre_perm(-P_GUARDIAN10, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xD7BA */
                if (ST(0x880) == 2) {
                    CB->stop_joindre(P_GUARDIAN10);
                    lieu_action_register("L20D116", P_ARKHAN2, "SG02DIA02", NULL);
                    lieu_action_register("L20D117", P_GUARDIAN10, "SP50DIA53", "SP50ARR03");
                    CB->joindre_perm(-P_GUARDIAN10, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xD845 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->joindre(-P_GUARDIAN10, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L20D106", P_ARKHAN2, "SG02DIA02", NULL);
                lieu_action_register("L20D118", P_GUARDIAN10, "SP50GAR26", "SP50GAR03");
                CB->stop_joindre(P_GUARDIAN9);
                CB->combat(P_GUARDIAN9, 0);
                CB->stop_joindre(P_GUARDIAN10);
                CB->combat(P_GUARDIAN10, 0);
                ST(V_ENGAGED_PERSONA) = 4950;
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xD908 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xD91A */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xD92C */
                if (ST(0x850) == 0) {
                    ST(0xE74) = 10;
                    ST(0x850) = 1;
                    CB->evenement(37, -1);
                    ST(0x78) = 0;
                    lieu_action_register("L20D108", P_ARKHAN3, "SG03DIA02", NULL);
                    CB->stop_joindre(P_GUARDIAN10);
                    CB->animation("SP50GAR27");
                    lieu_action_register("L20D109", P_GUARDIAN10, "P50DIA53", "SP50ARR03");
                    CB->stop_joindre(P_GUARDIAN9);
                    CB->animation("SP49GAR27");
                    CB->animation("P49ARR02");
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xDA07 */
                if (ST(0x850) == 1) {
                    CB->joindre(-P_GUARDIAN10, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("LCOD65", P_ARKHAN3, "SG03DIA02", NULL);
                    lieu_action_register("L20D119", P_GUARDIAN10, "SP50DIA53", "SP50ARR03");
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xDA8E */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->joindre(-P_GUARDIAN10, 0, 0, 0, ST(ST_HERO));
                CB->animation("SP49GAR26");
                lieu_action_register("L20D112", P_GUARDIAN10, "SP50GAR26", "SP50GAR03");
                CB->stop_joindre(P_GUARDIAN9);
                CB->combat(P_GUARDIAN9, 0);
                CB->stop_joindre(P_GUARDIAN10);
                CB->combat(P_GUARDIAN10, 0);
                ST(V_ENGAGED_PERSONA) = 4950;
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xDB43 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN10)) != VIE_DEAD) {
        /* 0xDB94 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            lieu_action_register("LCOD20", P_ARKHAN2, "SP02ATT51", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xDBF1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            lieu_action_register("LCOD21", P_ARKHAN3, "SP03ATT51", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xDC4B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN10)) == VIE_DEAD) {
        CB->fouille(P_GUARDIAN10);
        return;
    }
    /* 0xDCA9 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_GUARDIAN10)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->stop_joindre(P_GUARDIAN9);
        CB->combat(P_GUARDIAN9, 0);
        CB->stop_joindre(P_GUARDIAN10);
        CB->combat(P_GUARDIAN10, 0);
        ST(V_ENGAGED_PERSONA) = 4950;
        return;
    }
}

/* trigger 10 — sub_1000DD47 — zone 10 exit: night cinematic, music, next room */
static void trig10(void) {
    /* 0xDD59 */
    if (ST(0xE7C) < 3) {
        ST(0xE7C) = 3;
    }
    /* 0xDD7A */
    if (ST(0x88) == 0) {
        /* 0xDD8C */
        if (ST(0xB10) == 1 && ST(0xF38) == 0) {
            CB->divers_empile(DV_SCREEN, 5, 0);
            CB->cinematique("SC10_500");
            CB->divers(DV_NIGHT, 0, 0);
        }
        /* 0xDDDC */
        if (ST(0xB10) == 0 && ST(0xB58) != 0) {
            ST(0x88) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0xDE1A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L20D122A", P_ARKHAN2);
                CB->dialogue("L20D122B", P_ARKHAN2);
            }
            /* 0xDE4C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L20D123", P_ARKHAN3);
            }
            CB->joindre(ST(ST_HERO), 4623, -3620, 0xA010, -1);
            CB->animation("Sar_50");
            CB->modify_var(&ST(0x88), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xDEBC */
    if (ST(0x88) == 0) {
        /* 0xDECE */
        if (ST(0xB3C) == 0) {
            CB->divers(DV_MUSIC, 1, 4);
        }
        /* 0xDEF2 */
        if (ST(0xB3C) == 1) {
            CB->divers(DV_MUSIC, 2, 4);
        }
        /* 0xDF16 */
        if ((ST(0xE74) == 1 || ST(0xE74) == 10) && ST(V_ENGAGED_PERSONA) != 0) {
            ST(0x858) = 4950;
        }
        /* 0xDF58 */
        if ((ST(0xE74) == 111 || ST(0xE74) == 112) && ST(V_ENGAGED_PERSONA) != 0) {
            ST(0x858) = 49;
        }
        /* 0xDF9A */
        if ((ST(0xE74) == 2 || ST(0xE74) == 20) && ST(V_ENGAGED_PERSONA) != 0) {
            ST(0x858) = 5152;
        }
        ST(ST_LEVEL) = 20;
        /* 0xDFE8 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->change_piece(10, 0);
        }
        /* 0xE007 */
        if (ST(ST_HERO) > 1 && ST(0xB4C) == 0) {
            CB->change_piece(10, 1);
        }
        /* 0xE03B */
        if (ST(ST_HERO) > 1 && ST(0xB4C) == 1) {
            CB->change_piece(10, 2);
        }
        /* 0xE06F */
        if (ST(ST_HERO) > 1 && ST(0xB4C) == 2) {
            CB->change_piece(10, 3);
        }
        /* 0xE0A3 */
        if (ST(ST_HERO) > 1 && ST(0xB4C) == 3) {
            CB->change_piece(51, 0);
        }
    }
}

/* trigger 11 — sub_1000E0CD — zone 11 exit: music, room 21 variants */
static void trig11(void) {
    /* 0xE0DF */
    if (ST(0xB3C) == 0) {
        CB->divers(DV_MUSIC, 1, 4);
    }
    /* 0xE103 */
    if (ST(0xB3C) == 1) {
        CB->divers(DV_MUSIC, 2, 4);
    }
    ST(ST_LEVEL) = 20;
    /* 0xE133 */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        /* 0xE145 */
        if (ST(0x32C) == 0 && ST(0x330) == 0) {
            /* 0xE169 */
            if (ST(0xB3C) == 0) {
                CB->change_piece(21, 0);
            }
            /* 0xE188 */
            if (ST(0xB3C) == 1) {
                CB->change_piece(21, 1);
            }
            /* 0xE1AA */
            if (ST(0xB3C) == 2) {
                CB->change_piece(21, 2);
            }
        } else {
            CB->change_piece(21, 2);
        }
    }
    /* 0xE1E1 */
    if (ST(V_GUARD_FIGHT_DONE) == 1) {
        CB->change_piece(21, 2);
    }
    /* 0xE203 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        /* 0xE215 */
        if (ST(0xB58) != 0 && ST(0xB58) != 10) {
            ST(0xB58) = 10;
        }
        /* 0xE248 */
        if ((ST(0xE74) == 1 || ST(0xE74) == 10) && ST(V_ENGAGED_PERSONA) != 0) {
            ST(0x858) = 4950;
        }
        /* 0xE28A */
        if ((ST(0xE74) == 111 || ST(0xE74) == 112) && ST(V_ENGAGED_PERSONA) != 0) {
            ST(0x858) = 49;
        }
        /* 0xE2CC */
        if ((ST(0xE74) == 2 || ST(0xE74) == 20) && ST(V_ENGAGED_PERSONA) != 0) {
            ST(0x858) = 5152;
        }
        /* 0xE30E */
        if (ST(0xB3C) == 1) {
            CB->change_piece(21, 1);
        }
        /* 0xE330 */
        if (ST(0xB3C) == 2) {
            CB->change_piece(21, 2);
        }
    }
}

/* trigger 12 — sub_1000E34B — zone 12 (guard-post door): weapon check, room change */
static void trig12(void) {
    /* 0xE35A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && (ST(V_EVENT_17) == 0 || ST(0xAF8) == 1) && CB->direction_spec(0xC350)) {
        CB->bloque(BLOQUE_ALL);
        /* 0xE3DB */
        if (ST(ST_HERO) == P_ARKHAN1) {
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xE3F9 */
            if (ST(ST_PLAYER_WEAPON) == 38) {
                CB->select_pile(2, 2);
                CB->bloque(BLOQUE_ALL);
                CB->divers(DV_COMBAT_STANCE, 0, 0);
                CB->stop_joindre(-P_GUARDIAN1);
                ST(0x83C) = 1;
                ST(V_EVENT_17) = 1;
                ST(V_DLG_RANDOM) = CB->random(2);
                /* 0xE475 */
                if (ST(V_DLG_RANDOM) == 0) {
                    lieu_action_register("L20D07A", P_GUARDIAN1, "SP41ARR02", "SP41ARR02");
                } else {
                    lieu_action_register("L20D07B", P_GUARDIAN1, "SP41ARR02", "SP41ARR02");
                }
                ST(0xBC) = ST(ST_DEBUG_PRINT);
                CB->evenement(1, 5000);
                CB->joindre(-ST(ST_HERO), 5536, -4406, 0x7D7A, -1);
                CB->animation("Sar_50");
                CB->select_pile(0, 1);
            } else {
                ST(V_WEAPON_WARNED) = 1;
                ST(V_DLG_RANDOM) = CB->random(2);
                CB->animation("SP01OUV01");
                lieu_action_register("L20D11A", P_GUARDIAN1, "SP41DIA56", "SP41DIA56");
                lieu_action_register("L20D11B", P_GUARDIAN1, "SP41DIA54", "SP41ARR02");
                /* 0xE58B */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L20D48", P_ARKHAN1);
                }
                /* 0xE5AD */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L20D50", P_ARKHAN1);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("SP41ARR02_25");
                CB->modify_var(&ST(0xB8), 0);
                CB->modify_var(&ST(V_WEAPON_WARNED), 0);
                CB->modify_var(&ST(0x848), 0);
                CB->joindre_perm(P_GUARDIAN1, ST(ST_HERO));
                CB->bloque(BLOQUE_NONE);
                CB->select_pile(0, 1);
                return;
            }
        }
        /* 0xE662 */
        if (ST(ST_HERO) > 1) {
            /* 0xE674 */
            if (ST(0xB3C) == 0) {
                CB->divers(DV_MUSIC, 1, 4);
            }
            /* 0xE698 */
            if (ST(0xB3C) == 1) {
                CB->divers(DV_MUSIC, 2, 4);
            }
            /* 0xE6BC */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->evenement(34, 400);
            }
            /* 0xE6DE */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->evenement(34, 700);
            }
            /* 0xE700 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02OUV01");
            }
            /* 0xE71D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03OUV01");
            }
            ST(ST_LEVEL) = 20;
            return;
        }
    }
}

/* trigger 13 — sub_1000E744 — zone 13 (OB03 alcove): Arkhan comments, item puzzle */
static void trig13(void) {
    /* 0xE753 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xC000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE7BC */
        if (ST(0x32C) == 0 && ST(0x330) == 0) {
            /* 0xE7E0 */
            if (ST(ST_PLAYER_WEAPON) != 3) {
                /* 0xE7F2 */
                if (ST(0xB28) == 0) {
                    /* 0xE804 */
                    if (ST(0x840) == 0) {
                        /* 0xE816 */
                        if (ST(ST_HERO) == P_ARKHAN1) {
                            CB->animation("SG01PRE01");
                            CB->dialogue("L20D51", P_ARKHAN1);
                        }
                        /* 0xE843 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SG02PRE01");
                            CB->dialogue("L20D52", P_ARKHAN2);
                        }
                        /* 0xE870 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SG03PRE01");
                            CB->dialogue("L20D53", P_ARKHAN3);
                        }
                        ST(0x840) = 1;
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xE8D5 */
                    if (ST(0x840) == 1) {
                        /* 0xE8E7 */
                        if (ST(ST_HERO) == P_ARKHAN1) {
                            CB->animation("SG01PRE01");
                            CB->dialogue("L20D57", P_ARKHAN1);
                        }
                        /* 0xE914 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SG02PRE01");
                            CB->dialogue("L20D58", P_ARKHAN2);
                        }
                        /* 0xE941 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SG03PRE01");
                            CB->dialogue("L20D59", P_ARKHAN3);
                        }
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xE997 */
                if (ST(0xB28) == 1) {
                    /* 0xE9A9 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        /* 0xE9BB */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SG02PRE01");
                            CB->dialogue("L20D63", P_ARKHAN2);
                        }
                        /* 0xE9E8 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SG03PRE01");
                            CB->dialogue("L20D64", P_ARKHAN3);
                        }
                    }
                    /* 0xEA15 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        /* 0xEA27 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            /* 0xEA41 */
                            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                                CB->animation("SP02ACT97");
                            } else {
                                CB->animation("SP02ACT93");
                            }
                            CB->dialogue("L20D65", P_ARKHAN2);
                        }
                        /* 0xEA7E */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            /* 0xEA98 */
                            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                                CB->animation("SP03ACT97");
                            } else {
                                CB->animation("SP03ACT93");
                            }
                            CB->dialogue("L20D66", P_ARKHAN3);
                        }
                    }
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xEAFE */
                if (ST(0xB28) == 2) {
                    /* 0xEB10 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        /* 0xEB22 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SG02PRE01");
                            CB->dialogue("L20D67", P_ARKHAN2);
                        }
                        /* 0xEB4F */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SG03PRE01");
                            CB->dialogue("L20D68", P_ARKHAN3);
                        }
                    }
                    /* 0xEB7C */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        /* 0xEB8E */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            /* 0xEBA8 */
                            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                                CB->animation("SP02ACT97");
                            } else {
                                CB->animation("SP02ACT93");
                            }
                            CB->dialogue("L20D65", P_ARKHAN2);
                        }
                        /* 0xEBE5 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            /* 0xEBFF */
                            if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                                CB->animation("SP03ACT97");
                            } else {
                                CB->animation("SP03ACT93");
                            }
                            CB->dialogue("L20D66", P_ARKHAN3);
                        }
                    }
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xEC65 */
            if (ST(ST_PLAYER_WEAPON) == 3) {
                /* 0xEC77 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT18");
                    CB->dialogue("L20D71", P_ARKHAN2);
                }
                /* 0xECA4 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT18");
                }
                CB->etat_objet("OB03", -1, -1, -1, -1, -1, -1, -2, 0);
                CB->bruitage("L20B01", 3928, -4466);
                CB->etat_objet("OB03", 3970, -330, -4480, 0x6600, 0xE800, 0x4800, 0, 6);
                ST(0xB28) = 2;
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        /* 0xED5E */
        if (ST(0xB28) == 0) {
            /* 0xED70 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02PRE01");
                CB->dialogue("L20D87", P_ARKHAN2);
            }
            /* 0xED9D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03PRE01");
                CB->dialogue("L20D88", P_ARKHAN3);
            }
        }
        /* 0xEDCA */
        if (ST(0xB28) == 1) {
            /* 0xEDDC */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02PRE01");
                CB->dialogue("L20D89", P_ARKHAN2);
            }
            /* 0xEE09 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03PRE01");
                CB->dialogue("L20D90", P_ARKHAN3);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 — sub_1000EE58 — zone 14: comments, SC35 cinematic */
static void trig14(void) {
    /* 0xEE67 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(14)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xEEB2 */
        if (ST(0x32C) == 0 && ST(0x330) == 0) {
            CB->animation("Sar_50");
            CB->joindre(-ST(ST_HERO), 4351, -5102, 0xCAF2, -1);
            CB->animation("Sar_50");
            /* 0xEF12 */
            if (ST(0xB28) == 0) {
                /* 0xEF24 */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SP01ATT25");
                    CB->animation("ar_50");
                    CB->dialogue("LCOD38", P_ARKHAN1);
                }
                /* 0xEF5C */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xEF76 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->animation("SP02ATT26");
                    } else {
                        CB->animation("SP02ATT25");
                    }
                    CB->animation("ar_50");
                    CB->dialogue("LCOD39", P_ARKHAN2);
                }
                /* 0xEFBE */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xEFD8 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->animation("SP03ATT26");
                    } else {
                        CB->animation("SP03ATT25");
                    }
                    CB->animation("ar_50");
                    CB->dialogue("LCOD40", P_ARKHAN3);
                }
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xF049 */
            if (ST(0xB28) == 1) {
                /* 0xF05B */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xF075 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->animation("SP02ATT26");
                    } else {
                        CB->animation("SP02ATT25");
                    }
                    CB->animation("ar_50");
                    CB->dialogue("L20D81A", P_ARKHAN2);
                    CB->dialogue("L20D81B", P_ARKHAN2);
                }
                /* 0xF0CD */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xF0E7 */
                    if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        CB->animation("SP03ATT26");
                    } else {
                        CB->animation("SP03ATT25");
                    }
                    CB->animation("ar_50");
                    CB->dialogue("L20D82", P_ARKHAN3);
                }
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xF158 */
            if (ST(0xB28) == 2) {
                /* 0xF16A */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xF17C */
                    if (ST(0xB3C) == 0) {
                        CB->divers_empile(DV_MUSIC, 2, 0);
                        /* 0xF1A8 */
                        if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                            CB->animation("SP02ATT26");
                            lieu_action_register("L20D85", P_ARKHAN2, "ar_50", NULL);
                        } else {
                            CB->animation("SP02ATT25");
                            lieu_action_register("L20D85", P_ARKHAN2, "ar_50", NULL);
                        }
                        CB->animation("ar_50");
                        CB->divers_empile(DV_SCREEN, 5, 0);
                        CB->pos_perso(ST(ST_HERO), 4501, -5232, 0xF7B1, 1, "ar", 1, 0);
                        CB->cinematique("SC35_500");
                        CB->divers_empile(DV_SCREEN, 3, 0);
                        ST(0xB3C) = 1;
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->divers_empile(DV_MUSIC, 2, 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("L20D93", P_ARKHAN2, "SG02DIA02", NULL);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->divers_empile(DV_MUSIC, 2, 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xF306 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xF318 */
                    if (ST(0xB3C) == 0) {
                        CB->divers_empile(DV_MUSIC, 2, 0);
                        /* 0xF344 */
                        if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                            CB->animation("SP03ATT26");
                            lieu_action_register("L20D86", P_ARKHAN3, "ar_50", NULL);
                        } else {
                            CB->animation("SP03ATT25");
                            lieu_action_register("L20D86", P_ARKHAN3, "ar_50", NULL);
                        }
                        CB->animation("ar_50");
                        CB->divers_empile(DV_SCREEN, 5, 0);
                        CB->pos_perso(ST(ST_HERO), 4501, -5232, 0xF7B1, 1, "ar", 1, 0);
                        CB->cinematique("SC35_500");
                        CB->divers_empile(DV_SCREEN, 3, 0);
                        ST(0xB3C) = 1;
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->divers_empile(DV_MUSIC, 2, 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    } else {
                        lieu_action_register("L20D94", P_ARKHAN3, "SG03DIA02", NULL);
                        CB->animation("Sar_50");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
            return;
        }
        /* 0xF495 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L20D91", P_ARKHAN2, "SG02DIA01", NULL);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xF4E6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L20D92", P_ARKHAN3, "SG03DIA01", NULL);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 19 — sub_1000F55C — zone 19 (door opened by event 33): enter room 27 */
static void trig19(void) {
    ST(ST_LEVEL) = 20;
    CB->change_piece(27, 0);
}

/* trigger 26 — sub_1000F5C2 — Guardian 9: fight start, talk, search, strike */
static void trig26(void) {
    /* 0xF5D1 */
    if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = P_GUARDIAN9;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(P_GUARDIAN9);
        CB->animation("SP49GAR26");
        lieu_action_register("L20D112", P_GUARDIAN9, "SP49GAR02", "SP49GAR02");
        CB->combat(P_GUARDIAN9, 0);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xF67D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_GUARDIAN9);
        /* 0xF702 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xF714 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xF726 */
                if (ST(0x880) == 0) {
                    ST(0xE74) = 112;
                    ST(0x880) = 1;
                    CB->evenement(37, -1);
                    ST(0x78) = 0;
                    lieu_action_register("L20D100", P_ARKHAN2, "SG02DIA02", NULL);
                    CB->stop_joindre(P_GUARDIAN9);
                    CB->animation("SP49GAR27");
                    lieu_action_register("L20D101A", P_GUARDIAN9, "P49DIA51", "SP49ARR02");
                    lieu_action_register("L20D101B", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    lieu_action_register("L20D101C", P_GUARDIAN9, "SP49DIA54", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xF828 */
                if (ST(0x880) == 1) {
                    ST(0x880) = 2;
                    CB->stop_joindre(P_GUARDIAN9);
                    lieu_action_register("L20D102", P_ARKHAN2, "SG02DIA02", NULL);
                    lieu_action_register("L20D103", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xF8C2 */
                if (ST(0x880) == 2) {
                    CB->stop_joindre(P_GUARDIAN9);
                    lieu_action_register("LCOD64", P_ARKHAN2, "SG02DIA02", NULL);
                    lieu_action_register("L20D105", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xF94D */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->joindre(-P_GUARDIAN9, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L08D251B", P_ARKHAN2, "SG02DIA02", NULL);
                lieu_action_register("L20D107", P_GUARDIAN9, "SP49GAR26", "SP49GAR02");
                CB->stop_joindre(P_GUARDIAN9);
                CB->combat(P_GUARDIAN9, 0);
                ST(V_ENGAGED_PERSONA) = P_GUARDIAN9;
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xF9F8 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xFA0A */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xFA1C */
                if (ST(0x850) == 0) {
                    ST(0xE74) = 112;
                    ST(0x850) = 1;
                    CB->evenement(37, -1);
                    ST(0x78) = 0;
                    lieu_action_register("L20D108", P_ARKHAN3, "SG03DIA02", NULL);
                    CB->stop_joindre(P_GUARDIAN9);
                    CB->animation("SP49GAR27");
                    lieu_action_register("L20D101A", P_GUARDIAN9, "P49DIA51", "SP49ARR02");
                    lieu_action_register("L20D101B", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    lieu_action_register("L20D101C", P_GUARDIAN9, "SP49DIA54", "SP49ARR02");
                    lieu_action_register("L20D103", P_GUARDIAN9, "SP49DIA55", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xFB37 */
                if (ST(0x850) == 1) {
                    ST(0x850) = 2;
                    CB->joindre(-P_GUARDIAN9, 0, 0, 0, ST(ST_HERO));
                    CB->stop_joindre(P_GUARDIAN9);
                    lieu_action_register("LCOD65", P_ARKHAN3, "SG03DIA02", NULL);
                    lieu_action_register("L20D111", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xFBEE */
                if (ST(0x850) == 2) {
                    CB->joindre(-P_GUARDIAN9, 0, 0, 0, ST(ST_HERO));
                    CB->stop_joindre(P_GUARDIAN9);
                    lieu_action_register("LCOD65", P_ARKHAN3, "SG03DIA02", NULL);
                    lieu_action_register("L20D105", P_GUARDIAN9, "SP49DIA53", "SP49ARR02");
                    CB->joindre_perm(-P_GUARDIAN9, ST(ST_HERO));
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xFC96 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->joindre(-P_GUARDIAN9, 0, 0, 0, ST(ST_HERO));
                CB->animation("SP49GAR26");
                lieu_action_register("L20D112", P_GUARDIAN9, "SP49GAR02", "SP49GAR02");
                CB->stop_joindre(P_GUARDIAN9);
                CB->combat(P_GUARDIAN9, 0);
                ST(V_ENGAGED_PERSONA) = P_GUARDIAN9;
                CB->animation("Sar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xFD33 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
        /* 0xFD84 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            lieu_action_register("LCOD20", P_ARKHAN2, "SP02ATT51", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xFDE1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            lieu_action_register("LCOD21", P_ARKHAN3, "SP03ATT51", NULL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xFE3B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN9)) == VIE_DEAD) {
        CB->fouille(P_GUARDIAN9);
        return;
    }
    /* 0xFE99 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_GUARDIAN9)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->stop_joindre(P_GUARDIAN9);
        CB->combat(P_GUARDIAN9, 0);
        ST(V_ENGAGED_PERSONA) = P_GUARDIAN9;
        return;
    }
}

/* trigger 27 — sub_1000FF09 — zone 27: Arkhan examines the spot (direction 0xC000) */
static void trig27(void) {
    /* 0xFF18 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xC000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xFF72 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xFF84 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xFF9E */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT11");
                } else {
                    CB->animation("SP02ACT12");
                }
            }
            /* 0xFFCB */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xFFE5 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT86");
                } else {
                    CB->animation("SP02ACT85");
                }
            }
        }
        /* 0x10012 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x10024 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x1003E */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT11");
                } else {
                    CB->animation("SP03ACT12");
                }
            }
            /* 0x1006B */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x10085 */
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

/* trigger 35 — sub_10010121 — zone 35: Arkhan dialogues */
static void trig35(void) {
    /* 0x130 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(35)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x199 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("L20D124A", P_ARKHAN1);
            CB->dialogue("L20D124B", P_ARKHAN1);
            /* 0x1CB */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L20D125", P_ARKHAN1);
            }
            /* 0x1ED */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L20D126", P_ARKHAN1);
            }
        }
        /* 0x20F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L20D127A", P_ARKHAN2);
            CB->dialogue("L20D127B", P_ARKHAN2);
            /* 0x241 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L20D128", P_ARKHAN2);
            }
            /* 0x263 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L20D129", P_ARKHAN2);
            }
        }
        /* 0x285 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L20D130A", P_ARKHAN3);
            CB->dialogue("L20D130B", P_ARKHAN3);
            /* 0x2B7 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L20D131", P_ARKHAN3);
            }
            /* 0x2D9 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L20D132", P_ARKHAN3);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 36 — sub_1001031D — zones 36..39: shared handler (called by triggers 37..39) */
static void trig36(void) {
    /* 0x32C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && (CB->direction_zone(36) || (CB->divers(DV_IN_ZONE, ST(ST_HERO), 37) && CB->direction_zone(37)) || (CB->divers(DV_IN_ZONE, ST(ST_HERO), 38) && CB->direction_zone(38)) || (CB->divers(DV_IN_ZONE, ST(ST_HERO), 39) && CB->direction_zone(39)))) {
        /* 0x408 */
        if (ST(0xE0) == 0 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 36)) {
            ST(0xF0) = 36;
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x460 */
        if (ST(0xE0) == 0 || (CB->divers(DV_IN_ZONE, ST(ST_HERO), 36) && ST(0xF0) == 36) || (CB->divers(DV_IN_ZONE, ST(ST_HERO), 37) && ST(0xF0) == 37) || (CB->divers(DV_IN_ZONE, ST(ST_HERO), 38) && ST(0xF0) == 38) || (CB->divers(DV_IN_ZONE, ST(ST_HERO), 39) && ST(0xF0) == 39)) {
            ST(0xE0) = 1;
            /* 0x549 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                lieu_action_register("L20D133", P_ARKHAN1, "SG01DIA02", NULL);
            }
            /* 0x571 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L20D134A", P_ARKHAN2, "SG02DIA02", NULL);
                lieu_action_register("L20D134B", P_ARKHAN2, "SG02DIA03", NULL);
            }
            /* 0x5AF */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L20D135", P_ARKHAN3, "SG03DIA02", NULL);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x600 */
        if (ST(0xE0) == 1) {
            /* 0x612 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("L20D136", P_ARKHAN1);
            }
            /* 0x634 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L20D137", P_ARKHAN2);
            }
            /* 0x656 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L20D138", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 37 — sub_1001069A — zone 37 entry, calls trig36 */
static void trig37(void) {
    /* 0x6A9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(37)) {
        /* 0x6EC */
        if (ST(0xE0) == 0) {
            ST(0xF0) = 37;
        }
        trig36();
    }
}

/* trigger 38 — sub_1001070B — zone 38 entry, calls trig36 */
static void trig38(void) {
    /* 0x71A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(38)) {
        /* 0x75D */
        if (ST(0xE0) == 0) {
            ST(0xF0) = 38;
        }
        trig36();
    }
}

/* trigger 39 — sub_1001077C — zone 39 entry, calls trig36 */
static void trig39(void) {
    /* 0x78B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(39)) {
        /* 0x7CE */
        if (ST(0xE0) == 0) {
            ST(0xF0) = 39;
        }
        trig36();
    }
}

/* trigger 41 — sub_100085DD — return to the room: music, actor walks, fight resume */
static void trig41(void) {
    /* 0x85EF */
    if (ST(0xB3C) == 0) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x8613 */
    if (ST(0xB3C) == 1) {
        CB->divers(DV_MUSIC, 2, 1);
    }
    /* 0x8637 */
    if (ST(0xAF8) == 0 && ST(V_GUARD_FIGHT_DONE) == 0 && ST(0xE7C) == 2) {
        CB->select_pile(2, 1);
        ST(0x64) = 1;
        CB->joindre(P_LORY, 5625, -4509, -1, -1);
        CB->select_pile(0, 1);
    }
    /* 0x86AF */
    if (ST(V_GUARD_FIGHT_DONE) == 1 && ST(0xF64) == 0) {
        ST(0xE8) = ST(ST_HERO);
        ST(0xF64) = 1;
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        CB->animation("SP25ARR01_500");
        CB->divers_empile(DV_P25_SOUND, 0x1206, -5748);
        CB->animation("SP25ARR01_500");
        CB->evenement(39, 1250);
        CB->animation("P25ATA02");
        CB->select_pile(0, 1);
    }
    /* 0x8767 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB58) == 1) {
        ST(0xB58) = 2;
        CB->select_pile(2, 1);
        CB->joindre(P_PHEDORIA, 5594, -4523, -1, -1);
        CB->select_pile(1, 1);
        CB->joindre(P_LORY, 5550, -4507, -1, -1);
        CB->select_pile(0, 1);
    }
    CB->divers_empile(DV_MUSIC, 1, 1);
    /* 0x880E */
    if (ST(0x858) == 4950) {
        ST(V_ENGAGED_PERSONA) = ST(0x858);
        CB->combat(P_GUARDIAN9, 0);
        CB->combat(P_GUARDIAN10, 0);
    }
    /* 0x884E */
    if (ST(0x858) == 49) {
        ST(V_ENGAGED_PERSONA) = ST(0x858);
        CB->combat(P_GUARDIAN9, 0);
    }
    /* 0x8884 */
    if (ST(0x858) == 5152) {
        ST(V_ENGAGED_PERSONA) = ST(0x858);
        CB->combat(P_GUARDIAN9, 0);
        CB->combat(P_GUARDIAN10, 0);
    }
}
