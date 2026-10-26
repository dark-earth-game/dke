/*
 * lieu52.c — Native hand-port of LIEU52.DLL (level-52 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu52_trig.c (reached through
 * l52_trigger, see lieu52_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x100082A0, the state
 * pointer global is dword_10017F4C and the trigger table is
 * dword_10017E60[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1000896C), lieu_pre_scene (sub_10008B1C) and
 * sub_10001200 (float reset) are the shared lieu_common.c. The tick body
 * (sub_10007763) is level-52 specific.
 */

#include "lieu52_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10007763 (thunk 0x100010B9), level-52 specific.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->divers_empile(DV_MUSIC, 0, 1);
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x10008639).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_10017FCC = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu52_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x10008862);
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
        l52_trig00();   /* 0x10008869: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000874E — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x100088BC): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x100082A0).
 * Jump table @ 0x10008939, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu52_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu52: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l52_trigger(arg);             break;  /* dword_10017E60[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
