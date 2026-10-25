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
    CB->named_object(arg0);
    CB->named_object_int(name, count);
    if (count < 4 && arg4 == NULL) {
        CB->named_object("Sar_20");
    } else if (arg4 != NULL) {
        CB->named_object(arg4);
    } else {
        c8 b[256];
        snprintf(b, sizeof(b), "SP%02darr01_10", count);
        CB->named_object(b);
    }
}

/* -------------------------------------------------------------------------
 * event6 — sub_10005BEC (identical to LIEU01). Switch on arg (0..4):
 * LCOD zone triggers.
 * ---------------------------------------------------------------------- */
static void event6(i32 arg) {
    switch (arg) {
    case 0:  /* loc_10005C0A */
        if (ST(0x194) == 2 && ST(0x10) == 0 && ST(0x938) == 0) {
            CB->state_toggle(0, 1);
            ST(0x938) = 1;
            CB->group(3);
            l02_action_register("LCOD34", 2, "SG00CON01", NULL);
            CB->group(0);
        }
        break;
    case 1:  /* loc_10005C87 */
        if (ST(0x194) == 2 && ST(0x7C8) == 0) {
            CB->state_toggle(0, 1);
            ST(0x7C8) = 1;
            ST(0x76C) = 0;
            CB->group(3);
            l02_action_register("LCOD35", 2, "SG00CON01", NULL);
            CB->named_object("Sar_50");
            CB->op_9c(0x2A, 0, 0);
            CB->named_object("SG00CON01_1500");
            CB->named_object("ar_50");
            CB->group(0);
        }
        break;
    case 2:  /* loc_10005D34 */
        if (ST(0x194) == 3 && ST(0x10) == 0 && ST(0x768) == 0) {
            CB->state_toggle(0, 1);
            ST(0x768) = 1;
            CB->group(3);
            l02_action_register("LCOD36", 3, "SG00CON01", "SG00CON01");
            CB->named_object("Sar_50");
            CB->group(0);
        }
        break;
    case 3:  /* loc_10005DBF */
        if (ST(0x194) == 3 && ST(0x76C) == 0) {
            CB->state_toggle(0, 1);
            ST(0x7C8) = 0;
            ST(0x768) = 0;
            ST(0x76C) = 1;
            CB->group(3);
            l02_action_register("LCOD37", 3, "SG00CON01", NULL);
            CB->named_object("Sar_50");
            CB->op_9c(0x2A, 0, 0);
            CB->named_object("Sar_50");
            CB->group(0);
        }
        break;
    case 4:  /* loc_10005E70 */
        if (ST(0x194) == 3 && ST(0x318) == 0) {
            CB->state_toggle(2, 2);
            CB->state_toggle(1, 2);
            CB->state_toggle(0, 2);
            ST(0x318) = 1;
            CB->layer_op(0x23, 0, 0);
            CB->named_two_ints(0, -1);
            CB->named_two_ints(1, -1);
            CB->named_two_ints(2, -1);
            CB->named_two_ints(0x21, -1);
            CB->named_two_ints(0x22, -1);
            CB->named_two_ints(0x23, -1);
            CB->named_two_ints(0x24, -1);
            CB->named_two_ints(0x25, -1);
            CB->named_two_ints(0x26, -1);
            CB->named_two_ints(0x27, -1);
            CB->named_two_ints(0x29, -1);
            CB->group(3);
            CB->named_object("Sar_50");
            if (ST(0x10) == 1)
                CB->op_9c(0x17, 0, 0);
            CB->op_9c(0x10, 5, 0);
            CB->op_9c(0x06, 3, 0);
            CB->named_object("Sar_50");
            CB->cinematique("SC21_500");
            CB->op_9c(0x10, 3, 0);
            CB->op_9c(0x1B, 0, 0);
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
static i32 cb968(i32 id) { return CB->cb_968(id); }

/* Hit drain shared by the guard blocks of tick_body (inlined 3x in the
 * original, e.g. 0xA5C1): life = layer_op(6,a,-1) stored in [0x84];
 * life -= damage [0x130], floored at 1. */
static void e_hit_drain(i32 a) {
    ST(0x84) = CB->layer_op(6, a, -1);
    if (ST(0x84) - ST(0x130) > 0)
        CB->layer_op(6, a, ST(0x84) - ST(0x130));
    else
        CB->layer_op(6, a, 1);
}

/* Actor 33 walks back to its post (inlined 2x in the original:
 * 0xBB0C..0xBDE2 and 0xC227..0xC4FD). Returns 1 where the original
 * jumps to the tick_body epilogue. Ends with state_toggle(0,1) on every
 * path. */
static i32 e_a33_walk_back(void) {
    if (ST(0x50) != 3 && ST(0xF3C) == 1) {
        if (ST(0x50) == 0 || ST(0x50) == 1) {
            CB->actor_934(-0x21, (i32)0xFFFFFE02, (i32)0xFFFFF7B8, -1, -1);
            CB->named_object("SP33ARR01_10");
            CB->cb_46C(&ST(0x50), 2);
            CB->actor_934(-0x21, (i32)0xFFFFFE02, (i32)0xFFFFF862, -1, -1);
            CB->named_object("SP33ARR01_10");
            CB->cb_46C(&ST(0x50), 3);
            CB->actor_934(-0x21, (i32)0xFFFFFBBE, (i32)0xFFFFFA9C, 0xC350, -1);
            CB->state_toggle(0, 1);
            return 1;
        }
        if (ST(0x50) == 2) {
            CB->actor_934(-0x21, (i32)0xFFFFFE02, (i32)0xFFFFF862, -1, -1);
            CB->named_object("SP33ARR01_10");
            CB->cb_46C(&ST(0x50), 3);
            CB->actor_934(-0x21, (i32)0xFFFFFBBE, (i32)0xFFFFFA9C, 0xC350, -1);
            CB->state_toggle(0, 1);
            return 1;
        }
    }
    if (ST(0x50) != 3 && ST(0xF3C) == 2) {
        if (ST(0x50) == 0 || ST(0x50) == 1) {
            CB->actor_934(-0x21, (i32)0xFFFFFE02, (i32)0xFFFFFDC5, -1, -1);
            CB->named_object("SP33ARR01_10");
            CB->cb_46C(&ST(0x50), 2);
            CB->actor_934(-0x21, (i32)0xFFFFFE02, (i32)0xFFFFFD1C, -1, -1);
            CB->named_object("SP33ARR01_10");
            CB->cb_46C(&ST(0x50), 3);
            CB->actor_934(-0x21, (i32)0xFFFFFBBE, (i32)0xFFFFFA9C, 0xC350, -1);
            CB->state_toggle(0, 1);
            return 1;
        }
        if (ST(0x50) == 2) {
            CB->actor_934(-0x21, (i32)0xFFFFFE02, (i32)0xFFFFFD1C, -1, -1);
            CB->named_object("SP33ARR01_10");
            CB->cb_46C(&ST(0x50), 3);
            CB->actor_934(-0x21, (i32)0xFFFFFBBE, (i32)0xFFFFFA9C, 0xC350, -1);
            CB->state_toggle(0, 1);
            return 1;
        }
    }
    if (ST(0x50) == 3)
        CB->actor_934(-0x21, (i32)0xFFFFFBBE, (i32)0xFFFFFA9C, 0xC350, -1);
    CB->state_toggle(0, 1);
    return 0;
}

static void tick_body(void) {
    /* 0x9DCE */
    if (ST(0x194) == 4 && ST(0x1A0) == 0) {
        ST(0x1A0) = 1;
        CB->named_two_ints(0x2C, 0xBB8);
    }
    /* 0x9E11 — intro cinematic replay */
    if (ST(0x3D0) == 1) {
        ST(0x3D0) = 0;
        CB->group(3);
        if (CB->layer_op(0x1D, 0, 0)) CB->layer_op(6, 0, 0);
        if (CB->layer_op(0x1D, 1, 0)) CB->layer_op(6, 1, 0);
        if (CB->layer_op(0x1D, 2, 0)) CB->layer_op(6, 2, 0);
        if (CB->layer_op(0x1D, 3, 0)) CB->layer_op(6, 3, 0);
        if (ST(0xB04) == 0) CB->cinematique("SC22_500");
        if (ST(0xB04) != 0) CB->cinematique("SC23_500");
        CB->op_9c(0x1B, 0, 0);
    }
    /* 0x9F30 — door 2 / SP26 arrival */
    if (ST(0x3C4) == 1) {
        ST(0x3C4) = 0;
        CB->select_468(-ST(0x194));
        CB->actor_934(-ST(0x194), 0, 0, 0, 0x1A);
        CB->cb_964(0x21, 0);
        if (ST(0x194) == 2) l02_action_register("L02D209", 2, "SG00DIA03", "Sar_10");
        if (ST(0x194) == 3) l02_action_register("L02D210", 3, "SG00DIA03", "Sar_10");
        if (CB->query_990(0x14, 0x1F) < 0x50) {
            if (ST(0x194) == 2) {
                CB->named_object("SP02MAR30");
                CB->named_object("P02_65535");
            }
            if (ST(0x194) == 3) {
                CB->named_object("SP03MAR30");
                CB->named_object("P03_65535");
            }
        }
        if (ST(0x194) == 2) {
            CB->named_object("SG02PRE80");
            CB->named_object("G02PRE81");
            CB->named_object("SP26PRE60");
            CB->named_object("P26_65535");
            CB->named_object("SG02PRE82");
            if (ST(0x19C) != 0) CB->named_object("G02ARR02_10");
            else                CB->named_object("G02ARR01_10");
            CB->named_object("SP26ARR01_10");
        }
        if (ST(0x194) == 3) {
            CB->named_object("SG03PRE80");
            CB->named_object("G03PRE81");
            CB->named_object("SP26PRE60");
            CB->named_object("P26_65535");
            CB->named_object("SG03PRE82");
            if (ST(0x19C) != 0) CB->named_object("G03ARR02_10");
            else                CB->named_object("G03ARR01_10");
            CB->named_object("SP26ARR01_10");
        }
        CB->named_object("SP26PRE91");
        CB->named_object("P26ARR01_10");
        CB->op_9c(0x0C, 0x1A, 1);
        l02_action_register("L02D211", 0x1A, "P26DIA60", "SP26ARR03_10");
        CB->op_9c(0x0C, 0x1A, 0);
        CB->actor_934(-0x1A, 0, 0, 0xC350, -1);
        CB->named_object("SP26OUV01");
        CB->place_95C("porte2", 0x67E, (i32)0xFFFFF8A2);
        CB->cb_98C("IM03", 3, 0x3C, 0x5DC, 0);
        CB->named_object("P00_65535");
        ST(0x18) = 2;
        CB->op_9c(0x18, 1, 4);
        CB->cb_984(6, 1);
        return;
    }
    /* 0xA231 */
    if (ST(0xEB8) == 1 && ST(0xB04) == 0
        && (ST(0xF90) == 2 || ST(0xF90) == 1)
        && ST(0x10) == 1
        && CB->layer_op(2, 9, 2)
        && CB->query_954(0x1A)
        && ST(0x14) == 0 && ST(0x194) != 4) {
        ST(0x14) = 1;
        ST(0x8D0) = 1;
        l02_trig31();   /* call 0x10001000 -> sub_100149FB = trigger[31] */
        return;
    }
    /* 0xA2FB — corpse 13 */
    if (CB->layer_op(0x12, 0x0D, 0) && ST(0x1C4) != 2) {
        ST(0x30) = 0;
        ST(0x1C4) = 2;
        ST(0xE58) = 1;
        CB->state_toggle(1, 2);
        CB->cb_970(0x0D, -1);
        CB->select_468(-0x0D);
        CB->named_object("SP13IMP52");
        CB->named_object("P13CAD");
        CB->spawn_678(0x0D, 0, 0, 0, 2, "P13CAD", 0x1A, 0);
        CB->state_toggle(0, 1);
    }
    /* 0xA3BB — corpse 33 */
    if (CB->layer_op(0x12, 0x21, 0) && ST(0x214) != 2) {
        ST(0x30) = 0;
        ST(0x214) = 2;
        ST(0xE58) = 1;
        CB->state_toggle(1, 2);
        CB->cb_970(0x21, -1);
        CB->select_468(-0x21);
        CB->named_object("SP33IMP52");
        CB->named_object("P33CAD");
        CB->spawn_678(0x21, 0, 0, 0, 2, "P33CAD", 0x1B, 0);
        CB->state_toggle(0, 1);
    }
    /* 0xA47B — corpse 10 */
    if (CB->layer_op(0x12, 0x0A, 0) && ST(0x1B8) != 2) {
        ST(0x30) = 0;
        ST(0x1B8) = 2;
        ST(0xF1C) = 2;
        ST(0x380) = 0x14;
        CB->state_toggle(1, 2);
        CB->cb_970(0x0A, -1);
        CB->select_468(-0x0A);
        CB->named_object("SP10IMP51");
        CB->named_object("P10CAD");
        CB->spawn_678(0x0A, 0, 0, 0, 2, "P10CAD", 0x10, 0);
        CB->state_toggle(0, 1);
    }
    /* 0xA54A — actor 57 hit */
    if (CB->layer_op(0x12, 0x39, 0) && ST(0x274) != 2) {
        CB->state_toggle(1, 2);
        ST(0x34) = 0x39;
        CB->op_9c(0x12, 0x39, 1);
        CB->cb_970(0x39, -1);
        CB->cb_964(0x21, 0);
        e_hit_drain(0x39);
        if (CB->layer_op(6, 0x39, -1) <= 1) {
            ST(0x274) = 2;
            CB->select_468(-0x39);
            CB->named_object("SP57IMP52");
            CB->named_object("P57CAD");
            CB->spawn_678(0x39, 0, 0, 0, 2, "P57CAD", 7, 0);
            CB->op_9c(0x10, 2, 0);
        } else {
            CB->select_468(-0x39);
            CB->named_object("SP57IMP12");
            CB->named_object("P57IMP47");
            CB->named_object("P57GAR02");
            CB->cb_970(0x39, 0);
        }
        CB->state_toggle(0, 1);
    }
    /* 0xA701 — actor 58 hit */
    if (CB->layer_op(0x12, 0x3A, 0) && ST(0x278) != 2) {
        ST(0x34) = 0x3A;
        CB->state_toggle(1, 2);
        CB->op_9c(0x12, 0x3A, 1);
        CB->cb_970(0x3A, -1);
        CB->cb_964(0x21, 0);
        e_hit_drain(0x3A);
        if (CB->layer_op(6, 0x3A, -1) <= 1) {
            ST(0x278) = 2;
            CB->select_468(-0x3A);
            CB->named_object("SP58IMP55");
            CB->named_object("P58CAD");
            CB->spawn_678(0x3A, 0, 0, 0, 2, "P58CAD", 0x0B, 0);
            CB->op_9c(0x10, 2, 0);
        } else {
            CB->select_468(-0x3A);
            CB->named_object("SP58IMP15");
            CB->named_object("P58IMP50");
            CB->named_object("P58GAR05");
            CB->cb_970(0x3A, 0);
        }
        CB->state_toggle(0, 1);
    }
    /* 0xA8B8 — actor 59 hit */
    if (CB->layer_op(0x12, 0x3B, 0) && ST(0x27C) != 2) {
        ST(0x34) = 0x3B;
        CB->state_toggle(1, 2);
        CB->op_9c(0x12, 0x3B, 1);
        CB->cb_970(0x3B, -1);
        CB->cb_964(0x20, 0);
        e_hit_drain(0x3B);
        if (CB->layer_op(6, 0x3B, -1) <= 1) {
            ST(0x27C) = 2;
            CB->select_468(-0x3B);
            CB->named_object("SP59IMP53");
            CB->named_object("P59CAD");
            CB->spawn_678(0x3B, 0, 0, 0, 2, "P59CAD", 2, 0);
            CB->op_9c(0x10, 2, 0);
        } else {
            CB->select_468(-0x3B);
            CB->named_object("SP59IMP13");
            CB->named_object("P59IMP48");
            CB->named_object("P59GAR03");
            CB->cb_970(0x3B, 0);
        }
        CB->state_toggle(0, 1);
    }
    /* 0xAA6F */
    if (ST(0x3A8) == 1 && ST(0x194) == 1) {
        ST(0x3A8) = 0;
        CB->named_object("SP01IMP51");
        CB->named_two_ints(0x26, 0x7D0);
        CB->named_object("P84ACT02");
        CB->named_object("P84ARR01_1000");
    }
    /* 0xAAD3 */
    if (ST(0x3B8) == 1) {
        ST(0x3B8) = 0;
        CB->state_toggle(1, 1);
        CB->op_9c(0x1C, ST(0x194), 0);
        CB->op_9c(6, ST(0x194), 0);
        CB->cb_46C(&ST(0x194), 4);
        CB->state_toggle(0, 1);
    }
    /* 0xAB58 */
    if ((i32)ST(0xAF4) < 3 && ST(0x328) == 1 && ST(0xEC8) == 1) {
        ST(0x328) = 0;
        CB->named_object("SG01ATT07");
        CB->named_object("P84ARR02_10");
        CB->op_9c(6, ST(0x194), 0);
        CB->cb_46C(&ST(0x194), 4);
    }
    /* 0xABE2 — guard 57 alerted (hero 1) */
    if (ST(0x194) == 1 && ST(0x34) == 0x39 && ST(0xE0) == 0) {
        ST(0xE0) = 1;
        CB->group(3);
        CB->named_object_int("L02D107", 0x39);
        if (CB->layer_op(0x1D, 0x0B, 0))
            CB->spawn_678(0x0B, -1, -1, -1, 4, "P11ARR02", -1, 1);
        if (CB->layer_op(0x1D, 0x54, 0))
            CB->spawn_678(0x54, -1, -1, -1, 4, "P84ARR01", -1, 1);
        CB->spawn_678(0x1A, 0x406, (i32)0xFFFFF9C0, 0xAB38, 1, "P26ARR03_50", 0x1F, 0);
        CB->cb_458(2, 2);
        CB->named_object("SP26GAR26");
        CB->named_object("P26GAR03_50");
        CB->cb_46C(&ST(0xE0), 2);
        CB->cb_970(0x39, -1);
    }
    /* 0xAD23 — guard 57 alerted (heroes 2/3) */
    if ((i32)ST(0x194) > 1 && ST(0x34) == 0x39 && ST(0xE0) == 0
        && CB->query_990(0x16, 7) > 0x12C) {
        CB->state_toggle(1, 2);
        ST(0x15C) = 1;
        ST(0xE0) = 1;
        if (ST(0xF90) == 1 || ST(0xF90) == 2)
            CB->group(3);
        CB->layer_op(3, 0x39, 1);
        CB->cb_970(0x39, -1);
        CB->named_object("SP57GAR27");
        CB->named_object("P57ARR02_500");
        CB->actor_934(0x39, 0x61D, (i32)0xFFFFF8D4, 0x437A, -1);
        CB->named_object("SP57ARR02_500");
        CB->cb_46C(&ST(0xE0), 2);
        CB->group(0);
        CB->state_toggle(0, 1);
    }
    /* 0xAE59 — guard 57 back on guard */
    if ((i32)ST(0x194) > 1
        && (ST(0x34) == 0x39 || (ST(0x34) == 0 && ST(0x15C) == 1))
        && ST(0xE0) == 2
        && (CB->query_990(7, 0x14) < 0x96
            || (ST(0xF90) == 1 && ST(0x34) == 0x39))) {
        ST(0xE0) = 0;
        CB->named_object("SP57GAR26");
        CB->named_object("P57GAR02");
        if (ST(0x15C) == 1) {
            ST(0x15C) = 0;
            ST(0x34) = 0x39;
        }
        CB->cb_964(0x21, 0);
        CB->cb_970(0x39, 0);
    }
    /* 0xAF55 */
    if (ST(0xE0) == 2 && !cb968(0x1A)) {
        ST(0xE0) = 3;
        CB->actor_934(-ST(0x194), 0, 0, 0, 0x1A);
        l02_action_register("L02D217", 0x1A, "SP26GAR03", "SP26GAR03_10");
        CB->cb_46C(&ST(0xE0), 4);
    }
    /* 0xAFD6 */
    if (ST(0xE0) == 4 && !cb968(ST(0x194))) {
        ST(0xE0) = 5;
        CB->state_toggle(1, 2);
        CB->named_two_ints(0x27, 0x352);
        CB->named_object("SG26FRA01");
        CB->named_object("P26GAR03");
        CB->state_toggle(0, 1);
    }
    /* 0xB053 */
    if (ST(0x3BC) == 1) {
        ST(0x3BC) = 0;
        CB->state_toggle(2, 2);
        CB->op_9c(0x1C, ST(0x194), 0);
        CB->named_object("SG01ATT07");
        CB->named_object("P26GAR03");
        CB->op_9c(6, ST(0x194), 0);
        CB->cb_46C(&ST(0x194), 4);
        CB->state_toggle(0, 1);
    }
    /* 0xB0EE */
    if (ST(0x3A4) == 1) {
        ST(0x3A4) = 0;
        CB->named_object("SP00ACT01");
        CB->named_two_ints(1, 0x320);
    }
    /* 0xB12A */
    if (ST(0x324) == 1) {
        ST(0x324) = 0;
        ST(0x964) = 0;
        ST(0x18) = 2;
        CB->op_9c(0x18, 1, 4);
        CB->cb_984(1, 0);
    }
    /* 0xB185 — actor 11 (P11) dialogue */
    if (ST(0x88) == 1 && !cb968(0x0B)) {
        ST(0x358) = 2;
        ST(0x88) = 2;
        CB->select_468(-0x0B);
        if (ST(0x198) == 1) l02_action_register("L02D105", 1, "SP01ATT74", NULL);
        if (ST(0x198) == 0) l02_action_register("L02D106", 1, "SP01ATT75", NULL);
        CB->named_object("sar_10");
        CB->actor_934(0x0B, 0, 0, 0, ST(0x194));
        l02_action_register("L06D05C", 0x0B, "SP11DIA65", "SP11ARR02_10");
        l02_action_register("L06D05D", 0x0B, "SP11DIA57", "SP11ARR02_10");
        l02_action_register("L06D05E", 0x0B, "SP11DIA58", "SP11ARR02_10");
        l02_action_register("L06D05F", 0x0B, "SP11DIA64", "SP11ARR02_500");
        CB->named_object("P11_65535");
        CB->actor_934(0x0B, 0, 0, 0xCB20, -1);
        CB->named_object("P11ARR02");
        CB->actor_934(-0x0B, 0xBA, (i32)0xFFFFF982, 0xCB20, -1);
        CB->actor_934(-ST(0x194), 0, 0, ST(0x4C), -1);
        CB->group(0);
    }
    /* 0xB323 */
    if (ST(0x88) == 2 && CB->query_990(0x0E, 6) <= 0x212) {
        ST(0x88) = 3;
        CB->spawn_678(0x0B, 0xBA, (i32)0xFFFFF982, 0xCB20, 4, "", 0x0E, 1);
        CB->cb_46C(&ST(0x1BC), 1);
        CB->cb_93C(0x0E);
    }
    /* 0xB3A6 — actor 84 follows hero 1 */
    if (ST(0x194) == 1 && ST(0xB04) == 0 && ST(0xAF8) == 0
        && (i32)ST(0xAF4) < 3 && CB->layer_op(0x1D, 0x54, 0)) {
        if (ST(0x724) == 0) {
            /* 0xB41A */
            if ((ST(0x74) == 1 || ST(0x74) == 2)
                && CB->query_990(3, 0x14) < 0x96
                && CB->layer_op(8, 0x54, ST(0x194)) < 0xFA0) {
                ST(0x74) = 2;
                CB->select_468(-0x54);
                return;
            }
            /* 0xB496 */
            if (ST(0x74) == 2) {
                i32 near_ = CB->query_990(3, 0x14) < 0xC8
                         && CB->layer_op(8, 0x54, ST(0x194)) < 0x1770;
                if ((!near_ && ST(0x10) == 0) || ST(0x10) == 1) {
                    /* 0xB505 */
                    ST(0x74) = 1;
                    CB->select_468(-0x54);
                    CB->actor_934(-0x54, 0x578, (i32)0xFFFFFD49, 0xEA60, -1);
                    return;
                }
            }
        }
        /* 0xB53D */
        if (ST(0x74) != 0x2C && !cb968(0x54)
            && CB->query_990(3, 0x15) <= 0x64) {
            ST(0x7C) = 2;
            ST(0x74) = 0x2C;
            ST(0xBDC) = 2;
            CB->spawn_678(0x54, 0x578, (i32)0xFFFFFD49, -1, 4, "", 3, 0);
            CB->cb_93C(3);
        }
    }
    /* 0xB5CC */
    if (ST(0xB40) == 3 && ST(0x74) == 1) {
        i32 a0, a1, a2;
        ST(0x74) = 2;
        CB->state_toggle(1, 1);
        CB->spawn_978(0x27, (i32)0xFFFFFBBE, (i32)0xFFFFFA9C, 0x46, 0x46, 0);
        CB->spawn_978(0x28, (i32)0xFFFFFE06, (i32)0xFFFFF934, 0x46, 0x46, 0);
        CB->op_9c(0x10, 5, 0);
        CB->spawn_678(0x0D, -1, -1, -1, 4, "", 0x1A, 0);
        ST(0x1C4) = 4;
        CB->cb_93C(0x1A);
        /* coordinates queried axis 2,1,0 (original order) */
        a2 = CB->layer_op(1, 0x0D, 2);
        a1 = CB->layer_op(1, 0x0D, 1);
        a0 = CB->layer_op(1, 0x0D, 0);
        CB->spawn_678(0x21, a0, a1, a2, 1, "P33ARR01", 0x1B, 1);
        CB->op_9c(0x10, 3, 0);
        CB->named_object("SP33ATT01");
        l02_action_register("L04D04", 0x21, "P33DIA54", "SP33ARR01");
        l02_action_register("L04D05", 0x21, "SP33DIA59", "SP33ARR01_10");
        CB->cb_46C(&ST(0x30), 0);
        CB->named_object("SP33ARR01_10");
        CB->cb_46C(&ST(0x50), 3);
        CB->cb_46C(&ST(0xFC), 1);
        CB->actor_934(0x21, (i32)0xFFFFFBBE, (i32)0xFFFFFA9C, 0xC350, -1);
        CB->group(0);
        CB->state_toggle(0, 1);
    }
    /* 0xB7A1 — actor 13 follows */
    if (ST(0x1C4) == 1 && ST(0xE9C) == 3) {
        if (ST(0xB8) == 1 && CB->query_990(0x1A, 0x14) < 0x78 && ST(0x88) == 0) {
            CB->state_toggle(1, 1);
            if (CB->layer_op(3, 0x0D, 1) == 1)
                CB->layer_op(3, 0x0D, 0);
            ST(0x88) = 1;
            ST(0x90) = 1;
            CB->actor_934(-0x0D, 0, 0, 0, ST(0x194));
            CB->state_toggle(0, 1);
        }
        /* 0xB887 */
        if (ST(0xB8) == 1 && ST(0x38) % 0x14 == 0) {
            if (CB->query_990(0x1A, 0x14) >= 0xB4
                && CB->query_990(0x1A, 0x14) < 0x12C) {
                CB->state_toggle(1, 1);
                ST(0x88) = 0;
                CB->layer_op(3, 0x0D, 0);
                CB->actor_934(-0x0D, -1, -1, -1, ST(0x194));
                CB->state_toggle(0, 1);
            }
            /* 0xB945 */
            if (CB->query_990(0x1A, 0x14) >= 0x12C) {
                CB->state_toggle(1, 1);
                ST(0x88) = 0;
                CB->layer_op(3, 0x0D, 1);
                CB->actor_934(-0x0D, -1, -1, -1, ST(0x194));
                CB->state_toggle(0, 1);
            }
        }
    }
    /* 0xB9BE — actor 33 follows */
    if (ST(0xE9C) == 3 && ST(0x214) == 1 && ST(0x30) == 0) {
        CB->state_toggle(1, 1);
        if (ST(0xFC) == 1) {
            if (CB->query_990(0x1B, 0x14) < 0x96
                && CB->layer_op(8, 0x21, ST(0x194)) < 0xFA0) {
                ST(0xFC) = 2;
                CB->select_468(-0x21);
                CB->state_toggle(0, 1);
                return;
            }
            /* 0xBA81 */
            CB->state_toggle(0, 1);
        }
        /* 0xBA8E */
        if (ST(0xFC) == 2) {
            if (CB->query_990(0x1B, 0x14) < 0xC8
                && CB->layer_op(8, 0x21, ST(0x194)) < 0x1770) {
                /* 0xBDE8 */
                CB->state_toggle(0, 0);
            } else {
                /* 0xBAE2 */
                CB->state_toggle(1, 2);
                ST(0xFC) = 1;
                CB->named_object("SP33ARR01_10");
                if (e_a33_walk_back())
                    return;
                /* falls through to 0xBDE8 */
                CB->state_toggle(0, 0);
            }
        }
    }
    /* 0xBDF2 — actor 33 notices the hero */
    if (ST(0xE9C) == 3 && ST(0x214) == 1 && !cb968(0x21)
        && CB->query_990(0x1B, 0x27) <= 0x64
        && ST(0xF90) != 6 && ST(0xF90) != 5
        && ST(0x7C) == 0 && ST(0x34) == 0) {
        ST(0x7C) = 1;
        ST(0xF68) = ST(0x194);
        CB->state_toggle(0, 1);
        CB->group(3);
        CB->cb_458(5, 2);
        CB->actor_934(-0x21, 0, 0, 0, ST(0x194));
        l02_action_register("L02D65A", 0x21, "SP33DIA59", "SP33ARR01");
        l02_action_register("L02D65C", 0x21, "SP33DIA56", "SP33ARR01_10");
        CB->named_object("SP33ARR01_10");
        CB->cb_458(5, 1);
        CB->group(0);
    }
    /* 0xBF41 — actor 13 joins */
    if (ST(0x1C4) == 1 && ST(0xE9C) == 4 && ST(0x8C) == 1
        && CB->query_990(0x1A, 0x14) < 0x78) {
        CB->group(3);
        ST(0x30) = 1;
        ST(0x8C) = 2;
        ST(0xE9C) = 3;
        ST(0x70) = 1;
        CB->named_object("SP13LEV01");
        CB->named_object("P13_65535");
        CB->actor_934(-0x0D, 0, 0, 0, ST(0x194));
        CB->named_object("SP13ARR01");
        l02_action_register("L08D335A", 0x0D, "SP13DIA60", "SP13ARR01_10");
        l02_action_register("L08D335B", 0x0D, "SP13DIA60", "SP13ARR01_10");
        l02_action_register("L08D335C", 0x0D, "SP13DIA60", "SP13ARR01_10");
        ST(0x8A8) = 2;
        CB->cb_46C(&ST(0xB8), 1);
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        CB->layer_op(0x0C, 0x0D, 1);
    }
    /* 0xC0AB — actor 33 joins */
    if (ST(0x214) == 1 && ST(0xE9C) == 4 && ST(0x8C) == 1
        && CB->query_990(0x14, 0x1B) < 0x78) {
        CB->state_toggle(1, 1);
        CB->group(3);
        ST(0x30) = 1;
        ST(0xFC) = 1;
        ST(0x8C) = 2;
        ST(0xE9C) = 3;
        ST(0x70) = 1;
        ST(0x8A8) = 2;
        CB->named_object("SP33LEV01");
        CB->named_object("P33_65535");
        CB->actor_934(-0x21, 0, 0, 0, ST(0x194));
        CB->named_object("SP33ARR01");
        l02_action_register("L08D335A", 0x21, "SP33DIA60", "SP33ARR01_10");
        l02_action_register("L08D335B", 0x21, "SP33DIA60", "SP33ARR01_10");
        l02_action_register("L08D335C", 0x21, "SP33DIA60", "SP33ARR01_10");
        CB->actor_934(-0x21, (i32)0xFFFFFBBE, (i32)0xFFFFFA9C, 0xC350, -1);
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        if (e_a33_walk_back())   /* 0xC227 */
            return;
    }
    /* 0xC503 — actor 13 chatter */
    if (ST(0x90) == 1 && CB->layer_op(0x0B, 0, 0) >= 0x2710 && ST(0x1C4) == 1) {
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
        CB->layer_op(0x0B, 1, 0);
        CB->cb_46C(&ST(0x90), 1);
        CB->group(0);
        return;
    }
    /* 0xC625 — actor 10 follows */
    if (ST(0x380) == 8 && ST(0x1B8) == 1 && ST(0x30) == 0) {
        if (CB->query_990(0x14, 0x10) < 0x8C && ST(0xBC) == 0) {
            if (CB->layer_op(3, 0x0A, 1) == 1)
                CB->layer_op(3, 0x0A, 0);
            ST(0xBC) = 1;
            CB->actor_934(-0x0A, 0, 0, 0, ST(0x194));
        }
        /* 0xC6DE */
        if (ST(0xF4) == 1 && ST(0x38) % 0x14 == 0) {
            if (CB->query_990(0x14, 0x10) >= 0xB4) {
                CB->layer_op(3, 0x0A, 0);
                ST(0xBC) = 0;
                CB->actor_934(-0x0A, -1, -1, -1, ST(0x194));
            }
            /* 0xC764 */
            if (CB->query_990(0x14, 0x10) >= 0x12C) {
                ST(0xBC) = 0;
                CB->layer_op(3, 0x0A, 1);
                CB->actor_934(-0x0A, -1, -1, -1, ST(0x194));
            }
        }
    }
    /* 0xC7C0 — hero crosses the bridge line */
    if ((ST(0x380) == 8 || ST(0xE9C) == 3) && ST(0x194) != 4
        && CB->layer_op(1, ST(0x194), 1) < (i32)0xFFFFFAF6) {
        if (CB->layer_op(1, ST(0x194), 0) > 0x3B6 && ST(0xF8) == 0)
            ST(0xF8) = 1;
        /* 0xC85D */
        if (CB->layer_op(1, ST(0x194), 0) < 0x320 && ST(0xF8) == 2)
            ST(0xF8) = 3;
    }
    /* 0xC8A1 */
    if (ST(0xF8) == 1) {
        ST(0xF8) = 2;
        if (ST(0xE9C) == 3 && ST(0x1C4) == 1) {
            CB->select_468(-0x0D);
            ST(0xB8) = 0x14;
            ST(0x90) = 0x0A;
            if (ST(0x108) != 7 && ST(0x108) != 6)
                l02_action_register("L02D500", 0x0D, "SP13ARR01", "SP13ARR01_50");
        }
        /* 0xC94C */
        if (ST(0x380) == 8 && ST(0x1B8) == 1) {
            CB->select_468(-0x0A);
            ST(0xF4) = 0x14;
            if (ST(0x108) != 7 && ST(0x108) != 6) {
                if (ST(0x194) == 2) CB->named_object_int("L02D501", 2);
                if (ST(0x194) == 3) CB->named_object_int("L02D502", 3);
            }
        }
    }
    /* 0xC9F2 */
    if (ST(0xF8) == 3) {
        ST(0xF8) = 0;
        if (ST(0xE9C) == 3 && ST(0x1C4) == 1) {
            ST(0xB8) = 1;
            ST(0x90) = 1;
        }
        if (ST(0x380) == 8 && ST(0x1B8) == 1)
            ST(0xF4) = 1;
    }
    /* 0xCA88 — guard 57 */
    if (ST(0xB04) != 2 && ST(0xEB8) == 0) {
        if (ST(0x274) == 1 && ST(0x34) != 0x39 && ST(0x15C) == 0) {
            if (!cb968(0x39) && ST(0x114) == 1) {
                ST(0x114) = 0;
                CB->select_468(-0x39);
                return;
            }
            /* 0xCB23 */
            if ((i32)ST(0x194) >= 2 && CB->query_990(0x14, 0x16) < 0x12C
                && ST(0x64) == 0 && ST(0xDC) == 0) {
                ST(0xDC) = 1;
                CB->named_object("SP57GAR26");
                CB->named_object("P57GAR02_10");
                CB->flush_958(-0x39, ST(0x194));
                return;
            }
            /* 0xCBB1 */
            if (CB->query_990(0x14, 0x16) >= 0x1F4 && ST(0x64) == 0 && ST(0xDC) == 1) {
                ST(0x100) = 0;
                ST(0xDC) = 0;
                CB->select_468(-0x39);
                CB->named_object("SP57GAR27");
                CB->named_object("P57ARR02_10");
                return;
            }
            /* 0xCC31 */
            if (CB->query_990(0x14, 0x16) >= 0x1F4 && ST(0x64) == 1) {
                ST(0x100) = 0;
                ST(0x64) = 0;
                ST(0xD4) = 1;
                CB->select_468(-0x39);
                CB->actor_934(-0x39, 0x61D, (i32)0xFFFFF8D4, 0x437A, -1);
                return;
            }
        }
        /* 0xCCB1 */
        if (!cb968(0x39) && ST(0xD4) == 1) {
            ST(0xD4) = 2;
            CB->cb_964(0x21, 1);
        }
    }
    /* 0xCCF5 — guard 59 back to post */
    if (ST(0x27C) == 1 && CB->query_990(0x14, 2) >= 0x190
        && ST(0x64) == 1 && ST(0x34) != 0x3B) {
        ST(0x64) = 0;
        ST(0x68) = 0;
        ST(0xD4) = 1;
        CB->actor_934(0x3B, 0x1EA, (i32)0xFFFFFD44, 0x7D00, -1);
    }
    /* 0xCD83 — guard 59 */
    if (ST(0x68) == 0 && ST(0x27C) == 1 && ST(0x34) != 0x3B) {
        if (ST(0x720) == 0 && CB->query_990(2, 0x14) < 0x190) {
            ST(0x68) = 1;
            CB->group(3);
            CB->actor_934(0x3B, 0, 0, 0, ST(0x194));
            l02_action_register("L02D67", 0x3B, "SP59GAR26", "SP59GAR03_10");
            CB->cb_964(0x21, 1);
            CB->cb_93C(9);
            CB->group(0);
        }
        /* 0xCE50 */
        if (ST(0x720) == 1 && CB->query_990(2, 0x14) < 0x12C) {
            CB->cb_964(0x20, 0);
            CB->group(3);
            ST(0x68) = 1;
            ST(0x64) = 1;
            CB->named_object_int("L02D68", 0x3B);
            CB->actor_934(0x3B, 0x26F, (i32)0xFFFFFD44, 0x59D8, -1);
            CB->group(0);
        }
    }
    /* 0xCEE1 — guard 58 */
    if (ST(0x6C) == 0 && ST(0x278) == 1 && ST(0x34) != 0x3A
        && CB->query_990(0x0B, 0x14) < 0x320) {
        ST(0x6C) = 1;
        l02_action_register("L02D110", 0x3A, "SP58ARR04", "SP58GAR05_10");
    }
    /* 0xCF51 */
    if (ST(0x6C) == 1 && ST(0x278) == 1 && ST(0x34) != 0x3A
        && CB->query_990(0x0B, 0x14) < 0x190) {
        ST(0x6C) = 2;
        CB->cb_964(0x21, 0);
        ST(0x34) = 0x3A;
        CB->cb_970(0x3A, 0);
    }
    /* 0xCFCE */
    if ((ST(0x1C4) == 2 && ST(0x34) == 0x0D)
        || (ST(0x214) == 2 && ST(0x34) == 0x21)) {
        ST(0xE58) = 1;
        ST(0x30) = 0;
        ST(0x34) = 0;
    }
    /* 0xD037 — re-arm zones once the fighting guard is dead */
    if ((i32)ST(0x194) > 1 && ST(0x34) == 0x39 && ST(0x274) == 2) {
        ST(0x34) = 0;
        CB->spawn_978(1, 0x640, (i32)0xFFFFF8DA, 0x50, 0x5A, 0);
    }
    /* 0xD091 */
    if (ST(0x34) == 0x3A && ST(0x278) == 2) {
        ST(0x34) = 0;
        CB->spawn_978(1, 0x640, (i32)0xFFFFF8DA, 0x50, 0x5A, 0);
    }
    /* 0xD0D9 */
    if (ST(0x34) == 0x3B && ST(0x27C) == 2) {
        ST(0x34) = 0;
        CB->spawn_978(9, 0x1E6, (i32)0xFFFFFD9E, 0x78, 0x5A, 0);
    }
    /* 0xD121 — hero 1 at the gate */
    if (ST(0x358) == 0 && ST(0x194) == 1
        && CB->layer_op(1, ST(0x194), 3) < 0x0A && ST(0xF90) == 5) {
        if (ST(0x7C) == 1) {
            ST(0x7C) = 2;
            CB->spawn_678(0x54, 0x578, (i32)0xFFFFFD49, -1, 4, "", 3, 0);
            ST(0x74) = 0x2C;
            ST(0xBDC) = 2;
            CB->spawn_678(0x0B, 0x7D0, (i32)0xFFFFF982, 0x3A98, 1, "P11ARR02", 0x0E, 0);
            return;
        }
        /* 0xD1FA */
        if (ST(0x54) == 0) {
            ST(0x54) = 1;
            CB->group(3);
            ST(0x4C) = CB->layer_op(1, ST(0x194), 2);
            CB->spawn_678(0x0B, 0xBA, (i32)0xFFFFF982, 0x3A98, 1, "P11ARR02", 0x0E, 0);
            CB->named_object_int("L02D50", 0x0B);
            CB->actor_934(-ST(0x194), 0, 0, 0, 0x0B);
            CB->cb_93C(0x18);
            CB->cb_93C(0x19);
            CB->actor_934(0x0B, (i32)0xFFFFFF36, (i32)0xFFFFF963, 0x3A98, -1);
            CB->cb_46C(&ST(0x88), 1);
        }
    }
    /* 0xD2DC — debug overlay (sprintf into the global dword_1002C6A0) */
    snprintf(g_txt, sizeof(g_txt), "CRI:%d", ST(0x10C));
    CB->debug_text(g_txt, 0, 0xF0);
    snprintf(g_txt, sizeof(g_txt), "cri_on:%d", ST(0x110));
    CB->debug_text(g_txt, 0, 0xF8);
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
    case 0:  ST(0x320) = 1; break;
    case 1:  ST(0x324) = 1; break;
    case 2:  ST(0x328) = 1; break;

    case 7:  /* 0x33C increment chain 1->2->3->4->5 */
        if      (ST(0x33C) == 1) ST(0x33C) = 2;
        else if (ST(0x33C) == 2) ST(0x33C) = 3;
        else if (ST(0x33C) == 3) ST(0x33C) = 4;
        else if (ST(0x33C) == 4) ST(0x33C) = 5;
        break;

    case 8:  if (ST(0x340) == 1) ST(0x340) = 2; break;

    case 9:
        if (ST(0x1B0) != 2 && ST(0x344) == 1) {
            ST(0xB80) = 0;
            ST(0x344) = 0;
        }
        break;

    case 11: if (ST(0x34C) == 1) ST(0x34C) = 2; break;
    case 12: if (ST(0x350) == 1) ST(0x350) = 2; break;

    case 15: /* 0x8C increment chain (odd->even+1, 0->1..0xC->0xD) */
        if      (ST(0x8C) == 0)   ST(0x8C) = 1;
        else if (ST(0x8C) == 2)   ST(0x8C) = 3;
        else if (ST(0x8C) == 4)   ST(0x8C) = 5;
        else if (ST(0x8C) == 6)   ST(0x8C) = 7;
        else if (ST(0x8C) == 8)   ST(0x8C) = 9;
        else if (ST(0x8C) == 0xA) ST(0x8C) = 0xB;
        else if (ST(0x8C) == 0xC) ST(0x8C) = 0xD;
        break;

    case 16:
        if (ST(0x360) == 0) {
            ST(0x360) = 1;
            CB->cb_950(1);
            CB->cb_97C(2, 0);
            if (ST(0xB40) == 1) {
                ST(0xB40) = 2;
                CB->named_object("Sar_500");
                CB->cb_46C(&ST(0x68), 1);
            }
            if (ST(0xE30) == 1) {
                ST(0xE30) = 2;
                CB->named_object("Sar_500");
                CB->cb_46C(&ST(0xE0), 1);
            }
        }
        break;

    case 17: if (ST(0x364) == 1) ST(0x364) = 2; break;

    case 18:
        if ((i32)ST(0xF5C) > 0) {
            ST(0xF5C) -= 0x1388;
            CB->op_9c(0x29, 0, 0);
            CB->named_two_ints(0x12, 0x1388);
        }
        break;

    case 19:
        if (ST(0xEBC) == 1 && ST(0x18) != 0x1C)
            ST(0xEBC) = 2;
        break;

    case 20:
        if (ST(0x18) == 0x12)
            CB->place_95C("L08B06", 0xAB4, 0x7BF);
        if (ST(0x18) == 8) {
            CB->state_toggle(2, 1);
            CB->place_95C("L08B06", 0x2C, (i32)0xFFFFFC40);
            CB->state_toggle(0, 1);
        }
        break;

    case 21: if (ST(0x374) == 0) ST(0x374) = 1; break;
    case 22: if (ST(0x378) == 0) ST(0x378) = 1; break;
    case 25: if (ST(0x384) == 0) ST(0x384) = 2; break;
    case 27: if (ST(0x38C) == 0) ST(0x38C) = 1; break;

    case 28:
        if (ST(0x390) == 0) {
            CB->group(0);
            ST(0x18) = 0x17;
            CB->cb_984(0x19, -1);
        }
        break;

    case 29:
        if      (ST(0x394) == 2) ST(0x394) = 3;
        else if (ST(0x394) == 0) ST(0x394) = 1;
        break;

    case 30:
        if (ST(0x398) == 1) {
            ST(0x398) = 0;
            ST(0xBF4) = 1;
        }
        break;

    case 31: if (ST(0x39C) == 0) ST(0x39C) = 1; break;
    case 33: ST(0x3A4) = 1; break;
    case 34: ST(0x3A8) = 1; break;
    case 35: ST(0x3AC) = 1; break;
    case 36: ST(0x3B0) = 1; break;
    case 37: ST(0x3B4) = 1; break;
    case 38: ST(0x3B8) = 1; break;
    case 39: ST(0x3BC) = 1; break;
    case 40: ST(0x3C0) = 1; break;
    case 41: ST(0x3C4) = 1; break;

    case 42:
        if (ST(0xC54) != -1) {
            CB->state_toggle(2, 1);
            CB->op_9c(0x18, ST(0xC54), 4);
            CB->named_two_ints(0x2B, 0x1388);
            CB->state_toggle(0, 1);
        }
        break;

    case 43:
        CB->state_toggle(2, 1);
        CB->op_9c(0x18, ST(0xC54), 1);
        CB->named_two_ints(0x2A, 0xAFC8);
        CB->state_toggle(0, 1);
        break;

    case 44: ST(0x3D0) = 1; break;

    case 51: /* 0x3EC chain 0->1,2->3,4->5 */
        if      (ST(0x3EC) == 0) ST(0x3EC) = 1;
        else if (ST(0x3EC) == 2) ST(0x3EC) = 3;
        else if (ST(0x3EC) == 4) ST(0x3EC) = 5;
        break;
    case 52: /* 0x3F0 chain 0->1,2->3 */
        if      (ST(0x3F0) == 0) ST(0x3F0) = 1;
        else if (ST(0x3F0) == 2) ST(0x3F0) = 3;
        break;
    case 53: /* 0x3F4 chain 0->1,2->3,4->5 */
        if      (ST(0x3F4) == 0) ST(0x3F4) = 1;
        else if (ST(0x3F4) == 2) ST(0x3F4) = 3;
        else if (ST(0x3F4) == 4) ST(0x3F4) = 5;
        break;

    case 70:  /* alarm bells (named_two_ints F/G/H at fixed values) */
        CB->state_toggle(2, 1); CB->named_two_ints(0x46, 0x2134); CB->state_toggle(0, 1);
        break;
    case 71:
        CB->state_toggle(2, 1); CB->named_two_ints(0x47, 0x9C4);  CB->state_toggle(0, 1);
        break;
    case 72:
        CB->state_toggle(2, 1); CB->named_two_ints(0x48, 0x4844); CB->state_toggle(0, 1);
        break;
    case 73:
        CB->state_toggle(2, 1); CB->named_two_ints(0x46, 0x2904); CB->state_toggle(0, 1);
        break;
    case 74:
        CB->state_toggle(2, 1); CB->named_two_ints(0x46, 0x7530); CB->state_toggle(0, 1);
        break;

    case 80:
        CB->state_toggle(0, 2);
        CB->named_two_ints(0x51, 0x9C4);
        CB->named_object("SP01ACT99");
        CB->state_toggle(0, 1);
        break;

    case 94:  /* pipe puzzle SP02PIPI1/2 by layer query */
        CB->state_toggle(2, 1);
        CB->group(3);
        if (CB->layer_op(2, 9, 2))
            CB->named_object("SP02PIPI2");
        else
            CB->named_object("SP02PIPI1");
        CB->named_object("ar_50");
        CB->group(0);
        CB->state_toggle(0, 1);
        break;

    case 50: {  /* SOB24%c/SOB77%c via named_ex_988 */
        CB->layer_op(0x0A, 0x7D0, 0x7D0);
        CB->named_object("SP01ACT96");
        CB->named_object("ar");
        c8 buf1[48], buf2[48];
        snprintf(buf1, sizeof(buf1), "SOB24%c", (char)(ST(0x128) + 0x41));
        CB->named_ex_988(buf1, -1, -1, -1, -1, -1, -1, -2, 0);
        snprintf(buf2, sizeof(buf2), "SOB77%c", (char)(ST(0x128) + 0x41));
        CB->named_ex_988(buf2, -1, -1, -1, -1, -1, -1, -3, 0);
        CB->group(0);
        break;
    }
    case 81: {  /* P01ACT97 + layer-6 clamp(0x1E) */
        CB->state_toggle(0, 2);
        CB->named_object("P01ACT97");
        CB->named_two_ints(0x32, 0x320);
        e5_clamp6(0x1E);
        CB->state_toggle(0, 1);
        break;
    }
    case 90:  /* float accum grow (index 0) */
        CB->state_toggle(2, 1);
        if (g_float_accum[0] < 3.0f && g_float_active != 0) {
            CB->layer_op(0x26, 0x66, 0x10011);
            CB->named_two_ints(0x5A, 0x64);
            g_float_accum[0] *= 1.02f;
        }
        CB->state_toggle(0, 1);
        break;
    case 91:  /* float accum grow (index 1, dual layer_op) */
        CB->state_toggle(2, 1);
        if (g_float_accum[1] < 3.0f && g_float_active != 0) {
            CB->layer_op(0x26, 0x66, 0x10005);
            CB->layer_op(0x26, 0x66, 0x10007);
            CB->named_two_ints(0x5B, 0x64);
            g_float_accum[1] *= 1.02f;
        }
        CB->state_toggle(0, 1);
        break;
    case 92:  /* float accum grow (index 2, dual layer_op) */
        CB->state_toggle(2, 1);
        if (g_float_accum[2] < 3.0f && g_float_active != 0) {
            CB->layer_op(0x26, 0x66, 0x1000E);
            CB->layer_op(0x26, 0x66, 0x10010);
            CB->named_two_ints(0x5C, 0x64);
            g_float_accum[2] *= 1.02f;
        }
        CB->state_toggle(0, 1);
        break;
    case 93:  /* float accum decay (index 3) */
        CB->state_toggle(2, 1);
        if (g_float_accum[3] > 0.5f && g_float_active != 0) {
            CB->layer_op(0x25, 0x62, 0);
            CB->named_two_ints(0x5D, 0x64);
            g_float_accum[3] *= 0.98f;
        }
        CB->state_toggle(0, 1);
        break;
    case 98: {  /* float reset + respawn toggle */
        CB->state_toggle(2, 1);
        CB->group(3);
        if (g_float_active != 0) {
            g_float_active = 0;
            e4_reset_floats();
            i32 c2 = CB->layer_op(1, ST(0x194), 2);
            i32 c1 = CB->layer_op(1, ST(0x194), 1);
            i32 c0 = CB->layer_op(1, ST(0x194), 0);
            CB->spawn_678(ST(0x194) + 100, c0, c1, c2, 1,
                          "ar", ST(0x138), 0);
            CB->named_object("Sar_50");
            CB->named_two_ints(0x62, 0x64);
            CB->group(0);
            CB->state_toggle(0, 1);
        } else {
            g_float_active = 1;
            CB->group(0);
            CB->state_toggle(0, 1);
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
    CB->state_toggle(0, 2);
    CB->op_9c(0x30, 0, 0);
    CB->op_9c(0x30, 0, 0);
    CB->op_9c(0x30, 0, 0);
    CB->group(3);
}

/* case 4 / 22 / 65 template: SG01ATT01 + LCOD41/42/43 + ar_50 */
static void e5_att01(void) {
    e5_reset();
    CB->named_object("SG01ATT01");
    if (ST(0x194) == 1) CB->named_object_int("LCOD41", ST(0x194));
    if (ST(0x194) == 2) CB->named_object_int("LCOD42", ST(0x194));
    if (ST(0x194) == 3) CB->named_object_int("LCOD43", ST(0x194));
    CB->named_object("ar_50");
    CB->group(0);
}

/* Common tail: layer-6 value += k, clamped to 0x3FF (op 6 on [0x194]). */
static void e5_clamp6(i32 k) {
    if (CB->layer_op(6, ST(0x194), -1) + k < 0x400)
        CB->op_9c(6, ST(0x194), CB->layer_op(6, ST(0x194), -1) + k);
    else
        CB->op_9c(6, ST(0x194), 0x3FF);
}

/* SGxxMAN03/04 (levels 2,3) by [0x198], + ar_50 + clamp(k). */
static void e5_man34(i32 k) {
    if (ST(0x194) == 2) {
        if (ST(0x198) == 1) CB->named_object("SG02MAN03");
        if (ST(0x198) == 0) CB->named_object("SG02MAN04");
    }
    if (ST(0x194) == 3) {
        if (ST(0x198) == 1) CB->named_object("SG03MAN03");
        if (ST(0x198) == 0) CB->named_object("SG03MAN04");
    }
    CB->named_object("ar_50");
    e5_clamp6(k);
}

/* SG01ATT01 + LCOD pair (lvl2->a, lvl3->b) + ar_50. */
static void e5_att01_lcod(const c8 *a, const c8 *b) {
    CB->named_object("SG01ATT01");
    if (ST(0x194) == 2) CB->named_object_int(a, ST(0x194));
    if (ST(0x194) == 3) CB->named_object_int(b, ST(0x194));
    CB->named_object("ar_50");
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
        if (ST(0x18) == 0x23) {
            e5_reset();
            CB->named_object("SG00ATT01");
            CB->group(0);
        }
        break;

    case 42:  /* 0x250D */
        e5_reset();
        CB->named_object("SG01ATT01");
        if (ST(0x194) == 1) CB->named_object_int("LCOD41", ST(0x194));
        if (ST(0x194) == 2) CB->named_object_int("LCOD42", ST(0x194));
        if (ST(0x194) == 3) CB->named_object_int("LCOD43", ST(0x194));
        CB->named_object("ar_50");
        CB->group(0);
        break;

    case 49:  /* 0x25ED — Sar imp interaction (LCOD02/03 A-H) */
        if (ST(0xEA8) == 0 || ST(0xEA8) == 1) {
            e5_reset();
            if (ST(0x194) == 2) {
                l02_action_register("LCOD02A", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02B", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02C", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02D", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02E", 2, "SP02ACT64", "SP02ACT64");
                l02_action_register("LCOD02F", 2, "SP02ACT64", "SP02ACT64");
                if (ST(0xEA8) == 0) {
                    l02_action_register("LCOD02G", 2, "SP02ACT64", "SP02ACT64");
                    l02_action_register("LCOD02H", 2, "SP02ACT64", "SP02ACT64");
                }
                ST(0xEA8) = 1;
            }
            if (ST(0x194) == 3) {
                l02_action_register("LCOD03A", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03B", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03C", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03D", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03E", 3, "SP03ACT64", "SP03ACT64");
                l02_action_register("LCOD03F", 3, "SP03ACT64", "SP03ACT64");
                if (ST(0xEA8) == 0) {
                    l02_action_register("LCOD03G", 3, "SP03ACT64", "SP03ACT64");
                    l02_action_register("LCOD03H", 3, "SP03ACT64", "SP03ACT64");
                }
                ST(0xEA8) = 1;
            }
            if (ST(0x18) == 2 && ST(0xF90) == 1 && ST(0xEB8) == 1
                && CB->layer_op(0x1D, 0x1A, 0)) {
                CB->layer_op(0x17, 0, 0);
                if (CB->query_990(0x14, 0x1F) < 0x96)
                    CB->actor_934(-ST(0x194), 0, 0, 0, 0x1A);
                else
                    CB->actor_934(-ST(0x194), -1, -1, -1, 0x1A);
                if (CB->query_990(0x14, 0x1F) >= 0x96)
                    CB->named_two_ints(0x29, 0x7D0);
                else if (CB->query_990(0x14, 0x1F) < 0x96 && CB->query_954(0x1A) == 0)
                    CB->named_two_ints(0x29, 0x7D0);
                else
                    CB->named_two_ints(0x29, 0x1F4);
            } else {
                CB->named_object("Sar_50");
                CB->group(0);
            }
        } else {
            CB->named_object("Sar_50");
            CB->group(0);
        }
        break;

    case 50:  /* 0x29A5 */
        e5_reset();
        if (ST(0x194) == 2) l02_action_register("LCOD88", 2, "SG00DIA02", NULL);
        if (ST(0x194) == 3) l02_action_register("LCOD89", 3, "SG00DIA02", NULL);
        CB->group(0);
        break;

    case 46:  /* 0x2A47 — LCOD100/101/102/103/104 */
        e5_reset();
        if (ST(0x194) == 2) {
            l02_action_register("LCOD100A", 2, "SP02ACT64", "SP02ACT64");
            CB->named_object_int("LCOD100B", 2);
            CB->named_object_int("LCOD100C", 2);
            CB->named_object_int("LCOD100D", 2);
        }
        if (ST(0x194) == 3) {
            l02_action_register("LCOD101A", 3, "SP03ACT64", "SP03ACT64");
            CB->named_object_int("LCOD101B", 3);
            CB->named_object_int("LCOD101C", 3);
            CB->named_object_int("LCOD101D", 3);
        }
        CB->named_object_int("LCOD102A", 2);
        CB->named_object_int("LCOD102B", 2);
        CB->named_object_int("LCOD102C", 2);
        CB->named_object_int("LCOD102D", 2);
        CB->named_object_int("LCOD102E", 2);
        CB->named_object_int("LCOD102F", 2);
        if (ST(0x194) == 2) l02_action_register("LCOD103", 2, "SG02DIA01", NULL);
        if (ST(0x194) == 3) l02_action_register("LCOD104", 3, "SG03DIA01", NULL);
        CB->named_object("Sar_50");
        CB->group(0);
        break;

    case 70:  /* 0x2CEA */
        e5_reset();
        if (ST(0x194) == 2) l02_action_register("LCOD90", 2, "SG00DIA02", NULL);
        if (ST(0x194) == 3) l02_action_register("LCOD91", 3, "SG00DIA02", NULL);
        CB->group(0);
        break;

    case 13:  /* 0x39E1 — Sar mannequin dialogue + layer clamp */
        e5_reset();
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) CB->named_object("SG02MAN01");
            if (ST(0x198) == 0) CB->named_object("SG02MAN02");
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) CB->named_object("SG03MAN01");
            if (ST(0x198) == 0) CB->named_object("SG03MAN02");
        }
        CB->named_object("ar_50");
        e5_clamp6(0x64);
        CB->group(0);
        break;

    case 77:  /* 0x3575 — SPxxACT17 */
        e5_reset();
        if (ST(0x194) == 1) CB->named_object("SP01ACT17");
        if (ST(0x194) == 2) CB->named_object("SP02ACT17");
        if (ST(0x194) == 3) CB->named_object("SP03ACT17");
        CB->named_object("ar_50");
        if (ST(0x194) == 1) l02_action_register("LCOD41", ST(0x194), "SG01ATT01", NULL);
        if (ST(0x194) == 2) l02_action_register("LCOD42", ST(0x194), "SG01ATT01", NULL);
        if (ST(0x194) == 3) l02_action_register("LCOD43", ST(0x194), "SG01ATT01", NULL);
        CB->group(0);
        break;

    case 81:  /* 0x36B3 — LCOD23/24 A-G */
        e5_reset();
        if (ST(0xEAC) == 0) ST(0xEAC) = 1;
        if (ST(0x18) == 0x1C) ST(0x84) = 1;
        if (ST(0x194) == 2) {
            l02_action_register("LCOD23A", 2, "SP02ACT64", "SP02ACT64");
            CB->named_object_int("LCOD23B", 2);
            CB->named_object_int("LCOD23C", 2);
            CB->named_object_int("LCOD23D", 2);
            CB->named_object_int("LCOD23E", 2);
            CB->named_object_int("LCOD23F", 2);
            l02_action_register("LCOD23G", 2, "SG02DIA01", NULL);
        }
        if (ST(0x194) == 3) {
            l02_action_register("LCOD24A", 3, "SP03ACT64", "SP03ACT64");
            CB->named_object_int("LCOD24B", 3);
            CB->named_object_int("LCOD24C", 3);
            CB->named_object_int("LCOD24D", 3);
            CB->named_object_int("LCOD24E", 3);
            CB->named_object_int("LCOD24F", 3);
            l02_action_register("LCOD24G", 3, "SG03DIA01", NULL);
        }
        CB->named_object("Sar_50");
        CB->group(0);
        break;

    case 84:  /* 0x3871 */
        e5_reset();
        CB->named_object("SG01ATT01");
        if (ST(0x194) == 2) CB->named_object_int("LCOD672", ST(0x194));
        if (ST(0x194) == 3) CB->named_object_int("LCOD673", ST(0x194));
        CB->named_object("ar_50");
        CB->group(0);
        break;

    case 85:  /* 0x3929 */
        e5_reset();
        CB->named_object("SG01ATT01");
        if (ST(0x194) == 2) CB->named_object_int("LCOD674", ST(0x194));
        if (ST(0x194) == 3) CB->named_object_int("LCOD675", ST(0x194));
        CB->named_object("ar_50");
        CB->group(0);
        break;

    case 34:  /* 0x44E2 — SGxxMAN01/02 + clamp 0x4B */
        e5_reset();
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) CB->named_object("SG02MAN01");
            if (ST(0x198) == 0) CB->named_object("SG02MAN02");
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) CB->named_object("SG03MAN01");
            if (ST(0x198) == 0) CB->named_object("SG03MAN02");
        }
        CB->named_object("ar_50");
        e5_clamp6(0x4B);
        CB->group(0);
        break;

    case 36:  /* 0x464F — SGxxMAN03/04 + clamp 0x4B */
        e5_reset(); e5_man34(0x4B); CB->group(0);
        break;

    case 43:  /* 0x47BC — SGxxMAN03/04 + clamp 0x64 */
        e5_reset(); e5_man34(0x64); CB->group(0);
        break;

    case 56:  /* 0x4B32 — MAN03/04 + clamp 0x4B */
        e5_reset(); e5_man34(0x4B); CB->group(0);
        break;

    case 64:  /* 0x4C9F — MAN03/04 + clamp 0x64 */
        e5_reset(); e5_man34(0x64); CB->group(0);
        break;

    case 95:  /* 0x4E0C — MAN03/04 + clamp 0x96 */
        e5_reset(); e5_man34(0x96); CB->group(0);
        break;

    case 44:  /* 0x4929 — SGxxMAN (levels 0-3) + clamp 0x96 */
        e5_reset();
        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) CB->named_object("SG00MAN01");
            if (ST(0x198) == 0) CB->named_object("SG00MAN02");
        }
        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) CB->named_object("SG01MAN01");
            if (ST(0x198) == 0) CB->named_object("SG01MAN02");
        }
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) CB->named_object("SG02MAN03");
            if (ST(0x198) == 0) CB->named_object("SG02MAN04");
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) CB->named_object("SG03MAN03");
            if (ST(0x198) == 0) CB->named_object("SG03MAN04");
        }
        CB->named_object("ar_50");
        e5_clamp6(0x96);
        CB->group(0);
        break;

    case 1:   /* 0x5161 — SG01ATT01 + LCOD26/27 */
        e5_reset(); e5_att01_lcod("LCOD26", "LCOD27"); CB->group(0);
        break;

    case 2:   /* 0x5219 (shared 2/12/18/19) — SG01ATT01 + L25D79/82 */
    case 12:
    case 18:
    case 19:
        e5_reset(); e5_att01_lcod("L25D79", "L25D82"); CB->group(0);
        break;

    case 3:   /* 0x52D1 — state_toggle(1,1) + SG01ATT01 + LCOD39/40 */
        CB->state_toggle(1, 1);
        CB->op_9c(0x30, 0, 0);
        CB->op_9c(0x30, 0, 0);
        CB->op_9c(0x30, 0, 0);
        CB->group(3);
        e5_att01_lcod("LCOD39", "LCOD40");
        CB->group(0);
        break;

    /* 0x538C — big shared group: SG01ATT01 + LCOD39/40 */
    case 5: case 6: case 7: case 8: case 9: case 10: case 11:
    case 14: case 15: case 16: case 20: case 29: case 33:
    case 39: case 40: case 41: case 45: case 51: case 60:
    case 72: case 73: case 74: case 75: case 86: case 87:
    case 89: case 93: case 94: case 96:
        e5_reset(); e5_att01_lcod("LCOD39", "LCOD40"); CB->group(0);
        break;

    case 82:  /* 0x5444 — LCOD55/56 */
        e5_reset();
        if (ST(0x194) == 2) l02_action_register("LCOD55", 2, "SP02ACT64", NULL);
        if (ST(0x194) == 3) l02_action_register("LCOD56", 3, "SP02ACT64", NULL);
        CB->named_object("ar_50");
        CB->group(0);
        break;

    case 99:  /* 0x4F7D — SPxxATT20 first-time, else LCOD41/42/43 */
        e5_reset();
        if (ST(0x160) == 0) {
            ST(0x160) = 1;
            if (ST(0x194) == 1) CB->named_object("SP01ATT20");
            if (ST(0x194) == 2) CB->named_object("SP02ATT20");
            if (ST(0x194) == 3) CB->named_object("SP02ATT20");
            e5_clamp6(0x96);
            CB->named_object("ar_50");
        } else {
            if (ST(0x194) == 1) CB->named_object_int("LCOD41", ST(0x194));
            if (ST(0x194) == 2) CB->named_object_int("LCOD42", ST(0x194));
            if (ST(0x194) == 3) CB->named_object_int("LCOD43", ST(0x194));
            CB->named_object("SG01ATT01");
            CB->named_object("ar_50");
        }
        CB->group(0);
        break;

    case 21: {  /* 0x3B4E — OB21A/B/C by phase (edx), ATT19/20 + clamp */
        const c8 *ob;
        if      (phase == 0) ob = "OB21A";
        else if (phase == 1) ob = "OB21B";
        else                 ob = "OB21C";
        e5_reset();
        if (ST(0x198) == 1) {
            if (ST(0x194) == 2) CB->named_object("SP02ATT19");
            if (ST(0x194) == 3) CB->named_object("SP03ATT19");
        }
        if (ST(0x198) == 0) {
            if (ST(0x194) == 2) CB->named_object("SP02ATT20");
            if (ST(0x194) == 3) CB->named_object("SP03ATT20");
        }
        CB->named_object("ar_50");
        CB->named_ex_988(ob, -1, -1, -1, -1, -1, -1, -2, 3);
        e5_clamp6(0xC8);
        CB->group(0);
        break;
    }

    case 24: {  /* 0x3F9C — OB24%c/OB77%c timer, phase=array index */
        if (ST(0x10) != 0) break;
        e5_reset();
        if (ST(0x198) != 1 && (ST(0x198) != 0 || ST(0x194) == 1))
            goto case24_fallback;
        {
            i32 *timer = &ST(0x140 + phase * 4);
            if (*timer > 0) {
                (*timer)--;
                if (ST(0x194) == 1) CB->named_object("SP01ACT98");
                if (ST(0x194) == 2) CB->named_object("SP02ATT19");
                if (ST(0x194) == 3) CB->named_object("SP03ATT19");
                e5_clamp6(0x1E);
                CB->named_object("ar");
                CB->group(0);
                break;
            }
            if (*timer == 0) {
                if (ST(0x194) == 1) CB->named_object("SP01ACT98");
                if (ST(0x194) == 2) CB->named_object("SP02ATT19");
                if (ST(0x194) == 3) CB->named_object("SP03ATT19");
                c8 b1[16], b2[16];
                snprintf(b1, sizeof b1, "OB24%c", phase + 'A');
                snprintf(b2, sizeof b2, "OB77%c", phase + 'A');
                CB->named_ex_988(b1, -1, -1, -1, -1, -1, -1, -2, 0);
                CB->named_ex_988(b2, -1, -1, -1, -1, -1, -1, -3, 0);
                e5_clamp6(0x1E);
                if (ST(0x194) == 1) CB->named_object_int("LCOD41", ST(0x194));
                if (ST(0x194) == 2) CB->named_object_int("LCOD42", ST(0x194));
                if (ST(0x194) == 3) CB->named_object_int("LCOD43", ST(0x194));
                CB->named_object("SG01ATT01");
                CB->named_object("ar_50");
                CB->group(0);
                break;
            }
        }
    case24_fallback:
        if (ST(0x198) == 0 && ST(0x194) == 1) {
            ST(0x140 + phase * 4) = 0;
            CB->named_object("SP01ACT98");
            CB->named_two_ints(0x50, 0x7D0);
        }
        CB->group(0);
        break;
    }

    case 31: {  /* 0x4385 — OB31 + layer 0x14 reduction */
        e5_reset();
        if (ST(0x194) == 2) CB->named_object("SP02ATT20");
        if (ST(0x194) == 3) CB->named_object("SP03ATT20");
        CB->named_object("ar_50");
        e5_clamp6(0x32);
        CB->named_object("ar_50");
        {
            i32 lv = CB->layer_op(0x14, -1, 0);
            CB->op_9c(0x14, lv - 0x96, 0);
        }
        CB->named_ex_988("OB31", -1, -1, -1, -1, -1, -1, -2, 0);
        CB->group(0);
        break;
    }

    case 71: {  /* 0x2D8C — SOB86 FSM (multi-branch room logic) */
        CB->state_toggle(0, 2);
        CB->op_9c(0x30, 0, 0);
        CB->op_9c(0x30, 0, 0);
        CB->op_9c(0x30, 0, 0);
        if (ST(0xF44) == 0) {
            if (ST(0x18) == 0x1E) {
                if (ST(0xF90) != 5 || ST(0x2B0) != 2) goto case71_lcod;
                ST(0xF44) = 1;
                i32 v4 = CB->layer_op(1, ST(0x194), 2);
                CB->group(3);
                CB->op_9c(0x10, 5, 0);
                if (ST(0x194) == 2)
                    CB->spawn_678(2, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                if (ST(0x194) == 3)
                    CB->spawn_678(3, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                CB->named_ex_988("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                if (ST(0x194) == 2) {
                    CB->spawn_678(0x62, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                    CB->spawn_678(0x62, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                }
                if (ST(0x194) == 3) {
                    CB->spawn_678(0x63, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                    CB->spawn_678(0x63, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                }
                CB->op_9c(0x10, 3, 0);
                if (ST(0xF5C) == 0) {
                    ST(0xF44) = 0;
                    if (ST(0x194) == 2) {
                        CB->named_object_int("L31D83A", 2);
                        CB->named_object_int("L31D83B", 2);
                        if (ST(0xC50) == 0) CB->named_object_int("L31D83C", 2);
                    }
                    if (ST(0x194) == 3) {
                        CB->named_object_int("L31D84A", 3);
                        CB->named_object_int("L31D84B", 3);
                        if (ST(0xC50) == 0) CB->named_object_int("L31D84C", 3);
                    }
                    CB->named_object("Sar_50");
                    CB->op_9c(0x10, 5, 0);
                    if (ST(0x194) == 2)
                        CB->spawn_678(2, 0x115C, 0x169E, v4, 0, "ar", 1, 0);
                    if (ST(0x194) == 3)
                        CB->spawn_678(3, 0x115C, 0x169E, v4, 0, "ar", 1, 0);
                    CB->op_9c(0x10, 3, 0);
                }
                CB->named_object("Sar_50");
                CB->group(0);
                break;
            }
            if (ST(0x18) == 0x25) {
                if (ST(0xF90) != 0 || CB->query_990(1, 6) >= 0x15E)
                    goto case71_lcod;
                ST(0xF44) = 1;
                CB->group(3);
                CB->named_ex_988("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                CB->op_9c(0x10, 5, 0);
                if (ST(0x194) == 2)
                    CB->spawn_678(0x62,
                        CB->layer_op(1, ST(0x194), 0),
                        CB->layer_op(1, ST(0x194), 1),
                        CB->layer_op(1, ST(0x194), 2),
                        1, "ar", 6, 0);
                if (ST(0x194) == 3)
                    CB->spawn_678(0x63,
                        CB->layer_op(1, ST(0x194), 0),
                        CB->layer_op(1, ST(0x194), 1),
                        CB->layer_op(1, ST(0x194), 2),
                        1, "ar", 6, 0);
                CB->op_9c(0x10, 3, 0);
                CB->named_object("Sar_50");
                CB->group(0);
                break;
            }
        case71_lcod:
            CB->group(3);
            if (ST(0x194) == 2) l02_action_register("LCOD666", 2, "SG02DIA02", NULL);
            if (ST(0x194) == 3) l02_action_register("LCOD667", 3, "SG03DIA02", NULL);
            CB->group(0);
            break;
        } else {
            if (ST(0x18) == 0x23) {
                CB->group(3);
                if (ST(0x194) == 2) l02_action_register("LCOD670", 2, "SG02DIA02", NULL);
                if (ST(0x194) == 3) l02_action_register("LCOD671", 3, "SG03DIA02", NULL);
                CB->group(0);
                break;
            }
            ST(0xF44) = 0;
            CB->group(3);
            CB->op_9c(0x10, 5, 0);
            if (ST(0x194) == 2)
                CB->spawn_678(2,
                    CB->layer_op(1, 2, 0), CB->layer_op(1, 2, 1),
                    CB->layer_op(1, 2, 2), 0, "ar", 1, 0);
            if (ST(0x194) == 3)
                CB->spawn_678(3,
                    CB->layer_op(1, 3, 0), CB->layer_op(1, 3, 1),
                    CB->layer_op(1, 3, 2), 0, "ar", 1, 0);
            CB->op_9c(0x10, 3, 0);
            if (ST(0x194) == 2) l02_action_register("LCOD668", 2, "SG02DIA01", NULL);
            if (ST(0x194) == 3) l02_action_register("LCOD669", 3, "SG03DIA01", NULL);
            CB->group(0);
            break;
        }
    }

    case 88: {  /* 0x54F1 — SPxxATT02 health decrement */
        e5_reset();
        if (ST(0x6D8) == 0) {
            if (ST(0x198) == 0) {
                ST(0x6D8) = 1;
                if (ST(0x194) == 1) {
                    i32 v = CB->layer_op(6, 1, -1);
                    if (v - 0x32 > 0x64) CB->layer_op(6, 1, v - 0x32);
                    CB->named_object("SP01ATT02");
                    CB->named_object("P01ATT03");
                    CB->named_object("ar_50");
                }
                if (ST(0x194) == 2) {
                    i32 v = CB->layer_op(6, 2, -1);
                    if (v - 0x1E > 0x32) CB->layer_op(6, 2, v - 0x1E);
                    CB->named_object("SP02ATT02");
                    CB->named_object("P02ATT03");
                    CB->named_object("ar_50");
                }
                if (ST(0x194) == 3) {
                    i32 v = CB->layer_op(6, 3, -1);
                    if (v - 0x14 > 0x32) CB->layer_op(6, 3, v - 0x14);
                    CB->named_object("SP02ATT02");
                    CB->named_object("P02ATT03");
                    CB->named_object("ar_50");
                }
            }
            if (ST(0x198) == 1) {
                if (ST(0x194) == 1) {
                    CB->named_object("SG00ATT01");
                    CB->named_object_int("LCOD25", 1);
                    CB->named_object("ar_50");
                }
                if (ST(0x194) == 2) {
                    CB->named_object("SG00ATT01");
                    CB->named_object_int("LCOD26", 2);
                    CB->named_object("ar_50");
                }
                if (ST(0x194) == 3) {
                    CB->named_object("SG00ATT01");
                    CB->named_object_int("LCOD27", 3);
                    CB->named_object("ar_50");
                }
            }
        } else {
            if (ST(0x194) == 1) CB->named_object_int("LCOD41", ST(0x194));
            if (ST(0x194) == 2) CB->named_object_int("LCOD42", ST(0x194));
            if (ST(0x194) == 3) CB->named_object_int("LCOD43", ST(0x194));
            CB->named_object("SG01ATT01");
            CB->named_object("ar_50");
        }
        CB->group(0);
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

    CB->state_toggle(2, 2);
    CB->state_toggle(1, 2);
    CB->state_toggle(0, 2);
    CB->state_toggle(0, 1);
    CB->named_two_ints(0, -1);
    CB->named_two_ints(1, -1);
    CB->named_two_ints(2, -1);
    CB->named_two_ints(33, -1);
    CB->named_two_ints(34, -1);
    CB->named_two_ints(35, -1);
    CB->named_two_ints(36, -1);
    CB->named_two_ints(37, -1);
    CB->named_two_ints(38, -1);
    CB->named_two_ints(39, -1);
    CB->named_two_ints(41, -1);

    if (ST(0x194) != 4 || ST(0xE8C) == 1) {
        l02_trig00();   /* 0x1001A0A9: call sub_10001073 -> sub_100073F0 */
    } else {
        CB->op_9c(16, 1, 0);
        ST(0x1A0) = 1;
        for (i32 layer = 0; layer < 4; layer++) {
            if (CB->layer_op(29, layer, 0))
                CB->layer_op(6, layer, 0);
        }
        if (!ST(0xB04))
            CB->cinematique("SC22_500");
        else
            CB->cinematique("SC23_500");
        CB->op_9c(27, 0, 0);
    }
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1, 0x1001A0FC): drain pending sound, run body.
 * ---------------------------------------------------------------------- */
static void tick(void) {
    if (ST(0x134) != 0) {
        CB->state_toggle(2, 1);
        CB->named_two_ints(ST(0x134) + 0x59, 1000);
        ST(0x134) = 0;
        CB->state_toggle(0, 1);
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
