#include "../include/mem.h"

#include <GoldHEN.h> // i hate to put sdk headers before mine but GoldHEN is missing typedefs or my lsp is broky af

#define PROC_RW_FLAG_NONE_R       	0x00
#define PROC_RW_FLAG_DISABLE  		0x01
#define PROC_RW_FLAG_ENABLE_W   	0x02
#define PROC_RW_FLAG_FORCE_W	    0x04
#define PROC_RW_FLAG_COMBINED   	0x08

v mem_read(p destination, u64 source, u32 nbytes)
{
	struct proc_rw _proc_rw;
	_proc_rw.write_flags = PROC_RW_FLAG_NONE_R;
	_proc_rw.data = destination;
	_proc_rw.address = source;
	_proc_rw.length = nbytes;
	sys_sdk_proc_rw(&_proc_rw);
}

v mem_write(p source, u64 destination, u32 nbytes)
{
	struct proc_rw _proc_rw;
	_proc_rw.write_flags = PROC_RW_FLAG_ENABLE_W | PROC_RW_FLAG_FORCE_W;
	_proc_rw.data = source;
	_proc_rw.address = destination;
	_proc_rw.length = nbytes;
	sys_sdk_proc_rw(&_proc_rw);
}
