/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"

#define NEWD(str, typ) \
     eet_data_descriptor_new(str, sizeof(typ), \
				(void *(*) (void *))evas_list_next, \
				(void *(*) (void *, void *))evas_list_append, \
				(void *(*) (void *))evas_list_data, \
				(void *(*) (void *))evas_list_free, \
				(void  (*) (void *, int (*) (void *, const char *, void *, void *), void *))evas_hash_foreach, \
				(void *(*) (void *, const char *, void *))evas_hash_add, \
				(void  (*) (void *))evas_hash_free)

#define FREED(eed) \
	 if (eed) \
	     { \
		eet_data_descriptor_free((eed)); \
		(eed) = NULL; \
	     }

#define APP_NEW(d, e) \
   if (etk_entry_text_get(ETK_ENTRY(d))) \
     { \
	E_FREE(e); \
	e = strdup(etk_entry_text_get(ETK_ENTRY(d))); \
     } 

#define CFG_OPTIONS_NEWI(str, it, type) EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_options_edd, Ex_Options, str, it, type)

#define VER_NEWI(str, it, type) EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_version_edd, Ex_Config_Version, str, it, type)

#define IS_SELECTED(w) etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(w))

static Eet_Data_Descriptor *_ex_config_options_edd = NULL;
static Eet_Data_Descriptor *_ex_config_version_edd = NULL;

static Ex_Config_Version *_ex_options_version_parse(char *version);
static int _ex_options_version_compare(Ex_Config_Version *v1, Ex_Config_Version *v2);
static void _ex_options_set_cancel_cb(Etk_Object *object, void *data);
static void _ex_options_set_apply_cb(Etk_Object *object, void *data);
static void _ex_options_set_ok_cb(Etk_Object *object, void *data);
static Etk_Bool _ex_options_window_delete_cb(void *data);
static void _ex_options_set(void);
static Etk_Widget *_ex_options_page_1_create(void);
static Etk_Widget *_ex_options_page_2_create(void);
static void _ex_options_combobox_active_item_changed_cb(Etk_Object *object, void *data);
static Etk_Widget *_ex_options_page_3_create(void);
static Etk_Widget *_ex_options_page_4_create(void);

int
_ex_options_init(void)
{
   char     *home;
   char      buf[PATH_MAX];
   
   home = getenv("HOME");
   if(!home)
     {
	fprintf(stderr, "Cant find home directory!\n");
	return 0;
     }
   
   /* make sure ~/.e exists and is a dir */
      snprintf(buf, sizeof(buf), "%s/.e", home);
   if(!ecore_file_is_dir(buf))
     {
	if(ecore_file_exists(buf))
	  {
	     fprintf(stderr, "Cant create config path!\n");
	     return 0;
	  }
	
	if(!ecore_file_mkdir(buf))
	  {
	     fprintf(stderr, "Cant create config path!");
	     return 0;
	  }
     }
   
   /* make sure ~/.e/exhibit exists and is a dir */
   snprintf(buf, sizeof(buf), "%s/.e/exhibit", home);
   if(!ecore_file_is_dir(buf))
     {
	if(ecore_file_exists(buf))
	  {
	     fprintf(stderr, "Cant create config path!");
	     return 0;
	  }
	
	if(!ecore_file_mkdir(buf))
	  {
	     fprintf(stderr, "Cant create config path!");
	     return 0;
	  }
     }

   _ex_config_options_edd = NEWD("Ex_Options", Ex_Options);
   CFG_OPTIONS_NEWI("a1", app1, EET_T_STRING);
   CFG_OPTIONS_NEWI("a2", app2, EET_T_STRING);
   CFG_OPTIONS_NEWI("a3", app3, EET_T_STRING);
   CFG_OPTIONS_NEWI("a4", app4, EET_T_STRING);
   CFG_OPTIONS_NEWI("a1_cmd", app1_cmd, EET_T_STRING);
   CFG_OPTIONS_NEWI("a2_cmd", app2_cmd, EET_T_STRING);
   CFG_OPTIONS_NEWI("a3_cmd", app3_cmd, EET_T_STRING);
   CFG_OPTIONS_NEWI("a4_cmd", app4_cmd, EET_T_STRING);
   CFG_OPTIONS_NEWI("dlp", dl_path, EET_T_STRING);
   CFG_OPTIONS_NEWI("fp", fav_path, EET_T_STRING);
   CFG_OPTIONS_NEWI("bt", blur_thresh, EET_T_DOUBLE);
   CFG_OPTIONS_NEWI("st", sharpen_thresh, EET_T_DOUBLE);
   CFG_OPTIONS_NEWI("rt", brighten_thresh, EET_T_DOUBLE);
   CFG_OPTIONS_NEWI("lt", slide_interval, EET_T_DOUBLE);
   CFG_OPTIONS_NEWI("cv", comments_visible, EET_T_INT);
   CFG_OPTIONS_NEWI("ra", rotate_autosave, EET_T_INT);
   CFG_OPTIONS_NEWI("ts", thumbs_show, EET_T_INT);
   CFG_OPTIONS_NEWI("mf", monitor_focus, EET_T_INT);
   CFG_OPTIONS_NEWI("dv", default_view, EET_T_INT);
   CFG_OPTIONS_NEWI("ds", default_sort, EET_T_INT);
   CFG_OPTIONS_NEWI("saft", show_all_filetypes, EET_T_INT);
   CFG_OPTIONS_NEWI("lh", list_hidden, EET_T_INT);
   CFG_OPTIONS_NEWI("lw", last_w, EET_T_INT);
   CFG_OPTIONS_NEWI("lh", last_h, EET_T_INT);
   
   _ex_config_version_edd = NEWD("Ex_Config_Version", Ex_Config_Version);
   VER_NEWI("mj", major, EET_T_INT);
   VER_NEWI("mn", minor, EET_T_INT);
   VER_NEWI("pa", patch, EET_T_INT);

   return 1;
}

