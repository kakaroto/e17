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

#define CFG_OPTIONS_NEWI(str, it, type) EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_options_edd, Ex_Options, str, it, type)

#define VER_NEWI(str, it, type) EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_version_edd, Ex_Config_Version, str, it, type)

#define IS_SELECTED(w) etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(w))

static Eet_Data_Descriptor *_ex_config_options_edd = NULL;
static Eet_Data_Descriptor *_ex_config_version_edd = NULL;

static void _ex_options_set();
static void _ex_options_set_cancel_cb(Etk_Object *object, void *data);
static void _ex_options_set_apply_cb(Etk_Object *object, void *data);
static void _ex_options_set_ok_cb(Etk_Object *object, void *data);
static Etk_Bool _ex_options_window_delete_cb(void *data);


int
_ex_options_init()
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
   CFG_OPTIONS_NEWI("fp", fav_path, EET_T_STRING);
   CFG_OPTIONS_NEWI("bt", blur_thresh, EET_T_DOUBLE);
   CFG_OPTIONS_NEWI("st", sharpen_thresh, EET_T_DOUBLE);
   CFG_OPTIONS_NEWI("rt", brighten_thresh, EET_T_DOUBLE);
   CFG_OPTIONS_NEWI("lt", slide_interval, EET_T_DOUBLE);
   CFG_OPTIONS_NEWI("cv", comments_visible, EET_T_INT);
   CFG_OPTIONS_NEWI("ra", rotate_autosave, EET_T_INT);
   CFG_OPTIONS_NEWI("dv", default_view, EET_T_INT);
   CFG_OPTIONS_NEWI("ds", default_sort, EET_T_INT);
   CFG_OPTIONS_NEWI("lw", last_w, EET_T_INT);
   CFG_OPTIONS_NEWI("lh", last_h, EET_T_INT);
   
   _ex_config_version_edd = NEWD("Ex_Config_Version", Ex_Config_Version);
   VER_NEWI("mj", major, EET_T_INT);
   VER_NEWI("mn", minor, EET_T_INT);
   VER_NEWI("pa", patch, EET_T_INT);

   return 1;
}

int
_ex_options_shutdown()
{
   FREED(_ex_config_options_edd);
   FREED(_ex_config_version_edd);   
   return 1;
}  

Ex_Config_Version *
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
int
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
_ex_options_new()
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
   
   e->options->app1 =     NULL;
   e->options->app2 =     NULL;
   e->options->app3 =     NULL;
   e->options->app4 =     NULL;
   e->options->fav_path = NULL;   
   e->options->blur_thresh      = EX_DEFAULT_BLUR_THRESH;
   e->options->sharpen_thresh   = EX_DEFAULT_SHARPEN_THRESH;
   e->options->brighten_thresh  = EX_DEFAULT_BRIGHTEN_THRESH;
   e->options->slide_interval   = EX_DEFAULT_SLIDE_INTERVAL;
   e->options->comments_visible = EX_DEFAULT_COMMENTS_HIDDEN;
   e->options->default_view     = EX_IMAGE_ONE_TO_ONE;
   e->options->default_sort     = EX_SORT_BY_NAME;
   e->options->last_w           = EX_DEFAULT_WINDOW_WIDTH;
   e->options->last_h           = EX_DEFAULT_WINDOW_HEIGHT;
   e->options->rotate_autosave  = ETK_FALSE;
   e->version = _ex_options_version_parse(VERSION);        
}

