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

/* Facing helpers — currently unused (facing queries are permissive, see
 * cb_query_944/954/998), kept for when player-facing input lands. */
/* Facing tolerance ~50.6 deg (0x2400 BAM). Signed shortest-arc compare. */
static i32 __attribute__((unused)) facing_ok(i32 player_ang, i32 target_ang) {
    i32 d = (target_ang - player_ang) & 0xFFFF;
    if (d > 0x8000) d -= 0x10000;
    return (d < 0 ? -d : d) < 0x2400;
}

/* BAM angle from (dx,dz), matching engine: -(atan2*65536/2pi) & 0xFFFF. */
static i32 __attribute__((unused)) bam_angle(i32 dx, i32 dz) {
    double a = atan2((double)dz, (double)dx) * 65536.0 / (2.0 * M_PI);
    return (i32)(-(i32)a) & 0xFFFF;
}

/* Log a callback the first time it fires (avoid per-frame spam). */
#define CBLOG(name)                                                          \
    do { static i32 w = 0; if (!w) { w = 1;                                  \
        print("lieu_cb: %s (stub)\n", name); } } while (0)

/* -------------------------------------------------------------------------
 * Player movement blocking (group open/close brackets).
 * group(3) = open bracket (block player), group(0) = close bracket.
 * Original engine queues these as actions, but we apply directly since
 * dke has no action queue processor.
 * ---------------------------------------------------------------------- */
static i32 g_player_blocked = 0;
i32 lieu_player_blocked(void) { return g_player_blocked; }

/* -------------------------------------------------------------------------
 * State toggle layers (3 independent dialogue/scripting queues).
 * Original: mode 0→dword_4FC194, mode 1→dword_507274, mode 2→dword_507270.
 * Each has a mode byte (0/1/2) and a counter. layer=2 means "reset".
 * ---------------------------------------------------------------------- */
static i32 g_layer_mode[3]    = {0, 0, 0};
static i32 g_layer_counter[3] = {0, 0, 0};

/* -------------------------------------------------------------------------
 * Entity state tracking. The original engine tracks entity state in
 * per-entity structs (0x220-stride objects at dword_4DF5D0 for IDs 0-3,
 * separate NPC array for higher IDs). LIEU queries entity alive, HP,
 * visibility, combat readiness, and anim state via layer_op.
 *
 * We track a lightweight version: alive, HP (0x400 = full), visibility,
 * combat ready, anim state per entity. Spawn/kill callbacks update this.
 * ---------------------------------------------------------------------- */
#define ENT_MAX 256
typedef struct {
    i32 alive;        /* 1 = loaded/present */
    i32 hp;           /* 0..0x3FF (1023); layer_op(6) reads/writes */
    i32 visible;      /* 1 = visible; layer_op(12) toggles */
    i32 combat_ready; /* 1 = in combat mode; layer_op(18) tests */
    i32 anim_state;   /* layer_op(6) r/w with entity anim tag */
} ent_state_t;
static ent_state_t g_ents[ENT_MAX];

static void ents_clear(void) {
    for (i32 i = 0; i < ENT_MAX; i++) {
        g_ents[i].alive = 0;
        g_ents[i].hp = 0x200;
        g_ents[i].visible = 1;
        g_ents[i].combat_ready = 0;
        g_ents[i].anim_state = 0;
    }
    for (i32 i = 0; i < 4; i++) { g_ents[i].alive = 1; g_ents[i].hp = 0x3FF; }
    g_ents[12].alive = 1;
    g_ents[12].hp = 0x3FF;
}

static void ent_spawn(i32 id) {
    if (id >= 0 && id < ENT_MAX) {
        g_ents[id].alive = 1;
        if (g_ents[id].hp <= 0) g_ents[id].hp = 0x200;
        g_ents[id].visible = 1;
    }
}

static void ent_kill(i32 id) {
    if (id >= 0 && id < ENT_MAX) {
        g_ents[id].alive = 0;
        g_ents[id].hp = 0;
        g_ents[id].combat_ready = 0;
    }
}

static i32 g_op9c_counter = 0;

/* -------------------------------------------------------------------------
 * Callback implementations. Signatures match lieu_cb_t exactly.
 * ---------------------------------------------------------------------- */

/* slot 0 sub_44453E "Animation": dispatch by name prefix.
 * 'a' = actor anim, 'p'/'g' = placement, 's' = sound, '2' = 2D sprite. */
