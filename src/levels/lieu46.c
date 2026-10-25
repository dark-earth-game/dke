/*
 * lieu46.c — Native hand-port of LIEU46.DLL (level-46 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu46_trig.c (reached through
 * l46_trigger, see lieu46_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x100088F0, the state
 * pointer global is dword_1001859C and the trigger table is
 * dword_100184B0[42].
 *
 * event4, event5, event6, lieu_action_register, lieu_pre_scene and
 * sub_10001200 (float reset) are the shared lieu_common.c. The tick body
 * (sub_100076BC) is level-46 specific.
 */

#include "lieu46_int.h"

/* Per-frame body — sub_100076BC (thunk 0x100010B9), level-46 specific.
 * `return` == original `jmp` to the epilogue. */
static void tick_body(void) {
    /* 0x76CE */
    if (ST(0xB4C) >= 3 && ST(0x68) == 0 && ST(ST_LOCKED_CAMERA) == 2) {
        ST(0x68) = 1;
        CB->change_camera(2, CAM_LOCKED);
    }
    /* 0x771A */
    if (ST(0x68) == 1 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 2400) {
        ST(0x68) = 0;
        ST(ST_LOCKED_CAMERA) = 1;
        CB->change_camera(1, CAM_AUTO);
    }
    /* 0x777A */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        ST(V_END_GAME_REQ) = 0;
        CB->evenement(EV_END_GAME, 1500);
    }
    /* 0x77CC */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        ST(V_DEATH_EVENT_SENT) = 1;
        /* 0x7807 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x7824 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x7850 */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        ST(ST_LEVEL) = 46;
        CB->evenement(1, -1);
        CB->bloque(BLOQUE_NONE);
        CB->change_piece(6, -1);
    }
    /* 0x78A5 */
    if (ST(V_EVENT_53) == 3) {
        ST(V_EVENT_53) = 0;
        CB->animation("Sar_10");
        /* 0x78D1 */
        if (ST(0x32C) == 0 && ST(0x330) == 0) {
            CB->modify_var(&ST(V_EVENT_2), 1);
        }
        /* 0x790C */
        if (ST(0x32C) != 0 || ST(0x330) != 0) {
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
    /* 0x795A */
    if (CB->divers(DV_DESC_POS, ST(ST_HERO), 1) > -450 && ST(0x32C) == 0 && ST(0x330) == 0 && ST(V_EVENT_2) == 0 && ST(V_EVENT_53) == 0) {
        ST(V_EVENT_2) = 1;
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
    }
    /* 0x79DA */
    if (ST(0x32C) == 0 && ST(0x330) == 0 && ST(V_EVENT_2) == 1) {
        CB->bloque(BLOQUE_ALL);
        ST(V_EVENT_2) = 0;
        ST(0x330) = 1;
        ST(0xF24) = 3;
        CB->change_camera(7, CAM_LOCKED);
        CB->joindre(-ST(ST_HERO), 0, 0, 0x64, -1);
        /* 0x7A75 */
        if (ST(0xEB0) == 1 || ST(0xEB0) == 2) {
            ST(0xEB0) = 3;
        }
        /* 0x7AA8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L46D01", P_ARKHAN2, "SG00DIA04", "Sar_10");
        }
        /* 0x7AD3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L46D02", P_ARKHAN3, "SG00DIA04", "Sar_10");
        }
        lieu_action_register("L46D03", P_RYLSADHAR21, "SP21ATT01", "SP21ATT01_10");
        /* 0x7B17 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L46D04", P_ARKHAN2, "SG00DIA04", "Sar_10");
        }
        /* 0x7B42 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L46D05", P_ARKHAN3, "SG00DIA04", "Sar_10");
        }
        lieu_action_register("L46D06A", P_RYLSADHAR21, "SP21ARR01", "SP21ATT01_10");
        lieu_action_register("L46D06B", P_RYLSADHAR21, "SP21ATT01", "SP21ARR01_10");
        /* 0x7B9F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L46D07", P_ARKHAN2, "SG00DIA04", "Sar_10");
        }
        /* 0x7BCA */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L46D08", P_ARKHAN3, "SG00DIA04", "Sar_10");
        }
        lieu_action_register("L46D09A", P_RYLSADHAR21, "SP21ARR01", "SP21ATT01_10");
        lieu_action_register("L46D09B", P_RYLSADHAR21, "SP21ATT01", "SP21ARR01_10");
        lieu_action_register("L46D09C", P_RYLSADHAR21, "SP21ARR01", "SP21ARR01_10");
        lieu_action_register("L46D09D", P_RYLSADHAR21, "SP21ATT01", "SP21ARR01_10");
        lieu_action_register("L46D09E", P_RYLSADHAR21, "SP21ARR01", "SP21ARR01_10");
        /* 0x7C72 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L46D10", P_ARKHAN2, "SG00DIA04", "Sar_10");
        }
        /* 0x7C9D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L46D11", P_ARKHAN3, "SG00DIA04", "Sar_10");
        }
        lieu_action_register("L46D12A", P_RYLSADHAR21, "SP21ATT01", "SP21ATT01_10");
        lieu_action_register("L46D12B", P_RYLSADHAR21, "SP21ARR01", "SP21ARR01_10");
        lieu_action_register("L46D12C", P_RYLSADHAR21, "SP21ATT01", "SP21ARR01_10");
        lieu_action_register("L46D12D", P_RYLSADHAR21, "SP21ATT01", "SP21ARR01_10");
        lieu_action_register("L46D12E", P_RYLSADHAR21, "SP21ARR01", "SP21ARR01_10");
        CB->animation("SP21ATT03");
        CB->dialogue("L46D12F", P_RYLSADHAR21);
        CB->animation("P21_65535");
        CB->animation("P21cad");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->change_camera(0, CAM_AUTO);
        /* 0x7D94 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L46D14", P_ARKHAN2, "SG00DIA08", "Sar_10");
        }
        /* 0x7DBF */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L46D15", P_ARKHAN3, "SG00DIA08", "Sar_10");
        }
        CB->animation("Sar_10");
        CB->bloque(BLOQUE_NONE);
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x10008C89).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001BA8C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu46_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x10008EB4);
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
        l46_trig00();   /* 0x10008EB9: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x10008D9E — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x10008F0C): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x100088F0).
 * Jump table @ 0x10008F89, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu46_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu46: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l46_trigger(arg);             break;  /* dword_100184B0[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
