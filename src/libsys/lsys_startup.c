#include <stdio.h>
#include <string.h>
#include <stdlib.h>   /* getenv */
#include <locale.h>   /* setlocale */

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
 * lsi_open_stat_app stub
 *
 * Original calls lsi_open_stat_app when not in Master mode.
 * No equivalent in our port — stats app is a Win32-only companion process.
 * --------------------------------------------------------------------------*/
static void lsi_open_stat_app(void) {
    /* TODO: sub_lsi_open_stat_app — Win32 CreateProcess stats companion */
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

    /* TODO: sub_48C5D0 */

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
