/*
 * lieu53.c - Native hand-port of LIEU53.DLL (level-53 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick, the tick body and the
 * level's own event4 / event5 / event6. The 42 zone trigger handlers live in
 * lieu53_trig.c (reached through l53_trigger, see lieu53_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. LIEU53 is the L13 room script, built from
 * the same older script version as LIEU33 / LIEU47: the entry is at
 * 0x1000A070, the state pointer global is dword_1001990C and the trigger
 * table is dword_10019820[42].
 *
 * Its event4 (sub_10001200), event5 (sub_10001F9F) and event6
 * (sub_10005273) are not the lieu_common.c ones. They started from the
 * LIEU47 copies. lieu_action_register is the shared one. sub_1000A6C6 (the
 * LIEU33 pre-scene variant, 706 bytes) is never called in this DLL and is
 * not ported. The DLL .data the level code touches: the four event4 float
 * accumulators, the two event5 sprintf buffers and the tick print_msg
 * buffer; all are reset at INIT.
 */
#include <stdio.h>
#include <stdlib.h>

#include "lieu53_int.h"

/* event4 float accumulators (initially 1.0f). */
static float g_float_accum[4] = {1.0f, 1.0f, 1.0f, 1.0f};

/* event5 sprintf buffers: "OB24%c" and "OB77%c". */
static c8 g_ob24[32];
static c8 g_ob77[32];

/* tick body print_msg buffer (dword_10019620). */
static c8 g_txt[32];

/* The DLL is loaded afresh for every room: its globals restart. Called by
 * INIT, and by tools/lieu_difftest_harness.c before every case. */
void l53_reset_dll_globals(void) {
    for (i32 i = 0; i < 4; i++)
        g_float_accum[i] = 1.0f;
    g_ob24[0] = '\0';
    g_ob77[0] = '\0';
    g_txt[0] = '\0';
}

/* -------------------------------------------------------------------------
 * event4 - sub_10001200 (thunk 0x100010F0, op 4). switch(arg) over the
 * script-event ids (byte index table -> jump table @ 0x100029D3); mostly
 * simple state-machine setters. `break` == original jmp default.
 * ---------------------------------------------------------------------- */
