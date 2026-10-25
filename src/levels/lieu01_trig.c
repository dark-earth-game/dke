/*
 * lieu01_trig.c — LIEU01.DLL zone trigger handlers (the trigger table
 * dword_1002C880[42], filled at INIT). See lieu01.c for the entry, INIT,
 * tick and events.
 */

#include <stdio.h>

#include "lieu01_int.h"

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

 * trigger 41 — sub_10008798.  Fully ported.
 * ---------------------------------------------------------------------- */
static void trig41(void) {
    if (ST(V_INTRO_STATE) == 0 && ST(ST_HERO) == P_ARKHAN0) {
        ST(V_INTRO_STATE) = 1;
        CB->evenement(2, 0x0A);
        CB->evenement(0, 0x7530);
    }
    if (ST(ST_HERO) == P_ARKHAN0 && ST(0x74) == 1) {
        ST(0x68) = 1;
        ST(0x88) = 0x29A;
        l01_action_register("L01D17A", 0x0C, "SP12ARR03", "SP12ARR03_10");
        l01_action_register("L01D17B", 0x0C, "SP12ATT06", "SP12ARR03_10");
        CB->change_camera(6, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
        CB->modify_var(&ST(V_EVENT_1), 1);
        CB->modify_var(&ST(0x74), 0);
    }
    if (ST(V_G23_FIGHT_ON) == 1) {
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L01D147", P_GUARDIAN23);
        CB->combat(P_GUARDIAN23, 0);
        CB->bloque(BLOQUE_NONE);
        ST(0x94) = 1;
        ST(V_ENGAGED_PERSONA) = 0x58;
    }
}

/* trigger 25 — sub_10012DAE. Spawn "ar", register LCOD06-11 by state. */
static void trig25(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC738)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();
        ST(V_DLG_RANDOM) = CB->random(2);
        if (ST(V_DLG_RANDOM) == 0) {
            if (ST(ST_HERO) == P_ARKHAN0) l01_action_register("LCOD06", 0, "SG00PRE02", NULL);
            if (ST(ST_HERO) == P_ARKHAN1) l01_action_register("LCOD06", 1, "SG00PRE02", NULL);
            if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD07", 2, "SG00PRE02", NULL);
            if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD08", 3, "SG00PRE02", NULL);
        }
        if (ST(V_DLG_RANDOM) == 1) {
            if (ST(ST_HERO) == P_ARKHAN0) l01_action_register("LCOD09", 0, "SG00PRE03", NULL);
            if (ST(ST_HERO) == P_ARKHAN1) l01_action_register("LCOD09", 1, "SG00PRE03", NULL);
            if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("LCOD10", 2, "SG00PRE03", NULL);
            if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("LCOD11", 3, "SG00PRE03", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* triggers 30/31/32 — sub_10014C56/EDD, sub_10015164. Identical bodies,
 * differ only in query_944 arg (0x1E/0x1F/0x20). Spawn "ar", then
 * SPxxACT11/85/86 by level [0x194] + [0x198]. */
static void e_trig_spawn_act(i32 q) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_zone(q)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();
        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP00ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP00ACT86");
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP01ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP01ACT86");
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP02ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP02ACT85");
                else                       CB->animation("SP02ACT86");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP03ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP03ACT85");
                else                       CB->animation("SP03ACT86");
            }
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
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
        ST(ST_HERO) = P_ARKHAN0;
        ST(V_INTRO_STATE) = 0;
        ST(V_LEFT_HOME) = 0;
        ST(V_OB38A_STATE) = 0;
        ST(V_GATE_OB25_OPEN) = 0;
    }
    if (ST(0x964) == 2) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL)  = 2;
        ST(V_LEFT_HOME) = 1;
        ST(0xB88) = 0;
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xBE4) = 0;
        ST(V_GUARD_WAVE) = 0;
        ST(V_ZED_BRIEFING) = 3;
        ST(V_INTRO_STATE) = 2;
        ST(V_OB38A_STATE) = 1;
        ST(V_GATE_OB25_OPEN) = 1;
    }
    if (ST(0x964) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL)  = 2;
        ST(V_LEFT_HOME) = 1;
        ST(0xB88) = 0;
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xBE4) = 0;
        ST(V_ZED_BRIEFING) = 3;
        ST(V_INTRO_STATE) = 2;
        ST(V_OB38A_STATE) = 1;
        ST(V_GATE_OB25_OPEN) = 1;
    }
    if (ST(0x964) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL)  = 2;
        ST(V_LEFT_HOME) = 1;
        ST(0xB88) = 0;
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xBE4) = 0;
        ST(V_ZED_BRIEFING) = 3;
        ST(V_INTRO_STATE) = 2;
        ST(V_OB38A_STATE) = 1;
        ST(V_GATE_OB25_OPEN) = 1;
    }
    if (ST(0x964) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_KABAK)) = 1;
        ST(ST_LEVEL)  = 2;
        ST(V_LEFT_HOME) = 1;
        ST(V_GUARD_WAVE) = 1;
        ST(0xB88) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(V_GUARD_WAVE) = 1;
        ST(0xBE4) = 0;
        ST(V_ZED_BRIEFING) = 3;
        ST(V_INTRO_STATE) = 2;
        ST(V_OB38A_STATE) = 1;
        ST(V_GATE_OB25_OPEN) = 1;
    }
    if (ST(0x964) == 6) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL)  = 2;
        ST(V_LEFT_HOME) = 1;
        ST(0xB88) = 1;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(V_GUARD_WAVE) = 2;
        ST(0xBE4) = 1;
        ST(V_ZED_BRIEFING) = 3;
        ST(V_OB38A_STATE) = 1;
        ST(V_GATE_OB25_OPEN) = 1;
        ST(V_INTRO_STATE) = 2;
    }

    ST(0x64) = 0;  ST(0x68) = 0;  ST(0x6C) = 0;  ST(0x70) = 0;
    ST(0x74) = 0;  ST(0x78) = 0;  ST(0x7C) = 0;  ST(0x80) = 0;
    ST(0x84) = 0;  ST(0x88) = 0;  ST(V_EVENT15_STEP) = 0;  ST(0x90) = 0;
    ST(0x94) = 0;  ST(0x98) = 0;  ST(0xB8) = 0;  ST(0xBC) = 0;
    ST(0xD4) = 0;  ST(0xD8) = 0;  ST(0xDC) = 0;  ST(0xE0) = 0;
    ST(0xF0) = 0;  ST(V_EVENT_0) = 0; ST(V_EVENT_1) = 0; ST(V_EVENT_2) = 0;
    ST(V_EVENT_33) = 0; ST(V_EVENT_34) = 0; ST(V_EVENT_35) = 0; ST(V_EVENT_36) = 0;
    ST(V_EVENT_37) = 0; ST(V_EVENT_38) = 0; ST(V_EVENT_39) = 0; ST(V_EVENT_41) = 0;
    ST(ST_SCRIPT_BUSY) = 0;  ST(V_ENGAGED_PERSONA) = 0;  ST(V_LAST_WEAPON) = 0;

    CB->bruitage("L01AMB1", 93, -2600);

    CB->ajoute_zone(1, 374, -2894, 155,  90,  0);
    CB->ajoute_zone(3, 515, -2115, 105,  80,  0);
    CB->ajoute_zone(7, 470, -3061, 80,  170,  1);
    CB->ajoute_zone(8, 870, -2800, 120,  90,  0);
    CB->ajoute_zone(11, 43,  -2570, 188,  240,  0);
    CB->ajoute_zone(12, 900, -2820, 25,  40,  0);
    CB->ajoute_zone(13, -62,   -2586, 90,  100,  0);
    CB->ajoute_zone(14, 860, -2520, 60,  60,  0);
    CB->ajoute_zone(15, 860, -2580, 160,  160,  0);
    CB->ajoute_zone(20, 708, -2040, 70,  70,  0);
    CB->ajoute_zone(21, 630, -2150, 160,  240,  0);
    CB->ajoute_zone(22, 337, -2068, 50,  50,  0);
    CB->ajoute_zone(23, 367, -2100, 140,  140,  0);
    CB->ajoute_zone(24, 276, -2425, 285, 240,  0);
    CB->ajoute_zone(25, 715, -2544, 140,  180,  0);
    CB->ajoute_zone(26, 1000, -2447, 140,  120,  0);
    CB->ajoute_zone(27, 37,  -2732, 250,  90,  0);
    CB->ajoute_zone(28, 963, -2750, 80,  160,  0);
    CB->ajoute_zone(29, 679, -2965, 115,  135,  0);
    CB->ajoute_zone(30, 257, -2856, 210,  210,  0);
    CB->ajoute_zone(31, 789, -2326, 320, 238,  0);
    CB->ajoute_zone(32, 721, -2832, 175,  160,  0);

    CB->etat_objet("IM01", 464, 15, -2045, 0xC000, 0x4000, 0x8000, 0, 0);

    if (ST(V_OB44D_TAKEN) == 0) {
        CB->etat_objet("OB44D", 695, 50, -2986, 0, 0, 0x3800, -1, 4);
    }

    CB->vire_col(6, 0);

    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0 &&
        ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
        ST(0xB98) = 0;
    }

    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }

    if (ST(V_OB38A_STATE) == 0) {
        CB->ajoute_zone(5, 685, -2544, 140, 180, 0);
        CB->ajoute_zone(19, 704, -2542, 50, 50, 0);
        CB->etat_objet("OB38A", 703, 59, -2511, 0, 0xE800, 0x7200, -1, 0);
    }

    if (ST(V_GATE_OB25_OPEN) == 0) {
        CB->vire_col(6, 1);
        CB->etat_objet("OB25", 1000, 150, -2620, 0, 0xC000, 0, -1, 3);
        CB->ajoute_zone(2, 963, -2610, 80, 160, 0);
    }

    if (ST(V_INTRO_STATE) == 0 && ST(ST_HERO) == P_ARKHAN0) {
        ST(ST_SPECIAL_MODE) = 1;
        CB->pos_perso(P_KALHI12, 618, -3125, 0, 1, "P12ARR01", -1, 0);
        CB->animation("SP12ARR01_10");
        CB->change_camera(6, CAM_LOCKED);
        CB->bloque(BLOQUE_ALL);
        CB->ajoute_zone(4, 610, -2987, 240,  100, 1);
        CB->ajoute_zone(10, 480, -2300, 600, 100, 1);
        CB->pos_perso(0, 630, -3062, 0, 1, "P00ARR01", 6, 0);
        CB->animation("SP00ARR01");
    }

    if (ST(V_INTRO_STATE) != 0 && ST(V_LEFT_HOME) == 0 && ST(V_GUARD_FIGHT_DONE) == 0 &&
        ST(0xAF8) == 0 && ST(ST_LEVEL) == 2) {
        CB->pos_perso(ST(ST_HERO), 511, -2196, 0x80E8, 1, "ar", 6, 0);
        if (ST(ST_HERO) == P_ARKHAN0) {
            CB->ajoute_zone(4, 610, -2987, 240, 100, 1);
            CB->pos_perso(P_KALHI12, 618, -3125, 0, 1, "P12ARR03", -1, 0);
            ST(0x74)  = 1;
            ST(0x68)  = 1;
            ST(V_EVENT_0) = 2;
            CB->evenement(0, -1);
            CB->bloque(BLOQUE_ALL);
            CB->change_camera(6, CAM_LOCKED);
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->ajoute_zone(4, 610, -2987, 240, 100, 1);
            ST(V_EVENT_0) = 2;
            CB->pos_perso(P_KALHI12, 618, -3125, 0, 1, "P12ARR01", -1, 0);
            ST(0x88) = 0x29A;
        }
    }

    if (ST(V_INTRO_STATE) != 0 && ST(V_LEFT_HOME) == 1 && ST(V_GUARD_FIGHT_DONE) == 0 &&
        ST(0xAF8) == 0 && ST(ST_LEVEL) == 2) {
        CB->pos_perso(ST(ST_HERO), 511, -2196, 0x80E8, 1, "ar", 6, 0);
        CB->ajoute_zone(4, 610, -2987, 240, 100, 0);
        if ((i32)ST(ST_VIE(P_KABAK)) < 2) {
            CB->pos_perso(P_KABAK, 112, -2506, 0x5DC0, 1, "P87ATT04", 9, 1);
        }
        if (ST(ST_VIE(P_KABAK)) == VIE_DEAD) {
            i32 c0 = CB->divers(DV_DESC_POS, P_KABAK, 2);
            i32 c1 = CB->divers(DV_DESC_POS, P_KABAK, 1);
            i32 c2 = CB->divers(DV_DESC_POS, P_KABAK, 0);
            CB->pos_perso(P_KABAK, c2, c1, c0, 2, "P87cad", 9, 0);
        }
    }

    if (ST(V_INTRO_STATE) != 0 && ST(V_LEFT_HOME) == 1 && ST(V_GUARD_FIGHT_DONE) == 0 &&
        ST(0xAF8) == 1 && ST(ST_LEVEL) == 2) {
        CB->pos_perso(ST(ST_HERO), 511, -2196, 0x80E8, 1, "ar", 6, 0);
        CB->ajoute_zone(4, 610, -2987, 240, 100, 0);
        if ((i32)ST(ST_VIE(P_KABAK)) < 2) {
            CB->pos_perso(P_KABAK, 112, -2506, 0x5DC0, 1, "P87ATT04", 9, 1);
        }
        if (ST(ST_VIE(P_KABAK)) == VIE_DEAD) {
            i32 c0 = CB->divers(DV_DESC_POS, P_KABAK, 2);
            i32 c1 = CB->divers(DV_DESC_POS, P_KABAK, 1);
            i32 c2 = CB->divers(DV_DESC_POS, P_KABAK, 0);
            CB->pos_perso(P_KABAK, c2, c1, c0, 2, "P87cad", 9, 0);
        }
    }

    if (ST(V_INTRO_STATE) != 0 && ST(V_LEFT_HOME) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(ST_LEVEL) == 2) {
        CB->pos_perso(ST(ST_HERO), 511, -2196, 0x80E8, 1, "ar", 6, 0);
        if ((i32)ST(ST_VIE(P_KABAK)) < 2) {
            CB->pos_perso(P_KABAK, 320, -2530, 0xDD4, 1, "P87ATT01", 9, 1);
            CB->link_objet(0x57, "OB94", 0x1B, 1);
            ST(0xF0) = 1;
        }
        if (ST(ST_VIE(P_KABAK)) == VIE_DEAD) {
            i32 c0 = CB->divers(DV_DESC_POS, P_KABAK, 2);
            i32 c1 = CB->divers(DV_DESC_POS, P_KABAK, 1);
            i32 c2 = CB->divers(DV_DESC_POS, P_KABAK, 0);
            CB->pos_perso(P_KABAK, c2, c1, c0, 2, "P87cad", 9, 0);
        }
        if (ST(V_GUARD_WAVE) == 1) {
            if (ST(V_G23_FIGHT_ON) == 0) {
                if ((i32)ST(ST_VIE(P_GUARDIAN23)) < 2) {
                    CB->pos_perso(P_GUARDIAN23, 578, -3010, 0x1388, 1, "P88ARR02", 0x11, 1);
                }
                if (ST(ST_VIE(P_GUARDIAN23)) == VIE_DEAD) {
                    i32 c0 = CB->divers(DV_DESC_POS, P_GUARDIAN23, 2);
                    i32 c1 = CB->divers(DV_DESC_POS, P_GUARDIAN23, 1);
                    i32 c2 = CB->divers(DV_DESC_POS, P_GUARDIAN23, 0);
                    CB->pos_perso(P_GUARDIAN23, c2, c1, c0, 2, "P88cad", 0x11, 0);
                }
            }
            if (ST(V_G23_FIGHT_ON) == 1) {
                if ((i32)ST(ST_VIE(P_GUARDIAN23)) < 2) {
                    CB->pos_perso(P_GUARDIAN23, 592, -2809, 0xFB1D, 1, "P88GAR01", 0x11, 1);
                }
                if (ST(ST_VIE(P_GUARDIAN23)) == VIE_DEAD) {
                    i32 c0 = CB->divers(DV_DESC_POS, P_GUARDIAN23, 2);
                    i32 c1 = CB->divers(DV_DESC_POS, P_GUARDIAN23, 1);
                    i32 c2 = CB->divers(DV_DESC_POS, P_GUARDIAN23, 0);
                    CB->pos_perso(P_GUARDIAN23, c2, c1, c0, 2, "P88cad", 0x11, 0);
                }
            }
        }
        if (ST(V_GUARD_WAVE) == 2) {
            ST(V_G23_FIGHT_ON) = 0;
            if ((i32)ST(ST_VIE(P_GUARDIAN24)) < 2) {
                CB->pos_perso(P_GUARDIAN24, 638, -3062, 0x3A98, 1, "P89ARR01", 0x12, 1);
            }
            if (ST(ST_VIE(P_GUARDIAN24)) == VIE_DEAD) {
                i32 c0 = CB->divers(DV_DESC_POS, P_GUARDIAN24, 2);
                i32 c1 = CB->divers(DV_DESC_POS, P_GUARDIAN24, 1);
                i32 c2 = CB->divers(DV_DESC_POS, P_GUARDIAN24, 0);
                CB->pos_perso(P_GUARDIAN24, c2, c1, c0, 2, "P89cad", 0x12, 0);
            }
        }
    }

    if (ST(V_KABAK_DOOR_ZONE) == 1 || (ST(V_KABAK_DOOR_ZONE) == 0 && ST(V_GUARD_WAVE) != 0)) {
        CB->etat_objet("OB95", 315, 75, -2465, 0x4000, 0xEA00, 0, -1, 2);
    }

    if (ST(V_KABAK_DOOR_ZONE) == 1 || (i32)ST(ST_VIE(P_KABAK)) >= 2) {
        CB->ajoute_zone(16, 320, -2543, 90, 90, 0);
    }

    CB->divers(DV_MUSIC, 1, 0);
    CB->divers(DV_EVENT_SOUND, 0x10001, 0x23280000);

    ST(0x964) = 0;
}

