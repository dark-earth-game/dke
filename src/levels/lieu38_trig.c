/*
 * lieu38_trig.c - Native hand-port of the LIEU38.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and events live in lieu38.c; this
 * file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * Most handlers are copies of one "press action at a spot" template
 * (busy, bloque, animation, dialogue lines) that differ only in the zone /
 * angle they test and the lines they play. The copies that differ only in
 * those parameters share a static helper.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu38_int.h"

static void trig02(void); static void trig04(void); static void trig05(void);
static void trig06(void); static void trig07(void); static void trig08(void);
static void trig09(void); static void trig10(void); static void trig11(void);
static void trig12(void); static void trig13(void); static void trig14(void);
static void trig15(void); static void trig16(void); static void trig17(void);
static void trig21(void); static void trig22(void); static void trig23(void);
static void trig24(void); static void trig25(void); static void trig26(void);
static void trig27(void); static void trig28(void); static void trig29(void);
static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* trigger table at 0x1001BD60 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 100073F0   1 stub       2 10008722   3 stub       4 10008751
 *   5 1000878F   6 100087CD   7 10008B38   8 10008DFE   9 100090C4
 *  10 1000938A  11 10009650  12 10009916  13 10009A89  14 10009BE8
 *  15 10009D5A  16 10009ED7  17 1000A054  18 stub      19 stub
 *  20 stub      21 1000A1F2  22 1000A36F  23 1000A4EC  24 1000A76F
 *  25 1000A876  26 1000A9FE  27 1000AB5D  28 1000ACE3  29 1000AE69
 *  30..40 stub  41 10007CAA
 */
static void (*const g_trigger[42])(void) = {
    l38_trig00, trig_noop, trig02, trig_noop, trig04, trig05, trig06, trig07,
    trig08, trig09, trig10, trig11, trig12, trig13, trig14, trig15, trig16,
    trig17, trig_noop, trig_noop, trig_noop, trig21, trig22, trig23, trig24,
    trig25, trig26, trig27, trig28, trig29, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001BD60]). */
void l38_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* One ambient creature of trigger 0's block: the original evaluates
 * random(65535) first, then random(200). */
static void place_ambient(i32 x, i32 z) {
    i32 a = CB->random(65535);
    i32 y = CB->random(200) + 900;
    CB->place_animal(6, x, y, z, a, 1000);
}

/* Closing tail of the press-action template. */
static void talk_end(void) {
    CB->animation("Sar_50");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

/* Template of triggers 7..11: three lines the first time (flag 0x524),
 * then one of three at random. `spec` is the direction_spec angle. */
static void talk_random3(i32 spec) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(spec)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(0x524) == 0) {
                lieu_action_register("L38D15A", P_ARKHAN2, "SG00DIA01", NULL);
                lieu_action_register("L38D15B", P_ARKHAN2, "SG00DIA01", NULL);
                lieu_action_register("L38D15C", P_ARKHAN2, "SG00DIA01", NULL);
                ST(0x524) = 1;
            } else {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) {
                    lieu_action_register("L38D16", P_ARKHAN2, "SG00DIA01", NULL);
                }
                if (ST(V_DLG_RANDOM) == 1) {
                    lieu_action_register("L38D17", P_ARKHAN2, "SG00DIA01", NULL);
                }
                if (ST(V_DLG_RANDOM) == 2) {
                    lieu_action_register("L38D18", P_ARKHAN2, "SG00DIA01", NULL);
                }
            }
            talk_end();
            return;
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(0x524) == 0) {
                lieu_action_register("L38D19A", P_ARKHAN3, "SG00DIA01", NULL);
                lieu_action_register("L38D19B", P_ARKHAN3, "SG00DIA01", NULL);
                lieu_action_register("L38D19C", P_ARKHAN3, "SG00DIA01", NULL);
                ST(0x524) = 1;
            } else {
                ST(V_DLG_RANDOM) = CB->random(3);
                if (ST(V_DLG_RANDOM) == 0) {
                    lieu_action_register("L38D20", P_ARKHAN3, "SG00DIA01", NULL);
                }
                if (ST(V_DLG_RANDOM) == 1) {
                    lieu_action_register("L38D21", P_ARKHAN3, "SG00DIA01", NULL);
                }
                if (ST(V_DLG_RANDOM) == 2) {
                    lieu_action_register("L38D22", P_ARKHAN3, "SG00DIA01", NULL);
                }
            }
            talk_end();
            return;
        }
    }
}

