/*
 * lieu_common.c — script code every level DLL carries unchanged: the
 * dialogue helper action_register, the greeting scene lieu_pre_scene
 * (LIEU02 sub_1001A35C), event4 (sub_1000123A), event5
 * (sub_100022B4), event6 (sub_10005BEC) and the float accumulators they
 * use. tools/lieu_annotate.py lists, per DLL, which functions are
 * byte-identical to these (modulo relocations); a level whose copy differs
 * keeps its own port.
 *
 * Ported from LIEU01.DLL (LIEU02 is instruction-for-instruction the same).
 */

#include <stdio.h>

#include "lieu_level.h"

/* -------------------------------------------------------------------------
 * Helper: action_register  (sub_1001932C, alias thunk sub_1000100A)
 *
 * Original __fastcall(ecx=name, edx=count) + stdcall stack (arg0, arg4).
 *   named_object(arg0)
 *   named_object_int(name, count)
 *   if      (count < 4 && arg4 == NULL) named_object("Sar_20")
 *   else if (arg4 != NULL)              named_object(arg4)
 *   else { sprintf(b,"SP%02darr01_10",count); named_object(b) }
 * ---------------------------------------------------------------------- */
void lieu_action_register(const c8 *name, i32 count,
                            const c8 *arg0, const c8 *arg4) {
    CB->animation(arg0);
    CB->dialogue(name, count);
    if (count < 4 && arg4 == NULL) {
        CB->animation("Sar_20");
    } else if (arg4 != NULL) {
        CB->animation(arg4);
    } else {
        c8 b[64];
        snprintf(b, sizeof(b), "SP%02darr01_10", count);
        CB->animation(b);
    }
}

/* -------------------------------------------------------------------------
 * event6 — sub_10005BEC.  Switch on arg (0..4): LCOD zone triggers.
 * Fully ported.
 * ---------------------------------------------------------------------- */
