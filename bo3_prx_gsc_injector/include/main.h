#include "../include/plugin_common.h"
#include "../include/offsets.h"

//PS4Toolchain
#include <orbis/Pad.h>
#include <orbis/_types/pad.h>

// GoldHEN Plugins SDK
#include <Patcher.h>


attr_public const char *g_pluginName = "BO3 PRX GSC Injector";
attr_public const char *g_pluginDesc = ".gscc injector for Black ops 3 compatible with any fw supported by https://github.com/GoldHEN/GoldHEN_Plugins_SDK (Jul 12, 2023)";
attr_public const char *g_pluginAuth = "0xEB_0xFE";
attr_public uint32_t g_pluginVersion = 0x00000002;

const u32 PAD_BUTTON_MODIFIER = ORBIS_PAD_BUTTON_L1;
const u32 PAD_BUTTON_INJECT = PAD_BUTTON_MODIFIER | ORBIS_PAD_BUTTON_R3;
const u32 PAD_BUTTON_NEXT_GSC = PAD_BUTTON_MODIFIER | ORBIS_PAD_BUTTON_RIGHT;
const u32 PAD_BUTTON_PREV_GSC = PAD_BUTTON_MODIFIER | ORBIS_PAD_BUTTON_LEFT;

int g_errcode = 0;
char g_notif_str[128] = {0};
u8 g_busy = 0;
char g_gsc_dir_str[128];
char g_files_strarr[0xFF][256];
u8 g_files_count = 0;
char g_current_gsc_str[256] = {0};
u8 g_current_gsc_index = 0;
Patcher* scePadReadStateExtPatcher;
struct ScriptParseTree g_script_parse_tree;

int32_t scePadReadState(int32_t handle, OrbisPadData* pad_data);
int32_t scePadReadStateExt(int32_t handle, OrbisPadData* pad_data);
s32 get_file_size(const char* path);
s32 dir_or_file_exist(const char* path, s32 mode);
void inject();
void switch_gsc(u8 next_or_prev);
int32_t scePadReadState_hook(int32_t handle, OrbisPadData* pad_data);

HOOK_INIT(scePadReadState);