/* Template of triggers 15..17, 21, 22: one of two lines at random per hero.
 * `by_zone` selects direction_zone(arg) instead of direction_spec(arg). */
static void talk_random2(i32 by_zone, i32 arg, const c8 *a2, const c8 *b2, const c8 *a3, const c8 *b3) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && (by_zone ? CB->direction_zone(arg) : CB->direction_spec(arg))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        if (ST(ST_HERO) == P_ARKHAN2) {
            ST(V_DLG_RANDOM) = CB->random(2);
            if (ST(V_DLG_RANDOM) == 0) {
                lieu_action_register(a2, P_ARKHAN2, "SG00DIA01", NULL);
            }
            if (ST(V_DLG_RANDOM) == 1) {
                lieu_action_register(b2, P_ARKHAN2, "SG00DIA01", NULL);
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            ST(V_DLG_RANDOM) = CB->random(2);
            if (ST(V_DLG_RANDOM) == 0) {
                lieu_action_register(a3, P_ARKHAN3, "SG00DIA01", NULL);
            }
            if (ST(V_DLG_RANDOM) == 1) {
                lieu_action_register(b3, P_ARKHAN3, "SG00DIA01", NULL);
            }
        }
        talk_end();
    }
}

/* Template of triggers 12, 27..29: a line by hero and walk-set toggle
 * (ST_SPECIAL_MODE). The caller supplies the already evaluated test. */
static void talk_special(void) {
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
        lieu_action_register("L38D23", P_ARKHAN2, "SG00DIA01", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
        lieu_action_register("L38D24", P_ARKHAN2, "SG00DIA01", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
        lieu_action_register("L38D25", P_ARKHAN3, "SG00DIA01", NULL);
    }
    if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
        lieu_action_register("L38D26", P_ARKHAN3, "SG00DIA01", NULL);
    }
    talk_end();
}

/* Template of triggers 13, 14 and 26: a two-line toggle on flag `flag` with
 * LCOD70 / LCOD71 as the second line. */
static void talk_toggle(i32 flag, const c8 *first2, const c8 *first3) {
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    if (ST(ST_HERO) == P_ARKHAN2) {
        if (ST(flag) == 0) {
            lieu_action_register(first2, P_ARKHAN2, "SG00DIA01", NULL);
            ST(flag) = 1;
        } else {
            lieu_action_register("LCOD70", P_ARKHAN2, "SG00DIA01", NULL);
            ST(flag) = 0;
        }
    }
    if (ST(ST_HERO) == P_ARKHAN3) {
        if (ST(flag) == 0) {
            lieu_action_register(first3, P_ARKHAN3, "SG00DIA01", NULL);
            ST(flag) = 1;
        } else {
            lieu_action_register("LCOD71", P_ARKHAN3, "SG00DIA01", NULL);
            ST(flag) = 0;
        }
    }
    talk_end();
}

