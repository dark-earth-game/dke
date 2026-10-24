/*
 * actor_move.c — per-frame actor update: clip clock, animation messages,
 * foot-planted root motion, the locomotion state controller and the scripted
 * walk state machine.
 *
 * Ported from dkev.exe:
 *   sub_41CCB7 @ 0x41CCB7  per-actor update (clock, floor, pose, root motion)
 *   sub_41AED1 @ 0x41AED1  animation message processor
 *   sub_414F72, sub_41FC77, sub_41FDC2, sub_492310, sub_469B4F,
 *   sub_4154E1, sub_4154A0, sub_4188A3, sub_418810  helpers
 *   sub_4193CC @ 0x4193CC  locomotion state controller (applies the move)
 *   sub_419B2C / sub_419F86 / sub_41A194 / sub_41A8B2 / sub_418EE5
 *   sub_469222 @ 0x469222  walk state machine
 *   sub_428714 @ 0x428714  steer toward a point
 *   sub_468FFE, sub_468D56, sub_46977B, sub_461BA0, sub_45720D, sub_477FD4
 *
 * World positions: the original takes node positions in camera space
 * (starting the hierarchy walk from parameter_table) and converts back with
 * the inverse matrix at parameter_table+0x30 minus transformation_origin.
 * Starting from an identity matrix gives the same world positions directly,
 * so every "camera -> world" round trip here collapses to that.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "libsys/common.h"
#include "libsys/graphics/shading.h"
#include "state.h"
#include "actor.h"
#include "nav_grid.h"
#include "lieu_action.h"

/* Engine globals the controller reads. Values are the ones the port runs
 * with until the systems that set them are ported. */
i32 g_cinematic_mode         = 0;   /* cinematic_mode: actor id driving a cutscene */
i32 g_special_mode_active    = 0;   /* special_mode_active */
i32 g_player_movement_locked = 0;   /* player_movement_enabled (non-zero = locked) */
i32 g_level_slow             = 0;   /* dword_4F5E20: "slow" key of L%02d.DAT */
static i32 g_last_event_check_time = 0;   /* last_event_check_time */
static i32 g_idle_fidget_last       = -1; /* dword_4B2C90 */
i32 g_skip_next_animation    = 0;   /* dword_506BE0, set by message 0x1F */

static i32 now_ms(void) { return state ? (i32)state->current_game_time : 0; }

#define ROOT(e)  EROOT(e)
#define ANIM(e)  ((can_rec_t *)EPTR(e, 0xD50))
#define FLAGS(e) (ANIM(e) ? CAN_FLAGS(ANIM(e)) : 0)

/* =========================================================================
 * Node positions
 * ========================================================================= */

static void fmat_identity(fmat_t *m) {
    memset(m, 0, sizeof(*m));
    m->m[0] = m->m[4] = m->m[8] = 0x4000;
}

/* sub_41FDC2 @ 0x41FDC2 — world position of `n` through its parent chain,
 * written to n->+0x90/+0x94/+0x98 (raw_88[2..4]). */
static void node_world(obc_node_t *root, obc_node_t *n) {
    if (!root || !n) return;
    obc_node_t *chain[64];
    i32 i = 0;
    chain[0] = n;
    while (chain[i] != root && chain[i]->parent && i < 63) {
        chain[i + 1] = chain[i]->parent;
        i++;
    }
    fmat_t acc;
    fmat_identity(&acc);
    for (; i >= 0; i--) {
        obc_node_t *k = chain[i];
        fmat_t r, p;
        fmat_rotation(&r, k->rot[0], k->rot[1], k->rot[2]);
        fmat_position(&p, k->pos[0], k->pos[1], k->pos[2]);
        fmat_rotate_translation(&acc, &p);
        fmat_premul_rotation(&r, &acc);
        acc.t[0] += p.t[0]; acc.t[1] += p.t[1]; acc.t[2] += p.t[2];
    }
    n->raw_88[2] = acc.t[0];
    n->raw_88[3] = acc.t[1];
    n->raw_88[4] = acc.t[2];
}

#define NX(n) ((n)->raw_88[2])
#define NY(n) ((n)->raw_88[3])
#define NZ(n) ((n)->raw_88[4])

/* sub_469B4F @ 0x469B4F — lowest world Y over the whole tree. */
static i32 lowest_node_y(entity_t *e) {
    fmat_t stack_m[32];
    obc_node_t *stack_n[32];
    i32 sp = 0, lo = 0x7FFFFFFF;
    fmat_t cur;
    fmat_identity(&cur);
    obc_node_t *n = ROOT(e);
    for (;;) {
        while (n) {
            if (n->next && sp < 32) { stack_n[sp] = n->next; stack_m[sp] = cur; sp++; }
            fmat_t r, p;
            fmat_rotation(&r, n->rot[0], n->rot[1], n->rot[2]);
            fmat_position(&p, n->pos[0], n->pos[1], n->pos[2]);
            fmat_rotate_translation(&cur, &p);
            fmat_premul_rotation(&r, &cur);
            cur.t[0] += p.t[0]; cur.t[1] += p.t[1]; cur.t[2] += p.t[2];
            if (cur.t[1] < lo) lo = cur.t[1];
            n = n->child;
        }
        if (!sp) break;
        sp--; n = stack_n[sp]; cur = stack_m[sp];
    }
    return lo;
}

/* =========================================================================
 * Collision — sub_459E80 / sub_4558A0
 *
 * TODO: the probes (sub_4593F7, sub_4599A2, sub_455B65) are not ported, so
 * moves are never deflected and corridors are always clear. sub_459E80
 * itself always returns 0 in the original; only its rewrite of BF8/C00 is
 * missing.
 * ========================================================================= */

static i32 actor_collide(entity_t *e, i32 flags) { (void)e; (void)flags; return 0; }
static i32 corridor_blocked(const i32 *from, const i32 *to) { (void)from; (void)to; return 0; }

/* sub_418810 @ 0x418810 — move the root to (x, z) through collision. */
static void actor_move_to(entity_t *e, i32 x, i32 z) {
    obc_node_t *g = ROOT(e);
    E32(e, 0xBF8) = x - g->pos[0];
    E32(e, 0xC00) = z - g->pos[2];
    if (actor_collide(e, 3) == 0) {
        g->pos[0] += E32(e, 0xBF8);
        g->pos[2] += E32(e, 0xC00);
    }
    E32(e, 0xBF8) = 0;
    E32(e, 0xC00) = 0;
}

/* =========================================================================
 * Ballistic helpers
 * ========================================================================= */

static void ballistic_decay(entity_t *e, i32 tick) {          /* sub_4154E1 */
    if (E32(e, 0xC80) || E32(e, 0xC98))
        E32(e, 0xC80) -= 4 * (tick - E32(e, 0xC7C));
}

static void ballistic_reset_ground(entity_t *e) {             /* sub_4154A0 */
    E32(e, 0xCAC) = E32(e, 0x8C);
    E32(e, 0xC98) = ROOT(e)->pos[1];
}

