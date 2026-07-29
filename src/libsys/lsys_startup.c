#include <stdio.h>
#include <string.h>
#include <stdlib.h>   /* getenv */
#include <locale.h>   /* setlocale */
#include <time.h>     /* clock_gettime, time, localtime, strftime */

#ifdef PLATFORM_MACOS
#include <sys/sysctl.h>   /* sysctlbyname */
#endif

#include "lsys_startup.h"
#include "common.h"

/* ---------------------------------------------------------------------------
 * Globals — mirrors of original Win32 BSS/data segment addresses
 * --------------------------------------------------------------------------*/

/* 0x4C0B68 */
u16 g_ls_pause_state = 0;

/* 0x55C768 */
i32 g_ls_master  = 0;
/* 0x55C75C */
i32 g_ls_debug   = 0;
/* 0x55C760 */
i32 g_ls_info    = 0;
/* 0x55C764 */
i32 g_ls_lang_id = 0;

/* 0x55C79A */
c8  g_virtual_root[64] = {0};

/* 0x55C738 – default: HDISK (10) */
i32 g_ls_drive_type = 10;
/* 0x55C73C – default: drive 0 */
i32 g_ls_drive_num  = 0;

/* 0x55C89C – 0xAA0 dwords zeroed by sub_485840 */
u8  g_ls_workspace_a[0xAA0 * 4] = {0};
/* 0x55C638 – 0x44 dwords zeroed by sub_485840 */
u8  g_ls_workspace_b[0x44  * 4] = {0};

/* ---------------------------------------------------------------------------
 * sub_4953B0 buffer globals  (off_4C7748 → off_4C77D0)
 *
 * Both are 34 dwords = 136 bytes, matching rep movsd ecx=0x22.
 * TODO: identify buffer purpose (palette-related? animation?).
 * --------------------------------------------------------------------------*/
/* 0x4C7748 – source buffer (off_4C7748 in original data segment) */
u8 unknown_data_a[136] = {0};
/* 0x4C77D0 – destination buffer (off_4C77D0 in original data segment) */
u8 unknown_data_b[136] = {0};

/* ---------------------------------------------------------------------------
 * sub_495390 globals  (off_4C64EC / off_4C64F0)
 *
 * Two error/warn handler function pointers installed by sub_495390.
 * Original points them at sub_494560 and sub_4945F0 respectively.
 * --------------------------------------------------------------------------*/
/* 0x4C64EC – error handler slot 1 */
void (*g_error_handler_1)(void) = NULL;
/* 0x4C64F0 – error handler slot 2 */
void (*g_error_handler_2)(void) = NULL;

/* ---------------------------------------------------------------------------
 * sub_487D40  (00487D40)
 *
 *   mov  word_4C0B68, 0
 *   retn
 *
 * Resets the LIBSYS pause/flush state flag to 0.
 * --------------------------------------------------------------------------*/
// 00487D40
void ls_libsys_flag_reset(void) {
    g_ls_pause_state = 0;
}

/* ---------------------------------------------------------------------------
 * Default error-handler no-ops  (targets of sub_494560 / sub_4945F0)
 *
 * The original sub_495390 installs two function pointers (off_4C64EC /
 * off_4C64F0) that point to sub_494560 and sub_4945F0.  Those are the
 * default error/warning handlers.  We replace them with silent no-ops that
 * match the same void(void) signature.
 * --------------------------------------------------------------------------*/
static void ls_default_handler_a(void) { /* no-op — mirrors sub_494560 */ }
static void ls_default_handler_b(void) { /* no-op — mirrors sub_4945F0 */ }

/* ---------------------------------------------------------------------------
 * sub_495390  (0x00495390)
 *
 *   mov  off_4C64EC, offset sub_494560
 *   mov  off_4C64F0, offset sub_4945F0
 *   retn
 *
 * Installs two default error/warn handler function pointers.
 * --------------------------------------------------------------------------*/
// 00495390
void ls_install_default_error_handlers(void) {
    g_error_handler_1 = ls_default_handler_a;
    g_error_handler_2 = ls_default_handler_b;
    print("ls_install_default_error_handlers: handlers installed\n");
}

