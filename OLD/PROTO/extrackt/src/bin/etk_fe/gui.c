/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */

#include "Extrackt.h"
#include <Ecore.h>
#include <Etk.h>
#include <Enhance.h>
#include "gui.h"

#define GUI_FILE PACKAGE_DATA_DIR"/etk_fe/extrackt.glade"
#define ICON_FILE PACKAGE_DATA_DIR"/images/extrackt_icon.png"

#define TRACKS_NUM_COL 	etk_tree_nth_col_get(ETK_TREE(gui->tracks_tree), 0)
#define TRACKS_NAME_COL	etk_tree_nth_col_get(ETK_TREE(gui->tracks_tree), 1)
#define TRACKS_TIME_COL	etk_tree_nth_col_get(ETK_TREE(gui->tracks_tree), 2)
#define TRACKS_RIP_COL 	etk_tree_nth_col_get(ETK_TREE(gui->tracks_tree), 3)

#define E_CONTROL_BUTTON(name, width, height) \
   gui->controls.name[0] = etk_button_new(); \
   gui->controls.name[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/etk_fe/etk_fe.edj", #name); \
   etk_widget_pass_events_set(gui->controls.name[1], ETK_TRUE); \
   etk_widget_size_request_set(gui->controls.name[1], width, height); \
   etk_container_add(ETK_CONTAINER(gui->controls.name[0]), gui->controls.name[1]);

typedef struct _E_Gui_Etk E_Gui_Etk;

typedef enum _E_Gui_Menu_Item_Type
{
   E_GUI_MENU_ITEM_NORMAL,
   E_GUI_MENU_ITEM_SEPERATOR
} E_Gui_Menu_Item_Type;

struct _E_Gui_Etk
{
   Ecore_Fd_Handler *fd_handler;
   Ecore_Timer *poll_timer;
   Ecore_Timer *slow_timer;
   Ecore_Timer *fast_timer;
   Extrackt *ex;
   Enhance *en;
   
   Etk_Widget *tracks_tree;
   Etk_Widget *tracks_album;
   Etk_Widget *tracks_artist;   
   Etk_Tree_Col *tracks_num_col;
   Etk_Tree_Col *tracks_name_col;
   Etk_Tree_Col *tracks_time_col;
   Etk_Tree_Col *tracks_rip_col;   
   
   Etk_Widget *encoder_combo;
   Etk_Widget *encoder_executable_entry;
   Etk_Widget *encoder_command_line_entry;
   Etk_Widget *encoder_file_format_entry;
   Etk_Widget *encoder_wav_delete;
   
   Etk_Widget *ripper_combo;
   Etk_Widget *ripper_executable_entry;
   Etk_Widget *ripper_command_line_entry;
   Etk_Widget *ripper_file_format_entry;
   
   Etk_Widget *cddb_primary_server_entry;
   Etk_Widget *cddb_primary_cgi_entry;
   Etk_Widget *cddb_secondary_server_entry;
   Etk_Widget *cddb_secondary_cgi_entry;
   
   Etk_Widget *rip_cur_pbar;
   Etk_Widget *rip_tot_pbar;
   Etk_Widget *encode_cur_pbar;
   Etk_Widget *encode_tot_pbar;
   
   Etk_Widget *poll_interval_entry;
   Etk_Widget *cdrom_entry;
   Etk_Widget *interrupt_playback_cbox;
   Etk_Widget *rewind_stopped_cbox;
   Etk_Widget *startup_first_track_cbox;
   Etk_Widget *autoplay_insert_cbox;
   Etk_Widget *reshuffle_playback_cbox;
   Etk_Widget *workaround_eject_cbox;
   Etk_Widget *poll_drive_cbox;
   Etk_Widget *config_save_button;
   Etk_Widget *config_restore_button;   				   
   
   Etk_Widget *statusbar1;
   Etk_Widget *statusbar2;
   Etk_Widget *statusbar3;
   
   Etk_Widget *image_about;
};

/* global variables */
static E_Gui_Etk *gui;

/* subsystem functions */
/************************/

static int        _etk_fe_gui_pipefd_handler_cb(void *data, Ecore_Fd_Handler *fd_handler);
static int        _etk_fe_gui_poll_timer_handler_cb(void *data);
static int        _etk_fe_gui_slow_timer_handler_cb(void *data);
static int        _etk_fe_gui_fast_timer_handler_cb(void *data);
static void       _etk_fe_gui_show(E_Gui_Etk *gui);
static void       _etk_fe_config_load(E_Gui_Etk *gui);
static void       _etk_fe_config_save(E_Gui_Etk *gui);
static void	  _etk_fe_gui_encoder_progress_bar_update(void);
static Eina_List *_etk_fe_tree_selected_rows_get(Etk_Tree *tree);
Etk_Bool  _etk_fe_gui_window_deleted_cb(void *data);

/* will enter here when a read is available on the pipefd */
static int  
_etk_fe_gui_pipefd_handler_cb(void *data, Ecore_Fd_Handler *fd_handler)
{
   E_Gui_Etk *g;
   Extrackt *ex;
   char msg[4096]; // FIXME why setting the length of the msg / pipe static?
   int length;

   g = (E_Gui_Etk *)data;
   ex = g->ex;
   length = read(ex->pfd[0],msg,sizeof(msg)); // FIXME dont read on the frontend
   /* also must set a way to end command like the dot in cddb to accumulate data */
   ex_protocol_parse(ex, msg, length);
   
   return 1;
}

static int
_etk_fe_gui_poll_timer_handler_cb(void *data)
{
   E_Gui_Etk *gui;
   gui = (E_Gui_Etk *)data;
   
   /* check for a cdrom only if it isnt ripping/encoding */
   ex_cddev_update(gui->ex);
   return 1;
}

static int
_etk_fe_gui_fast_timer_handler_cb(void *data)
{
   E_Gui_Etk *gui;
   gui = (E_Gui_Etk *)data;
   
   _etk_fe_gui_encoder_progress_bar_update();
   return 1;
}

static int
_etk_fe_gui_slow_timer_handler_cb(void *data)
{
   E_Gui_Etk *gui;
   gui = (E_Gui_Etk *)data;
  
   ex_command_rip_update(gui->ex);
   
   if(gui->ex->cddb.status == EX_STATUS_DOING)
     {
	etk_statusbar_message_pop(ETK_STATUSBAR(gui->statusbar3), 0);
	etk_statusbar_message_push(ETK_STATUSBAR(gui->statusbar3), _("Cddb lookup..."), 0);
     }
   if(gui->ex->cddb.status == EX_STATUS_DONE)
     {
	etk_statusbar_message_pop(ETK_STATUSBAR(gui->statusbar3), 0);
	etk_statusbar_message_push(ETK_STATUSBAR(gui->statusbar3), _("Cddb lookup done."), 0);
     }

   if(gui->ex->rip.curr_track.number > 0)
     {
	char *str;
	char *num;
	int  size;
	  
	num = ex_util_num_to_str(gui->ex->rip.curr_track.number,
				 gui->ex->rip.curr_track.number);
	
	/* Track number [ done + 1 / total ] 
	 * 8 spaces + [] + / + \0 + : 
	 * FIXME 3 cyphers for done and total
	 */
	size = strlen(_("Rip")) + strlen(_("Track")) + strlen(num) +  19;
	str = calloc(size, sizeof(char));
	snprintf(str, size, "%s: %s %s [%d / %d ]", _("Rip"), _("Track"), num, gui->ex->rip.num_done + 1, gui->ex->rip.num_total);
	etk_statusbar_message_pop(ETK_STATUSBAR(gui->statusbar1), 0);
	etk_statusbar_message_push(ETK_STATUSBAR(gui->statusbar1), str, 0);
     }
   else
     {
	etk_statusbar_message_pop(ETK_STATUSBAR(gui->statusbar1), 0);
	etk_statusbar_message_push(ETK_STATUSBAR(gui->statusbar1), "Rip: Idle", 0);
     }
   
   if(gui->ex->encode.curr_track.number > 0)
     {
	char *str;
	char *num;
	int  size;
	  
	num = ex_util_num_to_str(gui->ex->encode.curr_track.number,
				 gui->ex->encode.curr_track.number);
	
	/* Track number [ done + 1 / total ] 
	 * 8 spaces + [] + / + \0 + : 
	 * FIXME 3 cyphers for done and total
	 */
	size = strlen(_("Encode")) + strlen(_("Track")) + strlen(num) + 19;
	str = calloc(size, sizeof(char));
	snprintf(str, size, "%s: %s %s [%d / %d ]", _("Encode"), _("Track"), num, gui->ex->encode.num_done + 1, gui->ex->encode.num_total);
	etk_statusbar_message_pop(ETK_STATUSBAR(gui->statusbar2), 0);	
	etk_statusbar_message_push(ETK_STATUSBAR(gui->statusbar2), str, 0);
     }
   else
     {	 
	etk_statusbar_message_pop(ETK_STATUSBAR(gui->statusbar2), 0);
	etk_statusbar_message_push(ETK_STATUSBAR(gui->statusbar2), "Encode: Idle", 0);
     }
   
   return 1;
}

Etk_Widget *
_etk_fe_gui_menu_item_new(E_Gui_Menu_Item_Type item_type, const char *label,
			  Etk_Menu_Shell *menu_shell,
			  Etk_Callback callback,
			  void *data)
{
   Etk_Widget *menu_item = NULL;
   
   switch (item_type)
     {
      case E_GUI_MENU_ITEM_NORMAL:
	menu_item = etk_menu_item_image_new_with_label(label);
	break;
      case E_GUI_MENU_ITEM_SEPERATOR:
	menu_item = etk_menu_item_separator_new();
	break;
      default:
	return NULL;
     }
   
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   if(callback)
     etk_signal_connect("activated", ETK_OBJECT(menu_item), callback, data);
   
   return menu_item;
}

void
_etk_fe_gui_encoder_combo_changed_cb(Etk_Object *object, void *data)
{
   Eina_List *l;
   Ex_Config_Encode *ece;   
   Ex_Config_Exe *ecx;
   E_Gui_Etk *gui;   
   Etk_Combobox *combobox;
   Etk_Combobox_Item *active_item;
   char *label;
   
   if (!(combobox = ETK_COMBOBOX(object)))
     return;
   if (!(active_item = etk_combobox_active_item_get(combobox)))
     return;
   if (!(label = etk_combobox_item_data_get(active_item)))
     return;      
   
   gui = data;
   if(!ex_command_encode_set(gui->ex, label))
     return;
	
   ece = gui->ex->config.encode;         
   for(l = ece->encoders; l; l=l->next)
     {	
	ecx = (Ex_Config_Exe *)l->data;
	ecx->def = 0;
     }   
      
   ecx = gui->ex->encode.encoder;
   ecx->def = 1;
   etk_entry_text_set(ETK_ENTRY(gui->encoder_executable_entry),ecx->exe);
   etk_entry_text_set(ETK_ENTRY(gui->encoder_command_line_entry),ecx->command_line_opts);
   etk_entry_text_set(ETK_ENTRY(gui->encoder_file_format_entry),ecx->file_format);   	
}

void
_etk_fe_gui_ripper_combo_changed_cb(Etk_Object *object, void *data)
{
   Eina_List *l;
   Ex_Config_Encode *ece;   
   Ex_Config_Exe *ecx;   
   E_Gui_Etk *gui;   
   Etk_Combobox *combobox;
   Etk_Combobox_Item *active_item;
   char *label;
   
   if (!(combobox = ETK_COMBOBOX(object)))
     return;
   if (!(active_item = etk_combobox_active_item_get(combobox)))
     return;
   if (!(label = etk_combobox_item_data_get(active_item)))
     return;      
   
   gui = data;
   if(!ex_command_rip_set(gui->ex, label))
     return;
   
   ece = gui->ex->config.encode;         
   for(l = ece->encoders; l; l=l->next)
     {	
	ecx = (Ex_Config_Exe *)l->data;
	ecx->def = 0;
     }
   
   ecx = gui->ex->rip.ripper;
   ecx->def = 1;
   etk_entry_text_set(ETK_ENTRY(gui->ripper_executable_entry),ecx->exe);
   etk_entry_text_set(ETK_ENTRY(gui->ripper_command_line_entry),ecx->command_line_opts);
   etk_entry_text_set(ETK_ENTRY(gui->ripper_file_format_entry),ecx->file_format);
}

static void
_etk_fe_gui_encoder_progress_bar_update(void)
{	
   double percent;

   switch(gui->ex->encode.status)
     {
      case EX_STATUS_DOING:
	 etk_progress_bar_pulse(ETK_PROGRESS_BAR(gui->encode_cur_pbar));
	 percent = (float)gui->ex->encode.num_done/(float)gui->ex->encode.num_total;
	 etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(gui->encode_tot_pbar), percent);
	 break;
      case EX_STATUS_DONE:
	 if(gui->ex->rip.num_total != 0)
	   percent = (float)gui->ex->encode.num_done/(float)gui->ex->encode.num_total;
	 /* on the last track to encode, the total and done are set to 0 */
	 else
	   percent = 1.0;
	 etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(gui->encode_tot_pbar), percent);
	 etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(gui->encode_cur_pbar), 1.0);
	 break;
      case EX_STATUS_NOT_DONE:
	 etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(gui->encode_tot_pbar), 0.0);
	 etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(gui->encode_cur_pbar), 0.0);
	 break;
      default:
	 break;
     }
}

