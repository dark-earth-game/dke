/*
 * lieu26.c - Native hand-port of LIEU26.DLL (level-26 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu26_trig.c (reached through
 * l26_trigger, see lieu26_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Same compiler/layout as LIEU01..LIEU03; the
 * entry is at 0x10009FB0, the state pointer global is dword_1001970C and the
 * trigger table is dword_10019620[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1000A67C), lieu_pre_scene (sub_1000A82C) and
 * sub_10001200 (float reset) are the shared lieu_common.c; the entry and
 * INIT are the same as LIEU25. tick_body (sub_10007772) is level-26
 * specific. The level code touches no DLL .data globals, so INIT has
 * nothing to reset.
 */

#include <stdlib.h>

#include "lieu26_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body - sub_10007772 (thunk 0x100010B9), level-26 specific.
 * `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x7784 */
    if (ST(ST_LOCKED_CAMERA) == 0 && ST(0x6C) == 1) {
        CB->load_anim("226TELES", 2);
        ST(0x6C) = 0;
    }
    /* 0x77C1 */
    if (ST(ST_LOCKED_CAMERA) == 1 && ST(0x6C) == 0) {
        ST(0x6C) = 1;
        CB->pause_anim(2, 0, 0);
    }
    /* 0x77FD */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_DEATH_EVENT_SENT) = 1;
        /* 0x7840 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x7869 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x7895 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x78C1 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x78E5 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x7902 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x792E */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        /* 0x794C */
        if (ST(0x68) == 0) {
            ST(0x68) = 1;
            CB->select_pile(2, 2);
            CB->play_anim(4, 0);
            CB->select_pile(1, 1);
            CB->evenement(0, 500);
            CB->bruitage("L26B01", -1, -1);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x79C5 */
        if (ST(0x68) == 1) {
            ST(0x68) = 2;
            CB->select_pile(2, 1);
            CB->evenement(0, 1000);
            CB->play_anim(0, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7A19 */
        if (ST(0x68) == 2) {
            CB->bloque(BLOQUE_NONE);
            /* 0x7A33 */
            if (ST(0xD14) == 0) {
                ST(0xD14) = 1;
            }
            CB->divers(DV_MUSIC, 1, 4);
            ST(ST_LEVEL) = 26;
            CB->change_piece(25, 0);
        }
    }
    /* 0x7A7F */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->select_pile(1, 1);
        CB->free_anim(2);
        CB->play_anim(1, 0);
        CB->select_pile(0, 1);
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x1000A349).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001978C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu26_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000A572);
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
        l26_trig00();   /* 0x1000A579: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000A45E - the hero is dead: play the death ending */
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
 * TICK (opcode -1): drain pending sound, run body.
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x10009FB0).
 * Jump table @ 0x1000A649, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu26_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu26: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l26_trigger(arg);             break;  /* dword_10019620[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
