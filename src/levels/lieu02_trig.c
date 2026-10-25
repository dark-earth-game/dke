/*
 * lieu02_trig.c — Native hand-port of the LIEU02.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu02.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 *
 * Callback slots: LIEU02 copies the engine table into DIFFERENT globals
 * than LIEU01 (the state pointer is dword_1002C98C, not C96C), so the
 * lieu_cb.h slot comments do not match this DLL's addresses. Members were
 * mapped by engine-table byte offset (INIT ingestion at 0x10019AFA):
 *   C480 named_object      C490 named_object_int  C694 named_two_ints
 *   C894 cinematique       C698 spawn_678         C9A8 named_ex_988
 *   C980 cb_960            C954 actor_934         C478 cb_458
 *   C9A4 cb_984            C97C place_95C         C998 spawn_978
 *   C95C cb_93C            C990 cb_970            C974 query_954
 *   C964 query_944         C9B8 query_998         C9B0 query_990
 *   C9AC cb_98C            C958 group             C474 cb_454
 *   C488 select_468        C984 cb_964            C9A0 cb_980
 *   C494 debug_text        C96C cb_94C            C978 flush_958
 *   C48C cb_46C            C960 query_940         C484 state_toggle
 *   C988 cb_968            C9BC attach_99C        C968 cb_948
 *   C9B4 layer_op          C994 cb_974            C970 cb_950
 *   C99C cb_97C            C35C cb_33C            C47C op_9c
 */

#include <stdio.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu02_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig00(void); static void trig01(void); static void trig02(void);
static void trig03(void); static void trig04(void); static void trig05(void);
static void trig06(void); static void trig07(void); static void trig08(void);
static void trig09(void); static void trig10(void); static void trig11(void);
static void trig12(void); static void trig13(void); static void trig15(void);
static void trig16(void); static void trig17(void); static void trig18(void);
static void trig19(void); static void trig23(void); static void trig24(void);
static void trig25(void); static void trig26(void); static void trig27(void);
static void trig28(void); static void trig29(void); static void trig30(void);
static void trig31(void); static void trig32(void); static void trig33(void);
static void trig34(void); static void trig35(void); static void trig36(void);
static void trig37(void); static void trig38(void); static void trig41(void);
static void trig_noop(void);   /* the 6 stub slots */

/* dword_1002C8A0[42] — filled at INIT (0x10019CC6, before the reload-flag
 * test) with the 5-byte jmp thunks at 0x10001000+5*i; static here.
 *   idx thunk     -> handler       idx thunk     -> handler
 *    0  10001073  -> 100073F0       21  1000103C  -> 10011AEF stub
 *    1  10001069  -> 1000D341       22  100010DC  -> 10011AFA stub
 *    2  100010BE  -> 1000D52D       23  1000108C  -> 10011B05
 *    3  100010EB  -> 1000DB44       24  1000105A  -> 10011D42
 *    4  10001082  -> 1000E14C       25  1000100F  -> 10011F7F
 *    5  100010AA  -> 1000E35B       26  100010FA  -> 10012166
 *    6  100010E1  -> 1000E560       27  10001014  -> 10012EBF
 *    7  1000101E  -> 1000E90F       28  10001087  -> 10014446
 *    8  10001055  -> 1000FCC6       29  100010F0  -> 1001462D
 *    9  10001078  -> 1000FE15       30  10001032  -> 10014814
 *   10  1000109B  -> 1001014F       31  10001000  -> 100149FB
 *   11  10001028  -> 1001065F       32  1000106E  -> 1001531B
 *   12  10001019  -> 1001070E       33  100010C3  -> 10015416
 *   13  100010A0  -> 10010C21       34  10001023  -> 10015511
 *   14  100010D2  -> 10010D28 stub  35  10001091  -> 100156DC
 *   15  100010E6  -> 10010D33       36  10001037  -> 10015793
 *   16  10001096  -> 10010E2B       37  100010C8  -> 100159B5
 *   17  100010B4  -> 10011576       38  1000104B  -> 10015EC5
 *   18  100010CD  -> 10011785       39  10001050  -> 10015FC9 stub
 *   19  1000107D  -> 100118DF       40  100010A5  -> 10015FD4 stub
 *   20  10001064  -> 10011AE4 stub  41  1000102D  -> 1000924D
 */
static void (*const g_trigger[42])(void) = {
    trig00, trig01, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig09, trig10, trig11, trig12, trig13, trig_noop, trig15,
    trig16, trig17, trig18, trig19, trig_noop, trig_noop, trig_noop,
    trig23, trig24, trig25, trig26, trig27, trig28, trig29, trig30,
    trig31, trig32, trig33, trig34, trig35, trig36, trig37, trig38,
    trig_noop, trig_noop, trig41,
};

/* Op 3 (0x1001A0B8): dword_1002C69C = dword_1002C8A0[arg]; call it. The
 * DLL does no bounds check (every slot is filled); the range test here
 * only guards the native table, as in lieu01.c. */
void l02_trigger(i32 idx) {
    if ((u32)idx < 42)
        g_trigger[idx]();
}

/* Direct entries for the core (see lieu02_int.h). */
void l02_trig00(void) { trig00(); }   /* sub_10001073 -> sub_100073F0 */
void l02_trig31(void) { trig31(); }   /* sub_10001000 -> sub_100149FB */

/* 0xB-byte stub slots (push ebp .. ret) — intentional no-op. */
static void trig_noop(void) { }

/* -------------------------------------------------------------------------
 * Trigger handlers (opcode 3).
 * ---------------------------------------------------------------------- */

/* Spawn at a position read back from layer 1 of `zone` (axis 2,1,0 queried
 * in original order into temps), as trig00 does repeatedly. */
static void a_spawn_at_layer(i32 zone, i32 flag, const c8 *name, i32 kind,
                             i32 z) {
    i32 c0 = CB->divers(DV_DESC_POS, zone, 2);
    i32 c1 = CB->divers(DV_DESC_POS, zone, 1);
    i32 c2 = CB->divers(DV_DESC_POS, zone, 0);
    CB->pos_perso(zone, c2, c1, c0, flag, name, kind, z);
}

/* Spawn "ar" at (x, y), facing read back from layer 1 axis 2. */
static void a_spawn_ar_facing(i32 x, i32 y) {
    i32 f = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
    CB->pos_perso(ST(ST_HERO), x, y, f, 1, "ar", 0x14, 0);
}

