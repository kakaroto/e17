#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

typedef struct _Cfg_File_Data Cfg_File_Data;

struct _E_Config_Dialog_Data 
{
   int iconsize;
   int orientation;
   int show_labels;
};

struct _Cfg_File_Data 
{
   E_Config_Dialog *cfd;
   char *file;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata); 
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata); 
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

void 
_config_mount_module(E_Container *con, Mount *m) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   
   v = E_NEW(E_Config_Dialog_View, 1);
   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;
   
   cfd = e_config_dialog_new(con, D_("Mount Configuration"), NULL, 0, v, m);
   m->config_dialog = cfd;
}

static void 
_fill_data(Mount *m, E_Config_Dialog_Data *cfdata) 
{
   cfdata->iconsize = m->conf->icon_size;
   cfdata->orientation = m->conf->orientation;
   cfdata->show_labels = m->conf->show_labels;
}

static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata;
   Mount *m;
   
   m = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   Mount *m;
   
   m = cfd->data;
   m->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *ob, *of;
   E_Radio_Group *rg;
   Mount *m;
   
   m = cfd->data;
   _fill_data(m, cfdata);
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("General Settings"), 0);
   ob = e_widget_check_add(evas, D_("Show Labels"), &(cfdata->show_labels));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Layout Settings"), 0);   
   rg = e_widget_radio_group_new(&(cfdata->orientation));
   ob = e_widget_radio_add(evas, D_("Use Vertical Layout"), MOUNT_ORIENT_VERT, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Use Horizontal Layout"), MOUNT_ORIENT_HORIZ, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   return o;
}

static int 
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   Mount *m;
   
   m = cfd->data;
   e_border_button_bindings_ungrab_all();
   m->conf->show_labels = cfdata->show_labels;
   m->conf->orientation = cfdata->orientation;
   e_config_save_queue();
   e_border_button_bindings_grab_all();
   
   _mount_cb_config_updated(m);
   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *ob, *of;
   E_Radio_Group *rg;
   Mount *m;
   
   m = cfd->data;
   _fill_data(m, cfdata);
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("General Settings"), 0);
   ob = e_widget_check_add(evas, D_("Show Labels"), &(cfdata->show_labels));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Layout Settings"), 0);   
   rg = e_widget_radio_group_new(&(cfdata->orientation));
   ob = e_widget_radio_add(evas, D_("Use Vertical Layout"), MOUNT_ORIENT_VERT, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Use Horizontal Layout"), MOUNT_ORIENT_HORIZ, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   of = e_widget_framelist_add(evas, D_("Icon Size"), 0);   
   ob = e_widget_slider_add(evas, 1, 0, _("%3.0f pixels"), 8.0, 128.0, 1.0, 0, NULL, &(cfdata->iconsize), 200);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);   
   return o;
}

static int 
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   Mount *m;
   
   m = cfd->data;
   e_border_button_bindings_ungrab_all();
   m->conf->show_labels = cfdata->show_labels;
   m->conf->orientation = cfdata->orientation;
   m->conf->icon_size = cfdata->iconsize;
   e_config_save_queue();
   e_border_button_bindings_grab_all();
   
   _mount_cb_config_updated(m);
   return 1;
}
