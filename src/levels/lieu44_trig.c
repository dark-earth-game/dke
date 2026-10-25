/*
 * lieu44_trig.c — Native hand-port of the LIEU44.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu44.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdio.h>

#include "lieu44_int.h"

static void trig_noop(void);   /* the 36 stub slots */
static void trig04(void); static void trig05(void); static void trig06(void);
static void trig07(void); static void trig41(void);

/* Trigger table (filled at INIT with 5-byte jmp thunks; static here).
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  10007D62 stub  28  10007DFC stub
 *    1  100078F2 stub  15  10007D6D stub  29  10007E07 stub
 *    2  10007D15 stub  16  10007D78 stub  30  10007E12 stub
 *    3  100079C2 stub  17  10007D83 stub  31  10007E1D stub
 *    4  100079CD       18  10007D8E stub  32  10007E28 stub
 *    5  10007A45       19  10007D99 stub  33  10007E33 stub
 *    6  10007C22       20  10007DA4 stub  34  10007E3E stub
 *    7  100078FD       21  10007DAF stub  35  10007E49 stub
 *    8  10007D20 stub  22  10007DBA stub  36  10007E54 stub
 *    9  10007D2B stub  23  10007DC5 stub  37  10007E5F stub
 *   10  10007D36 stub  24  10007DD0 stub  38  10007E6A stub
 *   11  10007D41 stub  25  10007DDB stub  39  10007E75 stub
 *   12  10007D4C stub  26  10007DE6 stub  40  10007E80 stub
 *   13  10007D57 stub  27  10007DF1 stub  41  100075C6
 */
static void (*const g_trigger[42])(void) = {
    l44_trig00, trig_noop, trig_noop, trig_noop, trig04, trig05, trig06, trig07,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx. */
void l44_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 — sub_100073F0 (thunk sub_10001073; INIT default) */
void l44_trig00(void) {
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    ST(0x70) = 0;
    ST(V_EVENT_0) = 0;
    CB->pos_perso(P_ARKHAN3, -1805, -1470, 0xBB80, 1, "ar", 1, 0);
    CB->change_camera(0, CAM_LOCKED);
    CB->ajoute_zone(4, 1770, -1470, 110, 265, 1);
    CB->ajoute_zone(7, -2, -1512, 3094, 1400, 1);
    CB->ajoute_zone(5, -1845, -1500, 120, 240, 0);
    CB->ajoute_zone(6, 1400, -1787, 150, 150, 0);
    CB->place_animal(7, 0, -6351, 0, 0, 1200);
    CB->place_animal(7, 0, -6351, 0, 0, 1200);
    CB->place_animal(7, 0, -6351, 0, 0, 1200);
    CB->place_animal(7, 0, -6351, 0, 0, 1200);
    CB->place_animal(7, 0, -6351, 0, 0, 1200);
    CB->place_animal(7, 0, -6351, 0, 0, 1200);
    CB->bruitage("L44AMB1", -1, -1);
    CB->cinematique("SC33_500");
    CB->bruitage("L44AMB1", -1, -1);
    CB->bloque(BLOQUE_ALL);
}

/* trigger 41 — sub_100075C6 */
static void trig41(void) {
    lieu_action_register("L41D15", P_ARKHAN3, "SG00DIA04", "Sar_10");
    CB->modify_var(&ST(0x6C), 1);
    CB->bloque(BLOQUE_NONE);
    CB->bruitage("L44AMB1", -1, -1);
}

/* trigger 7 — sub_100078FD */
static void trig07(void) {
    /* 0x7903 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(0x64) == 0) {
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x68) = 0;
        ST(0x64) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->animation("SP03ATT75");
        CB->animation("P03ATT78_500");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->modify_var(&ST(0x70), 1);
    }
}

/* trigger 4 — sub_100079CD */
static void trig04(void) {
    CB->retire_zone(4);
    CB->bloque(BLOQUE_ALL);
    lieu_action_register("LCOD93", P_ARKHAN3, "SG00DIA08", "Sar_10");
    CB->animation("Sar_10");
    CB->divers_empile(DV_SCREEN, 5, 0);
    ST(ST_LEVEL) = 44;
    CB->bloque(BLOQUE_NONE);
    CB->change_piece(43, 0);
}

/* trigger 5 — sub_10007A45 */
static void trig05(void) {
    /* 0x7A4B */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4650)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x7AA2 */
        if (ST(ST_PLAYER_WEAPON) == 0 || CB->divers(DV_CAN_FIGHT, 0, 2)) {
            /* 0x7ACB */
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ATT38");
                }
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ATT37");
                }
                lieu_action_register("L41D05", P_ARKHAN3, "G00DIA01", NULL);
            }
            /* 0x7B3D */
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ATT40");
                }
                if (!CB->divers(DV_CAN_FIGHT, 7, 2)) {
                    CB->animation("SP03ATT39");
                }
                lieu_action_register("L41D06", P_ARKHAN3, "G00DIA08", NULL);
            }
        } else {
            /* 0x7BB4 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L41D07", P_ARKHAN3, "SG00DIA60", NULL);
            }
            /* 0x7BDC */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L41D08", P_ARKHAN3, "SG00DIA60", NULL);
            }
        }
        /* 0x7C04 */
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 6 — sub_10007C22 */
static void trig06(void) {
    static c8 buf[32];   /* 0x10017DE0 */

    /* 0x7C28 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x7C6D */
        if (ST(ST_HERO) == 2) {
            lieu_action_register("LCOD53", P_ARKHAN2, "SG00SAV01", "Sar_10");
        }
        /* 0x7C98 */
        if (ST(ST_HERO) == 3) {
            lieu_action_register("LCOD54", P_ARKHAN3, "SG00SAV01", "Sar_10");
        }
        snprintf(buf, sizeof buf, "P%02d_65535", ST(ST_HERO));
        CB->animation("P00_65535");
        CB->bloque(BLOQUE_NONE);
        CB->divers_empile(DV_SAVE_SCREEN, 0, 0);
        CB->select_pile(0, 1);
    }
}
