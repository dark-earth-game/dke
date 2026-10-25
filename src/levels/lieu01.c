/*
 * lieu01.c — Native hand-port of LIEU01.DLL (level-1 scripting).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. Handlers call the engine only through
 * g_lieu_cb (see lieu_cb.h). See docs/functions/lieu01_handlers.md for
 * the opcode dispatch, trigger table and state-offset map.
 *
 * Status: COMPLETE — init, tick/tick_body, event4 (all cases), event5
 * (all cases), event6, all 25 triggers, opcode dispatch. No stubs remain.
 */

#include <stdio.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "lieu01.h"

/* Engine-provided callback vtable is defined in lieu_cb.c (g_lieu_cb). */

/* Current state buffer for the in-flight call (original used the global
 * dword_1002C96C; the entry point sets this per call). */
static lieu_state_t *S;

/* Shorthands mirroring the disassembly. */
#define ST(off)   LIEU_ST(S, (off))
#define CB        (g_lieu_cb)

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
static void action_register(const c8 *name, i32 count,
                            const c8 *arg0, const c8 *arg4) {
    CB->named_object(arg0);
    CB->named_object_int(name, count);
    if (count < 4 && arg4 == NULL) {
        CB->named_object("Sar_20");
    } else if (arg4 != NULL) {
        CB->named_object(arg4);
    } else {
        c8 b[64];
        snprintf(b, sizeof(b), "SP%02darr01_10", count);
        CB->named_object(b);
    }
}

/* Spawn "ar" actor at position from 3 layer-1 coordinate queries.
 * Queries evaluated axis 2,1,0 (original order) into temps. */
static void e_spawn_ar(void) {
    i32 a2 = CB->layer_op(1, ST(0x194), 2);
    i32 a1 = CB->layer_op(1, ST(0x194), 1);
    i32 a0 = CB->layer_op(1, ST(0x194), 0);
    CB->spawn_678(ST(0x194), a0, a1, a2, 1, "ar", 6, 0);
}

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig00(void); static void trig01(void); static void trig02(void);
static void trig03(void); static void trig04(void); static void trig05(void);
static void trig07(void); static void trig08(void); static void trig09(void);
static void trig11(void); static void trig15(void); static void trig17(void);
static void trig18(void); static void trig21(void); static void trig23(void);
static void trig24(void); static void trig25(void); static void trig26(void);
static void trig27(void); static void trig28(void); static void trig29(void);
static void trig30(void); static void trig31(void); static void trig32(void);
static void trig41(void);
static void trig_noop(void);   /* the 15 stub slots */

/* dword_1002C880[42] — populated at INIT in the original; static here. */
static void (*const g_trigger[42])(void) = {
    trig00, trig01, trig02, trig03, trig04, trig05, trig_noop, trig07,
    trig08, trig09, trig_noop, trig11, trig_noop, trig_noop, trig_noop,
    trig15, trig_noop, trig17, trig18, trig_noop, trig_noop, trig21,
    trig_noop, trig23, trig24, trig25, trig26, trig27, trig28, trig29,
    trig30, trig31, trig32, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* -------------------------------------------------------------------------
 * event6 — sub_10005BEC.  Switch on arg (0..4): LCOD zone triggers.
 * Fully ported.
 * ---------------------------------------------------------------------- */
static void event6(i32 arg) {
    switch (arg) {
    case 0:  /* loc_10005C0A */
        if (ST(0x194) == 2 && ST(0x10) == 0 && ST(0x938) == 0) {
            CB->state_toggle(0, 1);
            ST(0x938) = 1;
            CB->group(3);
            action_register("LCOD34", 2, "SG00CON01", NULL);
            CB->group(0);
        }
        break;
    case 1:  /* loc_10005C87 */
        if (ST(0x194) == 2 && ST(0x7C8) == 0) {
            CB->state_toggle(0, 1);
            ST(0x7C8) = 1;
            ST(0x76C) = 0;
            CB->group(3);
            action_register("LCOD35", 2, "SG00CON01", NULL);
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
            action_register("LCOD36", 3, "SG00CON01", "SG00CON01");
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
            action_register("LCOD37", 3, "SG00CON01", NULL);
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
 * trigger 41 — sub_10008798.  Fully ported.
 * ---------------------------------------------------------------------- */
static void trig41(void) {
    if (ST(0xAF0) == 0 && ST(0x194) == 0) {
        ST(0xAF0) = 1;
        CB->named_two_ints(2, 0x0A);
        CB->named_two_ints(0, 0x7530);
    }
    if (ST(0x194) == 0 && ST(0x74) == 1) {
        ST(0x68) = 1;
        ST(0x88) = 0x29A;
        action_register("L01D17A", 0x0C, "SP12ARR03", "SP12ARR03_10");
        action_register("L01D17B", 0x0C, "SP12ATT06", "SP12ARR03_10");
        CB->cb_458(6, 1);
        CB->group(0);
        CB->cb_46C(&ST(0x324), 1);
        CB->cb_46C(&ST(0x74), 0);
    }
    if (ST(0xF74) == 1) {
        CB->group(3);
        CB->named_object_int("L01D147", 0x58);
        CB->cb_970(0x58, 0);
        CB->group(0);
        ST(0x94) = 1;
        ST(0x34) = 0x58;
    }
}

/* trigger 25 — sub_10012DAE. Spawn "ar", register LCOD06-11 by state. */
static void trig25(void) {
    if (ST(0x0C) == 1 && ST(0x30) == 0 && CB->query_998(0xC738)) {
        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();
        ST(0x928) = CB->query_940(2);
        if (ST(0x928) == 0) {
            if (ST(0x194) == 0) action_register("LCOD06", 0, "SG00PRE02", NULL);
            if (ST(0x194) == 1) action_register("LCOD06", 1, "SG00PRE02", NULL);
            if (ST(0x194) == 2) action_register("LCOD07", 2, "SG00PRE02", NULL);
            if (ST(0x194) == 3) action_register("LCOD08", 3, "SG00PRE02", NULL);
        }
        if (ST(0x928) == 1) {
            if (ST(0x194) == 0) action_register("LCOD09", 0, "SG00PRE03", NULL);
            if (ST(0x194) == 1) action_register("LCOD09", 1, "SG00PRE03", NULL);
            if (ST(0x194) == 2) action_register("LCOD10", 2, "SG00PRE03", NULL);
            if (ST(0x194) == 3) action_register("LCOD11", 3, "SG00PRE03", NULL);
        }
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
    }
}

/* triggers 30/31/32 — sub_10014C56/EDD, sub_10015164. Identical bodies,
 * differ only in query_944 arg (0x1E/0x1F/0x20). Spawn "ar", then
 * SPxxACT11/85/86 by level [0x194] + [0x198]. */
static void e_trig_spawn_act(i32 q) {
    if (ST(0x0C) == 1 && ST(0x30) == 0 && ST(0x34) == 0 && ST(0x10) == 0
        && CB->query_944(q)) {
        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();
        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) CB->named_object("SP00ACT11");
            if (ST(0x198) == 0) CB->named_object("SP00ACT86");
        }
        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) CB->named_object("SP01ACT11");
            if (ST(0x198) == 0) CB->named_object("SP01ACT86");
        }
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) CB->named_object("SP02ACT11");
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 9, 2)) CB->named_object("SP02ACT85");
                else                       CB->named_object("SP02ACT86");
            }
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) CB->named_object("SP03ACT11");
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 9, 2)) CB->named_object("SP03ACT85");
                else                       CB->named_object("SP03ACT86");
            }
        }
        CB->named_object("ar_50");
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
    }
}
static void trig30(void) { e_trig_spawn_act(0x1E); }
static void trig31(void) { e_trig_spawn_act(0x1F); }
static void trig32(void) { e_trig_spawn_act(0x20); }

/* -------------------------------------------------------------------------
 * Trigger handlers (opcode 3), ported from asm via parallel agents.
 * All 25 triggers complete. No stubs remain.
 * ---------------------------------------------------------------------- */
/* -------------------------------------------------------------------------
 * Trigger handlers (opcode 3), ported from asm via parallel agents.
 * ---------------------------------------------------------------------- */

static void trig00(void) {   /* sub_100073F0 */
    if (ST(0x964) == 1) {
        ST(0x194) = 0;
        ST(0xAF0) = 0;
        ST(0xEC0) = 0;
        ST(0x700) = 0;
        ST(0x5DC) = 0;
    }
    if (ST(0x964) == 2) {
        ST(0x194) = 1;
        ST(0x18)  = 2;
        ST(0xEC0) = 1;
        ST(0xB88) = 0;
        ST(0xB04) = 0;
        ST(0xBE4) = 0;
        ST(0xEC4) = 0;
        ST(0xBA0) = 3;
        ST(0xAF0) = 2;
        ST(0x700) = 1;
        ST(0x5DC) = 1;
    }
    if (ST(0x964) == 3) {
        ST(0x194) = 2;
        ST(0x18)  = 2;
        ST(0xEC0) = 1;
        ST(0xB88) = 0;
        ST(0xB04) = 0;
        ST(0xBE4) = 0;
        ST(0xBA0) = 3;
        ST(0xAF0) = 2;
        ST(0x700) = 1;
        ST(0x5DC) = 1;
    }
    if (ST(0x964) == 4) {
        ST(0x194) = 2;
        ST(0x18)  = 2;
        ST(0xEC0) = 1;
        ST(0xB88) = 0;
        ST(0xB04) = 0;
        ST(0xBE4) = 0;
        ST(0xBA0) = 3;
        ST(0xAF0) = 2;
        ST(0x700) = 1;
        ST(0x5DC) = 1;
    }
    if (ST(0x964) == 5) {
        ST(0x194) = 2;
        ST(0x2EC) = 1;
        ST(0x18)  = 2;
        ST(0xEC0) = 1;
        ST(0xEC4) = 1;
        ST(0xB88) = 1;
        ST(0xB04) = 2;
        ST(0xEC4) = 1;
        ST(0xBE4) = 0;
        ST(0xBA0) = 3;
        ST(0xAF0) = 2;
        ST(0x700) = 1;
        ST(0x5DC) = 1;
    }
    if (ST(0x964) == 6) {
        ST(0x194) = 2;
        ST(0x18)  = 2;
        ST(0xEC0) = 1;
        ST(0xB88) = 1;
        ST(0xB04) = 2;
        ST(0xEC4) = 2;
        ST(0xBE4) = 1;
        ST(0xBA0) = 3;
        ST(0x700) = 1;
        ST(0x5DC) = 1;
        ST(0xAF0) = 2;
    }

    ST(0x64) = 0;  ST(0x68) = 0;  ST(0x6C) = 0;  ST(0x70) = 0;
    ST(0x74) = 0;  ST(0x78) = 0;  ST(0x7C) = 0;  ST(0x80) = 0;
    ST(0x84) = 0;  ST(0x88) = 0;  ST(0x8C) = 0;  ST(0x90) = 0;
    ST(0x94) = 0;  ST(0x98) = 0;  ST(0xB8) = 0;  ST(0xBC) = 0;
    ST(0xD4) = 0;  ST(0xD8) = 0;  ST(0xDC) = 0;  ST(0xE0) = 0;
    ST(0xF0) = 0;  ST(0x320) = 0; ST(0x324) = 0; ST(0x328) = 0;
    ST(0x3A4) = 0; ST(0x3A8) = 0; ST(0x3AC) = 0; ST(0x3B0) = 0;
    ST(0x3B4) = 0; ST(0x3B8) = 0; ST(0x3BC) = 0; ST(0x3C4) = 0;
    ST(0x30) = 0;  ST(0x34) = 0;  ST(0x40) = 0;

    CB->place_95C("L01AMB1", 0x5D, (i32)0xFFFFF5D8);

    CB->spawn_978(0x01, 0x176, (i32)0xFFFFF4B2, 0x9B,  0x5A,  0);
    CB->spawn_978(0x03, 0x203, (i32)0xFFFFF7BD, 0x69,  0x50,  0);
    CB->spawn_978(0x07, 0x1D6, (i32)0xFFFFF40B, 0x50,  0xAA,  1);
    CB->spawn_978(0x08, 0x366, (i32)0xFFFFF510, 0x78,  0x5A,  0);
    CB->spawn_978(0x0B, 0x2B,  (i32)0xFFFFF5F6, 0xBC,  0xF0,  0);
    CB->spawn_978(0x0C, 0x384, (i32)0xFFFFF4FC, 0x19,  0x28,  0);
    CB->spawn_978(0x0D, -62,   (i32)0xFFFFF5E6, 0x5A,  0x64,  0);
    CB->spawn_978(0x0E, 0x35C, (i32)0xFFFFF628, 0x3C,  0x3C,  0);
    CB->spawn_978(0x0F, 0x35C, (i32)0xFFFFF5EC, 0xA0,  0xA0,  0);
    CB->spawn_978(0x14, 0x2C4, (i32)0xFFFFF808, 0x46,  0x46,  0);
    CB->spawn_978(0x15, 0x276, (i32)0xFFFFF79A, 0xA0,  0xF0,  0);
    CB->spawn_978(0x16, 0x151, (i32)0xFFFFF7EC, 0x32,  0x32,  0);
    CB->spawn_978(0x17, 0x16F, (i32)0xFFFFF7CC, 0x8C,  0x8C,  0);
    CB->spawn_978(0x18, 0x114, (i32)0xFFFFF687, 0x11D, 0xF0,  0);
    CB->spawn_978(0x19, 0x2CB, (i32)0xFFFFF610, 0x8C,  0xB4,  0);
    CB->spawn_978(0x1A, 0x3E8, (i32)0xFFFFF671, 0x8C,  0x78,  0);
    CB->spawn_978(0x1B, 0x25,  (i32)0xFFFFF554, 0xFA,  0x5A,  0);
    CB->spawn_978(0x1C, 0x3C3, (i32)0xFFFFF542, 0x50,  0xA0,  0);
    CB->spawn_978(0x1D, 0x2A7, (i32)0xFFFFF46B, 0x73,  0x87,  0);
    CB->spawn_978(0x1E, 0x101, (i32)0xFFFFF4D8, 0xD2,  0xD2,  0);
    CB->spawn_978(0x1F, 0x315, (i32)0xFFFFF6EA, 0x140, 0xEE,  0);
    CB->spawn_978(0x20, 0x2D1, (i32)0xFFFFF4F0, 0xAF,  0xA0,  0);

    CB->named_ex_988("IM01", 0x1D0, 0x0F, (i32)0xFFFFF803, 0xC000, 0x4000, 0x8000, 0, 0);

    if (ST(0x8D8) == 0) {
        CB->named_ex_988("OB44D", 0x2B7, 0x32, (i32)0xFFFFF456, 0, 0, 0x3800, -1, 4);
    }

    CB->cb_964(6, 0);

    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0 &&
        ST(0xB98) == 2 && ST(0x240) != 2) {
        ST(0xB98) = 0;
    }

    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }

    if (ST(0x700) == 0) {
        CB->spawn_978(0x05, 0x2AD, (i32)0xFFFFF610, 0x8C, 0xB4, 0);
        CB->spawn_978(0x13, 0x2C0, (i32)0xFFFFF612, 0x32, 0x32, 0);
        CB->named_ex_988("OB38A", 0x2BF, 0x3B, (i32)0xFFFFF631, 0, 0xE800, 0x7200, -1, 0);
    }

    if (ST(0x5DC) == 0) {
        CB->cb_964(6, 1);
        CB->named_ex_988("OB25", 0x3E8, 0x96, (i32)0xFFFFF5C4, 0, 0xC000, 0, -1, 3);
        CB->spawn_978(0x02, 0x3C3, (i32)0xFFFFF5CE, 0x50, 0xA0, 0);
    }

    if (ST(0xAF0) == 0 && ST(0x194) == 0) {
        ST(0x198) = 1;
        CB->spawn_678(0x0C, 0x26A, (i32)0xFFFFF3CB, 0, 1, "P12ARR01", -1, 0);
        CB->named_object("SP12ARR01_10");
        CB->cb_458(6, 2);
        CB->group(3);
        CB->spawn_978(0x04, 0x262, (i32)0xFFFFF455, 0xF0,  0x64, 1);
        CB->spawn_978(0x0A, 0x1E0, (i32)0xFFFFF704, 0x258, 0x64, 1);
        CB->spawn_678(0, 0x276, (i32)0xFFFFF40A, 0, 1, "P00ARR01", 6, 0);
        CB->named_object("SP00ARR01");
    }

    if (ST(0xAF0) != 0 && ST(0xEC0) == 0 && ST(0xB04) == 0 &&
        ST(0xAF8) == 0 && ST(0x18) == 2) {
        CB->spawn_678(ST(0x194), 0x1FF, (i32)0xFFFFF76C, 0x80E8, 1, "ar", 6, 0);
        if (ST(0x194) == 0) {
            CB->spawn_978(0x04, 0x262, (i32)0xFFFFF455, 0xF0, 0x64, 1);
            CB->spawn_678(0x0C, 0x26A, (i32)0xFFFFF3CB, 0, 1, "P12ARR03", -1, 0);
            ST(0x74)  = 1;
            ST(0x68)  = 1;
            ST(0x320) = 2;
            CB->named_two_ints(0, -1);
            CB->group(3);
            CB->cb_458(6, 2);
        }
        if (ST(0x194) == 1) {
            CB->spawn_978(0x04, 0x262, (i32)0xFFFFF455, 0xF0, 0x64, 1);
            ST(0x320) = 2;
            CB->spawn_678(0x0C, 0x26A, (i32)0xFFFFF3CB, 0, 1, "P12ARR01", -1, 0);
            ST(0x88) = 0x29A;
        }
    }

    if (ST(0xAF0) != 0 && ST(0xEC0) == 1 && ST(0xB04) == 0 &&
        ST(0xAF8) == 0 && ST(0x18) == 2) {
        CB->spawn_678(ST(0x194), 0x1FF, (i32)0xFFFFF76C, 0x80E8, 1, "ar", 6, 0);
        CB->spawn_978(0x04, 0x262, (i32)0xFFFFF455, 0xF0, 0x64, 0);
        if ((i32)ST(0x2EC) < 2) {
            CB->spawn_678(0x57, 0x70, (i32)0xFFFFF636, 0x5DC0, 1, "P87ATT04", 9, 1);
        }
        if (ST(0x2EC) == 2) {
            i32 c0 = CB->layer_op(1, 0x57, 2);
            i32 c1 = CB->layer_op(1, 0x57, 1);
            i32 c2 = CB->layer_op(1, 0x57, 0);
            CB->spawn_678(0x57, c2, c1, c0, 2, "P87cad", 9, 0);
        }
    }

    if (ST(0xAF0) != 0 && ST(0xEC0) == 1 && ST(0xB04) == 0 &&
        ST(0xAF8) == 1 && ST(0x18) == 2) {
        CB->spawn_678(ST(0x194), 0x1FF, (i32)0xFFFFF76C, 0x80E8, 1, "ar", 6, 0);
        CB->spawn_978(0x04, 0x262, (i32)0xFFFFF455, 0xF0, 0x64, 0);
        if ((i32)ST(0x2EC) < 2) {
            CB->spawn_678(0x57, 0x70, (i32)0xFFFFF636, 0x5DC0, 1, "P87ATT04", 9, 1);
        }
        if (ST(0x2EC) == 2) {
            i32 c0 = CB->layer_op(1, 0x57, 2);
            i32 c1 = CB->layer_op(1, 0x57, 1);
            i32 c2 = CB->layer_op(1, 0x57, 0);
            CB->spawn_678(0x57, c2, c1, c0, 2, "P87cad", 9, 0);
        }
    }

    if (ST(0xAF0) != 0 && ST(0xEC0) == 1 && ST(0xB04) == 2 && ST(0x18) == 2) {
        CB->spawn_678(ST(0x194), 0x1FF, (i32)0xFFFFF76C, 0x80E8, 1, "ar", 6, 0);
        if ((i32)ST(0x2EC) < 2) {
            CB->spawn_678(0x57, 0x140, (i32)0xFFFFF61E, 0xDD4, 1, "P87ATT01", 9, 1);
            CB->attach_99C(0x57, "OB94", 0x1B, 1);
            ST(0xF0) = 1;
        }
        if (ST(0x2EC) == 2) {
            i32 c0 = CB->layer_op(1, 0x57, 2);
            i32 c1 = CB->layer_op(1, 0x57, 1);
            i32 c2 = CB->layer_op(1, 0x57, 0);
            CB->spawn_678(0x57, c2, c1, c0, 2, "P87cad", 9, 0);
        }
        if (ST(0xEC4) == 1) {
            if (ST(0xF74) == 0) {
                if ((i32)ST(0x2F0) < 2) {
                    CB->spawn_678(0x58, 0x242, (i32)0xFFFFF43E, 0x1388, 1, "P88ARR02", 0x11, 1);
                }
                if (ST(0x2F0) == 2) {
                    i32 c0 = CB->layer_op(1, 0x58, 2);
                    i32 c1 = CB->layer_op(1, 0x58, 1);
                    i32 c2 = CB->layer_op(1, 0x58, 0);
                    CB->spawn_678(0x58, c2, c1, c0, 2, "P88cad", 0x11, 0);
                }
            }
            if (ST(0xF74) == 1) {
                if ((i32)ST(0x2F0) < 2) {
                    CB->spawn_678(0x58, 0x250, (i32)0xFFFFF507, 0xFB1D, 1, "P88GAR01", 0x11, 1);
                }
                if (ST(0x2F0) == 2) {
                    i32 c0 = CB->layer_op(1, 0x58, 2);
                    i32 c1 = CB->layer_op(1, 0x58, 1);
                    i32 c2 = CB->layer_op(1, 0x58, 0);
                    CB->spawn_678(0x58, c2, c1, c0, 2, "P88cad", 0x11, 0);
                }
            }
        }
        if (ST(0xEC4) == 2) {
            ST(0xF74) = 0;
            if ((i32)ST(0x2F4) < 2) {
                CB->spawn_678(0x59, 0x27E, (i32)0xFFFFF40A, 0x3A98, 1, "P89ARR01", 0x12, 1);
            }
            if (ST(0x2F4) == 2) {
                i32 c0 = CB->layer_op(1, 0x59, 2);
                i32 c1 = CB->layer_op(1, 0x59, 1);
                i32 c2 = CB->layer_op(1, 0x59, 0);
                CB->spawn_678(0x59, c2, c1, c0, 2, "P89cad", 0x12, 0);
            }
        }
    }

    if (ST(0x6F4) == 1 || (ST(0x6F4) == 0 && ST(0xEC4) != 0)) {
        CB->named_ex_988("OB95", 0x13B, 0x4B, (i32)0xFFFFF65F, 0x4000, 0xEA00, 0, -1, 2);
    }

    if (ST(0x6F4) == 1 || (i32)ST(0x2EC) >= 2) {
        CB->spawn_978(0x10, 0x140, (i32)0xFFFFF611, 0x5A, 0x5A, 0);
    }

    CB->layer_op(0x18, 1, 0);
    CB->layer_op(4, 0x10001, 0x23280000);

    ST(0x964) = 0;
}