static void event4(i32 arg) {        /* sub_10001200 */
    switch (arg) {
    case 0:  ST(V_EVENT_0) = 1; break;
    case 1:  ST(V_EVENT_1) = 1; break;
    case 2:  ST(V_EVENT_2) = 1; break;

    case 7:  /* 0x33C increment chain 1->2->3->4->5 */
        if      (ST(V_EVENT_7) == 1) ST(V_EVENT_7) = 2;
        else if (ST(V_EVENT_7) == 2) ST(V_EVENT_7) = 3;
        else if (ST(V_EVENT_7) == 3) ST(V_EVENT_7) = 4;
        else if (ST(V_EVENT_7) == 4) ST(V_EVENT_7) = 5;
        break;

    case 8:  ST(V_EVENT_8) = 2; break;

    case 9:
        if (ST(ST_VIE(P_BOGDARAN)) != VIE_DEAD && ST(V_EVENT_9) == 1) {
            ST(0xB80) = 0;
            ST(V_EVENT_9) = 0;
        }
        break;

    case 11: if (ST(V_EVENT_11) == 1) ST(V_EVENT_11) = 2; break;
    case 12: if (ST(V_EVENT_12) == 1) ST(V_EVENT_12) = 2; break;

    case 15: /* 0x8C increment chain (odd->even+1, 0->1..12->13) */
        if      (ST(V_EVENT15_STEP) == 0)   ST(V_EVENT15_STEP) = 1;
        else if (ST(V_EVENT15_STEP) == 2)   ST(V_EVENT15_STEP) = 3;
        else if (ST(V_EVENT15_STEP) == 4)   ST(V_EVENT15_STEP) = 5;
        else if (ST(V_EVENT15_STEP) == 6)   ST(V_EVENT15_STEP) = 7;
        else if (ST(V_EVENT15_STEP) == 8)   ST(V_EVENT15_STEP) = 9;
        else if (ST(V_EVENT15_STEP) == 10) ST(V_EVENT15_STEP) = 11;
        else if (ST(V_EVENT15_STEP) == 12) ST(V_EVENT15_STEP) = 13;
        break;

    case 16:
        if (ST(V_EVENT_16) == 0) {
            ST(V_EVENT_16) = 1;
            CB->free_anim(1);
            CB->play_anim(2, 0);
            if (ST(0xB40) == 1) {
                ST(0xB40) = 2;
                CB->animation("Sar_10");
                CB->modify_var(&ST(0x68), 1);
            }
            if (ST(0xE30) == 1) {
                ST(0xE30) = 2;
                CB->animation("Sar_10");
                CB->modify_var(&ST(0xE0), 1);
            }
        }
        break;

    case 17: if (ST(V_EVENT_17) == 1) ST(V_EVENT_17) = 2; break;

    case EV_HUD_TIMER:
        if ((i32)ST(V_HUD_TIMER_MS) > 0) {
            ST(V_HUD_TIMER_MS) -= 5000;
            CB->divers_empile(DV_HUD_REFRESH, 0, 0);
            CB->evenement(EV_HUD_TIMER, 5000);
        }
        break;

    case 19:
        if (ST(0xEBC) == 1 && ST(ST_LEVEL) != 28)
            ST(0xEBC) = 2;
        break;

    case 20:
        if (ST(ST_LEVEL) == 18)
            CB->bruitage("L08B06", 2740, 1983);
        if (ST(ST_LEVEL) == 8) {
            CB->select_pile(2, 1);
            CB->bruitage("L08B06", 44, -960);
            CB->select_pile(0, 1);
        }
        break;

    case 21: if (ST(V_EVENT_21) == 0) ST(V_EVENT_21) = 1; break;
    case 22: if (ST(V_EVENT_22) == 0) ST(V_EVENT_22) = 1; break;
    case 25: if (ST(V_EVENT_25) == 0) ST(V_EVENT_25) = 2; break;
    case 27: if (ST(V_EVENT_27) == 0) ST(V_EVENT_27) = 1; break;

    case EV_TO_ROOM_25:
        if (ST(V_EVENT_28) == 0) {
            CB->bloque(BLOQUE_NONE);
            ST(ST_LEVEL) = 23;
            CB->change_piece(25, -1);
        }
        break;

    case 29:
        if      (ST(V_EVENT_29) == 2) ST(V_EVENT_29) = 3;
        else if (ST(V_EVENT_29) == 0) ST(V_EVENT_29) = 1;
        break;

    case 30:
        if (ST(V_EVENT_30) == 1) {
            ST(V_EVENT_30) = 0;
            ST(0xBF4) = 1;
            break;
        }
        /* the jne lands on case 31's code (no jmp to the exit) */
        /* fall through */

    case 31: if (ST(V_EVENT_31) == 0) ST(V_EVENT_31) = 1; break;
    case 33: ST(V_EVENT_33) = 1; break;
    case 34: ST(V_EVENT_34) = 1; break;
    case 35: ST(V_EVENT_35) = 1; break;
    case 36: ST(V_EVENT_36) = 1; break;
    case 37: ST(V_EVENT_37) = 1; break;
    case 38: ST(V_EVENT_38) = 1; break;
    case 39: ST(V_EVENT_39) = 1; break;
    case 40: ST(V_EVENT_40) = 1; break;
    case 41: ST(V_EVENT_41) = 1; break;

    case EV_MUSIC_LOOP_A:
        if (ST(V_MUSIC_TRACK) != -1) {
            CB->select_pile(2, 1);
            CB->divers_empile(DV_MUSIC, ST(V_MUSIC_TRACK), 4);
            CB->evenement(EV_MUSIC_LOOP_B, 5000);
            CB->select_pile(0, 1);
        }
        break;

    case EV_MUSIC_LOOP_B:
        CB->select_pile(2, 1);
        CB->divers_empile(DV_MUSIC, ST(V_MUSIC_TRACK), 1);
        CB->evenement(EV_MUSIC_LOOP_A, 45000);
        CB->select_pile(0, 1);
        break;

    case EV_END_GAME:      /* 44, not in LIEU33 (0x24CD) */
        ST(V_END_GAME_REQ) = 1;
        break;

    case 51: /* 0x3EC chain 0->1,2->3,4->5 */
        if      (ST(V_EVENT_51) == 0) ST(V_EVENT_51) = 1;
        else if (ST(V_EVENT_51) == 2) ST(V_EVENT_51) = 3;
        else if (ST(V_EVENT_51) == 4) ST(V_EVENT_51) = 5;
        break;
    case 52: /* 0x3F0 chain 0->1,2->3 */
        if      (ST(V_EVENT_52) == 0) ST(V_EVENT_52) = 1;
        else if (ST(V_EVENT_52) == 2) ST(V_EVENT_52) = 3;
        break;
    case 53: /* 0x3F4 chain 0->1,2->3,4->5 */
        if      (ST(V_EVENT_53) == 0) ST(V_EVENT_53) = 1;
        else if (ST(V_EVENT_53) == 2) ST(V_EVENT_53) = 3;
        else if (ST(V_EVENT_53) == 4) ST(V_EVENT_53) = 5;
        break;

    case EV_FLOAT_GROW_0:  /* float accum grow (index 0) */
        CB->select_pile(2, 2);
        if (g_float_accum[0] < 3.0f) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x10011);
            CB->evenement(EV_FLOAT_GROW_0, 100);
            g_float_accum[0] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case EV_FLOAT_GROW_1:  /* float accum grow (index 1, dual layer_op) */
        CB->select_pile(2, 2);
        if (g_float_accum[1] < 3.0f) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x10005);
            CB->divers(DV_SCALE_BONE, 0x66, 0x10007);
            CB->evenement(EV_FLOAT_GROW_1, 100);
            g_float_accum[1] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case EV_FLOAT_GROW_2:  /* float accum grow (index 2, dual layer_op) */
        CB->select_pile(2, 2);
        if (g_float_accum[2] < 3.0f) {
            CB->divers(DV_SCALE_BONE, 0x66, 0x1000E);
            CB->divers(DV_SCALE_BONE, 0x66, 0x10010);
            CB->evenement(EV_FLOAT_GROW_2, 100);
            g_float_accum[2] *= 1.02f;
        }
        CB->select_pile(0, 1);
        break;
    case EV_FLOAT_DECAY:  /* float accum decay (index 3) */
        CB->select_pile(2, 2);
        if (g_float_accum[3] > 0.5f) {
            CB->divers(DV_SCALE_BODY, 0x62, 0);
            CB->evenement(EV_FLOAT_DECAY, 100);
            g_float_accum[3] *= 0.98f;
        }
        CB->select_pile(0, 1);
        break;
    case EV_PIPE_SCENE:  /* pipe puzzle SP02PIPI1/2 by layer query */
        CB->select_pile(2, 2);
        CB->bloque(BLOQUE_ALL);
        if (CB->divers(DV_CAN_FIGHT, 9, 2))
            CB->animation("SP02PIPI2");
        else
            CB->animation("SP02PIPI1");
        if (ST(ST_HERO) == P_ARKHAN0)
            CB->dialogue("L01D23A", P_ARKHAN0);
        if (ST(ST_HERO) == P_ARKHAN1)
            CB->dialogue("L01D23A", P_ARKHAN1);
        if (ST(ST_HERO) == P_ARKHAN2)
            CB->dialogue("L01D23B", P_ARKHAN2);
        if (ST(ST_HERO) == P_ARKHAN3)
            CB->dialogue("L01D23C", P_ARKHAN3);
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
        break;

    case EV_GOURD_TO_FLASK: {  /* SOB24%c/SOB77%c via named_ex_988 */
        CB->divers(DV_TRANSITION, 2000, 2000);
        CB->animation("SP01ACT96");
        CB->animation("ar");
        c8 buf1[48], buf2[48];
        snprintf(buf1, sizeof(buf1), "SOB24%c", (char)(ST(ST_USED_VARIANT) + 'A'));
        CB->etat_objet(buf1, -1, -1, -1, -1, -1, -1, -2, 0);
        snprintf(buf2, sizeof(buf2), "SOB77%c", (char)(ST(ST_USED_VARIANT) + 'A'));
        CB->etat_objet(buf2, -1, -1, -1, -1, -1, -1, -3, 0);
        CB->bloque(BLOQUE_NONE);
        break;
    }
    default:
        break;
    }
}