/* Record +0x10C: gravity byte for ballistic clips (message 6 sets it). */
static u8 *anim_gravity(entity_t *e) {
    return ANIM(e) ? &ANIM(e)->hdr[0x10C] : NULL;
}

/* sub_4188A3 @ 0x4188A3 — bring the ballistic / floor state up to `tick`. */
static void ballistic_advance(entity_t *e, obc_node_t *n, i32 tick) {
    obc_node_t *g = ROOT(e);
    if (!n) return;
    node_world(g, n);
    i32 fy = NY(n);
    u8 *grav = anim_gravity(e);
    if (E32(e, 0xC80) || E32(e, 0xC98)) {
        f64 gg = -(f64)(grav ? *grav : 0) / 3.0;
        f64 tt = (f64)(tick - E32(e, 0xC7C));
        E32(e, 0xC98) = (i32)(tt * tt * gg + tt * (f64)E32(e, 0xC80) + (f64)E32(e, 0xC8C));
        E32(e, 0xC9C) = (i32)((f64)E32(e, 0xC84) * tt + (f64)E32(e, 0xC90));
        E32(e, 0xCA0) = (i32)((f64)E32(e, 0xC88) * tt + (f64)E32(e, 0xC94));
        g->pos[1] = E32(e, 0xC98);
        const u8 *zone = (const u8 *)EPTR(e, 0xA8);
        if (zone && (zone[0] & 4)) {
            E32(e, 0xC8C) += (E32(e, 0x8C) - E32(e, 0xCAC)) / 4;
            E32(e, 0xCAC) += (E32(e, 0x8C) - E32(e, 0xCAC)) / 4;
        }
    } else if (E32(e, 0xEC) == 0) {
        i32 dy = E32(e, 0x8C) - fy;
        const u8 *zone = (const u8 *)EPTR(e, 0xA8);
        if (E32(e, 0xD60) || (e != g_player_entity && g_cinematic_mode)) {
            g->pos[1] += (FLAGS(e) & 0x400) ? dy / 4 : dy;
        } else if (FLAGS(e) & 0x400) {
            g->pos[1] += dy;
        } else if (zone && (zone[0] & 4)) {
            g->pos[1] += dy / 8;
        } else {
            g->pos[1] += dy / 4;
        }
    }
}

/* =========================================================================
 * Animation messages — sub_41AED1 @ 0x41AED1
 * ========================================================================= */

static void sample_at_message(entity_t *e, i32 mtick) {
    static i16 pose[103];
    memset(pose, 0, sizeof(pose));
    anim_sample(ANIM(e), pose, mtick);
    actor_blend_pose(e, pose, E32(e, 0xC34) - E32(e, 0xC2C));
    node_world(ROOT(e), (obc_node_t *)EPTR(e, 0xF8));
}

/* sub_414F72 — next message due at or before `tick`. */
static i32 anim_next_message(entity_t *e, i32 tick, i32 *type, i32 *param, i32 *mtick) {
    can_rec_t *r = ANIM(e);
    if (!r || CAN_NMES(r) == 0) return 0;
    i32 idx = E32(e, 0xD54);
    if (CAN_NMES(r) <= idx || !r->mes[idx]) return 0;
    const u8 *m = r->mes[idx];
    i32 mt = *(const i32 *)(m + 4);
    if (mt > tick) return 0;
    *type  = (i8)m[1];
    *param = *(const i32 *)(m + 8);
    *mtick = mt;
    E32(e, 0xD54) = idx + 1;
    return 1;
}

static void anim_messages(entity_t *e, i32 tick) {
    i32 type, p, mt;
    while (anim_next_message(e, tick, &type, &p, &mt)) {
        if (type == 0) {                                     /* foot contact */
            E32(e, 0xEC) = 0;
            if (E32(e, 0xC6C)) {
                if (now_ms() - E32(e, 0xC70) < 15) continue;
                E32(e, 0xC6C) = 0;
            }
            if (p < 0 || p >= ENTITY_BONE_SLOTS || !EBONE(e, p)) continue;
            obc_node_t *b = EBONE(e, p);
            EPTR(e, 0xF8) = b;
            E32(e, 0xC1C) = NX(b); E32(e, 0xC20) = NY(b); E32(e, 0xC24) = NZ(b);
            if (EPTR(e, 0x144) == EPTR(e, 0xF0) && p == 0x14 && E32(e, 0xB0) != 0x40) continue;
            if (EPTR(e, 0x148) == EPTR(e, 0xF0) && p == 0x15 && E32(e, 0xB0) != 0x40) continue;
            sample_at_message(e, mt);
        }
        if (type == 'f') {                                   /* contact, free vertical */
            E32(e, 0xEC) = 1;
            if (p < 0 || p >= ENTITY_BONE_SLOTS) continue;
            obc_node_t *b = EBONE(e, p);
            EPTR(e, 0xF8) = b;
            if (b) { E32(e, 0xC1C) = NX(b); E32(e, 0xC20) = NY(b); E32(e, 0xC24) = NZ(b); }
            sample_at_message(e, mt);
        }
        if (type == 1 || type == 0x13) {
            /* Sound event: "A%03d" (negative param) or the 9-byte name table
             * at "fend1a", footsteps suffixed with the surface. Positional
             * sound playback is not ported. */
        }
        if (type == 3 || type == 4 || type == 5) {           /* ballistic impulse */
            ballistic_decay(e, tick);
            ballistic_advance(e, (obc_node_t *)EPTR(e, 0xF8), tick);
            ballistic_reset_ground(e);
            E32(e, 0xC7C) = tick;
            if (type == 3) E32(e, 0xC80) += p;
            if (type == 4) E32(e, 0xC84) += p;
            if (type == 5) E32(e, 0xC88) += p;
            E32(e, 0xC8C) = E32(e, 0xC98);
            E32(e, 0xC90) = E32(e, 0xC9C);
            E32(e, 0xC94) = E32(e, 0xCA0);
            E32(e, 0xCA4) = E32(e, 0xC9C);
            E32(e, 0xCA8) = E32(e, 0xCA0);
            sample_at_message(e, mt);
        }
        if (type == 6) {                                     /* landing */
            ballistic_advance(e, (obc_node_t *)EPTR(e, 0xF8), tick);
            u8 *grav = anim_gravity(e);
            if (grav) { *grav = (u8)p; if (*grav == 0) *grav = 8; }
            for (i32 o = 0xC80; o <= 0xCA8; o += 4) E32(e, o) = 0;
            sample_at_message(e, mt);
        }
        /* 0x11 / 0x15 strikes, 0x16 / 0x1E / 0x21 / 0x22 / 0x23 object
         * exchanges, 0x19 parry sparks, 0x1A level overlay, 0x24 counter:
         * combat and inventory, not ported. */
        if (type == 0x1F) g_skip_next_animation = 1;
        if (type == 0x20) E32(e, 0xCE8) = 1;
        if (getenv("DKE_MSGDBG")) print("actor: msg p%02d '%s' type=%d param=0x%X tick=%d\n",
                                        E32(e, 0xB0), ESTR(e, 0xC4), type, p, mt);
        if (type == 'e') {                                   /* re-parent a node */
            i32 a = p & 0xFF, b = ((p & ~0xFF) >> 6) / 4;
            if (a >= ENTITY_BONE_SLOTS || b < 0 || b >= ENTITY_BONE_SLOTS) continue;
            obc_node_t *c = EBONE(e, a);
            obc_node_t *np = ((p & ~0xFF) == 0x2100) ? (obc_node_t *)EPTR(e, 0xE9C) : EBONE(e, b);
            if (c && np && c->parent) {
                c->parent->child = NULL;
                c->parent = np;
                np->child = c;
            }
        }
    }
}

