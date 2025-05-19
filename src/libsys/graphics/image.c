#include <math.h>

#include "../common.h"
#include "../file_system.h"

#include "image.h"


u8* image_read_rle(const u8 *filename) {
    FILE *file = ls_open((char *)filename, "rb");

    if (!file) {
        print("Error: could not open file %s\n", filename);
        return NULL;
    }

    i32 version = ls_read_i32(file);
    i32 unknown = ls_read_i32(file);
    i32 width = ls_read_i32(file);
    i32 height = ls_read_i32(file);

    print("version: %d\n", version);
    print("unknown: %d\n", unknown);
    print("width: %d\n", width);
    print("height: %d\n", height);

    u8 palette[256 * 3];
    for (i32 i = 0; i < 256; i++) {
        palette[i * 3 + 0] = ls_read_u8(file);
        palette[i * 3 + 1] = ls_read_u8(file);
        palette[i * 3 + 2] = ls_read_u8(file);
    }

    // Decompress LZW image adta
    // u8 *pixels = (u8*)memory_alloc(size * 4);


    ls_close(file);
    // return pixels;
    return NULL;
}
