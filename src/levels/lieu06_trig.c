/*
 * lieu06_trig.c — Native hand-port of the LIEU06.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu06.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu06_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig01(void); static void trig02(void); static void trig03(void);
static void trig04(void); static void trig06(void); static void trig07(void);
static void trig11(void); static void trig12(void); static void trig13(void);
static void trig14(void); static void trig15(void); static void trig41(void);
static void trig_noop(void);   /* the 29 stub slots */

/* dword_10021840[42] — filled at INIT (0x1000FE06..) with the 5-byte jmp
 * thunks at 0x10001000+5*i; static here.
 *    0  100073F0             14  1000DCA3             28  1000E053 stub
 *    1  1000A44F             15  1000DD83             29  1000E05E stub
 *    2  1000A978             16  1000DFCF stub        30  1000E069 stub
 *    3  1000B111             17  1000DFDA stub        31  1000E074 stub
 *    4  1000B1F2             18  1000DFE5 stub        32  1000E07F stub
 *    5  1000D60B stub        19  1000DFF0 stub        33  1000E08A stub
 *    6  1000D616             20  1000DFFB stub        34  1000E095 stub
 *    7  1000D75C             21  1000E006 stub        35  1000E0A0 stub
 *    8  1000D8A2 stub        22  1000E011 stub        36  1000E0AB stub
 *    9  1000D8AD stub        23  1000E01C stub        37  1000E0B6 stub
 *   10  1000D8B8 stub        24  1000E027 stub        38  1000E0C1 stub
 *   11  1000D8C3             25  1000E032 stub        39  1000E0CC stub
 *   12  1000D8F4             26  1000E03D stub        40  1000E0D7 stub
 *   13  1000D935             27  1000E048 stub        41  10007FD7
 */
static void (*const g_trigger[42])(void) = {
    l06_trig00, trig01, trig02, trig03, trig04, trig_noop, trig06, trig07,
    trig_noop, trig_noop, trig_noop, trig11, trig12, trig13, trig14, trig15,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10021840]). */
void l06_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: start-point presets, state reset, zones, objects, actor placement */
void l06_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(6)) == 1) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 2;
        ST(0x358) = 0;
    }
    /* 0x743E */
    if (ST(ST_START_POINT(6)) == 2) {
        ST(ST_HERO) = P_ARKHAN1;
        ST(ST_LEVEL) = 2;
        ST(0x358) = 2;
    }
    /* 0x747A */
    if (ST(ST_START_POINT(6)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 2;
        ST(0xB1C) = 0;
        ST(0x740) = 0;
    }
    /* 0x74C5 */
    if (ST(ST_START_POINT(6)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_THANANDAR24)) = 1;
        ST(ST_LEVEL) = 2;
        ST(0xB1C) = 0;
        ST(0x73C) = 1;
        ST(0x740) = 2;
    }
    /* 0x752E */
    if (ST(ST_START_POINT(6)) == 5 || ST(ST_START_POINT(6)) == 6) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_VIE(P_THANANDAR24)) = VIE_DEAD;
        ST(ST_LEVEL) = 2;
        ST(0xB1C) = 0;
        ST(0x73C) = 1;
        ST(0x740) = 2;
    }
    /* 0x75A9 */
    if (ST(ST_START_POINT(6)) == 7) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 2;
        ST(0x73C) = 1;
        ST(0x740) = 2;
        ST(0xB1C) = 1;
    }
    /* 0x7603 */
    if (ST(ST_START_POINT(6)) == 8) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 2;
        ST(ST_VIE(P_THANANDAR24)) = VIE_DEAD;
        ST(0x730) = 1;
        ST(0x73C) = 1;
        ST(0x740) = 2;
        ST(0xB1C) = 1;
    }
    /* 0x767B */
    if (ST(ST_START_POINT(6)) == 9) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 2;
        ST(0xEB8) = 1;
    }
    /* 0x76B7 */
    if (ST(ST_START_POINT(6)) == 10) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 2;
        ST(0xEB8) = 2;
        ST(0xB34) = 1;
    }
    /* 0x7702 */
    if (ST(ST_START_POINT(6)) == 11) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 2;
        ST(0xEB8) = 2;
        ST(0xB34) = 2;
    }
    ST(V_EVENT_7) = 0;
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
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_EVENT_7) = 0;
    ST(0x35C) = 0;
    ST(V_EVENT_53) = 0;
    CB->evenement(0, -1);
    CB->evenement(1, -1);
    CB->evenement(2, -1);
    CB->evenement(7, -1);
    CB->evenement(15, -1);
    CB->evenement(53, -1);
    ST(V_ENGAGED_PERSONA) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_LAST_WEAPON) = 0;
    CB->ajoute_zone(1, 1700, -1842, 100, 135, 0);
    CB->ajoute_zone(2, 2066, -740, 105, 90, 0);
    CB->ajoute_zone(16, 2064, -612, 50, 50, 0);
    CB->ajoute_zone(3, 1730, -470, 90, 90, 0);
    CB->ajoute_zone(17, 1824, -370, 50, 50, 0);
    CB->ajoute_zone(13, 2375, -1340, 155, 130, 0);
    CB->ajoute_zone(15, 1803, -919, 215, 185, 0);
    CB->bruitage("L06AMB1", -1, -1);
    /* 0x7A2C */
    if (ST(V_GUARD_FIGHT_DONE) >= 2) {
        ST(0xEB8) = 2;
    }
    /* 0x7A4A */
    if (ST(ST_LEVEL) == 46) {
        CB->pos_perso(ST(ST_HERO), 1742, -540, 0xAD20, 1, "ar", 8, 0);
        CB->etat_objet("IM70", 1648, 18, -1886, 0xC000, 0x4000, 0x4000, 0, 2);
        CB->ajoute_zone(6, 2120, -1230, 100, 100, 0);
        CB->ajoute_zone(7, 2010, -1180, 100, 100, 0);
    }
    /* 0x7AE4 */
    if (ST(ST_LEVEL) == 2) {
        /* 0x7AF6 */
        if (ST(0xEB8) == 0) {
            CB->pos_perso(ST(ST_HERO), 1710, -1810, 0xDC43, 1, "ar", 8, 0);
            CB->pos_perso(P_DHORKAN11, 2060, -1260, 0x7B43, 1, "P11ATT01", 4, 0);
            CB->pos_perso(P_PHEDORIA, 1080, -2525, 0xC350, 1, "P26ARR03", 5, 0);
            CB->etat_objet("IM70", 1648, 18, -1886, 0xC000, 0x4000, 0x4000, 0, 2);
            CB->ajoute_zone(12, 2100, -1373, 310, 135, 0);
            /* 0x7BC8 */
            if (ST(ST_HERO) > 1) {
                CB->change_camera(2, CAM_LOCKED);
                CB->bloque(BLOQUE_ALL);
            }
        }
        /* 0x7BF5 */
        if (ST(0xEB8) == 1) {
            CB->pos_perso(ST(ST_HERO), 1896, -1937, 0xFB92, 1, "ar", 8, 0);
            CB->pos_perso(P_PHEDORIA, 1827, -1844, 0xF2C9, 1, "P26ARR03", 5, 0);
            CB->pos_perso(P_DHORKAN11, 2027, -777, 0, 1, "P11ATT03", 4, 0);
            CB->link_objet(P_DHORKAN11, "OB60", 28, 1);
            CB->etat_objet("IM70", 1670, 18, -1940, 0xC000, 0x4000, 0x9858, 0, 2);
            CB->retire_zone(1);
            CB->ajoute_zone(14, 1700, -1815, 55, 130, 1);
            CB->ajoute_zone(18, 1863, -543, 50, 50, 0);
            CB->ajoute_zone(6, 2120, -1230, 100, 100, 0);
            CB->ajoute_zone(7, 2010, -1180, 100, 100, 0);
        }
        /* 0x7D31 */
        if (ST(0xEB8) == 2) {
            CB->pos_perso(ST(ST_HERO), 1771, -1871, 0xDC43, 1, "ar", 8, 0);
            CB->etat_objet("IM70", 1648, 18, -1886, 0xC000, 0x4000, 0x4000, 0, 2);
            CB->ajoute_zone(6, 2120, -1230, 100, 100, 0);
            CB->ajoute_zone(7, 2010, -1180, 100, 100, 0);
        }
    }
    /* 0x7DCE */
    if (ST(ST_HERO) == P_ARKHAN1) {
        CB->ajoute_zone(11, 2050, -1500, 550, 100, 1);
    }
    /* 0x7DFE */
    if (ST(ST_VIE(P_DHORKAN11)) == VIE_DEAD && ST(0xEB8) < 2) {
        CB->pos_perso(P_DHORKAN11, CB->divers(DV_DESC_POS, P_DHORKAN11, 0), CB->divers(DV_DESC_POS, P_DHORKAN11, 1), CB->divers(DV_DESC_POS, P_DHORKAN11, 2), 2, "P11cad", 4, 0);
    }
    /* 0x7E72 */
    if (ST(0xB34) == 1 || ST(ST_LEVEL) == 46) {
        CB->vire_col(18, 0);
        ST(0xB34) = 1;
    }
    CB->load_anim("206OUV01", 0);
    CB->load_anim("206OUV02", 1);
    CB->load_anim("206OUV04", 2);
    ST(ST_START_POINT(6)) = 0;
    /* 0x7EEB */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 0 && ST(0xB98) == 2 && ST(ST_VIE(P_GUARDIAN4)) != VIE_DEAD) {
        ST(0xB98) = 0;
    }
    /* 0x7F54 */
    if (ST(0x338) == 0 && ST(0x348) == 0 && ST(0xAF8) == 1 && ST(0xB98) == 4) {
        ST(0x338) = 1;
        ST(0x348) = 1;
    }
    /* 0x7FBA */
    if (ST(0xEB8) < 2) {
        CB->divers(DV_MUSIC, 1, 0);
    }
}