/* =========================================================================
 * Per-actor update — sub_41CCB7 @ 0x41CCB7
 * ========================================================================= */

void actor_update(entity_t *e) {
    if (!e || !ROOT(e)) return;
    const u8 *d = EDESC(e);
    if (d && d[0x96]) return;
    if (!ANIM(e)) return;
    /* sub_450DE8 head look-at (E94/E98) is not ported. */
    E32(e, 0xE94) = 0xFFFF;

    i32 T = CAN_DURATION(ANIM(e));
    if (T == 0) return;

    i32 now = now_ms();
    if (now - E32(e, 0xC28) < 0) { E32(e, 0xC28) = now; E32(e, 0xC34) = E32(e, 0xC28); }
    i32 t     = (i32)((f64)(now - E32(e, 0xC28)) * g_anim_divisor);
    i32 tprev = (i32)((f64)(E32(e, 0xC34) - E32(e, 0xC28)) * g_anim_divisor);
    i32 loops = t / T, loopsp = tprev / T;
    if (FLAGS(e) & 0x200) { if (t >= T) t = T - 1; }
    else                  t = (loops - loopsp) * T + t % T;

    if (t >= T) {
        if (FLAGS(e) & 2) E32(e, 0xC2C) = now;
        i32 t3 = (i32)((f64)(now - E32(e, 0xC28)) * g_anim_divisor);
        if (3 * T <= t3) {
            if (FLAGS(e) & 0x2000) {
                actor_move_to(e, E32(e, 0xE88), E32(e, 0xE90));
                E32(e, 0xBF8) = 0; E32(e, 0xC10) = 0;
                E32(e, 0xC00) = 0; E32(e, 0xC18) = 0;
                E32(e, 0xC34) = now;
                E32(e, 0xC40) = t % T;
                while (t >= T) {
                    anim_messages(e, t);
                    t -= CAN_DURATION(ANIM(e));
                    E32(e, 0xD54) = 0;
                }
                anim_messages(e, t);
                return;
            }
        } else {
            E32(e, 0xE88) = ROOT(e)->pos[0];
            E32(e, 0xE8C) = ROOT(e)->pos[1];
            E32(e, 0xE90) = ROOT(e)->pos[2];
        }
        actor_bake_root_yaw(e);
    }

    E32(e, 0xC34) = now;
    E32(e, 0xC40) = t % T;
    while (t >= T) {
        anim_messages(e, t);
        i32 dur = CAN_DURATION(ANIM(e));
        if (dur <= 0) break;
        t -= dur;
        E32(e, 0xD54) = 0;
    }
    anim_messages(e, t);

    /* sub_41FC77: the foot is the most recently planted node. */
    if (E32(e, 0xC6C) == 0 && EPTR(e, 0xF0) && EPTR(e, 0xF8))
        EPTR(e, 0xF0) = EPTR(e, 0xF8);

    obc_node_t *foot = (obc_node_t *)EPTR(e, 0xF0);
    obc_node_t *g = ROOT(e);
    if (foot) {
        node_world(g, foot);
        i32 v[3] = { NX(foot), NY(foot), NZ(foot) };
        const u8 *zone = (const u8 *)EPTR(e, 0xA8);
        void *zout = NULL;
        if (zone && (zone[0] & 4)) {
            E32(e, 0x8C) = actor_floor_zone(g->pos[0], g->pos[2], e, &zout);
            EPTR(e, 0xA8) = zout;
        } else {
            i32 h = actor_floor_zone(v[0], v[2], e, &zout);
            if (abs(h - E32(e, 0x8C)) > 30000) {
                i32 rootp[3] = { g->pos[0], g->pos[1], g->pos[2] };
                if (corridor_blocked(rootp, v) == 0) {
                    E32(e, 0x8C) = h; EPTR(e, 0xA8) = zout;
                } else {
                    E32(e, 0x8C) = actor_floor_zone(g->pos[0], g->pos[2], e, &zout);
                    EPTR(e, 0xA8) = zout;
                }
            } else {
                E32(e, 0x8C) = h; EPTR(e, 0xA8) = zout;
            }
        }
    }

    {
        static i16 pose[103];
        memset(pose, 0, sizeof(pose));
        anim_sample(ANIM(e), pose, t);
        actor_blend_pose(e, pose, E32(e, 0xC34) - E32(e, 0xC2C));
    }
    /* Player head tilt toward the nearest companion and the +0x168 bone
     * reset depend on combat state (dword_505DDC, combat_ability_table);
     * not ported. */

    /* Root motion: the planted foot stays put, the root moves instead. */
    foot = (obc_node_t *)EPTR(e, 0xF0);
    if (E32(e, 0xC04) != -1 && foot) {
        i32 old[3] = { NX(foot), NY(foot), NZ(foot) };
        node_world(g, foot);
        i32 fw[3] = { NX(foot), NY(foot), NZ(foot) };
        i32 dX = 0, dZ = 0;
        if (E32(e, 0xC80) || E32(e, 0xC98)) {
            u8 *grav = anim_gravity(e);
            f64 gg = -(f64)(grav ? *grav : 0) / 3.0;
            f64 tt = (f64)(t - E32(e, 0xC7C));
            E32(e, 0xC98) = (i32)(tt * gg * tt + tt * (f64)E32(e, 0xC80) + (f64)E32(e, 0xC8C));
            dX = E32(e, 0xC9C);
            dZ = E32(e, 0xCA0);
            E32(e, 0xC9C) = (i32)((f64)E32(e, 0xC84) * tt + (f64)E32(e, 0xC90));
            E32(e, 0xCA0) = (i32)((f64)E32(e, 0xC88) * tt + (f64)E32(e, 0xC94));
            dX = E32(e, 0xC9C) - dX;
            dZ = E32(e, 0xCA0) - dZ;
            g->pos[1] = E32(e, 0xC98);
            const u8 *zone = (const u8 *)EPTR(e, 0xA8);
            if (zone && (zone[0] & 4)) {
                E32(e, 0xC8C) += (E32(e, 0x8C) - E32(e, 0xCAC)) / 4;
                E32(e, 0xCAC) += (E32(e, 0x8C) - E32(e, 0xCAC)) / 4;
            }
        } else if (E32(e, 0xEC) == 0) {
            const u8 *zone = (const u8 *)EPTR(e, 0xA8);
            if (E32(e, 0xB0) == 0x40)
                g->pos[1] += E32(e, 0x8C) - lowest_node_y(e);
            else if (zone && ((zone[0] & 4) || (zone[0] & 0x20)))
                g->pos[1] += (E32(e, 0x8C) - fw[1]) / 8;
            else
                g->pos[1] += E32(e, 0x8C) - fw[1];
        }
        i32 dlt[3] = { NX(foot) - old[0], NY(foot) - old[1], NZ(foot) - old[2] };
        if (E32(e, 0xC80) || E32(e, 0xC98)) {
            u32 a = (u32)g->rot[2] & 0xFFFF;
            i32 c = cosine_table[((a - 0x3FF8) >> 4) & 0xFFF];
            i32 s = cosine_table[((a + 8) >> 4) & 0xFFF];
            E32(e, 0xC00) = (c * dZ + s * dX) >> 14;
            E32(e, 0xBF8) = (-(c * dX) + s * dZ) >> 14;
        } else {
            E32(e, 0xBF8) = -dlt[0];
            E32(e, 0xBFC) = -dlt[1];
            E32(e, 0xC00) = -dlt[2];
        }
        if ((FLAGS(e) & 4) || actor_collide(e, 3) == 0) {
            E32(e, 0xC10) = -E32(e, 0xBF8);
            E32(e, 0xC18) = -E32(e, 0xC00);
            E32(e, 0xC3C) = 0;
        } else {
            E32(e, 0xC3C) = 1;
        }
    }
    if ((FLAGS(e) & 0x200) && E32(e, 0xC40) == T - 1) {
        E32(e, 0xBF8) = 0; E32(e, 0xC10) = 0;
        E32(e, 0xC00) = 0; E32(e, 0xC18) = 0;
    }
    E32(e, 0xC04) = 0;
    EPTR(e, 0xF4) = EPTR(e, 0xF0);
}