static void cb_named_object(const c8 *name) {
    if (!name || !name[0]) return;
    c8 low = name[0];
    if (low >= 'A' && low <= 'Z') low += 32;
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: named_object first='%c' name='%s'\n", low, name); }
    (void)low;
}

/* slot 1 sub_4449A5 "Dialogue": enqueue dialogue line. */
static void cb_named_object_int(const c8 *name, i32 v) {
    (void)v;
    if (name && name[0] == 'L' && name[1] >= '0' && name[1] <= '9' && name[3] == 'D')
        dlg_say(name);
}

/* slot 2 sub_444A3E "Evenement": sound/event trigger.
 * id = event source, param = -1 to arm, positive to trigger. */
static void cb_named_two_ints(i32 id, i32 p) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: named_two_ints id=%d param=%d\n", id, p); }
    (void)id; (void)p;
}
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
typedef struct {
    c8  name[24];
    i32 wx, wz, view, id;   /* wx=anim tile (*600=worldX), wz=x tile (*600=worldZ) */
    i32 ground;             /* PosPerso arg8 z = world Y ground (unscaled)         */
    i32 facing;             /* PosPerso arg4 y = facing angle                      */
} lieu_spawn_t;
#define LIEU_SPAWN_MAX 64
static lieu_spawn_t g_spawns[LIEU_SPAWN_MAX];
static i32          g_spawn_count = 0;

/* PosPerso (sub_44502D): anim*600=worldX, x*600=worldZ, y=facing, z=world Y. */
static void cb_spawn_678(i32 zone, i32 anim, i32 x, i32 y, i32 flag,
                         const c8 *name, i32 kind, i32 z) {
    (void)kind;
    if (name && g_spawn_count < LIEU_SPAWN_MAX) {
        lieu_spawn_t *s = &g_spawns[g_spawn_count++];
        i32 k = 0;
        while (name[k] && k < (i32)sizeof(s->name) - 1) { s->name[k] = name[k]; k++; }
        s->name[k] = '\0';
        s->wx = anim; s->wz = x; s->view = flag; s->id = zone;
        s->ground = z; s->facing = y;
    }
    /* Mark entity as alive/spawned. vie==2 means dead/cadaver placement. */
    if (zone >= 0 && zone < ENT_MAX) {
        if (flag == 2)
            ent_kill(zone);
        else
            ent_spawn(zone);
    }
    print("lieu_cb: PosPerso '%s' id=%d vie=%d tileXZ=(%d,%d) facing=%d groundY=%d\n",
          name ? name : "(null)", zone, flag, anim, x, y, z);
}

/* Ground Y + facing for spawn i (see lieu_spawn_get for name/tile/view). */
i32 lieu_spawn_ground(i32 i) {
    return (i >= 0 && i < g_spawn_count) ? g_spawns[i].ground : 0;
}
i32 lieu_spawn_facing(i32 i) {
    return (i >= 0 && i < g_spawn_count) ? g_spawns[i].facing : 0;
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
/* slot 6 sub_445189 "EtatObjet": object state/placement with 7 stack args.
 * Uppercases name, stores position (tile*600) and flags into action slot.
 * Without action processor: log and no-op. */
static void cb_named_ex_988(const c8 *name, i32 p, i32 a1, i32 a2, i32 a3,
                            i32 a4, i32 a5, i32 a6, i32 a7) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: named_ex_988 '%s' p=%d\n", name ? name : "", p); }
    (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7;
}

/* slot 8 sub_4453E1 "Joindre": make actor move to position.
 * Negative actor_id forces join even if already walking.
 * Without entity system: log only. */
static void cb_actor_934(i32 zone, i32 facing, i32 a2, i32 a3, i32 a4) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: actor_934 id=%d facing=%d\n", zone, facing); }
    (void)a2; (void)a3; (void)a4;
}
/* ChangeCamera (slot 9, dkev sub_4454F2): a = camera index, b = mode
 * (1 = auto-follow, 2 = forced/locked). The LIEU script drives the in-game
 * camera through this (e.g. cb_458(6,1) sets the opening bedroom view).
 * Queue it; the scene loop applies it via lieu_poll_camera_change. */
static i32 g_next_cam = -1;
static i32 g_next_cam_mode = 1;
static void cb_cb_458(i32 a, i32 b) {
    print("lieu_cb: ChangeCamera camera=%d mode=%d\n", a, b);
    if (a >= 0) { g_next_cam = a; g_next_cam_mode = b; }
}

