/*
 * lieu29_trig.c - Native hand-port of the LIEU29.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu29.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu29_int.h"

/* -------------------------------------------------------------------------
 * Forward decls for the trigger table.
 * ---------------------------------------------------------------------- */
static void trig02(void);
static void trig03(void);
static void trig04(void);
static void trig05(void);
static void trig06(void);
static void trig07(void);
static void trig08(void);
static void trig09(void);
static void trig11(void);
static void trig13(void);
static void trig41(void);
static void trig_noop(void);   /* the 29 stub slots (shared L01 trig06 body) */

/* trigger table at 0x1001C3F0 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000+5*i; static here. Slots 1, 10, 12, 14..40 are stubs
 * (empty bodies, identical to L01 trig06).
 *   idx handler        idx handler        idx handler
 *    0  100073F0       14  1000AF00 stub  28  1000AF9A stub
 *    1  100088F8 stub  15  1000AF0B stub  29  1000AFA5 stub
 *    2  10008903       16  1000AF16 stub  30  1000AFB0 stub
 *    3  10009C64       17  1000AF21 stub  31  1000AFBB stub
 *    4  10009F3D       18  1000AF2C stub  32  1000AFC6 stub
 *    5  1000A0E7       19  1000AF37 stub  33  1000AFD1 stub
 *    6  1000A1F5       20  1000AF42 stub  34  1000AFDC stub
 *    7  1000A303       21  1000AF4D stub  35  1000AFE7 stub
 *    8  1000A744       22  1000AF58 stub  36  1000AFF2 stub
 *    9  1000A848       23  1000AF63 stub  37  1000AFFD stub
 *   10  1000ABE4 stub  24  1000AF6E stub  38  1000B008 stub
 *   11  1000ABEF       25  1000AF79 stub  39  1000B013 stub
 *   12  1000AE1A stub  26  1000AF84 stub  40  1000B01E stub
 *   13  1000AE25       27  1000AF8F stub  41  10007946
 */
static void (*const g_trigger[42])(void) = {
    l29_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig09, trig_noop, trig11, trig_noop, trig13, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001C3F0]). */
void l29_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 - sub_100073F0 (thunk sub_10001073; INIT default) */
void l29_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(29)) == 1) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xB58) = 10;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_LEVEL) = 27;
    }
    /* 0x744D */
    if (ST(ST_START_POINT(29)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xB38) = 1;
        ST(ST_LEVEL) = 27;
    }
    /* 0x7489 */
    if (ST(ST_START_POINT(29)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xAF8) = 1;
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(ST_LEVEL) = 27;
    }
    /* 0x74D4 */
    if (ST(ST_START_POINT(29)) == 4) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xB38) = 3;
        ST(V_GUARD_FIGHT_DONE) = 2;
        ST(ST_LEVEL) = 27;
    }
    ST(ST_START_POINT(29)) = 0;
    CB->bruitage("L29AMB1", 7491, -4256);
    /* 0x7543 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 1, 0);
    }
    /* 0x7567 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB38) >= 1) {
        CB->divers(DV_MUSIC, 2, 0);
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
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(V_LAST_WEAPON) = 0;
    CB->ajoute_zone(4, 8127, -4598, 215, 235, 0);
    CB->ajoute_zone(5, 7689, -4542, 155, 180, 0);
    CB->ajoute_zone(6, 7887, -4564, 140, 225, 0);
    CB->ajoute_zone(7, 7498, -4475, 85, 135, 1);
    CB->ajoute_zone(8, 7542, -4297, 230, 135, 0);
    CB->ajoute_zone(9, 7845, -4405, 140, 220, 0);
    CB->ajoute_zone(10, 7848, -4400, 50, 50, 0);
    CB->ajoute_zone(11, 7608, -4274, 190, 140, 0);
    CB->ajoute_zone(12, 8277, -4496, 20, 10, 0);
    CB->ajoute_zone(13, 8277, -4496, 340, 340, 0);
    CB->pos_perso(ST(ST_HERO), 7564, -4464, 0xCA0D, 1, "ar", 1, 0);
    /* 0x7802 */
    if (ST(0xB58) == 10 && ST(0xB38) == 0 && ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->pos_perso(P_LORY, 8401, -4435, 0x4CFA, 1, "P15ARR01", 2, 0);
        /* 0x785D */
        if (ST(0xE04) == 0) {
            CB->pos_perso(P_PHEDORIA, 7738, -4538, 0x1C92, 1, "P26ARR01", 3, 0);
        }
        /* 0x7894 */
        if (ST(0xE04) >= 1) {
            CB->pos_perso(P_PHEDORIA, 7738, -4538, 0xFF68, 1, "P26ARR01", 3, 0);
        }
    }
    /* 0x78CB */
    if (ST(0xB38) == 1) {
        CB->divers(DV_MUSIC, 2, 0);
        ST(0xB38) = 2;
        ST(ST_VIE(P_LORY)) = VIE_DEAD;
        ST(ST_VIE(P_PHEDORIA)) = VIE_DEAD;
    }
    /* 0x791C */
    if (ST(0xB38) == 0 || ST(0xB38) > 2) {
        CB->vire_col(0xC, 0);
    }
}