int
_ex_options_shutdown(void)
{
   FREED(_ex_config_options_edd);
   FREED(_ex_config_version_edd);   
   return 1;
}  

static Ex_Config_Version *
_ex_options_version_parse(char *version)
{
   Ex_Config_Version *v;
   int res;
   
   v = calloc(1, sizeof(Ex_Config_Version));
   res = sscanf(version, "%d.%d.%d", &v->major, &v->minor, &v->patch);
   
   if(res < 3)
     return NULL;
   
   return v;
}

/*
 * Compare 2 versions, return 1 if v1 > v2
 *                     return 0 if v1 == v2
 *                     return -1 if v1 < v2
 */
static int
_ex_options_version_compare(Ex_Config_Version *v1, Ex_Config_Version *v2)
{
   if(v1->major > v2->major)
     return 1;
   else if (v1->major < v2->major)
     return -1;
   
   if(v1->minor > v2->minor)
     return 1;
   else if (v1->minor < v2->minor)
     return -1;
   
   if(v1->patch > v2->patch)
     return 1;
   else if (v1->patch < v2->patch)
     return -1;
   
   return 0;
}

Ex_Options *
_ex_options_new(void)
{
   Ex_Options *o;
   
   o = calloc(1, sizeof(Ex_Options));   
   return o;
}

void
_ex_options_default(Exhibit *e)
{   
   if(!e->options)
     e->options = _ex_options_new();

   D(("Config: using default options!\n"));
   
   /* TODO: free values before allocating if e->options != NULL */
   
   e->options->app1 = strdup("The Gimp");
   e->options->app1_cmd = strdup("gimp \"%s\"");
   e->options->app2 = strdup("Xv");
   e->options->app2_cmd = strdup("xv \"%s\"");
   e->options->app3 = strdup("Xpaint");
   e->options->app3_cmd = strdup("xpaint \"%s\"");
   e->options->app4 =     NULL;
   e->options->app4_cmd =     NULL;
   e->options->fav_path = NULL;   
   e->options->dl_path = strdup("/tmp");   
   e->options->blur_thresh      = EX_DEFAULT_BLUR_THRESH;
   e->options->sharpen_thresh   = EX_DEFAULT_SHARPEN_THRESH;
   e->options->brighten_thresh  = EX_DEFAULT_BRIGHTEN_THRESH;
   e->options->slide_interval   = EX_DEFAULT_SLIDE_INTERVAL;
   e->options->comments_visible = EX_DEFAULT_COMMENTS_HIDDEN;
   e->options->thumbs_show      = ETK_TRUE;
   e->options->default_view     = EX_IMAGE_ONE_TO_ONE;
   e->options->default_sort     = EX_SORT_BY_NAME;
   e->options->show_all_filetypes = ETK_FALSE;
   e->options->list_hidden	= ETK_FALSE;
   e->options->last_w           = EX_DEFAULT_WINDOW_WIDTH;
   e->options->last_h           = EX_DEFAULT_WINDOW_HEIGHT;
   e->options->rotate_autosave  = ETK_FALSE;
   e->options->monitor_focus    = ETK_FALSE;
   e->version = _ex_options_version_parse(VERSION);        
}