/* ---------------------------------------------------------------------------
 * sub_4953B0  (0x004953B0)
 *
 *   push  esi
 *   mov   ecx, 22h          ; 34 dwords = 136 bytes
 *   push  edi
 *   mov   esi, offset off_4C7748
 *   mov   edi, offset off_4C77D0
 *   rep   movsd
 *   pop   edi
 *   pop   esi
 *   retn
 *
 * Copies unknown_data_a → unknown_data_b (136 bytes).
 * TODO: identify buffer purpose.
 * --------------------------------------------------------------------------*/
// 004953B0
void ls_copy_unknown_a_to_b(void) {
    memcpy(unknown_data_b, unknown_data_a, sizeof(unknown_data_b));
    print("ls_copy_unknown_a_to_b: copied %u bytes\n", (u32)sizeof(unknown_data_b));
}

/* ---------------------------------------------------------------------------
 * sub_4843F0  (0x004843F0)
 *
 * CPU/memory check in original: calls QueryPerformanceFrequency, then uses
 * rdtsc across a calibration loop to estimate CPU clock in MHz.
 *
 * Port (macOS / Linux): use clock_gettime(CLOCK_MONOTONIC) for a 1-ms spin
 * to measure elapsed ticks, then derive MHz.  perf_counter_freq is set to
 * 1 000 000 000 (1 GHz — CLOCK_MONOTONIC reports in nanoseconds).
 *
 * On macOS: try sysctlbyname("hw.cpufrequency") first.
 * On Linux: parse /proc/cpuinfo "cpu MHz" line.
 * Fallback: 2000 MHz.
 * --------------------------------------------------------------------------*/
// 004843F0
void ls_measure_cpu_speed(cpu_info_t *out) {
    if (!out) return;

    out->perf_counter_freq = 1000000000ULL; /* CLOCK_MONOTONIC: 1 ns per tick */
    out->flags = 0;
    out->cpu_speed_mhz = 2000; /* safe fallback */

#ifdef PLATFORM_MACOS
    {
        u64 freq = 0;
        size_t size = sizeof(freq);
        if (sysctlbyname("hw.cpufrequency", &freq, &size, NULL, 0) == 0 && freq > 0) {
            out->cpu_speed_mhz = (u32)(freq / 1000000ULL);
            print("ls_measure_cpu_speed: hw.cpufrequency = %u MHz\n", out->cpu_speed_mhz);
            return;
        }
        /* Apple Silicon / newer macOS: try hw.cpufrequency_max */
        if (sysctlbyname("hw.cpufrequency_max", &freq, &size, NULL, 0) == 0 && freq > 0) {
            out->cpu_speed_mhz = (u32)(freq / 1000000ULL);
            print("ls_measure_cpu_speed: hw.cpufrequency_max = %u MHz\n", out->cpu_speed_mhz);
            return;
        }
    }
#endif

#ifdef PLATFORM_LINUX
    {
        FILE *f = fopen("/proc/cpuinfo", "r");
        if (f) {
            char line[256];
            while (fgets(line, sizeof(line), f)) {
                float mhz = 0.f;
                if (sscanf(line, "cpu MHz : %f", &mhz) == 1 && mhz > 0.f) {
                    out->cpu_speed_mhz = (u32)(mhz + 0.5f);
                    fclose(f);
                    print("ls_measure_cpu_speed: /proc/cpuinfo = %u MHz\n", out->cpu_speed_mhz);
                    return;
                }
            }
            fclose(f);
        }
    }
#endif

    /* Fallback path — hardcode 2000 MHz and log */
    print("ls_measure_cpu_speed: could not detect CPU speed, defaulting to %u MHz\n",
          out->cpu_speed_mhz);
}

/* ---------------------------------------------------------------------------
 * lsi_open_stat_app  (called from sub_487E30:loc_487F10)
 *
 * Original: Win32 companion process that opens f:\TEMP\LS1.CFG, reads a
 * session counter, increments it, writes a timestamped log line, then writes
 * back.  The file path and Win32 API (GetModuleFileNameA, CreateProcess) are
 * not portable.
 *
 * Port: open <game_path>/dke_stat.log for append and write a startup line.
 * If the file cannot be opened, return silently (matching the original
 * loc_4880AC early-exit path when fopen returns NULL).
 * --------------------------------------------------------------------------*/
