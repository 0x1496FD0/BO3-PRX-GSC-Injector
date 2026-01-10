#include <stdio.h>
#include <sys/stat.h>

#include "../include/fs.h"

s32 get_file_size(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file)
		return -1;
    fseek(file, 0, SEEK_END);
    s32 size = ftell(file);
    fclose(file);
    return size;
}

s32 dir_or_file_exist(const char* path, s32 mode)
{
    struct stat st;
    if (stat(path, &st) == 0)
	{
		if (mode == 0)
        	return S_ISDIR(st.st_mode);
		else
			return S_ISREG(st.st_mode);
	}
	return 0;
}