static void trig01(void) {   /* sub_1000B563 */
    if (ST(0xC) != 1) return;
    if (ST(0x30) != 0) return;

    if (ST(0x194) != 4
        && CB->query_998(0x7530)
        && (i32)CB->layer_op(1, ST(0x194), 2) > 0x61A8) {

        ST(0x30) = 1;
        CB->group(3);
        e_spawn_ar();

        if (ST(0x198) == 1) {
            if (ST(0x194) == 0) {
                CB->named_object("SP00ACT26");
                if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21A", 1);
                if (ST(0xBA0) == 1)     CB->named_object_int("L01D29A", 0xC);
                if (ST(0x68) == 1)      CB->cb_46C(&ST(0x324), 1);
            }
            if (ST(0x194) == 1) {
                CB->named_object("SP01ACT26");
                if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21A", 1);
                if (ST(0xBA0) == 1)     CB->named_object_int("L01D29B", 0xC);
                if (ST(0x68) == 1)      CB->cb_46C(&ST(0x324), 1);
            }
            if (ST(0x194) == 2) {
                CB->named_object("SP02ACT26");
                if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21B", 2);
            }
            if (ST(0x194) == 3) {
                CB->named_object("SP03ACT26");
                if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21C", 3);
            }
            ST(0x6C)++;
            CB->named_object("ar_10");
            CB->cb_46C(&ST(0x30), 0);
            CB->group(0);
            return;
        }

        if (ST(0x198) == 0) {
            if (ST(0x194) == 0) {
                CB->named_object("SP00ACT27");
                if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21D", 1);
                if (ST(0xBA0) == 1)     CB->named_object_int("L01D29C", 0xC);
                if (ST(0x68) == 1)      CB->cb_46C(&ST(0x324), 1);
            }
            if (ST(0x194) == 1) {
                CB->named_object("SP01ACT27");
                if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21D", 1);
                if (ST(0xBA0) == 1)     CB->named_object_int("L01D29C", 0xC);
                if (ST(0x68) == 1)      CB->cb_46C(&ST(0x324), 1);
            }
            if (ST(0x194) == 2) {
                CB->named_object("SP02ACT27");
                if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21B", 2);
            }
            if (ST(0x194) == 3) {
                CB->named_object("SP03ACT27");
                if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21C", 3);
            }
            ST(0x6C)++;
            CB->named_object("ar_10");
            CB->cb_46C(&ST(0x30), 0);
            CB->group(0);
            return;
        }
        /* ST(0x198) neither 0 nor 1: falls through to tail (loc_1000B9D5) */
    }

    /* tail: loc_1000B9D5 */
    if (ST(0x194) == 4) return;
    if (!CB->layer_op(9, ST(0x194), 0x1E)) return;
    if (!CB->query_944(0x1E)) return;
    if ((i32)CB->layer_op(1, ST(0x194), 2) >= 0x4268) return;
    trig30();
}

static void trig02(void) {   /* sub_1000BA47 */
    if (ST(0x5DC) == 1 && ST(0xC) == 1 && ST(0x30) == 0 && CB->query_998(0xC738)) {
        CB->named_object("SG00ATT01");
        if ((i32)ST(0x194) <= 1)
            CB->named_object_int("LCOD78", 1);
        if (ST(0x194) == 2)
            CB->named_object_int("LCOD76", 2);
        if (ST(0x194) == 3)
            CB->named_object_int("LCOD08", 3);
        CB->named_object("Sar_50");
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        return;
    }

    if (ST(0xC) != 1) return;
    if (ST(0x30) != 0) return;
    if (ST(0x194) != 0) return;
    if (!CB->query_954(-25)) return;

    if (ST(0x320) == 0) {
        CB->named_two_ints(0, -1);
        CB->named_two_ints(0, 0x3A98);
    }
    CB->group(3);
    ST(0x30) = 1;
    CB->cb_964(6, 0);
    if (ST(0x198) == 1)
        CB->named_object_int("L01D119", 0);
    else
        CB->named_object_int("L01D120", 0);

    if (ST(0x194) != 4) {
        if ((i32)CB->layer_op(1, ST(0x194), 0) < 0x3D9 &&
            (i32)CB->layer_op(1, ST(0x194), 1) < (i32)0xFFFFF5A6) {
            CB->actor_934(-ST(0x194), 0x3D9, (i32)0xFFFFF59F, 0xE678, -1);
            CB->cb_46C(&ST(0x30), 1);
        }
    }

    CB->named_object("SP00PRE00");
    CB->named_ex_988("OB25", 0x3E8, 0x96, (i32)0xFFFFF5C4, 0, 0xC000, 0, -2, 0);
    CB->actor_934(ST(0x194), 0x3CA, (i32)0xFFFFF65E, -1, -1);
    CB->cb_458(5, 2);
    CB->named_object("Sar_1000");
    CB->place_95C("L01B07", 0x3CA, (i32)0xFFFFF65E);
    CB->spawn_678(1, 0x3CA, (i32)0xFFFFF65E, 0x7274, 1, "ar", 6, 0);
    CB->named_object("Sar_2000");
    ST(0x5DC) = 1;
    CB->cb_46C(&ST(0x194), 1);
    CB->cb_458(3, 1);
    CB->cb_458(3, 1);
    CB->actor_934(ST(0x194), 0x3D7, (i32)0xFFFFF5F6, 0x7274, -1);
    CB->named_object("ar_200");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
}

static void trig03(void) {   /* sub_1000BDC7 */
    if (ST(0xC) != 1) return;                 /* cmp [eax+0Ch],1 ; jnz end   */
    if (ST(0x30) != 0) return;                /* cmp [eax+30h],0 ; jnz end   */
    if (CB->query_998(0xFF29) == 0) return;       /* ecx=0FF29h ; C998 ; jz end  */

    CB->state_toggle(0, 2);                        /* edx=2; ecx=0 ; C464         */
    CB->group(3);                                  /* ecx=3 ; C938                */
    ST(0x30) = 1;

    if (ST(0x194) == 0 && ST(0x70C) == 0) {
        ST(0x320) = 2;
        ST(0x70C) = 1;
        CB->named_object("SP12ARR03_10");          /* C460 */
        CB->named_object("SG00OUV01");
        CB->named_object("P00_65535");
        CB->cb_98C("IM01", 3, -60, 1500, 0);
        CB->cb_458(6, 2);                          /* edx=2; ecx=6 ; C458         */
        action_register("L01D32", 0xC, "SP12DIA59", "SP12ARR03_10");
        ST(0x18) = 1;
        CB->op_9c(0x18, 1, 4);                     /* push4; edx=1; ecx=18h; C45C */
        CB->cb_984(2, 0);                          /* edx=0; ecx=2 ; C984         */
        return;
    }

    if (ST(0x194) == 0 && ST(0x70C) == 1) {    /* loc_BEF7 */
        action_register("L01D33", 0, "SG00OUV00", NULL);
        CB->cb_46C(&ST(0x30), 0);                  /* ecx=state+30h; edx=0 ; C46C */
        CB->group(0);                              /* ecx=0 ; C938                */
        CB->state_toggle(0, 1);                    /* edx=1; ecx=0 ; C464         */
        return;
    }

    /* loc_BF47 */
    if (ST(0x194) == 1
        && (i32)ST(0xBA0) < 2                  /* cmp [0BA0h],2 ; jge C105    */
        && ST(0xACC) != 0x29A) {               /* cmp [0ACCh],29Ah ; jz C105  */
        ST(0x320) = 2;
        CB->named_two_ints(0, -1);                 /* edx=-1; ecx=0 ; C674        */
        ST(0x68) = 1;
        CB->state_toggle(1, 2);                    /* edx=2; ecx=1 ; C464         */
        CB->state_toggle(0, 1);                    /* edx=1; ecx=0 ; C464         */
        CB->named_object("SP12ARR03_100");
        ST(0x88) = -1;
        ST(0xACC) = 0x29A;

        if (ST(0x198) == 1) {                  /* cmp [198h],1 ; jnz C06F     */
            CB->named_object("SG01OUV02");
            CB->named_object_int("L01D34A", 1);    /* C470 */
            CB->named_object_int("L01D34B", 1);
            CB->cb_458(6, 2);
            action_register("L01D36", 0xC, "SP12DIA56", "SP12ARR03_10");
            CB->cb_458(4, 1);                      /* edx=1; ecx=4 ; C458         */
            CB->named_object("SG01OUV03");
        }
        if (ST(0x198) == 0) {                  /* loc_C06F: cmp [198h],0 ; jnz C100 */
            CB->named_object("SG01OUV02");
            CB->named_object_int("L01D35A", 1);
            CB->named_object_int("L01D35B", 1);
            CB->named_object_int("L01D35C", 1);
            CB->cb_458(6, 2);
            action_register("L01D37", 0xC, "SP12DIA59", "SP12ARR03_10");
            CB->cb_458(4, 1);
            CB->named_object("G01OUV03");
        }
    } else {
        CB->named_object("SG00OUV01");             /* loc_C105 */
    }

    /* loc_C110 (common tail) */
    CB->named_object("P00_65535");
    CB->cb_98C("IM01", 3, -60, 1500, 0);
    CB->group(0);                                  /* ecx=0 ; C938                */
    ST(0x18) = 1;
    CB->op_9c(0x18, 1, 4);                         /* push4; edx=1; ecx=18h; C45C */
    CB->cb_984(2, 0);                              /* edx=0; ecx=2 ; C984         */
}