/* Pending camera the script requested (or -1). Clears on read; *mode gets the
 * ChangeCamera mode (2 = forced -> disable auto-switch until next change). */
i32 lieu_poll_camera_change(i32 *mode) {
    i32 c = g_next_cam;
    if (c >= 0) { if (mode) *mode = g_next_cam_mode; g_next_cam = -1; }
    return c;
}
/* CHANGEPIECE (opcode 10, dkev sub_4455EE): a = target level id, b = entry
 * point id. Queues a room change unless the target is the current level or
 * negative (which the engine treats as a same-room reposition). The main
 * scene loop drains this via lieu_poll_level_change and reloads the level. */
static i32 g_next_level = -1;
static i32 g_next_entry = 0;
static void cb_cb_984(i32 a, i32 b) {
    print("lieu_cb: CHANGEPIECE target=%d entry=%d (cur=%d)\n", a, b, g_lieu_level);
    if (a >= 0 && a != g_lieu_level) { g_next_level = a; g_next_entry = b; }
}

/* Pending room change target (or -1). Clears on read; fills *entry. */
i32 lieu_poll_level_change(i32 *entry) {
    i32 t = g_next_level;
    if (t >= 0) { if (entry) *entry = g_next_entry; g_next_level = -1; }
    return t;
}
/* slot 11 sub_44569A "Bruitage": sound placement.
 * If name starts with 'Z', calls sub_44D23D (zone sound). Otherwise enqueues
 * action type 11 with name/id/coord. */
static void cb_place_95C(const c8 *name, i32 id, i32 coord) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: place_95C '%s' id=%d coord=%d\n", name ? name : "", id, coord); }
    (void)id; (void)coord;
}
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
/* slot 14 sub_445919 "Combat": enqueue combat action.
 * a = entity ID, b = combat mode (-1 = engage, 0 = assign target).
 * Marks entity as combat-ready. */
static void cb_cb_970(i32 a, i32 b) {
    print("lieu_cb: cb_970 (combat) entity=%d mode=%d\n", a, b);
    if (a >= 0 && a < ENT_MAX) {
        if (b == -1)
            g_ents[a].combat_ready = 1;
        else
            g_ents[a].combat_ready = 0;
    }
}
/* Facing tests (DirectionPerso/Zone/Spec). dke has no player-facing control
 * yet, so these are PERMISSIVE (return 1 = "facing satisfied") — the RE
 * recommendation, so zone/conversation triggers fire on entry rather than
 * never. Restore the real facing math once facing input + math land. */
static i32  cb_query_954(i32 a) { (void)a; return 1; }   /* DirectionPerso */
static i32  cb_query_944(i32 n) { (void)n; return 1; }   /* DirectionZone  */
static i32  cb_query_998(i32 a) { (void)a; return 1; }   /* DirectionSpec  */
/* query_990: distance between zone a and b, world units / 600. */
static i32  cb_query_990(i32 a, i32 b) {
    if (a < 1 || a >= LIEU_ZONE_COUNT || b < 1 || b >= LIEU_ZONE_COUNT) return 0;
    double dx = (double)(g_zones[a].center_x - g_zones[b].center_x);
    double dz = (double)(g_zones[a].center_z - g_zones[b].center_z);
    return (i32)(sqrt(dx * dx + dz * dz) / LIEU_ZONE_SCALE);
}
/* slot 19 sub_446159 "AnimObjet": object animation with params.
 * Enqueues action type 19 with name/coords/flags. */
static void cb_cb_98C(const c8 *name, i32 p, i32 a1, i32 a2, i32 a3) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: cb_98C '%s' p=%d\n", name ? name : "", p); }
    (void)a1; (void)a2; (void)a3;
}

/* slot 20 sub_446227 "Bloque": dialogue/cutscene group brackets.
 * group(3) = begin group (block player movement + camera).
 * group(0) = end group (close bracket).
 * Negative values (not used in L01) have immediate effects: bit 1=unlock
 * movement, bit 2=unlock camera, bit 4=reset combat state.
 * Since dke has no action queue, we apply the blocking directly. */
