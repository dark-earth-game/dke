/*
 * lieu01.c — Native hand-port of LIEU01.DLL (level-1 scripting).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Handlers call the engine only through
 * g_lieu_cb (see lieu_cb.h). See docs/functions/lieu01_handlers.md for
 * the opcode dispatch, trigger table and state-offset map.
 *
 * This file: entry / opcode dispatch, INIT, tick / tick body, events 4-6
 * and the helpers both halves use. The 42 trigger slots (25 handlers) are
 * in lieu01_trig.c.
 */

#include <stdio.h>

#include "lieu01_int.h"

/* Engine-provided callback vtable is defined in lieu_cb.c (g_lieu_cb). */

/* Current state buffer for the in-flight call (original used the global
 * dword_1002C96C; the entry point sets this per call). */
lieu_state_t *g_l01_S;

/* Shorthands mirroring the disassembly. */

/* -------------------------------------------------------------------------
 * Helper: action_register  (sub_1001932C, alias thunk sub_1000100A)
 *
 * Original __fastcall(ecx=name, edx=count) + stdcall stack (arg0, arg4).
 *   named_object(arg0)
 *   named_object_int(name, count)
 *   if      (count < 4 && arg4 == NULL) named_object("Sar_20")
 *   else if (arg4 != NULL)              named_object(arg4)
 *   else { sprintf(b,"SP%02darr01_10",count); named_object(b) }
 * ---------------------------------------------------------------------- */
void l01_action_register(const c8 *name, i32 count,
                            const c8 *arg0, const c8 *arg4) {
    CB->animation(arg0);
    CB->dialogue(name, count);
    if (count < 4 && arg4 == NULL) {
        CB->animation("Sar_20");
    } else if (arg4 != NULL) {
        CB->animation(arg4);
    } else {
        c8 b[64];
        snprintf(b, sizeof(b), "SP%02darr01_10", count);
        CB->animation(b);
    }
}

/* Spawn "ar" actor at position from 3 layer-1 coordinate queries.
 * Queries evaluated axis 2,1,0 (original order) into temps. */
void l01_spawn_ar(void) {
    i32 a2 = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
    i32 a1 = CB->divers(DV_DESC_POS, ST(ST_HERO), 1);
    i32 a0 = CB->divers(DV_DESC_POS, ST(ST_HERO), 0);
    CB->pos_perso(ST(ST_HERO), a0, a1, a2, 1, "ar", 6, 0);
}

/* -------------------------------------------------------------------------
 * event6 — sub_10005BEC.  Switch on arg (0..4): LCOD zone triggers.
 * Fully ported.
 * ---------------------------------------------------------------------- */
