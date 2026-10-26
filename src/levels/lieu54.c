/*
 * lieu54.c - Native hand-port of LIEU54.DLL (level-54 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick, the tick body and the
 * level's own event4 / event5 / event6. The 42 zone trigger handlers live in
 * lieu54_trig.c (reached through l54_trigger, see lieu54_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. LIEU54 is another build of the room-14
 * script (L14 dialogue codes, the Malrot / Provider fight), linked in a
 * non-standard layout: the shared library code (lieu_action_register,
 * pre90_scene, the pre-scene sub_10001856, event4/5/6) sits before the
 * level code. The entry is at 0x10001200, the state pointer global is
 * dword_10029CAC and the trigger table is dword_10029BC0[42].
 *
 * Its event4 (sub_10011A80), event5 (sub_1001281F) and event6
 * (sub_10015AF3) are instruction for instruction the LIEU47 ones (checked
 * by a normalised listing diff), not the lieu_common.c ones, so they are
 * ported here (see lieu47.c for the differences against LIEU33). The
 * pre-scene sub_10001856 lacks the `n != 6` test of lieu_pre_scene
 * (l54_pre_scene, with its own copy of pre90_scene). lieu_action_register
 * is the shared one. The DLL .data the level code touches: the four event4
 * float accumulators (initially 1.0f), the two event5 sprintf buffers and
 * the tick print_msg buffer (dword_100299C0); all are reset at INIT.
 *
 * The start point read by trigger 0 is dword 0x998 = ST_START_POINT(14),
 * the room-14 slot, not ST_START_POINT(54).
 */

#include <stdio.h>
#include <stdlib.h>

#include "lieu54_int.h"

/* event4 float accumulators (initially 1.0f). */
static float g_float_accum[4] = {1.0f, 1.0f, 1.0f, 1.0f};

/* event5 sprintf buffers: "OB24%c" and "OB77%c". */
static c8 g_ob24[32];
static c8 g_ob77[32];

/* tick body print_msg buffer (dword_100299C0). */
static c8 g_txt[32];

/* The DLL is loaded afresh for every room: its globals restart. Called by
 * INIT, and by tools/lieu_difftest_harness.c before every case. */
void l54_reset_dll_globals(void) {
    for (i32 i = 0; i < 4; i++)
        g_float_accum[i] = 1.0f;
    g_ob24[0] = '\0';
    g_ob77[0] = '\0';
    g_txt[0] = '\0';
}

/* -------------------------------------------------------------------------
 * pre90_scene - sub_10001746 (thunk sub_100010D2), identical to
 * the shared one in lieu_common.c (which keeps it private): __fastcall
 * (ecx = n). Builds "OB%02d" from state[V_PRE90_OBJECT] and plays the
 * SPnnPRE90 / SPnnPRE91 sequences of persona n around a link_objet.
 * ---------------------------------------------------------------------- */