static void trig00(void) {   /* sub_100073F0 */
    if (ST(0x968) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(0xEC0) = 0;
        ST(0xAF0) = 1;
        ST(0xBA0) = 1;
        ST(0x5DC) = 1;
        ST(ST_LEVEL)  = 1;
    }
    if (ST(0x968) == 2) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL)  = 6;
        ST(ST_VIE(P_GUARDIAN17)) = VIE_DEAD;
    }
    if (ST(0x968) == 3) {
        ST(0xAF8) = 1;
        ST(0xAF0) = 1;
        ST(0xEC0) = 1;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL)  = 1;
        ST(ST_VIE(P_GUARDIAN17)) = 1;
    }
    if (ST(0x968) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL)  = 3;
        ST(0xB2C) = 1;
        ST(0xB40) = 3;
        ST(0xE9C) = 3;
        ST(ST_VIE(P_KALHI13)) = 1;
    }
    if (ST(0x968) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL)  = 3;
        ST(0xB2C) = 2;
        ST(0xF3C) = 2;
        ST(0xB40) = 3;
        ST(0xE9C) = 3;
        ST(ST_VIE(P_KALHI33)) = 1;
    }
    if (ST(0x968) == 6) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL)  = 8;
        ST(0x380) = 7;
        ST(ST_VIE(P_DELIA)) = 1;
    }
    if (ST(0x968) == 7) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xEB8) = 1;
        ST(ST_LEVEL)  = 8;
    }
    if (ST(0x968) == 8) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_GUARDIAN17)) = 4;
        ST(ST_VIE(P_GUARDIAN18)) = 1;
        ST(0x720) = 0;
        ST(ST_LEVEL)  = 8;
    }
    if (ST(0x968) == 9) {
        ST(0xB04) = 2;
        ST(0xC08) = 0xA;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_GUARDIAN17)) = 4;
        ST(ST_VIE(P_GUARDIAN19)) = 1;
        ST(0x720) = 0;
        ST(ST_LEVEL)  = 8;
    }
    if (ST(0x968) == 0xA) {
        ST(0xB04) = 0;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xEB8) = 1;
        ST(ST_VIE(P_GUARDIAN17)) = 4;
        ST(ST_VIE(P_GUARDIAN19)) = 1;
        ST(0x720) = 1;
        ST(ST_LEVEL)  = 8;
    }
    /* (no case 0xB in the original) */
    if (ST(0x968) == 0xC) {
        ST(0xB04) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_GUARDIAN17)) = 4;
        ST(ST_VIE(P_GUARDIAN18)) = 4;
        ST(ST_VIE(P_GUARDIAN19)) = 1;
        ST(0x720) = 1;
        ST(ST_LEVEL)  = 8;
    }

    ST(0x320) = 0; ST(0x324) = 0; ST(0x328) = 0; ST(0x3A4) = 0;
    ST(0x3A8) = 0; ST(0x3B8) = 0; ST(0x3AC) = 0; ST(0x3B0) = 0;
    ST(0x3B4) = 0; ST(0x3BC) = 0; ST(0x3C4) = 0; ST(ST_SCRIPT_BUSY)  = 0;
    ST(0x34)  = 0; ST(0x40)  = 0; ST(0x64)  = 0; ST(0x68)  = 0;
    ST(0x6C)  = 0; ST(0x70)  = 0; ST(0x74)  = 0; ST(0x78)  = 0;
    ST(0x7C)  = 0; ST(0x80)  = 0; ST(0x84)  = 0; ST(0x88)  = 0;
    ST(0x8C)  = 0; ST(0x90)  = 0; ST(0x94)  = 0; ST(0x98)  = 0;
    ST(0xB8)  = 0; ST(0xBC)  = 0; ST(0xD4)  = 0; ST(0xD8)  = 0;
    ST(0xDC)  = 0; ST(0xE0)  = 0; ST(0xF0)  = 0; ST(0xF4)  = 0;
    ST(0xF8)  = 0; ST(0xFC)  = 0; ST(0x100) = 0; ST(0x114) = 0;
    ST(0xE00) = 0; ST(0x4C)  = 0; ST(0x50)  = 0; ST(0x54)  = 0;

    if (ST(0x15C) == 1)
        ST(0xE0) = 2;
    ST(0x108) = ST(ST_LEVEL);

    CB->ajoute_zone(1, 1600, -1830, 100,  110,  0);
    CB->ajoute_zone(4, -1720, -1140, 80,  100,  1);
    CB->ajoute_zone(5, -1000, -1460, 170,  90,  1);
    CB->ajoute_zone(6, 505, -2025, 100,  90,  0);
    CB->ajoute_zone(8, 1490, -1760, 200,  90,  1);
    CB->ajoute_zone(9, 486, -660,  120,  90,  0);
    CB->ajoute_zone(10, 1110, -781,  270, 155,  0);
    CB->ajoute_zone(12, 1219, -888,  135,  255,  0);
    CB->ajoute_zone(37, 1313, -1122, 185,  265, 0);
    CB->ajoute_zone(13, 233,  -813,  120,  120,  0);
    CB->ajoute_zone(15, 893, -1929, 130,  95,  0);
    CB->ajoute_zone(17, -1720, -1531, 80,  100,  1);
    CB->ajoute_zone(18, 1511, -832,  200,  90,  1);
    CB->ajoute_zone(19, -1040, -1250, 170,  90,  1);
    CB->ajoute_zone(22, 1541, -2020, 50,  50,  0);
    CB->ajoute_zone(23, -1560, -729,  225,  235,  0);
    CB->ajoute_zone(24, -1560, -1968, 225,  235,  0);
    CB->ajoute_zone(25, 306, -1635, 175,  175,  0);
    CB->ajoute_zone(28, 306, -1063, 175,  175,  0);
    CB->ajoute_zone(29, 701, -1635, 175,  175,  0);
    CB->ajoute_zone(30, 701, -1063, 175,  175,  0);
    CB->ajoute_zone(32, 1219, -1628, 90,  245,  0);
    CB->ajoute_zone(33, 1289, -1766, 105,  85,  0);
    CB->ajoute_zone(34, -201,  -1108, 185,  240,  0);
    CB->ajoute_zone(35, 1298, -2009, 150,  150,  0);
    CB->ajoute_zone(36, -205,  -1386, 250,  320, 0);
    CB->ajoute_zone(38, 893, -828,  175,  110,  0);

    CB->bruitage("L02AMB1", -1, -1);
    CB->vire_col(0x21, 0);
    CB->vire_col(0x20, 0);
    CB->etat_objet("IM03", 1662, 22, -1886, 0xC000, 0x4000, 0x4000, 0, 4);
    CB->etat_objet("IM04", 469, 13, -625,  0xC000, 0x4000, 0x8000, 0, 4);

    if (ST(0xB10) != 0 && ST(0xE74) == 0)
        ST(0xE74) = 1;

    if (ST(ST_LEVEL) == 1) {
        CB->pos_perso(ST(ST_HERO), 475, -2000, 0x7D0, 1, "ar", 0x14, 0);
        if (ST(ST_HERO) == P_ARKHAN0) {
            ST(0xBA0) = 2;
            CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x1B58, 0, 4);
            CB->pos_perso(P_ZED84, 375, -1840, 0xA2E4, 1, "P84ARR01", 3, 1);
            CB->link_objet(0x54, "OB68", 0x1D, 1);
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(0xBA0) < 3) {
                ST(0xBDC) = 1;
                CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
                CB->pos_perso(P_ZED84, 385, -1740, 0xA2E4, 1, "P84ARR01", 3, 1);
                CB->link_objet(0x54, "OB68", 0x1D, 1);
                CB->ajoute_zone(21, 1400, -695, 150, 150, 1);
                ST(0x7C) = 1;
            }
            if (ST(0xBA0) >= 3)
                CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
        }
        if (ST(ST_HERO) >= 2)
            CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
    }

    if (ST(ST_LEVEL) == 3) {
        CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
        if (ST(0xB2C) == 1) a_spawn_ar_facing(-1590, -1151);
        if (ST(0xB2C) == 2) a_spawn_ar_facing(-1592, -1542);
    }

    if (ST(ST_LEVEL) == 6) {
        ST(0x100) = 1;
        CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
        CB->pos_perso(ST(ST_HERO), 1555, -1825, 0x4692, 1, "ar", 0x14, 0);
    }

    if (ST(ST_LEVEL) == 7) {
        CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
        if (ST(0xAFC) == 1) a_spawn_ar_facing(0x5E7, -1640);
        if (ST(0xAFC) == 2) a_spawn_ar_facing(0x5E7, -932);
        ST(0xAFC) = 0;
    }

    if (ST(ST_LEVEL) == 8) {
        CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
        if (ST(ST_HERO) == P_ARKHAN1 && ST(0xEC8) == 1 && ST(0xAF4) < 3) {
            CB->pos_perso(ST(ST_HERO), -974, -1343, 0x3E94, 1, "ar", 0x14, 0);
            CB->pos_perso(P_ZED84, -1230, -1323, 0xBA23, 1, "P84ARR01", 3, 1);
            CB->retire_zone(19);
            CB->retire_zone(5);
        } else {
            a_spawn_ar_facing(-1120, -1340);
        }
    }

    if (ST(ST_LEVEL) == 9) {
        CB->pos_perso(ST(ST_HERO), 495, -753, 0x7DE7, 1, "ar", 0x14, 0);
        CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
    }

    if (ST(0xB04) != 2 && ST(ST_VIE(P_GUARDIAN17)) < 2 && ST(0xEB8) == 0) {
        if (ST(ST_LEVEL) != 6) {
            CB->retire_zone(1);
            CB->vire_col(0x21, 1);
            CB->pos_perso(P_GUARDIAN17, 1565, -1836, 0x437A, 1, "P57ARR02", 7, 1);
        }
        if (ST(ST_LEVEL) == 6) {
            ST(0x64) = 1;
            a_spawn_at_layer(0x39, 1, "P57ARR02", 7, 1);
        }
    }

    if (ST(0xB04) != 2 && ST(ST_VIE(P_GUARDIAN17)) == VIE_DEAD && ST(0xEB8) == 0)
        a_spawn_at_layer(0x39, 2, "P57CAD", 7, 0);

    if (ST(0x380) != 7 && ST(0x380) != 8 && ST(0xE9C) != 3 && ST(0xE9C) != 4
        && ST(0xEB8) == 0 && ST(ST_VIE(P_GUARDIAN17)) > 1) {
        if (ST(ST_VIE(P_GUARDIAN18)) == 1
            && (ST(0xB04) != 0 || (ST(0xB04) == 0 && ST(0xEB8) != 1))) {
            CB->retire_zone(1);
            CB->vire_col(0x21, 1);
            CB->pos_perso(P_GUARDIAN18, 1565, -1836, 0x437A, 1, "P58ARR04", 0x0B, 1);
        }
        if (ST(ST_VIE(P_GUARDIAN18)) == VIE_DEAD
            && (ST(0xB04) != 0 || (ST(0xB04) == 0 && ST(0xEB8) != 1)))
            a_spawn_at_layer(0x3A, 2, "P58CAD", 0x0B, 0);
    }

    if (ST(0x380) != 7 && ST(0x380) != 8 && ST(0xB04) == 2 && ST(0xC08) == 0xA) {
        if (ST(ST_VIE(P_GUARDIAN19)) != VIE_DEAD) {
            if (ST(ST_LEVEL) != 9) {
                CB->retire_zone(9);
                CB->pos_perso(P_GUARDIAN19, 490, -700, 0x7D00, 1, "P59ARR03", 2, 1);
            }
            if (ST(ST_LEVEL) == 9) {
                a_spawn_at_layer(0x3B, 1, "P59ARR03", 2, 1);
                ST(0x64) = 1;
                ST(0x68) = 1;
            }
        }
        if (ST(ST_VIE(P_GUARDIAN19)) == VIE_DEAD)
            a_spawn_at_layer(0x3B, 2, "P59CAD", 2, 0);
    }

    if (ST(0xE9C) == 3 && ST(0xEF0) != 0) {
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            if (ST(ST_LEVEL) == 9 || ST(ST_LEVEL) == 3 || ST(ST_LEVEL) == 1) {
                if (ST(ST_LEVEL) == 9)
                    CB->pos_perso(P_KALHI13, 659, -798, 0x274E, 1, "P13ARR01", 0x1A, 1);
                if (ST(ST_LEVEL) == 1)
                    CB->pos_perso(P_KALHI13, 283, -1932, 0xAD9C, 1, "P13ARR01", 0x1A, 1);
                if (ST(ST_LEVEL) == 3) {
                    if (ST(0xB2C) == 1)
                        CB->pos_perso(P_KALHI13, -1553, -1359, 0x1932, 1, "P13ARR01", 0x1A, 1);
                    if (ST(0xB2C) == 2)
                        CB->pos_perso(P_KALHI13, -1553, -1359, 0x7148, 1, "P13ARR01", 0x1A, 1);
                }
            } else {
                a_spawn_at_layer(0x0D, 1, "P13ARR01", 0x1A, 1);
            }
            if (ST(ST_LEVEL) != 7) {
                ST(0xB8) = 1;
                ST(0x90) = 1;
            }
            if (ST(ST_LEVEL) == 52) {
                ST(0xF8) = 2;
                ST(0xB8) = 0x14;
                ST(0x90) = 0xA;
            }
        }
        if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD)
            a_spawn_at_layer(0x0D, 2, "P13cad", 0x1A, 0);
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            i32 f = CB->divers(DV_DESC_POS, P_KALHI33, 2);
            CB->pos_perso(P_KALHI33, -1099, -1375, f, 1, "P33ARR01", 0x1B, 1);
        }
        if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD)
            a_spawn_at_layer(0x21, 2, "P33cad", 0x1B, 0);
    }

    if (ST(0xE9C) == 3 && ST(0xEF0) == 0) {
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            ST(0xB8) = 1;
            ST(0x90) = 1;
            if (ST(0xB2C) == 1)
                CB->pos_perso(P_KALHI13, -1720, -1140, 0xC350, 1, "P13ARR01", 0x1A, 1);
            if (ST(0xB2C) == 2)
                CB->pos_perso(P_KALHI13, -1720, -1530, 0xC350, 1, "P13ARR01", 0x1A, 1);
        }
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            if (ST(0xF3C) == 1)
                CB->pos_perso(P_KALHI33, -1340, -1620, 0xB9F0, 1, "P33ARR01", 0x1B, 1);
            if (ST(0xF3C) == 2)
                CB->pos_perso(P_KALHI33, -1340, -1180, 0xB9F0, 1, "P33ARR01", 0x1B, 1);
            CB->ajoute_zone(39, -1090, -1380, 70, 70, 0);
            CB->ajoute_zone(40, -506,  -1740, 70, 70, 0);
        }
    }

    if (ST(0xE9C) == 4) {
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            CB->pos_perso(P_KALHI13, -211, -1610, 0xBB80, 1, "P13ARR03", 0x1A, 1);
            ST(0x8C) = 1;
        }
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            CB->pos_perso(P_KALHI33, -211, -1610, 0xBB80, 1, "P33ARR03", 0x1B, 1);
            CB->ajoute_zone(39, -1090, -1380, 70, 70, 0);
            CB->ajoute_zone(40, -506,  -1740, 70, 70, 0);
            ST(0x8C) = 1;
        }
        if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD)
            a_spawn_at_layer(0x0D, 2, "P13cad", 0x1A, 0);
        if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD)
            a_spawn_at_layer(0x21, 2, "P33cad", 0x1B, 0);
    }

    if (ST(ST_VIE(P_DELIA)) == 1) {
        if (ST(0x380) == 8) {
            ST(0xF4) = 1;
            if (ST(ST_LEVEL) == 8)
                CB->pos_perso(P_DELIA, 1242, -1337, 0xBEC4, 1, "P10ARR01", 0x10, 0);
            if (ST(ST_LEVEL) == 9)
                CB->pos_perso(P_DELIA, 659, -798, 0x274E, 1, "P10ARR01", 0x10, 0);
            if (ST(ST_LEVEL) == 1)
                CB->pos_perso(P_DELIA, 283, -1932, 0xAD9C, 1, "P10ARR01", 0x10, 0);
            if (ST(ST_LEVEL) == 7)
                a_spawn_at_layer(0x0A, 1, "P10ARR01", 0x10, 0);
            if (ST(ST_LEVEL) == 6)
                a_spawn_at_layer(0x0A, 1, "P10ARR01", 0x10, 0);
        }
        if (ST(0x380) == 7) {
            ST(0x380) = 8;
            ST(0xF4) = 1;
            CB->pos_perso(P_DELIA, -991, -1334, 0x3F4D, 1, "P10ARR01", 0x10, 0);
        }
    }

    if (ST(0xF1C) == 2 && ST(ST_VIE(P_DELIA)) == VIE_DEAD && ST(0x380) == 0x14)
        a_spawn_at_layer(0x0A, 2, "P10cad", 0x10, 0);

    if (ST(0xEB8) == 1 && ST(0xB04) == 0) {
        CB->vire_col(0x21, 1);
        CB->pos_perso(P_PHEDORIA, 1564, -1827, 0x3735, 1, "P26ARR03", 0x1F, 1);
        CB->retire_zone(1);
    }

    if (ST(ST_LEVEL) == 52) {
        CB->etat_objet("IM68", 464, 16, -2061, 0xC000, 0x4000, 0x8000, 0, 4);
        CB->pos_perso(ST(ST_HERO), 1294, -2009, 0, 1, "ar", 0x14, 0);
    }

    CB->divers(DV_MUSIC, 1, 0);
    ST(0x968) = 0;
}
static void trig01(void) {   /* sub_1000D341 */
    if (ST(0xBD0) == 1)
        ST(0xBD0) = 2;

    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (!CB->direction_spec(0xAFC8)) return;

    if (ST(0xEC0) == 0)
        ST(0xEC0) = 1;

    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(ST_LEVEL) = 2;
        CB->animation("SG00OUV01");
        CB->animation("P00_65535");
        CB->anim_objet("IM03", 3, 0x3C, 0x5DC, 0);
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_MUSIC, 1, 4);
        CB->change_piece(6, 1);
        return;
    }

    if (ST(ST_HERO) >= 2) {
        CB->bloque(BLOQUE_ALL);
        ST(0xF68) = ST(ST_HERO);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(ST_LEVEL) = 2;
        CB->animation("SG00OUV01");
        CB->animation("P00_65535");
        CB->anim_objet("IM03", 3, 0x3C, 0x5DC, 0);
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_MUSIC, 1, 4);
        if (ST(0xB34) == 1)
            CB->change_piece(6, 0);
        if (ST(0xB34) != 1)
            CB->change_piece(6, 1);
    }
}