void
_ex_options_free(Exhibit *e)
{
   E_FREE(e->options->app1);
   E_FREE(e->options->app2);
   E_FREE(e->options->app3);
   E_FREE(e->options->app4);
   E_FREE(e->options->app1_cmd);
   E_FREE(e->options->app2_cmd);
   E_FREE(e->options->app3_cmd);
   E_FREE(e->options->app4_cmd);
   E_FREE(e->options->fav_path);
   E_FREE(e->options->dl_path);
   E_FREE(e->options);
}

int
_ex_options_save(Exhibit *e)
{
   Eet_File  *ef;
   char       buf[PATH_MAX];
   char      *home;
   int        ret;
   
   home = getenv("HOME");
   if(!home)
     return 0;
   
   snprintf(buf, sizeof(buf), "%s/.e/exhibit/config.eet", home);
   
   ef = eet_open(buf, EET_FILE_MODE_WRITE);
   if(!ef)
     return 0;
   
   ret = eet_data_write(ef, _ex_config_version_edd, "config/version", e->version, 1);
   if(!ret)
     fprintf(stderr, "Problem saving config!\n");
         
   ret = eet_data_write(ef, _ex_config_options_edd, "config/options", e->options, 1);
   if(!ret)
     fprintf(stderr, "Problem saving config/options!");

   D(("Saving configuation (%s)\n", buf));
   
   eet_close(ef);
   return ret;   
}

int
_ex_options_load(Exhibit *e)
{
   Eet_File *ef;
   char      buf[PATH_MAX];
   char     *home;
   
   home = getenv("HOME");
   if(!home)
     {
	_ex_options_default(e);
	return 0;
     }
   
   snprintf(buf, sizeof(buf), "%s/.e/exhibit/config.eet", home);
   
   if(!ecore_file_exists(buf) || ecore_file_size(buf) == 0)
     {
	/* no saved config */
	_ex_options_default(e);
	return 0;
     }
   
   ef = eet_open(buf, EET_FILE_MODE_READ);
   if(!ef)
     {
	fprintf(stderr, "Cant open configuration file! Using program defaults.\n");
	return 0;
     }
   
   E_FREE(e->version);
   e->version = eet_data_read(ef, _ex_config_version_edd, "config/version");
   if(!e->version)
     {
	fprintf(stderr, "Incompatible configuration file! Creating new one.\n");
	eet_close(ef);
	_ex_options_default(e);
	return 0;
     }
   else
     {
	Ex_Config_Version *v;
	
	v = _ex_options_version_parse(VERSION);
	if(_ex_options_version_compare(v, e->version) != 0)
	  {
	     fprintf(stderr, "Your version / configuration of Exhibit is not valid!\n");
	     _ex_options_default(e);	     
	     return 0;
	  }
     }
   
   e->options = eet_data_read(ef, _ex_config_options_edd, "config/options");
   D(("Config: Loaded saved options (%s)\n", buf));

   eet_close(ef);
   return 1;
}

static void
_ex_options_set_cancel_cb(Etk_Object *object, void *data)
{
   etk_object_destroy(ETK_OBJECT(object));
   E_FREE(e->opt_dialog);
}

static void
_ex_options_set_apply_cb(Etk_Object *object, void *data)
{
   _ex_options_set();
   _ex_options_save(e);
}

static void
_ex_options_set_ok_cb(Etk_Object *object, void *data)
{
   _ex_options_set();
   _ex_options_save(e);
   etk_object_destroy(ETK_OBJECT(object));

   E_FREE(e->opt_dialog);
}

static Etk_Bool 
_ex_options_window_delete_cb(void *data)
{
   etk_object_destroy(ETK_OBJECT(data));
   E_FREE(e->opt_dialog);
   return ETK_TRUE;
}

