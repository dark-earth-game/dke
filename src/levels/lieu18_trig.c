/*
 * lieu18_trig.c — Native hand-port of the LIEU18.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu18.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdio.h>
#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu18_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig01(void); static void trig02(void); static void trig03(void);
static void trig04(void); static void trig07(void); static void trig08(void);
static void trig09(void); static void trig10(void); static void trig11(void);
static void trig12(void); static void trig13(void); static void trig14(void);
static void trig15(void); static void trig16(void); static void trig18(void);
static void trig20(void); static void trig22(void); static void trig25(void);
static void trig41(void);
static void trig_noop(void);   /* the 24 stub slots */

/* dword_1002AC70[42] — filled at INIT with the 5-byte jmp thunks at
 * 0x10001000+5*i; static here.
 *   idx handler        idx handler        idx handler
 *    0  10007400       14  100129E2       28  100136C7 stub
 *    1  1000C759       15  100131C2       29  100136D2 stub
 *    2  1000C7C5       16  10012C37       30  100136DD stub
 *    3  1000CE5A       17  100131B7 stub  31  100136E8 stub
 *    4  1000DB12       18  10012ADD       32  100136F3 stub
 *    5  10011A8E stub  19  10013254 stub  33  100136FE stub
 *    6  10011A99 stub  20  1001325F       34  10013709 stub
 *    7  10011AA4       21  1001339C stub  35  10013714 stub
 *    8  10011CB1       22  100133A7       36  1001371F stub
 *    9  10011E91       23  1001357C stub  37  1001372A stub
 *   10  10011F46       24  10013587 stub  38  10013735 stub
 *   11  10012450       25  10013592       39  10013740 stub
 *   12  10012625       26  100136B1 stub  40  1001374B stub
 *   13  100127FA       27  100136BC stub  41  1000817B
 */
static void (*const g_trigger[42])(void) = {
    l18_trig00, trig01, trig02, trig03, trig04, trig_noop, trig_noop, trig07,
    trig08, trig09, trig10, trig11, trig12, trig13, trig14, trig15,
    trig16, trig_noop, trig18, trig_noop, trig20, trig_noop, trig22, trig_noop,
    trig_noop, trig25, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1002AC70]). */
void l18_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_10007400 (thunk sub_10001078; INIT default) — level setup */
void l18_trig00(void) {
    /* 0x7412 */
    if (ST(ST_START_POINT(18)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 1;
    }
    /* 0x744E */
    if (ST(ST_START_POINT(18)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 1;
        ST(0xDB0) = 0;
        ST(ST_VIE(P_SORDOS)) = VIE_DEAD;
    }
    /* 0x74A8 */
    if (ST(ST_START_POINT(18)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 1;
    }
    /* 0x74F3 */
    if (ST(ST_START_POINT(18)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 2;
    }
    /* 0x753E */
    if (ST(ST_START_POINT(18)) == 5) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 4;
        ST(ST_VIE(P_DHORKAN11)) = 1;
    }
    /* 0x7598 */
    if (ST(ST_START_POINT(18)) == 6) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 4;
        ST(ST_VIE(P_DHORKAN11)) = VIE_DEAD;
    }
    /* 0x75F2 */
    if (ST(ST_START_POINT(18)) == 7) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 7;
        ST(0xDC8) = 0;
        ST(V_GUARD_FIGHT_DONE) = 0;
    }
    /* 0x765B */
    if (ST(ST_START_POINT(18)) == 8) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 7;
        ST(0xDC8) = 0;
        ST(V_GUARD_FIGHT_DONE) = 1;
    }
    /* 0x76C4 */
    if (ST(ST_START_POINT(18)) == 9) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 7;
        ST(0xDC8) = 1;
        ST(0xDCC) = 0;
        ST(0xB10) = 0;
    }
    /* 0x773C */
    if (ST(ST_START_POINT(18)) == 10) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 7;
        ST(0xDC8) = 1;
        ST(0xDCC) = 0;
        ST(0xB10) = 1;
    }
    /* 0x77B4 */
    if (ST(ST_START_POINT(18)) == 11) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 7;
        ST(0xDC8) = 1;
        ST(0xDCC) = 1;
        ST(0xC14) = 2;
        ST(0xDD0) = 0;
    }
    /* 0x783B */
    if (ST(ST_START_POINT(18)) == 12) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 2;
        ST(0xDB0) = 7;
        ST(0xDC8) = 1;
        ST(0xDCC) = 1;
        ST(0xC28) = 1;
    }
    /* 0x78B3 */
    if (ST(ST_START_POINT(18)) == 13) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 48;
        ST(0xBC8) = 0;
        ST(0xC28) = 1;
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
    ST(0x50) = 0;
    ST(0x54) = 0;
    ST(0x58) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_EVENT_33) = 0;
    ST(V_EVENT_34) = 0;
    ST(V_EVENT_35) = 0;
    ST(V_EVENT_36) = 0;
    ST(V_EVENT_37) = 0;
    ST(V_EVENT_38) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    /* 0x7B68 */
    if (ST(0xC0C) == 100) {
        ST(0x84) = 2;
        ST(0xC0C) = 2;
    }
    /* 0x7B98 */
    if (ST(0x4DC) == 1) {
        /* 0x7BAA */
        if (ST(0xF78) == 0) {
            ST(0xF24) = 1;
        }
        ST(0x4DC) = 2;
    }
    /* 0x7BDA */
    if (ST(0x4DC) == 3) {
        ST(0x4DC) = 2;
        /* 0x7BFB */
        if (ST(0xF78) == 0) {
            ST(0xF24) = 1;
        }
    }
    CB->vire_col(11, 0);
    CB->ajoute_zone(1, 2400, 2115, 140, 120, 1);
    CB->ajoute_zone(5, 2025, 1375, 50, 50, 0);
    CB->ajoute_zone(10, 2000, 1375, 250, 250, 0);
    CB->ajoute_zone(11, 1805, 1590, 180, 140, 0);
    CB->ajoute_zone(12, 1900, 1730, 180, 140, 0);
    CB->ajoute_zone(14, 2200, 1410, 80, 200, 0);
    CB->ajoute_zone(15, 2450, 1460, 100, 100, 0);
    CB->ajoute_zone(18, 2770, 1555, 150, 150, 0);
    CB->ajoute_zone(19, 2825, 1550, 30, 30, 0);
    CB->ajoute_zone(20, 2350, 1446, 120, 165, 0);
    CB->ajoute_zone(21, 2360, 1400, 40, 40, 0);
    CB->ajoute_zone(22, 2610, 1460, 195, 100, 0);
    CB->ajoute_zone(23, 1773, 1593, 50, 50, 0);
    CB->ajoute_zone(24, 1813, 1723, 50, 50, 0);
    CB->ajoute_zone(25, 2111, 1576, 165, 127, 0);
    CB->ajoute_zone(35, 1975, 1975, 1, 1, 0);
    CB->ajoute_zone(16, 2805, 1950, 80, 140, 0);
    CB->bruitage("L18AMB1", -1, -1);
    CB->pos_perso(ST(ST_HERO), 2343, 1984, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
    /* 0x7E6F */
    if (ST(ST_VIE(P_ARMAL)) < 2) {
        CB->ajoute_zone(2, 1975, 1975, 90, 90, 1);
        /* 0x7E9C */
        if (ST(ST_VIE(P_MALROT7)) == VIE_DEAD && ST(0xC28) == 1) {
            ST(0xC28) = 0;
        }
        /* 0x7ECF */
        if (ST(0xC28) != 1) {
            CB->pos_perso(P_ARMAL, 2117, 2000, 0x4650, 1, "P06ATT01", 4, 0);
            CB->pos_perso(P_MALROT2, 2825, 1880, 0x4A38, 1, "P62ARR01", 3, 0);
        }
        /* 0x7F2B */
        if (ST(0xC28) == 1) {
            ST(0xC28) = 2;
            CB->change_camera(1, CAM_LOCKED);
            CB->pos_perso(P_ARMAL, 2117, 2000, 0x61A8, 1, "P06ARR01", 4, 0);
            CB->pos_perso(P_MALROT6, 2020, 1850, 0xEA60, 1, "P66ARR01", 0x11, 0);
        }
    }
    /* 0x7FA6 */
    if (ST(ST_VIE(P_ARMAL)) == VIE_DEAD) {
        CB->pos_perso(P_ARMAL, CB->divers(DV_DESC_POS, P_ARMAL, 0), CB->divers(DV_DESC_POS, P_ARMAL, 1), CB->divers(DV_DESC_POS, P_ARMAL, 2), 2, "P09cad", 4, 0);
        CB->pos_perso(P_MALROT2, CB->divers(DV_DESC_POS, P_MALROT2, 0), CB->divers(DV_DESC_POS, P_MALROT2, 1), CB->divers(DV_DESC_POS, P_MALROT2, 2), 2, "P62cad", 3, 0);
        CB->ajoute_zone(7, 2804, 1983, 120, 120, 0);
        CB->ajoute_zone(8, 2117, 2100, 120, 120, 0);
    }
    CB->load_anim("218POR1", 0);
    CB->load_anim("218POR2", 1);
    CB->load_anim("218POR3", 2);
    CB->load_anim("218FIX1", 3);
    CB->load_anim("218FER3", 4);
    CB->load_anim("218FER1", 5);
    CB->load_anim("218FER2", 6);
    CB->load_anim("218IMPA2", 7);
    CB->load_anim("218FRED", 8);
    CB->load_anim("218FIX2", 9);
    /* 0x812B */
    if (ST(0xBC8) < 2) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x814F */
    if (ST(0xBC8) >= 2) {
        CB->divers(DV_MUSIC, 2, 0);
    }
    ST(ST_START_POINT(18)) = 0;
}

/* trigger 1 — sub_1000C759 */
static void trig01(void) {
    CB->divers(DV_MUSIC, 1, 4);
    /* 0xC77D */
    if (ST(0x84) == 2 && ST(0xC0C) == 2) {
        ST(0xC0C) = 100;
    }
    ST(ST_LEVEL) = 18;
    CB->change_piece(48, -1);
}