/* =========================================================================
 * Locomotion controller — sub_4193CC and helpers
 * ========================================================================= */

#define D60_SET(e) (E32(e, 0xD60) || ((e) != g_player_entity && g_cinematic_mode))

static void play_clip(entity_t *e, const c8 *clip) {
    actor_play_anim(e, can_get_anim(clip), 0);
}

/* prepare + play, or the fixed fallback clip when the persona has none. */
static void play_state_or(entity_t *e, i32 st, const c8 *fallback, i32 keep_phase) {
    c8 clip[16] = {0};
    if (actor_prepare_anim(e, st, clip)) actor_play_anim(e, can_get_anim(clip), keep_phase);
    else                                 actor_play_anim(e, can_get_anim(fallback), keep_phase);
}

static void slope_flag(entity_t *e) {
    if (E32(e, 0xD0C) && ANIM(e)) {
        CAN_FLAGS(ANIM(e)) |= 0x100;
        CAN_BLEND(ANIM(e)) = 0;
    }
}

static void turn_tail(entity_t *e) {
    obc_node_t *g = ROOT(e);
    i32 now = now_ms();
    if (E32(e, 0xCF4) == 1 && now - g_last_event_check_time > 200) {
        g->rot[2] += (now - E32(e, 0xE8)) << 5;
        E32(e, 0xCF8) = E32(e, 0xCF4);
    }
    if (E32(e, 0xCF4) == -1 && now - g_last_event_check_time > 200) {
        g->rot[2] -= (now - E32(e, 0xE8)) << 5;
        E32(e, 0xCF8) = E32(e, 0xCF4);
    }
}

static i32 slope_state(entity_t *e) {                         /* sub_418EE5 */
    const u8 *d = EDESC(e);
    if (!d || d[0x9A] == 0) return 0;
    /* get_distance_between_entities (standing on another object) is not
     * ported: the reference height is always the actor's own floor. */
    i32 ref = E32(e, 0x8C);
    const u8 *zone = (const u8 *)EPTR(e, 0xA8);
    if (E32(e, 0xD0C) != 0) {
        if (E32(e, 0xD10) != 0 && zone && !(zone[0] & 4) && now_ms() - E32(e, 0xCE0) > 400) {
            E32(e, 0xCE0) = now_ms();
            return 0;
        }
        if (E32(e, 0x8C) < ref) return 1;
        if (E32(e, 0x8C) > ref) return -1;
        return E32(e, 0xD0C);
    }
    if (zone && (zone[0] & 4) && now_ms() - E32(e, 0xCE0) > 400) {
        if (E32(e, 0x8C) < ref && E32(e, 0xD10) == 0) { E32(e, 0xCE0) = now_ms(); return 1; }
        if (E32(e, 0x8C) > ref && E32(e, 0xD10) == 0) { E32(e, 0xCE0) = now_ms(); return -1; }
    }
    return 0;
}

static void loco_forward(entity_t *e) {                      /* sub_419B2C */
    i32 st;
    if (D60_SET(e)) {
        st = 1;   /* the companion-proximity walk (state 63) needs get_entity_distance */
        if (E32(e, 0xD58) != st) E32(e, 0xCF0) = 0;
    } else if (E32(e, 0xD0C) != 0) {
        st = (E32(e, 0xD0C) == 1) ? -11 : -13;
    } else if (g_special_mode_active || e != g_player_entity) {
        st = g_level_slow - 8;
    } else {
        st = 58;
    }
    if (E32(e, 0xCEC) == 4) {
        if (E32(e, 0xCF0) != 4) {
            actor_bake_root_yaw(e);
            play_state_or(e, D60_SET(e) ? st : -1, "p01mar01", 1);
            E32(e, 0xCF0) = 4;
        }
        return;
    }
    if (E32(e, 0xCF0) != 1) {
        actor_bake_root_yaw(e);
        play_state_or(e, st, "p01mar01", 1);
        slope_flag(e);
        E32(e, 0xD2C) = now_ms();
        E32(e, 0xCF0) = 1;
    }
    turn_tail(e);
}

static void loco_backward(entity_t *e) {                     /* sub_419F86 */
    i32 st;
    if (D60_SET(e))                                         st = 2;
    else if (g_special_mode_active || e != g_player_entity) st = g_level_slow - 10;
    else                                                    st = 61;
    if (E32(e, 0xCF0) != -1) {
        actor_bake_root_yaw(e);
        play_state_or(e, st, "p01mar05", 0);
        slope_flag(e);
        E32(e, 0xCF0) = -1;
    }
    turn_tail(e);
}

