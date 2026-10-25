/*
 * lieu13.c — Native hand-port of LIEU13.DLL (level-13 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu13_trig.c (reached through
 * l13_trigger, see lieu13_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x1000AE20, the state
 * pointer global is dword_1001A95C and the trigger table is
 * dword_1001A870[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1000B4EC), lieu_pre_scene (sub_1000B69C) and
 * sub_10001200 (float reset) are the shared lieu_common.c; the entry and
 * INIT are instruction-for-instruction identical to LIEU03. tick_body
 * (sub_1000785E) is level-13 specific.
 */

#include <stdlib.h>

#include "lieu13_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body — sub_1000785E (thunk 0x100010B9), level-13 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x786D */
    if (ST(0x70) == 0 && ST(ST_LOCKED_CAMERA) == 0) {
        ST(0x70) = 1;
        CB->play_anim(5, 1);
    }
    /* 0x78AA */
    if (ST(0x70) == 1 && ST(ST_LOCKED_CAMERA) != 0) {
        ST(0x70) = 0;
    }
    /* 0x78DA */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->select_pile(1, 1);
        /* 0x790B */
        if (ST(0xC90) == 0) {
            CB->bruitage("L13B01", 2147, 5160);
            /* 0x7932 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD39", P_ARKHAN2);
            }
            /* 0x7954 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD40", P_ARKHAN3);
            }
            CB->animation("Sar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        } else {
            /* 0x79A4 */
            if (ST(0xD54) == 1) {
                CB->bruitage("L13B02", 2147, 5160);
                /* 0x79CB */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L13D07", P_ARKHAN2);
                }
                /* 0x79ED */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L13D08", P_ARKHAN3);
                }
                CB->animation("Sar_50");
                CB->divers_empile(DV_SCREEN, 5, 0);
                /* 0x7A2C */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->cinematique("SC13_500");
                }
                /* 0x7A49 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->cinematique("SC14_500");
                }
                CB->animation("ar_50");
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x7AAC */
            if (ST(0xD54) == 2) {
                CB->bruitage("L13B03", 2147, 5160);
                /* 0x7AD3 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L13D09", P_ARKHAN2, "SG00DIA01", NULL);
                }
                /* 0x7AFB */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L13D10", P_ARKHAN3, "SG00DIA01", NULL);
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        CB->select_pile(0, 1);
    }
    /* 0x7B59 */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->select_pile(1, 1);
        /* 0x7B87 */
        if (ST(0x64) == 1 && ST(ST_LOCKED_CAMERA) == 3) {
            CB->play_anim(1, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x7BC7 */
        if (ST(0x64) == 2 && ST(ST_LOCKED_CAMERA) == 3) {
            CB->play_anim(2, 1);
            CB->select_pile(2, 1);
            CB->play_anim(1, 0);
            CB->select_pile(0, 1);
            ST(0xC90) = 1;
            return;
        }
    }
    /* 0x7C36 */
    if (ST(0x68) == 0 && ST(0xC90) == 1 && ST(ST_LOCKED_CAMERA) == 3) {
        ST(0x68) = 1;
        CB->play_anim(2, 1);
    }
    /* 0x7C85 */
    if (ST(0x68) == 1 && ST(ST_LOCKED_CAMERA) != 3) {
        ST(0x68) = 0;
    }
    /* 0x7CB5 */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        CB->evenement(33, 1500);
        CB->free_anim(5);
        CB->play_anim(6, 1);
        CB->play_anim(3, 0);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7D21 */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        ST(ST_LEVEL) = 13;
        CB->change_piece(49, 0);
    }
    /* 0x7D58 */
    if (ST(ST_SCRIPT_BUSY) == 42) {
        ST(ST_SCRIPT_BUSY) = 0;
        /* 0x7D76 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7D88 */
            if (ST(ST_PLAYER_WEAPON) == 23) {
                CB->animation("SP02VIC03");
                CB->animation("ar");
                lieu_action_register("LCOD57", P_ARKHAN2, "Sar", NULL);
            } else {
                CB->animation("Sar_50");
            }
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x7DE3 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7DF5 */
            if (ST(ST_PLAYER_WEAPON) == 23) {
                CB->animation("SP03VIC03");
                CB->animation("ar");
                lieu_action_register("LCOD58", P_ARKHAN3, "Sar", NULL);
            } else {
                CB->animation("Sar_50");
            }
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1000B1B9).
 * Callback ingestion (0x1000AE3A..0x1000B1AA), the trigger-table fill and
 * the unused dword_1001A9C8 = state+0xC8 are skipped: g_lieu_cb is provided
 * by the bridge, the trigger table is static in lieu13_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000B3E4);
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
        l13_trig00();   /* 0x1000B3E9: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000B2CE — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x1000B43C): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x1000AE20).
 * Jump table @ 0x1000B4B9, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu13_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu13: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l13_trigger(arg);             break;  /* dword_1001A870[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
