/*
 * lieu34_trig.c - Native hand-port of the LIEU34.DLL zone trigger
 * handlers (opcode 3). The entry point, INIT, tick and events live in
 * lieu34.c; this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 * Triggers 1 and 14..40 are the stub body shared with L01 trig06.
 */

#include <stdlib.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "levels/lieu34_int.h"

static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig10(void);
static void trig11(void); static void trig12(void); static void trig13(void);
static void trig41(void);
static void trig_noop(void);   /* the stub slots (shared L01 trig06 body) */

/* trigger table at 0x1001B940 - filled at INIT with the 5-byte jmp thunks
 * at 0x10001000; static here.
 *   0 100073F0   1 10008FBF stub   2 10008FCA   3 1000929E   4 100099CE
 *   5 10009AB6   6 10009F8F   7 1000A08C   8 1000A1A1   9 1000A438
 *  10 1000A6D5  11 1000A86C  12 1000AB09  13 1000AC57
 *  14..40 stubs (1000ADB5..1000AED3)  41 10007A1D
 */
static void (*const g_trigger[42])(void) = {
    l34_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig09, trig10, trig11, trig12, trig13, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x1001B940]). */
void l34_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 0 (level setup) - trig00_100073F0 */
void l34_trig00(void) {
    /* 0x7402 */
    if (ST(ST_START_POINT(34)) == 1) {
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xAF8) = 1;
        ST(ST_HERO) = P_ARKHAN3;
        ST(0xBB0) = 0;
    }
    /* 0x7444 */
    if (ST(ST_START_POINT(34)) == 2) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xBB0) = 2;
    }
    /* 0x7474 */
    if (ST(ST_START_POINT(34)) == 3) {
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xEF8) = 10;
        ST(0xE20) = 2;
        ST(0xEB8) = 1;
    }
    /* 0x74C2 */
    if (ST(ST_START_POINT(34)) == 4) {
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xAF8) = 1;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xBB0) = 0;
        ST(0xE8C) = 1;
    }
    /* 0x751F */
    if (ST(ST_START_POINT(34)) == 5) {
        ST(V_GUARD_FIGHT_DONE) = 0;
        ST(0xAF8) = 1;
        ST(ST_HERO) = P_ARKHAN2;
        ST(0xBB0) = 10;
        ST(0xE8C) = 1;
        ST(0xEF8) = 1;
    }
    CB->divers(DV_MUSIC, 1, 0);
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
    CB->ajoute_zone(4, 2223, 5762, 85, 280, 1);
    CB->ajoute_zone(5, 1618, 5368, 200, 212, 0);
    CB->ajoute_zone(7, 1580, 6200, 190, 240, 0);
    CB->ajoute_zone(8, 1393, 5495, 110, 110, 0);
    CB->ajoute_zone(9, 1342, 5556, 110, 110, 0);
    CB->ajoute_zone(10, 1481, 6048, 100, 140, 0);
    CB->ajoute_zone(11, 1359, 5990, 170, 130, 0);
    CB->ajoute_zone(12, 1918, 5518, 235, 230, 0);
    CB->ajoute_zone(13, 1950, 5949, 235, 230, 0);
    CB->etat_objet("IM23", 1630, -2645, 5360, 0, 0x400, 0, 0, 2);
    CB->pos_perso(ST(ST_HERO), 2083, 5773, 0x4048, 1, "ar", 1, 0);
    /* 0x77AB */
    if (ST(0xEF8) == 0) {
        /* 0x77BD */
        if (ST(0xBB0) != 2 && ST(ST_VIE(P_THANANDAR24)) < 2) {
            ST(0xC64) = 1;
            CB->pos_perso(P_THANANDAR24, 1504, 5714, 0xFCA5, 1, "P24ARR01", 2, 1);
            CB->pos_perso(P_SORDOS, 1493, 5883, 0x8404, 1, "P23ARR03", 3, 1);
            CB->divers_empile(DV_PERSONA_ITEMS, P_SORDOS, 50);
        } else {
            /* 0x7851 */
            ST(0xC64) = 2;
            CB->vire_col(6, 0);
            CB->pos_perso(P_SORDOS, 1347, 5774, 0xC730, 1, "P23ATT01", 3, 1);
            CB->link_objet(23, "OB50", 28, 1);
        }
    }
    /* 0x78A6 */
    if (ST(0xEF8) == 1 || ST(0xEF8) == 11) {
        /* 0x78CA */
        if (ST(ST_VIE(P_SORDOS)) != VIE_DEAD) {
            CB->vire_col(6, 0);
            CB->pos_perso(P_SORDOS, 1347, 5774, 0xC730, 1, "P23ATT01", 3, 1);
            CB->link_objet(23, "OB50", 28, 1);
        } else {
            /* 0x7927 */
            CB->pos_perso(P_SORDOS, 0, 0, 0, 2, "P23CAD", 3, 0);
        }
    }
    /* 0x7943 */
    if (ST(0xEF8) == 2) {
        CB->pos_perso(P_THANANDAR24, 0, 0, 0, 2, "P24CAD", 2, 0);
        CB->pos_perso(P_SORDOS, 0, 0, 0, 2, "P23CAD", 3, 0);
    }
    /* 0x798D */
    if (ST(0xEF8) == 3) {
        CB->pos_perso(P_SORDOS, 0, 0, 0, 2, "P23CAD", 3, 0);
    }
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0x6C) = 0;
    /* 0x79F7 */
    if (ST(0xE98) == 60) {
        ST(0xE98) = 65;
    }
}