/* trigger 0 - trig00_100073F0 */
void l38_trig00(void) {
    /* 0x73FB */
    if (ST(ST_START_POINT(38)) == 1) {
        ST(ST_LEVEL) = 37;
        ST(ST_HERO) = P_ARKHAN2;
    }
    ST(ST_START_POINT(38)) = 0;
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
    place_ambient(-300, 1125);
    place_ambient(-696, 947);
    place_ambient(-1000, 375);
    place_ambient(-921, 18);
    place_ambient(-752, -525);
    place_ambient(-555, -700);
    place_ambient(595, 468);
    place_ambient(788, 495);
    place_ambient(-1000, 375);
    place_ambient(-921, 18);
    place_ambient(-752, -525);
    place_ambient(-555, -700);
    CB->ajoute_zone(2, 8, 2214, 405, 120, 1);
    CB->ajoute_zone(4, 1314, 785, 130, 135, 1);
    CB->ajoute_zone(5, 532, 1422, 60, 180, 1);
    CB->ajoute_zone(6, -100, -572, 130, 160, 0);
    CB->ajoute_zone(7, -155, 1125, 290, 190, 0);
    CB->ajoute_zone(8, 53, 1051, 136, 226, 0);
    CB->ajoute_zone(9, 254, 955, 266, 255, 0);
    CB->ajoute_zone(10, -385, 1006, 166, 215, 0);
    CB->ajoute_zone(11, -537, 777, 130, 360, 0);
    CB->ajoute_zone(12, -645, 339, 165, 355, 0);
    CB->ajoute_zone(13, -1281, 110, 190, 660, 0);
    CB->ajoute_zone(14, -1243, 660, 281, 428, 0);
    CB->ajoute_zone(15, -1119, -360, 170, 235, 0);
    CB->ajoute_zone(16, -972, -596, 170, 295, 0);
    CB->ajoute_zone(17, -798, -813, 168, 244, 0);
    CB->ajoute_zone(21, -445, 1280, 255, 255, 0);
    CB->ajoute_zone(22, -738, 45, 255, 255, 0);
    CB->ajoute_zone(23, 51, -551, 150, 135, 0);
    CB->ajoute_zone(24, 90, -215, 236, 241, 0);
    CB->ajoute_zone(25, 362, -41, 450, 240, 0);
    CB->ajoute_zone(26, -408, -835, 605, 205, 0);
    CB->ajoute_zone(27, -640, -229, 262, 251, 0);
    CB->ajoute_zone(28, -605, -445, 250, 250, 0);
    CB->ajoute_zone(29, -346, -524, 252, 251, 0);
    /* 0x7A6E */
    if (ST(0x55C) == 0) {
        ST(0x55C) = 1;
    }
    /* 0x7A8F */
    if (ST(ST_LEVEL) == 37) {
        CB->pos_perso(ST(ST_HERO), 85, 1903, 0xAB65, 1, "ar", 1, 0);
    }
    /* 0x7AC9 */
    if (ST(ST_LEVEL) == 39) {
        /* 0x7ADD */
        if (ST(0xD84) == 1) {
            CB->pos_perso(ST(ST_HERO), 1491, 622, 0xA4F5, 1, "ar", 1, 0);
        }
        /* 0x7B15 */
        if (ST(0xD84) == 2) {
            CB->pos_perso(ST(ST_HERO), 394, 1366, 0x3287, 1, "ar", 1, 0);
        }
    }
    /* 0x7B52 */
    if (ST(0xC40) == 0) {
        CB->pos_perso(91, 211, -279, 0x76F9, 1, "P91ARR01", 0x14, 0);
    }
    /* 0x7B89 */
    if (ST(0xC40) == 1) {
        CB->pos_perso(91, 211, -279, 0x76F9, 1, "P91GAR01", 0x14, 0);
    }
    CB->bruitage("L38AMB1", -1, -1);
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 3;
    ST(0x70) = 0;
    ST(0x74) = 10;
    ST(0x78) = 0;
    ST(0x7C) = 0;
    ST(0x80) = 0;
    ST(0x84) = 0;
    ST(0x88) = 0;
    ST(0x524) = 0;
    /* 0x7C60 */
    if (ST(0xD84) == 2) {
        CB->vire_col(25, 0);
    }
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
}

/* trigger 41 - trig41_10007CAA */
static void trig41(void) {
    /* 0x7CB5 */
    if (ST(0xF34) == 0) {
        ST(0xF34) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x7CE5 */
        if (ST(ST_COMBAT_STANCE) == 1) {
            ST(0x88) = 1;
            CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
        }
        CB->change_camera(2, CAM_LOCKED);
        CB->joindre(ST(ST_HERO), -114, 1317, 0x7C1A, -1);
        CB->animation("Sar");
        /* 0x7D52 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L38D01A", P_ARKHAN2);
            CB->dialogue("L38D01B", P_ARKHAN2);
        }
        /* 0x7D84 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L38D02A", P_ARKHAN3);
            CB->dialogue("L38D02B", P_ARKHAN3);
        }
        CB->change_camera(2, CAM_AUTO);
        CB->bloque(BLOQUE_NONE);
        /* 0x7DCE */
        if (ST(0x88) != 0) {
            CB->divers_empile(DV_COMBAT_STANCE, 1, 0);
        }
    }
    /* 0x7DF2 */
    if (ST(0xC40) == 1) {
        ST(0xE0) = 99;
    }
}

