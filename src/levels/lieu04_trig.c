/*
 * lieu04_trig.c — Native hand-port of the LIEU04.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT and tick live in lieu04.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu04_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table (handlers also call each other).
 * ---------------------------------------------------------------------- */
static void trig01(void);
static void trig02(void);
static void trig03(void);
static void trig05(void);
static void trig07(void);
static void trig08(void);
static void trig09(void);
static void trig10(void);
static void trig11(void);
static void trig12(void);
static void trig13(void);
static void trig15(void);
static void trig16(void);
static void trig17(void);
static void trig19(void);
static void trig41(void);
static void trig_noop(void);   /* the stub slots */

/* Trigger table (LIEU04 0x100229A0, filled at INIT with the thunks at
 * 0x10001000+..): the slots that are not listed run trig_noop.
 *    0  l04_trig00
 *    1  trig01
 *    2  trig02
 *    3  trig03
 *    5  trig05
 *    7  trig07
 *    8  trig08
 *    9  trig09
 *   10  trig10
 *   11  trig11
 *   12  trig12
 *   13  trig13
 *   15  trig15
 *   16  trig16
 *   17  trig17
 *   19  trig19
 *   41  trig41
 */
static void (*const g_trigger[42])(void) = {
    l04_trig00, trig01, trig02, trig03, trig_noop, trig05,
    trig_noop, trig07, trig08, trig09, trig10, trig11,
    trig12, trig13, trig_noop, trig15, trig16, trig17,
    trig_noop, trig19, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x100229A0]). */