/* trigger 2 — sub_1000C7C5 */
static void trig02(void) {
    /* 0xC7E5 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xC350) && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_ARMAL)) == 1) {
        /* 0xC83A */
        if (ST(0xC0C) == 0 && ST(0xBC8) < 2 && ST(ST_ACTION_KEY) == 1) {
            CB->bloque(BLOQUE_ALL);
            CB->joindre(-ST(ST_HERO), 0, 0, 0, P_ARMAL);
            CB->joindre(-P_ARMAL, 0, 0, 0, ST(ST_HERO));
            CB->evenement(1, -1);
            ST(V_EVENT_1) = 0;
            ST(0x100) = 1;
            CB->retire_zone(2);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xC8F7 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L18D08", P_ARKHAN2, "SG00DIA07", NULL);
            }
            /* 0xC91F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L18D09", P_ARKHAN3, "SG00DIA07", NULL);
            }
            CB->dialogue("L18D07A", P_ARMAL);
            CB->dialogue("L18D07B", P_ARMAL);
            CB->dialogue("L18D07C", P_ARMAL);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->joindre_perm(P_ARMAL, ST(ST_HERO));
            CB->bloque(BLOQUE_NONE);
            CB->ajoute_zone(2, 1975, 1975, 100, 100, 1);
            return;
        }
        /* 0xC9C6 */
        if (ST(0xC0C) == 2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x74) = 10;
            CB->stop_joindre(-P_ARMAL);
            CB->vire_col(8, 0);
            CB->joindre(-ST(ST_HERO), 0, 0, 0, P_ARMAL);
            CB->joindre(-P_ARMAL, 0, 0, 0, ST(ST_HERO));
            CB->evenement(1, -1);
            ST(V_EVENT_0) = 0;
            CB->retire_zone(2);
            CB->animation("SP06ATT04");
            CB->animation("P06_65535");
            /* 0xCA8D */
            if (ST(0x114) == 0) {
                lieu_action_register("L18D119", P_ARMAL, "SP06DIA54", "SP06ATT02_10");
                ST(0x114) = 1;
            }
            CB->evenement(33, 1000);
            return;
        }
        /* 0xCADC */
        if ((ST(0xC0C) == 0 && ST(0xBC8) >= 2 && ST(0xDB0) == 0) || (ST(0xC0C) == 0 && ST(0xBC8) >= 2 && ST(0xDB0) >= 1) || ST(0xC0C) == 1 || ST(0xC0C) == 4 || ST(0x84) == 1) {
            ST(0x100) = 1;
            CB->retire_zone(2);
            CB->joindre(-ST(ST_HERO), 0, 0, 0, P_ARMAL);
            CB->joindre(-P_ARMAL, 0, 0, 0, ST(ST_HERO));
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x74) = 10;
            CB->bloque(BLOQUE_ALL);
            CB->stop_joindre(-P_ARMAL);
            /* 0xCC00 */
            if (ST(0xC0C) == 0 && ST(0xBC8) >= 2 && ST(0xDB0) == 0) {
                /* 0xCC36 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L18D08", P_ARKHAN2, "SG00DIA07", NULL);
                }
                /* 0xCC5E */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L18D09", P_ARKHAN3, "SG00DIA07", NULL);
                }
                lieu_action_register("L18D202", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
            }
            /* 0xCC9F */
            if (ST(0xC0C) == 0 && ST(0xBC8) >= 2 && ST(0xDB0) >= 1) {
                /* 0xCCD5 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L18D08", P_ARKHAN2, "SG00DIA07", NULL);
                }
                /* 0xCCFD */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L18D09", P_ARKHAN3, "SG00DIA07", NULL);
                }
                ST(V_DLG_RANDOM) = CB->random(2);
                /* 0xCD3C */
                if (ST(V_DLG_RANDOM) != 0) {
                    lieu_action_register("L18D203A", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                } else {
                    lieu_action_register("L18D203B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                }
            }
            /* 0xCD85 */
            if (ST(0xC0C) == 1 || ST(0xC0C) == 4 || ST(0x84) == 1) {
                /* 0xCDBB */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L18D204", P_ARKHAN2, "SG00DIA07", NULL);
                }
                /* 0xCDE3 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L18D205", P_ARKHAN3, "SG00DIA07", NULL);
                }
                lieu_action_register("L18D206", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->modify_var(&ST(0x74), 1);
            return;
        }
    }
}

/* trigger 3 — sub_1000CE5A */
static void trig03(void) {
    c8 buf1[64];
    c8 buf2[64];
    /* 0xCE6C */
    if (ST(ST_VIE(P_MALROT2)) >= 2 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_ACTION_KEY) != 0) {
        CB->select_pile(1, 2);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->fouille(P_MALROT2);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0xCF0F */
    if (CB->direction_perso(P_MALROT2) && ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        /* 0xCF4E */
        if (ST(ST_VIE(P_MALROT2)) < 2) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x70) = 20;
            CB->stop_joindre(-P_MALROT2);
            CB->joindre(ST(ST_HERO), 0, 0, 0, P_MALROT2);
            /* 0xCFA7 */
            if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 24 && !CB->divers(DV_CAN_FIGHT, 0, 0) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                /* 0xD010 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                }
                /* 0xD03B */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                }
                lieu_action_register("L18D159", P_MALROT2, "SP62DIA64", "SP62ARR01_10");
                CB->animation("G00PRE82");
                CB->animation("ar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x70), 1);
                return;
            }
            /* 0xD0C7 */
            if (ST(ST_PLAYER_WEAPON) != 24 || ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 0)) || (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 0) && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                /* 0xD157 */
                if (ST(0xBC8) == 0 || (ST(0xBC8) == 1 && ST(0x4C) == 666)) {
                    ST(0xF4C) = 1;
                    /* 0xD19C */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        /* 0xD1AE */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L18D139A", P_ARKHAN2, "SG00DIA04", NULL);
                            lieu_action_register("L18D139B", P_ARKHAN2, "SG00DIA04", NULL);
                        }
                        /* 0xD1EC */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L18D140", P_ARKHAN3, "SG00DIA04", NULL);
                        }
                        lieu_action_register("L18D141A", P_MALROT2, "SP62DIA64", "SP62ARR01_10");
                        lieu_action_register("L18D141B", P_MALROT2, "SP62DIA59", "SP62ARR01_10");
                    }
                    /* 0xD246 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        /* 0xD258 */
                        if (ST(0x98) == 1) {
                            /* 0xD26A */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D142", P_ARKHAN2, "SG00DIA03", NULL);
                            }
                            /* 0xD292 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D143", P_ARKHAN3, "SG00DIA03", NULL);
                            }
                            lieu_action_register("L18D144", P_MALROT2, "SP62DIA60", "SP62ARR01_10");
                        } else {
                            ST(0x98) = 1;
                            /* 0xD2E7 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D145", P_ARKHAN2, "SG00DIA08", NULL);
                            }
                            /* 0xD30F */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D146", P_ARKHAN3, "SG00DIA08", NULL);
                            }
                            lieu_action_register("L18D147", P_MALROT2, "SP62DIA63", "SP62ARR01_10");
                        }
                    }
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                    return;
                }
                /* 0xD382 */
                if (ST(0xBC8) == 1 && ST(0x4C) == 0) {
                    /* 0xD3A3 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L18D23A", P_ARKHAN2, "SG00DIA07", "Sar_10");
                        lieu_action_register("L18D23B", P_ARKHAN2, "SG00DIA07", "Sar_10");
                    }
                    /* 0xD3E7 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L18D24A", P_ARKHAN3, "SG00DIA07", "Sar_10");
                        lieu_action_register("L18D24B", P_ARKHAN3, "SG00DIA07", "Sar_10");
                    }
                    lieu_action_register("L18D274A", P_MALROT2, "SP62DIA64", "SP62ARR01_10");
                    lieu_action_register("L18D274B", P_MALROT2, "SP62DIA64", "SP62ARR01_10");
                    ST(0x4C) = 666;
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    return;
                }
                /* 0xD49B */
                if (ST(0xBC8) == 2) {
                    /* 0xD4AD */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        /* 0xD4BF */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L18D148", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                        /* 0xD4E7 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L18D149", P_ARKHAN3, "SG00DIA05", NULL);
                        }
                        lieu_action_register("L18D150", P_MALROT2, "SP62DIA64", "SP62ARR01_10");
                    }
                    /* 0xD528 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        /* 0xD53A */
                        if (ST(0x98) == 0) {
                            /* 0xD54C */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D154", P_ARKHAN2, "SG00DIA06", NULL);
                            }
                            /* 0xD574 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D155", P_ARKHAN3, "SG00DIA06", NULL);
                            }
                            lieu_action_register("L18D156", P_MALROT2, "SP62DIA59", "SP62ARR01_10");
                            ST(0x98) = 1;
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->modify_var(&ST(0x70), 1);
                            return;
                        }
                        /* 0xD5F6 */
                        if (ST(0x98) == 1) {
                            /* 0xD608 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D151", P_ARKHAN2, "SG00DIA07", NULL);
                            }
                            /* 0xD630 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D152", P_ARKHAN3, "SG00DIA07", NULL);
                            }
                            lieu_action_register("L18D153A", P_MALROT2, "SP62DIA64", "SP62ARR01_10");
                            lieu_action_register("L18D153B", P_MALROT2, "SP62DIA62", "SP62ARR01_10");
                            ST(0x98) = 0;
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->modify_var(&ST(0x70), 1);
                            return;
                        }
                    }
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    return;
                }
            }
            /* 0xD6FD */
            if (ST(ST_PLAYER_WEAPON) == 24) {
                /* 0xD70F */
                if (ST(0xBC8) < 2) {
                    /* 0xD721 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L18D184", P_ARKHAN2, "SG00PRE80", "SG00PRE81_10");
                    }
                    /* 0xD74C */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L18D185", P_ARKHAN3, "SG00PRE80", "SG00PRE81_10");
                    }
                    lieu_action_register("L18D186", P_MALROT2, "SP62DIA64", "SP62ARR01_10");
                    CB->animation("G00PRE82");
                    CB->animation("ar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                    return;
                }
                /* 0xD7D8 */
                if (ST(0xBC8) >= 2) {
                    /* 0xD7EA */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L18D184", P_ARKHAN2, "SG00PRE80", "SG00PRE81_10");
                    }
                    /* 0xD815 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L18D185", P_ARKHAN3, "SG00PRE80", "SG00PRE81_10");
                    }
                    lieu_action_register("L18D187", P_MALROT2, "SP62DIA62", "SP62ARR01");
                    lieu_pre_scene(ST(ST_HERO), P_MALROT2, 24, 6, 3);
                    CB->animation("P62ACT01");
                    CB->animation("P62ARR01");
                    /* 0xD88A */
                    if (ST(0xDC) == 1) {
                        CB->dialogue("L18D188", P_MALROT2);
                    }
                    /* 0xD8AC */
                    if (ST(0xDC) == 0) {
                        ST(0xDC) = 1;
                        CB->dialogue("L18D189", P_MALROT2);
                    }
                    lieu_pre_scene(ST(ST_HERO), P_MALROT2, 24, 6, 3);
                    ST(0x140 + 4 * ST(ST_USED_VARIANT)) = 0;
                    snprintf(buf1, sizeof(buf1), "SOB24%c", (char)(ST(ST_USED_VARIANT) + 65));
                    CB->etat_objet(buf1, -1, -1, -1, -1, -1, -1, -2, 3);
                    snprintf(buf2, sizeof(buf2), "OB77%c", (char)(ST(ST_USED_VARIANT) + 65));
                    CB->etat_objet(buf2, -1, -1, -1, -1, -1, -1, -3, 0);
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                    return;
                }
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xD9DE */
    if (!CB->direction_perso(P_MALROT2) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        trig07();
    }
    /* 0xDA16 */
    if (!CB->direction_perso(P_MALROT2) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
    }
    /* 0xDA4A */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MALROT2)) != VIE_DEAD && ST(0x70) != 10) {
        ST(ST_SCRIPT_BUSY) = 1;
        ST(V_EVENT15_STEP) = 1;
        ST(0xD8) = 1;
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        ST(0x70) = 10;
        CB->stop_joindre(-P_MALROT2);
        CB->animation("SP62ESA16");
        CB->animation("Sar_10");
        CB->evenement(52, 100);
    }
}