static void loco_idle(entity_t *e) {                         /* sub_41A194 */
    c8 clip[16] = {0};
    i32 st;
    if (D60_SET(e)) {
        st = 5;
    } else {
        st = (!g_special_mode_active && e == g_player_entity && E32(e, 0xB0) < 4) ? 59 : -5;
        if (!g_player_movement_locked && actor_anim_done(e) &&
            e == g_player_entity && E32(e, 0xB0) <= 3 && E32(e, 0xC78) == 0) {
            /* Companions within 300000 suppress the fidgets; that distance
             * test is not ported, so fidgets play whenever idle runs out. */
            if (E32(e, 0xE4) != 0) {
                E32(e, 0xCF0) = 1;
            } else {
                if (E32(e, 0xD58) == -5 || E32(e, 0xD58) == 59) {
                    do st = rand() & 3; while (st == g_idle_fidget_last);
                    g_idle_fidget_last = st;
                    st = (st == 0) ? (g_special_mode_active ? 55 : 54) : st + 1000;
                    E32(e, 0xCF0) = 1;
                }
                if (E32(e, 0xD58) >= 1000) {
                    st = g_special_mode_active ? -5 : 59;
                    E32(e, 0xCF0) = 1;
                }
            }
            E32(e, 0xE4) = 0;
        }
    }
    /* sub_41AD23 (CF0 == 3 and the restart path) is not ported. */
    if (E32(e, 0xCF0) != 0) {
        E32(e, 0xC30) = now_ms();
        E32(e, 0xCFC) = now_ms();
        E32(e, 0xCF8) = 0;
        actor_bake_root_yaw(e);
        if (st >= 1000) {
            if (st == 1001) st = 1;
            if (st == 1002) st = 4;
            if (st == 1003) st = 7;
            if (!g_special_mode_active) st += 9;
            snprintf(clip, sizeof(clip), "g%02darb%02d", 0 /* game_state: hero */, st);
        } else {
            actor_prepare_anim(e, st, clip);
        }
        play_clip(e, clip);
        E32(e, 0xD58) = actor_anim_state(e, clip);
        E32(e, 0xCF0) = 0;
        return;
    }
    if (E32(e, 0xCF4) == 0) {
        if (E32(e, 0xCF8) != 0) {
            E32(e, 0xC30) = now_ms();
            actor_bake_root_yaw(e);
            play_state_or(e, st, "g01arr09", 0);
            E32(e, 0xC04) = -1;
        }
        E32(e, 0xCF8) = E32(e, 0xCF4);
    }
    if (E32(e, 0xCF4) == 1) {
        i32 keep = 1;
        if (E32(e, 0xCF8) != 1) {
            if (D60_SET(e)) st = 3;
            else { st = -3; if (!g_special_mode_active && e == g_player_entity) st = 56; }
            if (now_ms() - E32(e, 0xCFC) > 300) {
                E32(e, 0xC30) = now_ms();
                actor_bake_root_yaw(e);
                play_state_or(e, st, "p01mar21", 0);
            } else keep = 0;
        }
        E32(e, 0xCF8) = keep;
    }
    if (E32(e, 0xCF4) == -1) {
        i32 keep = -1;
        if (E32(e, 0xCF8) != -1) {
            if (D60_SET(e)) st = 4;
            else { st = -4; if (!g_special_mode_active && e == g_player_entity) st = 57; }
            if (now_ms() - E32(e, 0xCFC) > 300) {
                E32(e, 0xC30) = now_ms();
                play_state_or(e, st, "p01mar17", 0);
            } else keep = 0;
        }
        E32(e, 0xCF8) = keep;
    }
}

static void loco_scripted(entity_t *e) {                     /* sub_41A8B2 */
    c8 clip[16] = {0};
    E32(e, 0xCF0) = 0; E32(e, 0xCEC) = 0;
    E32(e, 0xCF8) = 0; E32(e, 0xCF4) = 0;
    if (E32(e, 0xD04) != E32(e, 0xD08)) {
        i32 st = E32(e, 0xD04);
        actor_bake_root_yaw(e);
        if (actor_prepare_anim(e, st, clip)) play_clip(e, clip);
        if (ANIM(e) && (CAN_FLAGS(ANIM(e)) & 0x40) && EDESC(e) && st >= -1 && st < 0x80)
            *(i32 *)(EDESC(e) + 4 + st * 4) = *(u16 *)&ANIM(e)->hdr[0x100];
        E32(e, 0xD08) = E32(e, 0xD04);
        if (ANIM(e)) CAN_FLAGS(ANIM(e)) |= 0x80;   /* is_creature_special_level_type == 0 */
    }
}

void actor_control(entity_t *e) {                            /* sub_4193CC */
    c8 clip[16] = {0};
    const u8 *d = EDESC(e);
    if (!e || !ROOT(e) || (d && d[0x96])) return;
    /* D80/D88/D90 timed effect (sub_42D81F) not ported. */

    obc_node_t *g = ROOT(e);
    if (E32(e, 0xD14) == 0) {
        g->pos[0] += E32(e, 0xBF8);
        g->pos[2] += E32(e, 0xC00);
        if (E32(e, 0xEC) != 0 && E32(e, 0xC80) == 0 && E32(e, 0xC98) == 0)
            g->pos[1] += E32(e, 0xBFC);
        if (EDESC(e)) {
            *(i32 *)(EDESC(e) + 0x88) = g->pos[0];
            *(i32 *)(EDESC(e) + 0x8C) = g->pos[2];
            *(i32 *)(EDESC(e) + 0x90) = g->rot[2];
        }
        E32(e, 0xBF8) = 0; E32(e, 0xBFC) = 0; E32(e, 0xC00) = 0;
    }

    E32(e, 0xD0C) = slope_state(e);
    if (E32(e, 0xD10) != E32(e, 0xD0C)) {
        E32(e, 0xD10) = E32(e, 0xD0C);
        if (E32(e, 0xCEC) != 0) E32(e, 0xCF0) = 0;
    }

    if (E32(e, 0xD00) == 4) return;
    if (E32(e, 0xB0) < 4) {
        E32(e, 0xD00) = 0;
    } else if (E32(e, 0xD00) == 1 || E32(e, 0xD00) == 2) {
        i32 one_shot = E32(e, 0xD00) == 1 ? 30 : 31;
        if (actor_prepare_anim(e, one_shot, clip)) {
            actor_bake_root_yaw(e);
            play_clip(e, clip);
            E32(e, 0xD00) = 3;
        } else {
            if (E32(e, 0xD00) == 2) { E32(e, 0xCEC) = 0; E32(e, 0xCF0) = 1; }
            E32(e, 0xD00) = 0;
        }
        return;
    } else if (E32(e, 0xD00) == 3) {
        if (!actor_anim_done(e)) return;
        E32(e, 0xD00) = 0;
        E32(e, 0xCEC) = 0;
        E32(e, 0xCF0) = 1;
        obc_node_t *w = (obc_node_t *)EPTR(e, 0x178);
        if (w) w->rot[0] = w->rot[1] = w->rot[2] = 0;
    }

    if (E32(e, 0xD04) != 0) {
        loco_scripted(e);
        E32(e, 0xE8) = now_ms();
        return;
    }
    if (!(E32(e, 0xCF0) == 3 && (E32(e, 0xC80) || E32(e, 0xC98)))) {
        i32 m = E32(e, 0xCEC);
        if (m == 1 || m == 4)   loco_forward(e);
        if (m == -1 || m == -4) loco_backward(e);
        if (m == 0)             loco_idle(e);
    }
    if (E32(e, 0xCEC) == 3) {
        if (E32(e, 0xCF0) != 3) {
            actor_bake_root_yaw(e);
            i32 st = E32(e, 0xD0C) == 0 ? -6 : E32(e, 0xD0C) == 1 ? -12 : -14;
            play_state_or(e, st, "p01mar53", 0);
            slope_flag(e);
            E32(e, 0xD2C) = now_ms();
            E32(e, 0xCF0) = 3;
        }
        turn_tail(e);
    }
    if ((E32(e, 0xCEC) == 2 && E32(e, 0xCF0) != 2) ||
        (E32(e, 0xCEC) == -2 && E32(e, 0xCF0) != -2)) {
        i32 m = E32(e, 0xCEC);
        actor_bake_root_yaw(e);
        actor_prepare_anim(e, m == 2 ? -15 : -16, clip);
        if (E32(e, 0xD60)) clip[5] = 'c';
        play_clip(e, clip);
        E32(e, 0xCF0) = m;
    }
    E32(e, 0xE8) = now_ms();
}