void l04_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start-point presets, state reset, zones, objects, actor placement */
void l04_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(4)) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 3;
    }
    /* 0x742F */
    if (ST(ST_START_POINT(4)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xEA0) = 0;
        ST(0xBD0) = 1;
        ST(ST_LEVEL) = 3;
        ST(0xE3C) = 1;
    }
    /* 0x7489 */
    if (ST(ST_START_POINT(4)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xEA0) = 0;
        ST(0xBD0) = 2;
        ST(ST_LEVEL) = 3;
    }
    /* 0x74F2 */
    if (ST(ST_START_POINT(4)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xB40) = 3;
        ST(0xE9C) = 6;
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xEA0) = 1;
    }
    /* 0x754F */
    if (ST(ST_START_POINT(4)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xB40) = 4;
        ST(0xE9C) = 6;
        ST(0xEA0) = 0;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
        ST(ST_VIE(P_GUARDIAN25)) = 1;
    }
    /* 0x75CA */
    if (ST(ST_START_POINT(4)) == 6) {
        ST(ST_HERO) = P_ARKHAN3;
        ST(0xB40) = 3;
        ST(0xE9C) = 6;
        ST(0xEA0) = 0;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(0xAF8) = 1;
    }
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(0x74) = 0;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    ST(0x80) = 0;
    ST(0x84) = 0;
    ST(0x88) = 0;
    ST(V_EVENT15_STEP) = 0;
    ST(0x90) = 0;
    ST(0x94) = 0;
    ST(0x98) = 0;
    ST(0xB8) = 0;
    ST(0xBC) = 0;
    ST(0xD4) = 0;
    ST(0xD8) = 0;
    ST(0xDC) = 0;
    ST(0xE0) = 0;
    ST(0xF0) = 0;
    ST(0xF4) = 0;
    ST(0xF8) = 0;
    ST(0xFC) = 0;
    ST(0x100) = 0;
    ST(0x114) = 0;
    ST(V_WEAPON_WARNED) = 0;
    ST(0x4C) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_EVENT_33) = 0;
    ST(V_EVENT_34) = 0;
    ST(V_EVENT_35) = 0;
    ST(V_EVENT_36) = 0;
    ST(V_EVENT_37) = 0;
    ST(V_EVENT_38) = 0;
    ST(V_EVENT_39) = 0;
    ST(V_EVENT_41) = 0;
    CB->divers(DV_MUSIC, 1, 0);
    CB->ajoute_zone(2, -1853, -2260, 110, 140, 0);
    CB->ajoute_zone(7, -2486, -2405, 185, 270, 0);
    CB->ajoute_zone(8, -2012, -2400, 85, 60, 0);
    CB->ajoute_zone(9, -1920, -2452, 70, 115, 0);
    CB->ajoute_zone(15, -2221, -2013, 335, 355, 0);
    CB->ajoute_zone(16, -2195, -2267, 90, 120, 0);
    CB->ajoute_zone(17, -1940, -2600, 200, 160, 0);
    CB->ajoute_zone(18, -1940, -2620, 50, 50, 0);
    CB->ajoute_zone(19, -2323, -2542, 150, 100, 0);
    CB->etat_objet("IM69", -1926, 12, -2066, 0xC000, 0x4000, 0x8400, 0, -1);
    CB->vire_col(6, 0);
    CB->vire_col(0x11, 0);
    CB->vire_col(0x12, 0);
    CB->bruitage("L04AMB1", -1, -1);
    /* 0x7A0B */
    if (ST(0xBD0) == 2 && ST(0xB40) == 0) {
        ST(0xB40) = 1;
    }
    CB->pos_perso(ST(ST_HERO), -1851, -2277, 0x8461, 1, "ar", 6, 0);
    /* 0x7A69 */
    if (ST(ST_HERO) >= 2 && ST(0xB40) > 0 && ST(0xB40) < 4 && ST(ST_VIE(P_GUARDIAN2)) == VIE_DEAD) {
        CB->pos_perso(P_GUARDIAN2, CB->divers(DV_DESC_POS, P_GUARDIAN2, 0), CB->divers(DV_DESC_POS, P_GUARDIAN2, 1), CB->divers(DV_DESC_POS, P_GUARDIAN2, 2), 2, "P42cad", 0xD, 0);
    }
    /* 0x7B01 */
    if (ST(ST_HERO) >= 2 && ST(0xB40) > 0 && ST(0xB40) < 3 && ST(V_GUARD_FIGHT_DONE) == 0) {
        /* 0x7B49 */
        if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
            CB->pos_perso(P_KALHI13, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 2, "P13cad", 5, 0);
        }
        /* 0x7BAB */
        if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
            CB->pos_perso(P_KALHI33, CB->divers(DV_DESC_POS, P_KALHI33, 0), CB->divers(DV_DESC_POS, P_KALHI33, 1), CB->divers(DV_DESC_POS, P_KALHI33, 2), 2, "P33cad", 1, 0);
        }
    }
    /* 0x7C0D */
    if (ST(0xB40) == 0 && ST(V_GUARD_FIGHT_DONE) == 0) {
        ST(0xEA0) = 0;
    }
    /* 0x7C40 */
    if (ST(0xB40) == 1 && ST(V_GUARD_FIGHT_DONE) == 0) {
        ST(0xEA0) = 0;
        CB->vire_col(6, 1);
        /* 0x7C83 */
        if (ST(ST_VIE(P_GUARDIAN2)) <= 1) {
            /* 0x7C95 */
            if (ST(0xE90) == 0) {
                CB->pos_perso(P_GUARDIAN2, -1955, -2622, 0x1388, 1, "P42ARR01", -1, 0);
                CB->vire_col(3, 0);
            }
            /* 0x7CD9 */
            if (ST(0xE90) >= 1) {
                CB->pos_perso(P_GUARDIAN2, -2072, -2500, 0x7530, 1, "P42ARR03", 0xD, 0);
            }
            CB->pos_perso(P_KALHI13, -2360, -3204, 0x55F0, 1, "P13ARR03", 5, 1);
        }
        /* 0x7D35 */
        if (ST(ST_VIE(P_GUARDIAN2)) == VIE_DEAD) {
            CB->pos_perso(P_GUARDIAN2, CB->divers(DV_DESC_POS, P_GUARDIAN2, 0), CB->divers(DV_DESC_POS, P_GUARDIAN2, 1), CB->divers(DV_DESC_POS, P_GUARDIAN2, 2), 2, "P42cad", 0xD, 0);
            CB->pos_perso(P_KALHI13, -2555, -2220, 0xAFC8, 1, "P13ARR03", 5, 1);
        }
    }
    /* 0x7DBC */
    if (ST(0xB40) == 2 && ST(V_GUARD_FIGHT_DONE) == 0) {
        ST(0xEA0) = 1;
        CB->pos_perso(P_GUARDIAN2, CB->divers(DV_DESC_POS, P_GUARDIAN2, 0), CB->divers(DV_DESC_POS, P_GUARDIAN2, 1), CB->divers(DV_DESC_POS, P_GUARDIAN2, 2), 2, "P42cad", 0xD, 0);
        /* 0x7E3F */
        if ((ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1) && ST(V_KALHI13_TALK_C) == 3) {
            ST(V_EVENT15_STEP) = 1;
            ST(0x78) = 1;
            /* 0x7E9B */
            if (CB->divers(DV_DESC_POS, P_GUARDIAN2, 0) < -2200) {
                /* 0x7EAD */
                if (ST(ST_VIE(P_KALHI13)) == 1) {
                    CB->pos_perso(P_KALHI13, -1970, -2420, 0x59D8, 1, "P13ARR03", 5, 1);
                }
                /* 0x7EE4 */
                if (ST(ST_VIE(P_KALHI33)) == 1) {
                    CB->pos_perso(P_KALHI33, -1970, -2420, 0x59D8, 1, "P33ARR03", 1, 1);
                }
            }
            /* 0x7F26 */
            if (CB->divers(DV_DESC_POS, P_GUARDIAN2, 0) > -2200) {
                /* 0x7F38 */
                if (ST(ST_VIE(P_KALHI13)) == 1) {
                    CB->pos_perso(P_KALHI13, -2284, -2580, 0x9C40, 1, "P13ARR03", 5, 1);
                }
                /* 0x7F6F */
                if (ST(ST_VIE(P_KALHI33)) == 1) {
                    CB->pos_perso(P_KALHI33, -2284, -2580, 0x9C40, 1, "P33ARR03", 1, 1);
                }
            }
        }
    }
    /* 0x7FA6 */
    if (ST(0xB40) == 3 && ST(V_GUARD_FIGHT_DONE) == 0) {
        ST(0xEA0) = 1;
        CB->pos_perso(P_GUARDIAN2, CB->divers(DV_DESC_POS, P_GUARDIAN2, 0), CB->divers(DV_DESC_POS, P_GUARDIAN2, 1), CB->divers(DV_DESC_POS, P_GUARDIAN2, 2), 2, "P42cad", 0xD, 0);
    }
    /* 0x8029 */
    if (ST(0xB40) == 4) {
        /* 0x803B */
        if (ST(ST_VIE(P_GUARDIAN25)) <= 1) {
            ST(0xEA0) = 0;
            CB->pos_perso(P_GUARDIAN25, -2072, -2500, 0x7530, 1, "P93ARR02", 0xB, 1);
        }
        /* 0x8081 */
        if (ST(ST_VIE(P_GUARDIAN25)) == VIE_DEAD) {
            CB->pos_perso(P_GUARDIAN25, CB->divers(DV_DESC_POS, P_GUARDIAN25, 0), CB->divers(DV_DESC_POS, P_GUARDIAN25, 1), CB->divers(DV_DESC_POS, P_GUARDIAN25, 2), 2, "P93cad", 0xB, 0);
        }
        /* 0x80E3 */
        if (ST(ST_VIE(P_KONKALITE6)) <= 2 && ST(0xE30) <= 1) {
            ST(0xEA0) = 0;
            ST(0xE30) = 1;
            ST(ST_VIE(P_KONKALITE6)) = 1;
            CB->pos_perso(P_KONKALITE6, -2208, -2283, 0x592D, 1, "P76ARR01", 0xC, 1);
        }
        /* 0x8159 */
        if (ST(ST_VIE(P_KONKALITE6)) == VIE_DEAD && ST(0xE30) == 3) {
            ST(0xEA0) = 1;
            ST(0xF4) = 1;
            CB->pos_perso(P_KONKALITE6, CB->divers(DV_DESC_POS, P_KONKALITE6, 0), CB->divers(DV_DESC_POS, P_KONKALITE6, 1), CB->divers(DV_DESC_POS, P_KONKALITE6, 2), 2, "P76cad", 0xC, 0);
        }
    }
    CB->load_anim("204OUVER", 0);
    CB->load_anim("204FERME", 1);
    CB->load_anim("204PORTE", 2);
    CB->load_anim("204OUV2", 3);
    /* 0x8228 */
    if (ST(0xEA0) == 0) {
        CB->vire_col(0x11, 1);
        CB->ajoute_zone(3, -2187, -2452, 100, 100, 0);
    }
    /* 0x8265 */
    if (ST(0xEA0) == 1) {
        CB->vire_col(0x12, 1);
    }
    CB->divers(DV_EVENT_SOUND, 0x10001, 0xFA00000);
    CB->divers(DV_EVENT_SOUND, 0x20002, 0x13880000);
    /* 0x82B1 */
    if (ST(V_GUARD_FIGHT_DONE) == 0) {
        CB->divers(DV_EVENT_SOUND, 0x40004, 0x36B00000);
    }
    /* 0x82D8 */
    if (ST(V_GUARD_FIGHT_DONE) != 0) {
        CB->divers(DV_EVENT_SOUND, 0x30003, 0x2EE00000);
        CB->divers(DV_EVENT_SOUND, 0x40004, 0x36B00000);
        CB->divers(DV_EVENT_SOUND, 0x50005, 0x3E800000);
    }
    ST(ST_START_POINT(4)) = 0;
}

