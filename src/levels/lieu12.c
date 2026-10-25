/*
 * lieu12.c — Native hand-port of LIEU12.DLL (level-12 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu12_trig.c (reached through
 * l12_trigger, see lieu12_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Same compiler/layout as LIEU01..LIEU09; the
 * entry is at 0x10011DD0, the state pointer global is dword_100245BC and the
 * trigger table is dword_100244D0[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1001249C), lieu_pre_scene and sub_10001200
 * (float reset) are the shared lieu_common.c; the entry and INIT follow
 * LIEU03. tick_body (sub_10007C29) is level-12 specific.
 */

#include <stdio.h>
#include <stdlib.h>

#include "lieu12_int.h"

/* Global sprintf buffer of the debug overlay (dword_100242D0). */
static c8 g_txt[256];

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10007C29 (thunk 0x100010B9), level-12 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue (0x10008B38).
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x7C2F — debug overlay (sprintf into the global dword_100242D0) */
    snprintf(g_txt, sizeof(g_txt), "ONJOINDRE:%d", CB->on_joindre(P_BANDOR));
    CB->print_msg(g_txt, 0, 240);
    /* 0x7C5F */
    if (ST(0x90) == 1 && ST(0xC1C) == 2 && !CB->on_joindre(P_BANDOR)) {
        ST(0x90) = 2;
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7CAD */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        /* 0x7CCE: Bandor's facing in [0xF618, 0x10000] or [0, 0x2328]; the
         * original re-calls Divers for every comparison */
        if (ST(0xC1C) == 2 &&
            ((CB->divers(DV_DESC_POS, P_BANDOR, 2) >= 0xF618 && CB->divers(DV_DESC_POS, P_BANDOR, 2) <= 0x10000) ||
             (CB->divers(DV_DESC_POS, P_BANDOR, 2) >= 0 && CB->divers(DV_DESC_POS, P_BANDOR, 2) <= 0x2328)) &&
            ST(0x94) == 0) {
            /* 0x7D63 */
            CB->joindre(P_BANDOR, 7147, -1562, 0xFFA6, -1);
            ST(0x94) = 1;
            CB->joindre(P_KHORAD, 7356, -985, 0x8906, -1);
            CB->animation("P07ARR01_10");
            /* 0x7DB5 */
            if (!CB->on_joindre(P_BANDOR)) {
                CB->joindre(P_BANDOR, 7294, -1026, 0x9513, -1);
            }
            CB->modify_var(&ST(0x90), 1);
            /* 0x7DFB */
            if (ST(0x180) == 0) {
                CB->change_camera(2, CAM_AUTO);
            }
        } else {
            /* 0x7E22 */
            CB->joindre(P_KHORAD, 7356, -985, 0x8906, -1);
            CB->joindre(P_BANDOR, 7294, -1026, 0x9513, -1);
            CB->modify_var(&ST(0x90), 1);
            /* 0x7E71 */
            if (ST(0x180) == 0) {
                CB->change_camera(2, CAM_AUTO);
            }
        }
    }
    /* 0x7E93 */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->change_camera(99, CAM_AUTO);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 1);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7EFA */
    if (ST(ST_LOCKED_CAMERA) == 2 && CB->distance(1, 2) < 300 && ST(0x80C) == 0 && ST(0x100) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        ST(0x100) = 666;
        CB->animation("SP07MAR29");
        CB->animation("SP83GAR01_500");
        CB->animation("P83ATT01_500");
        CB->animation("P07ATT02_300");
    }
    /* 0x7FAA */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->joindre(ST(ST_HERO), 0, 0, 0x317A, -1);
        CB->change_camera(4, CAM_LOCKED);
        CB->change_camera(4, CAM_AUTO);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        /* 0x8023 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L12D108", P_ARKHAN2);
        }
        /* 0x8045 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L12D109", P_ARKHAN3);
        }
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x8074 */
    if ((CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) || CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD && (CB->direction_perso(P_BANDOR) || CB->direction_perso(P_KHORAD))) {
        /* 0x8140 */
        ST(V_CRI_USED) = 1;
        /* 0x814F */
        if (ST(0x74) != 99) {
            ST(0x74) = 83;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(33, 1000);
        return;
    }
    /* 0x818A */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        /* 0x81AB */
        if (ST(0x74) == 83) {
            ST(0x74) = 99;
            ST(V_DLG_RANDOM) = CB->random(2);
            /* 0x81DD */
            if (ST(V_DLG_RANDOM) == 0) {
                CB->dialogue("L12D128", P_BANDOR);
            }
            /* 0x81FF */
            if (ST(V_DLG_RANDOM) == 1) {
                CB->dialogue("L12D129", P_KHORAD);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(V_CRI_USED), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x824D */
    if (ST(0x74) == 99) {
        ST(0x74) = 83;
    }
    /* 0x8268 */
    if (ST(V_EVENT_34) == 1) {
        ST(V_EVENT_34) = 0;
        CB->play_anim(1, 0);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->evenement(35, 1258);
    }
    /* 0x82C2 */
    if (ST(V_EVENT_35) == 1) {
        ST(V_EVENT_35) = 0;
        CB->divers_empile(DV_SCREEN, 1, 0);
        /* 0x82F5 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->cinematique("SC26_500");
        }
        /* 0x8312 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->cinematique("SC27_500");
        }
        ST(ST_LEVEL) = 12;
        CB->change_piece(11, -1);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x8353 */
    if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD) {
        /* 0x8365 */
        if (ST(V_ENGAGED_PERSONA) == P_BANDOR) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
        /* 0x8380 */
        if (ST(V_ENGAGED_PERSONA) == 0x30F) {
            ST(V_ENGAGED_PERSONA) = P_KHORAD;
        }
    }
    /* 0x839E */
    if (ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
        /* 0x83B0 */
        if (ST(V_ENGAGED_PERSONA) == P_KHORAD) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
        /* 0x83CB */
        if (ST(V_ENGAGED_PERSONA) == 0x30F) {
            ST(V_ENGAGED_PERSONA) = P_BANDOR;
        }
    }
    /* 0x83E9 */
    if (CB->divers(DV_WAS_HIT, P_KHORAD, 0) && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        CB->divers_empile(DV_WAS_HIT, P_KHORAD, 1);
        CB->select_pile(1, 2);
        ST(ST_VIE(P_KHORAD)) = VIE_DEAD;
        CB->combat(P_KHORAD, -1);
        CB->stop_joindre(-P_KHORAD);
        CB->animation("SP83IMP51");
        CB->animation("P83CAD");
        CB->pos_perso(P_KHORAD, 0, 0, 0, 2, "P83CAD", 3, 0);
        /* 0x8493 */
        if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) != P_BANDOR) {
            ST(V_ENGAGED_PERSONA) = P_BANDOR;
            CB->select_pile(2, 2);
            CB->combat(P_BANDOR, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x84EA */
    if (CB->divers(DV_WAS_HIT, P_BANDOR, 0) && ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
        CB->divers_empile(DV_WAS_HIT, P_BANDOR, 1);
        CB->select_pile(2, 2);
        ST(ST_VIE(P_BANDOR)) = VIE_DEAD;
        CB->combat(P_BANDOR, -1);
        CB->stop_joindre(-P_BANDOR);
        CB->animation("SP07IMP51");
        CB->animation("P07CAD");
        CB->pos_perso(P_BANDOR, 0, 0, 0, 2, "P07CAD", 2, 0);
        /* 0x8594 */
        if (ST(ST_VIE(P_KHORAD)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) != P_KHORAD) {
            ST(V_ENGAGED_PERSONA) = P_KHORAD;
            CB->select_pile(1, 2);
            CB->combat(P_KHORAD, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x85EB */
    if (CB->distance(1, 2) < 250 && ST(ST_COMBAT_STANCE) == 1 && ST(0x6C) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        ST(0x6C) = 1;
        CB->select_pile(2, 2);
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        CB->joindre_perm(P_BANDOR, ST(ST_HERO));
        CB->stop_joindre(P_BANDOR);
        lieu_action_register("L12D112", P_BANDOR, "SP07GAR01", "SP07ATT02_10");
        CB->bloque(BLOQUE_NONE);
        CB->animation("P07ATT02_1000");
        CB->animation("P07ATT01_10");
        CB->select_pile(1, 2);
        CB->select_pile(1, 1);
        CB->joindre_perm(P_KHORAD, ST(ST_HERO));
        CB->stop_joindre(P_KHORAD);
        CB->animation("SP83GAR01_1000");
        CB->animation("P83ATT02_500");
        CB->select_pile(0, 1);
        return;
    }
    /* 0x874F */
    if (ST(0x6C) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        ST(0x6C) = 0;
        CB->change_camera(-4, CAM_LOCKED);
        CB->select_pile(2, 2);
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        CB->joindre_perm(P_BANDOR, ST(ST_HERO));
        CB->stop_joindre(P_BANDOR);
        lieu_action_register("L12D113", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
        CB->bloque(BLOQUE_NONE);
        CB->change_camera(0, CAM_AUTO);
        CB->select_pile(1, 2);
        CB->select_pile(1, 1);
        CB->joindre_perm(P_KHORAD, ST(ST_HERO));
        CB->stop_joindre(P_KHORAD);
        CB->animation("SP83ARR01");
        CB->select_pile(0, 1);
        return;
    }
    /* 0x88A3 */
    if (ST(ST_LOCKED_CAMERA) == 3 && ST(0x4BC) == 1 && ST(0x78) == 1) {
        CB->select_pile(2, 1);
        CB->pause_anim(0, 7, 0);
        ST(0x78) = 0;
        CB->select_pile(0, 1);
    }
    /* 0x890E */
    if (ST(ST_LOCKED_CAMERA) != 3 && ST(0x78) == 0) {
        ST(0x78) = 1;
    }
    /* 0x893B */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(ST_LOCKED_CAMERA) == 0 && ST(0x80) == 0) {
        CB->select_pile(1, 1);
        CB->pause_anim(1, 1, 0);
        ST(0x80) = 1;
        CB->select_pile(0, 1);
    }
    /* 0x89AF */
    if (ST(ST_LOCKED_CAMERA) != 0 && ST(0x80) == 1) {
        ST(0x80) = 0;
    }
    /* 0x89E2 */
    if (ST(ST_HERO) == 4 && ST(0x7C) == 0) {
        ST(0x7C) = 1;
        CB->evenement(EV_END_GAME, 3000);
    }
    /* 0x8A1F */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        /* 0x8A40 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x8A66 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x8A92 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x8ABE */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x8AEA */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x8B07 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
        return;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1001216D).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1002463C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu12_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x10012394);
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
        l12_trig00();   /* 0x10012399: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1001227E — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x100123EC): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x10011DD0).
 * Jump table @ 0x10012469, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu12_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu12: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l12_trigger(arg);             break;  /* dword_100244D0[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
