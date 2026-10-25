/*
 * lieu24_trig.c - Native hand-port of the LIEU24.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu24.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu24_int.h"

static void trig01(void); static void trig03(void); static void trig04(void);
static void trig08(void); static void trig09(void); static void trig12(void);
static void trig13(void); static void trig15(void); static void trig16(void);
static void trig17(void); static void trig18(void); static void trig19(void);
static void trig20(void); static void trig21(void); static void trig22(void);
static void trig23(void); static void trig24(void); static void trig25(void);
static void trig26(void); static void trig27(void); static void trig28(void);
static void trig29(void); static void trig30(void); static void trig31(void);
static void trig32(void); static void trig33(void); static void trig35(void);
static void trig39(void); static void trig40(void); static void trig41(void);
static void trig_noop(void);   /* the empty stub slots */

/* dword_10021BB0[42] - filled at INIT with the 5-byte jmp thunks; static
 * here. A slot is "stub" when it is the empty handler shared with LIEU01
 * (slots 2, 5, 6, 7, 10, 14, 34, 36, 37, 38). */
static void (*const g_trigger[42])(void) = {
    l24_trig00, trig01, trig_noop, trig03, trig04, trig_noop, trig_noop, trig_noop,
    trig08, trig09, trig_noop, l24_trig11, trig12, trig13, trig_noop, trig15,
    trig16, trig17, trig18, trig19, trig20, trig21, trig22, trig23,
    trig24, trig25, trig26, trig27, trig28, trig29, trig30, trig31,
    trig32, trig33, trig_noop, trig35, trig_noop, trig_noop, trig_noop, trig39,
    trig40, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10021BB0]). */