static void
_ex_options_set(void)
{
   Ex_Options_Dialog *dialog = e->opt_dialog;
   const char *string;
   double ss_int;
   
   /* STANDARD VIEW */
   if (IS_SELECTED(dialog->dv_btn_1))
     {
	e->options->default_view = EX_IMAGE_ONE_TO_ONE;
	_ex_tab_current_zoom_one_to_one(e);
     } 
   else if (IS_SELECTED(dialog->dv_btn_2))
     {
	e->options->default_view = EX_IMAGE_FIT_TO_WINDOW;
	e->cur_tab->fit_window = ETK_TRUE;
	_ex_tab_current_fit_to_window(e);
     }

   /* COMMENTS */
   if (IS_SELECTED(dialog->comments_visible))
     {
	e->options->comments_visible = EX_DEFAULT_COMMENTS_VISIBLE;
	_ex_comment_show(e);
	_ex_comment_load(e);
     }
   else
     {
	e->options->comments_visible = EX_DEFAULT_COMMENTS_HIDDEN;
	_ex_comment_hide(e);
     }

   /* ROTATE */
   if (IS_SELECTED(dialog->rotate_autosave))
	e->options->rotate_autosave = ETK_TRUE;
   else 
	e->options->rotate_autosave = ETK_FALSE;

   /* BLUR */
   string = etk_entry_text_get(ETK_ENTRY(dialog->blur_thresh));
   if (string)
   {
	e->options->blur_thresh = atof(string);
	if (e->options->blur_thresh <= 1)
	   _ex_main_dialog_show("One, Zero or negative value for blur tresh " \
		 "is not recommended! ", ETK_MESSAGE_DIALOG_WARNING);
     }
   else 
     {
	_ex_main_dialog_show("Missing value for blur thresh, but still " \
	      "saving the other options!", ETK_MESSAGE_DIALOG_WARNING);
     }

   /* SHARPEN */
   string = etk_entry_text_get(ETK_ENTRY(dialog->sharpen_thresh));
   if (string)
     {
	e->options->sharpen_thresh = atof(string);
	if (e->options->sharpen_thresh <= 0)
	   _ex_main_dialog_show("Zero or negative value for sharpen tresh " \
		 "is not recommended! ", ETK_MESSAGE_DIALOG_WARNING);
     }
   else 
     {
	_ex_main_dialog_show("Missing value for sharpen thresh, but still " \
	      "saving the other options!", ETK_MESSAGE_DIALOG_WARNING);
     }

   /* BRIGHTEN */
   string = etk_entry_text_get(ETK_ENTRY(dialog->brighten_thresh));
   if (string)
     {
	e->options->brighten_thresh = atof(string);
	if (e->options->brighten_thresh <= 0)
	   _ex_main_dialog_show("Zero or negative value for brighten tresh " \
		 "is not recommended! ", ETK_MESSAGE_DIALOG_WARNING);
	else if (e->options->brighten_thresh > 255)
	   _ex_main_dialog_show("Bigger then 255 value for brighten tresh " \
		 "is not recommended! ", ETK_MESSAGE_DIALOG_WARNING);
     } 
   else
     {
	_ex_main_dialog_show("Missing value for brighten thresh, but still " \
	      "saving the other options!", ETK_MESSAGE_DIALOG_WARNING);
     }

   /* SLIDESHOW */
   string = etk_entry_text_get(ETK_ENTRY(dialog->slide_interval));
   if (string)
     {
	ss_int = atof(string);
	if (ss_int <= 0)
	   _ex_main_dialog_show("Zero or negative value for slideshow interval " \
		 "is not possible, skipping! ", ETK_MESSAGE_DIALOG_WARNING);
	else
	  e->options->slide_interval = ss_int;
     }
   else
     {
	_ex_main_dialog_show("Missing value for slideshow interval, but still " \
	      "saving the other options!", ETK_MESSAGE_DIALOG_WARNING);
     }

   /* DOWNLOAD PATH */
   string = etk_entry_text_get(ETK_ENTRY(dialog->dl_path));
   if (string)
     {
	E_FREE(e->options->dl_path);
	e->options->dl_path = strdup(string);
     }
   else
     {
	_ex_main_dialog_show("Missing value for image download path, but still " \
	      "saving the other options!", ETK_MESSAGE_DIALOG_WARNING);
     }

   /* THUMBS */
   if (IS_SELECTED(dialog->thumbs_show))
	e->options->thumbs_show = ETK_TRUE;
   else 
	e->options->thumbs_show = ETK_FALSE;

   /* SORTING */
   e->options->default_sort = e->options->default_sort_tmp;
   etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
   etk_tree_clear(ETK_TREE(e->cur_tab->itree));
   etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));

   /* MONITOR FOCUS */
   if (IS_SELECTED(dialog->monitor_focus))
	e->options->monitor_focus = ETK_TRUE;
   else 
	e->options->monitor_focus = ETK_FALSE;

   /* SHOW ALL FILETYPESS */
   if (IS_SELECTED(dialog->show_all_filetypes))
	e->options->show_all_filetypes = ETK_TRUE;
   else 
	e->options->show_all_filetypes = ETK_FALSE;

   /* LIST HIDDEN FILES AND DIRECTORIES*/
   if (IS_SELECTED(dialog->list_hidden))
	e->options->list_hidden = ETK_TRUE;
   else 
	e->options->list_hidden = ETK_FALSE;

   /* RUN IN */
   APP_NEW(dialog->app1, e->options->app1);
   APP_NEW(dialog->app1_cmd, e->options->app1_cmd);
   APP_NEW(dialog->app2, e->options->app2);
   APP_NEW(dialog->app2_cmd, e->options->app2_cmd);
   APP_NEW(dialog->app3, e->options->app3);
   APP_NEW(dialog->app3_cmd, e->options->app3_cmd);
   APP_NEW(dialog->app4, e->options->app4);
   APP_NEW(dialog->app4_cmd, e->options->app4_cmd);

   /* REGENERATE EVERYTHING */
   etk_menu_shell_remove(ETK_MENU_SHELL(e->submenu), ETK_MENU_ITEM(e->app1_menu));
   etk_menu_shell_remove(ETK_MENU_SHELL(e->submenu), ETK_MENU_ITEM(e->app2_menu));
   etk_menu_shell_remove(ETK_MENU_SHELL(e->submenu), ETK_MENU_ITEM(e->app3_menu));
   etk_menu_shell_remove(ETK_MENU_SHELL(e->submenu), ETK_MENU_ITEM(e->app4_menu));
   _ex_menu_build_run_menu(NULL);
   _ex_main_populate_files(NULL, EX_TREE_UPDATE_ALL);
}

