#include <e.h>
#include <Ecore.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   int method;
   int use_import;
   int use_scrot;
   int prompt;
   double delay_time;
   char *location;
   char *filename;
   struct
   {
      int use_img_border;
      int use_dither;
      int use_frame;
      int use_window;
      int use_silent;
      int use_trim;
   } import;
   struct
   {
      int use_img_border;
      int use_thumb;
   } scrot;
   int use_app;
   char *app;
   
   Evas_Object *file_entry;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _prompt_cb_change(void *data, Evas_Object *obj);

/* Config Calls */
void
_config_screenshot_module(Config_Item *ci)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;
   
   v = E_NEW(E_Config_Dialog_View, 1);

   /* methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   /* create config diaolg */
   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, D_("Screenshot Configuration"), NULL, 0, v, ci);
   ss_config->config_dialog = cfd;
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   cfdata->prompt = ci->prompt;
   
   if (ci->use_import == 1)
     cfdata->method = 0;
   else if (ci->use_scrot == 1)
     cfdata->method = 1;

   cfdata->delay_time = ci->delay_time;
   
   cfdata->location = NULL;
   if (ci->location != NULL)
     cfdata->location = strdup(ci->location);

   cfdata->filename = NULL;   
   if (ci->filename != NULL)
     cfdata->filename = strdup(ci->filename);

   cfdata->use_app = ci->use_app;
   cfdata->app = NULL;
   if (ci->app != NULL)
     cfdata->app = strdup(ci->app);
   
   cfdata->import.use_img_border = ci->import.use_img_border;
   cfdata->import.use_dither = ci->import.use_dither;
   cfdata->import.use_frame = ci->import.use_frame;
   cfdata->import.use_window = ci->import.use_window;
   cfdata->import.use_silent = ci->import.use_silent;
   cfdata->import.use_trim = ci->import.use_trim;
   cfdata->scrot.use_img_border = ci->scrot.use_img_border;
   cfdata->scrot.use_thumb = ci->scrot.use_thumb;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;

   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (!ss_config) return;
   ss_config->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *ot;
   E_Radio_Group *rg;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_frametable_add(evas, D_("General Settings"), 1);

   if ((ecore_file_app_installed("import")) && 
       (ecore_file_app_installed("scrot")))
     {
	rg = e_widget_radio_group_new(&(cfdata->method));	
	ob = e_widget_radio_add(evas, D_("Use Import"), 0, rg);
	e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0); 
	ob = e_widget_radio_add(evas, D_("Use Scrot"), 1, rg);
	e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 0, 0, 1, 0);
     }   
   else if (ecore_file_app_installed("import")) 
     cfdata->method = 0;
   else if (ecore_file_app_installed("scrot"))
     cfdata->method = 1;

   ob = e_widget_label_add(evas, D_("Delay Time:"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);
   ob = e_widget_slider_add(evas, 1, 0, D_("%1.0f seconds"), 0.0, 60.0, 1.0, 0, &(cfdata->delay_time), NULL, 100);
   e_widget_frametable_object_append(of, ob, 1, 1, 2, 1, 1, 0, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
      
   of = e_widget_frametable_add(evas, D_("File Settings"), 1);
   ob = e_widget_check_add(evas, D_("Always Prompt For Filename"), &(cfdata->prompt));
   e_widget_on_change_hook_set(ob, _prompt_cb_change, cfdata);
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0);
   ob = e_widget_label_add(evas, D_("Save Directory:"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->location);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, D_("Filename:"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->filename);
   cfdata->file_entry = ob;
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 0, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   char tmp[4096];
   int length;
   Config_Item *ci;

   ci = cfd->data;
   
   ci->prompt = cfdata->prompt;
   
   if (cfdata->method == 0)
     {
        ci->use_import = 1;
        ci->use_scrot = 0;
     }
   else
     {
        ci->use_scrot = 1;
        ci->use_import = 0;
     }

   if (ci->location)
     evas_stringshare_del(ci->location);
   if (cfdata->location == NULL)
     ci->location = evas_stringshare_add(e_user_homedir_get());
   else
     {
        snprintf(tmp, sizeof(tmp), "%s", strdup(cfdata->location));
        if (tmp[(length = strlen(tmp) - 1)] == '/')
	  tmp[length] = '\0';
        ci->location = evas_stringshare_add(tmp);
     }

   if (ci->filename)
     evas_stringshare_del(ci->filename);
   if (cfdata->filename != NULL) 
     ci->filename = evas_stringshare_add(cfdata->filename); 

   ci->delay_time = cfdata->delay_time;
   e_config_save_queue();

   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *ob, *of, *ot;
   E_Radio_Group *rg;
   Config_Item *ci;
   
   ci = cfd->data;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_frametable_add(evas, D_("General Settings"), 1);

   if ((ecore_file_app_installed("import")) && 
       (ecore_file_app_installed("scrot")))
     {
	rg = e_widget_radio_group_new(&(cfdata->method));	
	ob = e_widget_radio_add(evas, D_("Use Import"), 0, rg);
	e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0); 
	ob = e_widget_radio_add(evas, D_("Use Scrot"), 1, rg);
	e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 0, 0, 1, 0);
     }   
   else if (ecore_file_app_installed("import")) 
     cfdata->method = 0;
   else if (ecore_file_app_installed("scrot"))
     cfdata->method = 1;

   ob = e_widget_label_add(evas, D_("Delay Time:"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);
   ob = e_widget_slider_add(evas, 1, 0, D_("%1.0f seconds"), 0.0, 60.0, 1.0, 0, &(cfdata->delay_time), NULL, 100);
   e_widget_frametable_object_append(of, ob, 1, 1, 2, 1, 1, 0, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_frametable_add(evas, D_("Image Viewer Settings"), 1);
   ob = e_widget_check_add(evas, D_("Launch Image Viewer After Screenshot"), &(cfdata->use_app));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 0, 1, 0);
   ob = e_widget_entry_add(evas, &(cfdata->app));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 0, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   of = e_widget_frametable_add(evas, D_("File Settings"), 1);
   ob = e_widget_check_add(evas, D_("Always Prompt For Filename"), &(cfdata->prompt));
   e_widget_on_change_hook_set(ob, _prompt_cb_change, cfdata);
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0);
   ob = e_widget_label_add(evas, D_("Save Directory:"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->location);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, D_("Filename:"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->filename);
   cfdata->file_entry = ob;
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 0, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   ot = e_widget_table_add(evas, 0);
   if (ecore_file_app_installed("import")) 
     {
	of = e_widget_frametable_add(evas, D_("Import Settings"), 0);
	ob = e_widget_check_add(evas, D_("Include Image Border"), &(cfdata->import.use_img_border));
	e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0);
	ob = e_widget_check_add(evas, D_("Use Image Dithering"), &(cfdata->import.use_dither));
	e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);
	ob = e_widget_check_add(evas, D_("Include Window Manager Frame"), &(cfdata->import.use_frame));
	e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 0, 0, 1, 0);
	ob = e_widget_check_add(evas, D_("Choose Window To Grab"), &(cfdata->import.use_window));
	e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 0, 0, 1, 0);
	ob = e_widget_check_add(evas, D_("Silent"), &(cfdata->import.use_silent));
	e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 0, 0, 1, 0);
	ob = e_widget_check_add(evas, D_("Trim Edges"), &(cfdata->import.use_trim));
	e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 0, 0, 1, 0);
	e_widget_table_object_append(ot, of, 0, 0, 1, 1, 1, 1, 1, 1);
     }
   if (ecore_file_app_installed("scrot")) 
     {
	of = e_widget_frametable_add(evas, D_("Scrot Settings"), 0);
	ob = e_widget_check_add(evas, D_("Include Image Border"), &(cfdata->scrot.use_img_border));
	e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0);
	ob = e_widget_check_add(evas, D_("Generate Thumbnail"), &(cfdata->scrot.use_thumb));
	e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);
	e_widget_table_object_append(ot, of, 1, 0, 1, 1, 1, 1, 1, 1);
     }
   e_widget_list_object_append(o, ot, 1, 1, 0.5);   
   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;
   
   ci = cfd->data;
   _basic_apply_data(cfd, cfdata);

   ci->use_app = cfdata->use_app;
   if (ci->app)
     evas_stringshare_del(ci->app);
   if (cfdata->app != NULL)
     ci->app = evas_stringshare_add(cfdata->app);
   else
     ci->app = evas_stringshare_add("");
   
   ci->import.use_img_border = cfdata->import.use_img_border;
   ci->import.use_dither = cfdata->import.use_dither;
   ci->import.use_frame = cfdata->import.use_frame;
   ci->import.use_window = cfdata->import.use_window;
   ci->import.use_silent = cfdata->import.use_silent;
   ci->import.use_trim = cfdata->import.use_trim;
   ci->scrot.use_img_border = cfdata->scrot.use_img_border;
   ci->scrot.use_thumb = cfdata->scrot.use_thumb;

   e_config_save_queue();
   return 1;
}

static void 
_prompt_cb_change(void *data, Evas_Object *obj) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   if (!cfdata) return;
   
   if (!cfdata->prompt) 
     {
	e_widget_disabled_set(cfdata->file_entry, 0);
	if (cfdata->filename)
	  e_widget_entry_text_set(cfdata->file_entry, cfdata->filename);
     }
   else 
     {
	e_widget_disabled_set(cfdata->file_entry, 1);
	e_widget_entry_text_set(cfdata->file_entry, "");
	cfdata->filename = NULL;
     }
}