void l24_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 - sub_100073F0 - level setup: start-point presets, event resets, zones, actor placement */
void l24_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(24)) == 1) {
        ST(ST_LEVEL) = 27;
        ST(0xAF4) = 1;
        ST(0xE78) = 100;
        ST(ST_HERO) = P_ARKHAN2;
    }
    /* 0x744D */
    if (ST(ST_START_POINT(24)) == 2) {
        ST(0xBA8) = 2;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 23;
        ST(ST_VIE(P_THANANDAR24)) = 1;
        ST(0xE78) = 100;
        CB->pos_perso(P_GUARDIAN16, 6100, -4210, 0, 2, "P56cad", 9, 0);
    }
    /* 0x74C9 */
    if (ST(ST_START_POINT(24)) == 3) {
        ST(0xBA8) = 3;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 23;
        ST(ST_VIE(P_THANANDAR24)) = VIE_DEAD;
        ST(0xE78) = 100;
        CB->pos_perso(P_THANANDAR24, 6093, -4043, 0x7BAC, 2, "P24cad", 8, 0);
        CB->pos_perso(P_GUARDIAN16, 6100, -4210, 0, 2, "P56cad", 9, 0);
    }
    /* 0x756A */
    if (ST(ST_START_POINT(24)) == 4) {
        ST(0xBA8) = 4;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 23;
        ST(ST_VIE(P_THANANDAR24)) = 1;
        ST(0xE78) = 100;
        ST(0xA5C) = 1;
        CB->pos_perso(P_GUARDIAN16, 6100, -4210, 0, 2, "P56cad", 9, 0);
    }
    /* 0x75F5 */
    if (ST(ST_START_POINT(24)) == 5) {
        ST(0xBA8) = 5;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xE78) = 100;
        ST(ST_LEVEL) = 23;
        ST(ST_VIE(P_THANANDAR24)) = 1;
        ST(0xA5C) = 1;
    }
    /* 0x765E */
    if (ST(ST_START_POINT(24)) == 6) {
        ST(0xBA8) = 6;
        ST(ST_HERO) = P_ARKHAN2;
        ST(ST_LEVEL) = 23;
        ST(ST_VIE(P_THANANDAR24)) = 1;
        ST(0xE78) = 100;
    }
    /* 0x76B8 */
    if (ST(ST_START_POINT(24)) == 7) {
        ST(0xBA8) = 8;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xE14) = 4;
        ST(ST_LEVEL) = 23;
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
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_PRE90_OBJECT) = 0;
    ST(V_LAST_WEAPON) = 0;
    ST(V_EVENT_0) = 0;
    ST(V_EVENT_1) = 0;
    ST(V_EVENT_2) = 0;
    ST(V_EVENT_33) = 0;
    CB->ajoute_zone(3, 6745, -4024, 85, 80, 0);
    CB->ajoute_zone(12, 6666, -4440, 95, 85, 0);
    CB->ajoute_zone(13, 6803, -4426, 115, 115, 0);
    CB->ajoute_zone(14, 6584, -4522, 10, 10, 0);
    CB->ajoute_zone(15, 6580, -4481, 135, 115, 0);
    CB->ajoute_zone(16, 6945, -4370, 165, 115, 0);
    CB->ajoute_zone(17, 7028, -4253, 130, 110, 0);
    CB->ajoute_zone(18, 7108, -4153, 130, 130, 0);
    CB->ajoute_zone(19, 7113, -4041, 110, 125, 0);
    CB->ajoute_zone(25, 6350, -4180, 80, 85, 0);
    CB->ajoute_zone(26, 6183, -4425, 80, 60, 0);
    CB->ajoute_zone(27, 5987, -4425, 80, 60, 0);
    CB->ajoute_zone(28, 5773, -4425, 80, 60, 0);
    CB->ajoute_zone(29, 6370, -4187, 180, 70, 0);
    CB->ajoute_zone(30, 6180, -4390, 80, 150, 0);
    CB->ajoute_zone(31, 5981, -4390, 80, 150, 0);
    CB->ajoute_zone(32, 5776, -4390, 80, 150, 0);
    CB->ajoute_zone(33, 6384, -4422, 70, 60, 0);
    CB->ajoute_zone(35, 6900, -4043, 80, 80, 0);
    CB->ajoute_zone(39, 6380, -4240, 200, 170, 0);
    /* 0x7B3B */
    if (ST(0x6E4) == 0) {
        CB->etat_objet("OB91A", 6985, -1560, -4558, 0x2200, 0x5600, 0x4800, -1, 0);
    }
    /* 0x7B7A */
    if (ST(0x6D0) == 0) {
        CB->etat_objet("OB86", 7190, -1490, -4135, 0xFC00, 0x7E00, 0xD800, -1, 4);
    }
    /* 0x7BB9 */
    if (ST(0xBB0) == 1 && ST(0xE14) == 1) {
        ST(0xE14) = 2;
    }
    /* 0x7BE9 */
    if (ST(ST_LEVEL) == 23) {
        CB->pos_perso(ST(ST_HERO), 6070, -4020, 0x826A, 1, "ar", 7, 0);
    }
    /* 0x7C26 */
    if (ST(ST_VIE(P_THANANDAR24)) == 0) {
        ST(ST_VIE(P_THANANDAR24)) = 1;
    }
    /* 0x7C44 */
    if (ST(ST_LEVEL) == 27) {
        ST(0xE3C) = 1;
        ST(V_EVENT_7) = 0;
        ST(0x734) = 0;
        CB->bruitage("L24AMB1", -1, -1);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0xE78) = 7;
        ST(0xAF8) = 1;
        ST(0xB98) = 3;
        CB->bloque(BLOQUE_ALL);
        CB->divers(DV_INVENTORY, 1, 0);
        CB->vire_col(0xC, 0);
        CB->vire_col(0x10, 1);
        CB->divers_empile(DV_SCREEN, 1, 0);
        CB->change_camera(2, CAM_LOCKED);
        CB->ajoute_zone(1, 6090, -3930, 100, 60, 1);
        CB->pos_perso(P_ARKHAN2, 6323, -4329, 0xFF67, 1, "G02ARR16", 7, 0);
        CB->pos_perso(P_THANANDAR24, 6000, -3250, 0x7F7B, 1, "P24ARR01", 8, 0);
        CB->pos_perso(P_GUARDIAN16, 6072, -4101, 0xA0F0, 1, "P56GAR03", 9, 0);
    } else {
        /* 0x7DB9 */
        if (ST(0xBA8) == 2 && ST(ST_VIE(P_THANANDAR24)) == 1) {
            CB->bruitage("L24AMB1", -1, -1);
            /* 0x7DEF */
            if (ST(0xBB0) == 0) {
                ST(0x74) = 1;
            }
            /* 0x7E0D */
            if (ST(0xBB0) == 1) {
                ST(V_ENGAGED_PERSONA) |= 0x2400;
                CB->combat(P_THANANDAR24, 0);
            }
            CB->pos_perso(P_GUARDIAN16, 0, 0, 0, 2, "P56cad", 9, 0);
            CB->pos_perso(P_THANANDAR24, 6100, -4250, 0, 1, "P24ARR01", 8, 0);
            CB->vire_col(0x10, 1);
            CB->ajoute_zone(4, 6090, -4020, 120, 90, 0);
            CB->place_animal(1, 6982, 0, -4152, 0, 150);
            CB->place_animal(1, 6694, 0, -4293, 5000, 600);
        }
        /* 0x7EE0 */
        if (ST(0xBA8) == 2 && ST(ST_VIE(P_THANANDAR24)) >= 2) {
            CB->bruitage("L24AMB1", -1, -1);
            CB->pos_perso(P_GUARDIAN16, 0, 0, 0, 2, "P56cad", 9, 0);
            CB->pos_perso(P_THANANDAR24, 0, 0, 0, 2, "P24cad", 8, 0);
            CB->vire_col(0x10, 1);
            CB->ajoute_zone(4, 6090, -4020, 120, 90, 0);
            CB->place_animal(1, 6982, 0, -4152, 0, 150);
            CB->place_animal(1, 6694, 0, -4293, 5000, 600);
        }
        /* 0x7FB8 */
        if (ST(0xBA8) == 4) {
            CB->bruitage("L24AMB1", -1, -1);
            CB->pos_perso(P_THANANDAR24, 6844, -4093, 0xBB8, 1, "P24ACT01", 8, 0);
            CB->pos_perso(P_GUARDIAN16, 0, 0, 0, 2, "P56cad", 9, 0);
            CB->place_animal(1, 6982, 0, -4152, 0, 150);
            CB->place_animal(1, 6694, 0, -4293, 5000, 600);
        }
        /* 0x805C */
        if (ST(0xBA8) == 5) {
            /* 0x806E */
            if (ST(ST_VIE(P_THANANDAR24)) == 1) {
                CB->pos_perso(P_THANANDAR24, 6830, -4113, 0x4268, 1, "P24ACT02", 8, 0);
            }
            /* 0x80A5 */
            if (ST(ST_VIE(P_THANANDAR24)) >= 2) {
                CB->pos_perso(P_THANANDAR24, 0, 0, 0, 2, "P24cad", 8, 0);
            }
            CB->place_animal(1, 6982, 0, -4152, 0, 150);
            CB->place_animal(1, 6694, 0, -4293, 5000, 600);
        }
        /* 0x8112 */
        if (ST(0xBA8) == 6 && ST(V_GUARD_FIGHT_DONE) < 2 && ST(0xBB0) < 3) {
            /* 0x8148 */
            if (ST(0xE14) == 2) {
                ST(0xE14) = 3;
            }
            /* 0x8169 */
            if (ST(0xE14) == 1) {
                ST(0xE14) = 2;
            }
            /* 0x818A */
            if (ST(0xE14) == 0) {
                ST(0xE14) = 1;
            }
            /* 0x81AB */
            if (ST(ST_VIE(P_KONKALITE1)) <= 1) {
                ST(ST_VIE(P_KONKALITE1)) = 1;
                CB->pos_perso(P_KONKALITE1, 6676, -4423, 0x4D8, 1, "P71ARR01", 0xB, 0);
                CB->divers_empile(DV_LOOK_AT_PLAYER, P_KONKALITE1, 1);
            }
            /* 0x8203 */
            if (ST(ST_VIE(P_KONKALITE1)) == VIE_DEAD) {
                CB->pos_perso(P_KONKALITE1, 0, 0, 0, 2, "P71cad", 0xB, 0);
            }
            /* 0x8231 */
            if (ST(ST_VIE(P_THANANDAR24)) == 1) {
                CB->pos_perso(P_THANANDAR24, 6830, -4113, 0x4268, 1, "P24ACT02", 8, 0);
            }
            /* 0x8268 */
            if (ST(ST_VIE(P_THANANDAR24)) >= 2) {
                CB->pos_perso(P_THANANDAR24, 0, 0, 0, 2, "P24cad", 8, 0);
            }
            CB->place_animal(1, 6982, 0, -4152, 0, 150);
            CB->place_animal(1, 6694, 0, -4293, 5000, 600);
        }
        /* 0x82D5 */
        if (ST(0xBA8) == 7 && ST(V_GUARD_FIGHT_DONE) < 2 && ST(0xBB0) < 3) {
            /* 0x830B */
            if (ST(ST_VIE(P_KONKALITE1)) == 1) {
                CB->pos_perso(P_KONKALITE1, 0, 0, 0, 1, "P71ARR01", 0xB, 0);
            }
            /* 0x8339 */
            if (ST(ST_VIE(P_KONKALITE1)) == VIE_DEAD) {
                CB->pos_perso(P_KONKALITE1, 0, 0, 0, 2, "P71cad", 0xB, 0);
            }
            /* 0x8367 */
            if (ST(ST_VIE(P_THANANDAR24)) >= 2) {
                CB->pos_perso(P_THANANDAR24, 0, 0, 0, 2, "P24cad", 8, 0);
            }
            CB->place_animal(1, 6982, 0, -4152, 0, 150);
            CB->place_animal(1, 6694, 0, -4293, 5000, 600);
        }
        /* 0x83D4 */
        if (ST(V_GUARD_FIGHT_DONE) >= 2 && ST(0xBB0) == 2) {
            CB->ajoute_zone(34, CB->divers(DV_DESC_POS, P_THANANDAR24, 0), CB->divers(DV_DESC_POS, P_THANANDAR24, 1), 100, 100, 0);
        }
        /* 0x8430 */
        if (ST(0xBA8) >= 2) {
            CB->vire_col(0x10, 1);
            CB->ajoute_zone(4, 6090, -4020, 120, 90, 0);
        }
        /* 0x846D */
        if (ST(0xBA8) >= 5 && ST(V_GUARD_FIGHT_DONE) < 2) {
            CB->bruitage("L24AMB2", 6711, -4288);
            CB->ajoute_zone(20, 6603, -4305, 100, 75, 0);
            CB->ajoute_zone(21, 6711, -4170, 150, 200, 0);
            CB->ajoute_zone(22, 6784, -4200, 90, 100, 0);
            CB->ajoute_zone(23, 6711, -4288, 95, 130, 0);
            CB->ajoute_zone(24, 6617, -4208, 85, 130, 0);
        }
    }
    CB->load_anim("224FERME", 0);
    CB->load_anim("224JOUR", 1);
    CB->load_anim("224NUIT", 2);
    ST(ST_START_POINT(24)) = 0;
    CB->vire_col(0x10, 1);
}

