/*
 * lieu22.c — Native hand-port of LIEU22.DLL (level-22 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu22_trig.c (reached through
 * l22_trigger, see lieu22_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x1000A3C0, the state
 * pointer global is dword_1001A93C and the trigger table is
 * dword_1001A850[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register, lieu_pre_scene and sub_10001200 (float reset) are
 * the shared lieu_common.c. tick_body (sub_10007985) is level-22 specific.
 */

#include <stdlib.h>

#include "lieu22_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10007985 (thunk 0x100010B9), level-22 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue (0x100083EC).
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x7997 */
    if (ST(ST_HERO) != 4 && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 20) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 22) && ST(ST_LOCKED_CAMERA) == 3 && CB->distance(1, 21) > 220 && !CB->direction_zone(21) && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) < -6535) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x7A90 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x7AA2 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L22D22", P_ARKHAN2, "SG02DIA01", NULL);
            }
            /* 0x7ACA */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L22D23", P_ARKHAN2, "SG02DIA01", NULL);
            }
        }
        /* 0x7AF2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7B04 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L22D24", P_ARKHAN3, "SG03DIA01", NULL);
            }
            /* 0x7B2C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L22D25", P_ARKHAN3, "SG03DIA01", NULL);
            }
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x7B7D */
    if (ST(V_DEATH_EVENT_SENT) == 1) {
        ST(V_DEATH_EVENT_SENT) = 0;
        /* 0x7BA3 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x7BCC */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x7BF8 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x7C24 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x7C48 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x7C65 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x7C91 */
    if (ST(V_GUARD_FIGHT_DONE) == 1 && ST(ST_LOCKED_CAMERA) == 2) {
        CB->bloque(BLOQUE_ALL);
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0x338) = 2;
        ST(0x348) = 2;
        ST(V_GUARD_WAVE) = 1;
        ST(0xB40) = 4;
        CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
        CB->joindre(-ST(ST_HERO), 4457, -6426, 0x79D5, -1);
        CB->evenement(0, 1000);
    }
    /* 0x7D4D */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        /* 0x7D6E */
        if (ST(0x32C) == 1 && ST(0x330) == 0) {
            ST(0x32C) = 5;
            CB->select_pile(1, 1);
            CB->pos_perso(P_PHEDORIA, 4449, -5941, 0x8974, 1, "P26ARR01", 4, 0);
            CB->joindre(-P_PHEDORIA, 4484, -6208, 0x849B, -1);
            CB->select_pile(0, 1);
            CB->change_piece(-22, 1);
            CB->divers_empile(DV_SCREEN, 1, 0);
            CB->pos_perso(P_RYLSADHAR21, -1, -1, -1, 4, "", -1, 0);
            CB->change_camera(4, CAM_LOCKED);
            CB->divers_empile(DV_MUSIC, 2, 0);
            /* 0x7E62 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->cinematique("SC08_500");
            }
            /* 0x7E7F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->cinematique("SC09_500");
            }
            CB->divers_empile(DV_MUSIC, 2, 1);
            CB->animation("SP02ARR68_50");
            /* 0x7EB9 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("P02ACT68");
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("ar");
            }
            /* 0x7EF3 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("P02ACT69");
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("ar");
            }
            CB->evenement(1, 500);
            ST(0xCE0) = 1;
            return;
        }
        /* 0x7F51 */
        if (ST(0x330) == 1 && ST(0x32C) == 0) {
            ST(0x330) = 5;
            CB->select_pile(1, 1);
            CB->pos_perso(P_PHEDORIA, 4449, -5941, 0x8974, 1, "P26ARR01", 4, 0);
            CB->joindre(-P_PHEDORIA, 4484, -6208, 0x849B, -1);
            CB->select_pile(0, 1);
            CB->change_piece(-22, 2);
            CB->divers_empile(DV_SCREEN, 1, 0);
            CB->pos_perso(P_ZED27, -1, -1, -1, 4, "", -1, 0);
            CB->change_camera(4, CAM_LOCKED);
            CB->divers_empile(DV_MUSIC, 2, 0);
            /* 0x8045 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->cinematique("SC06_500");
            }
            /* 0x8062 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->cinematique("SC07_500");
            }
            CB->divers_empile(DV_MUSIC, 2, 1);
            CB->animation("SP02ARR68_50");
            /* 0x809C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("P02ACT68");
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("ar");
            }
            /* 0x80D6 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("P02ACT69");
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("ar");
            }
            CB->evenement(1, 500);
            ST(0xCE0) = 1;
            return;
        }
    }
    /* 0x8134 */
    if (ST(0x88C) == 0 && ST(0xCE0) == 1 && CB->distance(1, 12) > 250) {
        ST(0x88C) = 1;
        CB->change_camera(-99, CAM_AUTO);
    }
    /* 0x8192 */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 2;
        lieu_action_register("L22D01", P_PHEDORIA, "SP26DIA53", "SP26ARR01");
        CB->joindre(P_PHEDORIA, 4531, -5943, -1, -1);
    }
    /* 0x81E5 */
    if (ST(V_EVENT_1) == 2 && CB->distance(4, 11) < 75) {
        ST(V_EVENT_1) = 0;
        CB->retire_zone(11);
        CB->pos_perso(P_PHEDORIA, -1, -1, -1, 4, "", -1, 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x8251 */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        /* 0x826F */
        if (ST(0x68) < 10) {
            ST(0x68)++;
            CB->select_pile(2, 2);
            CB->evenement(2, 50);
            /* 0x82B2 */
            if (CB->divers(DV_DARKNESS, -1, 0) > 15) {
                CB->divers_empile(DV_DARKNESS, CB->divers(DV_DARKNESS, -1, 0) - 20, 0);
            }
            /* 0x82F5 */
            if (CB->divers(DV_HP, ST(ST_HERO), -1) > 15) {
                CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) - 10));
            }
        }
    }
    /* 0x8339 */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        /* 0x8357 */
        if (ST(0x6C) < 50) {
            ST(0x6C)++;
            CB->select_pile(2, 2);
            CB->evenement(33, 25);
            /* 0x83A0 */
            if (CB->divers(DV_HP, ST(ST_HERO), -1) >= 1) {
                CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) - 25));
            }
            return;
        }
        ST(V_DEATH_EVENT_SENT) = 1;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1000A759).
 * Callback ingestion (0x1000A3C9..0x1000A74F), the trigger-table fill and
 * the unused dword_1001A9BC = state+0xC8 are skipped: g_lieu_cb is provided
 * by the bridge, the trigger table is static in lieu22_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000A982);
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
        l22_trig00();   /* 0x1000A989: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000A86E — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x1000A9DC): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x1000A3C0).
 * Jump table @ 0x1000AA59, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu22_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu22: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l22_trigger(arg);             break;  /* dword_1001A850[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