/* -------------------------------------------------------------------------
 * event5 - sub_10001F9F (thunk 0x10001005, op 5): ecx = arg (item id),
 * edx = phase. arg-1 indexes a byte table -> jump table @ 0x10005C9B. The
 * missing ids are genuine no-ops (default).
 * ---------------------------------------------------------------------- */
static void event5(i32 arg, i32 phase) {
    ST(ST_SCRIPT_BUSY) = 0;
    switch (arg) {

    /* 0x1FAD */
    case OB_DISH:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x1FD1 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD41", ST(ST_HERO));
        }
        /* 0x1FF9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD42", ST(ST_HERO));
        }
        /* 0x2021 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD43", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x2060 */
    case OB_FLAGON22:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x2084 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD41", ST(ST_HERO));
        }
        /* 0x20AC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD42", ST(ST_HERO));
        }
        /* 0x20D4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD43", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x2113 */
    case OB_F_THROWER:
        /* 0x211C */
        if (ST(ST_LEVEL) == 35) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            /* 0x2146 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D05", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0x216E */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D06", P_ARKHAN3, "SG00DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        break;

    /* 0x2197 */
    case OB_FLAGON42:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x21BB */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD41", ST(ST_HERO));
        }
        /* 0x21E3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD42", ST(ST_HERO));
        }
        /* 0x220B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD43", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x224A */
    case OB_SORDOS_DOC:
        /* 0x2256 */
        if (ST(V_LCOD02_SEEN) == 0 || ST(V_LCOD02_SEEN) == 1) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            /* 0x2292 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD02A", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02B", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02C", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02D", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02E", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02F", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                /* 0x233A */
                if (ST(V_LCOD02_SEEN) == 0) {
                    lieu_action_register("LCOD02G", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                    lieu_action_register("LCOD02H", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                }
                ST(V_LCOD02_SEEN) = 1;
            }
            /* 0x238D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD03A", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03B", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03C", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03D", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03E", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03F", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                /* 0x2435 */
                if (ST(V_LCOD02_SEEN) == 0) {
                    lieu_action_register("LCOD03G", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                    lieu_action_register("LCOD03H", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                }
                ST(V_LCOD02_SEEN) = 1;
            }
        }
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x2494 */
    case OB_DIAG_WEAPON:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x24B8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD88", P_ARKHAN2, "SG00DIA02", NULL);
        }
        /* 0x24E0 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD89", P_ARKHAN3, "SG00DIA02", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x2509 */
    case OB_RYLS_NOTES:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x252D */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD100A", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD100B", P_ARKHAN2);
            CB->dialogue("LCOD100C", P_ARKHAN2);
            CB->dialogue("LCOD100D", P_ARKHAN2);
        }
        /* 0x2588 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD101A", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD101B", P_ARKHAN3);
            CB->dialogue("LCOD101C", P_ARKHAN3);
            CB->dialogue("LCOD101D", P_ARKHAN3);
        }
        CB->dialogue("LCOD102A", P_ARKHAN2);
        CB->dialogue("LCOD102B", P_ARKHAN2);
        CB->dialogue("LCOD102C", P_ARKHAN2);
        CB->dialogue("LCOD102D", P_ARKHAN2);
        CB->dialogue("LCOD102E", P_ARKHAN2);
        CB->dialogue("LCOD102F", P_ARKHAN2);
        /* 0x2643 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD103", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x266B */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD104", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x269F */
    case OB_SPITROD:
        /* 0x26A8 */
        if (ST(ST_LEVEL) == 35) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            /* 0x26D2 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D05", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0x26FA */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D06", P_ARKHAN3, "SG00DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        break;

    /* 0x2723 */
    case OB_BOWL:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x2747 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD41", ST(ST_HERO));
        }
        /* 0x276F */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD42", ST(ST_HERO));
        }
        /* 0x2797 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD43", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x27D6 */
    case OB_THUNDERBOLT:
        /* 0x27DF */
        if (ST(ST_LEVEL) == 35) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            /* 0x2809 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D05", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0x2831 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D06", P_ARKHAN3, "SG00DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        break;

    /* 0x285A */
    case OB_ZED_THBOLT68:
        /* 0x2863 */
        if (ST(ST_LEVEL) == 35) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            /* 0x288D */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D05", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0x28B5 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D06", P_ARKHAN3, "SG00DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        break;

    /* 0x28DE */
    case OB_BANDOR_DIAG:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x2902 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD90", P_ARKHAN2, "SG00DIA02", NULL);
        }
        /* 0x292A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD91", P_ARKHAN3, "SG00DIA02", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x2953 */
    case OB_METAL_SUIT:
        CB->select_pile(0, 2);
        /* 0x296C */
        if (ST(V_SOB86_ACTIVE) == 0) {
            /* 0x297B */
            if (ST(ST_LEVEL) == 30) {
                /* 0x298D */
                if (ST(ST_LOCKED_CAMERA) == 5 && ST(ST_VIE(P_KONKALITE2)) == VIE_DEAD) {
                    ST(V_SOB86_ACTIVE) = 1;
                    CB->bloque(BLOQUE_ALL);
                    CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                    CB->divers_empile(DV_SCREEN, 5, 0);
                    /* 0x29FB */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->pos_perso(98, CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
                    }
                    /* 0x2A6F */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->pos_perso(99, CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
                    }
                    CB->divers_empile(DV_SCREEN, 3, 0);
                    /* 0x2AF5 */
                    if (ST(V_HUD_TIMER_MS) == 0) {
                        ST(V_SOB86_ACTIVE) = 0;
                        /* 0x2B16 */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->dialogue("L31D83A", P_ARKHAN2);
                            CB->dialogue("L31D83B", P_ARKHAN2);
                            /* 0x2B48 */
                            if (ST(0xC50) == 0) {
                                CB->dialogue("L31D83C", P_ARKHAN2);
                            }
                        }
                        /* 0x2B6A */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->dialogue("L31D84A", P_ARKHAN3);
                            CB->dialogue("L31D84B", P_ARKHAN3);
                            /* 0x2B9C */
                            if (ST(0xC50) == 0) {
                                CB->dialogue("L31D84C", P_ARKHAN3);
                            }
                        }
                        CB->animation("Sar_50");
                        CB->divers_empile(DV_SCREEN, 5, 0);
                        /* 0x2BDB */
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->pos_perso(P_ARKHAN2, CB->divers(DV_DESC_POS, P_ARKHAN2, 0), CB->divers(DV_DESC_POS, P_ARKHAN2, 1), CB->divers(DV_DESC_POS, P_ARKHAN2, 2), 0, "ar", 1, 0);
                        }
                        /* 0x2C3D */
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->pos_perso(P_ARKHAN3, CB->divers(DV_DESC_POS, P_ARKHAN3, 0), CB->divers(DV_DESC_POS, P_ARKHAN3, 1), CB->divers(DV_DESC_POS, P_ARKHAN3, 2), 0, "ar", 1, 0);
                        }
                        CB->divers_empile(DV_SCREEN, 3, 0);
                    }
                    CB->animation("Sar_50");
                    CB->bloque(BLOQUE_NONE);
                    return;
                } else {
                    CB->bloque(BLOQUE_ALL);
                    /* 0x2CD9 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD666", P_ARKHAN2, "SG02DIA02", NULL);
                    }
                    /* 0x2D01 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD667", P_ARKHAN3, "SG03DIA02", NULL);
                    }
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x2D33 */
            if (ST(ST_LEVEL) == 37 && ST(ST_LOCKED_CAMERA) == 0 && CB->distance(1, 6) < 350) {
                ST(V_SOB86_ACTIVE) = 1;
                CB->bloque(BLOQUE_ALL);
                CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                CB->divers_empile(DV_SCREEN, 5, 0);
                /* 0x2DBC */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(98, CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
                }
                /* 0x2E30 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(99, CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
                }
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("Sar_50");
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                CB->bloque(BLOQUE_ALL);
                /* 0x2EDE */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("LCOD666", P_ARKHAN2, "SG02DIA02", NULL);
                }
                /* 0x2F06 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("LCOD667", P_ARKHAN3, "SG03DIA02", NULL);
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        /* 0x2F3D */
        if (ST(ST_LEVEL) == 35) {
            CB->bloque(BLOQUE_ALL);
            /* 0x2F5A */
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD670", P_ARKHAN2, "SG02DIA02", NULL);
            }
            /* 0x2F82 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD671", P_ARKHAN3, "SG03DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
            return;
            return;
        }
        ST(V_SOB86_ACTIVE) = 0;
        CB->bloque(BLOQUE_ALL);
        CB->divers_empile(DV_SCREEN, 5, 0);
        /* 0x2FE8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(P_ARKHAN2, CB->divers(DV_DESC_POS, P_ARKHAN2, 0), CB->divers(DV_DESC_POS, P_ARKHAN2, 1), CB->divers(DV_DESC_POS, P_ARKHAN2, 2), 0, "ar", 1, 0);
        }
        /* 0x304A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(P_ARKHAN3, CB->divers(DV_DESC_POS, P_ARKHAN3, 0), CB->divers(DV_DESC_POS, P_ARKHAN3, 1), CB->divers(DV_DESC_POS, P_ARKHAN3, 2), 0, "ar", 1, 0);
        }
        CB->divers_empile(DV_SCREEN, 3, 0);
        /* 0x30BE */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD668", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x30E6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD669", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x3114 */
    case OB_FLASK77:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x3138 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->animation("SP01ACT17");
        }
        /* 0x3155 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ACT17");
        }
        /* 0x3172 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ACT17");
        }
        CB->animation("ar_50");
        /* 0x319A */
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("LCOD41", ST(ST_HERO), "SG01ATT01", NULL);
        }
        /* 0x31C8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD42", ST(ST_HERO), "SG01ATT01", NULL);
        }
        /* 0x31F6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD43", ST(ST_HERO), "SG01ATT01", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x3225 */
    case OB_PARCHMENT:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x3249 */
        if (ST(V_LCOD23_SEEN) == 0) {
            ST(V_LCOD23_SEEN) = 1;
        }
        /* 0x3DD6 - not in LIEU33 */
        if (ST(ST_LEVEL) == 28) {
            ST(0x84) = 1;
        }
        /* 0x326A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD23A", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD23B", P_ARKHAN2);
            CB->dialogue("LCOD23C", P_ARKHAN2);
            CB->dialogue("LCOD23D", P_ARKHAN2);
            CB->dialogue("LCOD23E", P_ARKHAN2);
            CB->dialogue("LCOD23F", P_ARKHAN2);
            lieu_action_register("LCOD23G", P_ARKHAN2, "SG02DIA01", NULL);
        }
        /* 0x32FB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD24A", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
            CB->dialogue("LCOD24B", P_ARKHAN3);
            CB->dialogue("LCOD24C", P_ARKHAN3);
            CB->dialogue("LCOD24D", P_ARKHAN3);
            CB->dialogue("LCOD24E", P_ARKHAN3);
            CB->dialogue("LCOD24F", P_ARKHAN3);
            lieu_action_register("LCOD24G", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x3398 */
    case OB_HELMET:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x33BC */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD672", ST(ST_HERO));
        }
        /* 0x33E4 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD673", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x3423 */
    case OB_BREASTPLATE:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x3447 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD674", ST(ST_HERO));
        }
        /* 0x346F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD675", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x34AE */
    case OB_VEGETABLE:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x34D2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x34E4 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN01");
            }
            /* 0x3501 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN02");
            }
        }
        /* 0x351E */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x3530 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN01");
            }
            /* 0x354D */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN02");
            }
        }
        CB->animation("ar_50");
        /* 0x3589 */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 100) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 100));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x35EE */
    case OB_VIAL:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x3612 */
        if (ST(0xE54) == 0) {
            ST(0xE54) = 1;
            /* 0x3633 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x3645 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT19");
                }
                /* 0x3662 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT19");
                }
            }
            /* 0x367F */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x3691 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT20");
                }
                /* 0x36AE */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT20");
                }
            }
            CB->animation("ar_50");
            /* 0x36EC */
            if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 200) < 1024) {
                CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 200));
            } else {
                CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
            }
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x375F */
        if (ST(0xE54) == 1) {
            CB->bloque(BLOQUE_ALL);
            /* 0x377C */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("LCOD41", ST(ST_HERO));
            }
            /* 0x37A4 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD42", ST(ST_HERO));
            }
            /* 0x37CC */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD43", ST(ST_HERO));
            }
            CB->animation("SG01ATT01");
            CB->animation("ar_50");
            CB->bloque(BLOQUE_NONE);
            break;
        }
        /* the jne lands on the next case's code (no jmp to the exit) */
        /* fall through */

    /* 0x380B */
    case OB_FULL_GOURD:
        /* 0x3814 */
        if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            /* 0x384D */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x3864 */
                if (ST(0x140 + 4 * phase) > 0) {
                    ST(0x140 + 4 * phase)--;
                }
                /* 0x388B */
                if (ST(0x140 + 4 * phase) > 0) {
                    /* 0x389D */
                    if (ST(ST_HERO) == P_ARKHAN1) {
                        CB->animation("SP01ACT98");
                    }
                    /* 0x38BA */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT19");
                    }
                    /* 0x38D7 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT19");
                    }
                    /* 0x3908 */
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
                    CB->animation("ar");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x3989 */
                if (ST(0x140 + 4 * phase) == 0) {
                    /* 0x399B */
                    if (ST(ST_HERO) == P_ARKHAN1) {
                        CB->animation("SP01ACT98");
                    }
                    /* 0x39B8 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT19");
                    }
                    /* 0x39D5 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT19");
                    }
                    snprintf(g_ob24, sizeof(g_ob24), "OB24%c", 'A' + phase);
                    CB->etat_objet(g_ob24, -1, -1, -1, -1, -1, -1, -2, 0);
                    snprintf(g_ob77, sizeof(g_ob77), "OB77%c", 'A' + phase);
                    CB->etat_objet(g_ob77, -1, -1, -1, -1, -1, -1, -3, 0);
                    /* 0x3A74 */
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
                    /* 0x3AD8 */
                    if (ST(ST_HERO) == P_ARKHAN1) {
                        CB->dialogue("LCOD41", ST(ST_HERO));
                    }
                    /* 0x3B00 */
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD42", ST(ST_HERO));
                    }
                    /* 0x3B28 */
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD43", ST(ST_HERO));
                    }
                    CB->animation("SG01ATT01");
                    CB->animation("ar_50");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            /* 0x3B73 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x3B85 */
                if (ST(ST_HERO) == P_ARKHAN1) {
                    ST(0x140 + 4 * phase) = 0;
                    CB->animation("SP01ACT99");
                    /* 0x3BCA */
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
                    CB->animation("P01ACT97");
                    CB->evenement(50, 800);
                    return;
                }
                /* 0x3C4E */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    ST(0x140 + 4 * phase)--;
                    CB->animation("SP02ATT20");
                    /* 0x3C8F */
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
                    /* 0x3CF8 */
                    if (ST(0x140 + 4 * phase) == 0) {
                        snprintf(g_ob24, sizeof(g_ob24), "OB24%c", 'A' + phase);
                        CB->etat_objet(g_ob24, -1, -1, -1, -1, -1, -1, -2, 0);
                        snprintf(g_ob77, sizeof(g_ob77), "OB77%c", 'A' + phase);
                        CB->etat_objet(g_ob77, -1, -1, -1, -1, -1, -1, -3, 0);
                        CB->dialogue("LCOD42", P_ARKHAN2);
                        CB->animation("SG01ATT01");
                    }
                    CB->animation("ar_50");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                /* 0x3DAB */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    ST(0x140 + 4 * phase)--;
                    CB->animation("SP03ATT20");
                    /* 0x3DEC */
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
                    /* 0x3E55 */
                    if (ST(0x140 + 4 * phase) == 0) {
                        snprintf(g_ob24, sizeof(g_ob24), "OB24%c", 'A' + phase);
                        CB->etat_objet(g_ob24, -1, -1, -1, -1, -1, -1, -2, 0);
                        snprintf(g_ob77, sizeof(g_ob77), "OB77%c", 'A' + phase);
                        CB->etat_objet(g_ob77, -1, -1, -1, -1, -1, -1, -3, 0);
                        CB->dialogue("LCOD43", P_ARKHAN3);
                        CB->animation("SG01ATT01");
                    }
                    CB->animation("ar_50");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                return;
            }
        }
        break;

    /* 0x3F06 */
    case OB_POTION:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x3F2A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ATT20");
        }
        /* 0x3F47 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ATT20");
        }
        CB->animation("ar_50");
        /* 0x3F83 */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 50) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 50));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->animation("ar_50");
        {
            i32 lv = CB->divers(DV_DARKNESS, -1, 0);
            CB->divers_empile(DV_DARKNESS, lv - 150, 0);
        }
        CB->etat_objet("OB31", -1, -1, -1, -1, -1, -1, -2, 0);
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4036 */
    case OB_POTATO:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x405A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x406C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN01");
            }
            /* 0x4089 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN02");
            }
        }
        /* 0x40A6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x40B8 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN01");
            }
            /* 0x40D5 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN02");
            }
        }
        CB->animation("ar_50");
        /* 0x4111 */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 75) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 75));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4176 */
    case OB_ROOT:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x419A */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x41AC */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            /* 0x41C9 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        /* 0x41E6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x41F8 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            /* 0x4215 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        /* 0x4251 */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 75) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 75));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x42B6 */
    case OB_FISH:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x42DA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x42EC */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            /* 0x4309 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        /* 0x4326 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x4338 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            /* 0x4355 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        /* 0x4391 */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 100) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 100));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x43F6 */
    case OB_MEAT:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x441A */
        if (ST(ST_HERO) == P_ARKHAN0) {
            /* 0x442C */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG00MAN01");
            }
            /* 0x4449 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG00MAN02");
            }
        }
        /* 0x4466 */
        if (ST(ST_HERO) == P_ARKHAN1) {
            /* 0x4478 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG01MAN01");
            }
            /* 0x4495 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG01MAN02");
            }
        }
        /* 0x44B2 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x44C4 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            /* 0x44E1 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        /* 0x44FE */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x4510 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            /* 0x452D */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        /* 0x456B */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 150) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 150));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x45D2 */
    case OB_RAT:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x45F6 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x4608 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            /* 0x4625 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        /* 0x4642 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x4654 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            /* 0x4671 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        /* 0x46AD */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 75) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 75));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4712 */
    case OB_ROAST_BIRD:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x4736 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x4748 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            /* 0x4765 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        /* 0x4782 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x4794 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            /* 0x47B1 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        /* 0x47ED */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 100) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 100));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4852 */
    case OB_MORTAR:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x4876 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x4888 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            /* 0x48A5 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        /* 0x48C2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x48D4 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            /* 0x48F1 */
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        /* 0x492F */
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 150) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 150));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4996 */
    case OB_UNIFORM:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x49BA */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD26", ST(ST_HERO));
        }
        /* 0x49E2 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD27", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4A21 */
    case OB_BOW_TO_KEY:
    case OB_CRYSTAL_2:
    case OB_CRYSTAL_1:
    case OB_CRYSTAL_3:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x4A45 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L25D79", ST(ST_HERO));
        }
        /* 0x4A6D */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L25D82", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4AAC */
    case OB_METAL_PLATE:
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        /* 0x4AD3 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", ST(ST_HERO));
        }
        /* 0x4AFB */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4B3A */
    case OB_METAL_POLE:
    case OB_F_THROW_AMMO:
    case OB_BULLETS:
    case OB_BOILER:
    case OB_HANDLE:
    case OB_PLATE_BEFORE:
    case OB_RED_KEY:
    case OB_KONK_KEY:
    case OB_SORDOS_KEY:
    case OB_BOGD_KEY:
    case OB_MIRROR:
    case OB_SHAFT:
    case OB_LEAD_PLAQUE:
    case OB_SM_TH_AMMO:
    case OB_OBJ_BEFORE40:
    case OB_OBJ_BEFORE41:
    case OB_CRYST_SLIV:
    case OB_PULLEY_BLADE:
    case OB_COIN:
    case OB_THBOLT_AMMO:
    case OB_ZED_AMMO:
    case OB_PICK_AX:
    case OB_CRYSTAL_KEY:
    case OB_FLASK86:
    case OB_YONG_COUNTER:
    case OB_STOCK_KEY:
    case OB_STILETTO:
    case OB_PESTLE:
    case OB_LARGE_KEY:
    case OB_CAN:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x4B5E */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", ST(ST_HERO));
        }
        /* 0x4B86 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4BC5 */
    case OB_BOOK:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x4BE9 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", ST(ST_HERO));
        }
        /* 0x4C11 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4C50 */
    case OB_TUBE:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x4C74 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            /* 0x4C86 */
            if (ST(ST_HERO) == P_ARKHAN1) {
                /* 0x4CA4 */
                if ((CB->divers(DV_HP, P_ARKHAN1, -1) - 50) > 100) {
                    CB->divers(DV_HP, P_ARKHAN1, (CB->divers(DV_HP, P_ARKHAN1, -1) - 50));
                }
                CB->animation("SP01ATT02");
                CB->animation("P01ATT03");
                CB->animation("ar_50");
            }
            /* 0x4CFD */
            if (ST(ST_HERO) == P_ARKHAN2) {
                /* 0x4D1B */
                if ((CB->divers(DV_HP, P_ARKHAN2, -1) - 30) > 50) {
                    CB->divers(DV_HP, P_ARKHAN2, (CB->divers(DV_HP, P_ARKHAN2, -1) - 30));
                }
                CB->animation("SP02ATT02");
                CB->animation("P02ATT03");
                CB->animation("ar_50");
            }
            /* 0x4D74 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                /* 0x4D92 */
                if ((CB->divers(DV_HP, P_ARKHAN3, -1) - 20) > 50) {
                    CB->divers(DV_HP, P_ARKHAN3, (CB->divers(DV_HP, P_ARKHAN3, -1) - 20));
                }
                CB->animation("SP02ATT02");
                CB->animation("P02ATT03");
                CB->animation("ar_50");
            }
        }
        /* 0x4DEB */
        if (ST(ST_SPECIAL_MODE) == 1) {
            /* 0x4DFD */
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD25", P_ARKHAN1);
                CB->animation("ar_50");
            }
            /* 0x4E35 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD26", P_ARKHAN2);
                CB->animation("ar_50");
            }
            /* 0x4E6D */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD27", P_ARKHAN3);
                CB->animation("ar_50");
            }
        }
        CB->bloque(BLOQUE_NONE);
        break;

    /* 0x4EA6 */
    case OB_JERRYCAN:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x4ECA */
        if (ST(0x6FC) != 2) {
            /* 0x4EDC */
            if (ST(ST_SPECIAL_MODE) == 0) {
                /* 0x4EEE */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    /* 0x4F0C */
                    if ((CB->divers(DV_HP, P_ARKHAN2, -1) - 30) > 50) {
                        CB->divers(DV_HP, P_ARKHAN2, (CB->divers(DV_HP, P_ARKHAN2, -1) - 30));
                    }
                    CB->animation("SP02ATT04");
                    CB->animation("P02ATT08");
                    CB->animation("ar_50");
                }
                /* 0x4F65 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    /* 0x4F83 */
                    if ((CB->divers(DV_HP, P_ARKHAN3, -1) - 20) > 50) {
                        CB->divers(DV_HP, P_ARKHAN3, (CB->divers(DV_HP, P_ARKHAN3, -1) - 20));
                    }
                    CB->animation("SP03ATT04");
                    CB->animation("P03ATT08");
                    CB->animation("ar_50");
                }
            }
            /* 0x4FDC */
            if (ST(ST_SPECIAL_MODE) == 1) {
                /* 0x4FEE */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD26", P_ARKHAN2);
                    CB->animation("ar_50");
                }
                /* 0x5026 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD27", P_ARKHAN3);
                    CB->animation("ar_50");
                }
            }
        }
        /* 0x505E */
        if (ST(0x6FC) == 2) {
            /* 0x5070 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD26", P_ARKHAN2);
                CB->animation("ar_50");
            }
            /* 0x50A8 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD27", P_ARKHAN3);
                CB->animation("ar_50");
            }
        }
        CB->bloque(BLOQUE_NONE);
        break;

    default:
        break;
    }
}

/* -------------------------------------------------------------------------
 * event6 - sub_10005273 (thunk 0x10001041, op 6). Switch on arg (0..4):
 * LCOD hint triggers and the SC21 ending; jump table @ 0x1000603B.
 * ---------------------------------------------------------------------- */
static void event6(i32 arg) {
    i32 stance_was_on = 0;   /* [ebp-4]: set in the SC21 ending, never read */

    switch (arg) {
    case EV6_LCOD34:  /* 0x525B */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD34", P_ARKHAN2, "SG00CON01", NULL);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD35:  /* 0x52A8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD35", P_ARKHAN2, "SG00CON01", NULL);
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->animation("SG00CON01");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD36:  /* 0x530F */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD36", P_ARKHAN3, "SG00CON01", "SG00CON01");
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD37:  /* 0x536A */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD37", P_ARKHAN3, "SG00CON01", NULL);
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_SC21_ENDING:  /* 0x53C6 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(0, 2);
            /* 0x53EE */
            if (ST(ST_COMBAT_STANCE) == 1) {
                stance_was_on = 1;
                CB->divers_empile(DV_COMBAT_STANCE, 0, 0);
            }
            CB->bloque(BLOQUE_ALL);
            CB->divers_empile(DV_SCREEN, 5, 0);
            CB->divers_empile(DV_HP, P_ARKHAN3, 1);
            CB->animation("Sar_50");
            CB->modify_var(&ST(ST_HERO), 4);
            CB->cinematique("SC21");
            CB->divers_empile(DV_SCREEN, 1, 0);
            CB->divers_empile(DV_END_GAME, 0, 0);
        }
        break;
    default:
        break;
    }
    (void)stance_was_on;
}
/* -------------------------------------------------------------------------
 * Per-frame body - sub_10006A59 (thunk 0x100010B4), level-53 specific.
 * `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    i32 t1 = CB->divers(DV_CAN_FIGHT, 9, 2);
    snprintf(g_txt, sizeof(g_txt), "gun:%d", t1);
    CB->print_msg(g_txt, 0, 240);
    /* 0x6A9F */
    if (ST(0x70) == 0 && ST(ST_LOCKED_CAMERA) == 0) {
        ST(0x70) = 1;
        CB->play_anim(5, 1);
    }
    /* 0x6ADC */
    if (ST(0x70) == 1 && ST(ST_LOCKED_CAMERA) != 0) {
        ST(0x70) = 0;
    }
    /* 0x6B0C */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->select_pile(1, 1);
        /* 0x6B3D */
        if (ST(0xC90) == 0) {
            CB->bruitage("L13B01", 2147, 5160);
            /* 0x6B64 */
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD39", P_ARKHAN2);
            }
            /* 0x6B86 */
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->dialogue("LCOD40", P_ARKHAN3);
            }
            CB->animation("Sar_200");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        } else {
            /* 0x6BD6 */
            if (ST(0xC94) == 1) {
                CB->bruitage("L13B02", 2147, 5160);
                /* 0x6BFD */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->dialogue("L13D07", P_ARKHAN2);
                }
                /* 0x6C1F */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->dialogue("L13D08", P_ARKHAN3);
                }
                CB->animation("Sar_50");
                CB->divers_empile(DV_SCREEN, 5, 0);
                /* 0x6C5E */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->cinematique("SC13_500");
                }
                /* 0x6C7B */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->cinematique("SC14_500");
                }
                CB->animation("ar_50");
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("ar_50");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
            /* 0x6CDE */
            if (ST(0xC94) == 2) {
                CB->bruitage("L13B03", 2147, 5160);
                /* 0x6D05 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L13D09", P_ARKHAN2, "SG00DIA01", NULL);
                }
                /* 0x6D2D */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L13D10", P_ARKHAN3, "SG00DIA01", NULL);
                }
                CB->animation("ar_200");
                CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
                CB->bloque(BLOQUE_NONE);
                return;
            }
        }
        CB->select_pile(0, 1);
    }
    /* 0x6D8B */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->select_pile(1, 1);
        /* 0x6DB9 */
        if (ST(0x64) == 1 && ST(ST_LOCKED_CAMERA) == 3) {
            CB->play_anim(1, 0);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x6DF9 */
        if (ST(0x64) == 2 && ST(ST_LOCKED_CAMERA) == 3) {
            CB->play_anim(2, 1);
            CB->select_pile(2, 1);
            CB->play_anim(1, 0);
            CB->select_pile(0, 1);
            ST(0xC90) = 1;
            return;
        }
    }
    /* 0x6E68 */
    if (ST(0x68) == 0 && ST(0xC90) == 1 && ST(ST_LOCKED_CAMERA) == 3) {
        ST(0x68) = 1;
        CB->play_anim(2, 1);
    }
    /* 0x6EB7 */
    if (ST(0x68) == 1 && ST(ST_LOCKED_CAMERA) != 3) {
        ST(0x68) = 0;
    }
    /* 0x6EE7 */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        CB->evenement(33, 1500);
        CB->free_anim(5);
        CB->play_anim(6, 1);
        CB->play_anim(3, 0);
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x6F53 */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        ST(ST_LEVEL) = 13;
        CB->change_piece(49, 0);
    }
    /* 0x6F8A */
    if (ST(ST_SCRIPT_BUSY) == 42) {
        ST(ST_SCRIPT_BUSY) = 0;
        /* 0x6FA8 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            /* 0x6FBA */
            if (ST(ST_PLAYER_WEAPON) == 23) {
                CB->animation("SP02VIC03");
                CB->animation("ar");
                lieu_action_register("LCOD57", P_ARKHAN2, "Sar", NULL);
            } else {
                CB->animation("Sar_50");
            }
            CB->bloque(BLOQUE_NONE);
            return;
        }
        /* 0x7015 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            /* 0x7027 */
            if (ST(ST_PLAYER_WEAPON) == 23) {
                CB->animation("SP03VIC03");
                CB->animation("ar");
                lieu_action_register("LCOD58", P_ARKHAN3, "Sar", NULL);
            } else {
                CB->animation("Sar_50");
            }
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x1000A3FA).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1001998C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu53_trig.c. There is no death
 * branch in this version.
 *   arg = reload flag (non-zero => skip arming).
 * ---------------------------------------------------------------------- */
static void init(i32 reload_flag) {
    if (reload_flag == 0)
        l53_trig00();   /* 0x1000A413: call sub_1000106E -> sub_100065C0 */
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1, 0x1000A466): drain pending sound, run body.
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x1000A070).
 * Jump table @ 0x1000A4E3, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu53_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu53: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        /* DLL .data restarts on every room load */
        l53_reset_dll_globals();
        init(arg);
        break;
    case LIEU_OP_EVENT:  l53_trigger(arg);             break;  /* dword_10019820[arg]() */
    case LIEU_OP_CALL4:  event4(arg);                  break;
    case LIEU_OP_CALL5:  event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