/* trigger 01 — sub_1000A44F — action key at the door IM70: leave the room */
static void trig01(void) {
    /* 0xA45E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x4650)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA4A9 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0xA4BB */
            if (ST(0x358) == 0) {
                ST(0x358) = 2;
                CB->change_camera(2, CAM_LOCKED);
                CB->animation("SP11ATT02");
                CB->dialogue("L06D05G", P_DHORKAN11);
                CB->animation("P11_65535");
                CB->joindre(-P_DHORKAN11, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L06D05C", P_DHORKAN11, "SP11DIA65", "SP11ARR02_10");
                lieu_action_register("L06D05D", P_DHORKAN11, "SP11DIA57", "SP11ARR02_10");
                lieu_action_register("L06D05E", P_DHORKAN11, "SP11DIA58", "SP11ARR02_10");
                lieu_action_register("L06D05F", P_DHORKAN11, "SP11DIA64", "SP11ARR02_10");
                CB->change_camera(0, CAM_LOCKED);
                CB->animation("SG00OUV01");
                CB->animation("P00_65535");
                CB->anim_objet("IM70", 3, -40, 1500, 0);
                ST(ST_LEVEL) = 6;
                CB->bloque(BLOQUE_NONE);
                CB->divers_empile(DV_MUSIC, 1, 4);
                CB->change_piece(2, -1);
            } else {
                /* 0xA61B */
                if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 1720) {
                    CB->pos_perso(ST(ST_HERO), CB->divers(DV_DESC_POS, ST(ST_HERO), 0) + 20, CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 8, 0);
                }
                CB->animation("SG00OUV01");
                CB->animation("P00_65535");
                CB->anim_objet("IM70", 3, -40, 1500, 0);
                ST(ST_LEVEL) = 6;
                CB->bloque(BLOQUE_NONE);
                CB->divers_empile(DV_MUSIC, 1, 4);
                CB->change_piece(2, -1);
            }
        }
        /* 0xA6FB */
        if (ST(ST_HERO) >= 2) {
            /* 0xA70D */
            if (ST(0xF2C) == 2) {
                ST(0xF2C) = 0;
            }
            /* 0xA72E */
            if (ST(V_GUARD_FIGHT_DONE) < 2 && ST(0xEB8) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(0x740) > 1 && ST(0x740) < 5 && ST(0x73C) == 0) {
                ST(0x73C) = 1;
                CB->stop_joindre(-P_DHORKAN11);
                ST(0x70) = 3;
                CB->change_camera(2, CAM_LOCKED);
                CB->pos_perso(ST(ST_HERO), 1724, -1820, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 8, 0);
                lieu_action_register("L06D54A", P_DHORKAN11, "SP11DIA53", "SP11ARR02_10");
                lieu_action_register("L06D54B", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                lieu_action_register("L06D54C", P_DHORKAN11, "SP11DIA57", "SP11ARR02_10");
                lieu_action_register("L06D54D", P_DHORKAN11, "SP11DIA59", "SP11ARR02_10");
                lieu_action_register("L06D54E", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                CB->change_camera(2, CAM_AUTO);
                CB->animation("SG00OUV01");
                CB->animation("P00_65535");
                CB->anim_objet("IM70", 3, -40, 1500, 0);
                ST(ST_LEVEL) = 6;
                CB->bloque(BLOQUE_NONE);
                CB->divers_empile(DV_MUSIC, 1, 4);
                CB->change_piece(2, -1);
                return;   /* 0xA8F2 (the jmp at 0xA8F7 is dead code) */
            }
            CB->animation("SG00OUV01");
            CB->animation("P00_65535");
            CB->anim_objet("IM70", 3, -40, 1500, 0);
            ST(ST_LEVEL) = 6;
            CB->bloque(BLOQUE_NONE);
            /* 0xA94B */
            if (ST(0xEB8) < 2) {
                CB->divers_empile(DV_MUSIC, 1, 4);
            }
            CB->change_piece(2, -1);
        }
    }
}