static void trig01(void) {   /* sub_1000B563 */
    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;

    if (ST(ST_HERO) != 4
        && CB->direction_spec(0x7530)
        && (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) > 0x61A8) {

        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        l01_spawn_ar();

        if (ST(ST_SPECIAL_MODE) == 1) {
            if (ST(ST_HERO) == P_ARKHAN0) {
                CB->animation("SP00ACT26");
                if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21A", P_ARKHAN1);
                if (ST(V_ZED_BRIEFING) == 1)     CB->dialogue("L01D29A", P_KALHI12);
                if (ST(0x68) == 1)      CB->modify_var(&ST(V_EVENT_1), 1);
            }
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT26");
                if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21A", P_ARKHAN1);
                if (ST(V_ZED_BRIEFING) == 1)     CB->dialogue("L01D29B", P_KALHI12);
                if (ST(0x68) == 1)      CB->modify_var(&ST(V_EVENT_1), 1);
            }
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT26");
                if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21B", P_ARKHAN2);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT26");
                if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21C", P_ARKHAN3);
            }
            ST(0x6C)++;
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }

        if (ST(ST_SPECIAL_MODE) == 0) {
            if (ST(ST_HERO) == P_ARKHAN0) {
                CB->animation("SP00ACT27");
                if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21D", P_ARKHAN1);
                if (ST(V_ZED_BRIEFING) == 1)     CB->dialogue("L01D29C", P_KALHI12);
                if (ST(0x68) == 1)      CB->modify_var(&ST(V_EVENT_1), 1);
            }
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT27");
                if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21D", P_ARKHAN1);
                if (ST(V_ZED_BRIEFING) == 1)     CB->dialogue("L01D29C", P_KALHI12);
                if (ST(0x68) == 1)      CB->modify_var(&ST(V_EVENT_1), 1);
            }
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT27");
                if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21B", P_ARKHAN2);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT27");
                if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21C", P_ARKHAN3);
            }
            ST(0x6C)++;
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* ST(ST_SPECIAL_MODE) neither 0 nor 1: falls through to tail (loc_1000B9D5) */
    }

    /* tail: loc_1000B9D5 */
    if (ST(ST_HERO) == 4) return;
    if (!CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x1E)) return;
    if (!CB->direction_zone(0x1E)) return;
    if ((i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) >= 0x4268) return;
    trig30();
}

static void trig02(void) {   /* sub_1000BA47 */
    if (ST(V_GATE_OB25_OPEN) == 1 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC738)) {
        CB->animation("SG00ATT01");
        if ((i32)ST(ST_HERO) <= 1)
            CB->dialogue("LCOD78", P_ARKHAN1);
        if (ST(ST_HERO) == P_ARKHAN2)
            CB->dialogue("LCOD76", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3)
            CB->dialogue("LCOD08", P_ARKHAN3);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (ST(ST_HERO) != P_ARKHAN0) return;
    if (!CB->direction_perso(-25)) return;

    if (ST(V_EVENT_0) == 0) {
        CB->evenement(0, -1);
        CB->evenement(0, 0x3A98);
    }
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    CB->vire_col(6, 0);
    if (ST(ST_SPECIAL_MODE) == 1)
        CB->dialogue("L01D119", 0);
    else
        CB->dialogue("L01D120", 0);

    if (ST(ST_HERO) != 4) {
        if ((i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 0x3D9 &&
            (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 1) < -2650) {
            CB->joindre(-ST(ST_HERO), 985, -2657, 0xE678, -1);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 1);
        }
    }

    CB->animation("SP00PRE00");
    CB->etat_objet("OB25", 1000, 150, -2620, 0, 0xC000, 0, -2, 0);
    CB->joindre(ST(ST_HERO), 970, -2466, -1, -1);
    CB->change_camera(5, CAM_LOCKED);
    CB->animation("Sar_1000");
    CB->bruitage("L01B07", 970, -2466);
    CB->pos_perso(P_ARKHAN1, 970, -2466, 0x7274, 1, "ar", 6, 0);
    CB->animation("Sar_2000");
    ST(V_GATE_OB25_OPEN) = 1;
    CB->modify_var(&ST(ST_HERO), 1);
    CB->change_camera(3, CAM_AUTO);
    CB->change_camera(3, CAM_AUTO);
    CB->joindre(ST(ST_HERO), 983, -2570, 0x7274, -1);
    CB->animation("ar_200");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

static void trig03(void) {   /* sub_1000BDC7 */
    if (ST(ST_ACTION_KEY) != 1) return;                 /* cmp [eax+0Ch],1 ; jnz end   */
    if (ST(ST_SCRIPT_BUSY) != 0) return;                /* cmp [eax+30h],0 ; jnz end   */
    if (CB->direction_spec(0xFF29) == 0) return;       /* ecx=0FF29h ; C998 ; jz end  */

    CB->select_pile(0, 2);                        /* edx=2; ecx=0 ; C464         */
    CB->bloque(BLOQUE_ALL);                                  /* ecx=3 ; C938                */
    ST(ST_SCRIPT_BUSY) = 1;

    if (ST(ST_HERO) == P_ARKHAN0 && ST(V_FRONT_DOOR_STAGE) == 0) {
        ST(V_EVENT_0) = 2;
        ST(V_FRONT_DOOR_STAGE) = 1;
        CB->animation("SP12ARR03_10");          /* C460 */
        CB->animation("SG00OUV01");
        CB->animation("P00_65535");
        CB->anim_objet("IM01", 3, -60, 1500, 0);
        CB->change_camera(6, CAM_LOCKED);                          /* edx=2; ecx=6 ; C458         */
        l01_action_register("L01D32", 0xC, "SP12DIA59", "SP12ARR03_10");
        ST(ST_LEVEL) = 1;
        CB->divers_empile(DV_MUSIC, 1, 4);                     /* push4; edx=1; ecx=18h; C45C */
        CB->change_piece(2, 0);                          /* edx=0; ecx=2 ; C984         */
        return;
    }

    if (ST(ST_HERO) == P_ARKHAN0 && ST(V_FRONT_DOOR_STAGE) == 1) {    /* loc_BEF7 */
        l01_action_register("L01D33", 0, "SG00OUV00", NULL);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);                  /* ecx=state+30h; edx=0 ; C46C */
        CB->bloque(BLOQUE_NONE);                              /* ecx=0 ; C938                */
        CB->select_pile(0, 1);                    /* edx=1; ecx=0 ; C464         */
        return;
    }

    /* loc_BF47 */
    if (ST(ST_HERO) == P_ARKHAN1
        && (i32)ST(V_ZED_BRIEFING) < 2                  /* cmp [0BA0h],2 ; jge C105    */
        && ST(V_KALHI12_D34_DONE) != 0x29A) {               /* cmp [0ACCh],29Ah ; jz C105  */
        ST(V_EVENT_0) = 2;
        CB->evenement(0, -1);                 /* edx=-1; ecx=0 ; C674        */
        ST(0x68) = 1;
        CB->select_pile(1, 2);                    /* edx=2; ecx=1 ; C464         */
        CB->select_pile(0, 1);                    /* edx=1; ecx=0 ; C464         */
        CB->animation("SP12ARR03_100");
        ST(0x88) = -1;
        ST(V_KALHI12_D34_DONE) = 0x29A;

        if (ST(ST_SPECIAL_MODE) == 1) {                  /* cmp [198h],1 ; jnz C06F     */
            CB->animation("SG01OUV02");
            CB->dialogue("L01D34A", P_ARKHAN1);    /* C470 */
            CB->dialogue("L01D34B", P_ARKHAN1);
            CB->change_camera(6, CAM_LOCKED);
            l01_action_register("L01D36", 0xC, "SP12DIA56", "SP12ARR03_10");
            CB->change_camera(4, CAM_AUTO);                      /* edx=1; ecx=4 ; C458         */
            CB->animation("SG01OUV03");
        }
        if (ST(ST_SPECIAL_MODE) == 0) {                  /* loc_C06F: cmp [198h],0 ; jnz C100 */
            CB->animation("SG01OUV02");
            CB->dialogue("L01D35A", P_ARKHAN1);
            CB->dialogue("L01D35B", P_ARKHAN1);
            CB->dialogue("L01D35C", P_ARKHAN1);
            CB->change_camera(6, CAM_LOCKED);
            l01_action_register("L01D37", 0xC, "SP12DIA59", "SP12ARR03_10");
            CB->change_camera(4, CAM_AUTO);
            CB->animation("G01OUV03");
        }
    } else {
        CB->animation("SG00OUV01");             /* loc_C105 */
    }

    /* loc_C110 (common tail) */
    CB->animation("P00_65535");
    CB->anim_objet("IM01", 3, -60, 1500, 0);
    CB->bloque(BLOQUE_NONE);                                  /* ecx=0 ; C938                */
    ST(ST_LEVEL) = 1;
    CB->divers_empile(DV_MUSIC, 1, 4);                         /* push4; edx=1; ecx=18h; C45C */
    CB->change_piece(2, 0);                              /* edx=0; ecx=2 ; C984         */
}

