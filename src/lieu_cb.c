/*
 * lieu_cb.c — Engine-side bridge for the LIEU level-scripting hand-port.
 *
 * Provides:
 *   - the 40-slot callback vtable g_lieu_cb that lieu01.c calls into,
 *   - the per-level state buffer (engine dword_505C40, 0xFA0 bytes),
 *   - lieu_enter / lieu_tick / lieu_exit for scene.c to drive.
 *
 * The callbacks are currently LOG-ONLY stubs: the dke engine does not yet
 * expose the object-queue / audio / cinematic / layer primitives the
 * original engine (dkev.exe sub_44xxxx) provided. This lets the ported
 * control flow RUN and be traced without crashing; wire real effects
 * incrementally as the engine grows. Grep "TODO wire" for the work list.
 */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "lieu01.h"
#include "player.h"
#include "dialogue.h"

/* -------------------------------------------------------------------------
 * Per-level state buffer + owned vtable pointer.
 * ---------------------------------------------------------------------- */
static lieu_state_t g_lieu_state[0xFA0];   /* 4000 bytes, zeroed per level */
static i32          g_lieu_level = 0;      /* active level id (0 = none)   */
static i32          g_lieu_room_entered = 0; /* post-intro re-init done     */

const lieu_cb_t *g_lieu_cb = NULL;         /* storage (extern in header)   */

/* -------------------------------------------------------------------------
 * Zone table (engine dword_506C80[41], stride 0x24). Populated by
 * AjouteZone (slot 12 = spawn_978) / cleared by RetireZone (slot 13 =
 * cb_93C). Zone id == LIEU trigger index (1..40). Feeds op-3 dispatch and
 * the query_990/944 callbacks. Only static (non-entity) zones for now.
 * ---------------------------------------------------------------------- */
typedef struct {
    i32 center_x;   /* world units; -1 = empty slot */
    i32 center_z;
    i32 size_x;     /* full extent (half = /2) */
    i32 size_z;
    i32 active;     /* 1 = always tested; 0 = only when g_force_zones */
} zone_t;

#define LIEU_ZONE_COUNT 41
#define LIEU_ZONE_SCALE 600            /* AjouteZone tile->world (0x258)   */
static zone_t g_zones[LIEU_ZONE_COUNT];
static i32    g_force_zones = 1;       /* dword_4FFB80; 1 = test all zones */

static void zones_clear(void) {
    for (i32 i = 0; i < LIEU_ZONE_COUNT; i++) g_zones[i].center_x = -1;
}

void lieu_zone_debug(void) {
    for (i32 i = 1; i < LIEU_ZONE_COUNT; i++) {
        if (g_zones[i].center_x == -1) continue;
        print("zone[%d] c=(%d,%d) half=(%d,%d) active=%d\n", i,
              g_zones[i].center_x, g_zones[i].center_z,
              g_zones[i].size_x / 2, g_zones[i].size_z / 2, g_zones[i].active);
    }
}

/* Facing tolerance ~50.6 deg (0x2400 BAM). Signed shortest-arc compare. */
static i32 facing_ok(i32 player_ang, i32 target_ang) {
    i32 d = (target_ang - player_ang) & 0xFFFF;
    if (d > 0x8000) d -= 0x10000;
    return (d < 0 ? -d : d) < 0x2400;
}

/* BAM angle from (dx,dz), matching engine: -(atan2*65536/2pi) & 0xFFFF. */
static i32 bam_angle(i32 dx, i32 dz) {
    double a = atan2((double)dz, (double)dx) * 65536.0 / (2.0 * M_PI);
    return (i32)(-(i32)a) & 0xFFFF;
}

/* Log a callback the first time it fires (avoid per-frame spam). */
#define CBLOG(name)                                                          \
    do { static i32 w = 0; if (!w) { w = 1;                                  \
        print("lieu_cb: %s (stub)\n", name); } } while (0)

/* -------------------------------------------------------------------------
 * Callback stubs. Signatures match lieu_cb_t exactly.
 * Queries return 0 (=> guarded handler branches stay inactive for now).
 * TODO wire: map each to a real dke engine primitive.
 * ---------------------------------------------------------------------- */
