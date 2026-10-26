/*
 * lieu50.c - Native hand-port of LIEU50.DLL (level-50 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick, the tick body and the
 * level's own event4 / event5 / event6. The 42 zone trigger handlers live in
 * lieu50_trig.c (reached through l50_trigger, see lieu50_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. LIEU50 is a variant of the level-12 script
 * (same rooms, dialogue codes and zones) linked with a different layout: the
 * entry is at 0x10010220, the state pointer global is dword_1002248C and the
 * trigger table is dword_100223A0[42].
 *
 * Its event4 (sub_1000AE60), event5 (sub_1000BBFF) and event6
 * (sub_1000EED3) are the same script version as LIEU33's (older than the
 * lieu_common.c ones, plus a V_END_GAME_REQ case in event4 and a level 28
 * test in the parchment case of event5), so they are ported here;
 * lieu_action_register is the shared one. The hero pre-scene sub_10010876
 * lacks the `n != 6` test of lieu_pre_scene, so it is ported here too
 * (l50_pre_scene), with its own copy of pre90_scene.
 * The DLL .data the level code touches: the four event4 float accumulators
 * (initially 1.0f) and the two event5 sprintf buffers (dword_100224F0 /
 * dword_100224C0); all are reset at INIT.
 *
 * Note: trigger 0 tests ST(0x990), the start point slot of level 12 (the
 * script was copied from LIEU12 without changing the slot).
 */

#include <stdio.h>
#include <stdlib.h>

#include "lieu50_int.h"

/* event4 float accumulators, dword_1001FA30..3C (initially 1.0f). */
static float g_float_accum[4] = {1.0f, 1.0f, 1.0f, 1.0f};

/* event5 sprintf buffers: dword_10025A60 ("OB24%c") and dword_10025A40
 * ("OB77%c"). */
static c8 g_ob24[32];
static c8 g_ob77[32];

/* -------------------------------------------------------------------------
 * pre90_scene - sub_1001A24C-equivalent (thunk sub_100010D7), identical to
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
 * sub_10010876 (thunk 0x100010AA): the hero greets persona n (search /
 * handshake sequence). __fastcall(ecx=hero, edx=n) + stdcall stack (a1
 * unused, a2, a3); ret 0Ch. a2/a3 go to Distance. Same as lieu_pre_scene
 * except that the Sp%02dMAR30 animation does not test n != 6.
 * ---------------------------------------------------------------------- */
void l50_pre_scene(i32 hero, i32 n, i32 a1, i32 a2, i32 a3) {
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
 * event4 - sub_1000AE60 (thunk 0x100010F0, op 4). switch(arg) over the
 * script-event ids (byte index table -> jump table @ 0x10001E5F); mostly
 * simple state-machine setters. `break` == original jmp default.
 * ---------------------------------------------------------------------- */
static void event4(i32 arg) {        /* sub_1000AE60 */
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
        }
        break;

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

    case EV_END_GAME:
        ST(V_END_GAME_REQ) = 1;
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
 * event5 - sub_1000BBFF (thunk 0x10001005, op 5): ecx = arg (item id),
 * edx = phase. arg-1 indexes a byte table -> jump table @ 0x10005105. The
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
        }
        break;

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
 * event6 - sub_1000523D (thunk 0x10001046, op 6). Switch on arg (0..4):
 * LCOD hint triggers and the SC21 ending; jump table @ 0x100054A5.
 * ---------------------------------------------------------------------- */