static void trig04(void) {                                  /* sub_1000C171 */
    /* --- A --- */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_zone(0x1D) &&
        CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x1D)) {
        trig29();
        return;
    }

    /* --- B (loc_1000C1C3) --- */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 &&
        ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KALHI12)) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        ST(V_EVENT_0) = 2;
        ST(V_KALHI12_TALK_NIGHT) = 1;
        ST(0x98)  = 1;
        if (ST(ST_LOCKED_CAMERA) != 6)
            CB->change_camera(6, CAM_LOCKED);
        CB->evenement(0, -1);
        if (ST(0x68) == 0) {
            ST(0x68) = 1;
            CB->animation("P12ARR01_100");
            l01_action_register("L01D41", 0x0C, "SP12ATT02", "SP12LEV01");
            CB->animation("P12ARR03_10");
            CB->divers_empile(DV_LOOK_AT_PLAYER, P_KALHI12, 1);
        } else {
            CB->animation("P12ARR03_100");
            l01_action_register("L01D41", 0x0C, "SP12ATT05", "SP12ARR03_10");
        }
        CB->modify_var(&ST(0x88), 0x29A);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    /* --- C (loc_1000C356): the ST(V_LEFT_HOME)==0 world --- */
    if (ST(V_LEFT_HOME) == 0) {

        /* first-run intro / dialogue tree */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x8660)) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(0x98) = 1;
            CB->change_camera(-7, CAM_LOCKED);

            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->joindre(-ST(ST_HERO), 0, 0, 0, 0x0C);

                /* loc_1000C410 */
                if (ST(V_KALHI12_TALK_NIGHT) == 0 && ST(V_EVENT_0) == 0) {
                    ST(V_EVENT_0) = 1;
                    ST(V_KALHI12_TALK_NIGHT) = 1;
                    CB->evenement(0, -1);
                    l01_action_register("L01D02", 0, "SG00DIA05", NULL);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    return;
                }

                /* loc_1000C488 */
                if ((i32)ST(V_KALHI12_TALK_NIGHT) > 0 && ST(0x68) == 0) {
                    ST(0x68) = 1;
                    CB->select_pile(1, 1);
                    CB->animation("SP12LEV01");
                    CB->animation("P12ARR03_10");
                    CB->modify_var(&ST(V_EVENT_1), 1);
                    CB->select_pile(0, 1);
                }

                /* loc_1000C4FF */
                if (ST(V_ZED_BRIEFING) == 0) {
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        if (ST(V_KALHI12_TALK_NIGHT) == 1) {
                            l01_action_register("L01D04A", 0, "SG00DIA07", NULL);
                            l01_action_register("L01D05A", 0x0C, "P12DIA50", "SP12ARR03_10");
                            l01_action_register("L01D04B", 0, "SG00DIA06", NULL);
                            l01_action_register("L01D05B", 0x0C, "SP12DIA56", "SP12ARR03_10");
                            CB->dialogue("L01D05C", P_KALHI12);
                            CB->dialogue("L01D05D", P_KALHI12);
                            ST(V_KALHI12_TALK_NIGHT) = 2;
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        if (ST(V_KALHI12_TALK_NIGHT) == 2) {
                            ST(V_KALHI12_TALK_NIGHT) = 3;
                            l01_action_register("L01D06A", 0, "SG00DIA06", NULL);
                            l01_action_register("L01D06B", 0, "SG00DIA02", NULL);
                            CB->dialogue("L01D07", P_KALHI12);
                            l01_action_register("L01D08A", 0, "SG00DIA01", NULL);
                            l01_action_register("L01D08B", 0, "SG00DIA07", NULL);
                            l01_action_register("L01D09A", 0x0C, "SP12DIA59", "SP12ARR03_10");
                            CB->dialogue("L01D09Z", P_KALHI12);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        if ((i32)ST(V_KALHI12_TALK_NIGHT) >= 3) {
                            ST(V_KALHI12_TALK_NIGHT) = 4;
                            l01_action_register("L01D10", 0, "SG00DIA03", NULL);
                            l01_action_register("L01D11", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }

                    /* loc_1000C709 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        if (ST(V_KALHI12_TALK_NIGHT) == 1) {
                            l01_action_register("L01D13A", 0, "SG00DIA03", NULL);
                            l01_action_register("L01D14", 0x0C, "P12DIA59", "SP12ARR03_10");
                            l01_action_register("L01D13B", 0, "SG00DIA07", NULL);
                            ST(V_KALHI12_TALK_NIGHT) = 2;
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        if (ST(V_KALHI12_TALK_NIGHT) == 2) {
                            l01_action_register("L01D15", 0, "SG00DIA01", "sar_1000");
                            l01_action_register("L01D16", 0, "SG00DIA06", NULL);
                            l01_action_register("L01D09A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            l01_action_register("L01D09B", 0x0C, "SP12DIA59", "SP12ARR03_10");
                            ST(V_KALHI12_TALK_NIGHT) = 3;
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        if ((i32)ST(V_KALHI12_TALK_NIGHT) >= 3) {
                            l01_action_register("L01D18", 1, "SG01DIA05", "Sar_10");
                            l01_action_register("L01D19", 0x0C, "SP12DIA59", "SP12ARR03_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                }

                /* loc_1000C895 */
                if (ST(V_ZED_BRIEFING) == 1) {
                    if (ST(V_KALHI12_TALK_BRIEF) == 0) {
                        ST(V_KALHI12_TALK_BRIEF) = 1;
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            l01_action_register("L01D46A", 0, "SG00DIA04", NULL);
                            l01_action_register("L01D20A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            l01_action_register("L01D46B", 0, "SG00DIA03", "sar_750");
                            l01_action_register("L01D46C", 0, "G00DIA05", NULL);
                            l01_action_register("L01D20B", 0x0C, "SP12DIA59", "SP12ARR03_10");
                        }
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            l01_action_register("L01D47", 0, "SG00DIA03", NULL);
                            l01_action_register("L01D26A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            CB->dialogue("L01D26B", P_KALHI12);
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    if (ST(V_KALHI12_TALK_BRIEF) == 1) {
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            l01_action_register("L01D88A", 0, "SG00DIA02", NULL);
                            CB->dialogue("L01D89A", P_KALHI12);
                        }
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            l01_action_register("L01D88B", 0, "SG00DIA09", NULL);
                            l01_action_register("L01D89B", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }

                /* loc_1000CA57 */
                if ((i32)ST(V_ZED_BRIEFING) >= 2) {
                    if (ST(V_KALHI12_TALK_POST) == 0) {
                        ST(V_KALHI12_TALK_POST) = 1;
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            l01_action_register("L01D24A", 1, "SG00DIA07", NULL);
                            l01_action_register("L01D25A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        }
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            l01_action_register("L01D83", 1, "SG00DIA07", NULL);
                            l01_action_register("L01D84A", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        }
                        CB->dialogue("L01D25B", P_KALHI12);
                        l01_action_register("L01D24B", 1, "SG00DIA06", NULL);
                        l01_action_register("L01D25C", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    if (ST(V_KALHI12_TALK_POST) == 1) {
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            l01_action_register("L01D27A", 1, "SG00DIA04", NULL);
                            l01_action_register("L01D28", 0x0C, "SP12DIA50", "SP12ARR03_10");
                        }
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            l01_action_register("L01D85A", 1, "SG00DIA02", NULL);
                            l01_action_register("L01D28", 0x0C, "SP12DIA50", "SP12ARR03_10");
                            l01_action_register("L01D85B", 1, "SG00DIA06", NULL);
                        }
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
            }
            /* fall through to loc_1000CC31 */
        }

        /* loc_1000CC31 : the ST(V_PLAYER_STRIKE)==1 branch */
        if (ST(V_PLAYER_STRIKE) == 1 && CB->direction_spec(0x8660)) {
            ST(V_PLAYER_STRIKE)  = -1;
            ST(V_EVENT_0) = 2;
            ST(0x98)  = 1;
            if (ST(ST_LOCKED_CAMERA) != 6)
                CB->change_camera(6, CAM_LOCKED);
            CB->evenement(0, -1);
            if (ST(V_KALHI12_TALK_NIGHT) == 0)
                ST(V_KALHI12_TALK_NIGHT) = 1;
            CB->bloque(BLOQUE_ALL);

            if ((i32)ST(ST_HERO) <= 1) {
                if (ST(0x714) == 1) {
                    /* loc_1000CDA9 */
                    if (CB->divers(DV_CAN_FIGHT, 0, 0) == 0) {
                        if (ST(0x68) == 1)
                            l01_action_register("L01D42", 0x0C, "P12ATT05", "SP12ARR03_10");
                        CB->animation("Sar_10");
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    if (CB->divers(DV_CAN_FIGHT, 0, 0) != 0) {
                        CB->divers(DV_LOOK_AT_PLAYER, P_KALHI12, 0);
                        ST(0x68)  = 0;
                        ST(V_EVENT_0) = 0x3E8;
                        ST(0x88)  = 0x3E8;
                        ST(V_EVENT_1) = 0;
                        if (ST(0x68) == 0)
                            l01_action_register("L01D43A", 0x0C, "SP12ATT02", "SP12ATT03_100");
                        l01_action_register("L01D98", 0, "SP00ATA08", NULL);
                        CB->dialogue("L01D43B", P_KALHI12);
                        CB->modify_var(&ST(0x78), 1);
                        CB->select_pile(1, 1);
                        CB->animation("P12ATT03_1000");
                        CB->select_pile(0, 1);
                        return;
                    }
                    /* fall through to loc_1000CF00 */
                } else {
                    /* loc_1000CCFC */
                    if (ST(0x68) == 0) {
                        ST(0x68) = 1;
                        l01_action_register("L01D41", 0x0C, "SP12ATT02", "SP12LEV01");
                        CB->animation("P12ARR03_10");
                        CB->divers_empile(DV_LOOK_AT_PLAYER, P_KALHI12, 1);
                    } else {
                        l01_action_register("L01D41", 0x0C, "SP12ATT05", "SP12ARR03_10");
                    }
                    CB->modify_var(&ST(0x88), 0x29A);
                    CB->animation("Sar_10");
                    ST(0x714) = 1;
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }

            /* loc_1000CF00 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->bruitage("L01B11", 576, -2992);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->bruitage("L01B11", 576, -2992);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* fall through to loc_1000CF8A */
        }
    }

    /* --- loc_1000CF8A : the ST(V_LEFT_HOME)!=0 "actor spawn" branch --- */
    if (ST(V_LEFT_HOME) != 0 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 &&
        CB->direction_spec(0x8660)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);

        if (ST(ST_SPECIAL_MODE) == 1) {
            if (ST(ST_HERO) == P_ARKHAN0) {
                CB->animation("SP00ACT26");
                if ((i32)ST(0x6C) <= 2)
                    CB->dialogue("L01D21A", P_ARKHAN1);
            }
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT26");
                if ((i32)ST(0x6C) <= 2)
                    CB->dialogue("L01D21A", P_ARKHAN1);
            }
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT26");
                if ((i32)ST(0x6C) <= 2)
                    CB->dialogue("L01D21B", P_ARKHAN2);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT26");
                if ((i32)ST(0x6C) <= 2)
                    CB->dialogue("L01D21C", P_ARKHAN2);
            }
            ST(0x6C)++;
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }

        if (ST(ST_SPECIAL_MODE) == 0) {
            if (ST(ST_HERO) == P_ARKHAN0) {
                CB->animation("SP00ACT27");
                if ((i32)ST(0x6C) <= 2)
                    CB->dialogue("L01D21D", P_ARKHAN1);
            }
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT27");
                if ((i32)ST(0x6C) <= 2)
                    CB->dialogue("L01D21D", P_ARKHAN1);
            }
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT27");
                if ((i32)ST(0x6C) <= 2)
                    CB->dialogue("L01D21B", P_ARKHAN2);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT27");
                if ((i32)ST(0x6C) <= 2)
                    CB->dialogue("L01D21C", P_ARKHAN2);
            }
            ST(0x6C)++;
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

static void trig05(void) {   /* sub_1000D24F */
    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (!CB->direction_zone(0x13)) return;

    ST(V_OB38A_STATE) = 2;
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    CB->retire_zone(5);
    CB->retire_zone(19);
    CB->animation("SG00PRE07");
    CB->etat_objet("OB38A", -1, -1, -1, -1, -1, -1, -3, 0);
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

static void trig07(void) {   /* sub_1000D319 */
    if (CB->direction_spec(0xAFC8) == 0) goto L_E241;

    if (ST(V_LEFT_HOME) != 0) goto L_DEB6;
    if (ST(ST_ACTION_KEY) != 1) goto L_DBFA;
    if (ST(ST_SCRIPT_BUSY) != 0) goto L_DBFA;
    ST(ST_SCRIPT_BUSY) = 1;
    CB->bloque(BLOQUE_ALL);
    ST(0x98) = 1;
    CB->change_camera(-7, CAM_LOCKED);                          /* ecx=0xFFFFFFF9, edx=2 */

    if (ST(ST_HERO) != P_ARKHAN0) {
        if (ST(ST_HERO) != P_ARKHAN1) goto L_DBFA;
    }
    /* D3BC */
    CB->joindre(-ST(ST_HERO), 0, 0, 0, 0xC);    /* ecx=-[194], edx=0, stack(last->first)=0,0,0xC */

    if (ST(V_KALHI12_TALK_NIGHT) != 0) goto L_D451;
    if (ST(V_EVENT_0) != 0) goto L_D451;
    ST(V_EVENT_0) = 1;
    ST(V_KALHI12_TALK_NIGHT) = 1;
    CB->evenement(0, -1);                  /* ecx=0, edx=-1 */
    l01_action_register("L01D02", 0, "SG00DIA05", NULL);
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    goto L_E631;

L_D451:
    if ((i32)ST(V_KALHI12_TALK_NIGHT) <= 0) goto L_D4C8;
    if (ST(0x68) != 0) goto L_D4C8;
    ST(0x68) = 1;
    CB->select_pile(1, 1);
    CB->animation("SP12LEV01");
    CB->animation("P12ARR03_10");
    CB->modify_var(&ST(V_EVENT_1), 1);
    CB->select_pile(0, 1);

L_D4C8:
    if (ST(V_ZED_BRIEFING) != 0) goto L_D85E;
    if (ST(ST_SPECIAL_MODE) != 1) goto L_D6D2;
    if (ST(V_KALHI12_TALK_NIGHT) != 1) goto L_D5A3;
    l01_action_register("L01D04A", 0, "SG00DIA07", NULL);
    l01_action_register("L01D05A", 0xC, "P12DIA50", "SP12ARR03_10");
    l01_action_register("L01D04B", 0, "SG00DIA06", NULL);
    l01_action_register("L01D05B", 0xC, "SP12DIA56", "SP12ARR03_10");
    CB->dialogue("L01D05C", P_KALHI12);
    CB->dialogue("L01D05D", P_KALHI12);
    ST(V_KALHI12_TALK_NIGHT) = 2;
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_D5A3:
    if (ST(V_KALHI12_TALK_NIGHT) != 2) goto L_D667;
    ST(V_KALHI12_TALK_NIGHT) = 3;
    l01_action_register("L01D06A", 0, "SG00DIA06", NULL);
    l01_action_register("L01D06B", 0, "SG00DIA02", NULL);
    CB->dialogue("L01D07", P_KALHI12);
    l01_action_register("L01D08A", 0, "SG00DIA01", NULL);
    l01_action_register("L01D08B", 0, "SG00DIA07", NULL);
    l01_action_register("L01D09A", 0xC, "SP12DIA59", "SP12ARR03_10");
    CB->dialogue("L01D09Z", P_KALHI12);
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_D667:
    if ((i32)ST(V_KALHI12_TALK_NIGHT) < 3) goto L_D6D2;
    ST(V_KALHI12_TALK_NIGHT) = 4;
    l01_action_register("L01D10", 0, "SG00DIA03", NULL);
    l01_action_register("L01D11", 0xC, "SP12DIA50", "SP12ARR03_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_D6D2:
    if (ST(ST_SPECIAL_MODE) != 0) goto L_D85E;
    if (ST(V_KALHI12_TALK_NIGHT) != 1) goto L_D762;
    l01_action_register("L01D13A", 0, "SG00DIA03", NULL);
    l01_action_register("L01D14", 0xC, "P12DIA59", "SP12ARR03_10");
    l01_action_register("L01D13B", 0, "SG00DIA07", NULL);
    ST(V_KALHI12_TALK_NIGHT) = 2;
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_D762:
    if (ST(V_KALHI12_TALK_NIGHT) != 2) goto L_D7FC;
    l01_action_register("L01D15", 0, "SG00DIA01", "sar_1000");
    l01_action_register("L01D16", 0, "SG00DIA06", NULL);
    l01_action_register("L01D09A", 0xC, "SP12DIA50", "SP12ARR03_10");
    l01_action_register("L01D09B", 0xC, "SP12DIA59", "SP12ARR03_10");
    ST(V_KALHI12_TALK_NIGHT) = 3;
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_D7FC:
    if ((i32)ST(V_KALHI12_TALK_NIGHT) < 3) goto L_D85E;
    l01_action_register("L01D18", 1, "SG01DIA05", "Sar_10");
    l01_action_register("L01D19", 0xC, "SP12DIA59", "SP12ARR03_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_D85E:
    if (ST(V_ZED_BRIEFING) != 1) goto L_DA20;
    if (ST(V_KALHI12_TALK_BRIEF) != 0) goto L_D97D;
    ST(V_KALHI12_TALK_BRIEF) = 1;
    if (ST(ST_SPECIAL_MODE) != 1) goto L_D911;
    l01_action_register("L01D46A", 0, "SG00DIA04", NULL);
    l01_action_register("L01D20A", 0xC, "SP12DIA50", "SP12ARR03_10");
    l01_action_register("L01D46B", 0, "SG00DIA03", "sar_750");
    l01_action_register("L01D46C", 0, "G00DIA05", NULL);
    l01_action_register("L01D20B", 0xC, "SP12DIA59", "SP12ARR03_10");

L_D911:
    if (ST(ST_SPECIAL_MODE) != 0) goto L_D95F;
    l01_action_register("L01D47", 0, "SG00DIA03", NULL);
    l01_action_register("L01D26A", 0xC, "SP12DIA50", "SP12ARR03_10");
    CB->dialogue("L01D26B", P_KALHI12);

L_D95F:
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_D97D:
    if (ST(V_KALHI12_TALK_BRIEF) != 1) goto L_DA20;
    if (ST(ST_SPECIAL_MODE) != 1) goto L_D9C4;
    l01_action_register("L01D88A", 0, "SG00DIA02", NULL);
    CB->dialogue("L01D89A", P_KALHI12);

L_D9C4:
    if (ST(ST_SPECIAL_MODE) != 0) goto L_DA02;
    l01_action_register("L01D88B", 0, "SG00DIA09", NULL);
    l01_action_register("L01D89B", 0xC, "SP12DIA50", "SP12ARR03_10");

L_DA02:
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_DA20:
    if ((i32)ST(V_ZED_BRIEFING) < 2) goto L_DBFA;
    if (ST(V_KALHI12_TALK_POST) != 0) goto L_DB32;
    ST(V_KALHI12_TALK_POST) = 1;
    if (ST(ST_SPECIAL_MODE) != 1) goto L_DA94;
    l01_action_register("L01D24A", 1, "SG00DIA07", NULL);
    l01_action_register("L01D25A", 0xC, "SP12DIA50", "SP12ARR03_10");

L_DA94:
    if (ST(ST_SPECIAL_MODE) != 0) goto L_DAD5;
    l01_action_register("L01D83", 1, "SG00DIA07", NULL);
    l01_action_register("L01D84A", 0xC, "SP12DIA50", "SP12ARR03_10");

L_DAD5:
    CB->dialogue("L01D25B", P_KALHI12);
    l01_action_register("L01D24B", 1, "SG00DIA06", NULL);
    l01_action_register("L01D25C", 0xC, "SP12DIA50", "SP12ARR03_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_DB32:
    if (ST(V_KALHI12_TALK_POST) != 1) goto L_DBFA;
    if (ST(ST_SPECIAL_MODE) != 1) goto L_DB85;
    l01_action_register("L01D27A", 1, "SG00DIA04", NULL);
    l01_action_register("L01D28", 0xC, "SP12DIA50", "SP12ARR03_10");

L_DB85:
    if (ST(ST_SPECIAL_MODE) != 0) goto L_DBDC;
    l01_action_register("L01D85A", 1, "SG00DIA02", NULL);
    l01_action_register("L01D28", 0xC, "SP12DIA50", "SP12ARR03_10");
    l01_action_register("L01D85B", 1, "SG00DIA06", NULL);

L_DBDC:
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_DBFA:
    if (ST(V_PLAYER_STRIKE) != 1) goto L_DEB6;
    ST(V_PLAYER_STRIKE) = -1;
    ST(V_EVENT_0) = 2;
    ST(0x98) = 1;
    if (ST(ST_LOCKED_CAMERA) == 6) goto L_DC55;
    CB->change_camera(6, CAM_LOCKED);
L_DC55:
    CB->evenement(0, -1);
    if (ST(V_KALHI12_TALK_NIGHT) != 0) goto L_DC83;
    ST(V_KALHI12_TALK_NIGHT) = 1;
L_DC83:
    CB->bloque(BLOQUE_ALL);
    if ((i32)ST(ST_HERO) > 1) goto L_DEB6;
    if (ST(0x714) == 1) goto L_DD5F;
    if (ST(0x68) != 0) goto L_DD08;
    ST(0x68) = 1;
    l01_action_register("L01D41", 0xC, "SP12ATT01", "SP12LEV01");
    CB->animation("P12ARR03_10");
    CB->divers_empile(DV_LOOK_AT_PLAYER, P_KALHI12, 1);
    goto L_DD21;
L_DD08:
    l01_action_register("L01D41", 0xC, "SP12ATT05", "SP12ARR03_10");
L_DD21:
    CB->modify_var(&ST(0x88), 0x29A);
    CB->animation("Sar_10");
    ST(0x714) = 1;
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_DD5F:
    if (ST(0x714) != 1) goto L_DEB6;
    if (CB->divers(DV_CAN_FIGHT, 0, 0) != 0) goto L_DDC8;
    if (ST(0x68) != 1) goto L_DDB0;
    l01_action_register("L01D42", 0xC, "SP12ATT05", "SP12ARR03_10");
L_DDB0:
    CB->animation("Sar_10");
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_DDC8:
    if (CB->divers(DV_CAN_FIGHT, 0, 0) == 0) goto L_DEB6;
    CB->divers(DV_LOOK_AT_PLAYER, P_KALHI12, 0);
    ST(0x68) = 0;
    ST(V_EVENT_0) = 0x3E8;
    ST(0x88) = 0x3E8;
    ST(V_EVENT_1) = 0;
    if (ST(0x68) != 0) goto L_DE52;
    l01_action_register("L01D43A", 0xC, "SP12ATT02", "SP12ATT03_100");
L_DE52:
    l01_action_register("L01D98", 0, "SP00ATA08", NULL);
    CB->dialogue("L01D43B", P_KALHI12);
    CB->modify_var(&ST(0x78), 1);
    CB->select_pile(1, 1);
    CB->animation("P12ATT03_1000");
    CB->select_pile(0, 1);
    goto L_E631;

L_DEB6:
    if (ST(V_LEFT_HOME) == 0) goto L_E197;
    if (CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN23, 0) != 0) goto L_E197;
    if (CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN24, 0) != 0) goto L_E197;
    if (ST(ST_ACTION_KEY) != 1) goto L_E197;
    if (ST(ST_SCRIPT_BUSY) != 0) goto L_E197;
    ST(ST_SCRIPT_BUSY) = 1;
    CB->bloque(BLOQUE_ALL);
    if (ST(ST_SPECIAL_MODE) != 1) goto L_E064;

    if (ST(ST_HERO) == P_ARKHAN0) {
        CB->animation("SP00ACT26");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21A", P_ARKHAN1);
    }
    /* DF7F */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->animation("SP01ACT26");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21A", P_ARKHAN1);
    }
    /* DFBB */
    if (ST(ST_HERO) == P_ARKHAN2) {
        CB->animation("SP02ACT26");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21B", P_ARKHAN2);
    }
    /* DFF7 */
    if (ST(ST_HERO) == P_ARKHAN3) {
        CB->animation("SP03ACT26");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21C", P_ARKHAN2);
    }
    /* E033 */
    ST(0x6C)++;
    CB->animation("ar_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_E064:
    if (ST(ST_SPECIAL_MODE) != 0) goto L_E197;
    if (ST(ST_HERO) == P_ARKHAN0) {
        CB->animation("SP00ACT27");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21D", P_ARKHAN1);
    }
    /* E0B2 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->animation("SP01ACT27");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21D", P_ARKHAN1);
    }
    /* E0EE */
    if (ST(ST_HERO) == P_ARKHAN2) {
        CB->animation("SP02ACT27");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21B", P_ARKHAN2);
    }
    /* E12A */
    if (ST(ST_HERO) == P_ARKHAN3) {
        CB->animation("SP03ACT27");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21C", P_ARKHAN2);
    }
    /* E166 */
    ST(0x6C)++;
    CB->animation("ar_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_E197:
    if (ST(ST_ACTION_KEY) != 1) goto L_E1EC;
    if (ST(ST_SCRIPT_BUSY) != 0) goto L_E1EC;
    if (CB->direction_perso(P_GUARDIAN23) == 0) goto L_E1EC;
    if (CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN23, 0) == 0) goto L_E1EC;
    trig17();
    goto L_E631;

L_E1EC:
    if (ST(ST_ACTION_KEY) != 1) goto L_E241;
    if (ST(ST_SCRIPT_BUSY) != 0) goto L_E241;
    if (CB->direction_perso(P_GUARDIAN24) == 0) goto L_E241;
    if (CB->divers(DV_PERSONA_PRESENT, P_GUARDIAN24, 0) == 0) goto L_E241;
    trig18();
    goto L_E631;

L_E241:
    if (CB->direction_spec(0x3A98) == 0) goto L_E631;
    if (ST(ST_ACTION_KEY) != 1) goto L_E631;
    if (ST(ST_SCRIPT_BUSY) != 0) goto L_E631;
    ST(ST_SCRIPT_BUSY) = 1;
    CB->bloque(BLOQUE_ALL);
    if (ST(ST_LOCKED_CAMERA) != 6) goto L_E2AB;
    CB->change_camera(CAM_ANY, CAM_AUTO);
L_E2AB:
    if (ST(ST_SPECIAL_MODE) != 1) goto L_E46E;

    if (ST(ST_HERO) == P_ARKHAN0) {
        CB->animation("SP00ACT26");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21A", P_ARKHAN1);
        if (ST(V_ZED_BRIEFING) == 1) CB->dialogue("L01D29A", P_KALHI12);
        if (ST(0x68) == 1) CB->modify_var(&ST(V_EVENT_1), 1);
    }
    /* E341 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->animation("SP01ACT26");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21A", P_ARKHAN1);
        if (ST(V_ZED_BRIEFING) == 1) CB->dialogue("L01D29B", P_KALHI12);
        if (ST(0x68) == 1) CB->modify_var(&ST(V_EVENT_1), 1);
    }
    /* E3C5 */
    if (ST(ST_HERO) == P_ARKHAN2) {
        CB->animation("SP02ACT26");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21B", P_ARKHAN2);
    }
    /* E401 */
    if (ST(ST_HERO) == P_ARKHAN3) {
        CB->animation("SP03ACT26");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21C", P_ARKHAN2);
    }
    /* E43D */
    ST(0x6C)++;
    CB->animation("ar_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_E46E:
    if (ST(ST_SPECIAL_MODE) != 0) goto L_E631;
    if (ST(ST_HERO) == P_ARKHAN0) {
        CB->animation("SP00ACT27");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21D", P_ARKHAN1);
        if (ST(V_ZED_BRIEFING) == 1) CB->dialogue("L01D29C", P_KALHI12);
        if (ST(0x68) == 1) CB->modify_var(&ST(V_EVENT_1), 1);
    }
    /* E504 */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->animation("SP01ACT27");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21D", P_ARKHAN1);
        if (ST(V_ZED_BRIEFING) == 1) CB->dialogue("L01D29C", P_KALHI12);
        if (ST(0x68) == 1) CB->modify_var(&ST(V_EVENT_1), 1);
    }
    /* E588 */
    if (ST(ST_HERO) == P_ARKHAN2) {
        CB->animation("SP02ACT27");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21B", P_ARKHAN2);
    }
    /* E5C4 */
    if (ST(ST_HERO) == P_ARKHAN3) {
        CB->animation("SP03ACT27");
        if ((i32)ST(0x6C) <= 2) CB->dialogue("L01D21C", P_ARKHAN2);
    }
    /* E600 */
    ST(0x6C)++;
    CB->animation("ar_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto L_E631;

L_E631:
    return;
}

static void trig08(void) {   /* sub_1000E636 */
    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (ST(ST_COMBAT_STANCE) != 0) return;
    if (!CB->direction_zone(0xC)) return;

    if (ST(ST_SPECIAL_MODE) == 1) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();
        if (ST(ST_HERO) == P_ARKHAN0) CB->animation("SP00ACT24");
        if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT24");
        if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ACT24");
        if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ACT24");
        if (ST(ST_HERO) == P_ARKHAN0) l01_action_register("L01D23A", 0, "G00DIA04", NULL);
        if (ST(ST_HERO) == P_ARKHAN1) l01_action_register("L01D23A", 1, "G00DIA04", NULL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("P00_65535");
            CB->animation("ar_10");
            CB->dialogue("L01D23B", P_ARKHAN2);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("P00_65535");
            CB->animation("ar_10");
            CB->dialogue("L01D23C", P_ARKHAN3);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    if (ST(ST_SPECIAL_MODE) != 0) return;
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    l01_spawn_ar();
    if (ST(ST_HERO) == P_ARKHAN0) CB->animation("SP00ACT25");
    if (ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT25");
    if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ACT25");
    if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ACT25");
    CB->animation("ar_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

static void trig09(void) {   /* sub_1000E991 */
    /* ---- block @0x1000E991 ---- */
    if (ST(ST_ACTION_KEY) != 1) goto L_EABD;
    if (ST(ST_SCRIPT_BUSY) != 0) goto L_EABD;
    if (ST(ST_COMBAT_STANCE) != 0) goto L_EABD;
    if (CB->direction_spec(0xBB80) == 0) goto L_EABD;
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x1C) == 0) goto L_EABD;
    if (CB->direction_perso(P_KABAK) != 0) goto L_EABD;
    CB->bloque(BLOQUE_ALL);
    if ((i32)ST(ST_HERO) <= 1)
        l01_action_register("LCOD52", 1, "SG00SAV01", "Sar_10");
    if (ST(ST_HERO) == P_ARKHAN2)
        l01_action_register("LCOD53", 2, "SG00SAV01", "Sar_10");
    if (ST(ST_HERO) == P_ARKHAN3)
        l01_action_register("LCOD54", 3, "SG00SAV01", "Sar_10");
    CB->animation("P00_65535");
    CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
    CB->bloque(BLOQUE_NONE);
    goto done;

L_EABD:
    if (ST(ST_ACTION_KEY) != 1) goto L_ECA1;
    if (ST(ST_VIE(P_KABAK)) != 1) goto L_ECA1;
    if (ST(V_ENGAGED_PERSONA) != 0) goto L_ECA1;
    if (ST(ST_SCRIPT_BUSY) != 0) goto L_ECA1;
    if (CB->direction_perso(P_KABAK) != 0) goto L_ECA1;
    if (CB->direction_spec(0x4E20) == 0) goto L_ECA1;
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 0xB) == 0) goto L_ECA1;
    ST(ST_SCRIPT_BUSY) = 1;
    CB->bloque(BLOQUE_ALL);
    if (ST(ST_SPECIAL_MODE) == 1) {
        if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT28");
        if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ACT28");
        if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ACT28");
        CB->animation("ar_200");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        goto done;
    }
    if (ST(ST_SPECIAL_MODE) != 0) goto L_ECA1;
    if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) CB->animation("SP01ACT29");
    if (ST(ST_HERO) == P_ARKHAN2) CB->animation("SP02ACT29");
    if (ST(ST_HERO) == P_ARKHAN3) CB->animation("SP03ACT29");
    CB->animation("ar_200");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    goto done;

L_ECA1:
    if (ST(ST_ACTION_KEY) != 1) goto L_10227;
    if (CB->direction_perso(P_KABAK) == 0) goto L_10227;
    if (ST(ST_SCRIPT_BUSY) != 0) goto L_10227;
    if (ST(ST_VIE(P_KABAK)) != 1) goto L_1014C;
    if (ST(V_ENGAGED_PERSONA) != 0) goto L_1014C;
    ST(ST_SCRIPT_BUSY) = 1;
    CB->bloque(BLOQUE_ALL);
    CB->stop_joindre(-P_KABAK);
    if (ST(ST_PLAYER_WEAPON) != 0) {
        if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0) {
            if (ST(ST_PLAYER_WEAPON) == 0) goto L_FF9D;
            if (ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) goto L_FF9D;
        }
    }

    /* ---- @0x1000ED6A : ST(ST_HERO)==P_ARKHAN1 dialogue state machine (0x898) ---- */
    if (ST(ST_HERO) != P_ARKHAN1) goto L_F130;
    if (ST(V_KABAK_TALK_H1) != 0) goto L_EF2E;
    ST(V_KABAK_TALK_H1) = 1;
    if (ST(ST_SPECIAL_MODE) == 1) {
        l01_action_register("L01D48",  1,    "SP01ATT70", NULL);
        l01_action_register("L01D49A", 0x57, "SP87DIA56", "SP87ARR01");
        l01_action_register("L01D49B", 0x57, "SP87DIA53", "SP87ARR01");
        l01_action_register("L01D50",  1,    "SG01DIA02", NULL);
        l01_action_register("L01D51A", 0x57, "SP87DIA55", "SP87ARR01_10");
        l01_action_register("L01D51B", 0x57, "SP87DIA50", "SP87ARR01_10");
        l01_action_register("L01D51C", 0x57, "SP87DIA60", "SP87ARR01_10");
    }
    if (ST(ST_SPECIAL_MODE) == 0) {
        l01_action_register("L01D52",  1,    "SG01DIA03", NULL);
        l01_action_register("L01D49A", 0x57, "SP87DIA56", "SP87ARR01");
        l01_action_register("L01D49B", 0x57, "SP87DIA53", "SP87ARR01");
        l01_action_register("L01D53",  1,    "SG01DIA08", NULL);
        l01_action_register("L01D54A", 0x57, "SP87DIA62", "SP87ARR01_10");
        l01_action_register("L01D54B", 0x57, "SP87DIA59", "SP87ARR01_10");
    }
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_EF2E:
    if (ST(V_KABAK_TALK_H1) != 1) goto L_EFCB;
    ST(V_KABAK_TALK_H1) = 2;
    l01_action_register("L01D128",  1,    "SG01DIA06", NULL);
    l01_action_register("L01D129A", 0x57, "SP87DIA63", "SP87ARR01_10");
    l01_action_register("L01D129B", 0x57, "SP87DIA64", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_EFCB:
    if (ST(V_KABAK_TALK_H1) != 2) goto L_F068;
    ST(V_KABAK_TALK_H1) = 3;
    l01_action_register("L01D130",  1,    "SG01DIA03", NULL);
    l01_action_register("L01D131A", 0x57, "SP87DIA63", "SP87ARR01_10");
    l01_action_register("L01D131B", 0x57, "SP87DIA65", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_F068:
    if (ST(V_KABAK_TALK_H1) != 3) goto L_F130;
    if (ST(ST_SPECIAL_MODE) == 1) {
        l01_action_register("L01D132", 1,    "SG01DIA01", NULL);
        l01_action_register("L01D133", 0x57, "SP87DIA54", "SP87ARR01_10");
    }
    if (ST(ST_SPECIAL_MODE) == 0) {
        l01_action_register("L01D134", 1,    "SG01DIA01", NULL);
        l01_action_register("L01D135", 0x57, "SP87DIA54", "SP87ARR01_10");
    }
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

    /* ---- @0x1000F130 : ST(ST_HERO)>=2 ---- */
L_F130:
    if ((i32)ST(ST_HERO) < 2) goto L_FF9D;
    if (ST(V_GUARD_FIGHT_DONE) != 0) goto L_F7B4;
    if (ST(0xB88) != 0) goto L_F522;

    if (ST(V_KABAK_TALK_H23) != 1) goto L_F2FE;
    ST(V_KABAK_TALK_H23) = 2;
    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            l01_action_register("L01D55A", 2, "SG02DIA07", NULL);
            l01_action_register("L01D55B", 2, "SG02DIA07", NULL);
            l01_action_register("L01D55C", 2, "SG02DIA07", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            l01_action_register("L01D56A", 2, "SG02DIA07", NULL);
            l01_action_register("L01D56B", 2, "SG02DIA07", NULL);
            l01_action_register("L01D56C", 2, "SG02DIA07", NULL);
        }
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        l01_action_register("L01D57A", 3, "SG03DIA07", NULL);
        l01_action_register("L01D57B", 3, "SG03DIA07", NULL);
    }
    l01_action_register("L01D59A", 0x57, "SP87DIA50", "SP87ARR01_10");
    l01_action_register("L01D59B", 0x57, "SP87DIA64", "SP87ARR01_10");
    l01_action_register("L01D59C", 0x57, "SP87DIA62", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_F2FE:
    if (ST(V_KABAK_TALK_H23) != 2) goto L_F420;
    ST(V_KABAK_TALK_H23) = 3;
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D62A", 2, "SG02DIA01", NULL);
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D62B", 3, "SG03DIA01", NULL);
    l01_action_register("L01D63A", 0x57, "SP87DIA50", "SP87ARR01_10");
    l01_action_register("L01D63B", 0x57, "SP87DIA51", "SP87ARR01_10");
    l01_action_register("L01D63C", 0x57, "SP87DIA56", "SP87ARR01_10");
    l01_action_register("L01D63D", 0x57, "SP87DIA53", "SP87ARR01_10");
    l01_action_register("L01D63E", 0x57, "SP87DIA54", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_F420:
    if ((i32)ST(V_KABAK_TALK_H23) < 3) goto L_F522;
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D64A", 2, "SG02DIA04", NULL);
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D64B", 3, "SG03DIA04", NULL);
    l01_action_register("L01D65A", 0x57, "SP87DIA55", "SP87ARR01_10");
    if (ST(V_KABAK_TALK_H23) == 3) {
        l01_action_register("L01D65B", 0x57, "SP87DIA56", "SP87ARR01_10");
        l01_action_register("L01D65C", 0x57, "SP87DIA57", "SP87ARR01_10");
        ST(V_KABAK_TALK_H23) = 4;
    }
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

    /* ---- @0x1000F522 : ST(0xB88) branch, state 0x77C ---- */
L_F522:
    if (ST(0xB88) == 0) goto L_F7B4;
    if (ST(V_KABAK_TALK_B88) != 0) goto L_F6B2;
    ST(V_KABAK_TALK_B88) = 1;
    if (ST(ST_HERO) == P_ARKHAN2) {
        l01_action_register("L01D55A",  2, "SG02DIA07", NULL);
        l01_action_register("L01D173A", 2, "SG02DIA07", NULL);
        l01_action_register("L01D173B", 2, "SG02DIA07", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D174", 3, "SG03DIA01", NULL);
    if ((i32)ST(V_KABAK_TALK_H23) < 3) {
        l01_action_register("L01D63C", 0x57, "SP87DIA60", "SP87ARR01_10");
        l01_action_register("L01D63D", 0x57, "SP87DIA61", "SP87ARR01_10");
        l01_action_register("L01D63E", 0x57, "SP87DIA62", "SP87ARR01_10");
    } else {
        l01_action_register("L01D175A", 0x57, "SP87DIA56", "SP87ARR01_10");
        l01_action_register("L01D175B", 0x57, "SP87DIA57", "SP87ARR01_10");
        l01_action_register("L01D175C", 0x57, "SP87DIA59", "SP87ARR01_10");
    }
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_F6B2:
    if ((i32)ST(V_KABAK_TALK_B88) < 1) goto L_F7B4;
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D64A", 2, "SG02DIA04", NULL);
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D64B", 3, "SG03DIA04", NULL);
    l01_action_register("L01D65A", 0x57, "SP87DIA53", "SP87ARR01_10");
    if (ST(V_KABAK_TALK_B88) == 1) {
        l01_action_register("L01D65B", 0x57, "SP87DIA63", "SP87ARR01_10");
        l01_action_register("L01D65C", 0x57, "SP87DIA64", "SP87ARR01_10");
        ST(V_KABAK_TALK_B88) = 2;
    }
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

    /* ---- @0x1000F7B4 : ST(V_GUARD_WAVE)==1, state 0x8BC ---- */
L_F7B4:
    if (ST(V_GUARD_WAVE) != 1) goto L_FB1A;
    if (ST(V_KABAK_TALK_G23) != 0) goto L_F911;
    ST(V_KABAK_TALK_G23) = 1;
    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(ST_SPECIAL_MODE) == 1) l01_action_register("L01D68", 2, "SG02DIA04", NULL);
        if (ST(ST_SPECIAL_MODE) == 0) l01_action_register("L01D69", 2, "SG03DIA04", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        if (ST(ST_SPECIAL_MODE) == 1) l01_action_register("L01D70", 3, "SG03DIA04", NULL);
        if (ST(ST_SPECIAL_MODE) == 0) l01_action_register("L01D71", 3, "SG03DIA04", NULL);
    }
    l01_action_register("L01D72A", 0x57, "SP87DIA53", "SP87ARR01_10");
    l01_action_register("L01D72B", 0x57, "SP87ATT50", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_F911:
    if (ST(V_KABAK_TALK_G23) != 1) goto L_F9E8;
    ST(V_KABAK_TALK_G23) = 2;
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D73", 2, "SG02DIA04", NULL);
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D74", 3, "SG03DIA04", NULL);
    l01_action_register("L01D75A", 0x57, "SP87DIA50", "SP87ARR01_10");
    l01_action_register("L01D75B", 0x57, "SP87DIA53", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_F9E8:
    if (ST(V_KABAK_TALK_G23) != 2) goto L_FB1A;
    ST(V_KABAK_TALK_G23) = 3;
    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(ST_SPECIAL_MODE) == 1) l01_action_register("L01D76", 2, "SG00DIA04", NULL);
        if (ST(ST_SPECIAL_MODE) == 0) l01_action_register("L01D77", 2, "SG00DIA04", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        if (ST(ST_SPECIAL_MODE) == 1) l01_action_register("L01D78", 2, "SG00DIA04", NULL);
        if (ST(ST_SPECIAL_MODE) == 0) l01_action_register("L01D79", 2, "SG00DIA04", NULL);
    }
    l01_action_register("L01D146", 0x57, "SP87ATT70", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

    /* ---- @0x1000FB1A : ST(V_GUARD_WAVE)==2, state 0xCD8 ---- */
L_FB1A:
    if (ST(V_GUARD_WAVE) != 2) goto L_FCC1;
    if (ST(V_KABAK_TALK_G24) != 0) goto L_FC1C;
    ST(V_KABAK_TALK_G24) = 1;
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D108", 2, "SG02DIA04", NULL);
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D109", 3, "SG03DIA04", NULL);
    l01_action_register("L01D110A", 0x57, "SP87ATT70", "SP87ARR01_10");
    l01_action_register("L01D110B", 0x57, "SP87DIA60", "SP87ARR01_10");
    l01_action_register("L01D110C", 0x57, "SP87DIA62", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_FC1C:
    if (ST(V_KABAK_TALK_G24) != 1) goto L_FCC1;
    ST(V_KABAK_TALK_G24) = 2;
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D111", 2, "SG02DIA04", NULL);
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D112", 3, "SG03DIA04", NULL);
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

    /* ---- @0x1000FCC1 : state 0xCD4 ---- */
L_FCC1:
    if (ST(V_KABAK_TALK_G23) == 3) goto L_FCE5;
    if (ST(V_KABAK_TALK_G24) != 2) goto L_FF9D;
L_FCE5:
    if (ST(V_KABAK_TALK_END) != 0) goto L_FDBC;
    ST(V_KABAK_TALK_END) = 1;
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D200", 2, "SG02DIA01", NULL);
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D201", 3, "SG03DIA01", NULL);
    l01_action_register("L01D202A", 0x57, "SP87DIA53", "SP87ARR01_10");
    l01_action_register("L01D202B", 0x57, "SP87DIA56", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_FDBC:
    if (ST(V_KABAK_TALK_END) != 1) goto L_FE7A;
    ST(V_KABAK_TALK_END) = 2;
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D203", 2, "SG02DIA05", NULL);
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D204", 3, "SG03DIA05", NULL);
    l01_action_register("L01D205", 0x57, "SP87DIA55", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

L_FE7A:
    if (ST(V_KABAK_TALK_END) != 2) goto L_FF9D;
    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(ST_SPECIAL_MODE) == 1) l01_action_register("L01D206", 2, "SG02DIA05", NULL);
        if (ST(ST_SPECIAL_MODE) == 0) l01_action_register("L01D207", 2, "SG02DIA05", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        if (ST(ST_SPECIAL_MODE) == 1) l01_action_register("L01D208", 3, "SG03DIA05", NULL);
        if (ST(ST_SPECIAL_MODE) == 0) l01_action_register("L01D209", 3, "SG03DIA05", NULL);
    }
    l01_action_register("L01D210", 0x57, "SP87DIA54", "SP87ARR01_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

    /* ---- @0x1000FF9D : prefetch-slot change handling ---- */
L_FF9D:
    if (ST(ST_PLAYER_WEAPON) == 0) goto L_1014C;
    if (CB->divers(DV_CAN_FIGHT, 0, 2) != 0) goto L_1014C;
    if (ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON)) goto L_1014C;
    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
    if (ST(ST_HERO) == P_ARKHAN1) l01_action_register("L01D105", 1, "SG00PRE80", "SG00PRE81");
    if (ST(ST_HERO) == P_ARKHAN2) l01_action_register("L01D106", 2, "SG00PRE80", "SG00PRE81");
    if (ST(ST_HERO) == P_ARKHAN3) l01_action_register("L01D107", 3, "SG00PRE80", "SG00PRE81");
    ST(V_DLG_RANDOM) = CB->random(3);
    if (ST(V_DLG_RANDOM) == 0) l01_action_register("L01D92A", 0x57, "SP87ATT60", "SP87ARR01_10");
    if (ST(V_DLG_RANDOM) == 1) l01_action_register("L01D92B", 0x57, "SP87ATT61", "SP87ARR01_10");
    if (ST(V_DLG_RANDOM) == 2) l01_action_register("L01D92C", 0x57, "SP87ATT62", "SP87ARR01_10");
    CB->animation("G00PRE82");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
    CB->joindre_perm(0x57, ST(ST_HERO));
    goto done;

    /* ---- @0x1001014C ---- */
L_1014C:
    if (ST(ST_VIE(P_KABAK)) != 1) goto L_10205;
    if (ST(V_ENGAGED_PERSONA) != 0x57) goto L_10205;
    if (ST(ST_COMBAT_STANCE) != 0) goto L_10205;
    CB->bloque(BLOQUE_ALL);
    if (ST(ST_HERO) == P_ARKHAN1) CB->dialogue("L01D38", P_ARKHAN1);
    if (ST(ST_HERO) == P_ARKHAN2) CB->dialogue("L01D39", P_ARKHAN2);
    if (ST(ST_HERO) == P_ARKHAN3) CB->dialogue("L01D40", P_ARKHAN3);
    CB->dialogue("L01D93", P_KABAK);
    CB->bloque(BLOQUE_NONE);
L_10205:
    if (ST(ST_VIE(P_KABAK)) != VIE_DEAD) goto L_10227;
    CB->fouille(P_KABAK);
    CB->fouille(P_KABAK);
    goto done;

    /* ---- @0x10010227 : end-of-level / return sequence ---- */
L_10227:
    if ((i32)ST(V_PLAYER_STRIKE) < 1) goto L_1026A;
    if (ST(V_ENGAGED_PERSONA) != 0) goto L_1026A;
    if (CB->direction_zone(9) == 0) goto L_1026A;
    if (ST(ST_VIE(P_KABAK)) != VIE_DEAD) goto L_102BF;
L_1026A:
    if (ST(V_CRI_KEY) != 1) goto done;
    if (ST(V_ENGAGED_PERSONA) != 0) goto done;
    if (ST(ST_COMBAT_STANCE) != 1) goto done;
    if (CB->direction_perso(P_KABAK) == 0) goto done;
    if (ST(ST_VIE(P_KABAK)) != 1) goto done;
L_102BF:
    CB->bloque(BLOQUE_ALL);
    CB->stop_joindre(-P_KABAK);
    ST(V_PLAYER_STRIKE) = -1;
    ST(V_ENGAGED_PERSONA) = 0x57;
    CB->animation("SP87ESA14");
    if (ST(V_KABAK_INTRO) == 0) {
        CB->animation("P87_65535");
        CB->evenement(2, 0x64);
        goto done;
    }
    if (ST(V_KABAK_INTRO) != 1) goto done;
    ST(V_KABAK_FIGHT_ON) = 1;
    ST(V_KABAK_INTRO) = 2;
    CB->dialogue("L01D86", P_KABAK);
    if (ST(V_GUARD_WAVE) == 1) goto L_10396;
    CB->bloque(BLOQUE_NONE);
    CB->combat(P_KABAK, 0);
    goto done;

L_10396:
    if (ST(V_GUARD_WAVE) != 1) goto done;
    if (ST(ST_VIE(P_GUARDIAN23)) != 1) goto done;
    if (ST(0x94) != 1) goto L_10413;
    CB->bloque(BLOQUE_NONE);
    CB->combat(P_KABAK, 0);
    CB->select_pile(2, 1);
    CB->dialogue("L01D136", P_GUARDIAN23);
    CB->combat(P_GUARDIAN23, 0);
    goto done;

L_10413:
    if (ST(0x94) != 0) goto done;
    ST(0x94) = 1;
    CB->select_pile(2, 1);
    CB->change_camera(6, CAM_LOCKED);
    CB->vire_col(1, 0);
    CB->animation("SP88LEV01");
    CB->animation("P88_65535");
    CB->vire_col(1, 1);
    CB->dialogue("L01D136", P_GUARDIAN23);
    CB->change_camera(CAM_ANY, CAM_AUTO);
    CB->bloque(BLOQUE_NONE);
    CB->combat(P_GUARDIAN23, 0);
    CB->combat(P_KABAK, 0);
    goto done;

done:
    return;
}

static void trig11(void) {   /* sub_1001053A */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 &&
        ST(ST_COMBAT_STANCE) == 0 &&
        (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 1) > -2500 &&
        CB->direction_spec(0x4000) != 0 &&
        (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) < 0x55F0 &&
        (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) > 0x32C8) {

        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();

        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP00ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP00ACT86");
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP01ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP01ACT86");
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2) == 0) CB->animation("SP02ACT11");
                else                        CB->animation("SP02ACT12");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2) == 0) CB->animation("SP02ACT86");
                else                        CB->animation("SP02ACT85");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2) == 0) CB->animation("SP03ACT11");
                else                        CB->animation("SP03ACT12");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2) == 0) CB->animation("SP03ACT86");
                else                        CB->animation("SP03ACT85");
            }
        }
        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    } else {
        if (CB->direction_zone(0xD) != 0 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            l01_spawn_ar();

            if (ST(ST_SPECIAL_MODE) == 1) {
                if ((i32)ST(ST_HERO) <= 1) CB->animation("SP01ACT28");
                if (ST(ST_HERO) == P_ARKHAN2)      CB->animation("SP02ACT28");
                if (ST(ST_HERO) == P_ARKHAN3)      CB->animation("SP03ACT28");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if ((i32)ST(ST_HERO) <= 1) CB->animation("SP01ACT29");
                if (ST(ST_HERO) == P_ARKHAN2)      CB->animation("SP02ACT29");
                if (ST(ST_HERO) == P_ARKHAN3)      CB->animation("SP03ACT29");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        } else if (CB->direction_spec(0x7D00) != 0 &&
                   CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x1B) != 0) {
            trig27();
        }
    }
}

static void trig15(void) {   /* sub_10010A95 */
    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (ST(V_ENGAGED_PERSONA) != 0) return;
    if (ST(ST_COMBAT_STANCE) != 0) return;
    if (CB->direction_zone(0xE) == 0) return;

    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    l01_spawn_ar();

    if (ST(ST_HERO) == P_ARKHAN0) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(3);
            if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD06", 0, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 0, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD09", 0, "SG00ATT01", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            l01_action_register("L01D176", 0, "SP00ACT86", NULL);
        }
    }

    if (ST(ST_HERO) == P_ARKHAN1) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(3);
            if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD06", 1, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 1, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD09", 1, "SG00ATT01", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            l01_action_register("L01D176", 1, "SP01ACT86", "Sar_10");
        }
    }

    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(3);
            if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD07", 2, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD76", 2, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD10", 2, "SG00ATT01", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            if (CB->divers(DV_CAN_FIGHT, 7, 2) != 0) l01_action_register("L01D177", 2, "SP02ACT85", "Sar_10");
            if (CB->divers(DV_CAN_FIGHT, 7, 2) == 0) l01_action_register("L01D177", 2, "SP02ACT86", "Sar_10");
        }
    }

    if (ST(ST_HERO) == P_ARKHAN3) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(3);
            if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD08", 3, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD77", 3, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD11", 3, "SG00ATT01", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            if (CB->divers(DV_CAN_FIGHT, 7, 2) != 0) l01_action_register("L01D178", 3, "SP03ACT85", "Sar_10");
            if (CB->divers(DV_CAN_FIGHT, 7, 2) == 0) l01_action_register("L01D178", 3, "SP03ACT86", "Sar_10");
        }
    }

    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