/* trigger 41 (zone-less script entry) - trig41_10007A1D */
static void trig41(void) {
    CB->divers(DV_MUSIC, 1, 1);
    /* 0x7A36 */
    if (ST(0xEF8) == 0) {
        /* 0x7A47 */
        if (ST(0xBB0) != 2 && ST(0xE8C) == 0) {
            CB->select_pile(2, 1);
            CB->change_camera(0, CAM_LOCKED);
            CB->joindre(-P_SORDOS, 0, 0, 0, ST(ST_HERO));
            CB->select_pile(1, 1);
            CB->bloque(BLOQUE_ALL);
            CB->joindre(-P_THANANDAR24, 0, 0, 0, ST(ST_HERO));
            lieu_action_register("L34D100A", P_THANANDAR24, "SP24DIA57", "SP24ARR01_10");
            lieu_action_register("L34D100B", P_THANANDAR24, "SP24DIA57", "SP24ARR01_10");
            CB->animation("SP23ATT03");
            lieu_action_register("L34D101", P_SORDOS, "P23GAR03", "SP23GAR03");
            lieu_action_register("L34D102A", P_THANANDAR24, "SP24DIA57", "SP24ARR01_10");
            lieu_action_register("L34D102B", P_THANANDAR24, "SP24DIA57", "SP24ARR01_10");
            CB->combat(P_SORDOS, 4);
            CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
            ST(V_ENGAGED_PERSONA) = P_SORDOS;
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
        }
        /* 0x7BAC */
        if (ST(0xBB0) != 2 && ST(0xE8C) == 1) {
            CB->select_pile(2, 1);
            CB->change_camera(0, CAM_LOCKED);
            CB->joindre(-P_SORDOS, 0, 0, 0, ST(ST_HERO));
            CB->select_pile(1, 1);
            CB->bloque(BLOQUE_ALL);
            CB->joindre(-P_THANANDAR24, 0, 0, 0, ST(ST_HERO));
            lieu_action_register("L34D102B", P_THANANDAR24, "SP24DIA57", "SP24ARR01_10");
            CB->combat(P_SORDOS, 4);
            CB->joindre_perm(P_THANANDAR24, ST(ST_HERO));
            ST(V_ENGAGED_PERSONA) = P_SORDOS;
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
        }
    }
    /* 0x7CA2 */
    if (ST(0xEF8) == 1 || ST(0xEF8) == 11) {
        CB->select_pile(2, 1);
        ST(0x68) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->link_objet(23, "OB50", 28, 0);
        CB->etat_objet("OB50", 1340, -2519, 5770, 0, 0xB600, 0xDE00, 0, 6);
        lieu_action_register("L34D40A", P_SORDOS, "SP23ATT03", "SP23GAR03");
        CB->pos_perso(P_SORDOS, 1525, 5810, 0xC000, 1, "P23GAR03", 3, 0);
        CB->vire_col(6, 1);
        CB->dialogue("L34D40B", P_SORDOS);
        ST(V_ENGAGED_PERSONA) = P_SORDOS;
        CB->combat(P_SORDOS, 4);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        return;
    }
}

