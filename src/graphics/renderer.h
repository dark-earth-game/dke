#pragma once

#include "../core/common.h"
#include "../core/system.h"

typedef struct {
    i32 width;
    i32 height;
    i32 bpp;
    i32 stride;
    u32 *back_buffer;
    u32 *front_buffer;
    u32 *frame_buffer;
} renderer_t;

void renderer_init(renderer_t *renderer, system_t *system);
void renderer_flip(renderer_t *renderer);
void renderer_clear(renderer_t *renderer);
void renderer_release(renderer_t *renderer);