static void trig17(void) {   /* sub_10011041 */
    /* --- region 1 ------------------------------------------------------ */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0
        && CB->direction_spec(0xBB80)
        && CB->divers(DV_IN_ZONE, ST(ST_HERO), 0x1C)
        && !CB->direction_perso(P_GUARDIAN23)) {
        CB->bloque(BLOQUE_ALL);
        if ((i32)ST(ST_HERO) <= 1)
            l01_action_register("LCOD52", 1, "SG00SAV01", "Sar_10");
        if (ST(ST_HERO) == P_ARKHAN2)
            l01_action_register("LCOD53", 2, "SG00SAV01", "Sar_10");
        if (ST(ST_HERO) == P_ARKHAN3)
            l01_action_register("LCOD54", 3, "SG00SAV01", "Sar_10");
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    /* --- region 2 ------------------------------------------------------ */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_GUARDIAN23)) {
        if (ST(ST_VIE(P_GUARDIAN23)) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(0x98) = 1;
            if (ST(ST_LOCKED_CAMERA) != 6)
                CB->change_camera(6, CAM_LOCKED);

            /* decide between CD0-machine and the 0x785 relocate block */
            if (!(ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON))) {
                /* loc_1001125C : CD0 == 0 */
                if (ST(V_G23_TALK) == 0) {
                    ST(V_G23_TALK) = 1;
                    if (ST(ST_HERO) == P_ARKHAN2)
                        l01_action_register("L01D159", 2, "SG00DIA01", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        l01_action_register("L01D160", 2, "SG00DIA01", NULL);
                    if (ST(0x94) == 0) {
                        ST(0x94) = 1;
                        l01_action_register("L01D97A", 0x58, "SP88LEV01", "SP88ARR01_10");
                    } else {
                        l01_action_register("L01D97A", 0x58, "SP88DIA59", "SP88ARR01_10");
                    }
                    l01_action_register("L01D97B", 0x58, "SP88DIA59", "SP88ARR01_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->joindre_perm(0x58, ST(ST_HERO));
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* loc_10011372 : CD0 == 1 */
                if (ST(V_G23_TALK) == 1) {
                    CB->stop_joindre(-P_GUARDIAN23);
                    CB->joindre(P_GUARDIAN23, 0, 0, 0, ST(ST_HERO));
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        if (ST(ST_SPECIAL_MODE) == 1)
                            l01_action_register("L01D161", 2, "SG00DIA04", NULL);
                        if (ST(ST_SPECIAL_MODE) == 0)
                            l01_action_register("L01D162", 2, "SG00DIA03", NULL);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        if (ST(ST_SPECIAL_MODE) == 1)
                            l01_action_register("L01D163", 3, "SG00DIA04", NULL);
                        if (ST(ST_SPECIAL_MODE) == 0)
                            l01_action_register("L01D164", 3, "SG00DIA03", NULL);
                    }
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        ST(V_G23_TALK) = 2;
                        l01_action_register("L01D165A", 0x58, "SP88DIA55", "SP88ARR01");
                        l01_action_register("L01D165B", 0x58, "SP88DIA62", "SP88ARR01");
                        l01_action_register("L01D165C", 0x58, "SP88DIA62", "SP88ARR01");
                        l01_action_register("L01D165D", 0x58, "SP88DIA62", "SP88ARR01");
                        l01_action_register("L01D165E", 0x58, "SP88DIA60", "SP88ARR01_10");
                    }
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->joindre(-P_KABAK, 0, 0, 0, 0x58);
                        l01_action_register("L01D166", 0x58, "SP88DIA60", "SP88ARR01_500");
                        ST(V_G23_TALK) = 2;
                        if (ST(0xD4) == 0) {
                            CB->pos_perso(P_KABAK, 320, -2530, 0x8CA0, 1, "P87ARR01", 9, 1);
                            CB->change_camera(2, CAM_LOCKED);
                            if (ST(0x80) == 0)
                                l01_action_register("L01D167", 0x57, "SP87DIA54", "SP87ARR01_10");
                            if (ST(0x80) != 0) {
                                ST(0x80) = 0;
                                CB->stop_joindre(-P_KABAK);
                                CB->animation("SP87GAR27");
                                CB->animation("P87ARR01_200");
                                l01_action_register("L01D167", 0x57, "P87DIA57", "SP87ARR01_10");
                            }
                            CB->modify_var(&ST(0xD4), 1);
                            CB->change_camera(6, CAM_LOCKED);
                        }
                        CB->animation("sar_100");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 1);
                        CB->pos_perso(P_KABAK, 320, -2530, 0xDD4, 1, "P87ATT01", 9, 1);
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->joindre_perm(0x58, ST(ST_HERO));
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* loc_100116C7 : CD0 == 2 */
                if (ST(V_G23_TALK) == 2) {
                    CB->stop_joindre(-P_GUARDIAN23);
                    CB->joindre(P_GUARDIAN23, 0, 0, 0, ST(ST_HERO));
                    if (ST(ST_HERO) == P_ARKHAN2)
                        l01_action_register("L01D168", 2, "SG00DIA07", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        l01_action_register("L01D169", 3, "SG00DIA07", NULL);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->joindre_perm(0x58, ST(ST_HERO));
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }

            /* loc_10011785 : object relocated */
            if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                if (ST(ST_HERO) == P_ARKHAN2)
                    l01_action_register("L01D106", 2, "SG00PRE80", "SG00PRE81");
                if (ST(ST_HERO) == P_ARKHAN3)
                    l01_action_register("L01D107", 3, "SG00PRE80", "SG00PRE81");
                l01_action_register("L01D170", 0x58, "SP88ARR02", "SP88ARR02_10");
                CB->animation("G00PRE82");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->joindre_perm(0x58, ST(ST_HERO));
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }

        /* loc_1001188A */
        if (ST(ST_VIE(P_GUARDIAN23)) == VIE_DEAD && ST(ST_SCRIPT_BUSY) == 0)
            CB->fouille(P_GUARDIAN23);
    }

    /* --- region 3 ------------------------------------------------------ */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) != 0 && ST(ST_COMBAT_STANCE) == 0
        && ST(ST_VIE(P_GUARDIAN23)) != VIE_DEAD && CB->direction_perso(P_GUARDIAN23)) {
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->bloque(BLOQUE_ALL);
            l01_action_register("LCOD20", 2, "SP02ATT51", NULL);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->bloque(BLOQUE_ALL);
            l01_action_register("LCOD21", 3, "SP03ATT51", NULL);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }

    /* --- region 4 ------------------------------------------------------ */
    if (!((i32)ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && (i32)ST(ST_VIE(P_GUARDIAN23)) < 2 && CB->direction_perso(P_GUARDIAN23))) {
        /* loc_100119DA */
        if (!(ST(V_CRI_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1
              && CB->direction_perso(P_GUARDIAN23) && ST(ST_VIE(P_GUARDIAN23)) == 1))
            return;
    }

    /* loc_10011A2F */
    ST(V_G23_FIGHT_ON) = 1;
    ST(V_ENGAGED_PERSONA) = 0x57;
    ST(V_PLAYER_STRIKE) = -1;
    CB->bloque(BLOQUE_ALL);
    CB->stop_joindre(-P_GUARDIAN23);
    CB->animation("SP88ESM05");
    CB->animation("P88_65535");
    l01_action_register("L01D147", 0x58, "P88DIA60", "SP88GAR01_10");
    if (ST(ST_VIE(P_KABAK)) == 1) {
        ST(V_KABAK_FIGHT_ON) = 1;
        CB->dialogue("L01D148", P_KABAK);
        CB->combat(P_KABAK, 0);
    }
    CB->bloque(BLOQUE_NONE);
    CB->combat(P_GUARDIAN23, 0);
}