static void cb_named_object(const c8 *name) { CBLOG("named_object"); (void)name; }
static void cb_named_object_int(const c8 *name, i32 v) {
    (void)v;
    /* L-code line ids ("L01D02", "L01D17A") are dialogue lines -> speak. */
    if (name && name[0] == 'L' && name[1] >= '0' && name[1] <= '9' && name[3] == 'D')
        dlg_say(name);
}
static void cb_named_two_ints(i32 id, i32 p) { CBLOG("named_two_ints"); (void)id; (void)p; }
/* Cinematique(name): the LIEU script asks to play cutscene <name> (e.g.
 * "SC22_500"). Map to "SCnn.CRH" and QUEUE it — the movie player must run
 * from the render phase (scene_render), not from here (called mid INIT /
 * mid update; playing synchronously re-enters the frame loop). */
static c8  g_lieu_movie[64];
static i32 g_lieu_movie_pending = 0;
static void cb_cinematique(const c8 *name) {
    c8 base[32];
    i32 i = 0;
    while (name[i] && name[i] != '_' && i < 30) { base[i] = name[i]; i++; }
    base[i] = '\0';
    snprintf(g_lieu_movie, sizeof(g_lieu_movie), "%s.CRH", base);
    g_lieu_movie_pending = 1;
    print("lieu_cb: cinematique '%s' -> queued %s\n", name, g_lieu_movie);
}

/* Drained by scene_render: returns a queued cutscene path (or NULL). */
const c8 *lieu_poll_movie(void) {
    if (!g_lieu_movie_pending) return NULL;
    g_lieu_movie_pending = 0;
    return g_lieu_movie;
}
/* slot 5 sub_44502D "PosPerso": position persona <name> at (x,y), view v.
 * The engine enqueues this as an action (dword_4FFB50) processed by the
 * persona/action system. dke has no persona render yet, so we record each
 * placement into a registry (for a future render pass) and log it. */
/* PosPerso arg mapping (verified vs engine sub_44502D/sub_463C2E):
 *   anim(p1) -> world X (tile, x600),  x(p2) -> world Z (tile, x600),
 *   y(p3) = facing (NOT position),     flag(p4) = vie (0..4 mode). */
typedef struct { c8 name[24]; i32 wx, wz, view, id; } lieu_spawn_t;
#define LIEU_SPAWN_MAX 64
static lieu_spawn_t g_spawns[LIEU_SPAWN_MAX];
static i32          g_spawn_count = 0;

static void cb_spawn_678(i32 zone, i32 anim, i32 x, i32 y, i32 flag,
                         const c8 *name, i32 kind, i32 z) {
    (void)y; (void)kind; (void)z;   /* y = facing; kind/z = life indices */
    if (name && g_spawn_count < LIEU_SPAWN_MAX) {
        lieu_spawn_t *s = &g_spawns[g_spawn_count++];
        i32 k = 0;
        while (name[k] && k < (i32)sizeof(s->name) - 1) { s->name[k] = name[k]; k++; }
        s->name[k] = '\0';
        s->wx = anim; s->wz = x; s->view = flag; s->id = zone;
    }
    print("lieu_cb: PosPerso '%s' id=%d vie=%d worldXZ=(%d,%d)\n",
          name ? name : "(null)", zone, flag, anim, x);
}

/* Registry accessors: wx and wz = world tile coords (multiply by 600 for
 * world units, then project). */
i32 lieu_spawn_count(void) { return g_spawn_count; }
const c8 *lieu_spawn_get(i32 i, i32 *wx, i32 *wz, i32 *view) {
    if (i < 0 || i >= g_spawn_count) return NULL;
    if (wx)   *wx   = g_spawns[i].wx;
    if (wz)   *wz   = g_spawns[i].wz;
    if (view) *view = g_spawns[i].view;
    return g_spawns[i].name;
}
static void cb_named_ex_988(const c8 *name, i32 p, i32 a1, i32 a2, i32 a3,
                            i32 a4, i32 a5, i32 a6, i32 a7) {
    CBLOG("named_ex_988"); (void)name; (void)p; (void)a1; (void)a2;
    (void)a3; (void)a4; (void)a5; (void)a6; (void)a7;
}
static void cb_actor_934(i32 zone, i32 facing, i32 a2, i32 a3, i32 a4) {
    CBLOG("actor_934"); (void)zone; (void)facing; (void)a2; (void)a3; (void)a4;
}
static void cb_cb_458(i32 a, i32 b) { CBLOG("cb_458"); (void)a; (void)b; }
static void cb_cb_984(i32 a, i32 b) { CBLOG("cb_984"); (void)a; (void)b; }
static void cb_place_95C(const c8 *name, i32 id, i32 coord) { CBLOG("place_95C"); (void)name; (void)id; (void)coord; }
/* slot 12 sub_445765 AjouteZone(id, cx, cz, sizeX, sizeZ, flag): register
 * a static zone rectangle (spatial args scaled tile->world by 600). */