static void trig02(void) {   /* sub_1000D52D */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9) && CB->direction_spec(0)) {
        trig09();
        return;
    }

    if (ST(0x10C) == 1 && ST(0x110) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(0x34) == 0
        && CB->direction_perso(P_GUARDIAN19) && ST(ST_VIE(P_GUARDIAN19)) == 1) {
        ST(0x110) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L02D97", P_GUARDIAN19);
        CB->animation("Sar_10");
        CB->modify_var(&ST(0x110), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    if (!CB->direction_perso(P_GUARDIAN19)) {
        /* loc_1000DB07 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9) && CB->direction_zone(9))
            trig09();
        return;
    }

    if (ST(ST_ACTION_KEY) == 1 && ST(0x34) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        if (ST(ST_VIE(P_GUARDIAN19)) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            if (ST(0x720) == 0) {
                CB->change_camera(8, CAM_LOCKED);
                CB->stop_joindre(-P_GUARDIAN19);
                CB->animation("SP59GAR03_10");
                ST(0x720) = 1;
                ST(0x68) = 5;
                CB->vire_col(0x20, 0);
                if (ST(ST_SPECIAL_MODE) == 1) {
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        l02_action_register("L02D84A", 2, "SG00DIA07", NULL);
                        l02_action_register("L02D84B", 2, "SG00DIA07", NULL);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        l02_action_register("L02D85A", 3, "SG00DIA07", NULL);
                        l02_action_register("L02D85B", 3, "SG00DIA07", NULL);
                    }
                }
                if (ST(ST_SPECIAL_MODE) == 0) {
                    if (ST(ST_HERO) == P_ARKHAN2)
                        l02_action_register("L02D86", 2, "SG00DIA07", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        l02_action_register("L02D87", 3, "SG00DIA07", NULL);
                }
                l02_action_register("L02D88", 0x3B, "SP59GAR27", "SP59ARR03");
                if (ST(ST_HERO) == P_ARKHAN2) {
                    l02_action_register("L02D89A", 2, "SG00DIA07", NULL);
                    l02_action_register("L02D89B", 2, "SG00DIA08", NULL);
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    l02_action_register("L02D90A", 3, "SG00DIA07", NULL);
                    l02_action_register("L02D90B", 3, "SG00DIA08", NULL);
                }
                l02_action_register("L02D91A", 0x3B, "SP59DIA59", "SP59ARR03_10");
                l02_action_register("L02D91B", 0x3B, "SP59DIA56", "SP59ARR03_10");
                l02_action_register("L02D91C", 0x3B, "SP59DIA53", "SP59ARR03_10");
                ST(0x64) = 1;
                CB->joindre(P_GUARDIAN19, 623, -700, 0x59D8, -1);
                CB->ajoute_zone(9, 486, -660, 120, 90, 0);
                CB->change_camera(CAM_ANY, CAM_AUTO);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            if (ST(0x720) == 1) {
                if (ST(ST_HERO) == P_ARKHAN2)
                    l02_action_register("L02D92", 2, "SG00DIA02", NULL);
                if (ST(ST_HERO) == P_ARKHAN3)
                    l02_action_register("LCOD65", 3, "SG00DIA02", NULL);
                l02_action_register("L02D94", 0x3B, "SP59DIA53", "SP59ARR03_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* ST(0x720) neither 0 nor 1: falls to loc_1000D9CD */
        }
        /* loc_1000D9CD */
        if (ST(ST_VIE(P_GUARDIAN19)) == VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0)
            CB->fouille(P_GUARDIAN19);
    }

    /* loc_1000D9F9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(0x34) == 0x3B && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_GUARDIAN19)) {
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D95", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D96", P_ARKHAN3);
    }

    /* loc_1000DA7D */
    if (ST(0x14) >= 1 && ST(0x34) == 0 && ST(ST_VIE(P_GUARDIAN19)) < 2) {
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L02D97", P_GUARDIAN19);
        CB->vire_col(0x20, 0);
        ST(0x34) = 0x3B;
        ST(0x14) = -1;
        CB->bloque(BLOQUE_NONE);
        CB->combat(P_GUARDIAN19, 0);
    }
}

static void trig03(void) {   /* sub_1000DB44 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x12)) {
        trig18();
        return;
    }
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
        return;
    }

    if (ST(0x10C) == 1 && ST(0x110) == 0 && ST(ST_COMBAT_STANCE) == 1
        && CB->direction_perso(P_ZED84) && ST(ST_VIE(P_ZED84)) == 1) {
        ST(0x110) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(0x74) = 0x3E8;
        CB->stop_joindre(-P_ZED84);
        CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
        l02_action_register("L02D81", 0x54, "SP84DIA56", "SP84ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(0x74), 2);
        CB->modify_var(&ST(0x110), 0);
        CB->animation("SP84ARR01_10");
        CB->joindre(-P_ZED84, 1400, -695, 0xEA60, -1);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    /* loc_1000DCC4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_ZED84)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x74) = 0x3E8;
        CB->stop_joindre(-P_ZED84);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x54);
        CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
        if (ST(0x724) == 0) {
            ST(0x724) = 1;
            if (ST(ST_SPECIAL_MODE) == 1) {
                l02_action_register("L02D71", 1, "G00DIA01", NULL);
                l02_action_register("L02D72", 1, "G00DIA01", NULL);
                l02_action_register("L02D73A", 0x54, "SP84ATT51", "SP84ARR01");
                l02_action_register("L02D73B", 0x54, "SP84DIA56", "SP84ARR01");
                l02_action_register("L02D73C", 0x54, "SP84DIA56", "SP84ARR01");
                l02_action_register("L02D74", 1, "G00DIA05", NULL);
                l02_action_register("L02D75", 0x54, "SP84DIA56", "SP84ARR01_10");
            } else {
                l02_action_register("L02D71", 1, "G00DIA01", NULL);
                l02_action_register("L02D76", 1, "G00DIA01", NULL);
                l02_action_register("L02D77", 0x54, "SP84ATT51", "SP84ARR01");
                l02_action_register("L02D73B", 0x54, "SP84DIA56", "SP84ARR01");
                l02_action_register("L02D73C", 0x54, "SP84DIA56", "SP84ARR01");
                l02_action_register("L02D78A", 1, "G00DIA05", NULL);
                l02_action_register("L02D78B", 1, "G00DIA05", NULL);
                l02_action_register("L02D78C", 1, "G00DIA05", NULL);
                l02_action_register("L02D75", 0x54, "SP84DIA56", "SP84ARR01_10");
            }
            /* loc_1000DF0D */
            CB->modify_var(&ST(0x74), 2);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->joindre(-P_ZED84, 1400, -695, 0xEA60, -1);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(0x724) == 1) {   /* loc_1000DF5B */
            l02_action_register("L02D80", 1, "G00DIA01", NULL);
            l02_action_register("L02D81", 0x54, "SP84DIA56", "SP84ARR01_10");
            CB->modify_var(&ST(0x74), 2);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->joindre(-P_ZED84, 1400, -695, 0xEA60, -1);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* ST(0x724) > 1: falls through to loc_1000DFEA */
    }

    /* loc_1000DFEA */
    if (ST(0x14) == 1 && ST(0x34) == 0 && CB->direction_perso(P_ZED84)) {
        ST(0x14) = -1;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_ZED84);
        ST(0x74) = 0xA;
        CB->animation("SP84ESA13");
        CB->animation("P84_65535");
        CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
        if (ST(0x778) == 0) {
            ST(0x778) = 1;
            l02_action_register("L02D82", 0x54, "SP84DIA59", "SP84ARR02");
            CB->joindre(-P_ZED84, 0, 0, 0xEA60, -1);
            CB->stop_joindre(-P_ZED84);
            CB->modify_var(&ST(0x74), 2);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(0x778) == 1) {
            ST(0x778) = 2;
            l02_action_register("L02D83", 0x54, "SP84DIA60", "SP84ACT03");
            CB->evenement(0x22, 0xC8);
        }
    }
}

static void trig04(void) {   /* sub_1000E14C */
    ST(0xB2C) = 1;
    CB->select_pile(0, 2);
    CB->retire_zone(4);
    if (ST(0xEC0) == 0)
        ST(0xEC0) = 1;

    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI13)) == 1) {
        CB->bloque(BLOQUE_ALL);
        ST(0xF68) = ST(ST_HERO);
        ST(0xB8) = 0x14;
        ST(0x90) = 0xA;
        CB->stop_joindre(-P_KALHI13);
        CB->dialogue("L02D500", P_KALHI13);
        CB->animation("SP13ARR01_300");
        CB->bloque(BLOQUE_NONE);
    }

    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1 && ST(0x7C) == 0 && ST(0x34) == 0) {
        ST(0x7C) = 1;
        ST(0xF68) = ST(ST_HERO);
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(5, CAM_LOCKED);
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        l02_action_register("L02D65A", 0x21, "SP33DIA59", "SP33ARR01");
        l02_action_register("L02D65C", 0x21, "SP33DIA56", "SP33ARR01_10");
        CB->animation("SP33ARR01_10");
        CB->change_camera(5, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
    }

    ST(ST_LEVEL) = 2;
    CB->divers_empile(DV_MUSIC, 1, 4);
    CB->change_piece(3, 0);
}

static void trig05(void) {   /* sub_1000E35B */
    ST(0xB44) = 2;
    CB->select_pile(0, 2);
    CB->retire_zone(5);

    if (ST(0x380) == 8) {
        CB->bloque(BLOQUE_ALL);
        ST(0xF68) = ST(ST_HERO);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->stop_joindre(-P_DELIA);
        ST(0xF4) = 0xA;
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D501", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D502", P_ARKHAN3);
        CB->bloque(BLOQUE_NONE);
        CB->modify_var(&ST(0xB44), 2);
    }

    if (ST(0xE9C) == 3) {
        ST(0xEF0) = 0;
        ST(0xE9C) = 5;
        if (ST(ST_VIE(P_KALHI13)) == 1 && ST(0x794) == 0) {
            CB->stop_joindre(-P_KALHI13);
            ST(0xB8) = 0xA;
        }
        if (ST(ST_VIE(P_KALHI33)) == 1)
            CB->stop_joindre(-P_KALHI33);
    }

    ST(ST_LEVEL) = 2;
    CB->divers_empile(DV_MUSIC, 1, 4);
    if (ST(0xB4C) != 3) {
        if (ST(0xF20) == 1 || ST(0xE48) != 0)
            CB->change_piece(8, 1);
        else
            CB->change_piece(8, 0);
    } else {
        CB->change_piece(8, 2);
    }
}