void
_ex_options_free(Exhibit *e)
{
   E_FREE(e->options->app1);
   E_FREE(e->options->app2);
   E_FREE(e->options->app3);
   E_FREE(e->options->app4);
   E_FREE(e->options->fav_path);
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

   D(("Saving configuation (%s)\n", e->options->fav_path));
   
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
   
   D(("Config: Loaded saved options (%s)\n", e->options->fav_path));

   D(("Default view: %d\n", e->options->default_view));

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
_ex_options_set()
{
   Ex_Options_Dialog *dialog = e->opt_dialog;
   const char *string;
   double ss_int;
   
   
   /* STANDARD VIEW */
   if (IS_SELECTED(dialog->dv_btn_1))
     {
	D(("Zoom 1:1 is checked\n"));
	e->options->default_view = EX_IMAGE_ONE_TO_ONE;
	_ex_tab_current_zoom_one_to_one(e);
     } 
   else if (IS_SELECTED(dialog->dv_btn_2))
     {
	D(("Fit to window is checked\n"));
	e->options->default_view = EX_IMAGE_FIT_TO_WINDOW;
	e->cur_tab->fit_window = ETK_TRUE;
	_ex_tab_current_fit_to_window(e);
     }

   /* COMMENTS */
   if (IS_SELECTED(dialog->comments_visible))
     {
	D(("Comments EX_DEFAULT_COMMENTS_VISIBLE\n"));
	e->options->comments_visible = EX_DEFAULT_COMMENTS_VISIBLE;
	_ex_comment_show(e);
	_ex_comment_load(e);
     }
   else
     {
	D(("Comments EX_DEFAULT_COMMENTS_HIDDEN\n"));
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
	D(("Setting blur thresh: %f\n", atof(string)));
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
	D(("Setting sharpen thresh: %f\n", atof(string)));
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
	D(("Setting brighten thresh: %f\n", atof(string)));
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
	  {
	     D(("Setting slide_interval: %f\n", ss_int));
	     e->options->slide_interval = ss_int;
	  }
     }
   else
     {
	_ex_main_dialog_show("Missing value for slideshow interval, but still " \
	      "saving the other options!", ETK_MESSAGE_DIALOG_WARNING);
     }

   /* SORTING */
   e->options->default_sort = e->options->default_sort_tmp;
   etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
   etk_tree_clear(ETK_TREE(e->cur_tab->itree));
   _ex_main_populate_files(e, NULL);

}

static Etk_Widget *
_ex_options_page_1_create()
{
   Etk_Widget *vbox, *hbox;
   Etk_Widget *vbox2;
   Etk_Widget *frame;
   Etk_Widget *label;
   char string[256];
   
   Ex_Options_Dialog *dialog = e->opt_dialog;
   vbox = etk_vbox_new(ETK_FALSE, 3);
   
   frame = etk_frame_new("Choose standard view");
   etk_box_pack_start(ETK_BOX(vbox), frame, ETK_FALSE, ETK_FALSE, 5);
   
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   dialog->dv_btn_1 = etk_radio_button_new_with_label("Zoom 1:1", NULL);
   etk_box_pack_start(ETK_BOX(vbox2), dialog->dv_btn_1, ETK_FALSE, ETK_FALSE, 0);
   
   dialog->dv_btn_2 = etk_radio_button_new_with_label_from_widget("Fit to window", 
	 ETK_RADIO_BUTTON(dialog->dv_btn_1));
   etk_box_pack_start(ETK_BOX(vbox2), dialog->dv_btn_2, ETK_FALSE, ETK_FALSE, 0);

   frame = etk_frame_new("Slideshow");
   etk_box_pack_start(ETK_BOX(vbox), frame, ETK_FALSE, ETK_FALSE, 5);
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), hbox);

   label = etk_label_new("Interval"); 
   etk_box_pack_start(ETK_BOX(hbox), label, ETK_FALSE, ETK_FALSE, 0);
   
   dialog->slide_interval = etk_entry_new();
   etk_box_pack_start(ETK_BOX(hbox), dialog->slide_interval, ETK_FALSE, ETK_FALSE, 0);
   
   frame = etk_frame_new("Comments");
   etk_box_pack_start(ETK_BOX(vbox), frame, ETK_FALSE, ETK_FALSE, 5);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   dialog->comments_visible = etk_check_button_new_with_label("Visible");
   etk_box_pack_start(ETK_BOX(vbox2), dialog->comments_visible, ETK_FALSE, ETK_FALSE, 0);
   
   /* 
    * Start toggling/setting the correct values from loaded options 
    */
   if (e->options->default_view == EX_IMAGE_ONE_TO_ONE)
      etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->dv_btn_1));
   else if (e->options->default_view == EX_IMAGE_FIT_TO_WINDOW)
      etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->dv_btn_2));

   if (e->options->comments_visible == EX_DEFAULT_COMMENTS_VISIBLE)
     etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(dialog->comments_visible));


   sprintf(string, "%.2f", e->options->slide_interval);
   D(("Entry gets texts for slide_interval: %s\n", string));
   etk_entry_text_set(ETK_ENTRY(dialog->slide_interval), string);
   
   return vbox;
}

