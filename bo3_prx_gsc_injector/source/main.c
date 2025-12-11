#include "../include/main.h"
#include "../include/prx_mem.h"
#include "../include/plugin_common.h"


int32_t scePadReadState_hook(int32_t handle, OrbisPadData* pad_data)
{
    int ret = 0;
    ret = scePadReadStateExt(handle, pad_data);
    if (ret)
		return ret;

	if (!g_busy)
	{
		switch (pad_data->buttons)
		{
			case PAD_BUTTON_INJECT:
				inject();
				break;
			case PAD_BUTTON_NEXT_GSC:
				switch_gsc(1);
				break;
			case PAD_BUTTON_PREV_GSC:
				switch_gsc(0);
				break;
		}
	}

	return ret;
}

int32_t attr_public plugin_load(int32_t argc, const char* argv[])
{
#pragma region GoldHEN Debug
    final_printf("[GoldHEN] %s Plugin Started.\n", g_pluginName);
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] Plugin Author(s): %s\n", g_pluginAuth);
#pragma endregion
	snprintf(g_gsc_dir_str, sizeof(g_gsc_dir_str), "/data/%s", g_pluginName);
	if (!dir_or_file_exist(g_gsc_dir_str, 0))
		sceKernelMkdir(g_gsc_dir_str, 0);

	char module[256];
    snprintf(module, sizeof(module), "/%s/common/lib/%s", sceKernelGetFsSandboxRandomWord(), "libScePad.sprx");
    int libScePad = 0;
    sys_dynlib_load_prx(module, &libScePad);
	if (!libScePad)
	{
		snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nFailed, error code 0%d", g_pluginName, ++g_errcode);
		NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);
		return 0;
	}

	scePadReadStateExtPatcher = (Patcher*)malloc(sizeof(Patcher));
    Patcher_Construct(scePadReadStateExtPatcher);

    uint8_t xor_edx_edx[5] = { 0x31, 0xD2, 0x90, 0x90, 0x90 };
    Patcher_Install_Patch(scePadReadStateExtPatcher, (uint64_t)scePadReadStateExt, xor_edx_edx, sizeof(xor_edx_edx));
	
	HOOK32(scePadReadState);

	snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nL1 + R3 to inject\nL1 + Dpad-Left/Right to select script", g_pluginName);
    NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);

	snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nMade with ❤️ by 0xEB_0xFE\nDiscord->biiinks78", g_pluginName);
    NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);
	
	return 0;
}

int32_t attr_public plugin_unload(int32_t argc, const char* argv[])
{
#pragma region GoldHEN Debug
    final_printf("[GoldHEN] <%s\\Ver.0x%08x> %s\n", g_pluginName, g_pluginVersion, __func__);
    final_printf("[GoldHEN] %s Plugin Ended.\n", g_pluginName);
#pragma endregion
	UNHOOK(scePadReadState);
	Patcher_Destroy(scePadReadStateExtPatcher);
    free(scePadReadStateExtPatcher);
	if (g_script_parse_tree.buffer)
		free(g_script_parse_tree.buffer);

	return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args)
{
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args)
{
    return 0;
}

void query_files()
{
	g_files_count = 0;

	char fp_buffer[256];
	while (1)
	{
		snprintf(fp_buffer, sizeof(fp_buffer), "%s/gscc_%d", g_gsc_dir_str, g_files_count);
		if (!dir_or_file_exist(fp_buffer, 1))
			break;
		snprintf(g_files_strarr[g_files_count], sizeof(g_files_strarr[g_files_count]), "gscc_%d", g_files_count);
		g_files_count++;
	}
}

void inject()
{
	g_busy = 1;
	g_errcode = 0;

	prx_mem_read(&g_script_parse_tree, g_offsets.script_parse_tree, sizeof(struct ScriptParseTree));
	u64 buffer;
	prx_mem_read(&buffer, g_offsets.script_parse_tree + g_offsets.script_parse_tree_buffer, sizeof(buffer));
	s32 crc32;
	prx_mem_read(&crc32, buffer + g_offsets.script_parse_tree_buffer_crc32, sizeof(crc32));

	query_files();
	if (!g_files_count)
	{
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nNo file detected\nPlace it as \"gscc_{i}\"\n[i=0-255]", g_pluginName);
		NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}

	if (strlen(g_current_gsc_str) == 0)
		snprintf(g_current_gsc_str, sizeof(g_current_gsc_str), "%s", g_files_strarr[0]);
	
	char gsc_full_path_str[256];
	snprintf(gsc_full_path_str, sizeof(gsc_full_path_str), "%s/%s", g_gsc_dir_str, g_current_gsc_str);

	s32 file_size = get_file_size(gsc_full_path_str);
	if (file_size == -1)
	{
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nInjection failed, error code 0%d", g_pluginName, ++g_errcode);
		NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}

	FILE* file = fopen(gsc_full_path_str, "r");
	if (file == NULL)
	{
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nInjection failed, error code 0%d", g_pluginName, ++g_errcode);
		NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}

	g_script_parse_tree.length = file_size + 1;
	g_script_parse_tree.buffer = malloc(g_script_parse_tree.length);
	if (!g_script_parse_tree.buffer)
	{
		fclose(file);
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nInjection failed, error code 0%d", g_pluginName, ++g_errcode);
		NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}

	size_t bytes_read = fread((void*)g_script_parse_tree.buffer, 1, file_size, file);
	if (bytes_read != file_size)
	{
    	free((void*)g_script_parse_tree.buffer);
    	fclose(file);
    	snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nInjection failed, error code 0%d", g_pluginName, ++g_errcode);
    	NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);
    	return;
	}

	fclose(file);

	prx_mem_write(&crc32, (u64)g_script_parse_tree.buffer + g_offsets.script_parse_tree_buffer_crc32, sizeof(crc32));
	prx_mem_write(&g_script_parse_tree, g_offsets.script_parse_tree, sizeof(struct ScriptParseTree));

	snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nSuccessfully injected\n%s", g_pluginName, g_current_gsc_str);
    NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);

	sleep(1);
	g_busy = 0;
}

void switch_gsc(u8 next_or_prev)
{
	g_busy = 1;

	query_files();
	if (!g_files_count)
	{
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nNo file detected\nPlace it as \"gscc_{i}\"\n[i=0-255]", g_pluginName);
		NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}
	switch (next_or_prev)
    {
        case 1:
            if (g_current_gsc_index + 1 < g_files_count)
                g_current_gsc_index++;
            else
                g_current_gsc_index = 0;
        	strcpy(g_current_gsc_str, g_files_strarr[g_current_gsc_index]);
			snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\n->\n%s", g_pluginName, g_current_gsc_str);
			break;
        default:
            if (g_current_gsc_index > 0 && g_current_gsc_index < g_files_count)
                g_current_gsc_index--;
            else
                g_current_gsc_index = g_files_count - 1;
			strcpy(g_current_gsc_str, g_files_strarr[g_current_gsc_index]);
            snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\n<-\n%s", g_pluginName, g_current_gsc_str);
			break;
    }

	NotifyStatic(TEX_ICON_SYSTEM, g_notif_str);

	sleep(1);
	g_busy = 0;
}

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