static void trig04(void) {                                  /* sub_1000C171 */
    /* --- A --- */
    if (ST(0x0C) == 1 && CB->query_944(0x1D) &&
        CB->layer_op(9, ST(0x194), 0x1D)) {
        trig29();
        return;
    }

    /* --- B (loc_1000C1C3) --- */
    if (ST(0x10C) == 1 && ST(0x110) == 0 && ST(0x34) == 0 &&
        ST(0x10) == 1 && CB->query_954(0x0C)) {
        ST(0x110) = 1;
        CB->group(3);
        ST(0x320) = 2;
        ST(0x708) = 1;
        ST(0x98)  = 1;
        if (ST(0xF90) != 6)
            CB->cb_458(6, 2);
        CB->named_two_ints(0, -1);
        if (ST(0x68) == 0) {
            ST(0x68) = 1;
            CB->named_object("P12ARR01_100");
            action_register("L01D41", 0x0C, "SP12ATT02", "SP12LEV01");
            CB->named_object("P12ARR03_10");
            CB->op_9c(0x0C, 0x0C, 1);
        } else {
            CB->named_object("P12ARR03_100");
            action_register("L01D41", 0x0C, "SP12ATT05", "SP12ARR03_10");
        }
        CB->cb_46C(&ST(0x88), 0x29A);
        CB->named_object("Sar_10");
        CB->cb_46C(&ST(0x110), 0);
        CB->group(0);
        return;
    }

    /* --- C (loc_1000C356): the ST(0xEC0)==0 world --- */
    if (ST(0xEC0) == 0) {

        /* first-run intro / dialogue tree */
        if (ST(0x0C) == 1 && ST(0x30) == 0 && CB->query_998(0x8660)) {
            ST(0x30) = 1;
            CB->group(3);
            ST(0x98) = 1;
            CB->cb_458(-7, 2);

            if (ST(0x194) == 0 || ST(0x194) == 1) {
                CB->actor_934(-ST(0x194), 0, 0, 0, 0x0C);

                /* loc_1000C410 */
                if (ST(0x708) == 0 && ST(0x320) == 0) {
                    ST(0x320) = 1;
                    ST(0x708) = 1;
                    CB->named_two_ints(0, -1);
                    action_register("L01D02", 0, "SG00DIA05", NULL);
                    CB->cb_46C(&ST(0x30), 0);
                    return;
                }

                /* loc_1000C488 */
                if ((i32)ST(0x708) > 0 && ST(0x68) == 0) {
                    ST(0x68) = 1;
                    CB->state_toggle(1, 1);
                    CB->named_object("SP12LEV01");
                    CB->named_object("P12ARR03_10");
                    CB->cb_46C(&ST(0x324), 1);
                    CB->state_toggle(0, 1);
                }

                /* loc_1000C4FF */
                if (ST(0xBA0) == 0) {
                    if (ST(0x198) == 1) {
                        if (ST(0x708) == 1) {
                            action_register("L01D04A", 0, "SG00DIA07", NULL);
                            action_register("L01D05A", 0x0C, "P12DIA50", "SP12ARR03_10");
                            action_register("L01D04B", 0, "SG00DIA06", NULL);
                            action_register("L01D05B", 0x0C, "SP12DIA56", "SP12ARR03_10");
                            CB->named_object_int("L01D05C", 0x0C);
                            CB->named_object_int("L01D05D", 0x0C);
                            ST(0x708) = 2;
                            CB->cb_46C(&ST(0x30), 0);
                            CB->group(0);
                            return;
                        }
                        if (ST(0x708) == 2) {
                            ST(0x708) = 3;
                            action_register("L01D06A", 0, "SG00DIA06", NULL);
                            action_register("L01D06B", 0, "SG00DIA02", NULL);
                            CB->named_object_int("L01D07", 0x0C);
                            action_register("L01D08A", 0, "SG00DIA01", NULL);
                            action_register("L01D08B", 0, "SG00DIA07", NULL);
                            action_register("L01D09A", 0x0C, "SP12DIA59", "SP12ARR03_10");
                            CB->named_object_int("L01D09Z", 0x0C);
                            CB->cb_46C(&ST(0x30), 0);
                            CB->group(0);
                            return;
                        }
                        if ((i32)ST(0x708) >= 3) {
                            ST(0x708) = 4;
                            action_register("L01D10", 0, "SG00DIA03", NULL);
                            action_register("L01D11", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            CB->cb_46C(&ST(0x30), 0);
                            CB->group(0);
                            return;
                        }
                    }

                    /* loc_1000C709 */
                    if (ST(0x198) == 0) {
                        if (ST(0x708) == 1) {
                            action_register("L01D13A", 0, "SG00DIA03", NULL);
                            action_register("L01D14", 0x0C, "P12DIA59", "SP12ARR03_10");
                            action_register("L01D13B", 0, "SG00DIA07", NULL);
                            ST(0x708) = 2;
                            CB->cb_46C(&ST(0x30), 0);
                            CB->group(0);
                            return;
                        }
                        if (ST(0x708) == 2) {
                            action_register("L01D15", 0, "SG00DIA01", "sar_1000");
                            action_register("L01D16", 0, "SG00DIA06", NULL);
                            action_register("L01D09A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            action_register("L01D09B", 0x0C, "SP12DIA59", "SP12ARR03_10");
                            ST(0x708) = 3;
                            CB->cb_46C(&ST(0x30), 0);
                            CB->group(0);
                            return;
                        }
                        if ((i32)ST(0x708) >= 3) {
                            action_register("L01D18", 1, "SG01DIA05", "Sar_10");
                            action_register("L01D19", 0x0C, "SP12DIA59", "SP12ARR03_10");
                            CB->cb_46C(&ST(0x30), 0);
                            CB->group(0);
                            return;
                        }
                    }
                }

                /* loc_1000C895 */
                if (ST(0xBA0) == 1) {
                    if (ST(0x8B8) == 0) {
                        ST(0x8B8) = 1;
                        if (ST(0x198) == 1) {
                            action_register("L01D46A", 0, "SG00DIA04", NULL);
                            action_register("L01D20A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            action_register("L01D46B", 0, "SG00DIA03", "sar_750");
                            action_register("L01D46C", 0, "G00DIA05", NULL);
                            action_register("L01D20B", 0x0C, "SP12DIA59", "SP12ARR03_10");
                        }
                        if (ST(0x198) == 0) {
                            action_register("L01D47", 0, "SG00DIA03", NULL);
                            action_register("L01D26A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            CB->named_object_int("L01D26B", 0x0C);
                        }
                        CB->cb_46C(&ST(0x30), 0);
                        CB->group(0);
                        return;
                    }
                    if (ST(0x8B8) == 1) {
                        if (ST(0x198) == 1) {
                            action_register("L01D88A", 0, "SG00DIA02", NULL);
                            CB->named_object_int("L01D89A", 0x0C);
                        }
                        if (ST(0x198) == 0) {
                            action_register("L01D88B", 0, "SG00DIA09", NULL);
                            action_register("L01D89B", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        }
                        CB->cb_46C(&ST(0x30), 0);
                        CB->group(0);
                        return;
                    }
                }

                /* loc_1000CA57 */
                if ((i32)ST(0xBA0) >= 2) {
                    if (ST(0x710) == 0) {
                        ST(0x710) = 1;
                        if (ST(0x198) == 1) {
                            action_register("L01D24A", 1, "SG00DIA07", NULL);
                            action_register("L01D25A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        }
                        if (ST(0x198) == 0) {
                            action_register("L01D83", 1, "SG00DIA07", NULL);
                            action_register("L01D84A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        }
                        CB->named_object_int("L01D25B", 0x0C);
                        action_register("L01D24B", 1, "SG00DIA06", NULL);
                        action_register("L01D25C", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        CB->cb_46C(&ST(0x30), 0);
                        CB->group(0);
                        return;
                    }
                    if (ST(0x710) == 1) {
                        if (ST(0x198) == 1) {
                            action_register("L01D27A", 1, "SG00DIA04", NULL);
                            action_register("L01D28", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        }
                        if (ST(0x198) == 0) {
                            action_register("L01D85A", 1, "SG00DIA02", NULL);
                            action_register("L01D28", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            action_register("L01D85B", 1, "SG00DIA06", NULL);
                        }
                        CB->cb_46C(&ST(0x30), 0);
                        CB->group(0);
                        return;
                    }
                }
            }
            /* fall through to loc_1000CC31 */
        }

        /* loc_1000CC31 : the ST(0x14)==1 branch */
        if (ST(0x14) == 1 && CB->query_998(0x8660)) {
            ST(0x14)  = -1;
            ST(0x320) = 2;
            ST(0x98)  = 1;
            if (ST(0xF90) != 6)
                CB->cb_458(6, 2);
            CB->named_two_ints(0, -1);
            if (ST(0x708) == 0)
                ST(0x708) = 1;
            CB->group(3);

            if ((i32)ST(0x194) <= 1) {
                if (ST(0x714) == 1) {
                    /* loc_1000CDA9 */
                    if (CB->layer_op(2, 0, 0) == 0) {
                        if (ST(0x68) == 1)
                            action_register("L01D42", 0x0C, "P12ATT05", "SP12ARR03_10");
                        CB->named_object("Sar_10");
                        CB->group(0);
                        return;
                    }
                    if (CB->layer_op(2, 0, 0) != 0) {
                        CB->layer_op(0x0C, 0x0C, 0);
                        ST(0x68)  = 0;
                        ST(0x320) = 0x3E8;
                        ST(0x88)  = 0x3E8;
                        ST(0x324) = 0;
                        if (ST(0x68) == 0)
                            action_register("L01D43A", 0x0C, "SP12ATT02", "SP12ATT03_100");
                        action_register("L01D98", 0, "SP00ATA08", NULL);
                        CB->named_object_int("L01D43B", 0x0C);
                        CB->cb_46C(&ST(0x78), 1);
                        CB->state_toggle(1, 1);
                        CB->named_object("P12ATT03_1000");
                        CB->state_toggle(0, 1);
                        return;
                    }
                    /* fall through to loc_1000CF00 */
                } else {
                    /* loc_1000CCFC */
                    if (ST(0x68) == 0) {
                        ST(0x68) = 1;
                        action_register("L01D41", 0x0C, "SP12ATT02", "SP12LEV01");
                        CB->named_object("P12ARR03_10");
                        CB->op_9c(0x0C, 0x0C, 1);
                    } else {
                        action_register("L01D41", 0x0C, "SP12ATT05", "SP12ARR03_10");
                    }
                    CB->cb_46C(&ST(0x88), 0x29A);
                    CB->named_object("Sar_10");
                    ST(0x714) = 1;
                    CB->group(0);
                    return;
                }
            }

            /* loc_1000CF00 */
            if (ST(0x194) == 2) {
                CB->place_95C("L01B11", 0x240, (i32)0xFFFFF450);
                CB->cb_46C(&ST(0x30), 0);
                CB->group(0);
                return;
            }
            if (ST(0x194) == 3) {
                CB->place_95C("L01B11", 0x240, (i32)0xFFFFF450);
                CB->cb_46C(&ST(0x30), 0);
                CB->group(0);
                return;
            }
            /* fall through to loc_1000CF8A */
        }
    }

    /* --- loc_1000CF8A : the ST(0xEC0)!=0 "actor spawn" branch --- */
    if (ST(0xEC0) != 0 && ST(0x0C) == 1 && ST(0x30) == 0 &&
        CB->query_998(0x8660)) {
        ST(0x30) = 1;
        CB->group(3);

        if (ST(0x198) == 1) {
            if (ST(0x194) == 0) {
                CB->named_object("SP00ACT26");
                if ((i32)ST(0x6C) <= 2)
                    CB->named_object_int("L01D21A", 1);
            }
            if (ST(0x194) == 1) {
                CB->named_object("SP01ACT26");
                if ((i32)ST(0x6C) <= 2)
                    CB->named_object_int("L01D21A", 1);
            }
            if (ST(0x194) == 2) {
                CB->named_object("SP02ACT26");
                if ((i32)ST(0x6C) <= 2)
                    CB->named_object_int("L01D21B", 2);
            }
            if (ST(0x194) == 3) {
                CB->named_object("SP03ACT26");
                if ((i32)ST(0x6C) <= 2)
                    CB->named_object_int("L01D21C", 2);
            }
            ST(0x6C)++;
            CB->named_object("ar_10");
            CB->cb_46C(&ST(0x30), 0);
            CB->group(0);
            return;
        }

        if (ST(0x198) == 0) {
            if (ST(0x194) == 0) {
                CB->named_object("SP00ACT27");
                if ((i32)ST(0x6C) <= 2)
                    CB->named_object_int("L01D21D", 1);
            }
            if (ST(0x194) == 1) {
                CB->named_object("SP01ACT27");
                if ((i32)ST(0x6C) <= 2)
                    CB->named_object_int("L01D21D", 1);
            }
            if (ST(0x194) == 2) {
                CB->named_object("SP02ACT27");
                if ((i32)ST(0x6C) <= 2)
                    CB->named_object_int("L01D21B", 2);
            }
            if (ST(0x194) == 3) {
                CB->named_object("SP03ACT27");
                if ((i32)ST(0x6C) <= 2)
                    CB->named_object_int("L01D21C", 2);
            }
            ST(0x6C)++;
            CB->named_object("ar_10");
            CB->cb_46C(&ST(0x30), 0);
            CB->group(0);
            return;
        }
    }
}

static void trig05(void) {   /* sub_1000D24F */
    if (ST(0xC) != 1) return;
    if (ST(0x30) != 0) return;
    if (!CB->query_944(0x13)) return;

    ST(0x700) = 2;
    CB->group(3);
    ST(0x30) = 1;
    CB->cb_93C(5);
    CB->cb_93C(0x13);
    CB->named_object("SG00PRE07");
    CB->named_ex_988("OB38A", -1, -1, -1, -1, -1, -1, -3, 0);
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
}

static void trig07(void) {   /* sub_1000D319 */
    if (CB->query_998(0xAFC8) == 0) goto L_E241;

    if (ST(0xEC0) != 0) goto L_DEB6;
    if (ST(0xC) != 1) goto L_DBFA;
    if (ST(0x30) != 0) goto L_DBFA;
    ST(0x30) = 1;
    CB->group(3);
    ST(0x98) = 1;
    CB->cb_458(-7, 2);                          /* ecx=0xFFFFFFF9, edx=2 */

    if (ST(0x194) != 0) {
        if (ST(0x194) != 1) goto L_DBFA;
    }
    /* D3BC */
    CB->actor_934(-ST(0x194), 0, 0, 0, 0xC);    /* ecx=-[194], edx=0, stack(last->first)=0,0,0xC */

    if (ST(0x708) != 0) goto L_D451;
    if (ST(0x320) != 0) goto L_D451;
    ST(0x320) = 1;
    ST(0x708) = 1;
    CB->named_two_ints(0, -1);                  /* ecx=0, edx=-1 */
    action_register("L01D02", 0, "SG00DIA05", NULL);
    CB->cb_46C(&ST(0x30), 0);
    goto L_E631;

L_D451:
    if ((i32)ST(0x708) <= 0) goto L_D4C8;
    if (ST(0x68) != 0) goto L_D4C8;
    ST(0x68) = 1;
    CB->state_toggle(1, 1);
    CB->named_object("SP12LEV01");
    CB->named_object("P12ARR03_10");
    CB->cb_46C(&ST(0x324), 1);
    CB->state_toggle(0, 1);

L_D4C8:
    if (ST(0xBA0) != 0) goto L_D85E;
    if (ST(0x198) != 1) goto L_D6D2;
    if (ST(0x708) != 1) goto L_D5A3;
    action_register("L01D04A", 0, "SG00DIA07", NULL);
    action_register("L01D05A", 0xC, "P12DIA50", "SP12ARR03_10");
    action_register("L01D04B", 0, "SG00DIA06", NULL);
    action_register("L01D05B", 0xC, "SP12DIA56", "SP12ARR03_10");
    CB->named_object_int("L01D05C", 0xC);
    CB->named_object_int("L01D05D", 0xC);
    ST(0x708) = 2;
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_D5A3:
    if (ST(0x708) != 2) goto L_D667;
    ST(0x708) = 3;
    action_register("L01D06A", 0, "SG00DIA06", NULL);
    action_register("L01D06B", 0, "SG00DIA02", NULL);
    CB->named_object_int("L01D07", 0xC);
    action_register("L01D08A", 0, "SG00DIA01", NULL);
    action_register("L01D08B", 0, "SG00DIA07", NULL);
    action_register("L01D09A", 0xC, "SP12DIA59", "SP12ARR03_10");
    CB->named_object_int("L01D09Z", 0xC);
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_D667:
    if ((i32)ST(0x708) < 3) goto L_D6D2;
    ST(0x708) = 4;
    action_register("L01D10", 0, "SG00DIA03", NULL);
    action_register("L01D11", 0xC, "SP12DIA50", "SP12ARR03_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_D6D2:
    if (ST(0x198) != 0) goto L_D85E;
    if (ST(0x708) != 1) goto L_D762;
    action_register("L01D13A", 0, "SG00DIA03", NULL);
    action_register("L01D14", 0xC, "P12DIA59", "SP12ARR03_10");
    action_register("L01D13B", 0, "SG00DIA07", NULL);
    ST(0x708) = 2;
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_D762:
    if (ST(0x708) != 2) goto L_D7FC;
    action_register("L01D15", 0, "SG00DIA01", "sar_1000");
    action_register("L01D16", 0, "SG00DIA06", NULL);
    action_register("L01D09A", 0xC, "SP12DIA50", "SP12ARR03_10");
    action_register("L01D09B", 0xC, "SP12DIA59", "SP12ARR03_10");
    ST(0x708) = 3;
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_D7FC:
    if ((i32)ST(0x708) < 3) goto L_D85E;
    action_register("L01D18", 1, "SG01DIA05", "Sar_10");
    action_register("L01D19", 0xC, "SP12DIA59", "SP12ARR03_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_D85E:
    if (ST(0xBA0) != 1) goto L_DA20;
    if (ST(0x8B8) != 0) goto L_D97D;
    ST(0x8B8) = 1;
    if (ST(0x198) != 1) goto L_D911;
    action_register("L01D46A", 0, "SG00DIA04", NULL);
    action_register("L01D20A", 0xC, "SP12DIA50", "SP12ARR03_10");
    action_register("L01D46B", 0, "SG00DIA03", "sar_750");
    action_register("L01D46C", 0, "G00DIA05", NULL);
    action_register("L01D20B", 0xC, "SP12DIA59", "SP12ARR03_10");

L_D911:
    if (ST(0x198) != 0) goto L_D95F;
    action_register("L01D47", 0, "SG00DIA03", NULL);
    action_register("L01D26A", 0xC, "SP12DIA50", "SP12ARR03_10");
    CB->named_object_int("L01D26B", 0xC);

L_D95F:
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_D97D:
    if (ST(0x8B8) != 1) goto L_DA20;
    if (ST(0x198) != 1) goto L_D9C4;
    action_register("L01D88A", 0, "SG00DIA02", NULL);
    CB->named_object_int("L01D89A", 0xC);

L_D9C4:
    if (ST(0x198) != 0) goto L_DA02;
    action_register("L01D88B", 0, "SG00DIA09", NULL);
    action_register("L01D89B", 0xC, "SP12DIA50", "SP12ARR03_10");

L_DA02:
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_DA20:
    if ((i32)ST(0xBA0) < 2) goto L_DBFA;
    if (ST(0x710) != 0) goto L_DB32;
    ST(0x710) = 1;
    if (ST(0x198) != 1) goto L_DA94;
    action_register("L01D24A", 1, "SG00DIA07", NULL);
    action_register("L01D25A", 0xC, "SP12DIA50", "SP12ARR03_10");

L_DA94:
    if (ST(0x198) != 0) goto L_DAD5;
    action_register("L01D83", 1, "SG00DIA07", NULL);
    action_register("L01D84A", 0xC, "SP12DIA50", "SP12ARR03_10");

L_DAD5:
    CB->named_object_int("L01D25B", 0xC);
    action_register("L01D24B", 1, "SG00DIA06", NULL);
    action_register("L01D25C", 0xC, "SP12DIA50", "SP12ARR03_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_DB32:
    if (ST(0x710) != 1) goto L_DBFA;
    if (ST(0x198) != 1) goto L_DB85;
    action_register("L01D27A", 1, "SG00DIA04", NULL);
    action_register("L01D28", 0xC, "SP12DIA50", "SP12ARR03_10");

L_DB85:
    if (ST(0x198) != 0) goto L_DBDC;
    action_register("L01D85A", 1, "SG00DIA02", NULL);
    action_register("L01D28", 0xC, "SP12DIA50", "SP12ARR03_10");
    action_register("L01D85B", 1, "SG00DIA06", NULL);

L_DBDC:
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_DBFA:
    if (ST(0x14) != 1) goto L_DEB6;
    ST(0x14) = -1;
    ST(0x320) = 2;
    ST(0x98) = 1;
    if (ST(0xF90) == 6) goto L_DC55;
    CB->cb_458(6, 2);
L_DC55:
    CB->named_two_ints(0, -1);
    if (ST(0x708) != 0) goto L_DC83;
    ST(0x708) = 1;
L_DC83:
    CB->group(3);
    if ((i32)ST(0x194) > 1) goto L_DEB6;
    if (ST(0x714) == 1) goto L_DD5F;
    if (ST(0x68) != 0) goto L_DD08;
    ST(0x68) = 1;
    action_register("L01D41", 0xC, "SP12ATT01", "SP12LEV01");
    CB->named_object("P12ARR03_10");
    CB->op_9c(0xC, 0xC, 1);
    goto L_DD21;
L_DD08:
    action_register("L01D41", 0xC, "SP12ATT05", "SP12ARR03_10");
L_DD21:
    CB->cb_46C(&ST(0x88), 0x29A);
    CB->named_object("Sar_10");
    ST(0x714) = 1;
    CB->group(0);
    goto L_E631;

L_DD5F:
    if (ST(0x714) != 1) goto L_DEB6;
    if (CB->layer_op(2, 0, 0) != 0) goto L_DDC8;
    if (ST(0x68) != 1) goto L_DDB0;
    action_register("L01D42", 0xC, "SP12ATT05", "SP12ARR03_10");
L_DDB0:
    CB->named_object("Sar_10");
    CB->group(0);
    goto L_E631;

L_DDC8:
    if (CB->layer_op(2, 0, 0) == 0) goto L_DEB6;
    CB->layer_op(0xC, 0xC, 0);
    ST(0x68) = 0;
    ST(0x320) = 0x3E8;
    ST(0x88) = 0x3E8;
    ST(0x324) = 0;
    if (ST(0x68) != 0) goto L_DE52;
    action_register("L01D43A", 0xC, "SP12ATT02", "SP12ATT03_100");
L_DE52:
    action_register("L01D98", 0, "SP00ATA08", NULL);
    CB->named_object_int("L01D43B", 0xC);
    CB->cb_46C(&ST(0x78), 1);
    CB->state_toggle(1, 1);
    CB->named_object("P12ATT03_1000");
    CB->state_toggle(0, 1);
    goto L_E631;

L_DEB6:
    if (ST(0xEC0) == 0) goto L_E197;
    if (CB->layer_op(0x1D, 0x58, 0) != 0) goto L_E197;
    if (CB->layer_op(0x1D, 0x59, 0) != 0) goto L_E197;
    if (ST(0xC) != 1) goto L_E197;
    if (ST(0x30) != 0) goto L_E197;
    ST(0x30) = 1;
    CB->group(3);
    if (ST(0x198) != 1) goto L_E064;

    if (ST(0x194) == 0) {
        CB->named_object("SP00ACT26");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21A", 1);
    }
    /* DF7F */
    if (ST(0x194) == 1) {
        CB->named_object("SP01ACT26");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21A", 1);
    }
    /* DFBB */
    if (ST(0x194) == 2) {
        CB->named_object("SP02ACT26");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21B", 2);
    }
    /* DFF7 */
    if (ST(0x194) == 3) {
        CB->named_object("SP03ACT26");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21C", 2);
    }
    /* E033 */
    ST(0x6C)++;
    CB->named_object("ar_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_E064:
    if (ST(0x198) != 0) goto L_E197;
    if (ST(0x194) == 0) {
        CB->named_object("SP00ACT27");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21D", 1);
    }
    /* E0B2 */
    if (ST(0x194) == 1) {
        CB->named_object("SP01ACT27");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21D", 1);
    }
    /* E0EE */
    if (ST(0x194) == 2) {
        CB->named_object("SP02ACT27");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21B", 2);
    }
    /* E12A */
    if (ST(0x194) == 3) {
        CB->named_object("SP03ACT27");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21C", 2);
    }
    /* E166 */
    ST(0x6C)++;
    CB->named_object("ar_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_E197:
    if (ST(0xC) != 1) goto L_E1EC;
    if (ST(0x30) != 0) goto L_E1EC;
    if (CB->query_954(0x58) == 0) goto L_E1EC;
    if (CB->layer_op(0x1D, 0x58, 0) == 0) goto L_E1EC;
    trig17();
    goto L_E631;

L_E1EC:
    if (ST(0xC) != 1) goto L_E241;
    if (ST(0x30) != 0) goto L_E241;
    if (CB->query_954(0x59) == 0) goto L_E241;
    if (CB->layer_op(0x1D, 0x59, 0) == 0) goto L_E241;
    trig18();
    goto L_E631;

L_E241:
    if (CB->query_998(0x3A98) == 0) goto L_E631;
    if (ST(0xC) != 1) goto L_E631;
    if (ST(0x30) != 0) goto L_E631;
    ST(0x30) = 1;
    CB->group(3);
    if (ST(0xF90) != 6) goto L_E2AB;
    CB->cb_458(0x63, 1);
L_E2AB:
    if (ST(0x198) != 1) goto L_E46E;

    if (ST(0x194) == 0) {
        CB->named_object("SP00ACT26");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21A", 1);
        if (ST(0xBA0) == 1) CB->named_object_int("L01D29A", 0xC);
        if (ST(0x68) == 1) CB->cb_46C(&ST(0x324), 1);
    }
    /* E341 */
    if (ST(0x194) == 1) {
        CB->named_object("SP01ACT26");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21A", 1);
        if (ST(0xBA0) == 1) CB->named_object_int("L01D29B", 0xC);
        if (ST(0x68) == 1) CB->cb_46C(&ST(0x324), 1);
    }
    /* E3C5 */
    if (ST(0x194) == 2) {
        CB->named_object("SP02ACT26");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21B", 2);
    }
    /* E401 */
    if (ST(0x194) == 3) {
        CB->named_object("SP03ACT26");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21C", 2);
    }
    /* E43D */
    ST(0x6C)++;
    CB->named_object("ar_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_E46E:
    if (ST(0x198) != 0) goto L_E631;
    if (ST(0x194) == 0) {
        CB->named_object("SP00ACT27");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21D", 1);
        if (ST(0xBA0) == 1) CB->named_object_int("L01D29C", 0xC);
        if (ST(0x68) == 1) CB->cb_46C(&ST(0x324), 1);
    }
    /* E504 */
    if (ST(0x194) == 1) {
        CB->named_object("SP01ACT27");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21D", 1);
        if (ST(0xBA0) == 1) CB->named_object_int("L01D29C", 0xC);
        if (ST(0x68) == 1) CB->cb_46C(&ST(0x324), 1);
    }
    /* E588 */
    if (ST(0x194) == 2) {
        CB->named_object("SP02ACT27");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21B", 2);
    }
    /* E5C4 */
    if (ST(0x194) == 3) {
        CB->named_object("SP03ACT27");
        if ((i32)ST(0x6C) <= 2) CB->named_object_int("L01D21C", 2);
    }
    /* E600 */
    ST(0x6C)++;
    CB->named_object("ar_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto L_E631;

L_E631:
    return;
}

static void trig08(void) {   /* sub_1000E636 */
    if (ST(0x0C) != 1) return;
    if (ST(0x30) != 0) return;
    if (ST(0x10) != 0) return;
    if (!CB->query_944(0xC)) return;

    if (ST(0x198) == 1) {
        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();
        if (ST(0x194) == 0) CB->named_object("SP00ACT24");
        if (ST(0x194) == 1) CB->named_object("SP01ACT24");
        if (ST(0x194) == 2) CB->named_object("SP02ACT24");
        if (ST(0x194) == 3) CB->named_object("SP03ACT24");
        if (ST(0x194) == 0) action_register("L01D23A", 0, "G00DIA04", NULL);
        if (ST(0x194) == 1) action_register("L01D23A", 1, "G00DIA04", NULL);
        if (ST(0x194) == 2) {
            CB->named_object("P00_65535");
            CB->named_object("ar_10");
            CB->named_object_int("L01D23B", 2);
        }
        if (ST(0x194) == 3) {
            CB->named_object("P00_65535");
            CB->named_object("ar_10");
            CB->named_object_int("L01D23C", 3);
        }
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        return;
    }

    if (ST(0x198) != 0) return;
    CB->group(3);
    ST(0x30) = 1;
    e_spawn_ar();
    if (ST(0x194) == 0) CB->named_object("SP00ACT25");
    if (ST(0x194) == 1) CB->named_object("SP01ACT25");
    if (ST(0x194) == 2) CB->named_object("SP02ACT25");
    if (ST(0x194) == 3) CB->named_object("SP03ACT25");
    CB->named_object("ar_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
}

static void trig09(void) {   /* sub_1000E991 */
    /* ---- block @0x1000E991 ---- */
    if (ST(0xC) != 1) goto L_EABD;
    if (ST(0x30) != 0) goto L_EABD;
    if (ST(0x10) != 0) goto L_EABD;
    if (CB->query_998(0xBB80) == 0) goto L_EABD;
    if (CB->layer_op(9, ST(0x194), 0x1C) == 0) goto L_EABD;
    if (CB->query_954(0x57) != 0) goto L_EABD;
    CB->group(3);
    if ((i32)ST(0x194) <= 1)
        action_register("LCOD52", 1, "SG00SAV01", "Sar_10");
    if (ST(0x194) == 2)
        action_register("LCOD53", 2, "SG00SAV01", "Sar_10");
    if (ST(0x194) == 3)
        action_register("LCOD54", 3, "SG00SAV01", "Sar_10");
    CB->named_object("P00_65535");
    CB->op_9c(0x16, 0, 0);
    CB->group(0);
    goto done;

L_EABD:
    if (ST(0xC) != 1) goto L_ECA1;
    if (ST(0x2EC) != 1) goto L_ECA1;
    if (ST(0x34) != 0) goto L_ECA1;
    if (ST(0x30) != 0) goto L_ECA1;
    if (CB->query_954(0x57) != 0) goto L_ECA1;
    if (CB->query_998(0x4E20) == 0) goto L_ECA1;
    if (CB->layer_op(9, ST(0x194), 0xB) == 0) goto L_ECA1;
    ST(0x30) = 1;
    CB->group(3);
    if (ST(0x198) == 1) {
        if (ST(0x194) == 0 || ST(0x194) == 1) CB->named_object("SP01ACT28");
        if (ST(0x194) == 2) CB->named_object("SP02ACT28");
        if (ST(0x194) == 3) CB->named_object("SP03ACT28");
        CB->named_object("ar_200");
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        goto done;
    }
    if (ST(0x198) != 0) goto L_ECA1;
    if (ST(0x194) == 0 || ST(0x194) == 1) CB->named_object("SP01ACT29");
    if (ST(0x194) == 2) CB->named_object("SP02ACT29");
    if (ST(0x194) == 3) CB->named_object("SP03ACT29");
    CB->named_object("ar_200");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    goto done;

L_ECA1:
    if (ST(0xC) != 1) goto L_10227;
    if (CB->query_954(0x57) == 0) goto L_10227;
    if (ST(0x30) != 0) goto L_10227;
    if (ST(0x2EC) != 1) goto L_1014C;
    if (ST(0x34) != 0) goto L_1014C;
    ST(0x30) = 1;
    CB->group(3);
    CB->select_468(-87);
    if (ST(0x19C) != 0) {
        if (CB->layer_op(2, 0, 2) == 0) {
            if (ST(0x19C) == 0) goto L_FF9D;
            if (ST(0x19C) != ST(0x40)) goto L_FF9D;
        }
    }

    /* ---- @0x1000ED6A : ST(0x194)==1 dialogue state machine (0x898) ---- */
    if (ST(0x194) != 1) goto L_F130;
    if (ST(0x898) != 0) goto L_EF2E;
    ST(0x898) = 1;
    if (ST(0x198) == 1) {
        action_register("L01D48",  1,    "SP01ATT70", NULL);
        action_register("L01D49A", 0x57, "SP87DIA56", "SP87ARR01");
        action_register("L01D49B", 0x57, "SP87DIA53", "SP87ARR01");
        action_register("L01D50",  1,    "SG01DIA02", NULL);
        action_register("L01D51A", 0x57, "SP87DIA55", "SP87ARR01_10");
        action_register("L01D51B", 0x57, "SP87DIA50", "SP87ARR01_10");
        action_register("L01D51C", 0x57, "SP87DIA60", "SP87ARR01_10");
    }
    if (ST(0x198) == 0) {
        action_register("L01D52",  1,    "SG01DIA03", NULL);
        action_register("L01D49A", 0x57, "SP87DIA56", "SP87ARR01");
        action_register("L01D49B", 0x57, "SP87DIA53", "SP87ARR01");
        action_register("L01D53",  1,    "SG01DIA08", NULL);
        action_register("L01D54A", 0x57, "SP87DIA62", "SP87ARR01_10");
        action_register("L01D54B", 0x57, "SP87DIA59", "SP87ARR01_10");
    }
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_EF2E:
    if (ST(0x898) != 1) goto L_EFCB;
    ST(0x898) = 2;
    action_register("L01D128",  1,    "SG01DIA06", NULL);
    action_register("L01D129A", 0x57, "SP87DIA63", "SP87ARR01_10");
    action_register("L01D129B", 0x57, "SP87DIA64", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_EFCB:
    if (ST(0x898) != 2) goto L_F068;
    ST(0x898) = 3;
    action_register("L01D130",  1,    "SG01DIA03", NULL);
    action_register("L01D131A", 0x57, "SP87DIA63", "SP87ARR01_10");
    action_register("L01D131B", 0x57, "SP87DIA65", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_F068:
    if (ST(0x898) != 3) goto L_F130;
    if (ST(0x198) == 1) {
        action_register("L01D132", 1,    "SG01DIA01", NULL);
        action_register("L01D133", 0x57, "SP87DIA54", "SP87ARR01_10");
    }
    if (ST(0x198) == 0) {
        action_register("L01D134", 1,    "SG01DIA01", NULL);
        action_register("L01D135", 0x57, "SP87DIA54", "SP87ARR01_10");
    }
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

    /* ---- @0x1000F130 : ST(0x194)>=2 ---- */
L_F130:
    if ((i32)ST(0x194) < 2) goto L_FF9D;
    if (ST(0xB04) != 0) goto L_F7B4;
    if (ST(0xB88) != 0) goto L_F522;

    if (ST(0x8B4) != 1) goto L_F2FE;
    ST(0x8B4) = 2;
    if (ST(0x194) == 2) {
        if (ST(0x198) == 1) {
            action_register("L01D55A", 2, "SG02DIA07", NULL);
            action_register("L01D55B", 2, "SG02DIA07", NULL);
            action_register("L01D55C", 2, "SG02DIA07", NULL);
        }
        if (ST(0x198) == 0) {
            action_register("L01D56A", 2, "SG02DIA07", NULL);
            action_register("L01D56B", 2, "SG02DIA07", NULL);
            action_register("L01D56C", 2, "SG02DIA07", NULL);
        }
    }
    if (ST(0x194) == 3) {
        action_register("L01D57A", 3, "SG03DIA07", NULL);
        action_register("L01D57B", 3, "SG03DIA07", NULL);
    }
    action_register("L01D59A", 0x57, "SP87DIA50", "SP87ARR01_10");
    action_register("L01D59B", 0x57, "SP87DIA64", "SP87ARR01_10");
    action_register("L01D59C", 0x57, "SP87DIA62", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_F2FE:
    if (ST(0x8B4) != 2) goto L_F420;
    ST(0x8B4) = 3;
    if (ST(0x194) == 2) action_register("L01D62A", 2, "SG02DIA01", NULL);
    if (ST(0x194) == 3) action_register("L01D62B", 3, "SG03DIA01", NULL);
    action_register("L01D63A", 0x57, "SP87DIA50", "SP87ARR01_10");
    action_register("L01D63B", 0x57, "SP87DIA51", "SP87ARR01_10");
    action_register("L01D63C", 0x57, "SP87DIA56", "SP87ARR01_10");
    action_register("L01D63D", 0x57, "SP87DIA53", "SP87ARR01_10");
    action_register("L01D63E", 0x57, "SP87DIA54", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_F420:
    if ((i32)ST(0x8B4) < 3) goto L_F522;
    if (ST(0x194) == 2) action_register("L01D64A", 2, "SG02DIA04", NULL);
    if (ST(0x194) == 3) action_register("L01D64B", 3, "SG03DIA04", NULL);
    action_register("L01D65A", 0x57, "SP87DIA55", "SP87ARR01_10");
    if (ST(0x8B4) == 3) {
        action_register("L01D65B", 0x57, "SP87DIA56", "SP87ARR01_10");
        action_register("L01D65C", 0x57, "SP87DIA57", "SP87ARR01_10");
        ST(0x8B4) = 4;
    }
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

    /* ---- @0x1000F522 : ST(0xB88) branch, state 0x77C ---- */
L_F522:
    if (ST(0xB88) == 0) goto L_F7B4;
    if (ST(0x77C) != 0) goto L_F6B2;
    ST(0x77C) = 1;
    if (ST(0x194) == 2) {
        action_register("L01D55A",  2, "SG02DIA07", NULL);
        action_register("L01D173A", 2, "SG02DIA07", NULL);
        action_register("L01D173B", 2, "SG02DIA07", NULL);
    }
    if (ST(0x194) == 3) action_register("L01D174", 3, "SG03DIA01", NULL);
    if ((i32)ST(0x8B4) < 3) {
        action_register("L01D63C", 0x57, "SP87DIA60", "SP87ARR01_10");
        action_register("L01D63D", 0x57, "SP87DIA61", "SP87ARR01_10");
        action_register("L01D63E", 0x57, "SP87DIA62", "SP87ARR01_10");
    } else {
        action_register("L01D175A", 0x57, "SP87DIA56", "SP87ARR01_10");
        action_register("L01D175B", 0x57, "SP87DIA57", "SP87ARR01_10");
        action_register("L01D175C", 0x57, "SP87DIA59", "SP87ARR01_10");
    }
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_F6B2:
    if ((i32)ST(0x77C) < 1) goto L_F7B4;
    if (ST(0x194) == 2) action_register("L01D64A", 2, "SG02DIA04", NULL);
    if (ST(0x194) == 3) action_register("L01D64B", 3, "SG03DIA04", NULL);
    action_register("L01D65A", 0x57, "SP87DIA53", "SP87ARR01_10");
    if (ST(0x77C) == 1) {
        action_register("L01D65B", 0x57, "SP87DIA63", "SP87ARR01_10");
        action_register("L01D65C", 0x57, "SP87DIA64", "SP87ARR01_10");
        ST(0x77C) = 2;
    }
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

    /* ---- @0x1000F7B4 : ST(0xEC4)==1, state 0x8BC ---- */
L_F7B4:
    if (ST(0xEC4) != 1) goto L_FB1A;
    if (ST(0x8BC) != 0) goto L_F911;
    ST(0x8BC) = 1;
    if (ST(0x194) == 2) {
        if (ST(0x198) == 1) action_register("L01D68", 2, "SG02DIA04", NULL);
        if (ST(0x198) == 0) action_register("L01D69", 2, "SG03DIA04", NULL);
    }
    if (ST(0x194) == 3) {
        if (ST(0x198) == 1) action_register("L01D70", 3, "SG03DIA04", NULL);
        if (ST(0x198) == 0) action_register("L01D71", 3, "SG03DIA04", NULL);
    }
    action_register("L01D72A", 0x57, "SP87DIA53", "SP87ARR01_10");
    action_register("L01D72B", 0x57, "SP87ATT50", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_F911:
    if (ST(0x8BC) != 1) goto L_F9E8;
    ST(0x8BC) = 2;
    if (ST(0x194) == 2) action_register("L01D73", 2, "SG02DIA04", NULL);
    if (ST(0x194) == 3) action_register("L01D74", 3, "SG03DIA04", NULL);
    action_register("L01D75A", 0x57, "SP87DIA50", "SP87ARR01_10");
    action_register("L01D75B", 0x57, "SP87DIA53", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_F9E8:
    if (ST(0x8BC) != 2) goto L_FB1A;
    ST(0x8BC) = 3;
    if (ST(0x194) == 2) {
        if (ST(0x198) == 1) action_register("L01D76", 2, "SG00DIA04", NULL);
        if (ST(0x198) == 0) action_register("L01D77", 2, "SG00DIA04", NULL);
    }
    if (ST(0x194) == 3) {
        if (ST(0x198) == 1) action_register("L01D78", 2, "SG00DIA04", NULL);
        if (ST(0x198) == 0) action_register("L01D79", 2, "SG00DIA04", NULL);
    }
    action_register("L01D146", 0x57, "SP87ATT70", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

    /* ---- @0x1000FB1A : ST(0xEC4)==2, state 0xCD8 ---- */
L_FB1A:
    if (ST(0xEC4) != 2) goto L_FCC1;
    if (ST(0xCD8) != 0) goto L_FC1C;
    ST(0xCD8) = 1;
    if (ST(0x194) == 2) action_register("L01D108", 2, "SG02DIA04", NULL);
    if (ST(0x194) == 3) action_register("L01D109", 3, "SG03DIA04", NULL);
    action_register("L01D110A", 0x57, "SP87ATT70", "SP87ARR01_10");
    action_register("L01D110B", 0x57, "SP87DIA60", "SP87ARR01_10");
    action_register("L01D110C", 0x57, "SP87DIA62", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_FC1C:
    if (ST(0xCD8) != 1) goto L_FCC1;
    ST(0xCD8) = 2;
    if (ST(0x194) == 2) action_register("L01D111", 2, "SG02DIA04", NULL);
    if (ST(0x194) == 3) action_register("L01D112", 3, "SG03DIA04", NULL);
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

    /* ---- @0x1000FCC1 : state 0xCD4 ---- */
L_FCC1:
    if (ST(0x8BC) == 3) goto L_FCE5;
    if (ST(0xCD8) != 2) goto L_FF9D;
L_FCE5:
    if (ST(0xCD4) != 0) goto L_FDBC;
    ST(0xCD4) = 1;
    if (ST(0x194) == 2) action_register("L01D200", 2, "SG02DIA01", NULL);
    if (ST(0x194) == 3) action_register("L01D201", 3, "SG03DIA01", NULL);
    action_register("L01D202A", 0x57, "SP87DIA53", "SP87ARR01_10");
    action_register("L01D202B", 0x57, "SP87DIA56", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_FDBC:
    if (ST(0xCD4) != 1) goto L_FE7A;
    ST(0xCD4) = 2;
    if (ST(0x194) == 2) action_register("L01D203", 2, "SG02DIA05", NULL);
    if (ST(0x194) == 3) action_register("L01D204", 3, "SG03DIA05", NULL);
    action_register("L01D205", 0x57, "SP87DIA55", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

L_FE7A:
    if (ST(0xCD4) != 2) goto L_FF9D;
    if (ST(0x194) == 2) {
        if (ST(0x198) == 1) action_register("L01D206", 2, "SG02DIA05", NULL);
        if (ST(0x198) == 0) action_register("L01D207", 2, "SG02DIA05", NULL);
    }
    if (ST(0x194) == 3) {
        if (ST(0x198) == 1) action_register("L01D208", 3, "SG03DIA05", NULL);
        if (ST(0x198) == 0) action_register("L01D209", 3, "SG03DIA05", NULL);
    }
    action_register("L01D210", 0x57, "SP87DIA54", "SP87ARR01_10");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

    /* ---- @0x1000FF9D : prefetch-slot change handling ---- */
L_FF9D:
    if (ST(0x19C) == 0) goto L_1014C;
    if (CB->layer_op(2, 0, 2) != 0) goto L_1014C;
    if (ST(0x19C) == ST(0x40)) goto L_1014C;
    ST(0x40) = ST(0x19C);
    if (ST(0x194) == 1) action_register("L01D105", 1, "SG00PRE80", "SG00PRE81");
    if (ST(0x194) == 2) action_register("L01D106", 2, "SG00PRE80", "SG00PRE81");
    if (ST(0x194) == 3) action_register("L01D107", 3, "SG00PRE80", "SG00PRE81");
    ST(0x928) = CB->query_940(3);
    if (ST(0x928) == 0) action_register("L01D92A", 0x57, "SP87ATT60", "SP87ARR01_10");
    if (ST(0x928) == 1) action_register("L01D92B", 0x57, "SP87ATT61", "SP87ARR01_10");
    if (ST(0x928) == 2) action_register("L01D92C", 0x57, "SP87ATT62", "SP87ARR01_10");
    CB->named_object("G00PRE82");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
    CB->flush_958(0x57, ST(0x194));
    goto done;

    /* ---- @0x1001014C ---- */
L_1014C:
    if (ST(0x2EC) != 1) goto L_10205;
    if (ST(0x34) != 0x57) goto L_10205;
    if (ST(0x10) != 0) goto L_10205;
    CB->group(3);
    if (ST(0x194) == 1) CB->named_object_int("L01D38", 1);
    if (ST(0x194) == 2) CB->named_object_int("L01D39", 2);
    if (ST(0x194) == 3) CB->named_object_int("L01D40", 3);
    CB->named_object_int("L01D93", 0x57);
    CB->group(0);
L_10205:
    if (ST(0x2EC) != 2) goto L_10227;
    CB->cb_454(0x57);
    CB->cb_454(0x57);
    goto done;

    /* ---- @0x10010227 : end-of-level / return sequence ---- */
L_10227:
    if ((i32)ST(0x14) < 1) goto L_1026A;
    if (ST(0x34) != 0) goto L_1026A;
    if (CB->query_944(9) == 0) goto L_1026A;
    if (ST(0x2EC) != 2) goto L_102BF;
L_1026A:
    if (ST(0x10C) != 1) goto done;
    if (ST(0x34) != 0) goto done;
    if (ST(0x10) != 1) goto done;
    if (CB->query_954(0x57) == 0) goto done;
    if (ST(0x2EC) != 1) goto done;
L_102BF:
    CB->group(3);
    CB->select_468(-87);
    ST(0x14) = -1;
    ST(0x34) = 0x57;
    CB->named_object("SP87ESA14");
    if (ST(0x8C8) == 0) {
        CB->named_object("P87_65535");
        CB->named_two_ints(2, 0x64);
        goto done;
    }
    if (ST(0x8C8) != 1) goto done;
    ST(0xF04) = 1;
    ST(0x8C8) = 2;
    CB->named_object_int("L01D86", 0x57);
    if (ST(0xEC4) == 1) goto L_10396;
    CB->group(0);
    CB->cb_970(0x57, 0);
    goto done;

L_10396:
    if (ST(0xEC4) != 1) goto done;
    if (ST(0x2F0) != 1) goto done;
    if (ST(0x94) != 1) goto L_10413;
    CB->group(0);
    CB->cb_970(0x57, 0);
    CB->state_toggle(2, 1);
    CB->named_object_int("L01D136", 0x58);
    CB->cb_970(0x58, 0);
    goto done;

L_10413:
    if (ST(0x94) != 0) goto done;
    ST(0x94) = 1;
    CB->state_toggle(2, 1);
    CB->cb_458(6, 2);
    CB->cb_964(1, 0);
    CB->named_object("SP88LEV01");
    CB->named_object("P88_65535");
    CB->cb_964(1, 1);
    CB->named_object_int("L01D136", 0x58);
    CB->cb_458(0x63, 1);
    CB->group(0);
    CB->cb_970(0x58, 0);
    CB->cb_970(0x57, 0);
    goto done;

done:
    return;
}

static void trig11(void) {   /* sub_1001053A */
    if (ST(0x194) != 4 && ST(0xC) == 1 && ST(0x30) == 0 && ST(0x34) == 0 &&
        ST(0x10) == 0 &&
        (i32)CB->layer_op(1, ST(0x194), 1) > (i32)0xFFFFF63C &&
        CB->query_998(0x4000) != 0 &&
        (i32)CB->layer_op(1, ST(0x194), 2) < 0x55F0 &&
        (i32)CB->layer_op(1, ST(0x194), 2) > 0x32C8) {

        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();

        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) CB->named_object("SP00ACT11");
            if (ST(0x198) == 0) CB->named_object("SP00ACT86");
        }
        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) CB->named_object("SP01ACT11");
            if (ST(0x198) == 0) CB->named_object("SP01ACT86");
        }
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) {
                if (CB->layer_op(2, 9, 2) == 0) CB->named_object("SP02ACT11");
                else                        CB->named_object("SP02ACT12");
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 9, 2) == 0) CB->named_object("SP02ACT86");
                else                        CB->named_object("SP02ACT85");
            }
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) {
                if (CB->layer_op(2, 9, 2) == 0) CB->named_object("SP03ACT11");
                else                        CB->named_object("SP03ACT12");
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 9, 2) == 0) CB->named_object("SP03ACT86");
                else                        CB->named_object("SP03ACT85");
            }
        }
        CB->named_object("ar_50");
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
    } else {
        if (CB->query_944(0xD) != 0 && ST(0xC) == 1 && ST(0x30) == 0) {
            ST(0x30) = 1;
            CB->group(3);
            e_spawn_ar();

            if (ST(0x198) == 1) {
                if ((i32)ST(0x194) <= 1) CB->named_object("SP01ACT28");
                if (ST(0x194) == 2)      CB->named_object("SP02ACT28");
                if (ST(0x194) == 3)      CB->named_object("SP03ACT28");
            }
            if (ST(0x198) == 0) {
                if ((i32)ST(0x194) <= 1) CB->named_object("SP01ACT29");
                if (ST(0x194) == 2)      CB->named_object("SP02ACT29");
                if (ST(0x194) == 3)      CB->named_object("SP03ACT29");
            }
            CB->named_object("ar_200");
            CB->cb_46C(&ST(0x30), 0);
            CB->group(0);
        } else if (CB->query_998(0x7D00) != 0 &&
                   CB->layer_op(9, ST(0x194), 0x1B) != 0) {
            trig27();
        }
    }
}

static void trig15(void) {   /* sub_10010A95 */
    if (ST(0x0C) != 1) return;
    if (ST(0x30) != 0) return;
    if (ST(0x34) != 0) return;
    if (ST(0x10) != 0) return;
    if (CB->query_944(0xE) == 0) return;

    CB->group(3);
    ST(0x30) = 1;
    e_spawn_ar();

    if (ST(0x194) == 0) {
        if (ST(0x198) == 1) {
            ST(0x928) = CB->query_940(3);
            if (ST(0x928) == 0) action_register("LCOD06", 0, "SG00ATT01", NULL);
            if (ST(0x928) == 1) action_register("LCOD75", 0, "SG00ATT01", NULL);
            if (ST(0x928) == 2) action_register("LCOD09", 0, "SG00ATT01", NULL);
        }
        if (ST(0x198) == 0) {
            action_register("L01D176", 0, "SP00ACT86", NULL);
        }
    }

    if (ST(0x194) == 1) {
        if (ST(0x198) == 1) {
            ST(0x928) = CB->query_940(3);
            if (ST(0x928) == 0) action_register("LCOD06", 1, "SG00ATT01", NULL);
            if (ST(0x928) == 1) action_register("LCOD75", 1, "SG00ATT01", NULL);
            if (ST(0x928) == 2) action_register("LCOD09", 1, "SG00ATT01", NULL);
        }
        if (ST(0x198) == 0) {
            action_register("L01D176", 1, "SP01ACT86", "Sar_10");
        }
    }

    if (ST(0x194) == 2) {
        if (ST(0x198) == 1) {
            ST(0x928) = CB->query_940(3);
            if (ST(0x928) == 0) action_register("LCOD07", 2, "SG00ATT01", NULL);
            if (ST(0x928) == 1) action_register("LCOD76", 2, "SG00ATT01", NULL);
            if (ST(0x928) == 2) action_register("LCOD10", 2, "SG00ATT01", NULL);
        }
        if (ST(0x198) == 0) {
            if (CB->layer_op(2, 7, 2) != 0) action_register("L01D177", 2, "SP02ACT85", "Sar_10");
            if (CB->layer_op(2, 7, 2) == 0) action_register("L01D177", 2, "SP02ACT86", "Sar_10");
        }
    }

    if (ST(0x194) == 3) {
        if (ST(0x198) == 1) {
            ST(0x928) = CB->query_940(3);
            if (ST(0x928) == 0) action_register("LCOD08", 3, "SG00ATT01", NULL);
            if (ST(0x928) == 1) action_register("LCOD77", 3, "SG00ATT01", NULL);
            if (ST(0x928) == 2) action_register("LCOD11", 3, "SG00ATT01", NULL);
        }
        if (ST(0x198) == 0) {
            if (CB->layer_op(2, 7, 2) != 0) action_register("L01D178", 3, "SP03ACT85", "Sar_10");
            if (CB->layer_op(2, 7, 2) == 0) action_register("L01D178", 3, "SP03ACT86", "Sar_10");
        }
    }

    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
}

static void trig17(void) {   /* sub_10011041 */
    /* --- region 1 ------------------------------------------------------ */
    if (ST(0x0C) == 1 && ST(0x30) == 0 && ST(0x10) == 0
        && CB->query_998(0xBB80)
        && CB->layer_op(9, ST(0x194), 0x1C)
        && !CB->query_954(0x58)) {
        CB->group(3);
        if ((i32)ST(0x194) <= 1)
            action_register("LCOD52", 1, "SG00SAV01", "Sar_10");
        if (ST(0x194) == 2)
            action_register("LCOD53", 2, "SG00SAV01", "Sar_10");
        if (ST(0x194) == 3)
            action_register("LCOD54", 3, "SG00SAV01", "Sar_10");
        CB->named_object("P00_65535");
        CB->op_9c(0x16, 0, 0);
        CB->group(0);
        return;
    }

    /* --- region 2 ------------------------------------------------------ */
    if (ST(0x0C) == 1 && ST(0x34) == 0 && ST(0x30) == 0 && CB->query_954(0x58)) {
        if (ST(0x2F0) == 1) {
            ST(0x30) = 1;
            CB->group(3);
            ST(0x98) = 1;
            if (ST(0xF90) != 6)
                CB->cb_458(6, 2);

            /* decide between CD0-machine and the 0x785 relocate block */
            if (!(ST(0x19C) != 0 && !CB->layer_op(2, 0, 2) && ST(0x19C) != ST(0x40))) {
                /* loc_1001125C : CD0 == 0 */
                if (ST(0xCD0) == 0) {
                    ST(0xCD0) = 1;
                    if (ST(0x194) == 2)
                        action_register("L01D159", 2, "SG00DIA01", NULL);
                    if (ST(0x194) == 3)
                        action_register("L01D160", 2, "SG00DIA01", NULL);
                    if (ST(0x94) == 0) {
                        ST(0x94) = 1;
                        action_register("L01D97A", 0x58, "SP88LEV01", "SP88ARR01_10");
                    } else {
                        action_register("L01D97A", 0x58, "SP88DIA59", "SP88ARR01_10");
                    }
                    action_register("L01D97B", 0x58, "SP88DIA59", "SP88ARR01_10");
                    CB->cb_46C(&ST(0x30), 0);
                    CB->flush_958(0x58, ST(0x194));
                    CB->group(0);
                    return;
                }
                /* loc_10011372 : CD0 == 1 */
                if (ST(0xCD0) == 1) {
                    CB->select_468(-88);
                    CB->actor_934(0x58, 0, 0, 0, ST(0x194));
                    if (ST(0x194) == 2) {
                        if (ST(0x198) == 1)
                            action_register("L01D161", 2, "SG00DIA04", NULL);
                        if (ST(0x198) == 0)
                            action_register("L01D162", 2, "SG00DIA03", NULL);
                    }
                    if (ST(0x194) == 3) {
                        if (ST(0x198) == 1)
                            action_register("L01D163", 3, "SG00DIA04", NULL);
                        if (ST(0x198) == 0)
                            action_register("L01D164", 3, "SG00DIA03", NULL);
                    }
                    if (ST(0x198) == 1) {
                        ST(0xCD0) = 2;
                        action_register("L01D165A", 0x58, "SP88DIA55", "SP88ARR01");
                        action_register("L01D165B", 0x58, "SP88DIA62", "SP88ARR01");
                        action_register("L01D165C", 0x58, "SP88DIA62", "SP88ARR01");
                        action_register("L01D165D", 0x58, "SP88DIA62", "SP88ARR01");
                        action_register("L01D165E", 0x58, "SP88DIA60", "SP88ARR01_10");
                    }
                    if (ST(0x198) == 0) {
                        CB->actor_934(-87, 0, 0, 0, 0x58);
                        action_register("L01D166", 0x58, "SP88DIA60", "SP88ARR01_500");
                        ST(0xCD0) = 2;
                        if (ST(0xD4) == 0) {
                            CB->spawn_678(0x57, 0x140, (i32)0xFFFFF61E, 0x8CA0, 1, "P87ARR01", 9, 1);
                            CB->cb_458(2, 2);
                            if (ST(0x80) == 0)
                                action_register("L01D167", 0x57, "SP87DIA54", "SP87ARR01_10");
                            if (ST(0x80) != 0) {
                                ST(0x80) = 0;
                                CB->select_468(-87);
                                CB->named_object("SP87GAR27");
                                CB->named_object("P87ARR01_200");
                                action_register("L01D167", 0x57, "P87DIA57", "SP87ARR01_10");
                            }
                            CB->cb_46C(&ST(0xD4), 1);
                            CB->cb_458(6, 2);
                        }
                        CB->named_object("sar_100");
                        CB->cb_46C(&ST(0x30), 1);
                        CB->spawn_678(0x57, 0x140, (i32)0xFFFFF61E, 0xDD4, 1, "P87ATT01", 9, 1);
                    }
                    CB->cb_46C(&ST(0x30), 0);
                    CB->flush_958(0x58, ST(0x194));
                    CB->group(0);
                    return;
                }
                /* loc_100116C7 : CD0 == 2 */
                if (ST(0xCD0) == 2) {
                    CB->select_468(-88);
                    CB->actor_934(0x58, 0, 0, 0, ST(0x194));
                    if (ST(0x194) == 2)
                        action_register("L01D168", 2, "SG00DIA07", NULL);
                    if (ST(0x194) == 3)
                        action_register("L01D169", 3, "SG00DIA07", NULL);
                    CB->cb_46C(&ST(0x30), 0);
                    CB->flush_958(0x58, ST(0x194));
                    CB->group(0);
                    return;
                }
            }

            /* loc_10011785 : object relocated */
            if (ST(0x19C) != 0 && !CB->layer_op(2, 0, 2) && ST(0x19C) != ST(0x40)) {
                ST(0x40) = ST(0x19C);
                if (ST(0x194) == 2)
                    action_register("L01D106", 2, "SG00PRE80", "SG00PRE81");
                if (ST(0x194) == 3)
                    action_register("L01D107", 3, "SG00PRE80", "SG00PRE81");
                action_register("L01D170", 0x58, "SP88ARR02", "SP88ARR02_10");
                CB->named_object("G00PRE82");
                CB->cb_46C(&ST(0x30), 0);
                CB->flush_958(0x58, ST(0x194));
                CB->group(0);
                return;
            }
        }

        /* loc_1001188A */
        if (ST(0x2F0) == 2 && ST(0x30) == 0)
            CB->cb_454(0x58);
    }

    /* --- region 3 ------------------------------------------------------ */
    if (ST(0x0C) == 1 && ST(0x30) == 0 && ST(0x34) != 0 && ST(0x10) == 0
        && ST(0x2F0) != 2 && CB->query_954(0x58)) {
        if (ST(0x194) == 2) {
            CB->group(3);
            action_register("LCOD20", 2, "SP02ATT51", NULL);
            CB->group(0);
            return;
        }
        if (ST(0x194) == 3) {
            CB->group(3);
            action_register("LCOD21", 3, "SP03ATT51", NULL);
            CB->group(0);
            return;
        }
    }

    /* --- region 4 ------------------------------------------------------ */
    if (!((i32)ST(0x14) >= 1 && ST(0x34) == 0 && (i32)ST(0x2F0) < 2 && CB->query_954(0x58))) {
        /* loc_100119DA */
        if (!(ST(0x10C) == 1 && ST(0x34) == 0 && ST(0x10) == 1
              && CB->query_954(0x58) && ST(0x2F0) == 1))
            return;
    }

    /* loc_10011A2F */
    ST(0xF74) = 1;
    ST(0x34) = 0x57;
    ST(0x14) = -1;
    CB->group(3);
    CB->select_468(-88);
    CB->named_object("SP88ESM05");
    CB->named_object("P88_65535");
    action_register("L01D147", 0x58, "P88DIA60", "SP88GAR01_10");
    if (ST(0x2EC) == 1) {
        ST(0xF04) = 1;
        CB->named_object_int("L01D148", 0x57);
        CB->cb_970(0x57, 0);
    }
    CB->group(0);
    CB->cb_970(0x58, 0);
}

static void trig18(void) {   /* sub_10011AF3 */
    if (ST(0x0C) == 1 && ST(0x34) == 0 && ST(0x30) == 0 && CB->query_954(0x59)) {
        if (ST(0x2F4) == 1) {
            ST(0x30) = 1;
            CB->group(3);
            ST(0x98) = 1;
            if (ST(0xF90) != 6)
                CB->cb_458(6, 2);
            if (!(ST(0x19C) != 0 && !CB->layer_op(2, 0, 2) && ST(0x19C) != ST(0x40))) {
                if (ST(0xCDC) == 0) {
                    ST(0xCDC) = 1;
                    CB->named_object("SP89LEV01");
                    if (ST(0x194) == 2)
                        action_register("L01D150", 2, "SG00DIA01", NULL);
                    if (ST(0x194) == 3)
                        action_register("L01D151", 3, "SG00DIA01", NULL);
                    action_register("L01D152A", 0x59, "P89DIA01", "SP89DIA01");
                    action_register("L01D152B", 0x59, "SP89DIA01", "SP89LEV02");
                    CB->named_object("P89ARR01");
                    CB->named_object("Sar_10");
                    CB->cb_46C(&ST(0x30), 0);
                    CB->group(0);
                    return;
                }
                if (ST(0xCDC) == 1) {
                    ST(0xCDC) = 2;
                    CB->named_object("SP89LEV01");
                    if (ST(0x194) == 2)
                        action_register("L01D153", 2, "SG00DIA01", NULL);
                    if (ST(0x194) == 3)
                        action_register("L01D154", 3, "SG00DIA01", NULL);
                    action_register("L01D155A", 0x59, "P89DIA01", "SP89DIA01");
                    action_register("L01D155B", 0x59, "SP89DIA01", "SP89DIA01");
                    action_register("L01D155C", 0x59, "SP89LEV02", "SP89ARR01");
                    CB->named_object("Sar_10");
                    CB->cb_46C(&ST(0x30), 0);
                    CB->group(0);
                    return;
                }
                if (ST(0xCDC) == 2) {
                    if (ST(0x194) == 2)
                        action_register("L01D156", 2, "SG00DIA01", NULL);
                    if (ST(0x194) == 3)
                        action_register("L01D157", 3, "SG00DIA01", NULL);
                    CB->cb_46C(&ST(0x30), 0);
                    CB->group(0);
                    return;
                }
            }
            /* L1E3A */
            if (ST(0x19C) != 0 && !CB->layer_op(2, 0, 2) && ST(0x19C) != ST(0x40)) {
                ST(0x40) = ST(0x19C);
                if (ST(0x194) == 2)
                    action_register("L01D117", 2, "SG00PRE80", "SG00PRE81");
                if (ST(0x194) == 3)
                    action_register("L01D118", 3, "SG00PRE80", "SG00PRE81");
                CB->named_object_int("L01D149", 0x59);
                CB->named_object("G00PRE82");
                CB->cb_46C(&ST(0x30), 0);
                CB->group(0);
                return;
            }
        }
        /* L1F20 */
        if (ST(0x2F4) == 2) {
            CB->group(3);
            ST(0x30) = 1;
            if (ST(0x194) == 2) {
                CB->named_object("SG00PRE03");
                if (ST(0x198) == 1)
                    CB->named_object_int("LCOD82", 2);
                if (ST(0x198) == 0)
                    CB->named_object_int("LCOD83", 2);
            }
            if (ST(0x194) == 3) {
                CB->named_object("SG00PRE03");
                if (ST(0x198) == 1)
                    CB->named_object_int("LCOD84", 3);
                if (ST(0x198) == 0)
                    CB->named_object_int("LCOD85", 3);
            }
            CB->named_object("Sar_50");
            CB->cb_46C(&ST(0x30), 0);
            CB->group(0);
            return;
        }
    }
    /* L2034 */
    if (ST(0x10C) == 1 && ST(0x110) == 0 && ST(0x10) == 1 && ST(0x34) == 0 &&
        CB->query_954(0x59) && ST(0x2F4) == 1 && ST(0x2EC) == 1) {
        ST(0xF04) = 1;
        ST(0x8C8) = 2;
        CB->named_object_int("L01D148", 0x57);
        CB->group(0);
        CB->cb_970(0x57, 0);
        ST(0x34) = 0x57;
        return;
    }
    /* L2101 */
    if ((i32)ST(0x14) < 1) return;
    if (ST(0x34) != 0) return;
    if ((i32)ST(0x2F4) >= 2) return;
    if (!CB->query_954(0x59)) return;
    CB->group(3);
    ST(0x98) = 1;
    CB->cb_458(-7, 2);
    ST(0x34) = 0x57;
    ST(0x14) = -1;
    CB->named_object("SP89IMP51");
    ST(0x2F4) = 2;
    if (ST(0x2EC) == 1) {
        ST(0xF04) = 1;
        ST(0x8C8) = 2;
        CB->named_object_int("L01D148", 0x57);
        CB->group(0);
        CB->cb_970(0x57, 0);
    }
    if (ST(0x2EC) == 1) return;
    CB->group(0);
}

static void trig21(void) {   /* sub_1001222A */
    if (ST(0x0C) != 1) return;
    if (ST(0x30) != 0) return;
    if (ST(0x34) != 0) return;
    if (ST(0x10) != 0) return;
    if (!CB->query_944(0x14)) return;

    CB->group(3);
    ST(0x30) = 1;
    e_spawn_ar();

    if (ST(0x194) == 0) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0) action_register("LCOD09", 0, "SG00PRE02", NULL);
        if (ST(0x928) == 1) action_register("LCOD75", 0, "SG00PRE02", NULL);
        if (ST(0x928) == 2) action_register("LCOD06", 0, "SG00PRE02", NULL);
    }
    if (ST(0x194) == 1) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0) action_register("LCOD09", 1, "SG00PRE02", NULL);
        if (ST(0x928) == 1) action_register("LCOD75", 1, "SG00PRE02", NULL);
        if (ST(0x928) == 2) action_register("LCOD06", 1, "SG00PRE02", NULL);
    }
    if (ST(0x194) == 2) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0) action_register("LCOD10", 2, "SG00PRE02", NULL);
        if (ST(0x928) == 1) action_register("LCOD76", 2, "SG00PRE02", NULL);
        if (ST(0x928) == 2) action_register("LCOD07", 2, "SG00PRE02", NULL);
    }
    if (ST(0x194) == 3) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0) action_register("LCOD11", 3, "SG00PRE02", NULL);
        if (ST(0x928) == 1) action_register("LCOD77", 3, "SG00PRE02", NULL);
        if (ST(0x928) == 2) action_register("LCOD08", 3, "SG00PRE02", NULL);
    }

    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
}

static void trig23(void) {   /* sub_100125A8 */
    if (ST(0x0C) != 1) return;
    if (ST(0x30) != 0) return;
    if (ST(0x34) != 0) return;
    if (ST(0x10) != 0) return;
    if (CB->query_944(0x16) == 0) return;

    CB->group(3);
    ST(0x30) = 1;

    e_spawn_ar();

    if (ST(0x194) == 0) {
        if (ST(0x198) == 1) {
            ST(0x928) = CB->query_940(3);
            if (ST(0x928) == 0) action_register("LCOD09", 0, "SG00ATT01", NULL);
            if (ST(0x928) == 1) action_register("LCOD75", 0, "SG00ATT01", NULL);
            if (ST(0x928) == 2) action_register("LCOD06", 0, "SG00ATT01", NULL);
        }
        if (ST(0x198) == 0)
            action_register("L01D176", 0, "SP00ACT86", "Sar_10");
    }

    if (ST(0x194) == 1) {
        if (ST(0x198) == 1) {
            ST(0x928) = CB->query_940(3);
            if (ST(0x928) == 0) action_register("LCOD09", 1, "SG00ATT01", NULL);
            if (ST(0x928) == 1) action_register("LCOD75", 1, "SG00ATT01", NULL);
            if (ST(0x928) == 2) action_register("LCOD06", 1, "SG00ATT01", NULL);
        }
        if (ST(0x198) == 0)
            action_register("L01D176", 1, "SP01ACT86", "Sar_10");
    }

    if (ST(0x194) == 2) {
        if (ST(0x198) == 1) {
            ST(0x928) = CB->query_940(3);
            if (ST(0x928) == 0) action_register("LCOD10", 2, "SG00ATT01", NULL);
            if (ST(0x928) == 1) action_register("LCOD76", 2, "SG00ATT01", NULL);
            if (ST(0x928) == 2) action_register("LCOD07", 2, "SG00ATT01", NULL);
        }
        if (ST(0x198) == 0) {
            if (CB->layer_op(2, 7, 2))
                action_register("L01D177", 2, "SP02ACT85", "Sar_10");
            if (!CB->layer_op(2, 7, 2))
                action_register("L01D177", 2, "SP02ACT86", "Sar_10");
        }
    }

    if (ST(0x194) == 3) {
        if (ST(0x198) == 1) {
            ST(0x928) = CB->query_940(3);
            if (ST(0x928) == 0) action_register("LCOD11", 3, "SG00ATT01", NULL);
            if (ST(0x928) == 1) action_register("LCOD77", 3, "SG00ATT01", NULL);
            if (ST(0x928) == 2) action_register("LCOD08", 3, "SG00ATT01", NULL);
        }
        if (ST(0x198) == 0) {
            if (CB->layer_op(2, 7, 2))
                action_register("L01D178", 3, "SP03ACT85", "Sar_10");
            if (!CB->layer_op(2, 7, 2))
                action_register("L01D178", 3, "SP03ACT86", "Sar_10");
        }
    }

    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
}

static void trig24(void) {   /* sub_10012AA6 */
    if (ST(0x0C) != 1) return;
    if (ST(0x30) != 0) return;
    if (CB->query_944(0x18) == 0) return;

    ST(0x30) = 1;
    CB->group(3);

    e_spawn_ar();

    if (ST(0x194) == 0) {
        ST(0x928) = CB->query_940(2);
        if (ST(0x928) == 0)
            action_register("LCOD75", 0, "SG00ATT01", NULL);
        if (ST(0x928) == 1)
            action_register("LCOD06", 0, "SG00ATT01", NULL);
    }
    if (ST(0x194) == 1) {
        ST(0x928) = CB->query_940(2);
        if (ST(0x928) == 0)
            action_register("LCOD75", 1, "SG00ATT01", NULL);
        if (ST(0x928) == 1)
            action_register("LCOD06", 1, "SG00ATT01", NULL);
    }
    if (ST(0x194) == 2) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0)
            action_register("LCOD10", 2, "SG00ATT01", NULL);
        if (ST(0x928) == 1)
            action_register("LCOD76", 2, "SG00ATT01", NULL);
        if (ST(0x928) == 2)
            action_register("LCOD07", 2, "SG00ATT01", NULL);
    }
    if (ST(0x194) == 3) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0)
            action_register("LCOD11", 3, "SG00ATT01", NULL);
        if (ST(0x928) == 1)
            action_register("LCOD77", 3, "SG00ATT01", NULL);
        if (ST(0x928) == 2)
            action_register("LCOD08", 3, "SG00ATT01", NULL);
    }

    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
}

static void trig26(void) {   /* sub_10012FFD */
    if (ST(0xC) == 1 && ST(0x30) == 0 && ST(0x34) == 0 && ST(0x10) == 0 &&
        CB->query_998(0xEFD8)) {
        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();

        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD09", 0, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD75", 0, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD06", 0, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0) {
                action_register("L01D176", 0, "SP00ACT86", "Sar_10");
            }
        }
        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD09", 1, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD75", 1, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD06", 1, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0) {
                action_register("L01D176", 1, "SP01ACT86", "Sar_10");
            }
        }
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD10", 2, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD76", 2, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD07", 2, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 7, 2)) action_register("L01D177", 2, "SP02ACT85", "Sar_10");
                if (!CB->layer_op(2, 7, 2)) action_register("L01D177", 2, "SP02ACT86", "Sar_10");
            }
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD11", 3, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD77", 3, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD08", 3, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 7, 2)) action_register("L01D178", 3, "SP03ACT85", "Sar_10");
                if (!CB->layer_op(2, 7, 2)) action_register("L01D178", 3, "SP03ACT86", "Sar_10");
            }
        }

        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        return;
    }

    if (ST(0xC) == 1 && ST(0x30) == 0 && ST(0x34) == 0 && ST(0x10) == 0 &&
        CB->query_998(0x4000)) {
        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();

        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) {
                if (CB->layer_op(2, 0, 2) == 0) action_register("LCOD25", 0, "SG00ATT01", NULL);
                else                         action_register("LCOD25", 0, "SG00ATT03", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 0, 2) == 0) action_register("LCOD25", 0, "SG00ATT02", NULL);
                else                         action_register("LCOD25", 0, "SG00ATT04", NULL);
            }
        }
        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) {
                if (CB->layer_op(2, 0, 2) == 0) action_register("LCOD25", 1, "SG01ATT01", NULL);
                else                         action_register("LCOD25", 1, "SG01ATT03", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 0, 2) == 0) action_register("LCOD25", 1, "SG01ATT02", NULL);
                else                         action_register("LCOD25", 1, "SG01ATT04", NULL);
            }
        }
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) {
                if (CB->layer_op(2, 0, 2) == 0)      action_register("LCOD26", 2, "SG02ATT01", NULL);
                else if (CB->layer_op(2, 9, 2) == 0) action_register("LCOD26", 2, "SG02ATT03", NULL);
                else                              action_register("LCOD26", 2, "SG02ATT05", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 0, 2) == 0)      action_register("LCOD26", 2, "SG02ATT02", NULL);
                else if (CB->layer_op(2, 9, 2) == 0) action_register("LCOD26", 2, "SG02ATT04", NULL);
                else                              action_register("LCOD26", 2, "SG02ATT06", NULL);
            }
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) {
                if (CB->layer_op(2, 0, 2) == 0)      action_register("LCOD27", 3, "SG03ATT01", NULL);
                else if (CB->layer_op(2, 9, 2) == 0) action_register("LCOD27", 3, "SG03ATT03", NULL);
                else                              action_register("LCOD27", 3, "SG03ATT05", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 0, 2) == 0)      action_register("LCOD27", 3, "SG03ATT02", NULL);
                else if (CB->layer_op(2, 9, 2) == 0) action_register("LCOD27", 3, "SG03ATT04", NULL);
                else                              action_register("LCOD27", 3, "SG03ATT06", NULL);
            }
        }

        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
    }
}