static void event6(i32 arg) {
    switch (arg) {
    case 0:  /* loc_10005C0A */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_COMBAT_STANCE) == 0 && ST(V_LCOD34_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD34_SHOWN) = 1;
            CB->bloque(BLOQUE_ALL);
            l01_action_register("LCOD34", 2, "SG00CON01", NULL);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case 1:  /* loc_10005C87 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(V_LCOD35_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD35_SHOWN) = 1;
            ST(V_LCOD37_SHOWN) = 0;
            CB->bloque(BLOQUE_ALL);
            l01_action_register("LCOD35", 2, "SG00CON01", NULL);
            CB->animation("Sar_50");
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->animation("SG00CON01_1500");
            CB->animation("ar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case 2:  /* loc_10005D34 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_COMBAT_STANCE) == 0 && ST(V_LCOD36_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD36_SHOWN) = 1;
            CB->bloque(BLOQUE_ALL);
            l01_action_register("LCOD36", 3, "SG00CON01", "SG00CON01");
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case 3:  /* loc_10005DBF */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(V_LCOD37_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD35_SHOWN) = 0;
            ST(V_LCOD36_SHOWN) = 0;
            ST(V_LCOD37_SHOWN) = 1;
            CB->bloque(BLOQUE_ALL);
            l01_action_register("LCOD37", 3, "SG00CON01", NULL);
            CB->animation("Sar_50");
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case 4:  /* loc_10005E70 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(0x318) == 0) {
            CB->select_pile(2, 2);
            CB->select_pile(1, 2);
            CB->select_pile(0, 2);
            ST(0x318) = 1;
            CB->divers(DV_CUT_SPEECH, 0, 0);
            CB->evenement(0, -1);
            CB->evenement(1, -1);
            CB->evenement(2, -1);
            CB->evenement(0x21, -1);
            CB->evenement(0x22, -1);
            CB->evenement(0x23, -1);
            CB->evenement(0x24, -1);
            CB->evenement(0x25, -1);
            CB->evenement(0x26, -1);
            CB->evenement(0x27, -1);
            CB->evenement(0x29, -1);
            CB->bloque(BLOQUE_ALL);
            CB->animation("Sar_50");
            if (ST(ST_COMBAT_STANCE) == 1)
                CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
            CB->divers_empile(DV_SCREEN, 5, 0);
            CB->divers_empile(DV_HP, P_ARKHAN3, 0);
            CB->animation("Sar_50");
            CB->cinematique("SC21_500");
            CB->divers_empile(DV_SCREEN, 3, 0);
            CB->divers_empile(DV_END_GAME, 0, 0);
        }
        break;
    default:
        break;
    }
}

/*
 * Per-frame body — sub_100088FE (11 KB, the level-1 state machine).
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp loc_1000B55E` (epilogue).
 * Complete: 0x88FE .. 0xB562 (all 2183 instructions).
 */
static void tick_body(void) {
    /* 0x88FE — debug subtitle */
    {
        c8 buf[64];
        snprintf(buf, sizeof(buf), "testos printos:%d", ST(0x960));
        CB->print_msg(buf, 0, 0xF0);
    }
    /* 0x8934 */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        CB->evenement(0x2C, 0xBB8);
    }
    /* 0x8977 — intro cinematic replay */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        if (CB->divers(DV_PERSONA_PRESENT, 0, 0)) CB->divers(DV_HP, 0, 0);
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) CB->divers(DV_HP, P_ARKHAN1, 0);
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) CB->divers(DV_HP, P_ARKHAN2, 0);
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) CB->divers(DV_HP, P_ARKHAN3, 0);
        if (ST(V_GUARD_FIGHT_DONE) == 0) CB->cinematique("SC22_500");
        if (ST(V_GUARD_FIGHT_DONE) != 0) CB->cinematique("SC23_500");
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x8A96 — level-0 arrival spawn */
    if (ST(V_EVENT_2) == 1 && ST(ST_HERO) == P_ARKHAN0) {
        ST(V_EVENT_2) = 0;
        CB->animation("SP12ARR01_10");
        CB->animation("SP00LEV01");
        CB->bruitage("L01B03", 638, -3062);
        CB->animation("P00MAR25");
        CB->animation("ar");
        CB->pos_perso(0, 628, -2950, 0xFBC0, 1, "ar", 6, 0);
        CB->divers_empile(DV_MUSIC, 1, 1);
        CB->change_camera(CAM_ANY, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x8B5B */
    if (ST(V_EVENT_34) == 1) {
        ST(V_EVENT_34) = 0;
        ST(V_ZED_BRIEFING) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L01D01A", P_ZED27);
        CB->dialogue("L01D01B", P_ZED27);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x8BC3 — section F: guarded by [1C0]==1 && [EC0]==0 */
    if (ST(ST_VIE(P_KALHI12)) == 1 && ST(V_LEFT_HOME) == 0) {
        /* 0x8BE7 */
        if (ST(V_EVENT_0) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x68) != 1) {
            ST(V_EVENT_0) = 2;
            ST(0x68)  = 1;
            ST(0xB8)  = ST(ST_LOCKED_CAMERA);
            ST(0xBC)  = 1;
            ST(0x98)  = 1;
            ST(ST_SCRIPT_BUSY)  = 1;
            CB->bloque(BLOQUE_ALL);
            if (ST(V_KALHI12_TALK_NIGHT) == 0) CB->change_camera(6, CAM_LOCKED);
            CB->animation("SP12LEV01");
            CB->animation("P12ARR03_25");
            CB->divers_empile(DV_LOOK_AT_PLAYER, P_KALHI12, 1);
            l01_action_register("L01D03", 0x0C, "P12ARR03", "SP12ARR03_100");
            CB->modify_var(&ST(ST_SCRIPT_BUSY),  0);
            CB->modify_var(&ST(V_KALHI12_TALK_NIGHT), 1);
            CB->modify_var(&ST(0x88),  1);
            if (ST(0xB8) != 0 && ST(0xB8) != 6)
                CB->change_camera(ST(ST_LOCKED_CAMERA), CAM_AUTO);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x8D67 */
        if (ST(0x88) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
            ST(0x88) = 2;
            CB->select_pile(1, 1);
            CB->divers_empile(DV_LOOK_AT_PLAYER, P_KALHI12, 0);
            CB->animation("P12WAA01");
            CB->animation("P12ARR03_10");
            CB->divers_empile(DV_LOOK_AT_PLAYER, P_KALHI12, 1);
            CB->modify_var(&ST(0x88), 0x29A);
            CB->select_pile(0, 1);
        }
        /* 0x8E05 */
        if (CB->distance(6, 4) > 0x190 && (i32)ST(V_EVENT_0) > 0 && ST(0x74) != 1) {
            /* 0x8E41 */
            if (ST(ST_LOCKED_CAMERA) == 6 && ST(0xBC) == 1) {
                ST(0xBC) = 2;
                CB->change_camera(ST(ST_LOCKED_CAMERA), CAM_AUTO);
            }
            /* 0x8E8A */
            if (ST(0x68) == 1 && ST(V_ENGAGED_PERSONA) == 0
                && CB->random(0xC8) == 1 && ST(0x88) == 0x29A) {
                CB->divers(DV_LOOK_AT_PLAYER, P_KALHI12, 0);
                ST(0x68) = 0;
                CB->select_pile(1, 1);
                CB->animation("SP12LEV03");
                CB->animation("P12ARR01_10");
                CB->select_pile(0, 1);
            }
            /* 0x8F22 */
            if (ST(V_EVENT_1) != 0)
                ST(V_EVENT_1) = 0;
        }
        /* 0x8F43 */
        if (CB->distance(6, 4) < 0x96 && (i32)ST(V_EVENT_0) > 0
            && ST(0x68) == 0 && ST(V_ENGAGED_PERSONA) == 0
            && CB->random(0x64) == 1 && ST(0x88) == 0x29A) {
            ST(0x68) = 1;
            CB->select_pile(1, 1);
            CB->animation("SP12LEV01");
            CB->animation("P12ARR03_10");
            CB->modify_var(&ST(V_EVENT_1), 1);
            CB->select_pile(0, 1);
        }
        /* 0x900D */
        if (ST(V_EVENT_1) == 1) {
            ST(V_EVENT_1) = 2;
            CB->divers(DV_LOOK_AT_PLAYER, P_KALHI12, 1);
        }
    }
    /* 0x9040 — section G: guarded by [EC0]==0 && [B04]==0 */
    if (ST(V_LEFT_HOME) == 0 && ST(V_GUARD_FIGHT_DONE) == 0) {
        /* 0x9064 */
        if (ST(0x78) == 1) {
            ST(0x78) = -1;
            CB->pos_perso(P_PHEDORIA, 400, -2330, 0x9470, 1,
                          "P26GAR03_10", -1, 0);
            CB->change_camera(4, CAM_LOCKED);
            CB->bruitage("PORTE3", 512, -2087);
            CB->dialogue("L01D45A", P_PHEDORIA);
            if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 7))
                CB->change_camera(0, CAM_LOCKED);
            /* 0x9106 */
            CB->joindre(ST(ST_HERO), 0, 0, 0, 0x1A);
            if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 4))
                CB->change_camera(0, CAM_LOCKED);
            /* 0x914C */
            CB->animation("Sar");
            CB->joindre(-ST(ST_HERO), 512, -2677, 0x859, -1);
            CB->dialogue("L01D99", 0);
            CB->change_camera(1, CAM_LOCKED);
            l01_action_register("L01D45B", 0x1A, "SP26GAR03", "SG26FRA01");
            CB->evenement(0x27, 0x2EE);
            CB->animation("P26GAR03_10");
        }
        /* 0x91CE */
        if (ST(V_EVENT_39) == 1) {
            ST(V_EVENT_39) = 0;
            CB->select_pile(2, 2);
            CB->stop_joindre(-ST(ST_HERO));
            CB->divers_empile(DV_BONE_SOUND, ST(ST_HERO), 0);
            if (ST(ST_HERO) == P_ARKHAN0) CB->animation("SG00ATT07");
            if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SG01ATT07");
            CB->modify_var(&ST(0x78), 2);
            CB->select_pile(0, 1);
        }
        /* 0x9287 */
        if (ST(0x78) == 2) {
            ST(0x78)  = -1;
            ST(V_EVENT_1) = 0;
            CB->divers(DV_LOOK_AT_PLAYER, P_KALHI12, 0);
            CB->animation("SP12ATT03");
            CB->change_camera(6, CAM_LOCKED);
            CB->dialogue("L01D44", P_KALHI12);
            CB->animation("SP12ATT03_2000");
            CB->divers_empile(DV_HP, ST(ST_HERO), 0);
            CB->modify_var(&ST(ST_HERO), 4);
        }
    }
    /* 0x9328 */
    if (ST(V_ENGAGED_PERSONA) == 0x57 && ST(V_GUARD_WAVE) == 0 && ST(ST_VIE(P_KABAK)) == VIE_DEAD && ST(V_LEFT_HOME) == 1)
        ST(V_ENGAGED_PERSONA) = 0;
    /* 0x9379 — door PORTE spawn */
    if (ST(ST_VIE(P_KABAK)) == VIE_DEAD && ST(V_GUARD_WAVE) != 0 && ST(0xD8) == 0 && ST(V_KABAK_DOOR_ZONE) == 0) {
        ST(0x80)  = 0;
        ST(0xD8)  = 1;
        ST(V_KABAK_DOOR_ZONE) = 1;
        CB->select_pile(2, 1);
        CB->ajoute_zone(16, 320, -2543, 90, 90, 0);
        CB->select_pile(0, 1);
    }
    /* 0x9426 — imp W (0x57) appearance */
    if (CB->divers(DV_WAS_HIT, P_KABAK, 0) && ST(ST_VIE(P_KABAK)) != VIE_DEAD
        && CB->divers(DV_PERSONA_PRESENT, P_KABAK, 0)) {
        CB->select_pile(1, 2);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        CB->divers_empile(DV_WAS_HIT, P_KABAK, 1);
        ST(ST_SCRIPT_BUSY)  = 0;
        ST(V_ENGAGED_PERSONA)  = 0;
        ST(ST_VIE(P_KABAK)) = VIE_DEAD;
        CB->combat(P_KABAK, -1);
        CB->stop_joindre(-P_KABAK);
        CB->animation("SP87IMP52");
        CB->animation("P87CAD");
        CB->pos_perso(P_KABAK, 0, 0, 0, 2, "P87CAD", 9, 0);
        if (ST(V_GUARD_WAVE) == 1 && ST(ST_VIE(P_GUARDIAN23)) == 1) {
            if (ST(0x94) == 1) {
                ST(V_G23_FIGHT_ON) = 1;
                ST(0xDC)  = 0x0A;
                CB->select_pile(2, 2);
                CB->stop_joindre(-P_GUARDIAN23);
                CB->dialogue("L01D147", P_GUARDIAN23);
                CB->combat(P_GUARDIAN23, 0);
                ST(V_ENGAGED_PERSONA) = 0x58;
            }
            if (ST(0x94) == 0) {
                ST(V_G23_FIGHT_ON) = 1;
                ST(0x94)  = 1;
                CB->select_pile(2, 2);
                CB->vire_col(1, 0);
                CB->animation("SP88LEV01");
                CB->animation("P88_65535");
                CB->dialogue("L01D147", P_GUARDIAN23);
                CB->vire_col(1, 1);
                CB->combat(P_GUARDIAN23, 0);
                ST(V_ENGAGED_PERSONA) = 0x58;
            }
        }
        CB->select_pile(0, 1);
    }
    /* 0x9652 — imp X (0x58) appearance */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN23, 0) && ST(ST_VIE(P_GUARDIAN23)) != VIE_DEAD && ST(0x94) == 0) {
        ST(0xE0)  = 1;
        ST(ST_SCRIPT_BUSY)  = 0;
        ST(V_ENGAGED_PERSONA)  = 0x57;
        ST(ST_VIE(P_GUARDIAN23)) = VIE_DEAD;
        CB->animation("SP88IMP99");
        CB->animation("P88CAD");
        CB->pos_perso(P_GUARDIAN23, 0, 0, 0, 2, "P88CAD", 0x11, 0);
        if (ST(ST_VIE(P_KABAK)) == 1) {
            ST(V_KABAK_FIGHT_ON) = 1;
            CB->select_pile(1, 2);
            CB->dialogue("L01D148", P_KABAK);
            CB->combat(P_KABAK, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x9784 — imp Y (0x59) appearance */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN24, 0) && ST(ST_VIE(P_GUARDIAN24)) != VIE_DEAD
        && CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN24, 0)) {
        ST(0xE0)  = 1;
        ST(ST_SCRIPT_BUSY)  = 0;
        ST(V_ENGAGED_PERSONA)  = 0;
        ST(ST_VIE(P_GUARDIAN24)) = VIE_DEAD;
        CB->select_pile(2, 2);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        CB->divers_empile(DV_WAS_HIT, P_GUARDIAN24, 1);
        CB->animation("SP89IMP51");
        CB->animation("P89CAD");
        CB->pos_perso(P_GUARDIAN24, 0, 0, 0, 2, "P89CAD", 0x12, 0);
        if (ST(ST_VIE(P_KABAK)) == 1) {
            ST(V_KABAK_FIGHT_ON) = 1;
            CB->select_pile(1, 2);
            CB->dialogue("L01D148", P_KABAK);
            CB->combat(P_KABAK, 0);
            ST(V_ENGAGED_PERSONA) = 0x57;
        }
        CB->select_pile(0, 1);
    }
    /* 0x98CA — imp X (0x58) variant */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN23, 0) && ST(ST_VIE(P_GUARDIAN23)) != VIE_DEAD && ST(0x94) == 1) {
        ST(0xE0)  = 1;
        ST(ST_SCRIPT_BUSY)  = 0;
        ST(ST_VIE(P_GUARDIAN23)) = VIE_DEAD;
        CB->select_pile(2, 2);
        CB->divers(DV_CUT_SPEECH, 0, 0);
        CB->divers_empile(DV_WAS_HIT, P_GUARDIAN23, 1);
        CB->combat(P_GUARDIAN23, -1);
        CB->stop_joindre(-P_GUARDIAN23);
        CB->animation("SP88IMP51");
        CB->animation("P88CAD");
        CB->pos_perso(P_GUARDIAN23, 0, 0, 0, 2, "P88CAD", 0x11, 0);
        if (ST(ST_VIE(P_KABAK)) == 1) {
            ST(V_KABAK_FIGHT_ON) = 1;
            CB->select_pile(1, 2);
            CB->dialogue("L01D148", P_KABAK);
            CB->combat(P_KABAK, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x9A0B */
    if (ST(ST_VIE(P_KABAK)) == 1 && ST(V_GUARD_WAVE) == 0) {
        if (ST(0x7C) == 0 && CB->distance(6, 9) <= 0xFA)
            ST(0x7C) = 1;
        /* 0x9A65 */
        if (ST(0x7C) == 2 && ST(ST_LOCKED_CAMERA) != 2 && ST(V_ENGAGED_PERSONA) == 0) {
            ST(0x7C) = 0;
            CB->select_pile(2, 1);
            CB->stop_joindre(-P_KABAK);
            if (ST(0x80) == 1) {
                ST(0x80) = 0;
                CB->bloque(BLOQUE_ALL);
                CB->animation("SP87GAR27");
                CB->animation("P87ARR01_10");
                CB->bloque(BLOQUE_NONE);
            }
            /* 0x9B06 */
            CB->joindre(P_KABAK, 0, 0, 0x5DC0, -1);
            CB->animation("SP87ATT04");
            CB->select_pile(0, 1);
        }
    }
    /* 0x9B34 */
    if (ST(ST_VIE(P_KABAK)) == 1 && ST(V_GUARD_WAVE) != 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(0x80) == 0) {
        if (ST(0x7C) == 0 && CB->distance(6, 9) <= 0xFA)
            ST(0x7C) = 1;
        /* 0x9BAF */
        if (ST(0x7C) == 2
            && (ST(ST_LOCKED_CAMERA) == 0 || ST(ST_LOCKED_CAMERA) == 6 || ST(ST_LOCKED_CAMERA) == 3)
            && CB->distance(6, 9) > 0x113) {
            ST(0x7C) = 0;
            CB->select_pile(2, 1);
            CB->stop_joindre(-P_KABAK);
            CB->pos_perso(P_KABAK, 320, -2530, 0xDD4, 1,
                          "P87ATT01", 9, 1);
            if (ST(0xF0) == 0) {
                ST(0xF0) = 1;
                CB->link_objet(0x57, "OB94", 0x1B, 1);
            }
            CB->select_pile(0, 1);
        }
    }
    /* 0x9C9D — Sar dialogue driver (guarded) */
    if (ST(ST_VIE(P_KABAK)) < 2 && ST(0x7C) == 1 && ST(V_KABAK_FIGHT_ON) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(0x7C) = 2;
        if (ST(ST_COMBAT_STANCE) == 0) {
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_KABAK);
            CB->animation("SP87ARR01_10");
            CB->joindre(P_KABAK, 0, 0, 0, ST(ST_HERO));
            if (ST(0x84) == 0) {
                ST(0x84) = 1;
                if (ST(ST_HERO) == P_ARKHAN1 && ST(V_KABAK_MET_H1) == 0) {
                    CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x57);
                    CB->animation("sar_10");
                    ST(V_KABAK_MET_H1) = 1;
                    CB->dialogue("L01D66A", P_ARKHAN1);
                    l01_action_register("L01D67", 0x57, "SP87ATT70", "SP87ARR01_10");
                }
                /* 0x9DDD */
                if (ST(ST_HERO) >= 2) {
                    CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x57);
                    CB->animation("sar_10");
                    if (ST(V_KABAK_TALK_H23) == 0) {
                        ST(V_KABAK_TALK_H23) = 1;
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            if (ST(ST_SPECIAL_MODE) == 1)
                                l01_action_register("L01D60A", 2, "SG02DIA03", NULL);
                            if (ST(ST_SPECIAL_MODE) == 0)
                                l01_action_register("L01D60B", 2, "SG02DIA03", NULL);
                        }
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            if (ST(ST_SPECIAL_MODE) == 1)
                                l01_action_register("L01D61A", 3, "SG03DIA03", NULL);
                            if (ST(ST_SPECIAL_MODE) == 0)
                                l01_action_register("L01D61B", 3, "SG03DIA03", NULL);
                        }
                        l01_action_register("L01D30A", 0x57, "SP87DIA50", "SP87ARR01_10");
                        l01_action_register("L01D30B", 0x57, "SP87DIA56", "SP87ARR01_10");
                    } else {
                        /* 0x9F33 */
                        l01_action_register("L01D139", 0x57, "SP87DIA50", "SP87ARR01_10");
                    }
                }
                /* 0x9F4C */
                CB->animation("SP87ARR01_10");
                CB->joindre_perm(0x57, ST(ST_HERO));
                CB->bloque(BLOQUE_NONE);
            } else {
                /* 0x9F7A */
                CB->animation("SP87ARR01_10");
                CB->joindre_perm(0x57, ST(ST_HERO));
                CB->bloque(BLOQUE_NONE);
            }
        }
        /* 0x9FA3 */
        if (ST(ST_COMBAT_STANCE) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(0x80) = 1;
            if (ST(0xF0) == 1) {
                ST(0xF0) = 0;
                CB->link_objet(0x57, "OB94", 0x1B, 0);
            }
            if (ST(0x84) == 0) {
                ST(0x84) = 1;
                if (ST(ST_HERO) == P_ARKHAN1) { ST(V_KABAK_MET_H1) = 1; CB->dialogue("L01D66A", P_ARKHAN1); }
                if (ST(ST_HERO) == P_ARKHAN2) { ST(V_KABAK_TALK_H23) = 1; CB->dialogue("L01D66B", P_ARKHAN2); }
                if (ST(ST_HERO) == P_ARKHAN3) { ST(V_KABAK_TALK_H23) = 1; CB->dialogue("L01D66C", P_ARKHAN3); }
                CB->animation("SP87ARR01_10");
                CB->joindre(-P_KABAK, 0, 0, 0, ST(ST_HERO));
                CB->animation("SP87GAR26");
                CB->animation("P87GAR02_10");
                if ((i32)ST(V_PLAYER_STRIKE) >= 1) {
                    ST(V_ENGAGED_PERSONA) = 0x57;
                    CB->evenement(2, 0x64);
                }
                if (ST(V_PLAYER_STRIKE) == 0) {
                    if (ST(V_KABAK_D81_SAID) == 0) {
                        ST(V_KABAK_D81_SAID) = 1;
                        if (ST(ST_HERO) == P_ARKHAN1)
                            l01_action_register("L01D81", 0x57, "SP87DIA01", "SP87GAR02_10");
                        if (ST(ST_HERO) >= 2)
                            l01_action_register("L01D82", 0x57, "SP87DIA01", "SP87GAR02_10");
                    }
                    CB->joindre_perm(0x57, ST(ST_HERO));
                    CB->bloque(BLOQUE_NONE);
                }
                /* [14]!=0 falls through to A223 */
            } else {
                /* 0xA1C7 */
                CB->animation("SP87ARR01_10");
                CB->joindre(-P_KABAK, 0, 0, 0, ST(ST_HERO));
                CB->animation("SP87GAR26");
                CB->animation("P87GAR02_10");
                CB->joindre_perm(0x57, ST(ST_HERO));
                CB->bloque(BLOQUE_NONE);
            }
        }
    }
    /* 0xA223 */
    if (ST(ST_VIE(P_KABAK)) < 2 && ST(0x7C) == 1 && ST(V_KABAK_FIGHT_ON) == 1 && ST(V_ENGAGED_PERSONA) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_ENGAGED_PERSONA) = 0x57;
        ST(0x7C) = 0x0A;
        ST(0x80) = 1;
        if (ST(0xF0) == 1) { ST(0xF0) = 0; CB->link_objet(0x57, "OB94", 0x1B, 0); }
        CB->animation("SP87ARR01_10");
        CB->joindre(-P_KABAK, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP87GAR26");
        l01_action_register("L01D87", 0x57, "SP87DIA01", "SP87GAR02_10");
        CB->bloque(BLOQUE_NONE);
        if (ST(V_GUARD_WAVE) == 1 && ST(ST_VIE(P_GUARDIAN23)) == 1) {
            if (ST(0x94) != 1) {
                ST(0x94) = 1;
                CB->animation("SP88LEV01");
                CB->animation("P88_65535");
            }
            CB->combat(P_GUARDIAN23, 0);
        }
        CB->combat(P_KABAK, 0);
    }
    /* 0xA395 */
    if (ST(ST_VIE(P_KABAK)) < 2 && ST(0x7C) == 2 && ST(V_ENGAGED_PERSONA) == 0) {
        if (ST(0x80) == 0 && ST(ST_COMBAT_STANCE) == 1) {
            ST(0x80) = 1;
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_KABAK);
            if (ST(0xF0) == 1) { ST(0xF0) = 0; CB->link_objet(0x57, "OB94", 0x1B, 0); }
            CB->joindre(-P_KABAK, 0, 0, 0, ST(ST_HERO));
            CB->animation("SP87ARR01_10");
            CB->animation("P87GAR26");
            if (ST(V_KABAK_D81_SAID) == 0) {
                ST(V_KABAK_D81_SAID) = 1;
                if (ST(ST_HERO) == P_ARKHAN1)
                    l01_action_register("L01D81", 0x57, "P87DIA01", "SP87GAR02_10");
                if (ST(ST_HERO) >= 2)
                    l01_action_register("L01D82", 0x57, "P87DIA01", "SP87GAR02_10");
            }
            CB->joindre_perm(0x57, ST(ST_HERO));
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA50D */
        if (ST(0x80) == 1 && ST(ST_COMBAT_STANCE) == 0) {
            ST(0x80) = 0;
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_LOCKED_CAMERA) == 0 || ST(ST_LOCKED_CAMERA) == 3 || ST(ST_LOCKED_CAMERA) == 6) {
                CB->dialogue("L01D80", P_KABAK);
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                CB->stop_joindre(-P_KABAK);
                CB->animation("SP87GAR27");
                CB->animation("P87ARR01_200");
                if (ST(V_KABAK_D80_SAID) == 0 && ST(ST_HERO) >= 2) {
                    ST(V_KABAK_D80_SAID) = 1;
                    l01_action_register("L01D80", 0x57, "P87DIA59", "SP87ARR01_10");
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->joindre_perm(0x57, ST(ST_HERO));
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0xA672 */
    if (ST(V_EVENT_2) == 1 && ST(V_ENGAGED_PERSONA) == 0x57) {
        ST(V_EVENT_2) = 2;
        ST(V_KABAK_INTRO) = 1;
        ST(V_ENGAGED_PERSONA)  = 0;
        CB->joindre(-P_KABAK, 0, 0, 0, ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN1)
            l01_action_register("L01D137", 0x57, "SP87DIA01", "SP87GAR02_10");
        if (ST(ST_HERO) >= 2)
            l01_action_register("L01D138A", 0x57, "SP87DIA01", "SP87GAR02_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->animation("Sar_10");
        CB->joindre_perm(0x57, ST(ST_HERO));
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA76F — imp W distance-gated dialogue/attack */
    if (ST(ST_VIE(P_GUARDIAN23)) == 1 && ST(V_ENGAGED_PERSONA) == 0 && CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN23, 0)) {
        /* 0xA7AA — sub-block, else falls to B03A */
        if (ST(ST_VIE(P_KABAK)) < 2 && ST(ST_HERO) != 4
            && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) <= -2800
            && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) >= 0x1F4
            && ST(0x98) == 0) {
            if (ST(0xCCC) == 0) {
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY)  = 1;
                ST(0xCCC) = 1;
                ST(0x98)  = 1;
                CB->change_camera(6, CAM_LOCKED);
                ST(0x7C) = 0;
                CB->pos_perso(P_KABAK, 320, -2530, 0x8CA0, 1, "P87ARR01", 9, 1);
                if (ST(ST_VIE(P_KABAK)) == 1 && ST(V_KABAK_TALK_H23) == 0) {
                    ST(V_KABAK_TALK_H23) = 1;
                    ST(0x94)  = 1;
                    CB->vire_col(1, 0);
                    CB->animation("SP88LEV01");
                    CB->animation("P88_65535");
                    CB->joindre(-P_GUARDIAN23, 0, 0, 0, ST(ST_HERO));
                    l01_action_register("L01D121", 0x58, "SP88DIA59", "SP88ARR01");
                    CB->change_camera(2, CAM_LOCKED);
                    CB->joindre(-P_KABAK, 0, 0, 0, 0x58);
                    if (ST(0x80) == 0)
                        l01_action_register("L01D31A", 0x57, "SP87DIA55", "SP87ARR01_100");
                    if (ST(0x80) != 0) {
                        ST(0x80) = 0;
                        CB->stop_joindre(-P_KABAK);
                        CB->animation("SP87GAR27");
                        CB->animation("P87ARR01_200");
                        l01_action_register("L01D31A", 0x57, "P87DIA55", "SP87ARR01_100");
                    }
                    CB->joindre(P_KABAK, 0, 0, 0, ST(ST_HERO));
                    l01_action_register("L01D30A", 0x57, "SP87DIA50", "SP87ARR01");
                    l01_action_register("L01D30B", 0x57, "SP87DIA56", "SP87ARR01");
                    CB->joindre(-P_KABAK, 0, 0, 0, 0x58);
                    l01_action_register("L01D31B", 0x57, "SP87DIA55", "SP87ARR01_100");
                    l01_action_register("L01D31C", 0x57, "SP87DIA59", "SP87ARR01_100");
                    CB->change_camera(6, CAM_LOCKED);
                    CB->pos_perso(P_KABAK, 320, -2530, 0xDD4, 1, "P87ATT01", 9, 1);
                    if (ST(0xE0) == 0) {
                        CB->link_objet(0x57, "OB94", 0x1B, 1);
                        ST(0xF0) = 1;
                    }
                    if (ST(ST_COMBAT_STANCE) == 0)
                        l01_action_register("L01D122", 0x58, "SP88DIA60", "SP88ARR01_100");
                    if (ST(ST_COMBAT_STANCE) == 1)
                        l01_action_register("L01D123", 0x58, "SP88DIA60", "SP88GAR01_500");
                    ST(0x84) = 1;
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->joindre_perm(0x58, ST(ST_HERO));
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xAB83 */
                if (ST(ST_VIE(P_KABAK)) == 1 && ST(V_KABAK_TALK_H23) == 1) {
                    ST(0x94) = 1;
                    CB->vire_col(1, 0);
                    CB->animation("SP88LEV01");
                    CB->animation("P88_65535");
                    CB->joindre(-P_GUARDIAN23, 0, 0, 0, ST(ST_HERO));
                    l01_action_register("L01D124", 0x58, "SP88DIA59", "SP88ARR01_10");
                    CB->vire_col(1, 1);
                    CB->change_camera(2, CAM_LOCKED);
                    CB->joindre(-P_KABAK, 0, 0, 0, 0x58);
                    if (ST(0x80) == 0 && ST(ST_COMBAT_STANCE) == 0)
                        l01_action_register("L01D31B", 0x57, "SP87DIA55", "SP87ARR01_100");
                    /* 0xAC7C — [80]/[10] gated L01D171 */
                    {
                        i32 do171;
                        if (ST(0x80) != 0) {
                            ST(0x80) = 0;
                            CB->stop_joindre(-P_KABAK);
                            CB->animation("SP87GAR27");
                            CB->animation("P87ARR01_200");
                            do171 = 1;
                        } else {
                            do171 = (ST(ST_COMBAT_STANCE) == 1) ? 1 : 0;
                        }
                        if (do171)
                            l01_action_register("L01D171", 0x57, "P87DIA55", "SP87ARR01_100");
                    }
                    l01_action_register("L01D31C", 0x57, "SP87DIA59", "SP87ARR01_100");
                    CB->change_camera(6, CAM_LOCKED);
                    if (ST(ST_COMBAT_STANCE) == 0)
                        l01_action_register("L01D122", 0x58, "SP88DIA60", "SP88ARR01_100");
                    if (ST(ST_COMBAT_STANCE) == 1)
                        l01_action_register("L01D123", 0x58, "SP88DIA60", "SP88GAR01_500");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->joindre_perm(0x58, ST(ST_HERO));
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0xADB7 */
            if (ST(0xCCC) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
                ST(0x98) = 1;
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                ST(0x7C) = 0;
                ST(0x80) = 0;
                CB->change_camera(6, CAM_LOCKED);
                CB->pos_perso(P_KABAK, 320, -2530, 0x8CA0, 1, "P87ARR01", 9, 1);
                if (ST(0x94) == 0) {
                    ST(0x94) = 1;
                    CB->vire_col(1, 0);
                    CB->animation("SP88LEV01");
                    CB->animation("P88_65535");
                    CB->joindre(-P_GUARDIAN23, 0, 0, 0, ST(ST_HERO));
                }
                if (ST(ST_COMBAT_STANCE) == 0)
                    l01_action_register("L01D94", 0x58, "SP88DIA54", "SP88ARR01_10");
                if (ST(ST_COMBAT_STANCE) == 1)
                    l01_action_register("L01D95", 0x58, "SP88DIA51", "SP88ARR01_10");
                CB->vire_col(1, 1);
                CB->change_camera(1, CAM_LOCKED);
                if (ST(0x80) == 0 && ST(ST_COMBAT_STANCE) == 0)
                    l01_action_register("L01D96", 0x57, "SP87DIA61", "SP87ARR01_10");
                /* 0xAF68 — [80]/[10] gated L01D172 */
                {
                    i32 do172;
                    if (ST(0x80) != 0) {
                        ST(0x80) = 0;
                        CB->stop_joindre(-P_KABAK);
                        CB->animation("SP87GAR27");
                        CB->animation("P87ARR01_200");
                        do172 = 1;
                    } else {
                        do172 = (ST(ST_COMBAT_STANCE) == 1) ? 1 : 0;
                    }
                    if (do172)
                        l01_action_register("L01D172", 0x57, "SP87DIA61", "SP87ARR01_10");
                }
                CB->change_camera(6, CAM_LOCKED);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->joindre_perm(0x58, ST(ST_HERO));
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xB03A */
        if (ST(ST_VIE(P_KABAK)) >= 2 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_HERO) != 4
            && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) < -2500
            && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) >= 0x1F4) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x94) = 1;
            ST(V_ENGAGED_PERSONA) = 0x57;
            CB->change_camera(6, CAM_LOCKED);
            CB->vire_col(1, 0);
            CB->animation("SP88LEV01");
            CB->animation("P88_65535");
            CB->joindre(-P_GUARDIAN23, 0, 0, 0, ST(ST_HERO));
            l01_action_register("L01D127", 0x58, "SP88DIA60", "SP88GAR01_500");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->vire_col(1, 1);
            CB->change_camera(0, CAM_AUTO);
            CB->bloque(BLOQUE_NONE);
            CB->combat(P_GUARDIAN23, 0);
            return;
        }
    }
    /* 0xB196 */
    if (ST(ST_VIE(P_GUARDIAN23)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(P_KABAK)) >= 2)
        ST(V_ENGAGED_PERSONA) = 0;
    /* 0xB1D5 */
    if (ST(ST_VIE(P_GUARDIAN23)) == VIE_DEAD && ST(V_G23_FIGHT_ON) == 1)
        ST(V_G23_FIGHT_ON) = 0;
    /* 0xB208 */
    if (ST(ST_VIE(P_GUARDIAN23)) < 2 && ST(V_ENGAGED_PERSONA) == 0 && CB->distance(6, 17) > 0x12C
        && (ST(ST_LOCKED_CAMERA) == 2 || ST(ST_LOCKED_CAMERA) == 3) && ST(0x94) == 1) {
        CB->stop_joindre(-P_GUARDIAN23);
        ST(0xDC) = 0x0A;
        ST(0x94) = 0;
        CB->pos_perso(P_GUARDIAN23, 578, -3010, 0x1388, 1, "P88ARR02", 0x11, 1);
    }
    /* 0xB2C8 */
    if (ST(ST_LOCKED_CAMERA) == 6 && CB->distance(6, 4) > 0xC8 && CB->distance(6, 7) > 0xC8
        && ST(0x98) != 0 && ST(V_LEFT_HOME) == 0 && CB->divers(DV_PERSONA_PRESENT, P_KALHI12, 0)) {
        CB->change_camera(CAM_ANY, CAM_AUTO);
        ST(0x98) = 0;
    }
    /* 0xB36D */
    if (ST(ST_LOCKED_CAMERA) == 6 && CB->distance(6, 7) > 0x12C && ST(0x98) != 0
        && ST(V_LEFT_HOME) == 1 && ST(V_GUARD_WAVE) == 0) {
        CB->change_camera(CAM_ANY, CAM_AUTO);
        ST(0x98) = 0;
    }
    /* 0xB3EF */
    if (ST(ST_LOCKED_CAMERA) == 6 && CB->distance(6, 17) > 0xC8 && ST(0x98) != 0
        && ST(V_LEFT_HOME) == 1 && ST(V_GUARD_WAVE) == 1 && CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN23, 0)) {
        CB->change_camera(CAM_ANY, CAM_AUTO);
        ST(0x98) = 0;
    }
    /* 0xB48B */
    if (ST(ST_LOCKED_CAMERA) == 6 && CB->distance(6, 18) > 0xC8 && ST(0x98) != 0
        && ST(V_LEFT_HOME) == 1 && ST(V_GUARD_WAVE) == 2 && CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN24, 0)) {
        CB->change_camera(CAM_ANY, CAM_AUTO);
        ST(0x98) = 0;
    }
    /* 0xB527 */
    if (ST(0xDC) == 1) {
        ST(0xDC) = 2;
        CB->joindre_perm(0x58, ST(ST_HERO));
    }
    /* 0xB55E */
}

