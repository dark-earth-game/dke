#ifndef DKE_LIEU_CB_H
#define DKE_LIEU_CB_H

/*
 * lieu_cb.h — Engine callback ABI for LIEU level-scripting DLLs.
 *
 * Every LIEU%02d.DLL talks to the engine through ONE thing: a 40-slot
 * table of function pointers handed to it at INIT (opcode 1). The DLL
 * copies those pointers into globals (dword_1002C4xx..C9xx) and calls
 * them by name. The pointers live in dkev.exe (sub_44xxxx). Runtime
 * handlers (opcode 0/3-6) call the SAME 40 slots, so this table is the
 * whole engine-facing surface — implement once, reuse for all 55 levels.
 *
 * Mapping recovered from LIEU01 INIT ingestion (FonctionEditor @ 0x18C60
 * case 1: a3[i] -> DLL global) crossed with docs/functions/lieu_dll.md
 * (byte-offset -> engine sub) and confirmed against real call sites in
 * sub_10005BEC / sub_10008798 / sub_1001932C.
 *
 * Convention: DLL calls these __fastcall(ecx, edx) with 0-1 extra stack
 * args. Field name = the DLL global (dword_1002CXXX) it was copied into.
 * Typed where a caller pinned the shape; void* + [?] where still unknown
 * (bridge as no-op + log until a caller reveals it).
 */

#include "libsys/common.h"

/* Per-level state buffer (engine dword_505C40, 0xFA0 = 4000 bytes).
 * Handlers index it by raw byte offset; keep it as bytes. */
typedef u8 lieu_state_t;
#define LIEU_ST(S, off)  (*(i32 *)((lieu_state_t *)(S) + (off)))

/* Known state offsets (see docs/functions/lieu01_handlers.md). */
#define LIEU_ST_RESULT      0x000
#define LIEU_ST_PENDING_SND 0x134  /* pending sound/cine id; base +0x59 */
#define LIEU_ST_SUBMODE     0x194  /* level sub-mode */
#define LIEU_ST_CINE_SEL    0xB04  /* 0->SC22500, 1->SC23500 */
#define LIEU_ST_FORCE_DEF   0xE8C

/*
 * Engine callback vtable. i = slot index (byte offset i*4 in engine
 * table). Named members are typed from confirmed call sites; the rest
 * stay void* until a caller pins them.
 */
