/*
 * lieu03.c — Native hand-port of LIEU03.DLL (level-3 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu03_trig.c (reached through
 * l03_trigger, see lieu03_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Same compiler/layout as LIEU01/LIEU02; the
 * entry is at 0x1000DD60, the state pointer global is dword_1001DFFC and the
 * trigger table is dword_1001DF10[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register (sub_1000E42C), lieu_pre_scene (sub_1000E5DC) and
 * sub_10001200 (float reset) are the shared lieu_common.c; the entry and
 * INIT are instruction-for-instruction identical to LIEU02. tick_body
 * (sub_100083AD) is level-3 specific.
 */

#include <stdlib.h>

#include "lieu03_int.h"

/* -------------------------------------------------------------------------
 * Engine slots with no typed member in lieu_cb_t yet.
 *   slot 33 (dword_1001DFD8, engine sub_446CBC): PlaceAnimal
 *   slot 35 (dword_1001E004, engine sub_4430B0): LoadAnim
 * Both are void* in lieu_cb_t and NULL in the bridge vtable, so a call is
 * skipped until the engine provides them. The argument lists are the raw
 * __fastcall arguments (ecx, edx, then the stack, last push first).
 * ---------------------------------------------------------------------- */
typedef void (*place_animal_fn)(i32 a, i32 b, i32 c, i32 d, i32 e, i32 f);
typedef void (*load_anim_fn)(const c8 *name, i32 a);

void l03_place_animal(i32 a, i32 b, i32 c, i32 d, i32 e, i32 f) {
    if (CB->place_animal)
        ((place_animal_fn)CB->place_animal)(a, b, c, d, e, f);
}

void l03_load_anim(const c8 *name, i32 a) {
    if (CB->load_anim)
        ((load_anim_fn)CB->load_anim)(name, a);
}