static Etk_Widget *
_ex_options_page_1_create(void)
{
   Etk_Widget *vbox, *hbox;
   Etk_Widget *vbox2;
   Etk_Widget *frame;
   Etk_Widget *label;
   char string[256];
   
   Ex_Options_Dialog *dialog = e->opt_dialog;
   vbox = etk_vbox_new(ETK_FALSE, 3);
   
   frame = etk_frame_new("Choose standard view");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   dialog->dv_btn_1 = etk_radio_button_new_with_label("Zoom 1:1", NULL);
   etk_box_append(ETK_BOX(vbox2), dialog->dv_btn_1, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   dialog->dv_btn_2 = etk_radio_button_new_with_label_from_widget("Fit to window", 
	 ETK_RADIO_BUTTON(dialog->dv_btn_1));
   etk_box_append(ETK_BOX(vbox2), dialog->dv_btn_2, ETK_BOX_START, ETK_BOX_NONE, 0);

   frame = etk_frame_new("Slideshow");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), hbox);

   label = etk_label_new("Interval"); 
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   dialog->slide_interval = etk_entry_new();
   etk_box_append(ETK_BOX(hbox), dialog->slide_interval, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   frame = etk_frame_new("Comments");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   dialog->comments_visible = etk_check_button_new_with_label("Visible");
   etk_box_append(ETK_BOX(vbox2), dialog->comments_visible, ETK_BOX_START, ETK_BOX_NONE, 0);

   frame = etk_frame_new("Image download");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), hbox);

   label = etk_label_new("Default storage path"); 
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   dialog->dl_path = etk_entry_new();
   etk_box_append(ETK_BOX(hbox), dialog->dl_path, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* 
    * Start toggling/setting the correct values from loaded options 
    */
   if (e->options->default_view == EX_IMAGE_ONE_TO_ONE)
      etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->dv_btn_1));
   else if (e->options->default_view == EX_IMAGE_FIT_TO_WINDOW)
      etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->dv_btn_2));

   if (e->options->comments_visible == EX_DEFAULT_COMMENTS_VISIBLE)
     etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->comments_visible));


   snprintf(string, sizeof(string), "%.2f", e->options->slide_interval);
   etk_entry_text_set(ETK_ENTRY(dialog->slide_interval), string);
   
   etk_entry_text_set(ETK_ENTRY(dialog->dl_path), e->options->dl_path);
   
   return vbox;
}

