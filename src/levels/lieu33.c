/*
 * lieu33.c - Native hand-port of LIEU33.DLL (level-33 scripting): the
 * @FonctionEditor@16 entry, INIT, the per-frame tick, the tick body and the
 * level's own event4 / event5 / event6. The 42 zone trigger handlers live in
 * lieu33_trig.c (reached through l33_trigger, see lieu33_int.h).
 *
 * Ported directly from disassembly (ImageBase 0x10000000). Each function
 * carries its original address. LIEU33 is an older build of the level-10
 * script (same rooms and dialogue codes as LIEU10) linked with a different
 * layout: the entry is at 0x10014240, the state pointer global is
 * dword_10025A08 and the trigger table is dword_10025930[42].
 *
 * Its event4 (sub_10001200), event5 (sub_10001F87) and event6
 * (sub_1000523D) are an older script version than the lieu_common.c ones
 * and are ported here; lieu_action_register is the shared one. The hero
 * pre-scene sub_10014896 lacks the `n != 6` test of lieu_pre_scene, so it is
 * ported here too (l33_pre_scene), with its own copy of pre90_scene.
 * The DLL .data the level code touches: the four event4 float accumulators
 * (dword_1001FA30..3C) and the two event5 sprintf buffers
 * (dword_10025A60 / dword_10025A40); all are reset at INIT.
 */

#include <stdio.h>
#include <stdlib.h>

#include "lieu33_int.h"

/* event4 float accumulators, dword_1001FA30..3C (initially 1.0f). */
static float g_float_accum[4] = {1.0f, 1.0f, 1.0f, 1.0f};

/* event5 sprintf buffers: dword_10025A60 ("OB24%c") and dword_10025A40
 * ("OB77%c"). */
static c8 g_ob24[32];
static c8 g_ob77[32];

/* The DLL is loaded afresh for every room: its globals restart. Called by
 * INIT, and by tools/lieu_difftest_harness.c before every case. */
void l33_reset_dll_globals(void) {
    for (i32 i = 0; i < 4; i++)
        g_float_accum[i] = 1.0f;
    g_ob24[0] = '\0';
    g_ob77[0] = '\0';
}

/* -------------------------------------------------------------------------
 * sub_10005E80 (thunk 0x1000103C) - Rylsadhar/Lory pre-scene: __fastcall
 * (ecx = a, edx = b); the caller also pushes 3 stack args (callee `ret 0xC`)
 * that it never reads. a = the hero side (SG%02dPRE80/82), b = the speaker.
 * ---------------------------------------------------------------------- */
static void pre_scene_10(i32 a, i32 b) {
    c8 buf[256];

    snprintf(buf, sizeof(buf), "SG%02dPRE80", a);
    CB->animation(buf);
    snprintf(buf, sizeof(buf), "G%02dPRE81", a);    /* sic: no leading 'S' */
    CB->animation(buf);
    snprintf(buf, sizeof(buf), "SP%02dPRE61", b);
    CB->animation(buf);
    snprintf(buf, sizeof(buf), "P%02d_65535", b);
    CB->animation(buf);
    snprintf(buf, sizeof(buf), "SG%02dPRE82", a);
    CB->animation(buf);
    snprintf(buf, sizeof(buf), "P%02darr01_10", a);
    CB->animation(buf);
    snprintf(buf, sizeof(buf), "SP%02darr01_10", b);
    CB->animation(buf);
}

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
 * sub_10014896 (thunk sub_100010AF): the hero greets persona n (search /
 * handshake sequence). __fastcall(ecx=hero, edx=n) + stdcall stack (a1
 * unused, a2, a3); ret 0Ch. a2/a3 go to Distance. Same as lieu_pre_scene
 * except that the Sp%02dMAR30 animation does not test n != 6.
 * ---------------------------------------------------------------------- */
