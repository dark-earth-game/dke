#include <math.h>
#include <string.h>

#include "../common.h"
#include "graphic.h"


void ls_init_graphic_system(graphic_t *graphic, system_t *system) {
    print("Initializing graphic...\n");
    i32 width = system->width;
    i32 height = system->height;

    graphic->width = system->width;
    graphic->height = system->height;
    graphic->bpp = system->bpp;
    graphic->stride = system->width * system->bpp;

    graphic->back_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
    graphic->front_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
    graphic->frame_buffer = (u32 *)memory_alloc(width * height * sizeof(u32));
}

inline void ls_graphic_flip(graphic_t *graphic) {
    memory_copy(
        (u8*)graphic->back_buffer,
        (u8*)graphic->frame_buffer,
        graphic->width * graphic->height * graphic->bpp
    );
}

inline void ls_graphic_clear(graphic_t *graphic) {
    // i32 width = graphic->width;
    // i32 height = graphic->height;
}

void ls_release_graphic_system(graphic_t *graphic) {
    memory_free(graphic->frame_buffer);
    memory_free(graphic->front_buffer);
    memory_free(graphic->back_buffer);
}
