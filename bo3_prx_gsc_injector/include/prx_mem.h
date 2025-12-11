#pragma once

#include "../include/plugin_common.h"


void prx_mem_read(void* destination, u64 source, u32 nbytes);
void prx_mem_write(void* source, u64 destination, u32 nbytes);