/* FIXME this function is exactly the same as the rip callback
 * just sets the encoder on, we should reuse the other */
void
_etk_fe_gui_rip_cb(Etk_Object *obj, void *data)
{
   E_Gui_Etk    *gui;
   Etk_Tree     *tree;
   Eina_List    *selected;
   Extrackt *ex;
   Etk_Tree_Row *row;   

   gui = data;
   tree = ETK_TREE(gui->tracks_tree);
   ex = gui->ex;
   
   selected = NULL;
   selected = _etk_fe_tree_selected_rows_get(ETK_TREE(gui->tracks_tree));
      
   row = etk_tree_first_row_get(tree);
   while(row)
     {	
	char          *ncol_string;
	Etk_Bool       selected;
	  
	etk_tree_row_fields_get(row, 
				TRACKS_NUM_COL,
				&ncol_string,				
				TRACKS_RIP_COL,
				&selected,
				NULL);
	if(selected)
	  {	     
	     /* append the track to the list of tracks to rip */
	     ex_command_rip_append(ex, strtol(ncol_string,NULL,10));
	  }
	
	row = etk_tree_row_next_get(row);
     }
   /* if none is selected rip them all */
   if(!ex->rip.tracks)
     {
	row = etk_tree_first_row_get(tree);
	while(row)
	  {	
	     char          *ncol_string;

	     Etk_Bool       selected;
	     etk_tree_row_fields_get(row, 
				TRACKS_NUM_COL,
				&ncol_string,				
				TRACKS_RIP_COL,
				&selected,
				NULL);
	     ex_command_rip_append(ex, strtol(ncol_string,NULL,10));
	     row = etk_tree_row_next_get(row);
	  }
     }
   /* actually rip them */
   ex_command_rip(ex);
}