static Etk_Widget *
_ex_options_page_2_create(void)
{
   Etk_Widget *vbox, *vbox2;
   Etk_Widget *label;
   Etk_Widget *frame, *table;
   Ex_Options_Dialog *dialog = e->opt_dialog;
   char string[256];
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   
   frame = etk_frame_new("Effect thresh");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);

   table = etk_table_new(2, 3, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_box_append(ETK_BOX(vbox2), table, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   label = etk_label_new("Blur thresh"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, ETK_TABLE_NONE, 0, 0);
   dialog->blur_thresh = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->blur_thresh, 1, 1, 0, 0, ETK_TABLE_NONE, 0, 0);
   
   label = etk_label_new("Sharpen thresh"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, ETK_TABLE_NONE, 0, 0);
   dialog->sharpen_thresh = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->sharpen_thresh, 1, 1, 1, 1, ETK_TABLE_NONE, 0, 0);

   label = etk_label_new("Brighten thresh"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2, ETK_TABLE_NONE, 0, 0);
   dialog->brighten_thresh = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->brighten_thresh, 1, 1, 2, 2, ETK_TABLE_NONE, 0, 0);

   frame = etk_frame_new("Rotate");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   dialog->rotate_autosave = etk_check_button_new_with_label("Autosave after rotate");
   etk_box_append(ETK_BOX(vbox2), dialog->rotate_autosave, ETK_BOX_START,
         ETK_BOX_NONE, 0);

   snprintf(string, sizeof(string), "%.2f", e->options->blur_thresh);
   etk_entry_text_set(ETK_ENTRY(dialog->blur_thresh), string);
   snprintf(string, sizeof(string), "%.2f", e->options->sharpen_thresh);
   etk_entry_text_set(ETK_ENTRY(dialog->sharpen_thresh), string);
   snprintf(string, sizeof(string), "%.2f", e->options->brighten_thresh);
   etk_entry_text_set(ETK_ENTRY(dialog->brighten_thresh), string);

   if (e->options->rotate_autosave)
     etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->rotate_autosave));

   return vbox;
}

static void 
_ex_options_combobox_active_item_changed_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Item *item;
   Ex_Options_Dialog *dialog = e->opt_dialog;

   item = etk_combobox_active_item_get(ETK_COMBOBOX(object));

   if (item == dialog->sort_date)
     e->options->default_sort_tmp = EX_SORT_BY_DATE;
   else if (item == dialog->sort_size)
     e->options->default_sort_tmp = EX_SORT_BY_SIZE;
   else if (item == dialog->sort_name)
     e->options->default_sort_tmp = EX_SORT_BY_NAME;
   else if (item == dialog->sort_resolution)
     e->options->default_sort_tmp = EX_SORT_BY_RESOLUTION;
}

