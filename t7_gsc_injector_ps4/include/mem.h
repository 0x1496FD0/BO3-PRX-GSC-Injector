#pragma once

#include "../include/types.h"

v mem_read(p destination, u64 source, u32 nbytes);
v mem_write(p source, u64 destination, u32 nbytes);