void l33_pre_scene(i32 hero, i32 n, i32 a1, i32 a2, i32 a3) {
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
 * event4 - sub_10001200 (thunk 0x100010F5, op 4). switch(arg) over the
 * script-event ids (byte index table -> jump table @ 0x10001E5F); mostly
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
        /* 0x100017C4: the jne lands on case 31's code (no jmp to the exit) */
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
 * event5 - sub_10001F87 (thunk 0x10001005, op 5): ecx = arg (item id),
 * edx = phase. arg-1 indexes a byte table -> jump table @ 0x10005105. The
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
        /* 0x3612/0x375F: the jne lands on the next case's code (no jmp to the exit) */
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
 * event6 - sub_1000523D (thunk 0x10001046, op 6). Switch on arg (0..4):
 * LCOD hint triggers and the SC21 ending; jump table @ 0x100054A5.
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
 * Per-frame body - sub_100083B1 (thunk 0x100010B9), level-33 specific.
 * Ported faithfully from disassembly (labels in comments = original
 * addresses). `return` == original `jmp` to the epilogue.
 * ---------------------------------------------------------------------- */
static void tick_body(void) {
    /* 0x83C3 */
    if (ST(ST_LOCKED_CAMERA) == 9 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 3400) {
        CB->change_camera(3, CAM_AUTO);
    }
    /* 0x8408 */
    if (ST(ST_HERO) == 4 && ST(V_DEATH_EVENT_SENT) == 0) {
        CB->bloque(BLOQUE_ALL);
        ST(V_DEATH_EVENT_SENT) = 1;
        /* 0x844B */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN0, 0)) {
            CB->divers(DV_HP, P_ARKHAN0, 0);
        }
        /* 0x8474 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN1, 0)) {
            CB->divers(DV_HP, P_ARKHAN1, 0);
        }
        /* 0x84A0 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0x84CC */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        /* 0x84F0 */
        if (ST(V_GUARD_FIGHT_DONE) == 0) {
            CB->cinematique("SC22");
        }
        /* 0x850D */
        if (ST(V_GUARD_FIGHT_DONE) != 0) {
            CB->cinematique("SC23");
        }
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
    /* 0x8536 */
    if (ST(0x6C) == 10 && CB->distance(30, 10) < 200 && ST(0x12C) == 10) {
        ST(0x6C) = 1;
    }
    /* 0x8589 */
    if (CB->divers(DV_WAS_HIT, P_KALHI13, 0) && ST(ST_VIE(P_KALHI13)) != VIE_DEAD && ST(0xE0) == 0) {
        ST(0xE58) = 10;
        ST(ST_SCRIPT_BUSY) = 0;
        ST(ST_VIE(P_KALHI13)) = VIE_DEAD;
        CB->select_pile(1, 2);
        CB->combat(P_KALHI13, -1);
        CB->stop_joindre(-P_KALHI13);
        CB->animation("SP13IMP52");
        CB->animation("P13CAD");
        CB->pos_perso(P_KALHI13, 0, 0, 0, 2, "P13CAD", 0x1F, 0);
        CB->select_pile(0, 1);
    }
    /* 0x865B */
    if (CB->divers(DV_WAS_HIT, P_KALHI13, 0) && ST(ST_VIE(P_KALHI13)) != VIE_DEAD && ST(0xE0) == 4 && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) > 450) {
        CB->bloque(BLOQUE_ALL);
        ST(0xE9C) = 10;
        ST(0xE0) = 5;
        CB->stop_joindre(-P_KALHI13);
        CB->divers_empile(DV_WAS_HIT, P_KALHI13, 1);
        lieu_action_register("L10D127", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI13);
        CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L10D128", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
        CB->joindre(-P_KALHI13, 0, 0, 0x4650, -1);
        CB->animation("SP13ACT20");
        CB->animation("P13_65535");
        CB->bruitage("L10B01", 4310, 1040);
        CB->divers_empile(DV_BIG_ANIM, 2, 0);
        CB->divers_empile(DV_MUSIC, 1, 4);
        ST(ST_LEVEL) = 10;
        CB->bloque(BLOQUE_NONE);
        ST(0x12C) = ST(0x6C);
        CB->change_piece(14, -1);
    }
    /* 0x880B */
    if (CB->divers(DV_WAS_HIT, P_KALHI33, 0) && ST(ST_VIE(P_KALHI33)) != VIE_DEAD && ST(0xE0) == 0) {
        ST(0xE58) = 10;
        ST(ST_SCRIPT_BUSY) = 0;
        ST(ST_VIE(P_KALHI33)) = VIE_DEAD;
        CB->select_pile(1, 2);
        CB->combat(P_KALHI33, -1);
        CB->stop_joindre(-P_KALHI33);
        CB->animation("SP33IMP52");
        CB->animation("P33CAD");
        CB->pos_perso(P_KALHI33, 0, 0, 0, 2, "P33CAD", 0x20, 0);
        CB->select_pile(0, 1);
    }
    /* 0x88DD */
    if (CB->divers(DV_WAS_HIT, P_KALHI33, 0) && ST(ST_VIE(P_KALHI33)) != VIE_DEAD && ST(0xE0) == 4 && CB->divers(DV_DESC_POS, ST(ST_HERO), 1) > 450) {
        CB->bloque(BLOQUE_ALL);
        ST(0xE9C) = 10;
        ST(0xE0) = 5;
        CB->stop_joindre(-P_KALHI33);
        CB->divers_empile(DV_WAS_HIT, P_KALHI33, 1);
        lieu_action_register("L10D127", P_KALHI33, "SP33DIA60", "SP33ARR01_10");
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI33);
        CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        lieu_action_register("L10D57", P_KALHI33, "SP33DIA55", "SP33ARR01_10");
        CB->joindre(-P_KALHI33, 0, 0, 0x4650, -1);
        CB->animation("SP33ACT20");
        CB->animation("P33_65535");
        CB->bruitage("L10B01", 4310, 1040);
        CB->divers_empile(DV_BIG_ANIM, 2, 0);
        CB->divers_empile(DV_MUSIC, 1, 4);
        ST(ST_LEVEL) = 10;
        CB->bloque(BLOQUE_NONE);
        CB->change_piece(14, -1);
        ST(0x12C) = ST(0x6C);
    }
    /* 0x8A8D */
    if (CB->divers(DV_WAS_HIT, P_DELIA, 0) && ST(ST_VIE(P_DELIA)) != VIE_DEAD) {
        ST(ST_SCRIPT_BUSY) = 0;
        ST(ST_VIE(P_DELIA)) = VIE_DEAD;
        ST(V_DELIA_KILLED) = 10;
        ST(V_DELIA_PHASE) = 20;
        CB->select_pile(1, 2);
        CB->combat(P_DELIA, -1);
        CB->stop_joindre(-P_DELIA);
        CB->animation("SP10IMP51");
        CB->animation("P10CAD");
        CB->pos_perso(P_DELIA, 0, 0, 0, 2, "P10CAD", 0x1E, 0);
        CB->select_pile(0, 1);
    }
    /* 0x8B51 */
    if (ST(V_ENGAGED_PERSONA) == P_GUARDIAN15 && ST(ST_VIE(P_GUARDIAN15)) == VIE_DEAD) {
        ST(0xEF4) = 2;
        ST(V_ENGAGED_PERSONA) = 0;
        /* 0x8B9B */
        if (CB->divers(DV_DESC_POS, P_GUARDIAN15, 1) > 315) {
            ST(0x100) = 1;
            CB->bloque(BLOQUE_ALL);
            ST(ST_SCRIPT_BUSY) = 1;
            /* 0x8BD3 */
            if (ST(ST_LOCKED_CAMERA) == 3) {
                CB->change_camera(7, CAM_LOCKED);
            }
            /* 0x8BF5 */
            if (ST(ST_LOCKED_CAMERA) == 7) {
                CB->change_camera(3, CAM_LOCKED);
            }
            CB->pos_perso(P_GUARDIAN15, CB->divers(DV_DESC_POS, P_GUARDIAN15, 0), 310, CB->divers(DV_DESC_POS, P_GUARDIAN15, 2), 2, "P55cad", 0x21, 0);
            CB->print_msg("j'ai tu\xe9 le garde", 0, 240);   /* debug text in the DLL .data */
            /* 0x8C6B */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI13);
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                /* 0x8CB7 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D51", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0x8CE2 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D52", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D53", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                /* 0x8D26 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D54", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0x8D51 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D55", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D56", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                CB->joindre(-P_KALHI13, 0, 0, 0x4650, -1);
            }
            /* 0x8DAB */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI33);
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                /* 0x8DF7 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D51", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0x8E22 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D52", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D53", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
                /* 0x8E66 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D54", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0x8E91 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D55", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D56", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
                CB->joindre(-P_KALHI33, 0, 0, 0x4650, -1);
            }
            /* 0x8EEB */
            if (ST(ST_LOCKED_CAMERA) == 7) {
                CB->change_camera(3, CAM_AUTO);
            }
            /* 0x8F0D */
            if (ST(ST_LOCKED_CAMERA) == 3) {
                CB->change_camera(7, CAM_AUTO);
            }
            CB->animation("sar_10");
            CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
            CB->bloque(BLOQUE_NONE);
        }
    }
    /* 0x8F53 */
    if ((ST(ST_VIE(P_KALHI13)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_KALHI13) || (ST(ST_VIE(P_KALHI33)) == VIE_DEAD && ST(V_ENGAGED_PERSONA) == P_KALHI33)) {
        ST(0xE58) = 10;
        ST(ST_SCRIPT_BUSY) = 0;
        ST(V_ENGAGED_PERSONA) = 0;
    }
    /* 0x8FBC */
    if (ST(V_EVENT_0) == 1) {
        ST(V_EVENT_0) = 2;
        CB->pause_anim(0, 3, -1);
        CB->play_anim(1, 0);
        CB->pause_anim(2, 3, -1);
        CB->play_anim(3, 0);
        CB->pause_anim(4, 3, -1);
        CB->play_anim(5, 0);
        CB->pause_anim(6, 3, -1);
        CB->play_anim(7, 0);
        CB->pause_anim(8, 0, -1);
        CB->play_anim(9, 0);
        CB->joindre(-P_SUNSEER1, 4647, -3500, 0x7530, -1);
        CB->joindre(-P_GUARDIAN15, 5070, -1700, 0x7530, -1);
        CB->animation("SG00DIA07");
        /* 0x90B5 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->dialogue("L10D80A", P_ARKHAN2);
            CB->dialogue("L10D80B", P_ARKHAN2);
        }
        /* 0x90E7 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->dialogue("L10D81A", P_ARKHAN3);
            CB->dialogue("L10D81B", P_ARKHAN3);
        }
        CB->animation("Sar_10");
        CB->modify_var(&ST(0xD4), 1);
    }
    /* 0x913B */
    if (ST(0xEC8) == 1 && (ST(0xAF4) >= 3 || (ST(0xAF4) < 3 && (CB->distance(12, 1) < 100 || CB->distance(12, 2) < 100 || ST(0x64) == 1)))) {
        ST(0xEC8) = 2;
        CB->select_pile(2, 1);
        CB->stop_joindre(-P_ZED84);
        CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
        CB->dialogue("L10D01A", P_ZED84);
        CB->evenement(33, 2000);
        CB->select_pile(0, 1);
    }
    /* 0x9226 */
    if (ST(V_EVENT_33) == 1) {
        ST(V_EVENT_33) = 0;
        ST(0xEC8) = 3;
        CB->bloque(BLOQUE_ALL);
        CB->joindre(-ST(ST_HERO), 0, 0, 0, P_ZED84);
        /* 0x927E */
        if (ST(ST_SPECIAL_MODE) == 1) {
            lieu_action_register("L10D96", P_ARKHAN1, "SG01DIA08", NULL);
        }
        /* 0x92A6 */
        if (ST(ST_SPECIAL_MODE) == 0) {
            lieu_action_register("L10D97", P_ARKHAN1, "SG01DIA08", NULL);
        }
        /* 0x92CE */
        if (ST(ST_LOCKED_CAMERA) != 6) {
            CB->change_camera(6, CAM_LOCKED);
        }
        CB->dialogue("L10D01B", P_ZED84);
        CB->select_pile(2, 1);
        CB->joindre(-P_ZED84, 0, 0, 0, ST(ST_HERO));
        CB->select_pile(0, 1);
        /* 0x9341 */
        if (CB->distance(12, 10) > 125) {
            CB->animation("SP84ACT01");
            CB->evenement(2, 1600);
        } else {
            CB->animation("SP84ACT03");
            CB->evenement(1, 200);
        }
        CB->select_pile(0, 1);
    }
    /* 0x939B */
    if (ST(V_EVENT_2) == 1) {
        ST(V_EVENT_2) = 0;
        CB->change_camera(6, CAM_AUTO);
        CB->animation("SG01ATT07");
        CB->animation("P84ARR02_100");
        lieu_action_register("L10D30", P_ZED84, "SP84DIA59", "SP84ARR01_1000");
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
    }
    /* 0x942A */
    if (ST(V_EVENT_1) == 1) {
        ST(V_EVENT_1) = 0;
        CB->change_camera(6, CAM_AUTO);
        CB->animation("SP01IMP51");
        CB->animation("P84ACT02");
        CB->animation("P84ARR01_100");
        lieu_action_register("L10D30", P_ZED84, "SP84DIA59", "SP84ARR01_1000");
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
    }
    /* 0x94C1 */
    if (ST(0x70) == 1 && !CB->on_joindre(P_RYLSADHAR20)) {
        ST(0xAF4) = 1;
        ST(0x70) = 0;
        CB->joindre(-P_RYLSADHAR20, 0, 0, 0, P_LORY);
    }
    /* 0x951D */
    if (CB->distance(10, 19) < 800 && ST(0xAF4) == 1 && ST(ST_HERO) == P_ARKHAN1) {
        ST(0xAF4) = 2;
        CB->bloque(BLOQUE_ALL);
        ST(0xE78) = 1;
        CB->change_camera(8, CAM_LOCKED);
        lieu_action_register("L10D05A", P_RYLSADHAR20, "SP20DIA54", "SP20DIA53");
        lieu_action_register("L10D05B", P_RYLSADHAR20, "SP20DIA53", "SP20DIA56");
        lieu_action_register("L10D05C", P_RYLSADHAR20, "SP20DIA54", "SP20DIA56");
        lieu_action_register("L10D05D", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01");
        lieu_action_register("L10D05E", P_RYLSADHAR20, "SP20DIA56", "SP20ARR01");
        lieu_action_register("L10D06", P_LORY, "SP15DIA53", "SP15ARR01");
        lieu_action_register("L10D07A", P_RYLSADHAR20, "SP20DIA60", "SP20DIA55");
        lieu_action_register("L10D07B", P_RYLSADHAR20, "SP20DIA55", "SP20ARR01");
        lieu_action_register("L10D08", P_LORY, "SP15DIA54", "SP15ARR01_10");
        lieu_action_register("L10D09", P_RYLSADHAR20, "SP20DIA50", "SP20ARR01_10");
        pre_scene_10(P_RYLSADHAR20, P_LORY);   /* the 3 stacked args are never read */
        CB->animation("P20ARR01_10");
        CB->animation("SP15ATT70");
        CB->dialogue("L10D10", P_LORY);
        CB->animation("P15ARR01_100");
        CB->animation("SP20ATT70");
        CB->dialogue("L10D11", P_RYLSADHAR20);
        CB->animation("P20ARR01_10");
        CB->animation("P15mar17");
        CB->animation("P15_65535");
        CB->joindre(-P_LORY, 4647, -3520, 0x7530, -1);
        CB->joindre(-P_RYLSADHAR20, 5320, -2640, 0x7530, -1);
        CB->change_camera(4, CAM_AUTO);
        CB->dialogue("L10D12A", P_ARKHAN1);
        lieu_action_register("L10D12B", P_ARKHAN1, "SG00DIA05", "Sar_10");
        CB->evenement(35, 1500);
    }
    /* 0x9789 */
    if (ST(V_EVENT_35) == 1) {
        ST(V_EVENT_35) = 0;
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x97B2 */
    if (ST(0xE7C) == 0 && CB->distance(26, 20) <= 80) {
        ST(0xE7C) = 1;
        CB->pos_perso(P_LORY, 4647, -3520, 0x7530, 4, "", 0x1A, 0);
        ST(0xA8) = ST(ST_GAME_TIME);
        CB->retire_zone(20);
        CB->retire_zone(26);
    }
    /* 0x983E */
    if (ST(0xE78) == 1 && CB->distance(27, 21) <= 80 && ST(0xF8) == 0) {
        ST(0xF8) = 1;
        CB->pos_perso(P_RYLSADHAR20, 5320, -2640, 0xAFC8, 4, "", 0x1B, 0);
        ST(0xA0) = ST(ST_GAME_TIME);
        CB->retire_zone(21);
        CB->retire_zone(27);
    }
    /* 0x98DC */
    if (ST(0xD4) == 1 && CB->divers(DV_DESC_POS, P_SUNSEER1, 1) < -3350) {
        ST(0xD4) = 2;
        CB->bloque(BLOQUE_NONE);
    }
    /* 0x9922 */
    if (ST(0xD4) == 2 && !CB->on_joindre(P_SUNSEER1)) {
        CB->free_anim(-10);
        ST(0xD4) = 3;
        CB->pos_perso(P_SUNSEER1, 4647, -3520, 0x7530, 4, "", 0x22, 0);
        CB->stop_joindre(-P_GUARDIAN15);
        CB->pos_perso(P_GUARDIAN15, 4480, 600, 0x88B8, 1, "P55ARR04", 0x21, 0);
    }
    /* 0x99B6 */
    if (ST(0xE7C) == 1 && ST(0xAF4) == 2 && ST(ST_HERO) == P_ARKHAN1) {
        ST(0xAF4) = 3;
        ST(0x7C) = 1;
        CB->ajoute_zone(8, 4540, 304, 80, 80, 0);
        CB->pos_perso(P_ZED84, 2300, -1550, 0xC350, 1, "P84ARR01", 0xC, 0);
        CB->link_objet(P_ZED84, "OB68", 0x1D, 1);
        CB->joindre(P_ZED84, 4540, 304, 0, -1);
    }
    /* 0x9A74 */
    if (ST(0xAF4) >= 3 && ST(0xEC8) == 0 && ST(ST_HERO) == P_ARKHAN1) {
        /* 0x9AA7 */
        if (ST(0x7C) == 1 && CB->distance(12, 10) < 150 && CB->divers(DV_ANGLE_TO, P_ZED84, ST(ST_HERO)) < 4000) {
            ST(0x7C) = 2;
            CB->stop_joindre(-P_ZED84);
            return;
        }
        /* 0x9B14 */
        if (ST(0x7C) == 2 && (CB->distance(12, 10) >= 200 || CB->divers(DV_ANGLE_TO, P_ZED84, ST(ST_HERO)) >= 6000)) {
            ST(0x7C) = 1;
            CB->stop_joindre(-P_ZED84);
            CB->joindre(-P_ZED84, 4540, 304, 0, -1);
            return;
        }
        /* 0x9B9A */
        if (ST(0x7C) != 0 && !CB->on_joindre(P_ZED84) && CB->distance(12, 8) <= 100) {
            ST(0x7C) = 0;
            CB->retire_zone(8);
            CB->stop_joindre(-P_ZED84);
            CB->animation("SP84ARR01_10");
        }
    }
    /* 0x9C05 */
    if (ST(ST_HERO) == P_ARKHAN1 && ST(V_EVENT_34) == 0) {
        /* 0x9C29 */
        if (ST(0x90) == 0 && (ST(ST_LOCKED_CAMERA) == 3 || ST(ST_LOCKED_CAMERA) == 7)) {
            ST(0x90) = 1;
            CB->evenement(34, 2000);
        }
        /* 0x9C7E */
        if (ST(0x90) == 1 && ST(ST_LOCKED_CAMERA) != 3 && ST(ST_LOCKED_CAMERA) != 7) {
            ST(0x90) = 0;
            CB->evenement(34, -1);
        }
    }
    /* 0x9CD3 */
    if (ST(ST_HERO) == P_ARKHAN1 && ST(V_EVENT_34) == 1 && ST(ST_SCRIPT_BUSY) == 0) {
        ST(V_EVENT_34) = 0;
        ST(V_DLG_RANDOM) = CB->random(9);
        /* 0x9D2C */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->dialogue("L10D29A", -1);
        }
        /* 0x9D4E */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->dialogue("L10D29B", -1);
        }
        /* 0x9D70 */
        if (ST(V_DLG_RANDOM) == 2) {
            CB->dialogue("L10D29C", -1);
        }
        /* 0x9D92 */
        if (ST(V_DLG_RANDOM) == 3) {
            CB->dialogue("L10D29D", -1);
        }
        /* 0x9DB4 */
        if (ST(V_DLG_RANDOM) == 4) {
            CB->dialogue("L10D29E", -1);
        }
        /* 0x9DD6 */
        if (ST(V_DLG_RANDOM) == 5) {
            CB->dialogue("L10D29F", -1);
        }
        /* 0x9DF8 */
        if (ST(V_DLG_RANDOM) == 6) {
            CB->dialogue("L10D29G", -1);
        }
        /* 0x9E1A */
        if (ST(V_DLG_RANDOM) == 7) {
            CB->dialogue("L10D29H", -1);
        }
        /* 0x9E3C */
        if (ST(V_DLG_RANDOM) == 8) {
            CB->dialogue("L10D29I", -1);
        }
        ST(V_DLG_RANDOM) = CB->random(3);
        /* 0x9E75 */
        if (ST(V_DLG_RANDOM) == 0) {
            CB->evenement(34, 2000);
        }
        /* 0x9E97 */
        if (ST(V_DLG_RANDOM) == 1) {
            CB->evenement(34, 4000);
        }
        /* 0x9EB9 */
        if (ST(V_DLG_RANDOM) == 2) {
            CB->evenement(34, 6000);
        }
    }
    /* 0x9EDB */
    if (((ST(0xAF8) == 1 && ST(0xE58) == 1) || (ST(0xE58) == 10 && CB->distance(10, 33) < 500 && ST(ST_LOCKED_CAMERA) == 3)) && ST(0xEB8) != 2) {
        CB->select_pile(0, 2);
        CB->bloque(BLOQUE_ALL);
        /* 0x9F68 */
        if (ST(0xE58) == 1 || ST(0xE58) == 10) {
            ST(0xE58) = 2;
        }
        /* 0x9F9B */
        if (ST(0xB40) == 0 || ST(0xBD0) == 0) {
            ST(0xB40) = 99;
            ST(0xBD0) = 99;
        }
        ST(0x4C) = 1;
        CB->joindre_perm(-P_GUARDIAN15, ST(ST_HERO));
        CB->stop_joindre(P_GUARDIAN15);
        CB->animation("SP55GAR05_10");
        /* 0xA015 */
        if (ST(0x7F8) == 1) {
            CB->dialogue("L10D35", P_GUARDIAN15);
        }
        /* 0xA037 */
        if (ST(0x7F8) == 0) {
            ST(0x7F8) = 1;
            CB->dialogue("L10D34", P_GUARDIAN15);
        }
        CB->animation("SP55GAR05_10");
        CB->animation("P55_65535");
        CB->evenement(38, 250);
        CB->animation("P55ATA25");
        CB->animation("P55GAR05");
        CB->joindre(P_GUARDIAN15, -1, -1, -1, ST(ST_HERO));
        return;
    }
    /* 0xA0C6 */
    if (((ST(0x4C) == 1 && ST(ST_HERO) < 4) || (ST(0xAF8) == 1 && (ST(0xE58) == 2 || (ST(0xB40) == 99 && ST(0xBD0) == 99)) && ST(ST_HERO) < 4)) && ST(V_EVENT_38) == 1) {
        ST(0x4C) = 0;
        CB->stop_joindre(-P_GUARDIAN15);
        CB->select_pile(2, 2);
        ST(V_EVENT_38) = 0;
        CB->divers_empile(DV_BONE_SOUND, ST(ST_HERO), 0);
        /* 0xA1A4 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02IMP55");
        }
        /* 0xA1C1 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03IMP55");
        }
        CB->divers_empile(DV_HP, ST(ST_HERO), 0);
        CB->modify_var(&ST(ST_HERO), 4);
        CB->select_pile(0, 1);
        return;
    }
    /* 0xA21F */
    if (ST(ST_VIE(P_KALHI13)) == 1 && ST(0xE9C) == 7 && ST(0x94) < 5) {
        ST(0xEF0) = 1;
        /* 0xA26B */
        if (CB->distance(31, 10) < 120 && ST(0x80) == 0 && ST(0x78) == 0) {
            /* 0xA2A7 */
            if (CB->divers(DV_RUN, P_KALHI13, 1) == 1) {
                CB->divers(DV_RUN, P_KALHI13, 0);
            }
            ST(0x78) = 1;
            ST(0x74) = 1;
            CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
        }
        /* 0xA309 */
        if (CB->distance(31, 10) < 300 && ST(0x80) == 1) {
            ST(0x80) = 2;
            CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
            CB->modify_var(&ST(0x94), 1);
        }
        /* 0xA36D */
        if (ST(0x6C) == 1 && (ST(0x38) % 20) == 0) {
            /* 0xA3A0 */
            if (CB->distance(31, 10) >= 180) {
                ST(0x78) = 0;
                CB->divers(DV_RUN, P_KALHI13, 0);
                CB->joindre(-P_KALHI13, -1, -1, -1, ST(ST_HERO));
            }
            /* 0xA3F9 */
            if (CB->distance(31, 10) >= 300) {
                ST(0x78) = 0;
                CB->divers(DV_RUN, P_KALHI13, 1);
                CB->joindre(-P_KALHI13, -1, -1, -1, ST(ST_HERO));
            }
        }
    }
    /* 0xA449 */
    if (ST(0xF4) == 1) {
        ST(0xF4) = 2;
        CB->divers_empile(DV_SCREEN, 5, 0);
        CB->pos_perso(P_KALHI13, -1, -1, -1, 4, "", 0x1F, 0);
        ST(ST_VIE(P_KALHI13)) = 4;
        CB->retire_zone(31);
        CB->pos_perso(P_KALHI33, CB->divers(DV_DESC_POS, P_KALHI13, 0), CB->divers(DV_DESC_POS, P_KALHI13, 1), CB->divers(DV_DESC_POS, P_KALHI13, 2), 1, "P33ARR01", 0x20, 0);
        CB->divers_empile(DV_SCREEN, 3, 0);
        CB->animation("SP33ATT01");
        lieu_action_register("L10D129A", P_KALHI33, "P33DIA54", "SP33ARR01");
        lieu_action_register("L10D129B", P_KALHI33, "SP33DIA59", "SP33ARR01_10");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->modify_var(&ST(0x6C), 1);
        CB->divers(DV_IDLE_TIME, 1, 0);
        CB->divers(DV_LOOK_AT_PLAYER, P_KALHI33, 1);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xA5A2 */
    if (ST(0x74) == 1 && CB->divers(DV_IDLE_TIME, 0, 0) >= 10000 && ST(ST_VIE(P_KALHI13)) == 1 && ST(0x94) == 0) {
        ST(0x74) = 0;
        /* 0xA5FE */
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            /* 0xA610 */
            if (ST(0x948) == 2) {
                lieu_action_register("L10D98", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
            }
            /* 0xA63B */
            if (ST(0x948) == 1) {
                ST(0x948) = 2;
                lieu_action_register("L10D99", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
            }
            /* 0xA675 */
            if (ST(0x948) == 0) {
                ST(0x948) = 1;
                lieu_action_register("L10D999", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
            }
        }
        /* 0xA6AF */
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            /* 0xA6C1 */
            if (ST(0x948) == 2) {
                lieu_action_register("L10D98", P_KALHI33, "SP33DIA60", "SP33ARR01_10");
            }
            /* 0xA6EC */
            if (ST(0x948) == 1) {
                ST(0x948) = 2;
                lieu_action_register("L10D99", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
            }
            /* 0xA726 */
            if (ST(0x948) == 0) {
                ST(0x948) = 1;
                lieu_action_register("L10D999", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
            }
        }
        CB->divers(DV_IDLE_TIME, 1, 0);
        CB->modify_var(&ST(0x74), 1);
        CB->bloque(BLOQUE_NONE);
        return;
    }
    /* 0xA793 */
    if (ST(ST_VIE(P_KALHI33)) == 1 && ST(0xE9C) == 7 && ST(0x94) < 5) {
        ST(0xEF0) = 1;
        /* 0xA7DF */
        if (CB->distance(32, 10) < 120 && ST(0x80) == 0 && ST(0x78) == 0) {
            /* 0xA81B */
            if (CB->divers(DV_RUN, P_KALHI33, 1) == 1) {
                CB->divers(DV_RUN, P_KALHI33, 0);
            }
            ST(0x78) = 1;
            ST(0x74) = 1;
            CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
        }
        /* 0xA87D */
        if (CB->distance(32, 10) < 300 && ST(0x80) == 1) {
            ST(0x80) = 2;
            CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
            CB->modify_var(&ST(0x94), 1);
        }
        /* 0xA8E1 */
        if (ST(0x6C) == 1 && (ST(0x38) % 20) == 0) {
            /* 0xA914 */
            if (CB->distance(32, 10) >= 180) {
                ST(0x78) = 0;
                CB->divers(DV_RUN, P_KALHI33, 0);
                CB->joindre(-P_KALHI33, -1, -1, -1, ST(ST_HERO));
            }
            /* 0xA96D */
            if (CB->distance(32, 10) >= 300) {
                ST(0x78) = 0;
                CB->divers(DV_RUN, P_KALHI33, 1);
                CB->joindre(-P_KALHI33, -1, -1, -1, ST(ST_HERO));
            }
        }
    }
    /* 0xA9BD */
    if (ST(ST_HERO) >= 2 && ST(0xE9C) >= 7 && ST(ST_VIE(P_GUARDIAN15)) == 1 && ST(0xEF4) != 2 && ST(0xEF4) != 3 && (ST(ST_VIE(P_KALHI13)) == 1 || ST(ST_VIE(P_KALHI33)) == 1) && ST(0x50) == 0) {
        /* 0xAA4A */
        if (ST(0x94) == 1 ||
            (CB->divers(DV_DESC_POS, ST(ST_HERO), 1) >= -700 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 4770 && ST(0x94) == 0) ||
            (CB->divers(DV_DESC_POS, ST(ST_HERO), 1) >= -600 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) >= 4770 && ST(0x94) == 0) ||
            (CB->divers(DV_DESC_POS, ST(ST_HERO), 1) >= -700 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 4770 && (ST(V_ENGAGED_PERSONA) == P_KALHI33 || ST(V_ENGAGED_PERSONA) == P_KALHI13)) ||
            (CB->divers(DV_DESC_POS, ST(ST_HERO), 1) >= -600 && CB->divers(DV_DESC_POS, ST(ST_HERO), 0) >= 4770 && (ST(V_ENGAGED_PERSONA) == P_KALHI33 || ST(V_ENGAGED_PERSONA) == P_KALHI13))) {
            ST(0x94) = 2;
            CB->select_pile(0, 2);
            /* 0xABED */
            if (ST(V_ENGAGED_PERSONA) == P_KALHI33) {
                ST(0xE9C) = 7;
                CB->combat(P_KALHI33, -1);
                ST(V_ENGAGED_PERSONA) = 0;
            }
            /* 0xAC27 */
            if (ST(V_ENGAGED_PERSONA) == P_KALHI13) {
                ST(0xE9C) = 7;
                CB->combat(P_KALHI13, -1);
                ST(V_ENGAGED_PERSONA) = 0;
            }
            ST(0xAC) = 0;
            /* 0xAC73 */
            if (ST(ST_LOCKED_CAMERA) != 3) {
                ST(0xAC) = ST(ST_LOCKED_CAMERA);
                CB->change_camera(3, CAM_LOCKED);
            }
            /* 0xACAC */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                ST(0x6C) = 100;
                CB->bloque(BLOQUE_ALL);
                CB->stop_joindre(-P_KALHI13);
                /* 0xACE0 */
                if (ST(0x80) == 0) {
                    lieu_action_register("L10D42", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                }
                /* 0xAD0B */
                if (ST(0x80) == 2) {
                    lieu_action_register("L10D32", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                }
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI13);
                CB->animation("SP13ARR01_10");
            }
            /* 0xAD5E */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                ST(0x6C) = 100;
                CB->bloque(BLOQUE_ALL);
                CB->stop_joindre(-P_KALHI33);
                /* 0xAD92 */
                if (ST(0x80) == 0) {
                    lieu_action_register("L10D42", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
                }
                /* 0xADBD */
                if (ST(0x80) == 2) {
                    lieu_action_register("L10D32", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
                }
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI33);
            }
            CB->modify_var(&ST(0x94), 3);
            CB->select_pile(0, 1);
        }
        /* 0xAE29 */
        if (ST(0x94) == 3) {
            ST(0x94) = 4;
            CB->vire_col(10, 0);
            ST(0xEF4) = 3;
            /* 0xAE66 */
            if (ST(ST_VIE(P_KALHI13)) == 1) {
                CB->stop_joindre(-P_KALHI13);
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L10D37A", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                lieu_action_register("L10D37B", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
                /* 0xAEE3 */
                if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 4000) {
                    CB->joindre(-P_KALHI13, 4770, -770, 0x6A4, -1);
                }
                /* 0xAF22 */
                if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 4000) {
                    CB->joindre(-P_KALHI13, 3800, -660, 0x3E8, -1);
                }
                CB->modify_var(&ST(0x94), 5);
            }
            /* 0xAF67 */
            if (ST(ST_VIE(P_KALHI33)) == 1) {
                CB->stop_joindre(-P_KALHI33);
                CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
                lieu_action_register("L10D50", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
                /* 0xAFCB */
                if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) < 4000) {
                    CB->joindre(-P_KALHI33, 4770, -770, 0x6A4, -1);
                }
                /* 0xB00A */
                if (CB->divers(DV_DESC_POS, ST(ST_HERO), 0) > 4000) {
                    CB->joindre(-P_KALHI33, 3800, -660, 0x3E8, -1);
                }
                CB->modify_var(&ST(0x94), 5);
            }
        }
    }
    /* 0xB04F */
    if (ST(0x94) == 5 && (!CB->on_joindre(P_KALHI13) || !CB->on_joindre(P_KALHI33))) {
        ST(0x94) = 6;
        CB->vire_col(10, 1);
        ST(0xE58) = 0;
        /* 0xB0B5 */
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            CB->stop_joindre(-P_KALHI13);
            CB->animation("SP13ARR01_10");
            CB->divers(DV_RUN, P_KALHI13, 1);
            CB->joindre(-P_KALHI13, 4630, 270, 0, -1);
            CB->modify_var(&ST(0x94), 7);
        }
        /* 0xB11F */
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            CB->stop_joindre(-P_KALHI33);
            CB->animation("SP33ARR01_10");
            CB->divers(DV_RUN, P_KALHI33, 1);
            CB->joindre(-P_KALHI33, 4630, 270, 0, -1);
            CB->modify_var(&ST(0x94), 7);
        }
    }
    /* 0xB189 */
    if (ST(ST_VIE(P_KALHI13)) == 1 && ST(0x94) == 7 && !CB->on_joindre(P_KALHI13) && ST(0xE58) != 2 && ST(0xB40) != 99 && CB->distance(31, 33) < 250) {
        ST(0x94) = 8;
        CB->stop_joindre(-P_KALHI13);
        CB->animation("SP13ARR01_10");
        lieu_action_register("L10D73", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
        lieu_action_register("L10D74", P_GUARDIAN15, "SP55DIA54", "SP55ARR04_10");
        lieu_action_register("L10D75", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
        lieu_action_register("L10D76", P_GUARDIAN15, "SP55DIA54", "SP55ARR04_10");
        CB->ajoute_zone(28, 4420, -140, 70, 70, 0);
        /* 0xB2A3 */
        if (ST(0xAC) != 0) {
            CB->change_camera(ST(0xAC), CAM_AUTO);
            ST(0xAC) = 0;
        } else {
            CB->change_camera(3, CAM_AUTO);
        }
        CB->modify_var(&ST(0xE0), 1);
        CB->joindre(-P_GUARDIAN15, 4420, -250, 0x7530, -1);
        CB->joindre(-P_KALHI13, 4500, 754, 0x4650, -1);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xB346 */
    if (ST(ST_VIE(P_KALHI33)) == 1 && ST(0x94) == 7 && !CB->on_joindre(P_KALHI33) && CB->distance(32, 33) < 250) {
        ST(0x94) = 8;
        CB->stop_joindre(-P_KALHI33);
        CB->animation("SP33ARR01_10");
        lieu_action_register("L10D38A", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
        lieu_action_register("L10D38B", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
        lieu_action_register("L10D39", P_GUARDIAN15, "SP55DIA54", "SP55ARR04_10");
        lieu_action_register("L10D40", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
        lieu_action_register("L10D41", P_GUARDIAN15, "SP55DIA57", "SP55ARR04_10");
        CB->ajoute_zone(20, 4647, -3520, 100, 100, 1);
        CB->ajoute_zone(23, 5180, -1200, 100, 100, 1);
        CB->divers(DV_RUN, P_GUARDIAN15, 1);
        CB->bloque(BLOQUE_NONE);
        CB->modify_var(&ST(0x94), 8);
        CB->modify_var(&ST(0xE0), 1);
        ST(0x50) = 0;
        /* 0xB4C4 */
        if (ST(0xAC) != 0) {
            CB->change_camera(ST(0xAC), CAM_AUTO);
            ST(0xAC) = 0;
        } else {
            CB->change_camera(3, CAM_AUTO);
        }
        CB->joindre(-P_GUARDIAN15, 5180, -1200, 0x7530, -1);
        CB->joindre(-P_KALHI33, 4500, 754, 0x4650, -1);
    }
    /* 0xB548 */
    if (ST(0x94) == 8 && ST(V_ENGAGED_PERSONA) == 0 && CB->distance(33, 28) < 100) {
        ST(0x94) = 9;
        CB->stop_joindre(-P_GUARDIAN15);
        CB->animation("SP55GAR05_10");
        CB->dialogue("L10D77", P_GUARDIAN15);
        /* 0xB5B7 */
        if (ST(0xAC) != 0) {
            CB->change_camera(ST(0xAC), CAM_AUTO);
            ST(0xAC) = 0;
        } else {
            CB->change_camera(3, CAM_AUTO);
        }
    }
    /* 0xB603 */
    if ((ST(0x94) == 8 || ST(0x94) == 9) && ST(ST_VIE(P_GUARDIAN15)) == 1 && ((CB->distance(33, 10) < 500 && CB->divers(DV_ANGLE_TO, P_GUARDIAN15, ST(ST_HERO)) < 70000) || CB->distance(33, 10) < 150)) {
        /* 0xB693 */
        if (ST(V_ENGAGED_PERSONA) == P_KALHI33) {
            CB->combat(P_KALHI33, -1);
        }
        /* 0xB6B2 */
        if (ST(V_ENGAGED_PERSONA) == P_KALHI13) {
            CB->combat(P_KALHI13, -1);
        }
        ST(V_ENGAGED_PERSONA) = P_GUARDIAN15;
        ST(0x94) = 10;
        CB->stop_joindre(-P_GUARDIAN15);
        CB->joindre(-P_GUARDIAN15, -1, -1, -1, ST(ST_HERO));
        ST(V_ENGAGED_PERSONA) = P_GUARDIAN15;
        CB->combat(P_GUARDIAN15, 0);
        ST(0x50) = 1;
        CB->animation("SP55GAR05_10");
        CB->dialogue("L10D34", P_GUARDIAN15);
        CB->select_pile(2, 1);
        /* 0xB76A */
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            CB->stop_joindre(-P_KALHI13);
            lieu_action_register("L10D43", P_KALHI13, "SP13DIA60", "SP13ARR01_10");
        }
        /* 0xB7A0 */
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            CB->stop_joindre(-P_KALHI33);
            lieu_action_register("L10D43", P_KALHI33, "SP33DIA60", "SP33ARR01_10");
        }
        /* 0xB7D6 */
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            CB->divers(DV_RUN, P_KALHI13, 1);
            CB->joindre(-P_KALHI13, 4500, 754, 0x4650, -1);
        }
        /* 0xB816 */
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            CB->divers(DV_RUN, P_KALHI33, 1);
            CB->joindre(-P_KALHI33, 4500, 754, 0x4650, -1);
        }
        ST(0x4C) = 1;
    }
    /* 0xB862 */
    if (ST(V_EVENT_39) == 1) {
        CB->select_pile(0, 1);
        /* 0xB88F */
        if (CB->divers(DV_WAS_HIT, ST(ST_HERO), 0)) {
            CB->divers(DV_WAS_HIT, ST(ST_HERO), 1);
            /* 0xB8CC */
            if ((CB->divers(DV_HP, ST(ST_HERO), -1) - 200) > 0) {
                CB->divers(DV_HP, ST(ST_HERO), (CB->divers(DV_HP, ST(ST_HERO), -1) - 200));
            } else {
                CB->bloque(BLOQUE_ALL);
                CB->combat(P_GUARDIAN15, -1);
                CB->divers(DV_HP, ST(ST_HERO), 0);
            }
        }
        ST(V_EVENT_39) = 0;
    }
    /* 0xB959 */
    if (ST(0x94) == 8 && CB->distance(33, 23) < 100 && ST(V_ENGAGED_PERSONA) == 0) {
        ST(0x94) = 9;
        CB->print_msg("distance33-23 ok", 0, 246);
        CB->select_pile(2, 1);
        CB->retire_zone(23);
        CB->stop_joindre(-P_GUARDIAN15);
        CB->animation("SP55ARR04_10");
        CB->joindre(-P_GUARDIAN15, 4647, -3520, 0x8CA0, -1);
        CB->select_pile(0, 1);
    }
    /* 0xBA0E */
    if (ST(0x94) == 9 && ST(V_ENGAGED_PERSONA) == 0 && CB->distance(33, 20) < 100) {
        ST(ST_VIE(P_GUARDIAN15)) = 4;
        CB->select_pile(2, 1);
        ST(0x94) = 10;
        CB->stop_joindre(-P_GUARDIAN15);
        CB->pos_perso(P_GUARDIAN15, 4480, 600, 0x88B8, 4, "P55ARR04", 0x21, 0);
        CB->select_pile(0, 1);
    }
    /* 0xBAB3 */
    if (ST(0xE0) == 1) {
        /* 0xBAC5 */
        if (ST(ST_VIE(P_KALHI13)) == 1 && !CB->on_joindre(P_KALHI13)) {
            ST(0xE0) = 2;
            CB->stop_joindre(-P_KALHI13);
            CB->animation("SP13ARR01_10");
            CB->joindre(-P_KALHI13, 4312, 754, 0x4650, -1);
            CB->modify_var(&ST(0xE0), 3);
        }
        /* 0xBB42 */
        if (ST(ST_VIE(P_KALHI33)) == 1 && !CB->on_joindre(P_KALHI33)) {
            ST(0xE0) = 2;
            CB->stop_joindre(-P_KALHI33);
            CB->animation("SP33ARR01_10");
            CB->joindre(-P_KALHI33, 4312, 754, 0x4650, -1);
            CB->modify_var(&ST(0xE0), 3);
        }
    }
    /* 0xBBBF */
    if (ST(0xE0) == 3) {
        /* 0xBBD1 */
        if (ST(ST_VIE(P_KALHI13)) == 1 && CB->distance(31, 7) < 100 && !CB->on_joindre(P_KALHI13)) {
            ST(0xE0) = 4;
            CB->stop_joindre(-P_KALHI13);
        }
        /* 0xBC29 */
        if (ST(ST_VIE(P_KALHI33)) == 1 && CB->distance(32, 7) < 100 && !CB->on_joindre(P_KALHI33)) {
            ST(0xE0) = 4;
            CB->stop_joindre(-P_KALHI33);
        }
    }
    /* 0xBC81 */
    if (ST(0xE0) == 4 && CB->distance(10, 7) < 250 && (ST(ST_VIE(P_GUARDIAN15)) > 1 || (ST(0xEF4) == 3 && ST(0x50) == 0))) {
        ST(0xE0) = 5;
        CB->bloque(BLOQUE_ALL);
        /* 0xBCFB */
        if (ST(ST_LOCKED_CAMERA) != 7) {
            CB->change_camera(7, CAM_AUTO);
        }
        ST(0xE9C) = 10;
        /* 0xBD2C */
        if (ST(ST_VIE(P_KALHI13)) == 1) {
            /* 0xBD3E */
            if (ST(0x100) == 0) {
                ST(0x100) = 1;
                CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI13);
                CB->joindre(-P_KALHI13, 0, 0, 0, ST(ST_HERO));
                /* 0xBD99 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D51", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0xBDC4 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D52", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D53", P_KALHI13, "SP13DIA53", "SP13ARR01_10");
                /* 0xBE08 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D54", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0xBE33 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D55", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D56", P_KALHI13, "SP13DIA54", "SP13ARR01_10");
            }
            CB->joindre(-P_KALHI13, 0, 0, 0x4650, -1);
            CB->animation("SP13ACT20");
            CB->animation("P13_65535");
            CB->bruitage("L10B01", 4310, 1040);
        }
        /* 0xBEB8 */
        if (ST(ST_VIE(P_KALHI33)) == 1) {
            CB->joindre(-ST(ST_HERO), 0, 0, 0, P_KALHI33);
            CB->joindre(-P_KALHI33, 0, 0, 0, ST(ST_HERO));
            /* 0xBF04 */
            if (ST(ST_VIE(P_GUARDIAN15)) == 1 && ST(0x94) >= 7) {
                ST(ST_VIE(P_GUARDIAN15)) = 4;
                CB->pos_perso(P_GUARDIAN15, 4480, 600, 0x88B8, 4, "P55ARR04", 0x21, 0);
            }
            /* 0xBF5C */
            if (ST(0xEF4) == 3) {
                /* 0xBF6E */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D47", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0xBF99 */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D48", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D49", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
            }
            /* 0xBFDD */
            if (ST(0xEF4) == 2 && ST(0x100) == 0) {
                ST(0x100) = 1;
                /* 0xC010 */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D51", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0xC03B */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D52", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D53", P_KALHI33, "SP33DIA53", "SP33ARR01_10");
                /* 0xC07F */
                if (ST(ST_HERO) == P_ARKHAN2) {
                    lieu_action_register("L10D54", P_ARKHAN2, "SG00DIA05", "Sar");
                }
                /* 0xC0AA */
                if (ST(ST_HERO) == P_ARKHAN3) {
                    lieu_action_register("L10D55", P_ARKHAN3, "SG00DIA05", "Sar");
                }
                lieu_action_register("L10D56", P_KALHI33, "SP33DIA54", "SP33ARR01_10");
            }
            lieu_action_register("L10D57", P_KALHI33, "SP33DIA55", "SP33ARR01_10");
            CB->joindre(-P_KALHI33, 0, 0, 0x4650, -1);
            CB->animation("SP33ACT20");
            CB->animation("P33_65535");
            CB->bruitage("L10B01", 4310, 1040);
        }
        CB->divers_empile(DV_BIG_ANIM, 2, 0);
        CB->divers_empile(DV_MUSIC, 1, 4);
        ST(ST_LEVEL) = 10;
        CB->change_piece(14, -1);
        ST(0x12C) = ST(0x6C);
    }
    /* 0xC19C */
    if (ST(ST_VIE(P_KALHI13)) == 1 && ST(0xE9C) == 8 && ST(0x84) == 1 && CB->distance(31, 10) < 120) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x84) = 2;
        ST(0xE9C) = 7;
        ST(V_KALHI13_TALK_C) = 2;
        CB->animation("SP13LEV01");
        lieu_action_register("L10D103", P_KALHI13, "P13DIA60", "SP13ARR01_10");
        lieu_action_register("L10D104", P_KALHI13, "P13DIA60", "SP13ARR01_10");
        lieu_action_register("L10D105", P_KALHI13, "P13DIA60", "SP13ARR01_10");
        CB->modify_var(&ST(0x6C), 1);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xC2B2 */
    if (ST(ST_VIE(P_KALHI33)) == 1 && ST(0xE9C) == 8 && ST(0x84) == 1 && CB->distance(32, 10) < 120) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        ST(0x84) = 2;
        ST(0xE9C) = 7;
        ST(V_KALHI13_TALK_C) = 2;
        CB->animation("SP33LEV01");
        lieu_action_register("L10D103", P_KALHI33, "P33DIA60", "SP33ARR01_10");
        lieu_action_register("L10D104", P_KALHI33, "P33DIA60", "SP33ARR01_10");
        lieu_action_register("L10D105", P_KALHI33, "P33DIA60", "SP33ARR01_10");
        CB->modify_var(&ST(0x6C), 1);
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
    /* 0xC3C8 */
    if (ST(V_DELIA_PHASE) == 6 && ST(ST_VIE(P_DELIA)) == 1 && ST(ST_SCRIPT_BUSY) == 0 && CB->divers(DV_DESC_POS, ST(ST_HERO), 3) > -750) {
        /* 0xC427 */
        if (CB->distance(30, 10) < 140 && ST(0x78) == 0) {
            /* 0xC451 */
            if (CB->divers(DV_RUN, P_DELIA, 1) == 1) {
                CB->divers(DV_RUN, P_DELIA, 0);
            }
            ST(0x78) = 1;
            CB->joindre(-P_DELIA, 0, 0, 0, ST(ST_HERO));
        }
        /* 0xC49B */
        if (ST(0x6C) == 1 && (ST(0x38) % 20) == 0) {
            /* 0xC4CE */
            if (CB->distance(30, 10) >= 180) {
                ST(0x78) = 0;
                CB->divers(DV_RUN, P_DELIA, 0);
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
            /* 0xC527 */
            if (CB->distance(30, 10) >= 300) {
                ST(0x78) = 0;
                CB->divers(DV_RUN, P_DELIA, 1);
                CB->joindre(-P_DELIA, -1, -1, -1, ST(ST_HERO));
            }
        }
    }
    /* 0xC577 */
    if (ST(0xDC) == 1) {
        ST(0xDC) = 2;
        ST(0xAC) = ST(ST_LOCKED_CAMERA);
        /* 0xC5AF */
        if (ST(ST_LOCKED_CAMERA) == 0) {
            CB->pause_anim(1, 0, 1);
            CB->pause_anim(2, 0, -1);
            CB->pause_anim(3, 0, -1);
        }
        /* 0xC5EE */
        if (ST(ST_LOCKED_CAMERA) == 1) {
            CB->pause_anim(1, 0, -1);
            CB->pause_anim(2, 0, 1);
            CB->pause_anim(3, 0, -1);
        }
        /* 0xC62D */
        if (ST(ST_LOCKED_CAMERA) == 5) {
            CB->pause_anim(1, 0, -1);
            CB->pause_anim(2, 0, -1);
            CB->pause_anim(3, 0, 1);
        }
    }
    /* 0xC677 */
    if (ST(ST_LOCKED_CAMERA) != ST(0xAC) && ST(0xDC) == 2) {
        ST(0xDC) = 1;
    }
    /* 0xC6AA */
    if (ST(V_EVENT_11) == 2) {
        ST(V_EVENT_11) = 3;
        CB->bloque(BLOQUE_ALL);
        CB->animation("Sar_10");
        /* 0xC6E1 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->animation("SP02PUI01");
            CB->animation("P02_65535");
        }
        /* 0xC709 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->animation("SP03PUI01");
            CB->animation("P03_65535");
        }
        CB->modify_var(&ST(V_EVENT_11), 4);
    }
    /* 0xC748 */
    if (ST(V_EVENT_11) == 4) {
        ST(V_EVENT_11) = 5;
        /* 0xC772 */
        if (CB->distance(10, 14) <= 500) {
            CB->cinematique("SC16");
            CB->animation("sar_50");
            /* 0xC7A2 */
            if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
                CB->divers(DV_HP, P_ARKHAN2, 0);
            }
            /* 0xC7CE */
            if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
                CB->divers(DV_HP, P_ARKHAN3, 0);
            }
            ST(V_DEATH_EVENT_SENT) = 1;
            CB->divers_empile(DV_END_GAME, 0, 0);
        } else {
            CB->cinematique("SC15");
            ST(0xB4C) = 2;
            CB->divers_empile(DV_MUSIC, 1, 4);
            CB->change_piece(51, -1);
            ST(0x12C) = ST(0x6C);
        }
    }
    /* 0xC865 */
    if (ST(V_EVENT_12) == 2) {
        ST(V_EVENT_12) = 3;
        CB->bloque(BLOQUE_ALL);
        /* 0xC891 */
        if (ST(ST_HERO) == P_ARKHAN2) {
            CB->divers_empile(DV_HP, P_ARKHAN2, 0);
        }
        /* 0xC8B5 */
        if (ST(ST_HERO) == P_ARKHAN3) {
            CB->divers_empile(DV_HP, P_ARKHAN3, 0);
        }
        CB->cinematique("SC16");
        CB->animation("sar_50");
        /* 0xC8F7 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN2, 0)) {
            CB->divers(DV_HP, P_ARKHAN2, 0);
        }
        /* 0xC923 */
        if (CB->divers(DV_PERSONA_PRESENT, P_ARKHAN3, 0)) {
            CB->divers(DV_HP, P_ARKHAN3, 0);
        }
        ST(V_DEATH_EVENT_SENT) = 1;
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
}

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - arming tail of FonctionEditor case 1 (0x1001425A).
 * Callback ingestion, the trigger-table fill and the unused
 * dword_10025A3C = state+0xC8 are skipped: g_lieu_cb is provided by the
 * bridge, the trigger table is static in lieu33_trig.c. There is no death
 * branch in this version.
 *   arg = reload flag (non-zero => skip arming).
 * ---------------------------------------------------------------------- */
static void init(i32 reload_flag) {
    if (reload_flag == 0)
        l33_trig00();   /* 0x100145E3: call sub_10001073 -> sub_10005FB0 */
}

/* -------------------------------------------------------------------------
 * TICK (opcode -1): drain pending sound, run body.
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
 * Entry point - @FonctionEditor@16 (thunk 0x10001046-block -> 0x10014240).
 * Jump table @ 0x100146B3, index = op+1 (0..7); op 0 / op 2 / out of range
 * fall through to `return state[0]`.
 * ---------------------------------------------------------------------- */
i32 lieu33_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu33: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   tick();                       break;  /* op -1 */
    case LIEU_OP_INIT:
        l33_reset_dll_globals();
        init(arg);
        break;
    case LIEU_OP_EVENT:  l33_trigger(arg);             break;  /* dword_10025930[arg]() */
    case LIEU_OP_CALL4:  event4(arg);                  break;
    case LIEU_OP_CALL5:  event5(arg, arg2);            break;
    case LIEU_OP_CALL6:  event6(arg);                  break;
    case LIEU_OP_TEARDOWN: /* engine-side cleanup; handler no-op */
    default:                                           break;
    }
    return ST(LIEU_ST_RESULT);
}