static void trig27(void) {   /* sub_100139C8 */

    /* --- early branch: advance/skip helper --- */
    if (ST(0x194) != 4 && ST(0x0C) == 1 && CB->query_944(0x0D) &&
        (i32)CB->layer_op(1, ST(0x194), 2) < 0x36B0) {
        trig11();
        return;
    }

    /* --- window > 0x4E20: spawn 'ar' actor set (variant A) --- */
    if (ST(0x194) != 4 && ST(0x0C) == 1 &&
        ST(0x30) == 0 && ST(0x34) == 0 && ST(0x10) == 0 &&
        CB->query_998(0x7E2C) &&
        (i32)CB->layer_op(1, ST(0x194), 2) > 0x4E20) {

        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();

        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) {
                CB->named_object("SP00ACT13");
                CB->named_object("P00_65535");
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) CB->named_object_int("LCOD09", 0);
                if (ST(0x928) == 1) CB->named_object_int("LCOD75", 0);
                if (ST(0x928) == 2) CB->named_object_int("LCOD06", 0);
            }
            if (ST(0x198) == 0)
                action_register("L01D176", 0, "SP00ACT86", NULL);
            CB->named_object("Sar_50");
        }

        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) {
                CB->named_object("SP00ACT13");
                CB->named_object("P00_65535");
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) CB->named_object_int("LCOD09", 1);
                if (ST(0x928) == 1) CB->named_object_int("LCOD75", 1);
                if (ST(0x928) == 2) CB->named_object_int("LCOD06", 1);
            }
            if (ST(0x198) == 0)
                action_register("L01D176", 1, "SP01ACT86", NULL);
            CB->named_object("Sar_50");
        }

        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD10", 2, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD76", 2, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD07", 2, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 7, 2))
                    action_register("L01D177", 2, "SP02ACT85", NULL);
                if (!CB->layer_op(2, 7, 2))
                    action_register("L01D177", 2, "SP02ACT86", NULL);
            }
        }

        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD11", 3, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD77", 3, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD08", 3, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 7, 2))
                    action_register("L01D178", 3, "SP03ACT85", NULL);
                if (!CB->layer_op(2, 7, 2))
                    action_register("L01D178", 3, "SP03ACT86", NULL);
            }
        }

        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        return;
    }

    /* --- window [0x36B0, 0x4E20]: spawn 'ar' actor set (variant B) --- */
    if (ST(0x194) != 4 && ST(0x0C) == 1 &&
        ST(0x30) == 0 && ST(0x34) == 0 && ST(0x10) == 0 &&
        (i32)CB->layer_op(1, ST(0x194), 2) >= 0x36B0 &&
        (i32)CB->layer_op(1, ST(0x194), 2) <= 0x4E20) {

        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();

        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) CB->named_object("SP00ACT11");
            if (ST(0x198) == 0) CB->named_object("SP00ACT86");
        }
        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) CB->named_object("SP01ACT11");
            if (ST(0x198) == 0) CB->named_object("SP01ACT86");
        }
        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) {
                if (CB->layer_op(2, 9, 2)) CB->named_object("SP02ACT12");
                else                   CB->named_object("SP02ACT11");
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 9, 2)) CB->named_object("SP02ACT85");
                else                   CB->named_object("SP02ACT86");
            }
        }
        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) {
                if (CB->layer_op(2, 9, 2)) CB->named_object("SP03ACT12");
                else                   CB->named_object("SP03ACT11");
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 9, 2)) CB->named_object("SP03ACT85");
                else                   CB->named_object("SP03ACT86");
            }
        }

        CB->named_object("ar_50");
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        return;
    }
}

