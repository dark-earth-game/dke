/*
 * lieu02.c — Native hand-port of LIEU02.DLL (level-2 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick, event4/5/6 and the
 * shared DLL helpers. The 42 zone trigger handlers live in lieu02_trig.c
 * (reached through l02_trigger, see lieu02_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Same compiler/layout as LIEU01.DLL; every
 * DLL global sits 0x20 bytes higher than in LIEU01 (engine slots C4xx..C9xx,
 * state pointer dword_1002C98C, trigger table dword_1002C8A0[42]), so the
 * lieu_cb_t members map 1:1 (LIEU02 slot = LIEU01 slot + 0x20).
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * sub_10001200 (float reset), the entry and INIT are instruction-for-
 * instruction identical to LIEU01 (only relocations differ), so those
 * bodies are the lieu01.c port verbatim. tick_body (sub_10009DC8) is
 * level-2 specific.
 */

#include <stdio.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "lieu02_int.h"

/* Current state buffer for the in-flight call (the DLL's dword_1002C98C;
 * the entry point sets it per call). */
lieu_state_t *g_l02_S;

/* Global sprintf buffer of the debug overlay (dword_1002C6A0). */
static c8 g_txt[256];

/* -------------------------------------------------------------------------
 * l02_action_register — sub_1001A1AC (thunk 0x1000100A).
 * Same code as LIEU01's sub_1001932C.
 * Original __fastcall(ecx=name, edx=count) + stdcall stack (arg0, arg4).
 *   named_object(arg0)
 *   named_object_int(name, count)
 *   if      (count < 4 && arg4 == NULL) named_object("Sar_20")
 *   else if (arg4 != NULL)              named_object(arg4)
 *   else { sprintf(b,"SP%02darr01_10",count); named_object(b) }
 * ---------------------------------------------------------------------- */
void l02_action_register(const c8 *name, i32 count,
                         const c8 *arg0, const c8 *arg4) {
    CB->animation(arg0);
    CB->dialogue(name, count);
    if (count < 4 && arg4 == NULL) {
        CB->animation("Sar_20");
    } else if (arg4 != NULL) {
        CB->animation(arg4);
    } else {
        c8 b[256];
        snprintf(b, sizeof(b), "SP%02darr01_10", count);
        CB->animation(b);
    }
}

/* -------------------------------------------------------------------------
 * event6 — sub_10005BEC (identical to LIEU01). Switch on arg (0..4):
 * LCOD zone triggers.
 * ---------------------------------------------------------------------- */
static void event6(i32 arg) {
    switch (arg) {
    case 0:  /* loc_10005C0A */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_COMBAT_STANCE) == 0 && ST(V_LCOD34_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD34_SHOWN) = 1;
            CB->bloque(BLOQUE_ALL);
            l02_action_register("LCOD34", 2, "SG00CON01", NULL);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case 1:  /* loc_10005C87 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(V_LCOD35_SHOWN) == 0) {
            CB->select_pile(0, 1);
            ST(V_LCOD35_SHOWN) = 1;
            ST(V_LCOD37_SHOWN) = 0;
            CB->bloque(BLOQUE_ALL);
            l02_action_register("LCOD35", 2, "SG00CON01", NULL);
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
            l02_action_register("LCOD36", 3, "SG00CON01", "SG00CON01");
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
            l02_action_register("LCOD37", 3, "SG00CON01", NULL);
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

/* -------------------------------------------------------------------------
 * Per-frame body — sub_10009DC8 (thunk 0x100010B9), level-2 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp loc_1000D33C` (epilogue).
 * ---------------------------------------------------------------------- */
/* engine slot C988 (lieu_cb_t.cb_968, offset 0x7C, sub_446B54): OnJoindre,
 * nonzero while the actor is still walking. */
static i32 cb968(i32 id) { return CB->on_joindre(id); }

/* Hit drain shared by the guard blocks of tick_body (inlined 3x in the
 * original, e.g. 0xA5C1): life = layer_op(6,a,-1) stored in [0x84];
 * life -= damage [0x130], floored at 1. */
static void e_hit_drain(i32 a) {
    ST(0x84) = CB->divers(DV_HP, a, -1);
    if (ST(0x84) - ST(V_HIT_DAMAGE) > 0)
        CB->divers(DV_HP, a, ST(0x84) - ST(V_HIT_DAMAGE));
    else
        CB->divers(DV_HP, a, 1);
}

/* Actor 33 walks back to its post (inlined 2x in the original:
 * 0xBB0C..0xBDE2 and 0xC227..0xC4FD). Returns 1 where the original
 * jumps to the tick_body epilogue. Ends with state_toggle(0,1) on every
 * path. */
static i32 e_a33_walk_back(void) {
    if (ST(0x50) != 3 && ST(0xF3C) == 1) {
        if (ST(0x50) == 0 || ST(0x50) == 1) {
            CB->joindre(-P_KALHI33, -510, -2120, -1, -1);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 2);
            CB->joindre(-P_KALHI33, -510, -1950, -1, -1);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 3);
            CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
            CB->select_pile(0, 1);
            return 1;
        }
        if (ST(0x50) == 2) {
            CB->joindre(-P_KALHI33, -510, -1950, -1, -1);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 3);
            CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
            CB->select_pile(0, 1);
            return 1;
        }
    }
    if (ST(0x50) != 3 && ST(0xF3C) == 2) {
        if (ST(0x50) == 0 || ST(0x50) == 1) {
            CB->joindre(-P_KALHI33, -510, -571, -1, -1);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 2);
            CB->joindre(-P_KALHI33, -510, -740, -1, -1);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 3);
            CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
            CB->select_pile(0, 1);
            return 1;
        }
        if (ST(0x50) == 2) {
            CB->joindre(-P_KALHI33, -510, -740, -1, -1);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 3);
            CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
            CB->select_pile(0, 1);
            return 1;
        }
    }
    if (ST(0x50) == 3)
        CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
    CB->select_pile(0, 1);
    return 0;
}