void
_etk_fe_gui_abort_rip_enc_cb(Etk_Object *obj, void *data)
{
   ex_command_rip_abort(gui->ex);
   ex_command_encode_abort(gui->ex);
}

void
_etk_fe_gui_abort_rip_cb(Etk_Object *obj, void *data)
{
   ex_command_rip_abort(gui->ex);
}

void
_etk_fe_gui_cddb_lookup_cb(Etk_Object *obj, void *data)
{
   E_Gui_Etk *gui;
   
   gui = data;
      
   ex_cddb_default_set(&(gui->ex->disc_data), gui->ex->disc_info.num_tracks);
   ex_cddb_match_find(gui->ex);   
}

void
_etk_fe_gui_rip_enc_cb(Etk_Object *obj, void *data)
{
   E_Gui_Etk    *gui;
   Etk_Tree     *tree;
   Eina_List    *selected;
   Extrackt *ex;
   Etk_Tree_Row *row;   

   gui = data;
   tree = ETK_TREE(gui->tracks_tree);
   ex = gui->ex;

   /* the only difference is to set this flag to on */
   ex->encode.on = 1;
   selected = NULL;
   selected = _etk_fe_tree_selected_rows_get(ETK_TREE(gui->tracks_tree));
      
   row = etk_tree_first_row_get(tree);
   
   while(row)
     {	
	char          *ncol_string;
	Etk_Bool       selected;
	  
	etk_tree_row_fields_get(row, 
				TRACKS_NUM_COL,
				&ncol_string,				
				TRACKS_RIP_COL,
				&selected,
				NULL);
	if(selected)
	  {	     
	     /* append the track to the list of tracks to rip */
	     ex_command_rip_append(ex, strtol(ncol_string,NULL,10));
	  }
	
	row = etk_tree_row_next_get(row);
     }
   /* if none is selected rip them all */
   if(!ex->rip.tracks)
     {
	row = etk_tree_first_row_get(tree);
	while(row)
	  {	
	     char          *ncol_string;

	     Etk_Bool       selected;
	     etk_tree_row_fields_get(row, 
				TRACKS_NUM_COL,
				&ncol_string,				
				TRACKS_RIP_COL,
				&selected,
				NULL);
	     ex_command_rip_append(ex, strtol(ncol_string,NULL,10));
	     row = etk_tree_row_next_get(row);
	  }
     }
   /* actually rip them */
   ex_command_rip(ex);
}

