/*
 * lieu45.c — Native hand-port of LIEU45.DLL (level-45 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu45_trig.c (reached through
 * l45_trigger, see lieu45_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x1000A360, the state
 * pointer global is dword_1001A6FC and the trigger table is
 * dword_1001A610[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1000AA2C), lieu_pre_scene and sub_10001200
 * (float reset) are the shared lieu_common.c. The tick body (sub_10007C1A)
 * is level-45 specific. The DLL has no writable globals of its own.
 */

#include "lieu45_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10007C1A (thunk 0x100010B9), level-45 specific.
 * `return` == original `jmp` to the epilogue (0x100081D3).
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x7C2C */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        ST(V_END_GAME_REQ) = 0;
        CB->evenement(44, 1500);
    }
    /* 0x7C7E */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        ST(V_DEATH_EVENT_SENT) = 1;
        /* 0x7CB9 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x7CD6 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x7D02 */
    if (ST(V_EVENT_2) == 1) {
        CB->select_pile(1, 1);
        ST(V_EVENT_2) = 0;
        CB->anim_objet("IM23", 1, 50, 475, 0x400);
        CB->etat_objet("IM23", -2030, 38, -393, 0, 0, 0, 0, 3);
        CB->select_pile(0, 1);
        CB->animation("ar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x7DA1 */
    if (ST(V_DELIA_PHASE) == 9) {
        ST(V_DELIA_PHASE) = 10;
    }
    /* 0x7DC2 */
    if (ST(V_DELIA_PHASE) == 10 && ST(ST_VIE(P_DELIA)) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        /* 0x7DFE */
        if (CB->distance(7, 1) < 140 && ST(0x74) == 0) {
            /* 0x7E28 */
            if (CB->divers(DV_RUN, P_DELIA, 1) == 1) {
                CB->divers(DV_RUN, P_DELIA, 0);
            }
            ST(0x74) = 1;
            CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
        }
        /* 0x7E72 */
        if (ST(0x70) == 1 && (ST(0x38) % 20) == 0) {
            /* 0x7EA5 */
            if (CB->distance(7, 1) >= 180) {
                ST(0x74) = 0;
                CB->divers(DV_RUN, P_DELIA, 0);
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
            /* 0x7EFE */
            if (CB->distance(7, 1) >= 300) {
                ST(0x74) = 0;
                CB->divers(DV_RUN, P_DELIA, 1);
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
        }
    }
    /* 0x7F4E */
    if (ST(ST_VIE(P_DELIA)) == VIE_DEAD && ST(V_DELIA_KILLED) == 0) {
        ST(V_DELIA_KILLED) = 45;
        ST(V_DELIA_PHASE) = 20;
        ST(V_ENGAGED_PERSONA) = 0;
    }
    /* 0x7FA4 */
    if (CB->divers(DV_WAS_HIT, P_DELIA, 0) && ST(ST_VIE(P_DELIA)) != VIE_DEAD) {
        ST(V_DELIA_PHASE) = 20;
        ST(V_DELIA_KILLED) = 45;
        ST(ST_VIE(P_DELIA)) = VIE_DEAD;
        CB->select_pile(1, 2);
        CB->combat(P_DELIA, -1);
        CB->stop_joindre(-P_DELIA);
        CB->animation("SP10IMP51");
        CB->animation("P10CAD");
        CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10CAD", 1, 0);
        CB->select_pile(0, 1);
    }
    /* 0x805F */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_DELIA) && ST(ST_VIE(P_DELIA)) == 1 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 1)) {
        CB->bloque(BLOQUE_ALL);
        CB->select_pile(1, 2);
        ST(V_CRI_USED) = 1;
        ST(V_EVENT_0) = 0;
        CB->evenement(0, 1000);
    }
    /* 0x8120 */
    if (ST(V_EVENT_0) == 1 && ST(ST_VIE(P_DELIA)) == 1) {
        ST(V_EVENT_0) = 0;
        CB->animation("SP10ARR01_100");
        CB->animation("P10DIA50");
        CB->dialogue("L16D122", P_DELIA);
        CB->animation("SP10GAR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->joindre_perm(P_BOGDARAN, ST(ST_HERO));
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        return;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1000A6F9).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001A77C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu45_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000A924);
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
        l45_trig00();   /* 0x1000A929: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000A80E — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x1000A97C): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x1000A360).
 * Jump table @ 0x1000A9F9, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu45_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu45: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l45_trigger(arg);             break;  /* dword_1001A610[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
