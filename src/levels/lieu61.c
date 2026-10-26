/*
 * lieu61.c - Native hand-port of LIEU61.DLL (level-61 scripting): the
 * @FonctionEditor@16 entry, INIT and the per-frame tick. The 42 zone trigger
 * handlers live in lieu61_trig.c (reached through l61_trigger, see
 * lieu61_int.h).
 *
 * Ported by hand from the disassembly (ImageBase 0x10000000). Unlike the
 * other levels this DLL was built with an optimizing compiler: no ebp frame
 * in the entry, and the callback copy reuses eax/ebx/ebp. The entry is at
 * 0x100014B0, the state pointer global is dword_1000721C (S) and the trigger
 * table is dword_10007140[42] (static here).
 *
 * Callback map (DLL global <- engine table offset), as copied by the entry;
 * the C port gets the same callbacks through g_lieu_cb:
 *   10006F10<-00 animation    10006F20<-04 dialogue    10006F28<-08 evenement
 *   10007134<-0C cinematique  10006F2C<-14 pos_perso   10007230<-18 etat_objet
 *   10007210<-1C col_objet    100071E8<-20 joindre     10006F0C<-24 change_camera
 *   1000722C<-28 change_piece 1000720C<-2C bruitage    10007224<-30 ajoute_zone
 *   100071F0<-34 retire_zone  10007220<-38 combat      10007204<-3C direction_perso
 *   100071F8<-40 direction_zone 10007240<-44 direction_spec
 *   10007238<-48 distance     10007234<-4C anim_objet  100071EC<-50 bloque
 *   10006E5C<-54 fouille      10006F18<-58 stop_joindre 10007214<-5C vire_col
 *   10007228<-60 simult       10006F24<-64 print_msg   10007200<-68 joindre_obj
 *   10007208<-6C joindre_perm 10006F1C<-70 modify_var  100071F4<-74 random
 *   10006F14<-78 select_pile  10007218<-7C on_joindre  10007244<-80 link_objet
 *   100071FC<-84 place_animal 1000723C<-88 (unused)    10006E58<-state+0xC8
 *   10006F30 <- the trigger handler pointer, stored just before the call.
 *
 * Op dispatch: jump table 100017FC indexed by op+1 (7 entries). Op -1 tick
 * is the empty sub_100011D0 (no pending-event drain, no tick body). Op 4 and
 * op 5 call the empty sub_10001900 / sub_10001910 (so, unlike the shared
 * lieu_event4/5, they do nothing). Op 6, op 0 and op 2 do nothing. Op 3
 * calls table[arg]. The trigger table is filled by the same entry block as
 * the callback copy (op 1), with the jmp thunks at 0x10001000.
 *
 * The level code touches no DLL .data global, so INIT resets nothing. INIT
 * has no death branch: it runs trigger 0 when `arg` is zero.
 */

#include <stdio.h>

#include "lieu61_int.h"

/* -------------------------------------------------------------------------
 * INIT (opcode 1) - tail of FonctionEditor case 1 (0x100017B9).
 * Callback ingestion and the trigger-table fill are skipped.
 *   arg = reload flag (non-zero => skip arming).
 * ---------------------------------------------------------------------- */
static void init(i32 reload_flag) {
    if (reload_flag == 0)
        l61_trig00();   /* 0x100017BD: call thunk sub_10001064 -> sub_10001206 */
}

/* -------------------------------------------------------------------------
 * Entry point - @FonctionEditor@16 (thunk 0x10001037 -> 0x100014B0).
 * ---------------------------------------------------------------------- */
i32 lieu61_fonction_editor(i32 op, lieu_state_t *state, i32 arg, i32 arg2) {
    g_lvl_S = state;
    if (g_lieu_trace)
        print("lieu61: op=%d arg=%d arg2=%d\n", op, arg, arg2);
    switch (op) {
    case LIEU_OP_TICK:   /* sub_100011D0: empty */      break;  /* op -1 */
    case LIEU_OP_INIT:   init(arg);                     break;
    case LIEU_OP_EVENT:  l61_trigger(arg);              break;  /* dword_10007140[arg]() */
    case LIEU_OP_CALL4:  /* sub_10001900: empty */      break;
    case LIEU_OP_CALL5:  /* sub_10001910: empty */      break;
    case LIEU_OP_CALL6:
    case LIEU_OP_TEARDOWN:
    default:                                            break;
    }
    return ST(LIEU_ST_RESULT);
}
