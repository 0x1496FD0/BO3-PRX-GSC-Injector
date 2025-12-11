#include "../include/prx_mem.h"


#define PROC_RW_FLAG_NONE_R       	0x00
#define PROC_RW_FLAG_DISABLE  		0x01
#define PROC_RW_FLAG_ENABLE_W   	0x02
#define PROC_RW_FLAG_FORCE_W	    0x04
#define PROC_RW_FLAG_COMBINED   	0x08


void prx_mem_read(void* destination, u64 source, u32 nbytes)
{
	struct proc_rw _proc_rw;
	_proc_rw.write_flags = PROC_RW_FLAG_NONE_R;
	_proc_rw.data = destination;
	_proc_rw.address = source;
	_proc_rw.length = nbytes;
	sys_sdk_proc_rw(&_proc_rw);
}

void prx_mem_write(void* source, u64 destination, u32 nbytes)
{
	struct proc_rw _proc_rw;
	_proc_rw.write_flags = PROC_RW_FLAG_ENABLE_W | PROC_RW_FLAG_FORCE_W;
	_proc_rw.data = source;
	_proc_rw.address = destination;
	_proc_rw.length = nbytes;
	sys_sdk_proc_rw(&_proc_rw);
}