/* trigger 2 - sub_10008903 */
static void trig02(void) {
    /* 0x8912 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_LORY)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x8978 */
        if (ST(0x64) == 0) {
            ST(0x64) = 1;
        }
        /* 0x8996 */
        if (ST(ST_VIE(P_LORY)) != VIE_DEAD) {
            CB->joindre(ST(ST_HERO), 0, 0, 0, P_LORY);
            CB->joindre(P_LORY, 0, 0, 0, ST(ST_HERO));
            /* 0x89DE */
            if (ST(ST_PLAYER_WEAPON) == 0 || (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) == ST(V_LAST_WEAPON))) {
                /* 0x8A1C */
                if (ST(0xCA0) == 0) {
                    ST(0xCA0) = 1;
                    /* 0x8A3D */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L29D01", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x8A65 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D02", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    lieu_action_register("L29D03A", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D03B", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D03C", P_LORY, "SP15DIA59", "SP15ARR01_10");
                    lieu_action_register("L29D03D", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D03E", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    /* 0x8B0A */
                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L29D04A", P_ARKHAN2, "SG00DIA05", NULL);
                        lieu_action_register("L29D04B", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x8B5A */
                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L29D05", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x8B94 */
                    if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L29D06", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    /* 0x8BCE */
                    if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L29D07", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    lieu_action_register("L29D08A", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D08B", P_LORY, "SP15DIA56", "SP15ARR01_10");
                    lieu_action_register("L29D08C", P_LORY, "SP15DIA56", "SP15ARR01_10");
                    lieu_action_register("L29D08D", P_LORY, "SP15DIA60", "SP15ARR01_10");
                    /* 0x8C6C */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L29D09A", P_ARKHAN2, "SG00DIA01", NULL);
                        lieu_action_register("L29D09B", P_ARKHAN2, "SG00DIA01", NULL);
                    }
                    /* 0x8CAA */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D10A", P_ARKHAN3, "SG00DIA01", NULL);
                        lieu_action_register("L29D10B", P_ARKHAN3, "SG00DIA01", NULL);
                    }
                    lieu_action_register("L29D11A", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D11B", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D11C", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D11D", P_LORY, "SP15DIA59", "SP15ARR01_10");
                    lieu_action_register("L29D11E", P_LORY, "SP15DIA56", "SP15ARR01_10");
                    /* 0x8D65 */
                    if (ST(ST_PLAYER_WEAPON) != 0) {
                        /* 0x8D77 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->animation("SP02PRE73");
                            CB->animation("ar");
                        }
                        /* 0x8D9F */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->animation("SP03PRE73");
                            CB->animation("ar");
                        }
                    }
                    CB->link_objet(0xF, "OB02", 0x1B, 1);
                    lieu_pre_scene(ST(ST_HERO), P_LORY, 2, 1, 2);
                    ST(0xB10) = 1;
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x8E2E */
                if (ST(0xCA0) == 1 && (ST(ST_HERO) != P_ARKHAN3 || ST(ST_SPECIAL_MODE) != 0)) {
                    ST(0xCA0) = 2;
                    /* 0x8E73 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L29D12A", P_ARKHAN2, "SG00DIA07", NULL);
                        lieu_action_register("L29D12B", P_ARKHAN2, "SG00DIA07", NULL);
                    }
                    /* 0x8EB1 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D13A", P_ARKHAN3, "SG00DIA05", NULL);
                        lieu_action_register("L29D13B", P_ARKHAN3, "SG00DIA05", NULL);
                        lieu_action_register("L29D13C", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    lieu_action_register("L29D14A", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D14B", P_LORY, "SP15DIA56", "SP15ARR01_10");
                    lieu_action_register("L29D14C", P_LORY, "SP15DIA56", "SP15ARR01_10");
                    lieu_action_register("L29D14D", P_LORY, "SP15DIA59", "SP15ARR01_10");
                    lieu_action_register("L29D14E", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D14F", P_LORY, "SP15DIA54", "SP15ARR01_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x8FC4 */
                if (ST(0xCA0) == 2 && (ST(ST_HERO) != P_ARKHAN3 || ST(ST_SPECIAL_MODE) != 0)) {
                    ST(0xCA0) = 3;
                    /* 0x9009 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L29D15", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x9031 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D16", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    lieu_action_register("L29D17A", P_LORY, "SP15DIA56", "SP15ARR01_10");
                    lieu_action_register("L29D17B", P_LORY, "SP15DIA56", "SP15ARR01_10");
                    lieu_action_register("L29D17C", P_LORY, "SP15DIA58", "SP15ARR01_10");
                    lieu_action_register("L29D17D", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D17E", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D17F", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D17G", P_LORY, "SP15DIA54", "SP15ARR01_10");
                    /* 0x9108 */
                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L29D18", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x916B */
                if (ST(0xCA0) == 3 && (ST(ST_HERO) != P_ARKHAN3 || ST(ST_SPECIAL_MODE) != 0)) {
                    ST(0xCA0) = 4;
                    /* 0x91B0 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L29D19", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x91D8 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D20", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    lieu_action_register("L29D21A", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D21B", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    /* 0x9232 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L29D22", P_ARKHAN2, "SG00DIA05", NULL);
                        /* 0x925A */
                        if (ST(ST_SPECIAL_MODE) == 0) {
                            lieu_action_register("L29D23", P_ARKHAN2, "SG00DIA05", NULL);
                        }
                    }
                    /* 0x9282 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D24", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    lieu_action_register("L29D25A", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D25B", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D25C", P_LORY, "SP15DIA59", "SP15ARR01_10");
                    lieu_action_register("L29D25D", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9337 */
                if (ST(0xCA0) == 4 && (ST(ST_HERO) != P_ARKHAN3 || ST(ST_SPECIAL_MODE) != 0)) {
                    ST(0xCA0) = 5;
                    /* 0x937C */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L29D26", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x93A4 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D27", P_ARKHAN3, "SG00DIA05", NULL);
                    }
                    lieu_action_register("L29D28A", P_LORY, "SP15DIA53", "SP15ARR01_10");
                    lieu_action_register("L29D28B", P_LORY, "SP15DIA54", "SP15ARR01_10");
                    lieu_action_register("L29D28C", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D28D", P_LORY, "SP15DIA56", "SP15ARR01_10");
                    lieu_action_register("L29D28E", P_LORY, "SP15DIA54", "SP15ARR01_10");
                    lieu_action_register("L29D28F", P_LORY, "SP15DIA57", "SP15ARR01_10");
                    lieu_action_register("L29D28G", P_LORY, "SP15DIA59", "SP15ARR01_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x94A4 */
                if (ST(0xCA0) >= 5 && (ST(ST_HERO) != P_ARKHAN3 || ST(ST_SPECIAL_MODE) != 0)) {
                    /* 0x94DA */
                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L29D29", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x9514 */
                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L29D30", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x954E */
                    if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L29D31", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    /* 0x9588 */
                    if (ST(0xB10) != 2) {
                        ST(V_DLG_RANDOM) = CB->random(2);
                    }
                    /* 0x95B1 */
                    if (ST(0xB10) == 2) {
                        ST(V_DLG_RANDOM) = 0;
                    }
                    /* 0x95D2 */
                    if (ST(V_DLG_RANDOM) == 0) {
                        lieu_action_register("L29D32", P_LORY, "SP15DIA60", "SP15ARR01_10");
                    }
                    /* 0x95FD */
                    if (ST(V_DLG_RANDOM) == 1) {
                        lieu_action_register("L29D33", P_LORY, "SP15DIA60", "SP15ARR01_10");
                    }
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x9651 */
                if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0 && ST(0xCA0) > 0) {
                    /* 0x9687 */
                    if (ST(0xCAC) == 0) {
                        ST(0xCAC) = 1;
                        CB->dialogue("LCOD22A", P_ARKHAN3);
                        lieu_action_register("L29D34", P_LORY, "SP15DIA51", "SP15ARR01_10");
                        lieu_action_register("L29D35", P_PHEDORIA, "SP26DIA59", "SP26ARR01_10");
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        CB->bloque(BLOQUE_NONE);
                        return;
                    }
                    /* 0x9713 */
                    if (ST(0xCAC) == 1) {
                        CB->dialogue("LCOD22C", P_ARKHAN3);
                        lieu_action_register("L29D36", P_LORY, "SP15DIA51", "SP15ARR01_10");
                        CB->evenement(1, 100);
                        CB->animation("Sar_20");
                        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                        return;
                    }
                }
            }
            /* 0x977F */
            if (ST(ST_PLAYER_WEAPON) == 2) {
                /* 0x9791 */
                if (ST(0xCA4) == 0) {
                    ST(0xCA4) = 1;
                    /* 0x97B2 */
                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
                        lieu_action_register("L29D37", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x97EC */
                    if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
                        lieu_action_register("L29D38", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x9826 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D39", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    lieu_action_register("L29D40A", P_LORY, "SP15DIA54", "SP15ARR01_10");
                    lieu_action_register("L29D40B", P_LORY, "SP15DIA54", "SP15ARR01_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x98A9 */
                if (ST(0xCA4) == 1) {
                    ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                    /* 0x98CF */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("L29D41", P_ARKHAN2, "SG00DIA05", NULL);
                    }
                    /* 0x98F7 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("L29D42", P_ARKHAN3, "SG00DIA02", NULL);
                    }
                    lieu_action_register("L29D43", P_LORY, "SP15DIA54", "SP15ARR01_10");
                    CB->animation("Sar_20");
                    CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x9961 */
            if (ST(ST_PLAYER_WEAPON) == 75) {
                /* 0x9973 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L29D41", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0x999B */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L29D42", P_ARKHAN3, "SG00DIA02", NULL);
                }
                lieu_action_register("L29D08B", P_LORY, "SP15DIA56", "SP15ARR01_10");
                lieu_action_register("L29D08D", P_LORY, "SP15DIA56", "SP15ARR01_10");
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9A23 */
            if (CB->divers(DV_CAN_FIGHT, 0, 2)) {
                /* 0x9A35 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L29D44", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0x9A5D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L29D45", P_ARKHAN3, "SG00DIA05", NULL);
                }
                lieu_action_register("L29D46", P_LORY, "SP15DIA53", "SP15ARR01_10");
                lieu_action_register("L29D35", P_PHEDORIA, "SP26DIA59", "SP26ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9AD5 */
            if (ST(ST_PLAYER_WEAPON) != 0 && ST(ST_PLAYER_WEAPON) != 2 && !CB->divers(DV_CAN_FIGHT, 0, 2)) {
                ST(V_LAST_WEAPON) = ST(ST_PLAYER_WEAPON);
                /* 0x9B24 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L29D66", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0x9B4C */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L29D67", P_ARKHAN3, "SG00DIA05", NULL);
                }
                lieu_action_register("L29D68A", P_LORY, "SP15DIA53", "SP15ARR01_10");
                lieu_action_register("L29D68B", P_LORY, "SP15DIA53", "SP15ARR01_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
    /* 0x9BC1 */
    if (ST(V_PLAYER_STRIKE) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_PLAYER_STRIKE) = -1;
        ST(V_ENGAGED_PERSONA) = -1;
        CB->animation("SP15DIA51");
        CB->animation("SP26GAR03");
        CB->dialogue("L29D47", P_PHEDORIA);
        CB->evenement(1, 100);
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        return;
    }
}

/* trigger 3 - sub_10009C64 */
static void trig03(void) {
    /* 0x9C73 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_perso(P_PHEDORIA) && ST(ST_COMBAT_STANCE) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(V_EVENT15_STEP) = 1;
        CB->stop_joindre(P_PHEDORIA);
        CB->joindre(P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        /* 0x9D13 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L29D48", P_ARKHAN2, "SG00DIA05", NULL);
        }
        /* 0x9D3B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L29D49", P_ARKHAN3, "SG00DIA02", NULL);
        }
        /* 0x9D63 */
        if (ST(0xCA0) == 0) {
            lieu_action_register("L29D50", P_PHEDORIA, "SP26DIA60", "SP26ARR01_10");
        } else {
            lieu_action_register("L29D51", P_PHEDORIA, "SP26DIA60", "SP26ARR01_10");
        }
        CB->animation("Sar_20");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x9DD2 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && !CB->divers(DV_WAS_HIT, P_LORY, 0) && !CB->divers(DV_WAS_HIT, P_PHEDORIA, 0)) {
        ST(V_PLAYER_STRIKE) = -1;
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9E59 */
        if (ST(0x164) == 0) {
            CB->animation("SP26IMP03");
            CB->dialogue("L29D52", P_PHEDORIA);
            CB->animation("SP26ARR01_100");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->animation("SP26ARR01");
            CB->bloque(BLOQUE_NONE);
            ST(0x164) = 1;
            return;
        }
        /* 0x9EC9 */
        if (ST(0x164) == 1 || ST(V_PLAYER_STRIKE) == 2) {
            CB->bloque(BLOQUE_ALL);
            CB->joindre(P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
            /* 0x9F12 */
            if (ST(0x164) == 1) {
                CB->dialogue("L29D53", P_PHEDORIA);
            }
            CB->evenement(1, 200);
        }
    }
}

/* trigger 4 - sub_10009F3D */
static void trig04(void) {
    /* 0x9F4C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x88B8)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0x9FA6 */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x9FB8 */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT28");
            }
            /* 0x9FE7 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT28");
            }
            /* 0xA004 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SP03ACT28");
            }
            CB->animation("ar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA04A */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0xA05C */
            if (ST(ST_HERO) == P_ARKHAN0 || ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SP01ACT29");
            }
            /* 0xA08B */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SP02ACT29");
            }
            /* 0xA0A8 */
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

/* trigger 5 - sub_1000A0E7 */
static void trig05(void) {
    /* 0xA0F6 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x64AF)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA150 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00DIA03");
            CB->dialogue("LCOD26", P_ARKHAN2);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA1A6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00DIA01");
            CB->dialogue("LCOD27", P_ARKHAN3);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 6 - sub_1000A1F5 */
static void trig06(void) {
    /* 0xA204 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x6914)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA25E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG00DIA03");
            CB->dialogue("LCOD26", P_ARKHAN2);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA2B4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG00DIA03");
            CB->dialogue("LCOD27", P_ARKHAN3);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 7 - sub_1000A303 */
static void trig07(void) {
    /* 0xA315 */
    if (ST(0x80) == 0 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB38) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(0x80) = 1;
        /* 0xA365 */
        if (ST(0xE04) >= 2 && ST(ST_VIE(P_PHEDORIA)) != VIE_DEAD) {
            /* 0xA389 */
            if (ST(0xE04) == 2) {
                CB->joindre(P_LORY, 0, 0, 0, ST(ST_HERO));
                CB->joindre(P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L29D69", P_PHEDORIA, "SP26DIA60", "SP26ARR01_10");
            }
            ST(0xE04) = 3;
            ST(ST_LEVEL) = 29;
            CB->change_piece(27, 1);
        }
        /* 0xA419 */
        if (ST(0xE04) == 1 && ST(ST_VIE(P_LORY)) != VIE_DEAD) {
            /* 0xA43D */
            if (ST(0xCA0) > 0) {
                CB->change_camera(1, CAM_LOCKED);
                lieu_action_register("L29D54A", P_LORY, "SP15DIA60", "SP15ARR01");
                CB->joindre(ST(ST_HERO), 0, 0, 0, P_LORY);
                lieu_action_register("L29D54B", P_LORY, "SP15DIA56", "SP15ARR01");
                lieu_action_register("L29D54C", P_LORY, "SP15DIA53", "SP15ARR01");
                lieu_action_register("L29D54D", P_LORY, "SP15DIA56", "SP15ARR01");
                CB->change_camera(0, CAM_LOCKED);
                /* 0xA4E9 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L29D55", P_ARKHAN2, "SG00DIA05", NULL);
                }
                /* 0xA511 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L29D56", P_ARKHAN3, "SG00DIA02", NULL);
                }
                CB->change_camera(2, CAM_LOCKED);
                lieu_action_register("L29D57", P_LORY, "SP15DIA56", "SP15ARR01");
                CB->change_camera(0, CAM_AUTO);
                ST(0xE04) = 2;
                ST(ST_LEVEL) = 29;
                CB->change_piece(27, 0);
            }
            /* 0xA597 */
            if (ST(0xCA0) == 0) {
                CB->change_camera(1, CAM_LOCKED);
                lieu_action_register("L29D74", P_LORY, "SP15DIA53", "SP15ARR01_10");
                CB->change_camera(0, CAM_LOCKED);
                ST(ST_LEVEL) = 29;
                CB->change_piece(27, 0);
            }
        }
    }
    /* 0xA5F8 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB38) >= 1) {
        CB->divers(DV_MUSIC, 2, 4);
        /* 0xA62E */
        if (ST(0xC48) != 0) {
            /* 0xA640 */
            if (ST(0xC48) == 99 || ST(0xC48) == 100) {
                /* 0xA664 */
                if (ST(0xC48) == 99) {
                    ST(0xC48) = 100;
                }
                ST(ST_LEVEL) = 29;
                CB->change_piece(27, 0);
            }
            /* 0xA69E */
            if (ST(0xC48) == 111) {
                ST(ST_LEVEL) = 29;
                CB->change_piece(27, 1);
            }
        } else {
            ST(ST_LEVEL) = 29;
            CB->change_piece(27, 1);
        }
    }
    /* 0xA6ED */
    if (ST(V_GUARD_FIGHT_DONE) != 2 && ST(0x88) == 0) {
        ST(0x88) = 1;
        CB->divers(DV_MUSIC, 1, 4);
        ST(ST_LEVEL) = 29;
        CB->change_piece(27, 0);
    }
}

/* trigger 8 - sub_1000A744 */
static void trig08(void) {
    /* 0xA753 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->direction_spec(0x4A38)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA7AD */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("L29D58", P_ARKHAN2, "SG02ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA7FE */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L29D59", P_ARKHAN3, "SG03ATT01", NULL);
            CB->animation("Sar_20");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 9 - sub_1000A848 */
static void trig09(void) {
    /* 0xA857 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(0xB38) >= 1 && CB->direction_zone(10)) {
        ST(ST_SCRIPT_BUSY) = 1;
        CB->bloque(BLOQUE_ALL);
        /* 0xA8C3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA8D5 */
            if (ST(0xD2C) == 0 && ST(ST_SPECIAL_MODE) == 1) {
                CB->joindre(P_ARKHAN2, 7810, -4412, 0xAEBD, -1);
                CB->animation("SP02ACT70");
                CB->animation("P02ACT74");
                CB->dialogue("L29D60A", P_ARKHAN2);
                CB->dialogue("L29D60B", P_ARKHAN2);
                CB->dialogue("L29D60C", P_ARKHAN2);
                CB->animation("SP02ACT76");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0xD2C) = 1;
                return;
            }
            /* 0xA99E */
            if (ST(0xD2C) == 1 && ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L29D61", P_ARKHAN2, "SG00ATT01", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xAA01 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02PRE08");
                CB->animation("P02_65535");
                CB->dialogue("L29D62", P_ARKHAN2);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xAA62 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xAA74 */
            if (ST(0xD2C) == 0 && ST(ST_SPECIAL_MODE) == 1) {
                CB->joindre(P_ARKHAN3, 7785, -4394, 0xAEC2, -1);
                CB->animation("SP03ACT70");
                CB->animation("P03ACT74");
                CB->dialogue("L29D63A", P_ARKHAN3);
                CB->dialogue("L29D63B", P_ARKHAN3);
                CB->animation("SP03ACT76");
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(0xD2C) = 1;
                return;
            }
            /* 0xAB2D */
            if (ST(0xD2C) == 1 && ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L29D64", P_ARKHAN2);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xAB8A */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03PRE08");
                CB->animation("P02_65535");
                CB->dialogue("L29D65", P_ARKHAN3);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 11 - sub_1000ABEF */
static void trig11(void) {
    /* 0xABFE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(0xB38) == 2) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAC57 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xAC69 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L29D70A", P_ARKHAN2, "SG02DIA05", NULL);
                lieu_action_register("L29D70B", P_ARKHAN2, "SG02DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xACD0 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02PRE08");
                CB->animation("P02_65535");
                CB->dialogue("L29D72", P_ARKHAN2);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->retire_zone(11);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0xAD3C */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xAD4E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L29D71A", P_ARKHAN3, "SG03DIA05", NULL);
                lieu_action_register("L29D71B", P_ARKHAN3, "SG03DIA05", NULL);
                CB->animation("Sar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0xADB5 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03PRE08");
                CB->animation("P02_65535");
                CB->dialogue("L29D73", P_ARKHAN3);
                CB->animation("ar_20");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->retire_zone(11);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
    }
}

/* trigger 13 - sub_1000AE25 */
static void trig13(void) {
    /* 0xAE34 */
    if (ST(ST_ACTION_KEY) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && CB->direction_zone(12)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAE8E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD29", P_ARKHAN2, "SG00DIA05", NULL);
        }
        /* 0xAEB6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD30", P_ARKHAN3, "SG00DIA05", NULL);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 41 - sub_10007946 */
static void trig41(void) {
    /* 0x7958 */
    if (ST(V_GUARD_FIGHT_DONE) == 2) {
        CB->divers(DV_MUSIC, 1, 1);
    }
    /* 0x797C */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xB38) == 1) {
        CB->divers(DV_MUSIC, 2, 1);
    }
    /* 0x79AF */
    if (ST(0x78) == 0 && ST(0xB38) != 1 && ST(0xB58) == 10 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xE04) < 1) {
        CB->bloque(BLOQUE_ALL);
        ST(0xE04) = 1;
        ST(0x78) = 1;
        lieu_action_register("L29D200", P_PHEDORIA, "SP26DIA59", "SP26ARR01_10");
        CB->divers(DV_LOOK_AT_PLAYER, P_LORY, 1);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x7A67 */
    if (ST(0xB38) >= 1) {
        CB->divers_empile(DV_MUSIC, 2, 1);
    }
}
