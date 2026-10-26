/*
 * lieu53.c - Native hand-port of LIEU53.DLL (level-53 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick, the tick body and the
 * event4 / event5 / event6 dispatch
 * (the handlers are the shared older-version ones, lieu_common_v1.c). The 42 zone trigger handlers live in
 * lieu53_trig.c (reached through l53_trigger, see lieu53_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. LIEU53 is the L13 room script, built from
 * the same older script version as LIEU33 / LIEU47: the entry is at
 * 0x1000A070, the state pointer global is dword_1001990C and the trigger
 * table is dword_10019820[42].
 *
 * Its event4 (sub_10001200), event5 (sub_10001F9F) and event6
 * (sub_10005273) are not the lieu_common.c ones. They started from the
 * LIEU47 copies. lieu_action_register is the shared one. sub_1000A6C6 (the
 * LIEU33 pre-scene variant, 706 bytes) is never called in this DLL and is
 * not ported. The DLL .data the level code touches: the four event4 float
 * accumulators, the two event5 sprintf buffers and the tick print_msg
 * buffer; all are reset at INIT.
 */
#include <stdio.h>
#include <stdlib.h>

#include "lieu53_int.h"

/* tick body print_msg buffer (dword_10019620). */
static c8 g_txt[32];

/* The DLL is loaded afresh for every room: its globals restart. Called by
 * INIT, and by tools/lieu_difftest_harness.c before every case. */
void l53_reset_dll_globals(void) {
    lieu_v1_reset();
    g_txt[0] = '\0';
}

/* -------------------------------------------------------------------------
 * Per-frame body - sub_10006A59 (thunk 0x100010B4), level-53 specific.
 * `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    i32 t1 = CB->divers(DV_CAN_FIGHT, 9, 2);
    snprintf(g_txt, sizeof(g_txt), "gun:%d", t1);
    CB->print_msg(g_txt, 0, 240);
    /* 0x6A9F */
    if (ST(0x70) == 0 && ST(ST_LOCKED_CAMERA) == 0) {
        ST(0x70) = 1;
        CB->play_anim(5, 1);
    }
    /* 0x6ADC */
    if (ST(0x70) == 1 && ST(ST_LOCKED_CAMERA) != 0) {
        ST(0x70) = 0;
    }
    /* 0x6B0C */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->select_pile(1, 1);
        /* 0x6B3D */
        if (ST(0xC90) == 0) {
            CB->bruitage("L13B01", 2147, 5160);
            /* 0x6B64 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD39", P_ARKHAN2);
            }
            /* 0x6B86 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD40", P_ARKHAN3);
            }
            CB->animation("Sar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        } else {
            /* 0x6BD6 */
            if (ST(0xC94) == 1) {
                CB->bruitage("L13B02", 2147, 5160);
                /* 0x6BFD */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L13D07", P_ARKHAN2);
                }
                /* 0x6C1F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L13D08", P_ARKHAN3);
                }
                CB->animation("Sar_50");
                CB->divers_empile(DV_SCREEN, 5, 0);
                /* 0x6C5E */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->cinematique("SC13_500");
                }
                /* 0x6C7B */
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
            /* 0x6CDE */
            if (ST(0xC94) == 2) {
                CB->bruitage("L13B03", 2147, 5160);
                /* 0x6D05 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L13D09", P_ARKHAN2, "SG00DIA01", NULL);
                }
                /* 0x6D2D */
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
    /* 0x6D8B */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->select_pile(1, 1);
        /* 0x6DB9 */
        if (ST(0x64) == 1 && ST(ST_LOCKED_CAMERA) == 3) {
            CB->play_anim(1, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x6DF9 */
        if (ST(0x64) == 2 && ST(ST_LOCKED_CAMERA) == 3) {
            CB->play_anim(2, 1);
            CB->select_pile(2, 1);
            CB->play_anim(1, 0);
            CB->select_pile(0, 1);
            ST(0xC90) = 1;
            return;
        }
    }
    /* 0x6E68 */
    if (ST(0x68) == 0 && ST(0xC90) == 1 && ST(ST_LOCKED_CAMERA) == 3) {
        ST(0x68) = 1;
        CB->play_anim(2, 1);
    }
    /* 0x6EB7 */
    if (ST(0x68) == 1 && ST(ST_LOCKED_CAMERA) != 3) {
        ST(0x68) = 0;
    }
    /* 0x6EE7 */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        CB->evenement(33, 1500);
        CB->free_anim(5);
        CB->play_anim(6, 1);
        CB->play_anim(3, 0);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x6F53 */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        ST(ST_LEVEL) = 13;
        CB->change_piece(49, 0);
    }
    /* 0x6F8A */
    if (ST(ST_SCRIPT_BUSY) == 42) {
        ST(ST_SCRIPT_BUSY) = 0;
        /* 0x6FA8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x6FBA */
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
        /* 0x7015 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7027 */
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
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x1000A3FA).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001998C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu53_trig.c. There is no death
 * branch in this version.
 *   arg = reload flag (non-zero => skip arming).
 * ---------------------------------------------------------------------- */
static void init(i32 reload_flag) {
    if (reload_flag == 0)
        l53_trig00();   /* 0x1000A413: call sub_1000106E -> sub_100065C0 */
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1, 0x1000A466): drain pending sound, run body.
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x1000A070).
 * Jump table @ 0x1000A4E3, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu53_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu53: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        /* DLL .data restarts on every room load */
        l53_reset_dll_globals();
        init(arg);
        break;
    case LIEU_OP_EVENT:  l53_trigger(arg);             break;  /* dword_10019820[arg]() */
    case LIEU_OP_CALL4:  lieu_v1_event4(arg, 1);            break;
    case LIEU_OP_CALL5:  lieu_v1_event5(arg, arg2, 1);      break;
    case LIEU_OP_CALL6:  lieu_v1_event6(arg);                   break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