static void trig18(void) {   /* sub_10011AF3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_GUARDIAN24)) {
        if (ST(ST_VIE(P_GUARDIAN24)) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(0x98) = 1;
            if (ST(ST_LOCKED_CAMERA) != 6)
                CB->change_camera(6, CAM_LOCKED);
            if (!(ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON))) {
                if (ST(V_G24_TALK) == 0) {
                    ST(V_G24_TALK) = 1;
                    CB->animation("SP89LEV01");
                    if (ST(ST_HERO) == P_ARKHAN2)
                        l01_action_register("L01D150", 2, "SG00DIA01", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        l01_action_register("L01D151", 3, "SG00DIA01", NULL);
                    l01_action_register("L01D152A", 0x59, "P89DIA01", "SP89DIA01");
                    l01_action_register("L01D152B", 0x59, "SP89DIA01", "SP89LEV02");
                    CB->animation("P89ARR01");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                if (ST(V_G24_TALK) == 1) {
                    ST(V_G24_TALK) = 2;
                    CB->animation("SP89LEV01");
                    if (ST(ST_HERO) == P_ARKHAN2)
                        l01_action_register("L01D153", 2, "SG00DIA01", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        l01_action_register("L01D154", 3, "SG00DIA01", NULL);
                    l01_action_register("L01D155A", 0x59, "P89DIA01", "SP89DIA01");
                    l01_action_register("L01D155B", 0x59, "SP89DIA01", "SP89DIA01");
                    l01_action_register("L01D155C", 0x59, "SP89LEV02", "SP89ARR01");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                if (ST(V_G24_TALK) == 2) {
                    if (ST(ST_HERO) == P_ARKHAN2)
                        l01_action_register("L01D156", 2, "SG00DIA01", NULL);
                    if (ST(ST_HERO) == P_ARKHAN3)
                        l01_action_register("L01D157", 3, "SG00DIA01", NULL);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* L1E3A */
            if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                if (ST(ST_HERO) == P_ARKHAN2)
                    l01_action_register("L01D117", 2, "SG00PRE80", "SG00PRE81");
                if (ST(ST_HERO) == P_ARKHAN3)
                    l01_action_register("L01D118", 3, "SG00PRE80", "SG00PRE81");
                CB->dialogue("L01D149", P_GUARDIAN24);
                CB->animation("G00PRE82");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* L1F20 */
        if (ST(ST_VIE(P_GUARDIAN24)) == VIE_DEAD) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00PRE03");
                if (ST(ST_SPECIAL_MODE) == 1)
                    CB->dialogue("LCOD82", P_ARKHAN2);
                if (ST(ST_SPECIAL_MODE) == 0)
                    CB->dialogue("LCOD83", P_ARKHAN2);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG00PRE03");
                if (ST(ST_SPECIAL_MODE) == 1)
                    CB->dialogue("LCOD84", P_ARKHAN3);
                if (ST(ST_SPECIAL_MODE) == 0)
                    CB->dialogue("LCOD85", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* L2034 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(V_ENGAGED_PERSONA) == 0 &&
        CB->direction_perso(P_GUARDIAN24) && ST(ST_VIE(P_GUARDIAN24)) == 1 && ST(ST_VIE(P_KABAK)) == 1) {
        ST(V_KABAK_FIGHT_ON) = 1;
        ST(V_KABAK_INTRO) = 2;
        CB->dialogue("L01D148", P_KABAK);
        CB->bloque(BLOQUE_NONE);
        CB->combat(P_KABAK, 0);
        ST(V_ENGAGED_PERSONA) = 0x57;
        return;
    }
    /* L2101 */
    if ((i32)ST(V_PLAYER_STRIKE) < 1) return;
    if (ST(V_ENGAGED_PERSONA) != 0) return;
    if ((i32)ST(ST_VIE(P_GUARDIAN24)) >= 2) return;
    if (!CB->direction_perso(P_GUARDIAN24)) return;
    CB->bloque(BLOQUE_ALL);
    ST(0x98) = 1;
    CB->change_camera(-7, CAM_LOCKED);
    ST(V_ENGAGED_PERSONA) = 0x57;
    ST(V_PLAYER_STRIKE) = -1;
    CB->animation("SP89IMP51");
    ST(ST_VIE(P_GUARDIAN24)) = VIE_DEAD;
    if (ST(ST_VIE(P_KABAK)) == 1) {
        ST(V_KABAK_FIGHT_ON) = 1;
        ST(V_KABAK_INTRO) = 2;
        CB->dialogue("L01D148", P_KABAK);
        CB->bloque(BLOQUE_NONE);
        CB->combat(P_KABAK, 0);
    }
    if (ST(ST_VIE(P_KABAK)) == 1) return;
    CB->bloque(BLOQUE_NONE);
}

