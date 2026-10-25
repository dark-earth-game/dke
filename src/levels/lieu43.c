/*
 * lieu43.c — Native hand-port of LIEU43.DLL (level-43 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu43_trig.c (reached through
 * l43_trigger, see lieu43_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x10009D20, the state
 * pointer global is dword_1001923C and the trigger table is
 * dword_10019150[42].
 *
 * event4 (sub_10003B6A), event5 (sub_10004BE4), event6 (sub_1000851C),
 * lieu_action_register (sub_1000A3EC), lieu_pre_scene and sub_10003B30
 * (float reset) are the shared lieu_common.c. The tick body (sub_10001521)
 * is level-43 specific. The DLL has .data globals (the two lever states),
 * reset at every INIT.
 */

#include <stdio.h>

#include "lieu43_int.h"

/* Global sprintf buffer of the debug overlay (dword_10018F50, starts ""). */
c8 g_l43_txt[256];

/* .data globals of the DLL (restart on every room load). */
i32 g_l43_hits;        /* dword_10015A30 */
i32 g_l43_left_pos = 666;   /* dword_10015A38 */
i32 g_l43_right_pos = 666;  /* dword_10015A3C */
i32 g_l43_left_init;   /* dword_10015A40 */
i32 g_l43_right_init;  /* dword_10015A44 */
i32 g_l43_cur_left;    /* dword_10015A48 */
i32 g_l43_cur_right;   /* dword_10015A4C */
i32 g_l43_flag[10];    /* dword_10015A50..A74 */

/* The DLL is loaded afresh for every room: its globals restart. Called by
 * INIT, and by tools/lieu_difftest_harness.c before every case. */