static void trig06(void) {   /* sub_1000E560 */
    if (ST(0xBD0) == 1)
        ST(0xBD0) = 2;
    if (ST(ST_ACTION_KEY) != 1) return;
    if (!CB->direction_spec(0x8000)) return;

    CB->select_pile(0, 2);
    CB->bloque(BLOQUE_ALL);

    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->change_camera(0, CAM_LOCKED);
        CB->animation("SG00OUV01");
        CB->animation("P00_65535");
        CB->anim_objet("IM68", 3, 0x3C, 0x5DC, 0);
        CB->bloque(BLOQUE_NONE);
        if (ST(0xEC0) == 0)
            CB->divers_empile(DV_MUSIC, 1, 4);
        ST(ST_LEVEL) = 2;
        CB->change_camera(CAM_ANY, CAM_AUTO);
        CB->change_piece(1, 0);
        return;
    }

    /* loc_1000E669 */
    if (ST(ST_HERO) < 2) return;

    if (!CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0))
        CB->change_camera(0, CAM_LOCKED);

    if (ST(0x380) == 8) {
        CB->change_camera(0, CAM_LOCKED);
        ST(0xF68) = ST(ST_HERO);
        ST(0xF4) = 0xA;
        CB->stop_joindre(-P_DELIA);
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D501", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D502", P_ARKHAN3);
    }

    if (ST(0xB40) == 3 && ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI13)) == 1) {
        CB->change_camera(0, CAM_LOCKED);
        ST(0xF68) = ST(ST_HERO);
        CB->stop_joindre(-P_KALHI13);
        ST(0xB8) = 0x14;
        ST(0x90) = 0xA;
        CB->dialogue("L02D500", P_KALHI13);
    }

    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1 && ST(0x7C) == 0 && ST(0x34) == 0) {
        ST(0x7C) = 1;
        ST(0xF68) = ST(ST_HERO);
        CB->change_camera(5, CAM_LOCKED);
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        l02_action_register("L02D65A", 0x21, "SP33DIA59", "SP33ARR01");
        l02_action_register("L02D65C", 0x21, "SP33DIA56", "SP33ARR01_10");
        CB->animation("SP33ARR01_10");
        CB->change_camera(0, CAM_LOCKED);
    }

    /* loc_1000E8A5 */
    CB->animation("SG00OUV01");
    CB->animation("P00_65535");
    CB->anim_objet("IM68", 3, 0x3C, 0x5DC, 0);
    CB->bloque(BLOQUE_NONE);
    ST(ST_LEVEL) = 2;
    CB->change_camera(CAM_ANY, CAM_AUTO);
    CB->change_piece(1, 0);
}

static void trig08(void) {   /* sub_1000FCC6 */
    CB->select_pile(0, 2);
    if (ST(0xBD0) == 1)
        ST(0xBD0) = 2;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (ST(0xEC0) == 0)
        ST(0xEC0) = 1;

    if (ST(ST_HERO) != 4 && ST(0xAF8) == 0 && ST(0xBDC) == 1
        && CB->divers(DV_PERSONA_PRESENT, P_ZED84, 0)) {
        i32 a = CB->divers(DV_DESC_POS, P_ZED84, 3);
        i32 b = CB->divers(DV_DESC_POS, ST(ST_HERO), 3);
        if (a < b) {
            ST(0xBDC) = 3;
            goto set_rest;
        }
    }
    ST(0xBDC) = 2;   /* loc_1000FDC2 */
set_rest:            /* loc_1000FDD1 */
    ST(0xF68) = ST(ST_HERO);
    ST(0xAFC) = 3;
    ST(ST_LEVEL) = 2;
    CB->change_piece(7, 0);
}

static void trig09(void) {   /* sub_1000FE15 */
    if (ST(0xBD0) == 1)
        ST(0xBD0) = 2;
    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (!CB->direction_spec(0xFFFF)) return;

    CB->select_pile(0, 2);
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    if (!CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0))
        CB->change_camera(8, CAM_LOCKED);
    if (ST(0xEC0) == 0)
        ST(0xEC0) = 1;

    if (ST(0x380) == 8) {
        CB->change_camera(8, CAM_LOCKED);
        ST(0xF68) = ST(ST_HERO);
        CB->stop_joindre(-P_DELIA);
        ST(0xF4) = 0xA;
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D501", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D502", P_ARKHAN3);
    }

    if (ST(0xB40) == 3 && ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI13)) == 1) {
        CB->change_camera(8, CAM_LOCKED);
        ST(0xF68) = ST(ST_HERO);
        CB->stop_joindre(-P_KALHI13);
        ST(0xB8) = 0x14;
        ST(0x90) = 0xA;
        CB->dialogue("L02D500", P_KALHI13);
    }

    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1 && ST(0x7C) == 0 && ST(0x34) == 0) {
        ST(0x7C) = 1;
        ST(0xF68) = ST(ST_HERO);
        CB->change_camera(5, CAM_LOCKED);
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        l02_action_register("L02D65A", 0x21, "SP33DIA59", "SP33ARR01");
        l02_action_register("L02D65C", 0x21, "SP33DIA56", "SP33ARR01_10");
        CB->animation("SP33ARR01_10");
        CB->change_camera(8, CAM_LOCKED);
    }

    /* loc_100100E8 */
    CB->animation("SG00OUV01");
    CB->animation("P00_65535");
    CB->anim_objet("IM04", 3, -60, 0x5DC, 0);
    ST(ST_LEVEL) = 2;
    CB->divers_empile(DV_MUSIC, 1, 4);
    CB->bloque(BLOQUE_NONE);
    CB->change_piece(9, 0);
}

