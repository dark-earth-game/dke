/*
 * lieu50.c - Native hand-port of LIEU50.DLL (level-50 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick, the tick body and the
 * event4 / event5 / event6 dispatch
 * (the handlers are the shared older-version ones, lieu_common_v1.c). The 42 zone trigger handlers live in
 * lieu50_trig.c (reached through l50_trigger, see lieu50_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. LIEU50 is a variant of the level-12 script
 * (same rooms, dialogue codes and zones) linked with a different layout: the
 * entry is at 0x10010220, the state pointer global is dword_1002248C and the
 * trigger table is dword_100223A0[42].
 *
 * Its event4 (sub_1000AE60), event5 (sub_1000BBFF) and event6
 * (sub_1000EED3) are the same script version as LIEU33's (older than the
 * lieu_common.c ones, plus a V_END_GAME_REQ case in event4 and a level 28
 * test in the parchment case of event5), so they are the lieu_common_v1.c ones (newer=1);
 * lieu_action_register is the shared one. The hero pre-scene sub_10010876
 * lacks the `n != 6` test of lieu_pre_scene, so lieu_common_v1.c carries it (lieu_v1_pre_scene).
 * The DLL .data the level code touches: the four event4 float accumulators
 * (initially 1.0f) and the two event5 sprintf buffers (dword_100224F0 /
 * dword_100224C0); all are reset at INIT.
 *
 * Note: trigger 0 tests ST(0x990), the start point slot of level 12 (the
 * script was copied from LIEU12 without changing the slot).
 */

#include <stdio.h>
#include <stdlib.h>

#include "lieu50_int.h"


