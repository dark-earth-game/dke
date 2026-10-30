#pragma once

#include "common.h"

typedef struct {
    u32 tick;
    u32 last_tick;
    u32 last_tick_fps;
    u32 frame_count;
} ls_timer_t;

typedef struct {
    b8 quit;
    i32 width;
    i32 height;
    i16 bpp;
    c8 *title;
    ls_timer_t timer;
    i32 actions[256];
    i32 motion_x;
    i32 motion_y;
    i32 prev_motion_x;
    i32 prev_motion_y;
    /* Mouse buttons — 0 = up, 1 = held, 2 = pressed-this-frame edge. */
    i32 mouse_left;
    i32 mouse_right;
    i32 mouse_middle;
} system_t;

i32 ls_init_instance(system_t *system, i32 width, i32 height, i32 bpp);
void ls_update_events(system_t *system);
u32 ls_tick();
void ls_delay(u32 delay);
void ls_set_title(system_t *system, c8 *title); // 00480C30
void ls_blit(system_t *system, u32 *front_buffer);
void ls_flip(system_t *system);
void ls_release(system_t *system);
