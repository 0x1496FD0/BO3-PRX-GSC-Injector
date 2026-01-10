#include "../include/main.h"
#include <stdlib.h>

s32 scePadReadState_hook(s32 handle, OrbisPadData* pad_data)
{
    s32 ret = 0;
    ret = scePadReadStateExt(handle, pad_data);
    if (ret)
		return ret;
	if (pad_data->buttons == PAD_BUTTON_TOGGLE_INPUTS && !g_busy)
	{
		g_busy = 1;
		g_inputs = !g_inputs;
		if (!g_inputs)
			snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nInputs disabled", g_notif_pfx);
		else
			snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nInputs re-enabled", g_notif_pfx);
		notif(TEX_ICON_SYSTEM, g_notif_str);
		sleep(BUSY_INTERRUPT_S);
		g_busy = 0;
	}	
	if (!g_busy && g_inputs)
	{
		switch (pad_data->buttons)
		{
			case PAD_BUTTON_INJECT:
				g_busy = 1;
				inject();
				break;
			case PAD_BUTTON_NEXT_GSC:
				g_busy = 1;
				switch_gsc(1);
				break;
			case PAD_BUTTON_PREV_GSC:
				g_busy = 1;
				switch_gsc(0);
				break;
		}
	}
	return ret;
}

s32 attr_public plugin_load(s32 argc, const char* argv[])
{
	snprintf(g_gsc_dir_str, sizeof(g_gsc_dir_str), "/data/T7 GSC Injector");
	if (!dir_or_file_exist(g_gsc_dir_str, 0))
		sceKernelMkdir(g_gsc_dir_str, 0777);

	char module[256];
    snprintf(module, sizeof(module), "/%s/common/lib/%s", sceKernelGetFsSandboxRandomWord(), "libScePad.sprx");
    s32 libScePad = 0;
    sys_dynlib_load_prx(module, &libScePad);
	if (!libScePad)
	{
		snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nFailed, error code 0%d", g_notif_pfx, ++g_errcode);
		notif(TEX_ICON_SYSTEM, g_notif_str);
		return 0;
	}

	g_pshellcode = (Patcher*)malloc(sizeof(Patcher));
    Patcher_Construct(g_pshellcode);

    u8 xor_edx_edx[5] = { 0x31, 0xD2, 0x90, 0x90, 0x90 };
    Patcher_Install_Patch(g_pshellcode, (u64)scePadReadStateExt, xor_edx_edx, sizeof(xor_edx_edx));
	
	HOOK32(scePadReadState);

	snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nMade with ❤️ by 0xEB_0xFE\nv.%s", g_notif_pfx, VERSION);
    notif(TEX_ICON_SYSTEM, g_notif_str);
	sleep(1);
	snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nL1 + R3 to inject\n\nL1 + <-/-> to select a gsc", g_notif_pfx);
    notif(TEX_ICON_SYSTEM, g_notif_str);
	sleep(1);
	snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nL1 + OPTIONS to toggle injection/gsc switch inputs listenning", g_notif_pfx);
	notif(TEX_ICON_SYSTEM, g_notif_str);

	return 0;
}

s32 attr_public plugin_unload(s32 argc, const char* argv[])
{
	UNHOOK(scePadReadState);
	Patcher_Destroy(g_pshellcode);
    free(g_pshellcode);
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

v query_files(const char* dir_path)
{
	g_files_count = 0;
	char fp_buffer[256];
	while (1)
	{
		snprintf(fp_buffer, sizeof(fp_buffer), "%s/gscc_%d", dir_path, g_files_count);
		if (!dir_or_file_exist(fp_buffer, 1))
			break;
		snprintf(g_files_strarr[g_files_count], sizeof(g_files_strarr[g_files_count]), "%s/gscc_%d", dir_path, g_files_count);
		g_files_count++;
	}
}

v inject()
{
	g_errcode = 0;

	mem_read(&g_script_parse_tree, g_offsets.script_parse_tree, sizeof(struct ScriptParseTree));
	u64 buffer;
	mem_read(&buffer, g_offsets.script_parse_tree + g_offsets.script_parse_tree_buffer, sizeof(buffer));
	s32 crc32;
	mem_read(&crc32, buffer + g_offsets.script_parse_tree_buffer_crc32, sizeof(crc32));

	query_files(g_gsc_dir_str);
	if (!g_files_count)
	{
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nNo file detected\nPlace it as \"gscc_{i}\"\n[i=0-255]", g_notif_pfx);
		notif(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}

	if (strlen(g_current_gsc_str) == 0)
		snprintf(g_current_gsc_str, sizeof(g_current_gsc_str), "%s", g_files_strarr[0]);
	
	s32 file_size = get_file_size(g_current_gsc_str);
	if (file_size == -1)
	{
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nInjection failed, error code 0%d", g_notif_pfx, ++g_errcode);
		notif(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}

	FILE* file = fopen(g_current_gsc_str, "r");
	if (file == NULL)
	{
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nInjection failed, error code 0%d", g_notif_pfx, ++g_errcode);
		notif(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}

	s32 new_length = file_size;
		
	g_script_parse_tree.buffer = malloc(new_length + 1);
	g_script_parse_tree.length = new_length;

	if (!g_script_parse_tree.buffer)
	{
		fclose(file);
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nInjection failed, error code 0%d", g_notif_pfx, ++g_errcode);
		notif(TEX_ICON_SYSTEM, g_notif_str);
		return;
	}

	s32 bytes_read = fread((p)g_script_parse_tree.buffer, 1, file_size, file);
	if (bytes_read != file_size)
	{
    	free((p)g_script_parse_tree.buffer);
    	fclose(file);
    	snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nInjection failed, error code 0%d", g_notif_pfx, ++g_errcode);
    	notif(TEX_ICON_SYSTEM, g_notif_str);
    	return;
	}

	fclose(file);

	mem_write(&crc32, (u64)g_script_parse_tree.buffer + g_offsets.script_parse_tree_buffer_crc32, sizeof(crc32));
	mem_write(&g_script_parse_tree, g_offsets.script_parse_tree, sizeof(struct ScriptParseTree));

	snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\nSuccessfully injected\n%s", g_notif_pfx, g_current_gsc_str);
    notif(TEX_ICON_SYSTEM, g_notif_str);

	sleep(BUSY_INTERRUPT_S);
	g_busy = 0;
}

v switch_gsc(u8 next_or_prev)
{
	query_files(g_gsc_dir_str);
	if (!g_files_count)
	{
		snprintf(g_notif_str, sizeof(g_notif_str),  "%s\n\nNo file detected\nPlace it as \"gscc_{i}\"\n[i=0-255]", g_notif_pfx);
		notif(TEX_ICON_SYSTEM, g_notif_str);
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
			snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\n->\n%s", g_notif_pfx, g_current_gsc_str);
			break;
        default:
            if (g_current_gsc_index > 0 && g_current_gsc_index < g_files_count)
                g_current_gsc_index--;
            else
                g_current_gsc_index = g_files_count - 1;
			strcpy(g_current_gsc_str, g_files_strarr[g_current_gsc_index]);
            snprintf(g_notif_str, sizeof(g_notif_str), "%s\n\n<-\n%s", g_notif_pfx, g_current_gsc_str);
			break;
    }

	notif(TEX_ICON_SYSTEM, g_notif_str);

	sleep(BUSY_INTERRUPT_S);
	g_busy = 0;
}