/* trigger 02 — sub_1000A978 — zone 16 (the lock): Dhorkan's coin or stiletto works the gate */
static void trig02(void) {
    /* 0xA987 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(16)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, -17);
        /* 0xA9EF */
        if (ST(0xB34) == 0 || (ST(ST_PLAYER_WEAPON) != OB_STILETTO && ST(ST_PLAYER_WEAPON) != OB_COIN)) {
            /* 0xAA25 */
            if (ST(ST_PLAYER_WEAPON) != OB_STILETTO && ST(ST_PLAYER_WEAPON) != OB_COIN) {
                /* 0xAA49 */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    lieu_action_register("L06D09A", P_ARKHAN1, "SP01ACT46_500", "sar_10");
                }
                /* 0xAA74 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0xAA8E */
                    if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        lieu_action_register("L06D09B", P_ARKHAN2, "SP02ACT46_500", "sar_10");
                    } else {
                        lieu_action_register("L06D09B", P_ARKHAN2, "SP02ACT47_500", "sar_10");
                    }
                }
                /* 0xAAD7 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xAAF1 */
                    if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                        lieu_action_register("L06D09C", P_ARKHAN3, "SP03ACT46_500", "sar_10");
                    } else {
                        lieu_action_register("L06D09C", P_ARKHAN3, "SP03ACT47_500", "sar_10");
                    }
                }
                /* 0xAB3A */
                if (ST(0xEB8) != 2 && ST(V_ENGAGED_PERSONA) == 0) {
                    l06_taunt_a();
                }
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xACAC */
            if (ST(ST_PLAYER_WEAPON) == OB_STILETTO || ST(ST_PLAYER_WEAPON) == OB_COIN) {
                ST(0xB34) = 1;
                /* 0xACDF */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT37");
                }
                /* 0xACFC */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT37");
                }
                /* 0xAD19 */
                if (ST(ST_PLAYER_WEAPON) == OB_STILETTO) {
                    ST(0xF60) = 0;
                }
                /* 0xAD3A */
                if (ST(ST_PLAYER_WEAPON) == OB_COIN) {
                    ST(0xF60) = 1;
                }
                CB->modify_var(&ST(0x90), 10);
                CB->play_anim(0, 0);
                CB->pause_anim(1, 0, -1);
                CB->bruitage("L06B02", 1863, -543);
                CB->vire_col(18, 0);
                CB->animation("Sar_10");
                /* 0xADB8 */
                if (ST(0xEB8) < 2 && (ST(0xCA0) == 5 || ST(0xF24) > 2)) {
                    /* 0xADEE */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L06D669", P_ARKHAN2, "SG00DIA01", NULL);
                    }
                    /* 0xAE16 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L06D670", P_ARKHAN3, "SG00DIA01", NULL);
                    }
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->modify_var(&ST(0x90), 1);
                CB->evenement(0, 1000);
                return;
            }
        }
        /* 0xAE83 */
        if (ST(0xB34) >= 1) {
            /* 0xAE95 */
            if (ST(0xB34) == 1 && (ST(ST_PLAYER_WEAPON) == OB_COIN || ST(ST_PLAYER_WEAPON) == OB_STILETTO)) {
                ST(0xB34) = 2;
                CB->vire_col(18, 1);
                /* 0xAEEA */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT38");
                }
                /* 0xAF07 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT38");
                }
                /* 0xAF18: cmp ST(0xF60), 0 / jne to the next instruction — no effect */
                CB->modify_var(&ST(0x90), 10);
                CB->bruitage("L06B02", 1863, -543);
                CB->play_anim(1, 0);
                CB->pause_anim(0, 0, -1);
                CB->animation("Sar_1000");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->modify_var(&ST(0x90), 2);
                CB->evenement(0, 1000);
                return;
            }
            /* 0xAFC0 */
            if (ST(0xB34) == 2 && (ST(ST_PLAYER_WEAPON) == OB_STILETTO || ST(ST_PLAYER_WEAPON) == OB_COIN)) {
                CB->vire_col(18, 0);
                ST(0xB34) = 1;
                /* 0xB012 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ACT38");
                }
                /* 0xB02F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ACT38");
                }
                /* 0xB04C */
                if (ST(ST_PLAYER_WEAPON) == OB_STILETTO) {
                    ST(0xF60) = 0;
                }
                /* 0xB06D */
                if (ST(ST_PLAYER_WEAPON) == OB_COIN) {
                    ST(0xF60) = 1;
                }
                CB->modify_var(&ST(0x90), 10);
                CB->bruitage("L06B02", 1863, -543);
                CB->play_anim(0, 0);
                CB->pause_anim(1, 0, -1);
                CB->animation("Sar_1000");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->modify_var(&ST(0x90), 1);
                CB->evenement(0, 1000);
                return;
            }
        }
    }
}