/* trigger 4 — sub_1000DB12 */
static void trig04(void) {
    c8 buf[256];
    i32 cf0, cf1_a, cf1_b;
    /* 0xDB2A */
    if (ST(ST_VIE(P_ARMAL)) >= 2 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_ACTION_KEY) != 0) {
        CB->select_pile(1, 2);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDB90 */
        if (ST(0xC14) == 2) {
            ST(0xC14) = 3;
            CB->divers_empile(DV_PERSONA_ITEMS, P_ARMAL, 84);
        }
        CB->fouille(P_ARMAL);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        return;
    }
    /* 0xDC05 */
    if (CB->direction_perso(P_ARMAL) && ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        /* 0xDC4B */
        if (CB->distance(6, 4) < 80 && ST(ST_VIE(P_ARMAL)) < 2) {
            CB->bloque(BLOQUE_ALL);
            snprintf(buf, sizeof(buf), "Sp%02dMAR30", ST(ST_HERO));
            CB->animation(buf);
            snprintf(buf, sizeof(buf), "P%02d_65535", ST(ST_HERO));
            CB->animation(buf);
        }
        /* 0xDCD2 */
        if (ST(ST_VIE(P_ARMAL)) < 2) {
            CB->select_pile(1, 2);
            CB->select_pile(0, 1);
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->stop_joindre(-P_ARMAL);
            /* 0xDD20 */
            if (ST(0x74) != 10 && ST(0x74) != 0) {
                ST(0x74) = 10;
            }
            CB->animation("SP06ARR01_10");
            CB->joindre(-ST(ST_HERO), 0, 0, 0, P_ARMAL);
            CB->joindre(-P_ARMAL, 0, 0, 0, ST(ST_HERO));
            CB->evenement(1, -1);
            ST(V_EVENT_1) = 0;
            /* 0xDDB1 */
            if (ST(0xBC8) != 2) {
                /* 0xDDC3 */
                if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 0) || (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 0) && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0xDE2F */
                    if (ST(0xBC8) == 0) {
                        ST(0xF4C) = 1;
                        /* 0xDE50 */
                        if (ST(0xDAC) == 0) {
                            ST(0xDAC) = 1;
                            /* 0xDE71 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xDE83 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D12", P_ARKHAN2, "SG00DIA07", "Sar_10");
                                }
                                /* 0xDEAE */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D13", P_ARKHAN3, "SG00DIA07", "Sar_10");
                                }
                            }
                            /* 0xDED9 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xDEEB */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D15", P_ARKHAN2, "SG00DIA07", "Sar_10");
                                }
                                /* 0xDF16 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D16", P_ARKHAN3, "SG00DIA07", "Sar_10");
                                }
                            }
                            CB->dialogue("L18D14A", P_ARMAL);
                            CB->dialogue("L18D14B", P_ARMAL);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x74), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xDF93 */
                        if (ST(0xDAC) == 1) {
                            /* 0xDFA5 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xDFB7 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D18", P_ARKHAN2, "SG00DIA07", "Sar_10");
                                }
                                /* 0xDFE2 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D19", P_ARKHAN3, "SG00DIA07", "Sar_10");
                                }
                            }
                            /* 0xE00D */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xE01F */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D21", P_ARKHAN2, "SG00DIA07", "Sar_10");
                                }
                                /* 0xE04A */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D22", P_ARKHAN3, "SG00DIA07", "Sar_10");
                                }
                            }
                            ST(V_DLG_RANDOM) = CB->random(3);
                            /* 0xE08C */
                            if (ST(V_DLG_RANDOM) == 0) {
                                CB->dialogue("L18D20A", P_ARMAL);
                            }
                            /* 0xE0AE */
                            if (ST(V_DLG_RANDOM) == 1) {
                                CB->dialogue("L18D20B", P_ARMAL);
                            }
                            /* 0xE0D0 */
                            if (ST(V_DLG_RANDOM) == 2) {
                                CB->dialogue("L18D20C", P_ARMAL);
                            }
                            CB->modify_var(&ST(0x74), 1);
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0xE124 */
                    if (ST(0xBC8) == 1) {
                        ST(0x74) = 24;
                        ST(0xBC8) = 2;
                        ST(V_EVENT_1) = 10;
                        CB->evenement(1, -1);
                        /* 0xE170 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L18D23A", P_ARKHAN2, "SG00DIA07", "Sar_10");
                            lieu_action_register("L18D23B", P_ARKHAN2, "SG00DIA07", "Sar_10");
                        }
                        /* 0xE1B4 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L18D24A", P_ARKHAN3, "SG00DIA07", "Sar_10");
                            lieu_action_register("L18D24B", P_ARKHAN3, "SG00DIA07", "Sar_10");
                        }
                        CB->vire_col(8, 0);
                        CB->joindre(-P_ARMAL, 0, 0, 0, ST(ST_HERO));
                        lieu_action_register("L18D25", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        /* 0xE23B */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L18D255", P_ARKHAN2, "SG00DIA07", "Sar_10");
                        }
                        /* 0xE266 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L18D256", P_ARKHAN3, "SG00DIA07", "Sar_10");
                        }
                        lieu_action_register("L18D25A", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        lieu_action_register("L18D25B", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        lieu_action_register("L18D25C", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        CB->vire_col(8, 1);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x74), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                } else if (ST(ST_PLAYER_WEAPON) == 24) {
                    /* 0xE335 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L18D184", P_ARKHAN2, "SG00PRE80", "SG00PRE81_10");
                    }
                    /* 0xE360 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L18D185", P_ARKHAN3, "SG00PRE80", "SG00PRE81_10");
                    }
                    /* 0xE38B */
                    if (ST(0x80) == 1) {
                        lieu_action_register("L18D257", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                    }
                    /* 0xE3B6 */
                    if (ST(0x80) == 0) {
                        lieu_action_register("L18D258", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        ST(0x80) = 1;
                    }
                    CB->animation("SP06ARR01_10");
                    CB->joindre_perm(P_ARMAL, ST(ST_HERO));
                    CB->animation("SG00PRE82");
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->modify_var(&ST(0x74), 1);
                    CB->bloque(BLOQUE_NONE);
                    return;
                } else if (ST(ST_PLAYER_WEAPON) != 0) {
                    cf1_a = CB->divers(DV_CAN_FIGHT, 0, 1);
                    /* 0xE480 */
                    if (cf1_a == 0) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0xE4A6 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        }
                        /* 0xE4D1 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                        }
                        CB->dialogue("L18D28A", P_ARMAL);
                        /* 0xE50C */
                        if (ST(0xBC8) >= 1) {
                            CB->dialogue("L18D28B", P_ARMAL);
                        }
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x74), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xE571 */
                    if (cf1_a == 1) {
                        /* 0xE583 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        }
                        /* 0xE5AE */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                        }
                        lieu_action_register("L18D116A", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                        lieu_action_register("L18D116B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                        lieu_action_register("L18D115B", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->modify_var(&ST(0x74), 1);
                        return;
                    }
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
            }
            /* 0xE685 */
            if (ST(0xBC8) == 2) {
                cf0 = CB->divers(DV_CAN_FIGHT, 0, 0);
                cf1_b = CB->divers(DV_CAN_FIGHT, 0, 1);
                /* 0xE6C1 */
                if (ST(ST_PLAYER_WEAPON) == 0 || (cf1_b != 0 && ST(0x84) != 1 && ST(0xC0C) != 1 && ST(0xC0C) != 4) || (cf0 != 0 && ST(0x84) != 1 && ST(0xC0C) != 1 && ST(0xC0C) != 4) || (ST(ST_PLAYER_WEAPON) != 0 && cf0 == 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                    /* 0xE792 */
                    if (ST(0xC0C) == 1 || ST(0xC0C) == 3 || ST(0x84) == 1) {
                        /* 0xE7C8 */
                        if (ST(0xDB0) == 0) {
                            /* 0xE7DA */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D29", P_ARKHAN2, "SG00DIA08", "Sar_10");
                            }
                            /* 0xE805 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D30", P_ARKHAN3, "SG00DIA08", "Sar_10");
                            }
                            lieu_action_register("L18D31A", P_ARMAL, "SP06DIA55", "SP06ARR01_10");
                            lieu_action_register("L18D31B", P_ARMAL, "SP06DIA55", "SP06ARR01_10");
                            lieu_action_register("L18D31C", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                            lieu_action_register("L18D31D", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                            lieu_action_register("L18D31E", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                            lieu_action_register("L18D31F", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x74), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xE8F8 */
                        if (ST(0xDB0) >= 1 && ST(0xC0C) == 1) {
                            /* 0xE91C */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D129", P_ARKHAN2, "SG00DIA01", "Sar_10");
                            }
                            /* 0xE947 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D130", P_ARKHAN3, "SG00DIA01", "Sar_10");
                            }
                            lieu_action_register("L18D131", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x74), 1);
                            CB->bloque(BLOQUE_NONE);
                            /* 0xE9B8 */
                            if (ST(0xC0C) == 3) {
                                ST(0xC0C) = 0;
                            }
                            return;
                        }
                    }
                    /* 0xE9DE */
                    if (ST(0xC0C) == 2) {
                        /* 0xE9F0 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L18D32", P_ARKHAN2, "SG00DIA01", "Sar_10");
                        }
                        /* 0xEA1B */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L18D33", P_ARKHAN3, "SG00DIA01", "Sar_10");
                        }
                        lieu_action_register("L18D34", P_ARMAL, "SP06DIA64", "SP06ARR01_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x74), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xEA91 */
                    if (ST(0xC0C) == 3) {
                        /* 0xEAA3 */
                        if (ST(0xF70) == 2) {
                            ST(0xC0C) = 4;
                            /* 0xEAC4 */
                            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L18D35", P_ARKHAN2, "SG00DIA02", "Sar_10");
                            }
                            /* 0xEB01 */
                            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L18D36", P_ARKHAN3, "SG00DIA02", "Sar_10");
                            }
                            /* 0xEB3E */
                            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L18D37", P_ARKHAN2, "SG00DIA02", "Sar_10");
                            }
                            /* 0xEB7B */
                            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L18D38", P_ARKHAN3, "SG00DIA02", "Sar_10");
                            }
                            ST(V_DLG_RANDOM) = CB->random(2);
                            /* 0xEBCF */
                            if (ST(V_DLG_RANDOM) == 0) {
                                lieu_action_register("L18D39", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                            }
                            /* 0xEBFA */
                            if (ST(V_DLG_RANDOM) == 1) {
                                lieu_action_register("L18D40", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                            }
                        }
                        /* 0xEC25 */
                        if (ST(0xF70) == 1) {
                            ST(0xC0C) = 0;
                            /* 0xEC46 */
                            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L18D259", P_ARKHAN2, "SG00DIA03", "Sar_10");
                            }
                            /* 0xEC83 */
                            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L18D260", P_ARKHAN3, "SG00DIA03", "Sar_10");
                            }
                            /* 0xECC0 */
                            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L18D261", P_ARKHAN2, "SG00DIA03", "Sar_10");
                            }
                            /* 0xECFD */
                            if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L18D262", P_ARKHAN3, "SG00DIA03", "Sar_10");
                            }
                            /* 0xED3A */
                            if (ST(0xDC4) == 1) {
                                lieu_action_register("L18D263A", P_ARMAL, "SP06DIA56", "SP06ARR01_10");
                                lieu_action_register("L18D263B", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                            }
                            /* 0xED7E */
                            if (ST(0xDC4) == 0) {
                                ST(0xDC4) = 1;
                                lieu_action_register("L18D264A", P_ARMAL, "SP06DIA56", "SP06ARR01_10");
                                lieu_action_register("L18D264B", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                            }
                        }
                        CB->modify_var(&ST(0x74), 1);
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xEE03 */
                    if (ST(0xC0C) == 4) {
                        /* 0xEE12 */
                        if (ST(0x68) == 1) {
                            /* 0xEE24 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xEE36 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D43", P_ARKHAN2, "SG00DIA05", "Sar_10");
                                }
                                /* 0xEE61 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D44", P_ARKHAN3, "SG00DIA05", "Sar_10");
                                }
                                lieu_action_register("L18D45A", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                lieu_action_register("L18D45B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                lieu_action_register("L18D45C", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                lieu_action_register("L18D45D", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xEF22 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xEF34 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D102A", P_ARKHAN2, "SG00DIA08", "Sar_10");
                                    lieu_action_register("L18D102B", P_ARKHAN2, "SG00DIA08", "Sar_10");
                                }
                                /* 0xEF78 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D103A", P_ARKHAN3, "SG00DIA08", "Sar_10");
                                    lieu_action_register("L18D103B", P_ARKHAN3, "SG00DIA08", "Sar_10");
                                }
                                lieu_action_register("L18D104A", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                                lieu_action_register("L18D104B", P_ARMAL, "SP06DIA60", "SP06GAR26");
                                CB->animation("P06GAR02_10");
                                CB->modify_var(&ST(0xE0), 1);
                                CB->evenement(51, 1000);
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                        }
                        /* 0xF03B */
                        if (ST(0x68) == 0) {
                            ST(0x68) = 1;
                            /* 0xF059 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xF06B */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D49", P_ARKHAN2, "SG00DIA01", "Sar_10");
                                }
                                /* 0xF096 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D50", P_ARKHAN3, "SG00DIA01", "Sar_10");
                                }
                                lieu_action_register("L18D51A", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                lieu_action_register("L18D51B", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                                /* 0xF0F3 */
                                if (ST(0xDB4) == 0) {
                                    ST(0xDB4) = 1;
                                    lieu_action_register("L18D54A", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                                    lieu_action_register("L18D54B", P_ARMAL, "SP06DIA56", "SP06ARR01_10");
                                }
                            }
                            /* 0xF146 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xF158 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D52", P_ARKHAN2, "SG00DIA05", "Sar_10");
                                }
                                /* 0xF183 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D53", P_ARKHAN3, "SG00DIA05", "Sar_10");
                                }
                                lieu_action_register("L18D265A", P_ARMAL, "SP06DIA64", "SP06ARR01_10");
                                lieu_action_register("L18D265B", P_ARMAL, "SP06DIA64", "SP06ARR01_10");
                            }
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x74), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                    }
                    /* 0xF212 */
                    if (ST(0xC0C) == 0) {
                        /* 0xF224 */
                        if (ST(0xDB0) == 0) {
                            ST(0xC0C) = 1;
                            ST(0xDB0) = 1;
                            /* 0xF254 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                /* 0xF266 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D55", P_ARKHAN2, "SG00DIA07", "Sar_10");
                                }
                                /* 0xF291 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D56", P_ARKHAN3, "SG00DIA07", "Sar_10");
                                }
                            }
                            /* 0xF2BC */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                /* 0xF2CE */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D57", P_ARKHAN2, "SG00DIA07", "Sar_10");
                                }
                                /* 0xF2F9 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D58", P_ARKHAN3, "SG00DIA07", "Sar_10");
                                }
                            }
                            lieu_action_register("L18D59A", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                            lieu_action_register("L18D59B", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                            lieu_action_register("L18D59C", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                            /* 0xF36F */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D60", P_ARKHAN2, "SG00DIA01", "Sar_10");
                            }
                            /* 0xF39A */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D61", P_ARKHAN3, "SG00DIA01", "Sar_10");
                            }
                            lieu_action_register("L18D31A", P_ARMAL, "SP06DIA55", "SP06ARR01_10");
                            lieu_action_register("L18D31B", P_ARMAL, "SP06DIA55", "SP06ARR01_10");
                            lieu_action_register("L18D31C", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                            lieu_action_register("L18D31D", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                            lieu_action_register("L18D31E", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                            lieu_action_register("L18D31F", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                            CB->animation("Sar_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x74), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xF498 */
                        if (ST(0xDB0) == 1) {
                            /* 0xF4AA */
                            if (ST(0x32C) == 0 && ST(V_GUARD_FIGHT_DONE) < 2 && ST(0xDC0) == 0) {
                                ST(0xDC0) = 1;
                                /* 0xF4EF */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D90A", P_ARKHAN2, "SG00DIA02", "Sar_10");
                                    lieu_action_register("L18D90B", P_ARKHAN2, "SG00DIA04", "Sar_10");
                                }
                                /* 0xF533 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D91A", P_ARKHAN3, "SG00DIA02", "Sar_10");
                                    lieu_action_register("L18D91B", P_ARKHAN3, "SG00DIA04", "Sar_10");
                                }
                                lieu_action_register("L18D92A", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                                lieu_action_register("L18D92B", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                                lieu_action_register("L18D92C", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                                lieu_action_register("L18D92D", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xF60D */
                            if (ST(ST_VIE(P_SORDOS)) == VIE_DEAD && ST(0x11C) == 0 && ST(ST_PLAYER_WEAPON) == 0) {
                                ST(0x11C) = 1;
                                /* 0xF652 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D279A", P_ARKHAN2, "SG00DIA07", NULL);
                                    lieu_action_register("L18D279B", P_ARKHAN2, "SG00DIA01", NULL);
                                    lieu_action_register("L18D279C", P_ARKHAN2, "SG00DIA08", NULL);
                                }
                                /* 0xF6A6 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D280A", P_ARKHAN3, "SG00DIA04", NULL);
                                    lieu_action_register("L18D280B", P_ARKHAN3, "SG00DIA08", NULL);
                                }
                                lieu_action_register("L18D281A", P_ARMAL, "SP06DIA58", "SP06ARR01_10");
                                lieu_action_register("L18D281B", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                lieu_action_register("L18D281C", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                                lieu_action_register("L18D281D", P_ARMAL, "SP06DIA56", "SP06ARR01_10");
                                /* 0xF748 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D282", P_ARKHAN2, "SG00DIA01", NULL);
                                }
                                /* 0xF770 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D283", P_ARKHAN3, "SG00DIA01", NULL);
                                }
                                /* 0xF798 */
                                if (ST(ST_VIE(P_DHORKAN11)) == VIE_DEAD) {
                                    ST(0xDB0) = 6;
                                } else {
                                    ST(0xDB0) = 5;
                                }
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xF7FF */
                            if (ST(ST_VIE(P_SORDOS)) == VIE_DEAD && ST(0x11C) == 1 && ST(ST_PLAYER_WEAPON) == 0) {
                                /* 0xF835 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D105", P_ARKHAN2, "SG00DIA05", "Sar_10");
                                }
                                /* 0xF860 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D106", P_ARKHAN3, "SG00DIA05", "Sar_10");
                                }
                                lieu_action_register("L18D107A", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                                lieu_action_register("L18D107B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                /* 0xF8BD */
                                if (ST(ST_VIE(P_DHORKAN11)) == VIE_DEAD) {
                                    lieu_action_register("L18D107C", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                                    ST(0xDB0) = 6;
                                } else {
                                    ST(0xDB0) = 5;
                                }
                                CB->modify_var(&ST(0x74), 1);
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xF93D */
                            if (ST(0xD88) != 0) {
                                ST(0xDB0) = 3;
                            } else {
                                /* 0xF963 */
                                if (ST(0xDC0) == 1) {
                                    /* 0xF975 */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        ST(0xDB0) = 2;
                                        /* 0xF996 */
                                        if (ST(ST_HERO) == P_ARKHAN2) {
                                            lieu_action_register("L18D63", P_ARKHAN2, "SG00DIA05", "Sar_10");
                                        }
                                        /* 0xF9C1 */
                                        if (ST(ST_HERO) == P_ARKHAN3) {
                                            lieu_action_register("L18D64", P_ARKHAN3, "SG00DIA05", "Sar_10");
                                        }
                                        lieu_action_register("L18D65A", P_ARMAL, "SP06DIA62", "SP06ARR01_10");
                                        lieu_action_register("L18D65B", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                                        /* 0xFA1E */
                                        if (ST(ST_HERO) == P_ARKHAN2) {
                                            lieu_action_register("L18D266", P_ARKHAN2, "SG00DIA05", "Sar_10");
                                        }
                                        /* 0xFA49 */
                                        if (ST(ST_HERO) == P_ARKHAN3) {
                                            lieu_action_register("L18D267", P_ARKHAN3, "SG00DIA05", "Sar_10");
                                        }
                                        lieu_action_register("L18D666A", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                                        lieu_action_register("L18D666B", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->modify_var(&ST(0x74), 1);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                    /* 0xFAD8 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        /* 0xFAEA */
                                        if (ST(ST_HERO) == P_ARKHAN2) {
                                            lieu_action_register("L18D66", P_ARKHAN2, "SG00DIA08", "Sar_10");
                                        }
                                        /* 0xFB15 */
                                        if (ST(ST_HERO) == P_ARKHAN3) {
                                            lieu_action_register("L18D67", P_ARKHAN3, "SG00DIA08", "Sar_10");
                                        }
                                        lieu_action_register("L18D68A", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                                        lieu_action_register("L18D68B", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                                        lieu_action_register("L18D68C", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                        CB->modify_var(&ST(0x74), 1);
                                        CB->bloque(BLOQUE_NONE);
                                        return;
                                    }
                                }
                            }
                        }
                        /* 0xFBBD */
                        if (ST(0xDB0) == 2) {
                            ST(0xDB0) = 3;
                            /* 0xFBDE */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D69", P_ARKHAN2, "SG00DIA01", "Sar_10");
                            }
                            /* 0xFC09 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D70", P_ARKHAN3, "SG00DIA01", "Sar_10");
                            }
                            lieu_action_register("L18D71A", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                            lieu_action_register("L18D71B", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x74), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xFC98 */
                        if (ST(0x11C) == 1 && ST(0xDB0) < 4) {
                            ST(0xDB0) = 4;
                        } else if (ST(0xDB0) == 3) {
                            ST(0xDB0) = 4;
                            /* 0xFCF1 */
                            if (ST(0xE98) == 0 && ST(ST_VIE(P_SORDOS)) != VIE_DEAD) {
                                /* 0xFD15 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D72A", P_ARKHAN2, "SG00DIA07", "Sar_10");
                                    lieu_action_register("L18D72B", P_ARKHAN2, "SG00DIA07", "Sar_10");
                                }
                                /* 0xFD59 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D73", P_ARKHAN3, "SG00DIA07", "Sar_10");
                                }
                                lieu_action_register("L18D74", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xFDCF */
                            if (ST(0xE98) != 0 && ST(ST_VIE(P_SORDOS)) != VIE_DEAD) {
                                /* 0xFDF3 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D75", P_ARKHAN2, "SG00DIA04", "Sar_10");
                                }
                                /* 0xFE1E */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D76", P_ARKHAN3, "SG00DIA04", "Sar_10");
                                }
                                lieu_action_register("L18D77A", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                lieu_action_register("L18D77B", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                lieu_action_register("L18D77C", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                lieu_action_register("L18D77D", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0xFEDF */
                            if (ST(0xE98) != 0 && ST(ST_VIE(P_SORDOS)) == VIE_DEAD && ST(0x11C) == 0) {
                                ST(0x11C) = 1;
                                /* 0xFF24 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D279A", P_ARKHAN2, "SG00DIA07", NULL);
                                    lieu_action_register("L18D279B", P_ARKHAN2, "SG00DIA01", NULL);
                                    lieu_action_register("L18D279C", P_ARKHAN2, "SG00DIA08", NULL);
                                }
                                /* 0xFF78 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D280A", P_ARKHAN3, "SG00DIA04", NULL);
                                    lieu_action_register("L18D280B", P_ARKHAN3, "SG00DIA08", NULL);
                                }
                                lieu_action_register("L18D281A", P_ARMAL, "SP06DIA58", "SP06ARR01_10");
                                lieu_action_register("L18D281B", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                lieu_action_register("L18D281C", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                                lieu_action_register("L18D281D", P_ARMAL, "SP06DIA56", "SP06ARR01_10");
                                /* 0x1A */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D282", P_ARKHAN2, "SG00DIA01", NULL);
                                }
                                /* 0x42 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D283", P_ARKHAN3, "SG00DIA01", NULL);
                                }
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                        }
                        /* 0x9C */
                        if (ST(0xB1C) > 0 && ST(0xDB0) == 4) {
                            ST(0xDB0) = 5;
                        }
                        /* 0xCF */
                        if (ST(0xDB0) == 4 && ST(ST_VIE(P_DHORKAN11)) < 2) {
                            ST(0xC0C) = 1;
                            ST(0xDB0) = 5;
                            /* 0x111 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D78", P_ARKHAN2, "SG00DIA01", "Sar_10");
                            }
                            /* 0x13C */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D79", P_ARKHAN3, "SG00DIA01", "Sar_10");
                            }
                            lieu_action_register("L18D80A", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                            lieu_action_register("L18D80B", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                            lieu_action_register("L18D80C", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x84), 0);
                            CB->modify_var(&ST(0x74), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x1F8 */
                        if ((ST(0xDB0) == 4 || ST(0xDB0) == 5) && ST(ST_VIE(P_DHORKAN11)) >= 2) {
                            ST(0xDB0) = 6;
                        }
                        /* 0x23D */
                        if (ST(0xDB0) == 5) {
                            ST(0xDB0) = 6;
                            /* 0x25E */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D81", P_ARKHAN2, "SG00DIA05", "Sar_10");
                            }
                            /* 0x289 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D82", P_ARKHAN3, "SG00DIA05", "Sar_10");
                            }
                            lieu_action_register("L18D83A", P_ARMAL, "SP06ATT61", "SP06ARR01_10");
                            lieu_action_register("L18D83B", P_ARMAL, "SP06ATT61", "SP06ARR01_10");
                            lieu_action_register("L18D83C", P_ARMAL, "SP06ATT61", "SP06ARR01_10");
                            lieu_action_register("L18D83D", P_ARMAL, "SP06DIA62", "SP06ARR01_10");
                            /* 0x318 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D268", P_ARKHAN2, "SG00DIA05", "Sar_10");
                            }
                            /* 0x343 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D269", P_ARKHAN3, "SG00DIA05", "Sar_10");
                            }
                            lieu_action_register("L18D270A", P_ARMAL, "SP06DIA62", "SP06ARR01_10");
                            lieu_action_register("L18D270B", P_ARMAL, "SP06DIA62", "SP06ARR01_10");
                            lieu_action_register("L18D270C", P_ARMAL, "SP06DIA62", "SP06ARR01_10");
                            /* 0x3B9 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D271", P_ARKHAN2, "SG00DIA05", "Sar_10");
                            }
                            /* 0x3E4 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D272", P_ARKHAN3, "SG00DIA05", "Sar_10");
                            }
                            lieu_action_register("L18D273A", P_ARMAL, "SP06DIA62", "SP06ARR01_10");
                            lieu_action_register("L18D273B", P_ARMAL, "SP06DIA62", "SP06ARR01_10");
                            lieu_action_register("L18D273C", P_ARMAL, "SP06DIA62", "SP06ARR01_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x84), 0);
                            CB->modify_var(&ST(0x74), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0x4A0 */
                        if (ST(0xDB0) >= 6 || (ST(0xDB0) >= 5 && ST(ST_VIE(P_DHORKAN11)) >= 2)) {
                            ST(0xDB0) = 7;
                            /* 0x4E5 */
                            if (ST(0xDC8) == 0 && ST(0xB3C) == 0 && ST(V_GUARD_FIGHT_DONE) != 0) {
                                /* 0x51B */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D84A", P_ARKHAN2, "SG00DIA06", "Sar_10");
                                    lieu_action_register("L18D84B", P_ARKHAN2, "SG00DIA06", "Sar_10");
                                }
                                /* 0x55F */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D85", P_ARKHAN3, "SG00DIA06", "Sar_10");
                                }
                                lieu_action_register("L18D86A", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                lieu_action_register("L18D86B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                lieu_action_register("L18D86C", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                lieu_action_register("L18D86D", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                            /* 0x620 */
                            if (ST(0xDCC) == 0 && ST(V_GUARD_FIGHT_DONE) != 0 && ST(0xB10) == 1) {
                                ST(0xDCC) = 1;
                                /* 0x665 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D87A", P_ARKHAN2, "SG00DIA06", "Sar_10");
                                    lieu_action_register("L18D87B", P_ARKHAN2, "SG00DIA06", "Sar_10");
                                }
                                /* 0x6A9 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D88A", P_ARKHAN3, "SG00DIA06", "Sar_10");
                                    lieu_action_register("L18D88B", P_ARKHAN3, "SG00DIA06", "Sar_10");
                                }
                                lieu_action_register("L18D89A", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                lieu_action_register("L18D89B", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            } else if (ST(0x31C) < 6 && ST(0x84) == 1) {
                                /* 0x77A */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D129", P_ARKHAN2, "SG00DIA01", "Sar_10");
                                }
                                /* 0x7A5 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D130", P_ARKHAN3, "SG00DIA01", "Sar_10");
                                }
                                lieu_action_register("L18D131", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            } else if (ST(0x31C) < 6 && ST(0x84) != 1) {
                                ST(0x84) = 1;
                                /* 0x853 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D105", P_ARKHAN2, "SG00DIA05", "Sar_10");
                                }
                                /* 0x87E */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D106", P_ARKHAN3, "SG00DIA05", "Sar_10");
                                }
                                lieu_action_register("L18D107A", P_ARMAL, "SP06DIA59", "SP06ARR01_10");
                                lieu_action_register("L18D107B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                lieu_action_register("L18D107C", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            } else if (ST(0x31C) >= 6) {
                                /* 0x93D */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L18D08", P_ARKHAN2, "SG00DIA07", NULL);
                                }
                                /* 0x965 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L18D09", P_ARKHAN3, "SG00DIA07", NULL);
                                }
                                ST(V_DLG_RANDOM) = CB->random(2);
                                /* 0x9A4 */
                                if (ST(V_DLG_RANDOM) != 0) {
                                    lieu_action_register("L18D203A", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                } else {
                                    lieu_action_register("L18D203B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                                }
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x74), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            }
                        }
                    }
                }
                /* 0xA1F */
                if (ST(ST_PLAYER_WEAPON) == 24) {
                    /* 0xA31 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L18D184", P_ARKHAN2, "SG00PRE80", "SG00PRE81_10");
                    }
                    /* 0xA5C */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L18D185", P_ARKHAN3, "SG00PRE80", "SG00PRE81_10");
                    }
                    /* 0xA87 */
                    if (ST(0x80) == 1) {
                        CB->dialogue("L18D257", P_ARMAL);
                    }
                    /* 0xAA9 */
                    if (ST(0x80) == 0) {
                        CB->dialogue("L18D258", P_ARMAL);
                        ST(0x80) = 1;
                    }
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->modify_var(&ST(0x74), 1);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0xB17 */
                if (ST(ST_PLAYER_WEAPON) == 49) {
                    /* 0xB29 */
                    if (ST(0xF50) == 0) {
                        /* 0xB3B */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L18D275", P_ARKHAN2, "SG00PRE80", "SG00PRE81_10");
                        }
                        /* 0xB66 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L18D276", P_ARKHAN3, "SG00PRE80", "SG00PRE81_10");
                        }
                        ST(0xF50) = 1;
                        lieu_action_register("L18D277A", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        lieu_action_register("L18D277B", P_ARMAL, "SP06PRE90", "SP06ARR01_10");
                        lieu_action_register("L18D277C", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        /* 0xBEB */
                        if (ST(ST_VIE(P_SORDOS)) == VIE_DEAD && ST(0x11C) == 0) {
                            ST(0x11C) = 1;
                            /* 0xC1E */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D279A", P_ARKHAN2, "SG00DIA07", NULL);
                                lieu_action_register("L18D279B", P_ARKHAN2, "SG00DIA01", NULL);
                                lieu_action_register("L18D279C", P_ARKHAN2, "SG00DIA08", NULL);
                            }
                            /* 0xC72 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D280A", P_ARKHAN3, "SG00DIA04", NULL);
                                lieu_action_register("L18D280B", P_ARKHAN3, "SG00DIA08", NULL);
                            }
                            lieu_action_register("L18D281A", P_ARMAL, "SP06DIA58", "SP06ARR01_10");
                            lieu_action_register("L18D281B", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                            lieu_action_register("L18D281C", P_ARMAL, "SP06DIA57", "SP06ARR01_10");
                            lieu_action_register("L18D281D", P_ARMAL, "SP06DIA56", "SP06ARR01_10");
                            /* 0xD14 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D282", P_ARKHAN2, "SG00DIA01", NULL);
                            }
                            /* 0xD3C */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D283", P_ARKHAN3, "SG00DIA01", NULL);
                            }
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->bloque(BLOQUE_NONE);
                            CB->modify_var(&ST(0x74), 1);
                            return;
                        }
                        lieu_action_register("L18D277D", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                    } else {
                        /* 0xDB4 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        }
                        /* 0xDDF */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                        }
                        CB->dialogue("L18D28A", P_ARMAL);
                        /* 0xE1A */
                        if (ST(0xBC8) >= 1) {
                            CB->dialogue("L18D28B", P_ARMAL);
                        }
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x74), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    CB->animation("Sar_10");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x70), 1);
                    return;
                }
                /* 0xEC1 */
                if (ST(ST_PLAYER_WEAPON) == 87) {
                    ST(V_WEAPON_WARNED) = 1;
                    /* 0xEE2 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        /* 0xEF4 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L18D93A", P_ARKHAN2, "SG00DIA06", "Sar_10");
                            lieu_action_register("L18D93B", P_ARKHAN2, "SG00DIA08", "Sar_10");
                            lieu_action_register("L18D93C", P_ARKHAN2, "SG00DIA06", "Sar_10");
                        }
                        /* 0xF51 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L18D94A", P_ARKHAN3, "SG00DIA06", "Sar_10");
                            lieu_action_register("L18D94B", P_ARKHAN3, "SG00DIA08", "Sar_10");
                            lieu_action_register("L18D94C", P_ARKHAN3, "SG00DIA06", "Sar_10");
                        }
                    }
                    /* 0xFAE */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        /* 0xFC0 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("L18D96A", P_ARKHAN2, "SG00DIA08", "Sar_10");
                            lieu_action_register("L18D96B", P_ARKHAN2, "SG00DIA08", "Sar_10");
                            lieu_action_register("L18D96C", P_ARKHAN2, "SG00DIA08", "Sar_10");
                        }
                        /* 0x101D */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("L18D97A", P_ARKHAN3, "SG00DIA08", "Sar_10");
                            lieu_action_register("L18D97B", P_ARKHAN3, "SG00DIA08", "Sar_10");
                            lieu_action_register("L18D97C", P_ARKHAN3, "SG00DIA08", "Sar_10");
                        }
                        lieu_action_register("L18D95A", P_ARMAL, "SP06PRE90", "SP06ARR01_10");
                        lieu_action_register("L18D95B", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        /* 0x10AC */
                        if (ST(0x570) == 2) {
                            /* 0x10BE */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D284", P_ARKHAN2, "SG00DIA08", "Sar_10");
                            }
                            /* 0x10E9 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D285", P_ARKHAN3, "SG00DIA08", "Sar_10");
                            }
                        } else {
                            /* 0x1119 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D286", P_ARKHAN2, "SG00DIA08", "Sar_10");
                            }
                            /* 0x1144 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D287", P_ARKHAN3, "SG00DIA08", "Sar_10");
                            }
                        }
                        lieu_action_register("L18D95C", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                    }
                    /* 0x1188 */
                    if (ST(0xC14) == 2) {
                        CB->animation("SP06ARR01_10");
                        CB->joindre_perm(P_ARMAL, ST(ST_HERO));
                        ST(0xC14) = 3;
                        CB->animation("P06ARR01_500");
                        lieu_action_register("L18D95D", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        lieu_action_register("L18D95E", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                        CB->link_objet(6, "OB84", 27, 1);
                    } else {
                        CB->animation("SP06ARR01_10");
                        CB->joindre_perm(P_ARMAL, ST(ST_HERO));
                        lieu_action_register("L18D95D", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
                    }
                    ST(V_WEAPON_WARNED) = 0;
                    lieu_pre_scene(ST(ST_HERO), P_ARMAL, 87, 6, 4);
                    CB->animation("SP06PRE73");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    CB->modify_var(&ST(0x74), 1);
                    CB->modify_var(&ST(V_WEAPON_WARNED), 0);
                    return;
                }
                /* 0x12D5 */
                if ((ST(0x84) == 1 || ST(0xC0C) == 1 || ST(0xC0C) == 4) && CB->divers(DV_CAN_FIGHT, 0, 0)) {
                    /* 0x1322 */
                    if (ST(0x84) == 1) {
                        ST(0x84) = 2;
                    }
                    /* 0x1343 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L18D111", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                    }
                    /* 0x136E */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L18D112", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                    }
                    CB->animation("SP06PRE90_600");
                    /* 0x13AA */
                    if (CB->divers(DV_ITEM_CHARGE, 0, -1) >= 80) {
                        ST(0xC0C) = 2;
                        CB->ajoute_zone(2, 1975, 1975, 100, 100, 1);
                        lieu_action_register("L18D113", P_ARMAL, "P06DIA62", "SP06ARR01_10");
                        lieu_pre_scene(ST(ST_HERO), P_ARMAL, ST(ST_PLAYER_WEAPON), 6, 4);
                        CB->animation("SP06PRE73");
                        lieu_action_register("L18D114", P_ARMAL, "P06DIA59", "SP06ARR01_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->modify_var(&ST(0x74), 1);
                        return;
                    }
                    /* 0x1480 */
                    if (CB->divers(DV_ITEM_CHARGE, 0, -1) < 80 && CB->divers(DV_ITEM_CHARGE, 0, -1) >= 40) {
                        lieu_action_register("L18D115A", P_ARMAL, "P06DIA54", "SP06ARR01_10");
                        lieu_action_register("L18D115B", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->modify_var(&ST(0x74), 1);
                        return;
                    }
                    /* 0x152A */
                    if (CB->divers(DV_ITEM_CHARGE, 0, -1) < 40) {
                        lieu_action_register("L18D116A", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                        lieu_action_register("L18D116B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                        lieu_action_register("L18D115B", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->modify_var(&ST(0x74), 1);
                        return;
                    }
                } else {
                    /* 0x15D4 */
                    if (ST(ST_PLAYER_WEAPON) != 0 && cf1_b == 0) {
                        ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                        /* 0x1607 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                        }
                        /* 0x1632 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                        }
                        CB->dialogue("L18D28A", P_ARMAL);
                        /* 0x166D */
                        if (ST(0xBC8) >= 1) {
                            CB->dialogue("L18D28B", P_ARMAL);
                        }
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->modify_var(&ST(0x74), 1);
                        return;
                    }
                    /* 0x16D7 */
                    if (ST(ST_PLAYER_WEAPON) != 0 && cf1_b != 0) {
                        /* 0x16F6 */
                        if (ST(0x84) == 1 || ST(0xC0C) == 1 || ST(0xC0C) == 4) {
                            /* 0x172C */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L18D111", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                            }
                            /* 0x1757 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L18D112", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                            }
                        } else {
                            /* 0x1787 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                            }
                            /* 0x17B2 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                            }
                        }
                        lieu_action_register("L18D116A", P_ARMAL, "SP06DIA60", "SP06ARR01_10");
                        lieu_action_register("L18D116B", P_ARMAL, "SP06DIA53", "SP06ARR01_10");
                        lieu_action_register("L18D115B", P_ARMAL, "SP06DIA61", "SP06ARR01_10");
                        CB->animation("G00PRE82");
                        CB->animation("ar_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        CB->modify_var(&ST(0x74), 1);
                        return;
                    }
                }
            }
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x189A */
    if (!CB->direction_perso(P_ARMAL) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 7)) {
        trig07();
    }
    /* 0x18D2 */
    if (!CB->direction_perso(P_ARMAL) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 8)) {
        trig08();
    }
    /* 0x1906 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_ARMAL)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        ST(V_EVENT_1) = 10;
        CB->evenement(1, -1);
        /* 0x197E */
        if (ST(0xBC8) != 2) {
            ST(0xF8) = 1;
            ST(ST_SCRIPT_BUSY) = 1;
            CB->animation("SP06ATT01_10");
            /* 0x19C7 */
            if (CB->divers(DV_DESC_POS, ST(ST_HERO), 1) <= 1945) {
                CB->animation("P06ATT05");
            }
            /* 0x19F5 */
            if (CB->divers(DV_DESC_POS, ST(ST_HERO), 1) > 1945) {
                CB->animation("P06ATT06");
            }
            CB->animation("P06_65535");
            CB->animation("P06GAR02_10");
            CB->evenement(51, 200);
            return;
        }
        /* 0x1A3D */
        if (ST(0xBC8) == 2) {
            ST(0xFC) = 1;
            ST(0x74) = 100;
            CB->stop_joindre(-P_ARMAL);
            CB->animation("SP06ESA13");
            CB->evenement(51, 200);
            return;
        }
    }
}

/* trigger 7 — sub_10011AA4 */
static void trig07(void) {
    /* 0x1AB7 */
    if (CB->direction_spec(0x3C2E)) {
        trig16();
        return;
    }
    /* 0x1AD0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x283C)) {
        ST(0x64) = 1;
        /* 0x1B1F */
        if (ST(0x88) == 3) {
            ST(0x88) = 0;
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            CB->animation("SG00PRE05");
            CB->bruitage("L18B01", -1, -1);
            /* 0x1B74 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("P02_65535");
            }
            /* 0x1B91 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("P03_65535");
            }
            CB->pause_anim(3, 1, -1);
            CB->change_camera(1, CAM_LOCKED);
            CB->bruitage("L18B02", 2400, 2115);
            CB->play_anim(5, 0);
            CB->play_anim(6, 0);
            CB->play_anim(4, 0);
            CB->evenement(34, 700);
            return;
        }
        /* 0x1C21 */
        if (ST(0x88) == 0) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x1C4A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD26", P_ARKHAN2, "SG00DIA04", NULL);
            }
            /* 0x1C72 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD27", P_ARKHAN3, "SG00DIA04", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 8 — sub_10011CB1 */
static void trig08(void) {
    /* 0x1CC0 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xEA60)) {
        ST(0x64) = 2;
        /* 0x1D0F */
        if (ST(0x88) == 2) {
            ST(0x88) = 0;
            ST(ST_SCRIPT_BUSY) = 1;
            CB->bloque(BLOQUE_ALL);
            CB->animation("SG00PRE05");
            CB->bruitage("L18B01", -1, -1);
            /* 0x1D64 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("P02_65535");
            }
            /* 0x1D81 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("P03_65535");
            }
            CB->bruitage("L18B02", 2400, 2115);
            CB->pause_anim(9, 1, -1);
            CB->play_anim(5, 0);
            CB->play_anim(6, 0);
            CB->play_anim(4, 0);
            CB->evenement(34, 1000);
            return;
        }
        /* 0x1E01 */
        if (ST(0x88) == 0) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x1E2A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD26", P_ARKHAN2, "SG00DIA04", NULL);
            }
            /* 0x1E52 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD27", P_ARKHAN3, "SG00DIA04", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 9 — sub_10011E91 */
static void trig09(void) {
    /* 0x1EA0 */
    if (ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x1ED9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02IMP01");
            CB->animation("P02_65535");
        }
        /* 0x1F01 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03IMP01");
            CB->animation("P03_65535");
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 10 — sub_10011F46 */
static void trig10(void) {
    /* 0x1F58 */
    if (ST(0xC24) == 2 && ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0xC24) = 3;
        CB->animation("SG00PRE06");
        /* 0x1FC8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("P02_65535");
        } else {
            CB->animation("P03_65535");
        }
        ST(0x150) = 0;
        CB->etat_objet("SOB77E", -1, -1, -1, -1, -1, -1, -3, 0);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x2038 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xA410) && ST(0xC24) == 0) {
        /* 0x208D */
        if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && CB->divers(DV_CAN_FIGHT, 0, 0)) || ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON)) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x20F9 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x210B */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L18D162", P_ARKHAN2, "SG00DIA04", NULL);
                }
                /* 0x2133 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L18D163", P_ARKHAN3, "SG00DIA04", NULL);
                }
            }
            /* 0x215B */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x216D */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L18D164", P_ARKHAN2, "SG00DIA08", NULL);
                }
                /* 0x2195 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("LCOD22A", P_ARKHAN3, "SG00DIA08", NULL);
                }
            }
            ST(V_DLG_RANDOM) = CB->random(2);
            /* 0x21D4 */
            if (ST(V_DLG_RANDOM) == 0) {
                CB->dialogue("L18D166A", 69);
            }
            /* 0x21F6 */
            if (ST(V_DLG_RANDOM) == 1) {
                CB->dialogue("L18D166B", 69);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x2236 */
        if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 0) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
            /* 0x22A4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L18D167", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
            }
            /* 0x22CF */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L18D168", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
            }
            ST(V_DLG_RANDOM) = CB->random(2);
            /* 0x2311 */
            if (ST(V_DLG_RANDOM) == 0) {
                CB->dialogue("L18D166A", 69);
            }
            /* 0x2333 */
            if (ST(V_DLG_RANDOM) == 1) {
                CB->dialogue("L18D166B", 69);
            }
            CB->animation("G00PRE82");
            /* 0x2360 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L18D169", P_ARKHAN2, "SG00DIA01", "Sar_10");
            }
            /* 0x238B */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L18D170", P_ARKHAN3, "SG00DIA01", "Sar_10");
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x23D1 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(5) && ST(0xC24) == 0) {
        ST(0xC24) = 1;
        ST(0xC24) = 2;
        CB->pause_anim(8, 1, -1);
        CB->evenement(36, 400);
    }
}

/* trigger 11 — sub_10012450 */
static void trig11(void) {
    /* 0x245F */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(23)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x24B9 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x24D3 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x24E5 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT13");
                }
                /* 0x2502 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT13");
                }
            } else {
                /* 0x2524 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT11");
                }
                /* 0x2541 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT11");
                }
            }
        }
        /* 0x255E */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x2578 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x258A */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT14");
                }
                /* 0x25A7 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT14");
                }
            } else {
                /* 0x25C9 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT12");
                }
                /* 0x25E6 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT12");
                }
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 — sub_10012625 */
static void trig12(void) {
    /* 0x2634 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(24)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x268E */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x26A8 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x26BA */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT13");
                }
                /* 0x26D7 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT13");
                }
            } else {
                /* 0x26F9 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT11");
                }
                /* 0x2716 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT11");
                }
            }
        }
        /* 0x2733 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x274D */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x275F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT14");
                }
                /* 0x277C */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT14");
                }
            } else {
                /* 0x279E */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT12");
                }
                /* 0x27BB */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT12");
                }
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 13 — sub_100127FA */
static void trig13(void) {
    /* 0x2809 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_spec(0x32C8) || CB->direction_spec(0x59D8))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x2876 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x2890 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x28A2 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT13");
                }
                /* 0x28BF */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT13");
                }
            } else {
                /* 0x28E1 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT11");
                }
                /* 0x28FE */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT11");
                }
            }
        }
        /* 0x291B */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x2935 */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x2947 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT14");
                }
                /* 0x2964 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT14");
                }
            } else {
                /* 0x2986 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT12");
                }
                /* 0x29A3 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT12");
                }
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 — sub_100129E2 */
static void trig14(void) {
    /* 0x29F1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x3E80)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE02");
        /* 0x2A56 */
        if (ST(0xBC) == 0) {
            ST(0xBC) = 1;
            /* 0x2A77 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD07", P_ARKHAN2);
            }
            /* 0x2A99 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD08", P_ARKHAN3);
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 — sub_100131C2 */
static void trig15(void) {
    /* 0x31D1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(15)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00OUV04");
        CB->animation("G00OUV09");
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 (also called by trigger 7) — sub_10012C37 */
static void trig16(void) {
    c8 buf1[64];
    c8 buf2[64];
    /* 0x2C46 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x3C2E)) {
        CB->select_pile(0, 1);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE02");
        ST(0x70) = 0;
        CB->stop_joindre(-P_MALROT2);
        /* 0x2CCF */
        if (ST(ST_VIE(P_ARMAL)) == 1 && ST(0xBC8) < 2) {
            /* 0x2CF3 */
            if (ST(0x800) == 1) {
                lieu_action_register("L18D183A", P_MALROT2, "SP62DIA63", "SP62ARR01_10");
                lieu_action_register("L18D183B", P_MALROT2, "SP62DIA59", "SP62ARR01_10");
            }
            /* 0x2D37 */
            if (ST(0x800) == 0) {
                ST(0x800) = 1;
                lieu_action_register("L18D182A", P_MALROT2, "SP62DIA60", "SP62ARR01_10");
                lieu_action_register("L18D182B", P_MALROT2, "SP62DIA60", "SP62ARR01_10");
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->animation("ar_10");
            CB->bloque(BLOQUE_NONE);
            CB->modify_var(&ST(0x70), 1);
            return;
        }
        CB->pos_perso(ST(ST_HERO), 2805, 1970, 0x3CC6, 1, "ar", 6, 0);
        /* 0x2DF2 */
        if (ST(ST_VIE(P_ARMAL)) == VIE_DEAD || ST(0xBC8) >= 2) {
            /* 0x2E16 */
            if (ST(ST_PLAYER_WEAPON) != 24 && ST(ST_PLAYER_WEAPON) != 77) {
                /* 0x2E3A */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT07");
                }
                /* 0x2E57 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT07");
                }
                CB->bruitage("L08B05", 2740, 1983);
                CB->evenement(20, 250);
                CB->evenement(2, 2000);
                return;
            }
            /* 0x2EAE */
            if (ST(ST_PLAYER_WEAPON) == 77) {
                /* 0x2EC0 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0x2ED2 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SP02ACT03");
                    }
                    /* 0x2EEF */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SP02ACT33");
                    }
                    CB->animation("P02_65535");
                }
                /* 0x2F17 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0x2F29 */
                    if (ST(ST_SPECIAL_MODE) == 1) {
                        CB->animation("SP03ACT03");
                    }
                    /* 0x2F46 */
                    if (ST(ST_SPECIAL_MODE) == 0) {
                        CB->animation("SP03ACT33");
                    }
                    CB->animation("P03_65535");
                }
                CB->select_pile(2, 1);
                /* 0x2F7E */
                if (ST(ST_VIE(P_MALROT2)) == 1) {
                    ST(0x70) = 0;
                    CB->stop_joindre(-P_MALROT2);
                    lieu_action_register("L18D176", P_MALROT2, "SP62DIA50", "SP62ARR01_10");
                }
                CB->select_pile(0, 1);
                CB->bruitage("L08B07", 2740, 1983);
                ST(0x140 + 4 * ST(ST_USED_VARIANT)) = 3;
                snprintf(buf1, sizeof(buf1), "SOB77%c", (char)(ST(ST_USED_VARIANT) + 65));
                CB->etat_objet(buf1, -1, -1, -1, -1, -1, -1, -2, 3);
                snprintf(buf2, sizeof(buf2), "OB24%c", (char)(ST(ST_USED_VARIANT) + 65));
                CB->etat_objet(buf2, -1, -1, -1, -1, -1, -1, -3, 0);
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                /* 0x30A0 */
                if (ST(ST_VIE(P_MALROT2)) == 1) {
                    CB->modify_var(&ST(0x70), 1);
                }
                return;
            }
            /* 0x30CB */
            if (ST(ST_PLAYER_WEAPON) == 24) {
                /* 0x30DD */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L08D71", P_ARKHAN2, "SG02ATT01", NULL);
                }
                /* 0x3105 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L08D72", P_ARKHAN3, "SG03ATT01", NULL);
                }
                /* 0x312D */
                if (ST(ST_VIE(P_MALROT2)) == 1) {
                    ST(0x70) = 0;
                    CB->stop_joindre(-P_MALROT2);
                    lieu_action_register("L18D181", P_MALROT2, "SP62DIA62", "SP62ARR01_10");
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->animation("ar_10");
                CB->bloque(BLOQUE_NONE);
                /* 0x3193 */
                if (ST(ST_VIE(P_MALROT2)) == 1) {
                    CB->modify_var(&ST(0x70), 1);
                }
                return;
            }
        }
    }
}

/* trigger 18 — sub_10012ADD */
static void trig18(void) {
    /* 0x2AEC */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(19) && CB->distance(6, 19) <= 75) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(V_DLG_RANDOM) = CB->random(2);
        /* 0x2B76 */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->animation("SG00PRE02");
        }
        /* 0x2B93 */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->animation("SG00PRE03");
        }
        /* 0x2BB0 */
        if (ST(0xD4) == 0) {
            ST(0xD4) = 1;
            /* 0x2BD1 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L18D173", P_ARKHAN2);
            }
            /* 0x2BF3 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L18D174", P_ARKHAN3);
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 20 — sub_1001325F */
static void trig20(void) {
    /* 0x326E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(21)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x32B9 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x32CB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0x32E8 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x332E */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x3340 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0x335D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT29");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 22 — sub_100133A7 */
static void trig22(void) {
    /* 0x33B6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x9470)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x3410 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x342A */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x343C */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT17");
                }
                /* 0x3459 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT17");
                }
            } else {
                /* 0x347B */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT15");
                }
                /* 0x3498 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT15");
                }
            }
        }
        /* 0x34B5 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x34CF */
            if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                /* 0x34E1 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT18");
                }
                /* 0x34FE */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT18");
                }
            } else {
                /* 0x3520 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT15");
                }
                /* 0x353D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT15");
                }
            }
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 25 — sub_10013592 */
static void trig25(void) {
    /* 0x35A1 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x35E8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x35FA */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD29", P_ARKHAN2);
            }
            /* 0x361C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD76", P_ARKHAN2);
            }
        }
        /* 0x363E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x3650 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("LCOD30", P_ARKHAN3);
            }
            /* 0x3672 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("LCOD77", P_ARKHAN3);
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 41 (tick-armed room hook) — sub_1000817B */
static void trig41(void) {
    CB->animation("Sar_10");
    /* 0x8198 */
    if (ST(0xBC8) < 2) {
        CB->divers_empile(DV_MUSIC, 1, 1);
    }
    /* 0x81BC */
    if (ST(0xBC8) >= 2) {
        CB->divers_empile(DV_MUSIC, 2, 1);
    }
    /* 0x81E0 */
    if (ST(ST_VIE(P_MALROT7)) == VIE_DEAD && ST(ST_VIE(P_ARMAL)) == 1) {
        ST(0x58) = 1;
        CB->select_pile(0, 2);
        CB->select_pile(1, 2);
        CB->select_pile(2, 2);
        CB->bloque(BLOQUE_ALL);
        CB->evenement(0, -1);
        CB->evenement(1, -1);
        ST(V_EVENT_0) = 0;
        ST(V_EVENT_1) = 0;
        ST(V_ENGAGED_PERSONA) |= 0x662;   /* engaged: P_ARMAL << 8 | P_MALROT2 */
        CB->stop_joindre(-P_MALROT2);
        CB->animation("Sar_10");
        CB->animation("SP62GAR02_10");
        CB->animation("SP06GAR02_10");
        CB->dialogue("L18D138", P_ARMAL);
        CB->modify_var(&ST(V_EVENT_51), 1);
        CB->modify_var(&ST(0xE0), 1);
        CB->modify_var(&ST(0xDA8), 3);
        CB->select_pile(0, 1);
        return;
    }
    /* 0x8322 */
    if (ST(0xC28) != 2 && ST(0x58) == 0) {
        /* 0x8343 */
        if (ST(0xBC8) < 2 && ST(ST_VIE(P_ARMAL)) == 1) {
            CB->bloque(BLOQUE_ALL);
            /* 0x8372 */
            if (ST(0xD9C) == 1) {
                lieu_action_register("L18D02", P_MALROT2, "SP62DIA59", "SP62ARR01_10");
            }
            /* 0x839D */
            if (ST(0xD9C) == 0) {
                ST(0xD9C) = 1;
                lieu_action_register("L18D01A", P_MALROT2, "SP62DIA60", "SP62ARR01_10");
                lieu_action_register("L18D01B", P_MALROT2, "SP62DIA60", "SP62ARR01_10");
            }
            CB->bloque(BLOQUE_NONE);
            CB->evenement(0, 2000);
            CB->evenement(1, 3000);
        }
        /* 0x8415 */
        if (ST(0xBC8) >= 2 && ST(ST_VIE(P_ARMAL)) == 1) {
            CB->bloque(BLOQUE_ALL);
            /* 0x8444 */
            if (ST(0xD9C) == 2) {
                CB->bloque(BLOQUE_ALL);
                ST(0x54) = 1;
                CB->change_camera(1, CAM_LOCKED);
                lieu_action_register("L18D05", P_MALROT2, "SP62DIA54", "SP62ARR01_10");
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_ARMAL);
                CB->dialogue("L18D133", P_ARMAL);
                CB->change_camera(0, CAM_AUTO);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x84D9 */
            if (ST(0xD9C) < 2) {
                CB->bloque(BLOQUE_ALL);
                ST(0xD9C) = 2;
                ST(0x54) = 1;
                CB->change_camera(1, CAM_LOCKED);
                lieu_action_register("L18D03", P_MALROT2, "SP62DIA64", "SP62ARR01_10");
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_ARMAL);
                CB->dialogue("L18D04A", P_ARMAL);
                CB->dialogue("L18D04B", P_ARMAL);
                CB->change_camera(0, CAM_AUTO);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            CB->bloque(BLOQUE_NONE);
            CB->evenement(0, 2000);
            CB->evenement(1, 2000);
            return;
        }
    }
    /* 0x85B7 */
    if (ST(0xC28) == 2 && ST(ST_VIE(P_ARMAL)) == 1) {
        ST(0xC28) = 3;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_MALROT6);
        lieu_action_register("L18D191A", P_MALROT6, "SP66DIA54", "SP66ARR01_10");
        CB->joindre(-P_MALROT6, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L18D191B", P_MALROT6, "SP66ATT50", "SP66ARR01_10");
        CB->animation("P66_65535");
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_MALROT6);
        CB->joindre(-P_ARMAL, 0, 0, 0, ST(ST_HERO));
        /* 0x86A6 */
        if (ST(0xBC8) == 2) {
            lieu_action_register("L18D192A", P_ARMAL, "SP06DIA58", "SP06ARR01_10");
            lieu_action_register("L18D192B", P_ARMAL, "SP06DIA58", "SP06ARR01_10");
        }
        /* 0x86EA */
        if (ST(0xBC8) != 2) {
            ST(0xBC8) = 2;
            lieu_action_register("L18D193A", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
            lieu_action_register("L18D193B", P_ARMAL, "SP06ATT60", "SP06ARR01_10");
            lieu_action_register("L18D193C", P_ARMAL, "SP06DIA58", "SP06ARR01_10");
        }
        lieu_action_register("L18D194", P_ARMAL, "SP06DIA54", "SP06ARR01_10");
        CB->joindre(ST(ST_HERO), 2238, 2016, 0x4010, -1);
        /* 0x8791 */
        if (ST(ST_PLAYER_WEAPON) != 0) {
            /* 0x87A3 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02PRE73");
                CB->animation("ar");
            }
            /* 0x87CB */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03PRE73");
                CB->animation("ar");
            }
        }
        CB->evenement(37, 100);
    }
}