/* trigger 01 - sub_1000B4AB - exit to room 23 */
static void trig01(void) {
    /* 0xB4BD */
    if (ST(0xF4) != 1 && ST(ST_ACTION_KEY) == 1) {
        ST(0x940) = ST(V_ENGAGED_PERSONA);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->select_pile(2, 2);
        CB->select_pile(1, 2);
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_NONE);
        CB->divers(DV_MUSIC, 1, 4);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->modify_var(&ST(0xBB4), 1);
        CB->modify_var(&ST(ST_LEVEL), 24);
        CB->change_piece(23, 1);
    }
}

/* trigger 03 - sub_1000B67D - door panel, scripted line */
static void trig03(void) {
    /* 0xB68C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xD354)) {
        /* 0xB6CF */
        if (ST(0xBA8) >= 4 && ST(ST_VIE(P_THANANDAR24)) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->animation("SG00PRE02_200");
            CB->evenement(0, 500);
            return;
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE02");
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 04 - sub_1000B770 - door opening */
static void trig04(void) {
    /* 0xB77F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xB7C7 */
        if (ST(0xBA8) == 2 && ST(0xA5C) < 2) {
            ST(0xBB4) = 1;
        }
        /* 0xB7FA */
        if (ST(0xBA8) == 2 && ST(0xA5C) == 2 && ST(ST_VIE(P_THANANDAR24)) == 1) {
            ST(0xBA8) = 4;
        }
        CB->evenement(37, 500);
        CB->animation("SG00OUV01");
        return;
    }
}

/* trigger 08 - sub_1000B9A3 - Thanandar: save point, search, talk, hit reaction */
static void trig08(void) {
    /* 0xB9B6 */
    if (CB->direction_spec(0xBB80) && ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 39) && !CB->direction_perso(P_THANANDAR24)) {
        CB->select_pile(0, 2);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xBA4C */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xBA77 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xBAA2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->select_pile(0, 1);
        return;
    }
    /* 0xBB12 */
    if (ST(ST_VIE(P_THANANDAR24)) == VIE_DEAD && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(0x8E4) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x8E4) = 1;
        CB->fouille(P_THANANDAR24);
        CB->bloque(BLOQUE_ALL);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xBBC5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && !CB->direction_perso(P_THANANDAR24)) {
        /* 0xBC16 */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 21)) {
            trig21();
        }
        /* 0xBC3B */
        if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 22)) {
            trig22();
        }
    }
    /* 0xBC4F */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_THANANDAR24) && ST(ST_VIE(P_THANANDAR24)) < 2) {
        CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
        CB->ajoute_zone(29, -1, -1, -1, -1, -1);
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xBCF5 */
        if (ST(0x7C) == 3) {
            ST(0x88) = 1;
        }
        ST(0x7C) = 0;
        CB->stop_joindre(P_THANANDAR24);
        CB->joindre(ST(ST_HERO), 0, 0, 0, 24);
        /* 0xBD46 */
        if (ST(0xBA8) < 4) {
            /* 0xBD58 */
            if (ST(0xA5C) == 0) {
                ST(0xA5C) = 1;
                /* 0xBD79 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L24D23", P_ARKHAN2, "SG00DIA01", NULL);
                    lieu_action_register("L24D24A", P_THANANDAR24, "SP24DIA53", "SP24ARR01_10");
                    lieu_action_register("L24D24B", P_THANANDAR24, "SP24DIA57", "SP24ARR01_10");
                    lieu_action_register("L24D25", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xBDE9 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L24D26", P_ARKHAN2, "SG00DIA01", NULL);
                    lieu_action_register("L24D27A", P_THANANDAR24, "SP24DIA56", "SP24ARR01_10");
                    lieu_action_register("L24D27B", P_THANANDAR24, "SP24DIA62", "SP24ARR01_10");
                    lieu_action_register("L24D27C", P_THANANDAR24, "SP24DIA58", "SP24ARR01_10");
                    lieu_action_register("L24D28", P_ARKHAN2, "SG00DIA07", NULL);
                }
                lieu_action_register("L24D29A", P_THANANDAR24, "SP24DIA59", "SP24ARR01_10");
                lieu_action_register("L24D29B", P_THANANDAR24, "SP24DIA50", "SP24ARR01_10");
                lieu_action_register("L24D29C", P_THANANDAR24, "SP24DIA60", "SP24ARR01_10");
                lieu_action_register("L24D29D", P_THANANDAR24, "SP24DIA61", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xBEE4 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xBF19 */
            if (ST(0xA5C) == 1) {
                ST(0xA5C) = 2;
                CB->bloque(BLOQUE_ALL);
                ST(0xE3C) = 1;
                /* 0xBF54 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L24D30", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0xBF7C */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L24D31", P_ARKHAN2, "SG00DIA05", NULL);
                }
                lieu_action_register("L24D32A", P_THANANDAR24, "SP24DIA56", "SP24ARR01");
                lieu_action_register("L24D32B", P_THANANDAR24, "SP24DIA57", "SP24ARR01");
                lieu_action_register("L24D32C", P_THANANDAR24, "SP24DIA64", "SP24ARR01");
                lieu_action_register("L24D32D", P_THANANDAR24, "SP24DIA63", "SP24ARR01");
                lieu_action_register("L24D32E", P_THANANDAR24, "SP24DIA58", "SP24ARR01");
                lieu_action_register("L24D33", P_ARKHAN2, "SG00DIA06", NULL);
                lieu_action_register("L24D34A", P_THANANDAR24, "SP24DIA65", "SP24ARR01_10");
                lieu_action_register("L24D34B", P_THANANDAR24, "SP24DIA54", "SP24ARR01_10");
                lieu_action_register("L24D34C", P_THANANDAR24, "SP24DIA57", "SP24ARR01_10");
                CB->bruitage("L24B01", 6090, -3930);
                CB->dialogue("L24D35", P_PHEDORIA);
                CB->evenement(1, 400);
                return;
            }
            /* 0xC0BC */
            if (ST(0xA5C) == 2) {
                CB->bloque(BLOQUE_ALL);
                /* 0xC0D9 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L24D36", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xC101 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D37", P_ARKHAN3, "SG00DIA06", NULL);
                }
                lieu_action_register("L24D38A", P_THANANDAR24, "SP24DIA58", "SP24ARR01_10");
                lieu_action_register("L24D38B", P_THANANDAR24, "SP24DIA62", "SP24ARR01_10");
                lieu_action_register("L24D38C", P_THANANDAR24, "SP24DIA63", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                /* 0xC18A */
                if (ST(V_ENGAGED_PERSONA) == 0 && g_l24_heisgoing == 0) {
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 1);
                    CB->bloque(BLOQUE_ALL);
                    CB->joindre(P_THANANDAR24, 5926, -4193, 0xC1C8, -1);
                    CB->animation("Sar_1000");
                    CB->modify_var(&g_l24_heisgoing, 1);
                } else if (ST(V_ENGAGED_PERSONA) == 0 && g_l24_heisgoing != 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                return;
            }
        }
        /* 0xC23B */
        if (ST(0xBA8) == 4) {
            /* 0xC24D */
            if (ST(0x958) == 0) {
                /* 0xC25F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L24D43", P_ARKHAN2, "SG00DIA04", NULL);
                }
                /* 0xC287 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D44", P_ARKHAN3, "SG00DIA04", NULL);
                }
                CB->animation("SP24DIA59");
                CB->dialogue("L24D45A", P_THANANDAR24);
                CB->dialogue("L24D45B", P_THANANDAR24);
                CB->animation("SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xC2F3 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC328 */
            if (ST(0x958) == 1) {
                ST(0x958) = 2;
                /* 0xC349 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L24D46", P_ARKHAN2, "SG00DIA04", NULL);
                }
                /* 0xC371 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D47", P_ARKHAN3, "SG00DIA04", NULL);
                }
                lieu_action_register("L24D48", P_THANANDAR24, "SP24DIA54", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xC3C0 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC3F5 */
            if (ST(0x958) == 2) {
                /* 0xC407 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L24D49", P_ARKHAN2, "SG00DIA04", NULL);
                }
                /* 0xC42F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D50", P_ARKHAN3, "SG00DIA04", NULL);
                }
                lieu_action_register("L24D51", P_THANANDAR24, "SP24ATT50", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xC47E */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xC4B3 */
        if (ST(0xBA8) == 5) {
            /* 0xC4C2 */
            if (ST(0x70) == 0) {
                ST(0x70) = 1;
                /* 0xC4E0 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L24D52A", P_ARKHAN2, "SG00DIA02", NULL);
                    lieu_action_register("L24D52B", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xC530 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L24D53A", P_ARKHAN2, "SG00DIA08", NULL);
                    lieu_action_register("L24D53B", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xC580 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D54A", P_ARKHAN3, "SG00DIA04", NULL);
                    lieu_action_register("L24D54B", P_ARKHAN3, "SG00DIA05", NULL);
                }
                lieu_action_register("L24D55A", P_THANANDAR24, "SP24DIA55", "SP24ARR01_10");
                lieu_action_register("L24D55B", P_THANANDAR24, "SP24DIA58", "SP24ARR01_10");
                lieu_action_register("L24D55C", P_THANANDAR24, "SP24DIA62", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xC617 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC649 */
            if (ST(0x70) == 1) {
                ST(0x70) = 2;
                /* 0xC667 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L24D56", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0xC68F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D57", P_ARKHAN3, "SG00DIA05", NULL);
                }
                lieu_action_register("L24D58", P_THANANDAR24, "SP24DIA56", "SP24ARR01_10");
                /* 0xC6D0 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L24D59", P_ARKHAN2, "SG00DIA01", NULL);
                }
                /* 0xC6F8 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D95", P_ARKHAN3, "SG00DIA01", NULL);
                }
                lieu_action_register("L24D60A", P_THANANDAR24, "SP24DIA60", "SP24ARR01_10");
                lieu_action_register("L24D60B", P_THANANDAR24, "SP24DIA59", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xC760 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC792 */
            if (ST(0x70) == 2) {
                /* 0xC7A4 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L24D49", P_ARKHAN2, "SG00DIA04", NULL);
                }
                /* 0xC7CC */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D50", P_ARKHAN3, "SG00DIA04", NULL);
                }
                lieu_action_register("L24D51", P_THANANDAR24, "SP24ATT50", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xC81B */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xC850 */
        if (ST(0xBA8) == 6) {
            /* 0xC85F */
            if (ST(0x70) == 0) {
                ST(0x70) = 1;
                /* 0xC87D */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L24D61A", P_ARKHAN2, "SG00DIA04", NULL);
                    lieu_action_register("L24D61B", P_ARKHAN2, "SG00DIA06", NULL);
                }
                /* 0xC8CD */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L24D62A", P_ARKHAN2, "SG00DIA04", NULL);
                    lieu_action_register("L24D62B", P_ARKHAN2, "SG00DIA07", NULL);
                }
                /* 0xC91D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D63A", P_ARKHAN3, "SG00DIA04", NULL);
                    lieu_action_register("L24D63B", P_ARKHAN3, "SG00DIA05", NULL);
                }
                lieu_action_register("L24D64", P_THANANDAR24, "SP24DIA63", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xC982 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xC9B4 */
            if (ST(0x70) == 1 && ST(0x95C) == 0) {
                ST(0x95C) = 1;
                /* 0xC9E7 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L24D65A", P_ARKHAN2, "SG00DIA06", NULL);
                    lieu_action_register("L24D65B", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0xCA37 */
                if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L24D66A", P_ARKHAN2, "SG00DIA08", NULL);
                    lieu_action_register("L24D66B", P_ARKHAN2, "SG00DIA02", NULL);
                }
                /* 0xCA87 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L24D67A", P_ARKHAN3, "SG00DIA06", NULL);
                    lieu_action_register("L24D67B", P_ARKHAN3, "SG00DIA02", NULL);
                }
                /* 0xCAD7 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L24D68", P_ARKHAN3, "SG00DIA08", NULL);
                }
                lieu_action_register("L24D69A", P_THANANDAR24, "SP24DIA55", "SP24ARR01_10");
                lieu_action_register("L24D69B", P_THANANDAR24, "SP24DIA58", "SP24ARR01_10");
                lieu_action_register("L24D69C", P_THANANDAR24, "SP24DIA63", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xCB6A */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xCB9C */
            if (ST(0x70) == 1 && ST(0x95C) == 1) {
                /* 0xCBC0 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L24D49", P_ARKHAN2, "SG00DIA04", NULL);
                }
                /* 0xCBE8 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L24D50", P_ARKHAN3, "SG00DIA04", NULL);
                }
                lieu_action_register("L24D51", P_THANANDAR24, "SP24ATT50", "SP24ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                /* 0xCC37 */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        CB->ajoute_zone(29, 6370, -4187, 180, 70, 0);
    }
    /* 0xCC87 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_THANANDAR24)) != VIE_DEAD && CB->direction_perso(P_THANANDAR24)) {
        ST(0x74) = 10;
        CB->stop_joindre(-P_THANANDAR24);
        ST(V_PLAYER_STRIKE) = -1;
        /* 0xCCF0 */
        if (ST(0xA60) == 0) {
            CB->bloque(BLOQUE_ALL);
            ST(0xA60) = 1;
            ST(0xA64) = 2;
            ST(0x6C) = 1;
            ST(0x74) = 2;
            CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
            CB->stop_joindre(P_THANANDAR24);
            lieu_action_register("L24D20", P_THANANDAR24, "SP24ESM05", "SP24GAR01_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
            return;
        }
        /* 0xCDB1 */
        if (ST(0xA60) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(0xBB0) = 1;
            ST(V_ENGAGED_PERSONA) |= 0x2400;
            ST(0xA60) = 2;
            CB->animation("SP24ESM07");
            CB->dialogue("L24D70", P_THANANDAR24);
            CB->combat(P_THANANDAR24, 0);
            /* 0xCE20 */
            if (ST(ST_VIE(P_KONKALITE1)) == 1) {
                CB->combat(P_KONKALITE1, 0);
                ST(V_ENGAGED_PERSONA) |= 0x2471;
            }
            ST(0x4C) = 2;
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 09 - sub_1000CE5D - Guardian16: save point, search, talk */
static void trig09(void) {
    /* 0xCE70 */
    if (CB->direction_spec(0xBB80) && ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 39) && !CB->direction_perso(P_GUARDIAN16)) {
        CB->select_pile(0, 2);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xCF06 */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xCF31 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xCF5C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->select_pile(0, 1);
        return;
    }
    /* 0xCFCC */
    if (ST(ST_VIE(P_GUARDIAN16)) == VIE_DEAD && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_VIE(P_GUARDIAN16)) = 3;
        CB->fouille(P_GUARDIAN16);
        CB->bloque(BLOQUE_ALL);
        CB->animation("Sar_10");
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xD050 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_GUARDIAN16) && ST(ST_VIE(P_GUARDIAN16)) < 2) {
        /* 0xD096 */
        if (ST(0xA58) == 0) {
            ST(ST_SCRIPT_BUSY) = 1;
            ST(0xA58) = 1;
            CB->combat(P_GUARDIAN16, -1);
            CB->animation("SP56GAR03_10");
            CB->dialogue("L24D71", P_ARKHAN2);
            lieu_action_register("L24D72", P_GUARDIAN16, "SP56ATT01", "SP56GAR03_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->combat(P_GUARDIAN16, 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xD132 */
        if (ST(0xA58) == 1) {
            ST(ST_SCRIPT_BUSY) = 1;
            CB->combat(P_GUARDIAN16, -1);
            CB->animation("SP56GAR03_10");
            CB->dialogue("L24D73", P_ARKHAN2);
            CB->dialogue("L24D04", P_GUARDIAN16);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->combat(P_GUARDIAN16, 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 11 - sub_1000D1BA - Konkalite1: save point, search, talk, strike reaction */
void l24_trig11(void) {
    /* 0xD1CD */
    if (CB->direction_spec(0xBB80) && ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->divers(DV_IN_ZONE, ST(ST_HERO), 39) && !CB->direction_perso(P_KONKALITE1)) {
        CB->select_pile(0, 2);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xD263 */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xD28E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xD2B9 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->select_pile(0, 1);
        return;
    }
    /* 0xD329 */
    if (ST(ST_VIE(P_KONKALITE1)) >= 2 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->fouille(P_KONKALITE1);
        CB->bloque(BLOQUE_ALL);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xD3BB */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_perso(P_KONKALITE1) && ST(ST_VIE(P_KONKALITE1)) < 2) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->joindre(ST(ST_HERO), 0, 0, 0, 71);
        /* 0xD440 */
        if (ST(0xA68) == 0) {
            ST(0xA68) = 1;
            /* 0xD461 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L24D74", P_ARKHAN2, "SG00DIA05", NULL);
            }
            /* 0xD489 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L24D75", P_ARKHAN3, "SG00DIA05", NULL);
            }
            lieu_action_register("L24D76", P_KONKALITE1, "SP71DIA53", "SP71ARR01_10");
            /* 0xD4C7 */
            if (ST(0x74) != 2 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                CB->dialogue("L24D77", P_THANANDAR24);
            }
            /* 0xD4F8 */
            if (ST(0x74) == 2 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                ST(0x74) = 10;
                CB->stop_joindre(-P_THANANDAR24);
                lieu_action_register("L24D77", P_THANANDAR24, "SP24DIA54", "SP24ARR01_10");
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            /* 0xD562 */
            if (ST(0x74) == 10 && ST(V_ENGAGED_PERSONA) == 0) {
                CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
            }
            return;
        }
        /* 0xD59E */
        if (ST(0xA68) == 1) {
            ST(0xA68) = 2;
            CB->joindre(P_KONKALITE1, 0, 0, 0, ST(ST_HERO));
            /* 0xD5DC */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L24D78", P_ARKHAN2, "SG00DIA05", NULL);
                lieu_action_register("L24D79", P_KONKALITE1, "SP71DIA54", "SP71ARR01_10");
            }
            /* 0xD62F */
            if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L24D80", P_ARKHAN2, "SG00DIA08", NULL);
                lieu_action_register("L24D81", P_KONKALITE1, "SP71DIA60", "SP71ARR01_10");
            }
            /* 0xD682 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L24D82A", P_ARKHAN3, "SG00DIA02", NULL);
                lieu_action_register("L24D82B", P_ARKHAN3, "SG00DIA05", NULL);
                lieu_action_register("L24D83", P_KONKALITE1, "SP71DIA61", "SP71ARR01_10");
            }
            /* 0xD6D6 */
            if (ST(0x74) != 2 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                CB->dialogue("L24D84", P_THANANDAR24);
            }
            /* 0xD707 */
            if (ST(0x74) == 2 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                ST(0x74) = 10;
                CB->stop_joindre(-P_THANANDAR24);
                lieu_action_register("L24D84", P_THANANDAR24, "SP24DIA60", "SP24ARR01_10");
            }
            CB->joindre_perm(P_KONKALITE1, 24);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            /* 0xD781 */
            if (ST(0x74) == 10 && ST(V_ENGAGED_PERSONA) == 0) {
                CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
            }
            CB->stop_joindre(P_KONKALITE1);
            return;
        }
        /* 0xD7C8 */
        if (ST(0xA68) == 2) {
            /* 0xD7DA */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00DIA02");
                CB->dialogue("L24D85A", P_ARKHAN2);
                CB->dialogue("L24D85B", P_ARKHAN2);
                CB->animation("sar_10");
            }
            /* 0xD822 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L24D86", P_ARKHAN3, "SG00DIA01", NULL);
            }
            CB->dialogue("L24D87", P_KONKALITE1);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0xD875 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_KONKALITE1)) != VIE_DEAD && CB->direction_zone(11)) {
        CB->divers_empile(DV_LOOK_AT_PLAYER, P_KONKALITE1, 0);
        ST(V_PLAYER_STRIKE) = -1;
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_KONKALITE1);
        CB->animation("SP71ESA21");
        CB->animation("P71_65535");
        CB->animation("Sar_10");
        /* 0xD91F */
        if (ST(0xDE0) == 0) {
            ST(0xDE0) = 1;
            /* 0xD93D */
            if (ST(0x74) == 0 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                CB->dialogue("L24D21", P_THANANDAR24);
            }
            /* 0xD96E */
            if (ST(0x74) != 0 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                CB->stop_joindre(-P_THANANDAR24);
                lieu_action_register("L24D21", P_THANANDAR24, "SP24DIA54", "SP24ARR01_10");
            }
            CB->animation("P71GAR27");
            CB->animation("P71_65535");
            CB->animation("P71ARR01_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->divers_empile(DV_LOOK_AT_PLAYER, P_KONKALITE1, 1);
            CB->bloque(BLOQUE_NONE);
            /* 0xD9FF */
            if (ST(0x74) != 0 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                ST(0x74) = 2;
                /* 0xDA2C */
                if (ST(V_ENGAGED_PERSONA) == 0) {
                    CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
                }
            }
            return;
        }
        /* 0xDA59 */
        if (ST(0xDE0) == 1) {
            ST(0xBB0) = 1;
            ST(0xBB8) = 1;
            ST(0xDE0) = 2;
            ST(0x64) = 10;
            ST(0x68) = 1;
            CB->animation("SP71GAR03_10");
            /* 0xDAB8 */
            if (ST(0x74) != 0 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                CB->stop_joindre(-P_THANANDAR24);
                lieu_action_register("L24D22", P_THANANDAR24, "SP24ATT61", "SP24ARR01_10");
            }
            /* 0xDAFD */
            if (ST(0x74) == 0 && ST(ST_VIE(P_THANANDAR24)) < 2) {
                CB->joindre(-P_THANANDAR24, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L24D22", P_THANANDAR24, "SP24ATT61", "SP24ARR01_10");
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            ST(V_ENGAGED_PERSONA) |= 0x2471;
            CB->combat(P_THANANDAR24, 0);
            CB->combat(P_KONKALITE1, 0);
            return;
        }
    }
}

/* trigger 12 - sub_1000DB9F - inspect object */
static void trig12(void) {
    /* 0xDBAE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x8CA0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDC08 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L24D88", P_ARKHAN2, "SG00PRE01", NULL);
        }
        /* 0xDC30 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L24D89", P_ARKHAN3, "SG00PRE01", NULL);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 13 - sub_1000DC6F - attack animation on an object */
static void trig13(void) {
    /* 0xDC7E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x88B8)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xDCE0 */
        if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
            /* 0xDCF2 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xDD04 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT85");
                }
                /* 0xDD21 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT85");
                }
            }
            /* 0xDD3E */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xDD50 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT87");
                }
                /* 0xDD6D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT87");
                }
            }
        } else {
            /* 0xDD8F */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xDDA1 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT84");
                }
                /* 0xDDBE */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT84");
                }
            }
            /* 0xDDDB */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xDDED */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT86");
                }
                /* 0xDE0A */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT86");
                }
            }
        }
        CB->animation("ar_200");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 15 - sub_1000DE54 - inspect zone 14 */
static void trig15(void) {
    /* 0xDE63 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_zone(14)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE03");
        lieu_action_register("LCOD76", ST(ST_HERO), "SG00PRE03", "ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 16 - sub_1000DEFE - chest OB91A */
static void trig16(void) {
    /* 0xDF0D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xB3B0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xDF58 */
        if (ST(0x6E4) == 0 && CB->direction_perso(-91)) {
            ST(0x6E4) = 1;
            CB->animation("SG00PRE07");
            CB->etat_objet("OB91A", -1, -1, -1, -1, -1, -1, -3, 0);
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        ST(V_DLG_RANDOM) = CB->random(2);
        /* 0xDFEF */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->animation("SG00PRE03");
        }
        /* 0xE00C */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->animation("SG00PRE01");
        }
        /* 0xE029 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L05D183", P_ARKHAN2);
        }
        /* 0xE04B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L05D184", P_ARKHAN3);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 17 - sub_1000E08F - random comment */
static void trig17(void) {
    /* 0xE09E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x9C40)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(V_DLG_RANDOM) = CB->random(2);
        ST(V_DLG_RANDOM) = CB->random(2);
        /* 0xE126 */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->animation("SG00PRE03");
        }
        /* 0xE143 */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->animation("SG00PRE01");
        }
        /* 0xE160 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L05D183", P_ARKHAN2);
        }
        /* 0xE182 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L05D184", P_ARKHAN3);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 18 - sub_1000E1C6 - object OB86 */
static void trig18(void) {
    /* 0xE1D5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(18)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(V_DLG_RANDOM) = CB->random(2);
        /* 0xE246 */
        if (ST(0x6D0) == 1) {
            /* 0xE258 */
            if (ST(V_DLG_RANDOM) == 0) {
                CB->animation("SG00PRE03");
            }
            /* 0xE275 */
            if (ST(V_DLG_RANDOM) == 1) {
                CB->animation("SG00PRE01");
            }
            /* 0xE292 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("L05D183", P_ARKHAN2);
            }
            /* 0xE2B4 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("L05D184", P_ARKHAN3);
            }
            CB->animation("ar_10");
        }
        /* 0xE2E1 */
        if (ST(0x6D0) == 0) {
            ST(0x6D0) = 1;
            ST(V_HUD_TIMER_MS) = 1200000;
            CB->animation("SG00PRE07");
            CB->etat_objet("OB86", 7190, -1490, -4135, 0xFC00, 0x7E00, 0xD800, -3, 0);
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 19 - sub_1000E360 - random comment */
static void trig19(void) {
    /* 0xE36F */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xBB80)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(V_DLG_RANDOM) = CB->random(2);
        ST(V_DLG_RANDOM) = CB->random(2);
        /* 0xE3F7 */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->animation("SG00PRE03");
        }
        /* 0xE414 */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->animation("SG00PRE01");
        }
        /* 0xE431 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L05D183", P_ARKHAN2);
        }
        /* 0xE453 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L05D184", P_ARKHAN3);
        }
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 20 - sub_1000E497 - Thanandar warning at the gate */
static void trig20(void) {
    /* 0xE4A6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ((CB->direction_spec(0xDEA8) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 20)) || !CB->divers(DV_IN_ZONE, ST(ST_HERO), 20))) {
        /* 0xE529 */
        if (ST(V_GUARD_FIGHT_DONE) < 2 && ST(0xA74) == 0 && ST(ST_VIE(P_THANANDAR24)) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xE573 */
            if (ST(0x74) == 2) {
                ST(0x74) = 10;
                CB->stop_joindre(-P_THANANDAR24);
            }
            ST(0xA74) = 1;
            /* 0xE5AB */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L24D90", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
            }
            /* 0xE5D6 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L24D91", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
            }
            /* 0xE5FE */
            if (ST(0x74) == 0) {
                CB->dialogue("L24D92A", P_THANANDAR24);
                CB->dialogue("L24D92B", P_THANANDAR24);
            }
            /* 0xE62D */
            if (ST(0x74) == 10) {
                lieu_action_register("L24D92A", P_THANANDAR24, "SP24DIA57", "SP24ARR01_10");
                lieu_action_register("L24D92B", P_THANANDAR24, "SP24DIA58", "SP24ARR01_10");
            }
            CB->animation("Sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            /* 0xE692 */
            if (ST(0x74) == 10 && ST(V_ENGAGED_PERSONA) == 0) {
                CB->joindre_perm(-P_THANANDAR24, ST(ST_HERO));
            }
            return;
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("Sar_10");
        /* 0xE6F5 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xE707 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD82", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
            } else {
                lieu_action_register("LCOD82", P_ARKHAN2, "SG00PRE80", "SG00PRE81");
            }
        }
        /* 0xE750 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xE762 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("LCOD84", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
            } else {
                lieu_action_register("LCOD85", P_ARKHAN3, "SG00PRE80", "SG00PRE81");
            }
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 21 - sub_1000E7CD - forwards to trigger 20 */
static void trig21(void) {
    /* 0xE7DC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0xA028)) {
        trig20();
    }
}

/* trigger 22 - sub_1000E81D - forwards to trigger 20 */
static void trig22(void) {
    /* 0xE82C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x14C8)) {
        trig20();
    }
}

/* trigger 23 - sub_1000E86D - forwards to trigger 20 */
static void trig23(void) {
    /* 0xE87C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x14C8)) {
        trig20();
    }
}

/* trigger 24 - sub_1000E8BD - forwards to trigger 20 */
static void trig24(void) {
    /* 0xE8CC */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x14C8)) {
        trig20();
    }
}

/* trigger 25 - sub_1000E90D - locked door comments */
static void trig25(void) {
    /* 0xE91C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ((CB->direction_spec(0xC738) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 25)) || !CB->divers(DV_IN_ZONE, ST(ST_HERO), 25))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xE9B6 */
        if (ST(0xA78) == 0) {
            ST(0xA78) = 1;
            /* 0xE9D7 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD13", P_ARKHAN2, "SG00OUV09", NULL);
            }
            /* 0xE9FF */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD14", P_ARKHAN3, "SG00OUV09", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xEA45 */
        if (ST(0xA78) == 1) {
            /* 0xEA57 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD16", P_ARKHAN2, "SG00OUV09", NULL);
            }
            /* 0xEA7F */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD17", P_ARKHAN3, "SG00OUV09", NULL);
            }
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 26 - sub_1000EABE - forwards to trigger 25 */
static void trig26(void) {
    /* 0xEACD */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x80E8)) {
        trig25();
    }
}

/* trigger 27 - sub_1000EB0E - forwards to trigger 25 */
static void trig27(void) {
    /* 0xEB1D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x80E8)) {
        trig25();
    }
}

/* trigger 28 - sub_1000EB5E - forwards to trigger 25 */
static void trig28(void) {
    /* 0xEB6D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x80E8)) {
        trig25();
    }
}

/* trigger 29 - sub_1000EBAE - door comments / save point */
static void trig29(void) {
    /* 0xEBCE */
    if (!CB->divers(DV_IN_ZONE, ST(ST_HERO), 8) && ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xEC24 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ACT26");
            /* 0xEC3E */
            if (ST(0x78) <= 2) {
                CB->dialogue("LCOD29", P_ARKHAN2);
            }
        }
        /* 0xEC60 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ACT26");
            /* 0xEC7A */
            if (ST(0x78) <= 2) {
                CB->dialogue("LCOD30", P_ARKHAN3);
            }
        }
        ST(0x78)++;
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xECDB */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 39)) {
        trig39();
    }
}

/* trigger 30 - sub_1000ECEB - forwards to trigger 29 */
static void trig30(void) {
    /* 0xECFE */
    if (CB->direction_spec(0xC738)) {
        trig29();
    }
    /* 0xED16 */
    if (CB->direction_spec(0x3E80)) {
        trig29();
    }
}

/* trigger 31 - sub_1000ED26 - forwards to trigger 29 */
static void trig31(void) {
    /* 0xED39 */
    if (CB->direction_spec(0xC738)) {
        trig29();
    }
    /* 0xED51 */
    if (CB->direction_spec(0x3E80)) {
        trig29();
    }
}

/* trigger 32 - sub_1000ED61 - forwards to trigger 29 */
static void trig32(void) {
    /* 0xED74 */
    if (CB->direction_spec(0xC738)) {
        trig29();
    }
}

/* trigger 33 - sub_1000ED84 - forwards to trigger 25 */
static void trig33(void) {
    /* 0xED93 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x80E8)) {
        trig25();
    }
}

/* trigger 35 - sub_1000B58A - door panel, scripted line */
static void trig35(void) {
    /* 0xB599 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x2E18)) {
        /* 0xB5DC */
        if (ST(0xBA8) >= 4 && ST(ST_VIE(P_THANANDAR24)) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            CB->animation("SG00PRE02");
            CB->evenement(0, 500);
            return;
        }
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SG00PRE02");
        CB->animation("ar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 39 - sub_1000B858 - save point */
static void trig39(void) {
    /* 0xB86B */
    if (CB->direction_spec(0xBB80) && ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->select_pile(0, 2);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xB8CE */
        if (ST(ST_HERO) <= 1) {
            lieu_action_register("LCOD52", P_ARKHAN1, "SG00SAV01", "Sar_10");
        }
        /* 0xB8F9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0xB924 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        CB->animation("P00_65535");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->select_pile(0, 1);
        return;
    }
}

/* trigger 40 - sub_1000EE16 - weapon warning by Thanandar */
static void trig40(void) {
    /* 0xEE28 */
    if (ST(ST_SCRIPT_BUSY) == 0 && ((ST(V_ENGAGED_PERSONA) >> 8) & 0xFF) == 0x24 && ST(ST_VIE(P_THANANDAR24)) != VIE_DEAD && CB->direction_perso(P_THANANDAR24)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SAR_10");
        /* 0xEE82 */
        if (ST(0x4C) == 1) {
            switch (ST(ST_HERO)) {
            case 2:   /* 0xEE9B */
                /* 0xEEA7 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L34D05", P_ARKHAN2);
                } else {
                    CB->dialogue("L34D06", P_ARKHAN2);
                }
                break;
            case 3:   /* 0xEED7 */
                CB->dialogue("L34D07", P_ARKHAN3);
                break;
            default:
                break;
            }
        }
        /* 0xEF13 */
        if (ST(0x4C) == 2) {
            switch (ST(ST_HERO)) {
            case 2:   /* 0xEF2C */
                /* 0xEF38 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("LCOD20", P_ARKHAN2);
                } else {
                    CB->dialogue("LCOD61", P_ARKHAN2);
                }
                break;
            case 3:   /* 0xEF68 */
                CB->dialogue("L34D07", P_ARKHAN3);
                break;
            default:
                break;
            }
        }
        /* 0xEFA7 */
        if (ST(V_WEAPON_WARNED) == 0) {
            CB->animation("SP24GAR01_500");
            CB->animation("P24GAR01_10");
            CB->dialogue("L34D201", P_THANANDAR24);
            CB->modify_var(&ST(V_WEAPON_WARNED), 1);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        return;
    }
    /* 0xF014 */
    if (ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_THANANDAR24)) != VIE_DEAD && CB->direction_perso(P_THANANDAR24)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->animation("SP24GAR01_500");
        CB->animation("P24GAR01_10");
        CB->dialogue("L34D201", P_THANANDAR24);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        return;
    }
}

/* trigger 41 - sub_1000857E - arrival zone: engage Thanandar / Konkalite */
static void trig41(void) {
    CB->bloque(BLOQUE_ALL);
    CB->animation("Sar_10");
    CB->divers(DV_MUSIC, 1, 0);
    /* 0x85B8 */
    if (ST(0xBA8) > 2 && ST(V_GUARD_FIGHT_DONE) < 2) {
        CB->divers_empile(DV_MUSIC, 1, 1);
    }
    /* 0x85EE */
    if (ST(0xBA8) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(0xBA8) = 1;
        ST(ST_SCRIPT_BUSY) = 1;
        CB->change_camera(2, CAM_LOCKED);
        CB->change_camera(2, CAM_LOCKED);
        CB->pos_perso(P_ARKHAN2, 6303, -4329, 0xFF67, 1, "G02ARR16", 7, 0);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->animation("SP02LEV01");
        CB->animation("P02_65535");
        CB->modify_var(&ST(V_EVENT15_STEP), 1);
    }
    /* 0x86AE */
    if (ST(0xBB0) == 1 && ST(0xBB8) == 2) {
        ST(0xB8) = 1;
    }
    /* 0x86E1 */
    if (ST(0xBB0) == 2 && ST(0xBB8) == 1) {
        ST(0xB8) = 2;
    }
    /* 0x8714 */
    if (ST(0xBB0) == 1 && ST(0xBB8) == 1) {
        ST(0xB8) = 3;
    }
    /* 0x8747 */
    if (((ST(0x940) >> 8) & 0xFF) == 0x24 && ST(ST_VIE(P_THANANDAR24)) == 1) {
        CB->combat(P_THANANDAR24, 0);
        ST(V_ENGAGED_PERSONA) |= 0x2400;
        /* 0x8781 */
        if (ST(0x4C) == 0) {
            ST(0x4C) = 1;
        }
        /* 0x879F */
        if (ST(ST_VIE(P_KONKALITE1)) == 1) {
            CB->combat(P_KONKALITE1, 0);
            ST(V_ENGAGED_PERSONA) |= 0x71;
        }
    }
    /* 0x87C7 */
    if ((ST(0x940) & 0xFF) == 0x71 && ST(ST_VIE(P_KONKALITE1)) == 1) {
        CB->combat(P_KONKALITE1, 0);
        ST(V_ENGAGED_PERSONA) |= 0x71;
    }
    CB->bloque(BLOQUE_NONE);
    CB->animation("Sar_10");
}