/* Float FSM accumulators — 10-element array at dword_10024A30.
 * Cases 90-93 read/write indices 0-3; sub_1000105F resets all 10 to 1.0f. */
static float g_float_accum[10] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                   1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static i32 g_float_active = 1;   /* dword_10024A58 gate flag */

static void e4_reset_floats(void) {
    for (i32 i = 0; i < 10; i++) g_float_accum[i] = 1.0f;
}
static void e5_clamp6(i32 k);  /* forward decl — defined in event5 section */

/*
 * event4 — sub_1000123A (4 KB). switch(arg) over ~99 script-event ids;
 * mostly simple state-machine setters. `break` == original jmp default.
 * ALL cases ported.
 */
static void event4(i32 arg) {        /* sub_1000123A */
    switch (arg) {
    case 0:  ST(V_EVENT_0) = 1; break;
    case 1:  ST(V_EVENT_1) = 1; break;
    case 2:  ST(V_EVENT_2) = 1; break;

    case 7:  /* 0x33C increment chain 1->2->3->4->5 */
        if      (ST(V_EVENT_7) == 1) ST(V_EVENT_7) = 2;
        else if (ST(V_EVENT_7) == 2) ST(V_EVENT_7) = 3;
        else if (ST(V_EVENT_7) == 3) ST(V_EVENT_7) = 4;
        else if (ST(V_EVENT_7) == 4) ST(V_EVENT_7) = 5;
        break;

    case 8:  if (ST(V_EVENT_8) == 1) ST(V_EVENT_8) = 2; break;

    case 9:
        if (ST(ST_VIE(P_BOGDARAN)) != VIE_DEAD && ST(V_EVENT_9) == 1) {
            ST(0xB80) = 0;
            ST(V_EVENT_9) = 0;
        }
        break;

    case 11: if (ST(V_EVENT_11) == 1) ST(V_EVENT_11) = 2; break;
    case 12: if (ST(V_EVENT_12) == 1) ST(V_EVENT_12) = 2; break;

    case 15: /* 0x8C increment chain (odd->even+1, 0->1..0xC->0xD) */
        if      (ST(V_EVENT15_STEP) == 0)   ST(V_EVENT15_STEP) = 1;
        else if (ST(V_EVENT15_STEP) == 2)   ST(V_EVENT15_STEP) = 3;
        else if (ST(V_EVENT15_STEP) == 4)   ST(V_EVENT15_STEP) = 5;
        else if (ST(V_EVENT15_STEP) == 6)   ST(V_EVENT15_STEP) = 7;
        else if (ST(V_EVENT15_STEP) == 8)   ST(V_EVENT15_STEP) = 9;
        else if (ST(V_EVENT15_STEP) == 0xA) ST(V_EVENT15_STEP) = 0xB;
        else if (ST(V_EVENT15_STEP) == 0xC) ST(V_EVENT15_STEP) = 0xD;
        break;

    case 16:
        if (ST(V_EVENT_16) == 0) {
            ST(V_EVENT_16) = 1;
            CB->free_anim(1);
            CB->play_anim(2, 0);
            if (ST(0xB40) == 1) {
                ST(0xB40) = 2;
                CB->animation("Sar_500");
                CB->modify_var(&ST(0x68), 1);
            }
            if (ST(0xE30) == 1) {
                ST(0xE30) = 2;
                CB->animation("Sar_500");
                CB->modify_var(&ST(0xE0), 1);
            }
        }
        break;

    case 17: if (ST(V_EVENT_17) == 1) ST(V_EVENT_17) = 2; break;

    case 18:
        if ((i32)ST(V_HUD_TIMER_MS) > 0) {
            ST(V_HUD_TIMER_MS) -= 0x1388;
            CB->divers_empile(DV_HUD_REFRESH, 0, 0);
            CB->evenement(0x12, 0x1388);
        }
        break;

    case 19:
        if (ST(0xEBC) == 1 && ST(ST_LEVEL) != 28)
            ST(0xEBC) = 2;
        break;

    case 20:
        if (ST(ST_LEVEL) == 18)
            CB->bruitage("L08B06", 2740, 1983);
        if (ST(ST_LEVEL) == 8) {
            CB->select_pile(2, 1);
            CB->bruitage("L08B06", 44, -960);
            CB->select_pile(0, 1);
        }
        break;

    case 21: if (ST(V_EVENT_21) == 0) ST(V_EVENT_21) = 1; break;
    case 22: if (ST(V_EVENT_22) == 0) ST(V_EVENT_22) = 1; break;
    case 25: if (ST(V_EVENT_25) == 0) ST(V_EVENT_25) = 2; break;
    case 27: if (ST(V_EVENT_27) == 0) ST(V_EVENT_27) = 1; break;

    case 28:
        if (ST(V_EVENT_28) == 0) {
            CB->bloque(BLOQUE_NONE);
            ST(ST_LEVEL) = 23;
            CB->change_piece(25, -1);
        }
        break;

    case 29:
        if      (ST(V_EVENT_29) == 2) ST(V_EVENT_29) = 3;
        else if (ST(V_EVENT_29) == 0) ST(V_EVENT_29) = 1;
        break;

    case 30:
        if (ST(V_EVENT_30) == 1) {
            ST(V_EVENT_30) = 0;
            ST(0xBF4) = 1;
        }
        break;

    case 31: if (ST(V_EVENT_31) == 0) ST(V_EVENT_31) = 1; break;
    case 33: ST(V_EVENT_33) = 1; break;
    case 34: ST(V_EVENT_34) = 1; break;
    case 35: ST(V_EVENT_35) = 1; break;
    case 36: ST(V_EVENT_36) = 1; break;
    case 37: ST(V_EVENT_37) = 1; break;
    case 38: ST(V_EVENT_38) = 1; break;
    case 39: ST(V_EVENT_39) = 1; break;
    case 40: ST(V_EVENT_40) = 1; break;
    case 41: ST(V_EVENT_41) = 1; break;

    case 42:
        if (ST(V_MUSIC_TRACK) != -1) {
            CB->select_pile(2, 1);
            CB->divers_empile(DV_MUSIC, ST(V_MUSIC_TRACK), 4);
            CB->evenement(0x2B, 0x1388);
            CB->select_pile(0, 1);
        }
        break;

    case 43:
        CB->select_pile(2, 1);
        CB->divers_empile(DV_MUSIC, ST(V_MUSIC_TRACK), 1);
        CB->evenement(0x2A, 0xAFC8);
        CB->select_pile(0, 1);
        break;

    case 44: ST(V_END_GAME_REQ) = 1; break;

    case 51: /* 0x3EC chain 0->1,2->3,4->5 */
        if      (ST(V_EVENT_51) == 0) ST(V_EVENT_51) = 1;
        else if (ST(V_EVENT_51) == 2) ST(V_EVENT_51) = 3;
        else if (ST(V_EVENT_51) == 4) ST(V_EVENT_51) = 5;
        break;
    case 52: /* 0x3F0 chain 0->1,2->3 */
        if      (ST(V_EVENT_52) == 0) ST(V_EVENT_52) = 1;
        else if (ST(V_EVENT_52) == 2) ST(V_EVENT_52) = 3;
        break;
    case 53: /* 0x3F4 chain 0->1,2->3,4->5 */
        if      (ST(V_EVENT_53) == 0) ST(V_EVENT_53) = 1;
        else if (ST(V_EVENT_53) == 2) ST(V_EVENT_53) = 3;
        else if (ST(V_EVENT_53) == 4) ST(V_EVENT_53) = 5;
        break;

    case 70:  /* alarm bells (named_two_ints F/G/H at fixed values) */
        CB->select_pile(2, 1); CB->evenement(0x46, 0x2134); CB->select_pile(0, 1);
        break;
    case 71:
        CB->select_pile(2, 1); CB->evenement(0x47, 0x9C4);  CB->select_pile(0, 1);
        break;
    case 72:
        CB->select_pile(2, 1); CB->evenement(0x48, 0x4844); CB->select_pile(0, 1);
        break;
    case 73:
        CB->select_pile(2, 1); CB->evenement(0x46, 0x2904); CB->select_pile(0, 1);
        break;
    case 74:
        CB->select_pile(2, 1); CB->evenement(0x46, 0x7530); CB->select_pile(0, 1);
        break;

    case 80:
        CB->select_pile(0, 2);
        CB->evenement(0x51, 0x9C4);
        CB->animation("SP01ACT99");
        CB->select_pile(0, 1);
        break;

    case 94:  /* pipe puzzle SP02PIPI1/2 by layer query */
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        if (CB->divers(DV_CAN_FIGHT, 9, 2))
            CB->animation("SP02PIPI2");
        else
            CB->animation("SP02PIPI1");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        break;

    case 50: {  /* SOB24%c/SOB77%c via named_ex_988 */
        CB->divers(DV_TRANSITION, 0x7D0, 0x7D0);
        CB->animation("SP01ACT96");
        CB->animation("ar");
        c8 buf1[48], buf2[48];
        snprintf(buf1, sizeof(buf1), "SOB24%c", (char)(ST(ST_USED_VARIANT) + 0x41));
        CB->etat_objet(buf1, -1, -1, -1, -1, -1, -1, -2, 0);
        snprintf(buf2, sizeof(buf2), "SOB77%c", (char)(ST(ST_USED_VARIANT) + 0x41));
        CB->etat_objet(buf2, -1, -1, -1, -1, -1, -1, -3, 0);
        CB->bloque(BLOQUE_NONE);
        break;
    }
    case 81: {  /* P01ACT97 + layer-6 clamp(0x1E) */
        CB->select_pile(0, 2);
        CB->animation("P01ACT97");
        CB->evenement(0x32, 0x320);
        e5_clamp6(0x1E);
        CB->select_pile(0, 1);
        break;
    }
    case 90:  /* float accum grow (index 0) */
        CB->select_pile(2, 1);
        if (g_float_accum[0] < 3.0f && g_float_active != 0) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x10011);
            CB->evenement(0x5A, 0x64);
            g_float_accum[0] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case 91:  /* float accum grow (index 1, dual layer_op) */
        CB->select_pile(2, 1);
        if (g_float_accum[1] < 3.0f && g_float_active != 0) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x10005);
            CB->divers(DV_SCALE_BONE, 0x66, 0x10007);
            CB->evenement(0x5B, 0x64);
            g_float_accum[1] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case 92:  /* float accum grow (index 2, dual layer_op) */
        CB->select_pile(2, 1);
        if (g_float_accum[2] < 3.0f && g_float_active != 0) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x1000E);
            CB->divers(DV_SCALE_BONE, 0x66, 0x10010);
            CB->evenement(0x5C, 0x64);
            g_float_accum[2] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case 93:  /* float accum decay (index 3) */
        CB->select_pile(2, 1);
        if (g_float_accum[3] > 0.5f && g_float_active != 0) {
            CB->divers(DV_SCALE_BODY, 0x62, 0);
            CB->evenement(0x5D, 0x64);
            g_float_accum[3] *= 0.98f;
        }
        CB->select_pile(0, 1);
        break;
    case 98: {  /* float reset + respawn toggle */
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        if (g_float_active != 0) {
            g_float_active = 0;
            e4_reset_floats();
            i32 c2 = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
            i32 c1 = CB->divers(DV_DESC_POS, ST(ST_HERO), 1);
            i32 c0 = CB->divers(DV_DESC_POS, ST(ST_HERO), 0);
            CB->pos_perso(ST(ST_HERO) + 100, c0, c1, c2, 1,
                          "ar", ST(0x138), 0);
            CB->animation("Sar_50");
            CB->evenement(0x62, 0x64);
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
        } else {
            g_float_active = 1;
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
        }
        break;
    }

    /* Original default cases (5,13,23,26,32,45-49,54-69,75-79,82-89,
     * 95-97) plus 3,4,6,10,14,24 are no-ops. */
    default:
        break;
    }
}
/*
 * event5 — sub_100022B4 (14 KB). switch(arg) over LCOD player-actions
 * (arg 1..99; original uses a byte index table -> jump table @ 0x5AAD).
 * Each case is independent. `break` == original `jmp def_10005AAD`.
 *
 * ALL cases ported. Default catches genuine no-ops from the original.
 */