static void tick_body(void) {
    /* 0x9DCE */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        CB->evenement(0x2C, 0xBB8);
    }
    /* 0x9E11 — intro cinematic replay */
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
    /* 0x9F30 — door 2 / SP26 arrival */
    if (ST(V_EVENT_41) == 1) {
        ST(V_EVENT_41) = 0;
        CB->stop_joindre(-ST(ST_HERO));
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x1A);
        CB->vire_col(0x21, 0);
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D209", 2, "SG00DIA03", "Sar_10");
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D210", 3, "SG00DIA03", "Sar_10");
        if (CB->distance(20, 31) < 0x50) {
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02MAR30");
                CB->animation("P02_65535");
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03MAR30");
                CB->animation("P03_65535");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02PRE80");
            CB->animation("G02PRE81");
            CB->animation("SP26PRE60");
            CB->animation("P26_65535");
            CB->animation("SG02PRE82");
            if (ST(ST_PLAYER_WEAPON) != 0) CB->animation("G02ARR02_10");
            else                CB->animation("G02ARR01_10");
            CB->animation("SP26ARR01_10");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03PRE80");
            CB->animation("G03PRE81");
            CB->animation("SP26PRE60");
            CB->animation("P26_65535");
            CB->animation("SG03PRE82");
            if (ST(ST_PLAYER_WEAPON) != 0) CB->animation("G03ARR02_10");
            else                CB->animation("G03ARR01_10");
            CB->animation("SP26ARR01_10");
        }
        CB->animation("SP26PRE91");
        CB->animation("P26ARR01_10");
        CB->divers_empile(DV_LOOK_AT_PLAYER, P_PHEDORIA, 1);
        l02_action_register("L02D211", 0x1A, "P26DIA60", "SP26ARR03_10");
        CB->divers_empile(DV_LOOK_AT_PLAYER, P_PHEDORIA, 0);
        CB->joindre(-P_PHEDORIA, 0, 0, 0xC350, -1);
        CB->animation("SP26OUV01");
        CB->bruitage("porte2", 1662, -1886);
        CB->anim_objet("IM03", 3, 0x3C, 0x5DC, 0);
        CB->animation("P00_65535");
        ST(ST_LEVEL) = 2;
        CB->divers_empile(DV_MUSIC, 1, 4);
        CB->change_piece(6, 1);
        return;
    }
    /* 0xA231 */
    if (ST(0xEB8) == 1 && ST(V_GUARD_FIGHT_DONE) == 0
        && (ST(ST_LOCKED_CAMERA) == 2 || ST(ST_LOCKED_CAMERA) == 1)
        && ST(ST_COMBAT_STANCE) == 1
        && CB->divers(DV_CAN_FIGHT, 9, 2)
        && CB->direction_perso(P_PHEDORIA)
        && ST(V_PLAYER_STRIKE) == 0 && ST(ST_HERO) != 4) {
        ST(V_PLAYER_STRIKE) = 1;
        ST(V_PHEDORIA_TALK_B) = 1;
        l02_trig31();   /* call 0x10001000 -> sub_100149FB = trigger[31] */
        return;
    }
    /* 0xA2FB — corpse 13 */
    if (CB->divers(DV_WAS_HIT, P_KALHI13, 0) && ST(ST_VIE(P_KALHI13)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 0;
        ST(ST_VIE(P_KALHI13)) = VIE_DEAD;
        ST(0xE58) = 1;
        CB->select_pile(1, 2);
        CB->combat(P_KALHI13, -1);
        CB->stop_joindre(-P_KALHI13);
        CB->animation("SP13IMP52");
        CB->animation("P13CAD");
        CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13CAD", 0x1A, 0);
        CB->select_pile(0, 1);
    }
    /* 0xA3BB — corpse 33 */
    if (CB->divers(DV_WAS_HIT, P_KALHI33, 0) && ST(ST_VIE(P_KALHI33)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 0;
        ST(ST_VIE(P_KALHI33)) = VIE_DEAD;
        ST(0xE58) = 1;
        CB->select_pile(1, 2);
        CB->combat(P_KALHI33, -1);
        CB->stop_joindre(-P_KALHI33);
        CB->animation("SP33IMP52");
        CB->animation("P33CAD");
        CB->pos_perso(P_KALHI33, 0, 0, 0, 2, "P33CAD", 0x1B, 0);
        CB->select_pile(0, 1);
    }
    /* 0xA47B — corpse 10 */
    if (CB->divers(DV_WAS_HIT, P_DELIA, 0) && ST(ST_VIE(P_DELIA)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 0;
        ST(ST_VIE(P_DELIA)) = VIE_DEAD;
        ST(V_DELIA_KILLED) = 2;
        ST(V_DELIA_PHASE) = 0x14;
        CB->select_pile(1, 2);
        CB->combat(P_DELIA, -1);
        CB->stop_joindre(-P_DELIA);
        CB->animation("SP10IMP51");
        CB->animation("P10CAD");
        CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10CAD", 0x10, 0);
        CB->select_pile(0, 1);
    }
    /* 0xA54A — actor 57 hit */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN17, 0) && ST(ST_VIE(P_GUARDIAN17)) != VIE_DEAD) {
        CB->select_pile(1, 2);
        ST(V_ENGAGED_PERSONA) = 0x39;
        CB->divers_empile(DV_WAS_HIT, P_GUARDIAN17, 1);
        CB->combat(P_GUARDIAN17, -1);
        CB->vire_col(0x21, 0);
        e_hit_drain(0x39);
        if (CB->divers(DV_HP, P_GUARDIAN17, -1) <= 1) {
            ST(ST_VIE(P_GUARDIAN17)) = VIE_DEAD;
            CB->stop_joindre(-P_GUARDIAN17);
            CB->animation("SP57IMP52");
            CB->animation("P57CAD");
            CB->pos_perso(P_GUARDIAN17, 0, 0, 0, 2, "P57CAD", 7, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
        } else {
            CB->stop_joindre(-P_GUARDIAN17);
            CB->animation("SP57IMP12");
            CB->animation("P57IMP47");
            CB->animation("P57GAR02");
            CB->combat(P_GUARDIAN17, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0xA701 — actor 58 hit */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN18, 0) && ST(ST_VIE(P_GUARDIAN18)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 0x3A;
        CB->select_pile(1, 2);
        CB->divers_empile(DV_WAS_HIT, P_GUARDIAN18, 1);
        CB->combat(P_GUARDIAN18, -1);
        CB->vire_col(0x21, 0);
        e_hit_drain(0x3A);
        if (CB->divers(DV_HP, P_GUARDIAN18, -1) <= 1) {
            ST(ST_VIE(P_GUARDIAN18)) = VIE_DEAD;
            CB->stop_joindre(-P_GUARDIAN18);
            CB->animation("SP58IMP55");
            CB->animation("P58CAD");
            CB->pos_perso(P_GUARDIAN18, 0, 0, 0, 2, "P58CAD", 0x0B, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
        } else {
            CB->stop_joindre(-P_GUARDIAN18);
            CB->animation("SP58IMP15");
            CB->animation("P58IMP50");
            CB->animation("P58GAR05");
            CB->combat(P_GUARDIAN18, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0xA8B8 — actor 59 hit */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN19, 0) && ST(ST_VIE(P_GUARDIAN19)) != VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 0x3B;
        CB->select_pile(1, 2);
        CB->divers_empile(DV_WAS_HIT, P_GUARDIAN19, 1);
        CB->combat(P_GUARDIAN19, -1);
        CB->vire_col(0x20, 0);
        e_hit_drain(0x3B);
        if (CB->divers(DV_HP, P_GUARDIAN19, -1) <= 1) {
            ST(ST_VIE(P_GUARDIAN19)) = VIE_DEAD;
            CB->stop_joindre(-P_GUARDIAN19);
            CB->animation("SP59IMP53");
            CB->animation("P59CAD");
            CB->pos_perso(P_GUARDIAN19, 0, 0, 0, 2, "P59CAD", 2, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
        } else {
            CB->stop_joindre(-P_GUARDIAN19);
            CB->animation("SP59IMP13");
            CB->animation("P59IMP48");
            CB->animation("P59GAR03");
            CB->combat(P_GUARDIAN19, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0xAA6F */
    if (ST(V_EVENT_34) == 1 && ST(ST_HERO) == P_ARKHAN1) {
        ST(V_EVENT_34) = 0;
        CB->animation("SP01IMP51");
        CB->evenement(0x26, 0x7D0);
        CB->animation("P84ACT02");
        CB->animation("P84ARR01_1000");
    }
    /* 0xAAD3 */
    if (ST(V_EVENT_38) == 1) {
        ST(V_EVENT_38) = 0;
        CB->select_pile(1, 1);
        CB->divers_empile(DV_BONE_SOUND, ST(ST_HERO), 0);
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
        CB->select_pile(0, 1);
    }
    /* 0xAB58 */
    if ((i32)ST(0xAF4) < 3 && ST(V_EVENT_2) == 1 && ST(0xEC8) == 1) {
        ST(V_EVENT_2) = 0;
        CB->animation("SG01ATT07");
        CB->animation("P84ARR02_10");
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
    }
    /* 0xABE2 — guard 57 alerted (hero 1) */
    if (ST(ST_HERO) == P_ARKHAN1 && ST(V_ENGAGED_PERSONA) == 0x39 && ST(0xE0) == 0) {
        ST(0xE0) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L02D107", P_GUARDIAN17);
        if (CB->divers(DV_PERSONA_PRESENT, P_DHORKAN11, 0))
            CB->pos_perso(P_DHORKAN11, -1, -1, -1, 4, "P11ARR02", -1, 1);
        if (CB->divers(DV_PERSONA_PRESENT, P_ZED84, 0))
            CB->pos_perso(P_ZED84, -1, -1, -1, 4, "P84ARR01", -1, 1);
        CB->pos_perso(P_PHEDORIA, 1030, -1600, 0xAB38, 1, "P26ARR03_50", 0x1F, 0);
        CB->change_camera(2, CAM_LOCKED);
        CB->animation("SP26GAR26");
        CB->animation("P26GAR03_50");
        CB->modify_var(&ST(0xE0), 2);
        CB->combat(P_GUARDIAN17, -1);
    }
    /* 0xAD23 — guard 57 alerted (heroes 2/3) */
    if ((i32)ST(ST_HERO) > 1 && ST(V_ENGAGED_PERSONA) == 0x39 && ST(0xE0) == 0
        && CB->distance(22, 7) > 0x12C) {
        CB->select_pile(1, 2);
        ST(V_GUARD17_AWAY) = 1;
        ST(0xE0) = 1;
        if (ST(ST_LOCKED_CAMERA) == 1 || ST(ST_LOCKED_CAMERA) == 2)
            CB->bloque(BLOQUE_ALL);
        CB->divers(DV_RUN, P_GUARDIAN17, 1);
        CB->combat(P_GUARDIAN17, -1);
        CB->animation("SP57GAR27");
        CB->animation("P57ARR02_500");
        CB->joindre(P_GUARDIAN17, 1565, -1836, 0x437A, -1);
        CB->animation("SP57ARR02_500");
        CB->modify_var(&ST(0xE0), 2);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xAE59 — guard 57 back on guard */
    if ((i32)ST(ST_HERO) > 1
        && (ST(V_ENGAGED_PERSONA) == 0x39 || (ST(V_ENGAGED_PERSONA) == 0 && ST(V_GUARD17_AWAY) == 1))
        && ST(0xE0) == 2
        && (CB->distance(7, 20) < 0x96
            || (ST(ST_LOCKED_CAMERA) == 1 && ST(V_ENGAGED_PERSONA) == 0x39))) {
        ST(0xE0) = 0;
        CB->animation("SP57GAR26");
        CB->animation("P57GAR02");
        if (ST(V_GUARD17_AWAY) == 1) {
            ST(V_GUARD17_AWAY) = 0;
            ST(V_ENGAGED_PERSONA) = 0x39;
        }
        CB->vire_col(0x21, 0);
        CB->combat(P_GUARDIAN17, 0);
    }
    /* 0xAF55 */
    if (ST(0xE0) == 2 && !cb968(0x1A)) {
        ST(0xE0) = 3;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x1A);
        l02_action_register("L02D217", 0x1A, "SP26GAR03", "SP26GAR03_10");
        CB->modify_var(&ST(0xE0), 4);
    }
    /* 0xAFD6 */
    if (ST(0xE0) == 4 && !cb968(ST(ST_HERO))) {
        ST(0xE0) = 5;
        CB->select_pile(1, 2);
        CB->evenement(0x27, 0x352);
        CB->animation("SG26FRA01");
        CB->animation("P26GAR03");
        CB->select_pile(0, 1);
    }
    /* 0xB053 */
    if (ST(V_EVENT_39) == 1) {
        ST(V_EVENT_39) = 0;
        CB->select_pile(2, 2);
        CB->divers_empile(DV_BONE_SOUND, ST(ST_HERO), 0);
        CB->animation("SG01ATT07");
        CB->animation("P26GAR03");
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
        CB->select_pile(0, 1);
    }
    /* 0xB0EE */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        CB->animation("SP00ACT01");
        CB->evenement(1, 0x320);
    }
    /* 0xB12A */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        ST(0x964) = 0;
        ST(ST_LEVEL) = 2;
        CB->divers_empile(DV_MUSIC, 1, 4);
        CB->change_piece(1, 0);
    }
    /* 0xB185 — actor 11 (P11) dialogue */
    if (ST(0x88) == 1 && !cb968(0x0B)) {
        ST(0x358) = 2;
        ST(0x88) = 2;
        CB->stop_joindre(-P_DHORKAN11);
        if (ST(ST_SPECIAL_MODE) == 1) l02_action_register("L02D105", 1, "SP01ATT74", NULL);
        if (ST(ST_SPECIAL_MODE) == 0) l02_action_register("L02D106", 1, "SP01ATT75", NULL);
        CB->animation("sar_10");
        CB->joindre(P_DHORKAN11, 0, 0, 0, ST(ST_HERO));
        l02_action_register("L06D05C", 0x0B, "SP11DIA65", "SP11ARR02_10");
        l02_action_register("L06D05D", 0x0B, "SP11DIA57", "SP11ARR02_10");
        l02_action_register("L06D05E", 0x0B, "SP11DIA58", "SP11ARR02_10");
        l02_action_register("L06D05F", 0x0B, "SP11DIA64", "SP11ARR02_500");
        CB->animation("P11_65535");
        CB->joindre(P_DHORKAN11, 0, 0, 0xCB20, -1);
        CB->animation("P11ARR02");
        CB->joindre(-P_DHORKAN11, 186, -1662, 0xCB20, -1);
        CB->joindre(-ST(ST_HERO), 0, 0, ST(0x4C), -1);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xB323 */
    if (ST(0x88) == 2 && CB->distance(14, 6) <= 0x212) {
        ST(0x88) = 3;
        CB->pos_perso(P_DHORKAN11, 186, -1662, 0xCB20, 4, "", 0x0E, 1);
        CB->modify_var(&ST(ST_VIE(P_DHORKAN11)), 1);
        CB->retire_zone(14);
    }
    /* 0xB3A6 — actor 84 follows hero 1 */
    if (ST(ST_HERO) == P_ARKHAN1 && ST(V_GUARD_FIGHT_DONE) == 0 && ST(0xAF8) == 0
        && (i32)ST(0xAF4) < 3 && CB->divers(DV_PERSONA_PRESENT, P_ZED84, 0)) {
        if (ST(V_ZED84_TALK_A) == 0) {
            /* 0xB41A */
            if ((ST(0x74) == 1 || ST(0x74) == 2)
                && CB->distance(3, 20) < 0x96
                && CB->divers(DV_ANGLE_TO, P_ZED84, ST(ST_HERO)) < 0xFA0) {
                ST(0x74) = 2;
                CB->stop_joindre(-P_ZED84);
                return;
            }
            /* 0xB496 */
            if (ST(0x74) == 2) {
                i32 near_ = CB->distance(3, 20) < 0xC8
                         && CB->divers(DV_ANGLE_TO, P_ZED84, ST(ST_HERO)) < 0x1770;
                if ((!near_ && ST(ST_COMBAT_STANCE) == 0) || ST(ST_COMBAT_STANCE) == 1) {
                    /* 0xB505 */
                    ST(0x74) = 1;
                    CB->stop_joindre(-P_ZED84);
                    CB->joindre(-P_ZED84, 1400, -695, 0xEA60, -1);
                    return;
                }
            }
        }
        /* 0xB53D */
        if (ST(0x74) != 0x2C && !cb968(0x54)
            && CB->distance(3, 21) <= 0x64) {
            ST(0x7C) = 2;
            ST(0x74) = 0x2C;
            ST(0xBDC) = 2;
            CB->pos_perso(P_ZED84, 1400, -695, -1, 4, "", 3, 0);
            CB->retire_zone(3);
        }
    }
    /* 0xB5CC */
    if (ST(0xB40) == 3 && ST(0x74) == 1) {
        i32 a0, a1, a2;
        ST(0x74) = 2;
        CB->select_pile(1, 1);
        CB->ajoute_zone(39, -1090, -1380, 70, 70, 0);
        CB->ajoute_zone(40, -506, -1740, 70, 70, 0);
        CB->divers_empile(DV_SCREEN, 5, 0);
        CB->pos_perso(P_KALHI13, -1, -1, -1, 4, "", 0x1A, 0);
        ST(ST_VIE(P_KALHI13)) = 4;
        CB->retire_zone(26);
        /* coordinates queried axis 2,1,0 (original order) */
        a2 = CB->divers(DV_DESC_POS, P_KALHI13, 2);
        a1 = CB->divers(DV_DESC_POS, P_KALHI13, 1);
        a0 = CB->divers(DV_DESC_POS, P_KALHI13, 0);
        CB->pos_perso(P_KALHI33, a0, a1, a2, 1, "P33ARR01", 0x1B, 1);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->animation("SP33ATT01");
        l02_action_register("L04D04", 0x21, "P33DIA54", "SP33ARR01");
        l02_action_register("L04D05", 0x21, "SP33DIA59", "SP33ARR01_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->animation("SP33ARR01_10");
        CB->modify_var(&ST(0x50), 3);
        CB->modify_var(&ST(0xFC), 1);
        CB->joindre(P_KALHI33, -1090, -1380, 0xC350, -1);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xB7A1 — actor 13 follows */
    if (ST(ST_VIE(P_KALHI13)) == 1 && ST(0xE9C) == 3) {
        if (ST(0xB8) == 1 && CB->distance(26, 20) < 0x78 && ST(0x88) == 0) {
            CB->select_pile(1, 1);
            if (CB->divers(DV_RUN, P_KALHI13, 1) == 1)
                CB->divers(DV_RUN, P_KALHI13, 0);
            ST(0x88) = 1;
            ST(0x90) = 1;
            CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
            CB->select_pile(0, 1);
        }
        /* 0xB887 */
        if (ST(0xB8) == 1 && ST(0x38) % 0x14 == 0) {
            if (CB->distance(26, 20) >= 0xB4
                && CB->distance(26, 20) < 0x12C) {
                CB->select_pile(1, 1);
                ST(0x88) = 0;
                CB->divers(DV_RUN, P_KALHI13, 0);
                CB->joindre(-P_KALHI13, -1, -1, -1, ST(ST_HERO));
                CB->select_pile(0, 1);
            }
            /* 0xB945 */
            if (CB->distance(26, 20) >= 0x12C) {
                CB->select_pile(1, 1);
                ST(0x88) = 0;
                CB->divers(DV_RUN, P_KALHI13, 1);
                CB->joindre(-P_KALHI13, -1, -1, -1, ST(ST_HERO));
                CB->select_pile(0, 1);
            }
        }
    }
    /* 0xB9BE — actor 33 follows */
    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->select_pile(1, 1);
        if (ST(0xFC) == 1) {
            if (CB->distance(27, 20) < 0x96
                && CB->divers(DV_ANGLE_TO, P_KALHI33, ST(ST_HERO)) < 0xFA0) {
                ST(0xFC) = 2;
                CB->stop_joindre(-P_KALHI33);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xBA81 */
            CB->select_pile(0, 1);
        }
        /* 0xBA8E */
        if (ST(0xFC) == 2) {
            if (CB->distance(27, 20) < 0xC8
                && CB->divers(DV_ANGLE_TO, P_KALHI33, ST(ST_HERO)) < 0x1770) {
                /* 0xBDE8 */
                CB->select_pile(0, 0);
            } else {
                /* 0xBAE2 */
                CB->select_pile(1, 2);
                ST(0xFC) = 1;
                CB->animation("SP33ARR01_10");
                if (e_a33_walk_back())
                    return;
                /* falls through to 0xBDE8 */
                CB->select_pile(0, 0);
            }
        }
    }
    /* 0xBDF2 — actor 33 notices the hero */
    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1 && !cb968(0x21)
        && CB->distance(27, 39) <= 0x64
        && ST(ST_LOCKED_CAMERA) != 6 && ST(ST_LOCKED_CAMERA) != 5
        && ST(0x7C) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(0x7C) = 1;
        ST(V_SAVED_HERO) = ST(ST_HERO);
        CB->select_pile(0, 1);
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(5, CAM_LOCKED);
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        l02_action_register("L02D65A", 0x21, "SP33DIA59", "SP33ARR01");
        l02_action_register("L02D65C", 0x21, "SP33DIA56", "SP33ARR01_10");
        CB->animation("SP33ARR01_10");
        CB->change_camera(5, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xBF41 — actor 13 joins */
    if (ST(ST_VIE(P_KALHI13)) == 1 && ST(0xE9C) == 4 && ST(V_EVENT15_STEP) == 1
        && CB->distance(26, 20) < 0x78) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(V_EVENT15_STEP) = 2;
        ST(0xE9C) = 3;
        ST(0x70) = 1;
        CB->animation("SP13LEV01");
        CB->animation("P13_65535");
        CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP13ARR01");
        l02_action_register("L08D335A", 0x0D, "SP13DIA60", "SP13ARR01_10");
        l02_action_register("L08D335B", 0x0D, "SP13DIA60", "SP13ARR01_10");
        l02_action_register("L08D335C", 0x0D, "SP13DIA60", "SP13ARR01_10");
        ST(V_KALHI13_TALK_C) = 2;
        CB->modify_var(&ST(0xB8), 1);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->divers(DV_LOOK_AT_PLAYER, P_KALHI13, 1);
    }
    /* 0xC0AB — actor 33 joins */
    if (ST(ST_VIE(P_KALHI33)) == 1 && ST(0xE9C) == 4 && ST(V_EVENT15_STEP) == 1
        && CB->distance(20, 27) < 0x78) {
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0xFC) = 1;
        ST(V_EVENT15_STEP) = 2;
        ST(0xE9C) = 3;
        ST(0x70) = 1;
        ST(V_KALHI13_TALK_C) = 2;
        CB->animation("SP33LEV01");
        CB->animation("P33_65535");
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP33ARR01");
        l02_action_register("L08D335A", 0x21, "SP33DIA60", "SP33ARR01_10");
        l02_action_register("L08D335B", 0x21, "SP33DIA60", "SP33ARR01_10");
        l02_action_register("L08D335C", 0x21, "SP33DIA60", "SP33ARR01_10");
        CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        if (e_a33_walk_back())   /* 0xC227 */
            return;
    }
    /* 0xC503 — actor 13 chatter */
    if (ST(0x90) == 1 && CB->divers(DV_IDLE_TIME, 0, 0) >= 0x2710 && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0x90) = 0;
        if (ST(0x948) == 2)
            l02_action_register("L04D07", 0x0D, "SP13DIA60", "SP13ARR01_10");
        if (ST(0x948) == 1) {
            ST(0x948) = 2;
            l02_action_register("L04D08", 0x0D, "SP13DIA53", "SP13ARR01_10");
        }
        if (ST(0x948) == 0) {
            ST(0x948) = 1;
            l02_action_register("L04D09", 0x0D, "SP13DIA54", "SP13ARR01_10");
        }
        CB->divers(DV_IDLE_TIME, 1, 0);
        CB->modify_var(&ST(0x90), 1);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xC625 — actor 10 follows */
    if (ST(V_DELIA_PHASE) == 8 && ST(ST_VIE(P_DELIA)) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        if (CB->distance(20, 16) < 0x8C && ST(0xBC) == 0) {
            if (CB->divers(DV_RUN, P_DELIA, 1) == 1)
                CB->divers(DV_RUN, P_DELIA, 0);
            ST(0xBC) = 1;
            CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
        }
        /* 0xC6DE */
        if (ST(0xF4) == 1 && ST(0x38) % 0x14 == 0) {
            if (CB->distance(20, 16) >= 0xB4) {
                CB->divers(DV_RUN, P_DELIA, 0);
                ST(0xBC) = 0;
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
            /* 0xC764 */
            if (CB->distance(20, 16) >= 0x12C) {
                ST(0xBC) = 0;
                CB->divers(DV_RUN, P_DELIA, 1);
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
        }
    }
    /* 0xC7C0 — hero crosses the bridge line */
    if ((ST(V_DELIA_PHASE) == 8 || ST(0xE9C) == 3) && ST(ST_HERO) != 4
        && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) < -1290) {
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 0x3B6 && ST(0xF8) == 0)
            ST(0xF8) = 1;
        /* 0xC85D */
        if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 0x320 && ST(0xF8) == 2)
            ST(0xF8) = 3;
    }
    /* 0xC8A1 */
    if (ST(0xF8) == 1) {
        ST(0xF8) = 2;
        if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI13)) == 1) {
            CB->stop_joindre(-P_KALHI13);
            ST(0xB8) = 0x14;
            ST(0x90) = 0x0A;
            if (ST(V_FROM_LEVEL) != 7 && ST(V_FROM_LEVEL) != 6)
                l02_action_register("L02D500", 0x0D, "SP13ARR01", "SP13ARR01_50");
        }
        /* 0xC94C */
        if (ST(V_DELIA_PHASE) == 8 && ST(ST_VIE(P_DELIA)) == 1) {
            CB->stop_joindre(-P_DELIA);
            ST(0xF4) = 0x14;
            if (ST(V_FROM_LEVEL) != 7 && ST(V_FROM_LEVEL) != 6) {
                if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D501", P_ARKHAN2);
                if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D502", P_ARKHAN3);
            }
        }
    }
    /* 0xC9F2 */
    if (ST(0xF8) == 3) {
        ST(0xF8) = 0;
        if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI13)) == 1) {
            ST(0xB8) = 1;
            ST(0x90) = 1;
        }
        if (ST(V_DELIA_PHASE) == 8 && ST(ST_VIE(P_DELIA)) == 1)
            ST(0xF4) = 1;
    }
    /* 0xCA88 — guard 57 */
    if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(0xEB8) == 0) {
        if (ST(ST_VIE(P_GUARDIAN17)) == 1 && ST(V_ENGAGED_PERSONA) != 0x39 && ST(V_GUARD17_AWAY) == 0) {
            if (!cb968(0x39) && ST(0x114) == 1) {
                ST(0x114) = 0;
                CB->stop_joindre(-P_GUARDIAN17);
                return;
            }
            /* 0xCB23 */
            if ((i32)ST(ST_HERO) >= 2 && CB->distance(20, 22) < 0x12C
                && ST(0x64) == 0 && ST(0xDC) == 0) {
                ST(0xDC) = 1;
                CB->animation("SP57GAR26");
                CB->animation("P57GAR02_10");
                CB->joindre_perm(-0x39, ST(ST_HERO));
                return;
            }
            /* 0xCBB1 */
            if (CB->distance(20, 22) >= 0x1F4 && ST(0x64) == 0 && ST(0xDC) == 1) {
                ST(0x100) = 0;
                ST(0xDC) = 0;
                CB->stop_joindre(-P_GUARDIAN17);
                CB->animation("SP57GAR27");
                CB->animation("P57ARR02_10");
                return;
            }
            /* 0xCC31 */
            if (CB->distance(20, 22) >= 0x1F4 && ST(0x64) == 1) {
                ST(0x100) = 0;
                ST(0x64) = 0;
                ST(0xD4) = 1;
                CB->stop_joindre(-P_GUARDIAN17);
                CB->joindre(-P_GUARDIAN17, 1565, -1836, 0x437A, -1);
                return;
            }
        }
        /* 0xCCB1 */
        if (!cb968(0x39) && ST(0xD4) == 1) {
            ST(0xD4) = 2;
            CB->vire_col(0x21, 1);
        }
    }
    /* 0xCCF5 — guard 59 back to post */
    if (ST(ST_VIE(P_GUARDIAN19)) == 1 && CB->distance(20, 2) >= 0x190
        && ST(0x64) == 1 && ST(V_ENGAGED_PERSONA) != 0x3B) {
        ST(0x64) = 0;
        ST(0x68) = 0;
        ST(0xD4) = 1;
        CB->joindre(P_GUARDIAN19, 490, -700, 0x7D00, -1);
    }
    /* 0xCD83 — guard 59 */
    if (ST(0x68) == 0 && ST(ST_VIE(P_GUARDIAN19)) == 1 && ST(V_ENGAGED_PERSONA) != 0x3B) {
        if (ST(V_G19_TALK) == 0 && CB->distance(2, 20) < 0x190) {
            ST(0x68) = 1;
            CB->bloque(BLOQUE_ALL);
            CB->joindre(P_GUARDIAN19, 0, 0, 0, ST(ST_HERO));
            l02_action_register("L02D67", 0x3B, "SP59GAR26", "SP59GAR03_10");
            CB->vire_col(0x21, 1);
            CB->retire_zone(9);
            CB->bloque(BLOQUE_NONE);
        }
        /* 0xCE50 */
        if (ST(V_G19_TALK) == 1 && CB->distance(2, 20) < 0x12C) {
            CB->vire_col(0x20, 0);
            CB->bloque(BLOQUE_ALL);
            ST(0x68) = 1;
            ST(0x64) = 1;
            CB->dialogue("L02D68", P_GUARDIAN19);
            CB->joindre(P_GUARDIAN19, 623, -700, 0x59D8, -1);
            CB->bloque(BLOQUE_NONE);
        }
    }
    /* 0xCEE1 — guard 58 */
    if (ST(0x6C) == 0 && ST(ST_VIE(P_GUARDIAN18)) == 1 && ST(V_ENGAGED_PERSONA) != 0x3A
        && CB->distance(11, 20) < 0x320) {
        ST(0x6C) = 1;
        l02_action_register("L02D110", 0x3A, "SP58ARR04", "SP58GAR05_10");
    }
    /* 0xCF51 */
    if (ST(0x6C) == 1 && ST(ST_VIE(P_GUARDIAN18)) == 1 && ST(V_ENGAGED_PERSONA) != 0x3A
        && CB->distance(11, 20) < 0x190) {
        ST(0x6C) = 2;
        CB->vire_col(0x21, 0);
        ST(V_ENGAGED_PERSONA) = 0x3A;
        CB->combat(P_GUARDIAN18, 0);
    }
    /* 0xCFCE */
    if ((ST(ST_VIE(P_KALHI13)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == 0x0D)
        || (ST(ST_VIE(P_KALHI33)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == 0x21)) {
        ST(0xE58) = 1;
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = 0;
    }
    /* 0xD037 — re-arm zones once the fighting guard is dead */
    if ((i32)ST(ST_HERO) > 1 && ST(V_ENGAGED_PERSONA) == 0x39 && ST(ST_VIE(P_GUARDIAN17)) == VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 0;
        CB->ajoute_zone(1, 1600, -1830, 80, 90, 0);
    }
    /* 0xD091 */
    if (ST(V_ENGAGED_PERSONA) == 0x3A && ST(ST_VIE(P_GUARDIAN18)) == VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 0;
        CB->ajoute_zone(1, 1600, -1830, 80, 90, 0);
    }
    /* 0xD0D9 */
    if (ST(V_ENGAGED_PERSONA) == 0x3B && ST(ST_VIE(P_GUARDIAN19)) == VIE_DEAD) {
        ST(V_ENGAGED_PERSONA) = 0;
        CB->ajoute_zone(9, 486, -610, 120, 90, 0);
    }
    /* 0xD121 — hero 1 at the gate */
    if (ST(0x358) == 0 && ST(ST_HERO) == P_ARKHAN1
        && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) < 0x0A && ST(ST_LOCKED_CAMERA) == 5) {
        if (ST(0x7C) == 1) {
            ST(0x7C) = 2;
            CB->pos_perso(P_ZED84, 1400, -695, -1, 4, "", 3, 0);
            ST(0x74) = 0x2C;
            ST(0xBDC) = 2;
            CB->pos_perso(P_DHORKAN11, 2000, -1662, 0x3A98, 1, "P11ARR02", 0x0E, 0);
            return;
        }
        /* 0xD1FA */
        if (ST(0x54) == 0) {
            ST(0x54) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(0x4C) = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
            CB->pos_perso(P_DHORKAN11, 186, -1662, 0x3A98, 1, "P11ARR02", 0x0E, 0);
            CB->dialogue("L02D50", P_DHORKAN11);
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x0B);
            CB->retire_zone(24);
            CB->retire_zone(25);
            CB->joindre(P_DHORKAN11, -202, -1693, 0x3A98, -1);
            CB->modify_var(&ST(0x88), 1);
        }
    }
    /* 0xD2DC — debug overlay (sprintf into the global dword_1002C6A0) */
    snprintf(g_txt, sizeof(g_txt), "CRI:%d", ST(V_CRI_KEY));
    CB->print_msg(g_txt, 0, 0xF0);
    snprintf(g_txt, sizeof(g_txt), "cri_on:%d", ST(V_CRI_USED));
    CB->print_msg(g_txt, 0, 0xF8);
    /* 0xD33C */
}

/* Float FSM accumulators — 10-element array at dword_10025A30 (LIEU01:
 * dword_10024A30). Cases 90-93 read/write indices 0-3; sub_10001200
 * (thunk sub_1000105F) resets all 10 to 1.0f. */
static float g_float_accum[10] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                   1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static i32 g_float_active = 1;   /* dword_10025A58 gate flag */

static void e4_reset_floats(void) {
    for (i32 i = 0; i < 10; i++) g_float_accum[i] = 1.0f;
}
static void e5_clamp6(i32 k);  /* forward decl — defined in event5 section */

/*
 * event4 — sub_1000123A (4 KB), identical to LIEU01. switch(arg) over ~99 script-event ids;
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
 * event5 — sub_100022B4 (14 KB), identical to LIEU01. switch(arg) over LCOD player-actions
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
                l02_action_register("LCOD02A", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02B", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02C", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02D", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02E", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02F", 2, "SP02ACT64", "SP02ACT64");
                if (ST(V_LCOD02_SEEN) == 0) {
                    l02_action_register("LCOD02G", 2, "SP02ACT64", "SP02ACT64");
                    l02_action_register("LCOD02H", 2, "SP02ACT64", "SP02ACT64");
                }
                ST(V_LCOD02_SEEN) = 1;
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                l02_action_register("LCOD03A", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03B", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03C", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03D", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03E", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03F", 3, "SP03ACT64", "SP03ACT64");
                if (ST(V_LCOD02_SEEN) == 0) {
                    l02_action_register("LCOD03G", 3, "SP03ACT64", "SP03ACT64");
                    l02_action_register("LCOD03H", 3, "SP03ACT64", "SP03ACT64");
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
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD88", 2, "SG00DIA02", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD89", 3, "SG00DIA02", NULL);
        CB->bloque(BLOQUE_NONE);
        break;

    case 46:  /* 0x2A47 — LCOD100/101/102/103/104 */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) {
            l02_action_register("LCOD100A", 2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD100B", P_ARKHAN2);
            CB->dialogue("LCOD100C", P_ARKHAN2);
            CB->dialogue("LCOD100D", P_ARKHAN2);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            l02_action_register("LCOD101A", 3, "SP03ACT64", "SP03ACT64");
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
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD103", 2, "SG02DIA01", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD104", 3, "SG03DIA01", NULL);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case 70:  /* 0x2CEA */
        e5_reset();
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD90", 2, "SG00DIA02", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD91", 3, "SG00DIA02", NULL);
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
        if (ST(ST_HERO) == P_ARKHAN1) l02_action_register("LCOD41", ST(ST_HERO), "SG01ATT01", NULL);
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD42", ST(ST_HERO), "SG01ATT01", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD43", ST(ST_HERO), "SG01ATT01", NULL);
        CB->bloque(BLOQUE_NONE);
        break;

    case 81:  /* 0x36B3 — LCOD23/24 A-G */
        e5_reset();
        if (ST(V_LCOD23_SEEN) == 0) ST(V_LCOD23_SEEN) = 1;
        if (ST(ST_LEVEL) == 28) ST(0x84) = 1;
        if (ST(ST_HERO) == P_ARKHAN2) {
            l02_action_register("LCOD23A", 2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD23B", P_ARKHAN2);
            CB->dialogue("LCOD23C", P_ARKHAN2);
            CB->dialogue("LCOD23D", P_ARKHAN2);
            CB->dialogue("LCOD23E", P_ARKHAN2);
            CB->dialogue("LCOD23F", P_ARKHAN2);
            l02_action_register("LCOD23G", 2, "SG02DIA01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            l02_action_register("LCOD24A", 3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD24B", P_ARKHAN3);
            CB->dialogue("LCOD24C", P_ARKHAN3);
            CB->dialogue("LCOD24D", P_ARKHAN3);
            CB->dialogue("LCOD24E", P_ARKHAN3);
            CB->dialogue("LCOD24F", P_ARKHAN3);
            l02_action_register("LCOD24G", 3, "SG03DIA01", NULL);
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
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD55", 2, "SP02ACT64", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD56", 3, "SP02ACT64", NULL);
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
            if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD666", 2, "SG02DIA02", NULL);
            if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD667", 3, "SG03DIA02", NULL);
            CB->bloque(BLOQUE_NONE);
            break;
        } else {
            if (ST(ST_LEVEL) == 35) {
                CB->bloque(BLOQUE_ALL);
                if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD670", 2, "SG02DIA02", NULL);
                if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD671", 3, "SG03DIA02", NULL);
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
            if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD668", 2, "SG02DIA01", NULL);
            if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD669", 3, "SG03DIA01", NULL);
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
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x10019AFA).
 * Callback ingestion (0x10019AFA..0x10019CC1), the trigger-table fill
 * (0x10019CC6..0x10019E60) and the unused dword_1002CA0C = state+0xC8 are
 * skipped: g_lieu_cb is provided by the bridge, the trigger table is
 * static in lieu02_trig.c.
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
        l02_trig00();   /* 0x1001A0A9: call sub_10001073 -> sub_100073F0 */
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
 * TICK (opcode -1, 0x1001A0FC): drain pending sound, run body.
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
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x10019AE0).
 * Jump table @ 0x1001A179, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu02_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_l02_S = state;
    if (g_lieu_trace)
        print("lieu02: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:   init(arg);                    break;
    case LIEU_OP_EVENT:  l02_trigger(arg);             break;  /* dword_1002C8A0[arg]() */
    case LIEU_OP_CALL4:  event4(arg);                  break;
    case LIEU_OP_CALL5:  event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