static void trig07(void) {   /* sub_1000E90F */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x12)) {
        trig18();
        return;
    }
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
        return;
    }

    if (ST(ST_HERO) == P_ARKHAN1 && ST(0x10C) == 1 && ST(0x110) == 0 && ST(ST_COMBAT_STANCE) == 1
        && CB->direction_perso(P_GUARDIAN17) && ST(ST_VIE(P_GUARDIAN17)) == 1) {
        ST(0x110) = 1;
        CB->bloque(BLOQUE_ALL);
        l02_action_register("L02D41", 0x39, "SP57DIA60", "SP57GAR02_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(0x110), 0);
        ST(0x34) = 0x39;
        CB->combat(P_GUARDIAN17, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    /* loc_1000EA4B */
    if (ST(ST_HERO) > 1 && ST(0x10C) == 1 && ST(0x110) == 0 && ST(ST_COMBAT_STANCE) == 1
        && CB->direction_perso(P_GUARDIAN17) && ST(ST_VIE(P_GUARDIAN17)) == 1) {
        ST(0x110) = 1;
        CB->bloque(BLOQUE_ALL);
        l02_action_register("L02D41", 0x39, "SP57DIA60", "SP57GAR02_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(0x110), 0);
        ST(0x34) = 0x39;
        CB->vire_col(0x21, 0);
        CB->combat(P_GUARDIAN17, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    /* loc_1000EB3A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && CB->direction_perso(P_GUARDIAN17)) {
        if (ST(ST_VIE(P_GUARDIAN17)) >= 2)
            goto l_fb24;
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;

        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(0x7C) == 0) {
                CB->pos_perso(P_ZED84, 1386, -726, 0xC71B, 4, "P84ARR01", 3, 1);
                ST(0x74) = 0x2C;
                ST(0xBDC) = 2;
                ST(0x7C) = 1;
            }

            /* loc_1000EC10 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(0x718) == 0 && ST(0xC88) == 0) {
                    ST(0x64) = 1;
                    ST(0x718) = 1;
                    CB->vire_col(0x21, 0);
                    CB->ajoute_zone(1, 1600, -1830, 80, 90, 0);
                    l02_action_register("L02D04", 1, "SP01ATT74", NULL);
                    l02_action_register("L02D05", 0x39, "SP57DIA56", "SP57ARR02");
                    if (ST(0x358) == 0)
                        l02_action_register("L02D06", 1, "SG00DIA01", NULL);
                    if (ST(0x358) == 2)
                        l02_action_register("L02D07", 1, "SG00DIA01", NULL);
                    l02_action_register("L02D08", 0x39, "SP57ATT72", "SP57ARR02_10");
                    CB->joindre(-P_GUARDIAN17, 1540, -2000, 0x1667, -1);
                    CB->animation("Sar_100");
                    CB->modify_var(&ST(0x114), 1);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* loc_1000ED7D */
                if (ST(0x718) == 0 && ST(0xC88) >= 1) {
                    ST(0x64) = 1;
                    ST(0xDC) = 0;
                    CB->vire_col(0x21, 0);
                    ST(0x718) = 1;
                    CB->ajoute_zone(1, 1600, -1830, 80, 90, 0);
                    l02_action_register("L02D66", 1, "SG00DIA03", NULL);
                    if (ST(0xC88) == 2) {
                        CB->animation("SP57GAR27");
                        l02_action_register("L02D09", 0x39, "P57DIA51", "SP57ARR02_10");
                    } else {
                        l02_action_register("L02D09", 0x39, "SP57DIA51", "SP57ARR02_10");
                    }
                    CB->joindre(-P_GUARDIAN17, 1540, -2000, 0x1667, -1);
                    CB->animation("Sar_100");
                    CB->modify_var(&ST(0x114), 1);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* loc_1000EEB9 */
                if (ST(0x718) == 1) {
                    if (ST(0x64) == 1) {
                        l02_action_register("L02D10", 1, "SG00DIA02", "Sar_10");
                        l02_action_register("L02D103", 0x39, "SP57DIA57", "SP57ARR02_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    if (ST(0x64) == 0) {   /* loc_1000EF2A */
                        ST(0x64) = 1;
                        CB->vire_col(0x21, 0);
                        CB->ajoute_zone(1, 1600, -1830, 80, 90, 0);
                        l02_action_register("L02D11", 1, "SP01ATT70", NULL);
                        l02_action_register("L02D12", 0x39, "SP57DIA57", "SP57ARR02_10");
                        CB->joindre(-P_GUARDIAN17, 1540, -2000, 0x1667, -1);
                        CB->animation("Sar_100");
                        CB->modify_var(&ST(0x114), 1);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }

            /* loc_1000EFF8 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (ST(0xC88) == 0 && ST(0x718) == 0) {
                    ST(0xC88) = 1;
                    l02_action_register("L02D13", 1, "SG00DIA03", NULL);
                    l02_action_register("L02D14", 0x39, "SP57DIA57", "SP57ARR02");
                    l02_action_register("L02D15", 1, "SG00DIA01", NULL);
                    l02_action_register("L02D16", 0x39, "SP57DIA61", "SP57ARR02_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* loc_1000F0B9 */
                if (((ST(0xC88) == 1 || ST(0xC88) == 2) && ST(0x718) == 0)
                    || (ST(0x718) == 1 && ST(0x64) == 0)) {
                    ST(0x718) = 0;   /* loc_1000F110 */
                    ST(0xDC) = 1;
                    if (ST(0x928) == 1) {
                        l02_action_register("L02D19", 1, "SG00DIA06", NULL);
                        ST(0x928) = 0;
                    } else {
                        l02_action_register("L02D19", 1, "SG00DIA04", NULL);
                        ST(0x928) = 1;
                    }
                    if (ST(0xC88) == 1) {
                        l02_action_register("L02D20A", 0x39, "SP57DIA63", "SP57ARR02");
                        CB->animation("SP57GAR26");
                    } else {
                        l02_action_register("L02D20B", 0x39, "SP57DIA63", "SP57ARR02");
                        CB->animation("SP57GAR26");
                    }
                    ST(0xC88) = 2;
                    CB->animation("P57GAR02_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* loc_1000F226 */
                if (ST(0x718) == 1 && ST(0x64) == 1) {
                    l02_action_register("L02D17", 1, "SG00DIA07", NULL);
                    l02_action_register("L02D18", 0x39, "SP57DIA53", "SP57ARR02_10");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
        }

        /* loc_1000F29F */
        if (ST(ST_HERO) >= 2 && ST(0x64) == 0) {
            CB->stop_joindre(-P_GUARDIAN17);
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(0x71C) == 0) {
                    ST(0x71C) = 1;
                    if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D21", 2, "SG00DIA07", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D22", 3, "SG00DIA07", NULL);
                    l02_action_register("L02D23", 0x39, "SP57DIA60", "SP57GAR02_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(-57, ST(ST_HERO));
                    return;
                }
                /* loc_1000F39B */
                if (ST(0x71C) == 1) {
                    if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D24", 2, "SG00DIA07", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D25", 3, "SG00DIA07", NULL);
                    if (!CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(0x71C) = 2;
                        l02_action_register("L02D26", 0x39, "SP57DIA60", "SP57GAR02_10");
                    }
                    if (CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(0x71C) = 3;
                        l02_action_register("L02D27", 0x39, "SP57DIA60", "SP57GAR02_10");
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(-57, ST(ST_HERO));
                    return;
                }
                /* loc_1000F4AF */
                if (ST(0x71C) == 2) {
                    if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D28", 2, "SG00DIA07", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D29", 3, "SG00DIA07", NULL);
                    if (!CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(0xDC) = 0;
                        ST(0x64) = 1;
                        ST(0x718) = 1;
                        CB->vire_col(0x21, 0);
                        CB->ajoute_zone(1, 1600, -1830, 80, 90, 0);
                        ST(0x71C) = 2;
                        l02_action_register("L02D30", 0x39, "SP57GAR27", "SP57ARR02_10");
                        /* NB: zone +0x39 here (not -57), as in the original. */
                        CB->joindre(P_GUARDIAN17, 1540, -2000, 0x1667, -1);
                        CB->animation("Sar_100");
                        CB->modify_var(&ST(0x114), 1);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* loc_1000F5FE */
                    if (CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(0x71C) = 3;
                        l02_action_register("L02D31", 0x39, "SP57DIA60", "SP57GAR02_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->joindre_perm(-57, ST(ST_HERO));
                        return;
                    }
                }
                /* loc_1000F671 */
                if (ST(0x71C) == 3) {
                    if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D32", 2, "SG00DIA07", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D33", 3, "SG00DIA07", NULL);
                    if (!CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(0x718) = 1;
                        ST(0xDC) = 0;
                        CB->vire_col(0x21, 0);
                        ST(0x64) = 1;
                        ST(0x71C) = 2;
                        CB->ajoute_zone(1, 1600, -1830, 80, 90, 0);
                        l02_action_register("L02D12", 0x39, "SP57GAR27", "SP57ARR02_10");
                        CB->joindre(P_GUARDIAN17, 1540, -2000, 0x1667, -1);
                        CB->animation("Sar_100");
                        CB->modify_var(&ST(0x114), 1);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* loc_1000F7C0 */
                    if (CB->divers(DV_CAN_FIGHT, 0, 2)) {
                        ST(0x71C) = 3;
                        l02_action_register("L02D35A", 0x39, "SP57DIA60", "SP57GAR02_10");
                        l02_action_register("L02D35B", 0x39, "SP57DIA60", "SP57GAR02_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->joindre_perm(-57, ST(ST_HERO));
                        return;
                    }
                }
            }

            /* loc_1000F84C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (ST(0x71C) == 0) {
                    ST(0x71C) = 1;
                    ST(0xDC) = 1;
                    if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D36", 2, "SG00DIA03", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D37", 3, "SG00DIA03", NULL);
                    CB->dialogue("L02D38", P_GUARDIAN17);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->joindre_perm(-57, ST(ST_HERO));
                    return;
                }
                /* loc_1000F922 */
                if (ST(0x71C) >= 1) {
                    ST(0xDC) = 1;
                    ST(0x71C) = 2;
                    if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D39", 2, "SG00DIA08", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D40", 3, "SG00DIA08", NULL);
                    l02_action_register("L02D41", 0x39, "SP57DIA60", "SP57GAR02_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    ST(0x34) = 0x39;
                    CB->vire_col(0x21, 0);
                    CB->combat(P_GUARDIAN17, 0);
                    return;
                }
            }
        }

        /* loc_1000F9FF */
        if (ST(ST_HERO) >= 2 && ST(0x64) == 1) {
            if (ST(0x100) == 0) {
                if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD64", 2, "SG00DIA02", "Sar_10");
                if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD65", 3, "SG00DIA02", "Sar_10");
            }
            if (ST(0x100) == 1) {
                if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D42", 2, "SG00DIA02", "Sar_10");
                if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D43", 3, "SG00DIA02", "Sar_10");
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->joindre_perm(-57, ST(ST_HERO));
            return;
        }

    l_fb24:   /* loc_1000FB24 */
        if (ST(ST_VIE(P_GUARDIAN17)) == VIE_DEAD) {
            CB->fouille(P_GUARDIAN17);
            return;
        }
    }

    /* loc_1000FB46 */
    if (ST(ST_ACTION_KEY) == 1 && ST(0x34) == 0x39 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_perso(P_GUARDIAN17) && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("L02D44", P_ARKHAN1);
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D45", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D46", P_ARKHAN3);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }

    /* loc_1000FC36 */
    if (ST(0x14) >= 1 && ST(0x34) == 0 && ST(ST_VIE(P_GUARDIAN17)) < 2 && CB->direction_perso(P_GUARDIAN17)) {
        CB->retire_zone(1);
        CB->stop_joindre(-P_GUARDIAN17);
        CB->vire_col(0x21, 0);
        ST(0x34) = 0x39;
        ST(0x14) = -1;
        CB->combat(P_GUARDIAN17, 0);
    }
}

/* ---- fork D: trig10, trig12, trig16, trig31, trig41 ---- */

/* Shared body of triggers 10 and 12 (sub_1001014F / sub_1001070E; the two
 * functions are instruction-identical except the first query_998 arg:
 * 0 for trig10, 0xE678 for trig12. sub_100159B5 = trig37 is also
 * byte-identical to trig10). Spawn "ar" at the layer-1 coords, then
 * SPxxACT13/85/86 + LCODxx by level [0x194], [0x198] and query_940(3). */
static void d_trig_act13(i32 q) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && (CB->direction_spec(q) || CB->direction_spec(0xC000))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        {   /* coords evaluated axis 2,1,0 (original order) */
            i32 a2 = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
            i32 a1 = CB->divers(DV_DESC_POS, ST(ST_HERO), 1);
            i32 a0 = CB->divers(DV_DESC_POS, ST(ST_HERO), 0);
            CB->pos_perso(ST(ST_HERO), a0, a1, a2, 1, "ar", 6, 0);
        }
        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SP00ACT13");
                CB->animation("P00_65535");
                ST(0x928) = CB->random(3);
                if (ST(0x928) == 0) CB->dialogue("LCOD09", 0);
                if (ST(0x928) == 1) CB->dialogue("LCOD75", 0);
                if (ST(0x928) == 2) CB->dialogue("LCOD06", 0);
            }
            if (ST(ST_SPECIAL_MODE) == 0)
                l02_action_register("L01D176", 0, "SP00ACT86", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SP00ACT13");   /* sic: SP00, not SP01 */
                CB->animation("P00_65535");
                ST(0x928) = CB->random(3);
                if (ST(0x928) == 0) CB->dialogue("LCOD09", P_ARKHAN1);
                if (ST(0x928) == 1) CB->dialogue("LCOD75", P_ARKHAN1);
                if (ST(0x928) == 2) CB->dialogue("LCOD06", P_ARKHAN1);
            }
            if (ST(ST_SPECIAL_MODE) == 0)
                l02_action_register("L01D176", 1, "SP01ACT86", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(0x928) = CB->random(3);
                if (ST(0x928) == 0) l02_action_register("LCOD10", 2, "SG00ATT01", NULL);
                if (ST(0x928) == 1) l02_action_register("LCOD76", 2, "SG00ATT01", NULL);
                if (ST(0x928) == 2) l02_action_register("LCOD07", 2, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2))
                    l02_action_register("L01D177", 2, "SP02ACT85", "Sar_10");
                if (!CB->divers(DV_CAN_FIGHT, 7, 2))
                    l02_action_register("L01D177", 2, "SP02ACT86", "Sar_10");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(0x928) = CB->random(3);
                if (ST(0x928) == 0) l02_action_register("LCOD11", 3, "SG00ATT01", NULL);
                if (ST(0x928) == 1) l02_action_register("LCOD77", 3, "SG00ATT01", NULL);
                if (ST(0x928) == 2) l02_action_register("LCOD08", 3, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2))
                    l02_action_register("L01D178", 3, "SP03ACT85", "Sar_10");
                if (!CB->divers(DV_CAN_FIGHT, 7, 2))
                    l02_action_register("L01D178", 3, "SP03ACT86", "Sar_10");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

static void trig10(void) {   /* sub_1001014F */
    d_trig_act13(0);
}

static void trig12(void) {   /* sub_1001070E */
    d_trig_act13(0xE678);
}

static void trig16(void) {   /* sub_10010E2B */
    if (CB->direction_perso(P_DELIA)) {
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0) {
            if (ST(ST_VIE(P_DELIA)) < 2) {
                ST(ST_SCRIPT_BUSY) = 1;
                CB->bloque(BLOQUE_ALL);
                CB->stop_joindre(-P_DELIA);
                ST(0xF4) = 0x14;
                /* loc_10010F09 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2)
                    || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(0x40))) {
                    if (ST(ST_HERO) == P_ARKHAN2)
                        l02_action_register("L02D47", 2, "SG00DIA02", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        l02_action_register("L02D48", 3, "SG00DIA02", NULL);
                    l02_action_register("L02D49", 0x0A, "SP10DIA50", "SP10ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0xF4), 1);
                }
                /* loc_10010FA2 */
                if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2)
                    && ST(ST_PLAYER_WEAPON) != ST(0x40)) {
                    ST(0x40) = ST(ST_PLAYER_WEAPON);
                    if (ST(ST_HERO) == P_ARKHAN2)
                        l02_action_register("LCOD18", 2, "SG00PRE80", "SG00PRE81");
                    if (ST(ST_HERO) == P_ARKHAN3)
                        l02_action_register("LCOD19", 3, "SG00PRE80", "SG00PRE81");
                    l02_action_register("L02D104", 0x0A, "SP10DIA54", "SP10ARR01_10");
                    CB->animation("G00PRE82");   /* sic: no leading 'S' in the DLL */
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0xF4), 1);
                }
            }
            /* loc_100110AE */
            if (ST(ST_VIE(P_DELIA)) == VIE_DEAD)
                CB->fouille(P_DELIA);
        }
        /* loc_100110CB */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0x0A) {
            if (ST(ST_HERO) == P_ARKHAN2) {
                if (ST(ST_SPECIAL_MODE) == 1) CB->dialogue("L45D01", P_ARKHAN2);
                if (ST(ST_SPECIAL_MODE) == 0) CB->dialogue("L45D02", P_ARKHAN2);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                if (ST(ST_SPECIAL_MODE) == 1) CB->dialogue("L45D03", P_ARKHAN3);
                if (ST(ST_SPECIAL_MODE) == 0) CB->dialogue("LCOD22A", P_ARKHAN3);
            }
        }
        /* loc_100111A4 */
        if (ST(0x14) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_VIE(P_DELIA)) == 1) {
            ST(0xF4) = 0x0A;
            CB->stop_joindre(-P_DELIA);
            CB->bloque(BLOQUE_ALL);
            ST(0x14) = -1;
            if (ST(0x7CC) == 1) {
                ST(0x380) = 0x14;
                ST(0xF1C) = 2;
                ST(ST_VIE(P_DELIA)) = VIE_DEAD;
                CB->animation("SP10IMP51");
                CB->animation("P10cad_10");
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            if (ST(0x7CC) == 0) {
                ST(0x7CC) = 1;
                CB->animation("SP10ESA01");
                CB->animation("P10GAR01_10");
                CB->dialogue("L45D04", P_DELIA);
                CB->animation("Sar_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0xF4), 1);
                return;
            }
        }
        return;
    }

    /* loc_100112FC: zone not owned by actor 10 -> re-dispatch the triggers
     * whose layer-9 test passes for the current hero. */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x01)) trig01();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x04)) trig04();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x05)) trig05();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x06)) trig06();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x08)) trig08();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x09)) trig09();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x0D)) trig13();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x0F)) trig15();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x11)) trig17();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x12)) trig18();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x13)) trig19();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x1F)) trig31();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x20)) trig32();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x21)) trig33();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x22)) trig34();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x23)) trig35();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x26)) trig38();
}

