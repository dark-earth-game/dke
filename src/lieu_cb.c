/*
 * lieu_cb.c — Engine-side bridge for the LIEU level-scripting hand-port.
 *
 * Provides:
 *   - the 40-slot callback vtable g_lieu_cb that lieu01.c calls into,
 *   - the per-level state buffer (engine dword_505C40, 0xFA0 bytes),
 *   - lieu_enter / lieu_tick / lieu_exit for scene.c to drive.
 *
 * Two kinds of callback live here:
 *
 *  - DEFERRED. The original primitives (dkev sub_444xxx..sub_446xxx) do not
 *    act: they allocate an action node, fill it, and let the per-frame
 *    driver sub_44CDB8 @ 0x44CDB8 dispatch it one head node per pile per
 *    frame. Those now go through src/lieu_action.c and land in
 *    lieu_action_dispatch() at the bottom of this file — the port of the
 *    40-case switch in sub_44A58B @ 0x44A58B.
 *
 *  - IMMEDIATE. A handful of callbacks are queued in the original but are
 *    still applied straight away here: zones (AJOUTEZONE/RETIREZONE),
 *    camera (CHANGECAMERA), room change (CHANGEPIECE), persona placement
 *    (POSPERSO), dialogue, cutscenes and the BLOQUE brackets. INIT emits a
 *    burst of those and the scene code consumes them the same frame; moving
 *    them behind the queue would stagger level setup over ~40 frames. They
 *    stay immediate until the scene side can cope with staggered setup.
 *
 * Effects that need engine subsystems dke has not ported (positional audio,
 * 2D anim playback, object meshes) record their state in the small
 * registries below and log once, so scripts still advance.
 */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "libsys/common.h"
#include "lieu_cb.h"
#include "lieu_action.h"
#include "lieu01.h"
#include "player.h"
#include "dialogue.h"
#include "actor.h"
#include "state.h"

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
    void *entity;   /* dword_506CA0: actor the zone follows (POSPERSO), or NULL */
} zone_t;

#define LIEU_ZONE_COUNT 41
#define LIEU_ZONE_SCALE 600            /* AjouteZone tile->world (0x258)   */
static zone_t g_zones[LIEU_ZONE_COUNT];
static i32    g_force_zones = 0;       /* dword_4FFB80: inactive zones answer the action key */
static i32    g_action_key  = 0;       /* key 0x20 (Space) held this frame */
static i32    g_action_pressed = 0;    /* dword_505C4C */

/* sub_449F9F feeds the action key in; scene.c calls this every frame. */
void lieu_set_action_key(i32 down) { g_action_key = down; }

/* dword_4B52DC: -1 while the level script's INIT burst is being drained. */
static i32 g_lieu_loading = 0;
i32 lieu_loading(void) { return g_lieu_loading; }

static void zones_clear(void) {
    for (i32 i = 0; i < LIEU_ZONE_COUNT; i++) {
        g_zones[i].center_x = -1;
        g_zones[i].entity   = NULL;
    }
}

/* sub_463FB5 @ 0x464019: POSPERSO hangs a zone on the placed actor. The zone
 * then moves with it (sub_449E03 reads the actor's position as the centre). */
void lieu_zone_attach(i32 zone, void *entity, i32 size, i32 active) {
    if (zone < 0 || zone >= LIEU_ZONE_COUNT) return;
    g_zones[zone].entity = entity;
    g_zones[zone].size_x = size;
    g_zones[zone].size_z = size;
    g_zones[zone].active = active;
}

void lieu_zone_detach(void *entity) {
    for (i32 i = 1; i < LIEU_ZONE_COUNT; i++)
        if (g_zones[i].entity == entity) g_zones[i].entity = NULL;
}

void lieu_zone_debug(void) {
    i32 minx = 0x7FFFFFFF, maxx = -0x7FFFFFFF;
    i32 minz = 0x7FFFFFFF, maxz = -0x7FFFFFFF;
    i32 n = 0;

    for (i32 i = 1; i < LIEU_ZONE_COUNT; i++) {
        if (g_zones[i].center_x == -1) continue;
        print("zone[%d] c=(%d,%d) half=(%d,%d) active=%d\n", i,
              g_zones[i].center_x, g_zones[i].center_z,
              g_zones[i].size_x / 2, g_zones[i].size_z / 2, g_zones[i].active);
        if (g_zones[i].center_x < minx) minx = g_zones[i].center_x;
        if (g_zones[i].center_x > maxx) maxx = g_zones[i].center_x;
        if (g_zones[i].center_z < minz) minz = g_zones[i].center_z;
        if (g_zones[i].center_z > maxz) maxz = g_zones[i].center_z;
        n++;
    }

    /*
     * Coordinate-space check. AjouteZone rectangles, PosPerso placements and
     * the player transform must all live in the same space: LIEU script tile
     * x600 (LIEU_ZONE_SCALE, engine 0x258 — the same divisor the HUD uses).
     * Printing the three side by side makes any future drift obvious instead
     * of showing up as triggers that never fire.
     */
    if (n) {
        i32 px = 0, pz = 0;
        i32 have_player = player_get_pos(&px, NULL, &pz);
        print("lieu: zone space X[%d..%d] Z[%d..%d] over %d zones\n",
              minx, maxx, minz, maxz, n);
        for (i32 i = 0; i < lieu_spawn_count(); i++) {
            i32 tx = 0, tz = 0, view = 0;
            const c8 *nm = lieu_spawn_get(i, &tx, &tz, &view);
            print("lieu: spawn '%s' world=(%d,%d)\n", nm ? nm : "?",
                  tx * LIEU_ZONE_SCALE, tz * LIEU_ZONE_SCALE);
        }
        /* The player transform is seeded from the camera anchor after INIT,
         * so at room-entry time it is still (0,0) — report that rather than
         * a misleading "outside every zone". */
        if (have_player && (px || pz))
            print("lieu: player world=(%d,%d) zone=%d\n", px, pz, lieu_detect_zone());
        else if (have_player)
            print("lieu: player not placed yet (transform seeded after INIT)\n");
    }
}

