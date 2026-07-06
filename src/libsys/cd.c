#include "cd.h"
#include "common.h"

u32 g_cd_present = 0;

// 0x00487770
i32 lsys_init_cd_system(void) {
    /*
     * Original:
     *   mciSendStringA("open cdaudio wait", ..., hwndCallback)
     *   if eax != 0: dword_55C784 = 0; return 0   ; MCI error → no CD
     *   mciSendStringA("set cdaudio time format milliseconds", ...)
     *   dword_55C784 = 1; return 1                 ; CD present
     *
     * Patched no-CD build: MCI call would fail at "open cdaudio wait" and the
     * original returns 0 (no CD). We match that behaviour directly.
     *
     * TODO: native MCI equivalent if CD audio ever becomes a target.
     */
    g_cd_present = 0;
    print("lsys_init_cd_system: no-CD build — CD subsystem disabled\n");
    return 0;
}