static void cb_group(i32 a) {
    if (a < 0) {
        i32 flags = -a;
        if (flags & 1) g_player_blocked = 0;
        if (flags & 2) { /* camera unlock — no-op for now */ }
        if (flags & 4) { /* combat reset — no-op for now */ }
        print("lieu_cb: group unblock flags=0x%X\n", flags);
    } else if (a == 3) {
        g_player_blocked = 1;
    } else if (a == 0) {
        g_player_blocked = 0;
    }
}

/* slot 21 sub_44637B "Fouille": remove entity and clear its zone slots.
 * In the original, iterates 41 zone slots and clears any whose entity
 * pointer matches. Also marks entity as no longer alive. */
static void cb_cb_454(i32 a) {
    print("lieu_cb: cb_454 (fouille) entity=%d\n", a);
    if (a >= 0 && a < ENT_MAX) ent_kill(a);
}

/* slot 22 sub_44648B: camera select (positive) or actor stop (negative).
 * Positive a: enqueues camera/zone select action (type 22).
 * Negative a: IMMEDIATE — stops actor movement (clears entity walk/join state).
 * Without entity system, negative branch logs only. */
static void cb_select_468(i32 a) {
    if (a < 0) {
        print("lieu_cb: select_468 stop actor %d\n", -a);
    } else {
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: select_468 select=%d\n", a); }
    }
}

/* slot 23 sub_44660D: flag/state set.
 * Enqueues action type 23 with flag_id and value. */
static void cb_cb_964(i32 a, i32 b) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: cb_964 flag=%d val=%d\n", a, b); }
    (void)a; (void)b;
}

/* slot 25: debug/subtitle text — no-op. */
static void cb_debug_text(const c8 *t, i32 edx, i32 len) { (void)t; (void)edx; (void)len; }

/* slot 27 sub_446808 "JoindrePerm": permanent actor join.
 * Like actor_934 (Joindre) but persistent. Negative zone forces join. */
static void cb_flush_958(i32 zone, i32 sub) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: flush_958 zone=%d sub=%d\n", zone, sub); }
    (void)zone; (void)sub;
}

/* slot 28: cb_46C sets the state slot the handler passed. */
static void cb_cb_46C(i32 *slot, i32 edx) { if (slot) *slot = edx; }

/* slot 29: query_940 — random integer in [0, n]. */
static i32  cb_query_940(i32 n) {
    return (i32)(((u32)n * ((u32)rand() & 0x7FFFu)) / 32767u) & 0xFFFF;
}

/* slot 30 sub_446A05: state_toggle(mode, layer).
 * Manages 3 independent dialogue layer tables. When layer=2 the table is
 * reset (memset to -1) and mode set to 1 with counter 0. */
static void cb_state_toggle(i32 mode, i32 layer) {
    if (mode < 0 || mode > 2) return;
    g_layer_mode[mode] = layer;
    if (layer == 2) {
        g_layer_mode[mode]    = 1;
        g_layer_counter[mode] = 0;
    }
}

/* slot 32 sub_446BE9 "LinkObjet": object attach/toggle.
 * Enqueues action type 32 with name/zone/flags. */
static void cb_attach_99C(i32 zone, const c8 *name, i32 a, i32 b) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: attach_99C zone=%d '%s'\n", zone, name ? name : ""); }
    (void)a; (void)b;
}

/* -------------------------------------------------------------------------
 * slot 34 sub_446E19 "layer_op": the engine's general-purpose operation
 * dispatcher (48-case switch). All cases called from L01 are implemented.
 *
 * Case map (op value → behaviour):
 *   0  Entity position query (same as 1 but via different struct path)
 *   1  Entity position: sub-arg 0=X/600, 1=Z/600, 2=facing(u16), 3=Y/600
 *   2  Combat ability check
 *   4  Sound/WAV file load (layer=idx|channel<<16, arg=flags)
 *   6  Entity HP/energy: arg<0 → read, arg>=0 → write
 *   9  Entity in zone test
 *  10  Timer interval set
 *  12  Entity visibility toggle
 *  18  Entity combat ready / alive check
 *  20  Counter read
 *  23  Entity movement stop
 *  24  Music control
 *  29  Entity alive/loaded check
 *  35  Reset/clear state
 *  37  Visual scale effect
 *  38  Visual parameter set
 * ---------------------------------------------------------------------- */