static Etk_Widget *
_ex_options_page_2_create()
{
   Etk_Widget *vbox, *vbox2;
   Etk_Widget *label;
   Etk_Widget *frame, *table;
   Ex_Options_Dialog *dialog = e->opt_dialog;
   char string[256];
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   
   frame = etk_frame_new("Effect thresh");
   etk_box_pack_start(ETK_BOX(vbox), frame, ETK_FALSE, ETK_FALSE, 5);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);

   table = etk_table_new(2, 3, ETK_FALSE);
   etk_box_pack_start(ETK_BOX(vbox2), table, ETK_FALSE, ETK_FALSE, 0);
   
   label = etk_label_new("Blur thresh"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 0, 0, 
	 ETK_FILL_POLICY_NONE);
   dialog->blur_thresh = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->blur_thresh, 1, 1, 0, 0, 0, 0, 
	 ETK_FILL_POLICY_NONE);
   
   label = etk_label_new("Sharpen thresh"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 0, 0, 
	 ETK_FILL_POLICY_NONE);
   dialog->sharpen_thresh = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->sharpen_thresh, 1, 1, 1, 1, 0, 0, 
	 ETK_FILL_POLICY_NONE);

   label = etk_label_new("Brighten thresh"); 
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 2, 2, 0, 0, 
	 ETK_FILL_POLICY_NONE);
   dialog->brighten_thresh = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), dialog->brighten_thresh, 1, 1, 2, 2, 0, 0, 
	 ETK_FILL_POLICY_NONE);

   frame = etk_frame_new("Rotate");
   etk_box_pack_start(ETK_BOX(vbox), frame, ETK_FALSE, ETK_FALSE, 5);
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   dialog->rotate_autosave = etk_check_button_new_with_label("Autosave after rotate");
   etk_box_pack_start(ETK_BOX(vbox2), dialog->rotate_autosave, ETK_FALSE, 
	 ETK_FALSE, 0);

   
   sprintf(string, "%.2f", e->options->blur_thresh);
   D(("Entry gets texts for blur tresh: %s\n", string));
   etk_entry_text_set(ETK_ENTRY(dialog->blur_thresh), string);
   
   sprintf(string, "%.2f", e->options->sharpen_thresh);
   D(("Entry gets texts for sharpen tresh: %s\n", string));
   etk_entry_text_set(ETK_ENTRY(dialog->sharpen_thresh), string);
   
   sprintf(string, "%.2f", e->options->brighten_thresh);
   D(("Entry gets texts for brighten tresh: %s\n", string));
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

   D(("Selected item %p, e->options->default_sort_tmp %d\n", item,
	    e->options->default_sort_tmp));
}