static void cb_spawn_978(i32 id, i32 param, i32 x, i32 y, i32 z, i32 flag) {
    if (id >= 1 && id < LIEU_ZONE_COUNT) {
        g_zones[id].center_x = param * LIEU_ZONE_SCALE;
        g_zones[id].center_z = x     * LIEU_ZONE_SCALE;
        g_zones[id].size_x   = y     * LIEU_ZONE_SCALE;
        g_zones[id].size_z   = z     * LIEU_ZONE_SCALE;
        g_zones[id].active   = flag;
    }
}
/* slot 13 sub_4458AD RetireZone(id): clear a zone slot. */
static void cb_cb_93C(i32 id) {
    if (id >= 1 && id < LIEU_ZONE_COUNT) g_zones[id].center_x = -1;
}
static void cb_cb_970(i32 a, i32 b) { CBLOG("cb_970"); (void)a; (void)b; }
/* query_954 "DirectionPerso": needs the character/named-object table
 * (dke has no object entities yet) -> 0 until they exist. */
static i32  cb_query_954(i32 a) { (void)a; return 0; }
/* query_944 "DirectionZone": is the player facing zone n (within ~50.6 deg)? */
static i32  cb_query_944(i32 n) {
    i32 px, pz;
    if (n < 1 || n >= LIEU_ZONE_COUNT || g_zones[n].center_x == -1) return 0;
    if (!player_get_pos(&px, NULL, &pz)) return 0;
    i32 target = bam_angle(g_zones[n].center_x - px, g_zones[n].center_z - pz);
    return facing_ok((i32)player_get_facing(), target);
}
/* query_998 "DirectionSpec": is the player facing absolute angle n? */
static i32  cb_query_998(i32 angle) {
    return facing_ok((i32)player_get_facing(), angle & 0xFFFF);
}
/* query_990: distance between zone a and b, world units / 600. */
static i32  cb_query_990(i32 a, i32 b) {
    if (a < 1 || a >= LIEU_ZONE_COUNT || b < 1 || b >= LIEU_ZONE_COUNT) return 0;
    double dx = (double)(g_zones[a].center_x - g_zones[b].center_x);
    double dz = (double)(g_zones[a].center_z - g_zones[b].center_z);
    return (i32)(sqrt(dx * dx + dz * dz) / LIEU_ZONE_SCALE);
}
static void cb_cb_98C(const c8 *name, i32 p, i32 a1, i32 a2, i32 a3) {
    CBLOG("cb_98C"); (void)name; (void)p; (void)a1; (void)a2; (void)a3;
}
static void cb_group(i32 a) { CBLOG("group"); (void)a; }
static void cb_cb_454(i32 a) { CBLOG("cb_454"); (void)a; }
static void cb_select_468(i32 a) { CBLOG("select_468"); (void)a; }
static void cb_cb_964(i32 a, i32 b) { CBLOG("cb_964"); (void)a; (void)b; }
static void cb_debug_text(const c8 *t, i32 edx, i32 len) { (void)t; (void)edx; (void)len; }
static void cb_flush_958(i32 zone, i32 sub) { CBLOG("flush_958"); (void)zone; (void)sub; }
/* cb_46C sets the state slot the handler passed (guard reset / field set). */
static void cb_cb_46C(i32 *slot, i32 edx) { if (slot) *slot = edx; }
/* query_940 sub_4469BA: random integer in [0, n]. Engine ls_random uses a
 * 15-bit rand (0..32767); mask host rand() (31-bit) to match the range. */
