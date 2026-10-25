/*
 * lieu04.c — Native hand-port of LIEU04.DLL (level-4 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu04_trig.c (reached through
 * l04_trigger, see lieu04_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Same compiler/layout as LIEU01..03; the
 * entry is at 0x100114E0, the state pointer global is dword_10022A8C and the
 * trigger table is dword_100229A0[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_10011BAC), lieu_pre_scene (sub_10011D5C) and
 * sub_10001200 (float reset) are the shared lieu_common.c; the entry and
 * INIT are instruction-for-instruction identical to LIEU03. tick_body
 * (sub_10008392) is level-4 specific.
 */

#include <stdlib.h>

#include "lieu04_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10008392 (thunk 0x100010B9), level-4 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x83A4 */
    if (ST(ST_LOCKED_CAMERA) == 0) {
        /* 0x83B6 */
        if (ST(0x114) != 0) {
            ST(0x114) = 0;
        }
        /* 0x83D7 */
        if (ST(V_WEAPON_WARNED) != 0) {
            ST(V_WEAPON_WARNED) = 0;
        }
    }
    /* 0x83F8 */
    if (ST(ST_LOCKED_CAMERA) == 1) {
        /* 0x840A */
        if (ST(0xEA0) == 0 && ST(0x114) == 0) {
            CB->select_pile(2, 2);
            ST(0x114) = 1;
            CB->play_anim(1, 0);
            CB->select_pile(0, 1);
        }
        /* 0x8467 */
        if (ST(0xEA0) == 1 && ST(0x114) == 0) {
            CB->select_pile(2, 2);
            ST(0x114) = 1;
            ST(V_WEAPON_WARNED) = 0;
            CB->play_anim(0, 0);
            CB->select_pile(0, 1);
        }
    }
    /* 0x84D0 */
    if (ST(ST_LOCKED_CAMERA) == 2 && ST(0xEA0) == 1 && ST(V_WEAPON_WARNED) == 0) {
        CB->select_pile(2, 2);
        ST(0x114) = 0;
        ST(V_WEAPON_WARNED) = 1;
        CB->play_anim(3, 0);
        CB->select_pile(0, 1);
    }
    /* 0x854E */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        CB->evenement(0x2C, 3000);
    }
    /* 0x8591 */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        /* 0x85C2 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x85EB */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x8617 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x8643 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x8667 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x8684 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x86B0 */
    if (ST(ST_VIE(P_GUARDIAN2)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_GUARDIAN2) {
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = 0;
        CB->select_pile(1, 2);
        CB->dialogue("L04D200", -1);
        CB->dialogue("L04D201", -1);
        CB->select_pile(0, 1);
    }
    /* 0x8726 */
    if (ST(ST_VIE(P_GUARDIAN25)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_GUARDIAN25) {
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = 0;
        CB->select_pile(2, 2);
        CB->dialogue("L04D202", -1);
        CB->dialogue("L04D203", -1);
        CB->select_pile(0, 2);
    }
    /* 0x879C */
    if ((ST(ST_VIE(P_KALHI13)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_KALHI13) || (ST(ST_VIE(P_KALHI33)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_KALHI33)) {
        ST(0xE58) = 1;
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = 0;
    }
    /* 0x8805 */
    if (ST(ST_VIE(P_KONKALITE6)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_KONKALITE6) {
        ST(0xF4) = 1;
        CB->ajoute_zone(10, -2215, -2410, 120, 130, 0);
        ST(0xE30) = 3;
        ST(V_ENGAGED_PERSONA) = 0;
        ST(ST_SCRIPT_BUSY) = 0;
    }
    /* 0x887A */
    if (ST(ST_HERO) != 4 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_PLAYER_STRIKE) == 1 && CB->divers(DV_CAN_FIGHT, 9, 2) && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) > -200) {
        /* 0x88E7 */
        if (ST(0xB40) == 1 && ST(ST_VIE(P_GUARDIAN2)) != VIE_DEAD) {
            ST(0xBC) = 1;
        }
        /* 0x891A */
        if (ST(0xB40) == 4 && ST(ST_VIE(P_GUARDIAN25)) != VIE_DEAD) {
            ST(0xBC) = 1;
        }
    }
    /* 0x894A */
    if (ST(0x64) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_PLAYER_STRIKE) == 1 && CB->divers(DV_CAN_FIGHT, 9, 2) && ST(ST_LOCKED_CAMERA) == 1 && CB->direction_spec(0xAFC8) && ST(ST_HERO) != 4 && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) < -200) {
        CB->select_pile(1, 2);
        ST(0x64) = 1;
        CB->evenement(0x23, -1);
        CB->bruitage("L04D10", -1, -1);
        CB->dialogue("L04D212", -1);
        CB->select_pile(0, 1);
    }
    /* 0x8A51 */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN2, 0) && ST(ST_VIE(P_GUARDIAN2)) != VIE_DEAD) {
        CB->divers(DV_WAS_HIT, P_GUARDIAN2, 1);
        CB->select_pile(1, 2);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        /* 0x8AA6 */
        if (ST(0x98) > 1) {
            CB->combat(P_GUARDIAN2, -1);
        }
        /* 0x8ADC */
        if ((CB->divers(DV_HP, P_GUARDIAN2, -1) - ST(V_HIT_DAMAGE)) > 0) {
            CB->divers(DV_HP, P_GUARDIAN2, (CB->divers(DV_HP, P_GUARDIAN2, -1) - ST(V_HIT_DAMAGE)));
        } else {
            CB->divers(DV_HP, P_GUARDIAN2, 1);
        }
        /* 0x8B3D */
        if (CB->divers(DV_HP, P_GUARDIAN2, -1) <= 1) {
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_GUARDIAN2);
            CB->animation("SP42IMP53");
            CB->animation("P42CAD");
            CB->bloque(BLOQUE_NONE);
            CB->pos_perso(P_GUARDIAN2, 0, 0, 0, 2, "P42CAD", 0xD, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
        } else {
            /* 0x8BB3 */
            if (ST(V_ENGAGED_PERSONA) == 0) {
                ST(V_ENGAGED_PERSONA) = P_GUARDIAN2;
                ST(0xBC) = 1;
            }
            CB->stop_joindre(-P_GUARDIAN2);
            CB->animation("SP42IMP13");
            CB->animation("P42IMP48");
            CB->animation("P42GAR03");
            /* 0x8C0C */
            if (ST(0x98) > 1) {
                CB->combat(P_GUARDIAN2, 0);
            }
        }
        CB->select_pile(0, 1);
    }
    /* 0x8C40 */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN25, 0) && ST(ST_VIE(P_GUARDIAN25)) != VIE_DEAD) {
        CB->select_pile(1, 2);
        CB->divers_empile(DV_WAS_HIT, P_GUARDIAN25, 1);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        /* 0x8C95 */
        if (ST(0x98) > 1) {
            CB->combat(P_GUARDIAN25, -1);
        }
        /* 0x8CCB */
        if ((CB->divers(DV_HP, P_GUARDIAN25, -1) - ST(V_HIT_DAMAGE)) > 0) {
            CB->divers_empile(DV_HP, P_GUARDIAN25, (CB->divers(DV_HP, P_GUARDIAN25, -1) - ST(V_HIT_DAMAGE)));
        } else {
            CB->divers_empile(DV_HP, P_GUARDIAN25, 1);
        }
        /* 0x8D2C */
        if (CB->divers(DV_HP, P_GUARDIAN25, -1) <= 1) {
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_GUARDIAN25);
            CB->animation("SP93IMP52");
            CB->animation("P93CAD");
            CB->bloque(BLOQUE_NONE);
            CB->pos_perso(P_GUARDIAN25, 0, 0, 0, 2, "P93CAD", 0xB, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
        } else {
            /* 0x8DA2 */
            if (ST(V_ENGAGED_PERSONA) == 0) {
                ST(V_ENGAGED_PERSONA) = P_GUARDIAN25;
                ST(0xBC) = 1;
            }
            CB->stop_joindre(-P_GUARDIAN25);
            CB->animation("SP93IMP12");
            CB->animation("P93IMP47");
            CB->animation("P93GAR02");
            /* 0x8DFB */
            if (ST(0x98) > 1) {
                CB->combat(P_GUARDIAN25, 0);
            }
        }
        CB->select_pile(0, 1);
    }
    /* 0x8E2F */
    if (CB->divers(DV_WAS_HIT, P_KONKALITE6, 0)) {
        /* 0x8E41 */
        if (ST(ST_VIE(P_KONKALITE6)) != VIE_DEAD && ST(ST_LOCKED_CAMERA) == 2) {
            CB->select_pile(1, 2);
            CB->divers_empile(DV_WAS_HIT, P_KONKALITE6, 1);
            CB->combat(P_KONKALITE6, -1);
            ST(0x84) = CB->divers(DV_HP, P_KONKALITE6, -1);
            /* 0x8EC2 */
            if ((ST(0x84) - ST(V_HIT_DAMAGE)) > 0) {
                CB->divers(DV_HP, P_KONKALITE6, (ST(0x84) - ST(V_HIT_DAMAGE)));
            } else {
                CB->divers(DV_HP, P_KONKALITE6, 1);
            }
            /* 0x8F1C */
            if (CB->divers(DV_HP, P_KONKALITE6, -1) <= 1) {
                ST(ST_VIE(P_KONKALITE6)) = VIE_DEAD;
                ST(0xE30) = 3;
                ST(0xF4) = 1;
                CB->ajoute_zone(10, -2215, -2410, 120, 130, 0);
                CB->stop_joindre(-P_KONKALITE6);
                CB->animation("SP76IMP51");
                CB->animation("P76CAD");
                CB->pos_perso(P_KONKALITE6, 0, 0, 0, 2, "P76CAD", 0xC, 0);
                CB->divers_empile(DV_SCREEN, 2, 0);
                /* 0x8FC8 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L04D61", P_ARKHAN3);
                }
                /* 0x8FEA */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L04D62", P_ARKHAN3);
                }
            } else {
                CB->stop_joindre(-P_KONKALITE6);
                CB->animation("SP76IMP11");
                CB->animation("P76IMP46");
                CB->animation("P76GAR01");
                CB->combat(P_KONKALITE6, 0);
            }
            CB->select_pile(0, 1);
        } else {
            CB->divers_empile(DV_WAS_HIT, P_KONKALITE6, 1);
        }
    }
    /* 0x9076 */
    if (CB->divers(DV_WAS_HIT, P_KALHI13, 0)) {
        /* 0x9088 */
        if (ST(ST_VIE(P_KALHI13)) != VIE_DEAD && ST(0xB40) == 2) {
            CB->select_pile(1, 2);
            ST(0xE58) = 1;
            ST(ST_VIE(P_KALHI13)) = VIE_DEAD;
            CB->combat(P_KALHI13, -1);
            CB->divers(DV_HP, P_KALHI13, -1);
            CB->stop_joindre(-P_KALHI13);
            CB->animation("SP13IMP52");
            CB->animation("P13CAD");
            CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13CAD", 5, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
        } else {
            CB->divers_empile(DV_WAS_HIT, P_KALHI13, 1);
        }
    }
    /* 0x917F */
    if (CB->divers(DV_WAS_HIT, P_KALHI33, 0) && ST(ST_VIE(P_KALHI33)) != VIE_DEAD && ST(0xB40) == 2) {
        CB->select_pile(1, 2);
        ST(ST_VIE(P_KALHI33)) = VIE_DEAD;
        ST(0xE58) = 1;
        CB->combat(P_KALHI33, -1);
        CB->stop_joindre(-P_KALHI33);
        CB->animation("SP33IMP52");
        CB->animation("P33CAD");
        CB->pos_perso(P_KALHI33, 0, 0, 0, 2, "P33CAD", 1, 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0x9242 */
    if (ST(0x68) == 1 && ST(V_GUARD_FIGHT_DONE) == 0) {
        ST(0x68) = 2;
        CB->retire_zone(3);
        CB->change_camera(2, CAM_LOCKED);
        /* 0x928D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L04D01", P_ARKHAN2, "G00DIA01", NULL);
        }
        /* 0x92B5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L04D02", P_ARKHAN3, "G00DIA01", NULL);
        }
        CB->joindre(-ST(ST_HERO), -2314, -2266, 0x2EE0, -1);
        CB->etat_objet("SOB89", -1, -1, -1, -1, -1, -1, -2, 0);
        CB->animation("SP13LEV01");
        CB->animation("Sar_10");
        CB->modify_var(&ST(0x4C), 1);
        CB->modify_var(&ST(0x68), 3);
        CB->change_camera(99, CAM_AUTO);
    }
    /* 0x936C */
    if (ST(0x4C) == 1 && ST(ST_LOCKED_CAMERA) == 1) {
        ST(0x4C) = 0x63;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-ST(ST_HERO), -2103, -2553, 0x8096, -1);
        /* 0x93D5 */
        if (CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0)) {
            CB->joindre(-P_KALHI13, -2178, -2480, 0x940E, -1);
        }
        /* 0x940B */
        if (CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0)) {
            CB->joindre(-P_KALHI33, -2178, -2480, 0x940E, -1);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(0x4C), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x945A */
    if (ST(0x68) == 3 && !CB->on_joindre(ST(ST_HERO))) {
        ST(0x68) = 4;
        CB->stop_joindre(-ST(ST_HERO));
        CB->animation("Sar_10");
        /* 0x94B1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L04D03A", P_KALHI13, "SP13DIA58", "SP13ARR01_10");
            lieu_action_register("L04D03B", P_KALHI13, "SP13DIA59", "SP13ARR01_10");
        }
        /* 0x94F5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD45", P_KALHI13, "SP13DIA61", "SP13ARR01_10");
            lieu_action_register("LCOD46", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->joindre(-P_KALHI13, -2420, -2240, 0xABE0, -1);
        CB->modify_var(&ST(0x68), 5);
    }
    /* 0x9563 */
    if (ST(0x68) == 5 && !CB->on_joindre(P_KALHI13)) {
        ST(0x68) = 6;
        ST(ST_SCRIPT_BUSY) = 0;
        ST(0x7C) = 1;
        CB->divers(DV_LOOK_AT_PLAYER, P_KALHI13, 1);
        CB->divers(DV_IDLE_TIME, 1, 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x95D5 */
    if (ST(0x74) == 1 && ST(V_GUARD_FIGHT_DONE) == 0) {
        ST(0x74) = 2;
        CB->divers_empile(DV_SCREEN, 5, 0);
        ST(ST_VIE(P_KALHI13)) = 4;
        CB->pos_perso(P_KALHI13, -1, -1, -1, 4, "", 5, 0);
        CB->retire_zone(5);
        CB->pos_perso(P_KALHI33, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 1, "P33ARR01", 1, 1);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->animation("SP33ATT01");
        lieu_action_register("L04D04", P_KALHI33, "P33DIA54", "SP33ARR01");
        lieu_action_register("L04D05", P_KALHI33, "SP33DIA59", "SP33ARR01_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->divers(DV_IDLE_TIME, 1, 0);
        CB->modify_var(&ST(0x7C), 1);
        CB->bloque(BLOQUE_NONE);
        CB->divers(DV_LOOK_AT_PLAYER, P_KALHI33, 1);
        CB->select_pile(0, 1);
    }
    /* 0x974D */
    if (ST(0xFC) == 1 && CB->on_joindre(P_KALHI13) && ST(0xB40) < 3 && ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->select_pile(2, 1);
        CB->stop_joindre(-P_KALHI13);
        CB->select_pile(1, 0);
    }
    /* 0x97BE */
    if ((ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1) && ST(ST_SCRIPT_BUSY) == 0 && ST(0xB40) < 3 && ST(0xFC) == 0 && ST(V_GUARD_FIGHT_DONE) == 0 && ST(0x4C) != 0x63) {
        /* 0x9836 */
        if (ST(V_KALHI13_TALK_C) <= 2) {
            /* 0x9848 */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->select_pile(1, 1);
                /* 0x9873 */
                if (CB->distance(6, 5) < 140 && ST(0x88) == 0) {
                    ST(0x88) = 1;
                    ST(0x78) = 1;
                    CB->stop_joindre(P_KALHI13);
                    CB->pos_perso(P_KALHI13, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 1, "P13ARR01", 5, 1);
                    CB->joindre_perm(P_KALHI13, ST(ST_HERO));
                }
                /* 0x9920 */
                if (ST(0x7C) == 1 && (ST(0x38) % 20) == 0 && CB->distance(6, 5) >= 180) {
                    ST(0x88) = 0;
                    CB->divers(DV_RUN, P_KALHI13, 0);
                    CB->joindre(-P_KALHI13, -1, -1, -1, ST(ST_HERO));
                }
                CB->select_pile(0, 1);
            }
            /* 0x99B3 */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->select_pile(1, 1);
                /* 0x99DE */
                if (CB->distance(6, 1) < 140 && ST(0x88) == 0) {
                    /* 0x9A0B */
                    if (CB->divers(DV_RUN, P_KALHI33, 1) == 1) {
                        CB->divers(DV_RUN, P_KALHI33, 0);
                    }
                    ST(0x88) = 1;
                    ST(0x78) = 1;
                    CB->stop_joindre(P_KALHI33);
                    CB->pos_perso(P_KALHI33, CB->divers(DV_DESC_POS, P_KALHI33, 0), CB->divers(DV_DESC_POS, P_KALHI33, 1), CB->divers(DV_DESC_POS, P_KALHI33, 2), 1, "P33ARR01", 1, 1);
                    CB->joindre_perm(P_KALHI33, ST(ST_HERO));
                }
                /* 0x9AB8 */
                if (ST(0x7C) == 1 && (ST(0x38) % 20) == 0 && CB->distance(6, 1) >= 180) {
                    ST(0x88) = 0;
                    CB->divers(DV_RUN, P_KALHI33, 0);
                    CB->joindre(-P_KALHI33, -1, -1, -1, ST(ST_HERO));
                }
                CB->select_pile(0, 1);
            }
        }
        /* 0x9B4B */
        if (ST(V_EVENT15_STEP) == 1 && ST(V_KALHI13_TALK_C) == 3 && (CB->distance(5, 6) < 160 || CB->distance(1, 6) < 160)) {
            ST(V_KALHI13_TALK_C) = 2;
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(V_EVENT15_STEP) = 2;
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 0xD);
            /* 0x9BF7 */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->animation("SP13LEV01");
                lieu_action_register("L04D06A", P_KALHI13, "P13DIA60", "SP13ARR01_10");
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L04D06B", P_KALHI13, "P13DIA60", "SP13ARR01_10");
                lieu_action_register("L04D06C", P_KALHI13, "P13DIA60", "SP13ARR01_10");
                CB->divers(DV_LOOK_AT_PLAYER, P_KALHI13, 1);
            }
            /* 0x9C8E */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->animation("SP33LEV01");
                lieu_action_register("L04D06A", P_KALHI33, "P33DIA60", "SP33ARR01_10");
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L04D06B", P_KALHI33, "P33DIA60", "SP33ARR01_10");
                lieu_action_register("L04D06C", P_KALHI33, "P33DIA60", "SP33ARR01_10");
                CB->divers(DV_LOOK_AT_PLAYER, P_KALHI33, 1);
            }
            CB->modify_var(&ST(0x7C), 1);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
    /* 0x9D4F */
    if (ST(ST_SCRIPT_BUSY) == 0 && ST(0x78) == 1 && CB->divers(DV_IDLE_TIME, 0, 0) >= 0x2710 && ST(V_GUARD_FIGHT_DONE) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(0x78) = 0;
        /* 0x9DB7 */
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            /* 0x9DC9 */
            if (ST(0x948) == 2) {
                lieu_action_register("L04D07", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
            }
            /* 0x9DF4 */
            if (ST(0x948) == 1) {
                ST(0x948) = 2;
                lieu_action_register("L04D08", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
            }
            /* 0x9E2E */
            if (ST(0x948) == 0) {
                ST(0x948) = 1;
                lieu_action_register("L04D09", P_KALHI13, "SP13DIA61", "SP13ARR01_10");
            }
        }
        /* 0x9E68 */
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            /* 0x9E7A */
            if (ST(0x948) == 2) {
                lieu_action_register("L04D07", P_KALHI33, "SP33DIA60", "SP33ARR01_10");
            }
            /* 0x9EA5 */
            if (ST(0x948) == 1) {
                ST(0x948) = 2;
                lieu_action_register("L04D08", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
            }
            /* 0x9EDF */
            if (ST(0x948) == 0) {
                ST(0x948) = 1;
                lieu_action_register("L04D09", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
            }
        }
        CB->divers(DV_IDLE_TIME, 1, 0);
        CB->modify_var(&ST(0x78), 1);
        return;
    }
    /* 0x9F44 */
    if (ST(0x98) == 0 && ST(ST_VIE(P_GUARDIAN2)) == 1 && ST(V_GUARD_FIGHT_DONE) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_EVENT_34) == 1) {
        ST(V_EVENT_34) = 0;
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        /* 0x9FC5 */
        if (ST(ST_LOCKED_CAMERA) != 1) {
            CB->change_camera(1, CAM_LOCKED);
        }
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->dialogue("L04D204", -1);
        /* 0xA015 */
        if (ST(0xE90) == 0) {
            CB->dialogue("L04D10A", P_GUARDIAN2);
            CB->dialogue("L04D10B", P_GUARDIAN2);
        }
        /* 0xA047 */
        if (ST(0xE90) == 1) {
            lieu_action_register("L04D11A", P_GUARDIAN2, "SP42DIA62", "SP42ARR03_10");
            lieu_action_register("L04D11B", P_GUARDIAN2, "SP42DIA60", "SP42ARR03_10");
            ST(0xE90) = 2;
        }
        CB->change_camera(99, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xA0BF */
    if (ST(0x98) == 0 && ST(ST_VIE(P_GUARDIAN25)) == 1) {
        /* 0xA0E3 */
        if (ST(V_EVENT_33) == 1) {
            CB->select_pile(1, 1);
            ST(V_EVENT_33) = 0;
            /* 0xA114 */
            if (ST(0xD48) == 0) {
                CB->bloque(BLOQUE_ALL);
                CB->change_camera(-2, CAM_LOCKED);
            }
            CB->divers_empile(DV_RANDOM, 0, 0);
            CB->divers_empile(DV_RANDOM, 0, 0);
            ST(V_DLG_RANDOM) = CB->random(3);
            /* 0xA176 */
            if (ST(V_DLG_RANDOM) == 0) {
                ST(0xD4) = 1;
            }
            /* 0xA197 */
            if (ST(V_DLG_RANDOM) == 1) {
                ST(0xD4) = 2;
            }
            /* 0xA1B8 */
            if (ST(V_DLG_RANDOM) == 2) {
                ST(0xD4) = 3;
            }
        }
        /* 0xA1D9 */
        if (ST(0xD4) == 1) {
            ST(0xD4) = 0;
            ST(0xD8) = 1;
            CB->bruitage("L04B02", -1920, -2452);
            CB->joindre(-P_GUARDIAN25, -1920, -2452, 0xAFC8, -1);
        }
        /* 0xA23A */
        if (ST(0xD4) == 2) {
            ST(0xD4) = 0;
            ST(0xD8) = 1;
            CB->dialogue("L04D207", -1);
            CB->bruitage("L04B03", -2012, -2400);
            CB->joindre(-P_GUARDIAN25, -2012, -2400, 0, -1);
        }
        /* 0xA2A8 */
        if (ST(0xD4) == 3) {
            ST(0xD4) = 0;
            ST(0xD8) = 1;
            CB->bruitage("L04B04", -2187, -2452);
            CB->joindre(-P_GUARDIAN25, -2163, -2460, 0x2809, -1);
        }
        /* 0xA309 */
        if (ST(0xD8) == 1 && !CB->on_joindre(P_GUARDIAN25)) {
            ST(0xD8) = 2;
            CB->stop_joindre(-P_GUARDIAN25);
            /* 0xA348 */
            if (ST(0xD48) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                /* 0xA371 */
                if (ST(V_DLG_RANDOM) == 0) {
                    lieu_action_register("L04D15", P_GUARDIAN25, "SP93DIA59", "SP93ARR02_10");
                }
                /* 0xA39C */
                if (ST(V_DLG_RANDOM) == 1) {
                    lieu_action_register("L04D16", P_GUARDIAN25, "SP93DIA60", "SP93ARR02_10");
                }
                /* 0xA3C7 */
                if (ST(V_DLG_RANDOM) == 2) {
                    lieu_action_register("L04D17", P_GUARDIAN25, "SP93DIA61", "SP93ARR02_10");
                }
            }
            /* 0xA3F2 */
            if (ST(0xD48) == 0) {
                lieu_action_register("L04D18A", P_GUARDIAN25, "SP93DIA55", "SP93ARR02_10");
                lieu_action_register("L04D18B", P_GUARDIAN25, "SP93DIA60", "SP93ARR02_10");
                lieu_action_register("L04D18C", P_GUARDIAN25, "SP93DIA53", "SP93ARR02_10");
            }
            CB->modify_var(&ST(0xD8), 3);
        }
        /* 0xA466 */
        if (ST(0xD8) == 3) {
            ST(0xD8) = 4;
            CB->joindre(-P_GUARDIAN25, -2072, -2500, 0x5DC0, -1);
            CB->modify_var(&ST(0xD8), 5);
        }
        /* 0xA4BA */
        if (ST(0xD8) == 5 && !CB->on_joindre(P_GUARDIAN25)) {
            ST(0xD8) = 0;
            CB->stop_joindre(-P_GUARDIAN25);
            CB->evenement(0x21, 10000);
            /* 0xA509 */
            if (ST(0xD48) == 0) {
                ST(0xD48) = 1;
                CB->change_camera(99, CAM_AUTO);
                CB->bloque(BLOQUE_NONE);
            }
            CB->select_pile(0, 1);
        }
    }
    /* 0xA54F */
    if (ST(ST_HERO) != 4 && ST(ST_VIE(P_GUARDIAN2)) == 1 && (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < -2000 || ST(0xBC) == 1) && ST(0x98) == 0 && ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0x22, -1);
        CB->pos_perso(P_KALHI13, -2555, -2220, 0xAFC8, 1, "P13ARR03", 5, 1);
        ST(0x98) = 1;
        ST(0x6C) = 1;
        ST(0xD8) = 0;
        /* 0xA646 */
        if (ST(0xAE4) == 1) {
            CB->animation("SP42ARR03_10");
            CB->joindre(-P_GUARDIAN2, 0, 0, 0, ST(ST_HERO));
            CB->animation("SP42GAR26_10");
            CB->animation("P42_65535");
            CB->animation("P42GAR03_10");
            CB->joindre_perm(P_GUARDIAN2, ST(ST_HERO));
            CB->dialogue("L04D12", P_GUARDIAN2);
            CB->animation("Sar_50");
            CB->modify_var(&ST(0xDC), 1);
            CB->bloque(BLOQUE_NONE);
        }
        /* 0xA6F1 */
        if (ST(0xAE4) == 0) {
            ST(0xAE4) = 1;
            /* 0xA723 */
            if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < -2000) {
                CB->pos_perso(P_GUARDIAN2, -1955, -2622, 0x1388, 1, "P42ARR01", 0xD, 0);
                CB->animation("SP42ATT00");
                CB->dialogue("L04D13", P_GUARDIAN2);
                CB->animation("P42GAR03_10");
            }
            CB->pos_perso(P_GUARDIAN2, -2044, -2561, CB->divers(DV_DESC_POS, P_GUARDIAN2, 2), 1, "P42GAR03", 0xD, 0);
            CB->animation("SP42ATT01");
            CB->animation("P42GAR03_10");
            CB->joindre_perm(P_GUARDIAN2, ST(ST_HERO));
            CB->dialogue("L04D14A", P_GUARDIAN2);
            CB->dialogue("L04D14B", P_GUARDIAN2);
            CB->animation("Sar_50");
            CB->vire_col(3, 1);
            CB->modify_var(&ST(0xDC), 1);
            CB->bloque(BLOQUE_NONE);
        }
        CB->select_pile(0, 1);
    }
    /* 0xA846 */
    if (ST(ST_HERO) != 4 && ST(ST_VIE(P_GUARDIAN25)) == 1 && (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < -2000 || ST(0xBC) == 1) && ST(0x98) == 0) {
        CB->select_pile(1, 2);
        ST(0x6C) = 1;
        ST(0x98) = 1;
        ST(0xD4) = 0;
        ST(0xD8) = 0;
        ST(V_EVENT_33) = 0;
        CB->bloque(BLOQUE_ALL);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        CB->evenement(0x21, -1);
        CB->stop_joindre(-P_GUARDIAN25);
        CB->joindre_perm(-P_GUARDIAN25, ST(ST_HERO));
        CB->stop_joindre(P_GUARDIAN25);
        /* 0xA970 */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < -2000) {
            CB->animation("SP93GAR26");
            CB->animation("P93GAR02_10");
        } else {
            CB->animation("SP93GAR02");
        }
        CB->joindre_perm(P_GUARDIAN25, ST(ST_HERO));
        /* 0xA9C1 */
        if (ST(0xAE4) == 0x29A) {
            CB->dialogue("L04D20", P_GUARDIAN25);
            ST(0xAE4) = 0x29B;
        }
        /* 0xA9F5 */
        if (ST(0xAE4) < 0x29A) {
            ST(0xAE4) = 0x29A;
            CB->dialogue("L04D19", P_GUARDIAN25);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(0xDC), 1);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xAA5D */
    if (ST(ST_HERO) != 4 && ST(0xDC) == 1 && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) < -200) {
        ST(0xDC) = 2;
        CB->select_pile(1, 2);
        /* 0xAAC3 */
        if (ST(ST_VIE(P_GUARDIAN2)) == 1) {
            CB->select_pile(0, 2);
            CB->pos_perso(P_KALHI13, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 1, "P13ARR05", 5, 1);
            CB->bruitage("L04AMB1", -1, -1);
            ST(V_ENGAGED_PERSONA) = P_GUARDIAN2;
            ST(0x98) = 2;
            CB->stop_joindre(-P_GUARDIAN2);
            CB->animation("SP42GAR03");
            CB->combat(P_GUARDIAN2, 0);
        }
        /* 0xAB82 */
        if (ST(ST_VIE(P_GUARDIAN25)) == 1) {
            ST(V_ENGAGED_PERSONA) = P_GUARDIAN25;
            ST(0x98) = 2;
            CB->stop_joindre(-P_GUARDIAN25);
            CB->animation("SP93GAR02");
            CB->combat(P_GUARDIAN25, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0xABDF */
    if (ST(0x98) == 2 && ((ST(ST_VIE(P_GUARDIAN2)) == 1 && CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN2, 0)) || (ST(ST_VIE(P_GUARDIAN25)) == 1 && CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN25, 0))) && CB->distance(6, 3) <= 150) {
        CB->select_pile(1, 2);
        ST(0x98) = 3;
        /* 0xAC83 */
        if (ST(0xB40) == 1 && ST(ST_VIE(P_GUARDIAN2)) == 1 && CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN2, 0)) {
            CB->dialogue("L04D205", -1);
            CB->dialogue("L04D206", -1);
        }
        /* 0xACE1 */
        if (ST(0xB40) == 4 && ST(ST_VIE(P_GUARDIAN25)) == 1 && CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN25, 0)) {
            CB->dialogue("L04D208", -1);
            CB->dialogue("L04D209", -1);
            CB->dialogue("L04D210A", -1);
            CB->dialogue("L04D210B", -1);
        }
        CB->select_pile(0, 1);
        return;
    }
    /* 0xAD71 */
    if (ST(0xE0) == 1) {
        ST(0xE0) = 2;
        ST(ST_SCRIPT_BUSY) = 1;
        CB->retire_zone(3);
        /* 0xADA9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L04D23", P_ARKHAN2);
        }
        /* 0xADCB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L04D24", P_ARKHAN3);
        }
        CB->joindre(-ST(ST_HERO), -2357, -2356, -1, -1);
        CB->evenement(1, 1000);
    }
    /* 0xAE20 */
    if (ST(V_EVENT_1) == 1) {
        ST(V_ENGAGED_PERSONA) = P_KONKALITE6;
        ST(V_EVENT_1) = 0;
        ST(0xEA0) = 1;
        CB->free_anim(0);
        CB->free_anim(1);
        CB->free_anim(2);
        CB->load_anim("204PORT3", 5);
        CB->divers_empile(DV_SCREEN, 2, 0);
        CB->evenement(2, 200);
        CB->animation("SP76ACT86");
        CB->animation("P76GAR01");
        CB->divers_empile(DV_SCREEN, 2, 0);
        CB->dialogue("L04D60A", P_KONKALITE6);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x4C);
        CB->dialogue("L04D60B", P_KONKALITE6);
        CB->animation("Sar_10");
        CB->divers_empile(DV_SCREEN, 2, 0);
        CB->animation("SP76GAR01_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->combat(P_KONKALITE6, 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xAF5F */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        /* 0xAF80 */
        if (ST(ST_VIE(P_KONKALITE6)) != VIE_DEAD) {
            CB->select_pile(2, 2);
            CB->vire_col(0x11, 1);
            CB->vire_col(0x12, 0);
            CB->select_pile(1, 2);
            CB->play_anim(5, 0);
            CB->free_anim(3);
            CB->select_pile(0, 1);
        } else {
            CB->select_pile(2, 2);
            CB->play_anim(4, 0);
            CB->free_anim(5);
            CB->vire_col(0x12, 1);
            CB->evenement(0x26, 900);
            CB->select_pile(0, 1);
        }
    }
    /* 0xB04E */
    if (ST(V_EVENT_38) == 1) {
        ST(V_EVENT_38) = 0;
        CB->select_pile(2, 2);
        CB->load_anim("204OUVER", 0);
        CB->load_anim("204OUV2", 1);
        CB->play_anim(1, 0);
        CB->free_anim(4);
        CB->animation("Sar_50");
        CB->select_pile(0, 1);
    }
    /* 0xB0CC */
    if (ST(V_EVENT_35) == 1 && ST(0xF4) != 1) {
        ST(0xF4) = 1;
    }
    /* 0xB0FF */
    if (ST(ST_LOCKED_CAMERA) != 1 && ST(0x8EC) == 1 && ST(0xF4) != 1) {
        ST(0xF4) = 1;
    }
    /* 0xB144 */
    if (ST(ST_HERO) != 4 && ST(ST_VIE(P_KONKALITE6)) == VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_LOCKED_CAMERA) == 1 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < -1930 && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) > -2600 && ST(0xF4) == 1) {
        ST(V_EVENT_35) = 0;
        ST(0xF4) = 0;
        CB->select_pile(1, 2);
        /* 0xB20F */
        if (ST(0xF8) == 0) {
            ST(0xF8) = 1;
            CB->dialogue("L04D211A", -1);
            CB->dialogue("L04D212", -1);
            CB->evenement(0x23, 5000);
            CB->select_pile(0, 1);
            return;
        }
        /* 0xB272 */
        if (ST(0xF8) == 1) {
            /* 0xB284 */
            if (ST(0x8EC) == 0) {
                ST(0x8EC) = 1;
                CB->dialogue("L04D213A", -1);
                CB->dialogue("L04D213B", -1);
                CB->evenement(0x23, 4000);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xB2E7 */
            if (ST(0x8EC) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                /* 0xB310 */
                if (ST(V_DLG_RANDOM) == 0) {
                    CB->dialogue("L04D214", -1);
                    CB->dialogue("L04D215", -1);
                    CB->evenement(0x23, 5000);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xB364 */
                if (ST(V_DLG_RANDOM) == 1) {
                    CB->dialogue("L04D216", -1);
                    CB->dialogue("L04D217", -1);
                    CB->evenement(0x23, 6000);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xB3B8 */
                if (ST(V_DLG_RANDOM) == 2) {
                    CB->dialogue("L04D218", -1);
                    CB->dialogue("L04D219", -1);
                    CB->evenement(0x23, 5000);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xB40C */
                if (ST(V_DLG_RANDOM) == 3) {
                    CB->dialogue("L04D220", -1);
                    CB->dialogue("L04D221", -1);
                    CB->evenement(0x23, 8000);
                    CB->select_pile(0, 1);
                    return;
                }
            }
        }
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x10011879).
 * Callback ingestion (0x100114FA..0x10011860), the trigger-table fill and
 * the unused dword_10022B0C = state+0xC8 are skipped: g_lieu_cb is provided
 * by the bridge, the trigger table is static in lieu04_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x10011AA4);
 * every other path returns state[0].
 * ---------------------------------------------------------------------- */
static i32 init(i32 reload_flag) {
    if (reload_flag)
        return 0;

    CB->select_pile(2, 2);
    CB->select_pile(1, 2);
    CB->select_pile(0, 2);
    CB->select_pile(0, 1);
    CB->evenement(0, -1);
    CB->evenement(1, -1);
    CB->evenement(2, -1);
    CB->evenement(33, -1);
    CB->evenement(34, -1);
    CB->evenement(35, -1);
    CB->evenement(36, -1);
    CB->evenement(37, -1);
    CB->evenement(38, -1);
    CB->evenement(39, -1);
    CB->evenement(41, -1);

    if (ST(ST_HERO) != 4 || ST(0xE8C) == 1) {
        l04_trig00();   /* 0x10011AA9: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1001198E — the hero is dead: play the death ending */
    CB->divers_empile(DV_SCREEN, 1, 0);
    ST(V_DEATH_EVENT_SENT) = 1;
    for (i32 layer = 0; layer < 4; layer++) {
        if (CB->divers(DV_PERSONA_PRESENT, layer, 0))
            CB->divers(DV_HP, layer, 0);
    }
    if (ST(V_GUARD_FIGHT_DONE) == 0)
        CB->cinematique("SC22_500");
    if (ST(V_GUARD_FIGHT_DONE) != 0)
        CB->cinematique("SC23_500");
    CB->divers_empile(DV_END_GAME, 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1, 0x10011AFC): drain pending sound, run body.
 * ---------------------------------------------------------------------- */
static void tick(void) {
    if (ST(V_PENDING_EVENT) != 0) {
        CB->select_pile(2, 1);
        CB->evenement(ST(V_PENDING_EVENT) + EV_PENDING_BASE, 1000);
        ST(V_PENDING_EVENT) = 0;
        CB->select_pile(0, 1);
    }
    tick_body();
}

/* -------------------------------------------------------------------------
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x100114E0).
 * Jump table @ 0x10011B79, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu04_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu04: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l04_trigger(arg);             break;  /* dword_100229A0[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