void
_etk_fe_gui_config_save_cb(Etk_Object *obj, void *data)
{
   _etk_fe_config_save((E_Gui_Etk *)data);
}

void
_etk_fe_gui_config_restore_cb(Etk_Object *obj, void *data)
{
   _etk_fe_config_load((E_Gui_Etk *)data);
}

static void
_etk_fe_gui_show(E_Gui_Etk *gui)
{   
   Enhance *en;
   
   en = enhance_new();
   gui->en = en;
   
   enhance_callback_data_set(en, "_etk_fe_gui_window_deleted_cb", gui);
   enhance_callback_data_set(en, "_etk_fe_gui_rip_enc_cb", gui);
   enhance_callback_data_set(en, "_etk_fe_gui_rip_cb", gui);
   enhance_callback_data_set(en, "_etk_fe_gui_abort_rip_enc_cb", gui);
   enhance_callback_data_set(en, "_etk_fe_gui_abort_rip_cb", gui);
   enhance_callback_data_set(en, "_etk_fe_gui_cddb_lookup_cb", gui);   
   enhance_callback_data_set(en, "_etk_fe_gui_config_save_cb", gui);
   enhance_callback_data_set(en, "_etk_fe_gui_config_restore_cb", gui);
   
   /* FIXME i cant set the same callback on two buttons */
   enhance_callback_data_set(en, "_etk_fe_gui_encoder_combo_changed_cb", gui);
   enhance_callback_data_set(en, "_etk_fe_gui_ripper_combo_changed_cb", gui);
   
   enhance_file_load(en, "window1", GUI_FILE);

   /* bind variables */
   gui->tracks_tree = enhance_var_get(en, "tracks_tree");
   gui->tracks_album = enhance_var_get(en, "tracks_album");
   gui->tracks_artist = enhance_var_get(en, "tracks_artist");
   
   gui->encoder_combo = enhance_var_get(en, "encoder_combo");
   gui->encoder_executable_entry = enhance_var_get(en, "encoder_executable_entry");
   gui->encoder_command_line_entry = enhance_var_get(en, "encoder_command_line_entry");
   gui->encoder_file_format_entry = enhance_var_get(en, "encoder_file_format_entry");
   gui->encoder_wav_delete = enhance_var_get(en, "encoder_wav_delete");
	
   gui->ripper_combo = enhance_var_get(en, "ripper_combo");   
   gui->ripper_executable_entry = enhance_var_get(en, "ripper_executable_entry");
   gui->ripper_command_line_entry = enhance_var_get(en, "ripper_command_line_entry");
   gui->ripper_file_format_entry = enhance_var_get(en, "ripper_file_format_entry");
     
   gui->rip_cur_pbar = enhance_var_get(en, "rip_cur_pbar");
   gui->rip_tot_pbar = enhance_var_get(en, "rip_tot_pbar");
   gui->encode_cur_pbar = enhance_var_get(en, "encode_cur_pbar");   
   gui->encode_tot_pbar = enhance_var_get(en, "encode_tot_pbar");
   etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(gui->encode_cur_pbar), 0.015);   
   
   gui->cddb_primary_server_entry = enhance_var_get(en, "cddb_primary_server_entry");
   gui->cddb_primary_cgi_entry = enhance_var_get(en, "cddb_primary_cgi_entry");
   gui->cddb_secondary_server_entry = enhance_var_get(en, "cddb_secondary_server_entry");
   gui->cddb_secondary_cgi_entry = enhance_var_get(en, "cddb_secondary_cgi_entry");
   
   gui->poll_interval_entry = enhance_var_get(en, "poll_interval_entry");   
   gui->cdrom_entry = enhance_var_get(en, "cdrom_entry");
   gui->interrupt_playback_cbox = enhance_var_get(en, "interrupt_playback_cbox");
   gui->rewind_stopped_cbox = enhance_var_get(en, "rewind_stopped_cbox");
   gui->startup_first_track_cbox = enhance_var_get(en, "startup_first_track_cbox");
   gui->autoplay_insert_cbox = enhance_var_get(en, "autoplay_insert_cbox");
   gui->reshuffle_playback_cbox = enhance_var_get(en, "reshuffle_playback_cbox");
   gui->workaround_eject_cbox = enhance_var_get(en, "workaround_eject_cbox");
   gui->poll_drive_cbox = enhance_var_get(en, "poll_drive_cbox");
   gui->config_save_button = enhance_var_get(en, "config_save_button");
   gui->config_restore_button = enhance_var_get(en, "config_restore_button");
   
   gui->statusbar1 = enhance_var_get(en, "statusbar1");
   gui->statusbar2 = enhance_var_get(en, "statusbar2");
   gui->statusbar3 = enhance_var_get(en, "statusbar3");
   
   gui->image_about = enhance_var_get(en, "image_about");
   etk_image_set_from_file(ETK_IMAGE(gui->image_about), ICON_FILE, NULL),

   etk_statusbar_message_push(ETK_STATUSBAR(gui->statusbar1), "Rip: Idle", 0);
   etk_statusbar_message_push(ETK_STATUSBAR(gui->statusbar2), "Encode: Idle", 0);

   etk_combobox_column_add(ETK_COMBOBOX(gui->ripper_combo), ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.5);
   etk_combobox_build(ETK_COMBOBOX(gui->ripper_combo));   
   etk_combobox_column_add(ETK_COMBOBOX(gui->encoder_combo), ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_NONE, 0.5);
   etk_combobox_build(ETK_COMBOBOX(gui->encoder_combo));   
   
   /* create tree for holding track names, times, and rip status */
   etk_widget_size_request_set(gui->tracks_tree, 320, 350);   

   etk_tree_multiple_select_set(ETK_TREE(gui->tracks_tree), ETK_FALSE);
   gui->tracks_num_col = etk_tree_col_new(ETK_TREE(gui->tracks_tree),
			 _("No."),
			 20, 0.0);
   etk_tree_col_model_add(gui->tracks_num_col, etk_tree_model_text_new());
   gui->tracks_name_col = etk_tree_col_new(ETK_TREE(gui->tracks_tree), 
			  _("Tracks"),
			  200, 0.0);
   etk_tree_col_model_add(gui->tracks_name_col, etk_tree_model_text_new());
   gui->tracks_time_col = etk_tree_col_new(ETK_TREE(gui->tracks_tree), 
		          _("Length"),
			  60, 0.0);
   etk_tree_col_model_add(gui->tracks_time_col, etk_tree_model_text_new());
   gui->tracks_rip_col = etk_tree_col_new(ETK_TREE(gui->tracks_tree), 
			 _("Rip"),
			 20, 0.0);   
   etk_tree_col_model_add(gui->tracks_rip_col, etk_tree_model_checkbox_new());
   etk_tree_headers_visible_set(ETK_TREE(gui->tracks_tree), 1);
   etk_tree_build(ETK_TREE(gui->tracks_tree));   
      
   return;
}