typedef struct {
    /* i=0  0x00 sub_44453E — NamedObject(name): push a named object/ref. */
    void (*named_object)(const c8 *name);                  /* C460 */
    /* i=1  0x04 sub_4449A5 — NamedObject + int. */
    void (*named_object_int)(const c8 *name, i32 val);     /* C470 */
    /* i=2  0x08 sub_444A3E — NamedObjectTwoInts(id, param). Object/sound
     * trigger. tick: (id+0x59,1000) start; INIT/handlers: (id,-1) arm. */
    void (*named_two_ints)(i32 id, i32 param);             /* C674 */
    /* i=3  0x0C sub_444AC0 — Cinematique(name): play movie (thiscall). */
    void (*cinematique)(const c8 *name);                   /* C874 */
    /* i=4  0x10 KillPerso — NOT ingested by LIEU01. */
    /* i=5  0x14 sub_44502D — spawn object/actor, full form.
     * (ecx=zone, edx=anim, x, y, flag, name, kind, z). */
    void (*spawn_678)(i32 zone, i32 anim, i32 x, i32 y, i32 flag,
                      const c8 *name, i32 kind, i32 z);    /* C678 */
    /* i=6  0x18 sub_445189 — NamedObject + params (ecx=name, edx=p,
     * +7 stack a1..a7, a1 = last pushed). e.g. "SOB86", "OB21A". */
    void (*named_ex_988)(const c8 *name, i32 p, i32 a1, i32 a2, i32 a3,
                         i32 a4, i32 a5, i32 a6, i32 a7);  /* C988 */
    void *cb_960;   /* i=7  0x1C sub_4452CD  [?]                       */
    /* i=8  0x20 sub_4453E1 — place actor at position.
     * (ecx=zone, edx=facing, a2, a3, a4). */
    void (*actor_934)(i32 zone, i32 facing, i32 a2, i32 a3, i32 a4); /* C934 */
    /* NOTE: i=15 (C954) declared below in sequence. */
    /* i=9  0x24 sub_4454F2 — seen (6,1). */
    void (*cb_458)(i32 ecx, i32 edx);                      /* C458 */
    /* i=10 0x28 sub_4455EE — seen (0x19, -1). */
    void (*cb_984)(i32 ecx, i32 edx);                      /* C984 */
    /* i=11 0x2C sub_44569A — place named object at coord.
     * (ecx=name, edx=id, coord). */
    void (*place_95C)(const c8 *name, i32 id, i32 coord);  /* C95C */
    /* i=12 0x30 sub_445765 — AjouteZone(id, cx, cz, sizeX, sizeZ, flag):
     * register a script zone rectangle (spatial args scaled x600). id ==
     * LIEU trigger index. (Named spawn_978 historically; it is NOT a spawn.) */
    void (*spawn_978)(i32 id, i32 cx, i32 cz, i32 sx, i32 sz, i32 flag); /* C978=AjouteZone */
    /* i=13 0x34 sub_4458AD — RetireZone(id): clear a script zone slot. */
    void (*cb_93C)(i32 id);                                /* C93C=RetireZone */
    /* i=14 0x38 sub_445919 — seen (0x58,0). */
    void (*cb_970)(i32 ecx, i32 edx);                      /* C970 */
    /* i=15 0x3C sub_44599B — query int (ecx). Returns value tested nz. */
    i32 (*query_954)(i32 ecx);                             /* C954 */
    /* i=16 0x40 sub_445C82 — query int (ecx), tested nz. */
    i32 (*query_944)(i32 ecx);                             /* C944 */
    /* i=17 0x44 sub_445E99 — query int (ecx), tested nz. */
    i32 (*query_998)(i32 ecx);                             /* C998 */
    /* i=18 0x48 sub_445FD5 — query int (distance/timer). Returns value
     * compared against thresholds. (ecx, edx). */
    i32 (*query_990)(i32 ecx, i32 edx);                    /* C990 */
    /* i=19 0x4C sub_446159 — named object placement (ecx=name, edx=p,
     * +3 stack a1,a2,a3; last push=a1). e.g. "IM01". */
    void (*cb_98C)(const c8 *name, i32 p, i32 a1, i32 a2, i32 a3); /* C98C */
    /* i=20 0x50 sub_446227 — group begin/end bracket. ecx=3 open, 0 close. */
    void (*group)(i32 ecx);                                /* C938 */
    /* i=21 0x54 sub_44637B — level-exit / fade trigger (ecx, e.g. 0x57). */
    void (*cb_454)(i32 ecx);                               /* C454 */
    /* i=22 0x58 sub_44648B — select camera/zone (ecx, often negative). */
    void (*select_468)(i32 ecx);                           /* C468 */
    /* i=23 0x5C sub_44660D — flag/state set (ecx, edx). */
    void (*cb_964)(i32 ecx, i32 edx);                      /* C964 */
    void *cb_980;   /* i=24 0x60 sub_446685  [?]                       */
    /* i=25 0x64 sub_4466E9 — debug/subtitle text (ecx=str, edx, len). */
    void (*debug_text)(const c8 *text, i32 edx, i32 len);  /* C474 */
    void *cb_94C;   /* i=26 0x68 sub_446763  [?]                       */
    /* i=27 0x6C sub_446808 — commit/flush dialogue group (ecx=zone, edx=submode). */
    void (*flush_958)(i32 zone, i32 submode);              /* C958 */
    /* i=28 0x70 sub_4468E1 — set state slot (ecx=&state[off], edx=val). */
    void (*cb_46C)(i32 *slot, i32 edx);                    /* C46C */
    /* i=29 0x74 sub_4469BA — query int (ecx). Returns value cmp'd to 1. */
    i32 (*query_940)(i32 ecx);                             /* C940 */
    /* i=30 0x78 sub_446A05 — state_toggle(mode, layer). */
    void (*state_toggle)(i32 mode, i32 layer);             /* C464 */
    void *cb_968;   /* i=31 0x7C sub_446B54  [?]                       */
    /* i=32 0x80 sub_446BE9 — object attach/toggle (ecx=zone, edx=name, +2 stack). */
    void (*attach_99C)(i32 zone, const c8 *name, i32 a, i32 b); /* C99C */
    void *cb_948;   /* i=33 0x84 sub_446CBC  [?]                       */
    /* i=34 0x88 sub_446E19 — layer_op(op, layer, arg). Returns bool. */
    i32 (*layer_op)(i32 op, i32 layer, i32 arg);           /* C994 */
    void *cb_974;   /* i=35 0x8C sub_4430B0  [?]                       */
    /* i=36 0x90 sub_443273 — seen (1). */
    void (*cb_950)(i32 ecx);                               /* C950 */
    /* i=37 0x94 sub_44356F — seen (2, 0). */
    void (*cb_97C)(i32 ecx, i32 edx);                      /* C97C */
    void *cb_33C;   /* i=38 0x98 sub_443643  [?]                       */
    /* i=39 0x9C sub_446D84 — op_9c(op, a, arg). INIT: (16,1,0)(27,0,0). */
    void (*op_9c)(i32 op, i32 a, i32 arg);                 /* C45C */
} lieu_cb_t;