static Etk_Widget *
_ex_options_page_3_create(void)
{
   Etk_Widget *vbox, *vbox1, *hbox;
   Etk_Widget *vbox2;
   Etk_Widget *frame, *label;
   Etk_Widget *image;
   Ex_Options_Dialog *dialog = e->opt_dialog;

   vbox = etk_vbox_new(ETK_FALSE, 3);
   
   frame = etk_frame_new("Thumbs");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   vbox1 = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(frame), vbox1);

   dialog->thumbs_show = etk_check_button_new_with_label("Show Thumbnails");
   etk_box_append(ETK_BOX(vbox1), dialog->thumbs_show, ETK_BOX_START, ETK_BOX_NONE, 0);
   if (e->options->thumbs_show)
     etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->thumbs_show));

   frame = etk_frame_new("Directory listing");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox2), hbox, ETK_BOX_START, 
	 ETK_BOX_NONE, 0);

   label = etk_label_new("Default sort by"); 
   etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, 
	 ETK_BOX_NONE, 0);
  
   e->options->default_sort_tmp = e->options->default_sort;

   dialog->default_sort = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(dialog->default_sort), ETK_COMBOBOX_IMAGE, 
	 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(dialog->default_sort), ETK_COMBOBOX_LABEL, 
	 75, ETK_COMBOBOX_EXPAND, 0.0);
   etk_combobox_build(ETK_COMBOBOX(dialog->default_sort));
   etk_box_append(ETK_BOX(hbox), dialog->default_sort, ETK_BOX_START, 
	 ETK_BOX_NONE, 0);

   image = etk_image_new_from_stock(ETK_STOCK_OFFICE_CALENDAR, ETK_STOCK_SMALL);
   dialog->sort_date = etk_combobox_item_append(ETK_COMBOBOX(dialog->default_sort), image, "Date");
   image = etk_image_new_from_stock(ETK_STOCK_DRIVE_HARDDISK, ETK_STOCK_SMALL);
   dialog->sort_size = etk_combobox_item_append(ETK_COMBOBOX(dialog->default_sort), image, "Size");
   image = etk_image_new_from_stock(ETK_STOCK_TEXT_X_GENERIC, ETK_STOCK_SMALL);
   dialog->sort_name = etk_combobox_item_append(ETK_COMBOBOX(dialog->default_sort), image, "Name");
   image = etk_image_new_from_stock(ETK_STOCK_UTILITIES_SYSTEM_MONITOR, ETK_STOCK_SMALL);
   dialog->sort_resolution = etk_combobox_item_append(ETK_COMBOBOX(dialog->default_sort), image, "Resolution");
   
   etk_signal_connect("active-item-changed", ETK_OBJECT(dialog->default_sort), 
	 ETK_CALLBACK(_ex_options_combobox_active_item_changed_cb), NULL);
   
   dialog->monitor_focus = etk_check_button_new_with_label("Autofocus new images added to your current dir");
   etk_box_append(ETK_BOX(vbox2), dialog->monitor_focus, ETK_BOX_START, ETK_BOX_NONE, 0);
   dialog->show_all_filetypes = etk_check_button_new_with_label("Show all kind of filetypes");
   etk_box_append(ETK_BOX(vbox2), dialog->show_all_filetypes, ETK_BOX_START, ETK_BOX_NONE, 0);
   dialog->list_hidden = etk_check_button_new_with_label("List hidden files and directories");
   etk_box_append(ETK_BOX(vbox2), dialog->list_hidden, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   if (e->options->monitor_focus)
     etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->monitor_focus));
   if (e->options->show_all_filetypes)
     etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->show_all_filetypes));
   if (e->options->list_hidden)
     etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->list_hidden));

   if (e->options->default_sort == EX_SORT_BY_DATE)
      etk_combobox_active_item_set(ETK_COMBOBOX(dialog->default_sort), dialog->sort_date);
   else if (e->options->default_sort == EX_SORT_BY_SIZE)
      etk_combobox_active_item_set(ETK_COMBOBOX(dialog->default_sort), dialog->sort_size);
   else if (e->options->default_sort == EX_SORT_BY_NAME)
      etk_combobox_active_item_set(ETK_COMBOBOX(dialog->default_sort), dialog->sort_name);
   else if (e->options->default_sort == EX_SORT_BY_RESOLUTION)
      etk_combobox_active_item_set(ETK_COMBOBOX(dialog->default_sort), dialog->sort_resolution);

   return vbox;
}