/* -------------------------------------------------------------------------
 * Per-frame body — sub_100019B2 (thunk 0x100010B4), level-50 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue (0x100025FD).
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->change_camera(99, CAM_AUTO);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 1);
        CB->bloque(BLOQUE_NONE);
    }
    if (ST(ST_LOCKED_CAMERA) == 2 && CB->distance(1, 2) < 300 && ST(0x80C) == 0 && ST(0x100) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        ST(0x100) = 666;
        CB->animation("SP07MAR29");
        CB->animation("SP83GAR01_500");
        CB->animation("P83ATT01");
        CB->animation("P07ATT02");
    }
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->joindre(ST(ST_HERO), 0, 0, 0x317A, -1);
        CB->change_camera(4, CAM_LOCKED);
        CB->change_camera(4, CAM_AUTO);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L12D108", P_ARKHAN2);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L12D109", P_ARKHAN3);
        }
        CB->bloque(BLOQUE_NONE);
        return;
    }
    if ((CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) || CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD && (CB->direction_perso(P_BANDOR) || CB->direction_perso(P_KHORAD))) {
        ST(V_CRI_USED) = 1;
        if (ST(0x74) != 99) {
            ST(0x74) = 83;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(33, 1000);
        return;
    }
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        if (ST(0x74) == 83) {
            ST(0x74) = 99;
            ST(V_DLG_RANDOM) = CB->random(2);
            if (ST(V_DLG_RANDOM) == 0) {
                CB->dialogue("L12D128", P_BANDOR);
            }
            if (ST(V_DLG_RANDOM) == 1) {
                CB->dialogue("L12D129", P_KHORAD);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(V_CRI_USED), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    if (ST(0x74) == 99) {
        ST(0x74) = 83;
    }
    if (ST(V_EVENT_34) == 1) {
        ST(V_EVENT_34) = 0;
        CB->play_anim(1, 0);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->cinematique("SC26_500");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->cinematique("SC27_500");
        }
        ST(ST_LEVEL) = 12;
        CB->bloque(BLOQUE_NONE);
        CB->change_piece(11, -1);
    }
    if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD) {
        if (ST(V_ENGAGED_PERSONA) == P_BANDOR) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
        if (ST(V_ENGAGED_PERSONA) == 0x30F) {
            ST(V_ENGAGED_PERSONA) = P_KHORAD;
        }
    }
    if (ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
        if (ST(V_ENGAGED_PERSONA) == P_KHORAD) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
        if (ST(V_ENGAGED_PERSONA) == 0x30F) {
            ST(V_ENGAGED_PERSONA) = P_BANDOR;
        }
    }
    if (CB->divers(DV_WAS_HIT, P_KHORAD, 0) && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        CB->divers_empile(DV_WAS_HIT, P_KHORAD, 1);
        CB->select_pile(1, 2);
        ST(ST_VIE(P_KHORAD)) = VIE_DEAD;
        CB->combat(P_KHORAD, -1);
        CB->stop_joindre(-P_KHORAD);
        CB->animation("SP83IMP51");
        CB->animation("P83CAD");
        CB->pos_perso(P_KHORAD, 0, 0, 0, 2, "P83CAD", 3, 0);
        if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) != P_BANDOR) {
            ST(V_ENGAGED_PERSONA) = P_BANDOR;
            CB->select_pile(2, 2);
            CB->combat(P_BANDOR, 0);
        }
        CB->select_pile(0, 1);
    }
    if (CB->divers(DV_WAS_HIT, P_BANDOR, 0) && ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
        CB->divers_empile(DV_WAS_HIT, P_BANDOR, 1);
        CB->select_pile(2, 2);
        ST(ST_VIE(P_BANDOR)) = VIE_DEAD;
        CB->combat(P_BANDOR, -1);
        CB->stop_joindre(-P_BANDOR);
        CB->animation("SP07IMP51");
        CB->animation("P07CAD");
        CB->pos_perso(P_BANDOR, 0, 0, 0, 2, "P07CAD", 2, 0);
        if (ST(ST_VIE(P_KHORAD)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) != P_KHORAD) {
            ST(V_ENGAGED_PERSONA) = P_KHORAD;
            CB->select_pile(1, 2);
            CB->combat(P_KHORAD, 0);
        }
        CB->select_pile(0, 1);
    }
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
        CB->animation("SP83GAR01_3000");
        CB->animation("P83ATT02_2000");
        CB->animation("P83DIA50_10");
        CB->select_pile(0, 1);
        return;
    }
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
    if (ST(ST_LOCKED_CAMERA) == 3 && ST(0x4BC) == 1 && ST(0x78) == 1) {
        CB->pause_anim(0, 7, 0);
        ST(0x78) = 0;
    }
    if (ST(ST_LOCKED_CAMERA) != 3 && ST(0x78) == 0) {
        ST(0x78) = 1;
    }
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(ST_LOCKED_CAMERA) == 0 && ST(0x80) == 0) {
        CB->select_pile(1, 1);
        CB->pause_anim(1, 1, 0);
        ST(0x80) = 1;
        CB->select_pile(0, 1);
    }
    if (ST(ST_LOCKED_CAMERA) != 0 && ST(0x80) == 1) {
        ST(0x80) = 0;
    }
    if (ST(ST_HERO) == 4 && ST(0x7C) == 0) {
        ST(0x7C) = 1;
        CB->evenement(EV_END_GAME, 3000);
    }
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
        return;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x1001023A).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1002250C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu50_trig.c. There is no death
 * branch in this version.
 *   arg = reload flag (non-zero => skip arming).
 * ---------------------------------------------------------------------- */
static void init(i32 reload_flag) {
    if (reload_flag == 0)
        l50_trig00();   /* 0x100105C3: call sub_1000106E -> sub_10001200 */
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1, 0x10010616): drain pending sound, run body.
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

/* The DLL is loaded afresh for every room: its globals restart. Called by
 * INIT, and by tools/lieu_difftest_harness.c before every case. */
void l50_reset_dll_globals(void) {
    lieu_v1_reset();
}

/* -------------------------------------------------------------------------
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x10010220).
 * Jump table @ 0x10010693, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu50_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu50: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        l50_reset_dll_globals();
        init(arg);
        break;
    case LIEU_OP_EVENT:  l50_trigger(arg);             break;  /* dword_100223A0[arg]() */
    case LIEU_OP_CALL4:  lieu_v1_event4(arg, 1);            break;
    case LIEU_OP_CALL5:  lieu_v1_event5(arg, arg2, 1);      break;
    case LIEU_OP_CALL6:  lieu_v1_event6(arg);                   break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