/* Common case prologue (appears in nearly every case). */
static void e5_reset(void) {
    CB->select_pile(0, 2);
    CB->divers_empile(DV_RANDOM, 0, 0);
    CB->divers_empile(DV_RANDOM, 0, 0);
    CB->divers_empile(DV_RANDOM, 0, 0);
    CB->bloque(BLOQUE_ALL);
}

/* case 4 / 22 / 65 template: SG01ATT01 + LCOD41/42/43 + ar_50 */
static void e5_att01(void) {
    e5_reset();
    CB->animation("SG01ATT01");
    if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
    if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
    if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
    CB->animation("ar_50");
    CB->bloque(BLOQUE_NONE);
}

/* Common tail: layer-6 value += k, clamped to 0x3FF (op 6 on [0x194]). */
static void e5_clamp6(i32 k) {
    if (CB->divers(DV_HP, ST(ST_HERO), -1) + k < 0x400)
        CB->divers_empile(DV_HP, ST(ST_HERO), CB->divers(6, ST(ST_HERO), -1) + k);
    else
        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
}

/* SGxxMAN03/04 (levels 2,3) by [0x198], + ar_50 + clamp(k). */
static void e5_man34(i32 k) {
    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG02MAN03");
        if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG02MAN04");
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG03MAN03");
        if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG03MAN04");
    }
    CB->animation("ar_50");
    e5_clamp6(k);
}