static Etk_Widget *
_ex_options_page_4_create(void)
{
   Etk_Widget *vbox, *vbox2;
   Etk_Widget *frame, *table;
   Etk_Widget *label;
   Ex_Options_Dialog *dialog = e->opt_dialog;

   vbox = etk_vbox_new(ETK_FALSE, 3);
   
   frame = etk_frame_new("Run in ...");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 5);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   label = etk_label_new("Hint: application command has %s as file argument."); 
   etk_box_append(ETK_BOX(vbox2), label, ETK_BOX_START, ETK_BOX_NONE, 5);
   etk_box_append(ETK_BOX(vbox2), etk_hseparator_new(), ETK_BOX_START, ETK_BOX_NONE, 5);

   table = etk_table_new(3, 5, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_box_append(ETK_BOX(vbox2), table, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   label = etk_label_new("Menu display"); 
   etk_table_attach(ETK_TABLE(table), label, 1, 1, 0, 0, ETK_TABLE_NONE, 0, 0);
   label = etk_label_new("Command"); 
   etk_table_attach(ETK_TABLE(table), label, 2, 2, 0, 0, ETK_TABLE_NONE, 0, 0);

   label = etk_label_new("Application 1"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, ETK_TABLE_NONE, 5, 0);
   dialog->app1 = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->app1, 1, 1, 1, 1, ETK_TABLE_NONE, 0, 0);
   dialog->app1_cmd = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->app1_cmd, 2, 2, 1, 1, ETK_TABLE_NONE, 0, 0);
   
   label = etk_label_new("Application 2"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2, ETK_TABLE_NONE, 0, 0);
   dialog->app2 = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->app2, 1, 1, 2, 2, ETK_TABLE_NONE, 0, 0);
   dialog->app2_cmd = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->app2_cmd, 2, 2, 2, 2, ETK_TABLE_NONE, 0, 0);
   
   label = etk_label_new("Application 3"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 3, 3, ETK_TABLE_NONE, 0, 0);
   dialog->app3 = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->app3, 1, 1, 3, 3, ETK_TABLE_NONE, 0, 0);
   dialog->app3_cmd = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->app3_cmd, 2, 2, 3, 3, ETK_TABLE_NONE, 0, 0);

   label = etk_label_new("Application 4"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 4, 4, ETK_TABLE_NONE, 0, 0);
   dialog->app4 = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->app4, 1, 1, 4, 4, ETK_TABLE_NONE, 0, 0);
   dialog->app4_cmd = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->app4_cmd, 2, 2, 4, 4, ETK_TABLE_NONE, 0, 0);

   if (e->options->app1)
	etk_entry_text_set(ETK_ENTRY(dialog->app1), e->options->app1);
   if (e->options->app1_cmd)
	etk_entry_text_set(ETK_ENTRY(dialog->app1_cmd), e->options->app1_cmd);

   if (e->options->app2)
	etk_entry_text_set(ETK_ENTRY(dialog->app2), e->options->app2);
   if (e->options->app2_cmd)
	etk_entry_text_set(ETK_ENTRY(dialog->app2_cmd), e->options->app2_cmd);
   
   if (e->options->app3)
	etk_entry_text_set(ETK_ENTRY(dialog->app3), e->options->app3);
   if (e->options->app3_cmd)
	etk_entry_text_set(ETK_ENTRY(dialog->app3_cmd), e->options->app3_cmd);

   if (e->options->app4)
	etk_entry_text_set(ETK_ENTRY(dialog->app4), e->options->app4);
   if (e->options->app4_cmd)
	etk_entry_text_set(ETK_ENTRY(dialog->app4_cmd), e->options->app4_cmd);

   return vbox;
}

void
_ex_options_window_show(void)
{
   Etk_Widget *win;
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *notebook;
   Etk_Widget *page;
   Etk_Widget *button;
   
   if(e->opt_dialog)
     return;
   
   e->opt_dialog = calloc(1, sizeof(Ex_Options_Dialog));   
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Exhibit options");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete-event", ETK_OBJECT(win),
		      ETK_CALLBACK(_ex_options_window_delete_cb), win);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   notebook = etk_notebook_new();
   etk_box_append(ETK_BOX(vbox), notebook, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   page = _ex_options_page_1_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "General", page);
   page = _ex_options_page_2_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Effects", page);
   page = _ex_options_page_3_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Dir/thumb listing", page);
   page = _ex_options_page_4_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Exec commands", page);

   etk_box_append(ETK_BOX(vbox), etk_hseparator_new(), 
		      ETK_BOX_START, ETK_BOX_NONE, 5);

   hbox = etk_hbox_new(ETK_FALSE, 3);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button),
			      ETK_CALLBACK(_ex_options_set_ok_cb), win);
   
   button = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   etk_button_label_set(ETK_BUTTON(button), "Apply");
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("clicked", ETK_OBJECT(button),
		      ETK_CALLBACK(_ex_options_set_apply_cb), NULL);
   
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CANCEL);
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button),
			      ETK_CALLBACK(_ex_options_set_cancel_cb), win);

   etk_widget_show_all(ETK_WIDGET(win));
}