static i32  cb_query_940(i32 n) {
    return (i32)(((u32)n * ((u32)rand() & 0x7FFFu)) / 32767u) & 0xFFFF;
}
static void cb_state_toggle(i32 mode, i32 layer) { CBLOG("state_toggle"); (void)mode; (void)layer; }
static void cb_attach_99C(i32 zone, const c8 *name, i32 a, i32 b) {
    CBLOG("attach_99C"); (void)zone; (void)name; (void)a; (void)b;
}
static i32  cb_layer_op(i32 op, i32 layer, i32 arg) { CBLOG("layer_op"); (void)op; (void)layer; (void)arg; return 0; }
static void cb_cb_950(i32 a) { CBLOG("cb_950"); (void)a; }
static void cb_cb_97C(i32 a, i32 b) { CBLOG("cb_97C"); (void)a; (void)b; }
static void cb_op_9c(i32 op, i32 a, i32 arg) { CBLOG("op_9c"); (void)op; (void)a; (void)arg; }

/* Concrete vtable. Unlisted (void*) slots stay NULL — the ported handlers
 * never call them. */
static const lieu_cb_t g_lieu_cb_impl = {
    .named_object     = cb_named_object,
    .named_object_int = cb_named_object_int,
    .named_two_ints   = cb_named_two_ints,
    .cinematique      = cb_cinematique,
    .spawn_678        = cb_spawn_678,
    .named_ex_988     = cb_named_ex_988,
    .actor_934        = cb_actor_934,
    .cb_458           = cb_cb_458,
    .cb_984           = cb_cb_984,
    .place_95C        = cb_place_95C,
    .spawn_978        = cb_spawn_978,
    .cb_93C           = cb_cb_93C,
    .cb_970           = cb_cb_970,
    .query_954        = cb_query_954,
    .query_944        = cb_query_944,
    .query_998        = cb_query_998,
    .query_990        = cb_query_990,
    .cb_98C           = cb_cb_98C,
    .group            = cb_group,
    .cb_454           = cb_cb_454,
    .select_468       = cb_select_468,
    .cb_964           = cb_cb_964,
    .debug_text       = cb_debug_text,
    .flush_958        = cb_flush_958,
    .cb_46C           = cb_cb_46C,
    .query_940        = cb_query_940,
    .state_toggle     = cb_state_toggle,
    .attach_99C       = cb_attach_99C,
    .layer_op         = cb_layer_op,
    .cb_950           = cb_cb_950,
    .cb_97C           = cb_cb_97C,
    .op_9c            = cb_op_9c,
};

/* -------------------------------------------------------------------------
 * Public driver — scene.c calls these.
 * ---------------------------------------------------------------------- */
void lieu_enter(i32 level_id) {
    g_lieu_level = level_id;
    g_lieu_cb    = &g_lieu_cb_impl;
    memset(g_lieu_state, 0, sizeof(g_lieu_state));
    zones_clear();   /* engine memsets dword_506C80 to -1 before INIT arms */
    g_spawn_count = 0;
    g_lieu_room_entered = 0;
    dlg_reset();
    /* Trace on via env LIEU_TRACE=1 (observe opcode dispatch at runtime). */
    g_lieu_trace = (getenv("LIEU_TRACE") != NULL);

    if (level_id != 1) {
        print("lieu: level %02d has no native scripting yet (only L01 ported)\n",
              level_id);
        return;
    }

    /* Seed the state fields the original engine sets before/around INIT for
     * a FRESH new-game entry. In dkev these come from game_init_46BEEB
     * (submode) and the level-entry code (entry phase). Values for
     * "new game -> level 1, first area":
     *   0x194 submode    = 0   (first sub-area)
     *   0x964 entry phase = 1  (trig00 fresh-entry setup path)
     *   0x0C  level-active flag (guards on most triggers read this)
     * TODO: drive these from the real dke game-init instead of hard-coding. */
    /* submode 4 = intro path: INIT plays the opening cutscene (SC22/SC23)
     * instead of the in-room spawn setup. TEMP bring-up value so new game
     * shows the intro FMV; the real entry submode comes from engine state
     * once the LIEU state buffer is mapped onto live dke gameplay state. */
    LIEU_ST(g_lieu_state, 0x194) = 4;
    LIEU_ST(g_lieu_state, 0x964) = 1;
    LIEU_ST(g_lieu_state, 0x0C)  = 1;

    print("lieu: LIEU01 native scripting — running INIT\n");
    lieu01_fonction_editor(LIEU_OP_INIT, g_lieu_state, 0);   /* op 1 */
}