/* trigger 01 — sub_1000B459 */
static void trig01(void) {
    /* 0xB468 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 19) && !CB->direction_perso(P_KALHI33)) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0xB4F3 */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xB51E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xB549 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xB5A3 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KALHI33) && ST(ST_VIE(P_KALHI33)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L04D41", P_KALHI13, "SP13DIA59", "SP13ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xB66A */
    if (CB->direction_perso(P_KALHI33)) {
        /* 0xB679 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
            /* 0xB6A9 */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->select_pile(0, 2);
                CB->bloque(BLOQUE_ALL);
                ST(ST_SCRIPT_BUSY) = 1;
                ST(0x7C) = 0x64;
                ST(0x78) = 2;
                CB->stop_joindre(-P_KALHI33);
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                CB->joindre(ST(ST_HERO), 0, 0, 0, 0x21);
                /* 0xB738 */
                if (ST(V_KALHI13_TALK_A) == 1) {
                    /* 0xB74A */
                    if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                        /* 0xB79F */
                        if (ST(V_KALHI13_TALK_B) == 0) {
                            ST(V_KALHI13_TALK_B) = 1;
                            /* 0xB7C0 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L04D25", P_ARKHAN2, "SG00DIA07", NULL);
                            }
                            /* 0xB7E8 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L04D26", P_ARKHAN3, "SG00DIA07", NULL);
                            }
                            lieu_action_register("L04D27A", P_KALHI33, "SP33DIA56", "SP33ARR01_10");
                            lieu_action_register("L04D27B", P_KALHI33, "SP33DIA56", "SP33ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x7C), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xB881 */
                        if (ST(V_KALHI13_TALK_B) == 1) {
                            ST(V_KALHI13_TALK_B) = 2;
                            /* 0xB8A2 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L04D28A", P_ARKHAN2, "SG00DIA05", NULL);
                                lieu_action_register("L04D28B", P_ARKHAN2, "SG00DIA05", NULL);
                            }
                            /* 0xB8E0 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L04D29", P_ARKHAN3, "SG00DIA05", NULL);
                            }
                            lieu_action_register("L04D30", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x7C), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xB960 */
                        if (ST(V_KALHI13_TALK_B) == 2) {
                            /* 0xB972 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L04D31", P_ARKHAN2, "SG00DIA06", NULL);
                            }
                            /* 0xB99A */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L04D32", P_ARKHAN3, "SG00DIA06", NULL);
                            }
                            lieu_action_register("L04D33", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x7C), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                    }
                    /* 0xBA1A */
                    if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 1 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0xBA83 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L04D34", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        }
                        /* 0xBAAE */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L04D35", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                        }
                        lieu_action_register("L04D36", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x7C), 1);
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
            }
            /* 0xBB47 */
            if (ST(ST_VIE(P_KALHI33)) == VIE_DEAD) {
                CB->fouille(P_KALHI33);
            }
        }
        /* 0xBB61 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == P_KALHI33) {
            CB->bloque(BLOQUE_ALL);
            /* 0xBB9C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xBBAE */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L04D37", P_ARKHAN2);
                }
                /* 0xBBD0 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L04D38", P_ARKHAN2);
                }
            }
            /* 0xBBF2 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xBC04 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L04D39", P_ARKHAN3);
                }
                /* 0xBC26 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L04D40", P_ARKHAN3);
                }
            }
            CB->bloque(BLOQUE_NONE);
        }
        /* 0xBC4D */
        if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KALHI33)) == 1) {
            /* 0xBC7D */
            if (ST(V_PLAYER_STRIKE) == 2) {
                ST(0x7C) = 0x64;
                ST(V_PLAYER_STRIKE) = -1;
                CB->stop_joindre(-P_KALHI33);
                CB->dialogue("L04D43", P_KALHI33);
                ST(V_KALHI13_TALK_C) = 2;
                ST(V_ENGAGED_PERSONA) = P_KALHI33;
                CB->combat(P_KALHI33, 0);
                return;
            }
            CB->stop_joindre(-P_KALHI33);
            ST(V_PLAYER_STRIKE) = -1;
            CB->bloque(BLOQUE_ALL);
            ST(0x7C) = 0x64;
            CB->animation("SP33ESA15");
            CB->animation("P33_65535");
            /* 0xBD33 */
            if (ST(V_KALHI13_TALK_C) == 0) {
                ST(V_KALHI13_TALK_C) = 1;
                lieu_action_register("L04D41", P_KALHI33, "P33DIA59_10", "SP33ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x7C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xBD9B */
            if (ST(V_KALHI13_TALK_C) == 1) {
                ST(V_KALHI13_TALK_C) = 2;
                lieu_action_register("L04D42", P_KALHI33, "P33DIA59_10", "SP33ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x7C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xBE03 */
            if (ST(V_KALHI13_TALK_C) == 2) {
                lieu_action_register("L04D43", P_KALHI33, "P33GAR26", "SP33GAR02_10");
                CB->bloque(BLOQUE_NONE);
                ST(V_ENGAGED_PERSONA) = P_KALHI33;
                CB->combat(P_KALHI33, 0);
                CB->select_pile(0, 1);
                return;
            }
        }
        return;
    }
    /* 0xBE74 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
        trig02();
    }
    /* 0xBE99 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
    }
    /* 0xBEBE */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        trig07();
    }
    /* 0xBEE3 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
    }
    /* 0xBF08 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
        trig09();
    }
    /* 0xBF2D */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13)) {
        trig13();
    }
    /* 0xBF52 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 14)) {
        trig_noop();   /* trig14: empty stub (0x1000ED5A) */
    }
    /* 0xBF77 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 15)) {
        trig15();
    }
    /* 0xBF9C */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 16)) {
        trig16();
    }
    /* 0xBFC1 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 19)) {
        trig19();
    }
}

/* trigger 02 — sub_1000BFD1 */
static void trig02(void) {
    /* 0xBFE0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(V_EVENT_34) = 0;
        CB->evenement(0x22, -1);
        /* 0xC044 */
        if (ST(0x6C) == 1) {
            /* 0xC056 */
            if (ST(ST_VIE(P_GUARDIAN2)) == 1) {
                /* 0xC068 */
                if (ST(0xE90) == 0) {
                    ST(0xE90) = 1;
                }
                CB->change_camera(1, CAM_LOCKED);
                CB->stop_joindre(-P_GUARDIAN2);
                CB->combat(P_GUARDIAN2, -1);
                CB->animation("SP42GAR27_10");
                CB->animation("P42_65535");
                lieu_action_register("L04D21", P_GUARDIAN2, "P42DIA59", "SP42ARR03_10");
            }
            /* 0xC0E3 */
            if (ST(ST_VIE(P_GUARDIAN25)) == 1) {
                CB->change_camera(1, CAM_LOCKED);
                CB->stop_joindre(-P_GUARDIAN25);
                CB->combat(P_GUARDIAN25, -1);
                CB->animation("SP93GAR27");
                CB->animation("P93_65535");
                lieu_action_register("L04D22", P_GUARDIAN25, "P93DIA53", "SP93ARR02_10");
            }
        }
        CB->change_camera(99, CAM_AUTO);
        CB->animation("SG00OUV01");
        CB->animation("P00_65535");
        CB->anim_objet("IM69", 3, -60, 0x5DC, 0);
        CB->bloque(BLOQUE_NONE);
        ST(ST_LEVEL) = 4;
        /* 0xC1A2 */
        if (ST(0xB40) == 2 && ST(V_KALHI13_TALK_C) < 3) {
            ST(0xB40) = 3;
        }
        CB->divers_empile(DV_MUSIC, 1, 4);
        CB->change_piece(3, 0);
    }
}