static void pre90_scene(i32 n) {
    c8 ob[256];
    c8 b[256];

    snprintf(ob, sizeof(ob), "OB%02d", ST(V_PRE90_OBJECT));
    snprintf(b, sizeof(b), "SP%02dPRE90", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    CB->link_objet(n, ob, 0x1B, 0);
    snprintf(b, sizeof(b), "P%02dPRE91", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02darr01_10", n);
    CB->animation(b);
}

/* -------------------------------------------------------------------------
 * sub_10001856 (thunk sub_100010AA): the hero greets persona n (search /
 * handshake sequence). __fastcall(ecx=hero, edx=n) + stdcall stack (a1
 * unused, a2, a3); ret 0Ch. a2/a3 go to Distance. Same as lieu_pre_scene
 * except that the Sp%02dMAR30 animation does not test n != 6.
 * ---------------------------------------------------------------------- */
void l54_pre_scene(i32 hero, i32 n, i32 a1, i32 a2, i32 a3) {
    c8 b[256];
    (void)a1;   /* [ebp+8] is never read */

    if (ST(ST_PLAYER_WEAPON) != 0 && ST(V_WEAPON_WARNED) == 1 &&
        (ST(ST_LEVEL) == 5 || ST(ST_LEVEL) == 15 || ST(ST_LEVEL) == 18 ||
         ST(ST_LEVEL) == 36 || ST(ST_LEVEL) == 48)) {
        ST(ST_PLAYER_WEAPON) = 0;
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02PRE73");
            CB->animation("P02_65535");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03PRE73");
            CB->animation("P03_65535");
        }
    }
    if (ST(ST_PLAYER_WEAPON) != 0 && ST(V_PRE90_OBJECT) != 0)
        pre90_scene(n);

    if (CB->distance(a2, a3) < 80) {
        snprintf(b, sizeof(b), "Sp%02dMAR30", hero);
        CB->animation(b);
        snprintf(b, sizeof(b), "P%02d_65535", hero);
        CB->animation(b);
    }
    snprintf(b, sizeof(b), "SG%02dPRE80", hero);
    CB->animation(b);
    snprintf(b, sizeof(b), "G%02dPRE81", hero);    /* sic: no leading 'S' */
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02dPRE60", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "P%02d_65535", n);
    CB->animation(b);
    snprintf(b, sizeof(b), "SG%02dPRE82", hero);
    CB->animation(b);
    if (ST(ST_PLAYER_WEAPON) != 0)
        snprintf(b, sizeof(b), "G%02darr02_10", hero);
    else
        snprintf(b, sizeof(b), "G%02darr01_10", hero);
    CB->animation(b);
    snprintf(b, sizeof(b), "SP%02darr01_10", n);
    CB->animation(b);
}

/* -------------------------------------------------------------------------
 * event4 - sub_10011A80 (thunk 0x100010F0, op 4). switch(arg) over the
 * script-event ids (byte index table -> jump table @ 0x100126F3); mostly
 * simple state-machine setters. `break` == original jmp default.
 * ---------------------------------------------------------------------- */
static void event4(i32 arg) {        /* sub_10011A80 */
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
 * event5 - sub_1001281F (thunk 0x10001005, op 5): ecx = arg (item id),
 * edx = phase. arg-1 indexes a byte table -> jump table @ 0x100159BB. The
 * missing ids are genuine no-ops (default).
 * ---------------------------------------------------------------------- */
static void event5(i32 arg, i32 phase) {
    ST(ST_SCRIPT_BUSY) = 0;
    switch (arg) {

    case OB_DISH:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD41", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD42", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD43", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_FLAGON22:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD41", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD42", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD43", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_F_THROWER:
        if (ST(ST_LEVEL) == 35) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D05", P_ARKHAN2, "SG00DIA02", NULL);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D06", P_ARKHAN3, "SG00DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        break;

    case OB_FLAGON42:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD41", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD42", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD43", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_SORDOS_DOC:
        if (ST(V_LCOD02_SEEN) == 0 || ST(V_LCOD02_SEEN) == 1) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD02A", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02B", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02C", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02D", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02E", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                lieu_action_register("LCOD02F", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                if (ST(V_LCOD02_SEEN) == 0) {
                    lieu_action_register("LCOD02G", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                    lieu_action_register("LCOD02H", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
                }
                ST(V_LCOD02_SEEN) = 1;
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("LCOD03A", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03B", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03C", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03D", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03E", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
                lieu_action_register("LCOD03F", P_ARKHAN3, "SP03ACT64", "SP03ACT64");
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

    case OB_DIAG_WEAPON:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD88", P_ARKHAN2, "SG00DIA02", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD89", P_ARKHAN3, "SG00DIA02", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_RYLS_NOTES:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD100A", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD100B", P_ARKHAN2);
            CB->dialogue("LCOD100C", P_ARKHAN2);
            CB->dialogue("LCOD100D", P_ARKHAN2);
        }
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
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD103", P_ARKHAN2, "SG02DIA01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD104", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->animation("Sar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_SPITROD:
        if (ST(ST_LEVEL) == 35) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D05", P_ARKHAN2, "SG00DIA02", NULL);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D06", P_ARKHAN3, "SG00DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        break;

    case OB_BOWL:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->dialogue("LCOD41", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD42", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD43", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_THUNDERBOLT:
        if (ST(ST_LEVEL) == 35) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D05", P_ARKHAN2, "SG00DIA02", NULL);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D06", P_ARKHAN3, "SG00DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        break;

    case OB_ZED_THBOLT68:
        if (ST(ST_LEVEL) == 35) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("L35D05", P_ARKHAN2, "SG00DIA02", NULL);
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                lieu_action_register("L35D06", P_ARKHAN3, "SG00DIA02", NULL);
            }
            CB->bloque(BLOQUE_NONE);
        }
        break;

    case OB_BANDOR_DIAG:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD90", P_ARKHAN2, "SG00DIA02", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD91", P_ARKHAN3, "SG00DIA02", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_METAL_SUIT:
        CB->select_pile(0, 2);
        if (ST(V_SOB86_ACTIVE) == 0) {
            if (ST(ST_LEVEL) == 30) {
                if (ST(ST_LOCKED_CAMERA) == 5 && ST(ST_VIE(P_KONKALITE2)) == VIE_DEAD) {
                    ST(V_SOB86_ACTIVE) = 1;
                    CB->bloque(BLOQUE_ALL);
                    CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                    CB->divers_empile(DV_SCREEN, 5, 0);
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->pos_perso(98, CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->pos_perso(99, CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 1, 0);
                    }
                    CB->divers_empile(DV_SCREEN, 3, 0);
                    if (ST(V_HUD_TIMER_MS) == 0) {
                        ST(V_SOB86_ACTIVE) = 0;
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->dialogue("L31D83A", P_ARKHAN2);
                            CB->dialogue("L31D83B", P_ARKHAN2);
                            if (ST(0xC50) == 0) {
                                CB->dialogue("L31D83C", P_ARKHAN2);
                            }
                        }
                        if (ST(ST_HERO) == P_ARKHAN3) {
                            CB->dialogue("L31D84A", P_ARKHAN3);
                            CB->dialogue("L31D84B", P_ARKHAN3);
                            if (ST(0xC50) == 0) {
                                CB->dialogue("L31D84C", P_ARKHAN3);
                            }
                        }
                        CB->animation("Sar_50");
                        CB->divers_empile(DV_SCREEN, 5, 0);
                        if (ST(ST_HERO) == P_ARKHAN2) {
                            CB->pos_perso(P_ARKHAN2, CB->divers(DV_DESC_POS, P_ARKHAN2, 0), CB->divers(DV_DESC_POS, P_ARKHAN2, 1), CB->divers(DV_DESC_POS, P_ARKHAN2, 2), 0, "ar", 1, 0);
                        }
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
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        lieu_action_register("LCOD666", P_ARKHAN2, "SG02DIA02", NULL);
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        lieu_action_register("LCOD667", P_ARKHAN3, "SG03DIA02", NULL);
                    }
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            if (ST(ST_LEVEL) == 37 && ST(ST_LOCKED_CAMERA) == 0 && CB->distance(1, 6) < 350) {
                ST(V_SOB86_ACTIVE) = 1;
                CB->bloque(BLOQUE_ALL);
                CB->etat_objet("SOB86", -1, -1, -1, -1, -1, -1, -6, 0);
                CB->divers_empile(DV_SCREEN, 5, 0);
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->pos_perso(98, CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->pos_perso(99, CB->divers(DV_DESC_POS, ST(ST_HERO), 0), CB->divers(DV_DESC_POS, ST(ST_HERO), 1), CB->divers(DV_DESC_POS, ST(ST_HERO), 2), 1, "ar", 6, 0);
                }
                CB->divers_empile(DV_SCREEN, 3, 0);
                CB->animation("Sar_50");
                CB->bloque(BLOQUE_NONE);
                return;
            } else {
                CB->bloque(BLOQUE_ALL);
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("LCOD666", P_ARKHAN2, "SG02DIA02", NULL);
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("LCOD667", P_ARKHAN3, "SG03DIA02", NULL);
                }
                CB->bloque(BLOQUE_NONE);
                return;
            }
            return;
        }
        if (ST(ST_LEVEL) == 35) {
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN2) {
                lieu_action_register("LCOD670", P_ARKHAN2, "SG02DIA02", NULL);
            }
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
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->pos_perso(P_ARKHAN2, CB->divers(DV_DESC_POS, P_ARKHAN2, 0), CB->divers(DV_DESC_POS, P_ARKHAN2, 1), CB->divers(DV_DESC_POS, P_ARKHAN2, 2), 0, "ar", 1, 0);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->pos_perso(P_ARKHAN3, CB->divers(DV_DESC_POS, P_ARKHAN3, 0), CB->divers(DV_DESC_POS, P_ARKHAN3, 1), CB->divers(DV_DESC_POS, P_ARKHAN3, 2), 0, "ar", 1, 0);
        }
        CB->divers_empile(DV_SCREEN, 3, 0);
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD668", P_ARKHAN2, "SG02DIA01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD669", P_ARKHAN3, "SG03DIA01", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_FLASK77:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN1) {
            CB->animation("SP01ACT17");
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ACT17");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ACT17");
        }
        CB->animation("ar_50");
        if (ST(ST_HERO) == P_ARKHAN1) {
            lieu_action_register("LCOD41", ST(ST_HERO), "SG01ATT01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD42", ST(ST_HERO), "SG01ATT01", NULL);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            lieu_action_register("LCOD43", ST(ST_HERO), "SG01ATT01", NULL);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_PARCHMENT:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(V_LCOD23_SEEN) == 0) {
            ST(V_LCOD23_SEEN) = 1;
        }
        /* 0x3DD6 - not in LIEU33 */
        if (ST(ST_LEVEL) == 28) {
            ST(0x84) = 1;
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            lieu_action_register("LCOD23A", P_ARKHAN2, "SP02ACT64", "SP02ACT64");
            CB->dialogue("LCOD23B", P_ARKHAN2);
            CB->dialogue("LCOD23C", P_ARKHAN2);
            CB->dialogue("LCOD23D", P_ARKHAN2);
            CB->dialogue("LCOD23E", P_ARKHAN2);
            CB->dialogue("LCOD23F", P_ARKHAN2);
            lieu_action_register("LCOD23G", P_ARKHAN2, "SG02DIA01", NULL);
        }
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

    case OB_HELMET:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD672", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD673", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_BREASTPLATE:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD674", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD675", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_VEGETABLE:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN01");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN02");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN01");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN02");
            }
        }
        CB->animation("ar_50");
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 100) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 100));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_VIAL:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(0xE54) == 0) {
            ST(0xE54) = 1;
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT19");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT19");
                }
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SP02ATT20");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SP03ATT20");
                }
            }
            CB->animation("ar_50");
            if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 200) < 1024) {
                CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 200));
            } else {
                CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
            }
            CB->bloque(BLOQUE_NONE);
            return;
        }
        if (ST(0xE54) == 1) {
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->dialogue("LCOD41", ST(ST_HERO));
            }
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->dialogue("LCOD42", ST(ST_HERO));
            }
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

    case OB_FULL_GOURD:
        if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 0) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(0x140 + 4 * phase) > 0) {
                    ST(0x140 + 4 * phase)--;
                }
                if (ST(0x140 + 4 * phase) > 0) {
                    if (ST(ST_HERO) == P_ARKHAN1) {
                        CB->animation("SP01ACT98");
                    }
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT19");
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT19");
                    }
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
                    CB->animation("ar");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
                if (ST(0x140 + 4 * phase) == 0) {
                    if (ST(ST_HERO) == P_ARKHAN1) {
                        CB->animation("SP01ACT98");
                    }
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->animation("SP02ATT19");
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->animation("SP03ATT19");
                    }
                    snprintf(g_ob24, sizeof(g_ob24), "OB24%c", 'A' + phase);
                    CB->etat_objet(g_ob24, -1, -1, -1, -1, -1, -1, -2, 0);
                    snprintf(g_ob77, sizeof(g_ob77), "OB77%c", 'A' + phase);
                    CB->etat_objet(g_ob77, -1, -1, -1, -1, -1, -1, -3, 0);
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
                    if (ST(ST_HERO) == P_ARKHAN1) {
                        CB->dialogue("LCOD41", ST(ST_HERO));
                    }
                    if (ST(ST_HERO) == P_ARKHAN2) {
                        CB->dialogue("LCOD42", ST(ST_HERO));
                    }
                    if (ST(ST_HERO) == P_ARKHAN3) {
                        CB->dialogue("LCOD43", ST(ST_HERO));
                    }
                    CB->animation("SG01ATT01");
                    CB->animation("ar_50");
                    CB->bloque(BLOQUE_NONE);
                    return;
                }
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (ST(ST_HERO) == P_ARKHAN1) {
                    ST(0x140 + 4 * phase) = 0;
                    CB->animation("SP01ACT99");
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
                    CB->animation("P01ACT97");
                    CB->evenement(50, 800);
                    return;
                }
                if (ST(ST_HERO) == P_ARKHAN2) {
                    ST(0x140 + 4 * phase)--;
                    CB->animation("SP02ATT20");
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
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
                if (ST(ST_HERO) == P_ARKHAN3) {
                    ST(0x140 + 4 * phase)--;
                    CB->animation("SP03ATT20");
                    if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 30) < 1024) {
                        CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 30));
                    } else {
                        CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
                    }
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

    case OB_POTION:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02ATT20");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03ATT20");
        }
        CB->animation("ar_50");
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

    case OB_POTATO:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN01");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN02");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN01");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN02");
            }
        }
        CB->animation("ar_50");
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 75) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 75));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_ROOT:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 75) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 75));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_FISH:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 100) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 100));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_MEAT:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN0) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG00MAN01");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG00MAN02");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN1) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG01MAN01");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG01MAN02");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 150) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 150));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_RAT:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 75) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 75));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_ROAST_BIRD:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 100) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 100));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_MORTAR:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG02MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG02MAN04");
            }
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            if (ST(ST_SPECIAL_MODE) == 1) {
                CB->animation("SG03MAN03");
            }
            if (ST(ST_SPECIAL_MODE) == 0) {
                CB->animation("SG03MAN04");
            }
        }
        CB->animation("ar_50");
        if ((CB->divers(DV_HP, ST(ST_HERO), -1) + 150) < 1024) {
            CB->divers_empile(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) + 150));
        } else {
            CB->divers_empile(DV_HP, ST(ST_HERO), 0x3FF);
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_UNIFORM:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD26", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD27", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_BOW_TO_KEY:
    case OB_CRYSTAL_2:
    case OB_CRYSTAL_1:
    case OB_CRYSTAL_3:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L25D79", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L25D82", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_METAL_PLATE:
        CB->select_pile(1, 1);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

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
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_BOOK:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("LCOD39", ST(ST_HERO));
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("LCOD40", ST(ST_HERO));
        }
        CB->animation("SG01ATT01");
        CB->animation("ar_50");
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_TUBE:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(ST_SPECIAL_MODE) == 0) {
            if (ST(ST_HERO) == P_ARKHAN1) {
                if ((CB->divers(DV_HP, P_ARKHAN1, -1) - 50) > 100) {
                    CB->divers(DV_HP, P_ARKHAN1, (CB->divers(DV_HP, P_ARKHAN1, -1) - 50));
                }
                CB->animation("SP01ATT02");
                CB->animation("P01ATT03");
                CB->animation("ar_50");
            }
            if (ST(ST_HERO) == P_ARKHAN2) {
                if ((CB->divers(DV_HP, P_ARKHAN2, -1) - 30) > 50) {
                    CB->divers(DV_HP, P_ARKHAN2, (CB->divers(DV_HP, P_ARKHAN2, -1) - 30));
                }
                CB->animation("SP02ATT02");
                CB->animation("P02ATT03");
                CB->animation("ar_50");
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                if ((CB->divers(DV_HP, P_ARKHAN3, -1) - 20) > 50) {
                    CB->divers(DV_HP, P_ARKHAN3, (CB->divers(DV_HP, P_ARKHAN3, -1) - 20));
                }
                CB->animation("SP02ATT02");
                CB->animation("P02ATT03");
                CB->animation("ar_50");
            }
        }
        if (ST(ST_SPECIAL_MODE) == 1) {
            if (ST(ST_HERO) == P_ARKHAN1) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD25", P_ARKHAN1);
                CB->animation("ar_50");
            }
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD26", P_ARKHAN2);
                CB->animation("ar_50");
            }
            if (ST(ST_HERO) == P_ARKHAN3) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD27", P_ARKHAN3);
                CB->animation("ar_50");
            }
        }
        CB->bloque(BLOQUE_NONE);
        break;

    case OB_JERRYCAN:
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        if (ST(0x6FC) != 2) {
            if (ST(ST_SPECIAL_MODE) == 0) {
                if (ST(ST_HERO) == P_ARKHAN2) {
                    if ((CB->divers(DV_HP, P_ARKHAN2, -1) - 30) > 50) {
                        CB->divers(DV_HP, P_ARKHAN2, (CB->divers(DV_HP, P_ARKHAN2, -1) - 30));
                    }
                    CB->animation("SP02ATT04");
                    CB->animation("P02ATT08");
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    if ((CB->divers(DV_HP, P_ARKHAN3, -1) - 20) > 50) {
                        CB->divers(DV_HP, P_ARKHAN3, (CB->divers(DV_HP, P_ARKHAN3, -1) - 20));
                    }
                    CB->animation("SP03ATT04");
                    CB->animation("P03ATT08");
                    CB->animation("ar_50");
                }
            }
            if (ST(ST_SPECIAL_MODE) == 1) {
                if (ST(ST_HERO) == P_ARKHAN2) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD26", P_ARKHAN2);
                    CB->animation("ar_50");
                }
                if (ST(ST_HERO) == P_ARKHAN3) {
                    CB->animation("SG00ATT01");
                    CB->dialogue("LCOD27", P_ARKHAN3);
                    CB->animation("ar_50");
                }
            }
        }
        if (ST(0x6FC) == 2) {
            if (ST(ST_HERO) == P_ARKHAN2) {
                CB->animation("SG00ATT01");
                CB->dialogue("LCOD26", P_ARKHAN2);
                CB->animation("ar_50");
            }
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
 * event6 - sub_10015AF3 (thunk 0x10001041, op 6). Switch on arg (0..4):
 * LCOD hint triggers and the SC21 ending; jump table @ 0x10015D5B.
 * ---------------------------------------------------------------------- */
static void event6(i32 arg) {
    i32 stance_was_on = 0;   /* [ebp-4]: set in the SC21 ending, never read */

    switch (arg) {
    case EV6_LCOD34:        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD34", P_ARKHAN2, "SG00CON01", NULL);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD35:        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD35", P_ARKHAN2, "SG00CON01", NULL);
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->animation("SG00CON01");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD36:        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD36", P_ARKHAN3, "SG00CON01", "SG00CON01");
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD37:        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD37", P_ARKHAN3, "SG00CON01", NULL);
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_SC21_ENDING:        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(0, 2);
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
 * Per-frame body - sub_10003F67 (thunk 0x100010B4), level-14 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    snprintf(g_txt, sizeof(g_txt), "comb:%d", ST(V_ENGAGED_PERSONA));
    CB->print_msg(g_txt, 0, 240);
    snprintf(g_txt, sizeof(g_txt), "Malfrat4:%d", ST(ST_VIE(P_MALROT4)));
    CB->print_msg(g_txt, 0, 248);
    /* 0x3FD6 */
    if (ST(V_EVENT_0) == 1 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(V_ENGAGED_PERSONA) = P_MALROT1;
        CB->combat(P_MALROT1, 0);
        CB->select_pile(2, 1);
        CB->evenement(2, -1);
        CB->stop_joindre(P_MALROT1);
        ST(V_EVENT_2) = 0;
        ST(V_EVENT_0) = 0;
        CB->select_pile(0, 1);
        /* 0x4066 */
        if (ST(ST_VIE(P_MALROT4)) == 1 && ST(0x170) != 1 && ST(V_DELIA_PHASE) != 3 && ST(V_DELIA_PHASE) != 4) {
            CB->select_pile(1, 1);
            CB->evenement(1, -1);
            CB->animation("Sar_10");
            ST(V_EVENT_1) = 0;
            CB->stop_joindre(-P_MALROT4);
            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
            CB->joindre(-P_MALROT4, 0, 0, 0, ST(ST_HERO));
            CB->modify_var(&ST(0xCB8), 1);
            ST(V_ENGAGED_PERSONA) = 0x1814;
            CB->combat(P_MALROT4, 0);
            CB->select_pile(0, 1);
        }
    }
    /* 0x415F */
    if (ST(V_EVENT_1) == 1 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(0xCB4) == 0 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(V_EVENT_1) = 0;
        /* 0x41B3 */
        if (ST(0xCB0) == 1) {
            ST(0xCB0) = 0;
            CB->select_pile(1, 1);
            CB->animation("SP64ARR01_10");
            ST(0x70) = CB->random(3);
            /* 0x4200 */
            if (ST(0x70) == 0) {
                CB->joindre(-P_MALROT4, 6245, 1272, 0xBC4D, -1);
                CB->modify_var(&ST(0xCB0), 2);
            }
            /* 0x4242 */
            if (ST(0x70) == 1) {
                CB->joindre(-P_MALROT4, 5737, 1624, 0x62CC, -1);
                CB->modify_var(&ST(0xCB0), 3);
            }
            /* 0x4284 */
            if (ST(0x70) == 2) {
                CB->joindre(-P_MALROT4, 4332, 1306, 0x4260, -1);
                CB->modify_var(&ST(0xCB0), 4);
            }
            CB->evenement(1, 1000);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x42EB */
        if (ST(0xCB0) == 2) {
            ST(0xCB0) = 0;
            CB->select_pile(1, 1);
            CB->animation("SP64ARR01_10");
            ST(0x70) = CB->random(2);
            /* 0x4338 */
            if (ST(0x70) == 0) {
                CB->joindre(-P_MALROT4, 5737, 1624, 0x62CC, -1);
                CB->modify_var(&ST(0xCB0), 3);
            }
            /* 0x437A */
            if (ST(0x70) == 1) {
                CB->joindre(-P_MALROT4, 4332, 1306, 0x4260, -1);
                CB->modify_var(&ST(0xCB0), 4);
            }
            CB->evenement(1, 1000);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x43E1 */
        if (ST(0xCB0) == 3) {
            ST(0xCB0) = 0;
            CB->select_pile(1, 1);
            CB->animation("SP64ARR01_10");
            ST(0x70) = CB->random(2);
            /* 0x442E */
            if (ST(0x70) == 0) {
                CB->joindre(-P_MALROT4, 6245, 1272, 0xBC4D, -1);
                CB->modify_var(&ST(0xCB0), 2);
            }
            /* 0x4470 */
            if (ST(0x70) == 1) {
                CB->joindre(-P_MALROT4, 4332, 1306, 0x4260, -1);
                CB->modify_var(&ST(0xCB0), 4);
            }
            CB->evenement(1, 1000);
            CB->select_pile(0, 1);
            return;
        }
        /* 0x44D7 */
        if (ST(0xCB0) == 4) {
            ST(0xCB0) = 0;
            CB->select_pile(1, 1);
            CB->animation("SP64ARR01_10");
            ST(0x70) = CB->random(2);
            /* 0x4524 */
            if (ST(0x70) == 0) {
                CB->joindre(-P_MALROT4, 6245, 1272, 0xBC4D, -1);
                CB->modify_var(&ST(0xCB0), 2);
            }
            /* 0x4566 */
            if (ST(0x70) == 1) {
                CB->joindre(-P_MALROT4, 5737, 1624, 0x62CC, -1);
                CB->modify_var(&ST(0xCB0), 3);
            }
            CB->evenement(1, 1000);
            CB->select_pile(0, 1);
            return;
        }
    }
    /* 0x45CD */
    if (ST(0xCB4) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && CB->distance(1, 2) > 200) {
        CB->select_pile(1, 1);
        ST(0x90) = 0;
        ST(0xCB4) = 0;
        ST(0xCB0) = 10;
        ST(0xCB8) = 0;
        CB->stop_joindre(-P_MALROT4);
        CB->joindre(P_MALROT4, 4787, 1423, 0xB798, -1);
        CB->modify_var(&ST(0xCB0), 1);
        CB->evenement(1, 7500);
        CB->select_pile(0, 1);
    }
    /* 0x46C2 */
    if (ST(V_EVENT_2) == 1 && ST(V_GUARD_FIGHT_DONE) == 2 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD && ST(V_DELIA_PHASE) == 0 && ST(0xC44) == 0) {
        CB->select_pile(2, 1);
        CB->divers(DV_SCREEN, 3, 0);
        ST(V_EVENT_2) = 0;
        /* 0x4767 */
        if (CB->divers(DV_DESC_POS, P_MALROT1, 2) > 55000 || CB->divers(DV_DESC_POS, P_MALROT1, 2) < 10000) {
            CB->stop_joindre(-P_MALROT1);
            CB->divers_empile(DV_LOOK_AT_PLAYER, P_MALROT1, 0);
            CB->animation("SP61ATT05");
            CB->animation("P61ATT04");
            CB->divers_empile(DV_LOOK_AT_PLAYER, P_MALROT1, 1);
        }
        CB->evenement(2, 5000);
        CB->select_pile(0, 1);
    }
    /* 0x47F8 */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        /* 0x4819 */
        if (ST(0xDC) == 64) {
            ST(0xDC) = 99;
            CB->dialogue("L14D235", P_MALROT4);
            ST(V_ENGAGED_PERSONA) = P_MALROT4;
            CB->combat(P_MALROT4, 0);
            CB->animation("Sar_50");
            CB->modify_var(&ST(V_CRI_USED), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x489D */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 3) && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD && CB->direction_perso(P_PROVIDER)) {
        ST(V_CRI_USED) = 1;
        /* 0x4925 */
        if (ST(0xE0) != 99) {
            ST(0xE0) = 16;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(34, 1000);
        return;
    }
    /* 0x4966 */
    if (ST(V_EVENT_34) == 1) {
        ST(V_EVENT_34) = 0;
        /* 0x4987 */
        if (ST(0xE0) == 16) {
            ST(0xE0) = 99;
            lieu_action_register("L14D236", P_PROVIDER, "SP16GAR03", "SP16GAR03");
            CB->animation("Sar_50");
            CB->modify_var(&ST(V_CRI_USED), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x49ED */
    if (ST(0xE0) == 99) {
        ST(0xE0) = 16;
    }
    /* 0x4A0E */
    if (ST(V_EVENT_35) == 1) {
        ST(V_EVENT_35) = 0;
        /* 0x4A2F */
        if (ST(0xF0) == 61) {
            ST(0xF0) = 99;
            lieu_action_register("L14D237", P_MALROT1, "SP61GAR02", "SP61GAR02");
            ST(V_ENGAGED_PERSONA) = P_MALROT1;
            CB->combat(P_MALROT1, 0);
            CB->animation("Sar_50");
            CB->modify_var(&ST(V_CRI_USED), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x4ABC */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 24) && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD && CB->direction_perso(P_MALROT1)) {
        ST(V_CRI_USED) = 1;
        ST(0x4CC) = 1;
        /* 0x4B53 */
        if (ST(0xF0) != 99) {
            ST(0xF0) = 61;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(35, 1000);
        return;
    }
    /* 0x4B94 */
    if (ST(0xF0) == 99) {
        ST(0xF0) = 16;
    }
    /* 0x4BC3 */
    if (CB->divers(DV_IN_ZONE, ST(ST_HERO), 33) && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_DELIA)) == 1 && CB->direction_perso(P_DELIA)) {
        ST(V_CRI_USED) = 1;
        /* 0x4C4B */
        if (ST(V_WEAPON_WARNED) != 99) {
            ST(V_WEAPON_WARNED) = 10;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(37, 1000);
        return;
    }
    /* 0x4C8C */
    if (ST(V_WEAPON_WARNED) == 99) {
        ST(V_WEAPON_WARNED) = 10;
    }
    /* 0x4CAD */
    if (ST(V_EVENT_37) == 1) {
        ST(V_EVENT_37) = 0;
        /* 0x4CCE */
        if (ST(V_WEAPON_WARNED) == 10) {
            ST(V_WEAPON_WARNED) = 99;
            lieu_action_register("L16D122", P_DELIA, "SP10DIA50", "SP10ARR01_10");
            CB->animation("Sar_50");
            CB->modify_var(&ST(V_CRI_USED), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    /* 0x4D34 */
    if (ST(V_EVENT_36) == 1) {
        CB->select_pile(1, 2);
        ST(V_EVENT_36) = 0;
        /* 0x4D65 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->play_anim(0, 1);
            CB->play_anim(1, 1);
        }
        /* 0x4D94 */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
            CB->play_anim(2, 1);
            CB->play_anim(3, 1);
        }
        ST(0xFC) = 1;
        CB->select_pile(0, 2);
    }
    /* 0x4DE2 */
    if (ST(ST_VIE(P_MALROT4)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) != 0) {
        /* 0x4E00 */
        if (ST(V_ENGAGED_PERSONA) == P_MALROT4) {
            ST(V_ENGAGED_PERSONA) = 0;
            return;
        }
        /* 0x4E23 */
        if (ST(V_ENGAGED_PERSONA) == 0x1814) {
            ST(V_ENGAGED_PERSONA) = P_MALROT1;
            return;
        }
        /* 0x4E46 */
        if (ST(V_ENGAGED_PERSONA) == 0x1910) {
            ST(V_ENGAGED_PERSONA) = P_PROVIDER;
            return;
        }
    }
    /* 0x4E69 */
    if (ST(ST_VIE(P_PROVIDER)) == VIE_DEAD) {
        /* 0x4E78 */
        if (ST(V_ENGAGED_PERSONA) == P_PROVIDER) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
        /* 0x4E96 */
        if (ST(V_ENGAGED_PERSONA) == 0x1910) {
            ST(V_ENGAGED_PERSONA) = P_MALROT4;
        }
        CB->ajoute_zone(38, 3315, 945, 275, 280, 0);
        CB->ajoute_zone(39, 3318, 957, 50, 50, 0);
    }
    /* 0x4EF0 */
    if (ST(ST_VIE(P_MALROT1)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) != 0) {
        /* 0x4F0E */
        if (ST(V_ENGAGED_PERSONA) == P_MALROT1) {
            ST(V_ENGAGED_PERSONA) = 0;
            return;
        } else {
            ST(V_ENGAGED_PERSONA) = P_MALROT4;
        }
        CB->ajoute_zone(9, 3096, 1157, 470, 135, 0);
    }
    /* 0x4F63 */
    if (ST(ST_VIE(35)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == 35) {
        ST(V_ENGAGED_PERSONA) = 0;
    }
    /* 0x4F90 */
    if (ST(0xBE8) >= 1) {
        /* 0x4F9F */
        if (ST(0x78) == 1 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD && CB->distance(1, 3) > 480 && ST(V_ENGAGED_PERSONA) == 0) {
            ST(0x78) = 0;
            CB->joindre(P_PROVIDER, 3267, 915, 0xCE17, -1);
            ST(0x64) = 0;
        }
        /* 0x501E */
        if (ST(0x78) == 0 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2 && CB->distance(1, 3) < 200 && ST(V_ENGAGED_PERSONA) == 0) {
            ST(0x78) = 1;
            CB->joindre(P_PROVIDER, 0, 0, 0, ST(ST_HERO));
        }
    }
    /* 0x50A4 */
    if (ST(0x64) == 1 && ST(0xCCC) >= 1 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2 && CB->distance(1, 3) < 250 && ST(ST_HERO) == P_ARKHAN2) {
        ST(0x64) = 0;
        CB->dialogue("L14D263", P_PROVIDER);
    }
    /* 0x5132 */
    if (ST(0x64) == 0 && ST(0xCCC) >= 1 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD && ST(V_GUARD_FIGHT_DONE) != 2 && CB->distance(1, 3) > 400) {
        ST(0x64) = 1;
    }
    /* 0x51A1 */
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(0xC44) == 0 && ST(0x88) == 0 && CB->distance(1, 24) < 200 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD) {
        /* 0x5213 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->select_pile(2, 2);
            CB->bloque(BLOQUE_ALL);
            ST(0x88) = 1;
            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT1, 1);
            CB->joindre(P_MALROT1, 0, 0, 0, ST(ST_HERO));
            CB->animation("SP61GAR26");
            CB->animation("P61GAR02");
            CB->dialogue("L14D171", P_MALROT1);
            /* 0x52A4 */
            if (ST(ST_SPECIAL_MODE) == 1) {
                lieu_action_register("L14D139A", P_ARKHAN2, "SG00DIA02", NULL);
            }
            /* 0x52CC */
            if (ST(ST_SPECIAL_MODE) == 0) {
                lieu_action_register("L14D139B", P_ARKHAN2, "SG00DIA02", NULL);
            }
            CB->dialogue("L14D140", P_MALROT1);
            CB->animation("P61GAR27");
            CB->animation("P61ARR01");
            CB->joindre(P_MALROT1, 3649, 1268, 0xE98, -1);
            CB->animation("P61ATT04");
            CB->bloque(BLOQUE_NONE);
            CB->select_pile(0, 1);
        }
        /* 0x5356 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(2, 2);
            CB->bloque(BLOQUE_ALL);
            ST(0x88) = 1;
            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT1, 1);
            CB->joindre(P_MALROT1, 0, 0, 0, ST(ST_HERO));
            CB->animation("SP61GAR26");
            CB->animation("P61GAR02");
            CB->dialogue("L14D172", P_MALROT1);
            CB->bloque(BLOQUE_NONE);
            ST(0x4CC) = 1;
            ST(V_ENGAGED_PERSONA) = P_MALROT1;
            CB->combat(P_MALROT1, 0);
            /* 0x5417 */
            if (ST(ST_VIE(P_MALROT4)) == 1) {
                CB->select_pile(1, 1);
                CB->evenement(1, -1);
                CB->animation("Sar_10");
                ST(V_EVENT_1) = 0;
                CB->stop_joindre(-P_MALROT4);
                CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
                CB->joindre(-P_MALROT4, 0, 0, 0, ST(ST_HERO));
                CB->modify_var(&ST(0xCB8), 1);
                ST(V_ENGAGED_PERSONA) = 0x1814;
                CB->combat(P_MALROT4, 0);
                CB->select_pile(0, 1);
            }
        }
    }
    /* 0x54D7 */
    if (ST(0x7C) == 1) {
        CB->select_pile(2, 1);
        ST(0x7C) = 40;
        /* 0x5505 */
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            CB->joindre(-P_KALHI13, 4781, 1328, -1, -1);
        }
        CB->modify_var(&ST(0x7C), 2);
        CB->select_pile(0, 1);
    }
    /* 0x554E */
    if (ST(0x7C) == 2) {
        CB->select_pile(2, 1);
        /* 0x5570 */
        if (ST(ST_VIE(P_KALHI13)) == 1 && !CB->on_joindre(P_KALHI13)) {
            ST(0x7C) = 3;
            CB->stop_joindre(-P_KALHI13);
            CB->joindre(-P_KALHI13, 5181, 979, -1, -1);
            CB->animation("Sar_10");
            CB->modify_var(&ST(0x7C), 4);
        }
        CB->select_pile(0, 1);
    }
    /* 0x55EE */
    if (ST(0x7C) == 4) {
        CB->select_pile(2, 1);
        /* 0x5610 */
        if (ST(ST_VIE(P_KALHI13)) == 1 && !CB->on_joindre(P_KALHI13)) {
            ST(0x7C) = 5;
            CB->pos_perso(P_KALHI13, 0, 0, 0, 4, "", -1, 0);
            CB->modify_var(&ST(ST_VIE(P_KALHI13)), 1);
            CB->vire_col(26, 1);
            CB->pos_perso(P_MALROT4, 5592, 1439, 0x2D9D, 1, "P64ARR01", 2, 601);
            CB->select_pile(1, 1);
            CB->animation("Sar_10");
            CB->modify_var(&ST(0xCB0), 1);
            CB->evenement(1, 2000);
            CB->select_pile(0, 1);
            ST(0xBE8) = 1;
            CB->retire_zone(9);
        }
        CB->select_pile(0, 1);
    }
    /* 0x572B */
    if (CB->distance(1, 4) > 700 && ST(0x80) == 0 && ST(0xC44) == 0) {
        CB->select_pile(2, 1);
        ST(0x80) = 1;
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0x579D */
    if (CB->divers(DV_WAS_HIT, P_MALROT4, 0) && ST(ST_VIE(P_MALROT4)) != VIE_DEAD) {
        ST(ST_VIE(P_MALROT4)) = VIE_DEAD;
        CB->select_pile(1, 2);
        CB->stop_joindre(-P_MALROT4);
        CB->combat(P_MALROT4, -1);
        CB->divers_empile(DV_BONE_SOUND, P_MALROT4, 0);
        CB->animation("SP64IMP51");
        CB->animation("P64CAD");
        CB->pos_perso(P_MALROT4, 0, 0, 0, 2, "P64CAD", 2, 0);
        ST(0x4CC) = 1;
        CB->select_pile(0, 1);
        /* 0x585B */
        if (ST(ST_VIE(P_MALROT1)) == 1) {
            CB->select_pile(2, 2);
            CB->evenement(2, -1);
            ST(V_EVENT_1) = 0;
            ST(V_ENGAGED_PERSONA) = 0x1814;
            CB->combat(P_MALROT1, 0);
            CB->select_pile(0, 1);
        }
    }
    /* 0x58CA */
    if (CB->divers(DV_WAS_HIT, P_PROVIDER, 0) && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        ST(ST_VIE(P_PROVIDER)) = VIE_DEAD;
        CB->combat(P_PROVIDER, -1);
        ST(V_ENGAGED_PERSONA) = P_PROVIDER;
        CB->stop_joindre(-P_PROVIDER);
        CB->divers_empile(DV_BONE_SOUND, P_PROVIDER, 0);
        CB->animation("SP16IMP53");
        CB->animation("P16CAD");
        CB->pos_perso(P_PROVIDER, 0, 0, 0, 2, "P16CAD", 3, 0);
    }
    /* 0x5970 */
    if (CB->divers(DV_WAS_HIT, P_MALROT1, 0) && ST(ST_VIE(P_MALROT1)) != VIE_DEAD) {
        CB->select_pile(2, 2);
        ST(ST_VIE(P_MALROT1)) = VIE_DEAD;
        CB->stop_joindre(-P_MALROT1);
        CB->combat(P_MALROT1, -1);
        CB->divers_empile(DV_BONE_SOUND, P_MALROT1, 0);
        CB->animation("SP61IMP52");
        CB->animation("P61CAD");
        CB->pos_perso(P_MALROT1, 0, 0, 0, 2, "P61CAD", 0x18, 0);
        ST(0x4CC) = 1;
        CB->select_pile(0, 1);
        /* 0x5A2E */
        if (ST(ST_VIE(P_MALROT4)) == 1) {
            CB->select_pile(1, 1);
            CB->evenement(1, -1);
            CB->animation("Sar_10");
            ST(V_EVENT_1) = 0;
            CB->stop_joindre(-P_MALROT4);
            CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
            CB->joindre(-P_MALROT4, 0, 0, 0, ST(ST_HERO));
            CB->modify_var(&ST(0xCB8), 1);
            ST(V_ENGAGED_PERSONA) = 0x1814;
            CB->combat(P_MALROT4, 0);
            CB->select_pile(0, 1);
        }
    }
    /* 0x5AF9 */
    if (CB->divers(DV_WAS_HIT, 35, 0) && ST(ST_VIE(35)) != VIE_DEAD) {
        CB->select_pile(1, 2);
        CB->divers_empile(DV_WAS_HIT, 35, 1);
        CB->combat(35, -1);
        /* 0x5B63 */
        if ((CB->divers(DV_HP, 35, -1) - ST(V_HIT_DAMAGE)) > 0) {
            CB->divers(DV_HP, 35, (CB->divers(DV_HP, 35, -1) - ST(V_HIT_DAMAGE)));
        } else {
            CB->divers(DV_HP, 35, 1);
        }
        /* 0x5BC4 */
        if (CB->divers(DV_HP, 35, -1) <= 1) {
            ST(ST_VIE(35)) = VIE_DEAD;
            CB->stop_joindre(-35);
            CB->divers_empile(DV_BONE_SOUND, 35, 0);
            CB->animation("SP35IMP51");
            CB->animation("P35CAD");
            CB->pos_perso(0x23, 0, 0, 0, 2, "P35CAD", 4, 0);
            CB->divers_empile(DV_SCREEN, 2, 0);
        } else {
            CB->stop_joindre(-35);
            CB->divers_empile(DV_BONE_SOUND, 35, 0);
            CB->animation("SP35IMP06");
            CB->animation("P35GAR01");
            CB->combat(35, 0);
        }
        CB->select_pile(0, 1);
    }
    /* 0x5C95 */
    if (ST(ST_COMBAT_STANCE) == 1 && CB->divers(DV_CAN_FIGHT, 9, 2) && ST(V_EVENT15_STEP) == 0 && ST(V_ENGAGED_PERSONA) == 0 && CB->distance(1, 3) < 200 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(V_EVENT15_STEP) = 1;
        CB->divers(DV_LOOK_AT_PLAYER, P_PROVIDER, 1);
        CB->dialogue("L14D167", P_PROVIDER);
        CB->joindre(-P_PHEDORIA, 0, 0, 0, ST(ST_HERO));
        CB->animation("SP16GAR03");
        CB->joindre_perm(P_PROVIDER, ST(ST_HERO));
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x5D8E */
    if (ST(ST_COMBAT_STANCE) == 0 && ST(V_EVENT15_STEP) == 1 && ST(V_ENGAGED_PERSONA) == 0 && CB->distance(1, 3) < 250 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(V_EVENT15_STEP) = 0;
        CB->divers(DV_LOOK_AT_PLAYER, P_PROVIDER, 0);
        CB->dialogue("L14D168", P_PHEDORIA);
        CB->stop_joindre(-P_PROVIDER);
        CB->joindre(P_PROVIDER, 3667, 1075, 0x8A83, -1);
        CB->animation("SP16ARR01");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x5E61 */
    if (ST(ST_COMBAT_STANCE) == 1 && CB->distance(1, 3) > 300 && ST(V_EVENT15_STEP) == 1 && ST(ST_VIE(P_PROVIDER)) != VIE_DEAD) {
        CB->bloque(BLOQUE_ALL);
        ST(V_EVENT15_STEP) = 0;
        CB->divers(DV_LOOK_AT_PLAYER, P_PROVIDER, 0);
        CB->stop_joindre(-P_PROVIDER);
        CB->joindre(P_PROVIDER, 3667, 1075, 0x8A83, -1);
        CB->animation("SP16ARR01");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x5F15 */
    if (ST(ST_COMBAT_STANCE) == 1 && CB->divers(DV_CAN_FIGHT, 9, 2) && ST(0xCB8) == 1 && ST(0x90) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && CB->distance(1, 2) < 200) {
        CB->select_pile(1, 1);
        CB->evenement(1, -1);
        ST(V_EVENT_1) = 1;
        ST(0x90) = 1;
        CB->animation("Sar_10");
        CB->bloque(BLOQUE_ALL);
        CB->stop_joindre(-P_MALROT4);
        CB->divers(DV_LOOK_AT_PLAYER, P_MALROT4, 1);
        CB->joindre(-P_MALROT4, 0, 0, 0, ST(ST_HERO));
        CB->dialogue("L14D169", P_MALROT4);
        CB->modify_var(&ST(0xCB8), 1);
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0x6068 */
    if (ST(ST_COMBAT_STANCE) == 0 && ST(0x90) == 1 && ST(ST_VIE(P_MALROT4)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) == 0 && ST(0xCB8) == 1 && CB->distance(1, 2) < 200) {
        CB->bloque(BLOQUE_ALL);
        ST(0x90) = 0;
        CB->dialogue("L14D170A", P_MALROT4);
        CB->dialogue("L14D170B", P_MALROT4);
        CB->stop_joindre(-P_MALROT4);
        CB->animation("SP64ARR01_10");
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x6132 */
    if (ST(0x98) != 0 && CB->distance(1, 24) > 150) {
        ST(0x98) = 0;
        CB->select_pile(2, 1);
        CB->joindre(P_MALROT1, 3649, 1268, 0xE98, -1);
        CB->animation("SP61ATT04");
        CB->evenement(2, 5000);
        CB->select_pile(0, 1);
    }
    /* 0x61BF */
    if (ST(ST_COMBAT_STANCE) == 1 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(0xB8) == 0 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD && CB->distance(1, 24) < 150) {
        CB->select_pile(2, 2);
        CB->bloque(BLOQUE_ALL);
        ST(0xB8) = 1;
        CB->evenement(2, -1);
        ST(V_EVENT_2) = 0;
        CB->joindre_perm(P_MALROT1, ST(ST_HERO));
        CB->stop_joindre(P_MALROT1);
        CB->animation("SP61GAR26");
        CB->animation("P61GAR02");
        CB->bloque(BLOQUE_NONE);
        CB->select_pile(0, 1);
    }
    /* 0x62C0 */
    if (ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(0xB8) == 1 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD) {
        CB->select_pile(2, 2);
        CB->bloque(BLOQUE_ALL);
        ST(0xB8) = 0;
        /* 0x6347 */
        if (CB->distance(1, 24) < 200) {
            CB->dialogue("L14D204", P_MALROT1);
        }
        CB->animation("SP61GAR27");
        CB->animation("P61ARR01");
        CB->joindre(P_MALROT1, 3649, 1268, 0xE98, -1);
        CB->bloque(BLOQUE_NONE);
        CB->evenement(2, 100);
        CB->select_pile(0, 1);
    }
    /* 0x63BD */
    if (ST(V_ENGAGED_PERSONA) == 0 && ST(ST_SCRIPT_BUSY) == 0 && ST(0xB8) == 1 && ST(ST_VIE(P_MALROT1)) != VIE_DEAD && CB->distance(1, 24) > 300) {
        CB->select_pile(2, 2);
        CB->bloque(BLOQUE_ALL);
        ST(0xB8) = 0;
        CB->animation("SP61GAR27");
        CB->animation("P61ARR01");
        CB->joindre(P_MALROT1, 3649, 1268, 0xE98, -1);
        CB->bloque(BLOQUE_NONE);
        CB->evenement(2, 100);
        CB->select_pile(0, 1);
    }
    /* 0x64A7 */
    if (CB->distance(1, 2) > 400 && ST(0xCBC) >= 1) {
        ST(0xCBC) = 0;
    }
    /* 0x64E3 */
    if (CB->distance(1, 25) > 400 && ST(0x84) >= 1) {
        ST(0x84) = 0;
    }
    /* 0x6516 */
    if (ST(V_DELIA_PHASE) == 4 && ST(ST_VIE(P_DELIA)) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        CB->select_pile(2, 1);
        /* 0x6560 */
        if (CB->distance(33, 1) < 100 && ST(0xBC) == 0) {
            ST(0xBC) = 1;
            CB->joindre_perm(-P_DELIA, ST(ST_HERO));
            CB->stop_joindre(P_DELIA);
            CB->pos_perso(P_DELIA, CB->divers(DV_DESC_POS, P_DELIA, 0), CB->divers(DV_DESC_POS, P_DELIA, 1), CB->divers(DV_DESC_POS, P_DELIA, 2), 1, "P10ARR01", 0x21, 0);
        }
        /* 0x6604 */
        if (ST(0xD4) == 1 && (ST(0x38) % 20) == 0) {
            /* 0x6637 */
            if (CB->distance(33, 1) >= 180) {
                CB->divers(DV_RUN, P_DELIA, 0);
                ST(0xBC) = 0;
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
            /* 0x6693 */
            if (CB->distance(33, 1) >= 300) {
                ST(0xBC) = 0;
                CB->divers(DV_RUN, P_DELIA, 1);
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
        }
        CB->select_pile(0, 1);
    }
    /* 0x66FB */
    if (CB->divers(DV_WAS_HIT, P_DELIA, 0) && ST(ST_VIE(P_DELIA)) != VIE_DEAD && !CB->divers(DV_IN_ZONE, ST(ST_HERO), 33)) {
        ST(V_DELIA_PHASE) = 20;
        ST(V_DELIA_KILLED) = 14;
        ST(0x170) = 1;
        ST(ST_VIE(P_DELIA)) = VIE_DEAD;
        CB->select_pile(1, 2);
        CB->combat(P_DELIA, -1);
        CB->stop_joindre(-P_DELIA);
        CB->animation("SP10IMP51");
        CB->animation("P10CAD");
        CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10CAD", 0x21, 0);
        CB->select_pile(0, 1);
    }
    /* 0x67E5 */
    if (ST(ST_LOCKED_CAMERA) == 2 && ST(0xFC) == 0 && ST(0xBF4) == 0) {
        /* 0x681B */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->play_anim(0, 1);
            CB->play_anim(1, 1);
            CB->bruitage("L14AMB4", -1, -1);
            ST(0xFC) = 1;
        }
        /* 0x686B */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
            CB->play_anim(2, 1);
            CB->play_anim(3, 1);
            CB->bruitage("L14AMB3", -1, -1);
            ST(0xFC) = 1;
        }
    }
    /* 0x68BE */
    if (ST(ST_LOCKED_CAMERA) != 2 && ST(0xFC) == 1) {
        ST(0xFC) = 0;
        /* 0x68F1 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->bruitage("L14AMB2", -1, -1);
        }
        /* 0x6915 */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
            CB->bruitage("L14AMB1", -1, -1);
        }
    }
    /* 0x6939 */
    if (ST(ST_LOCKED_CAMERA) != 2 && ST(0x100) == 1) {
        ST(0x100) = 0;
    }
    /* 0x696C */
    if (ST(0xBF4) == 1 && ST(0x100) == 0 && ST(ST_LOCKED_CAMERA) == 2) {
        /* 0x69A2 */
        if (ST(V_GUARD_FIGHT_DONE) == 2) {
            CB->bruitage("L14AMB2", -1, -1);
        }
        /* 0x69C6 */
        if (ST(V_GUARD_FIGHT_DONE) != 2) {
            CB->bruitage("L14AMB1", -1, -1);
        }
        CB->pause_anim(0, 0, 0);
        CB->pause_anim(1, 0, 0);
        ST(0x100) = 1;
        ST(0x560) = 0;
    }
    /* 0x6A23 */
    if (ST(ST_HERO) == 4 && ST(0x68) == 0) {
        ST(0x68) = 1;
        CB->evenement(44, 3000);
    }
    /* 0x6A60 */
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        /* 0x6A86 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x6AAF */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x6ADB */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x6B07 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x6B2B */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        /* 0x6B48 */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
        return;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x1000158A).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1002967C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu54_trig.c. There is no death
 * branch in this version.
 *   arg = reload flag (non-zero => skip arming).
 * ---------------------------------------------------------------------- */
static void init(i32 reload_flag) {
    if (reload_flag == 0)
        l54_trig00();   /* 0x100015A3: call sub_1000106E -> sub_10001D60 */
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1, 0x100015F6): drain pending sound, run body.
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x10001200).
 * Jump table @ 0x10001673, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu54_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu54: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        l54_reset_dll_globals();
        init(arg);
        break;
    case LIEU_OP_EVENT:  l54_trigger(arg);             break;  /* dword_10029BC0[arg]() */
    case LIEU_OP_CALL4:  event4(arg);                  break;
    case LIEU_OP_CALL5:  event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
