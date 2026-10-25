/*
 * lieu_difftest_harness.c — C side of tools/lieu_difftest.py.
 *
 * Links one level port (src/levels/lieuNN*.c) against mock callbacks and
 * runs it on cases read from stdin. It does not link the engine or
 * lieu_common.c unless built with -DREAL_SHARED (ops 4/5/6): otherwise
 * lieu_action_register, lieu_pre_scene and the shared event handlers are
 * traced stubs, so the trace stays at the level of the DLL's own calls.
 * With -DHAVE_LEVEL_RESET -DLEVEL_RESET=lNN_reset_dll_globals the level's
 * DLL globals are reset before every case.
 *
 * Build (done by the script):
 *   clang -std=c99 -O1 -w -Isrc -Isrc/libsys -DLEVEL_ENTRY=lieuNN_fonction_editor \
 *         -DCB_MOCKS_INC='"/path/cbmocks.inc"' tools/lieu_difftest_harness.c \
 *         src/levels/lieuNN*.c
 *
 * Input, per case:
 *   CASE op arg arg2 npool
 *   npool integers: the return-value pool
 *   1024 integers: the state block (4096 bytes)
 * Output, per case: "T <call>" for every callback call in order,
 * "W <offset> <value>" for every state dword that changed, "R <ret>" and
 * "END".
 *
 * Callbacks that return a value draw it from the pool: the index is
 * hash(call text) + 2654435761 * (times this exact call was made before),
 * so both sides get the same value for the same call regardless of
 * evaluation order. tools/lieu_difftest.py repeats the formula.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lieu_cb.h"
#include "levels/lieu_level.h"

i32 LEVEL_ENTRY(i32 op, lieu_state_t *state, i32 arg, i32 arg2);
#ifdef HAVE_LEVEL_RESET
void LEVEL_RESET(void);
#endif

lieu_state_t *g_lvl_S;
__attribute__((weak)) i32 g_lieu_trace;
const lieu_cb_t *g_lieu_cb;

static i32 *g_pool;
static int g_npool;
static lieu_state_t g_stbuf[0x4000];
static u64 g_keys[8192];
static int g_cnt[8192];

static u64 fnv(const char *s) {
    u64 h = 0xcbf29ce484222325ULL;
    for (; *s; s++) { h ^= (u8)*s; h *= 0x100000001b3ULL; }
    return h;
}
static i32 draw(const char *key) {
    u64 h = fnv(key);
    unsigned i = (unsigned)(h & 8191);
    while (g_keys[i] && g_keys[i] != h) i = (i + 1) & 8191;
    g_keys[i] = h;
    int c = g_cnt[i]++;
    return g_pool[(h + (u64)c * 2654435761ULL) % (u64)g_npool];
}
static char sbuf[8][512];
static int sidx;
static const char *S_(const c8 *p) {
    char *b = sbuf[sidx++ & 7];
    if (!p) return "(null)";
    snprintf(b, 512, "\"%s\"", p);
    return b;
}
/* a pointer argument: into the state block, or to a DLL global of the level */
static const char *P_(const void *p) {
    char *b = sbuf[sidx++ & 7];
    if ((const lieu_state_t *)p >= g_stbuf && (const lieu_state_t *)p < g_stbuf + 0x1000 / sizeof(lieu_state_t))
        snprintf(b, 512, "&ST(%d)", (int)((const lieu_state_t *)p - g_stbuf));
    else
        snprintf(b, 512, "&DLL-global");
    return b;
}
#define OUT(...) do { printf("T "); printf(__VA_ARGS__); printf("\n"); } while (0)
static char kb[1024];
#define KEY(...) snprintf(kb, sizeof kb, __VA_ARGS__)

static i32 g_last_divers;   /* result of the latest divers() mock */
#ifndef REAL_SHARED
/* Shared code every DLL carries (lieu_common.c): traced stubs. */
void lieu_event4(i32 a) { OUT("lieu_event4(%d)", a); }
void lieu_event5(i32 a, i32 b) { OUT("lieu_event5(%d,%d)", a, b); }
void lieu_event6(i32 a) { OUT("lieu_event6(%d)", a); }
/* The shared helper asks Divers(DV_ANIM_NAME) and returns the text it left
 * behind; the stub turns the non-zero result into a string ("?<hex>") the
 * same way tools/lieu_difftest.py prints an unknown pointer. */
