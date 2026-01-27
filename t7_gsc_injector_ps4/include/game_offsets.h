#pragma once

#include "../include/types.h"

struct ScriptParseTree
{
	char* name;
	s32 length;
	s32 pad;
	p buffer;
} g_script_parse_tree;

struct _offsets
{
	const u64 script_parse_tree;
	const u64 script_parse_tree_buffer;
	const u64 script_parse_tree_buffer_crc32;
}
g_offsets =
{
	.script_parse_tree = 0x547EEF0,
	.script_parse_tree_buffer = 0x10,
	.script_parse_tree_buffer_crc32 = 0x8
};
