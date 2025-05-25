#pragma once

#include "../libsys/common.h"
#include "../libsys/system.h"

typedef struct {
    i32 width;
    i32 height;
    i32 bpp;
    i32 stride;
    u32 *back_buffer;
    u32 *front_buffer;
    u32 *frame_buffer;
} graphic_t;

void ls_init_graphic_system(graphic_t *graphic, system_t *system);
void ls_graphic_flip(graphic_t *graphic);
void ls_graphic_clear(graphic_t *graphic);
void ls_release_graphic_system(graphic_t *graphic);