void l43_reset_dll_globals(void) {
    g_l43_hits = 0;
    g_l43_left_pos = g_l43_right_pos = 666;
    g_l43_left_init = g_l43_right_init = 0;
    g_l43_cur_left = g_l43_cur_right = 0;
    for (i32 i = 0; i < 10; i++)
        g_l43_flag[i] = 0;
    g_l43_txt[0] = '\0';
}

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10001521 (thunk 0x100010B9), level-43 specific.
 * `return` == original `jmp` to the epilogue (0x100021B2).
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x152A */
    if (ST(0xE3C) == 0) {
        ST(0xE3C) = 1;
    }
    snprintf(g_l43_txt, sizeof(g_l43_txt), "current_gauche=%d", g_l43_cur_left);
    CB->print_msg(g_l43_txt, 0, 200);
    snprintf(g_l43_txt, sizeof(g_l43_txt), "current_droit=%d", g_l43_cur_right);
    CB->print_msg(g_l43_txt, 0, 208);
    /* 0x159F — persona 40 was hit (first reaction, counter < 4) */
    if (CB->divers(DV_WAS_HIT, 40, 0) && ST(0xEC) == 0 && !CB->divers(DV_P40_TIMER, 0, 0)) {
        i32 loc_m4 = g_l43_hits;
        g_l43_hits++;
        /* 0x15F0 */
        if (loc_m4 < 4) {
            CB->select_pile(0, 2);
            CB->divers_empile(DV_WAS_HIT, 40, 1);
            CB->combat(40, -1);
            CB->divers(DV_HP, 40, CB->divers(DV_HP, 40, -1) - 50);
            CB->animation("SP40IMP01");
            CB->animation("P40GAR01");
            CB->combat(40, 0);
            CB->select_pile(0, 1);
        }
    }
    /* 0x167F */
    if ((CB->divers(DV_WAS_HIT, 40, 0) && g_l43_hits == 4 && ST(0xEC) == 0)
        || (ST(ST_PLAYER_WEAPON) == 27 && CB->divers(DV_WAS_HIT, 40, 0) && !CB->divers(DV_P40_TIMER, 0, 0) && ST(0xEC) == 0)) {
        /* 0x170D */
        CB->select_pile(0, 2);
        CB->divers(DV_WAS_HIT, 40, 1);
        g_l43_hits = 0;
        CB->combat(40, -1);
        CB->divers(DV_P40_TIMER, 1, 1);
        CB->animation("SP40IMP11");
        CB->animation("P40IMP46");
        CB->animation("P40GAR02");
        CB->divers_empile(DV_P40_TIMER, 1, 2);
        CB->select_pile(0, 1);
    }
    /* 0x1798 */
    if ((CB->divers(DV_WAS_HIT, 40, 0) && CB->divers(DV_P40_TIMER, 0, 0) == 2 && ST(0xEC) == 0)
        || (ST(ST_PLAYER_WEAPON) == 27 && CB->divers(DV_WAS_HIT, 40, 0) && CB->divers(DV_P40_TIMER, 0, 0) == 2 && ST(0xEC) == 0)) {
        /* 0x1832 */
        CB->select_pile(0, 2);
        CB->divers(DV_WAS_HIT, 40, 1);
        CB->animation("SP40IMP02");
        CB->animation("P40GAR02");
        CB->select_pile(0, 1);
    }
    /* 0x1874 */
    if (ST(ST_HERO) == 4 && ST(0x90) == 0) {
        ST(0x90) = 1;
        CB->evenement(EV_END_GAME, 3000);
    }
    /* 0x18B7 */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        /* 0x18D8 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x18FE */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x192A */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x1956 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x1982 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x199F */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
        return;
    }
    /* 0x19D0 — lever animations start when the locked camera 4 is entered */
    if (ST(ST_LOCKED_CAMERA) == 4 && ST(0x6C) == 0) {
        CB->select_pile(1, 1);
        CB->play_anim(0, 0);
        CB->select_pile(2, 1);
        CB->play_anim(1, 0);
        CB->select_pile(0, 1);
        ST(0x6C) = 1;
    }
    /* 0x1A41 */
    if (ST(ST_LOCKED_CAMERA) != 4) {
        /* 0x1A53 */
        if (ST(0x6C) == 1) {
            ST(0x6C) = 2;
        }
        /* 0x1A6E */
        if (ST(0x6C) == 3) {
            ST(0x6C) = 2;
        }
    }
    /* 0x1A89 */
    if (ST(ST_LOCKED_CAMERA) == 4 && ST(0x6C) == 2) {
        CB->pause_anim(0, 4, 0);
        CB->pause_anim(1, 4, 0);
        CB->select_pile(2, 2);
        /* 0x1ADB */
        if (g_l43_cur_left == 6) {
            CB->pause_anim(6, 3, 0);
        }
        /* 0x1AFA */
        if (g_l43_cur_left == 7) {
            CB->pause_anim(7, 3, 0);
        }
        /* 0x1B19 */
        if (g_l43_cur_left == 8) {
            CB->pause_anim(8, 3, 0);
        }
        /* 0x1B38 */
        if (g_l43_cur_right == 12) {
            CB->pause_anim(12, 3, 0);
        }
        /* 0x1B57 */
        if (g_l43_cur_right == 13) {
            CB->pause_anim(13, 3, 0);
        }
        /* 0x1B76 */
        if (g_l43_cur_right == 14) {
            CB->pause_anim(14, 3, 0);
        }
        ST(0x6C) = 3;
        CB->select_pile(0, 1);
    }
    /* 0x1BAE */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->select_pile(2, 2);
        /* 0x1BDF */
        if (G_A54 != 0) {
            CB->play_anim(6, 0);
        }
        /* 0x1BF9 */
        if (G_A50 != 0 || G_A58 != 0 || G_A60 != 0) {
            /* 0x1C20 */
            if (G_A50 != 0) {
                G_A50 = 0;
            }
            CB->play_anim(7, 0);
        }
        /* 0x1C44 */
        if (G_A5C != 0) {
            CB->play_anim(8, 0);
        }
        /* 0x1C5E */
        if (G_A68 != 0) {
            CB->play_anim(12, 0);
        }
        /* 0x1C78 */
        if (G_A64 != 0 || G_A6C != 0 || G_A74 != 0) {
            /* 0x1C9F */
            if (G_A64 != 0) {
                G_A64 = 0;
            }
            CB->play_anim(13, 0);
        }
        /* 0x1CC3 */
        if (G_A70 != 0) {
            CB->play_anim(14, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x1CEA */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->select_pile(2, 2);
        /* 0x1D15 */
        if (G_A54 != 0) {
            CB->play_anim(10, 0);
        }
        /* 0x1D35 */
        if (G_A58 != 0) {
            CB->play_anim(9, 0);
        }
        /* 0x1D4F */
        if (G_A5C != 0) {
            CB->play_anim(10, 0);
        }
        /* 0x1D69 */
        if (G_A60 != 0) {
            CB->play_anim(11, 0);
        }
        /* 0x1D83 */
        if (G_A68 != 0) {
            CB->play_anim(16, 0);
        }
        /* 0x1D9D */
        if (G_A6C != 0) {
            CB->play_anim(15, 0);
        }
        /* 0x1DB7 */
        if (G_A70 != 0) {
            CB->play_anim(16, 0);
        }
        /* 0x1DD1 */
        if (G_A74 != 0) {
            CB->play_anim(17, 0);
        }
        /* 0x1DEB */
        if (G_A54 != 0) {
            CB->free_anim(7);
        }
        /* 0x1E03 */
        if (G_A58 != 0) {
            CB->free_anim(6);
        }
        /* 0x1E1B */
        if (G_A5C != 0) {
            CB->free_anim(7);
        }
        /* 0x1E33 */
        if (G_A60 != 0) {
            CB->free_anim(8);
        }
        /* 0x1E4B */
        if (G_A68 != 0) {
            CB->free_anim(13);
        }
        /* 0x1E63 */
        if (G_A6C != 0) {
            CB->free_anim(12);
        }
        /* 0x1E7B */
        if (G_A70 != 0) {
            CB->free_anim(13);
        }
        /* 0x1E93 */
        if (G_A74 != 0) {
            CB->free_anim(14);
        }
        CB->evenement(2, 1000);
        CB->select_pile(0, 1);
    }
    /* 0x1EC8 */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        CB->select_pile(2, 2);
        /* 0x1EF9 */
        if (G_A54 != 0) {
            G_A54 = 0;
            CB->free_anim(10);
        }
        /* 0x1F1B */
        if (G_A58 != 0) {
            G_A58 = 0;
            CB->free_anim(9);
        }
        /* 0x1F3D */
        if (G_A5C != 0) {
            G_A5C = 0;
            CB->free_anim(10);
        }
        /* 0x1F5F */
        if (G_A60 != 0) {
            G_A60 = 0;
            CB->free_anim(11);
        }
        /* 0x1F81 */
        if (G_A68 != 0) {
            G_A68 = 0;
            CB->free_anim(16);
        }
        /* 0x1FA3 */
        if (G_A6C != 0) {
            G_A6C = 0;
            CB->free_anim(15);
        }
        /* 0x1FC5 */
        if (G_A70 != 0) {
            G_A70 = 0;
            CB->free_anim(16);
        }
        /* 0x1FE7 */
        if (G_A74 != 0) {
            G_A74 = 0;
            CB->free_anim(17);
        }
        CB->select_pile(0, 1);
    }
    /* 0x2016 — both levers set right: persona 40 turns up */
    if (g_l43_left_pos == -1 && g_l43_right_pos == 1 && ST(0xEC) == 0) {
        ST(0xEC) = 1;
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        CB->combat(40, -1);
        CB->divers(DV_P40_TIMER, 1, 1);
        {
            /* the original evaluates the three position queries z, y, x */
            i32 d2 = CB->divers(DV_DESC_POS, 40, 2);
            i32 d1 = CB->divers(DV_DESC_POS, 40, 1);
            i32 d0 = CB->divers(DV_DESC_POS, 40, 0);
            CB->pos_perso(40, d0, d1, d2, 1, "P40ARR01", -1, 0);
        }
        CB->evenement(35, 3000);
    }
    /* 0x20EB */
    if (ST(V_EVENT_35) == 1) {
        ST(V_EVENT_35) = 0;
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(0, CAM_LOCKED);
        CB->animation("Sar_50");
        CB->divers_empile(DV_BIG_ANIM, 3, 0);
        CB->animation("Sar_50");
        CB->evenement(38, 2500);
    }
    /* 0x215C */
    if (ST(V_EVENT_38) == 1) {
        ST(V_EVENT_38) = 0;
        CB->divers_empile(DV_SCREEN, 5, 0);
        CB->cinematique("SC25_500");
        CB->animation("ar");
        CB->change_piece(52, 0);
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1000A0B9).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_100192BC = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu43_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000A2E2);
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
        l43_trig00();   /* 0x1000A2E9: call sub_10001073 -> trig00_10001200 */
        return 0;
    }
    /* 0x1000A1CE — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x1000A33C): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x10009D20).
 * Jump table @ 0x1000A3B9, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu43_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu43: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        l43_reset_dll_globals();
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l43_trigger(arg);             break;  /* dword_10019150[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