/* =========================================================================
 * Walk system — sub_469222 and helpers
 * ========================================================================= */

/* WRAPDIFF exactly as compiled (both inputs 0..0xFFFF). */
static i32 wrapdiff(i32 a, i32 b) {
    i32 r = a - b;
    if (b <= a) { if (r > 0x8000) r -= 0x10000; }
    else        { if (r < -0x8000) r += 0x10000; }
    return r;
}

static i32 heading_to(f64 dx, f64 dz) {                     /* sub_4440A6 */
    return (-(i32)(atan2(dx, dz) * 65536.0 / 6.283185307179586)) & 0xFFFF;
}

/* sub_461BA0 @ 0x461BA0 — give up and snap to the target. */
static void walk_abort(entity_t *e) {
    obc_node_t *g = ROOT(e);
    E32(e, 0xCEC) = 0; E32(e, 0xCF4) = 0; E32(e, 0xCF0) = 1; E32(e, 0xD14) = 0;
    E32(e, 0xBF8) = E32(e, 0xCC0) - g->pos[0];
    E32(e, 0xC00) = E32(e, 0xCC4) - g->pos[2];
    if (actor_collide(e, 3) == 0) { g->pos[0] += E32(e, 0xBF8); g->pos[2] += E32(e, 0xC00); }
    E32(e, 0xBF8) = 0; E32(e, 0xC00) = 0;
    g->rot[2] = E32(e, 0xCC8);
    E32(e, 0xCDC) = 1; E32(e, 0xCBC) = 0; E32(e, 0xC44) = 0;
}

/* sub_428714 @ 0x428714 — steer toward (x, z). 1 = arrived, 0 = moving. */
i32 actor_steer(entity_t *e, i32 x, i32 z) {
    if (!e || !ROOT(e)) return 1;
    obc_node_t *g = ROOT(e);
    i32 tgt_h = E32(e, 0xCC8);
    f64 dx = (f64)(x - g->pos[0]), dz = (f64)(z - g->pos[2]);
    f64 dist = sqrt(dx * dx + dz * dz) / 600.0;
    i32 cur;
    if (ANIM(e) && (CAN_FLAGS(ANIM(e)) & 1) && g->child)
        cur = (g->rot[2] - (g->child->rot[0] - CAN_ROOT_YAW(ANIM(e)))) & 0xFFFF;
    else
        cur = g->rot[2] & 0xFFFF;
    tgt_h &= 0xFFFF;
    E32(e, 0xD14) = 0;
    if (E32(e, 0xD04) != 0) {
        E32(e, 0xD08) = 0; E32(e, 0xD04) = 0;
        E32(e, 0xCEC) = 0;
        E32(e, 0xCF0) = 0x7CD;
    }
    if (dist < 15.0) {
        if (E32(e, 0xCBC) == 2) E32(e, 0xCEC) = 0;
        E32(e, 0xBF8) = x - g->pos[0];
        E32(e, 0xC00) = z - g->pos[2];
        if (actor_collide(e, 3) == 0) { g->pos[0] += E32(e, 0xBF8); g->pos[2] += E32(e, 0xC00); }
        E32(e, 0xBF8) = 0; E32(e, 0xC00) = 0;
        if (E32(e, 0xCC8) == -1) {
            if (E32(e, 0xCBC) != 3) E32(e, 0xCBC) = 4;
            E32(e, 0xC04) = -1; E32(e, 0xCF8) = 0; E32(e, 0xCF4) = 0; E32(e, 0xCF0) = 1;
            return 1;
        }
        i32 dd = -wrapdiff(tgt_h, cur);
        if (E32(e, 0xCBC) == 3) return 1;
        if (E32(e, 0xCB4) != 0) {
            if (E32(e, 0xCBC) != 0) {
                E32(e, 0xD14) = 0; E32(e, 0xCDC) = 1; E32(e, 0xCBC) = 0; E32(e, 0xC44) = 0;
            } else {
                E32(e, 0xCBC) = 4;
                g->rot[2] -= dd;
            }
            return 1;
        }
        if (abs(dd) < 0x400) {
            E32(e, 0xC04) = -1; E32(e, 0xCBC) = 4;
            E32(e, 0xCF8) = 0; E32(e, 0xCF4) = 0; E32(e, 0xCF0) = 1;
            g->rot[2] -= dd;
        } else if (E32(e, 0xCBC) != 2) {
            g->rot[2] -= dd;
        } else {
            E32(e, 0xD14) = 1;
            if (dd < 0) E32(e, 0xCF4) = 1;
            else        E32(e, 0xCF4) = -1;
        }
        return 1;
    }

    i32 dd = -wrapdiff(heading_to(dx, dz), cur);
    if (e == g_player_entity) {
        if (abs(dd) < 0x400) {
            if (dist < 50.0 && E32(e, 0xCEC) != 1) E32(e, 0xCEC) = 4;
            else                                   E32(e, 0xCEC) = 1;
            g->rot[2] -= dd;
            E32(e, 0xCF4) = 0;
            return 0;
        }
        if (dist < 100.0 && abs(dd) > 0x6000) {
            E32(e, 0xCEC) = -1;
            if (dd < 0) g->rot[2] -= dd + 0x8000;
            else        g->rot[2] -= dd - 0x8000;
            E32(e, 0xCF4) = 0;
            return 0;
        }
        if (abs(dd) > 0x400) { E32(e, 0xD14) = 1; g->rot[2] -= dd / 4; }
        return 0;
    }
    if (abs(dd) <= 0x400) {
        E32(e, 0xCEC) = (E32(e, 0xCD4) && dist > 100.0) ? 3 : 1;
        g->rot[2] -= dd;
        E32(e, 0xCF4) = 0;
        return 0;
    }
    E32(e, 0xD14) = 1;
    g->rot[2] -= dd / 4;
    return 0;
}

