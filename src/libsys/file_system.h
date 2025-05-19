#pragma once

#include "common.h"

bool ls_exists(const char* path);
i32 ls_get_filesize(const char* path);

FILE* ls_open(const char* path, const char* mode);
void ls_close(FILE* file);
void ls_seek(FILE* file, i32 offset, i32 origin);

void ls_read_buffer(FILE* file, void* buffer, i32 size);

u8 ls_read_i8(FILE* file);
u16 ls_read_i16(FILE* file);
u32 ls_read_i32(FILE* file);

i8 ls_read_u8(FILE* file);
i16 ls_read_u16(FILE* file);
i32 ls_read_u32(FILE* file);