static void trig28(void) {   /* sub_1001427D */
    if (ST(0x194) != 4 && ST(0x0C) == 1 && ST(0x30) == 0 && ST(0x34) == 0 &&
        ST(0x10) == 0 && CB->query_998(0x9858) &&
        (i32)CB->layer_op(1, ST(0x194), 2) < 0xABE0) {

        CB->group(3);
        ST(0x30) = 1;
        e_spawn_ar();

        if (ST(0x194) == 0) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD09", 0, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD75", 0, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD06", 0, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0)
                action_register("L01D176", 0, "SP00ACT86", "Sar_10");
        }

        if (ST(0x194) == 1) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD09", 1, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD75", 1, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD06", 1, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0)
                action_register("L01D176", 1, "SP01ACT86", "Sar_10");
        }

        if (ST(0x194) == 2) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD10", 2, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD76", 2, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD07", 2, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 7, 2) != 0)
                    action_register("L01D177", 2, "SP02ACT85", "Sar_10");
                if (CB->layer_op(2, 7, 2) == 0)
                    action_register("L01D177", 2, "SP02ACT86", "Sar_10");
            }
        }

        if (ST(0x194) == 3) {
            if (ST(0x198) == 1) {
                ST(0x928) = CB->query_940(3);
                if (ST(0x928) == 0) action_register("LCOD11", 3, "SG00ATT01", NULL);
                if (ST(0x928) == 1) action_register("LCOD77", 3, "SG00ATT01", NULL);
                if (ST(0x928) == 2) action_register("LCOD08", 3, "SG00ATT01", NULL);
            }
            if (ST(0x198) == 0) {
                if (CB->layer_op(2, 7, 2) != 0)
                    action_register("L01D178", 3, "SP03ACT85", "Sar_10");
                if (CB->layer_op(2, 7, 2) == 0)
                    action_register("L01D178", 3, "SP03ACT86", "Sar_10");
            }
        }

        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        return;
    }

    if (ST(0x0C) == 1 && ST(0x30) == 0 && ST(0x10) == 0 && CB->query_998(0xBB80)) {
        CB->group(3);
        if ((i32)ST(0x194) <= 1)
            action_register("LCOD52", 1, "SG00SAV01", "Sar_10");
        if (ST(0x194) == 2)
            action_register("LCOD53", 2, "SG00SAV01", "Sar_10");
        if (ST(0x194) == 3)
            action_register("LCOD54", 3, "SG00SAV01", "Sar_10");
        CB->named_object("P00_65535");
        CB->op_9c(0x16, 0, 0);
        CB->group(0);
    }
}