static void trig31(void) {   /* sub_100149FB */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD
        && ST(0x34) == 0 && CB->direction_perso(P_PHEDORIA)) {

        if (ST(ST_PLAYER_WEAPON) == 0x31 && ST(0xEA8) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->vire_col(0x21, 0);
            if (ST(ST_HERO) == P_ARKHAN2)
                l02_action_register("L02D209", 2, "SG00DIA03", "Sar_10");
            if (ST(ST_HERO) == P_ARKHAN3)
                l02_action_register("L02D210", 3, "SG00DIA03", "Sar_10");
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
                CB->animation("G02PRE81");    /* sic */
                CB->animation("SP26PRE60");
                CB->animation("P26_65535");
                CB->animation("SG02PRE82");
                if (ST(ST_PLAYER_WEAPON) != 0) CB->animation("G02ARR02_10");
                else                CB->animation("G02ARR01_10");
                CB->animation("SP26ARR01_10");
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03PRE80");
                CB->animation("G03PRE81");    /* sic */
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

        /* loc_10014D4B */
        if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2)
            || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(0x40))) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            if (ST(0xAEC) == 0) {
                ST(0xAEC) = 1;
                if (ST(ST_HERO) == P_ARKHAN2)
                    l02_action_register("L02D200", 2, "SG00DIA07", NULL);
                if (ST(ST_HERO) == P_ARKHAN3)
                    l02_action_register("L02D201", 3, "SG00DIA07", NULL);
                l02_action_register("L02D202",  0x1A, "SP26DIA56", "SP26ARR03_10");
                l02_action_register("L02D203A", 0x1A, "SP26DIA59", "SP26ARR03_10");
                l02_action_register("L02D203B", 0x1A, "SP26DIA59", "SP26ARR03_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            if (ST(0xAEC) == 1 && ST(0xD8) == 0) {
                ST(0xD8) = 1;
                if (ST(ST_HERO) == P_ARKHAN2)
                    l02_action_register("L02D204", 2, "SG00DIA07", NULL);
                if (ST(ST_HERO) == P_ARKHAN3)
                    l02_action_register("L02D205", 3, "SG00DIA07", NULL);
                l02_action_register("L02D206", 0x1A, "SP26DIA60", "SP26ARR03_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            if (ST(0xAEC) == 1 && ST(0xD8) == 1) {
                if (ST(ST_HERO) == P_ARKHAN2)
                    l02_action_register("L02D207", 2, "SG00DIA07", "Sar_10");
                if (ST(ST_HERO) == P_ARKHAN3)
                    l02_action_register("L02D208", 3, "SG00DIA07", "Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* none matched: falls through (group still open, [0x30]=1) */
        }

        /* loc_10014FE3 */
        if (ST(ST_PLAYER_WEAPON) != 0
            && (ST(ST_PLAYER_WEAPON) != 0x31 || (ST(ST_PLAYER_WEAPON) == 0x31 && ST(0xEA8) == 0))
            && !CB->divers(DV_CAN_FIGHT, 0, 2)
            && ST(ST_PLAYER_WEAPON) != ST(0x40)) {
            ST(0x40) = ST(ST_PLAYER_WEAPON);
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            if (ST(ST_HERO) == P_ARKHAN2)
                l02_action_register("L02D212", 2, "SG00PRE80", "SG00PRE81");
            if (ST(ST_HERO) == P_ARKHAN3)
                l02_action_register("L02D213", 3, "SG00PRE80", "SG00PRE81");
            l02_action_register("L02D214", 0x1A, "SP26DIA59", "SP26ARR03_10");
            CB->animation("G00PRE82");   /* sic */
            CB->animation("ar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }

    /* loc_10015125 */
    if ((ST(0x14) >= 1 || (ST(0x10C) == 1 && ST(0x110) == 0))
        && ST(0x34) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        if (ST(0x14) >= 1)
            ST(0x14) = -1;
        if (ST(0x10C) == 1 && ST(0x110) == 0) {
            ST(0x10C) = 0;
            ST(0x110) = 1;
        }
        if (ST(0x8D0) == 0) {
            ST(0x8D0) = 1;
            CB->animation("SP26ESA25");
            CB->animation("P26_65535");
            l02_action_register("L02D215", 0x1A, "SP26DIA60", "SP26ARR03_100");
            if (ST(0x110) == 1)
                CB->modify_var(&ST(0x110), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(0x8D0) == 1) {
            CB->vire_col(0x21, 0);
            CB->joindre_perm(-26, ST(ST_HERO));
            CB->stop_joindre(P_PHEDORIA);
            if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x1F)) {
                CB->animation("SP26ESA25");
                CB->animation("P26_65535");
                l02_action_register("L02D216", 0x1A, "SP26DIA60", "SP26ARR03_10");
            }
            CB->evenement(0x27, 0x352);
            CB->animation("SG26FRA01");
            CB->animation("P26GAR03_10");
        }
    }
}

static void trig41(void) {   /* sub_1000924D */
    CB->divers_empile(DV_MUSIC, 1, 1);

    if (ST(0xF68) != ST(ST_HERO)
        && (ST(0x380) == 8
            || (ST(0xE9C) == 3 && ST(0xEF0) != 0
                && (ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1)))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(0x380) == 8) {
                CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
                l02_action_register("LCOD48", 0x0A, "SP10DIA54", "SP10ARR01_10");
            }
            if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI13)) == 1) {
                if (ST(0xB8) == 0) CB->change_camera(2, CAM_LOCKED);
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                l02_action_register("LCOD49", 0x0D, "SP13DIA54", "SP13ARR01_10");
            }
            if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1) {
                if (ST(0xB8) == 0) CB->change_camera(2, CAM_LOCKED);
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                l02_action_register("LCOD49", 0x21, "SP33DIA54", "SP33ARR01_10");
            }
            if (ST(ST_SPECIAL_MODE) == 1) l02_action_register("LCOD50", 2, "SG00DIA02", NULL);
            if (ST(ST_SPECIAL_MODE) == 0) l02_action_register("LCOD51", 2, "SG00DIA02", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(0x380) == 8) {
                CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
                l02_action_register("LCOD44", 0x0A, "SP10DIA54", "SP10ARR01_10");
            }
            if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI13)) == 1) {
                if (ST(0xB8) == 0) CB->change_camera(2, CAM_LOCKED);
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                l02_action_register("LCOD45", 0x0D, "SP13DIA54", "SP13ARR01_10");
            }
            if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1) {
                if (ST(0xB8) == 0) CB->change_camera(2, CAM_LOCKED);
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                l02_action_register("LCOD45", 0x21, "SP33DIA54", "SP33ARR01_10");
            }
            if (ST(ST_SPECIAL_MODE) == 1) l02_action_register("LCOD46", 3, "SG00DIA08", NULL);
            if (ST(ST_SPECIAL_MODE) == 0) l02_action_register("LCOD47", 3, "SG00DIA08", NULL);
        }
        if (ST(0xB8) == 0) CB->change_camera(2, CAM_AUTO);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }

    /* loc_10009672 */
    if (ST(0x380) == 7)
        ST(0x380) = 8;

    if (ST(ST_HERO) == P_ARKHAN0) {
        CB->change_camera(0, CAM_LOCKED);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(ST(ST_HERO), 0, 0, 0, 0x54);
        CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
        CB->dialogue("L02D01", P_ZED84);
        l02_action_register("L02D02A", 0x54, "SP84DIA60", "SP84ARR01_10");
        l02_action_register("L02D02B", 0x54, "SP84DIA60", "SP84ARR01_10");
        l02_action_register("L02D02C", 0x54, "SP84DIA59", "SP84ARR01_10");
        CB->evenement(0x21, 0x1F4);
        CB->animation("SP84ACT04");
    }

    /* loc_10009775 */
    if (ST(ST_HERO) == P_ARKHAN1 && ST(0xAF8) == 0 && ST(0xB04) == 0) {
        if (ST(0xBA0) == 1) {
            CB->change_camera(0, CAM_LOCKED);
            ST(0xBA0) = 3;
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->joindre(ST(ST_HERO), 0, 0, 0, 0x54);
            l02_action_register("L02D03A", 0x54, "SP84DIA54", "SP84ARR01");
            l02_action_register("L02D03B", 0x54, "SP84DIA56", "SP84ARR01");
            l02_action_register("L02D03C", 0x54, "SP84DIA54", "SP84ARR01");
            l02_action_register("L02D03D", 0x54, "SP84DIA62", "SP84ARR01");
            l02_action_register("L02D03E", 0x54, "SP84DIA56", "SP84ARR01_10");
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->modify_var(&ST(0x74), 1);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(3, CAM_AUTO);
            CB->joindre(-P_ZED84, 1400, -695, -1, -1);
        }
        /* loc_100098E7 */
        if (ST(0xBA0) == 2) {
            ST(0xBA0) = 3;
            CB->change_camera(0, CAM_LOCKED);
            CB->bloque(BLOQUE_ALL);
            CB->joindre(ST(ST_HERO), 0, 0, 0, 0x54);
            l02_action_register("L02D03Z", 0x54, "SP84DIA54", "SP84ARR01_10");
            l02_action_register("L02D03B", 0x54, "SP84DIA56", "SP84ARR01_10");
            l02_action_register("L02D03C", 0x54, "SP84DIA54", "SP84ARR01_10");
            l02_action_register("L02D03D", 0x54, "SP84DIA62", "SP84ARR01");
            l02_action_register("L02D03E", 0x54, "SP84DIA56", "SP84ARR01_10");
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->modify_var(&ST(0x74), 1);
            CB->bloque(BLOQUE_NONE);
            CB->change_camera(3, CAM_AUTO);
            CB->joindre(-P_ZED84, 1400, -695, -1, -1);
        }
    }

    /* loc_10009A17 */
    if (ST(0xEC8) == 1 && ST(0xAF4) < 3) {
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        l02_action_register("L02D666A", 0x54, "SP84DIA60", "SP84ARR01_10");
        if (ST(ST_VIE(P_GUARDIAN3)) == VIE_DEAD && ST(ST_VIE(P_GUARDIAN4)) == VIE_DEAD)
            l02_action_register("L02D666B", 0x54, "SP84DIA61", "SP84ARR01_10");
        if (ST(ST_SPECIAL_MODE) == 1) l02_action_register("L02D667", 1, "G00DIA02", NULL);
        if (ST(ST_SPECIAL_MODE) == 0) l02_action_register("L02D668", 1, "G00DIA01", NULL);
        CB->dialogue("L02D669", P_ZED84);
        CB->animation("SP84ACT01");
        CB->evenement(2, 0x640);
        CB->select_pile(0, 1);
    }

    /* loc_10009B34 */
    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1 && CB->divers(DV_PERSONA_PRESENT, P_KALHI33, 0)
        && ST(0xEF0) == 0) {
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        ST(0x948) = 1;
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        l02_action_register("L02D70A", 0x21, "SP33DIA56", "SP33ARR01_10");
        l02_action_register("L02D70B", 0x21, "SP33DIA59", "SP33ARR01_10");
        ST(0x50) = 1;
        if (ST(0xF3C) == 1) {
            CB->joindre(-P_KALHI33, -510, -2120, -1, -1);
            CB->bloque(BLOQUE_NONE);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 2);
            CB->joindre(P_KALHI33, -510, -1950, -1, -1);
        }
        if (ST(0xF3C) == 2) {
            CB->joindre(-P_KALHI33, -510, -571, -1, -1);
            CB->bloque(BLOQUE_NONE);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 2);
            CB->joindre(P_KALHI33, -510, -740, -1, -1);
        }
        CB->animation("SP33ARR01_10");
        CB->modify_var(&ST(0x50), 3);
        CB->joindre(P_KALHI33, -1090, -1380, 0xC350, -1);
        ST(0xFC) = 1;
        CB->select_pile(0, 1);
    }

    /* loc_10009D36 */
    if (ST(0xE9C) == 3 && ST(0xEF0) == 0)
        ST(0xEF0) = 1;

    if (ST(0xE9C) == 3) {
        if (ST(ST_VIE(P_KALHI13)) == 1) CB->divers(DV_LOOK_AT_PLAYER, P_KALHI13, 1);
        if (ST(ST_VIE(P_KALHI33)) == 1) CB->divers(DV_LOOK_AT_PLAYER, P_KALHI33, 1);
    }
}

/* -------------------------------------------------------------------------
 * sub_1001A24C (thunk sub_100010D7) — used only by c_pre_scene.
 * __fastcall(ecx=n). Builds "OB%02d" from state[0xD38] and plays the
 * SPnnPRE90 / SPnnPRE91 sequences of actor n around an attach_99C.
 * ---------------------------------------------------------------------- */