Etk_Bool
_etk_fe_gui_window_deleted_cb(void *data)
{
   etk_fe_shutdown(gui);
   return ETK_TRUE;
}

static void
_etk_fe_config_load(E_Gui_Etk *gui)
{         
   char *tmp;
   
   if(!ex_config_read(gui->ex))
     return;

   /*** Fill in config for Config - CD tab ***/
   etk_entry_text_set(ETK_ENTRY(gui->cdrom_entry),
		      gui->ex->config.cd->cdrom);
   
#define CD_IF_TOGGLE(cfg, button) \
   if(gui->ex->config.cd->cfg) \
       { \
	  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(gui->button), \
					  ETK_TRUE); \
       }
#define ENC_IF_TOGGLE(cfg, button) \
   if(gui->ex->config.encode->cfg) \
       { \
	  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(gui->button), \
					  ETK_TRUE); \
       }
   
   CD_IF_TOGGLE(interrupt_playback, interrupt_playback_cbox);
   CD_IF_TOGGLE(rewind_stopped, rewind_stopped_cbox);
   CD_IF_TOGGLE(startup_first_track, startup_first_track_cbox);
   CD_IF_TOGGLE(autoplay_insert, autoplay_insert_cbox);
   CD_IF_TOGGLE(reshuffle_playback, reshuffle_playback_cbox);
   CD_IF_TOGGLE(workaround_eject, workaround_eject_cbox);
   CD_IF_TOGGLE(poll_drive, poll_drive_cbox);
   
   tmp = ex_util_num_to_str(gui->ex->config.cd->poll_interval,
			    gui->ex->config.cd->poll_interval);
   etk_entry_text_set(ETK_ENTRY(gui->poll_interval_entry), tmp);
   E_FREE(tmp);
        
   /* encoder tab */
   /***************/
     {
	Eina_List *l;
	Ex_Config_Encode *ece;
	Ex_Config_Exe *ecx;

	ece = gui->ex->config.encode;

	ENC_IF_TOGGLE(wav_delete, encoder_wav_delete);
	
	for(l = ece->encoders; l; l=l->next)
	  {
	     Etk_Combobox_Item *item;
	     
	     ecx = (Ex_Config_Exe *)l->data;
	     item = etk_combobox_item_append(ETK_COMBOBOX(gui->encoder_combo), _(ecx->name));
	     etk_combobox_item_data_set_full(item, strdup(_(ecx->name)), free);
	     if(ecx->def)
	       {
		  etk_combobox_active_item_set(ETK_COMBOBOX(gui->encoder_combo), item);
		  etk_entry_text_set(ETK_ENTRY(gui->encoder_executable_entry), ecx->exe);
		  etk_entry_text_set(ETK_ENTRY(gui->encoder_command_line_entry), ecx->command_line_opts);
		  etk_entry_text_set(ETK_ENTRY(gui->encoder_file_format_entry), ecx->file_format);
	       }
	  }
     }
   
   /* ripper tab */
   /***************/
     {
	Eina_List *l;
	Ex_Config_Exe *ecx;
	
	for(l = gui->ex->config.rippers; l; l=l->next)
	  {
	     Etk_Combobox_Item *item;
	     
	     ecx = (Ex_Config_Exe *)l->data;	     
	     item = etk_combobox_item_append(ETK_COMBOBOX(gui->ripper_combo), _(ecx->name));
	     etk_combobox_item_data_set_full(item, strdup(_(ecx->name)), free);
	     if(ecx->def)
	       {
		  etk_combobox_active_item_set(ETK_COMBOBOX(gui->ripper_combo), item);
		  etk_entry_text_set(ETK_ENTRY(gui->ripper_executable_entry),ecx->exe);
		  etk_entry_text_set(ETK_ENTRY(gui->ripper_command_line_entry),ecx->command_line_opts);
		  etk_entry_text_set(ETK_ENTRY(gui->ripper_file_format_entry),ecx->file_format);
	       }
	  }
     }

   /* discdb tab */
   /**************/
     {
	Ex_Config_Cddb_Server *ecs;
	ecs = gui->ex->config.cddb->primary;

	etk_entry_text_set(ETK_ENTRY(gui->cddb_primary_server_entry),ecs->name);
	etk_entry_text_set(ETK_ENTRY(gui->cddb_primary_cgi_entry),ecs->cgi_prog);
     } 
}

