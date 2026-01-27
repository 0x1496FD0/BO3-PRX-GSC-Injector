#pragma once

#include <Patcher.h>

#include "../include/plugin_common.h"
#include "../include/pad.h"
#include "../include/fs.h"
#include "../include/mem.h"
#include "../include/game_offsets.h"

#define VERSION "final"
#define BUSY_INTERRUPT_S 1 // 1 second is ok, tested 500 it crashed the game every time

char* g_notif_pfx = "⚡T7 GSC Injector";
int g_errcode = 0;
char g_notif_str[128] = {0};
u8 g_busy = 0;
u8 g_inputs = 1;
char g_gsc_dir_str[128];
char* g_files_strarr[MAX_FILES_ENTRIES];
u8 g_files_count = 0;
char g_current_gsc_str[256] = {0};
u8 g_current_gsc_index = 0;

Patcher* g_pshellcode;
s32 scePadReadStateExt(s32 handle, OrbisPadData* pad_data);
s32 scePadReadState(s32 handle, OrbisPadData* pad_data);
v inject();
v switch_gsc(u8 next_or_prev);
HOOK_INIT(scePadReadState);