/* -------------------------------------------------------------------------
 * Facing tests — DirectionPerso / DirectionZone / DirectionSpec.
 *
 * These gate most script triggers: "is the player looking at the thing?".
 * Decoded from sub_44599B @ 0x44599B (DirectionPerso) and sub_445C82 @
 * 0x445C82 (DirectionZone), which share the same maths:
 *
 *   target_ang = -(atan2(dx, dz) * 65536 / 2pi) & 0xFFFF
 *   diff       = signed shortest arc (target_ang - player_ang)
 *   facing     = abs(diff) < 0x2400            (+/- 50.6 degrees)
 *
 * Note the atan2 argument order: the engine passes the X delta FIRST
 * (asm 0x445BBE pushes dz then dx, so dx lands as argument 1).
 *
 * DirectionPerso additionally returns true regardless of angle when the
 * target is within 30000 world units (asm 0x445A8B compares the squared
 * distance against dbl_4B0678 = 9e8). DirectionZone has no such bypass.
 * ---------------------------------------------------------------------- */
#define LIEU_FACE_TOLERANCE  0x2400      /* +/- 50.6 degrees, BAM        */
#define LIEU_FACE_NEAR_SQ    900000000.0 /* dbl_4B0678: (30000 units)^2  */

/* Signed shortest-arc compare against the tolerance. */
static i32 facing_ok(i32 player_ang, i32 target_ang) {
    i32 d = (target_ang - player_ang) & 0xFFFF;
    if (d > 0x8000) d -= 0x10000;
    return (d < 0 ? -d : d) < LIEU_FACE_TOLERANCE;
}

/* BAM angle toward (dx,dz). The engine passes the X delta as atan2's first
 * argument (sub_44599B @ 0x445BBE), so keep that order here too. */
static i32 bam_angle(i32 dx, i32 dz) {
    double a = atan2((double)dx, (double)dz) * 65536.0 / (2.0 * M_PI);
    return (i32)(-(i32)a) & 0xFFFF;
}

/* Player heading in BAM. scene.c keeps the live heading in cosine-table
 * units (4096 per turn) and mirrors it into entity+0x1E0. */
static i32 player_facing_bam(void) {
    return (i32)player_get_facing() & 0xFFFF;
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
static i32 g_player_blocked = 0;   /* player_movement_enabled @ 0x4FF080 */
static i32 g_pile_actor[3];        /* dword_4B53F0: last ANIMATION actor per pile */
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
    i32 walking;      /* JOINDRE in flight (engine: entity+0xCBC != 0) */
    i32 joined;       /* JOINDREPERM in flight (engine: entity+0xCB4 != 0) */
    /* Walk destination handed over by JOINDRE (engine: entity+0xCC0/0xCC4/
     * 0xCC8, consumed by the entity movement system). */
    i32 dest_x, dest_z, dest_facing;
    u32 walk_ms;      /* last time the walk stepper advanced this actor */
} ent_state_t;
static ent_state_t g_ents[ENT_MAX];

/* Engine tests entity+0xCBC / +0xCB4 before accepting a new join
 * (sub_4453E1 @ 0x445446, sub_446808 @ 0x446871). */
static i32 ent_is_walking(i32 id) {
    return (id >= 0 && id < ENT_MAX) ? g_ents[id].walking : 0;
}
static i32 ent_is_joined(i32 id) {
    return (id >= 0 && id < ENT_MAX) ? g_ents[id].joined : 0;
}

