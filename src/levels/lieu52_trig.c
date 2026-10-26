/*
 * lieu52_trig.c — Native hand-port of the LIEU52.DLL zone trigger handlers
 * (opcode 3). The entry point, INIT, tick and tick body live in lieu52.c;
 * this file holds the 42-slot trigger table and its handlers.
 *
 * Ported from disassembly (ImageBase 0x10000000). Each function carries
 * its original address. Handlers call the engine only through g_lieu_cb.
 */

#include "lieu52_int.h"

static void trig_noop(void);   /* the empty slots (identical to L01 trig06) */
static void trig02(void); static void trig03(void); static void trig04(void);
static void trig05(void); static void trig06(void); static void trig07(void);
static void trig08(void); static void trig09(void); static void trig10(void);
static void trig11(void); static void trig15(void); static void trig41(void);

/* dword_10017E60[42] — filled at INIT with the 5-byte jmp thunks; static here.
 * Slots 1, 12-14, 16-40 are empty stubs; the others are
 *   0 100073F0   2 100077BB   3 10007858   4 100078F5   5 10007992
 *   6 10007A2F   7 10007AC9   8 10007B66   9 10007C03  10 10007CA0
 *  11 10007D3D  15 10007DFB  41 10007739
 */
static void (*const g_trigger[42])(void) = {
    l52_trig00, trig_noop, trig02, trig03, trig04, trig05, trig06, trig07,
    trig08, trig09, trig10, trig11, trig_noop, trig_noop, trig_noop, trig15,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop, trig_noop,
    trig_noop, trig_noop, trig_noop, trig_noop, trig41,
};

/* Op 3: run trigger handler idx (call [4*arg + 0x10017E60]). */
void l52_trigger(i32 idx) {
    if (idx >= 0 && idx < 42)
        g_trigger[idx]();
}

/* Empty slots: the stub bodies are `push ebp; mov ebp,esp; ... ret`. */
static void trig_noop(void) {
}

/* trigger 00 — sub_100073F0 — level setup: zones, hero placement, story reset */
void l52_trig00(void) {
    if (ST(ST_START_POINT(52)) == 1) {
        ST(0xEC) = 1;
    }
    CB->ajoute_zone(15, 514, -2094, 110, 125, 0);
    CB->ajoute_zone(2, 517, -3105, 150, 130, 0);
    CB->ajoute_zone(3, 353, -2966, 150, 135, 0);
    CB->ajoute_zone(4, 37, -2696, 125, 125, 0);
    CB->ajoute_zone(5, 37, -2526, 125, 236, 0);
    CB->ajoute_zone(6, 151, -2435, 130, 120, 0);
    CB->ajoute_zone(7, 654, -2191, 160, 175, 0);
    CB->ajoute_zone(8, 856, -2430, 135, 140, 0);
    CB->ajoute_zone(9, 968, -2496, 120, 135, 0);
    CB->ajoute_zone(10, 995, -2657, 115, 140, 0);
    CB->ajoute_zone(11, 867, -2770, 140, 130, 0);
    /* 0x1000756A */
    if (ST(0xEC) == 0) {
        CB->pos_perso(ST(ST_HERO), 508, -2095, 0x8000, 1, "ar", 1, 0);
    }
    /* 0x100075A7 */
    if (ST(0xEC) == 1) {
        ST(0xE3C) = 0;
        CB->divers(DV_DARKNESS, 0, 0);
        CB->divers(DV_HP, 0, 0x3FF);
        CB->divers(DV_INVENTORY, 2, 0);
        CB->pos_perso(0, 508, -2095, 0x8000, 1, "ar", 1, 0);
    }
    /* 0x1000761D */
    CB->divers(DV_MUSIC, 1, 0);
    CB->bruitage("L27AMB1", -1, -1);
    ST(0x64) = 0;
    ST(0x68) = 0;
    ST(0xB8) = 0;
    ST(0xBC) = 0;
    ST(ST_SCRIPT_BUSY) = 0;
    ST(V_ENGAGED_PERSONA) = 0;
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
}

/* trigger 41 — sub_10007739 */
static void trig41(void) {
    if (ST(0xEC) == 1) {
        CB->evenement(0, 750);
    }
}

/* Triggers 2-11 are one template: action key pressed, no script running, not
 * in combat stance and the hero faces `angle` -> lock input, credits page n,
 * "Sar_50", release. */
static void credits_trigger(i32 angle, i32 n) {
    if (ST(ST_ACTION_KEY) == 1 && ST(ST_SCRIPT_BUSY) == 0 && ST(ST_COMBAT_STANCE) == 0 &&
        CB->direction_spec(angle)) {
        CB->bloque(BLOQUE_ALL);
        ST(ST_SCRIPT_BUSY) = 1;
        CB->divers_empile(DV_CREDITS, n, 0);
        CB->animation("Sar_50");
        CB->modify_var(&ST(ST_SCRIPT_BUSY), 0);
        CB->bloque(BLOQUE_NONE);
    }
}

/* trigger 02 — sub_100077BB */
static void trig02(void) { credits_trigger(0x8000, 2); }
/* trigger 03 — sub_10007858 */
static void trig03(void) { credits_trigger(0x4000, 5); }
/* trigger 04 — sub_100078F5 */
static void trig04(void) { credits_trigger(0x4000, 4); }
/* trigger 05 — sub_10007992 */
static void trig05(void) { credits_trigger(0x4000, 7); }
/* trigger 06 — sub_10007A2F */
static void trig06(void) { credits_trigger(0, 3); }
/* trigger 07 — sub_10007AC9 */
static void trig07(void) { credits_trigger(0xC000, 1); }
/* trigger 08 — sub_10007B66 */
static void trig08(void) { credits_trigger(0x1F40, 8); }
/* trigger 09 — sub_10007C03 */
static void trig09(void) { credits_trigger(0xC000, 10); }
/* trigger 10 — sub_10007CA0 */
static void trig10(void) { credits_trigger(0xC000, 6); }
/* trigger 11 — sub_10007D3D */
static void trig11(void) { credits_trigger(0x8000, 9); }

/* trigger 15 — sub_10007DFB */
static void trig15(void) {
    if (ST(0xEC) == 0 && CB->direction_spec(0)) {
        ST(ST_LEVEL) = 52;
        CB->animation("SG00OUV01");
        CB->animation("ar");
        CB->change_piece(2, 0);
    } else {
        CB->divers(DV_HP, 0, 0);
        ST(ST_HERO) = 2;
        CB->divers_empile(DV_SCREEN, 5, 0);
        CB->divers_empile(DV_END_GAME, 0, 0);
    }
}