void lieu_event6(i32 arg) {
    switch (arg) {
    case EV6_LCOD34:  /* loc_10005C0A */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_COMBAT_STANCE) == 0 && ST(V_LCOD34_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD34_SHOWN) = 1;
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD34", 2, "SG00CON01", NULL);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD35:  /* loc_10005C87 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(V_LCOD35_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD35_SHOWN) = 1;
            ST(V_LCOD37_SHOWN) = 0;
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD35", 2, "SG00CON01", NULL);
            CB->animation("Sar_50");
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->animation("SG00CON01_1500");
            CB->animation("ar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD36:  /* loc_10005D34 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_COMBAT_STANCE) == 0 && ST(V_LCOD36_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD36_SHOWN) = 1;
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD36", 3, "SG00CON01", "SG00CON01");
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD37:  /* loc_10005DBF */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(V_LCOD37_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD35_SHOWN) = 0;
            ST(V_LCOD36_SHOWN) = 0;
            ST(V_LCOD37_SHOWN) = 1;
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD37", 3, "SG00CON01", NULL);
            CB->animation("Sar_50");
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_SC21_ENDING:  /* loc_10005E70 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(0x318) == 0) {
            CB->select_pile(2, 2);
            CB->select_pile(1, 2);
            CB->select_pile(0, 2);
            ST(0x318) = 1;
            CB->divers(DV_CUT_SPEECH, 0, 0);
            CB->evenement(0, -1);
            CB->evenement(1, -1);
            CB->evenement(2, -1);
            CB->evenement(0x21, -1);
            CB->evenement(0x22, -1);
            CB->evenement(0x23, -1);
            CB->evenement(0x24, -1);
            CB->evenement(0x25, -1);
            CB->evenement(0x26, -1);
            CB->evenement(0x27, -1);
            CB->evenement(0x29, -1);
            CB->bloque(BLOQUE_ALL);
            CB->animation("Sar_50");
            if (ST(ST_COMBAT_STANCE) == 1)
                CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
            CB->divers_empile(DV_SCREEN, 5, 0);
            CB->divers_empile(DV_HP, P_ARKHAN3, 0);
            CB->animation("Sar_50");
            CB->cinematique("SC21_500");
            CB->divers_empile(DV_SCREEN, 3, 0);
            CB->divers_empile(DV_END_GAME, 0, 0);
        }
        break;
    default:
        break;
    }
}

/* Float FSM accumulators — 10-element array at dword_10024A30.
 * Cases 90-93 read/write indices 0-3; sub_1000105F resets all 10 to 1.0f. */
static float g_float_accum[10] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                   1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static i32 g_float_active = 1;   /* dword_10024A58 gate flag */

static void e4_reset_floats(void) {
    for (i32 i = 0; i < 10; i++) g_float_accum[i] = 1.0f;
}
static void e5_clamp6(i32 k);  /* forward decl — defined in event5 section */

/*
 * event4 — sub_1000123A (4 KB). switch(arg) over ~99 script-event ids;
 * mostly simple state-machine setters. `break` == original jmp default.
 * ALL cases ported.
 */
void lieu_event4(i32 arg) {        /* sub_1000123A */
    switch (arg) {
    case 0:  ST(V_EVENT_0) = 1; break;
    case 1:  ST(V_EVENT_1) = 1; break;
    case 2:  ST(V_EVENT_2) = 1; break;

    case 7:  /* 0x33C increment chain 1->2->3->4->5 */
        if      (ST(V_EVENT_7) == 1) ST(V_EVENT_7) = 2;
        else if (ST(V_EVENT_7) == 2) ST(V_EVENT_7) = 3;
        else if (ST(V_EVENT_7) == 3) ST(V_EVENT_7) = 4;
        else if (ST(V_EVENT_7) == 4) ST(V_EVENT_7) = 5;
        break;

    case 8:  if (ST(V_EVENT_8) == 1) ST(V_EVENT_8) = 2; break;

    case 9:
        if (ST(ST_VIE(P_BOGDARAN)) != VIE_DEAD && ST(V_EVENT_9) == 1) {
            ST(0xB80) = 0;
            ST(V_EVENT_9) = 0;
        }
        break;

    case 11: if (ST(V_EVENT_11) == 1) ST(V_EVENT_11) = 2; break;
    case 12: if (ST(V_EVENT_12) == 1) ST(V_EVENT_12) = 2; break;

    case 15: /* 0x8C increment chain (odd->even+1, 0->1..0xC->0xD) */
        if      (ST(V_EVENT15_STEP) == 0)   ST(V_EVENT15_STEP) = 1;
        else if (ST(V_EVENT15_STEP) == 2)   ST(V_EVENT15_STEP) = 3;
        else if (ST(V_EVENT15_STEP) == 4)   ST(V_EVENT15_STEP) = 5;
        else if (ST(V_EVENT15_STEP) == 6)   ST(V_EVENT15_STEP) = 7;
        else if (ST(V_EVENT15_STEP) == 8)   ST(V_EVENT15_STEP) = 9;
        else if (ST(V_EVENT15_STEP) == 0xA) ST(V_EVENT15_STEP) = 0xB;
        else if (ST(V_EVENT15_STEP) == 0xC) ST(V_EVENT15_STEP) = 0xD;
        break;

    case 16:
        if (ST(V_EVENT_16) == 0) {
            ST(V_EVENT_16) = 1;
            CB->free_anim(1);
            CB->play_anim(2, 0);
            if (ST(0xB40) == 1) {
                ST(0xB40) = 2;
                CB->animation("Sar_500");
                CB->modify_var(&ST(0x68), 1);
            }
            if (ST(0xE30) == 1) {
                ST(0xE30) = 2;
                CB->animation("Sar_500");
                CB->modify_var(&ST(0xE0), 1);
            }
        }
        break;

    case 17: if (ST(V_EVENT_17) == 1) ST(V_EVENT_17) = 2; break;

    case EV_HUD_TIMER:
        if ((i32)ST(V_HUD_TIMER_MS) > 0) {
            ST(V_HUD_TIMER_MS) -= 0x1388;
            CB->divers_empile(DV_HUD_REFRESH, 0, 0);
            CB->evenement(EV_HUD_TIMER, 5000);
        }
        break;

    case 19:
        if (ST(0xEBC) == 1 && ST(ST_LEVEL) != 28)
            ST(0xEBC) = 2;
        break;

    case 20:
        if (ST(ST_LEVEL) == 18)
            CB->bruitage("L08B06", 2740, 1983);
        if (ST(ST_LEVEL) == 8) {
            CB->select_pile(2, 1);
            CB->bruitage("L08B06", 44, -960);
            CB->select_pile(0, 1);
        }
        break;

    case 21: if (ST(V_EVENT_21) == 0) ST(V_EVENT_21) = 1; break;
    case 22: if (ST(V_EVENT_22) == 0) ST(V_EVENT_22) = 1; break;
    case 25: if (ST(V_EVENT_25) == 0) ST(V_EVENT_25) = 2; break;
    case 27: if (ST(V_EVENT_27) == 0) ST(V_EVENT_27) = 1; break;

    case EV_TO_ROOM_25:
        if (ST(V_EVENT_28) == 0) {
            CB->bloque(BLOQUE_NONE);
            ST(ST_LEVEL) = 23;
            CB->change_piece(25, -1);
        }
        break;

    case 29:
        if      (ST(V_EVENT_29) == 2) ST(V_EVENT_29) = 3;
        else if (ST(V_EVENT_29) == 0) ST(V_EVENT_29) = 1;
        break;

    case 30:
        if (ST(V_EVENT_30) == 1) {
            ST(V_EVENT_30) = 0;
            ST(0xBF4) = 1;
        }
        break;

    case 31: if (ST(V_EVENT_31) == 0) ST(V_EVENT_31) = 1; break;
    case 33: ST(V_EVENT_33) = 1; break;
    case 34: ST(V_EVENT_34) = 1; break;
    case 35: ST(V_EVENT_35) = 1; break;
    case 36: ST(V_EVENT_36) = 1; break;
    case 37: ST(V_EVENT_37) = 1; break;
    case 38: ST(V_EVENT_38) = 1; break;
    case 39: ST(V_EVENT_39) = 1; break;
    case 40: ST(V_EVENT_40) = 1; break;
    case 41: ST(V_EVENT_41) = 1; break;

    case EV_MUSIC_LOOP_A:
        if (ST(V_MUSIC_TRACK) != -1) {
            CB->select_pile(2, 1);
            CB->divers_empile(DV_MUSIC, ST(V_MUSIC_TRACK), 4);
            CB->evenement(EV_MUSIC_LOOP_B, 5000);
            CB->select_pile(0, 1);
        }
        break;

    case EV_MUSIC_LOOP_B:
        CB->select_pile(2, 1);
        CB->divers_empile(DV_MUSIC, ST(V_MUSIC_TRACK), 1);
        CB->evenement(EV_MUSIC_LOOP_A, 45000);
        CB->select_pile(0, 1);
        break;

    case EV_END_GAME: ST(V_END_GAME_REQ) = 1; break;

    case 51: /* 0x3EC chain 0->1,2->3,4->5 */
        if      (ST(V_EVENT_51) == 0) ST(V_EVENT_51) = 1;
        else if (ST(V_EVENT_51) == 2) ST(V_EVENT_51) = 3;
        else if (ST(V_EVENT_51) == 4) ST(V_EVENT_51) = 5;
        break;
    case 52: /* 0x3F0 chain 0->1,2->3 */
        if      (ST(V_EVENT_52) == 0) ST(V_EVENT_52) = 1;
        else if (ST(V_EVENT_52) == 2) ST(V_EVENT_52) = 3;
        break;
    case 53: /* 0x3F4 chain 0->1,2->3,4->5 */
        if      (ST(V_EVENT_53) == 0) ST(V_EVENT_53) = 1;
        else if (ST(V_EVENT_53) == 2) ST(V_EVENT_53) = 3;
        else if (ST(V_EVENT_53) == 4) ST(V_EVENT_53) = 5;
        break;

    case 70:  /* alarm bells (named_two_ints F/G/H at fixed values) */
        CB->select_pile(2, 1); CB->evenement(0x46, 8500); CB->select_pile(0, 1);
        break;
    case 71:
        CB->select_pile(2, 1); CB->evenement(0x47, 2500);  CB->select_pile(0, 1);
        break;
    case 72:
        CB->select_pile(2, 1); CB->evenement(0x48, 18500); CB->select_pile(0, 1);
        break;
    case 73:
        CB->select_pile(2, 1); CB->evenement(0x46, 10500); CB->select_pile(0, 1);
        break;
    case 74:
        CB->select_pile(2, 1); CB->evenement(0x46, 30000); CB->select_pile(0, 1);
        break;

    case 80:
        CB->select_pile(0, 2);
        CB->evenement(0x51, 2500);
        CB->animation("SP01ACT99");
        CB->select_pile(0, 1);
        break;

    case EV_PIPE_SCENE:  /* pipe puzzle SP02PIPI1/2 by layer query */
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        if (CB->divers(DV_CAN_FIGHT, 9, 2))
            CB->animation("SP02PIPI2");
        else
            CB->animation("SP02PIPI1");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        break;

    case EV_GOURD_TO_FLASK: {  /* SOB24%c/SOB77%c via named_ex_988 */
        CB->divers(DV_TRANSITION, 0x7D0, 0x7D0);
        CB->animation("SP01ACT96");
        CB->animation("ar");
        c8 buf1[48], buf2[48];
        snprintf(buf1, sizeof(buf1), "SOB24%c", (char)(ST(ST_USED_VARIANT) + 0x41));
        CB->etat_objet(buf1, -1, -1, -1, -1, -1, -1, -2, 0);
        snprintf(buf2, sizeof(buf2), "SOB77%c", (char)(ST(ST_USED_VARIANT) + 0x41));
        CB->etat_objet(buf2, -1, -1, -1, -1, -1, -1, -3, 0);
        CB->bloque(BLOQUE_NONE);
        break;
    }
    case 81: {  /* P01ACT97 + layer-6 clamp(0x1E) */
        CB->select_pile(0, 2);
        CB->animation("P01ACT97");
        CB->evenement(EV_GOURD_TO_FLASK, 800);
        e5_clamp6(0x1E);
        CB->select_pile(0, 1);
        break;
    }
    case EV_FLOAT_GROW_0:  /* float accum grow (index 0) */
        CB->select_pile(2, 1);
        if (g_float_accum[0] < 3.0f && g_float_active != 0) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x10011);
            CB->evenement(EV_FLOAT_GROW_0, 100);
            g_float_accum[0] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case EV_FLOAT_GROW_1:  /* float accum grow (index 1, dual layer_op) */
        CB->select_pile(2, 1);
        if (g_float_accum[1] < 3.0f && g_float_active != 0) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x10005);
            CB->divers(DV_SCALE_BONE, 0x66, 0x10007);
            CB->evenement(EV_FLOAT_GROW_1, 100);
            g_float_accum[1] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case EV_FLOAT_GROW_2:  /* float accum grow (index 2, dual layer_op) */
        CB->select_pile(2, 1);
        if (g_float_accum[2] < 3.0f && g_float_active != 0) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x1000E);
            CB->divers(DV_SCALE_BONE, 0x66, 0x10010);
            CB->evenement(EV_FLOAT_GROW_2, 100);
            g_float_accum[2] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case EV_FLOAT_DECAY:  /* float accum decay (index 3) */
        CB->select_pile(2, 1);
        if (g_float_accum[3] > 0.5f && g_float_active != 0) {
            CB->divers(DV_SCALE_BODY, 0x62, 0);
            CB->evenement(EV_FLOAT_DECAY, 100);
            g_float_accum[3] *= 0.98f;
        }
        CB->select_pile(0, 1);
        break;
    case EV_RESPAWN_TOGGLE: {  /* float reset + respawn toggle */
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        if (g_float_active != 0) {
            g_float_active = 0;
            e4_reset_floats();
            i32 c2 = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
            i32 c1 = CB->divers(DV_DESC_POS, ST(ST_HERO), 1);
            i32 c0 = CB->divers(DV_DESC_POS, ST(ST_HERO), 0);
            CB->pos_perso(ST(ST_HERO) + 100, c0, c1, c2, 1,
                          "ar", ST(0x138), 0);
            CB->animation("Sar_50");
            CB->evenement(EV_RESPAWN_TOGGLE, 100);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
        } else {
            g_float_active = 1;
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
        }
        break;
    }

    /* Original default cases (5,13,23,26,32,45-49,54-69,75-79,82-89,
     * 95-97) plus 3,4,6,10,14,24 are no-ops. */
    default:
        break;
    }
}
/*
 * event5 — sub_100022B4 (14 KB). switch(arg) over LCOD player-actions
 * (arg 1..99; original uses a byte index table -> jump table @ 0x5AAD).
 * Each case is independent. `break` == original `jmp def_10005AAD`.
 *
 * ALL cases ported. Default catches genuine no-ops from the original.
 */

