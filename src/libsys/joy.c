#include "joy.h"
#include "common.h"

// 0x004811A0
void lsys_init_joy_system(void) {
    /*
     * Original:
     *   Zeroes unk_558BB0[0..1FE] (joystick state table, ~10 slots × 0x198 bytes).
     *   Iterates unk_558BB0 … unk_55943C in 0x198-byte strides:
     *     joyGetPosEx(slot_index, &pji)
     *     if OK: joyConfigChanged(0); joyGetDevCapsA(slot_index, caps_ptr, 0x194)
     *   No return value.
     *
     * Port: winmm joystick API is Windows-only. NOP stub — joystick input is not
     * wired into the game loop yet (system_t.actions[] covers keyboard/mouse).
     *
     * TODO: enumerate controllers via IOKit (macOS) / evdev (Linux) and fill a
     *   platform-neutral joystick table when joystick support is targeted.
     */
    print("lsys_init_joy_system: stub — joystick enumeration skipped\n");
}