const c8 *lieu_anim_name(i32 persona, i32 state) {
    static char buf[16];
    i32 r = CB->divers(DV_ANIM_NAME, persona, state);
    if (!r) return NULL;
    snprintf(buf, sizeof buf, "?%x", (u32)r);
    return buf;
}
void lieu_action_register(const c8 *n, i32 sp, const c8 *a0, const c8 *a4) {
    OUT("lieu_action_register(%s,%d,%s,%s)", S_(n), sp, S_(a0), S_(a4));
}
void lieu_pre_scene(i32 a, i32 b, i32 c, i32 d, i32 e) {
    OUT("lieu_pre_scene(%d,%d,%d,%d,%d)", a, b, c, d, e);
}
#else
/* ev build: the real src/levels/lieu_common.c is linked; the only engine
 * function it needs is the text Divers(DV_ANIM_NAME) left behind, which the
 * mock turns into "?<hex of the last Divers result>", the way
 * tools/lieu_difftest.py prints a pointer it cannot resolve. */
const c8 *lieu_divers_text(void) {
    static char buf[16];
    snprintf(buf, sizeof buf, "?%x", (u32)g_last_divers);
    return buf;
}
void lieu_common_reset(void);
#endif

#include CB_MOCKS_INC

static void on_alarm(int sig) {
    (void)sig;
    const char m[] = "X timeout\n";
    write(1, m, sizeof m - 1);
    _exit(3);
}

int main(void) {
    g_lieu_cb = &g_cb;
    signal(SIGALRM, on_alarm);
    setvbuf(stdout, NULL, _IOFBF, 1 << 16);
    int op, arg, arg2, np, nmore;
    while (scanf(" CASE %d %d %d %d %d", &op, &arg, &arg2, &np, &nmore) == 5) {
        g_npool = np;
        g_pool = malloc(sizeof(i32) * np);
        for (int i = 0; i < np; i++) scanf("%d", &g_pool[i]);
        static i32 init[0x1000 / 4];
        for (int i = 0; i < 0x1000 / 4; i++) scanf("%d", &init[i]);
        memset(g_stbuf, 0, sizeof g_stbuf);
        memcpy(g_stbuf, init, sizeof init);
        memset(g_keys, 0, sizeof g_keys);
        memset(g_cnt, 0, sizeof g_cnt);
        static i32 more[2 * 4096];
        for (int i = 0; i < 2 * nmore; i++) scanf("%d", &more[i]);
        printf("B\n");
        fflush(stdout);
#ifdef HAVE_LEVEL_RESET
        LEVEL_RESET();      /* lNN_reset_dll_globals: the level's own DLL globals */
#endif
#ifdef REAL_SHARED
        /* The DLL's globals (float accumulators...) start fresh per case;
         * a forked child per case also resets those of a level's own
         * event copies. */
        fflush(stdout);
        pid_t pid = fork();
        if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                printf("X crashed\n");
                fflush(stdout);
                return 1;
            }
            free(g_pool);
            continue;
        }
        lieu_common_reset();
#endif
        alarm(5);
        i32 ret = LEVEL_ENTRY(op, g_stbuf, arg, arg2);
        for (int i = 0; i < nmore; i++) ret = LEVEL_ENTRY(op, g_stbuf, more[2 * i], more[2 * i + 1]);
        alarm(0);
        for (int i = 0; i < 0x1000 / 4; i++) {
            i32 v = ((i32 *)g_stbuf)[i];
            if (v != init[i]) printf("W %d %d\n", i * 4, v);
        }
        printf("R %d\nEND\n", ret);
        fflush(stdout);
#ifdef REAL_SHARED
        _exit(0);
#endif
        free(g_pool);
    }
    return 0;
}
