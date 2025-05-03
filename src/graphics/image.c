#include <math.h>

#include "../core/common.h"
#include "../core/file_system.h"

#include "image.h"


u8* image_read_rle(const u8 *filename) {
    FILE *file = file_open((char *)filename, "rb");

    if (!file) {
        print("Error: could not open file %s\n", filename);
        return NULL;
    }

    i32 version = file_read_i32(file);
    i32 unknown = file_read_i32(file);
    i32 width = file_read_i32(file);
    i32 height = file_read_i32(file);

    print("version: %d\n", version);
    print("unknown: %d\n", unknown);
    print("width: %d\n", width);
    print("height: %d\n", height);

    u8 palette[256 * 3];
    for (i32 i = 0; i < 256; i++) {
        palette[i * 3 + 0] = file_read_u8(file);
        palette[i * 3 + 1] = file_read_u8(file);
        palette[i * 3 + 2] = file_read_u8(file);
    }

    // Decompress LZW image adta
    // u8 *pixels = (u8*)memory_alloc(size * 4);


    file_close(file);
    // return pixels;
    return NULL;
}
