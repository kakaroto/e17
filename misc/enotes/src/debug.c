/**************************************************/
/**               E  -  N O T E S                **/
/**                                              **/
/**  The contents of this file are released to   **/
/**  the public under the General Public Licence **/
/**  Version 2.                                  **/
/**                                              **/
/**  By  Thomas Fletcher (www.fletch.vze.com)    **/
/**                                              **/
/**************************************************/


#include "debug.h"

extern MainConfig *main_config;

DebugFuncLst    func_list[] = {
	/* Main */
	{"main", 0},

	/* config */
	{"mainconfig_new", 1},
	{"mainconfig_free", 1},
	{"read_global_configuration", 1},
	{"read_local_configuration", 1},
	{"read_configuration", 1},
	{"check_local_configuration", 1},
	{"processopt", 1},

	/* controlcentre */
	{"setup_cc", 1},
	{"cc_resize", 2},
	{"cc_close", 2},
	{"cc_saveload", 2},
	{"cc_newnote", 2},
	{"cc_settings", 2},

	/* ipc */
	{"find_server", 1},
	{"setup_server", 1},
	{"send_to_server", 1},
	{"ipc_svr_data_recv", 2},
	{"parse_message", 1},
	{"ipc_close_enotes", 1},
	{"fix_newlines", 1},

	/* msgbox */
	{"msgbox", 1},
	{"msgbox_okbtn_clicked", 1},
	{"msgbox_resize", 1},
	{"msgbox_close", 1},

	/* note */
	{"new_note", 1},
	{"new_note_with_values", 1},
	{"append_note", 1},
	{"remove_note", 1},
	{"setup_note", 1},
	{"note_ecore_close", 2},
	{"note_ecore_resize", 2},
	{"note_edje_close", 2},
	{"note_edje_minimise", 2},
	{"get_date_string", 1},
	{"note_edje_close_timer", 2},
	{"get_note_by_title", 1},
	{"get_note_by_content", 1},
	{"get_title_by_note", 1},
	{"get_content_by_note", 1},
	{"get_cycle_begin", 1},
	{"get_cycle_next_note", 1},
	{"get_cycle_previous_note", 1},

	/* saveload  -  saveload */
	{"setup_saveload", 1},
	{"setup_saveload_win", 1},
	{"saveload_setup_button", 1},
	{"fill_saveload_tree", 1},
	{"setup_saveload_opt", 1},
	{"ecore_saveload_resize", 2},
	{"ecore_saveload_close", 2},
	{"ewl_saveload_revert", 2},
	{"ewl_saveload_close", 2},
	{"ewl_saveload_load", 2},
	{"ewl_saveload_save", 2},
	{"ewl_saveload_listitem_click", 2},

	/* saveload  -  load */
	{"setup_load", 1},
	{"setup_load_win", 1},
	{"load_setup_button", 1},
	{"fill_load_tree", 1},
	{"setup_load_opt", 1},
	{"ecore_load_resize", 2},
	{"ecore_load_close", 2},
	{"ewl_load_revert", 2},
	{"ewl_load_close", 2},
	{"ewl_load_load", 2},
	{"ewl_load_delete", 2},
	{"ewl_load_listitem_click", 2},

	/* settings */
	{"setup_settings", 1},
	{"setup_settings_win", 1},
	{"fill_tree", 1},
	{"settings_setup_button", 1},
	{"setup_settings_opt", 1},
	{"setup_settings_opt_int", 1},
	{"ecore_settings_resize", 2},
	{"ecore_settings_close", 2},
	{"ewl_settings_revert", 2},
	{"ewl_settings_close", 2},
	{"ewl_settings_save", 2},
	{"save_settings", 1},

	/* storage */
	{"free_note_stor", 1},
	{"append_note_stor", 1},
	{"remove_note_stor", 1},
	{"stor_cycle_begin", 1},
	{"stor_cycle_end", 1},
	{"stor_cycle_next", 1},
	{"stor_cycle_prev", 1},
	{"stor_cycle_get_notestor", 1},
	{"make_storage_fn", 1},
	{"get_notestor_from_value", 1},
	{"get_value_from_notestor", 1},

	/* usage */
	{"read_usage_configuration", 1},
	{"read_usage_for_configuration_fn", 1},
	{"print_usage", 1},

	/* xml */
	{"free_xmlentry", 1},
	{"xml_read", 1},
	{"xml_read_end", 1},
	{"xml_read_next_entry", 1},
	{"xml_read_prev_entry", 1},
	{"xml_read_entry_get_entry", 1},
	{"xml_write", 1},
	{"xml_write_end", 1},
	{"xml_write_append_entry", 1}
};


void
debug_msg_lvl(char *msg, int level)
{
	if (main_config == NULL)
		return;

	if (main_config->debug >= level)
		printf(DEBUG_MSG, msg);
	return;
}

void
debug_msg(char *msg)
{
	if (main_config == NULL)
		return;

	if (main_config->debug > 0)
		printf(DEBUG_MSG, msg);
	return;
}

void
debug_func_in(char *function)
{
	int             a = 0;

	if (main_config == NULL) {
		return;
	}
	while (&func_list[a] != NULL) {
		if (!strcmp(func_list[a].name, function)) {
			if (main_config->debug == func_list[a].level ||
			    main_config->debug > func_list[a].level)
				printf(DEBUG_FUNC_IN, func_list[a].level,
				       function);
			return;
		}
		a++;
	}

	printf("E-Notes: DEBUGGING ERROR.  Asked to debug missing function.\n");
}

void
debug_func_out(char *function)
{
	int             a = 0;

	if (main_config == NULL) {
		return;
	}

	while (&func_list[a] != NULL) {
		if (!strcmp(func_list[a].name, function)) {
			if (main_config->debug == func_list[a].level ||
			    main_config->debug > func_list[a].level)
				printf(DEBUG_FUNC_OUT, func_list[a].level,
				       function);
			return;
		}
		a++;
	}

	printf("E-Notes: DEBUGGING ERROR.  Asked to debug missing function.\n");
}
