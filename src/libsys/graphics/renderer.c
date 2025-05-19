#include <math.h>
#include <string.h>

#include "../common.h"
#include "renderer.h"


void renderer_init(renderer_t *renderer, system_t *system) {
    print("Initializing renderer...\n");
    i32 width = system->width;
    i32 height = system->height;

    renderer->width = system->width;
    renderer->height = system->height;
    renderer->bpp = system->bpp;
    renderer->stride = system->width * system->bpp;

    renderer->back_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
    renderer->front_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
    renderer->frame_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
}

inline void renderer_flip(renderer_t *renderer) {
    memory_copy(
        (u8*)renderer->back_buffer,
        (u8*)renderer->frame_buffer,
        renderer->width * renderer->height * renderer->bpp
    );
}

inline void renderer_clear(renderer_t *renderer) {
    // i32 width = renderer->width;
    // i32 height = renderer->height;
}

void renderer_release(renderer_t *renderer) {
    memory_free(renderer->frame_buffer);
    memory_free(renderer->front_buffer);
    memory_free(renderer->back_buffer);
}