/* trigger 03 — sub_1000C1ED */
static void trig03(void) {
    /* 0xC20D */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13) && CB->direction_perso(P_GUARDIAN2)) {
        trig13();
    }
    /* 0xC245 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && CB->direction_perso(P_GUARDIAN25)) {
        trig11();
        return;
    }
    /* 0xC274 */
    if (ST(0xEA0) == 0 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x1F40)) {
        CB->pos_perso(ST(ST_HERO), -2158, -2451, 0x286D, 1, "ar", 6, 0);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xC2F9 */
        if (ST(ST_PLAYER_WEAPON) == 0x59) {
            CB->vire_col(0x11, 0);
            CB->vire_col(0x12, 1);
            ST(V_EVENT_16) = 0;
            ST(0xEA0) = 1;
            CB->animation("SG00OUV15");
            CB->evenement(0x10, 1200);
            return;
        }
        /* 0xC366 */
        if (ST(ST_PLAYER_WEAPON) == 0xE || ST(ST_PLAYER_WEAPON) == 0xF || ST(ST_PLAYER_WEAPON) == 0x10) {
            CB->animation("SG00OUV14");
            CB->animation("ar_10");
            /* 0xC3B2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L04D44", P_ARKHAN2);
            }
            /* 0xC3D4 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L04D45", P_ARKHAN3);
            }
            /* 0xC3F6 */
            if (ST(0xB40) == 1) {
                CB->dialogue("L04D46", P_KALHI13);
            }
            /* 0xC418 */
            if (ST(0xE30) == 1) {
                CB->dialogue("L04D47A", P_KONKALITE6);
                CB->dialogue("L04D47C", P_KONKALITE6);
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        CB->animation("SG00OUV12");
        CB->animation("ar_10");
        /* 0xC48E */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD12", P_ARKHAN1);
        }
        /* 0xC4B0 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD13", P_ARKHAN2);
        }
        /* 0xC4D2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD14", P_ARKHAN3);
        }
        /* 0xC4F4 */
        if (ST(0xB40) == 1) {
            CB->dialogue("L04D46", P_KALHI13);
        }
        /* 0xC516 */
        if (ST(0xE30) == 1) {
            CB->dialogue("L04D47A", P_KONKALITE6);
            CB->dialogue("L04D47C", P_KONKALITE6);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 05 — sub_1000C575 */
static void trig05(void) {
    /* 0xC584 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 19) && !CB->direction_perso(P_KALHI13)) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0xC60F */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xC63A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xC665 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xC6BC */
    if (ST(ST_UPDOWN_KEY) == 1 && ST(0xB8) == 0xA && CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        trig07();
        return;
    }
    /* 0xC70A */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KALHI13) && ST(ST_VIE(P_KALHI13)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L04D41", P_KALHI13, "SP13DIA59", "SP13ARR01_10");
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xC7D1 */
    if (CB->direction_perso(P_KALHI13)) {
        /* 0xC7E0 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
            /* 0xC810 */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->select_pile(0, 2);
                CB->bloque(BLOQUE_ALL);
                ST(0x7C) = 0x64;
                ST(0x78) = 2;
                ST(ST_SCRIPT_BUSY) = 1;
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                CB->joindre(ST(ST_HERO), 0, 0, 0, 0xD);
                /* 0xC894 */
                if (ST(V_KALHI13_TALK_A) == 0) {
                    ST(V_KALHI13_TALK_A) = 1;
                    /* 0xC8B5 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L04D51A", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0xC8DD */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L04D52A", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L04D53A", P_KALHI13, "SP13DIA50", "SP13ARR01_10");
                    /* 0xC91E */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L04D51B", P_ARKHAN2, "SG00DIA02", NULL);
                        lieu_action_register("L04D51C", P_ARKHAN2, "SG00DIA02", NULL);
                    }
                    /* 0xC95C */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L04D52B", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L04D53B", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                    lieu_action_register("L04D53C", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                    /* 0xC9B6 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        /* 0xC9C8 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L04D54", P_ARKHAN2, "SG00DIA04", NULL);
                        }
                        /* 0xC9F0 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L04D55", P_ARKHAN3, "SG00DIA04", NULL);
                        }
                        lieu_action_register("L04D56A", P_KALHI13, "SP13DIA55", "SP13ARR01_10");
                        lieu_action_register("L04D56B", P_KALHI13, "SP13DIA56", "SP13ARR01_10");
                    }
                    /* 0xCA4A */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        /* 0xCA5C */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L04D57", P_ARKHAN2, "SG00DIA04", NULL);
                        }
                        /* 0xCA84 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L04D58", P_ARKHAN3, "SG00DIA04", NULL);
                        }
                        lieu_action_register("L04D59", P_KALHI13, "SP13DIA59", "SP13ARR01_10");
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->modify_var(&ST(0x7C), 1);
                    CB->bloque(BLOQUE_NONE);
                    CB->select_pile(0, 1);
                    return;
                }
                /* 0xCB04 */
                if (ST(V_KALHI13_TALK_A) == 1) {
                    /* 0xCB16 */
                    if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                        /* 0xCB6B */
                        if (ST(V_KALHI13_TALK_B) == 0) {
                            ST(V_KALHI13_TALK_B) = 1;
                            /* 0xCB8C */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L04D25", P_ARKHAN2, "SG00DIA07", NULL);
                            }
                            /* 0xCBB4 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L04D26", P_ARKHAN3, "SG00DIA07", NULL);
                            }
                            lieu_action_register("L04D27A", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
                            lieu_action_register("L04D27B", P_KALHI13, "SP13DIA61", "SP13ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x7C), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xCC4D */
                        if (ST(V_KALHI13_TALK_B) == 1) {
                            ST(V_KALHI13_TALK_B) = 2;
                            /* 0xCC6E */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L04D63A", P_ARKHAN2, "SG00DIA05", NULL);
                                lieu_action_register("L04D63B", P_ARKHAN2, "SG00DIA06", NULL);
                            }
                            /* 0xCCAC */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L04D64A", P_ARKHAN3, "SG00DIA05", NULL);
                                lieu_action_register("L04D64B", P_ARKHAN3, "SG00DIA05", NULL);
                            }
                            lieu_action_register("L04D65", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x7C), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                        /* 0xCD42 */
                        if (ST(V_KALHI13_TALK_B) == 2) {
                            /* 0xCD54 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L04D66", P_ARKHAN2, "SG00DIA05", NULL);
                            }
                            /* 0xCD7C */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L04D67", P_ARKHAN3, "SG00DIA05", NULL);
                            }
                            lieu_action_register("L04D68", P_KALHI13, "SP13DIA65", "SP13ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x7C), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->select_pile(0, 1);
                            return;
                        }
                    }
                    /* 0xCDFC */
                    if (ST(ST_PLAYER_WEAPON) == 1) {
                        /* 0xCE0E */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L04D69A", P_ARKHAN2, "SG00DIA02", NULL);
                            CB->dialogue("L04D69B", P_ARKHAN2);
                        }
                        /* 0xCE46 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->dialogue("L04D70", P_ARKHAN3);
                        }
                        lieu_pre_scene(ST(ST_HERO), P_KALHI13, 1, 6, 5);
                        CB->dialogue("L04D71", P_KALHI13);
                        CB->modify_var(&ST(0x74), 1);
                        return;
                    }
                    /* 0xCEAC */
                    if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 1 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0xCF15 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L04D34", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        }
                        /* 0xCF40 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L04D35", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                        }
                        lieu_action_register("L04D36", P_KALHI13, "SP13DIA64", "SP13ARR01_10");
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x7C), 1);
                        CB->bloque(BLOQUE_NONE);
                        CB->select_pile(0, 1);
                        return;
                    }
                }
            }
            /* 0xCFD9 */
            if (ST(ST_VIE(P_KALHI13)) == VIE_DEAD) {
                CB->fouille(P_KALHI13);
            }
        }
        /* 0xCFF3 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == P_KALHI13) {
            /* 0xD014 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xD026 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L04D37", P_ARKHAN2);
                }
                /* 0xD048 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L04D38", P_ARKHAN2);
                }
            }
            /* 0xD06A */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xD07C */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L04D39", P_ARKHAN3);
                }
                /* 0xD09E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L04D40", P_ARKHAN3);
                }
            }
        }
        /* 0xD0BD */
        if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KALHI13)) == 1) {
            CB->select_pile(0, 2);
            /* 0xD0FA */
            if (ST(V_PLAYER_STRIKE) == 2) {
                ST(0x7C) = 0x64;
                ST(V_PLAYER_STRIKE) = -1;
                CB->stop_joindre(-P_KALHI13);
                CB->dialogue("L04D43", P_KALHI13);
                ST(V_KALHI13_TALK_C) = 2;
                ST(V_ENGAGED_PERSONA) = P_KALHI13;
                CB->combat(P_KALHI13, 0);
                return;
            }
            CB->stop_joindre(-P_KALHI13);
            ST(0x7C) = 0x64;
            CB->bloque(BLOQUE_ALL);
            ST(V_PLAYER_STRIKE) = -1;
            CB->animation("SP13ESA15");
            CB->animation("P13_65535");
            /* 0xD1B0 */
            if (ST(V_KALHI13_TALK_C) == 0) {
                ST(V_KALHI13_TALK_C) = 1;
                lieu_action_register("L04D41", P_KALHI13, "P13DIA59_10", "SP13ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x7C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xD218 */
            if (ST(V_KALHI13_TALK_C) == 1) {
                ST(V_KALHI13_TALK_C) = 2;
                lieu_action_register("L04D42", P_KALHI13, "P13DIA59_10", "SP13ARR01_10");
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x7C), 1);
                CB->select_pile(0, 1);
                return;
            }
            /* 0xD280 */
            if (ST(V_KALHI13_TALK_C) == 2) {
                lieu_action_register("L04D43", P_KALHI13, "P13GAR26_10", "SP13GAR02_10");
                CB->bloque(BLOQUE_NONE);
                CB->combat(P_KALHI13, 0);
                ST(V_ENGAGED_PERSONA) = P_KALHI13;
                CB->select_pile(0, 1);
                return;
            }
        }
        return;
    }
    /* 0xD2F1 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 2)) {
        trig02();
    }
    /* 0xD316 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
    }
    /* 0xD33B */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        trig07();
    }
    /* 0xD360 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
    }
    /* 0xD385 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
        trig09();
    }
    /* 0xD3AA */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13)) {
        trig13();
    }
    /* 0xD3CF */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 14)) {
        trig_noop();   /* trig14: empty stub (0x1000ED5A) */
    }
    /* 0xD3F4 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 15)) {
        trig15();
    }
    /* 0xD419 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 16)) {
        trig16();
    }
    /* 0xD43E */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 19)) {
        trig19();
    }
}