/* SG01ATT01 + LCOD pair (lvl2->a, lvl3->b) + ar_50. */
static void e5_att01_lcod(const c8 *a, const c8 *b) {
    CB->animation("SG01ATT01");
    if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue(a, ST(ST_HERO));
    if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue(b, ST(ST_HERO));
    CB->animation("ar_50");
}

static void event5(i32 arg, i32 phase) { /* sub_100022B4(ecx=arg, edx=phase) */
    switch (arg) {

    case 4:   /* 0x22E1 */
    case 22:  /* 0x23C1 */
    case 65:  /* 0x2C0A */
        e5_att01();
        break;

    case 23:  /* 0x24A1 (shared 23/27/35/55) */
    case 27:
    case 35:
    case 55:
        if (ST(ST_LEVEL) == 35) {
            e5_reset();
            CB->animation("SG00ATT01");
            CB->bloque(BLOQUE_NONE);
        }
        break;

    case 42:  /* 0x250D */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case 49:  /* 0x25ED — Sar imp interaction (LCOD02/03 A-H) */
        if (ST(V_LCOD02_SEEN) == 0 || ST(V_LCOD02_SEEN) == 1) {
            e5_reset();
            if (ST(ST_HERO) == P_ARKHAN2) {
                l01_action_register("LCOD02A", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02B", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02C", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02D", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02E", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02F", 2, "SP02ACT64", "SP02ACT64");
                if (ST(V_LCOD02_SEEN) == 0) {
                    l01_action_register("LCOD02G", 2, "SP02ACT64", "SP02ACT64");
                    l01_action_register("LCOD02H", 2, "SP02ACT64", "SP02ACT64");
                }
                ST(V_LCOD02_SEEN) = 1;
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                l01_action_register("LCOD03A", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03B", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03C", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03D", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03E", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03F", 3, "SP03ACT64", "SP03ACT64");
                if (ST(V_LCOD02_SEEN) == 0) {
                    l01_action_register("LCOD03G", 3, "SP03ACT64", "SP03ACT64");
                    l01_action_register("LCOD03H", 3, "SP03ACT64", "SP03ACT64");
                }
                ST(V_LCOD02_SEEN) = 1;
            }
            if (ST(ST_LEVEL) == 2 && ST(ST_LOCKED_CAMERA) == 1 && ST(0xEB8) == 1
                && CB->divers(DV_PERSONA_PRESENT, P_PHEDORIA, 0)) {
                CB->divers(DV_COMBAT_STANCE, 0, 0);
                if (CB->distance(20, 31) < 0x96)
                    CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x1A);
                else
                    CB->joindre(-ST(ST_HERO), -1, -1, -1, 0x1A);
                if (CB->distance(20, 31) >= 0x96)
                    CB->evenement(0x29, 0x7D0);
                else if (CB->distance(20, 31) < 0x96 && CB->direction_perso(P_PHEDORIA) == 0)
                    CB->evenement(0x29, 0x7D0);
                else
                    CB->evenement(0x29, 0x1F4);
            } else {
                CB->animation("Sar_50");
                CB->bloque(BLOQUE_NONE);
            }
        } else {
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;

    case 50:  /* 0x29A5 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD88", 2, "SG00DIA02", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD89", 3, "SG00DIA02", NULL);
        CB->bloque(BLOQUE_NONE);
        break;

    case 46:  /* 0x2A47 — LCOD100/101/102/103/104 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) {
            l01_action_register("LCOD100A", 2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD100B", P_ARKHAN2);
            CB->dialogue("LCOD100C", P_ARKHAN2);
            CB->dialogue("LCOD100D", P_ARKHAN2);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            l01_action_register("LCOD101A", 3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD101B", P_ARKHAN3);
            CB->dialogue("LCOD101C", P_ARKHAN3);
            CB->dialogue("LCOD101D", P_ARKHAN3);
        }
        CB->dialogue("LCOD102A", P_ARKHAN2);
        CB->dialogue("LCOD102B", P_ARKHAN2);
        CB->dialogue("LCOD102C", P_ARKHAN2);
        CB->dialogue("LCOD102D", P_ARKHAN2);
        CB->dialogue("LCOD102E", P_ARKHAN2);
        CB->dialogue("LCOD102F", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD103", 2, "SG02DIA01", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD104", 3, "SG03DIA01", NULL);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case 70:  /* 0x2CEA */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD90", 2, "SG00DIA02", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD91", 3, "SG00DIA02", NULL);
        CB->bloque(BLOQUE_NONE);
        break;

    case 13:  /* 0x39E1 — Sar mannequin dialogue + layer clamp */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG02MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG02MAN02");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG03MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG03MAN02");
        }
        CB->animation("ar_50");
        e5_clamp6(0x64);
        CB->bloque(BLOQUE_NONE);
        break;

    case 77:  /* 0x3575 — SPxxACT17 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT17");
        if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ACT17");
        if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ACT17");
        CB->animation("ar_50");
        if (ST(ST_HERO) == P_ARKHAN1) l01_action_register("LCOD41", ST(ST_HERO), "SG01ATT01", NULL);
        if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD42", ST(ST_HERO), "SG01ATT01", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD43", ST(ST_HERO), "SG01ATT01", NULL);
        CB->bloque(BLOQUE_NONE);
        break;

    case 81:  /* 0x36B3 — LCOD23/24 A-G */
        e5_reset();
        if (ST(V_LCOD23_SEEN) == 0) ST(V_LCOD23_SEEN) = 1;
        if (ST(ST_LEVEL) == 28) ST(0x84) = 1;
        if (ST(ST_HERO) == P_ARKHAN2) {
            l01_action_register("LCOD23A", 2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD23B", P_ARKHAN2);
            CB->dialogue("LCOD23C", P_ARKHAN2);
            CB->dialogue("LCOD23D", P_ARKHAN2);
            CB->dialogue("LCOD23E", P_ARKHAN2);
            CB->dialogue("LCOD23F", P_ARKHAN2);
            l01_action_register("LCOD23G", 2, "SG02DIA01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            l01_action_register("LCOD24A", 3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD24B", P_ARKHAN3);
            CB->dialogue("LCOD24C", P_ARKHAN3);
            CB->dialogue("LCOD24D", P_ARKHAN3);
            CB->dialogue("LCOD24E", P_ARKHAN3);
            CB->dialogue("LCOD24F", P_ARKHAN3);
            l01_action_register("LCOD24G", 3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case 84:  /* 0x3871 */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD672", ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD673", ST(ST_HERO));
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case 85:  /* 0x3929 */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD674", ST(ST_HERO));
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD675", ST(ST_HERO));
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case 34:  /* 0x44E2 — SGxxMAN01/02 + clamp 0x4B */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG02MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG02MAN02");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG03MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG03MAN02");
        }
        CB->animation("ar_50");
        e5_clamp6(0x4B);
        CB->bloque(BLOQUE_NONE);
        break;

    case 36:  /* 0x464F — SGxxMAN03/04 + clamp 0x4B */
        e5_reset(); e5_man34(0x4B); CB->bloque(BLOQUE_NONE);
        break;

    case 43:  /* 0x47BC — SGxxMAN03/04 + clamp 0x64 */
        e5_reset(); e5_man34(0x64); CB->bloque(BLOQUE_NONE);
        break;

    case 56:  /* 0x4B32 — MAN03/04 + clamp 0x4B */
        e5_reset(); e5_man34(0x4B); CB->bloque(BLOQUE_NONE);
        break;

    case 64:  /* 0x4C9F — MAN03/04 + clamp 0x64 */
        e5_reset(); e5_man34(0x64); CB->bloque(BLOQUE_NONE);
        break;

    case 95:  /* 0x4E0C — MAN03/04 + clamp 0x96 */
        e5_reset(); e5_man34(0x96); CB->bloque(BLOQUE_NONE);
        break;

    case 44:  /* 0x4929 — SGxxMAN (levels 0-3) + clamp 0x96 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG00MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG00MAN02");
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG01MAN01");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG01MAN02");
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG02MAN03");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG02MAN04");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SG03MAN03");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SG03MAN04");
        }
        CB->animation("ar_50");
        e5_clamp6(0x96);
        CB->bloque(BLOQUE_NONE);
        break;

    case 1:   /* 0x5161 — SG01ATT01 + LCOD26/27 */
        e5_reset(); e5_att01_lcod("LCOD26", "LCOD27"); CB->bloque(BLOQUE_NONE);
        break;

    case 2:   /* 0x5219 (shared 2/12/18/19) — SG01ATT01 + L25D79/82 */
    case 12:
    case 18:
    case 19:
        e5_reset(); e5_att01_lcod("L25D79", "L25D82"); CB->bloque(BLOQUE_NONE);
        break;

    case 3:   /* 0x52D1 — state_toggle(1,1) + SG01ATT01 + LCOD39/40 */
        CB->select_pile(1, 1);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->bloque(BLOQUE_ALL);
        e5_att01_lcod("LCOD39", "LCOD40");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x538C — big shared group: SG01ATT01 + LCOD39/40 */
    case 5: case 6: case 7: case 8: case 9: case 10: case 11:
    case 14: case 15: case 16: case 20: case 29: case 33:
    case 39: case 40: case 41: case 45: case 51: case 60:
    case 72: case 73: case 74: case 75: case 86: case 87:
    case 89: case 93: case 94: case 96:
        e5_reset(); e5_att01_lcod("LCOD39", "LCOD40"); CB->bloque(BLOQUE_NONE);
        break;

    case 82:  /* 0x5444 — LCOD55/56 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD55", 2, "SP02ACT64", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD56", 3, "SP02ACT64", NULL);
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case 99:  /* 0x4F7D — SPxxATT20 first-time, else LCOD41/42/43 */
        e5_reset();
        if (ST(V_ATT20_SEEN) == 0) {
            ST(V_ATT20_SEEN) = 1;
            if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ATT20");
            if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT20");
            if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP02ATT20");
            e5_clamp6(0x96);
            CB->animation("ar_50");
        } else {
            if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
            if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
            if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
            CB->animation("SG01ATT01");
            CB->animation("ar_50");
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case 21: {  /* 0x3B4E — OB21A/B/C by phase (edx), ATT19/20 + clamp */
        const c8 *ob;
        if      (phase == 0) ob = "OB21A";
        else if (phase == 1) ob = "OB21B";
        else                 ob = "OB21C";
        e5_reset();
        if (ST(ST_SPECIAL_MODE) == 1) {
            if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT19");
            if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT19");
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT20");
            if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT20");
        }
        CB->animation("ar_50");
        CB->etat_objet(ob, -1, -1, -1, -1, -1, -1, -2, 3);
        e5_clamp6(0xC8);
        CB->bloque(BLOQUE_NONE);
        break;
    }

    case 24: {  /* 0x3F9C — OB24%c/OB77%c timer, phase=array index */
        if (ST(ST_COMBAT_STANCE) != 0) break;
        e5_reset();
        if (ST(ST_SPECIAL_MODE) != 1 && (ST(ST_SPECIAL_MODE) != 0 || ST(ST_HERO) == P_ARKHAN1))
            goto case24_fallback;
        {
            i32 *timer = &ST(0x140 + phase * 4);
            if (*timer > 0) {
                (*timer)--;
                if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT98");
                if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT19");
                if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT19");
                e5_clamp6(0x1E);
                CB->animation("ar");
                CB->bloque(BLOQUE_NONE);
                break;
            }
            if (*timer == 0) {
                if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT98");
                if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT19");
                if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT19");
                c8 b1[16], b2[16];
                snprintf(b1, sizeof b1, "OB24%c", phase + 'A');
                snprintf(b2, sizeof b2, "OB77%c", phase + 'A');
                CB->etat_objet(b1, -1, -1, -1, -1, -1, -1, -2, 0);
                CB->etat_objet(b2, -1, -1, -1, -1, -1, -1, -3, 0);
                e5_clamp6(0x1E);
                if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
                if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
                if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
                CB->animation("SG01ATT01");
                CB->animation("ar_50");
                CB->bloque(BLOQUE_NONE);
                break;
            }
        }
    case24_fallback:
        if (ST(ST_SPECIAL_MODE) == 0 && ST(ST_HERO) == P_ARKHAN1) {
            ST(0x140 + phase * 4) = 0;
            CB->animation("SP01ACT98");
            CB->evenement(0x50, 0x7D0);
        }
        CB->bloque(BLOQUE_NONE);
        break;
    }

    case 31: {  /* 0x4385 — OB31 + layer 0x14 reduction */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ATT20");
        if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ATT20");
        CB->animation("ar_50");
        e5_clamp6(0x32);
        CB->animation("ar_50");
        {
            i32 lv = CB->divers(DV_DARKNESS, -1, 0);
            CB->divers_empile(DV_DARKNESS, lv - 0x96, 0);
        }
        CB->etat_objet("OB31", -1, -1, -1, -1, -1, -1, -2, 0);
        CB->bloque(BLOQUE_NONE);
        break;
    }

    case 71: {  /* 0x2D8C — SOB86 FSM (multi-branch room logic) */
        CB->select_pile(0, 2);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        CB->divers_empile(DV_RANDOM, 0, 0);
        if (ST(V_SOB86_ACTIVE) == 0) {
            if (ST(ST_LEVEL) == 30) {
                if (ST(ST_LOCKED_CAMERA) != 5 || ST(ST_VIE(P_KONKALITE2)) != VIE_DEAD) goto case71_lcod;
                ST(V_SOB86_ACTIVE) = 1;
                i32 v4 = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
                CB->bloque(BLOQUE_ALL);
                CB->divers_empile(DV_SCREEN, 5, 0);
                if (ST(ST_HERO) == P_ARKHAN2)
                    CB->pos_perso(P_ARKHAN2, 4444, 5790, v4, 1, "ar", 1, 0);
                if (ST(ST_HERO) == P_ARKHAN3)
                    CB->pos_perso(P_ARKHAN3, 4444, 5790, v4, 1, "ar", 1, 0);
                CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(0x62, 4444, 5790, v4, 1, "ar", 1, 0);
                    CB->pos_perso(0x62, 4444, 5790, v4, 1, "ar", 1, 0);
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(0x63, 4444, 5790, v4, 1, "ar", 1, 0);
                    CB->pos_perso(0x63, 4444, 5790, v4, 1, "ar", 1, 0);
                }
                CB->divers_empile(DV_SCREEN, 3, 0);
                if (ST(V_HUD_TIMER_MS) == 0) {
                    ST(V_SOB86_ACTIVE) = 0;
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("L31D83A", P_ARKHAN2);
                        CB->dialogue("L31D83B", P_ARKHAN2);
                        if (ST(0xC50) == 0) CB->dialogue("L31D83C", P_ARKHAN2);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("L31D84A", P_ARKHAN3);
                        CB->dialogue("L31D84B", P_ARKHAN3);
                        if (ST(0xC50) == 0) CB->dialogue("L31D84C", P_ARKHAN3);
                    }
                    CB->animation("Sar_50");
                    CB->divers_empile(DV_SCREEN, 5, 0);
                    if (ST(ST_HERO) == P_ARKHAN2)
                        CB->pos_perso(P_ARKHAN2, 4444, 5790, v4, 0, "ar", 1, 0);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        CB->pos_perso(P_ARKHAN3, 4444, 5790, v4, 0, "ar", 1, 0);
                    CB->divers_empile(DV_SCREEN, 3, 0);
                }
                CB->animation("Sar_50");
                CB->bloque(BLOQUE_NONE);
                break;
            }
            if (ST(ST_LEVEL) == 37) {
                if (ST(ST_LOCKED_CAMERA) != 0 || CB->distance(1, 6) >= 0x15E)
                    goto case71_lcod;
                ST(V_SOB86_ACTIVE) = 1;
                CB->bloque(BLOQUE_ALL);
                CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                CB->divers_empile(DV_SCREEN, 5, 0);
                if (ST(ST_HERO) == P_ARKHAN2)
                    CB->pos_perso(0x62,
                        CB->divers(1, ST(ST_HERO), 0),
                        CB->divers(1, ST(ST_HERO), 1),
                        CB->divers(1, ST(ST_HERO), 2),
                        1, "ar", 6, 0);
                if (ST(ST_HERO) == P_ARKHAN3)
                    CB->pos_perso(0x63,
                        CB->divers(1, ST(ST_HERO), 0),
                        CB->divers(1, ST(ST_HERO), 1),
                        CB->divers(1, ST(ST_HERO), 2),
                        1, "ar", 6, 0);
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("Sar_50");
                CB->bloque(BLOQUE_NONE);
                break;
            }
        case71_lcod:
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD666", 2, "SG02DIA02", NULL);
            if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD667", 3, "SG03DIA02", NULL);
            CB->bloque(BLOQUE_NONE);
            break;
        } else {
            if (ST(ST_LEVEL) == 35) {
                CB->bloque(BLOQUE_ALL);
                if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD670", 2, "SG02DIA02", NULL);
                if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD671", 3, "SG03DIA02", NULL);
                CB->bloque(BLOQUE_NONE);
                break;
            }
            ST(V_SOB86_ACTIVE) = 0;
            CB->bloque(BLOQUE_ALL);
            CB->divers_empile(DV_SCREEN, 5, 0);
            if (ST(ST_HERO) == P_ARKHAN2)
                CB->pos_perso(P_ARKHAN2,
                    CB->divers(1, 2, 0), CB->divers(1, 2, 1),
                    CB->divers(1, 2, 2), 0, "ar", 1, 0);
            if (ST(ST_HERO) == P_ARKHAN3)
                CB->pos_perso(P_ARKHAN3,
                    CB->divers(1, 3, 0), CB->divers(1, 3, 1),
                    CB->divers(1, 3, 2), 0, "ar", 1, 0);
            CB->divers_empile(DV_SCREEN, 3, 0);
            if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD668", 2, "SG02DIA01", NULL);
            if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD669", 3, "SG03DIA01", NULL);
            CB->bloque(BLOQUE_NONE);
            break;
        }
    }

    case 88: {  /* 0x54F1 — SPxxATT02 health decrement */
        e5_reset();
        if (ST(0x6D8) == 0) {
            if (ST(ST_SPECIAL_MODE) == 0) {
                ST(0x6D8) = 1;
                if (ST(ST_HERO) == P_ARKHAN1) {
                    i32 v = CB->divers(DV_HP, P_ARKHAN1, -1);
                    if (v - 0x32 > 0x64) CB->divers(DV_HP, P_ARKHAN1, v - 0x32);
                    CB->animation("SP01ATT02");
                    CB->animation("P01ATT03");
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    i32 v = CB->divers(DV_HP, P_ARKHAN2, -1);
                    if (v - 0x1E > 0x32) CB->divers(DV_HP, P_ARKHAN2, v - 0x1E);
                    CB->animation("SP02ATT02");
                    CB->animation("P02ATT03");
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    i32 v = CB->divers(DV_HP, P_ARKHAN3, -1);
                    if (v - 0x14 > 0x32) CB->divers(DV_HP, P_ARKHAN3, v - 0x14);
                    CB->animation("SP02ATT02");
                    CB->animation("P02ATT03");
                    CB->animation("ar_50");
                }
            }
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(ST_HERO) == P_ARKHAN1) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD25", P_ARKHAN1);
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD26", P_ARKHAN2);
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD27", P_ARKHAN3);
                    CB->animation("ar_50");
                }
            }
        } else {
            if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD41", ST(ST_HERO));
            if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD42", ST(ST_HERO));
            if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD43", ST(ST_HERO));
            CB->animation("SG01ATT01");
            CB->animation("ar_50");
        }
        CB->bloque(BLOQUE_NONE);
        break;
    }

    /* Original default cases (17,25,26,28,30,32,37,38,47,48,52-54,57-59,
     * 61-63,66-69,76,78-80,83,90-92,98) are genuine no-ops. */
    default:
        break;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail from FonctionEditor case 1.
 * Callback ingestion is skipped: g_lieu_cb is provided by the bridge.
 *   arg = reload flag (non-zero => skip arming).
 * ---------------------------------------------------------------------- */
static void init(i32 reload_flag) {
    if (reload_flag)
        return;

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
        l01_trig00();   /* sub_10001073 default path */
    } else {
        CB->divers_empile(DV_SCREEN, 1, 0);
        ST(V_DEATH_EVENT_SENT) = 1;
        for (i32 layer = 0; layer < 4; layer++) {
            if (CB->divers(DV_PERSONA_PRESENT, layer, 0))
                CB->divers(DV_HP, layer, 0);
        }
        if (!ST(V_GUARD_FIGHT_DONE))
            CB->cinematique("SC22_500");
        else
            CB->cinematique("SC23_500");
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
}

/* -------------------------------------------------------------------------
 * TICK (opcode 0) + DEINIT (opcode -1): drain pending sound, run body.
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
 * Entry point — mirrors @FonctionEditor@16 opcode dispatch.
 * ---------------------------------------------------------------------- */
i32 g_lieu_trace = 0;

i32 lieu01_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_l01_S = state;
    if (g_lieu_trace)
        print("lieu01: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:   init(arg);                    break;
    case LIEU_OP_EVENT:  l01_trigger(arg); break;
    case LIEU_OP_CALL4:  event4(arg);                  break;
    case LIEU_OP_CALL5:  event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