static i32 cb_layer_op(i32 op, i32 layer, i32 arg) {
    switch (op) {
    case 0:
    case 1: {
        for (i32 i = 0; i < g_spawn_count; i++) {
            if (g_spawns[i].id == layer) {
                switch (arg) {
                case 0:  return g_spawns[i].wx;
                case 1:  return g_spawns[i].wz;
                case 2:  return (u16)g_spawns[i].facing;
                case 3:  return g_spawns[i].ground / 600;
                default: return 0;
                }
            }
        }
        if (layer >= 0 && layer < 4) {
            i32 px, pz;
            if (player_get_pos(&px, NULL, &pz)) {
                switch (arg) {
                case 0:  return px / 600;
                case 1:  return pz / 600;
                case 2:  return 0;
                default: return 0;
                }
            }
        }
        return 0;
    }
    case 2:
        return 0;
    case 4: {
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: layer_op(4) sound idx=%d arg=0x%X\n", layer, arg); }
        return 0;
    }
    case 6: {
        /* HP/energy read (arg < 0) or write (arg >= 0).
         * layer = entity ID. arg=-1 → return current HP. */
        if (layer >= 0 && layer < ENT_MAX) {
            if (arg < 0) return g_ents[layer].hp;
            g_ents[layer].hp = arg;
            if (arg <= 0) ent_kill(layer);
        }
        return 0;
    }
    case 9: {
        if (arg < 1 || arg >= LIEU_ZONE_COUNT) return 0;
        if (g_zones[arg].center_x == -1) return 0;
        i32 ex = 0, ez = 0;
        for (i32 i = 0; i < g_spawn_count; i++) {
            if (g_spawns[i].id == layer) {
                ex = g_spawns[i].wx * 600;
                ez = g_spawns[i].wz * 600;
                goto got_pos;
            }
        }
        if (layer >= 0 && layer < 4) {
            player_get_pos(&ex, NULL, &ez);
        } else {
            return 0;
        }
    got_pos: ;
        i32 dx = g_zones[arg].center_x - ex;
        i32 dz = g_zones[arg].center_z - ez;
        i32 hx = g_zones[arg].size_x / 2, hz = g_zones[arg].size_z / 2;
        if ((dx < 0 ? -dx : dx) > hx) return 0;
        if ((dz < 0 ? -dz : dz) > hz) return 0;
        return 1;
    }
    case 10: {
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: layer_op(10) timer layer=%d arg=%d\n", layer, arg); }
        return 0;
    }
    case 12:
        /* Entity visibility: layer = entity ID, arg = 0 (hide) / 1 (show). */
        if (layer >= 0 && layer < ENT_MAX)
            g_ents[layer].visible = arg;
        return 0;
    case 18:
        /* Entity combat ready / alive. Returns 1 if the entity is alive and
         * in combat-capable state. Used in tick_body to gate imp appearance
         * (layer_op(0x12, 0x57, 0) checks if imp W is alive). */
        if (layer >= 0 && layer < ENT_MAX)
            return g_ents[layer].alive;
        return 0;
    case 20:
        /* Counter read. Returns the deferred counter value. */
        return g_op9c_counter;
    case 23: {
        /* Entity movement stop. Clears walk/join state. */
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: layer_op(23) stop entity=%d\n", layer); }
        return 0;
    }
    case 24: {
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: layer_op(24) music idx=%d cmd=%d\n", layer, arg); }
        return 0;
    }
    case 29:
        /* Entity alive/loaded check. Returns 1 if the entity is present
         * (spawned and not dead). Critical for imp appearance logic and
         * party member checks in tick_body. */
        if (layer >= 0 && layer < ENT_MAX)
            return g_ents[layer].alive;
        return 0;
    case 35:
        return 0;
    case 37: {
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: layer_op(37) visual scale layer=%d\n", layer); }
        return 0;
    }
    case 38: {
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: layer_op(38) visual param layer=%d arg=0x%X\n", layer, arg); }
        return 0;
    }
    default:
        return 0;
    }
}

/* slot 36 sub_443273: enqueues action type 39 with ecx at action[0x48].
 * Without action processor: no-op. */
static void cb_cb_950(i32 a) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: cb_950 a=%d\n", a); }
    (void)a;
}

/* slot 37 sub_44356F: enqueues action type 35 with a1 at action[0x70] and
 * a2 at action[0x48]. Guards a1 < 20. */
static void cb_cb_97C(i32 a, i32 b) {
    static i32 w = 0;
    if (!w) { w = 1; print("lieu_cb: cb_97C a=%d b=%d\n", a, b); }
    (void)a; (void)b;
}

