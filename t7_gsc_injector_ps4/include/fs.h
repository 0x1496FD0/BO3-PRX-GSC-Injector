#pragma once

#include "types.h"

#define MAX_FILES_ENTRIES 255

s32 get_file_size(const char* path);
s32 dir_or_file_exist(const char* path, s32 mode);
u8 get_files_from_dir(const char* dir_path, char* files_path_buffer[MAX_FILES_ENTRIES], bool recursive, u8* i);