/* sub_468FFE — a clear straight line to the target or a 6000-unit neighbour. */
static i32 walk_clear_line(const i32 *from, const i32 *tgt, i32 *out) {
    static const i32 off[9][2] = { {0,0}, {0,6000}, {6000,6000}, {6000,0}, {6000,-6000},
                                   {0,-6000}, {-6000,-6000}, {-6000,0}, {-6000,6000} };
    out[0] = tgt[0]; out[2] = tgt[2];
    if (corridor_blocked(from, tgt) == 0) return 1;
    for (i32 k = 1; k < 9; k++) {
        out[0] = tgt[0] + off[k][0];
        out[2] = tgt[2] + off[k][1];
        if (corridor_blocked(from, out) == 0) return 1;
    }
    return 0;
}

/* sub_477FD4 — recursive 4-way flood fill (bounds-checked here). */
static void flood(u8 *g, i32 idx, u8 v) {
    g[idx] = v;
    u8 n = (u8)(v + 1);
    if (idx + 1  < 0x1000 && n < g[idx + 1])  flood(g, idx + 1, n);
    if (idx - 1  >= 0     && n < g[idx - 1])  flood(g, idx - 1, n);
    if (idx + 64 < 0x1000 && n < g[idx + 64]) flood(g, idx + 64, n);
    if (idx - 64 >= 0     && n < g[idx - 64]) flood(g, idx - 64, n);
}

static i32 nav_index(i32 x, i32 z) {
    if (g_nav.cell_width <= 0 || g_nav.cell_height <= 0) return -1;
    return (x - g_nav.world_x_origin) / g_nav.cell_width +
           (((z - g_nav.world_y_origin) / g_nav.cell_height) << 6);
}

/* sub_45720D — distance field from (x, z) in the actor's nav workspace. */
static void walk_flood_field(entity_t *e, i32 x, i32 z) {
    u8 *g = (u8 *)EPTR(e, 0xCD8);
    if (!g) return;
    memset(g, 0, 0x1000);
    for (i32 i = 0; i < 0x1000; i++) if (g_nav.grid[i] == NAV_FREE) g[i] = 0xFF;
    i32 idx = nav_index(x, z), col = idx % 64;
    if (idx >= 0x40 && idx < 0xFC0 && col != 0 && col != 0x3F) flood(g, idx, 0);
    for (i32 i = 0; i < 0x1000; i++) if (g[i] == 0) g[i] = 0xFF;
}

/* sub_46977B — step toward the lowest of the 8 neighbours. */
static void walk_descend(entity_t *e, i32 idx, i32 *dx, i32 *dz, i32 *best) {
    const u8 *g = (const u8 *)EPTR(e, 0xCD8);
    if (!g || idx < 0x41 || idx > 0xFBE) return;
    i32 v = g[idx];
    if (g[idx + 0x01] < v) { *dx =  1; *dz =  0; v = g[idx + 0x01]; }
    if (g[idx + 0x41] < v) { *dx =  1; *dz =  1; v = g[idx + 0x41]; }
    if (g[idx + 0x40] < v) { *dx =  0; *dz =  1; v = g[idx + 0x40]; }
    if (g[idx + 0x3F] < v) { *dx = -1; *dz =  1; v = g[idx + 0x3F]; }
    if (g[idx - 0x01] < v) { *dx = -1; *dz =  0; v = g[idx - 0x01]; }
    if (g[idx - 0x41] < v) { *dx = -1; *dz = -1; v = g[idx - 0x41]; }
    if (g[idx - 0x40] < v) { *dx =  0; *dz = -1; v = g[idx - 0x40]; }
    if (g[idx - 0x3F] < v) { *dx =  1; *dz = -1; v = g[idx - 0x3F]; }
    *best = v;
}

/* sub_468D56 — choose the next waypoint. 1 = gave up (snapped). */
static i32 walk_next_waypoint(entity_t *e) {
    obc_node_t *g = ROOT(e);
    i32 steps = 0, x = g->pos[0], z = g->pos[2];
    i32 dx = 0, dz = 0, best = 0, px = x, pz = z;
    i32 rootp[3] = { g->pos[0], g->pos[1], g->pos[2] };
    i32 idx = nav_index(x, z);
    walk_descend(e, idx, &dx, &dz, &best);
    idx += (dz << 6) + dx;
    x += dx * g_nav.cell_width;
    z += dz * g_nav.cell_height;
    while (best != 0) {
        if (steps++ >= 0x5C) break;
        px = x; pz = z;
        walk_descend(e, idx, &dx, &dz, &best);
        idx += (dz << 6) + dx;
        x += dx * g_nav.cell_width;
        z += dz * g_nav.cell_height;
        i32 probe[3] = { x, 0, z };
        if (corridor_blocked(rootp, probe)) break;
    }
    if (steps > 0x5A) { walk_abort(e); return 1; }
    i32 probe[3] = { px, 0, pz };
    if (corridor_blocked(rootp, probe)) {
        f64 fx = (f64)(E32(e, 0xCC0) - g->pos[0]), fz = (f64)(E32(e, 0xCC4) - g->pos[2]);
        f64 len = sqrt(fz * fz + fx * fx);
        f64 s = len > 0 ? 6000.0 / len : 0;
        i32 ix = (i32)(s * fx), iz = (i32)(s * fz);
        i32 off[3][2] = { { ix, iz }, { -iz, ix }, { iz, -ix } };
        for (i32 k = 1; k < 5; k++)
            for (i32 j = 0; j < 3; j++) {
                probe[0] = px - off[j][0] * k;
                probe[2] = pz - off[j][1] * k;
                if (!corridor_blocked(rootp, probe)) { px = probe[0]; pz = probe[2]; goto done; }
            }
    }
done:
    E32(e, 0xCCC) = px;
    E32(e, 0xCD0) = pz;
    return 0;
}