static void c_pre90_scene(i32 n) {                          /* sub_1001A24C */
    c8 ob[256];
    c8 b[256];
    snprintf(ob, sizeof(ob), "OB%02d", ST(0xD38));
    snprintf(b, sizeof(b), "SP%02dPRE90", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    CB->link_objet(n, ob, 0x1B, 0);
    snprintf(b, sizeof(b), "P%02dPRE91", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02darr01_10", n);
    CB->animation(b);
}

/* -------------------------------------------------------------------------
 * sub_1001A35C (thunk sub_100010AF) — used only by trig26.
 * __fastcall(ecx=hero, edx=n) + stdcall stack (a1 unused, a2, a3);
 * ret 0Ch. a2/a3 go to query_990.
 * ---------------------------------------------------------------------- */
static void c_pre_scene(i32 hero, i32 n, i32 a1, i32 a2, i32 a3) { /* sub_1001A35C */
    c8 b[256];
    (void)a1;   /* [ebp+8] is never read */

    if (ST(ST_PLAYER_WEAPON) != 0 && ST(0xE00) == 1 &&
        (ST(ST_LEVEL) == 5 || ST(ST_LEVEL) == 15 || ST(ST_LEVEL) == 18 ||
         ST(ST_LEVEL) == 36 || ST(ST_LEVEL) == 48)) {
        ST(ST_PLAYER_WEAPON) = 0;
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02PRE73");
            CB->animation("P02_65535");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03PRE73");
            CB->animation("P03_65535");
        }
    }
    if (ST(ST_PLAYER_WEAPON) != 0 && ST(0xD38) != 0)
        c_pre90_scene(n);

    if (CB->distance(a2, a3) < 0x50 && n != 6) {
        snprintf(b, sizeof(b), "Sp%02dMAR30", hero);
        CB->animation(b);
        snprintf(b, sizeof(b), "P%02d_65535", hero);
        CB->animation(b);
    }
    snprintf(b, sizeof(b), "SG%02dPRE80", hero);
    CB->animation(b);
    snprintf(b, sizeof(b), "G%02dPRE81", hero);    /* sic: no leading 'S' */
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02dPRE60", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "SG%02dPRE82", hero);
    CB->animation(b);
    if (ST(ST_PLAYER_WEAPON) != 0)
        snprintf(b, sizeof(b), "G%02darr02_10", hero);
    else
        snprintf(b, sizeof(b), "G%02darr01_10", hero);
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02darr01_10", n);
    CB->animation(b);
}

/* trigger 26 — sub_10012166. Actor 13 (SP13) dialogue zone; when the
 * actor query fails, re-dispatches the triggers whose layer-9 bit is set. */
static void trig26(void) {   /* sub_10012166 */
    if (ST(0x10C) == 1 && ST(0x110) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 1
        && CB->direction_perso(P_KALHI13) && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(0x110) = 1;
        CB->bloque(BLOQUE_ALL);
        l02_action_register("L04D41", 0x0D, "SP13DIA59", "SP13ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(0x110), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    if (CB->direction_perso(P_KALHI13)) {
        if (ST(ST_ACTION_KEY) == 1 && ST(0x34) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
            CB->select_pile(0, 2);
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(0xB8) = 0x64;
                ST(0x90) = 2;
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                CB->joindre(ST(ST_HERO), 0, 0, 0, 0x0D);

                if (ST(0x8A0) == 0) {                     /* 0x1001230E */
                    ST(0x8A0) = 1;
                    if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L04D51A", 2, "SG00DIA02", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L04D52A", 3, "SG00DIA02", NULL);
                    l02_action_register("L04D53A", 0x0D, "SP13DIA50", "SP13ARR01_10");
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        l02_action_register("L04D51B", 2, "SG00DIA02", NULL);
                        l02_action_register("L04D51C", 2, "SG00DIA02", NULL);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L04D52B", 3, "SG00DIA02", NULL);
                    l02_action_register("L04D53B", 0x0D, "SP13DIA53", "SP13ARR01_10");
                    l02_action_register("L04D53C", 0x0D, "SP13DIA54", "SP13ARR01_10");
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L04D54", 2, "SG00DIA04", NULL);
                        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L04D55", 3, "SG00DIA04", NULL);
                        l02_action_register("L04D56A", 0x0D, "SP13DIA55", "SP13ARR01_10");
                        l02_action_register("L04D56B", 0x0D, "SP13DIA56", "SP13ARR01_10");
                    }
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L04D57", 2, "SG00DIA04", NULL);
                        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L04D58", 3, "SG00DIA04", NULL);
                        l02_action_register("L04D59", 0x0D, "SP13DIA59", "SP13ARR01_10");
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->modify_var(&ST(0xB8), 1);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }

                if (ST(0x8A0) == 1) {                     /* 0x1001256F */
                    if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2)
                        || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(0x40))) {
                        /* 0x100125D6 */
                        if (ST(0x828) == 0 || ST(0x828) == 1) {
                            ST(0x828) = 2;
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                l02_action_register("L04D63A", 2, "SG00DIA05", NULL);
                                l02_action_register("L04D63B", 2, "SG00DIA06", NULL);
                            }
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                l02_action_register("L04D64A", 3, "SG00DIA05", NULL);
                                l02_action_register("L04D64B", 3, "SG00DIA05", NULL);
                            }
                            l02_action_register("L02D108", 0x0D, "SP13DIA53", "SP13ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0xB8), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        if (ST(0x828) == 2) {             /* 0x100126E0 */
                            if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D101A", 2, "SG00DIA06", NULL);
                            if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D101B", 3, "SG00DIA06", NULL);
                            l02_action_register("L02D102", 0x0D, "SP13DIA54", "SP13ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0xB8), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                    }
                    /* 0x1001279D */
                    if (ST(ST_PLAYER_WEAPON) == 1) {
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            l02_action_register("L02D114A", 2, "SG00DIA02", NULL);
                            CB->dialogue("L02D114B", P_ARKHAN2);
                        }
                        if (ST(ST_HERO) == P_ARKHAN3)
                            CB->dialogue("L02D115", P_ARKHAN3);
                        c_pre_scene(ST(ST_HERO), 0x0D, 1, 6, 5);
                        CB->dialogue("L02D116", P_KALHI13);
                        CB->animation("SP13ARR01_50");
                        CB->modify_var(&ST(0x74), 1);
                        return;   /* sic: no group(0) / state_toggle here */
                    }
                    /* 0x10012858 */
                    if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 1 && !CB->divers(DV_CAN_FIGHT, 0, 2)
                        && ST(ST_PLAYER_WEAPON) != ST(0x40)) {
                        ST(0x40) = ST(ST_PLAYER_WEAPON);
                        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L08D336", 2, "SG00PRE80", "SG00PRE81");
                        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L08D337", 3, "SG00PRE80", "SG00PRE81");
                        l02_action_register("L08D338", 0x0D, "SP13DIA54", "SP13ARR01_10");
                        CB->animation("G00PRE82");   /* sic */
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0xB8), 1);
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
            }
            /* 0x10012988 */
            if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD)
                CB->fouille(P_KALHI13);
        }

        /* 0x100129A5 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(0x34) == 0x0D) {
            if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L08D333", P_ARKHAN2);
            if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L08D334", P_ARKHAN3);
        }

        /* 0x10012A16 */
        if (ST(0x14) >= 1 && ST(0x34) == 0 && ST(ST_VIE(P_KALHI13)) == 1) {
            CB->select_pile(0, 2);
            if (ST(0x14) == 2) {
                ST(0x14) = -1;
                CB->stop_joindre(-P_KALHI13);
                CB->dialogue("L04D43", P_KALHI13);
                ST(0x8A8) = 2;
                ST(0x34) = 0x0D;
                CB->combat(P_KALHI13, 0);
                return;
            }
            CB->stop_joindre(-P_KALHI13);                          /* 0x10012AB6 */
            ST(0xB8) = 0x64;
            CB->bloque(BLOQUE_ALL);
            ST(0x14) = -1;
            CB->animation("SP13ESA15");
            CB->animation("P13_65535");
            if (ST(0x8A8) == 0) {
                ST(0x8A8) = 1;
                l02_action_register("L04D41", 0x0D, "P13DIA59_10", "SP13ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0xB8), 1);
                CB->select_pile(0, 1);
                return;
            }
            if (ST(0x8A8) == 1) {
                ST(0x8A8) = 2;
                l02_action_register("L04D42", 0x0D, "P13DIA59_10", "SP13ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0xB8), 1);
                CB->select_pile(0, 1);
                return;
            }
            if (ST(0x8A8) == 2) {
                ST(0xE9C) = 4;
                l02_action_register("L04D43", 0x0D, "P13GAR26_10", "SP13GAR02_10");
                CB->bloque(BLOQUE_NONE);
                CB->combat(P_KALHI13, 0);
                ST(0x34) = 0x0D;
                CB->select_pile(0, 1);
                return;
            }
        }
        return;
    }

    /* 0x10012C45 — actor 13 query failed: forward to the triggers whose
     * layer-9 bit is set for the current hero. */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x01)) trig01();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x04)) trig04();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x05)) trig05();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x06)) trig06();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x08)) trig08();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x09)) trig09();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x0D)) trig13();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x0F)) trig15();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x11)) trig17();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x12)) trig18();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x13)) trig19();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x1F)) trig31();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x20)) trig32();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x21)) trig33();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x22)) trig34();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x23)) trig35();
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x26)) trig38();
}

/* trig27 tail, inlined 5 times identically in the original (0x100131CB,
 * 0x10013540, 0x10013928, 0x10013DD0, 0x100140F3): walk actor 33 along
 * its path by step state[0x50] (0..3) and route [0xF3C] (1/2), then
 * state_toggle(0,1). */
static void c_t27_walk(void) {
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
            return;
        }
        if (ST(0x50) == 2) {
            CB->joindre(-P_KALHI33, -510, -1950, -1, -1);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 3);
            CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
            CB->select_pile(0, 1);
            return;
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
            return;
        }
        if (ST(0x50) == 2) {
            CB->joindre(-P_KALHI33, -510, -740, -1, -1);
            CB->animation("SP33ARR01_10");
            CB->modify_var(&ST(0x50), 3);
            CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
            CB->select_pile(0, 1);
            return;
        }
    }
    if (ST(0x50) == 3)
        CB->joindre(-P_KALHI33, -1090, -1380, 0xC350, -1);
    CB->select_pile(0, 1);
}

/* trigger 27 — sub_10012EBF. Actor 33 (SP33) dialogue zone. */
static void trig27(void) {   /* sub_10012EBF */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x05)) { trig05(); return; }
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x13)) { trig19(); return; }

    if (ST(0x10C) == 1 && ST(0x110) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 1
        && CB->direction_perso(P_KALHI33) && ST(ST_VIE(P_KALHI33)) == 1) {
        ST(0x110) = 1;
        CB->bloque(BLOQUE_ALL);
        l02_action_register("L04D41", 0x21, "SP33DIA59", "SP33ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(0x110), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && CB->direction_perso(P_KALHI33)) {
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            CB->select_pile(1, 2);
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->stop_joindre(-P_KALHI33);
            CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
            CB->joindre(ST(ST_HERO), 0, 0, 0, 0x21);

            if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2)
                || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(0x40))) {
                /* 0x100130EE */
                if (ST(0x828) == 0 || ST(0x828) == 1) {
                    ST(0x828) = 2;
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        l02_action_register("L04D28A", 2, "SG00DIA05", NULL);
                        if (ST(0x2A8) != 2)
                            l02_action_register("L04D28B", 2, "SG00DIA05", NULL);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L04D29", 3, "SG00DIA05", NULL);
                    l02_action_register("L04D30", 0x21, "SP33DIA53", "SP33ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    c_t27_walk();
                    return;
                }
                if (ST(0x828) == 2) {                     /* 0x100134AC */
                    if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L02D101A", 2, "SG00DIA06", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L02D101B", 3, "SG00DIA06", NULL);
                    l02_action_register("L02D102", 0x21, "SP33DIA54", "SP33ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    c_t27_walk();
                    return;
                }
            }
            /* 0x10013821 */
            if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 1 && !CB->divers(DV_CAN_FIGHT, 0, 2)
                && ST(ST_PLAYER_WEAPON) != ST(0x40)) {
                ST(0x40) = ST(ST_PLAYER_WEAPON);
                if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("L08D336", 2, "SG00PRE80", "SG00PRE81");
                if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("L08D337", 3, "SG00PRE80", "SG00PRE81");
                l02_action_register("L08D338", 0x21, "SP33DIA54", "SP33ARR01_10");
                CB->animation("G00PRE82");   /* sic */
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                c_t27_walk();
                return;
            }
        }
        /* 0x10013C09 */
        if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
            CB->fouille(P_KALHI33);
            return;
        }
    }

    /* 0x10013C2B */
    if (ST(ST_ACTION_KEY) == 1 && ST(0x34) == 0x21 && CB->direction_perso(P_KALHI33)) {
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L08D333", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L08D334", P_ARKHAN3);
    }

    /* 0x10013CA0 */
    if (ST(0x14) >= 1 && ST(0x34) == 0 && ST(ST_VIE(P_KALHI33)) == 1 && CB->direction_perso(P_KALHI33)) {
        CB->select_pile(1, 2);
        if (ST(0x14) == 2) {
            ST(0x14) = -1;
            CB->stop_joindre(-P_KALHI33);
            CB->dialogue("L08D341", P_KALHI33);
            ST(0x8A8) = 2;
            ST(0x34) = 0x21;
            CB->combat(P_KALHI33, 0);
            return;
        }
        CB->stop_joindre(-P_KALHI33);                              /* 0x10013D56 */
        ST(0x14) = -1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SP33ESA15");
        CB->animation("P33_65535");
        if (ST(0x8A8) == 0) {
            ST(0x8A8) = 1;
            l02_action_register("L08D339", 0x21, "P33DIA59_10", "SP33ARR01_10");
            CB->bloque(BLOQUE_NONE);
            c_t27_walk();
            return;
        }
        if (ST(0x8A8) == 1) {
            ST(0x8A8) = 2;
            l02_action_register("L08D340", 0x21, "P33DIA59_10", "SP33ARR01_10");
            CB->bloque(BLOQUE_NONE);
            c_t27_walk();
            return;
        }
        if (ST(0x8A8) == 2) {
            ST(0xE9C) = 4;
            l02_action_register("L08D341", 0x21, "P33GAR26", "SP33GAR02_10");
            CB->bloque(BLOQUE_NONE);
            CB->combat(P_KALHI33, 0);
            ST(0x34) = 0x21;
            CB->select_pile(0, 1);
            return;
        }
    }
}