/* slot 39 sub_446D84 "Diversempile": deferred action dispatch.
 * Original enqueues action type 37 ("DiversEmpile") with op/a/arg at
 * action[0x14/0x18/0x1C]. The action processor switches on op to apply
 * effects. We apply the critical effects immediately since dke has no
 * deferred action queue. */
static void cb_op_9c(i32 op, i32 a, i32 arg) {
    switch (op) {
    case 0x06:
        /* Set entity anim state. a = entity ID, arg = state value. */
        if (a >= 0 && a < ENT_MAX) g_ents[a].hp = arg;
        break;
    case 0x0C:
        /* Show/hide companion. a = entity ID, arg = 0 (hide) / 1 (show). */
        if (a >= 0 && a < ENT_MAX) g_ents[a].visible = arg;
        break;
    case 0x10: {
        /* Camera mode set. a = camera mode, arg = sub-param. */
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: op_9c(0x10) camera mode=%d sub=%d\n", a, arg); }
        break;
    }
    case 0x12:
        /* Entity combat enable/disable. a = entity ID, arg = 0/1. */
        if (a >= 0 && a < ENT_MAX) g_ents[a].combat_ready = arg;
        break;
    case 0x14:
        /* Counter write. Stored for layer_op(20) reads. */
        g_op9c_counter = a;
        break;
    case 0x16: {
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: op_9c(0x16) a=%d arg=%d\n", a, arg); }
        break;
    }
    case 0x17: {
        /* Stop entity movement. a = entity ID. */
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: op_9c(0x17) stop entity=%d\n", a); }
        break;
    }
    case 0x18: {
        /* Sound/music control. a = sound index, arg = sub-command. */
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: op_9c(0x18) sound=%d cmd=%d\n", a, arg); }
        break;
    }
    case 0x1B: {
        /* Screen fade / transition. */
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: op_9c(0x1B) fade a=%d\n", a); }
        break;
    }
    case 0x1C: {
        /* Entity reset/remove. a = entity ID. */
        if (a >= 0 && a < ENT_MAX) ent_kill(a);
        break;
    }
    case 0x29: {
        /* Timer trigger. a = timer value. Deferred. */
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: op_9c(0x29) timer=%d\n", a); }
        break;
    }
    case 0x2A: {
        /* Long timer. */
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: op_9c(0x2A) long_timer a=%d\n", a); }
        break;
    }
    case 0x30:
        /* Action queue clear / sync point. Called 3x in sequence as a
         * flush barrier. No-op without action queue. */
        break;
    default: {
        static i32 w = 0;
        if (!w) { w = 1; print("lieu_cb: op_9c unknown op=%d a=%d arg=%d\n", op, a, arg); }
        break;
    }
    }
}

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
    ents_clear();    /* reset entity alive/HP/visibility state              */
    g_spawn_count = 0;
    g_op9c_counter = 0;
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
    lieu01_fonction_editor(LIEU_OP_INIT, g_lieu_state, 0, 0);   /* op 1 */
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
        lieu01_fonction_editor(LIEU_OP_TICK, g_lieu_state, 0, 0);   /* op -1 */
}

void lieu_exit(void) {
    /* Teardown: engine op 2 (DLL handler is a no-op). */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_TEARDOWN, g_lieu_state, 0, 0); /* op 2 */
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
    lieu01_fonction_editor(LIEU_OP_INIT, g_lieu_state, 0, 0);
    print("lieu: room INIT placed %d personas\n", g_spawn_count);
    lieu_zone_debug();
    /* Dialogue now fires from the trigger handlers on zone entry (op 3),
     * not demo-queued. Facing gates are permissive (see query_944/954/998)
     * since dke has no player-facing control yet. */
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
        lieu01_fonction_editor(LIEU_OP_EVENT, g_lieu_state, zone_id, 0);
}
void lieu_object_timer(i32 id) {        /* op 4: event4(id)                */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_CALL4, g_lieu_state, id, 0);
}
void lieu_pending_event(i32 v, i32 phase) { /* op 5: event5(v, phase)      */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_CALL5, g_lieu_state, v, phase);
}

/* Read/write the live state buffer (so the engine can populate the fields
 * handler guards read: 0xC, 0x18, 0x194 submode, 0x964 phase, ...). */
i32 *lieu_state_slot(i32 off) { return &LIEU_ST(g_lieu_state, off); }