static void lsi_open_stat_app(void) {
    /* Resolve log path relative to the running process working directory.
     * If state->game_path is not yet set we fall back to "." so the file
     * still lands somewhere visible. */
    const c8 *log_dir = ".";

    /* state is declared in state.h (included via lsys_startup.h) and defined
     * in main.c.  Use its game_path field to place the log next to the game. */
    if (state && state->game_path && state->game_path[0] != '\0') {
        log_dir = state->game_path;
    }

    c8 log_path[512];
    snprintf(log_path, sizeof(log_path), "%s/dke_stat.log", log_dir);

    FILE *f = fopen(log_path, "a");
    if (!f) {
        /* Original: fopen failure → jump to loc_4880AC → return 1 silently */
        print("lsi_open_stat_app: could not open %s (non-fatal)\n", log_path);
        return;
    }

    /* Write a timestamped startup line */
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    c8 ts[64] = {0};
    if (tm_info) {
        strftime(ts, sizeof(ts), "%d/%m/%Y : %H:%M:%S", tm_info);
    }

    fprintf(f, "DKE startup : %s\n", ts);
    fclose(f);

    print("lsi_open_stat_app: logged startup to %s\n", log_path);
}

/* ---------------------------------------------------------------------------
 * sub_487E30  (00487E30)
 *
 * Reads LIBSYS.Master / LIBSYS.Debug / LIBSYS.Info from win.ini (via
 * GetProfileStringA on Windows) and sets the four global flags.
 * Also queries the system language ID.
 *
 * Port: reads the already-parsed flags from state_t; derives language from
 * POSIX LANG env-var on Unix, or GetUserDefaultLangID on Windows.
 *
 * LANG prefix → LCID mapping (subset used by original):
 *   en_ → 0x0409 (English US)
 *   fr_ → 0x040C (French)
 *   de_ → 0x0407 (German)
 *   it_  → 0x0410 (Italian)
 *   es_ → 0x040A (Spanish)
 *   default → 0x0409
 * --------------------------------------------------------------------------*/
// 00487E30
void ls_libsys_init_flags(state_t *state) {
    if (state->ls_master_enabled) {
        g_ls_master = 1;
        g_ls_debug  = 0;
        g_ls_info   = 0;
    } else {
        g_ls_master = 0;
        g_ls_debug  = state->ls_debug_enabled ? 1 : 0;
        g_ls_info   = state->ls_info_enabled  ? 1 : 0;
        lsi_open_stat_app();
    }

#ifdef _WIN32
    /* Original: GetSystemDefaultLangID() & 0x3FF */
    {
        /* Avoid pulling in windows.h here; cast via the platform layer
         * if needed. For now replicate the logic with the LANG fallback. */
        extern unsigned short ls_platform_get_lang_id(void); /* provided by platforms/windows.c */
        g_ls_lang_id = ls_platform_get_lang_id() & 0x3FF;
    }
#else
    /* POSIX: honour the LANG environment variable */
    setlocale(LC_ALL, "");
    {
        const c8 *lang = getenv("LANG");
        i32 lcid = 0x0409; /* default: English US */
        if (lang) {
            if      (strncmp(lang, "fr_", 3) == 0) { lcid = 0x040C; }
            else if (strncmp(lang, "de_", 3) == 0) { lcid = 0x0407; }
            else if (strncmp(lang, "it_", 3) == 0) { lcid = 0x0410; }
            else if (strncmp(lang, "es_", 3) == 0) { lcid = 0x040A; }
            /* en_ and anything else → 0x0409 */
        }
        g_ls_lang_id = lcid & 0x3FF;
    }
#endif

    state->ls_lang_id = g_ls_lang_id;

    print("ls_libsys_init_flags: master=%d debug=%d info=%d lang=0x%03X\n",
          g_ls_master, g_ls_debug, g_ls_info, g_ls_lang_id);
}