/* trigger 2 (Thanandar / Sordos talk and fight) - trig02_10008FCA */
static void trig02(void) {
    /* 0x8FCF */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_THANANDAR24) && ST(ST_VIE(P_THANANDAR24)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L34D201", P_THANANDAR24);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x907E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_VIE(P_THANANDAR24)) != VIE_DEAD && ST(ST_COMBAT_STANCE) == 0 && CB->direction_perso(P_THANANDAR24)) {
        CB->bloque(BLOQUE_ALL);
        /* 0x90D8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x90EA */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D05", P_ARKHAN2);
            }
            /* 0x910F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->dialogue("L34D06", P_ARKHAN2);
            }
        }
        /* 0x9131 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L34D07", P_ARKHAN3);
        }
        /* 0x9153 */
        if (ST(V_ENGAGED_PERSONA) != P_THANANDAR24 && ST(V_ENGAGED_PERSONA) != 0x914) {
            lieu_action_register("L34D200A", P_THANANDAR24, "SP24DIA59", "SP24ARR01_10");
            lieu_action_register("L34D200B", P_THANANDAR24, "SP24DIA60", "SP24ARR01_10");
        } else {
            CB->dialogue("L34D201", P_THANANDAR24);
        }
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x91D3 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_THANANDAR24)) == VIE_DEAD) {
        CB->fouille(P_THANANDAR24);
        return;
    }
    /* 0x9222 */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == P_SORDOS && ST(ST_VIE(P_THANANDAR24)) != VIE_DEAD) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->stop_joindre(-P_THANANDAR24);
        ST(V_ENGAGED_PERSONA) = 0x914;
        CB->combat(P_THANANDAR24, 4);
        return;
    }
}

