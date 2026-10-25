/*
 * lieu41.c - Native hand-port of LIEU41.DLL (level-41 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu41_trig.c (reached through
 * l41_trigger, see lieu41_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x10008240, the state
 * pointer global is dword_100181EC and the trigger table is
 * dword_10018100[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register, lieu_pre_scene and sub_10001200 (float reset) are
 * the shared lieu_common.c; the entry and INIT follow LIEU38. tick_body
 * (sub_100075FB) is level-41 specific. The level code touches no DLL .data
 * global, so INIT resets nothing.
 */

#include <stdio.h>
#include <stdlib.h>

#include "lieu41_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body - sub_100075FB (thunk 0x100010B9), level-41 specific.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x7601 - the hero is dead: queue the game-over event once */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        ST(V_END_GAME_REQ) = 0;
        CB->evenement(EV_END_GAME, 1500);
    }
    /* 0x7653 */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        ST(V_DEATH_EVENT_SENT) = 1;
        /* 0x768E */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x76AB */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x76D7 - first time in the locked camera 1 */
    if (ST(ST_LOCKED_CAMERA) == 1 && ST(0x64) == 0) {
        ST(0x64) = 1;
        lieu_action_register("L41D01", P_ARKHAN3, "SG00DIA04", "Sar_10");
    }
    /* 0x771D - action key, none of the zones 3..7 holds the hero */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0
        && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)
        && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 4)
        && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 5)
        && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 6)
        && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        /* 0x77EA */
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, -3);
        /* 0x781E: ST(0x68) is the line counter of the hero's remarks */
        if (ST(0x68) == 2) {
            ST(0x68) = 2;
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L41D14", P_ARKHAN3, "SG00DIA06", "SP03ACT77");
            } else {
                lieu_action_register("L41D15", P_ARKHAN3, "SG00DIA06", "SP03ACT76");
            }
        }
        /* 0x7882 */
        if (ST(0x68) == 1) {
            ST(0x68) = 2;
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L41D12", P_ARKHAN3, "SG00DIA08", "SP03ACT77");
            } else {
                lieu_action_register("L41D13", P_ARKHAN3, "SG00DIA08", "SP03ACT76");
            }
        }
        /* 0x78E6 */
        if (ST(0x68) == 0) {
            ST(0x68) = 1;
            /* 0x7906 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT71");
                    lieu_action_register("L41D10", P_ARKHAN3, "P03ACT74", "SP03ACT77");
                } else {
                    CB->animation("SP03ACT70");
                    lieu_action_register("L41D10", P_ARKHAN3, "P03ACT74", "SP03ACT76");
                }
            }
            /* 0x797A */
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ACT73");
                    lieu_action_register("L41D11", P_ARKHAN3, "P03ACT74", "SP03ACT79");
                } else {
                    CB->animation("SP03ACT72");
                    lieu_action_register("L41D11", P_ARKHAN3, "P03ACT74", "SP03ACT78");
                }
            }
        }
        /* 0x79F3 */
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    } else {
        /* 0x7A1C - event 0/1 re-arm the side zones */
        if (ST(V_EVENT_0) == 1) {
            ST(V_EVENT_0) = 0;
            CB->ajoute_zone(6, 640, -585, 885, 70, 1);
        }
        /* 0x7A5B */
        if (ST(V_EVENT_1) == 1) {
            ST(V_EVENT_1) = 0;
            CB->ajoute_zone(7, 640, -2350, 885, 70, 1);
        }
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x100085D9).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001826C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu41_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x10008802);
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
        l41_trig00();   /* 0x10008809: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x100086EE - the hero is dead: play the death ending */
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x10008240).
 * Jump table @ 0x100088D9, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu41_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu41: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l41_trigger(arg);             break;  /* dword_10018100[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