/* ---------------------------------------------------------------------------
 * sub_485840  (00485840)
 *
 * 1. Zero-initialises g_ls_workspace_a (0xAA0 dwords) and g_ls_workspace_b
 *    (0x44 dwords).
 * 2. Calls sub_48C5D0 (TODO).
 * 3. Builds a virtual root string from g_ls_drive_type and g_ls_drive_num
 *    using the format "%s%d:%c%s" → e.g. "HDISK0:>" and stores it in
 *    g_virtual_root.  Strips a trailing '>' if present.
 *
 * Drive-type table (asm switch on dword_55C738 - 1, range 0..0x22):
 *   1  (case 0,1)  → "FLOPPY"
 *   10 (case 9)    → "HDISK"
 *   20 (case 19)   → "HDISK"
 *   30 (case 29)   → "REMOTE_SYS"
 *   31 (case 30)   → "REMOTE_DEV2"
 *   32 (case 31)   → "REMOTE_TMP"
 *   33 (case 32)   → "REMOTE_TOOLS"
 *   34 (case 33)   → "REMOTE_DEV1"
 *   35 (case 34)   → "REMOTE_USERS"
 *   default        → no sprintf, empty g_virtual_root
 *
 * Note: ls_main+21 in the original does:
 *   strcpy(game_path, "HDISK:>EARTH>")
 * so the virtual root for the game itself is always that literal.
 * We keep ls_build_virtual_root faithful to the asm; callers that need
 * the real game path use state->game_path (already set by ls_get_base_path).
 * --------------------------------------------------------------------------*/
// 00485840
void ls_build_virtual_root(void) {
    /* Zero the two workspace globals (original: rep stosd) */
    memset(g_ls_workspace_a, 0, sizeof(g_ls_workspace_a));
    memset(g_ls_workspace_b, 0, sizeof(g_ls_workspace_b));

    /* sub_48C5D0 @ 0x48C5D0 — Win32 drive enumeration (205 instructions).
     * Calls GetLogicalDriveStringsA / GetDriveTypeA / GetVolumeInformationA
     * to populate g_ls_workspace_a with per-drive records (type, number,
     * volume label). Compares current directory against drive roots to fill
     * g_ls_workspace_b with the matching entry. Dev-environment only: maps
     * network volume labels "DEV1/DEV2/TOOLS/TEMP/USERS" to REMOTE_* types.
     * No-op on Unix — g_ls_drive_type is pre-set to 10 (HDISK). */

    /* Derive drive-type name from g_ls_drive_type */
    const c8 *drive_name = NULL;
    switch (g_ls_drive_type) {
        case  1: /* fall through */
        case  2: drive_name = "FLOPPY";      break;
        case 10: drive_name = "HDISK";       break;
        case 20: drive_name = "HDISK";       break;
        case 30: drive_name = "REMOTE_SYS";  break;
        case 31: drive_name = "REMOTE_DEV2"; break;
        case 32: drive_name = "REMOTE_TMP";  break;
        case 33: drive_name = "REMOTE_TOOLS";break;
        case 34: drive_name = "REMOTE_DEV1"; break;
        case 35: drive_name = "REMOTE_USERS";break;
        default:
            /* asm default case: g_virtual_root stays empty */
            g_virtual_root[0] = '\0';
            return;
    }

    /* Format: "%s%d:%c%s" → <name><num>:><empty-suffix>
     * The fourth %s argument in the asm is a stack-local that was never
     * written (comes from the uninitialised var_100 area), so it is
     * effectively "".  The '>` separator is the %c argument (0x3E = '>').
     * Result examples:
     *   HDISK (type=10, num=0) → "HDISK0:>"
     *   FLOPPY (type=1,  num=1) → "FLOPPY1:>"
     */
    c8 tmp[256];
    snprintf(tmp, sizeof(tmp), "%s%d:%c%s",
             drive_name, g_ls_drive_num, '>', "");

    /* Strip trailing '>' if present (asm does this via strlen + zero) */
    i32 len = (i32)strlen(tmp);
    if (len > 0 && tmp[len - 1] == '>') {
        tmp[len - 1] = '\0';
        len--;
    }

    /* Copy into g_virtual_root (asm: rep movsd + movsb) */
    if (len >= (i32)sizeof(g_virtual_root)) {
        len = (i32)sizeof(g_virtual_root) - 1;
    }
    memcpy(g_virtual_root, tmp, (size_t)(len + 1));

    print("ls_build_virtual_root: \"%s\" (drive_type=%d num=%d)\n",
          g_virtual_root, g_ls_drive_type, g_ls_drive_num);
}