static void ents_clear(void) {
    for (i32 i = 0; i < ENT_MAX; i++) {
        g_ents[i].alive = 0;
        g_ents[i].hp = 0x200;
        g_ents[i].visible = 1;
        g_ents[i].combat_ready = 0;
        g_ents[i].anim_state = 0;
        g_ents[i].walking = 0;
        g_ents[i].joined = 0;
        g_ents[i].dest_x = 0;
        g_ents[i].dest_z = 0;
        g_ents[i].dest_facing = 0;
        g_ents[i].walk_ms = 0;
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

/*
 * slot 0 sub_44453E @ 0x44453E "Animation" — queue an ANIMATION action.
 *
 * asm: strcpy + _strlwr the name, then dispatch on its first character:
 *   '2'  2D sprite: build "<prefix><name>.ANI", register it in the anim2d
 *        list (dword_505A90 / byte_505AA0, cap 20 "too many anim2d"), load it
 *        via sub_41333A @ 0x41333A, node+0x0C = -2 (no owning entity).
 *   's'  sound-prefixed: drop the 's', node+0x70 = 1.
 *   'a'  actor animation; "ar..." holds for 500 ms (node+0x40 = 0x1F4),
 *        anything else completes as soon as the animation is handed over
 *        (node+0x40 = -1).
 * A "_NN" suffix is split off the name and stored as a number at node+0x5C.
 */
static void cb_named_object(const c8 *name) {
    if (!name) {
        print("lieu_cb: Scripteur, tu es un bougre d'ane : mets un nom d'animation\n");
        return;
    }
    lieu_action_t *n = lieu_action_push(LA_ANIMATION);

    c8 buf[32];
    snprintf(buf, sizeof(buf), "%s", name);
    for (c8 *q = buf; *q; q++) if (*q >= 'A' && *q <= 'Z') *q += 32;   /* _strlwr */
    /* sub_443720 @ 0x443720 (sound/anim preload for the name) not ported. */
    n->flag70   = 0;
    n->duration = -1;
    memset(n->name, 0, 16);

    if (buf[0] == '2') {
        /* 2D animation: sub_41333A loads "<anim2d dir><name>.ANI" into the
         * anim2d player (anim_state_531BE0 + 0x115F8); the node only waits
         * for it. The 2D player is not ported. */
        n->actor = -2;
        return;
    }
    if (buf[0] == 's') {                                   /* 444729 */
        memmove(buf, buf + 1, strlen(buf));
        n->flag70 = 1;
    }
    c8 *u = strchr(buf, '_');                              /* 444778 */
    if (u) { *u = '\0'; n->num5C = atoi(u + 1); }
    else   n->num5C = 0;

    if (buf[0] == 'a') {                                   /* 4447D4 */
        n->duration = (buf[1] == 'r') ? 0x1F4 : -1;
        entity_t *e = actor_find(0);
        if (e) {
            if (E32(e, 0xD60))                         actor_prepare_anim(e, 5, n->name);
            else if (e != g_player_entity)             actor_prepare_anim(e, -5, n->name);
            else                                       actor_prepare_anim(e, 0x3B, n->name);
        }
        n->actor = 0;
        n->p44   = 1;
    }
    if (buf[0] == 'p' || buf[0] == 'g') {                  /* 4448BF */
        c8 num[3] = { buf[1], buf[2], 0 };
        if (num[0] == '0') { num[0] = num[1]; num[1] = 0; }
        i32 id = atoi(num);
        if (id > 0 && id < 4) id = 0;                      /* heroes -> player */
        snprintf(n->name, sizeof(n->name), "%s", buf);
        n->actor = id;
    }
}

/* slot 1 sub_4449A5 "Dialogue": enqueue dialogue line. */
static void cb_named_object_int(const c8 *name, i32 v) {
    (void)v;
    if (name && name[0] == 'L' && name[1] >= '0' && name[1] <= '9' && name[3] == 'D')
        dlg_say(name);
}

/*
 * slot 2 sub_444A3E @ 0x444A3E "Evenement" — queue an EVENEMENT action.
 * id lands at node+0x48, the delay at node+0x40. The dispatcher (case 2 of
 * sub_44A58B @ 0x44AD41) turns it into an object-timer entry: delay >= 0
 * arms a timer that fires FonctionEditor opcode 4 with `id` once it
 * expires, delay == -1 cancels the pending timer for that id.
 */
static void cb_named_two_ints(i32 id, i32 p) {
    lieu_action_t *n = lieu_action_push(LA_EVENEMENT);
    n->id48     = id;
    n->duration = p;
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
    /* sub_44502D @ 0x44502D queues the placement; case 5 of sub_44A58B runs
     * sub_4640BC with these fields. */
    {
        lieu_action_t *n = lieu_action_push(LA_POSPERSO);
        if (name) {
            snprintf(n->name, sizeof(n->name), "%s", name);
            for (c8 *q = n->name; *q; q++) if (*q >= 'A' && *q <= 'Z') *q += 32;
        }
        n->actor = zone;                    /* persona id   (+0x0C)        */
        n->x     = anim * LIEU_ZONE_SCALE;  /* tile X * 600 (+0x14)        */
        n->z     = x * LIEU_ZONE_SCALE;     /* tile Z * 600 (+0x1C)        */
        n->p20   = y;                       /* facing       (+0x20)        */
        if (flag < 0 || flag > 4)
            print("lieu_cb: Bad parameter vie in POSPERSO function\n");
        n->p4C   = flag;                    /* vie          (+0x4C)        */
        n->p50   = kind;                    /* zone         (+0x50)        */
        n->p2C   = z;                       /* zone radius  (+0x2C)        */
    }
    /* Mark entity as alive/spawned. vie==2 means dead/cadaver placement. */
    if (zone >= 0 && zone < ENT_MAX) {
        if (flag == 2)
            ent_kill(zone);
        else
            ent_spawn(zone);
    }
    print("lieu_cb: PosPerso '%s' id=%d vie=%d tileXZ=(%d,%d) facing=%d zone=%d radius=%d\n",
          name ? name : "(null)", zone, flag, anim, x, y, kind, z);
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
/*
 * slot 6 sub_445189 @ 0x445189 "EtatObjet" — queue an ETATOBJET action.
 * asm field map (name is _strupr'd first):
 *   node+0x30 = name      node+0x14 = p  * 600   (world X)
 *   node+0x0C = a6        node+0x18 = a1 * 600   (world Y)
 *   node+0x54 = a7        node+0x1C = a2 * 600   (world Z)
 *   node+0x20 = a3        node+0x24 = a4         node+0x28 = a5
 */
static void cb_named_ex_988(const c8 *name, i32 p, i32 a1, i32 a2, i32 a3,
                            i32 a4, i32 a5, i32 a6, i32 a7) {
    lieu_action_t *n = lieu_action_push(LA_ETATOBJET);
    if (name) {
        i32 k = 0;
        while (name[k] && k < (i32)sizeof(n->name) - 1) {
            c8 ch = name[k];
            if (ch >= 'a' && ch <= 'z') ch -= 32;        /* _strupr */
            n->name[k] = ch;
            k++;
        }
        n->name[k] = '\0';
    }
    n->x     = p  * LIEU_ZONE_SCALE;
    n->y     = a1 * LIEU_ZONE_SCALE;
    n->z     = a2 * LIEU_ZONE_SCALE;
    n->p20   = a3;
    n->p24   = a4;
    n->p28   = a5;
    n->actor = a6;
    n->p54   = a7;
}

/*
 * slot 8 sub_4453E1 @ 0x4453E1 "Joindre" — walk an actor to a point.
 *
 * Signature is (ecx = actor, edx = tile X, a2 = tile Z, a3, a4): the second
 * argument is a COORDINATE, not a facing angle — asm 0x4454A7/0x4454BC scale
 * both by 600 the same way PosPerso does. A negative actor id forces the
 * join even while the actor is already walking; the engine otherwise drops
 * the request (asm 0x445446: entity+0xCBC != 0 && actor >= 0 -> return).
 *
 * asm: node+0x0C = abs(actor)  node+0x14 = X*600  node+0x1C = Z*600
 *      node+0x10 = a4          node+0x24 = a3
 */
static void cb_actor_934(i32 actor, i32 tile_x, i32 tile_z, i32 a3, i32 a4) {
    i32 id = actor < 0 ? -actor : actor;

    /* Engine guard: skip if this actor already has a walk in flight and the
     * caller did not force it with a negative id. */
    if (actor >= 0 && ent_is_walking(id)) return;

    lieu_action_t *n = lieu_action_push(LA_JOINDRE);
    n->actor = id;
    n->x     = tile_x * LIEU_ZONE_SCALE;
    n->z     = tile_z * LIEU_ZONE_SCALE;
    n->p10   = a4;
    n->p24   = a3;
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
/* slot 11 sub_44569A @ 0x44569A "Bruitage" — queue a positional sound.
 * name = WAV base name, id = source entity/object, coord = placement. */
static void cb_place_95C(const c8 *name, i32 id, i32 coord) {
    lieu_action_t *n = lieu_action_push(LA_BRUITAGE);
    if (name) snprintf(n->name, sizeof(n->name), "%s", name);
    n->actor = id;
    n->p20   = coord;
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
/* Defined further down with the object registry / spawn helpers. */
typedef struct lieu_obj_s lieu_obj_t;
static const lieu_obj_t *obj_lookup(const c8 *name);
static i32 spawn_get_world(i32 id, i32 *wx, i32 *wz);
static i32 obj_world(const lieu_obj_t *o, i32 *x, i32 *z);

/*
 * slot 15 sub_44599B @ 0x44599B "DirectionPerso" — is the player looking at
 * actor/object `a`?
 *   a >= 0  a persona id; its placement is the target.
 *   a <  0  an object: -a > 1000 selects "IM%02d" (-a - 1000), otherwise
 *           "OB%02d" (-a), looked up by name.
 * Within 30000 world units the angle is not consulted at all.
 * Falls back to permissive when the target or the player transform is
 * unknown, so a missing placement cannot deadlock a script.
 */
static i32 cb_query_954(i32 a) {
    i32 px, pz;
    if (!player_get_pos(&px, NULL, &pz)) return 1;

    i32 tx = 0, tz = 0;
    if (a >= 0) {
        if (!spawn_get_world(a, &tx, &tz)) return 1;
    } else {
        i32 v = -a;
        c8 name[16];
        if (v > 1000) snprintf(name, sizeof(name), "IM%02d", v - 1000);
        else          snprintf(name, sizeof(name), "OB%02d", v);
        const lieu_obj_t *o = obj_lookup(name);
        if (!o || !obj_world(o, &tx, &tz)) return 1;
    }

    double dx = (double)(tx - px), dz = (double)(tz - pz);
    if (dx * dx + dz * dz < LIEU_FACE_NEAR_SQ) return 1;    /* close enough */
    i32 pa = player_facing_bam(), ta = bam_angle(tx - px, tz - pz);
    i32 ok = facing_ok(pa, ta);
    if (getenv("DKE_FACEDBG"))
        print("face: DirectionPerso(%d) player=(%d,%d)@0x%X target=(%d,%d)@0x%X dist=%.0f -> %d\n",
              a, px, pz, pa, tx, tz, ta, sqrt(dx*dx+dz*dz), ok);
    return ok;
}

/*
 * slot 16 sub_445C82 @ 0x445C82 "DirectionZone" — is the player looking at
 * zone `n`? The engine prefers the entity attached to the zone record
 * (dword_506CA0[n], zone struct +0x20) and falls back to the zone centre
 * (dword_506C80[n] / +8); the port only tracks centres. No distance bypass.
 */
static i32 cb_query_944(i32 n) {
    i32 px, pz;
    if (n < 1 || n >= LIEU_ZONE_COUNT) return 1;
    if (g_zones[n].center_x == -1) return 1;
    if (!player_get_pos(&px, NULL, &pz)) return 1;
    i32 pa = player_facing_bam();
    i32 ta = bam_angle(g_zones[n].center_x - px, g_zones[n].center_z - pz);
    i32 ok = facing_ok(pa, ta);
    if (getenv("DKE_FACEDBG"))
        print("face: DirectionZone(%d) player=(%d,%d)@0x%X zone=(%d,%d)@0x%X -> %d\n",
              n, px, pz, pa, g_zones[n].center_x, g_zones[n].center_z, ta, ok);
    return ok;
}

/* slot 17 sub_445E99 "DirectionSpec" — target resolution not yet decoded,
 * so this stays permissive rather than guessing which thing it aims at. */
static i32 cb_query_998(i32 a) { (void)a; return 1; }
/* query_990: distance between zone a and b, world units / 600. */
static i32  cb_query_990(i32 a, i32 b) {
    if (a < 1 || a >= LIEU_ZONE_COUNT || b < 1 || b >= LIEU_ZONE_COUNT) return 0;
    double dx = (double)(g_zones[a].center_x - g_zones[b].center_x);
    double dz = (double)(g_zones[a].center_z - g_zones[b].center_z);
    return (i32)(sqrt(dx * dx + dz * dz) / LIEU_ZONE_SCALE);
}
/* slot 19 sub_446159 @ 0x446159 "AnimObjet" — queue an object animation.
 * asm: node+0x30 = name, node+0x20 = p, node+0x24 = a1, node+0x28 = a3,
 *      node+0x40 = a2 (hold time in ms). */
static void cb_cb_98C(const c8 *name, i32 p, i32 a1, i32 a2, i32 a3) {
    lieu_action_t *n = lieu_action_push(LA_ANIMOBJET);
    if (name) snprintf(n->name, sizeof(n->name), "%s", name);
    n->p20      = p;
    n->p24      = a1;
    n->p28      = a3;
    n->duration = a2;
}

/* slot 20 sub_446227 "Bloque": dialogue/cutscene group brackets.
 * group(3) = begin group (block player movement + camera).
 * group(0) = end group (close bracket).
 * Negative values (not used in L01) have immediate effects: bit 1=unlock
 * movement, bit 2=unlock camera, bit 4=reset combat state.
 * Since dke has no action queue, we apply the blocking directly. */
static void cb_group(i32 a) {                               /* sub_446227 */
    lieu_action_t *n = lieu_action_push(LA_BLOQUE);
    if (a < 0) {
        /* Negative: immediate. 1 = lock movement, 2 = lock camera
         * (dword_4FF0A0), 4 = stop the player's locomotion. */
        a = -a;
        if (a & 1) g_player_blocked = 1;
        if ((a & 4) && g_player_entity) {
            E32(g_player_entity, 0xCEC) = 0; E32(g_player_entity, 0xCF0) = 1;
            E32(g_player_entity, 0xCF4) = 0; E32(g_player_entity, 0xCF8) = 0;
        }
        return;
    }
    n->num5C = a;
    if (a == 0) { n->p60 = 0; n->p64 = 0; }
    if (a & 1) n->p64 = a;
    if (a & 2) n->p60 = a;
}

/* slot 21 sub_44637B "Fouille": remove entity and clear its zone slots.
 * In the original, iterates 41 zone slots and clears any whose entity
 * pointer matches. Also marks entity as no longer alive. */
static void cb_cb_454(i32 a) {
    print("lieu_cb: cb_454 (fouille) entity=%d\n", a);
    if (a >= 0 && a < ENT_MAX) ent_kill(a);
}

/*
 * slot 22 sub_44648B @ 0x44648B "StopJoindre".
 *   a < 0  IMMEDIATE: stop actor -a. asm clears the join block
 *          (entity+0xCB4 = 0, +0xCEC = 0, +0xCF0 = 1, +0xCF4 = 0) and the
 *          walk block (+0xCBC = 0, +0xD14 = 0, +0xCDC = 1, +0xC44 = 0).
 *   a >= 0 queue a STOPJOINDRE action.
 * Both paths first refuse ids > 4 whose entity slot dword_4DF5D0 is empty
 * ("DirectionPerso : Perso %d non charge !!").
 */
static void cb_select_468(i32 a) {
    i32 id = a < 0 ? -a : a;
    if (id > 4 && (id >= ENT_MAX || !g_ents[id].alive)) {
        print("lieu_cb: StopJoindre : Perso %d non charge !!\n", id);
        return;
    }
    if (a < 0) {
        if (id < ENT_MAX) { g_ents[id].walking = 0; g_ents[id].joined = 0; }
        print("lieu_cb: StopJoindre — actor %d stopped\n", id);
        return;
    }
    lieu_action_t *n = lieu_action_push(LA_STOPJOINDRE);
    n->actor = a;
}

/* slot 23 sub_44660D @ 0x44660D "VireCol" — queue a script flag write.
 * asm: node+0x68 = ecx (flag id), node+0x4C = edx (value). */
static void cb_cb_964(i32 flag_id, i32 value) {
    lieu_action_t *n = lieu_action_push(LA_VIRECOL);
    n->p68 = flag_id;
    n->p4C = value;
}

/* slot 25: debug/subtitle text — no-op. */
static void cb_debug_text(const c8 *t, i32 edx, i32 len) { (void)t; (void)edx; (void)len; }

/*
 * slot 27 sub_446808 @ 0x446808 "JoindrePerm" — permanent actor join.
 * Like Joindre but the actor keeps following its target. Engine drops the
 * request when the actor already has a permanent join and the id is not
 * negative (asm 0x446871: entity+0xCB4 != 0 && actor >= 0 -> return), and
 * when the entity does not exist at all.
 * asm: node+0x0C = abs(actor), node+0x10 = target.
 */
static void cb_flush_958(i32 actor, i32 target) {
    i32 id = actor < 0 ? -actor : actor;
    if (id >= ENT_MAX || !g_ents[id].alive) return;      /* sub_44411D -> NULL */
    if (actor >= 0 && ent_is_joined(id)) return;

    lieu_action_t *n = lieu_action_push(LA_JOINDREPERM);
    n->actor = id;
    n->p10   = target;
}

/* slot 28: cb_46C sets the state slot the handler passed. */
static void cb_cb_46C(i32 *slot, i32 edx) {                 /* sub_4468E1 */
    if (!slot) {
        print("lieu_cb: Bad parameter ptr in MODIFYVAR function\n");
        return;
    }
    lieu_action_t *n = lieu_action_push(LA_MODIFYVAR);
    n->slot74 = slot;
    n->num5C  = edx;
}

/* slot 29: query_940 — random integer in [0, n]. */
static i32  cb_query_940(i32 n) {
    return (i32)(((u32)n * ((u32)rand() & 0x7FFFu)) / 32767u) & 0xFFFF;
}

/*
 * slot 30 sub_446A05 @ 0x446A05 "SelectPile" — pick the action pile.
 *
 * `pile` becomes the allocation target (dword_505C30 @ 0x505C30) and `mode`
 * is written to that pile's enable flag (dword_4FC194 / dword_507274 /
 * dword_507270). mode == 2 means RESET: the engine memsets the pile's 0x2EE0
 * bytes (100 nodes x 0x78) to -1, forces the enable flag back to 1 and zeroes
 * the count. Anything else is stored as-is, so mode 0 parks the pile.
 */
static void cb_state_toggle(i32 pile, i32 mode) {
    if (pile < 0 || pile > 2) return;

    lieu_action_select_pile(pile);
    g_layer_mode[pile] = mode;

    if (mode == 2) {
        lieu_action_clear_pile(pile);
        g_layer_mode[pile]    = 1;
        g_layer_counter[pile] = 0;
    } else {
        lieu_action_enable_pile(pile, mode);
    }
}

/* slot 32 sub_446BE9 @ 0x446BE9 "LinkObjet" — attach an object to a holder.
 * asm: name _strupr'd into node+0x30, node+0x0C = holder id,
 *      node+0x54 = a, node+0x5C = b. */
static void cb_attach_99C(i32 holder, const c8 *name, i32 a, i32 b) {
    lieu_action_t *n = lieu_action_push(LA_LINKOBJET);
    if (name) {
        i32 k = 0;
        while (name[k] && k < (i32)sizeof(n->name) - 1) {
            c8 ch = name[k];
            if (ch >= 'a' && ch <= 'z') ch -= 32;        /* _strupr */
            n->name[k] = ch;
            k++;
        }
        n->name[k] = '\0';
    }
    n->actor = holder;
    n->p54   = a;
    n->num5C = b;
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

/* slot 36 sub_443273 @ 0x443273 "FreeAnim" — release a 2D anim slot.
 * asm: type 0x27 (FREEANIM), node+0x48 = slot. */
static void cb_cb_950(i32 slot) {
    lieu_action_t *n = lieu_action_push(LA_FREEANIM);
    n->id48 = slot;
}

/* slot 37 sub_44356F @ 0x44356F "PlayAnim" — start a 2D anim slot.
 * asm: type 0x23 (PLAYANIM), node+0x48 = slot, node+0x70 = mode. Slots
 * >= 20 are rejected with "Plantage! animation invalid". */
static void cb_cb_97C(i32 slot, i32 mode) {
    if (slot >= 20) {
        print("lieu_cb: Plantage! animation invalid (slot %d)\n", slot);
        return;
    }
    lieu_action_t *n = lieu_action_push(LA_PLAYANIM);
    n->id48   = slot;
    n->flag70 = mode;
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
 * Script-visible object state.
 *
 * ETATOBJET / LINKOBJET / VIRECOL / the 2D-anim opcodes all mutate engine
 * state dke has not ported (object meshes at dword_4DF5D0, the anim2d slot
 * table at anim_state_531BE0, the collision flag table). We keep the values
 * in these registries so the state is real and queryable rather than lost,
 * and so a later render/audio pass can consume it without re-reversing the
 * scripts.
 * ---------------------------------------------------------------------- */
#define LIEU_OBJ_MAX 64
struct lieu_obj_s {
    c8  name[32];
    i32 x, y, z;      /* world units (tile x600) */
    i32 p20, p24, p28;
    i32 owner;        /* entity the object belongs to, -1 = none */
    i32 linked_to;    /* LINKOBJET holder id, -1 = unlinked      */
    i32 used;
};
static lieu_obj_t g_objs[LIEU_OBJ_MAX];
static i32        g_obj_count = 0;

static const lieu_obj_t *obj_lookup(const c8 *name) {
    if (!name || !name[0]) return NULL;
    for (i32 i = 0; i < g_obj_count; i++)
        if (strcmp(g_objs[i].name, name) == 0) return &g_objs[i];
    return NULL;
}

/* World position recorded for an object by ETATOBJET. */
static i32 obj_world(const lieu_obj_t *o, i32 *x, i32 *z) {
    if (!o) return 0;
    if (x) *x = o->x;
    if (z) *z = o->z;
    return 1;
}

static lieu_obj_t *obj_find(const c8 *name, i32 create) {
    if (!name || !name[0]) return NULL;
    for (i32 i = 0; i < g_obj_count; i++)
        if (strcmp(g_objs[i].name, name) == 0) return &g_objs[i];
    if (!create || g_obj_count >= LIEU_OBJ_MAX) return NULL;
    lieu_obj_t *o = &g_objs[g_obj_count++];
    memset(o, 0, sizeof(*o));
    snprintf(o->name, sizeof(o->name), "%s", name);
    o->owner = -1;
    o->linked_to = -1;
    o->used = 1;
    return o;
}

/* VIRECOL flag table (engine: per-level collision/state flags). */
#define LIEU_FLAG_MAX 128
static i32 g_flags[LIEU_FLAG_MAX];

/* 2D anim slots driven by PLAYANIM / PAUSEANIM / LOADANIM / FREEANIM.
 * 0 = free, 1 = loaded, 2 = playing, 3 = paused. */
#define LIEU_ANIM_SLOTS 20
static i32 g_anim_slot[LIEU_ANIM_SLOTS];

static void objs_clear(void) {
    memset(g_objs, 0, sizeof(g_objs));
    g_obj_count = 0;
    memset(g_flags, 0, sizeof(g_flags));
    memset(g_anim_slot, 0, sizeof(g_anim_slot));
}

/* Live world X/Z of persona `id` (sub_44411D -> entity +0x50 node). */
static i32 spawn_get_world(i32 id, i32 *wx, i32 *wz) {
    entity_t *e = actor_find(id);
    if (!e || !EROOT(e)) return 0;
    if (wx) *wx = EROOT(e)->pos[0];
    if (wz) *wz = EROOT(e)->pos[2];
    return 1;
}

/* -------------------------------------------------------------------------
 * Requested animation clips.
 *
 * An ANIMATION action's name is the CAN clip id and carries its own persona:
 * "p12arr03" is Kaly's arrival-03 clip, "p00mar25" is Arkhan walking. The
 * engine resolves the entity and calls play_entity_animation; the port has no
 * entity animation player, so the clip is published here and the scene
 * renderer swaps the persona's CAN over to it.
 * ---------------------------------------------------------------------- */

/*
 * lieu_action_dispatch — port of the 40-case switch in sub_44A58B @ 0x44A58B.
 *
 * Returns 1 when the action is complete (the driver then calls
 * lieu_action_done -> sub_4442EE @ 0x4442EE), 0 to hold it at the head of the
 * pile for another frame. The original expresses "hold" through the entity
 * struct (entity+0xC28 start time vs node+0x40) and the per-pile busy flag
 * dword_4B5418[]; we hold on node->start_ms instead.
 */
i32 lieu_action_dispatch(lieu_action_t *n, u32 now_ms) {
    if (!n) return 1;

    /* Generic hold: a node with a positive duration stays at the head until
     * it elapses (asm 0x44ACB2: now - entity[0xC28] > node[0x40]). */
    i32 held = (n->duration >= 0 &&
                (i32)(now_ms - n->start_ms) <= n->duration);

    switch (n->type) {

    case LA_ANIMATION: {                       /* case 0 @ 0x44A8E3 */
        static i32 busy[3];                    /* dword_4B5418[pile] */
        if (g_skip_next_animation) { g_skip_next_animation = 0; return 1; }
        i32 pile = lieu_action_run_pile();
        if (pile < 0 || pile > 2) pile = 0;
        entity_t *e = actor_find(n->actor);
        if (n->actor != -2 && !e) {
            print("lieu_act: Animation %s with perso %d not loaded\n", n->name, n->actor);
            return 1;
        }
        if (n->actor != -2 && EDESC(e) && EDESC(e)[0x96]) {
            print("lieu_act: Animation %s with cadavre %d\n", n->name, n->actor);
            return 1;
        }
        /* 0x44A9C4: dword_4FF098 / dword_4FF09C dialogue wait — not wired. */
        if (n->num5C == 0xFFFF)                /* wait for the current clip */
            return actor_anim_done(e);
        if (n->actor == -2) return 1;          /* 2D anim player not ported */
        g_pile_actor[pile] = n->actor;         /* dword_4B53F0[pile] */
        e = actor_find(n->actor);
        if (!e) return 0;
        if (E32(e, 0xCBC)) return 0;           /* still walking */

        i32 start = 0;
        if (n->flag70) start = 2;
        else if ((actor_anim_done(e) || n->prev_type == LA_JOINDRE) && !busy[pile]) start = 1;
        if (start) {
            E32(e, 0xD58) = actor_anim_state(e, n->name);
            actor_bake_root_yaw(e);
            actor_play_anim(e, can_get_anim(n->name), 0);
            E32(e, 0xC48) = n->num5C;
            E32(e, 0xCF8) = E32(e, 0xCF4) = E32(e, 0xCF0) = E32(e, 0xCEC) = 0;
            print("lieu_act: ANIMATION '%s' actor=%d hold=%d\n", n->name, n->actor, n->duration);
            if (start == 2 || n->duration == -1) return 1;
            busy[pile] = 1;
            return 0;
        }
        if (n->duration != -1 && busy[pile] == 1 &&
            (i32)now_ms - E32(e, 0xC28) > n->duration) {
            busy[pile] = 0;
            return 1;
        }
        return 0;
    }

    case LA_POSPERSO:                          /* case 5 @ 0x44AF10 */
        actor_pos_perso(n->actor, n->x, n->z, n->p20, n->p4C, n->name, n->p50, n->p2C);
        return 1;

    case LA_EVENEMENT:                         /* case 2 @ 0x44AD41 */
        /* duration -1 cancels the pending timer, otherwise arm one that
         * fires opcode 4 with node+0x48 (asm 0x44AE21..0x44AEF0). */
        if (n->duration == -1) {
            lieu_timer_remove(n->id48);
            print("lieu_act: EVENEMENT cancel id=%d\n", n->id48);
        } else {
            lieu_timer_add(n->id48, n->duration, now_ms);
            print("lieu_act: EVENEMENT arm id=%d in %d ms\n", n->id48, n->duration);
        }
        return 1;

    case LA_ETATOBJET: {                       /* case 6 @ 0x44B00F */
        lieu_obj_t *o = obj_find(n->name, 1);
        if (o) {
            /* asm writes entity+0xCC/0xD0/0xD4 = node+0x14/0x18/0x1C and
             * entity+0xE0/0xE4/0xE8/0xEC = node+0x20/0x24/0x28/0x54. */
            o->x = n->x; o->y = n->y; o->z = n->z;
            o->p20 = n->p20; o->p24 = n->p24; o->p28 = n->p28;
            o->owner = n->actor;
        }
        print("lieu_act: ETATOBJET '%s' owner=%d w=(%d,%d,%d) p=(%d,%d,%d)\n",
              n->name, n->actor, n->x, n->y, n->z, n->p20, n->p24, n->p28);
        return 1;
    }

    case LA_JOINDRE: {                         /* case 8 @ 0x44B5E3 */
        /* Arms the actor's walk (entity +0xCC0/+0xCC4/+0xCC8, state +0xCBC = 1)
         * and completes; sub_469222 walks it on later frames. A JOINDRE that
         * repeats the running walk waits for it to end (heroes only). */
        entity_t *e = actor_find(n->actor);
        entity_t *t = actor_find(n->p10);
        if (!e || !EROOT(e)) return 1;
        obc_node_t *eg = EROOT(e);
        E32(e, 0xCB4) = 0;
        if (t && EROOT(t)) {
            obc_node_t *tg = EROOT(t);
            if (n->x == 0 && n->z == 0 && n->p24 == 0) { n->x = eg->pos[0]; n->z = eg->pos[2]; }
            else                                      { n->x = tg->pos[0]; n->z = tg->pos[2]; }
            n->p24 = bam_angle(tg->pos[0] - eg->pos[0], tg->pos[2] - eg->pos[2]);
        } else if (n->x == 0 && n->z == 0) {
            if (n->p10 < 0 && n->p24 == 0) {
                i32 zi = -n->p10 - 1;
                if (zi >= 0 && zi < LIEU_ZONE_COUNT)
                    n->p24 = bam_angle(g_zones[zi].center_x - eg->pos[0],
                                       g_zones[zi].center_z - eg->pos[2]);
            }
            n->x = eg->pos[0];
            n->z = eg->pos[2];
        }
        if ((E32(e, 0xCBC) != 0 || E32(e, 0xC44) != 0) &&
            (n->x == E32(e, 0xCC0) || E32(e, 0xCC4) == n->z || n->p24 == E32(e, 0xCC8))) {
            if (n->actor > 3) { E32(e, 0xC44) = 0; return 1; }
            if (E32(e, 0xCBC) == 0 && E32(e, 0xC44) == 1) { E32(e, 0xC44) = 0; return 1; }
            return 0;
        }
        if (E32(e, 0xD60) != 0 && e != g_player_entity) {
            print("lieu_act: JOINDRE executed during COMBAT by perso %d\n", E32(e, 0xB0));
            return 1;
        }
        E32(e, 0xCC0) = n->x;
        E32(e, 0xCC4) = n->z;
        E32(e, 0xCC8) = n->p24;
        E32(e, 0xCBC) = 1;
        E32(e, 0xC44) = 1;
        print("lieu_act: JOINDRE actor=%d (%d,%d) -> (%d,%d) facing=0x%X\n",
              n->actor, eg->pos[0], eg->pos[2], n->x, n->z, n->p24 & 0xFFFF);
        return 1;
    }

    case LA_JOINDREPERM: {                     /* case 0x1B @ 0x44C695 */
        entity_t *e = actor_find(n->actor);
        entity_t *t = actor_find(n->p10);
        if (e && t) {
            if (E32(e, 0xD60) != 0 && e != g_player_entity) {
                print("lieu_act: JOINDREPERM executed during COMBAT by perso %d\n", E32(e, 0xB0));
                return 1;
            }
            E32(e, 0xCB4) = 1;
            E32(e, 0xCB8) = n->p10;
        }
        return 1;
    }

    case LA_BRUITAGE:                          /* case 0x0B */
        /* Positional sound: needs the sound emitter path (sub_44D23D for
         * 'Z'-prefixed zone sounds). Recorded, not yet audible. */
        print("lieu_act: BRUITAGE '%s' src=%d coord=%d (no emitter yet)\n",
              n->name, n->actor, n->p20);
        return 1;

    case LA_ANIMOBJET: {                       /* case 0x13 */
        if (n->aux_state == 0) {
            n->aux_state = 1;
            lieu_obj_t *o = obj_find(n->name, 1);
            if (o) { o->p20 = n->p20; o->p24 = n->p24; o->p28 = n->p28; }
            print("lieu_act: ANIMOBJET '%s' p=(%d,%d,%d) hold=%d\n",
                  n->name, n->p20, n->p24, n->p28, n->duration);
        }
        return held ? 0 : 1;
    }

    case LA_STOPJOINDRE: {                     /* case 0x16 @ 0x44C435 */
        entity_t *e = actor_find(n->actor);
        if (e) {
            if (E32(e, 0xCB4)) {
                E32(e, 0xCB4) = 0; E32(e, 0xCEC) = 0; E32(e, 0xCF0) = 1; E32(e, 0xCF4) = 0;
            }
            if (E32(e, 0xCBC)) {
                E32(e, 0xCDC) = 1; E32(e, 0xD14) = 0; E32(e, 0xCBC) = 0; E32(e, 0xC44) = 0;
                E32(e, 0xCEC) = 0; E32(e, 0xCF0) = 1; E32(e, 0xCF4) = 0;
            }
        }
        return 1;
    }

    case LA_VIRECOL:                           /* case 0x17 */
        if (n->p68 >= 0 && n->p68 < LIEU_FLAG_MAX) g_flags[n->p68] = n->p4C;
        print("lieu_act: VIRECOL flag[%d] = %d\n", n->p68, n->p4C);
        return 1;

    case LA_LINKOBJET: {                       /* case 0x20 */
        lieu_obj_t *o = obj_find(n->name, 1);
        if (o) o->linked_to = n->actor;
        print("lieu_act: LINKOBJET '%s' -> holder %d (%d,%d)\n",
              n->name, n->actor, n->p54, n->num5C);
        return 1;
    }

    case LA_PLAYANIM:                          /* case 0x23 */
        if (n->id48 >= 0 && n->id48 < LIEU_ANIM_SLOTS) g_anim_slot[n->id48] = 2;
        print("lieu_act: PLAYANIM slot=%d mode=%d\n", n->id48, n->flag70);
        return 1;

    case LA_PAUSEANIM:                         /* case 0x24 */
        if (n->id48 >= 0 && n->id48 < LIEU_ANIM_SLOTS) g_anim_slot[n->id48] = 3;
        print("lieu_act: PAUSEANIM slot=%d\n", n->id48);
        return 1;

    case LA_LOADANIM:                          /* case 0x26 */
        if (n->id48 >= 0 && n->id48 < LIEU_ANIM_SLOTS) g_anim_slot[n->id48] = 1;
        print("lieu_act: LOADANIM slot=%d\n", n->id48);
        return 1;

    case LA_FREEANIM:                          /* case 0x27 */
        if (n->id48 >= 0 && n->id48 < LIEU_ANIM_SLOTS) g_anim_slot[n->id48] = 0;
        print("lieu_act: FREEANIM slot=%d\n", n->id48);
        return 1;

    case LA_BLOQUE:                            /* case 0x14 @ 0x44BC8D */
        if ((n->num5C & 4) && g_player_entity) {
            E32(g_player_entity, 0xCEC) = 0; E32(g_player_entity, 0xCF0) = 1;
            E32(g_player_entity, 0xCF4) = 0; E32(g_player_entity, 0xCF8) = 0;
        }
        /* dword_4FF0A0 (camera lock) = p60 != 0 — camera follow not ported. */
        g_player_blocked = (n->p64 != 0);
        if (n->num5C == 0 && g_player_entity &&
            (E32(g_player_entity, 0xD58) == -5 || E32(g_player_entity, 0xD58) == 0x3B))
            E32(g_player_entity, 0xE4) = 1;
        return 1;

    case LA_MODIFYVAR: {                       /* case 0x1C @ 0x44C5F7 */
        i32 pile = lieu_action_run_pile();
        entity_t *e = actor_find(g_pile_actor[(pile >= 0 && pile < 3) ? pile : 0]);
        if (e && n->prev_type != LA_JOINDRE && !actor_anim_done(e)) return 0;
        if (n->slot74) *n->slot74 = n->num5C;
        return 1;
    }

    default:
        print("lieu_act: %s (type %d) not wired — completing\n",
              lieu_action_name(n->type), n->type);
        return 1;
    }
}

/* -------------------------------------------------------------------------
 * Public driver — scene.c calls these.
 * ---------------------------------------------------------------------- */

/* sub_448F4D @ 0x4494B2..0x449530: run the script's INIT, then drain every
 * action it queued before the level starts (with dword_4B52DC = -1, which
 * makes is_entity_in_animation report "done"), then fire trigger 41
 * (sub_44A263, "room entered"). */
static void lieu_run_init(void) {
    g_lieu_loading = 1;
    lieu01_fonction_editor(LIEU_OP_INIT, g_lieu_state, 0, 0);   /* op 1 */
    u32 t0 = state ? state->current_game_time : 0;
    for (i32 guard = 0; lieu_action_pending() && guard < 100000; guard++)
        lieu_action_run(t0 + (u32)guard);
    lieu01_fonction_editor(LIEU_OP_EVENT, g_lieu_state, 0x29, 0);  /* op 3 (41) */
    g_lieu_loading = 0;
}
void lieu_enter(i32 level_id) {
    g_lieu_level = level_id;
    g_lieu_cb    = &g_lieu_cb_impl;
    memset(g_lieu_state, 0, sizeof(g_lieu_state));
    zones_clear();   /* engine memsets dword_506C80 to -1 before INIT arms */
    ents_clear();    /* reset entity alive/HP/visibility state              */
    objs_clear();    /* object/flag/anim-slot registries                    */
    lieu_action_reset();  /* drop stale actions + object timers            */
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
    lieu_run_init();
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
i32 lieu_detect_zone(void) {                              /* sub_449E03 */
    i32 px, pz;
    if (!player_get_pos(&px, NULL, &pz)) return -1;
    /* 0x44A179: the action key (unless in combat) makes every zone live. */
    if (g_player_entity && E32(g_player_entity, 0xD60) == 0 && g_action_key) {
        if (!g_player_blocked) { g_action_pressed = 1; g_force_zones = 1; }
    } else {
        g_action_pressed = 0;
        g_force_zones = 0;
    }
    for (i32 i = 1; i < LIEU_ZONE_COUNT; i++) {           /* first hit wins  */
        zone_t *zn = &g_zones[i];
        if (zn->center_x == -1 && !zn->entity) continue;  /* empty slot      */
        if (zn->active != 1 && g_force_zones != 1) continue;
        i32 cx = zn->center_x, cz = zn->center_z;
        if (zn->entity) {
            entity_t *e = (entity_t *)zn->entity;
            if (e == g_player_entity || !EROOT(e)) continue;
            cx = EROOT(e)->pos[0];
            cz = EROOT(e)->pos[2];
        }
        i32 dx = cx - px, dz = cz - pz;
        i32 hx = zn->size_x / 2, hz = zn->size_z / 2;
        if ((dx < 0 ? -dx : dx) > hx) continue;
        if ((dz < 0 ? -dz : dz) > hz) continue;
        return i;                                         /* == trigger idx  */
    }
    return -1;
}

void lieu_tick(u32 now_ms) {
    /* Per-frame: engine calls op -1 every frame (drain + tick_body). */
    if (g_lieu_level == 1)
        lieu01_fonction_editor(LIEU_OP_TICK, g_lieu_state, 0, 0);   /* op -1 */

    (void)now_ms;   /* the piles are drained by actor_frame (sub_461468 order) */
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
    lieu_run_init();
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