/* Common case prologue (appears in nearly every case). */
static void e5_reset(void) {
    CB->select_pile(0, 2);
    CB->divers_empile(DV_RANDOM, 0, 0);
    CB->divers_empile(DV_RANDOM, 0, 0);
    CB->divers_empile(DV_RANDOM, 0, 0);
    CB->bloque(BLOQUE_ALL);
}

/* case 4 / 22 / 65 template: SG01ATT01 + LCOD41/42/43 + ar_50 */
static void e5_att01(void) {
    e5_reset();
    CB->animation("SG01ATT01");
    if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
    if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
    if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
    CB->animation("ar_50");
    CB->bloque(BLOQUE_NONE);
}

/* Common tail: layer-6 value += k, clamped to 0x3FF (op 6 on [0x194]). */
static void e5_clamp6(i32 k) {
    if (CB->divers(DV_HP, ST(ST_HERO), -1) + k < 0x400)
        CB->divers_empile(DV_HP, ST(ST_HERO), CB->divers(6, ST(ST_HERO), -1) + k);
    else
        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
}

/* SGxxMAN03/04 (levels 2,3) by [0x198], + ar_50 + clamp(k). */
static void e5_man34(i32 k) {
    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG02MAN03");
        if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG02MAN04");
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG03MAN03");
        if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG03MAN04");
    }
    CB->animation("ar_50");
    e5_clamp6(k);
}

