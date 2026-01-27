#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#include <orbis/libkernel.h>

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

u8 get_files_from_dir(const char* dir_path, char* files_path_buffer[MAX_FILES_ENTRIES], bool recursive, u8* i)
{
	s32 fd;
	OrbisKernelStat st;
	fd = sceKernelOpen(dir_path, O_RDONLY | O_DIRECTORY, 0777);
	if (fd <= 0)
		return 0;
	if (sceKernelFstat(fd, &st) < 0)
	{
		sceKernelClose(fd);
		return 0;
	}

	s32 buffer_size = 0x40000;
	p buffer = malloc(buffer_size);

	s32 ret = sceKernelGetdents(fd, (char*)buffer, buffer_size);
	if (ret <= 0)
	{
    	free(buffer);
    	sceKernelClose(fd);
    	return 0;
	}

	struct dirent* entry = (struct dirent*)buffer;
    u8 entries_count = 0;
	if (i)
		entries_count = *i;
	while (entry < (struct dirent*)(buffer + ret) && entries_count < MAX_FILES_ENTRIES)
	{
        if (entry->d_type == DT_REG)
		{
			files_path_buffer[entries_count] = malloc(strlen(dir_path) + strlen(entry->d_name) + 2);
			strcpy(files_path_buffer[entries_count], dir_path);
			strcat(files_path_buffer[entries_count], "/");
			strcat(files_path_buffer[entries_count], entry->d_name);
			entries_count++;
		}
		if (recursive && entry->d_type == DT_DIR && entry->d_name[0] != '.')
		{
			u32 size = strlen(dir_path) + strlen(entry->d_name) + 2;
			char* next_path = malloc(size);
			strcpy(next_path, dir_path);
			strcat(next_path, "/");
			strcat(next_path, entry->d_name);
			entries_count += get_files_from_dir(next_path, files_path_buffer, recursive, &entries_count);
			free(next_path);
		}
		entry = (struct dirent*)((char*)entry + entry->d_reclen);
    }
	free(buffer);
	sceKernelClose(fd);

	if (i)
		return entries_count - *i;
	return entries_count;
}