/* trigger 07 — sub_1000D459 */
static void trig07(void) {
    /* 0xD468 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x88B8) && ST(0xB8) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD4D4 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xD4E6 */
            if (ST(0x5C4) == 0 || ST(0x5C4) == 2) {
                /* 0xD512 */
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    /* 0xD524 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ACT85");
                    }
                    /* 0xD541 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ACT85");
                    }
                }
                /* 0xD566 */
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    /* 0xD578 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ACT86");
                    }
                    /* 0xD595 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ACT86");
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xD5C3 */
                if (ST(0x5C4) == 0 && ST(0xE30) > 1) {
                    ST(0x5C4) = 1;
                    CB->bruitage("L04B01", -2488, -2338);
                }
                CB->animation("ar_20");
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xD623 */
            if (ST(0x5C4) == 1) {
                ST(0x5C4) = 2;
                CB->animation("SG00PRE08");
                CB->animation("P00_65535");
                ST(0xED8) = 1;
                /* 0xD669 */
                if (ST(0xED0) == 5) {
                    ST(0xED0) = 5;
                }
                /* 0xD68A */
                if (ST(0xED0) == 2) {
                    ST(0xED0) = 3;
                }
                /* 0xD6AB */
                if (ST(0xED0) == 1) {
                    ST(0xED0) = 2;
                }
                /* 0xD6CC */
                if (ST(0xED0) == 0) {
                    ST(0xED0) = 1;
                }
                /* 0xD6ED */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L04D72A", P_ARKHAN2, "G00DIA01", NULL);
                    lieu_action_register("L04D72B", P_ARKHAN2, "SG00DIA04", NULL);
                }
                /* 0xD72B */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L04D73A", P_ARKHAN3, "G00DIA01", NULL);
                    lieu_action_register("L04D73B", P_ARKHAN3, "SG00DIA04", NULL);
                }
                CB->etat_objet("OB19", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xD7A5 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xD7B7 */
            if (ST(0x5C4) == 1 && ST(0xE30) > 1) {
                ST(0x5C4) = 2;
                CB->animation("SG00PRE08");
                CB->animation("P00_65535");
                ST(0xED8) = 1;
                /* 0xD80F */
                if (ST(0xED0) == 5) {
                    ST(0xED0) = 5;
                }
                /* 0xD830 */
                if (ST(0xED0) == 2) {
                    ST(0xED0) = 3;
                }
                /* 0xD851 */
                if (ST(0xED0) == 1) {
                    ST(0xED0) = 2;
                }
                /* 0xD872 */
                if (ST(0xED0) == 0) {
                    ST(0xED0) = 1;
                }
                /* 0xD893 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L04D72A", P_ARKHAN2, "G00DIA01", NULL);
                    lieu_action_register("L04D72B", P_ARKHAN2, "SG00DIA04", NULL);
                }
                /* 0xD8D1 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L04D73A", P_ARKHAN3, "G00DIA01", NULL);
                    lieu_action_register("L04D73B", P_ARKHAN3, "SG00DIA04", NULL);
                }
                CB->etat_objet("OB19", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xD94B */
            if (ST(0x5C4) == 0 || ST(0x5C4) == 2) {
                ST(0xFC) = 1;
                CB->vire_col(0xC, 0);
                CB->joindre(ST(ST_HERO), -2488, -2338, 0, -1);
                CB->animation("ar");
                CB->pos_perso(ST(ST_HERO), -2488, -2338, 1, 1, "ar", 6, 0);
                CB->retire_zone(7);
                CB->ajoute_zone(7, -2486, -2405, 185, 270, 1);
                /* 0xDA09 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xDA23 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02LEV04");
                    }
                    /* 0xDA48 */
                    if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP02LEV03");
                    }
                    CB->animation("G02ARR16");
                }
                /* 0xDA70 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xDA8A */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03LEV04");
                    }
                    /* 0xDAAF */
                    if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("SP03LEV03");
                    }
                    CB->animation("G03ARR16");
                }
                /* 0xDAD7 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xDAF1 */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("P02LEV05");
                    }
                    /* 0xDB16 */
                    if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("P02LEV01");
                    }
                }
                /* 0xDB33 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xDB4D */
                    if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("P03LEV05");
                    }
                    /* 0xDB72 */
                    if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                        CB->animation("P03LEV01");
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar_10");
                CB->modify_var(&ST(0xFC), 0);
                CB->vire_col(0xC, 1);
                CB->ajoute_zone(7, -2486, -2405, 185, 270, 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 08 — sub_1000DBF6 */
static void trig08(void) {
    /* 0xDC16 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13) && CB->direction_perso(P_GUARDIAN2)) {
        trig13();
    }
    /* 0xDC4E */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && CB->direction_perso(P_GUARDIAN25)) {
        trig11();
        return;
    }
    /* 0xDC7A */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDCC2 */
        if (ST(ST_PLAYER_WEAPON) == 0xE || ST(ST_PLAYER_WEAPON) == 0xF || ST(ST_PLAYER_WEAPON) == 0x10 || ST(ST_PLAYER_WEAPON) == 0x59) {
            CB->animation("SG00OUV14");
            CB->animation("ar_10");
            /* 0xDD20 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L04D44", P_ARKHAN2);
            }
            /* 0xDD42 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L04D45", P_ARKHAN3);
            }
            CB->animation("Sar_10");
        } else {
            CB->animation("SG00OUV12");
            CB->animation("ar_10");
            /* 0xDD8A */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("LCOD12", P_ARKHAN1);
            }
            /* 0xDDAC */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD13", P_ARKHAN2);
            }
            /* 0xDDCE */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD14", P_ARKHAN3);
            }
            CB->animation("Sar_10");
        }
        /* 0xDDFB */
        if (ST(0xB40) != 0) {
            /* 0xDE0D */
            if (ST(ST_VIE(P_GUARDIAN25)) != VIE_DEAD) {
                CB->dialogue("L04D222", -1);
            }
            /* 0xDE2F */
            if (ST(ST_VIE(P_GUARDIAN25)) == VIE_DEAD) {
                CB->dialogue("L04D223", -1);
                CB->dialogue("L04D224", -1);
                CB->dialogue("L04D225", -1);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 09 — sub_1000DE88 */
static void trig09(void) {
    /* 0xDEA8 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13) && CB->direction_perso(P_GUARDIAN2)) {
        trig13();
    }
    /* 0xDEE0 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && CB->direction_perso(P_GUARDIAN25)) {
        trig11();
        return;
    }
    /* 0xDF0C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xAFC8)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDF57 */
        if (ST(ST_PLAYER_WEAPON) == 0xE || ST(ST_PLAYER_WEAPON) == 0xF || ST(ST_PLAYER_WEAPON) == 0x10 || ST(ST_PLAYER_WEAPON) == 0x59) {
            CB->animation("SG00OUV14");
            CB->animation("ar_10");
            /* 0xDFB5 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L04D44", P_ARKHAN2);
            }
            /* 0xDFD7 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L04D45", P_ARKHAN3);
            }
            CB->animation("Sar_10");
        } else {
            CB->animation("SG00OUV13");
            CB->animation("ar_10");
            /* 0xE01F */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("LCOD12", P_ARKHAN1);
            }
            /* 0xE041 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD13", P_ARKHAN2);
            }
            /* 0xE063 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD14", P_ARKHAN3);
            }
            CB->animation("Sar_10");
        }
        /* 0xE090 */
        if (ST(0xB40) != 0) {
            /* 0xE0A2 */
            if (ST(ST_VIE(P_GUARDIAN25)) != VIE_DEAD && ST(ST_HERO) > 1) {
                CB->dialogue("L04D226A", -1);
                CB->dialogue("L04D226B", -1);
                CB->dialogue("L04D226C", -1);
            }
            /* 0xE0F6 */
            if (ST(ST_VIE(P_GUARDIAN25)) == VIE_DEAD) {
                CB->dialogue("L04D227", -1);
                CB->dialogue("L04D228", -1);
                CB->dialogue("L04D229", -1);
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 10 — sub_1000E14F */
static void trig10(void) {
    /* 0xE15E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x9C40)) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), -2277, -2361, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        CB->load_anim("204PORT2", 4);
        CB->retire_zone(10);
        CB->vire_col(0x11, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0xEA0) = 1;
        CB->evenement(2, 500);
        /* 0xE23E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02OUV16");
        }
        /* 0xE25B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03OUV16");
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 11 — sub_1000E29A */
static void trig11(void) {
    /* 0xE2A9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 19) && !CB->direction_perso(P_GUARDIAN25)) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0xE334 */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xE35F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xE38A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xE3E4 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_GUARDIAN25) && ST(ST_VIE(P_GUARDIAN25)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L04D17", P_GUARDIAN25);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xE492 */
    if (ST(ST_HERO) != 4 && CB->direction_perso(P_GUARDIAN25)) {
        /* 0xE4B4 */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_GUARDIAN25)) == VIE_DEAD) {
            CB->select_pile(0, 2);
            CB->fouille(P_GUARDIAN25);
            CB->select_pile(0, 1);
        }
        /* 0xE518 */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_GUARDIAN25)) == 1 && CB->direction_perso(P_GUARDIAN25) && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) < -200) {
            CB->bloque(BLOQUE_ALL);
            /* 0xE58C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xE59E */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L04D80", P_ARKHAN2);
                }
                /* 0xE5C0 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L04D81", P_ARKHAN2);
                }
            }
            /* 0xE5E2 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xE5F4 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("LCOD21", P_ARKHAN3);
                }
                /* 0xE616 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("LCOD22D", P_ARKHAN3);
                }
            }
            CB->bloque(BLOQUE_NONE);
        }
        return;
    }
    /* 0xE653 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
    }
    /* 0xE678 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
    }
    /* 0xE69D */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
        trig09();
    }
}

