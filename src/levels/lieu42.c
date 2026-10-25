/*
 * lieu42.c — Native hand-port of LIEU42.DLL (level-42 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu42_trig.c (reached through
 * l42_trigger, see lieu42_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x1000BA90, the state
 * pointer global is dword_1001BA0C and the trigger table is
 * dword_1001B920[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1000C15C), lieu_pre_scene and sub_10001200
 * (float reset) are the shared lieu_common.c. The tick body (sub_10007A01)
 * is level-42 specific. The DLL has no writable globals of its own.
 */

#include "lieu42_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10007A01 (thunk 0x100010B9), level-42 specific.
 * `return` == original `jmp` to the epilogue (0x1000894C).
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x7A13 */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        CB->evenement(44, 3000);
    }
    /* 0x7A56 */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        /* 0x7A87 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x7AB0 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x7ADC */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x7B08 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x7B2C */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x7B49 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x7B75 */
    if (ST(0x88) == 0 && ST(ST_LOCKED_CAMERA) == 3) {
        ST(0x88) = 1;
        CB->bruitage("L42AMB2", -3700, -1199);
    }
    /* 0x7BBD */
    if (ST(0x88) == 1 && ST(ST_LOCKED_CAMERA) != 3) {
        ST(0x88) = 0;
        CB->bruitage("L42AMB1", -2796, -1288);
    }
    /* 0x7C02 */
    if (ST(0x7C) == 0 && ST(ST_LOCKED_CAMERA) == 4) {
        CB->select_pile(1, 1);
        ST(0x7C) = 1;
        CB->play_anim(0, 0);
        CB->play_anim(1, 0);
        CB->play_anim(2, 0);
        CB->select_pile(0, 1);
    }
    /* 0x7C70 */
    if (ST(0x7C) == 1 && ST(ST_LOCKED_CAMERA) != 4) {
        ST(0x7C) = 0;
    }
    /* 0x7CA0 */
    if (ST(V_EVENT_0) == 1) {
        /* 0x7CB2 */
        if (ST(0x84) == -11) {
            ST(V_EVENT_0) = 0;
            CB->select_pile(1, 2);
            CB->free_anim(2);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7D0D */
        if (ST(0x84) == 11) {
            ST(V_EVENT_0) = 0;
            CB->select_pile(1, 2);
            CB->free_anim(2);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7D68 */
        if (ST(0x84) == -12) {
            ST(V_EVENT_0) = 0;
            CB->select_pile(1, 2);
            CB->free_anim(1);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7DC3 */
        if (ST(0x84) == 12) {
            ST(V_EVENT_0) = 0;
            CB->select_pile(1, 2);
            CB->free_anim(1);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7E1E */
        if (ST(0x84) == -13) {
            ST(V_EVENT_0) = 0;
            CB->select_pile(1, 2);
            CB->free_anim(0);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7E76 */
        if (ST(0x84) == 13) {
            ST(V_EVENT_0) = 0;
            CB->select_pile(1, 2);
            CB->free_anim(0);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0x7ECE */
    if (ST(V_EVENT_2) == 1) {
        /* 0x7EE0 */
        if (ST(0x84) == -11) {
            ST(V_EVENT_2) = 0;
            CB->select_pile(2, 2);
            CB->load_anim("242HAU03", 2);
            CB->play_anim(2, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7F4B */
        if (ST(0x84) == 11) {
            ST(V_EVENT_2) = 0;
            CB->select_pile(2, 2);
            CB->load_anim("242BAS03", 2);
            CB->play_anim(2, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7FB6 */
        if (ST(0x84) == -12) {
            ST(V_EVENT_2) = 0;
            CB->select_pile(2, 2);
            CB->load_anim("242HAU02", 1);
            CB->play_anim(1, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x8021 */
        if (ST(0x84) == 12) {
            ST(V_EVENT_2) = 0;
            CB->select_pile(2, 2);
            CB->load_anim("242BAS02", 1);
            CB->play_anim(1, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x808C */
        if (ST(0x84) == -13) {
            ST(V_EVENT_2) = 0;
            CB->select_pile(2, 2);
            CB->load_anim("242HAU01", 0);
            CB->play_anim(0, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x80F1 */
        if (ST(0x84) == 13) {
            ST(V_EVENT_2) = 0;
            CB->select_pile(2, 2);
            CB->load_anim("242BAS01", 0);
            CB->play_anim(0, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0x8156 */
    if (ST(0x80) == 0 && ST(ST_LOCKED_CAMERA) == 5) {
        ST(0x80) = 1;
        CB->select_pile(1, 1);
        /* 0x8196 */
        if (ST(0x70) == 1) {
            CB->play_anim(3, 0);
        }
        /* 0x81B2 */
        if (ST(0x70) == -1) {
            CB->play_anim(3, 0);
        }
        /* 0x81CE */
        if (ST(0x74) == 1) {
            CB->play_anim(4, 0);
        }
        /* 0x81EA */
        if (ST(0x74) == -1) {
            CB->play_anim(4, 0);
        }
        /* 0x8206 */
        if (ST(0x78) == 1) {
            CB->play_anim(5, 0);
        }
        /* 0x8222 */
        if (ST(0x78) == -1) {
            CB->play_anim(5, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x824E */
    if (ST(0x80) == 1 && ST(ST_LOCKED_CAMERA) != 5) {
        ST(0x80) = 0;
    }
    /* 0x8281 */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        /* 0x82A2 */
        if (ST(0x84) == -14) {
            CB->select_pile(1, 2);
            CB->free_anim(3);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x82EE */
        if (ST(0x84) == 14) {
            CB->select_pile(1, 2);
            CB->free_anim(3);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x833A */
        if (ST(0x84) == -15) {
            CB->select_pile(1, 2);
            CB->free_anim(4);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x8386 */
        if (ST(0x84) == 15) {
            CB->select_pile(1, 2);
            CB->free_anim(4);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x83D2 */
        if (ST(0x84) == -16) {
            ST(0xE0) = 1;
            CB->select_pile(1, 2);
            CB->free_anim(5);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x842D */
        if (ST(0x84) == 16) {
            ST(0xE0) = 2;
            CB->select_pile(1, 2);
            CB->free_anim(5);
            CB->play_anim(6, 0);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0x8488 */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        /* 0x84A9 */
        if (ST(0x84) == -16) {
            ST(0xE0) = 3;
            CB->select_pile(2, 2);
            CB->load_anim("242HAU06", 5);
            CB->play_anim(5, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x8514 */
        if (ST(0x84) == 16) {
            ST(0xE0) = 4;
            CB->select_pile(2, 2);
            CB->load_anim("242BAS06", 5);
            CB->play_anim(5, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x857F */
        if (ST(0x84) == -15) {
            CB->select_pile(2, 2);
            CB->load_anim("242HAU05", 4);
            CB->play_anim(4, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x85DB */
        if (ST(0x84) == 15) {
            CB->select_pile(1, 2);
            CB->load_anim("242BAS05", 4);
            CB->play_anim(4, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x8637 */
        if (ST(0x84) == -14) {
            CB->select_pile(2, 2);
            CB->load_anim("242HAU04", 3);
            CB->play_anim(3, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x8693 */
        if (ST(0x84) == 14) {
            CB->select_pile(2, 2);
            CB->load_anim("242BAS04", 3);
            CB->play_anim(3, 0);
            CB->free_anim(6);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0x86EF */
    if (ST(0x8B0) == 1 && ST(ST_LOCKED_CAMERA) == 3) {
        ST(0x8B0) = 2;
        lieu_action_register("L42D02A", P_ARKHAN3, "SG03DIA01", NULL);
        lieu_action_register("L42D02B", P_ARKHAN3, "SG03DIA01", NULL);
        lieu_action_register("L42D02C", P_ARKHAN3, "SG03DIA01", NULL);
    }
    /* 0x8764 */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        /* 0x8785 */
        if (ST(0x56C) == 0) {
            CB->select_pile(2, 1);
            CB->anim_objet("IM88", 1, 30, 1000, 0x400);
            CB->anim_objet("IM88", 1, -30, 0x32, 0);
            CB->select_pile(0, 1);
        }
        /* 0x87E6 */
        if (ST(0x56C) == 1) {
            CB->select_pile(2, 1);
            CB->vire_col(30, 1);
            CB->vire_col(-7, 1);
            CB->vire_col(-8, 1);
            CB->divers_empile(DV_SCREEN, 5, 0);
            CB->cinematique("SC32_500");
            CB->change_piece(-42, 1);
            CB->vire_col(29, 0);
            CB->etat_objet("IM88", 1725, -5835, -1253, 0x4000, 0xC000, 0, 0, 4);
            CB->change_camera(0, CAM_LOCKED);
            CB->divers_empile(DV_SCREEN, 3, 0);
            ST(0x80) = 1;
            ST(0xB8) = 1;
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0x890F */
    if (ST(0xB8) == 1 && CB->distance(1, 8) > 500) {
        CB->change_camera(0, CAM_AUTO);
        ST(0xB8) = 0;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1000BE29).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001BA8C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu42_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000C052);
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
        l42_trig00();   /* 0x1000C059: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000BF3E — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x1000C0AC): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x1000BA90).
 * Jump table @ 0x1000C129, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu42_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu42: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l42_trigger(arg);             break;  /* dword_1001B920[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
