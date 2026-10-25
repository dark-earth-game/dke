/*
 * lieu40_trig.c - Native hand-port of the LIEU40.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and events live in lieu40.c; this
 * file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * Triggers 5..11 and 17 are copies of one "press action at a spot"
 * template (busy, bloque, one line, release) that differ only in the angle
 * or zone they test and the line they play; the part after the test is a
 * static helper.
 */

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu40_int.h"

static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig10(void);
static void trig11(void); static void trig13(void); static void trig14(void);
static void trig15(void); static void trig16(void); static void trig17(void);
static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* dword_10018180[42] - filled at INIT (0x10008A86..) with the 5-byte jmp
 * thunks at 0x10001000+; static here.
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  10008034       28  100083EB stub
 *    1  100079EF stub  15  10008172       29  100083F6 stub
 *    2  100079FA stub  16  100082B0       30  10008401 stub
 *    3  10007A05 stub  17  100082E6       31  1000840C stub
 *    4  10007A10 stub  18  1000837D stub  32  10008417 stub
 *    5  10007A1B       19  10008388 stub  33  10008422 stub
 *    6  10007ACB       20  10008393 stub  34  1000842D stub
 *    7  10007B7B       21  1000839E stub  35  10008438 stub
 *    8  10007C2B       22  100083A9 stub  36  10008443 stub
 *    9  10007CDB       23  100083B4 stub  37  1000844E stub
 *   10  10007D8B       24  100083BF stub  38  10008459 stub
 *   11  10007E3B       25  100083CA stub  39  10008464 stub
 *   12  10007EEB stub  26  100083D5 stub  40  1000846F stub
 *   13  10007EF6       27  100083E0 stub  41  100076CB
 */
static void (*const g_trigger[42])(void) = {
    l40_trig00, trig_noop, trig_noop, trig_noop, trig_noop, trig05, trig06, trig07,
    trig08, trig09, trig10, trig11, trig_noop, trig13, trig14, trig15,
    trig16, trig17, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10018180]). */
void l40_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 - sub_100073F0 - level setup: start points, ambience, hero placement, zones */
void l40_trig00(void) {
    /* 0x73F6 */
    if (ST(ST_START_POINT(40)) == 1) {
        ST(ST_HERO) = P_ARKHAN3;
        ST(ST_LEVEL) = 51;
    }
    ST(ST_START_POINT(40)) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    CB->bruitage("L40AMB1", -1, -1);
    CB->divers(DV_MUSIC, 1, 0);
    /* 0x746E: came from room 51 */
    if (ST(ST_LEVEL) == 51) {
        CB->pos_perso(P_ARKHAN3, 4815, -1350, 0x8CA0, 1, "P03ATT99", 1, 0);
        /* 0x74A2 */
        if (CB->divers(DV_DARKNESS, -1, 0) < 750) {
            CB->divers_empile(DV_DARKNESS, 750, 0);
        }
        CB->etat_objet("OB29A", -1, -1, -1, -1, -1, -1, -6, 0);
    }
    /* 0x74EF: came from room 41 */
    if (ST(ST_LEVEL) == 41) {
        CB->pos_perso(P_ARKHAN3, 2378, -1443, CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
    }
    ST(0x64) = 0;
    CB->ajoute_zone(5, 6269, -1469, 220, 1433, 0);
    CB->ajoute_zone(6, 2283, -1793, 185, 160, 0);
    CB->ajoute_zone(7, 2359, -1179, 160, 172, 0);
    CB->ajoute_zone(8, 2660, -659, 935, 160, 0);
    CB->ajoute_zone(9, 5816, -2245, 987, 135, 0);
    CB->ajoute_zone(10, 5786, -720, 1032, 155, 0);
    CB->ajoute_zone(11, 4181, -2279, 614, 332, 0);
    CB->ajoute_zone(12, 4228, -2339, 50, 50, 0);
    CB->ajoute_zone(13, 3008, -1104, 231, 191, 0);
    CB->ajoute_zone(14, 3169, -1837, 195, 212, 0);
    CB->ajoute_zone(15, 4875, -1827, 195, 220, 0);
    CB->ajoute_zone(16, 2264, -1433, 95, 335, 1);
}

/* Common tail of the "press action at a spot" triggers: one line, release. */
static void say_line(const c8 *code) {
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    lieu_action_register(code, P_ARKHAN3, "SG03DIA05", NULL);
    CB->animation("Sar_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

/* trigger 05 - sub_10007A1B */
static void trig05(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xBA31)) {
        say_line("L40D01");
    }
}

/* trigger 06 - sub_10007ACB */
static void trig06(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xF557)) {
        say_line("L40D02");
    }
}