static void trig29(void) {   /* sub_100148A3 */
    if (ST(0xC) == 1 && CB->query_944(0x1D) && CB->query_954(0xC) && CB->layer_op(0x1D, 0xC, 0))
        return;

    if (ST(0xC) != 1) return;
    if (ST(0x30) != 0) return;
    if (ST(0x10) != 0) return;

    CB->group(3);
    ST(0x30) = 1;

    if (ST(0xF90) != 6) {
        CB->cb_458(6, 2);
        ST(0x98) = 1;
    }

    if (ST(0x8D8) == 0) {
        ST(0x8D8) = 1;
        CB->named_object("SG00PRE07");
        CB->named_ex_988("OB44D", -1, -1, -1, -1, -1, -1, -3, 0);
        CB->named_object("Sar_50");
        CB->cb_46C(&ST(0x30), 0);
        CB->group(0);
        return;
    }

    CB->named_object("SG00ATT01");

    if (ST(0x194) == 0) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0) CB->named_object_int("LCOD09", 0);
        if (ST(0x928) == 1) CB->named_object_int("LCOD75", 0);
        if (ST(0x928) == 2) CB->named_object_int("LCOD06", 0);
    }
    if (ST(0x194) == 1) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0) CB->named_object_int("LCOD09", 1);
        if (ST(0x928) == 1) CB->named_object_int("LCOD75", 1);
        if (ST(0x928) == 2) CB->named_object_int("LCOD06", 1);
    }
    if (ST(0x194) == 2) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0) CB->named_object_int("LCOD10", 2);
        if (ST(0x928) == 1) CB->named_object_int("LCOD76", 2);
        if (ST(0x928) == 2) CB->named_object_int("LCOD07", 2);
    }
    if (ST(0x194) == 3) {
        ST(0x928) = CB->query_940(3);
        if (ST(0x928) == 0) CB->named_object_int("LCOD11", 3);
        if (ST(0x928) == 1) CB->named_object_int("LCOD77", 3);
        if (ST(0x928) == 2) CB->named_object_int("LCOD08", 3);
    }

    CB->named_object("Sar_50");
    CB->cb_46C(&ST(0x30), 0);
    CB->group(0);
}