/* trigger 3 (Sordos talk and fight) - trig03_1000929E */
static void trig03(void) {
    /* 0x92A4 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000) && CB->divers(DV_IN_ZONE, ST(ST_HERO), 6) && !CB->direction_perso(P_SORDOS)) {
        trig06();   /* call 0x100010e1 -> trig06_10009F8F */
        return;
    }
    /* 0x9330 */
    if (ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(ST_COMBAT_STANCE) == 1 && CB->direction_perso(P_SORDOS) && ST(ST_VIE(P_SORDOS)) == 1) {
        ST(V_CRI_USED) = 1;
        CB->bloque(BLOQUE_ALL);
        CB->dialogue("L34D202", P_SORDOS);
        CB->animation("Sar_10");
        CB->modify_var(&ST(V_CRI_USED), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x93DE */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && (ST(V_ENGAGED_PERSONA) == P_SORDOS || ST(V_ENGAGED_PERSONA) == 0x914) && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SORDOS)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        /* 0x944E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD62", P_ARKHAN2);
        }
        /* 0x946B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD63", P_ARKHAN3);
        }
        CB->dialogue("L34D202", P_SORDOS);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0x94AA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) < 23 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SORDOS)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x68) = 10;
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 23);
        /* 0x9538 */
        if (ST(0xB1C) == 0) {
            /* 0x954F */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x9561 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L34D13", P_ARKHAN2, "SG02DIA05", NULL);
                }
                /* 0x9584 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L34D14", P_ARKHAN2, "SG02DIA05", NULL);
                }
            }
            /* 0x95AC */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x95BE */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    lieu_action_register("L34D15", P_ARKHAN3, "SG03DIA05", NULL);
                }
                /* 0x95E1 */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L34D16", P_ARKHAN3, "SG03DIA05", NULL);
                }
            }
            CB->stop_joindre(P_SORDOS);
            lieu_action_register("L34D17A", P_SORDOS, "SP23DIA59", "SP23GAR03");
            lieu_action_register("L34D17B", P_SORDOS, "SP23DIA59", "SP23GAR03");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            ST(0xE20) = 1;
            ST(V_ENGAGED_PERSONA) = P_SORDOS;
            CB->combat(P_SORDOS, 4);
            return;
        }
        /* 0x9694 */
        if (ST(0xB1C) == 1) {
            /* 0x96A6 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x96B8 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L34D22", P_ARKHAN2);
                    CB->dialogue("L34D23", P_SORDOS);
                    CB->dialogue("L34D24", P_ARKHAN2);
                    CB->dialogue("L34D25", P_SORDOS);
                }
                /* 0x970A */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    CB->dialogue("L34D27", P_ARKHAN2);
                    CB->dialogue("L34D23", P_SORDOS);
                    CB->dialogue("L34D28A", P_ARKHAN2);
                    CB->dialogue("L34D28B", P_ARKHAN2);
                    CB->dialogue("L34D29", P_SORDOS);
                }
                CB->dialogue("L34D26A", P_SORDOS);
                CB->dialogue("L34D26B", P_SORDOS);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(V_ENGAGED_PERSONA) = P_SORDOS;
                ST(0xE20) = 1;
                CB->combat(P_SORDOS, 4);
                return;
            }
            /* 0x97D5 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x97E7 */
                if (ST(ST_SPECIAL_MODE) == 1) {
                    CB->dialogue("L34D30", P_ARKHAN3);
                    CB->dialogue("L34D23", P_SORDOS);
                    CB->dialogue("L34D31", P_ARKHAN2);
                    CB->dialogue("L34D29", P_SORDOS);
                }
                /* 0x983E */
                if (ST(ST_SPECIAL_MODE) == 0) {
                    lieu_action_register("L34D16", P_ARKHAN3, "SG03DIA05", NULL);
                }
                CB->dialogue("L34D26A", P_SORDOS);
                CB->dialogue("L34D26B", P_SORDOS);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                ST(V_ENGAGED_PERSONA) = P_SORDOS;
                CB->combat(P_SORDOS, 4);
                ST(0xE20) = 1;
                return;
            }
        }
    }
    /* 0x98CA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && ST(ST_VIE(P_SORDOS)) == VIE_DEAD && CB->direction_perso(P_SORDOS)) {
        CB->fouille(P_SORDOS);
        return;
    }
    /* 0x993B */
    if (ST(V_PLAYER_STRIKE) >= 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(V_PLAYER_STRIKE) = -1;
        CB->select_pile(2, 1);
        CB->stop_joindre(P_SORDOS);
        CB->select_pile(0, 1);
        ST(0x68) = 10;
        ST(V_ENGAGED_PERSONA) = P_SORDOS;
        CB->combat(P_SORDOS, 4);
        return;
    }
}

/* trigger 4 (exit to room 31) - trig04_100099CE */
static void trig04(void) {
    CB->divers(DV_MUSIC, 1, 4);
    /* 0x99E5 */
    if (ST(ST_HERO) != 4) {
        /* 0x99F7 */
        if (ST(V_ENGAGED_PERSONA) != 0 && ST(0xEF8) == 0) {
            /* 0x9A1E */
            if (ST(0xE20) < 2) {
                /* 0x9A2B */
                if (CB->distance(1, 3) >= 175) {
                    ST(0xEF8) = 1;
                } else {
                    ST(0xEF8) = 11;
                }
            } else {
                ST(0xEF8) = 1;
            }
            ST(V_ENGAGED_PERSONA) = 0;
            CB->divers(DV_COMBAT_STANCE, 0, 0);
        }
        ST(ST_LEVEL) = 34;
        CB->change_piece(31, 0);
    }
}