/* trigger 07 - sub_10007B7B */
static void trig07(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x3D9B)) {
        say_line("L40D02");
    }
}

/* trigger 08 - sub_10007C2B */
static void trig08(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xF557)) {
        say_line("L40D03");
    }
}

/* trigger 09 - sub_10007CDB */
static void trig09(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x9344)) {
        say_line("L40D03");
    }
}

/* trigger 10 - sub_10007D8B */
static void trig10(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xF79E)) {
        say_line("L40D03");
    }
}

/* trigger 11 - sub_10007E3B */
static void trig11(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(12)) {
        say_line("L40D03");
    }
}

/* Triggers 13..15: a random one of four lines (the copies differ only in
 * their string literals, which are the same text). */
static void say_random(void) {
    CB->bloque(BLOQUE_ALL);
    ST(ST_SCRIPT_BUSY) = 1;
    ST(V_DLG_RANDOM) = CB->random(4);
    if (ST(V_DLG_RANDOM) == 0) {
        lieu_action_register("L40D06A", P_ARKHAN3, "SG03DIA05", NULL);
    }
    if (ST(V_DLG_RANDOM) == 1) {
        lieu_action_register("L40D06B", P_ARKHAN3, "SG03DIA05", NULL);
    }
    if (ST(V_DLG_RANDOM) == 2) {
        lieu_action_register("LCOD08", P_ARKHAN3, "SG03DIA05", NULL);
    }
    if (ST(V_DLG_RANDOM) == 3) {
        lieu_action_register("LCOD77", P_ARKHAN3, "SG03DIA05", NULL);
    }
    CB->animation("Sar_10");
    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
    CB->bloque(BLOQUE_NONE);
}

/* trigger 13 - sub_10007EF6 */
static void trig13(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        say_random();
    }
}

/* trigger 14 - sub_10008034 */
static void trig14(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        say_random();
    }
}

/* trigger 15 - sub_10008172 */
static void trig15(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        say_random();
    }
}

/* trigger 16 - sub_100082B0 - exit to room 41 */
static void trig16(void) {
    CB->divers(DV_MUSIC, 1, 4);
    ST(ST_LEVEL) = 40;
    CB->change_piece(41, 0);
}

/* trigger 17 - sub_100082E6 */
static void trig17(void) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->dialogue("L40D08", P_ARKHAN3);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 41 - sub_100076CB - cutscene on entering the room, once */
static void trig41(void) {
    CB->divers(DV_MUSIC, 1, 1);
    /* 0x76E3 */
    if (ST(0x4F8) == 0) {
        CB->change_camera(4, CAM_LOCKED);
        CB->divers_empile(DV_SCREEN, 3, 0);
        ST(0x4F8) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SP03ATT99");
        CB->animation("P03ATT99");
        CB->animation("SP03ATT98");
        CB->dialogue("L40D07A", P_ARKHAN3);
        CB->animation("G03ARR13_10");
        lieu_action_register("L40D07B", P_ARKHAN3, "SG00ATT01", NULL);
        CB->animation("SG03ARR13_10");
        CB->joindre(-ST(ST_HERO), 0, 0, 0xC299, -1);
        CB->animation("SG03ARR13_10");
        CB->change_camera(0, CAM_LOCKED);
        CB->joindre(-ST(ST_HERO), 4987, -1407, 0xC299, -1);
        CB->animation("SG03ARR13_10");
        lieu_action_register("L40D04A", P_ARKHAN3, "SG03DIA06", NULL);
        lieu_action_register("L40D04B", P_ARKHAN3, "SG03DIA07", NULL);
        CB->animation("SG03ARR13_10");
        CB->change_camera(4, CAM_LOCKED);
        CB->joindre(-ST(ST_HERO), 0, 0, 0x825F, -1);
        CB->animation("SG03ARR13_10");
        lieu_action_register("L40D05A", P_ARKHAN3, "SG00CON01", NULL);
        lieu_action_register("L40D05B", P_ARKHAN3, "SG00ATT01", NULL);
        CB->change_camera(0, CAM_AUTO);
        CB->animation("Sar_10");
        CB->bloque(BLOQUE_NONE);
    }
}