static void trig_noop(void) { /* 0xB-byte stub slots — intentional no-op */ }

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
        CB->debug_text(buf, 0, 0xF0);
    }
    /* 0x8934 */
    if (ST(0x194) == 4 && ST(0x1A0) == 0) {
        ST(0x1A0) = 1;
        CB->named_two_ints(0x2C, 0xBB8);
    }
    /* 0x8977 — intro cinematic replay */
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
    /* 0x8A96 — level-0 arrival spawn */
    if (ST(0x328) == 1 && ST(0x194) == 0) {
        ST(0x328) = 0;
        CB->named_object("SP12ARR01_10");
        CB->named_object("SP00LEV01");
        CB->place_95C("L01B03", 0x27E, (i32)0xFFFFF40A);
        CB->named_object("P00MAR25");
        CB->named_object("ar");
        CB->spawn_678(0, 0x274, (i32)0xFFFFF47A, 0xFBC0, 1, "ar", 6, 0);
        CB->op_9c(0x18, 1, 1);
        CB->cb_458(0x63, 1);
        CB->group(0);
        return;
    }
    /* 0x8B5B */
    if (ST(0x3A8) == 1) {
        ST(0x3A8) = 0;
        ST(0xBA0) = 1;
        CB->group(3);
        CB->named_object_int("L01D01A", 0x1B);
        CB->named_object_int("L01D01B", 0x1B);
        CB->group(0);
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
            CB->group(3);
            if (ST(0x708) == 0) CB->cb_458(6, 2);
            CB->named_object("SP12LEV01");
            CB->named_object("P12ARR03_25");
            CB->op_9c(0x0C, 0x0C, 1);
            action_register("L01D03", 0x0C, "P12ARR03", "SP12ARR03_100");
            CB->cb_46C(&ST(0x30),  0);
            CB->cb_46C(&ST(0x708), 1);
            CB->cb_46C(&ST(0x88),  1);
            if (ST(0xB8) != 0 && ST(0xB8) != 6)
                CB->cb_458(ST(0xF90), 1);
            CB->group(0);
            return;
        }
        /* 0x8D67 */
        if (ST(0x88) == 1 && ST(0x30) == 0) {
            ST(0x88) = 2;
            CB->state_toggle(1, 1);
            CB->op_9c(0x0C, 0x0C, 0);
            CB->named_object("P12WAA01");
            CB->named_object("P12ARR03_10");
            CB->op_9c(0x0C, 0x0C, 1);
            CB->cb_46C(&ST(0x88), 0x29A);
            CB->state_toggle(0, 1);
        }
        /* 0x8E05 */
        if (CB->query_990(6, 4) > 0x190 && (i32)ST(0x320) > 0 && ST(0x74) != 1) {
            /* 0x8E41 */
            if (ST(0xF90) == 6 && ST(0xBC) == 1) {
                ST(0xBC) = 2;
                CB->cb_458(ST(0xF90), 1);
            }
            /* 0x8E8A */
            if (ST(0x68) == 1 && ST(0x34) == 0
                && CB->query_940(0xC8) == 1 && ST(0x88) == 0x29A) {
                CB->layer_op(0x0C, 0x0C, 0);
                ST(0x68) = 0;
                CB->state_toggle(1, 1);
                CB->named_object("SP12LEV03");
                CB->named_object("P12ARR01_10");
                CB->state_toggle(0, 1);
            }
            /* 0x8F22 */
            if (ST(0x324) != 0)
                ST(0x324) = 0;
        }
        /* 0x8F43 */
        if (CB->query_990(6, 4) < 0x96 && (i32)ST(0x320) > 0
            && ST(0x68) == 0 && ST(0x34) == 0
            && CB->query_940(0x64) == 1 && ST(0x88) == 0x29A) {
            ST(0x68) = 1;
            CB->state_toggle(1, 1);
            CB->named_object("SP12LEV01");
            CB->named_object("P12ARR03_10");
            CB->cb_46C(&ST(0x324), 1);
            CB->state_toggle(0, 1);
        }
        /* 0x900D */
        if (ST(0x324) == 1) {
            ST(0x324) = 2;
            CB->layer_op(0x0C, 0x0C, 1);
        }
    }
    /* 0x9040 — section G: guarded by [EC0]==0 && [B04]==0 */
    if (ST(0xEC0) == 0 && ST(0xB04) == 0) {
        /* 0x9064 */
        if (ST(0x78) == 1) {
            ST(0x78) = -1;
            CB->spawn_678(0x1A, 0x190, (i32)0xFFFFF6E6, 0x9470, 1,
                          "P26GAR03_10", -1, 0);
            CB->cb_458(4, 2);
            CB->place_95C("PORTE3", 0x200, (i32)0xFFFFF7D9);
            CB->named_object_int("L01D45A", 0x1A);
            if (CB->layer_op(9, ST(0x194), 7))
                CB->cb_458(0, 2);
            /* 0x9106 */
            CB->actor_934(ST(0x194), 0, 0, 0, 0x1A);
            if (CB->layer_op(9, ST(0x194), 4))
                CB->cb_458(0, 2);
            /* 0x914C */
            CB->named_object("Sar");
            CB->actor_934(-ST(0x194), 0x200, (i32)0xFFFFF58B, 0x859, -1);
            CB->named_object_int("L01D99", 0);
            CB->cb_458(1, 2);
            action_register("L01D45B", 0x1A, "SP26GAR03", "SG26FRA01");
            CB->named_two_ints(0x27, 0x2EE);
            CB->named_object("P26GAR03_10");
        }
        /* 0x91CE */
        if (ST(0x3BC) == 1) {
            ST(0x3BC) = 0;
            CB->state_toggle(2, 2);
            CB->select_468(-ST(0x194));
            CB->op_9c(0x1C, ST(0x194), 0);
            if (ST(0x194) == 0) CB->named_object("SG00ATT07");
            if (ST(0x194) == 1) CB->named_object("SG01ATT07");
            CB->cb_46C(&ST(0x78), 2);
            CB->state_toggle(0, 1);
        }
        /* 0x9287 */
        if (ST(0x78) == 2) {
            ST(0x78)  = -1;
            ST(0x324) = 0;
            CB->layer_op(0x0C, 0x0C, 0);
            CB->named_object("SP12ATT03");
            CB->cb_458(6, 2);
            CB->named_object_int("L01D44", 0x0C);
            CB->named_object("SP12ATT03_2000");
            CB->op_9c(6, ST(0x194), 0);
            CB->cb_46C(&ST(0x194), 4);
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
        CB->state_toggle(2, 1);
        CB->spawn_978(0x10, 0x140, (i32)0xFFFFF611, 0x5A, 0x5A, 0);
        CB->state_toggle(0, 1);
    }
    /* 0x9426 — imp W (0x57) appearance */
    if (CB->layer_op(0x12, 0x57, 0) && ST(0x2EC) != 2
        && CB->layer_op(0x1D, 0x57, 0)) {
        CB->state_toggle(1, 2);
        CB->layer_op(0x23, 0, 0);
        CB->op_9c(0x12, 0x57, 1);
        ST(0x30)  = 0;
        ST(0x34)  = 0;
        ST(0x2EC) = 2;
        CB->cb_970(0x57, -1);
        CB->select_468((i32)0xFFFFFFA9);
        CB->named_object("SP87IMP52");
        CB->named_object("P87CAD");
        CB->spawn_678(0x57, 0, 0, 0, 2, "P87CAD", 9, 0);
        if (ST(0xEC4) == 1 && ST(0x2F0) == 1) {
            if (ST(0x94) == 1) {
                ST(0xF74) = 1;
                ST(0xDC)  = 0x0A;
                CB->state_toggle(2, 2);
                CB->select_468((i32)0xFFFFFFA8);
                CB->named_object_int("L01D147", 0x58);
                CB->cb_970(0x58, 0);
                ST(0x34) = 0x58;
            }
            if (ST(0x94) == 0) {
                ST(0xF74) = 1;
                ST(0x94)  = 1;
                CB->state_toggle(2, 2);
                CB->cb_964(1, 0);
                CB->named_object("SP88LEV01");
                CB->named_object("P88_65535");
                CB->named_object_int("L01D147", 0x58);
                CB->cb_964(1, 1);
                CB->cb_970(0x58, 0);
                ST(0x34) = 0x58;
            }
        }
        CB->state_toggle(0, 1);
    }
    /* 0x9652 — imp X (0x58) appearance */
    if (CB->layer_op(0x12, 0x58, 0) && ST(0x2F0) != 2 && ST(0x94) == 0) {
        ST(0xE0)  = 1;
        ST(0x30)  = 0;
        ST(0x34)  = 0x57;
        ST(0x2F0) = 2;
        CB->named_object("SP88IMP99");
        CB->named_object("P88CAD");
        CB->spawn_678(0x58, 0, 0, 0, 2, "P88CAD", 0x11, 0);
        if (ST(0x2EC) == 1) {
            ST(0xF04) = 1;
            CB->state_toggle(1, 2);
            CB->named_object_int("L01D148", 0x57);
            CB->cb_970(0x57, 0);
        }
        CB->state_toggle(0, 1);
    }
    /* 0x9784 — imp Y (0x59) appearance */
    if (CB->layer_op(0x12, 0x59, 0) && ST(0x2F4) != 2
        && CB->layer_op(0x1D, 0x59, 0)) {
        ST(0xE0)  = 1;
        ST(0x30)  = 0;
        ST(0x34)  = 0;
        ST(0x2F4) = 2;
        CB->state_toggle(2, 2);
        CB->layer_op(0x23, 0, 0);
        CB->op_9c(0x12, 0x59, 1);
        CB->named_object("SP89IMP51");
        CB->named_object("P89CAD");
        CB->spawn_678(0x59, 0, 0, 0, 2, "P89CAD", 0x12, 0);
        if (ST(0x2EC) == 1) {
            ST(0xF04) = 1;
            CB->state_toggle(1, 2);
            CB->named_object_int("L01D148", 0x57);
            CB->cb_970(0x57, 0);
            ST(0x34) = 0x57;
        }
        CB->state_toggle(0, 1);
    }
    /* 0x98CA — imp X (0x58) variant */
    if (CB->layer_op(0x12, 0x58, 0) && ST(0x2F0) != 2 && ST(0x94) == 1) {
        ST(0xE0)  = 1;
        ST(0x30)  = 0;
        ST(0x2F0) = 2;
        CB->state_toggle(2, 2);
        CB->layer_op(0x23, 0, 0);
        CB->op_9c(0x12, 0x58, 1);
        CB->cb_970(0x58, -1);
        CB->select_468((i32)0xFFFFFFA8);
        CB->named_object("SP88IMP51");
        CB->named_object("P88CAD");
        CB->spawn_678(0x58, 0, 0, 0, 2, "P88CAD", 0x11, 0);
        if (ST(0x2EC) == 1) {
            ST(0xF04) = 1;
            CB->state_toggle(1, 2);
            CB->named_object_int("L01D148", 0x57);
            CB->cb_970(0x57, 0);
        }
        CB->state_toggle(0, 1);
    }
    /* 0x9A0B */
    if (ST(0x2EC) == 1 && ST(0xEC4) == 0) {
        if (ST(0x7C) == 0 && CB->query_990(6, 9) <= 0xFA)
            ST(0x7C) = 1;
        /* 0x9A65 */
        if (ST(0x7C) == 2 && ST(0xF90) != 2 && ST(0x34) == 0) {
            ST(0x7C) = 0;
            CB->state_toggle(2, 1);
            CB->select_468((i32)0xFFFFFFA9);
            if (ST(0x80) == 1) {
                ST(0x80) = 0;
                CB->group(3);
                CB->named_object("SP87GAR27");
                CB->named_object("P87ARR01_10");
                CB->group(0);
            }
            /* 0x9B06 */
            CB->actor_934(0x57, 0, 0, 0x5DC0, -1);
            CB->named_object("SP87ATT04");
            CB->state_toggle(0, 1);
        }
    }
    /* 0x9B34 */
    if (ST(0x2EC) == 1 && ST(0xEC4) != 0 && ST(0x34) == 0 && ST(0x80) == 0) {
        if (ST(0x7C) == 0 && CB->query_990(6, 9) <= 0xFA)
            ST(0x7C) = 1;
        /* 0x9BAF */
        if (ST(0x7C) == 2
            && (ST(0xF90) == 0 || ST(0xF90) == 6 || ST(0xF90) == 3)
            && CB->query_990(6, 9) > 0x113) {
            ST(0x7C) = 0;
            CB->state_toggle(2, 1);
            CB->select_468((i32)0xFFFFFFA9);
            CB->spawn_678(0x57, 0x140, (i32)0xFFFFF61E, 0xDD4, 1,
                          "P87ATT01", 9, 1);
            if (ST(0xF0) == 0) {
                ST(0xF0) = 1;
                CB->attach_99C(0x57, "OB94", 0x1B, 1);
            }
            CB->state_toggle(0, 1);
        }
    }
    /* 0x9C9D — Sar dialogue driver (guarded) */
    if (ST(0x2EC) < 2 && ST(0x7C) == 1 && ST(0xF04) == 0 && ST(0x34) == 0) {
        ST(0x7C) = 2;
        if (ST(0x10) == 0) {
            CB->group(3);
            CB->select_468((i32)0xFFFFFFA9);
            CB->named_object("SP87ARR01_10");
            CB->actor_934(0x57, 0, 0, 0, ST(0x194));
            if (ST(0x84) == 0) {
                ST(0x84) = 1;
                if (ST(0x194) == 1 && ST(0xCC8) == 0) {
                    CB->actor_934(-ST(0x194), 0, 0, 0, 0x57);
                    CB->named_object("sar_10");
                    ST(0xCC8) = 1;
                    CB->named_object_int("L01D66A", 1);
                    action_register("L01D67", 0x57, "SP87ATT70", "SP87ARR01_10");
                }
                /* 0x9DDD */
                if (ST(0x194) >= 2) {
                    CB->actor_934(-ST(0x194), 0, 0, 0, 0x57);
                    CB->named_object("sar_10");
                    if (ST(0x8B4) == 0) {
                        ST(0x8B4) = 1;
                        if (ST(0x194) == 2) {
                            if (ST(0x198) == 1)
                                action_register("L01D60A", 2, "SG02DIA03", NULL);
                            if (ST(0x198) == 0)
                                action_register("L01D60B", 2, "SG02DIA03", NULL);
                        }
                        if (ST(0x194) == 3) {
                            if (ST(0x198) == 1)
                                action_register("L01D61A", 3, "SG03DIA03", NULL);
                            if (ST(0x198) == 0)
                                action_register("L01D61B", 3, "SG03DIA03", NULL);
                        }
                        action_register("L01D30A", 0x57, "SP87DIA50", "SP87ARR01_10");
                        action_register("L01D30B", 0x57, "SP87DIA56", "SP87ARR01_10");
                    } else {
                        /* 0x9F33 */
                        action_register("L01D139", 0x57, "SP87DIA50", "SP87ARR01_10");
                    }
                }
                /* 0x9F4C */
                CB->named_object("SP87ARR01_10");
                CB->flush_958(0x57, ST(0x194));
                CB->group(0);
            } else {
                /* 0x9F7A */
                CB->named_object("SP87ARR01_10");
                CB->flush_958(0x57, ST(0x194));
                CB->group(0);
            }
        }
        /* 0x9FA3 */
        if (ST(0x10) == 1) {
            CB->group(3);
            ST(0x80) = 1;
            if (ST(0xF0) == 1) {
                ST(0xF0) = 0;
                CB->attach_99C(0x57, "OB94", 0x1B, 0);
            }
            if (ST(0x84) == 0) {
                ST(0x84) = 1;
                if (ST(0x194) == 1) { ST(0xCC8) = 1; CB->named_object_int("L01D66A", 1); }
                if (ST(0x194) == 2) { ST(0x8B4) = 1; CB->named_object_int("L01D66B", 2); }
                if (ST(0x194) == 3) { ST(0x8B4) = 1; CB->named_object_int("L01D66C", 3); }
                CB->named_object("SP87ARR01_10");
                CB->actor_934((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
                CB->named_object("SP87GAR26");
                CB->named_object("P87GAR02_10");
                if ((i32)ST(0x14) >= 1) {
                    ST(0x34) = 0x57;
                    CB->named_two_ints(2, 0x64);
                }
                if (ST(0x14) == 0) {
                    if (ST(0xC84) == 0) {
                        ST(0xC84) = 1;
                        if (ST(0x194) == 1)
                            action_register("L01D81", 0x57, "SP87DIA01", "SP87GAR02_10");
                        if (ST(0x194) >= 2)
                            action_register("L01D82", 0x57, "SP87DIA01", "SP87GAR02_10");
                    }
                    CB->flush_958(0x57, ST(0x194));
                    CB->group(0);
                }
                /* [14]!=0 falls through to A223 */
            } else {
                /* 0xA1C7 */
                CB->named_object("SP87ARR01_10");
                CB->actor_934((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
                CB->named_object("SP87GAR26");
                CB->named_object("P87GAR02_10");
                CB->flush_958(0x57, ST(0x194));
                CB->group(0);
            }
        }
    }
    /* 0xA223 */
    if (ST(0x2EC) < 2 && ST(0x7C) == 1 && ST(0xF04) == 1 && ST(0x34) == 0) {
        CB->group(3);
        ST(0x34) = 0x57;
        ST(0x7C) = 0x0A;
        ST(0x80) = 1;
        if (ST(0xF0) == 1) { ST(0xF0) = 0; CB->attach_99C(0x57, "OB94", 0x1B, 0); }
        CB->named_object("SP87ARR01_10");
        CB->actor_934((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
        CB->named_object("SP87GAR26");
        action_register("L01D87", 0x57, "SP87DIA01", "SP87GAR02_10");
        CB->group(0);
        if (ST(0xEC4) == 1 && ST(0x2F0) == 1) {
            if (ST(0x94) != 1) {
                ST(0x94) = 1;
                CB->named_object("SP88LEV01");
                CB->named_object("P88_65535");
            }
            CB->cb_970(0x58, 0);
        }
        CB->cb_970(0x57, 0);
    }
    /* 0xA395 */
    if (ST(0x2EC) < 2 && ST(0x7C) == 2 && ST(0x34) == 0) {
        if (ST(0x80) == 0 && ST(0x10) == 1) {
            ST(0x80) = 1;
            CB->group(3);
            CB->select_468((i32)0xFFFFFFA9);
            if (ST(0xF0) == 1) { ST(0xF0) = 0; CB->attach_99C(0x57, "OB94", 0x1B, 0); }
            CB->actor_934((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
            CB->named_object("SP87ARR01_10");
            CB->named_object("P87GAR26");
            if (ST(0xC84) == 0) {
                ST(0xC84) = 1;
                if (ST(0x194) == 1)
                    action_register("L01D81", 0x57, "P87DIA01", "SP87GAR02_10");
                if (ST(0x194) >= 2)
                    action_register("L01D82", 0x57, "P87DIA01", "SP87GAR02_10");
            }
            CB->flush_958(0x57, ST(0x194));
            CB->group(0);
            return;
        }
        /* 0xA50D */
        if (ST(0x80) == 1 && ST(0x10) == 0) {
            ST(0x80) = 0;
            CB->group(3);
            if (ST(0xF90) == 0 || ST(0xF90) == 3 || ST(0xF90) == 6) {
                CB->named_object_int("L01D80", 0x57);
                CB->group(0);
                return;
            } else {
                CB->select_468((i32)0xFFFFFFA9);
                CB->named_object("SP87GAR27");
                CB->named_object("P87ARR01_200");
                if (ST(0xC80) == 0 && ST(0x194) >= 2) {
                    ST(0xC80) = 1;
                    action_register("L01D80", 0x57, "P87DIA59", "SP87ARR01_10");
                }
                CB->cb_46C(&ST(0x30), 0);
                CB->flush_958(0x57, ST(0x194));
                CB->group(0);
                return;
            }
        }
    }
    /* 0xA672 */
    if (ST(0x328) == 1 && ST(0x34) == 0x57) {
        ST(0x328) = 2;
        ST(0x8C8) = 1;
        ST(0x34)  = 0;
        CB->actor_934((i32)0xFFFFFFA9, 0, 0, 0, ST(0x194));
        if (ST(0x194) == 1)
            action_register("L01D137", 0x57, "SP87DIA01", "SP87GAR02_10");
        if (ST(0x194) >= 2)
            action_register("L01D138A", 0x57, "SP87DIA01", "SP87GAR02_10");
        CB->cb_46C(&ST(0x30), 0);
        CB->named_object("Sar_10");
        CB->flush_958(0x57, ST(0x194));
        CB->group(0);
        return;
    }
    /* 0xA76F — imp W distance-gated dialogue/attack */
    if (ST(0x2F0) == 1 && ST(0x34) == 0 && CB->layer_op(0x1D, 0x58, 0)) {
        /* 0xA7AA — sub-block, else falls to B03A */
        if (ST(0x2EC) < 2 && ST(0x194) != 4
            && CB->layer_op(1, ST(0x194), 1) <= (i32)0xFFFFF510
            && CB->layer_op(1, ST(0x194), 0) >= 0x1F4
            && ST(0x98) == 0) {
            if (ST(0xCCC) == 0) {
                CB->group(3);
                ST(0x30)  = 1;
                ST(0xCCC) = 1;
                ST(0x98)  = 1;
                CB->cb_458(6, 2);
                ST(0x7C) = 0;
                CB->spawn_678(0x57, 0x140, (i32)0xFFFFF61E, 0x8CA0, 1, "P87ARR01", 9, 1);
                if (ST(0x2EC) == 1 && ST(0x8B4) == 0) {
                    ST(0x8B4) = 1;
                    ST(0x94)  = 1;
                    CB->cb_964(1, 0);
                    CB->named_object("SP88LEV01");
                    CB->named_object("P88_65535");
                    CB->actor_934((i32)0xFFFFFFA8, 0, 0, 0, ST(0x194));
                    action_register("L01D121", 0x58, "SP88DIA59", "SP88ARR01");
                    CB->cb_458(2, 2);
                    CB->actor_934((i32)0xFFFFFFA9, 0, 0, 0, 0x58);
                    if (ST(0x80) == 0)
                        action_register("L01D31A", 0x57, "SP87DIA55", "SP87ARR01_100");
                    if (ST(0x80) != 0) {
                        ST(0x80) = 0;
                        CB->select_468((i32)0xFFFFFFA9);
                        CB->named_object("SP87GAR27");
                        CB->named_object("P87ARR01_200");
                        action_register("L01D31A", 0x57, "P87DIA55", "SP87ARR01_100");
                    }
                    CB->actor_934(0x57, 0, 0, 0, ST(0x194));
                    action_register("L01D30A", 0x57, "SP87DIA50", "SP87ARR01");
                    action_register("L01D30B", 0x57, "SP87DIA56", "SP87ARR01");
                    CB->actor_934((i32)0xFFFFFFA9, 0, 0, 0, 0x58);
                    action_register("L01D31B", 0x57, "SP87DIA55", "SP87ARR01_100");
                    action_register("L01D31C", 0x57, "SP87DIA59", "SP87ARR01_100");
                    CB->cb_458(6, 2);
                    CB->spawn_678(0x57, 0x140, (i32)0xFFFFF61E, 0xDD4, 1, "P87ATT01", 9, 1);
                    if (ST(0xE0) == 0) {
                        CB->attach_99C(0x57, "OB94", 0x1B, 1);
                        ST(0xF0) = 1;
                    }
                    if (ST(0x10) == 0)
                        action_register("L01D122", 0x58, "SP88DIA60", "SP88ARR01_100");
                    if (ST(0x10) == 1)
                        action_register("L01D123", 0x58, "SP88DIA60", "SP88GAR01_500");
                    ST(0x84) = 1;
                    CB->cb_46C(&ST(0x30), 0);
                    CB->flush_958(0x58, ST(0x194));
                    CB->group(0);
                    return;
                }
                /* 0xAB83 */
                if (ST(0x2EC) == 1 && ST(0x8B4) == 1) {
                    ST(0x94) = 1;
                    CB->cb_964(1, 0);
                    CB->named_object("SP88LEV01");
                    CB->named_object("P88_65535");
                    CB->actor_934((i32)0xFFFFFFA8, 0, 0, 0, ST(0x194));
                    action_register("L01D124", 0x58, "SP88DIA59", "SP88ARR01_10");
                    CB->cb_964(1, 1);
                    CB->cb_458(2, 2);
                    CB->actor_934((i32)0xFFFFFFA9, 0, 0, 0, 0x58);
                    if (ST(0x80) == 0 && ST(0x10) == 0)
                        action_register("L01D31B", 0x57, "SP87DIA55", "SP87ARR01_100");
                    /* 0xAC7C — [80]/[10] gated L01D171 */
                    {
                        i32 do171;
                        if (ST(0x80) != 0) {
                            ST(0x80) = 0;
                            CB->select_468((i32)0xFFFFFFA9);
                            CB->named_object("SP87GAR27");
                            CB->named_object("P87ARR01_200");
                            do171 = 1;
                        } else {
                            do171 = (ST(0x10) == 1) ? 1 : 0;
                        }
                        if (do171)
                            action_register("L01D171", 0x57, "P87DIA55", "SP87ARR01_100");
                    }
                    action_register("L01D31C", 0x57, "SP87DIA59", "SP87ARR01_100");
                    CB->cb_458(6, 2);
                    if (ST(0x10) == 0)
                        action_register("L01D122", 0x58, "SP88DIA60", "SP88ARR01_100");
                    if (ST(0x10) == 1)
                        action_register("L01D123", 0x58, "SP88DIA60", "SP88GAR01_500");
                    CB->cb_46C(&ST(0x30), 0);
                    CB->flush_958(0x58, ST(0x194));
                    CB->group(0);
                    return;
                }
            }
            /* 0xADB7 */
            if (ST(0xCCC) == 1 && ST(0x30) == 0 && ST(0x34) == 0) {
                ST(0x98) = 1;
                CB->group(3);
                ST(0x30) = 1;
                ST(0x7C) = 0;
                ST(0x80) = 0;
                CB->cb_458(6, 2);
                CB->spawn_678(0x57, 0x140, (i32)0xFFFFF61E, 0x8CA0, 1, "P87ARR01", 9, 1);
                if (ST(0x94) == 0) {
                    ST(0x94) = 1;
                    CB->cb_964(1, 0);
                    CB->named_object("SP88LEV01");
                    CB->named_object("P88_65535");
                    CB->actor_934((i32)0xFFFFFFA8, 0, 0, 0, ST(0x194));
                }
                if (ST(0x10) == 0)
                    action_register("L01D94", 0x58, "SP88DIA54", "SP88ARR01_10");
                if (ST(0x10) == 1)
                    action_register("L01D95", 0x58, "SP88DIA51", "SP88ARR01_10");
                CB->cb_964(1, 1);
                CB->cb_458(1, 2);
                if (ST(0x80) == 0 && ST(0x10) == 0)
                    action_register("L01D96", 0x57, "SP87DIA61", "SP87ARR01_10");
                /* 0xAF68 — [80]/[10] gated L01D172 */
                {
                    i32 do172;
                    if (ST(0x80) != 0) {
                        ST(0x80) = 0;
                        CB->select_468((i32)0xFFFFFFA9);
                        CB->named_object("SP87GAR27");
                        CB->named_object("P87ARR01_200");
                        do172 = 1;
                    } else {
                        do172 = (ST(0x10) == 1) ? 1 : 0;
                    }
                    if (do172)
                        action_register("L01D172", 0x57, "SP87DIA61", "SP87ARR01_10");
                }
                CB->cb_458(6, 2);
                CB->cb_46C(&ST(0x30), 0);
                CB->flush_958(0x58, ST(0x194));
                CB->group(0);
                return;
            }
        }
        /* 0xB03A */
        if (ST(0x2EC) >= 2 && ST(0x34) == 0 && ST(0x194) != 4
            && CB->layer_op(1, ST(0x194), 1) < (i32)0xFFFFF63C
            && CB->layer_op(1, ST(0x194), 0) >= 0x1F4) {
            CB->group(3);
            ST(0x30) = 1;
            ST(0x94) = 1;
            ST(0x34) = 0x57;
            CB->cb_458(6, 2);
            CB->cb_964(1, 0);
            CB->named_object("SP88LEV01");
            CB->named_object("P88_65535");
            CB->actor_934((i32)0xFFFFFFA8, 0, 0, 0, ST(0x194));
            action_register("L01D127", 0x58, "SP88DIA60", "SP88GAR01_500");
            CB->cb_46C(&ST(0x30), 0);
            CB->cb_964(1, 1);
            CB->cb_458(0, 1);
            CB->group(0);
            CB->cb_970(0x58, 0);
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
    if (ST(0x2F0) < 2 && ST(0x34) == 0 && CB->query_990(6, 0x11) > 0x12C
        && (ST(0xF90) == 2 || ST(0xF90) == 3) && ST(0x94) == 1) {
        CB->select_468((i32)0xFFFFFFA8);
        ST(0xDC) = 0x0A;
        ST(0x94) = 0;
        CB->spawn_678(0x58, 0x242, (i32)0xFFFFF43E, 0x1388, 1, "P88ARR02", 0x11, 1);
    }
    /* 0xB2C8 */
    if (ST(0xF90) == 6 && CB->query_990(6, 4) > 0xC8 && CB->query_990(6, 7) > 0xC8
        && ST(0x98) != 0 && ST(0xEC0) == 0 && CB->layer_op(0x1D, 0x0C, 0)) {
        CB->cb_458(0x63, 1);
        ST(0x98) = 0;
    }
    /* 0xB36D */
    if (ST(0xF90) == 6 && CB->query_990(6, 7) > 0x12C && ST(0x98) != 0
        && ST(0xEC0) == 1 && ST(0xEC4) == 0) {
        CB->cb_458(0x63, 1);
        ST(0x98) = 0;
    }
    /* 0xB3EF */
    if (ST(0xF90) == 6 && CB->query_990(6, 0x11) > 0xC8 && ST(0x98) != 0
        && ST(0xEC0) == 1 && ST(0xEC4) == 1 && CB->layer_op(0x1D, 0x58, 0)) {
        CB->cb_458(0x63, 1);
        ST(0x98) = 0;
    }
    /* 0xB48B */
    if (ST(0xF90) == 6 && CB->query_990(6, 0x12) > 0xC8 && ST(0x98) != 0
        && ST(0xEC0) == 1 && ST(0xEC4) == 2 && CB->layer_op(0x1D, 0x59, 0)) {
        CB->cb_458(0x63, 1);
        ST(0x98) = 0;
    }
    /* 0xB527 */
    if (ST(0xDC) == 1) {
        ST(0xDC) = 2;
        CB->flush_958(0x58, ST(0x194));
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
 * event5 — sub_100022B4 (14 KB). switch(arg) over LCOD player-actions
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
                action_register("LCOD02A", 2, "SP02ACT64", "SP02ACT64");
                action_register("LCOD02B", 2, "SP02ACT64", "SP02ACT64");
                action_register("LCOD02C", 2, "SP02ACT64", "SP02ACT64");
                action_register("LCOD02D", 2, "SP02ACT64", "SP02ACT64");
                action_register("LCOD02E", 2, "SP02ACT64", "SP02ACT64");
                action_register("LCOD02F", 2, "SP02ACT64", "SP02ACT64");
                if (ST(0xEA8) == 0) {
                    action_register("LCOD02G", 2, "SP02ACT64", "SP02ACT64");
                    action_register("LCOD02H", 2, "SP02ACT64", "SP02ACT64");
                }
                ST(0xEA8) = 1;
            }
            if (ST(0x194) == 3) {
                action_register("LCOD03A", 3, "SP03ACT64", "SP03ACT64");
                action_register("LCOD03B", 3, "SP03ACT64", "SP03ACT64");
                action_register("LCOD03C", 3, "SP03ACT64", "SP03ACT64");
                action_register("LCOD03D", 3, "SP03ACT64", "SP03ACT64");
                action_register("LCOD03E", 3, "SP03ACT64", "SP03ACT64");
                action_register("LCOD03F", 3, "SP03ACT64", "SP03ACT64");
                if (ST(0xEA8) == 0) {
                    action_register("LCOD03G", 3, "SP03ACT64", "SP03ACT64");
                    action_register("LCOD03H", 3, "SP03ACT64", "SP03ACT64");
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
        if (ST(0x194) == 2) action_register("LCOD88", 2, "SG00DIA02", NULL);
        if (ST(0x194) == 3) action_register("LCOD89", 3, "SG00DIA02", NULL);
        CB->group(0);
        break;

    case 46:  /* 0x2A47 — LCOD100/101/102/103/104 */
        e5_reset();
        if (ST(0x194) == 2) {
            action_register("LCOD100A", 2, "SP02ACT64", "SP02ACT64");
            CB->named_object_int("LCOD100B", 2);
            CB->named_object_int("LCOD100C", 2);
            CB->named_object_int("LCOD100D", 2);
        }
        if (ST(0x194) == 3) {
            action_register("LCOD101A", 3, "SP03ACT64", "SP03ACT64");
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
        if (ST(0x194) == 2) action_register("LCOD103", 2, "SG02DIA01", NULL);
        if (ST(0x194) == 3) action_register("LCOD104", 3, "SG03DIA01", NULL);
        CB->named_object("Sar_50");
        CB->group(0);
        break;

    case 70:  /* 0x2CEA */
        e5_reset();
        if (ST(0x194) == 2) action_register("LCOD90", 2, "SG00DIA02", NULL);
        if (ST(0x194) == 3) action_register("LCOD91", 3, "SG00DIA02", NULL);
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
        if (ST(0x194) == 1) action_register("LCOD41", ST(0x194), "SG01ATT01", NULL);
        if (ST(0x194) == 2) action_register("LCOD42", ST(0x194), "SG01ATT01", NULL);
        if (ST(0x194) == 3) action_register("LCOD43", ST(0x194), "SG01ATT01", NULL);
        CB->group(0);
        break;

    case 81:  /* 0x36B3 — LCOD23/24 A-G */
        e5_reset();
        if (ST(0xEAC) == 0) ST(0xEAC) = 1;
        if (ST(0x18) == 0x1C) ST(0x84) = 1;
        if (ST(0x194) == 2) {
            action_register("LCOD23A", 2, "SP02ACT64", "SP02ACT64");
            CB->named_object_int("LCOD23B", 2);
            CB->named_object_int("LCOD23C", 2);
            CB->named_object_int("LCOD23D", 2);
            CB->named_object_int("LCOD23E", 2);
            CB->named_object_int("LCOD23F", 2);
            action_register("LCOD23G", 2, "SG02DIA01", NULL);
        }
        if (ST(0x194) == 3) {
            action_register("LCOD24A", 3, "SP03ACT64", "SP03ACT64");
            CB->named_object_int("LCOD24B", 3);
            CB->named_object_int("LCOD24C", 3);
            CB->named_object_int("LCOD24D", 3);
            CB->named_object_int("LCOD24E", 3);
            CB->named_object_int("LCOD24F", 3);
            action_register("LCOD24G", 3, "SG03DIA01", NULL);
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
        if (ST(0x194) == 2) action_register("LCOD55", 2, "SP02ACT64", NULL);
        if (ST(0x194) == 3) action_register("LCOD56", 3, "SP02ACT64", NULL);
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
            if (ST(0x194) == 2) action_register("LCOD666", 2, "SG02DIA02", NULL);
            if (ST(0x194) == 3) action_register("LCOD667", 3, "SG03DIA02", NULL);
            CB->group(0);
            break;
        } else {
            if (ST(0x18) == 0x23) {
                CB->group(3);
                if (ST(0x194) == 2) action_register("LCOD670", 2, "SG02DIA02", NULL);
                if (ST(0x194) == 3) action_register("LCOD671", 3, "SG03DIA02", NULL);
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
            if (ST(0x194) == 2) action_register("LCOD668", 2, "SG02DIA01", NULL);
            if (ST(0x194) == 3) action_register("LCOD669", 3, "SG03DIA01", NULL);
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
 * INIT (opcode 1) — arming tail from FonctionEditor case 1.
 * Callback ingestion is skipped: g_lieu_cb is provided by the bridge.
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
        trig00();   /* sub_10001073 default path */
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
 * TICK (opcode 0) + DEINIT (opcode -1): drain pending sound, run body.
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
 * Entry point — mirrors @FonctionEditor@16 opcode dispatch.
 * ---------------------------------------------------------------------- */
i32 g_lieu_trace = 0;

i32 lieu01_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    S = state;
    if (g_lieu_trace)
        print("lieu01: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:   init(arg);                    break;
    case LIEU_OP_EVENT:  if ((u32)arg < 42) g_trigger[arg](); break;
    case LIEU_OP_CALL4:  event4(arg);                  break;
    case LIEU_OP_CALL5:  event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