/* Bridge-provided vtable (defined in lieu_cb.c). Filled by lieu_enter
 * before any LIEU handler runs. lieu01.c calls exclusively through this. */
extern const lieu_cb_t *g_lieu_cb;

/* -------------------------------------------------------------------------
 * Driver API (lieu_cb.c) — replaces the old .script replay path.
 *   lieu_enter — on level load: reset state, install vtable, run INIT.
 *   lieu_tick  — per frame: run the opcode-0 tick handler.
 *   lieu_exit  — on level release: run deinit.
 * Only level 1 is hand-ported (src/lieu01.c); other levels are no-ops.
 * ---------------------------------------------------------------------- */
void lieu_enter(i32 level_id);
void lieu_tick(u32 now_ms);   /* now_ms = game clock, drives holds + timers */
void lieu_exit(void);

/* Runtime event feeders (see lieu_cb.c). Call when the engine detects the
 * corresponding condition; not driven during normal play yet. */
void lieu_zone_trigger(i32 zone_id);   /* op 3: trigger[zone_id]  */
void lieu_object_timer(i32 id);        /* op 4: event4(id)        */
void lieu_pending_event(i32 v, i32 phase); /* op 5: event5(v, phase)  */

/* Pointer to a field in the live state buffer, so engine code can set the
 * guards handlers read (e.g. *lieu_state_slot(0x194) = submode). */
i32 *lieu_state_slot(i32 off);

/* Per-frame zone lookup (op-3 source). Returns a zone/trigger id, or -1
 * for none. Stub returns -1 until player transform + zone geometry exist. */
i32 lieu_detect_zone(void);

/* Cutscene queued by cb_cinematique. Returns the "SCnn.CRH" path to play
 * (once), or NULL. Drained by scene_render (safe render context). */
const c8 *lieu_poll_movie(void);

/* CHANGEPIECE (opcode 10): pending room-change target level id (or -1 if
 * none). Set when the LIEU script calls slot 10 to walk to another room.
 * Clears on read; writes the destination entry-point id to *entry. */
i32 lieu_poll_level_change(i32 *entry);

/* ChangeCamera (slot 9): pending camera the script requested (or -1). Clears
 * on read; *mode gets the mode (2 = forced/locked, else auto-follow). */
i32 lieu_poll_camera_change(i32 *mode);

/* One-shot intro->room transition: re-runs INIT at submode 0 so trig00
 * places the room's arrival characters. Call after the intro movie plays. */
void lieu_enter_room(void);
void lieu_zone_debug(void);

/* Persona placements recorded by PosPerso (spawn_678). For a future dke
 * character-render pass. lieu_spawn_get returns the name or NULL. */
i32 lieu_spawn_count(void);
const c8 *lieu_spawn_get(i32 i, i32 *x, i32 *y, i32 *view);
i32 lieu_spawn_ground(i32 i);   /* PosPerso z = world Y ground (unscaled) */
i32 lieu_spawn_facing(i32 i);   /* PosPerso y = facing angle              */

/* Player movement blocking from group(3)/group(0) brackets.
 * Returns 1 while a dialogue/cutscene group is open. */
i32 lieu_player_blocked(void);

/* CAN clip the script last asked a persona to play (ANIMATION actions, whose
 * name is the clip id: "p12arr03" -> persona 12). Returns NULL when the
 * script has not driven that persona; *serial changes whenever the clip does,
 * so a renderer can reload only on change. */
const c8 *lieu_persona_clip(i32 persona, i32 *serial);

#endif /* DKE_LIEU_CB_H */