static void trig21(void) {   /* sub_1001222A */
    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (ST(V_ENGAGED_PERSONA) != 0) return;
    if (ST(ST_COMBAT_STANCE) != 0) return;
    if (!CB->direction_zone(0x14)) return;

    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    l01_spawn_ar();

    if (ST(ST_HERO) == P_ARKHAN0) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD09", 0, "SG00PRE02", NULL);
        if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 0, "SG00PRE02", NULL);
        if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD06", 0, "SG00PRE02", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN1) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD09", 1, "SG00PRE02", NULL);
        if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 1, "SG00PRE02", NULL);
        if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD06", 1, "SG00PRE02", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN2) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD10", 2, "SG00PRE02", NULL);
        if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD76", 2, "SG00PRE02", NULL);
        if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD07", 2, "SG00PRE02", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD11", 3, "SG00PRE02", NULL);
        if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD77", 3, "SG00PRE02", NULL);
        if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD08", 3, "SG00PRE02", NULL);
    }

    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

static void trig23(void) {   /* sub_100125A8 */
    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (ST(V_ENGAGED_PERSONA) != 0) return;
    if (ST(ST_COMBAT_STANCE) != 0) return;
    if (CB->direction_zone(0x16) == 0) return;

    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;

    l01_spawn_ar();

    if (ST(ST_HERO) == P_ARKHAN0) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(3);
            if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD09", 0, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 0, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD06", 0, "SG00ATT01", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0)
            l01_action_register("L01D176", 0, "SP00ACT86", "Sar_10");
    }

    if (ST(ST_HERO) == P_ARKHAN1) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(3);
            if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD09", 1, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 1, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD06", 1, "SG00ATT01", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0)
            l01_action_register("L01D176", 1, "SP01ACT86", "Sar_10");
    }

    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(3);
            if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD10", 2, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD76", 2, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD07", 2, "SG00ATT01", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            if (CB->divers(DV_CAN_FIGHT, 7, 2))
                l01_action_register("L01D177", 2, "SP02ACT85", "Sar_10");
            if (!CB->divers(DV_CAN_FIGHT, 7, 2))
                l01_action_register("L01D177", 2, "SP02ACT86", "Sar_10");
        }
    }

    if (ST(ST_HERO) == P_ARKHAN3) {
        if (ST(ST_SPECIAL_MODE) == 1) {
            ST(V_DLG_RANDOM) = CB->random(3);
            if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD11", 3, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD77", 3, "SG00ATT01", NULL);
            if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD08", 3, "SG00ATT01", NULL);
        }
        if (ST(ST_SPECIAL_MODE) == 0) {
            if (CB->divers(DV_CAN_FIGHT, 7, 2))
                l01_action_register("L01D178", 3, "SP03ACT85", "Sar_10");
            if (!CB->divers(DV_CAN_FIGHT, 7, 2))
                l01_action_register("L01D178", 3, "SP03ACT86", "Sar_10");
        }
    }

    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