/* trigger 2 - trig02_10008722 */
static void trig02(void) {
    ST(ST_LEVEL) = 38;
    CB->change_piece(37, 0);
}

/* trigger 4 - trig04_10008751 */
static void trig04(void) {
    ST(0xD5C) = 1;
    ST(ST_LEVEL) = 38;
    CB->retire_zone(4);
    CB->change_piece(39, 0);
}

/* trigger 5 - trig05_1000878F */
static void trig05(void) {
    ST(0xD5C) = 2;
    ST(ST_LEVEL) = 38;
    CB->retire_zone(5);
    CB->change_piece(39, 0);
}

/* trigger 6 - trig06_100087CD */
static void trig06(void) {
    /* 0x87D7 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(6)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8836 */
        if (ST(ST_PLAYER_WEAPON) != 74) {
            /* 0x8848 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L38D07", P_ARKHAN2, "SG00DIA01", NULL);
                /* 0x8870 */
                if (ST(0xB10) == 1) {
                    lieu_action_register("L38D08", P_ARKHAN2, "SG00DIA01", NULL);
                }
                talk_end();
                return;
            }
            /* 0x88C1 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L38D09", P_ARKHAN3, "SG00DIA01", NULL);
                /* 0x88E9 */
                if (ST(0xB10) == 1) {
                    lieu_action_register("L38D10", P_ARKHAN3, "SG00DIA01", NULL);
                }
                talk_end();
                return;
            }
            return;
        }
        /* 0x893F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x8951 */
            if (ST(0x62C) == 0) {
                ST(0x62C) = 1;
                lieu_action_register("L38D07", P_ARKHAN2, "SG00DIA01", NULL);
                lieu_action_register("L38D11", P_ARKHAN2, "SG00DIA01", NULL);
                CB->animation("SP02ACT50");
                CB->etat_objet("OB45", -1, -1, -1, -1, -1, -1, -3, 99);
                talk_end();
                return;
            } else {
                CB->animation("SP02ACT50");
                lieu_action_register("L38D12", P_ARKHAN2, "SG00DIA01", NULL);
                talk_end();
                return;
            }
        }
        /* 0x8A3F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x8A51 */
            if (ST(0x62C) == 0) {
                ST(0x62C) = 1;
                lieu_action_register("L38D09", P_ARKHAN3, "SG00DIA01", NULL);
                lieu_action_register("L38D13", P_ARKHAN3, "SG00DIA01", NULL);
                CB->animation("SP03ACT50");
                CB->etat_objet("OB45", -1, -1, -1, -1, -1, -1, -3, 99);
                talk_end();
                return;
            }
            CB->animation("SP03ACT50");
            lieu_action_register("L38D14", P_ARKHAN3, "SG00DIA01", NULL);
            talk_end();
            return;
        }
    }
}

/* trigger 7 - trig07_10008B38 */
static void trig07(void) {
    talk_random3(0x8000);
}

/* trigger 8 - trig08_10008DFE */
static void trig08(void) {
    talk_random3(0x61A8);
}

/* trigger 9 - trig09_100090C4 */
static void trig09(void) {
    talk_random3(0x61A8);
}

/* trigger 10 - trig10_1000938A */
static void trig10(void) {
    talk_random3(0x9664);
}

/* trigger 11 - trig11_10009650 */
static void trig11(void) {
    talk_random3(0xB3B0);
}