static void
_etk_fe_config_save(E_Gui_Etk *gui)
{   
   const char *tmp;
   
   if(!gui->ex->config.cd)
     return;
   
   /*** Some macros to help in saving strings and toggles ***/
#define SAVE_STR(cfg, str) \
   if(str) \
     { \
	E_FREE(cfg); \
	cfg = E_STRDUP(str); \
     } \
   
#define SAVE_TOGGLE(cfg, btn) \
   if(etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(btn))) \
       { \
	  cfg = 1; \
       } \
   else \
     { \
	cfg = 0; \
     }
   
   /*** Save config for Config - CD tab ***/   
   
#define CD_SAVE_STR(cfg, str) SAVE_STR(gui->ex->config.cd->cfg, str)
#define CD_SAVE_TOGGLE(cfg, bt) \
     SAVE_TOGGLE(gui->ex->config.cd->cfg, gui->bt)
   
   CD_SAVE_STR(cdrom, etk_entry_text_get(ETK_ENTRY(gui->cdrom_entry)));
   CD_SAVE_TOGGLE(interrupt_playback, interrupt_playback_cbox);
   CD_SAVE_TOGGLE(rewind_stopped, rewind_stopped_cbox);
   CD_SAVE_TOGGLE(startup_first_track, startup_first_track_cbox);
   CD_SAVE_TOGGLE(autoplay_insert, autoplay_insert_cbox);
   CD_SAVE_TOGGLE(reshuffle_playback, reshuffle_playback_cbox);
   CD_SAVE_TOGGLE(workaround_eject, workaround_eject_cbox);
   CD_SAVE_TOGGLE(poll_drive, poll_drive_cbox);

   tmp = etk_entry_text_get(ETK_ENTRY(gui->poll_interval_entry));
   if(tmp)
     gui->ex->config.cd->poll_interval = atoi(tmp);
   else
     gui->ex->config.cd->poll_interval = 1;     

   /*** Save config for Config->Rip - Ripper ***/
