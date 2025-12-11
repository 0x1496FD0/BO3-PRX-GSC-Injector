#pragma once

#include <stdint.h>


struct ScriptParseTree
{
	char* name;
	int32_t length;
	int32_t pad;
	void* buffer;
};

struct _offsets
{
	const uint64_t script_parse_tree;
	const uint64_t script_parse_tree_buffer;
	const uint64_t script_parse_tree_buffer_crc32;
}
g_offsets =
{
	.script_parse_tree = 0x547EEF0,
	.script_parse_tree_buffer = 0x10,
	.script_parse_tree_buffer_crc32 = 0x8
};
