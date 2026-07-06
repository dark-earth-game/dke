#include "mouse.h"
#include "common.h"

u16 g_mouse_enabled = 0;

// 0x00487BB0
void lsys_init_mouse_system(void) {
    // Original: mov word_55C77C, 1 ; retn
    // Platform layer (macos.m / linux.c) handles cursor pos + button state via
    // system_t.motion_x/y and actions[]. Nothing else to init on non-Windows.
    // TODO: native cursor hide/clip when movie playback path is ported.
    g_mouse_enabled = 1;
    print("lsys_init_mouse_system: mouse enabled\n");
}

// mirrors lsys_shut_mouse_system
void lsys_shut_mouse_system(void) {
    // Original: ClipCursor(NULL); word_55C77C = 0;
    // TODO: release cursor clip on native platform.
    g_mouse_enabled = 0;
    print("lsys_shut_mouse_system: mouse disabled\n");
}