/* -------------------------------------------------------------------------
 * Per-frame body — sub_100083AD (thunk 0x100010B9), level-3 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x83BF */
    if (ST(0x100) == 0 && ST(ST_LOCKED_CAMERA) == 1 && CB->distance(8, 15) < 0x19) {
        ST(0x100) = 1;
        CB->select_pile(2, 1);
        CB->play_anim(0, 0);
        CB->select_pile(0, 1);
    }
    /* 0x8432 */
    if (ST(0x100) == 1 && CB->distance(8, 15) > 0x5A) {
        ST(0x100) = 0;
    }
    /* 0x846C */
    if (ST(ST_HERO) != 4 && ST(0xFC) != 0x63 && ST(0xE9C) < 3 && ST(0xE9C) != 0) {
        /* 0x84BA */
        if ((ST(0x38) % 100) == 0 && ST(0xE0) != -1 && ST(0xF0) != -1 && ST(0xF4) != -1 && ST(0x74) == 0x63) {
            ST(0xE0) = CB->divers(DV_DESC_POS, P_KALHI33, 0);
            ST(0xF0) = CB->divers(DV_DESC_POS, P_KALHI33, 1);
            ST(0xF4) = CB->divers(DV_DESC_POS, P_KALHI33, 2);
        }
        /* 0x856B */
        if (ST(0xE0) != 0 && ST(0xF0) != 0 && ST(0xF4) != 0 && CB->on_joindre(P_KALHI33) && CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0) && ST(ST_LOCKED_CAMERA) == 1 && CB->divers(DV_IDLE_TIME, 0, 0) > 0x7D0) {
            CB->divers(DV_IDLE_TIME, 1, 0);
            /* 0x862B */
            if (abs(ST(0xE0) - CB->divers(DV_DESC_POS, P_KALHI33, 0)) < 0xF && abs(ST(0xF0) - CB->divers(DV_DESC_POS, P_KALHI33, 1)) < 0xF) {
                CB->select_pile(0, 2);
                ST(0xE0) = -1;
                ST(0xF0) = -1;
                ST(0xF4) = -1;
                CB->bloque(BLOQUE_ALL);
                CB->divers_empile(DV_SCREEN, 1, 0);
                CB->change_camera(0, CAM_LOCKED);
                CB->pos_perso(ST(ST_HERO), -1878, -716, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 8, 0);
                CB->pos_perso(P_KALHI33, -1856, -815, 0x85CB, 1, "P33ARR01", 5, 0);
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->evenement(0x23, 0x7D0);
                CB->select_pile(0, 1);
            } else {
                ST(0xE0) = 0;
                ST(0xF0) = 0;
                ST(0xF4) = 0;
            }
        }
    }
    /* 0x8797 */
    if (ST(V_EVENT_35) == 1 && CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0)) {
        ST(V_EVENT_35) = 0;
        CB->change_camera(99, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x87EA */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_DEATH_EVENT_SENT) = 1;
        /* 0x882D */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x8856 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x8882 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x88AE */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        CB->evenement(0x2C, 0xBB8);
        return;
    }
    /* 0x88E7 */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        /* 0x8908 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x8925 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
        return;
    }
    /* 0x8956 */
    if ((ST(ST_VIE(P_KALHI13)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_KALHI13) || (ST(ST_VIE(P_KALHI33)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_KALHI33)) {
        ST(0xE9C) = 2;
        ST(0xE58) = 1;
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = 0;
    }
    /* 0x89D6 */
    if (CB->divers(DV_WAS_HIT, P_KALHI13, 0) && ST(ST_VIE(P_KALHI13)) != VIE_DEAD && CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0)) {
        ST(ST_VIE(P_KALHI13)) = VIE_DEAD;
        ST(0xE58) = 1;
        ST(0xE9C) = 2;
        CB->select_pile(1, 2);
        CB->combat(P_KALHI13, -1);
        CB->stop_joindre(-P_KALHI13);
        CB->animation("SP13IMP52");
        CB->animation("P13CAD");
        CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13CAD", 4, 0);
        CB->select_pile(0, 1);
    }
    /* 0x8AB3 */
    if (CB->divers(DV_WAS_HIT, P_KALHI33, 0) && ST(ST_VIE(P_KALHI33)) != VIE_DEAD && CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0) && ST(0xF3C) == 0) {
        ST(ST_VIE(P_KALHI33)) = VIE_DEAD;
        ST(0xE58) = 1;
        ST(0xE9C) = 2;
        CB->select_pile(1, 2);
        CB->combat(P_KALHI33, -1);
        CB->stop_joindre(-P_KALHI33);
        CB->animation("SP33IMP52");
        CB->animation("P33CAD");
        CB->pos_perso(P_KALHI33, 0, 0, 0, 2, "P33CAD", 5, 0);
        CB->select_pile(0, 1);
    }
    /* 0x8BA2 */
    if (CB->divers(DV_WAS_HIT, P_DELIA, 0) && ST(ST_VIE(P_DELIA)) != VIE_DEAD) {
        ST(ST_VIE(P_DELIA)) = VIE_DEAD;
        ST(0x68) = 0xA;
        ST(V_DELIA_KILLED) = 3;
        ST(V_DELIA_PHASE) = 0x14;
        CB->select_pile(1, 2);
        CB->combat(P_DELIA, -1);
        CB->stop_joindre(-P_DELIA);
        CB->animation("SP10IMP51");
        CB->animation("P10CAD");
        CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10CAD", 0xC, 0);
        CB->select_pile(0, 1);
    }
    /* 0x8C66 */
    if (ST(0x6C) == 0 && (ST(V_DELIA_PHASE) == 8 || ST(V_DELIA_PHASE) == 9)) {
        ST(V_DELIA_PHASE) = 9;
        ST(0x6C) = 1;
        /* 0x8CB7 */
        if (ST(ST_VIE(P_DELIA)) == 1) {
            ST(0x68) = 1;
        }
    }
    /* 0x8CD5 */
    if (ST(0xB40) == 3 && ST(0xE9C) == 0 && ST(ST_VIE(P_KALHI33)) == 1 && CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0)) {
        ST(0xE9C) = 1;
        ST(0x948) = 1;
        CB->select_pile(1, 1);
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP33ARR01_50");
        CB->dialogue("L03D14", P_KALHI33);
        CB->animation("SP33ARR01_50");
        CB->joindre(P_KALHI33, -1791, -1556, -1, -1);
        CB->bloque(BLOQUE_NONE);
        CB->animation("SP33ARR01");
        CB->joindre(P_KALHI33, -1400, -1556, 0xC350, -1);
        CB->select_pile(0, 1);
        return;
    }
    /* 0x8DF0 */
    if (ST(0xB40) == 3 && ST(0xE9C) == 0 && ST(ST_VIE(P_KALHI13)) == 1 && CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0)) {
        ST(0xE9C) = 1;
    }
    /* 0x8E4F */
    if (ST(ST_VIE(P_KALHI13)) == 1 && ST(0xE9C) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->select_pile(1, 1);
        /* 0x8E99 */
        if (CB->distance(8, 4) < 0x5A && ST(0x88) == 0) {
            ST(0x88) = 1;
            ST(0x78) = 1;
            CB->stop_joindre(-P_KALHI13);
            CB->pos_perso(P_KALHI13, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 1, "P13ARR01", 4, 1);
            CB->joindre_perm(P_KALHI13, ST(ST_HERO));
        }
        /* 0x8F46 */
        if (ST(0x7C) == 1 && (ST(0x38) % 20) == 0) {
            /* 0x8F79 */
            if (CB->distance(8, 4) >= 0xC8 && CB->distance(8, 4) < 0x12C) {
                ST(0x88) = 0;
                CB->divers(DV_RUN, P_KALHI13, 0);
                CB->joindre(-P_KALHI13, -1, -1, -1, ST(ST_HERO));
            }
            /* 0x8FF0 */
            if (CB->distance(8, 4) >= 0x12C) {
                ST(0x88) = 0;
                CB->divers(DV_RUN, P_KALHI13, 1);
                CB->joindre(-P_KALHI13, -1, -1, -1, ST(ST_HERO));
            }
        }
        CB->select_pile(0, 1);
    }
    /* 0x9050 */
    if (ST(ST_VIE(P_KALHI33)) == 1 && ST(0xE9C) == 1 && ST(0x90) == 0 && !CB->on_joindre(P_KALHI33) && CB->distance(5, 13) < 0x12C) {
        ST(0x90) = 0x3E8;
        CB->select_pile(1, 1);
        CB->retire_zone(13);
        CB->animation("SP33ARR01_10");
        CB->pos_perso(P_KALHI33, -1, -1, -1, 4, "", -1, 0);
        CB->select_pile(0, 1);
    }
    /* 0x9115 */
    if (ST(0xF3C) == 0 && CB->divers(DV_IN_ZONE, P_KALHI33, 1)) {
        ST(0xF3C) = 2;
    }
    /* 0x9150 */
    if (ST(0xF3C) == 0 && CB->divers(DV_IN_ZONE, P_KALHI33, 6)) {
        ST(0xF3C) = 1;
    }
    /* 0x918B */
    if ((ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1) && ST(V_EVENT15_STEP) == 1 && ST(0xE9C) == 2) {
        /* 0x91DC */
        if (CB->distance(8, 4) < 0x82 && CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0)) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(V_EVENT15_STEP) = 2;
            ST(0xE9C) = 1;
            ST(0x70) = 1;
            CB->animation("SP13LEV01");
            lieu_action_register("L03D500A", P_KALHI13, "P13DIA60", "SP13ARR01_10");
            lieu_action_register("L03D500B", P_KALHI13, "P13DIA60", "SP13ARR01_10");
            ST(V_KALHI13_TALK_C) = 2;
            CB->modify_var(&ST(0x7C), 1);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x92D0 */
        if (CB->distance(8, 5) < 0x82 && CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0)) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(V_EVENT15_STEP) = 2;
            ST(0xE9C) = 1;
            ST(0x70) = 1;
            ST(V_KALHI13_TALK_C) = 2;
            CB->animation("SP33LEV01");
            lieu_action_register("L03D500A", P_KALHI33, "P33DIA60", "SP33ARR01_10");
            lieu_action_register("L03D500B", P_KALHI33, "P33DIA60", "SP33ARR01_10");
            CB->joindre(P_KALHI33, -1340, -1620, 0xC350, -1);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x93C3 */
    if (ST(ST_HERO) != 4 && ST(0xE9C) == 1 && CB->divers(DV_PERSONA_PRESENT, P_KALHI13, 0) && ST(0x74) == 1) {
        ST(0x74) = 2;
        CB->select_pile(1, 1);
        CB->divers_empile(DV_SCREEN, 5, 0);
        ST(ST_VIE(P_KALHI13)) = 4;
        CB->pos_perso(P_KALHI13, -1, -1, -1, 4, "", 4, 0);
        CB->retire_zone(5);
        CB->pos_perso(P_KALHI33, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 1, "P33ARR01", 5, 1);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->animation("SP33ATT01");
        lieu_action_register("L03D501A", P_KALHI33, "P33DIA54", "SP33ARR01");
        lieu_action_register("L03D501B", P_KALHI33, "SP33DIA59", "SP33ARR01_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->modify_var(&ST(0xE0), CB->divers(DV_DESC_POS, P_KALHI33, 0));
        CB->modify_var(&ST(0xF0), CB->divers(DV_DESC_POS, P_KALHI33, 1));
        CB->modify_var(&ST(0xF4), CB->divers(DV_DESC_POS, P_KALHI33, 2));
        CB->modify_var(&ST(0x74), 0x63);
        CB->divers(DV_IDLE_TIME, 1, 0);
        i32 d1 = CB->distance(1, 4);
        /* 0x95D7 */
        if (d1 <= CB->distance(6, 4)) {
            CB->joindre(P_KALHI33, -1721, -1535, -1, -1);
            CB->animation("SP33ARR01");
            CB->joindre(P_KALHI33, -1340, -1535, -1, -1);
        } else {
            CB->joindre(P_KALHI33, -1721, -1155, -1, -1);
            CB->animation("SP33ARR01");
            CB->joindre(P_KALHI33, -1340, -1155, -1, -1);
        }
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0x967A */
    if (ST(0x78) == 1 && CB->divers(DV_IDLE_TIME, 0, 0) >= 0x2710) {
        ST(0x78) = 0;
        CB->select_pile(1, 1);
        /* 0x96C2 */
        if (ST(0x948) == 2) {
            lieu_action_register("L03D601", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
        }
        /* 0x96ED */
        if (ST(0x948) == 1) {
            ST(0x948) = 2;
            lieu_action_register("L03D602", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
        }
        /* 0x9727 */
        if (ST(0x948) == 0) {
            ST(0x948) = 1;
            lieu_action_register("L03D603", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
        }
        CB->divers_empile(DV_IDLE_TIME, 1, 0);
        CB->animation("SP13ARR01_10");
        CB->modify_var(&ST(0x78), 1);
        CB->select_pile(0, 1);
        return;
    }
    /* 0x97A4 */
    if (ST(V_DELIA_PHASE) == 9 && ST(ST_VIE(P_DELIA)) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        /* 0x97E0 */
        if (CB->distance(8, 12) < 0x8C && ST(0x84) == 0) {
            ST(0x84) = 1;
            CB->joindre_perm(-P_DELIA, ST(ST_HERO));
            CB->stop_joindre(P_DELIA);
            CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 1, "P10ARR01", 0xC, 0);
        }
        /* 0x9881 */
        if (ST(0x68) == 1 && (ST(0x38) % 20) == 0) {
            /* 0x98B4 */
            if (CB->distance(8, 12) >= 0xB4) {
                ST(0x84) = 0;
                CB->divers(DV_RUN, P_DELIA, 0);
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
            /* 0x9910 */
            if (CB->distance(8, 12) >= 0x12C) {
                ST(0x84) = 0;
                CB->divers(DV_RUN, P_DELIA, 1);
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
        }
    }
/* WARN call 100095cf used twice */
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1000E103).
 * Callback ingestion (0x1000DD7A..0x1000E0EF), the trigger-table fill and
 * the unused dword_1001E07C = state+0xC8 are skipped: g_lieu_cb is provided
 * by the bridge, the trigger table is static in lieu03_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000E322);
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
        l03_trig00();   /* 0x1000E329: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000E210 — the hero is dead: play the death ending */
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
 * TICK (opcode -1, 0x1000E37C): drain pending sound, run body.
 * ---------------------------------------------------------------------- */
static void tick(void) {
    if (ST(V_PENDING_EVENT) != 0) {
        CB->select_pile(2, 1);
        CB->evenement(ST(V_PENDING_EVENT) + 0x59, 1000);
        ST(V_PENDING_EVENT) = 0;
        CB->select_pile(0, 1);
    }
    tick_body();
}

/* -------------------------------------------------------------------------
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x1000DD60).
 * Jump table @ 0x1000E3F9, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu03_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu03: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l03_trigger(arg);             break;  /* dword_1001DF10[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