void actor_walk_update(entity_t *e) {                        /* sub_469222 */
    if (!e || !ROOT(e)) return;
    obc_node_t *g = ROOT(e);
    i32 now = now_ms();

    if (E32(e, 0xCB4) != 0) {                                /* face a joined actor */
        entity_t *t = NULL;
        if (g_player_entity && E32(e, 0xCB8) == E32(g_player_entity, 0xB0)) t = g_player_entity;
        if (g_companion_entity && E32(g_companion_entity, 0xB0) == E32(e, 0xCB8)) t = g_companion_entity;
        if (g_secondary_companion_entity && E32(g_secondary_companion_entity, 0xB0) == E32(e, 0xCB8))
            t = g_secondary_companion_entity;
        if (t && ROOT(t)) {
            i32 ang = heading_to((f64)(ROOT(t)->pos[0] - g->pos[0]), (f64)(ROOT(t)->pos[2] - g->pos[2]));
            E32(e, 0xCC8) = ang;
            actor_steer(e, g->pos[0], g->pos[2]);
            return;
        }
    }
    if (now - E32(e, 0xC0) > 15000 && E32(e, 0xCBC) != 1) {
        walk_abort(e);
        return;
    }
    i32 tgt[3] = { E32(e, 0xCC0), 0, E32(e, 0xCC4) };
    i32 from[3] = { g->pos[0], g->pos[1], g->pos[2] };
    i32 out[3] = { 0, 0, 0 };

    if (E32(e, 0xCBC) == 4) {
        E32(e, 0xD14) = 0; E32(e, 0xCBC) = 0; E32(e, 0xC44) = 0; E32(e, 0xCDC) = 1;
    }
    if (E32(e, 0xCBC) == 1) {
        E32(e, 0xC0) = now;
        if (walk_clear_line(from, tgt, out)) {
            E32(e, 0xCC0) = out[0]; E32(e, 0xCC4) = out[2];
            E32(e, 0xCBC) = 2;
            actor_steer(e, E32(e, 0xCC0), E32(e, 0xCC4));
            return;
        }
        f64 dx = (f64)(E32(e, 0xCC0) - g->pos[0]), dz = (f64)(E32(e, 0xCC4) - g->pos[2]);
        if (dx * dx + dz * dz < 48000.0) {
            E32(e, 0xD14) = 0; E32(e, 0xCBC) = 0;
            g->rot[2] = E32(e, 0xCC8);
            return;
        }
        nav_snap_to_walkable(&E32(e, 0xCC0), &E32(e, 0xCC4));
        walk_flood_field(e, E32(e, 0xCC0), E32(e, 0xCC4));
        if (walk_next_waypoint(e)) return;
        actor_steer(e, E32(e, 0xCCC), E32(e, 0xCD0));
        E32(e, 0xCBC) = 3;
    }
    if (E32(e, 0xCBC) == 2)
        actor_steer(e, E32(e, 0xCC0), E32(e, 0xCC4));
    if (E32(e, 0xCBC) == 3) {
        from[0] = g->pos[0]; from[2] = g->pos[2];
        if (walk_clear_line(from, tgt, out)) {
            E32(e, 0xCC0) = out[0]; E32(e, 0xCC4) = out[2];
            E32(e, 0xCBC) = 2;
            actor_steer(e, E32(e, 0xCC0), E32(e, 0xCC4));
            return;
        }
        if (actor_steer(e, E32(e, 0xCCC), E32(e, 0xCD0))) {
            E32(e, 0xC0) = now;
            E32(e, 0xBF8) = E32(e, 0xCCC) - g->pos[0];
            E32(e, 0xC00) = E32(e, 0xCD0) - g->pos[2];
            if (actor_collide(e, 3) == 0) { g->pos[0] += E32(e, 0xBF8); g->pos[2] += E32(e, 0xC00); }
            E32(e, 0xBF8) = 0; E32(e, 0xC00) = 0;
            if (walk_next_waypoint(e)) return;
            actor_steer(e, E32(e, 0xCCC), E32(e, 0xCD0));
        }
    }
}

/* =========================================================================
 * Frame order — sub_461468 @ 0x46162B..0x461851
 * ========================================================================= */

static void companion_tick(entity_t *c) {
    if (!c || !EDESC(c) || EDESC(c)[0x96]) return;
    /* +0xD60 combat AI (sub_4701E0) not ported. */
    actor_update(c);
    /* sub_435A65(c, floor): effect anchor, not ported. */
    actor_control(c);
    if (E32(c, 0xCBC) != 0 || E32(c, 0xCB4) != 0) actor_walk_update(c);
}

/* sub_46642E @ 0x46642E, the player half. Input handling lives in scene.c
 * until sub_451BF9 is ported. */
static void player_tick(void) {
    entity_t *p = g_player_entity;
    if (!p) return;
    if (E32(p, 0xCBC) != 0) { actor_walk_update(p); return; }
    if (g_player_movement_locked) {
        E32(p, 0xCEC) = 0;
        E32(p, 0xCF4) = 0;
        if (E32(p, 0xD04) != 0 && actor_anim_done(p)) {
            c8 clip[16] = {0};
            E32(p, 0xCF0) = 1;
            E32(p, 0xD04) = 0;
            if (E32(p, 0xD60))            actor_prepare_anim(p, 5, clip);
            else if (g_special_mode_active) actor_prepare_anim(p, -5, clip);
            else                          actor_prepare_anim(p, 59, clip);
            actor_bake_root_yaw(p);
            actor_play_anim(p, can_get_anim(clip), 0);
        }
    }
}

static void actor_trace(u32 now) {
    static u32 last = 0;
    if (!getenv("DKE_ACTORDBG") || now - last < 500) return;
    last = now;
    entity_t *cast[3] = { g_player_entity, g_companion_entity, g_secondary_companion_entity };
    for (i32 i = 0; i < 3; i++) {
        entity_t *e = cast[i];
        if (!e || !ROOT(e)) continue;
        if (getenv("DKE_POSEDBG")) {
            c8 buf[512]; i32 o = 0;
            for (i32 j = 0; j < 8; j++) {
                obc_node_t *b = EBONE(e, j);
                if (b) o += snprintf(buf + o, sizeof(buf) - o, " %s(%d,%d,%d)", b->name, b->rot[0], b->rot[1], b->rot[2]);
            }
            print("actor: pose p%02d%s\n", E32(e, 0xB0), buf);
        }
        print("actor: t=%u p%02d pos=(%d,%d,%d) hdg=%04X anim=%s t=%d/%d cec=%d cbc=%d foot=%s\n",
              now, E32(e, 0xB0), ROOT(e)->pos[0], ROOT(e)->pos[1], ROOT(e)->pos[2],
              ROOT(e)->rot[2] & 0xFFFF, ESTR(e, 0xC4), actor_anim_time(e),
              ANIM(e) ? CAN_DURATION(ANIM(e)) : 0, E32(e, 0xCEC), E32(e, 0xCBC),
              EPTR(e, 0xF0) ? ((obc_node_t *)EPTR(e, 0xF0))->name : "-");
    }
}

void actor_frame(u32 now) {
    actor_trace(now);
    if (g_player_entity) actor_update(g_player_entity);
    companion_tick(g_companion_entity);
    companion_tick(g_secondary_companion_entity);
    /* 0x4617A3: time_backup = current_game_time; current_game_time = now. */
    if (state) {
        state->time_backup = state->current_game_time;
        state->current_game_time = now;
    }
    actor_update_timing(now);
    player_tick();
    lieu_action_run(now);                         /* sub_44CDB8 */
    if (g_player_entity) actor_control(g_player_entity);
}
