/*
 * lieu35.c - Native hand-port of LIEU35.DLL (level-35 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu35_trig.c (reached through
 * l35_trigger, see lieu35_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Same compiler/layout as LIEU01..LIEU03; the
 * entry is at 0x1000BE40, the state pointer global is dword_1001B88C and the
 * trigger table is dword_1001B7A0[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1000C50C), lieu_pre_scene and sub_10001200
 * (float reset) are the shared lieu_common.c; the entry and INIT follow
 * LIEU30. tick_body (sub_1000816D) is level-35 specific. The level code
 * touches no DLL .data global (no debug sprintf), so INIT resets nothing.
 */

#include <stdio.h>
#include <stdlib.h>

#include "lieu35_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body - sub_1000816D (thunk 0x100010B9), level-35 specific.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x817F */
    if (ST(ST_HERO) != 4) {
        ST(0xE8) = ST(ST_HERO);
    }
    /* 0x81A8 */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        ST(V_END_GAME_REQ) = 0;
        CB->evenement(EV_END_GAME, 1500);
    }
    /* 0x81FA */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->select_pile(2, 2);
        CB->divers_empile(DV_SCREEN, 3, 0);
    }
    /* 0x823D */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        CB->divers_empile(DV_HP, ST(0xE8), 0);
        ST(V_DEATH_EVENT_SENT) = 1;
        /* 0x8290 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x82AD */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x82D9 */
    if (ST(ST_VIE(92)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) != 0) {
        ST(V_ENGAGED_PERSONA) = 0;
    }
    /* 0x8306 */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->bloque(BLOQUE_NONE);
        CB->divers(DV_MUSIC, 1, 4);
        ST(ST_LEVEL) = 35;
        CB->change_piece(30, 0);
    }
    /* 0x835A */
    if (ST(V_EVENT_53) == 3) {
        ST(V_EVENT_53) = 0;
        /* 0x837B */
        if (ST(0x4D8) == 0) {
            CB->joindre(-ST(ST_HERO), 0, 0, 10, -1);
            ST(0x4D8) = 1;
            /* 0x83B9 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D01", P_ARKHAN2, "SG00DIA01", NULL);
            }
            /* 0x83E1 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D02", P_ARKHAN3, "SG00DIA01", NULL);
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x842A */
    if (ST(0x64) == 0 && ST(ST_VIE(92)) == 1 && (ST(ST_LOCKED_CAMERA) == 2 || ST(ST_LOCKED_CAMERA) == 3) && (CB->distance(6, 20) < 300 || CB->distance(6, 40) < 250)) {
        CB->select_pile(0, 2);
        ST(0x64) = 1;
        /* 0x84C1 */
        if (ST(0x908) == 0) {
            ST(0x908) = 1;
            CB->pos_perso(92, 4062, 6903, 0x9858, 1, "p92mar01", 0x28, 600);
        }
        ST(V_ENGAGED_PERSONA) = 92;
        CB->combat(92, 0);
        CB->select_pile(0, 1);
    }
    /* 0x8530 */
    if (ST(V_HUD_TIMER_MS) == 0 && ST(0x68) == 0) {
        ST(0x68) = 1;
        CB->select_pile(1, 2);
        CB->bloque(BLOQUE_ALL);
        CB->evenement(EV_HUD_TIMER, -1);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8594 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L35D27", P_ARKHAN2, "SG00DIA08", NULL);
            CB->animation("SP02NOY01");
            CB->animation("P02_65535");
        }
        /* 0x85D2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L35D28", P_ARKHAN3, "SG00DIA08", NULL);
            CB->animation("SP03NOY01");
            CB->animation("P03_65535");
        }
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
        CB->select_pile(0, 1);
    }
    /* 0x864F */
    if (ST(V_SOB86_ACTIVE) == 666 && ST(0x68) == 0) {
        ST(0x68) = 1;
        CB->select_pile(1, 2);
        CB->bloque(BLOQUE_ALL);
        CB->evenement(EV_HUD_TIMER, -1);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x86B3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02NOY01");
            CB->animation("P02_65535");
        }
        /* 0x86DB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03NOY01");
            CB->animation("P03_65535");
        }
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
        CB->select_pile(0, 1);
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x1000C1CF).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001B90C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu35_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000C402);
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
        l35_trig00();   /* 0x1000C409: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000C2EE - the hero is dead: play the death ending */
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x1000BE40).
 * Jump table @ 0x1000C4D9, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu35_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu35: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l35_trigger(arg);             break;  /* dword_1001B7A0[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