/* SG01ATT01 + LCOD pair (lvl2->a, lvl3->b) + ar_50. */
static void e5_att01_lcod(const c8 *a, const c8 *b) {
    CB->animation("SG01ATT01");
    if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue(a, ST(ST_HERO));
    if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue(b, ST(ST_HERO));
    CB->animation("ar_50");
}

void lieu_event5(i32 arg, i32 phase) { /* sub_100022B4(ecx=arg, edx=phase) */
    switch (arg) {

    case OB_DISH:   /* 0x22E1 */
    case OB_FLAGON22:  /* 0x23C1 */
    case OB_BOWL:  /* 0x2C0A */
        e5_att01();
        break;

    case OB_THBOLT_BEF:  /* 0x24A1 (shared 23/27/35/55) */
    case OB_F_THROWER:
    case OB_ZED_THBOLT35:
    case OB_SPITROD:
        if (ST(ST_LEVEL) == 35) {
            e5_reset();
            CB->animation("SG00ATT01");
            CB->bloque(BLOQUE_NONE);
        }
        break;

    case OB_FLAGON42:  /* 0x250D */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_SORDOS_DOC:  /* 0x25ED — Sar imp interaction (LCOD02/03 A-H) */
        if (ST(V_LCOD02_SEEN) == 0 || ST(V_LCOD02_SEEN) == 1) {
            e5_reset();
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD02A", 2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02B", 2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02C", 2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02D", 2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02E", 2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02F", 2, "SP02ACT64", "SP02ACT64");
                if (ST(V_LCOD02_SEEN) == 0) {
                    lieu_action_register("LCOD02G", 2, "SP02ACT64", "SP02ACT64");
                    lieu_action_register("LCOD02H", 2, "SP02ACT64", "SP02ACT64");
                }
                ST(V_LCOD02_SEEN) = 1;
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD03A", 3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03B", 3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03C", 3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03D", 3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03E", 3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03F", 3, "SP03ACT64", "SP03ACT64");
                if (ST(V_LCOD02_SEEN) == 0) {
                    lieu_action_register("LCOD03G", 3, "SP03ACT64", "SP03ACT64");
                    lieu_action_register("LCOD03H", 3, "SP03ACT64", "SP03ACT64");
                }
                ST(V_LCOD02_SEEN) = 1;
            }
            if (ST(ST_LEVEL) == 2 && ST(ST_LOCKED_CAMERA) == 1 && ST(0xEB8) == 1
                && CB->divers(DV_PERSONA_PRESENT, P_PHEDORIA, 0)) {
                CB->divers(DV_COMBAT_STANCE, 0, 0);
                if (CB->distance(20, 31) < 0x96)
                    CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x1A);
                else
                    CB->joindre(-ST(ST_HERO), -1, -1, -1, 0x1A);
                if (CB->distance(20, 31) >= 0x96)
                    CB->evenement(0x29, 2000);
                else if (CB->distance(20, 31) < 0x96 && CB->direction_perso(P_PHEDORIA) == 0)
                    CB->evenement(0x29, 2000);
                else
                    CB->evenement(0x29, 500);
            } else {
                CB->animation("Sar_50");
                CB->bloque(BLOQUE_NONE);
            }
        } else {
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;

    case OB_DIAG_WEAPON:  /* 0x29A5 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) lieu_action_register("LCOD88", 2, "SG00DIA02", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) lieu_action_register("LCOD89", 3, "SG00DIA02", NULL);
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_RYLS_NOTES:  /* 0x2A47 — LCOD100/101/102/103/104 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD100A", 2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD100B", P_ARKHAN2);
            CB->dialogue("LCOD100C", P_ARKHAN2);
            CB->dialogue("LCOD100D", P_ARKHAN2);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD101A", 3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD101B", P_ARKHAN3);
            CB->dialogue("LCOD101C", P_ARKHAN3);
            CB->dialogue("LCOD101D", P_ARKHAN3);
        }
        CB->dialogue("LCOD102A", P_ARKHAN2);
        CB->dialogue("LCOD102B", P_ARKHAN2);
        CB->dialogue("LCOD102C", P_ARKHAN2);
        CB->dialogue("LCOD102D", P_ARKHAN2);
        CB->dialogue("LCOD102E", P_ARKHAN2);
        CB->dialogue("LCOD102F", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN2) lieu_action_register("LCOD103", 2, "SG02DIA01", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) lieu_action_register("LCOD104", 3, "SG03DIA01", NULL);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_BANDOR_DIAG:  /* 0x2CEA */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) lieu_action_register("LCOD90", 2, "SG00DIA02", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) lieu_action_register("LCOD91", 3, "SG00DIA02", NULL);
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_VEGETABLE:  /* 0x39E1 — Sar mannequin dialogue + layer clamp */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG02MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG02MAN02");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG03MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG03MAN02");
        }
        CB->animation("ar_50");
        e5_clamp6(0x64);
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_FLASK77:  /* 0x3575 — SPxxACT17 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT17");
        if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ACT17");
        if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ACT17");
        CB->animation("ar_50");
        if (ST(ST_HERO) == P_ARKHAN1) lieu_action_register("LCOD41", ST(ST_HERO), "SG01ATT01", NULL);
        if (ST(ST_HERO) == P_ARKHAN2) lieu_action_register("LCOD42", ST(ST_HERO), "SG01ATT01", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) lieu_action_register("LCOD43", ST(ST_HERO), "SG01ATT01", NULL);
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_PARCHMENT:  /* 0x36B3 — LCOD23/24 A-G */
        e5_reset();
        if (ST(V_LCOD23_SEEN) == 0) ST(V_LCOD23_SEEN) = 1;
        if (ST(ST_LEVEL) == 28) ST(0x84) = 1;
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD23A", 2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD23B", P_ARKHAN2);
            CB->dialogue("LCOD23C", P_ARKHAN2);
            CB->dialogue("LCOD23D", P_ARKHAN2);
            CB->dialogue("LCOD23E", P_ARKHAN2);
            CB->dialogue("LCOD23F", P_ARKHAN2);
            lieu_action_register("LCOD23G", 2, "SG02DIA01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD24A", 3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD24B", P_ARKHAN3);
            CB->dialogue("LCOD24C", P_ARKHAN3);
            CB->dialogue("LCOD24D", P_ARKHAN3);
            CB->dialogue("LCOD24E", P_ARKHAN3);
            CB->dialogue("LCOD24F", P_ARKHAN3);
            lieu_action_register("LCOD24G", 3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_HELMET:  /* 0x3871 */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD672", ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD673", ST(ST_HERO));
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_BREASTPLATE:  /* 0x3929 */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD674", ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD675", ST(ST_HERO));
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_POTATO:  /* 0x44E2 — SGxxMAN01/02 + clamp 0x4B */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG02MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG02MAN02");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG03MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG03MAN02");
        }
        CB->animation("ar_50");
        e5_clamp6(0x4B);
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_ROOT:  /* 0x464F — SGxxMAN03/04 + clamp 0x4B */
        e5_reset(); e5_man34(0x4B); CB->bloque(BLOQUE_NONE);
        break;

    case OB_FISH:  /* 0x47BC — SGxxMAN03/04 + clamp 0x64 */
        e5_reset(); e5_man34(0x64); CB->bloque(BLOQUE_NONE);
        break;

    case OB_RAT:  /* 0x4B32 — MAN03/04 + clamp 0x4B */
        e5_reset(); e5_man34(0x4B); CB->bloque(BLOQUE_NONE);
        break;

    case OB_ROAST_BIRD:  /* 0x4C9F — MAN03/04 + clamp 0x64 */
        e5_reset(); e5_man34(0x64); CB->bloque(BLOQUE_NONE);
        break;

    case OB_MORTAR:  /* 0x4E0C — MAN03/04 + clamp 0x96 */
        e5_reset(); e5_man34(0x96); CB->bloque(BLOQUE_NONE);
        break;

    case OB_MEAT:  /* 0x4929 — SGxxMAN (levels 0-3) + clamp 0x96 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG00MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG00MAN02");
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG01MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG01MAN02");
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG02MAN03");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG02MAN04");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG03MAN03");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG03MAN04");
        }
        CB->animation("ar_50");
        e5_clamp6(0x96);
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_UNIFORM:   /* 0x5161 — SG01ATT01 + LCOD26/27 */
        e5_reset(); e5_att01_lcod("LCOD26", "LCOD27"); CB->bloque(BLOQUE_NONE);
        break;

    case OB_BOW_TO_KEY:   /* 0x5219 (shared 2/12/18/19) — SG01ATT01 + L25D79/82 */
    case OB_CRYSTAL_2:
    case OB_CRYSTAL_1:
    case OB_CRYSTAL_3:
        e5_reset(); e5_att01_lcod("L25D79", "L25D82"); CB->bloque(BLOQUE_NONE);
        break;

    case OB_METAL_PLATE:   /* 0x52D1 — state_toggle(1,1) + SG01ATT01 + LCOD39/40 */
        CB->select_pile(1, 1);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->bloque(BLOQUE_ALL);
        e5_att01_lcod("LCOD39", "LCOD40");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x538C — big shared group: SG01ATT01 + LCOD39/40 */
    case OB_METAL_POLE: case OB_F_THROW_AMMO: case OB_BULLETS: case OB_BOILER: case OB_HANDLE: case OB_PLATE_BEFORE: case OB_RED_KEY:
    case OB_KONK_KEY: case OB_SORDOS_KEY: case OB_BOGD_KEY: case OB_MIRROR: case OB_SHAFT: case OB_LEAD_PLAQUE:
    case OB_SM_TH_AMMO: case OB_OBJ_BEFORE40: case OB_OBJ_BEFORE41: case OB_CRYST_SLIV: case OB_PULLEY_BLADE: case OB_COIN:
    case OB_THBOLT_AMMO: case OB_ZED_AMMO: case OB_PICK_AX: case OB_CRYSTAL_KEY: case OB_FLASK86: case OB_YONG_COUNTER:
    case OB_STOCK_KEY: case OB_STILETTO: case OB_PESTLE: case OB_LARGE_KEY:
        e5_reset(); e5_att01_lcod("LCOD39", "LCOD40"); CB->bloque(BLOQUE_NONE);
        break;

    case OB_BOOK:  /* 0x5444 — LCOD55/56 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) lieu_action_register("LCOD55", 2, "SP02ACT64", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) lieu_action_register("LCOD56", 3, "SP02ACT64", NULL);
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_CAN:  /* 0x4F7D — SPxxATT20 first-time, else LCOD41/42/43 */
        e5_reset();
        if (ST(V_ATT20_SEEN) == 0) {
            ST(V_ATT20_SEEN) = 1;
            if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ATT20");
            if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT20");
            if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP02ATT20");
            e5_clamp6(0x96);
            CB->animation("ar_50");
        } else {
            if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
            if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
            if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
            CB->animation("SG01ATT01");
            CB->animation("ar_50");
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_VIAL: {  /* 0x3B4E — OB21A/B/C by phase (edx), ATT19/20 + clamp */
        const c8 *ob;
        if      (phase == 0) ob = "OB21A";
        else if (phase == 1) ob = "OB21B";
        else                 ob = "OB21C";
        e5_reset();
        if (ST(ST_SPECIAL_MODE) == 1) {
            if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT19");
            if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT19");
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT20");
            if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT20");
        }
        CB->animation("ar_50");
        CB->etat_objet(ob, -1, -1, -1, -1, -1, -1, -2, 3);
        e5_clamp6(0xC8);
        CB->bloque(BLOQUE_NONE);
        break;
    }

    case OB_FULL_GOURD: {  /* 0x3F9C — OB24%c/OB77%c timer, phase=array index */
        if (ST(ST_COMBAT_STANCE) != 0) break;
        e5_reset();
        if (ST(ST_SPECIAL_MODE) != 1 && (ST(ST_SPECIAL_MODE) != 0 || ST(ST_HERO) == P_ARKHAN1))
            goto case24_fallback;
        {
            i32 *timer = &ST(0x140 + phase * 4);
            if (*timer > 0) {
                (*timer)--;
                if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT98");
                if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT19");
                if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT19");
                e5_clamp6(0x1E);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                break;
            }
            if (*timer == 0) {
                if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT98");
                if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT19");
                if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT19");
                c8 b1[16], b2[16];
                snprintf(b1, sizeof b1, "OB24%c", phase + 'A');
                snprintf(b2, sizeof b2, "OB77%c", phase + 'A');
                CB->etat_objet(b1, -1, -1, -1, -1, -1, -1, -2, 0);
                CB->etat_objet(b2, -1, -1, -1, -1, -1, -1, -3, 0);
                e5_clamp6(0x1E);
                if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
                if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
                if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
                CB->animation("SG01ATT01");
                CB->animation("ar_50");
                CB->bloque(BLOQUE_NONE);
                break;
            }
        }
    case24_fallback:
        if (ST(ST_SPECIAL_MODE) == 0 && ST(ST_HERO) == P_ARKHAN1) {
            ST(0x140 + phase * 4) = 0;
            CB->animation("SP01ACT98");
            CB->evenement(0x50, 2000);
        }
        CB->bloque(BLOQUE_NONE);
        break;
    }

    case OB_POTION: {  /* 0x4385 — OB31 + layer 0x14 reduction */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT20");
        if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT20");
        CB->animation("ar_50");
        e5_clamp6(0x32);
        CB->animation("ar_50");
        {
            i32 lv = CB->divers(DV_DARKNESS, -1, 0);
            CB->divers_empile(DV_DARKNESS, lv - 0x96, 0);
        }
        CB->etat_objet("OB31", -1, -1, -1, -1, -1, -1, -2, 0);
        CB->bloque(BLOQUE_NONE);
        break;
    }

    case OB_METAL_SUIT: {  /* 0x2D8C — SOB86 FSM (multi-branch room logic) */
        CB->select_pile(0, 2);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        if (ST(V_SOB86_ACTIVE) == 0) {
            if (ST(ST_LEVEL) == 30) {
                if (ST(ST_LOCKED_CAMERA) != 5 || ST(ST_VIE(P_KONKALITE2)) != VIE_DEAD) goto case71_lcod;
                ST(V_SOB86_ACTIVE) = 1;
                i32 v4 = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
                CB->bloque(BLOQUE_ALL);
                CB->divers_empile(DV_SCREEN, 5, 0);
                if (ST(ST_HERO) == P_ARKHAN2)
                    CB->pos_perso(P_ARKHAN2, 4444, 5790, v4, 1, "ar", 1, 0);
                if (ST(ST_HERO) == P_ARKHAN3)
                    CB->pos_perso(P_ARKHAN3, 4444, 5790, v4, 1, "ar", 1, 0);
                CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(0x62, 4444, 5790, v4, 1, "ar", 1, 0);
                    CB->pos_perso(0x62, 4444, 5790, v4, 1, "ar", 1, 0);
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(0x63, 4444, 5790, v4, 1, "ar", 1, 0);
                    CB->pos_perso(0x63, 4444, 5790, v4, 1, "ar", 1, 0);
                }
                CB->divers_empile(DV_SCREEN, 3, 0);
                if (ST(V_HUD_TIMER_MS) == 0) {
                    ST(V_SOB86_ACTIVE) = 0;
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("L31D83A", P_ARKHAN2);
                        CB->dialogue("L31D83B", P_ARKHAN2);
                        if (ST(0xC50) == 0) CB->dialogue("L31D83C", P_ARKHAN2);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("L31D84A", P_ARKHAN3);
                        CB->dialogue("L31D84B", P_ARKHAN3);
                        if (ST(0xC50) == 0) CB->dialogue("L31D84C", P_ARKHAN3);
                    }
                    CB->animation("Sar_50");
                    CB->divers_empile(DV_SCREEN, 5, 0);
                    if (ST(ST_HERO) == P_ARKHAN2)
                        CB->pos_perso(P_ARKHAN2, 4444, 5790, v4, 0, "ar", 1, 0);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        CB->pos_perso(P_ARKHAN3, 4444, 5790, v4, 0, "ar", 1, 0);
                    CB->divers_empile(DV_SCREEN, 3, 0);
                }
                CB->animation("Sar_50");
                CB->bloque(BLOQUE_NONE);
                break;
            }
            if (ST(ST_LEVEL) == 37) {
                if (ST(ST_LOCKED_CAMERA) != 0 || CB->distance(1, 6) >= 0x15E)
                    goto case71_lcod;
                ST(V_SOB86_ACTIVE) = 1;
                CB->bloque(BLOQUE_ALL);
                CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                CB->divers_empile(DV_SCREEN, 5, 0);
                if (ST(ST_HERO) == P_ARKHAN2)
                    CB->pos_perso(0x62,
                        CB->divers(1, ST(ST_HERO), 0),
                        CB->divers(1, ST(ST_HERO), 1),
                        CB->divers(1, ST(ST_HERO), 2),
                        1, "ar", 6, 0);
                if (ST(ST_HERO) == P_ARKHAN3)
                    CB->pos_perso(0x63,
                        CB->divers(1, ST(ST_HERO), 0),
                        CB->divers(1, ST(ST_HERO), 1),
                        CB->divers(1, ST(ST_HERO), 2),
                        1, "ar", 6, 0);
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("Sar_50");
                CB->bloque(BLOQUE_NONE);
                break;
            }
        case71_lcod:
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN2) lieu_action_register("LCOD666", 2, "SG02DIA02", NULL);
            if (ST(ST_HERO) == P_ARKHAN3) lieu_action_register("LCOD667", 3, "SG03DIA02", NULL);
            CB->bloque(BLOQUE_NONE);
            break;
        } else {
            if (ST(ST_LEVEL) == 35) {
                CB->bloque(BLOQUE_ALL);
                if (ST(ST_HERO) == P_ARKHAN2) lieu_action_register("LCOD670", 2, "SG02DIA02", NULL);
                if (ST(ST_HERO) == P_ARKHAN3) lieu_action_register("LCOD671", 3, "SG03DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                break;
            }
            ST(V_SOB86_ACTIVE) = 0;
            CB->bloque(BLOQUE_ALL);
            CB->divers_empile(DV_SCREEN, 5, 0);
            if (ST(ST_HERO) == P_ARKHAN2)
                CB->pos_perso(P_ARKHAN2,
                    CB->divers(1, 2, 0), CB->divers(1, 2, 1),
                    CB->divers(1, 2, 2), 0, "ar", 1, 0);
            if (ST(ST_HERO) == P_ARKHAN3)
                CB->pos_perso(P_ARKHAN3,
                    CB->divers(1, 3, 0), CB->divers(1, 3, 1),
                    CB->divers(1, 3, 2), 0, "ar", 1, 0);
            CB->divers_empile(DV_SCREEN, 3, 0);
            if (ST(ST_HERO) == P_ARKHAN2) lieu_action_register("LCOD668", 2, "SG02DIA01", NULL);
            if (ST(ST_HERO) == P_ARKHAN3) lieu_action_register("LCOD669", 3, "SG03DIA01", NULL);
            CB->bloque(BLOQUE_NONE);
            break;
        }
    }

    case OB_TUBE: {  /* 0x54F1 — SPxxATT02 health decrement */
        e5_reset();
        if (ST(0x6D8) == 0) {
            if (ST(ST_SPECIAL_MODE) == 0) {
                ST(0x6D8) = 1;
                if (ST(ST_HERO) == P_ARKHAN1) {
                    i32 v = CB->divers(DV_HP, P_ARKHAN1, -1);
                    if (v - 0x32 > 0x64) CB->divers(DV_HP, P_ARKHAN1, v - 0x32);
                    CB->animation("SP01ATT02");
                    CB->animation("P01ATT03");
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    i32 v = CB->divers(DV_HP, P_ARKHAN2, -1);
                    if (v - 0x1E > 0x32) CB->divers(DV_HP, P_ARKHAN2, v - 0x1E);
                    CB->animation("SP02ATT02");
                    CB->animation("P02ATT03");
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    i32 v = CB->divers(DV_HP, P_ARKHAN3, -1);
                    if (v - 0x14 > 0x32) CB->divers(DV_HP, P_ARKHAN3, v - 0x14);
                    CB->animation("SP02ATT02");
                    CB->animation("P02ATT03");
                    CB->animation("ar_50");
                }
            }
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD25", P_ARKHAN1);
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD26", P_ARKHAN2);
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD27", P_ARKHAN3);
                    CB->animation("ar_50");
                }
            }
        } else {
            if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
            if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
            if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
            CB->animation("SG01ATT01");
            CB->animation("ar_50");
        }
        CB->bloque(BLOQUE_NONE);
        break;
    }

    /* Original default cases (17,25,26,28,30,32,37,38,47,48,52-54,57-59,
     * 61-63,66-69,76,78-80,83,90-92,98) are genuine no-ops. */
    default:
        break;
    }
}