static Etk_Widget *
_ex_options_page_3_create()
{
   Etk_Widget *vbox, *hbox;
   Etk_Widget *frame, *label;
   Etk_Widget *image;
   Ex_Options_Dialog *dialog = e->opt_dialog;

   vbox = etk_vbox_new(ETK_FALSE, 3);
   
   frame = etk_frame_new("Thumb sorting");
   etk_box_pack_start(ETK_BOX(vbox), frame, ETK_FALSE, ETK_FALSE, 5);
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), hbox);

   label = etk_label_new("Default sort by"); 
   etk_box_pack_start(ETK_BOX(hbox), label, ETK_FALSE, 
	 ETK_FALSE, 0);
  
   e->options->default_sort_tmp = e->options->default_sort;

   dialog->default_sort = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(dialog->default_sort), ETK_COMBOBOX_IMAGE, 
	 24, ETK_FALSE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
   etk_combobox_column_add(ETK_COMBOBOX(dialog->default_sort), ETK_COMBOBOX_LABEL, 
	 75, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
   etk_combobox_build(ETK_COMBOBOX(dialog->default_sort));
   etk_box_pack_start(ETK_BOX(hbox), dialog->default_sort, ETK_FALSE, 
	 ETK_FALSE, 0);

   image = etk_image_new_from_stock(ETK_STOCK_OFFICE_CALENDAR, ETK_STOCK_SMALL);
   dialog->sort_date = etk_combobox_item_append(ETK_COMBOBOX(dialog->default_sort), image, "Date");
   image = etk_image_new_from_stock(ETK_STOCK_DRIVE_HARDDISK, ETK_STOCK_SMALL);
   dialog->sort_size = etk_combobox_item_append(ETK_COMBOBOX(dialog->default_sort), image, "Size");
   image = etk_image_new_from_stock(ETK_STOCK_TEXT_X_GENERIC, ETK_STOCK_SMALL);
   dialog->sort_name = etk_combobox_item_append(ETK_COMBOBOX(dialog->default_sort), image, "Name");
   image = etk_image_new_from_stock(ETK_STOCK_UTILITIES_SYSTEM_MONITOR, ETK_STOCK_SMALL);
   dialog->sort_resolution = etk_combobox_item_append(ETK_COMBOBOX(dialog->default_sort), image, "Resolution");
   
   etk_signal_connect("active_item_changed", ETK_OBJECT(dialog->default_sort), 
	 ETK_CALLBACK(_ex_options_combobox_active_item_changed_cb), NULL);

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
_ex_options_page_4_create()
{
   Etk_Widget *vbox;

   vbox = etk_vbox_new(ETK_FALSE, 3);

   return vbox;
}

void
_ex_options_window_show(Exhibit *e)
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
   etk_signal_connect("delete_event", ETK_OBJECT(win),
		      ETK_CALLBACK(_ex_options_window_delete_cb), win);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   notebook = etk_notebook_new();
   etk_box_pack_start(ETK_BOX(vbox), notebook, ETK_TRUE, ETK_TRUE, 0);
   
   page = _ex_options_page_1_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "General", page);
   page = _ex_options_page_2_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Effects", page);
   page = _ex_options_page_3_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Dir/thumb listing", page);
   page = _ex_options_page_4_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Exec commands", page);

   etk_box_pack_start(ETK_BOX(vbox), etk_hseparator_new(), 
		      ETK_FALSE, ETK_FALSE, 5);

   hbox = etk_hbox_new(ETK_FALSE, 3);
   etk_box_pack_start(ETK_BOX(vbox), hbox, ETK_FALSE, ETK_FALSE, 0);
   
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
   etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button),
			      ETK_CALLBACK(_ex_options_set_ok_cb), win);
   
   button = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   etk_button_label_set(ETK_BUTTON(button), "Apply");
   etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);
   etk_signal_connect("clicked", ETK_OBJECT(button),
		      ETK_CALLBACK(_ex_options_set_apply_cb), NULL);
   
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CANCEL);
   etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);
   etk_signal_connect_swapped("clicked", ETK_OBJECT(button),
			      ETK_CALLBACK(_ex_options_set_cancel_cb), win);

   etk_widget_show_all(ETK_WIDGET(win));
}

