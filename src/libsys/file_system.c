#include <sys/stat.h>

#include "file_system.h"
#include "common.h"

inline bool ls_exists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

inline i32 ls_get_filesize(const char* path) {
    struct stat buffer;
    stat(path, &buffer);
    return buffer.st_size;
}

inline FILE* ls_open(const char* path, const char* mode) {
    return fopen(path, mode);
}

inline void ls_close(FILE* file) {
    fclose(file);
}

inline void ls_seek(FILE* file, i32 offset, i32 origin) {
    fseek(file, offset, origin);
}

// read number of bytes
inline void ls_read_buffer(FILE* file, void* buffer, i32 size) {
    fread(buffer, size, 1, file);
}

inline u8 ls_read_i8(FILE* file) {
    return fgetc(file);
}

inline u16 ls_read_i16(FILE* file) {
    u16 value = 0;
    value |= fgetc(file);
    value |= fgetc(file) << 8;
    return value;
}

inline u32 ls_read_i32(FILE* file) {
    u32 value = 0;
    value |= fgetc(file);
    value |= fgetc(file) << 8;
    value |= fgetc(file) << 16;
    value |= fgetc(file) << 24;
    return value;
}

inline i8 ls_read_u8(FILE* file) {
    return (i8)ls_read_i8(file);
}

inline i16 ls_read_u16(FILE* file) {
    return (i16)ls_read_i16(file);
}

inline i32 ls_read_u32(FILE* file) {
    return (i32)ls_read_i32(file);
}
