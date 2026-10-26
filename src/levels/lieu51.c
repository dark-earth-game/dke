/*
 * lieu51.c — Native hand-port of LIEU51.DLL (level-51 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu51_trig.c (reached through
 * l51_trigger, see lieu51_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x1000AB40, the state
 * pointer global is dword_1001A81C and the trigger table is
 * dword_1001A730[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1000B20C), lieu_pre_scene (sub_1000B3BC) and
 * sub_10001200 (float reset) are the shared lieu_common.c. The tick body
 * (sub_10008186) is level-51 specific.
 */

#include <stdio.h>

#include "lieu51_int.h"

/* Debug sprintf buffer of the tick body (DLL global at 0x1001A530, starts ""). */
static c8 g_txt[256];

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10008186 (thunk 0x100010B9), level-51 specific.
 * `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    snprintf(g_txt, sizeof(g_txt), "col:%d ", CB->divers(DV_PLAYER_ZONE, 0, 0));
    CB->print_msg(g_txt, 0, 0xDC);
    snprintf(g_txt, sizeof(g_txt), "test21:%d ", ST(0xF0));
    CB->print_msg(g_txt, 0, 0xF0);
    snprintf(g_txt, sizeof(g_txt), "test23:%d ", ST(0xF8));
    CB->print_msg(g_txt, 0, 0xFA);
    snprintf(g_txt, sizeof(g_txt), "test24:%d ", ST(0xFC));
    CB->print_msg(g_txt, 0, 0x104);
    snprintf(g_txt, sizeof(g_txt), "test25:%d ", ST(0x100));
    CB->print_msg(g_txt, 0, 0x10E);
    snprintf(g_txt, sizeof(g_txt), "test26:%d ", ST(0x114));
    CB->print_msg(g_txt, 0, 0x118);
    snprintf(g_txt, sizeof(g_txt), "test30:%d ", ST(0x54));
    CB->print_msg(g_txt, 0, 0x122);
    snprintf(g_txt, sizeof(g_txt), "test31:%d ", ST(0x58));
    CB->print_msg(g_txt, 0, 0x12C);
    /* 0x830A */
    if ((CB->divers(DV_PLAYER_ZONE, 0, 0) == 10 || CB->divers(DV_PLAYER_ZONE, 0, 0) == 9 || CB->divers(DV_PLAYER_ZONE, 0, 0) == 6) && CB->direction_zone(11) && ST(V_EVENT15_STEP) == 0) {
        /* 0x8377 */
        if (ST(0x88) == 0) {
            ST(0x88) = ST(ST_GAME_TIME);
        }
        /* 0x83A0 */
        if ((ST(ST_GAME_TIME) - ST(0x88)) > 500) {
            ST(0x88) = 0;
            ST(V_EVENT15_STEP) = 1;
            CB->bloque(BLOQUE_ALL);
            /* 0x83EB */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x8405 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT29");
                }
                /* 0x8422 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT29");
                }
            } else {
                /* 0x8444 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT28");
                }
                /* 0x8461 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT28");
                }
            }
            /* 0x847E */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD67", P_ARKHAN2);
            }
            /* 0x84A0 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD68", P_ARKHAN3);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(V_EVENT15_STEP), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
    /* 0x84E9 */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_DEATH_EVENT_SENT) = 1;
        CB->cinematique("SC23_500");
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x854D */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        /* 0x856B */
        if (ST(0x6C) == 7) {
            CB->ajoute_zone(7, 5203, -800, 185, 520, 1);
        }
        /* 0x859B */
        if (ST(0x6C) == 8) {
            CB->ajoute_zone(8, 5010, -1195, 165, 435, 1);
        }
        /* 0x85CB */
        if (ST(0x6C) == 9) {
            CB->ajoute_zone(9, 5230, -1407, 316, 180, 1);
        }
        /* 0x85FB */
        if (ST(0x6C) == 13) {
            CB->ajoute_zone(13, 5395, -1725, 90, 570, 1);
        }
        /* 0x8628 */
        if (ST(0x6C) == 14) {
            CB->ajoute_zone(14, 5120, -2011, 490, 190, 1);
        }
        /* 0x8658 */
        if (ST(0x6C) == 15) {
            CB->ajoute_zone(15, 4630, -2150, 510, 160, 1);
        }
        /* 0x8688 */
        if (ST(0x6C) == 16) {
            CB->ajoute_zone(16, 4020, -2000, 690, 150, 1);
        }
        /* 0x86B8 */
        if (ST(0x6C) == 17) {
            CB->ajoute_zone(17, 3663, -1590, 115, 735, 1);
        }
        /* 0x86E5 */
        if (ST(0x6C) == 18) {
            CB->ajoute_zone(18, 3535, -1084, 115, 370, 1);
        }
        /* 0x8712 */
        if (ST(0x6C) == 19) {
            CB->ajoute_zone(19, 3723, -853, 306, 200, 1);
        }
        /* 0x8742 */
        if (ST(0x6C) == 20) {
            CB->ajoute_zone(20, 3966, -730, 170, 140, 1);
        }
        /* 0x8772 */
        if (ST(0x6C) == 21) {
            CB->ajoute_zone(21, 4370, -645, 690, 110, 1);
        }
        /* 0x879F */
        if (ST(0x6C) == 22) {
            CB->ajoute_zone(22, 4900, -560, 415, 105, 1);
        }
    }
    /* 0x87CC */
    if (ST(0x68) == 1 && !CB->on_joindre(ST(ST_HERO))) {
        ST(0x68) = 2;
        CB->stop_joindre(-ST(ST_HERO));
        /* 0x8818 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ATT36");
            CB->animation("P02_65535");
        }
        /* 0x8840 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ATT36");
            CB->animation("P03_65535");
        }
        CB->modify_var(&ST(0x68), 3);
    }
    /* 0x8879 */
    if (ST(0x68) == 3) {
        CB->divers(DV_MUSIC, 1, 4);
        ST(0x68) = 4;
        /* 0x88A9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->divers_empile(DV_SCREEN, 1, 0);
            CB->cinematique("SC18_500");
        }
        /* 0x88D8 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->divers_empile(DV_SCREEN, 1, 0);
            CB->cinematique("SC19_500");
        }
        CB->bloque(BLOQUE_NONE);
        CB->change_piece(40, 0);
    }
    /* 0x8919 */
    if (ST(0x70) == 1 && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) < -180) {
        ST(0x70) = 0;
        CB->ajoute_zone(25, 4460, -103, 370, 90, 1);
    }
    /* 0x8975 */
    if (ST(ST_LOCKED_CAMERA) == 5 && ST(0x51C) == 0) {
        ST(0x51C) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x89B3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L51D107", P_ARKHAN2, "SG00DIA01", "Sar_10");
        }
        /* 0x89DE */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L51D108", P_ARKHAN3, "SG00DIA01", "Sar_10");
        }
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1000AED9).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001A89C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu51_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000B102);
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
        l51_trig00();   /* 0x1000B109: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000AFEE — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x1000B15C): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x1000AB40).
 * Jump table @ 0x1000B1D9, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu51_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu51: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        g_txt[0] = '\0';   /* .data globals restart on every room load */
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l51_trigger(arg);             break;  /* dword_1001A730[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
