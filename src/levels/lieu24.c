/*
 * lieu24.c - Native hand-port of LIEU24.DLL (level-24 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu24_trig.c (reached through
 * l24_trigger, see lieu24_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x10010FC0, the state
 * pointer global is dword_10021C9C and the trigger table is
 * dword_10021BB0[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register, lieu_pre_scene and sub_10001200 (float reset) are
 * the shared lieu_common.c. tick_body (sub_1000880D) is level-24 specific.
 */

#include <stdio.h>
#include <stdlib.h>

#include "lieu24_int.h"

/* DLL .data globals (restart on every room load, see the entry). */
i32 g_l24_heisgoing;           /* dword_1001DB34 */
static c8 g_txt[256];          /* debug sprintf buffer, dword_100219B0 */

/* The DLL is loaded afresh for every room: its globals restart. Called by
 * INIT, and by tools/lieu_difftest_harness.c before every case. */
void l24_reset_dll_globals(void) {
    g_l24_heisgoing = 0;
    g_txt[0] = '\0';
}

/* -------------------------------------------------------------------------
 * Per-frame body - sub_1000880D (thunk 0x100010B9), level-24 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    snprintf(g_txt, sizeof(g_txt), "heisgoing : %d", g_l24_heisgoing);
    CB->print_msg(g_txt, 0, 310);
    /* 0x8847 */
    if (g_l24_heisgoing == 1 && !CB->on_joindre(P_THANANDAR24)) {
        CB->animation("Sar_10");
        g_l24_heisgoing = 2;
        ST(ST_SCRIPT_BUSY) = 0;
        CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x88A8 */
    if (ST(ST_COMBAT_STANCE) == 0 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8) || CB->divers(DV_IN_ZONE, ST(ST_HERO), 11))) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        switch (ST(ST_HERO)) {
        case 2:   /* 0x8945 */
            /* 0x8945 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD20", P_ARKHAN2);
            } else {
                CB->dialogue("LCOD61", P_ARKHAN2);
            }
            break;
        case 3:   /* 0x8981 */
            CB->dialogue("LCOD21", P_ARKHAN3);
            break;
        default:
            break;
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x89F2 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KONKALITE1) && ST(ST_VIE(P_KONKALITE1)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->joindre_perm(P_KONKALITE1, ST(ST_HERO));
        ST(V_PLAYER_STRIKE) = 2;
        l24_trig11();
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x8AE6 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8) && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_THANANDAR24) && ST(ST_VIE(P_THANANDAR24)) == 1) {
        ST(V_CRI_USED) = 1;
        ST(L24_THANANDAR_FIGHT_STATE) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("P24GAR01_10");
        CB->dialogue("L34D201", P_THANANDAR24);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        /* 0x8BC0 */
        if (ST(ST_VIE(P_KONKALITE1)) == 1) {
            CB->combat(P_KONKALITE1, 0);
            ST(V_ENGAGED_PERSONA) |= 0x71;
        }
        CB->combat(P_THANANDAR24, 0);
        ST(V_ENGAGED_PERSONA) |= 0x2400;
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x8C2D */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        ST(V_END_GAME_REQ) = 0;
        CB->evenement(44, 2500);
    }
    /* 0x8C7F */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        /* 0x8CB0 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x8CD9 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x8D05 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x8D31 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x8D55 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x8D72 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x8DA6 */
    if (CB->divers(DV_WAS_HIT, P_THANANDAR24, 0) && ST(ST_VIE(P_THANANDAR24)) == 1) {
        CB->select_pile(2, 2);
        CB->divers_empile(DV_WAS_HIT, P_THANANDAR24, 1);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        /* 0x8DFB */
        if (ST(ST_VIE(P_KONKALITE1)) == 1) {
            CB->combat(P_KONKALITE1, 0);
            ST(V_ENGAGED_PERSONA) |= 0x71;
        }
        /* 0x8E23 */
        if (ST(L24_THANANDAR_FIGHT_STATE) == 0) {
            ST(L24_THANANDAR_FIGHT_STATE) = 1;
        }
        /* 0x8E58 */
        if ((CB->divers(DV_HP, P_THANANDAR24, -1) - ST(V_HIT_DAMAGE)) > 0) {
            CB->divers(DV_HP, P_THANANDAR24, (CB->divers(DV_HP, P_THANANDAR24, -1) - ST(V_HIT_DAMAGE)));
        } else {
            CB->divers(DV_HP, P_THANANDAR24, 1);
        }
        /* 0x8EB9 */
        if (CB->divers(DV_HP, P_THANANDAR24, -1) <= 100) {
            ST(L24_THANANDAR_FIGHT_STATE) = 2;
            ST(L24_THANANDAR_KILLED_MARK) = 5;
            ST(ST_VIE(P_THANANDAR24)) = VIE_DEAD;
            ST(V_ENGAGED_PERSONA) &= 0xFF;
            CB->combat(P_THANANDAR24, -1);
            CB->animation("SP24IMP51");
            CB->animation("SP24_65535");
            CB->animation("P24CAD");
            CB->pos_perso(P_THANANDAR24, 0, 0, 0, 2, "P24CAD", 8, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
        } else {
            CB->stop_joindre(-P_THANANDAR24);
            CB->animation("SP24IMP11");
            CB->animation("P24IMP46");
            CB->animation("SP24_65535");
            CB->animation("P24GAR01");
            CB->combat(P_THANANDAR24, 0);
            ST(V_ENGAGED_PERSONA) |= 0x2400;
        }
    }
    /* 0x8FB8 */
    if (ST(ST_VIE(P_THANANDAR24)) == VIE_DEAD && ((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) == 0x24) {
        ST(L24_THANANDAR_FIGHT_STATE) = 2;
        ST(L24_THANANDAR_KILLED_MARK) = 5;
        /* 0x8FF7 */
        if (ST(L24_ARKHAN23_STEP) < 6) {
            ST(L24_THANANDAR_COMPANION) = 1;
        }
        ST(V_ENGAGED_PERSONA) &= 0xFF;
        CB->combat(P_THANANDAR24, -1);
    }
    /* 0x9034 */
    if (ST(ST_VIE(P_KONKALITE1)) == VIE_DEAD && (ST(V_ENGAGED_PERSONA) & 0xFF) == 0x71) {
        ST(V_ENGAGED_PERSONA) &= 0xFF00;
        CB->combat(P_KONKALITE1, -1);
    }
    /* 0x9079 */
    if (CB->divers(DV_WAS_HIT, P_KONKALITE1, 0) && ST(ST_VIE(P_KONKALITE1)) != VIE_DEAD) {
        CB->select_pile(1, 2);
        CB->divers_empile(DV_WAS_HIT, P_KONKALITE1, 1);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        /* 0x90E2 */
        if ((CB->divers(DV_HP, P_KONKALITE1, -1) - ST(V_HIT_DAMAGE)) > 0) {
            CB->divers(DV_HP, P_KONKALITE1, (CB->divers(DV_HP, P_KONKALITE1, -1) - ST(V_HIT_DAMAGE)));
        } else {
            CB->divers(DV_HP, P_KONKALITE1, 1);
        }
        /* 0x9143 */
        if (CB->divers(DV_HP, P_KONKALITE1, -1) <= 100) {
            CB->combat(P_KONKALITE1, -1);
            ST(V_ENGAGED_PERSONA) &= 0xFF00;
            ST(ST_VIE(P_KONKALITE1)) = VIE_DEAD;
            CB->stop_joindre(-P_KONKALITE1);
            CB->animation("SP71IMP53");
            CB->animation("P71CAD");
            CB->pos_perso(P_KONKALITE1, 0, 0, 0, 2, "P71CAD", 0xB, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
        } else {
            CB->stop_joindre(-P_KONKALITE1);
            CB->animation("SP71IMP13");
            CB->animation("P71IMP48");
            CB->animation("P71GAR03");
            CB->combat(P_KONKALITE1, 0);
            ST(V_ENGAGED_PERSONA) |= 0x71;
        }
        /* 0x9216 */
        if (ST(ST_VIE(P_THANANDAR24)) == 1 && ((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) != 0x24) {
            CB->select_pile(2, 1);
            CB->dialogue("L24D70", P_THANANDAR24);
            CB->combat(P_THANANDAR24, 0);
            ST(V_ENGAGED_PERSONA) |= 0x2400;
            CB->select_pile(0, 1);
        }
        CB->select_pile(0, 1);
    }
    /* 0x9292 */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN16, 0) && ST(ST_VIE(P_GUARDIAN16)) != VIE_DEAD) {
        CB->select_pile(0, 2);
        CB->select_pile(1, 2);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->divers_empile(DV_WAS_HIT, P_THANANDAR24, 1);
        CB->animation("SP56IMP13");
        CB->animation("SP56_65535");
        CB->combat(P_GUARDIAN16, -1);
        CB->pos_perso(P_GUARDIAN16, 0, 0, 0, 2, "P56CAD", 9, 0);
        CB->vire_col(0x10, 0);
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = 0;
        ST(L24_GUARDIAN16_STATE) = 2;
        ST(ST_VIE(P_GUARDIAN16)) = VIE_DEAD;
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0x93A5 */
    if (ST(V_EVENT15_STEP) == 1 && !CB->on_joindre(P_GUARDIAN16)) {
        ST(V_EVENT15_STEP) = 0;
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(2, CAM_AUTO);
        CB->animation("G00CON01_10");
        lieu_action_register("L24D02", P_GUARDIAN16, "SP56ATT01", "SP56GAR03_200");
        CB->animation("P56GAR03");
        CB->joindre(-ST(ST_HERO), 6205, -4200, 0x2968, -1);
        lieu_action_register("L24D01", P_ARKHAN2, "SG00DIA06", NULL);
        lieu_action_register("L24D03", P_GUARDIAN16, "SP56ATT02", "SP56GAR03_200");
        CB->modify_var(&ST(V_EVENT15_STEP), 2);
    }
    /* 0x948F */
    if (ST(V_EVENT15_STEP) == 2 && !CB->on_joindre(ST(ST_HERO))) {
        ST(V_EVENT15_STEP) = 0;
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = L24_ENGAGED_GUARDIAN;
        CB->vire_col(0xC, 1);
        CB->animation("Sar_10");
        CB->bloque(BLOQUE_NONE);
        CB->combat(P_GUARDIAN16, 0);
        return;
    }
    /* 0x9513 */
    if (ST(V_ENGAGED_PERSONA) == L24_ENGAGED_GUARDIAN && ST(ST_VIE(P_GUARDIAN16)) == 1 && ST(ST_LOCKED_CAMERA) == 0 && CB->divers(DV_DESC_POS, P_GUARDIAN16, 0) > 6480 && ST(L24_GUARDIAN16_MOVED) == 0) {
        ST(L24_GUARDIAN16_MOVED) = 1;
        CB->pos_perso(P_GUARDIAN16, 6570, -4420, 0x41A0, 1, "P56GAR03", 9, 0);
    }
    /* 0x95A9 */
    if (ST(V_ENGAGED_PERSONA) == L24_ENGAGED_GUARDIAN && ST(ST_VIE(P_GUARDIAN16)) == 1 && ST(ST_LOCKED_CAMERA) == 1 && ST(L24_GUARDIAN16_MOVED) == 1) {
        ST(L24_GUARDIAN16_MOVED) = 0;
    }
    /* 0x9609 */
    if (CB->distance(8, 7) < 200 && ST(L24_FIGHT_PAIR_MODE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_THANANDAR24)) == 1) {
        ST(V_ENGAGED_PERSONA) |= 0x2400;
        CB->combat(P_THANANDAR24, 0);
        /* 0x9664 */
        if (ST(L24_THANANDAR_DIALOGUE_KIND) == 0) {
            ST(L24_THANANDAR_DIALOGUE_KIND) = 1;
        }
    }
    /* 0x968B */
    if (CB->distance(11, 7) < 200 && ST(L24_FIGHT_PAIR_MODE) == 2 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE1)) == 1) {
        ST(V_ENGAGED_PERSONA) |= 0x71;
        CB->combat(P_KONKALITE1, 0);
    }
    /* 0x96EF */
    if (CB->distance(11, 7) < 200 && ST(L24_FIGHT_PAIR_MODE) == 3 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_THANANDAR24)) == 1) {
        ST(V_ENGAGED_PERSONA) |= 0x2471;
        CB->combat(P_THANANDAR24, 0);
        CB->combat(P_KONKALITE1, 0);
        /* 0x9757 */
        if (ST(L24_THANANDAR_DIALOGUE_KIND) == 0) {
            ST(L24_THANANDAR_DIALOGUE_KIND) = 1;
        }
    }
    /* 0x9775 */
    if (ST(ST_VIE(P_GUARDIAN16)) >= 2 && ST(L24_GUARDIAN16_STATE) == 0) {
        ST(L24_GUARDIAN16_STATE) = 1;
    }
    /* 0x97A8 */
    if (ST(ST_VIE(P_GUARDIAN16)) >= 2 && ST(V_ENGAGED_PERSONA) == L24_ENGAGED_GUARDIAN) {
        ST(V_ENGAGED_PERSONA) = 0;
        ST(L24_THANANDAR_WEAPON_WARNED) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x97EC */
        if (ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("L24D05", P_ARKHAN2, "SG00DIA02", NULL);
        }
        /* 0x9814 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            lieu_action_register("L24D06", P_ARKHAN2, "SG00DIA03", NULL);
        }
        CB->pos_perso(P_THANANDAR24, 6080, -3850, 0x7F7B, 1, "P24ARR01", 8, 0);
        CB->stop_joindre(-P_THANANDAR24);
        CB->animation("SP24ARR01_10");
        CB->modify_var(&ST(L24_GUARDIAN16_AFTERMATH), 1);
        CB->vire_col(0x10, 0);
        /* 0x989B */
        if (ST(ST_LOCKED_CAMERA) != 0 || CB->divers(DV_DESC_POS, ST(ST_HERO), 1) < -4320) {
            CB->bloque(BLOQUE_NONE);
        }
    }
    /* 0x98D8 */
    if (ST(L24_GUARDIAN16_AFTERMATH) == 1 && ST(ST_LOCKED_CAMERA) == 0 && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) > -4320) {
        ST(L24_GUARDIAN16_AFTERMATH) = 2;
        CB->divers(DV_COMBAT_STANCE, 0, 0);
        /* 0x993D */
        if (ST(L24_THANANDAR_COMPANION) == 0) {
            ST(L24_ARKHAN23_STEP) = 2;
            /* 0x9969 */
            if (CB->divers(DV_DESC_POS, P_ARKHAN2, 1) <= -4150 || (CB->divers(DV_DESC_POS, P_ARKHAN2, 1) > -4150 && (CB->divers(DV_DESC_POS, P_ARKHAN2, 0) < 6010 || CB->divers(DV_DESC_POS, P_ARKHAN2, 0) > 6200))) {
                CB->divers_empile(DV_MUSIC, 1, 1);
                ST(L24_THANANDAR_ARRIVAL_STEP) = 1;
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->animation("Sar_10");
                CB->joindre(-ST(ST_HERO), 0, 0, 0, 24);
            }
            /* 0x9A3D */
            if (CB->divers(DV_DESC_POS, P_ARKHAN2, 1) > -4150 && CB->divers(DV_DESC_POS, P_ARKHAN2, 0) > 6010 && CB->divers(DV_DESC_POS, P_ARKHAN2, 0) < 6200) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                ST(L24_THANANDAR_ARRIVAL_STEP) = 1;
                ST(L24_HERO_NEAR_AT_ENTRY) = 1;
                CB->joindre(-ST(ST_HERO), 0, 0, 0xA, -1);
                /* 0x9ADB */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0x9AED */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0x9B07 */
                        if (CB->divers(DV_CAN_FIGHT, 8, 2) || CB->divers(DV_CAN_FIGHT, 4, 2) || CB->divers(DV_CAN_FIGHT, 5, 2)) {
                            CB->animation("SP02MAR30");
                        } else {
                            CB->animation("SP02MAR29");
                        }
                    }
                    /* 0x9B68 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0x9B82 */
                        if (CB->divers(DV_CAN_FIGHT, 8, 2) || CB->divers(DV_CAN_FIGHT, 4, 2) || CB->divers(DV_CAN_FIGHT, 5, 2)) {
                            CB->animation("SP03MAR30");
                        } else {
                            CB->animation("SP03MAR29");
                        }
                    }
                }
                /* 0x9BE3 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0x9BF5 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        /* 0x9C0F */
                        if (CB->divers(DV_CAN_FIGHT, 8, 2) || CB->divers(DV_CAN_FIGHT, 4, 2) || CB->divers(DV_CAN_FIGHT, 5, 2)) {
                            CB->animation("SP02MAR82");
                        } else {
                            CB->animation("SP02MAR81");
                        }
                    }
                    /* 0x9C70 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        /* 0x9C8A */
                        if (CB->divers(DV_CAN_FIGHT, 8, 2) || CB->divers(DV_CAN_FIGHT, 4, 2) || CB->divers(DV_CAN_FIGHT, 5, 2)) {
                            CB->animation("SP03MAR82");
                        } else {
                            CB->animation("SP03MAR81");
                        }
                    }
                }
            }
        }
        /* 0x9CEB */
        if (ST(L24_THANANDAR_COMPANION) != 0) {
            CB->pos_perso(P_THANANDAR24, -1, -1, -1, 4, "P24ARR01", 8, 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
    /* 0x9D24 */
    if (ST(L24_ARRIVAL_ANIM_PAUSE) == 1 && ST(ST_LOCKED_CAMERA) != 0) {
        ST(L24_ARRIVAL_ANIM_PAUSE) = 2;
        CB->pause_anim(0, 7, -1);
    }
    /* 0x9D66 */
    if (ST(L24_ARRIVAL_ANIM_PAUSE) == 2 && ST(ST_LOCKED_CAMERA) == 0) {
        ST(L24_ARRIVAL_ANIM_PAUSE) = 1;
        CB->pause_anim(0, 7, 0);
    }
    /* 0x9DA8 */
    if (ST(L24_THANANDAR_ARRIVAL_STEP) == 1 && (ST(L24_HERO_NEAR_AT_ENTRY) == 0 || (ST(L24_HERO_NEAR_AT_ENTRY) == 1 && CB->distance(7, 1) > 250))) {
        ST(L24_THANANDAR_ARRIVAL_STEP) = 1000;
        CB->animation("Sar_10");
        CB->animation("SP24ARR01_10");
        CB->joindre(-P_THANANDAR24, 6090, -4130, 0x78, -1);
        CB->modify_var(&ST(L24_THANANDAR_ARRIVAL_STEP), 2);
    }
    /* 0x9E4E */
    if (ST(L24_THANANDAR_ARRIVAL_STEP) == 2 && CB->divers(DV_DESC_POS, P_THANANDAR24, 1) <= -4020) {
        ST(L24_THANANDAR_ARRIVAL_STEP) = 10;
        ST(L24_ARRIVAL_ANIM_PAUSE) = 1;
        CB->stop_joindre(-P_THANANDAR24);
        CB->evenement(38, 750);
        CB->animation("SP24ATT02");
        CB->animation("P24_65535");
        CB->joindre(-P_THANANDAR24, -1, -1, -1, ST(ST_HERO));
        CB->modify_var(&ST(L24_THANANDAR_ARRIVAL_STEP), 3);
    }
    /* 0x9F03 */
    if (ST(V_EVENT_37) == 1) {
        ST(V_EVENT_37) = 0;
        /* 0x9F24 */
        if (ST(L24_DOOR_ANIM_DONE) == 0) {
            ST(L24_DOOR_ANIM_DONE) = 1;
            /* 0x9F45 */
            if (ST(V_GUARD_FIGHT_DONE) != 2) {
                CB->play_anim(1, 0);
            }
            /* 0x9F64 */
            if (ST(V_GUARD_FIGHT_DONE) == 2) {
                CB->play_anim(2, 0);
            }
            CB->evenement(37, 1000);
        } else {
            ST(L24_ENGAGED_SAVED) = ST(V_ENGAGED_PERSONA);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            ST(ST_LEVEL) = 24;
            /* 0x9FD1 */
            if (ST(L24_ARKHAN23_STEP) >= 2 && ST(V_GUARD_FIGHT_DONE) < 2) {
                CB->divers_empile(DV_MUSIC, 1, 4);
            }
            /* 0xA007 */
            if (ST(V_ROUTE_STAGE) < 3) {
                CB->change_piece(23, 0);
            } else {
                CB->change_piece(23, 1);
            }
        }
        ST(V_EVENT_37) = 0;
    }
    /* 0xA04A */
    if (ST(V_EVENT_38) == 1) {
        CB->select_pile(2, 2);
        ST(V_EVENT_38) = 0;
        CB->play_anim(0, 0);
        CB->bruitage("L23B01", -1, -1);
        CB->select_pile(0, 1);
    }
    /* 0xA0A4 */
    if (ST(L24_THANANDAR_ARRIVAL_STEP) == 3 && (CB->distance(7, 8) <= 120 || CB->divers(DV_DESC_POS, P_THANANDAR24, 0) > 6200)) {
        ST(L24_THANANDAR_APPROACH_STEP) = 2;
        ST(L24_THANANDAR_ARRIVAL_STEP) = 4;
        CB->vire_col(0x10, 1);
        CB->ajoute_zone(4, 6090, -4020, 120, 90, 0);
        CB->retire_zone(1);
        CB->stop_joindre(-P_THANANDAR24);
        lieu_action_register("L24D07A", P_THANANDAR24, "SP24DIA59", "SP24ARR01_10");
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 24);
        lieu_action_register("L24D07B", P_THANANDAR24, "SP24DIA56", "SP24ARR01_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->place_animal(1, 6982, 0, -4152, 0, 150);
        CB->place_animal(1, 6694, 0, -4293, 5000, 600);
        /* 0xA1EC */
        if (ST(V_ENGAGED_PERSONA) == 0) {
            CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
        }
    }
    /* 0xA214 */
    if (ST(ST_VIE(P_THANANDAR24)) >= 2 && ST(L24_THANANDAR_FIGHT_STATE) == 1) {
        ST(L24_THANANDAR_FIGHT_STATE) = 2;
        ST(L24_THANANDAR_KILLED_MARK) = 5;
        /* 0xA256 */
        if (ST(L24_ARKHAN23_STEP) < 6) {
            ST(L24_THANANDAR_COMPANION) = 1;
        }
        /* 0xA277 */
        if (ST(ST_VIE(P_KONKALITE1)) != 1) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
    }
    /* 0xA295 */
    if (ST(ST_VIE(P_KONKALITE1)) == VIE_DEAD && ST(L24_KONKALITE_FIGHT_STATE) == 1) {
        ST(L24_KONKALITE_FIGHT_STATE) = 2;
        /* 0xA2C8 */
        if (ST(ST_VIE(P_THANANDAR24)) != 1) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
    }
    /* 0xA2E3 */
    if (ST(L24_THANANDAR_WEAPON_WARNED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_THANANDAR24) && CB->distance(7, 8) < 250 && ST(ST_VIE(P_THANANDAR24)) == 1 && ST(L24_THANANDAR_APPROACH_STEP) == 2) {
        /* 0xA362 */
        if (ST(L24_THANANDAR_WARN_STAGE) == 0) {
            ST(L24_THANANDAR_WEAPON_WARNED) = 1;
            ST(L24_THANANDAR_MOOD) = 2;
            ST(L24_THANANDAR_WARN_STAGE) = 1;
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_THANANDAR24);
            CB->animation("SP24GAR01_10");
            CB->dialogue("L24D08", P_THANANDAR24);
            /* 0xA3C9 */
            if (ST(V_ENGAGED_PERSONA) == 0) {
                CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
            }
            CB->bloque(BLOQUE_NONE);
            /* 0xA3F6 */
            if (ST(L24_THANANDAR_DIALOGUE_KIND) == 0) {
                ST(L24_THANANDAR_DIALOGUE_KIND) = 2;
            }
            return;
        }
        /* 0xA419 */
        if (ST(L24_THANANDAR_WARN_STAGE) >= 2) {
            ST(L24_THANANDAR_WEAPON_WARNED) = 1;
            ST(L24_THANANDAR_MOOD) = 2;
            CB->stop_joindre(-P_THANANDAR24);
            CB->animation("SP24GAR01_10");
            /* 0xA456 */
            if (ST(V_ENGAGED_PERSONA) == 0) {
                CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
            }
        }
    }
    /* 0xA47B */
    if (ST(L24_THANANDAR_WEAPON_WARNED) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_THANANDAR24)) == 1 && CB->direction_perso(P_THANANDAR24) && CB->distance(7, 8) < 300) {
        /* 0xA4EB */
        if (ST(L24_THANANDAR_WARN_STAGE) == 0) {
            ST(L24_THANANDAR_WEAPON_WARNED) = 0;
        }
        /* 0xA509 */
        if (ST(L24_THANANDAR_WARN_STAGE) == 1) {
            CB->combat(P_THANANDAR24, -1);
            ST(V_ENGAGED_PERSONA) &= 0xFF;
            ST(L24_THANANDAR_WEAPON_WARNED) = 0;
            ST(L24_THANANDAR_MOOD) = 2;
            ST(L24_THANANDAR_WARN_STAGE) = 2;
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_THANANDAR24);
            lieu_action_register("L24D09", P_THANANDAR24, "SP24GAR01", "SP24ARR01_10");
            /* 0xA58A */
            if (ST(V_ENGAGED_PERSONA) == 0) {
                CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA5D0 */
        if (ST(L24_THANANDAR_WARN_STAGE) >= 2) {
            ST(L24_THANANDAR_WEAPON_WARNED) = 0;
            ST(L24_THANANDAR_MOOD) = 2;
            CB->stop_joindre(-P_THANANDAR24);
            CB->animation("SP24ARR01_100");
            /* 0xA60D */
            if (ST(V_ENGAGED_PERSONA) == 0) {
                CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
            }
        }
    }
    /* 0xA635 */
    if (ST(L24_ARKHAN23_STEP) >= 4 && ST(ST_VIE(P_THANANDAR24)) == 1 && ST(V_ENGAGED_PERSONA) == 0) {
        /* 0xA665 */
        if (ST(L24_THANANDAR_ESCORT_STATE) != 3) {
            /* 0xA688 */
            if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) >= 6760 && CB->distance(7, 8) < 195 && ST(L24_THANANDAR_APPROACH_STEP) == 0) {
                ST(L24_THANANDAR_APPROACH_STEP) = 1;
            }
            /* 0xA6D9 */
            if (CB->distance(7, 8) > 250 && ST(L24_THANANDAR_APPROACH_STEP) == 2) {
                ST(L24_THANANDAR_APPROACH_STEP) = 3;
            }
        }
        /* 0xA703 */
        if (ST(L24_THANANDAR_APPROACH_STEP) == 1 && ST(L24_THANANDAR_TALK_ABORTED) == 0) {
            /* 0xA727 */
            if (ST(L24_THANANDAR_MET) == 0) {
                ST(L24_THANANDAR_MET) = 1;
            }
            ST(L24_THANANDAR_APPROACH_STEP) = 2;
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 24);
            CB->joindre(-P_THANANDAR24, 0, 0, 0, ST(ST_HERO));
            /* 0xA78E */
            if (ST(L24_THANANDAR_GREETED) == 0) {
                ST(L24_THANANDAR_GREETED) = 1;
            }
            /* 0xA7AF */
            if (ST(L24_THANANDAR_VISIT_COUNT) >= 2) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                CB->animation("SP24ARR01_10");
                /* 0xA7E0 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                ST(ST_SCRIPT_BUSY) = 0;
            }
            /* 0xA81C */
            if (ST(L24_THANANDAR_PROVOKED) == 0 && ST(L24_THANANDAR_VISIT_COUNT) < 2 && ST(L24_THANANDAR_FIGHT_STATE) != 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
                ST(ST_SCRIPT_BUSY) = 1;
                CB->bloque(BLOQUE_ALL);
                /* 0xA887 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L24D10", P_ARKHAN2);
                }
                /* 0xA8A9 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L24D11", P_ARKHAN3);
                }
                /* 0xA8C8 */
                if (ST(L24_THANANDAR_CHAT_COUNT) == 0) {
                    ST(L24_THANANDAR_ESCORT_STATE) = 1;
                    /* 0xA8E6 */
                    if (ST(L24_THANANDAR_PROVOKED) == 0 && ST(L24_THANANDAR_VISIT_COUNT) == 0) {
                        lieu_action_register("L24D12", P_THANANDAR24, "SP24DIA55", "SP24ARR01_10");
                    }
                    /* 0xA923 */
                    if (ST(L24_THANANDAR_PROVOKED) == 0 && ST(L24_THANANDAR_VISIT_COUNT) != 0) {
                        lieu_action_register("L24D13", P_THANANDAR24, "SP24DIA60", "SP24ARR01_10");
                    }
                    CB->modify_var(&ST(L24_THANANDAR_CHAT_COUNT), 1);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    /* 0xA98A */
                    if (ST(V_ENGAGED_PERSONA) == 0) {
                        CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
                    }
                    return;
                }
                /* 0xA9B4 */
                if (ST(L24_THANANDAR_CHAT_COUNT) == 1) {
                    lieu_action_register("L24D14A", P_THANANDAR24, "SP24DIA59", "SP24ARR01_10");
                    lieu_action_register("L24D14B", P_THANANDAR24, "SP24ATT50", "SP24ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    /* 0xAA0E */
                    if (ST(V_ENGAGED_PERSONA) == 0) {
                        CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
                    }
                    return;
                }
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
            }
            /* 0xAA52 */
            if (ST(L24_THANANDAR_PROVOKED) == 1 || ST(L24_THANANDAR_VISIT_COUNT) >= 2 || (ST(L24_THANANDAR_FIGHT_STATE) == 1 && ST(ST_COMBAT_STANCE) == 0)) {
                CB->bloque(BLOQUE_ALL);
                ST(L24_THANANDAR_FIGHT_STATE) = 1;
                /* 0xAAAE */
                if (ST(L24_THANANDAR_DIALOGUE_KIND) == 0) {
                    ST(L24_THANANDAR_DIALOGUE_KIND) = 1;
                }
                ST(L24_KONKALITE_FIGHT_STATE) = 1;
                /* 0xAADB */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L24D10", P_ARKHAN2);
                }
                /* 0xAAFD */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L24D11", P_ARKHAN3);
                }
                CB->joindre(-P_KONKALITE1, 6647, -4360, 0xCF08, -1);
                lieu_action_register("L24D15A", P_THANANDAR24, "SP24DIA56", "SP24ARR01_10");
                lieu_action_register("L24D15B", P_THANANDAR24, "SP24DIA60", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(V_ENGAGED_PERSONA) |= 0x2471;
                CB->combat(P_THANANDAR24, 0);
                CB->combat(P_KONKALITE1, 0);
            }
        }
        /* 0xABA9 */
        if (ST(L24_THANANDAR_APPROACH_STEP) == 3 && ST(V_ENGAGED_PERSONA) == 0 && ST(L24_THANANDAR_TALK_ABORTED) == 0) {
            /* 0xABD9 */
            if (ST(L24_THANANDAR_ESCORT_STATE) == 1) {
                ST(L24_THANANDAR_ESCORT_STATE) = 3;
                CB->bloque(BLOQUE_ALL);
                ST(L24_THANANDAR_MOOD) = 4;
                CB->stop_joindre(-P_THANANDAR24);
                lieu_action_register("L24D20", P_THANANDAR24, "SP24DIA60", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                /* 0xAC48 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
                }
                return;
            }
            /* 0xAC72 */
            if (ST(L24_THANANDAR_ESCORT_STATE) == 0) {
                ST(L24_THANANDAR_APPROACH_STEP) = 0;
                ST(L24_THANANDAR_CHAT_COUNT) = 1;
                CB->stop_joindre(-P_THANANDAR24);
                /* 0xACA7 */
                if (ST(L24_ARKHAN23_STEP) == 4) {
                    CB->joindre(-P_THANANDAR24, 6844, -4093, 0xBB8, -1);
                    CB->animation("SP24ACT01_10");
                }
                /* 0xACE0 */
                if (ST(L24_ARKHAN23_STEP) == 5 || ST(L24_ARKHAN23_STEP) == 6) {
                    CB->joindre(-P_THANANDAR24, 6830, -4113, 0x4268, -1);
                    CB->animation("SP24ACT02_10");
                }
            }
        }
        /* 0xAD28 */
        if (ST(L24_THANANDAR_APPROACH_STEP) == 3 && ST(V_ENGAGED_PERSONA) == 0 && ST(L24_THANANDAR_TALK_ABORTED) != 0 && CB->distance(8, 7) > 250) {
            ST(L24_THANANDAR_TALK_ABORTED) = 0;
            ST(L24_THANANDAR_APPROACH_STEP) = 0;
            ST(L24_THANANDAR_CHAT_COUNT) = 1;
            CB->stop_joindre(-P_THANANDAR24);
            /* 0xADA8 */
            if (ST(L24_ARKHAN23_STEP) == 4) {
                CB->joindre(-P_THANANDAR24, 6844, -4093, 0xBB8, -1);
                CB->animation("SP24ACT01_10");
            }
            /* 0xADE1 */
            if (ST(L24_ARKHAN23_STEP) == 5 || ST(L24_ARKHAN23_STEP) == 6) {
                CB->joindre(-P_THANANDAR24, 6830, -4113, 0x4268, -1);
                CB->animation("SP24ACT02_10");
            }
        }
    }
    /* 0xAE2C */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->joindre(-ST(ST_HERO), 0, 0, 0xA, -1);
        CB->joindre(-P_THANANDAR24, 0, 0, 0xA, -1);
        CB->stop_joindre(-P_THANANDAR24);
        lieu_action_register("L24D17", P_THANANDAR24, "SP24DIA53", "SP24ATT01_10");
        CB->joindre(-P_THANANDAR24, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L24D18A", P_THANANDAR24, "SP24DIA59", "SP24ARR01_10");
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 24);
        lieu_action_register("L24D18B", P_THANANDAR24, "SP24DIA62", "SP24ARR01_10");
        lieu_action_register("L24D18C", P_THANANDAR24, "SP24DIA65", "SP24ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        /* 0xAF47 */
        if (ST(V_ENGAGED_PERSONA) == 0) {
            CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
        }
        return;
    }
    /* 0xAF74 */
    if (ST(L24_THANANDAR_WALK_BACK) == 1) {
        i32 t1 = CB->divers(DV_DESC_POS, ST(ST_HERO), 1);
        /* 0xAFA8 */
        if (t1 >= CB->divers(DV_DESC_POS, P_THANANDAR24, 1)) {
            ST(L24_THANANDAR_WALK_BACK) = 2;
            CB->stop_joindre(-P_THANANDAR24);
            CB->joindre(-ST(ST_HERO), 0, 0, 0xC738, -1);
            /* 0xAFF4 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xB006 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xB020 */
                    if (CB->divers(DV_CAN_FIGHT, 8, 2) || CB->divers(DV_CAN_FIGHT, 4, 2) || CB->divers(DV_CAN_FIGHT, 5, 2)) {
                        CB->animation("SP02MAR30");
                    }
                    /* 0xB079 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02MAR31");
                    } else {
                        CB->animation("SP02MAR29");
                    }
                }
                /* 0xB0A6 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xB0C0 */
                    if (CB->divers(DV_CAN_FIGHT, 8, 2) || CB->divers(DV_CAN_FIGHT, 4, 2) || CB->divers(DV_CAN_FIGHT, 5, 2)) {
                        CB->animation("SP03MAR30");
                    }
                    /* 0xB119 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03MAR31");
                    } else {
                        CB->animation("SP03MAR29");
                    }
                }
            }
            /* 0xB146 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xB158 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xB172 */
                    if (CB->divers(DV_CAN_FIGHT, 8, 2) || CB->divers(DV_CAN_FIGHT, 4, 2) || CB->divers(DV_CAN_FIGHT, 5, 2)) {
                        CB->animation("SP02MAR82");
                    }
                    /* 0xB1CB */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02MAR83");
                    } else {
                        CB->animation("SP02MAR81");
                    }
                }
                /* 0xB1F8 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xB212 */
                    if (CB->divers(DV_CAN_FIGHT, 8, 2) || CB->divers(DV_CAN_FIGHT, 4, 2) || CB->divers(DV_CAN_FIGHT, 5, 2)) {
                        CB->animation("SP03MAR82");
                    }
                    /* 0xB26B */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03MAR83");
                    } else {
                        CB->animation("SP03MAR81");
                    }
                }
            }
            CB->joindre(-P_THANANDAR24, 6085, -3990, 0, -1);
        }
    }
    /* 0xB2B1 */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        ST(L24_THANANDAR_MOOD) = 2;
        CB->stop_joindre(-P_THANANDAR24);
        CB->animation("Sar_10");
        /* 0xB2F4 */
        if (ST(ST_VIE(P_THANANDAR24)) < 2) {
            lieu_action_register("L24D19A", P_THANANDAR24, "SP24DIA60", "SP24ARR01");
            lieu_action_register("L24D19B", P_THANANDAR24, "SP24DIA54", "SP24ARR01_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        /* 0xB346 */
        if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_THANANDAR24)) < 2) {
            CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
        }
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xB38D */
    if (ST(V_GUARD_FIGHT_DONE) < 2 && ST(L24_THANANDAR_FIGHT_STATE) == 2 && CB->distance(7, 34) < 200 && CB->direction_zone(34) && ST(L24_ZONE34_ONCE) == 0 && ST(L24_ZONE34_DONE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(L24_ZONE34_ONCE) = 1;
        ST(ST_SCRIPT_BUSY) = 1;
        ST(L24_ZONE34_DONE) = 1;
        /* 0xB438 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L24D93", P_ARKHAN2, "SG00DIA04", "Sar_10");
        }
        /* 0xB463 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L24D94", P_ARKHAN3, "SG00DIA04", "Sar_10");
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->retire_zone(34);
        CB->bloque(BLOQUE_NONE);
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x10011359).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_10021D1C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu24_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x10011582);
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

    if (ST(ST_HERO) != 4 || ST(V_CHASE_STATE) == 1) {
        l24_trig00();   /* 0x10011589: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1001146E - the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x100115DC): drain pending sound, run body.
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x10010FC0).
 * Jump table @ 0x10011659, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu24_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu24: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        l24_reset_dll_globals();
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l24_trigger(arg);             break;  /* dword_10021BB0[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
