/*
 * lieu99.c — Native hand-port of LIEU99.DLL (level-99 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu99_trig.c (reached through
 * l99_trigger, see lieu99_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). The entry is at
 * 0x10008AE0, the state pointer global is dword_100183DC and the trigger
 * table is dword_100182F0[42].
 *
 * event4 (sub_10001200+), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_100091AC), lieu_pre_scene (sub_1000935C) and
 * the float reset are the shared lieu_common.c; the entry and INIT are
 * identical to LIEU03's. tick_body (sub_100077F7) is level-99 specific.
 *
 * LIEU99 is a developer test room (see lieu99_trig.c): the scene phase
 * ST(0x100) runs 0 -> 1 -> 2 -> 0 as the event timers 0/1 fire, each phase
 * dispatching a fixed cast and a camera.
 */

#include "lieu99_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body — sub_100077F7 (thunk 0x100010B9), level-99 specific.
 * Labels in comments = original addresses.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x7802 — phase 0 */
    if (ST(0x100) == 0) {
        /* 0x780F */
        if (!CB->on_joindre(ST(ST_HERO)) && ST(0x64) == 0) {
            ST(0x64) = 1;
            lieu_action_register("L16D300A", 2, "SG02DIA02", NULL);
            lieu_action_register("L16D301", P_MAOR, "SP17DIA56", "SP17ARR01_10");
            lieu_action_register("L16D304A", 2, "SG00DIA04", NULL);
            lieu_action_register("L16D304B", 2, "SG00DIA05", NULL);
            lieu_action_register("L16D305", P_MAOR, "SP17DIA58", "SP17ARR01_10");
            CB->divers_empile(DV_SPECIAL_MODE, 0, 0);
            CB->animation("SG02ARR13");
            lieu_action_register("L16D311A", 2, "SG00DIA31", "SG02ARR13");
            lieu_action_register("L16D311B", 2, "SG00DIA32", "SG02ARR13");
            lieu_action_register("L16D312A", P_MAOR, "SP17DIA59", "SP17ARR01_10");
            lieu_action_register("L16D312B", P_MAOR, "SP17DIA57", "SP17ARR01_10");
            lieu_action_register("LCOD94", 2, "SG00DIA27", "SG02ARR13");
            CB->divers_empile(DV_SPECIAL_MODE, 0, 0);
            CB->joindre(-ST(ST_HERO), 839, -1573, -1, -1);
            CB->evenement(0, 2000);
        }
        /* 0x798D */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 3) > 750 && ST(0x68) == 0) {
            CB->select_pile(1, 2);
            ST(0x68) = 1;
            CB->divers(DV_LOOK_AT_PLAYER, P_MAOR, 1);
            CB->select_pile(0, 1);
        }
        /* 0x79FA */
        if (ST(V_EVENT_0) == 1) {
            ST(V_EVENT_0) = 0;
            CB->select_pile(2, 2);
            CB->divers_empile(DV_SCREEN, 5, 0);
            if (ST(0x70) == 0) {
                ST(0x70) = 1;
                CB->cinematique("SC01_500");
            } else {
                ST(0x70) = 0;
                CB->cinematique("SC02_500");
            }
            CB->pos_perso(P_MAOR, -1, -1, -1, 4, "", -1, 0);
            CB->pos_perso(P_BOGDARAN, -1, -1, -1, 4, "", -1, 0);
            CB->change_camera(4, CAM_LOCKED);
            CB->stop_joindre(-ST(ST_HERO));
            CB->pos_perso(2, 112, -3133, 0xC000, 1, "ar", 1, 0);
            CB->pos_perso(P_PHEDORIA, 302, -1775, 0x4000, 1, "P26GAR03", 2, 0);
            CB->pos_perso(P_ZED27, 21, -1764, 0xC000, 1, "P27GAR02", 3, 0);
            CB->evenement(1, 500);
        }
        /* 0x7B61 */
        if (ST(V_EVENT_1) == 1) {
            ST(V_EVENT_1) = 0;
            ST(0x100) = 1;
            ST(0x64) = 0;
            ST(0x68) = 0;
            CB->divers(DV_MUSIC, 1, 4);
            CB->divers(DV_MUSIC, 2, 0);
            CB->divers_empile(DV_SCREEN, 3, 0);
            CB->divers_empile(DV_MUSIC, 2, 1);
            CB->divers_empile(DV_CINEMATIC, 1, 2);
            CB->select_pile(0, 1);
        }
    }
    /* 0x7C10 — phase 1 */
    if (ST(0x100) == 1) {
        /* 0x7C22 */
        if (ST(0x64) == 0) {
            ST(0x64) = 1;
            CB->evenement(0, 10000);
        }
        /* 0x7C4A */
        if (ST(V_EVENT_0) == 1) {
            ST(V_EVENT_0) = 0;
            CB->divers_empile(DV_SCREEN, 5, 0);
            if (ST(0x70) == 0) {
                ST(0x70) = 1;
                CB->cinematique("SC01_500");
            } else {
                ST(0x70) = 0;
                CB->cinematique("SC02_500");
            }
            CB->divers_empile(DV_CINEMATIC, 0, 0);
            CB->pos_perso(P_PHEDORIA, -1, -1, -1, 4, "", -1, 0);
            CB->pos_perso(P_ZED27, -1, -1, -1, 4, "", -1, 0);
            CB->change_camera(3, CAM_LOCKED);
            CB->pos_perso(2, 800, -1556, 0xC499, 1, "ar", 1, 0);
            CB->divers_empile(DV_USE_ITEM, 24, 0);
            CB->pos_perso(P_LEONA, 1230, -1717, 0, 1, "P14ARR01", 3, 0);
            if (ST(0x70) != 0) {
                CB->pos_perso(P_MALROT2, 1287, -1524, 0x8585, 1, "P62ATT02", 2, 0);
            } else {
                CB->pos_perso(P_MALROT2, 1287, -1524, 0x8585, 1, "P62ATT04", 2, 0);
            }
            CB->evenement(1, 500);
        }
        /* 0x7DE3 */
        if (ST(V_EVENT_1) == 1) {
            CB->select_pile(0, 2);
            ST(V_EVENT_1) = 0;
            ST(0x100) = 2;
            ST(0x64) = 0;
            ST(0x68) = 0;
            CB->divers(DV_MUSIC, 2, 4);
            CB->divers(DV_MUSIC, 3, 0);
            CB->divers_empile(DV_SCREEN, 3, 0);
            CB->divers_empile(DV_MUSIC, 3, 1);
            CB->stop_joindre(-ST(ST_HERO));
            CB->joindre(ST(ST_HERO), 1115, -1556, -1, -1);
            CB->select_pile(0, 1);
        }
    }
    /* 0x7EC1 — phase 2 */
    if (ST(0x100) == 2) {
        /* 0x7ED3 */
        if (CB->distance(1, 3) < 400 && ST(0x74) == 0 && CB->divers(DV_PERSONA_PRESENT, P_LEONA, 0) && CB->on_joindre(ST(ST_HERO))) {
            CB->select_pile(2, 2);
            ST(0x74) = 1;
            CB->divers(DV_LOOK_AT_PLAYER, P_LEONA, 1);
            CB->select_pile(0, 1);
        }
        /* 0x7F6B */
        if (CB->distance(1, 2) < 250 && ST(0x64) == 0 && CB->divers(DV_PERSONA_PRESENT, P_MALROT2, 0) && CB->on_joindre(ST(ST_HERO))) {
            CB->select_pile(2, 2);
            ST(0x64) = 1;
            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT2, 1);
            CB->animation("SP62ARR01");
            CB->joindre(P_MALROT2, 0, 0, 0, ST(ST_HERO));
            CB->animation("SP62ARR01");
            CB->select_pile(0, 1);
        }
        /* 0x8036 */
        if (!CB->on_joindre(P_MALROT2) && !CB->on_joindre(ST(ST_HERO)) && ST(0x64) == 1) {
            ST(0x64) = 2;
            CB->joindre(ST(ST_HERO), 0, 0, 0, P_MALROT2);
            CB->animation("Sar_50");
            lieu_action_register("L18D184", 2, "Sar", NULL);
            lieu_action_register("L18D187", P_MALROT2, "SP62ATT60", "SP62ARR01");
            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT2, 0);
            lieu_pre_scene(ST(ST_HERO), P_MALROT2, 24, 1, 2);
            CB->modify_var(&ST(0x6C), 1);
        }
        /* 0x8111 */
        if (ST(0x6C) == 1) {
            ST(0x6C) = 0;
            CB->animation("P62ACT01");
            CB->animation("P62ARR01");
            CB->dialogue("L18D189", P_MALROT2);
            CB->animation("SP62ACT01");
            CB->animation("P62ARR01");
            CB->dialogue("L18D188", P_MALROT2);
            CB->animation("SG02PRE80");
            CB->animation("G02PRE81");
            CB->animation("SP62PRE60");
            CB->animation("G02PRE82");
            CB->animation("P62ARR01");
            CB->animation("G02ARR01");
            lieu_action_register("L18D153A", P_MALROT2, "SP62ATT60", "SP62ARR01_10");
            CB->evenement(0, 2000);
        }
        /* 0x81E0 */
        if (ST(V_EVENT_0) == 1) {
            ST(V_EVENT_0) = 0;
            CB->divers_empile(DV_SCREEN, 5, 0);
            if (ST(0x70) == 0) {
                ST(0x70) = 1;
                CB->cinematique("SC01_500");
            } else {
                ST(0x70) = 0;
                CB->cinematique("SC02_500");
            }
            CB->divers(DV_USE_ITEM, 0, 0);
            CB->pos_perso(P_MALROT2, -1, -1, -1, 4, "", -1, 0);
            CB->pos_perso(P_LEONA, -1, -1, -1, 4, "", -1, 0);
            CB->change_camera(7, CAM_LOCKED);
            CB->pos_perso(2, 1506, -699, 0x4000, 1, "ar", 1, 0);
            CB->pos_perso(P_MAOR, 1150, -1018, 0x3A3C, 1, "P17ARR01", 2, 0);
            CB->pos_perso(P_BOGDARAN, 968, -1070, 0xC0A5, 1, "P08ARR01", 3, 0);
            CB->divers_empile(DV_CINEMATIC, 0, 0);
            CB->evenement(1, 500);
        }
        /* 0x8340 */
        if (ST(V_EVENT_1) == 1) {
            ST(V_EVENT_1) = 0;
            ST(0x100) = 0;
            ST(0x64) = 0;
            ST(0x68) = 0;
            ST(0x6C) = 0;
            ST(0x74) = 0;
            CB->divers(DV_MUSIC, 3, 4);
            CB->divers(DV_MUSIC, 1, 0);
            CB->divers_empile(DV_SCREEN, 3, 0);
            CB->divers_empile(DV_MUSIC, 1, 1);
            CB->joindre(-ST(ST_HERO), 946, -931, 0xAE1E, -1);
            CB->animation("Sar_50");
            CB->select_pile(2, 2);
            lieu_action_register("L05D110A", P_BOGDARAN, "SP08DIA60", "SP08ARR03_20");
            lieu_action_register("L05D110B", P_BOGDARAN, "SP08DIA56", "SP08ARR03_20");
            CB->animation("SP08ARR03");
            CB->joindre(-P_BOGDARAN, 839, -1573, -1, -1);
            CB->select_pile(0, 1);
        }
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x10008E79).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001845C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu99_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x100090A2);
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
        l99_trig00();   /* 0x100090A9: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x10008F7C — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x100090FC): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x10008AE0).
 * Jump table @ 0x10009179, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu99_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu99: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l99_trigger(arg);             break;  /* dword_100182F0[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