/* -------------------------------------------------------------------------
 * sub_1001A24C (thunk sub_100010D7) — used by lieu_pre_scene.
 * __fastcall(ecx=n). Builds "OB%02d" from state[0xD38] and plays the
 * SPnnPRE90 / SPnnPRE91 sequences of actor n around an attach_99C.
 * ---------------------------------------------------------------------- */
static void pre90_scene(i32 n) {                          /* sub_1001A24C */
    c8 ob[256];
    c8 b[256];
    snprintf(ob, sizeof(ob), "OB%02d", ST(V_PRE90_OBJECT));
    snprintf(b, sizeof(b), "SP%02dPRE90", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    CB->link_objet(n, ob, 0x1B, 0);
    snprintf(b, sizeof(b), "P%02dPRE91", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02darr01_10", n);
    CB->animation(b);
}

/* -------------------------------------------------------------------------
 * sub_1001A35C (thunk sub_100010AF): the hero greets persona n (search / handshake sequence).
 * __fastcall(ecx=hero, edx=n) + stdcall stack (a1 unused, a2, a3);
 * ret 0Ch. a2/a3 go to query_990.
 * ---------------------------------------------------------------------- */
void lieu_pre_scene(i32 hero, i32 n, i32 a1, i32 a2, i32 a3) { /* sub_1001A35C */
    c8 b[256];
    (void)a1;   /* [ebp+8] is never read */

    if (ST(ST_PLAYER_WEAPON) != 0 && ST(V_WEAPON_WARNED) == 1 &&
        (ST(ST_LEVEL) == 5 || ST(ST_LEVEL) == 15 || ST(ST_LEVEL) == 18 ||
         ST(ST_LEVEL) == 36 || ST(ST_LEVEL) == 48)) {
        ST(ST_PLAYER_WEAPON) = 0;
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02PRE73");
            CB->animation("P02_65535");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03PRE73");
            CB->animation("P03_65535");
        }
    }
    if (ST(ST_PLAYER_WEAPON) != 0 && ST(V_PRE90_OBJECT) != 0)
        pre90_scene(n);

    if (CB->distance(a2, a3) < 0x50 && n != 6) {
        snprintf(b, sizeof(b), "Sp%02dMAR30", hero);
        CB->animation(b);
        snprintf(b, sizeof(b), "P%02d_65535", hero);
        CB->animation(b);
    }
    snprintf(b, sizeof(b), "SG%02dPRE80", hero);
    CB->animation(b);
    snprintf(b, sizeof(b), "G%02dPRE81", hero);    /* sic: no leading 'S' */
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02dPRE60", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "SG%02dPRE82", hero);
    CB->animation(b);
    if (ST(ST_PLAYER_WEAPON) != 0)
        snprintf(b, sizeof(b), "G%02darr02_10", hero);
    else
        snprintf(b, sizeof(b), "G%02darr01_10", hero);
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02darr01_10", n);
    CB->animation(b);
}

const c8 *lieu_anim_name(i32 persona, i32 state) {
    return CB->divers(DV_ANIM_NAME, persona, state) ? lieu_divers_text() : NULL;
}

/* A level DLL is loaded per room (LoadLibraryA/FreeLibrary, dkev
 * 0x43A4xx), so its globals start from their initial values every visit. */
void lieu_common_reset(void) {
    e4_reset_floats();
    g_float_active = 1;
}