static void event6(i32 arg) {
    i32 stance_was_on = 0;   /* [ebp-4]: set in the SC21 ending, never read */

    switch (arg) {
    case EV6_LCOD34:
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD34", P_ARKHAN2, "SG00CON01", NULL);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD35:
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD35", P_ARKHAN2, "SG00CON01", NULL);
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->animation("SG00CON01");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD36:
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD36", P_ARKHAN3, "SG00CON01", "SG00CON01");
            CB->animation("Sar_50");
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_LCOD37:
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->select_pile(0, 2);
            CB->bloque(BLOQUE_ALL);
            lieu_action_register("LCOD37", P_ARKHAN3, "SG00CON01", NULL);
            CB->divers_empile(DV_WORLD_TICK, 0, 0);
            CB->bloque(BLOQUE_NONE);
        }
        break;
    case EV6_SC21_ENDING:
        if (ST(ST_HERO) == P_ARKHAN3) {
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
 * Per-frame body — sub_100019B2 (thunk 0x100010B4), level-50 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue (0x100025FD).
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 0;
        CB->change_camera(99, CAM_AUTO);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->divers(DV_LOOK_AT_PLAYER, P_BANDOR, 1);
        CB->bloque(BLOQUE_NONE);
    }
    if (ST(ST_LOCKED_CAMERA) == 2 && CB->distance(1, 2) < 300 && ST(0x80C) == 0 && ST(0x100) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        ST(0x100) = 666;
        CB->animation("SP07MAR29");
        CB->animation("SP83GAR01_500");
        CB->animation("P83ATT01");
        CB->animation("P07ATT02");
    }
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->joindre(ST(ST_HERO), 0, 0, 0x317A, -1);
        CB->change_camera(4, CAM_LOCKED);
        CB->change_camera(4, CAM_AUTO);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L12D108", P_ARKHAN2);
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L12D109", P_ARKHAN3);
        }
        CB->bloque(BLOQUE_NONE);
        return;
    }
    if ((CB->divers(DV_IN_ZONE, ST(ST_HERO), 2) || CB->divers(DV_IN_ZONE, ST(ST_HERO), 3)) && ST(V_CRI_KEY) == 1 && ST(V_CRI_USED) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_COMBAT_STANCE) == 1 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD && (CB->direction_perso(P_BANDOR) || CB->direction_perso(P_KHORAD))) {
        ST(V_CRI_USED) = 1;
        if (ST(0x74) != 99) {
            ST(0x74) = 83;
        }
        CB->bloque(BLOQUE_ALL);
        CB->evenement(33, 1000);
        return;
    }
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        if (ST(0x74) == 83) {
            ST(0x74) = 99;
            ST(V_DLG_RANDOM) = CB->random(2);
            if (ST(V_DLG_RANDOM) == 0) {
                CB->dialogue("L12D128", P_BANDOR);
            }
            if (ST(V_DLG_RANDOM) == 1) {
                CB->dialogue("L12D129", P_KHORAD);
            }
            CB->animation("Sar_50");
            CB->modify_var(&ST(V_CRI_USED), 0);
            CB->bloque(BLOQUE_NONE);
            return;
        }
    }
    if (ST(0x74) == 99) {
        ST(0x74) = 83;
    }
    if (ST(V_EVENT_34) == 1) {
        ST(V_EVENT_34) = 0;
        CB->play_anim(1, 0);
        CB->animation("Sar_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->cinematique("SC26_500");
        }
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->cinematique("SC27_500");
        }
        ST(ST_LEVEL) = 12;
        CB->bloque(BLOQUE_NONE);
        CB->change_piece(11, -1);
    }
    if (ST(ST_VIE(P_BANDOR)) == VIE_DEAD) {
        if (ST(V_ENGAGED_PERSONA) == P_BANDOR) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
        if (ST(V_ENGAGED_PERSONA) == 0x30F) {
            ST(V_ENGAGED_PERSONA) = P_KHORAD;
        }
    }
    if (ST(ST_VIE(P_KHORAD)) == VIE_DEAD) {
        if (ST(V_ENGAGED_PERSONA) == P_KHORAD) {
            ST(V_ENGAGED_PERSONA) = 0;
        }
        if (ST(V_ENGAGED_PERSONA) == 0x30F) {
            ST(V_ENGAGED_PERSONA) = P_BANDOR;
        }
    }
    if (CB->divers(DV_WAS_HIT, P_KHORAD, 0) && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        CB->divers_empile(DV_WAS_HIT, P_KHORAD, 1);
        CB->select_pile(1, 2);
        ST(ST_VIE(P_KHORAD)) = VIE_DEAD;
        CB->combat(P_KHORAD, -1);
        CB->stop_joindre(-P_KHORAD);
        CB->animation("SP83IMP51");
        CB->animation("P83CAD");
        CB->pos_perso(P_KHORAD, 0, 0, 0, 2, "P83CAD", 3, 0);
        if (ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) != P_BANDOR) {
            ST(V_ENGAGED_PERSONA) = P_BANDOR;
            CB->select_pile(2, 2);
            CB->combat(P_BANDOR, 0);
        }
        CB->select_pile(0, 1);
    }
    if (CB->divers(DV_WAS_HIT, P_BANDOR, 0) && ST(ST_VIE(P_BANDOR)) != VIE_DEAD) {
        CB->divers_empile(DV_WAS_HIT, P_BANDOR, 1);
        CB->select_pile(2, 2);
        ST(ST_VIE(P_BANDOR)) = VIE_DEAD;
        CB->combat(P_BANDOR, -1);
        CB->stop_joindre(-P_BANDOR);
        CB->animation("SP07IMP51");
        CB->animation("P07CAD");
        CB->pos_perso(P_BANDOR, 0, 0, 0, 2, "P07CAD", 2, 0);
        if (ST(ST_VIE(P_KHORAD)) != VIE_DEAD && ST(V_ENGAGED_PERSONA) != P_KHORAD) {
            ST(V_ENGAGED_PERSONA) = P_KHORAD;
            CB->select_pile(1, 2);
            CB->combat(P_KHORAD, 0);
        }
        CB->select_pile(0, 1);
    }
    if (CB->distance(1, 2) < 250 && ST(ST_COMBAT_STANCE) == 1 && ST(0x6C) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        ST(0x6C) = 1;
        CB->select_pile(2, 2);
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        CB->joindre_perm(P_BANDOR, ST(ST_HERO));
        CB->stop_joindre(P_BANDOR);
        lieu_action_register("L12D112", P_BANDOR, "SP07GAR01", "SP07ATT02_10");
        CB->bloque(BLOQUE_NONE);
        CB->animation("P07ATT02_1000");
        CB->animation("P07ATT01_10");
        CB->select_pile(1, 2);
        CB->select_pile(1, 1);
        CB->joindre_perm(P_KHORAD, ST(ST_HERO));
        CB->stop_joindre(P_KHORAD);
        CB->animation("SP83GAR01_3000");
        CB->animation("P83ATT02_2000");
        CB->animation("P83DIA50_10");
        CB->select_pile(0, 1);
        return;
    }
    if (ST(0x6C) == 1 && ST(ST_COMBAT_STANCE) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(V_ENGAGED_PERSONA) == 0 && ST(ST_VIE(P_BANDOR)) != VIE_DEAD && ST(ST_VIE(P_KHORAD)) != VIE_DEAD) {
        ST(0x6C) = 0;
        CB->change_camera(-4, CAM_LOCKED);
        CB->select_pile(2, 2);
        CB->select_pile(2, 1);
        CB->bloque(BLOQUE_ALL);
        CB->joindre_perm(P_BANDOR, ST(ST_HERO));
        CB->stop_joindre(P_BANDOR);
        lieu_action_register("L12D113", P_BANDOR, "SP07DIA51", "SP07ARR01_10");
        CB->bloque(BLOQUE_NONE);
        CB->change_camera(0, CAM_AUTO);
        CB->select_pile(1, 2);
        CB->select_pile(1, 1);
        CB->joindre_perm(P_KHORAD, ST(ST_HERO));
        CB->stop_joindre(P_KHORAD);
        CB->animation("SP83ARR01");
        CB->select_pile(0, 1);
        return;
    }
    if (ST(ST_LOCKED_CAMERA) == 3 && ST(0x4BC) == 1 && ST(0x78) == 1) {
        CB->pause_anim(0, 7, 0);
        ST(0x78) = 0;
    }
    if (ST(ST_LOCKED_CAMERA) != 3 && ST(0x78) == 0) {
        ST(0x78) = 1;
    }
    if (ST(V_GUARD_FIGHT_DONE) == 2 && ST(ST_LOCKED_CAMERA) == 0 && ST(0x80) == 0) {
        CB->select_pile(1, 1);
        CB->pause_anim(1, 1, 0);
        ST(0x80) = 1;
        CB->select_pile(0, 1);
    }
    if (ST(ST_LOCKED_CAMERA) != 0 && ST(0x80) == 1) {
        ST(0x80) = 0;
    }
    if (ST(ST_HERO) == 4 && ST(0x7C) == 0) {
        ST(0x7C) = 1;
        CB->evenement(EV_END_GAME, 3000);
    }
    if (ST(V_END_GAME_REQ) == 1) {
        ST(V_END_GAME_REQ) = 0;
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22_500");
        }
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23_500");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
        return;
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x1001023A).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_1002250C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu50_trig.c. There is no death
 * branch in this version.
 *   arg = reload flag (non-zero => skip arming).
 * ---------------------------------------------------------------------- */
static void init(i32 reload_flag) {
    if (reload_flag == 0)
        l50_trig00();   /* 0x100105C3: call sub_1000106E -> sub_10001200 */
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1, 0x10010616): drain pending sound, run body.
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046 -> 0x10010220).
 * Jump table @ 0x10010693, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu50_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu50: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        /* DLL .data restarts on every room load */
        for (i32 i = 0; i < 4; i++)
            g_float_accum[i] = 1.0f;
        g_ob24[0] = '\0';
        g_ob77[0] = '\0';
        init(arg);
        break;
    case LIEU_OP_EVENT:  l50_trigger(arg);             break;  /* dword_100223A0[arg]() */
    case LIEU_OP_CALL4:  event4(arg);                  break;
    case LIEU_OP_CALL5:  event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