static void trig24(void) {   /* sub_10012AA6 */
    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (CB->direction_zone(0x18) == 0) return;

    ST(ST_SCRIPT_BUSY) = 1;
    CB->bloque(BLOQUE_ALL);

    l01_spawn_ar();

    if (ST(ST_HERO) == P_ARKHAN0) {
        ST(V_DLG_RANDOM) = CB->random(2);
        if (ST(V_DLG_RANDOM) == 0)
            l01_action_register("LCOD75", 0, "SG00ATT01", NULL);
        if (ST(V_DLG_RANDOM) == 1)
            l01_action_register("LCOD06", 0, "SG00ATT01", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN1) {
        ST(V_DLG_RANDOM) = CB->random(2);
        if (ST(V_DLG_RANDOM) == 0)
            l01_action_register("LCOD75", 1, "SG00ATT01", NULL);
        if (ST(V_DLG_RANDOM) == 1)
            l01_action_register("LCOD06", 1, "SG00ATT01", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN2) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0)
            l01_action_register("LCOD10", 2, "SG00ATT01", NULL);
        if (ST(V_DLG_RANDOM) == 1)
            l01_action_register("LCOD76", 2, "SG00ATT01", NULL);
        if (ST(V_DLG_RANDOM) == 2)
            l01_action_register("LCOD07", 2, "SG00ATT01", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0)
            l01_action_register("LCOD11", 3, "SG00ATT01", NULL);
        if (ST(V_DLG_RANDOM) == 1)
            l01_action_register("LCOD77", 3, "SG00ATT01", NULL);
        if (ST(V_DLG_RANDOM) == 2)
            l01_action_register("LCOD08", 3, "SG00ATT01", NULL);
    }

    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

static void trig26(void) {   /* sub_10012FFD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 &&
        CB->direction_spec(0xEFD8)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();

        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD09", 0, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 0, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD06", 0, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                l01_action_register("L01D176", 0, "SP00ACT86", "Sar_10");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD09", 1, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 1, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD06", 1, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                l01_action_register("L01D176", 1, "SP01ACT86", "Sar_10");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD10", 2, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD76", 2, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD07", 2, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) l01_action_register("L01D177", 2, "SP02ACT85", "Sar_10");
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) l01_action_register("L01D177", 2, "SP02ACT86", "Sar_10");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD11", 3, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD77", 3, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD08", 3, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) l01_action_register("L01D178", 3, "SP03ACT85", "Sar_10");
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) l01_action_register("L01D178", 3, "SP03ACT86", "Sar_10");
            }
        }

        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 &&
        CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();

        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0) l01_action_register("LCOD25", 0, "SG00ATT01", NULL);
                else                         l01_action_register("LCOD25", 0, "SG00ATT03", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0) l01_action_register("LCOD25", 0, "SG00ATT02", NULL);
                else                         l01_action_register("LCOD25", 0, "SG00ATT04", NULL);
            }
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0) l01_action_register("LCOD25", 1, "SG01ATT01", NULL);
                else                         l01_action_register("LCOD25", 1, "SG01ATT03", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0) l01_action_register("LCOD25", 1, "SG01ATT02", NULL);
                else                         l01_action_register("LCOD25", 1, "SG01ATT04", NULL);
            }
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0)      l01_action_register("LCOD26", 2, "SG02ATT01", NULL);
                else if (CB->divers(DV_CAN_FIGHT, 9, 2) == 0) l01_action_register("LCOD26", 2, "SG02ATT03", NULL);
                else                              l01_action_register("LCOD26", 2, "SG02ATT05", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0)      l01_action_register("LCOD26", 2, "SG02ATT02", NULL);
                else if (CB->divers(DV_CAN_FIGHT, 9, 2) == 0) l01_action_register("LCOD26", 2, "SG02ATT04", NULL);
                else                              l01_action_register("LCOD26", 2, "SG02ATT06", NULL);
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0)      l01_action_register("LCOD27", 3, "SG03ATT01", NULL);
                else if (CB->divers(DV_CAN_FIGHT, 9, 2) == 0) l01_action_register("LCOD27", 3, "SG03ATT03", NULL);
                else                              l01_action_register("LCOD27", 3, "SG03ATT05", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 0, 2) == 0)      l01_action_register("LCOD27", 3, "SG03ATT02", NULL);
                else if (CB->divers(DV_CAN_FIGHT, 9, 2) == 0) l01_action_register("LCOD27", 3, "SG03ATT04", NULL);
                else                              l01_action_register("LCOD27", 3, "SG03ATT06", NULL);
            }
        }

        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

static void trig27(void) {   /* sub_100139C8 */

    /* --- early branch: advance/skip helper --- */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && CB->direction_zone(0x0D) &&
        (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) < 0x36B0) {
        trig11();
        return;
    }

    /* --- window > 0x4E20: spawn 'ar' actor set (variant A) --- */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 &&
        ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 &&
        CB->direction_spec(0x7E2C) &&
        (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) > 0x4E20) {

        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();

        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SP00ACT13");
                CB->animation("P00_65535");
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) CB->dialogue("LCOD09", 0);
                if (ST(V_DLG_RANDOM) == 1) CB->dialogue("LCOD75", 0);
                if (ST(V_DLG_RANDOM) == 2) CB->dialogue("LCOD06", 0);
            }
            if (ST(ST_SPECIAL_MODE) == 0)
                l01_action_register("L01D176", 0, "SP00ACT86", NULL);
            CB->animation("Sar_50");
        }

        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SP00ACT13");
                CB->animation("P00_65535");
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) CB->dialogue("LCOD09", P_ARKHAN1);
                if (ST(V_DLG_RANDOM) == 1) CB->dialogue("LCOD75", P_ARKHAN1);
                if (ST(V_DLG_RANDOM) == 2) CB->dialogue("LCOD06", P_ARKHAN1);
            }
            if (ST(ST_SPECIAL_MODE) == 0)
                l01_action_register("L01D176", 1, "SP01ACT86", NULL);
            CB->animation("Sar_50");
        }

        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD10", 2, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD76", 2, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD07", 2, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2))
                    l01_action_register("L01D177", 2, "SP02ACT85", NULL);
                if (!CB->divers(DV_CAN_FIGHT, 7, 2))
                    l01_action_register("L01D177", 2, "SP02ACT86", NULL);
            }
        }

        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD11", 3, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD77", 3, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD08", 3, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2))
                    l01_action_register("L01D178", 3, "SP03ACT85", NULL);
                if (!CB->divers(DV_CAN_FIGHT, 7, 2))
                    l01_action_register("L01D178", 3, "SP03ACT86", NULL);
            }
        }

        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    /* --- window [0x36B0, 0x4E20]: spawn 'ar' actor set (variant B) --- */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 &&
        ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 &&
        (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) >= 0x36B0 &&
        (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) <= 0x4E20) {

        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();

        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP00ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP00ACT86");
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) CB->animation("SP01ACT11");
            if (ST(ST_SPECIAL_MODE) == 0) CB->animation("SP01ACT86");
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP02ACT12");
                else                   CB->animation("SP02ACT11");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP02ACT85");
                else                   CB->animation("SP02ACT86");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP03ACT12");
                else                   CB->animation("SP03ACT11");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) CB->animation("SP03ACT85");
                else                   CB->animation("SP03ACT86");
            }
        }

        CB->animation("ar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

static void trig28(void) {   /* sub_1001427D */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 &&
        ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x9858) &&
        (i32)CB->divers(DV_DESC_POS, ST(ST_HERO), 2) < 0xABE0) {

        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        l01_spawn_ar();

        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD09", 0, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 0, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD06", 0, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0)
                l01_action_register("L01D176", 0, "SP00ACT86", "Sar_10");
        }

        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD09", 1, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD75", 1, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD06", 1, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0)
                l01_action_register("L01D176", 1, "SP01ACT86", "Sar_10");
        }

        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD10", 2, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD76", 2, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD07", 2, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2) != 0)
                    l01_action_register("L01D177", 2, "SP02ACT85", "Sar_10");
                if (CB->divers(DV_CAN_FIGHT, 7, 2) == 0)
                    l01_action_register("L01D177", 2, "SP02ACT86", "Sar_10");
            }
        }

        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) l01_action_register("LCOD11", 3, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 1) l01_action_register("LCOD77", 3, "SG00ATT01", NULL);
                if (ST(V_DLG_RANDOM) == 2) l01_action_register("LCOD08", 3, "SG00ATT01", NULL);
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2) != 0)
                    l01_action_register("L01D178", 3, "SP03ACT85", "Sar_10");
                if (CB->divers(DV_CAN_FIGHT, 7, 2) == 0)
                    l01_action_register("L01D178", 3, "SP03ACT86", "Sar_10");
            }
        }

        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xBB80)) {
        CB->bloque(BLOQUE_ALL);
        if ((i32)ST(ST_HERO) <= 1)
            l01_action_register("LCOD52", 1, "SG00SAV01", "Sar_10");
        if (ST(ST_HERO) == P_ARKHAN2)
            l01_action_register("LCOD53", 2, "SG00SAV01", "Sar_10");
        if (ST(ST_HERO) == P_ARKHAN3)
            l01_action_register("LCOD54", 3, "SG00SAV01", "Sar_10");
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
    }
}