/* trigger 12 - trig12_10009916 */
static void trig12(void) {
    /* 0x9920 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0xAFC8)) {
        talk_special();
    }
}

/* trigger 13 - trig13_10009A89 */
static void trig13(void) {
    /* 0x9A93 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x5140)) {
        talk_toggle(0x7C, "L38D27", "L38D28");
    }
}

/* trigger 14 - trig14_10009BE8 */
static void trig14(void) {
    /* 0x9BF2 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && !CB->direction_spec(0xA7F8) && !CB->direction_spec(0x7D00)) {
        talk_toggle(0x78, "L38D29", "L38D30");
    }
}

/* trigger 15 - trig15_10009D5A */
static void trig15(void) {
    talk_random2(0, 0x5208, "L38D31", "L38D18", "L38D32", "L38D22");
}

/* trigger 16 - trig16_10009ED7 */
static void trig16(void) {
    talk_random2(0, 0x61A8, "L38D31", "L38D18", "L38D32", "L38D22");
}

/* trigger 17 - trig17_1000A054 */
static void trig17(void) {
    talk_random2(0, 0x7530, "L38D31", "L38D18", "L38D32", "L38D22");
}

/* trigger 21 - trig21_1000A1F2 */
static void trig21(void) {
    talk_random2(1, 21, "L38D33", "L38D18", "L38D34", "L38D22");
}

/* trigger 22 - trig22_1000A36F */
static void trig22(void) {
    talk_random2(1, 22, "L38D35", "L38D33", "L38D36", "L38D34");
}

/* trigger 23 - trig23_1000A4EC */
static void trig23(void) {
    /* 0xA4F6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && (CB->direction_spec(0xD6D8) || CB->direction_spec(0xFDE8))) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA577 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA589 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xA59B */
                if (ST(0x80) == 0) {
                    lieu_action_register("L38D37", P_ARKHAN2, "SG00DIA01", NULL);
                    ST(0x80) = 1;
                } else {
                    lieu_action_register("L38D38", P_ARKHAN2, "SG00DIA01", NULL);
                }
            }
            /* 0xA5ED */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xA607 */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT05");
                }
                /* 0xA62C */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP02ACT31");
                }
                CB->dialogue("L38D39", P_ARKHAN2);
            }
        }
        /* 0xA659 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            /* 0xA67D */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0xA68F */
                if (ST(0x80) == 0) {
                    lieu_action_register("L38D40", P_ARKHAN3, "SG00DIA01", NULL);
                    ST(0x80) = 1;
                } else {
                    lieu_action_register("L38D41", P_ARKHAN3, "SG00DIA01", NULL);
                }
            }
            /* 0xA6E1 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0xA6FB */
                if (CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT05");
                }
                /* 0xA720 */
                if (!CB->divers(DV_CAN_FIGHT, 9, 2)) {
                    CB->animation("SP03ACT31");
                }
                CB->dialogue("L38D42", P_ARKHAN3);
            }
        }
        talk_end();
    }
}

/* trigger 24 - trig24_1000A76F */
static void trig24(void) {
    /* 0xA779 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x24B8)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA7D8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L38D43A", P_ARKHAN2, "SG00DIA01", NULL);
            lieu_action_register("L38D43B", P_ARKHAN2, "SG00DIA01", NULL);
        }
        /* 0xA816 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L38D44A", P_ARKHAN3, "SG00DIA01", NULL);
            lieu_action_register("L38D44B", P_ARKHAN3, "SG00DIA01", NULL);
        }
        talk_end();
    }
}

/* trigger 25 - trig25_1000A876 */
static void trig25(void) {
    /* 0xA883 */
    if (ST(ST_HERO) != 4 && ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(25) && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) < 150) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA914 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA926 */
            if (ST(0x84) == 0) {
                lieu_action_register("L38D45", P_ARKHAN2, "SG00DIA01", NULL);
                ST(0x84) = 1;
            } else {
                lieu_action_register("L38D46", P_ARKHAN2, "SG00DIA01", NULL);
            }
        }
        /* 0xA978 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xA98A */
            if (ST(0x84) == 0) {
                lieu_action_register("L38D47", P_ARKHAN3, "SG00DIA01", NULL);
                ST(0x84) = 1;
            } else {
                lieu_action_register("L38D48", P_ARKHAN3, "SG00DIA01", NULL);
            }
        }
        talk_end();
    }
}

/* trigger 26 - trig26_1000A9FE */
static void trig26(void) {
    /* 0xAA08 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_spec(0x88B8)) {
        talk_toggle(0x78, "L38D29", "L38D30");
    }
}

/* trigger 27 - trig27_1000AB5D */
static void trig27(void) {
    /* 0xAB67 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_spec(0xBB80) || CB->direction_spec(0xFDE8))) {
        talk_special();
    }
}

/* trigger 28 - trig28_1000ACE3 */
static void trig28(void) {
    /* 0xACED */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_spec(0xBB80) || CB->direction_spec(0xFDE8))) {
        talk_special();
    }
}

/* trigger 29 - trig29_1000AE69 */
static void trig29(void) {
    /* 0xAE73 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && (CB->direction_spec(0xBB80) || CB->direction_spec(0xFDE8))) {
        talk_special();
    }
}