#define EXE_SAVE_STR(cfg, str) SAVE_STR(cfg, etk_entry_text_get(ETK_ENTRY(gui->str)))
     {
	Eina_List *l;
	Ex_Config_Exe *exe;	  
	
	for(l = gui->ex->config.rippers; l; l = l->next)
	  {
	     Etk_Combobox_Item *active_item;
	     
	     exe = l->data;
	     
	     if((active_item = etk_combobox_active_item_get(ETK_COMBOBOX(gui->ripper_combo))))
	       {
		  char *selection = NULL;
		  
		  selection = etk_combobox_item_data_get(active_item);
		  if(selection)
		    {
		       if(!strcmp(exe->name, selection))
			 break;
		    }
	       }
	     
	     exe = NULL;
	  }
	
	if(exe)
	  {
	     EXE_SAVE_STR(exe->exe, ripper_executable_entry);
	     EXE_SAVE_STR(exe->command_line_opts, ripper_command_line_entry);
	     EXE_SAVE_STR(exe->file_format, ripper_file_format_entry);
	  }	
     }
   
   /*** Save config for Config->Encoder - Encoder ***/
     {
	Eina_List *l;
	Ex_Config_Exe *exe;	  
	
	for(l = gui->ex->config.encode->encoders; l; l = l->next)
	  {	
	     Etk_Combobox_Item *active_item;
	     
	     exe = l->data;
	     
	     if((active_item = etk_combobox_active_item_get(ETK_COMBOBOX(gui->encoder_combo))))
	       {
		  char *selection = NULL;
		  
		  selection = etk_combobox_item_data_get(active_item);
		  if(selection)
		    {
		       if(!strcmp(exe->name, selection))
			 break;
		    }
	       }	     

	     exe = NULL;
	  }
	
	if(exe)
	  {
	     EXE_SAVE_STR(exe->exe, encoder_executable_entry);
	     EXE_SAVE_STR(exe->command_line_opts, encoder_command_line_entry);
	     EXE_SAVE_STR(exe->file_format, encoder_file_format_entry);
	  }	
     }   
   
   ex_config_write(gui->ex);
}

/* the frontend functions */
/**************************/

void
etk_fe_init(Extrackt *ex)
{
   //E_Gui_Etk *gui;
   
   enhance_init();
   etk_init(NULL,NULL);
   /* create main window */
   gui = calloc(1, sizeof(E_Gui_Etk));
   
   gui->ex = ex;
   ex->fe->data = (void*)gui;   
      
   _etk_fe_gui_show(gui);
   _etk_fe_config_load(gui);   
   
   /* attach the pipe to the main loop */
   gui->fd_handler = ecore_main_fd_handler_add(ex->pfd[0],
	 ECORE_FD_READ,_etk_fe_gui_pipefd_handler_cb,gui,NULL,NULL);
      
   /* the timer to poll for the cd */
   if(ex->config.cd->poll_drive)
     gui->poll_timer = ecore_timer_add(ex->config.cd->poll_interval, _etk_fe_gui_poll_timer_handler_cb, gui);

   gui->slow_timer = ecore_timer_add(1, _etk_fe_gui_slow_timer_handler_cb, gui);
   gui->fast_timer = ecore_timer_add(0.025, _etk_fe_gui_fast_timer_handler_cb, gui);
   etk_fe_disc_update(gui);
}

void
etk_fe_main(void *data)
{
   E_Gui_Etk *gui;
   
   gui = data;   
   etk_main();	
}

void
etk_fe_shutdown(void *data)
{
   E_Gui_Etk *gui;
   
   gui = data;
   _etk_fe_config_save(gui);
   ecore_timer_del(gui->slow_timer);
   ecore_timer_del(gui->fast_timer);   
   etk_main_quit();   
   enhance_shutdown();
}

