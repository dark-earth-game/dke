/*
 * lieu23.c — Native hand-port of LIEU23.DLL (level-23 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick and the tick body. The
 * 42 zone trigger handlers live in lieu23_trig.c (reached through
 * l23_trigger, see lieu23_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. The entry is at 0x1000C810, the state
 * pointer global is dword_1001CCBC and the trigger table is
 * dword_1001CBD0[42].
 *
 * event4 (sub_1000123A), event5 (sub_100022B4), event6 (sub_10005BEC),
 * lieu_action_register, lieu_pre_scene and sub_10001200 (float reset) are
 * the shared lieu_common.c. tick_body (sub_100085D5) is level-23 specific.
 * The DLL has no .data globals of its own to reset on INIT.
 */

#include <stdlib.h>

#include "lieu23_int.h"

/* -------------------------------------------------------------------------
 * Per-frame body — sub_100085D5 (thunk 0x100010B9), level-23 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue (0x1000A55A).
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 34279 */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        ST(V_DEATH_EVENT_SENT) = 1;
        CB->evenement(44, 1500);
    }
    /* 34346 */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        CB->bloque(BLOQUE_ALL);
        ST(V_DEATH_EVENT_SENT) = 1;
        /* 34410 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 34451 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 34495 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 34539 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 34575 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 34604 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 34656 */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN13, 0) && ST(ST_VIE(P_GUARDIAN13)) != VIE_DEAD && ST(0x4F0) != 0) {
        CB->select_pile(1, 2);
        CB->divers_empile(DV_WAS_HIT, P_GUARDIAN13, 1);
        CB->combat(P_GUARDIAN13, -1);
        /* 34780 */
        if ((CB->divers(DV_HP, P_GUARDIAN13, -1) - ST(V_HIT_DAMAGE)) > 0) {
            CB->divers(DV_HP, P_GUARDIAN13, (CB->divers(DV_HP, P_GUARDIAN13, -1) - ST(V_HIT_DAMAGE)));
        } else {
            CB->divers(DV_HP, P_GUARDIAN13, 1);
        }
        /* 34877 */
        if (CB->divers(DV_HP, P_GUARDIAN13, -1) <= 1) {
            ST(ST_VIE(P_GUARDIAN13)) = VIE_DEAD;
            CB->stop_joindre(-P_GUARDIAN13);
            CB->divers_empile(DV_BONE_SOUND, 53, 0);
            CB->animation("SP53IMP53");
            CB->animation("P53_65535");
            CB->animation("P53CAD");
            CB->pos_perso(P_GUARDIAN13, 0, 0, 0, 2, "P53CAD", 0xA, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
            /* 35018 */
            if (ST(ST_VIE(P_GUARDIAN14)) != VIE_DEAD) {
                ST(V_ENGAGED_PERSONA) = P_GUARDIAN14;
            } else {
                ST(V_ENGAGED_PERSONA) = 0;
                ST(0x4EC) = 3;
                ST(0x4F0) = 0;
            }
        } else {
            CB->stop_joindre(-P_GUARDIAN13);
            CB->divers_empile(DV_BONE_SOUND, 53, 0);
            CB->animation("SP53IMP13");
            CB->animation("P53IMP48");
            CB->animation("P53_65535");
            CB->animation("P53GAR03");
            CB->combat(P_GUARDIAN13, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 35207 */
    if (CB->divers(DV_WAS_HIT, P_GUARDIAN14, 0) && ST(ST_VIE(P_GUARDIAN14)) != VIE_DEAD && ST(0x4F0) != 0) {
        CB->select_pile(2, 2);
        CB->divers_empile(DV_WAS_HIT, P_GUARDIAN14, 1);
        /* 35315 */
        if ((CB->divers(DV_HP, P_GUARDIAN14, -1) - ST(V_HIT_DAMAGE)) > 0) {
            CB->divers(DV_HP, P_GUARDIAN14, (CB->divers(DV_HP, P_GUARDIAN14, -1) - ST(V_HIT_DAMAGE)));
        } else {
            CB->divers(DV_HP, P_GUARDIAN14, 1);
        }
        CB->combat(P_GUARDIAN14, -1);
        /* 35428 */
        if (CB->divers(DV_HP, P_GUARDIAN14, -1) <= 1) {
            ST(ST_VIE(P_GUARDIAN14)) = VIE_DEAD;
            CB->stop_joindre(-P_GUARDIAN14);
            CB->divers_empile(DV_BONE_SOUND, 54, 0);
            CB->animation("SP54IMP52");
            CB->animation("P54_65535");
            CB->animation("P54CAD");
            CB->pos_perso(P_GUARDIAN14, 0, 0, 0, 2, "P54CAD", 0xB, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
            /* 35569 */
            if (ST(ST_VIE(P_GUARDIAN13)) != VIE_DEAD) {
                ST(V_ENGAGED_PERSONA) = P_GUARDIAN13;
            } else {
                ST(V_ENGAGED_PERSONA) = 0;
                ST(0x4EC) = 3;
                ST(0x4F0) = 0;
            }
        } else {
            CB->stop_joindre(-P_GUARDIAN14);
            CB->divers_empile(DV_BONE_SOUND, 54, 0);
            CB->animation("SP54IMP12");
            CB->animation("P54IMP47");
            CB->animation("P54_65535");
            CB->animation("P54GAR02");
            CB->combat(P_GUARDIAN14, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 35750 */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        CB->select_pile(2, 1);
        CB->bruitage("L23B01", 6464, -3652);
        CB->select_pile(1, 1);
        CB->play_anim(0, 0);
        CB->select_pile(0, 1);
    }
    /* 35859 */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->select_pile(2, 1);
        CB->bruitage("L23B02", 6405, -3627);
        CB->select_pile(1, 1);
        CB->play_anim(1, 0);
        CB->select_pile(0, 1);
    }
    /* 35968 */
    if (ST(0x64) == 1 && ST(0xE78) == 2 && ST(ST_HERO) == P_ARKHAN1 && CB->distance(4, 2) <= 150) {
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_RYLSADHAR20);
        CB->pos_perso(P_RYLSADHAR20, 6463, -3671, 0xBE84, 1, "P20ARR01", -1, 0);
        CB->change_camera(2, CAM_LOCKED);
        CB->animation("SP20ARR01_50");
        CB->evenement(33, 475);
        CB->animation("P20OUV01");
        ST(0xE78) = 6;
        ST(0xA0) = ST(ST_GAME_TIME);
        CB->animation("P20MAR02");
        CB->evenement(1, 475);
        CB->animation("P20MAR02");
        CB->pos_perso(P_RYLSADHAR20, -1, -1, -1, 4, "", -1, 0);
        CB->animation("Sar_50");
        CB->modify_var(&ST(0x64), 4);
    }
    /* 36297 */
    if (ST(0x64) == 4 && ST(ST_HERO) == P_ARKHAN1 && ST(0xAF8) == 0) {
        ST(0x64) = 5;
        CB->bruitage("L23BO1", 6464, -3652);
        CB->change_camera(0, CAM_LOCKED);
        CB->change_camera(0, CAM_AUTO);
        CB->free_anim(1);
        CB->bloque(BLOQUE_NONE);
    }
    /* 36426 */
    if (ST(0x68) == 1 && ST(ST_HERO) == P_ARKHAN1) {
        ST(0x68) = 0;
        lieu_action_register("L23D19", P_ZED84, "SP84DIA59", "SP84ARR02_10");
        CB->joindre(-P_ZED84, 6369, -3645, 0x2D62, -1);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, 84);
        CB->modify_var(&ST(0x68), 2);
    }
    /* 36573 */
    if (ST(0x68) == 2 && ST(ST_HERO) == P_ARKHAN1 && !CB->on_joindre(P_ZED84)) {
        /* 36635 */
        if (CB->distance(6, 9) <= 125) {
            ST(0x68) = 0;
            CB->stop_joindre(-P_ZED84);
            CB->bloque(BLOQUE_ALL);
            CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 84);
            CB->animation("SP84ACT03");
            CB->animation("Sar_800");
            /* 36767 */
            if (ST(ST_PLAYER_WEAPON) == 0) {
                CB->animation("P01IMP51");
            }
            /* 36796 */
            if (ST(ST_PLAYER_WEAPON) != 0) {
                CB->animation("P01IMP52");
            }
            CB->divers_empile(DV_BONE_SOUND, ST(ST_HERO), 0);
            CB->animation("P84ACT02");
            CB->dialogue("L10D93", ST(ST_HERO));
            CB->dialogue("L23D20", P_ZED84);
            CB->modify_var(&ST(0x70), 1);
        }
        /* 36925 */
        if (CB->distance(6, 9) > 125) {
            ST(0x68) = 0;
            CB->stop_joindre(-P_ZED84);
            CB->bloque(BLOQUE_ALL);
            CB->joindre(-ST(ST_HERO), 0, 0, 0, 84);
            CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
            CB->animation("SP84ARR02_10");
            /* 37046 */
            if (ST(ST_LOCKED_CAMERA) != 2) {
                CB->change_camera(2, CAM_LOCKED);
            }
            CB->animation("SP84ACT01");
            CB->animation("Sar_800");
            /* 37102 */
            if (ST(ST_PLAYER_WEAPON) == 0) {
                CB->animation("P01IMP51");
            }
            /* 37131 */
            if (ST(ST_PLAYER_WEAPON) != 0) {
                CB->animation("P01IMP52");
            }
            CB->divers_empile(DV_BONE_SOUND, ST(ST_HERO), 0);
            CB->dialogue("L23D20", P_ZED84);
            /* 37217 */
            if (CB->divers(DV_DESC_POS, ST(ST_HERO), 1) > -3081) {
                /* 37252 */
                if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 6000) {
                    CB->change_camera(1, CAM_LOCKED);
                }
                /* 37303 */
                if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) <= 6000) {
                    CB->change_camera(0, CAM_LOCKED);
                }
            }
            CB->dialogue("L23D11", P_ZED84);
            CB->divers_empile(DV_HP, ST(ST_HERO), 0);
            CB->modify_var(&ST(ST_HERO), 4);
        }
    }
    /* 37394 */
    if (ST(0x70) == 1 && ST(ST_HERO) == P_ARKHAN1) {
        ST(0x70) = 2;
        CB->dialogue("L23D11", P_ZED84);
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
    }
    /* 37502 */
    if (ST(0x78) == 1 && !CB->on_joindre(P_PHEDORIA) && ST(ST_HERO) >= 2 && ST(0x84) == 0) {
        ST(0x78) = 0;
        CB->divers(DV_RUN, P_PHEDORIA, 0);
        CB->animation("SP26GAR03_10");
        /* 37616 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 37634 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->dialogue("L23D102A", P_ARKHAN2);
                lieu_action_register("L23D102B", P_ARKHAN2, "SG02ARR17", "SG02ARR17");
            }
            /* 37693 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L23D21X", P_ARKHAN2, "SG02ARR17", "SG02ARR17");
            }
            CB->dialogue("L23D106", P_PHEDORIA);
        }
        /* 37752 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("L23D44B", P_PHEDORIA, "SP26GAR03", "SP26GAR03");
        }
        CB->animation("SG26FRA03");
        CB->evenement(2, 700);
        CB->animation("P26GAR03_10");
    }
    /* 37833 */
    if (ST(V_EVENT_2) == 1) {
        CB->select_pile(1, 1);
        ST(V_EVENT_2) = 2;
        CB->divers_empile(DV_HP, ST(ST_HERO), 1);
        CB->divers_empile(DV_BONE_SOUND, ST(ST_HERO), 0);
        /* 37930 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SG02ATT07");
        }
        /* 37959 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SG03ATT07");
        }
        /* 37988 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("P02_65535");
        }
        /* 38017 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("P03_65535");
        }
        lieu_action_register("L23D13", P_PHEDORIA, "SP26GAR03", "SP26GAR03_500");
        CB->modify_var(&ST(ST_HERO), 4);
        CB->modify_var(&ST(0x6C), 1);
        CB->select_pile(0, 1);
    }
    /* 38124 */
    if (ST(0x78) == 1 && !CB->on_joindre(P_PHEDORIA) && ST(ST_HERO) >= 2 && ST(0x84) == 1) {
        ST(0x78) = 0;
        CB->divers(DV_RUN, P_PHEDORIA, 0);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        /* 38256 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("L23D102A", P_ARKHAN2, "SG02ARR17", NULL);
            CB->dialogue("L23D102B", P_ARKHAN2);
        }
        /* 38330 */
        if (ST(ST_HERO) == P_ARKHAN2 && ST(ST_SPECIAL_MODE) == 0) {
            lieu_action_register("L23D103A", P_ARKHAN2, "SG02ARR17", NULL);
            CB->dialogue("L23D103B", P_ARKHAN2);
        }
        /* 38404 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("L23D104", P_ARKHAN3, "SG03ARR17", NULL);
        }
        /* 38462 */
        if (ST(ST_HERO) == P_ARKHAN3 && ST(ST_SPECIAL_MODE) == 0) {
            lieu_action_register("L23D105", P_ARKHAN3, "SG03ARR17", NULL);
        }
        lieu_action_register("L23D44A", P_PHEDORIA, "SP26DIA60", "SP26ARR03_10");
        lieu_action_register("L23D44B", P_PHEDORIA, "SP26GAR26", "SP26GAR03");
        CB->animation("SG26FRA03");
        CB->evenement(34, 700);
        CB->animation("P26GAR03_10");
    }
    /* 38608 */
    if (ST(V_EVENT_34) == 1) {
        CB->select_pile(1, 1);
        ST(V_EVENT_34) = 2;
        CB->divers_empile(DV_HP, ST(ST_HERO), 1);
        CB->divers_empile(DV_BONE_SOUND, ST(ST_HERO), 0);
        /* 38705 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("G02ATT07");
        }
        /* 38734 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("G03ATT07");
        }
        /* 38763 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("P02_65535");
        }
        /* 38792 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("P03_65535");
        }
        CB->dialogue("L23D13", P_PHEDORIA);
        CB->modify_var(&ST(0x84), 4);
        CB->select_pile(0, 1);
    }
    /* 38873 */
    if (ST(0x84) == 4 && ST(ST_HERO) >= 2) {
        ST(0x84) = 5;
        CB->divers(DV_RUN, P_KALHI13, 1);
        CB->joindre(-P_KALHI13, 6190, -3370, 0xA028, -1);
        CB->modify_var(&ST(0x84), 6);
    }
    /* 38993 */
    if (ST(0x84) == 6 && !CB->on_joindre(P_KALHI13)) {
        ST(0x84) = 7;
        CB->divers(DV_RUN, P_KALHI13, 0);
        CB->stop_joindre(-P_KALHI13);
        CB->joindre(-P_KALHI13, 0, 0, 0, 26);
        lieu_action_register("L23D45", P_KALHI13, "SP13DIA60", "SP13GAR02_10");
        CB->joindre(-P_PHEDORIA, 0, 0, 0, 13);
        CB->animation("SP26gar27");
        CB->animation("P26_65535");
        CB->link_objet(0x1A, "OB61", 0x1B, 0);
        lieu_action_register("L23D46A", P_PHEDORIA, "SP26DIA60", "SP26ARR01_10");
        lieu_action_register("L23D46B", P_PHEDORIA, "SP26DIA54", "SP26ARR01_10");
        CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP13ARR01_50");
        CB->modify_var(&ST(ST_HERO), 4);
    }
    /* 39321 */
    if (ST(0x4EC) != 0 && ST(0x4EC) != 3) {
        /* 39357 */
        if (ST(ST_LOCKED_CAMERA) == 0) {
            /* 39375 */
            if (ST(ST_HERO) != 4 && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) > -1300 && ST(V_ENGAGED_PERSONA) != 0 && ST(0xB8) == 0) {
                ST(0xB8) = 1;
                CB->select_pile(1, 1);
                /* 39492 */
                if (ST(ST_VIE(P_GUARDIAN13)) != VIE_DEAD) {
                    CB->combat(P_GUARDIAN13, -1);
                    CB->animation("SP53GAR03");
                    CB->joindre_perm(P_GUARDIAN13, ST(ST_HERO));
                }
                CB->select_pile(2, 1);
                /* 39575 */
                if (ST(ST_VIE(P_GUARDIAN14)) != VIE_DEAD) {
                    CB->combat(P_GUARDIAN14, -1);
                    CB->animation("SP54GAR02");
                    CB->joindre_perm(P_GUARDIAN14, ST(ST_HERO));
                }
                CB->select_pile(0, 1);
            }
            /* 39655 */
            if (ST(ST_HERO) != 4 && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) < -1400 && ST(V_ENGAGED_PERSONA) != 0 && ST(0xB8) == 1) {
                ST(0xB8) = 0;
                CB->select_pile(1, 1);
                /* 39772 */
                if (ST(ST_VIE(P_GUARDIAN13)) != VIE_DEAD) {
                    CB->stop_joindre(P_GUARDIAN13);
                    CB->combat(P_GUARDIAN13, 0);
                }
                CB->select_pile(2, 1);
                /* 39830 */
                if (ST(ST_VIE(P_GUARDIAN14)) != VIE_DEAD) {
                    CB->stop_joindre(P_GUARDIAN14);
                    CB->combat(P_GUARDIAN14, 0);
                }
                CB->select_pile(0, 1);
            }
        }
        /* 39885 */
        if (ST(0x4EC) == 1) {
            /* 39903 */
            if (ST(ST_LOCKED_CAMERA) == 0 && ST(0x4F0) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
                /* 39951 */
                if (ST(ST_COMBAT_STANCE) == 1 && ST(V_PLAYER_STRIKE) != 0 && ST(0xBC) == 0) {
                    ST(0xBC) = 1;
                    /* 40017 */
                    if (ST(ST_PLAYER_WEAPON) != OB_THBOLT_BEF) {
                        /* 40035 */
                        if (ST(ST_PLAYER_WEAPON) != OB_F_THROWER) {
                            CB->evenement(38, 750);
                        } else {
                            CB->evenement(38, 2500);
                        }
                    } else {
                        CB->evenement(38, 2000);
                    }
                }
                /* 40111 */
                if (ST(V_EVENT_38) == 1) {
                    ST(V_EVENT_38) = 0;
                    ST(V_PLAYER_STRIKE) = -1;
                    CB->bloque(BLOQUE_ALL);
                    ST(0x4F0) = 1;
                    ST(0xB8) = 99;
                    CB->divers(DV_WAS_HIT, P_GUARDIAN13, 1);
                    CB->divers(DV_WAS_HIT, P_GUARDIAN14, 1);
                    CB->change_camera(2, CAM_LOCKED);
                    CB->stop_joindre(-P_GUARDIAN13);
                    CB->stop_joindre(-P_GUARDIAN14);
                    CB->joindre(-P_GUARDIAN13, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("L23D33", P_GUARDIAN13, "SP53GAR26", "SP53GAR03_10");
                    CB->joindre(-P_GUARDIAN14, 0, 0, 0, ST(ST_HERO));
                    lieu_action_register("L23D34", P_GUARDIAN14, "SP54GAR26", "SP54GAR02_10");
                    ST(V_ENGAGED_PERSONA) = 5354;
                    CB->combat(P_GUARDIAN13, 0);
                    CB->combat(P_GUARDIAN14, 0);
                    CB->change_camera(0, CAM_LOCKED);
                    CB->change_camera(99, CAM_AUTO);
                    CB->animation("Sar_50");
                    CB->modify_var(&ST(0xB8), 0);
                    CB->bloque(BLOQUE_NONE);
                }
            }
            /* 40485 */
            if (ST(ST_LOCKED_CAMERA) == 1 && ST(0x4F0) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
                CB->bloque(BLOQUE_ALL);
                ST(0x4F0) = 1;
                ST(0xB8) = 99;
                CB->change_camera(2, CAM_LOCKED);
                CB->joindre(-P_GUARDIAN13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L23D33", P_GUARDIAN13, "SP53GAR26", "SP53GAR03_10");
                CB->joindre(-P_GUARDIAN14, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L23D34", P_GUARDIAN14, "SP54GAR26", "SP54GAR02_10");
                ST(V_ENGAGED_PERSONA) = 5354;
                CB->combat(P_GUARDIAN13, 0);
                CB->combat(P_GUARDIAN14, 0);
                CB->change_camera(0, CAM_LOCKED);
                CB->change_camera(99, CAM_AUTO);
                CB->animation("Sar_50");
                CB->modify_var(&ST(0xB8), 0);
                CB->bloque(BLOQUE_NONE);
            }
        }
        /* 40807 */
        if (ST(0x4EC) == 2 && ST(ST_LOCKED_CAMERA) == 0 && ST(0x4F0) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
            /* 40876 */
            if (ST(ST_VIE(P_GUARDIAN13)) == 1 && ST(ST_VIE(P_GUARDIAN14)) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(0x4F0) = 1;
                ST(0xB8) = 99;
                CB->change_camera(2, CAM_LOCKED);
                CB->joindre(-P_GUARDIAN13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L23D35", P_GUARDIAN13, "SP53DIA60", "SP53ARR03_10");
                CB->joindre(-P_GUARDIAN14, 0, 0, 0, ST(ST_HERO));
                ST(V_ENGAGED_PERSONA) = 5354;
                CB->combat(P_GUARDIAN13, 0);
                CB->combat(P_GUARDIAN14, 0);
                CB->change_camera(0, CAM_LOCKED);
                CB->change_camera(99, CAM_AUTO);
                CB->animation("Sar_50");
                CB->modify_var(&ST(0xB8), 0);
                CB->bloque(BLOQUE_NONE);
            }
            /* 41158 */
            if (ST(ST_VIE(P_GUARDIAN13)) == 1 && ST(ST_VIE(P_GUARDIAN14)) == VIE_DEAD) {
                CB->bloque(BLOQUE_ALL);
                ST(0x4F0) = 1;
                ST(0xB8) = 99;
                CB->change_camera(2, CAM_LOCKED);
                CB->joindre(-P_GUARDIAN13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L23D36", P_GUARDIAN13, "SP53DIA60", "SP53ARR03_10");
                ST(V_ENGAGED_PERSONA) = P_GUARDIAN13;
                CB->combat(P_GUARDIAN13, 0);
                CB->change_camera(0, CAM_LOCKED);
                CB->change_camera(99, CAM_AUTO);
                CB->animation("Sar_50");
                CB->modify_var(&ST(0xB8), 0);
                CB->bloque(BLOQUE_NONE);
            }
            /* 41398 */
            if (ST(ST_VIE(P_GUARDIAN13)) == VIE_DEAD && ST(ST_VIE(P_GUARDIAN14)) == 1) {
                CB->bloque(BLOQUE_ALL);
                ST(0x4F0) = 1;
                ST(0xB8) = 99;
                CB->change_camera(2, CAM_LOCKED);
                CB->joindre(-P_GUARDIAN14, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L23D37", P_GUARDIAN14, "SP54DIA60", "SP54ARR02_10");
                ST(V_ENGAGED_PERSONA) = P_GUARDIAN14;
                CB->combat(P_GUARDIAN14, 0);
                CB->change_camera(0, CAM_LOCKED);
                CB->change_camera(99, CAM_AUTO);
                CB->animation("Sar_50");
                CB->modify_var(&ST(0xB8), 0);
                CB->bloque(BLOQUE_NONE);
            }
        }
        /* 41635 */
        if (ST(V_ENGAGED_PERSONA) != 0 && ST(ST_VIE(P_GUARDIAN13)) == VIE_DEAD && ST(ST_VIE(P_GUARDIAN14)) == VIE_DEAD) {
            ST(V_ENGAGED_PERSONA) = 0;
            ST(0x4EC) = 3;
            ST(0x4F0) = 0;
        }
    }
    /* 41731 */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->select_pile(2, 1);
        /* 41780 */
        if (ST(0x90) == 0) {
            ST(0x90) = 1;
            CB->play_anim(0, 0);
            /* 41823 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->evenement(0, 1450);
            }
            /* 41854 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->evenement(0, 1800);
            }
            CB->select_pile(0, 1);
            return;
        } else {
            ST(ST_LEVEL) = 23;
            CB->change_piece(25, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 41946 */
    if (ST(V_EVENT_27) == 1) {
        ST(V_EVENT_27) = 0;
        /* 41979 */
        if (ST(0x94) == 0) {
            ST(0x94) = 1;
            CB->select_pile(1, 1);
            CB->evenement(27, 1250);
            /* 42044 */
            if (ST(0xB4C) == 2) {
                CB->animation("223OUV07");
            } else {
                /* 42078 */
                if (ST(V_GUARD_FIGHT_DONE) < 2) {
                    CB->animation("223OUV01");
                }
                /* 42107 */
                if (ST(V_GUARD_FIGHT_DONE) >= 2) {
                    CB->animation("223OUV04");
                }
            }
            return;
        }
        ST(ST_LEVEL) = 23;
        /* 42158 */
        if (ST(0xBA8) < 2) {
            CB->change_piece(24, 0);
        }
        /* 42189 */
        if (ST(0xBA8) >= 2 && ST(0xBA8) < 5) {
            CB->change_piece(24, 0);
        }
        /* 42238 */
        if (ST(0xBA8) >= 5 && ST(V_GUARD_FIGHT_DONE) < 2) {
            CB->change_piece(24, 1);
        }
        /* 42290 */
        if (ST(0xBA8) >= 5 && ST(V_GUARD_FIGHT_DONE) >= 2) {
            CB->change_piece(24, 2);
        }
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) — arming tail of FonctionEditor case 1 (0x1000CBA9).
 * Callback ingestion (0x1000C82A..0x1000C9F1), the trigger-table fill and
 * the unused dword_1001CD3C = state+0xC8 are skipped: g_lieu_cb is provided
 * by the bridge, the trigger table is static in lieu23_trig.c.
 *   arg = reload flag (non-zero => skip arming).
 * Returns 1 where the original returns 0 directly (the death ending, 0x1000CDD4);
 * every other path returns state[0].
 * ---------------------------------------------------------------------- */
static i32 init(i32 reload_flag) {
    if (reload_flag)
        return 0;

    CB->select_pile(2, 2);
    CB->select_pile(1, 2);
    CB->select_pile(0, 2);
    CB->select_pile(0, 1);
    CB->evenement(0, -1);
    CB->evenement(1, -1);
    CB->evenement(2, -1);
    CB->evenement(33, -1);
    CB->evenement(34, -1);
    CB->evenement(35, -1);
    CB->evenement(36, -1);
    CB->evenement(37, -1);
    CB->evenement(38, -1);
    CB->evenement(39, -1);
    CB->evenement(41, -1);

    if (ST(ST_HERO) != 4 || ST(0xE8C) == 1) {
        l23_trig00();   /* 0x1000CDD9: call sub_10001073 -> sub_100073F0 */
        return 0;
    }
    /* 0x1000CCBE — the hero is dead: play the death ending */
    CB->divers_empile(DV_SCREEN, 1, 0);
    ST(V_DEATH_EVENT_SENT) = 1;
    for (i32 layer = 0; layer < 4; layer++) {
        if (CB->divers(DV_PERSONA_PRESENT, layer, 0))
            CB->divers(DV_HP, layer, 0);
    }
    if (ST(V_GUARD_FIGHT_DONE) == 0)
        CB->cinematique("SC22_500");
    if (ST(V_GUARD_FIGHT_DONE) != 0)
        CB->cinematique("SC23_500");
    CB->divers_empile(DV_END_GAME, 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1, 0x1000CE2C): drain pending sound, run body.
 * ---------------------------------------------------------------------- */
static void tick(void) {
    if (ST(V_PENDING_EVENT) != 0) {
        CB->select_pile(2, 1);
        CB->evenement(ST(V_PENDING_EVENT) + EV_PENDING_BASE, 1000);
        ST(V_PENDING_EVENT) = 0;
        CB->select_pile(0, 1);
    }
    tick_body();
}

/* -------------------------------------------------------------------------
 * Entry point — @FonctionEditor@16 (thunk 0x10001046 -> 0x1000C810).
 * Jump table @ 0x1000CEA9, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu23_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu23: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        if (init(arg))
            return 0;
        break;
    case LIEU_OP_EVENT:  l23_trigger(arg);             break;  /* dword_1001CBD0[arg]() */
    case LIEU_OP_CALL4:  lieu_event4(arg);                  break;
    case LIEU_OP_CALL5:  lieu_event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  lieu_event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