static void trig11(void) {   /* sub_1001065F */
    if (ST(ST_ACTION_KEY) != 1) return;
    if (!CB->direction_perso(P_GUARDIAN18)) return;
    if (ST(ST_VIE(P_GUARDIAN18)) == 1 && ST(ST_COMBAT_STANCE) == 0) {
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D45", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D46", P_ARKHAN3);
    }
    if (ST(ST_VIE(P_GUARDIAN18)) == VIE_DEAD)
        CB->fouille(P_GUARDIAN18);
}

/* triggers 13/15/38 — sub_10010C21, sub_10010D33, sub_10015EC5. Same body:
 * LCOD06/07/08 + SG00PRE03 by level [0x194]. trig15 has no [0x10] guard;
 * the query_998 arg differs (0xFFDC / 0x8000 / 0). */
static void trig13(void) {   /* sub_10010C21 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_spec(0xFFDC)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN1) l02_action_register("LCOD06", 1, "SG00PRE03", NULL);
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD07", 2, "SG00PRE03", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD08", 3, "SG00PRE03", NULL);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

static void trig15(void) {   /* sub_10010D33 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0
        && CB->direction_spec(0x8000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN1) l02_action_register("LCOD06", 1, "SG00PRE03", NULL);
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD07", 2, "SG00PRE03", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD08", 3, "SG00PRE03", NULL);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

static void trig38(void) {   /* sub_10015EC5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN1) l02_action_register("LCOD06", 1, "SG00PRE03", NULL);
        if (ST(ST_HERO) == P_ARKHAN2) l02_action_register("LCOD07", 2, "SG00PRE03", NULL);
        if (ST(ST_HERO) == P_ARKHAN3) l02_action_register("LCOD08", 3, "SG00PRE03", NULL);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* triggers 32/33 — sub_1001531B, sub_10015416. L02D29x/30x by level. */
static void trig32(void) {   /* sub_1001531B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_spec(0xCE8C)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("L02D298", P_ARKHAN1);
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D300", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D301", P_ARKHAN3);
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

static void trig33(void) {   /* sub_10015416 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_spec(0xEE08)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("L02D299", P_ARKHAN1);
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D302", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D303", P_ARKHAN3);
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

static void trig35(void) {   /* sub_100156DC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_spec(0x7FF4)) {
        CB->bloque(BLOQUE_ALL);
        ST(0xF68) = ST(ST_HERO);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(ST_LEVEL) = 2;
        CB->animation("SG00OUV01");
        CB->animation("ar");
        CB->change_piece(52, 0);   /* no group(0): the DLL leaves the group open */
    }
}


static void trig17(void) {   /* sub_10011576 */
    CB->select_pile(0, 2);
    ST(0xB2C) = 2;
    CB->retire_zone(17);
    if (ST(0xEC0) == 0)
        ST(0xEC0) = 1;
    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI13)) == 1) {
        CB->bloque(BLOQUE_ALL);
        ST(0xF68) = ST(ST_HERO);
        ST(0xB8) = 0x14;
        ST(0x90) = 0x0A;
        CB->stop_joindre(-P_KALHI13);
        CB->dialogue("L02D500", P_KALHI13);
        CB->animation("SP13ARR01_300");
        CB->bloque(BLOQUE_NONE);
    }
    if (ST(0xE9C) == 3 && ST(ST_VIE(P_KALHI33)) == 1 && ST(0x7C) == 0 && ST(0x34) == 0) {
        ST(0x7C) = 1;
        ST(0xF68) = ST(ST_HERO);
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        CB->change_camera(5, CAM_LOCKED);
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        l02_action_register("L02D65A", 0x21, "SP33DIA59", "SP33ARR01");
        l02_action_register("L02D65C", 0x21, "SP33DIA56", "SP33ARR01_10");
        CB->animation("SP33ARR01_10");
        CB->change_camera(5, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
    }
    ST(ST_LEVEL) = 2;
    CB->divers_empile(DV_MUSIC, 1, 4);
    CB->change_piece(3, 0);
}

static void trig18(void) {   /* sub_10011785 */
    CB->select_pile(0, 2);
    if (ST(0xBD0) == 1)
        ST(0xBD0) = 2;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    CB->retire_zone(18);
    if (ST(0xEC0) == 0)
        ST(0xEC0) = 1;
    if (ST(ST_HERO) != 4 && ST(0xAF8) == 0 && ST(0xBDC) == 1
        && CB->divers(DV_PERSONA_PRESENT, P_ZED84, 0)) {
        i32 d54 = CB->divers(DV_DESC_POS, P_ZED84, 3);
        i32 dme = CB->divers(DV_DESC_POS, ST(ST_HERO), 3);
        if (d54 < dme) ST(0xBDC) = 3;
        else           ST(0xBDC) = 2;
    } else {
        ST(0xBDC) = 2;
    }
    ST(0xAFC) = 4;
    ST(0xF68) = ST(ST_HERO);
    ST(ST_LEVEL) = 2;
    CB->change_piece(7, 0);
}

static void trig19(void) {   /* sub_100118DF */
    CB->select_pile(0, 2);
    ST(0xB44) = 1;
    CB->retire_zone(19);
    if (ST(0x380) == 8) {
        CB->bloque(BLOQUE_ALL);
        ST(0xF68) = ST(ST_HERO);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->stop_joindre(-P_DELIA);
        ST(0xF4) = 0x0A;
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D501", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D502", P_ARKHAN3);
        CB->bloque(BLOQUE_NONE);
        CB->modify_var(&ST(0xB44), 1);
    }
    if (ST(0xE9C) == 3) {
        ST(0xEF0) = 0;
        ST(0xE9C) = 5;
        if (ST(ST_VIE(P_KALHI13)) == 1 && ST(0x794) == 0) {
            CB->stop_joindre(-P_KALHI13);
            ST(0xB8) = 0x0A;
        }
        if (ST(ST_VIE(P_KALHI33)) == 1)
            CB->stop_joindre(-P_KALHI33);
    }
    CB->divers_empile(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 2;
    if (ST(0xB4C) == 3)
        CB->change_piece(8, 2);
    else if (ST(0xF20) == 1 || ST(0xE48) != 0)
        CB->change_piece(8, 1);
    else
        CB->change_piece(8, 0);
}

static void trig34(void) {   /* sub_10015511 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_zone(0x22)) {
        CB->bloque(BLOQUE_ALL);
        {
            i32 a2 = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
            i32 a1 = CB->divers(DV_DESC_POS, ST(ST_HERO), 1);
            i32 a0 = CB->divers(DV_DESC_POS, ST(ST_HERO), 0);
            CB->pos_perso(ST(ST_HERO), a0, a1, a2, 1, "ar", 0x14, 0);
        }
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT13");
        if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ACT13");
        if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ACT13");
        CB->animation("G00ATT01");
        if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("LCOD09", P_ARKHAN1);
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("LCOD10", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("LCOD11", P_ARKHAN3);
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}


/* triggers 23/24 — sub_10011B05, sub_10011D42. Same body, query_944 arg
 * 0x17/0x18. SPxxATT84..87 by [0x198]/[0x194]. The [0x198]==0, level-2
 * branch names "SP03ATT87"/"SP03ATT86" in the DLL (sic) — kept. */
static void e_trig_att(i32 q) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_zone(q)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_SPECIAL_MODE) == 1) {
            if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ATT84");
            if (ST(ST_HERO) == P_ARKHAN2) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP02ATT85");
                else                       CB->animation("SP02ATT84");
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP03ATT85");
                else                       CB->animation("SP03ATT84");
            }
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        } else if (ST(ST_SPECIAL_MODE) == 0) {
            if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ATT86");
            if (ST(ST_HERO) == P_ARKHAN2) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP03ATT87");
                else                       CB->animation("SP03ATT86");
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP03ATT87");
                else                       CB->animation("SP03ATT86");
            }
            CB->animation("ar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
}
static void trig23(void) { e_trig_att(0x17); }   /* sub_10011B05 */
static void trig24(void) { e_trig_att(0x18); }   /* sub_10011D42 */

/* triggers 25/28/29/30 — sub_10011F7F, sub_10014446, sub_1001462D,
 * sub_10014814. Same body, query_944 arg 0x19/0x1C/0x1D/0x1E. Spawn "ar",
 * then SPxxACT11/32 by [0x194]/[0x198]. */
static void e_trig_act(i32 q) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_zone(q)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        {
            i32 a2 = CB->divers(DV_DESC_POS, ST(ST_HERO), 2);
            i32 a1 = CB->divers(DV_DESC_POS, ST(ST_HERO), 1);
            i32 a0 = CB->divers(DV_DESC_POS, ST(ST_HERO), 0);
            CB->pos_perso(ST(ST_HERO), a0, a1, a2, 1, "ar", 6, 0);
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP01ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP01ACT32");
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP02ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP02ACT32");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP03ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP03ACT32");
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}
static void trig25(void) { e_trig_act(0x19); }   /* sub_10011F7F */
static void trig28(void) { e_trig_act(0x1C); }   /* sub_10014446 */
static void trig29(void) { e_trig_act(0x1D); }   /* sub_1001462D */
static void trig30(void) { e_trig_act(0x1E); }   /* sub_10014814 */

static void trig36(void) {   /* sub_10015793 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(0x34) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_zone(0x24)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, -0x25);
        ST(0x928) = CB->random(3);
        if (ST(0x928) == 0) CB->animation("SG00PRE00");
        if (ST(0x928) == 1) CB->animation("SG00PRE01");
        if (ST(0x928) == 2) CB->animation("SG00PRE02");
        if (ST(ST_HERO) == P_ARKHAN1 && ST(ST_SPECIAL_MODE) == 1) {
            CB->dialogue("L02D111A", P_ARKHAN1);
            CB->dialogue("L02D111B", P_ARKHAN1);
            if (ST(0xE00) == 0) {
                CB->dialogue("L02D111C", P_ARKHAN1);
                ST(0xE00) = 1;
            }
        }
        if (ST(ST_HERO) == P_ARKHAN1 && ST(ST_SPECIAL_MODE) == 0) {
            CB->dialogue("L07D34A", P_ARKHAN1);
            CB->dialogue("L07D34B", P_ARKHAN1);
        }
        if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L02D112", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L02D113", P_ARKHAN3);
        CB->animation("ar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 37 — sub_100159B5: byte-identical to trig10 (sub_1001014F), see
 * d_trig_act13. */
static void trig37(void) { d_trig_act13(0); }   /* sub_100159B5 */
