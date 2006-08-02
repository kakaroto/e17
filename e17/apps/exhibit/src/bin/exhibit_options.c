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

static Eet_Data_Descriptor *_ex_config_options_edd = NULL;
static Eet_Data_Descriptor *_ex_config_version_edd = NULL;


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
   CFG_OPTIONS_NEWI("dv", default_view, EET_T_INT);
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
   e->options->comments_visible = EX_DEFAULT_COMMENTS_VISIBLE;
   e->options->default_view     = EX_IMAGE_ONE_TO_ONE;
   e->options->default_sort     = 0; /* TODO: enumerate sort types */
   e->options->last_w           = EX_DEFAULT_WINDOW_WIDTH;
   e->options->last_h           = EX_DEFAULT_WINDOW_HEIGHT;   
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
   
   
   eet_close(ef);
   return 1;
}

void
_ex_options_window_show(Exhibit *e)
{
   Etk_Widget *win;
   Etk_Widget *vbox, *vbox2;
   Etk_Widget *btn, *btn2;
   Etk_Widget *frame;
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Exhibit options");
   etk_window_resize(ETK_WINDOW(win), 200, 150);
   etk_container_border_width_set(ETK_CONTAINER(win), 10);
   etk_signal_connect("delete_event", ETK_OBJECT(win),
		      ETK_CALLBACK(_ex_options_window_delete_cb), win);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   frame = etk_frame_new("Choose standard view");
   etk_box_pack_start(ETK_BOX(vbox), frame, ETK_TRUE, ETK_FALSE, 0);
   
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   btn = etk_radio_button_new_with_label("Zoom 1:1", NULL);
   etk_box_pack_start(ETK_BOX(vbox2), btn, ETK_TRUE, ETK_FALSE, 0);
   
   btn2 = etk_radio_button_new_with_label_from_widget("Fit to window", 
						      ETK_RADIO_BUTTON(btn));
   etk_box_pack_start(ETK_BOX(vbox2), btn2, ETK_TRUE, ETK_FALSE, 0);
      
   etk_widget_show_all(ETK_WIDGET(win));
}

Etk_Bool 
_ex_options_window_delete_cb(void *data)
{
   etk_object_destroy(ETK_OBJECT(data));
  return ETK_TRUE;
}
