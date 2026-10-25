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
    i32 a2 = CB->divers(1, ST(0x194), 2);
    i32 a1 = CB->divers(1, ST(0x194), 1);
    i32 a0 = CB->divers(1, ST(0x194), 0);
    CB->pos_perso(ST(0x194), a0, a1, a2, 1, "ar", 6, 0);
}

/* -------------------------------------------------------------------------
 * event6 — sub_10005BEC.  Switch on arg (0..4): LCOD zone triggers.
 * Fully ported.
 * ---------------------------------------------------------------------- */
static void event6(i32 arg) {
    switch (arg) {
    case 0:  /* loc_10005C0A */
        if (ST(0x194) == 2 && ST(0x10) == 0 && ST(0x938) == 0) {
            CB->select_pile(0, 1);
            ST(0x938) = 1;
            CB->bloque(3);
            l01_action_register("LCOD34", 2, "SG00CON01", NULL);
            CB->bloque(0);
        }
        break;
    case 1:  /* loc_10005C87 */
        if (ST(0x194) == 2 && ST(0x7C8) == 0) {
            CB->select_pile(0, 1);
            ST(0x7C8) = 1;
            ST(0x76C) = 0;
            CB->bloque(3);
            l01_action_register("LCOD35", 2, "SG00CON01", NULL);
            CB->animation("Sar_50");
            CB->divers_empile(0x2A, 0, 0);
            CB->animation("SG00CON01_1500");
            CB->animation("ar_50");
            CB->bloque(0);
        }
        break;
    case 2:  /* loc_10005D34 */
        if (ST(0x194) == 3 && ST(0x10) == 0 && ST(0x768) == 0) {
            CB->select_pile(0, 1);
            ST(0x768) = 1;
            CB->bloque(3);
            l01_action_register("LCOD36", 3, "SG00CON01", "SG00CON01");
            CB->animation("Sar_50");
            CB->bloque(0);
        }
        break;
    case 3:  /* loc_10005DBF */
        if (ST(0x194) == 3 && ST(0x76C) == 0) {
            CB->select_pile(0, 1);
            ST(0x7C8) = 0;
            ST(0x768) = 0;
            ST(0x76C) = 1;
            CB->bloque(3);
            l01_action_register("LCOD37", 3, "SG00CON01", NULL);
            CB->animation("Sar_50");
            CB->divers_empile(0x2A, 0, 0);
            CB->animation("Sar_50");
            CB->bloque(0);
        }
        break;
    case 4:  /* loc_10005E70 */
        if (ST(0x194) == 3 && ST(0x318) == 0) {
            CB->select_pile(2, 2);
            CB->select_pile(1, 2);
            CB->select_pile(0, 2);
            ST(0x318) = 1;
            CB->divers(0x23, 0, 0);
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
            CB->bloque(3);
            CB->animation("Sar_50");
            if (ST(0x10) == 1)
                CB->divers_empile(0x17, 0, 0);
            CB->divers_empile(0x10, 5, 0);
            CB->divers_empile(0x06, 3, 0);
            CB->animation("Sar_50");
            CB->cinematique("SC21_500");
            CB->divers_empile(0x10, 3, 0);
            CB->divers_empile(0x1B, 0, 0);
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
    if (ST(0x194) == 4 && ST(0x1A0) == 0) {
        ST(0x1A0) = 1;
        CB->evenement(0x2C, 0xBB8);
    }
    /* 0x8977 — intro cinematic replay */
    if (ST(0x3D0) == 1) {
        ST(0x3D0) = 0;
        CB->bloque(3);
        if (CB->divers(0x1D, 0, 0)) CB->divers(6, 0, 0);
        if (CB->divers(0x1D, 1, 0)) CB->divers(6, 1, 0);
        if (CB->divers(0x1D, 2, 0)) CB->divers(6, 2, 0);
        if (CB->divers(0x1D, 3, 0)) CB->divers(6, 3, 0);
        if (ST(0xB04) == 0) CB->cinematique("SC22_500");
        if (ST(0xB04) != 0) CB->cinematique("SC23_500");
        CB->divers_empile(0x1B, 0, 0);
    }
    /* 0x8A96 — level-0 arrival spawn */
    if (ST(0x328) == 1 && ST(0x194) == 0) {
        ST(0x328) = 0;
        CB->animation("SP12ARR01_10");
        CB->animation("SP00LEV01");
        CB->bruitage("L01B03", 0x27E, (i32)0xFFFFF40A);
        CB->animation("P00MAR25");
        CB->animation("ar");
        CB->pos_perso(0, 0x274, (i32)0xFFFFF47A, 0xFBC0, 1, "ar", 6, 0);
        CB->divers_empile(0x18, 1, 1);
        CB->change_camera(0x63, 1);
        CB->bloque(0);
        return;
    }
    /* 0x8B5B */
    if (ST(0x3A8) == 1) {
        ST(0x3A8) = 0;
        ST(0xBA0) = 1;
        CB->bloque(3);
        CB->dialogue("L01D01A", 0x1B);
        CB->dialogue("L01D01B", 0x1B);
        CB->bloque(0);
        return;
    }
    /* 0x8BC3 — section F: guarded by [1C0]==1 && [EC0]==0 */
    if (ST(0x1C0) == 1 && ST(0xEC0) == 0) {
        /* 0x8BE7 */
        if (ST(0x320) == 1 && ST(0x30) == 0 && ST(0x68) != 1) {
            ST(0x320) = 2;
            ST(0x68)  = 1;
            ST(0xB8)  = ST(0xF90);
            ST(0xBC)  = 1;
            ST(0x98)  = 1;
            ST(0x30)  = 1;
            CB->bloque(3);
            if (ST(0x708) == 0) CB->change_camera(6, 2);
            CB->animation("SP12LEV01");
            CB->animation("P12ARR03_25");
            CB->divers_empile(0x0C, 0x0C, 1);
            l01_action_register("L01D03", 0x0C, "P12ARR03", "SP12ARR03_100");
            CB->modify_var(&ST(0x30),  0);
            CB->modify_var(&ST(0x708), 1);
            CB->modify_var(&ST(0x88),  1);
            if (ST(0xB8) != 0 && ST(0xB8) != 6)
                CB->change_camera(ST(0xF90), 1);
            CB->bloque(0);
            return;
        }
        /* 0x8D67 */
        if (ST(0x88) == 1 && ST(0x30) == 0) {
            ST(0x88) = 2;
            CB->select_pile(1, 1);
            CB->divers_empile(0x0C, 0x0C, 0);
            CB->animation("P12WAA01");
            CB->animation("P12ARR03_10");
            CB->divers_empile(0x0C, 0x0C, 1);
            CB->modify_var(&ST(0x88), 0x29A);
            CB->select_pile(0, 1);
        }
        /* 0x8E05 */
        if (CB->distance(6, 4) > 0x190 && (i32)ST(0x320) > 0 && ST(0x74) != 1) {
            /* 0x8E41 */
            if (ST(0xF90) == 6 && ST(0xBC) == 1) {
                ST(0xBC) = 2;
                CB->change_camera(ST(0xF90), 1);
            }
            /* 0x8E8A */
            if (ST(0x68) == 1 && ST(0x34) == 0
                && CB->random(0xC8) == 1 && ST(0x88) == 0x29A) {
                CB->divers(0x0C, 0x0C, 0);
                ST(0x68) = 0;
                CB->select_pile(1, 1);
                CB->animation("SP12LEV03");
                CB->animation("P12ARR01_10");
                CB->select_pile(0, 1);
            }
            /* 0x8F22 */
            if (ST(0x324) != 0)
                ST(0x324) = 0;
        }
        /* 0x8F43 */
        if (CB->distance(6, 4) < 0x96 && (i32)ST(0x320) > 0
            && ST(0x68) == 0 && ST(0x34) == 0
            && CB->random(0x64) == 1 && ST(0x88) == 0x29A) {
            ST(0x68) = 1;
            CB->select_pile(1, 1);
            CB->animation("SP12LEV01");
            CB->animation("P12ARR03_10");
            CB->modify_var(&ST(0x324), 1);
            CB->select_pile(0, 1);
        }
        /* 0x900D */
        if (ST(0x324) == 1) {
            ST(0x324) = 2;
            CB->divers(0x0C, 0x0C, 1);
        }
    }
    /* 0x9040 — section G: guarded by [EC0]==0 && [B04]==0 */
    if (ST(0xEC0) == 0 && ST(0xB04) == 0) {
        /* 0x9064 */
        if (ST(0x78) == 1) {
            ST(0x78) = -1;
            CB->pos_perso(0x1A, 0x190, (i32)0xFFFFF6E6, 0x9470, 1,
                          "P26GAR03_10", -1, 0);
            CB->change_camera(4, 2);
            CB->bruitage("PORTE3", 0x200, (i32)0xFFFFF7D9);
            CB->dialogue("L01D45A", 0x1A);
            if (CB->divers(9, ST(0x194), 7))
                CB->change_camera(0, 2);
            /* 0x9106 */
            CB->joindre(ST(0x194), 0, 0, 0, 0x1A);
            if (CB->divers(9, ST(0x194), 4))
                CB->change_camera(0, 2);
            /* 0x914C */
            CB->animation("Sar");
            CB->joindre(-ST(0x194), 0x200, (i32)0xFFFFF58B, 0x859, -1);
            CB->dialogue("L01D99", 0);
            CB->change_camera(1, 2);
            l01_action_register("L01D45B", 0x1A, "SP26GAR03", "SG26FRA01");
            CB->evenement(0x27, 0x2EE);
            CB->animation("P26GAR03_10");
        }
        /* 0x91CE */
        if (ST(0x3BC) == 1) {
            ST(0x3BC) = 0;
            CB->select_pile(2, 2);
            CB->stop_joindre(-ST(0x194));
            CB->divers_empile(0x1C, ST(0x194), 0);
            if (ST(0x194) == 0) CB->animation("SG00ATT07");
            if (ST(0x194) == 1) CB->animation("SG01ATT07");
            CB->modify_var(&ST(0x78), 2);
            CB->select_pile(0, 1);
        }
        /* 0x9287 */
        if (ST(0x78) == 2) {
            ST(0x78)  = -1;
            ST(0x324) = 0;
            CB->divers(0x0C, 0x0C, 0);
            CB->animation("SP12ATT03");
            CB->change_camera(6, 2);
            CB->dialogue("L01D44", 0x0C);
            CB->animation("SP12ATT03_2000");
            CB->divers_empile(6, ST(0x194), 0);
            CB->modify_var(&ST(0x194), 4);
        }
    }
    /* 0x9328 */
    if (ST(0x34) == 0x57 && ST(0xEC4) == 0 && ST(0x2EC) == 2 && ST(0xEC0) == 1)
        ST(0x34) = 0;
    /* 0x9379 — door PORTE spawn */
    if (ST(0x2EC) == 2 && ST(0xEC4) != 0 && ST(0xD8) == 0 && ST(0x6F4) == 0) {
        ST(0x80)  = 0;
        ST(0xD8)  = 1;
        ST(0x6F4) = 1;
        CB->select_pile(2, 1);
        CB->ajoute_zone(0x10, 0x140, (i32)0xFFFFF611, 0x5A, 0x5A, 0);
        CB->select_pile(0, 1);
    }
    /* 0x9426 — imp W (0x57) appearance */
    if (CB->divers(0x12, 0x57, 0) && ST(0x2EC) != 2
        && CB->divers(0x1D, 0x57, 0)) {
        CB->select_pile(1, 2);
        CB->divers(0x23, 0, 0);
        CB->divers_empile(0x12, 0x57, 1);
        ST(0x30)  = 0;
        ST(0x34)  = 0;
        ST(0x2EC) = 2;
        CB->combat(0x57, -1);
        CB->stop_joindre((i32)0xFFFFFFA9);
        CB->animation("SP87IMP52");
        CB->animation("P87CAD");
        CB->pos_perso(0x57, 0, 0, 0, 2, "P87CAD", 9, 0);
        if (ST(0xEC4) == 1 && ST(0x2F0) == 1) {
            if (ST(0x94) == 1) {
                ST(0xF74) = 1;
                ST(0xDC)  = 0x0A;
                CB->select_pile(2, 2);
                CB->stop_joindre((i32)0xFFFFFFA8);
                CB->dialogue("L01D147", 0x58);
                CB->combat(0x58, 0);
                ST(0x34) = 0x58;
            }
            if (ST(0x94) == 0) {
                ST(0xF74) = 1;
                ST(0x94)  = 1;
                CB->select_pile(2, 2);
                CB->vire_col(1, 0);
                CB->animation("SP88LEV01");
                CB->animation("P88_65535");
                CB->dialogue("L01D147", 0x58);
                CB->vire_col(1, 1);
                CB->combat(0x58, 0);
                ST(0x34) = 0x58;
            }
        }
        CB->select_pile(0, 1);
    }
    /* 0x9652 — imp X (0x58) appearance */
    if (CB->divers(0x12, 0x58, 0) && ST(0x2F0) != 2 && ST(0x94) == 0) {
        ST(0xE0)  = 1;
        ST(0x30)  = 0;
        ST(0x34)  = 0x57;
        ST(0x2F0) = 2;
        CB->animation("SP88IMP99");
        CB->animation("P88CAD");
        CB->pos_perso(0x58, 0, 0, 0, 2, "P88CAD", 0x11, 0);
        if (ST(0x2EC) == 1) {
            ST(0xF04) = 1;
            CB->select_pile(1, 2);
            CB->dialogue("L01D148", 0x57);
            CB->combat(0x57, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x9784 — imp Y (0x59) appearance */
    if (CB->divers(0x12, 0x59, 0) && ST(0x2F4) != 2
        && CB->divers(0x1D, 0x59, 0)) {
        ST(0xE0)  = 1;
        ST(0x30)  = 0;
        ST(0x34)  = 0;
        ST(0x2F4) = 2;
        CB->select_pile(2, 2);
        CB->divers(0x23, 0, 0);
        CB->divers_empile(0x12, 0x59, 1);
        CB->animation("SP89IMP51");
        CB->animation("P89CAD");
        CB->pos_perso(0x59, 0, 0, 0, 2, "P89CAD", 0x12, 0);
        if (ST(0x2EC) == 1) {
            ST(0xF04) = 1;
            CB->select_pile(1, 2);
            CB->dialogue("L01D148", 0x57);
            CB->combat(0x57, 0);
            ST(0x34) = 0x57;
        }
        CB->select_pile(0, 1);
    }
    /* 0x98CA — imp X (0x58) variant */
    if (CB->divers(0x12, 0x58, 0) && ST(0x2F0) != 2 && ST(0x94) == 1) {
        ST(0xE0)  = 1;
        ST(0x30)  = 0;
        ST(0x2F0) = 2;
        CB->select_pile(2, 2);
        CB->divers(0x23, 0, 0);
        CB->divers_empile(0x12, 0x58, 1);
        CB->combat(0x58, -1);
        CB->stop_joindre((i32)0xFFFFFFA8);
        CB->animation("SP88IMP51");
        CB->animation("P88CAD");
        CB->pos_perso(0x58, 0, 0, 0, 2, "P88CAD", 0x11, 0);
        if (ST(0x2EC) == 1) {
            ST(0xF04) = 1;
            CB->select_pile(1, 2);
            CB->dialogue("L01D148", 0x57);
            CB->combat(0x57, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x9A0B */
    if (ST(0x2EC) == 1 && ST(0xEC4) == 0) {
        if (ST(0x7C) == 0 && CB->distance(6, 9) <= 0xFA)
            ST(0x7C) = 1;
        /* 0x9A65 */
        if (ST(0x7C) == 2 && ST(0xF90) != 2 && ST(0x34) == 0) {
            ST(0x7C) = 0;
            CB->select_pile(2, 1);
            CB->stop_joindre((i32)0xFFFFFFA9);
            if (ST(0x80) == 1) {
                ST(0x80) = 0;
                CB->bloque(3);
                CB->animation("SP87GAR27");
                CB->animation("P87ARR01_10");
                CB->bloque(0);
            }
            /* 0x9B06 */
            CB->joindre(0x57, 0, 0, 0x5DC0, -1);
            CB->animation("SP87ATT04");
            CB->select_pile(0, 1);
        }
    }
    /* 0x9B34 */
    if (ST(0x2EC) == 1 && ST(0xEC4) != 0 && ST(0x34) == 0 && ST(0x80) == 0) {
        if (ST(0x7C) == 0 && CB->distance(6, 9) <= 0xFA)
            ST(0x7C) = 1;
        /* 0x9BAF */
        if (ST(0x7C) == 2
            && (ST(0xF90) == 0 || ST(0xF90) == 6 || ST(0xF90) == 3)
            && CB->distance(6, 9) > 0x113) {
            ST(0x7C) = 0;
            CB->select_pile(2, 1);
            CB->stop_joindre((i32)0xFFFFFFA9);
            CB->pos_perso(0x57, 0x140, (i32)0xFFFFF61E, 0xDD4, 1,
                          "P87ATT01", 9, 1);
            if (ST(0xF0) == 0) {
                ST(0xF0) = 1;
                CB->link_objet(0x57, "OB94", 0x1B, 1);
            }
            CB->select_pile(0, 1);
        }
    }
    /* 0x9C9D — Sar dialogue driver (guarded) */
    if (ST(0x2EC) < 2 && ST(0x7C) == 1 && ST(0xF04) == 0 && ST(0x34) == 0) {
        ST(0x7C) = 2;
        if (ST(0x10) == 0) {
            CB->bloque(3);
            CB->stop_joindre((i32)0xFFFFFFA9);
            CB->animation("SP87ARR01_10");
            CB->joindre(0x57, 0, 0, 0, ST(0x194));
            if (ST(0x84) == 0) {
                ST(0x84) = 1;
                if (ST(0x194) == 1 && ST(0xCC8) == 0) {
                    CB->joindre(-ST(0x194), 0, 0, 0, 0x57);
                    CB->animation("sar_10");
                    ST(0xCC8) = 1;
                    CB->dialogue("L01D66A", 1);
                    l01_action_register("L01D67", 0x57, "SP87ATT70", "SP87ARR01_10");
                }
                /* 0x9DDD */
                if (ST(0x194) >= 2) {
                    CB->joindre(-ST(0x194), 0, 0, 0, 0x57);
                    CB->animation("sar_10");
                    if (ST(0x8B4) == 0) {
                        ST(0x8B4) = 1;
                        if (ST(0x194) == 2) {
                            if (ST(0x198) == 1)
                                l01_action_register("L01D60A", 2, "SG02DIA03", NULL);
                            if (ST(0x198) == 0)
                                l01_action_register("L01D60B", 2, "SG02DIA03", NULL);
                        }
                        if (ST(0x194) == 3) {
                            if (ST(0x198) == 1)
                                l01_action_register("L01D61A", 3, "SG03DIA03", NULL);
                            if (ST(0x198) == 0)
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
                CB->joindre_perm(0x57, ST(0x194));
                CB->bloque(0);
            } else {
                /* 0x9F7A */
                CB->animation("SP87ARR01_10");
                CB->joindre_perm(0x57, ST(0x194));
                CB->bloque(0);
            }
        }
        /* 0x9FA3 */
        if (ST(0x10) == 1) {
            CB->bloque(3);
            ST(0x80) = 1;
            if (ST(0xF0) == 1) {
                ST(0xF0) = 0;
                CB->link_objet(0x57, "OB94", 0x1B, 0);
            }
            if (ST(0x84) == 0) {
                ST(0x84) = 1;
                if (ST(0x194) == 1) { ST(0xCC8) = 1; CB->dialogue("L01D66A", 1); }
                if (ST(0x194) == 2) { ST(0x8B4) = 1; CB->dialogue("L01D66B", 2); }
                if (ST(0x194) == 3) { ST(0x8B4) = 1; CB->dialogue("L01D66C", 3); }
                CB->animation("SP87ARR01_10");
                CB->joindre((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
                CB->animation("SP87GAR26");
                CB->animation("P87GAR02_10");
                if ((i32)ST(0x14) >= 1) {
                    ST(0x34) = 0x57;
                    CB->evenement(2, 0x64);
                }
                if (ST(0x14) == 0) {
                    if (ST(0xC84) == 0) {
                        ST(0xC84) = 1;
                        if (ST(0x194) == 1)
                            l01_action_register("L01D81", 0x57, "SP87DIA01", "SP87GAR02_10");
                        if (ST(0x194) >= 2)
                            l01_action_register("L01D82", 0x57, "SP87DIA01", "SP87GAR02_10");
                    }
                    CB->joindre_perm(0x57, ST(0x194));
                    CB->bloque(0);
                }
                /* [14]!=0 falls through to A223 */
            } else {
                /* 0xA1C7 */
                CB->animation("SP87ARR01_10");
                CB->joindre((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
                CB->animation("SP87GAR26");
                CB->animation("P87GAR02_10");
                CB->joindre_perm(0x57, ST(0x194));
                CB->bloque(0);
            }
        }
    }
    /* 0xA223 */
    if (ST(0x2EC) < 2 && ST(0x7C) == 1 && ST(0xF04) == 1 && ST(0x34) == 0) {
        CB->bloque(3);
        ST(0x34) = 0x57;
        ST(0x7C) = 0x0A;
        ST(0x80) = 1;
        if (ST(0xF0) == 1) { ST(0xF0) = 0; CB->link_objet(0x57, "OB94", 0x1B, 0); }
        CB->animation("SP87ARR01_10");
        CB->joindre((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
        CB->animation("SP87GAR26");
        l01_action_register("L01D87", 0x57, "SP87DIA01", "SP87GAR02_10");
        CB->bloque(0);
        if (ST(0xEC4) == 1 && ST(0x2F0) == 1) {
            if (ST(0x94) != 1) {
                ST(0x94) = 1;
                CB->animation("SP88LEV01");
                CB->animation("P88_65535");
            }
            CB->combat(0x58, 0);
        }
        CB->combat(0x57, 0);
    }
    /* 0xA395 */
    if (ST(0x2EC) < 2 && ST(0x7C) == 2 && ST(0x34) == 0) {
        if (ST(0x80) == 0 && ST(0x10) == 1) {
            ST(0x80) = 1;
            CB->bloque(3);
            CB->stop_joindre((i32)0xFFFFFFA9);
            if (ST(0xF0) == 1) { ST(0xF0) = 0; CB->link_objet(0x57, "OB94", 0x1B, 0); }
            CB->joindre((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
            CB->animation("SP87ARR01_10");
            CB->animation("P87GAR26");
            if (ST(0xC84) == 0) {
                ST(0xC84) = 1;
                if (ST(0x194) == 1)
                    l01_action_register("L01D81", 0x57, "P87DIA01", "SP87GAR02_10");
                if (ST(0x194) >= 2)
                    l01_action_register("L01D82", 0x57, "P87DIA01", "SP87GAR02_10");
            }
            CB->joindre_perm(0x57, ST(0x194));
            CB->bloque(0);
            return;
        }
        /* 0xA50D */
        if (ST(0x80) == 1 && ST(0x10) == 0) {
            ST(0x80) = 0;
            CB->bloque(3);
            if (ST(0xF90) == 0 || ST(0xF90) == 3 || ST(0xF90) == 6) {
                CB->dialogue("L01D80", 0x57);
                CB->bloque(0);
                return;
            } else {
                CB->stop_joindre((i32)0xFFFFFFA9);
                CB->animation("SP87GAR27");
                CB->animation("P87ARR01_200");
                if (ST(0xC80) == 0 && ST(0x194) >= 2) {
                    ST(0xC80) = 1;
                    l01_action_register("L01D80", 0x57, "P87DIA59", "SP87ARR01_10");
                }
                CB->modify_var(&ST(0x30), 0);
                CB->joindre_perm(0x57, ST(0x194));
                CB->bloque(0);
                return;
            }
        }
    }
    /* 0xA672 */
    if (ST(0x328) == 1 && ST(0x34) == 0x57) {
        ST(0x328) = 2;
        ST(0x8C8) = 1;
        ST(0x34)  = 0;
        CB->joindre((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
        if (ST(0x194) == 1)
            l01_action_register("L01D137", 0x57, "SP87DIA01", "SP87GAR02_10");
        if (ST(0x194) >= 2)
            l01_action_register("L01D138A", 0x57, "SP87DIA01", "SP87GAR02_10");
        CB->modify_var(&ST(0x30), 0);
        CB->animation("Sar_10");
        CB->joindre_perm(0x57, ST(0x194));
        CB->bloque(0);
        return;
    }
    /* 0xA76F — imp W distance-gated dialogue/attack */
    if (ST(0x2F0) == 1 && ST(0x34) == 0 && CB->divers(0x1D, 0x58, 0)) {
        /* 0xA7AA — sub-block, else falls to B03A */
        if (ST(0x2EC) < 2 && ST(0x194) != 4
            && CB->divers(1, ST(0x194), 1) <= (i32)0xFFFFF510
            && CB->divers(1, ST(0x194), 0) >= 0x1F4
            && ST(0x98) == 0) {
            if (ST(0xCCC) == 0) {
                CB->bloque(3);
                ST(0x30)  = 1;
                ST(0xCCC) = 1;
                ST(0x98)  = 1;
                CB->change_camera(6, 2);
                ST(0x7C) = 0;
                CB->pos_perso(0x57, 0x140, (i32)0xFFFFF61E, 0x8CA0, 1, "P87ARR01", 9, 1);
                if (ST(0x2EC) == 1 && ST(0x8B4) == 0) {
                    ST(0x8B4) = 1;
                    ST(0x94)  = 1;
                    CB->vire_col(1, 0);
                    CB->animation("SP88LEV01");
                    CB->animation("P88_65535");
                    CB->joindre((i32)0xFFFFFFA8, 0, 0, 0, ST(0x194));
                    l01_action_register("L01D121", 0x58, "SP88DIA59", "SP88ARR01");
                    CB->change_camera(2, 2);
                    CB->joindre((i32)0xFFFFFFA9, 0, 0, 0, 0x58);
                    if (ST(0x80) == 0)
                        l01_action_register("L01D31A", 0x57, "SP87DIA55", "SP87ARR01_100");
                    if (ST(0x80) != 0) {
                        ST(0x80) = 0;
                        CB->stop_joindre((i32)0xFFFFFFA9);
                        CB->animation("SP87GAR27");
                        CB->animation("P87ARR01_200");
                        l01_action_register("L01D31A", 0x57, "P87DIA55", "SP87ARR01_100");
                    }
                    CB->joindre(0x57, 0, 0, 0, ST(0x194));
                    l01_action_register("L01D30A", 0x57, "SP87DIA50", "SP87ARR01");
                    l01_action_register("L01D30B", 0x57, "SP87DIA56", "SP87ARR01");
                    CB->joindre((i32)0xFFFFFFA9, 0, 0, 0, 0x58);
                    l01_action_register("L01D31B", 0x57, "SP87DIA55", "SP87ARR01_100");
                    l01_action_register("L01D31C", 0x57, "SP87DIA59", "SP87ARR01_100");
                    CB->change_camera(6, 2);
                    CB->pos_perso(0x57, 0x140, (i32)0xFFFFF61E, 0xDD4, 1, "P87ATT01", 9, 1);
                    if (ST(0xE0) == 0) {
                        CB->link_objet(0x57, "OB94", 0x1B, 1);
                        ST(0xF0) = 1;
                    }
                    if (ST(0x10) == 0)
                        l01_action_register("L01D122", 0x58, "SP88DIA60", "SP88ARR01_100");
                    if (ST(0x10) == 1)
                        l01_action_register("L01D123", 0x58, "SP88DIA60", "SP88GAR01_500");
                    ST(0x84) = 1;
                    CB->modify_var(&ST(0x30), 0);
                    CB->joindre_perm(0x58, ST(0x194));
                    CB->bloque(0);
                    return;
                }
                /* 0xAB83 */
                if (ST(0x2EC) == 1 && ST(0x8B4) == 1) {
                    ST(0x94) = 1;
                    CB->vire_col(1, 0);
                    CB->animation("SP88LEV01");
                    CB->animation("P88_65535");
                    CB->joindre((i32)0xFFFFFFA8, 0, 0, 0, ST(0x194));
                    l01_action_register("L01D124", 0x58, "SP88DIA59", "SP88ARR01_10");
                    CB->vire_col(1, 1);
                    CB->change_camera(2, 2);
                    CB->joindre((i32)0xFFFFFFA9, 0, 0, 0, 0x58);
                    if (ST(0x80) == 0 && ST(0x10) == 0)
                        l01_action_register("L01D31B", 0x57, "SP87DIA55", "SP87ARR01_100");
                    /* 0xAC7C — [80]/[10] gated L01D171 */
                    {
                        i32 do171;
                        if (ST(0x80) != 0) {
                            ST(0x80) = 0;
                            CB->stop_joindre((i32)0xFFFFFFA9);
                            CB->animation("SP87GAR27");
                            CB->animation("P87ARR01_200");
                            do171 = 1;
                        } else {
                            do171 = (ST(0x10) == 1) ? 1 : 0;
                        }
                        if (do171)
                            l01_action_register("L01D171", 0x57, "P87DIA55", "SP87ARR01_100");
                    }
                    l01_action_register("L01D31C", 0x57, "SP87DIA59", "SP87ARR01_100");
                    CB->change_camera(6, 2);
                    if (ST(0x10) == 0)
                        l01_action_register("L01D122", 0x58, "SP88DIA60", "SP88ARR01_100");
                    if (ST(0x10) == 1)
                        l01_action_register("L01D123", 0x58, "SP88DIA60", "SP88GAR01_500");
                    CB->modify_var(&ST(0x30), 0);
                    CB->joindre_perm(0x58, ST(0x194));
                    CB->bloque(0);
                    return;
                }
            }
            /* 0xADB7 */
            if (ST(0xCCC) == 1 && ST(0x30) == 0 && ST(0x34) == 0) {
                ST(0x98) = 1;
                CB->bloque(3);
                ST(0x30) = 1;
                ST(0x7C) = 0;
                ST(0x80) = 0;
                CB->change_camera(6, 2);
                CB->pos_perso(0x57, 0x140, (i32)0xFFFFF61E, 0x8CA0, 1, "P87ARR01", 9, 1);
                if (ST(0x94) == 0) {
                    ST(0x94) = 1;
                    CB->vire_col(1, 0);
                    CB->animation("SP88LEV01");
                    CB->animation("P88_65535");
                    CB->joindre((i32)0xFFFFFFA8, 0, 0, 0, ST(0x194));
                }
                if (ST(0x10) == 0)
                    l01_action_register("L01D94", 0x58, "SP88DIA54", "SP88ARR01_10");
                if (ST(0x10) == 1)
                    l01_action_register("L01D95", 0x58, "SP88DIA51", "SP88ARR01_10");
                CB->vire_col(1, 1);
                CB->change_camera(1, 2);
                if (ST(0x80) == 0 && ST(0x10) == 0)
                    l01_action_register("L01D96", 0x57, "SP87DIA61", "SP87ARR01_10");
                /* 0xAF68 — [80]/[10] gated L01D172 */
                {
                    i32 do172;
                    if (ST(0x80) != 0) {
                        ST(0x80) = 0;
                        CB->stop_joindre((i32)0xFFFFFFA9);
                        CB->animation("SP87GAR27");
                        CB->animation("P87ARR01_200");
                        do172 = 1;
                    } else {
                        do172 = (ST(0x10) == 1) ? 1 : 0;
                    }
                    if (do172)
                        l01_action_register("L01D172", 0x57, "SP87DIA61", "SP87ARR01_10");
                }
                CB->change_camera(6, 2);
                CB->modify_var(&ST(0x30), 0);
                CB->joindre_perm(0x58, ST(0x194));
                CB->bloque(0);
                return;
            }
        }
        /* 0xB03A */
        if (ST(0x2EC) >= 2 && ST(0x34) == 0 && ST(0x194) != 4
            && CB->divers(1, ST(0x194), 1) < (i32)0xFFFFF63C
            && CB->divers(1, ST(0x194), 0) >= 0x1F4) {
            CB->bloque(3);
            ST(0x30) = 1;
            ST(0x94) = 1;
            ST(0x34) = 0x57;
            CB->change_camera(6, 2);
            CB->vire_col(1, 0);
            CB->animation("SP88LEV01");
            CB->animation("P88_65535");
            CB->joindre((i32)0xFFFFFFA8, 0, 0, 0, ST(0x194));
            l01_action_register("L01D127", 0x58, "SP88DIA60", "SP88GAR01_500");
            CB->modify_var(&ST(0x30), 0);
            CB->vire_col(1, 1);
            CB->change_camera(0, 1);
            CB->bloque(0);
            CB->combat(0x58, 0);
            return;
        }
    }
    /* 0xB196 */
    if (ST(0x2F0) == 2 && ST(0x34) != 0 && ST(0x2EC) >= 2)
        ST(0x34) = 0;
    /* 0xB1D5 */
    if (ST(0x2F0) == 2 && ST(0xF74) == 1)
        ST(0xF74) = 0;
    /* 0xB208 */
    if (ST(0x2F0) < 2 && ST(0x34) == 0 && CB->distance(6, 0x11) > 0x12C
        && (ST(0xF90) == 2 || ST(0xF90) == 3) && ST(0x94) == 1) {
        CB->stop_joindre((i32)0xFFFFFFA8);
        ST(0xDC) = 0x0A;
        ST(0x94) = 0;
        CB->pos_perso(0x58, 0x242, (i32)0xFFFFF43E, 0x1388, 1, "P88ARR02", 0x11, 1);
    }
    /* 0xB2C8 */
    if (ST(0xF90) == 6 && CB->distance(6, 4) > 0xC8 && CB->distance(6, 7) > 0xC8
        && ST(0x98) != 0 && ST(0xEC0) == 0 && CB->divers(0x1D, 0x0C, 0)) {
        CB->change_camera(0x63, 1);
        ST(0x98) = 0;
    }
    /* 0xB36D */
    if (ST(0xF90) == 6 && CB->distance(6, 7) > 0x12C && ST(0x98) != 0
        && ST(0xEC0) == 1 && ST(0xEC4) == 0) {
        CB->change_camera(0x63, 1);
        ST(0x98) = 0;
    }
    /* 0xB3EF */
    if (ST(0xF90) == 6 && CB->distance(6, 0x11) > 0xC8 && ST(0x98) != 0
        && ST(0xEC0) == 1 && ST(0xEC4) == 1 && CB->divers(0x1D, 0x58, 0)) {
        CB->change_camera(0x63, 1);
        ST(0x98) = 0;
    }
    /* 0xB48B */
    if (ST(0xF90) == 6 && CB->distance(6, 0x12) > 0xC8 && ST(0x98) != 0
        && ST(0xEC0) == 1 && ST(0xEC4) == 2 && CB->divers(0x1D, 0x59, 0)) {
        CB->change_camera(0x63, 1);
        ST(0x98) = 0;
    }
    /* 0xB527 */
    if (ST(0xDC) == 1) {
        ST(0xDC) = 2;
        CB->joindre_perm(0x58, ST(0x194));
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

    case 17: if (ST(0x364) == 1) ST(0x364) = 2; break;

    case 18:
        if ((i32)ST(0xF5C) > 0) {
            ST(0xF5C) -= 0x1388;
            CB->divers_empile(0x29, 0, 0);
            CB->evenement(0x12, 0x1388);
        }
        break;

    case 19:
        if (ST(0xEBC) == 1 && ST(0x18) != 0x1C)
            ST(0xEBC) = 2;
        break;

    case 20:
        if (ST(0x18) == 0x12)
            CB->bruitage("L08B06", 0xAB4, 0x7BF);
        if (ST(0x18) == 8) {
            CB->select_pile(2, 1);
            CB->bruitage("L08B06", 0x2C, (i32)0xFFFFFC40);
            CB->select_pile(0, 1);
        }
        break;

    case 21: if (ST(0x374) == 0) ST(0x374) = 1; break;
    case 22: if (ST(0x378) == 0) ST(0x378) = 1; break;
    case 25: if (ST(0x384) == 0) ST(0x384) = 2; break;
    case 27: if (ST(0x38C) == 0) ST(0x38C) = 1; break;

    case 28:
        if (ST(0x390) == 0) {
            CB->bloque(0);
            ST(0x18) = 0x17;
            CB->change_piece(0x19, -1);
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
            CB->select_pile(2, 1);
            CB->divers_empile(0x18, ST(0xC54), 4);
            CB->evenement(0x2B, 0x1388);
            CB->select_pile(0, 1);
        }
        break;

    case 43:
        CB->select_pile(2, 1);
        CB->divers_empile(0x18, ST(0xC54), 1);
        CB->evenement(0x2A, 0xAFC8);
        CB->select_pile(0, 1);
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
        CB->bloque(3);
        if (CB->divers(2, 9, 2))
            CB->animation("SP02PIPI2");
        else
            CB->animation("SP02PIPI1");
        CB->animation("ar_50");
        CB->bloque(0);
        CB->select_pile(0, 1);
        break;

    case 50: {  /* SOB24%c/SOB77%c via named_ex_988 */
        CB->divers(0x0A, 0x7D0, 0x7D0);
        CB->animation("SP01ACT96");
        CB->animation("ar");
        c8 buf1[48], buf2[48];
        snprintf(buf1, sizeof(buf1), "SOB24%c", (char)(ST(0x128) + 0x41));
        CB->etat_objet(buf1, -1, -1, -1, -1, -1, -1, -2, 0);
        snprintf(buf2, sizeof(buf2), "SOB77%c", (char)(ST(0x128) + 0x41));
        CB->etat_objet(buf2, -1, -1, -1, -1, -1, -1, -3, 0);
        CB->bloque(0);
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
            CB->divers(0x26, 0x66, 0x10011);
            CB->evenement(0x5A, 0x64);
            g_float_accum[0] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case 91:  /* float accum grow (index 1, dual layer_op) */
        CB->select_pile(2, 1);
        if (g_float_accum[1] < 3.0f && g_float_active != 0) {
            CB->divers(0x26, 0x66, 0x10005);
            CB->divers(0x26, 0x66, 0x10007);
            CB->evenement(0x5B, 0x64);
            g_float_accum[1] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case 92:  /* float accum grow (index 2, dual layer_op) */
        CB->select_pile(2, 1);
        if (g_float_accum[2] < 3.0f && g_float_active != 0) {
            CB->divers(0x26, 0x66, 0x1000E);
            CB->divers(0x26, 0x66, 0x10010);
            CB->evenement(0x5C, 0x64);
            g_float_accum[2] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case 93:  /* float accum decay (index 3) */
        CB->select_pile(2, 1);
        if (g_float_accum[3] > 0.5f && g_float_active != 0) {
            CB->divers(0x25, 0x62, 0);
            CB->evenement(0x5D, 0x64);
            g_float_accum[3] *= 0.98f;
        }
        CB->select_pile(0, 1);
        break;
    case 98: {  /* float reset + respawn toggle */
        CB->select_pile(2, 1);
        CB->bloque(3);
        if (g_float_active != 0) {
            g_float_active = 0;
            e4_reset_floats();
            i32 c2 = CB->divers(1, ST(0x194), 2);
            i32 c1 = CB->divers(1, ST(0x194), 1);
            i32 c0 = CB->divers(1, ST(0x194), 0);
            CB->pos_perso(ST(0x194) + 100, c0, c1, c2, 1,
                          "ar", ST(0x138), 0);
            CB->animation("Sar_50");
            CB->evenement(0x62, 0x64);
            CB->bloque(0);
            CB->select_pile(0, 1);
        } else {
            g_float_active = 1;
            CB->bloque(0);
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
    CB->divers_empile(0x30, 0, 0);
    CB->divers_empile(0x30, 0, 0);
    CB->divers_empile(0x30, 0, 0);
    CB->bloque(3);
}

/* case 4 / 22 / 65 template: SG01ATT01 + LCOD41/42/43 + ar_50 */
static void e5_att01(void) {
    e5_reset();
    CB->animation("SG01ATT01");
    if (ST(0x194) == 1) CB->dialogue("LCOD41", ST(0x194));
    if (ST(0x194) == 2) CB->dialogue("LCOD42", ST(0x194));
    if (ST(0x194) == 3) CB->dialogue("LCOD43", ST(0x194));
    CB->animation("ar_50");
    CB->bloque(0);
}

/* Common tail: layer-6 value += k, clamped to 0x3FF (op 6 on [0x194]). */
static void e5_clamp6(i32 k) {
    if (CB->divers(6, ST(0x194), -1) + k < 0x400)
        CB->divers_empile(6, ST(0x194), CB->divers(6, ST(0x194), -1) + k);
    else
        CB->divers_empile(6, ST(0x194), 0x3FF);
}

/* SGxxMAN03/04 (levels 2,3) by [0x198], + ar_50 + clamp(k). */
static void e5_man34(i32 k) {
    if (ST(0x194) == 2) {
        if (ST(0x198) == 1) CB->animation("SG02MAN03");
        if (ST(0x198) == 0) CB->animation("SG02MAN04");
    }
    if (ST(0x194) == 3) {
        if (ST(0x198) == 1) CB->animation("SG03MAN03");
        if (ST(0x198) == 0) CB->animation("SG03MAN04");
    }
    CB->animation("ar_50");
    e5_clamp6(k);
}

/* SG01ATT01 + LCOD pair (lvl2->a, lvl3->b) + ar_50. */
static void e5_att01_lcod(const c8 *a, const c8 *b) {
    CB->animation("SG01ATT01");
    if (ST(0x194) == 2) CB->dialogue(a, ST(0x194));
    if (ST(0x194) == 3) CB->dialogue(b, ST(0x194));
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
        if (ST(0x18) == 0x23) {
            e5_reset();
            CB->animation("SG00ATT01");
            CB->bloque(0);
        }
        break;

    case 42:  /* 0x250D */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(0x194) == 1) CB->dialogue("LCOD41", ST(0x194));
        if (ST(0x194) == 2) CB->dialogue("LCOD42", ST(0x194));
        if (ST(0x194) == 3) CB->dialogue("LCOD43", ST(0x194));
        CB->animation("ar_50");
        CB->bloque(0);
        break;

    case 49:  /* 0x25ED — Sar imp interaction (LCOD02/03 A-H) */
        if (ST(0xEA8) == 0 || ST(0xEA8) == 1) {
            e5_reset();
            if (ST(0x194) == 2) {
                l01_action_register("LCOD02A", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02B", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02C", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02D", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02E", 2, "SP02ACT64", "SP02ACT64");
                l01_action_register("LCOD02F", 2, "SP02ACT64", "SP02ACT64");
                if (ST(0xEA8) == 0) {
                    l01_action_register("LCOD02G", 2, "SP02ACT64", "SP02ACT64");
                    l01_action_register("LCOD02H", 2, "SP02ACT64", "SP02ACT64");
                }
                ST(0xEA8) = 1;
            }
            if (ST(0x194) == 3) {
                l01_action_register("LCOD03A", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03B", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03C", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03D", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03E", 3, "SP03ACT64", "SP03ACT64");
                l01_action_register("LCOD03F", 3, "SP03ACT64", "SP03ACT64");
                if (ST(0xEA8) == 0) {
                    l01_action_register("LCOD03G", 3, "SP03ACT64", "SP03ACT64");
                    l01_action_register("LCOD03H", 3, "SP03ACT64", "SP03ACT64");
                }
                ST(0xEA8) = 1;
            }
            if (ST(0x18) == 2 && ST(0xF90) == 1 && ST(0xEB8) == 1
                && CB->divers(0x1D, 0x1A, 0)) {
                CB->divers(0x17, 0, 0);
                if (CB->distance(0x14, 0x1F) < 0x96)
                    CB->joindre(-ST(0x194), 0, 0, 0, 0x1A);
                else
                    CB->joindre(-ST(0x194), -1, -1, -1, 0x1A);
                if (CB->distance(0x14, 0x1F) >= 0x96)
                    CB->evenement(0x29, 0x7D0);
                else if (CB->distance(0x14, 0x1F) < 0x96 && CB->direction_perso(0x1A) == 0)
                    CB->evenement(0x29, 0x7D0);
                else
                    CB->evenement(0x29, 0x1F4);
            } else {
                CB->animation("Sar_50");
                CB->bloque(0);
            }
        } else {
            CB->animation("Sar_50");
            CB->bloque(0);
        }
        break;

    case 50:  /* 0x29A5 */
        e5_reset();
        if (ST(0x194) == 2) l01_action_register("LCOD88", 2, "SG00DIA02", NULL);
        if (ST(0x194) == 3) l01_action_register("LCOD89", 3, "SG00DIA02", NULL);
        CB->bloque(0);
        break;

    case 46:  /* 0x2A47 — LCOD100/101/102/103/104 */
        e5_reset();
        if (ST(0x194) == 2) {
            l01_action_register("LCOD100A", 2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD100B", 2);
            CB->dialogue("LCOD100C", 2);
            CB->dialogue("LCOD100D", 2);
        }
        if (ST(0x194) == 3) {
            l01_action_register("LCOD101A", 3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD101B", 3);
            CB->dialogue("LCOD101C", 3);
            CB->dialogue("LCOD101D", 3);
        }
        CB->dialogue("LCOD102A", 2);
        CB->dialogue("LCOD102B", 2);
        CB->dialogue("LCOD102C", 2);
        CB->dialogue("LCOD102D", 2);
        CB->dialogue("LCOD102E", 2);
        CB->dialogue("LCOD102F", 2);
        if (ST(0x194) == 2) l01_action_register("LCOD103", 2, "SG02DIA01", NULL);
        if (ST(0x194) == 3) l01_action_register("LCOD104", 3, "SG03DIA01", NULL);
        CB->animation("Sar_50");
        CB->bloque(0);
        break;

    case 70:  /* 0x2CEA */
        e5_reset();
        if (ST(0x194) == 2) l01_action_register("LCOD90", 2, "SG00DIA02", NULL);
        if (ST(0x194) == 3) l01_action_register("LCOD91", 3, "SG00DIA02", NULL);
        CB->bloque(0);
        break;

    case 13:  /* 0x39E1 — Sar mannequin dialogue + layer clamp */
        e5_reset();
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) CB->animation("SG02MAN01");
            if (ST(0x198) == 0) CB->animation("SG02MAN02");
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) CB->animation("SG03MAN01");
            if (ST(0x198) == 0) CB->animation("SG03MAN02");
        }
        CB->animation("ar_50");
        e5_clamp6(0x64);
        CB->bloque(0);
        break;

    case 77:  /* 0x3575 — SPxxACT17 */
        e5_reset();
        if (ST(0x194) == 1) CB->animation("SP01ACT17");
        if (ST(0x194) == 2) CB->animation("SP02ACT17");
        if (ST(0x194) == 3) CB->animation("SP03ACT17");
        CB->animation("ar_50");
        if (ST(0x194) == 1) l01_action_register("LCOD41", ST(0x194), "SG01ATT01", NULL);
        if (ST(0x194) == 2) l01_action_register("LCOD42", ST(0x194), "SG01ATT01", NULL);
        if (ST(0x194) == 3) l01_action_register("LCOD43", ST(0x194), "SG01ATT01", NULL);
        CB->bloque(0);
        break;

    case 81:  /* 0x36B3 — LCOD23/24 A-G */
        e5_reset();
        if (ST(0xEAC) == 0) ST(0xEAC) = 1;
        if (ST(0x18) == 0x1C) ST(0x84) = 1;
        if (ST(0x194) == 2) {
            l01_action_register("LCOD23A", 2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD23B", 2);
            CB->dialogue("LCOD23C", 2);
            CB->dialogue("LCOD23D", 2);
            CB->dialogue("LCOD23E", 2);
            CB->dialogue("LCOD23F", 2);
            l01_action_register("LCOD23G", 2, "SG02DIA01", NULL);
        }
        if (ST(0x194) == 3) {
            l01_action_register("LCOD24A", 3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD24B", 3);
            CB->dialogue("LCOD24C", 3);
            CB->dialogue("LCOD24D", 3);
            CB->dialogue("LCOD24E", 3);
            CB->dialogue("LCOD24F", 3);
            l01_action_register("LCOD24G", 3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->bloque(0);
        break;

    case 84:  /* 0x3871 */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(0x194) == 2) CB->dialogue("LCOD672", ST(0x194));
        if (ST(0x194) == 3) CB->dialogue("LCOD673", ST(0x194));
        CB->animation("ar_50");
        CB->bloque(0);
        break;

    case 85:  /* 0x3929 */
        e5_reset();
        CB->animation("SG01ATT01");
        if (ST(0x194) == 2) CB->dialogue("LCOD674", ST(0x194));
        if (ST(0x194) == 3) CB->dialogue("LCOD675", ST(0x194));
        CB->animation("ar_50");
        CB->bloque(0);
        break;

    case 34:  /* 0x44E2 — SGxxMAN01/02 + clamp 0x4B */
        e5_reset();
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) CB->animation("SG02MAN01");
            if (ST(0x198) == 0) CB->animation("SG02MAN02");
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) CB->animation("SG03MAN01");
            if (ST(0x198) == 0) CB->animation("SG03MAN02");
        }
        CB->animation("ar_50");
        e5_clamp6(0x4B);
        CB->bloque(0);
        break;

    case 36:  /* 0x464F — SGxxMAN03/04 + clamp 0x4B */
        e5_reset(); e5_man34(0x4B); CB->bloque(0);
        break;

    case 43:  /* 0x47BC — SGxxMAN03/04 + clamp 0x64 */
        e5_reset(); e5_man34(0x64); CB->bloque(0);
        break;

    case 56:  /* 0x4B32 — MAN03/04 + clamp 0x4B */
        e5_reset(); e5_man34(0x4B); CB->bloque(0);
        break;

    case 64:  /* 0x4C9F — MAN03/04 + clamp 0x64 */
        e5_reset(); e5_man34(0x64); CB->bloque(0);
        break;

    case 95:  /* 0x4E0C — MAN03/04 + clamp 0x96 */
        e5_reset(); e5_man34(0x96); CB->bloque(0);
        break;

    case 44:  /* 0x4929 — SGxxMAN (levels 0-3) + clamp 0x96 */
        e5_reset();
        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) CB->animation("SG00MAN01");
            if (ST(0x198) == 0) CB->animation("SG00MAN02");
        }
        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) CB->animation("SG01MAN01");
            if (ST(0x198) == 0) CB->animation("SG01MAN02");
        }
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) CB->animation("SG02MAN03");
            if (ST(0x198) == 0) CB->animation("SG02MAN04");
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) CB->animation("SG03MAN03");
            if (ST(0x198) == 0) CB->animation("SG03MAN04");
        }
        CB->animation("ar_50");
        e5_clamp6(0x96);
        CB->bloque(0);
        break;

    case 1:   /* 0x5161 — SG01ATT01 + LCOD26/27 */
        e5_reset(); e5_att01_lcod("LCOD26", "LCOD27"); CB->bloque(0);
        break;

    case 2:   /* 0x5219 (shared 2/12/18/19) — SG01ATT01 + L25D79/82 */
    case 12:
    case 18:
    case 19:
        e5_reset(); e5_att01_lcod("L25D79", "L25D82"); CB->bloque(0);
        break;

    case 3:   /* 0x52D1 — state_toggle(1,1) + SG01ATT01 + LCOD39/40 */
        CB->select_pile(1, 1);
        CB->divers_empile(0x30, 0, 0);
        CB->divers_empile(0x30, 0, 0);
        CB->divers_empile(0x30, 0, 0);
        CB->bloque(3);
        e5_att01_lcod("LCOD39", "LCOD40");
        CB->bloque(0);
        break;

    /* 0x538C — big shared group: SG01ATT01 + LCOD39/40 */
    case 5: case 6: case 7: case 8: case 9: case 10: case 11:
    case 14: case 15: case 16: case 20: case 29: case 33:
    case 39: case 40: case 41: case 45: case 51: case 60:
    case 72: case 73: case 74: case 75: case 86: case 87:
    case 89: case 93: case 94: case 96:
        e5_reset(); e5_att01_lcod("LCOD39", "LCOD40"); CB->bloque(0);
        break;

    case 82:  /* 0x5444 — LCOD55/56 */
        e5_reset();
        if (ST(0x194) == 2) l01_action_register("LCOD55", 2, "SP02ACT64", NULL);
        if (ST(0x194) == 3) l01_action_register("LCOD56", 3, "SP02ACT64", NULL);
        CB->animation("ar_50");
        CB->bloque(0);
        break;

    case 99:  /* 0x4F7D — SPxxATT20 first-time, else LCOD41/42/43 */
        e5_reset();
        if (ST(0x160) == 0) {
            ST(0x160) = 1;
            if (ST(0x194) == 1) CB->animation("SP01ATT20");
            if (ST(0x194) == 2) CB->animation("SP02ATT20");
            if (ST(0x194) == 3) CB->animation("SP02ATT20");
            e5_clamp6(0x96);
            CB->animation("ar_50");
        } else {
            if (ST(0x194) == 1) CB->dialogue("LCOD41", ST(0x194));
            if (ST(0x194) == 2) CB->dialogue("LCOD42", ST(0x194));
            if (ST(0x194) == 3) CB->dialogue("LCOD43", ST(0x194));
            CB->animation("SG01ATT01");
            CB->animation("ar_50");
        }
        CB->bloque(0);
        break;

    case 21: {  /* 0x3B4E — OB21A/B/C by phase (edx), ATT19/20 + clamp */
        const c8 *ob;
        if      (phase == 0) ob = "OB21A";
        else if (phase == 1) ob = "OB21B";
        else                 ob = "OB21C";
        e5_reset();
        if (ST(0x198) == 1) {
            if (ST(0x194) == 2) CB->animation("SP02ATT19");
            if (ST(0x194) == 3) CB->animation("SP03ATT19");
        }
        if (ST(0x198) == 0) {
            if (ST(0x194) == 2) CB->animation("SP02ATT20");
            if (ST(0x194) == 3) CB->animation("SP03ATT20");
        }
        CB->animation("ar_50");
        CB->etat_objet(ob, -1, -1, -1, -1, -1, -1, -2, 3);
        e5_clamp6(0xC8);
        CB->bloque(0);
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
                if (ST(0x194) == 1) CB->animation("SP01ACT98");
                if (ST(0x194) == 2) CB->animation("SP02ATT19");
                if (ST(0x194) == 3) CB->animation("SP03ATT19");
                e5_clamp6(0x1E);
                CB->animation("ar");
                CB->bloque(0);
                break;
            }
            if (*timer == 0) {
                if (ST(0x194) == 1) CB->animation("SP01ACT98");
                if (ST(0x194) == 2) CB->animation("SP02ATT19");
                if (ST(0x194) == 3) CB->animation("SP03ATT19");
                c8 b1[16], b2[16];
                snprintf(b1, sizeof b1, "OB24%c", phase + 'A');
                snprintf(b2, sizeof b2, "OB77%c", phase + 'A');
                CB->etat_objet(b1, -1, -1, -1, -1, -1, -1, -2, 0);
                CB->etat_objet(b2, -1, -1, -1, -1, -1, -1, -3, 0);
                e5_clamp6(0x1E);
                if (ST(0x194) == 1) CB->dialogue("LCOD41", ST(0x194));
                if (ST(0x194) == 2) CB->dialogue("LCOD42", ST(0x194));
                if (ST(0x194) == 3) CB->dialogue("LCOD43", ST(0x194));
                CB->animation("SG01ATT01");
                CB->animation("ar_50");
                CB->bloque(0);
                break;
            }
        }
    case24_fallback:
        if (ST(0x198) == 0 && ST(0x194) == 1) {
            ST(0x140 + phase * 4) = 0;
            CB->animation("SP01ACT98");
            CB->evenement(0x50, 0x7D0);
        }
        CB->bloque(0);
        break;
    }

    case 31: {  /* 0x4385 — OB31 + layer 0x14 reduction */
        e5_reset();
        if (ST(0x194) == 2) CB->animation("SP02ATT20");
        if (ST(0x194) == 3) CB->animation("SP03ATT20");
        CB->animation("ar_50");
        e5_clamp6(0x32);
        CB->animation("ar_50");
        {
            i32 lv = CB->divers(0x14, -1, 0);
            CB->divers_empile(0x14, lv - 0x96, 0);
        }
        CB->etat_objet("OB31", -1, -1, -1, -1, -1, -1, -2, 0);
        CB->bloque(0);
        break;
    }

    case 71: {  /* 0x2D8C — SOB86 FSM (multi-branch room logic) */
        CB->select_pile(0, 2);
        CB->divers_empile(0x30, 0, 0);
        CB->divers_empile(0x30, 0, 0);
        CB->divers_empile(0x30, 0, 0);
        if (ST(0xF44) == 0) {
            if (ST(0x18) == 0x1E) {
                if (ST(0xF90) != 5 || ST(0x2B0) != 2) goto case71_lcod;
                ST(0xF44) = 1;
                i32 v4 = CB->divers(1, ST(0x194), 2);
                CB->bloque(3);
                CB->divers_empile(0x10, 5, 0);
                if (ST(0x194) == 2)
                    CB->pos_perso(2, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                if (ST(0x194) == 3)
                    CB->pos_perso(3, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                if (ST(0x194) == 2) {
                    CB->pos_perso(0x62, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                    CB->pos_perso(0x62, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                }
                if (ST(0x194) == 3) {
                    CB->pos_perso(0x63, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                    CB->pos_perso(0x63, 0x115C, 0x169E, v4, 1, "ar", 1, 0);
                }
                CB->divers_empile(0x10, 3, 0);
                if (ST(0xF5C) == 0) {
                    ST(0xF44) = 0;
                    if (ST(0x194) == 2) {
                        CB->dialogue("L31D83A", 2);
                        CB->dialogue("L31D83B", 2);
                        if (ST(0xC50) == 0) CB->dialogue("L31D83C", 2);
                    }
                    if (ST(0x194) == 3) {
                        CB->dialogue("L31D84A", 3);
                        CB->dialogue("L31D84B", 3);
                        if (ST(0xC50) == 0) CB->dialogue("L31D84C", 3);
                    }
                    CB->animation("Sar_50");
                    CB->divers_empile(0x10, 5, 0);
                    if (ST(0x194) == 2)
                        CB->pos_perso(2, 0x115C, 0x169E, v4, 0, "ar", 1, 0);
                    if (ST(0x194) == 3)
                        CB->pos_perso(3, 0x115C, 0x169E, v4, 0, "ar", 1, 0);
                    CB->divers_empile(0x10, 3, 0);
                }
                CB->animation("Sar_50");
                CB->bloque(0);
                break;
            }
            if (ST(0x18) == 0x25) {
                if (ST(0xF90) != 0 || CB->distance(1, 6) >= 0x15E)
                    goto case71_lcod;
                ST(0xF44) = 1;
                CB->bloque(3);
                CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                CB->divers_empile(0x10, 5, 0);
                if (ST(0x194) == 2)
                    CB->pos_perso(0x62,
                        CB->divers(1, ST(0x194), 0),
                        CB->divers(1, ST(0x194), 1),
                        CB->divers(1, ST(0x194), 2),
                        1, "ar", 6, 0);
                if (ST(0x194) == 3)
                    CB->pos_perso(0x63,
                        CB->divers(1, ST(0x194), 0),
                        CB->divers(1, ST(0x194), 1),
                        CB->divers(1, ST(0x194), 2),
                        1, "ar", 6, 0);
                CB->divers_empile(0x10, 3, 0);
                CB->animation("Sar_50");
                CB->bloque(0);
                break;
            }
        case71_lcod:
            CB->bloque(3);
            if (ST(0x194) == 2) l01_action_register("LCOD666", 2, "SG02DIA02", NULL);
            if (ST(0x194) == 3) l01_action_register("LCOD667", 3, "SG03DIA02", NULL);
            CB->bloque(0);
            break;
        } else {
            if (ST(0x18) == 0x23) {
                CB->bloque(3);
                if (ST(0x194) == 2) l01_action_register("LCOD670", 2, "SG02DIA02", NULL);
                if (ST(0x194) == 3) l01_action_register("LCOD671", 3, "SG03DIA02", NULL);
                CB->bloque(0);
                break;
            }
            ST(0xF44) = 0;
            CB->bloque(3);
            CB->divers_empile(0x10, 5, 0);
            if (ST(0x194) == 2)
                CB->pos_perso(2,
                    CB->divers(1, 2, 0), CB->divers(1, 2, 1),
                    CB->divers(1, 2, 2), 0, "ar", 1, 0);
            if (ST(0x194) == 3)
                CB->pos_perso(3,
                    CB->divers(1, 3, 0), CB->divers(1, 3, 1),
                    CB->divers(1, 3, 2), 0, "ar", 1, 0);
            CB->divers_empile(0x10, 3, 0);
            if (ST(0x194) == 2) l01_action_register("LCOD668", 2, "SG02DIA01", NULL);
            if (ST(0x194) == 3) l01_action_register("LCOD669", 3, "SG03DIA01", NULL);
            CB->bloque(0);
            break;
        }
    }

    case 88: {  /* 0x54F1 — SPxxATT02 health decrement */
        e5_reset();
        if (ST(0x6D8) == 0) {
            if (ST(0x198) == 0) {
                ST(0x6D8) = 1;
                if (ST(0x194) == 1) {
                    i32 v = CB->divers(6, 1, -1);
                    if (v - 0x32 > 0x64) CB->divers(6, 1, v - 0x32);
                    CB->animation("SP01ATT02");
                    CB->animation("P01ATT03");
                    CB->animation("ar_50");
                }
                if (ST(0x194) == 2) {
                    i32 v = CB->divers(6, 2, -1);
                    if (v - 0x1E > 0x32) CB->divers(6, 2, v - 0x1E);
                    CB->animation("SP02ATT02");
                    CB->animation("P02ATT03");
                    CB->animation("ar_50");
                }
                if (ST(0x194) == 3) {
                    i32 v = CB->divers(6, 3, -1);
                    if (v - 0x14 > 0x32) CB->divers(6, 3, v - 0x14);
                    CB->animation("SP02ATT02");
                    CB->animation("P02ATT03");
                    CB->animation("ar_50");
                }
            }
            if (ST(0x198) == 1) {
                if (ST(0x194) == 1) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD25", 1);
                    CB->animation("ar_50");
                }
                if (ST(0x194) == 2) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD26", 2);
                    CB->animation("ar_50");
                }
                if (ST(0x194) == 3) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD27", 3);
                    CB->animation("ar_50");
                }
            }
        } else {
            if (ST(0x194) == 1) CB->dialogue("LCOD41", ST(0x194));
            if (ST(0x194) == 2) CB->dialogue("LCOD42", ST(0x194));
            if (ST(0x194) == 3) CB->dialogue("LCOD43", ST(0x194));
            CB->animation("SG01ATT01");
            CB->animation("ar_50");
        }
        CB->bloque(0);
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

    if (ST(0x194) != 4 || ST(0xE8C) == 1) {
        l01_trig00();   /* sub_10001073 default path */
    } else {
        CB->divers_empile(16, 1, 0);
        ST(0x1A0) = 1;
        for (i32 layer = 0; layer < 4; layer++) {
            if (CB->divers(29, layer, 0))
                CB->divers(6, layer, 0);
        }
        if (!ST(0xB04))
            CB->cinematique("SC22_500");
        else
            CB->cinematique("SC23_500");
        CB->divers_empile(27, 0, 0);
    }
}

/* -------------------------------------------------------------------------
 * TICK (opcode 0) + DEINIT (opcode -1): drain pending sound, run body.
 * ---------------------------------------------------------------------- */
static void tick(void) {
    if (ST(0x134) != 0) {
        CB->select_pile(2, 1);
        CB->evenement(ST(0x134) + 0x59, 1000);
        ST(0x134) = 0;
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