/* trigger 03 — sub_1000B111 — zone 17: walk to the exit (event 53 chain) */
static void trig03(void) {
    /* 0xB120 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(17)) {
        ST(V_EVENT_53) = 0;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-ST(ST_HERO));
        ST(ST_SCRIPT_BUSY) = 1;
        CB->vire_col(11, 0);
        CB->vire_col(12, 0);
        CB->vire_col(19, 0);
        CB->vire_col(24, 0);
        CB->joindre(-ST(ST_HERO), 1800, -421, 0x5596, -1);
        CB->evenement(53, 2000);
    }
}

/* trigger 04 — sub_1000B1F2 — action key facing Dhorkan: the conversation tree */
static void trig04(void) {
    /* 0xB201 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_DHORKAN11)) {
        /* 0xB244 */
        if (ST(ST_VIE(P_DHORKAN11)) == 1) {
            /* 0xB256 */
            if (ST(0x94) == 0) {
                CB->change_camera(2, CAM_LOCKED);
            }
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0x70) = 3;
            CB->stop_joindre(-P_DHORKAN11);
            /* 0xB2A6 */
            if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2) || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                /* 0xB2FB */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    CB->evenement(7, -1);
                    /* 0xB31D */
                    if (ST(0x73C) == 0) {
                        /* 0xB32F */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L06D10A", P_ARKHAN1, "SG00DIA05", NULL);
                        }
                        /* 0xB357 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L06D10B", P_ARKHAN1, "SG00DIA05", NULL);
                        }
                        lieu_action_register("L06D10C", P_DHORKAN11, "SP11DIA59", "SP11ARR02_100");
                        lieu_action_register("L06D10D", P_DHORKAN11, "SP11DIA59", "SP11ARR02_100");
                        ST(0x73C) = 1;
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->evenement(7, 6000);
                        CB->modify_var(&ST(0x70), 1);
                        CB->modify_var(&ST(0x94), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xB419 */
                    if (ST(0x73C) == 1) {
                        /* 0xB42B */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L06D11A", P_ARKHAN1, "SG01DIA06", NULL);
                        }
                        /* 0xB453 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L06D11B", P_ARKHAN1, "SG01DIA06", NULL);
                        }
                        lieu_action_register("L06D12A", P_DHORKAN11, "SP11DIA54", "SP11ARR02");
                        lieu_action_register("L06D12B", P_DHORKAN11, "SP11DIA59", "SP11ARR02_10");
                        lieu_action_register("L06D10D", P_DHORKAN11, "SP11DIA59", "SP11ARR02_10");
                        ST(0x73C) = 2;
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->evenement(7, 2000);
                        CB->modify_var(&ST(0x70), 1);
                        CB->modify_var(&ST(0x94), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xB52E */
                    if (ST(0x73C) == 2) {
                        /* 0xB540 */
                        if (ST(ST_SPECIAL_MODE) == 1) {
                            lieu_action_register("L06D01A", P_ARKHAN1, "SG01DIA01", NULL);
                        }
                        /* 0xB568 */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L06D01B", P_ARKHAN1, "SG01DIA01", NULL);
                        }
                        lieu_action_register("L06D666", P_DHORKAN11, "SP11DIA59", "SP11ARR02_10");
                        ST(0x73C) = 2;
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->evenement(7, 2000);
                        CB->modify_var(&ST(0x70), 1);
                        CB->modify_var(&ST(0x94), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                }
                /* 0xB611 */
                if (ST(ST_HERO) == P_ARKHAN2 || ST(ST_HERO) == P_ARKHAN3) {
                    /* 0xB635 */
                    if (ST(0xB1C) == 0) {
                        /* 0xB647 */
                        if (ST(0x740) == 0) {
                            ST(0x740) = 1;
                            /* 0xB668 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0xB67A */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D13A", P_ARKHAN2, "SG00DIA07", NULL);
                                    lieu_action_register("L06D13B", P_ARKHAN2, "SG00DIA07", NULL);
                                    lieu_action_register("L06D13C", P_ARKHAN2, "SG00DIA08", NULL);
                                }
                                /* 0xB6CE */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D14A", P_ARKHAN2, "SG00DIA08", NULL);
                                    lieu_action_register("L06D14B", P_ARKHAN2, "SG00DIA07", NULL);
                                    lieu_action_register("L06D14C", P_ARKHAN2, "SG00DIA08", NULL);
                                }
                            }
                            /* 0xB722 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0xB734 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D63", P_ARKHAN2, "SG00DIA07", NULL);
                                }
                                /* 0xB75C */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D64", P_ARKHAN2, "SG00DIA07", NULL);
                                }
                            }
                            lieu_action_register("L06D45A", P_DHORKAN11, "SP11DIA51", "SP11ARR02");
                            lieu_action_register("L06D45B", P_DHORKAN11, "SP11DIA57", "SP11ARR02");
                            /* 0xB7B6 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L06D46A", P_ARKHAN2, "SG00DIA01", NULL);
                                lieu_action_register("L06D46B", P_ARKHAN2, "SG00DIA05", NULL);
                            }
                            /* 0xB7F4 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L06D47A", P_ARKHAN3, "SG00DIA01", NULL);
                            }
                            lieu_action_register("L06D65A", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                            lieu_action_register("L06D65B", P_DHORKAN11, "SP11DIA57", "SP11ARR02_10");
                            lieu_action_register("L06D65C", P_DHORKAN11, "SP11DIA58", "SP11ARR02_10");
                            lieu_action_register("L06D65D", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                            lieu_action_register("L06D65E", P_DHORKAN11, "SP11DIA57", "SP11ARR02_10");
                            lieu_action_register("L06D65F", P_DHORKAN11, "SP11DIA58", "SP11ARR02_10");
                            lieu_action_register("L06D65G", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x94), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->modify_var(&ST(0x70), 1);
                            return;
                        }
                        /* 0xB914 */
                        if (ST(0x740) == 1) {
                            ST(0x740) = 2;
                            /* 0xB935 */
                            if (ST(0xEB0) == 0) {
                                /* 0xB947 */
                                if (ST(0xF2C) == 1) {
                                    ST(0x7C) = 0;
                                }
                                /* 0xB965 */
                                if (ST(0xF2C) == 0) {
                                    ST(0x7C) = 1;
                                }
                            }
                            /* 0xB983 */
                            if (ST(0xEB0) != 0) {
                                /* 0xB995 */
                                if (ST(0xF2C) == 1) {
                                    ST(0xBC) = 0;
                                }
                                /* 0xB9B6 */
                                if (ST(0xF2C) == 0) {
                                    ST(0xBC) = 0x29A;
                                }
                            }
                            /* 0xB9D7 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0xB9E9 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D72A", P_ARKHAN2, "SG00DIA06", NULL);
                                    lieu_action_register("L06D72B", P_ARKHAN2, "SG00DIA07", NULL);
                                }
                                /* 0xBA27 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D73A", P_ARKHAN2, "SG00DIA07", NULL);
                                    lieu_action_register("L06D73B", P_ARKHAN2, "SG00DIA08", NULL);
                                }
                            }
                            /* 0xBA65 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0xBA77 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D74", P_ARKHAN3, "SG00DIA06", NULL);
                                }
                                /* 0xBA9F */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D75", P_ARKHAN3, "SG00DIA08", NULL);
                                }
                            }
                            lieu_action_register("L06D76A", P_DHORKAN11, "SP11DIA53", "SP11ARR02_10");
                            lieu_action_register("L06D76B", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                            lieu_action_register("L06D76C", P_DHORKAN11, "SP11DIA59", "SP11ARR02_10");
                            lieu_action_register("L06D76D", P_DHORKAN11, "SP11DIA61", "SP11ARR02_10");
                            lieu_action_register("L06D76E", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x70), 1);
                            CB->modify_var(&ST(0x94), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xBB8D */
                        if ((ST(0x740) == 2 || ST(0x740) == 3) && ST(0x7C) == 0 && ST(0xBC) == 0) {
                            /* 0xBBD2 */
                            if (ST(ST_VIE(P_THANANDAR24)) < 2) {
                                ST(0x740) = 3;
                                ST(0xBC) = 0x29A;
                                /* 0xBC02 */
                                if (ST(0x730) != 0 || ST(0xEB0) == 0) {
                                    ST(0x7C) = 1;
                                }
                                /* 0xBC32 */
                                if (ST(0xF2C) == 0) {
                                    /* 0xBC44 */
                                    if (ST(ST_HERO) == P_ARKHAN2) {
                                        lieu_action_register("L06D48A", P_ARKHAN2, "SG00DIA01", NULL);
                                    }
                                    /* 0xBC6C */
                                    if (ST(ST_HERO) == P_ARKHAN3) {
                                        lieu_action_register("L06D48B", P_ARKHAN2, "SG00DIA01", NULL);
                                    }
                                    lieu_action_register("L06D51A", P_DHORKAN11, "SP11DIA55", "SP11ARR02_10");
                                    lieu_action_register("L06D51B", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                                    lieu_action_register("L06D51C", P_DHORKAN11, "SP11DIA57", "SP11ARR02_10");
                                }
                                /* 0xBCDF */
                                if (ST(0xF2C) == 1 && ST(0x4FC) == 0) {
                                    ST(0x4FC) = 1;
                                    ST(0xF2C) = 2;
                                    /* 0xBD21 */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        /* 0xBD33 */
                                        if (ST(ST_HERO) == P_ARKHAN2) {
                                            lieu_action_register("L06D204A", P_ARKHAN2, "SG00DIA01", NULL);
                                            lieu_action_register("L06D204B", P_ARKHAN2, "SG00DIA01", NULL);
                                        }
                                        /* 0xBD71 */
                                        if (ST(ST_HERO) == P_ARKHAN3) {
                                            lieu_action_register("L06D205A", P_ARKHAN2, "SG00DIA01", NULL);
                                            lieu_action_register("L06D205B", P_ARKHAN2, "SG00DIA01", NULL);
                                        }
                                        lieu_action_register("L06D206A", P_DHORKAN11, "SP11DIA53", "SP11ARR02_10");
                                        lieu_action_register("L06D206B", P_DHORKAN11, "SP11DIA54", "SP11ARR02_10");
                                        lieu_action_register("L06D206C", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                                    }
                                    /* 0xBDFA */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        /* 0xBE0C */
                                        if (ST(ST_HERO) == P_ARKHAN2) {
                                            lieu_action_register("L06D207A", P_ARKHAN2, "SG00DIA01", NULL);
                                            lieu_action_register("L06D207B", P_ARKHAN2, "SG00DIA02", NULL);
                                            lieu_action_register("L06D207C", P_ARKHAN2, "SG00DIA03", NULL);
                                        }
                                        /* 0xBE60 */
                                        if (ST(ST_HERO) == P_ARKHAN3) {
                                            lieu_action_register("L06D208A", P_ARKHAN3, "SG00DIA01", NULL);
                                            lieu_action_register("L06D208B", P_ARKHAN3, "SG00DIA02", NULL);
                                            lieu_action_register("L06D208C", P_ARKHAN3, "SG00DIA03", NULL);
                                        }
                                        lieu_action_register("L06D209A", P_DHORKAN11, "SP11DIA53", "SP11ARR02_10");
                                        lieu_action_register("L06D209B", P_DHORKAN11, "SP11DIA54", "SP11ARR02_10");
                                        lieu_action_register("L06D209C", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                                    }
                                }
                                /* 0xBEFF */
                                if (ST(0xF2C) == 1 && ST(0x4FC) == 1) {
                                    ST(0xF2C) = 2;
                                    /* 0xBF32 */
                                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L06D210", P_ARKHAN2, "SG00DIA01", NULL);
                                    }
                                    /* 0xBF6C */
                                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("L06D211", P_ARKHAN2, "SG00DIA01", NULL);
                                    }
                                    /* 0xBFA6 */
                                    if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L06D212", P_ARKHAN3, "SG00DIA01", NULL);
                                    }
                                    /* 0xBFE0 */
                                    if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("L06D213", P_ARKHAN3, "SG00DIA01", NULL);
                                    }
                                    lieu_action_register("L06D214A", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                                    lieu_action_register("L06D214B", P_DHORKAN11, "SP11DIA58", "SP11ARR02_10");
                                }
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x94), 1);
                                CB->bloque(BLOQUE_NONE);
                                CB->modify_var(&ST(0x70), 1);
                                return;
                            }
                            /* 0xC095 */
                            if (ST(ST_VIE(P_THANANDAR24)) == VIE_DEAD) {
                                ST(0x740) = 4;
                                /* 0xC0B6 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L06D49A", P_ARKHAN2, "SG00DIA05", NULL);
                                    lieu_action_register("L06D49B", P_ARKHAN2, "SG00DIA08", NULL);
                                    lieu_action_register("L06D49C", P_ARKHAN2, "SG00DIA06", NULL);
                                }
                                /* 0xC10A */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L06D50A", P_ARKHAN3, "SG00DIA05", NULL);
                                }
                                lieu_action_register("L06D77A", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
                                lieu_action_register("L06D77B", P_DHORKAN11, "SP11DIA57", "SP11ARR02_10");
                                /* 0xC164 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    lieu_action_register("L06D78", P_ARKHAN2, "SG00DIA05", NULL);
                                }
                                /* 0xC18C */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L06D79", P_ARKHAN3, "SG00DIA05", NULL);
                                }
                                lieu_action_register("L06D80A", P_DHORKAN11, "SP11DIA62", "SP11ARR02_10");
                                lieu_action_register("L06D80B", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                                lieu_action_register("L06D80C", P_DHORKAN11, "SP11DIA61", "SP11ARR02_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x94), 1);
                                CB->bloque(BLOQUE_NONE);
                                CB->modify_var(&ST(0x70), 1);
                                return;
                            }
                        }
                        /* 0xC248 */
                        if ((ST(0x740) == 2 || ST(0x740) == 3) && ST(0xEB0) != 0 && ST(0x730) == 0 && ST(0x7C) == 0) {
                            ST(0x730) = 1;
                            ST(0x7C) = 1;
                            /* 0xC2BA */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0xC2CC */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D21A", P_ARKHAN2, "SG00DIA06", NULL);
                                }
                                /* 0xC2F4 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D21B", P_ARKHAN2, "SG00DIA06", NULL);
                                }
                            }
                            /* 0xC31C */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0xC32E */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D21C", P_ARKHAN3, "SG00DIA05", NULL);
                                }
                                /* 0xC356 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D21D", P_ARKHAN3, "SG00DIA05", NULL);
                                }
                            }
                            lieu_action_register("L06D22A", P_DHORKAN11, "SP11DIA53", "SP11ARR02");
                            lieu_action_register("L06D22B", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x70), 1);
                            CB->modify_var(&ST(0x94), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xC3F9 */
                        if ((ST(0x740) == 2 || ST(0x740) == 3) && ST(0x7C) >= 1) {
                            /* 0xC42C */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                lieu_action_register("L06D52A", P_ARKHAN2, "SG00DIA07", "Sar_10");
                            }
                            /* 0xC457 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L06D52B", P_ARKHAN3, "SG00DIA07", "Sar_10");
                            }
                            /* 0xC47F */
                            if (ST(0x7C) == 1) {
                                lieu_action_register("L06D53A", P_DHORKAN11, "SP11DIA54", "SP11ARR02_10");
                                lieu_action_register("L06D53B", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                            } else {
                                lieu_action_register("L06D80C", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                            }
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x94), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->modify_var(&ST(0x70), 1);
                            CB->modify_var(&ST(0x7C), 2);
                            return;
                        }
                        /* 0xC53E */
                        if (ST(0x740) == 4) {
                            ST(0x740) = 5;
                            /* 0xC55F */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0xC571 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D15A", P_ARKHAN2, "SG00DIA01", NULL);
                                }
                                /* 0xC599 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D81A", P_ARKHAN2, "SG00DIA01", NULL);
                                    lieu_action_register("L06D81B", P_ARKHAN2, "SG00DIA03", NULL);
                                }
                            }
                            /* 0xC5D7 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                /* 0xC5E9 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D15C", P_ARKHAN3, "SG00DIA01", NULL);
                                }
                                /* 0xC611 */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D15D", P_ARKHAN3, "SG00DIA01", NULL);
                                }
                            }
                            lieu_action_register("L06D16A", P_DHORKAN11, "SP11DIA59", "SP11ARR02");
                            lieu_action_register("L06D16B", P_DHORKAN11, "SP11DIA60", "SP11ARR02");
                            lieu_action_register("L06D16C", P_DHORKAN11, "SP11DIA61", "SP11ARR02");
                            lieu_action_register("L06D16D", P_DHORKAN11, "SP11DIA59", "SP11ARR02");
                            lieu_action_register("L06D16E", P_DHORKAN11, "SP11DIA60", "SP11ARR02");
                            lieu_action_register("L06D16F", P_DHORKAN11, "SP11DIA61", "SP11ARR02_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x70), 1);
                            CB->modify_var(&ST(0x94), 1);
                            CB->bloque(BLOQUE_NONE);
                            return;
                        }
                        /* 0xC718 */
                        if (ST(0x740) == 5) {
                            /* 0xC72A */
                            if (ST(0x72C) == 0) {
                                ST(0x72C) = 1;
                                /* 0xC74B */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    /* 0xC75D */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L06D17A", P_ARKHAN2, "SG00DIA08", NULL);
                                    }
                                    /* 0xC785 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("L06D17B", P_ARKHAN2, "SG00DIA08", NULL);
                                    }
                                }
                                /* 0xC7AD */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    lieu_action_register("L06D17C", P_ARKHAN3, "SG00DIA08", NULL);
                                }
                                lieu_action_register("L06D18A", P_DHORKAN11, "SP11DIA64", "SP11ARR02");
                                lieu_action_register("L06D18B", P_DHORKAN11, "SP11DIA59", "SP11ARR02_10");
                                lieu_action_register("L06D18C", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                CB->modify_var(&ST(0x70), 1);
                                CB->modify_var(&ST(0x94), 1);
                                CB->bloque(BLOQUE_NONE);
                                return;
                            } else {
                                /* 0xC86E */
                                if (ST(V_EVENT_7) != 0) {
                                    ST(V_EVENT_7) = 100;
                                    CB->evenement(7, -1);
                                }
                                /* 0xC89F */
                                if (ST(V_EVENT_7) == 0) {
                                    ST(V_EVENT_7) = 3;
                                }
                                /* 0xC8C0 */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    /* 0xC8D2 */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L06D19A", P_ARKHAN2, "SG00DIA02", NULL);
                                        lieu_action_register("L06D19B", P_ARKHAN2, "SG00DIA08", NULL);
                                    }
                                    /* 0xC910 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("L06D19C", P_ARKHAN2, "SG00DIA08", NULL);
                                    }
                                }
                                /* 0xC938 */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    /* 0xC94A */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L06D19D", P_ARKHAN3, "SG00DIA08", NULL);
                                    }
                                    /* 0xC972 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("L06D19E", P_ARKHAN3, "SG00DIA08", NULL);
                                    }
                                }
                                /* 0xC99A */
                                if (ST(V_EVENT_7) == 3) {
                                    lieu_action_register("L06D20", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->modify_var(&ST(0x70), 1);
                                    CB->modify_var(&ST(0x94), 1);
                                    CB->change_camera(0, CAM_AUTO);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->evenement(7, 15000);
                                    return;
                                }
                                /* 0xCA2B */
                                if (ST(V_EVENT_7) == 100) {
                                    ST(V_EVENT_7) = 5;
                                    ST(0x74) = 4;
                                    return;
                                }
                            }
                        }
                    }
                    /* 0xCA5D */
                    if (ST(0xB1C) != 0 && ST(0x730) == 0) {
                        ST(0x740) = 5;
                        ST(0x730) = 1;
                        /* 0xCA9F */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            /* 0xCAB1 */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L06D21A", P_ARKHAN2, "SG00DIA06", NULL);
                            }
                            /* 0xCAD9 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L06D21B", P_ARKHAN2, "SG00DIA06", NULL);
                            }
                        }
                        /* 0xCB01 */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            /* 0xCB13 */
                            if (ST(ST_SPECIAL_MODE) == 0) {
                                lieu_action_register("L06D21C", P_ARKHAN3, "SG00DIA06", NULL);
                            }
                            /* 0xCB3B */
                            if (ST(ST_SPECIAL_MODE) == 1) {
                                lieu_action_register("L06D21D", P_ARKHAN3, "SG00DIA06", NULL);
                            }
                        }
                        lieu_action_register("L06D22A", P_DHORKAN11, "SP11DIA53", "SP11ARR02");
                        lieu_action_register("L06D22B", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->modify_var(&ST(0x70), 1);
                        CB->modify_var(&ST(0x94), 1);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0xCBDE */
                    if (ST(0x730) == 1) {
                        /* 0xCBF0 */
                        if (ST(0x72C) == 0) {
                            ST(0x72C) = 1;
                            /* 0xCC11 */
                            if (ST(ST_HERO) == P_ARKHAN2) {
                                /* 0xCC23 */
                                if (ST(ST_SPECIAL_MODE) == 1) {
                                    lieu_action_register("L06D17A", P_ARKHAN2, "SG00DIA08", NULL);
                                }
                                /* 0xCC4B */
                                if (ST(ST_SPECIAL_MODE) == 0) {
                                    lieu_action_register("L06D17B", P_ARKHAN2, "SG00DIA08", NULL);
                                }
                            }
                            /* 0xCC73 */
                            if (ST(ST_HERO) == P_ARKHAN3) {
                                lieu_action_register("L06D17C", P_ARKHAN3, "SG00DIA08", NULL);
                            }
                            lieu_action_register("L06D18A", P_DHORKAN11, "SP11DIA64", "SP11ARR02");
                            lieu_action_register("L06D18B", P_DHORKAN11, "SP11DIA59", "SP11ARR02_10");
                            lieu_action_register("L06D18C", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                            CB->modify_var(&ST(0x94), 1);
                            CB->bloque(BLOQUE_NONE);
                            CB->modify_var(&ST(0x70), 1);
                            return;
                        }
                        /* 0xCD2F */
                        if (ST(0x72C) == 1) {
                            /* 0xCD41 */
                            if (ST(0xB1C) == 1) {
                                /* 0xCD53 */
                                if (ST(0x944) == 0) {
                                    ST(0x944) = 1;
                                    /* 0xCD74 */
                                    if (ST(ST_HERO) == P_ARKHAN2) {
                                        lieu_action_register("L06D23A", P_ARKHAN2, "SG00DIA04", NULL);
                                    }
                                    /* 0xCD9C */
                                    if (ST(ST_HERO) == P_ARKHAN3) {
                                        lieu_action_register("L06D23B", P_ARKHAN3, "SG00DIA04", NULL);
                                    }
                                    lieu_action_register("L06D24", P_DHORKAN11, "SP11DIA51", "SP11ARR02_10");
                                    /* 0xCDDD */
                                    if (ST(ST_HERO) == P_ARKHAN2) {
                                        /* 0xCDEF */
                                        if (ST(ST_SPECIAL_MODE) == 1) {
                                            lieu_action_register("L06D25A", P_ARKHAN2, "SG00DIA07", NULL);
                                        }
                                        /* 0xCE17 */
                                        if (ST(ST_SPECIAL_MODE) == 0) {
                                            lieu_action_register("L06D25B", P_ARKHAN2, "SG00DIA07", NULL);
                                        }
                                    }
                                    /* 0xCE3F */
                                    if (ST(ST_HERO) == P_ARKHAN3) {
                                        lieu_action_register("L06D25D", P_ARKHAN3, "SG00DIA07", NULL);
                                    }
                                    CB->change_camera(0, CAM_LOCKED);
                                    lieu_action_register("L06D26A", P_DHORKAN11, "SP11ATT60", "SP11ARR02");
                                    lieu_action_register("L06D26B", P_DHORKAN11, "SP11DIA59", "SP11ARR02");
                                    lieu_action_register("L06D26C", P_DHORKAN11, "SP11DIA62", "SP11ARR02");
                                    CB->change_camera(1, CAM_LOCKED);
                                    lieu_action_register("L06D26D", P_DHORKAN11, "SP11DIA58", "SP11ARR02");
                                    lieu_action_register("L06D26E", P_DHORKAN11, "SP11DIA57", "SP11ARR02");
                                    lieu_action_register("L06D26F", P_DHORKAN11, "SP11DIA54", "SP11ARR02");
                                    lieu_action_register("L06D26G", P_DHORKAN11, "SP11DIA62", "SP11ARR02");
                                    CB->change_camera(2, CAM_LOCKED);
                                    lieu_action_register("L06D26H", P_DHORKAN11, "SP11DIA53", "SP11ARR02");
                                    lieu_action_register("L06D26I", P_DHORKAN11, "SP11DIA55", "SP11ARR02");
                                    lieu_action_register("L06D26J", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                                    CB->modify_var(&ST(0x94), 1);
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->modify_var(&ST(0x70), 1);
                                    return;
                                }
                                /* 0xCFD7 */
                                if (ST(0x944) == 1) {
                                    /* 0xCFE9 */
                                    if (ST(ST_HERO) == P_ARKHAN2) {
                                        lieu_action_register("L06D27", P_ARKHAN2, "SG00DIA08", NULL);
                                    }
                                    /* 0xD011 */
                                    if (ST(ST_HERO) == P_ARKHAN3) {
                                        lieu_action_register("L06D667A", P_ARKHAN3, "SG00DIA08", NULL);
                                        lieu_action_register("L06D667B", P_ARKHAN3, "SG00DIA08", NULL);
                                    }
                                    lieu_action_register("L06D28A", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                                    lieu_action_register("L06D28B", P_DHORKAN11, "SP11DIA61", "SP11ARR02_10");
                                    CB->modify_var(&ST(0x94), 1);
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->bloque(BLOQUE_NONE);
                                    return;
                                }
                            } else {
                                /* 0xD0BB */
                                if (ST(V_EVENT_7) != 0) {
                                    ST(V_EVENT_7) = 100;
                                    CB->evenement(7, -1);
                                }
                                /* 0xD0EC */
                                if (ST(V_EVENT_7) == 0) {
                                    ST(V_EVENT_7) = 3;
                                }
                                /* 0xD10D */
                                if (ST(ST_HERO) == P_ARKHAN2) {
                                    /* 0xD11F */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L06D19A", P_ARKHAN2, "SG00DIA08", NULL);
                                    }
                                    /* 0xD147 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("L06D19B", P_ARKHAN2, "SG00DIA08", NULL);
                                    }
                                }
                                /* 0xD16F */
                                if (ST(ST_HERO) == P_ARKHAN3) {
                                    /* 0xD181 */
                                    if (ST(ST_SPECIAL_MODE) == 1) {
                                        lieu_action_register("L06D19C", P_ARKHAN3, "SG00DIA08", NULL);
                                    }
                                    /* 0xD1A9 */
                                    if (ST(ST_SPECIAL_MODE) == 0) {
                                        lieu_action_register("L06D19D", P_ARKHAN3, "SG00DIA08", NULL);
                                    }
                                }
                                /* 0xD1D1 */
                                if (ST(V_EVENT_7) == 3) {
                                    lieu_action_register("L06D20", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
                                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                                    CB->modify_var(&ST(0x70), 1);
                                    CB->modify_var(&ST(0x94), 1);
                                    CB->change_camera(0, CAM_AUTO);
                                    CB->bloque(BLOQUE_NONE);
                                    CB->evenement(7, 15000);
                                    return;
                                }
                                /* 0xD262 */
                                if (ST(V_EVENT_7) == 100) {
                                    ST(V_EVENT_7) = 5;
                                    ST(0x74) = 4;
                                    return;
                                }
                            }
                        }
                    }
                }
            }
            /* 0xD294 */
            if (ST(ST_PLAYER_WEAPON) != 0 && !CB->divers(DV_CAN_FIGHT, 0, 2) && ST(ST_PLAYER_WEAPON) != ST(V_LAST_WEAPON)) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                /* 0xD2EB */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    lieu_action_register("L06D200", P_ARKHAN1, "SG00PRE80", "SG00PRE81");
                }
                /* 0xD316 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("LCOD18", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
                }
                /* 0xD341 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("LCOD19", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
                }
                lieu_action_register("L06D203", P_DHORKAN11, "SP11DIA54", "SP11ARR02_10");
                CB->animation("G00PRE82");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->modify_var(&ST(0x94), 1);
                CB->bloque(BLOQUE_NONE);
                CB->modify_var(&ST(0x70), 1);
            }
        }
        /* 0xD3DF */
        if (ST(ST_VIE(P_DHORKAN11)) == VIE_DEAD) {
            CB->fouille(P_DHORKAN11);
        }
    }
    /* 0xD3F9 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_DHORKAN11 && CB->direction_perso(P_DHORKAN11) && ST(0xE0) == 0) {
        ST(0xE0) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD468 */
        if (ST(0xB1C) == 0) {
            /* 0xD47A */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("L06D66", P_ARKHAN1);
            }
            /* 0xD49C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L06D67", P_ARKHAN2);
            }
            /* 0xD4BE */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L06D68", P_ARKHAN3);
            }
        }
        /* 0xD4E0 */
        if (ST(0xB1C) != 0) {
            /* 0xD4F2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L06D69", P_ARKHAN2);
            }
            /* 0xD514 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L06D70", P_ARKHAN3);
            }
        }
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xD53B */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0x734) = 1;
        CB->evenement(7, -1);
        CB->stop_joindre(-P_DHORKAN11);
        ST(V_ENGAGED_PERSONA) = P_DHORKAN11;
        ST(V_PLAYER_STRIKE) = -1;
        CB->retire_zone(12);
        CB->animation("SP11ESA14");
        CB->animation("P11_65535");
        lieu_action_register("L06D32", P_DHORKAN11, "SP11DIA60", "SP11GAR26_10");
        CB->animation("P11_65535");
        CB->bloque(BLOQUE_NONE);
        CB->combat(P_DHORKAN11, 0);
    }
}