void
etk_fe_disc_update(void *data)
{
   E_Gui_Etk    *gui;
   Ex_Disc_Data *ddata;
   Etk_Tree_Row *row;
   int i;
   
   gui = data;
   ddata = &(gui->ex->disc_data);
   
   etk_label_set(ETK_LABEL(gui->tracks_artist), ddata->artist);
   etk_label_set(ETK_LABEL(gui->tracks_album), ddata->title);

   etk_tree_freeze(ETK_TREE(gui->tracks_tree));
   
   row = etk_tree_first_row_get(ETK_TREE(gui->tracks_tree));
   
   for(i=0; i < gui->ex->disc_info.num_tracks; i++)
     {
	char *track_name;
	char *track_time;
	char *track_num;
	char *mins;
	char *secs;
	int   size;
	int   tmp;
	int   selected = 1;
	
	track_num = ex_util_num_to_str(i + 1, gui->ex->disc_info.num_tracks);
	
	size = strlen(gui->ex->disc_data.track[i].name) + 1;
	track_name = calloc(size, sizeof(char));
	snprintf(track_name, size, "%s", 
		 gui->ex->disc_data.track[i].name);
	
	
	if(gui->ex->disc_info.track[i].length.mins < 10)
	  tmp = gui->ex->disc_info.track[i].length.mins * 10;
	else
	  tmp = gui->ex->disc_info.track[i].length.mins;
	mins = ex_util_num_to_str(gui->ex->disc_info.track[i].length.mins,
				   tmp);
	
	if(gui->ex->disc_info.track[i].length.secs < 10)
	  tmp = gui->ex->disc_info.track[i].length.secs * 10;
	else
	  tmp = gui->ex->disc_info.track[i].length.secs;
	
	secs = ex_util_num_to_str(gui->ex->disc_info.track[i].length.secs,
				   tmp);
	
	size = strlen(mins) + strlen(secs) + 2;
	track_time = calloc(size, sizeof(char));
	snprintf(track_time, size, "%s:%s", mins, secs);
		
	if(row)
	  {
	     etk_tree_row_fields_get(row,
				     TRACKS_RIP_COL,
				     &selected,
				     NULL);	
	     
	     etk_tree_row_fields_set(row, ETK_FALSE,
				     TRACKS_NUM_COL, track_num,
				     TRACKS_NAME_COL, track_name,
				     TRACKS_TIME_COL, track_time,
				     TRACKS_RIP_COL, selected,
				     NULL);
	  }
	else
	  {
	     etk_tree_row_append(ETK_TREE(gui->tracks_tree), NULL,
				  TRACKS_NUM_COL, track_num,
				  TRACKS_NAME_COL, track_name,
				  TRACKS_TIME_COL, track_time,
				  TRACKS_RIP_COL, selected,
				  NULL);
	  }
	
	E_FREE(track_name);
	E_FREE(track_time);
	E_FREE(track_num);
	E_FREE(mins);
	E_FREE(secs);
	
	if(row)
	  row = etk_tree_row_next_get(row);	
     }
   
   etk_tree_thaw(ETK_TREE(gui->tracks_tree));
}

void
etk_fe_rip_percent_update(double percent)
{
   double percent_total;
   char   text[32];
   
   snprintf(text, sizeof(text), "%.2f%% done", percent * 100.0);
   etk_progress_bar_text_set(ETK_PROGRESS_BAR(gui->rip_cur_pbar), text);
   etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(gui->rip_cur_pbar), percent);
   if(gui->ex->rip.num_total != 0)
     percent_total = ((float)gui->ex->rip.num_done/(float)gui->ex->rip.num_total) + (percent/gui->ex->rip.num_total);
   else
     percent_total =  percent;
   snprintf(text, sizeof(text), "%.2f%% done", percent_total * 100.0);
   etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(gui->rip_tot_pbar), percent_total);
   etk_progress_bar_text_set(ETK_PROGRESS_BAR(gui->rip_tot_pbar), text);      
}

Eina_List *
_etk_fe_tree_selected_rows_get(Etk_Tree *tree)
{
  Eina_List *selected_rows = NULL;
  Etk_Tree_Row *row;

  if (!tree)
    return NULL;

  if (!etk_tree_multiple_select_get(tree))
    {
      selected_rows = eina_list_append(selected_rows, etk_tree_selected_row_get(tree));
    }
  else
    {
      for (row = etk_tree_first_row_get(tree); row; row = row->next)
        {
          if (etk_tree_row_is_selected(row))
            {
              selected_rows = eina_list_append(selected_rows, row);
            }
        }
    }
  return selected_rows;
}

/* the exported frontend struct */
/********************************/

Extrackt_Frontend etk_fe = 
{
   etk_fe_init,
   etk_fe_main,
   etk_fe_shutdown,
   etk_fe_disc_update,
   etk_fe_rip_percent_update,
   NULL
};

/* return the main fe functions 
 * instead of declaring it extern */

Extrackt_Frontend *
etk_fe_get(void)
{
   return &etk_fe;
}