/* trigger 12 — sub_1000E6AD */
static void trig12(void) {
    /* 0xE6BF */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_KONKALITE6) && ST(ST_VIE(P_KONKALITE6)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L04D88", P_KONKALITE6);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xE76E */
    if (CB->direction_perso(P_KONKALITE6)) {
        /* 0xE77D */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
            /* 0xE7AD */
            if (ST(ST_VIE(P_KONKALITE6)) < 2) {
                ST(ST_SCRIPT_BUSY) = 1;
                /* 0xE7CB */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    /* 0xE7DD */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("L04D84", P_ARKHAN2);
                    }
                    /* 0xE7FF */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("L04D82", P_ARKHAN3);
                    }
                }
                /* 0xE821 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    /* 0xE833 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("L04D86", P_ARKHAN2);
                    }
                    /* 0xE855 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("L04D83", P_ARKHAN3);
                    }
                }
                CB->dialogue("L04D88", P_KONKALITE6);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            }
            /* 0xE898 */
            if (ST(ST_VIE(P_KONKALITE6)) == VIE_DEAD) {
                CB->select_pile(0, 2);
                CB->fouille(P_KONKALITE6);
                CB->select_pile(0, 1);
            }
        }
        return;
    }
    /* 0xE8E2 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        trig07();
    }
    /* 0xE907 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 10)) {
        trig10();
    }
    /* 0xE92C */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 15)) {
        trig15();
    }
    /* 0xE951 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 16)) {
        trig16();
    }
}

/* trigger 13 — sub_1000E961 */
static void trig13(void) {
    /* 0xE970 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 19) && !CB->direction_perso(P_GUARDIAN2)) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0xE9FB */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xEA26 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xEA51 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xEAAB */
    if (ST(ST_HERO) != 4 && CB->direction_perso(P_GUARDIAN2)) {
        /* 0xEACD */
        if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_GUARDIAN2)) == VIE_DEAD) {
            /* 0xEB0F */
            if (ST(0xB40) < 2 || ST(ST_VIE(P_KALHI33)) == 1) {
                CB->select_pile(0, 2);
                CB->fouille(P_GUARDIAN2);
                CB->select_pile(0, 1);
            }
            /* 0xEB58 */
            if (ST(0xB40) >= 2 && ST(ST_VIE(P_KALHI33)) != 1) {
                CB->bloque(BLOQUE_ALL);
                CB->fouille(P_GUARDIAN2);
                /* 0xEB92 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L04D99", P_ARKHAN2);
                }
                /* 0xEBB4 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L04D100", P_ARKHAN3);
                }
                CB->bloque(BLOQUE_NONE);
            }
        }
        /* 0xEBDB */
        if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == P_GUARDIAN2 && CB->direction_perso(P_GUARDIAN2) && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) < -200) {
            /* 0xEC41 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xEC53 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L04D80", P_ARKHAN2);
                }
                /* 0xEC75 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L04D81", P_ARKHAN2);
                }
            }
            /* 0xEC97 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xECA9 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L04D82", P_ARKHAN3);
                }
                /* 0xECCB */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L04D83", P_ARKHAN3);
                }
            }
        }
        return;
    }
    /* 0xED00 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) {
        trig03();
    }
    /* 0xED25 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
    }
    /* 0xED4A */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9)) {
        trig09();
    }
}