/* trigger 06 — sub_1000D616 — object OB21A (action key: first look, then comment) */
static void trig06(void) {
    /* 0xD625 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x7D00)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD670 */
        if (ST(0x5CC) == 0) {
            ST(0x5CC) = 1;
            CB->animation("SG00PRE07");
            CB->etat_objet("OB21A", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xD6D8 */
        if (ST(0x5CC) == 1) {
            /* 0xD6EA */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L06D55C", P_ARKHAN2, "SG00PRE03", NULL);
            }
            /* 0xD712 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L06D55D", P_ARKHAN3, "SG00PRE03", NULL);
            }
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 07 — sub_1000D75C — object OB93 (action key: first look, then comment) */
static void trig07(void) {
    /* 0xD76B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x7D00)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xD7B6 */
        if (ST(0x6EC) == 0) {
            ST(0x6EC) = 1;
            CB->animation("SG00PRE07");
            CB->etat_objet("OB93", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xD81E */
        if (ST(0x6EC) == 1) {
            /* 0xD830 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L06D55C", P_ARKHAN2, "SG00PRE03", NULL);
            }
            /* 0xD858 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L06D55D", P_ARKHAN3, "SG00PRE03", NULL);
            }
            CB->animation("ar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 11 — sub_1000D8C3 — zone 11: one-shot, removes itself */
static void trig11(void) {
    /* 0xD8D2 */
    if (ST(0x6C) == 0) {
        ST(0x6C) = 1;
        CB->retire_zone(11);
    }
}

/* trigger 12 — sub_1000D8F4 — zone 12: talk to Dhorkan (trigger 4) */
static void trig12(void) {
    /* 0xD907 */
    if (CB->direction_perso(P_DHORKAN11) && ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0) {
        trig04();
    }
}

/* trigger 13 — sub_1000D935 — zone 13: action key, Dhorkan taunt */
static void trig13(void) {
    /* 0xD944 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xB798)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD98F */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0xD9A1 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ATT84");
            }
            /* 0xD9D0 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xD9EA */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ATT84");
                } else {
                    CB->animation("SP02ATT85");
                }
            }
            /* 0xDA17 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xDA31 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ATT84");
                } else {
                    CB->animation("SP03ATT85");
                }
            }
        }
        /* 0xDA5E */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xDA70 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ATT86");
            }
            /* 0xDA9F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0xDAB9 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ATT86");
                } else {
                    CB->animation("SP02ATT87");
                }
            }
            /* 0xDAE6 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0xDB00 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT86");
                } else {
                    CB->animation("SP03ATT87");
                }
            }
        }
        /* 0xDB2D */
        if (ST(0xEB8) < 2 && ST(V_ENGAGED_PERSONA) == 0) {
            l06_taunt_a();
        }
        CB->animation("ar_200");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 14 — sub_1000DCA3 — zone 14: Phedoria calls the hero back, or leave */
static void trig14(void) {
    CB->retire_zone(14);
    /* 0xDCC0 */
    if (ST(0x88) == 0 && ST(0xB34) != 1) {
        CB->bloque(BLOQUE_ALL);
        lieu_action_register("L06D61A", P_PHEDORIA, "SP26DIA60", "SP26ARR03_10");
        lieu_action_register("L06D61B", P_PHEDORIA, "SP26DIA56", "SP26ARR03_10");
        CB->evenement(2, 3000);
        CB->divers(DV_RUN, P_PHEDORIA, 1);
        CB->joindre(-P_PHEDORIA, 2015, -761, -1, -1);
    }
    /* 0xDD5C */
    if (ST(0x88) == 1) {
        ST(ST_LEVEL) = 6;
        CB->change_piece(2, -1);
    }
}

/* trigger 15 — sub_1000DD83 — zone 15: LCOD06..08 hero comment, Dhorkan taunt */
static void trig15(void) {
    /* 0xDD92 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x2710)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDDEC */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("LCOD06", P_ARKHAN1, "SG00OUV09", NULL);
        }
        /* 0xDE14 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD07", P_ARKHAN2, "SG00OUV09", NULL);
        }
        /* 0xDE3C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD08", P_ARKHAN3, "SG00OUV09", NULL);
        }
        /* 0xDE64 */
        if (ST(0xEB8) < 2 && ST(V_ENGAGED_PERSONA) == 0) {
            l06_taunt_a();
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 41 — sub_10007FD7 — Dhorkan intro scene (slot 41) */
static void trig41(void) {
    /* 0x7FE9 */
    if (ST(0xEB8) < 2) {
        CB->divers_empile(DV_MUSIC, 1, 1);
    }
    /* 0x800D */
    if (ST(0xEB8) == 0 && ST(0x734) == 1 && ST(V_GUARD_FIGHT_DONE) < 2) {
        CB->change_camera(2, CAM_LOCKED);
        CB->bloque(BLOQUE_ALL);
        ST(0x78) = 1;
        ST(0x734) = 0;
        CB->retire_zone(11);
        CB->animation("SP11ATT02");
        CB->animation("P11_65535");
        lieu_action_register("L06D03A", P_DHORKAN11, "SP11DIA54", "SP11ARR02_10");
        lieu_action_register("L06D03B", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
        CB->change_camera(0, CAM_LOCKED);
        CB->change_camera(0, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
        CB->modify_var(&ST(0x70), 1);
        /* 0x8102 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->modify_var(&ST(V_EVENT_7), 3);
            ST(0x74) = 2;
        }
    }
    /* 0x8137 */
    if (ST(ST_HERO) >= 2 && ST(0xEB8) == 0 && ST(V_GUARD_FIGHT_DONE) < 2 && ST(0x78) == 0) {
        /* 0x817C */
        if (ST(0x740) == 0) {
            ST(0x73C) = 0;
            CB->animation("SP11ATT04_100");
            CB->animation("P11_65535");
            CB->dialogue("L06D43A", P_DHORKAN11);
            CB->animation("SP11ATT02");
            CB->animation("P11_65535");
            lieu_action_register("L06D43B", P_DHORKAN11, "P11DIA50", "SP11ARR02_10");
            CB->change_camera(0, CAM_AUTO);
            CB->bloque(BLOQUE_NONE);
            CB->modify_var(&ST(0x70), 1);
        }
        /* 0x821B */
        if (ST(0x740) >= 1) {
            CB->animation("SP11ATT02");
            CB->animation("P11_65535");
            /* 0x8243 */
            if (ST(0x740) == 1) {
                lieu_action_register("L06D43C", P_DHORKAN11, "SP11DIA59", "SP11ARR02_10");
            }
            /* 0x826E */
            if (ST(0x740) > 1 && ST(0x740) < 5) {
                lieu_action_register("L06D44A", P_DHORKAN11, "SP11DIA55", "SP11ARR02_10");
                lieu_action_register("L06D44B", P_DHORKAN11, "SP11DIA56", "SP11ARR02_10");
            }
            /* 0x82C4 */
            if (ST(0x740) >= 5) {
                lieu_action_register("L06D44C", P_DHORKAN11, "SP11DIA54", "SP11ARR02");
                lieu_action_register("L06D44D", P_DHORKAN11, "SP11DIA60", "SP11ARR02_10");
            }
            CB->change_camera(0, CAM_AUTO);
            CB->bloque(BLOQUE_NONE);
            CB->modify_var(&ST(0x70), 1);
        }
    }
    /* 0x8331 */
    if (ST(0xEB8) == 1 && ST(V_GUARD_FIGHT_DONE) < 2) {
        ST(0xEB8) = 2;
        ST(V_EVENT15_STEP) = 1;
    }
}