static void trig29(void) {   /* sub_100148A3 */
    if (ST(ST_ACTION_KEY) == 1 && CB->direction_zone(0x1D) && CB->direction_perso(P_KALHI12) && CB->divers(DV_PERSONA_PRESENT, P_KALHI12, 0))
        return;

    if (ST(ST_ACTION_KEY) != 1) return;
    if (ST(ST_SCRIPT_BUSY) != 0) return;
    if (ST(ST_COMBAT_STANCE) != 0) return;

    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;

    if (ST(ST_LOCKED_CAMERA) != 6) {
        CB->change_camera(6, CAM_LOCKED);
        ST(0x98) = 1;
    }

    if (ST(V_OB44D_TAKEN) == 0) {
        ST(V_OB44D_TAKEN) = 1;
        CB->animation("SG00PRE07");
        CB->etat_objet("OB44D", -1, -1, -1, -1, -1, -1, -3, 0);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }

    CB->animation("SG00ATT01");

    if (ST(ST_HERO) == P_ARKHAN0) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0) CB->dialogue("LCOD09", 0);
        if (ST(V_DLG_RANDOM) == 1) CB->dialogue("LCOD75", 0);
        if (ST(V_DLG_RANDOM) == 2) CB->dialogue("LCOD06", 0);
    }
    if (ST(ST_HERO) == P_ARKHAN1) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0) CB->dialogue("LCOD09", P_ARKHAN1);
        if (ST(V_DLG_RANDOM) == 1) CB->dialogue("LCOD75", P_ARKHAN1);
        if (ST(V_DLG_RANDOM) == 2) CB->dialogue("LCOD06", P_ARKHAN1);
    }
    if (ST(ST_HERO) == P_ARKHAN2) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0) CB->dialogue("LCOD10", P_ARKHAN2);
        if (ST(V_DLG_RANDOM) == 1) CB->dialogue("LCOD76", P_ARKHAN2);
        if (ST(V_DLG_RANDOM) == 2) CB->dialogue("LCOD07", P_ARKHAN2);
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        ST(V_DLG_RANDOM) = CB->random(3);
        if (ST(V_DLG_RANDOM) == 0) CB->dialogue("LCOD11", P_ARKHAN3);
        if (ST(V_DLG_RANDOM) == 1) CB->dialogue("LCOD77", P_ARKHAN3);
        if (ST(V_DLG_RANDOM) == 2) CB->dialogue("LCOD08", P_ARKHAN3);
    }

    CB->animation("Sar_50");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

static void trig_noop(void) { /* 0xB-byte stub slots — intentional no-op */ }

/* Op 3: run trigger idx (the DLL indexes the table without a bounds check). */
void l01_trigger(i32 idx) {
    if ((u32)idx < 42) g_trigger[idx]();
}

/* INIT's default path calls trigger 0 (sub_10001073). */
void l01_trig00(void) { trig00(); }
