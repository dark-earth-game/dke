/*
 * lieu40.c - Native hand-port of LIEU40.DLL (level-40 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu40_trig.c (reached through
 * l40_trigger, see lieu40_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Same compiler/layout as LIEU01..LIEU03; the
 * entry is at 0x100088A0, the state pointer global is dword_1001826C and the
 * trigger table is dword_10018180[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register, lieu_pre_scene and sub_10001200 (float reset) are
 * the shared lieu_common.c; the entry and INIT follow LIEU38. tick_body
 * (sub_100078BF) is level-40 specific. The level code touches no DLL .data
 * global, so INIT resets nothing.
 */

#include "lieu40_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body - sub_100078BF (thunk 0x100010B9), level-40 specific:
 * only the hero's death ending.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x78C5 */
    if (ST(ST_HERO) == 4 && ST(0x64) == 0) {
        ST(0x64) = 1;
        /* 0x78F2 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x791A */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x7946 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x7972 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x799C */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x79B9 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x10008C39).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_100182EC = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu40_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x10008E62);
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

    /* 0x8D2A */
    if (ST(ST_HERO) != 4 || ST(0xE8C) == 1) {
        l40_trig00();   /* 0x10008E69: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x8D4E - the hero is dead: play the death ending */
    CB->divers_empile(DV_SCREEN, 1, 0);
    ST(V_DEATH_EVENT_SENT) = 1;
    /* 0x8D6F */
    if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
        CB->divers(DV_HP, P_ARKHAN0, 0);
    }
    /* 0x8D95 */
    if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
        CB->divers(DV_HP, P_ARKHAN1, 0);
    }
    /* 0x8DC1 */
    if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
        CB->divers(DV_HP, P_ARKHAN2, 0);
    }
    /* 0x8DED */
    if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
        CB->divers(DV_HP, P_ARKHAN3, 0);
    }
    /* 0x8E19 */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->cinematique("SC22_500");
    }
    /* 0x8E36 */
    if (ST(V_GUARD_FIGHT_DONE) != 0) {
        CB->cinematique("SC23_500");
    }
    CB->divers_empile(DV_END_GAME, 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1): drain pending sound, run body.
 * ---------------------------------------------------------------------- */
static void tick(void) {
    /* 0x8EBC */
    if (ST(V_PENDING_EVENT) != 0) {
        CB->select_pile(2, 1);
        CB->evenement(ST(V_PENDING_EVENT) + EV_PENDING_BASE, 1000);
        ST(V_PENDING_EVENT) = 0;
        CB->select_pile(0, 1);
    }
    tick_body();
}

/* -------------------------------------------------------------------------
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x100088A0).
 * Jump table @ 0x10008F39, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu40_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu40: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l40_trigger(arg);             break;  /* dword_10018180[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