/* trigger 15 — sub_1000ED65 */
static void trig15(void) {
    /* 0xED74 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xEA60)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xEDCE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L04D95A", P_ARKHAN2, "SG00DIA04", NULL);
            lieu_action_register("L04D95B", P_ARKHAN2, "SG00DIA04", NULL);
        }
        /* 0xEE0C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L04D96", P_ARKHAN3, "SG00DIA04", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 — sub_1000EE4B */
static void trig16(void) {
    /* 0xEE5A */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC350)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xEEB4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L04D97", P_ARKHAN2, "SG00PRE02", NULL);
        }
        /* 0xEEDC */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L04D98", P_ARKHAN3, "SG00PRE02", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 17 — sub_1000EF1B */
static void trig17(void) {
    /* 0xEF3B */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13) && CB->direction_perso(P_GUARDIAN2)) {
        trig13();
    }
    /* 0xEF73 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && CB->direction_perso(P_GUARDIAN25)) {
        trig11();
        return;
    }
    /* 0xEF9F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(18) && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) < -200 && ST(ST_HERO) != 4) {
        CB->bloque(BLOQUE_ALL);
        CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xF0A6 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("LCOD28", P_ARKHAN1, "SG00DIA01", NULL);
        }
        /* 0xF0CE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG00DIA01", NULL);
        }
        /* 0xF0F6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG00DIA01", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 19 — sub_1000F140 */
static void trig19(void) {
    /* 0xF160 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 13) && CB->direction_perso(P_GUARDIAN2)) {
        trig13();
    }
    /* 0xF198 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 11) && CB->direction_perso(P_GUARDIAN25)) {
        trig11();
        return;
    }
    /* 0xF1C4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0)) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0xF21C */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xF247 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xF272 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xF2C9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x4000)) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xF330 */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD75", P_ARKHAN1, "SG00ATT01", NULL);
        }
        /* 0xF358 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD76", P_ARKHAN2, "SG00ATT01", NULL);
        }
        /* 0xF380 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD77", P_ARKHAN3, "SG00ATT01", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
}

/* trigger 41 — sub_10008331 */
static void trig41(void) {
    CB->divers_empile(DV_MUSIC, 1, 1);
    /* 0x8355 */
    if (ST(0xB40) == 1) {
        CB->evenement(0x22, 2500);
    }
    /* 0x8377 */
    if (ST(0xB40) == 4) {
        CB->evenement(0x21, 2000);
    }
}