/* trigger 5 (the IM23 gate) - trig05_10009AB6 */
static void trig05(void) {
    /* 0x9ABB */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(5)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9B1B */
        if (ST(0xBB0) == 2 && ST(ST_VIE(P_SORDOS)) != VIE_DEAD) {
            ST(0x68) = 10;
            CB->stop_joindre(-P_SORDOS);
            CB->dialogue("L34D41", P_SORDOS);
            CB->combat(P_SORDOS, 4);
            ST(ST_SCRIPT_BUSY) = 0;
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x9B96 */
        if (ST(0xEB8) == 0 && ST(0xE20) == 2) {
            /* 0x9BB9 */
            if (ST(ST_PLAYER_WEAPON) == 0xF) {
                ST(0xEB8) = 1;
                CB->joindre_obj("IM23", 75);
                CB->animation("SG01OUV10");
                CB->animation("G01OUV11");
                CB->anim_objet("IM23", 1, -54, 0x3E8, 0);
                CB->animation("ar_50");
                CB->animation("G01OUV05");
                CB->anim_objet("IM23", 1, 0x36, 0x226, 0x200);
                CB->animation("ar_50");
                CB->etat_objet("OB49", -1, -1, -1, -1, -1, -1, -3, 0);
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9C93 */
            if (ST(ST_PLAYER_WEAPON) == 0xB || ST(ST_PLAYER_WEAPON) == 0xE || ST(ST_PLAYER_WEAPON) == 0x10 || ST(ST_PLAYER_WEAPON) == 0x4B || ST(ST_PLAYER_WEAPON) == 0x59 || ST(ST_PLAYER_WEAPON) == 0x60) {
                CB->animation("SG02OUV10");
                CB->animation("ar");
                /* 0x9D0F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L04D44", P_ARKHAN2);
                }
                /* 0x9D31 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L04D45", P_ARKHAN3);
                }
                CB->animation("Sar_10");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x9D82 */
            if (ST(ST_PLAYER_WEAPON) != 0xF && ST(ST_PLAYER_WEAPON) != 0xB && ST(ST_PLAYER_WEAPON) != 0xE && ST(ST_PLAYER_WEAPON) != 0x10 && ST(ST_PLAYER_WEAPON) != 0x4B && ST(ST_PLAYER_WEAPON) != 0x59 && ST(ST_PLAYER_WEAPON) != 0x60) {
                CB->joindre_obj("IM23", 75);
                CB->animation("SG01OUV04");
                /* 0x9E15 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("LCOD13", P_ARKHAN3, "G00OUV09", NULL);
                }
                /* 0x9E3D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("LCOD14", P_ARKHAN3, "G00OUV09", NULL);
                }
                CB->animation("Sar_100");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        /* 0x9E94 */
        if (ST(0xEB8) != 0 && ST(0xE20) == 2) {
            CB->joindre_obj("IM23", 75);
            CB->animation("SG01OUV04");
            CB->animation("G01OUV06");
            CB->anim_objet("IM23", 1, -50, 0x3E8, 0);
            /* 0x9EF0 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD07", P_ARKHAN2);
            }
            /* 0x9F13 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD11", P_ARKHAN3);
            }
            CB->animation("G01OUV07");
            CB->evenement(2, 800);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            CB->animation("Sar_50");
            return;
        }
    }
}

/* trigger 6 (take the OB50 item) - trig06_10009F8F */
static void trig06(void) {
    /* 0x9F94 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x4000)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0x9FF4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02PRE06");
        }
        /* 0xA011 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03PRE06");
        }
        CB->etat_objet("OB50", -1, -1, -1, -1, -1, -1, -3, -1);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->retire_zone(6);
        CB->animation("ar");
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 7 (look at the shelf) - trig07_1000A08C */
static void trig07(void) {
    /* 0xA091 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(7)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA0E2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA0F5 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L34D34", P_ARKHAN2, "SG02PRE03", NULL);
            }
            /* 0xA11D */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L34D35", P_ARKHAN2, "SG02PRE03", NULL);
            }
        }
        /* 0xA14B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L34D36", P_ARKHAN3, "SG03PRE03", NULL);
        }
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 8 (shelves) - trig08_1000A1A1 */
static void trig08(void) {
    /* 0xA1A7 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 9) && CB->direction_spec(0x5208)) {
        trig09();   /* call 0x10001078 -> trig09_1000A438 */
    }
    /* 0xA1DF */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x6F22)) {
        /* 0xA21C */
        if (ST(0x784) == 0) {
            ST(0x784) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xA256 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02PRE06");
            }
            /* 0xA274 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03PRE06");
            }
            CB->etat_objet("OB44A", 0, 0, 0, 0, 0, 0, -3, 0);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA2D5 */
        if (ST(0x54C) == 0) {
            ST(0x54C) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xA30C */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02PRE06");
            }
            /* 0xA32A */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03PRE06");
            }
            CB->etat_objet("OB64A", 0, 0, 0, 0, 0, 0, -3, 0);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA38B */
        if (ST(0x784) == 1 && ST(0x54C) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xA3C5 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD10", P_ARKHAN2);
            }
            /* 0xA3E8 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD11", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 9 (shelf details) - trig09_1000A438 */
static void trig09(void) {
    /* 0xA43D */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x59D8)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA494 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA4A6 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D500", P_ARKHAN2);
                CB->animation("Sar_50");
            }
            /* 0xA4D3 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT93");
                CB->animation("ar_50");
            }
        }
        /* 0xA4FC */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xA50E */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D501", P_ARKHAN3);
                CB->animation("Sar_50");
            }
            /* 0xA53B */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT97");
                CB->animation("ar_50");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA581 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x1F40)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA5D7 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA5E9 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L34D506", P_ARKHAN2, "SG02OUV13", NULL);
                CB->animation("Sar_50");
            }
            /* 0xA61C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02OUV13");
                CB->animation("ar_50");
            }
        }
        /* 0xA645 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xA657 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L34D507", P_ARKHAN3, "SG03OUV13", NULL);
                CB->animation("Sar_50");
            }
            /* 0xA68A */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03OUV13");
                CB->animation("ar_50");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 10 (shelves, west) - trig10_1000A6D5 */
static void trig10(void) {
    /* 0xA6DA */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0xF6E0)) {
        /* 0xA71B */
        if (ST(0x824) == 0) {
            ST(0x824) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xA752 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG02PRE06");
            }
            /* 0xA770 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG03PRE06");
            }
            CB->etat_objet("OB43A", 0, 0, 0, 0, 0, 0, -3, 0);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0xA7D1 */
        if (ST(0x824) == 1) {
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0xA7F3 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD10", P_ARKHAN2);
            }
            /* 0xA81C */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD11", P_ARKHAN3);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* trigger 11 (look around the room) - trig11_1000A86C */
static void trig11(void) {
    /* 0xA871 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x2EE0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xA8C2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xA8D9 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D500", P_ARKHAN2);
                CB->animation("Sar_50");
            }
            /* 0xA908 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT93");
                CB->animation("ar_50");
            }
        }
        /* 0xA930 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xA942 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D501", P_ARKHAN3);
                CB->animation("Sar_50");
            }
            /* 0xA96F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT97");
                CB->animation("ar_50");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA9B5 */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_spec(0x5DC0)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAA0B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xAA1D */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L34D506", P_ARKHAN2, "SG02OUV13", NULL);
                CB->animation("Sar_50");
            }
            /* 0xAA50 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02OUV13");
                CB->animation("ar_50");
            }
        }
        /* 0xAA79 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xAA8B */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L34D507", P_ARKHAN3, "SG03OUV13", NULL);
                CB->animation("Sar_50");
            }
            /* 0xAABE */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03OUV13");
                CB->animation("ar_50");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 12 (zone 12 inspection) - trig12_1000AB09 */
static void trig12(void) {
    /* 0xAB0E */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(12)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xAB65 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xAB77 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D502", P_ARKHAN2);
                CB->animation("Sar_50");
            }
            /* 0xABA4 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT02");
                CB->animation("ar_50");
            }
        }
        /* 0xABCD */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xABDF */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D503", P_ARKHAN3);
                CB->animation("Sar_50");
            }
            /* 0xAC0C */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT02");
                CB->animation("ar_50");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}

/* trigger 13 (zone 13 inspection) - trig13_1000AC57 */
static void trig13(void) {
    /* 0xAC5C */
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 && CB->direction_zone(13)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        /* 0xACB3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0xACC5 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D504A", P_ARKHAN2);
                CB->dialogue("L34D504B", P_ARKHAN2);
                CB->animation("Sar_50");
            }
            /* 0xAD02 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP02ACT02");
                CB->animation("ar_50");
            }
        }
        /* 0xAD2B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0xAD3C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L34D505", P_ARKHAN3);
                CB->animation("Sar_50");
            }
            /* 0xAD69 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SP03ACT02");
                CB->animation("ar_50");
            }
        }
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
        return;
    }
}