/*
 * lieu_detect_zone — per-frame zone lookup feeding op 3, mirroring the
 * original engine's sub_449F9F: read player position (player_entity[0x50]
 * +0x3C/+0x44 = X/Z) then sub_449E03(X,Z) -> zone id (or -1).
 *
 * dke has no live player transform or level zone geometry yet, so this
 * returns -1 (no zone -> no trigger). Wire it when those exist:
 *   1. export player_entity + its transform,
 *   2. load L01 zone rectangles (LCOD table),
 *   3. point-in-zone test -> zone id.
 */
i32 lieu_detect_zone(void) {
    i32 px, pz;
    if (!player_get_pos(&px, NULL, &pz)) return -1;
    for (i32 i = 1; i < LIEU_ZONE_COUNT; i++) {          /* first hit wins  */
        if (g_zones[i].center_x == -1) continue;         /* empty slot      */
        if (g_zones[i].active != 1 && g_force_zones != 1) continue;
        i32 dx = g_zones[i].center_x - px;
        i32 dz = g_zones[i].center_z - pz;
        i32 hx = g_zones[i].size_x / 2, hz = g_zones[i].size_z / 2;
        if ((dx < 0 ? -dx : dx) > hx) continue;
        if ((dz < 0 ? -dz : dz) > hz) continue;
        return i;                                        /* == trigger idx  */
    }
    return -1;
}

void lieu_tick(void) {
    /* Per-frame: engine calls op -1 every frame (drain + tick_body). */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_TICK, g_lieu_state, 0);   /* op -1 */
}

void lieu_exit(void) {
    /* Teardown: engine op 2 (DLL handler is a no-op). */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_TEARDOWN, g_lieu_state, 0); /* op 2 */
    g_lieu_level = 0;
}

/*
 * lieu_enter_room — one-shot transition after the intro cutscene: switch to
 * the in-room submode (0) and re-run INIT so trig00 places the arrival
 * characters + registers zones. Mirrors the original intro->room flow.
 * Called by scene_render once the queued intro movie has played.
 */
void lieu_enter_room(void) {
    if (g_lieu_level != 1 || g_lieu_room_entered) return;
    g_lieu_room_entered = 1;
    LIEU_ST(g_lieu_state, 0x194) = 0;   /* in-room submode */
    zones_clear();
    g_spawn_count = 0;
    print("lieu: intro done -> entering room (submode 0), re-running INIT\n");
    lieu01_fonction_editor(LIEU_OP_INIT, g_lieu_state, 0);
    print("lieu: room INIT placed %d personas\n", g_spawn_count);
    lieu_zone_debug();

    /* DEMO: queue the opening exchange so the dialogue pipeline (subtitle +
     * WAV) is exercised on new game. The trigger-driven conversation is
     * gated on gameplay state (zone entry / facing) dke can't produce yet;
     * these lines are the real level-1 opening from NWDIAL01. */
    dlg_say("L01D02");    /* "Kalhi, tu dors?" */
    dlg_say("L01D17A");
}

/* -------------------------------------------------------------------------
 * Runtime event feeders. The original engine drives these from zone
 * detection (sub_449F9F -> op 3), object timers (op 4) and a pending-event
 * flag (op 5). dke has none of those subsystems yet, so nothing calls the
 * feeders during normal play — call them from a debug keybind (or wire to
 * real engine systems later) to exercise the trigger / event handlers.
 * ---------------------------------------------------------------------- */
void lieu_zone_trigger(i32 zone_id) {   /* op 3: run trigger[zone_id]      */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_EVENT, g_lieu_state, zone_id);
}
void lieu_object_timer(i32 id) {        /* op 4: event4(id)                */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_CALL4, g_lieu_state, id);
}
void lieu_pending_event(i32 v) {        /* op 5: event5(v)                 */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_CALL5, g_lieu_state, v);
}

/* Read/write the live state buffer (so the engine can populate the fields
 * handler guards read: 0xC, 0x18, 0x194 submode, 0x964 phase, ...). */
i32 *lieu_state_slot(i32 off) { return &LIEU_ST(g_lieu_state, off); }
