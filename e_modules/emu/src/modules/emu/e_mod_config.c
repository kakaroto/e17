#include "e.h"
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

typedef struct _Cfg_File_Data Cfg_File_Data;

struct _E_Config_Dialog_Data
{
   int autofit;
   int follower;
   int iconsize;
   int allow_overlap;
   double follow_speed;
   double autoscroll_speed;
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
_config_emu_module(E_Container *con, Emu *emu)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   /* Dialog Methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   /* Create The Dialog */
   cfd = e_config_dialog_new(con, _("Emu Configuration"), NULL, 0, v, emu);
   emu->config_dialog = cfd;
}

static void 
_fill_data(Emu *emu, E_Config_Dialog_Data *cfdata)
{
   cfdata->autofit = (emu->conf->width == EMU_WIDTH_AUTO);
   cfdata->follower = emu->conf->follower;
   cfdata->iconsize = emu->conf->iconsize;   
   cfdata->allow_overlap = emu->conf->allow_overlap;
   cfdata->follow_speed = emu->conf->follow_speed;
   cfdata->autoscroll_speed = emu->conf->autoscroll_speed;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Emu *emu;
   
   emu = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(emu, cfdata);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Emu *emu;

   emu = cfd->data;
   emu->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *ob;
   Emu *emu;
   
   emu = cfd->data;
   _fill_data(emu, cfdata);

   o = e_widget_list_add(evas, 0, 0);
   ob = e_widget_check_add(evas, _("Show Follower"), &(cfdata->follower));
   e_widget_list_object_append(o, ob, 1, 1, 0.5);
   ob = e_widget_check_add(evas, _("Auto Fit Icons"), &(cfdata->autofit));
   e_widget_list_object_append(o, ob, 1, 1, 0.5);

   ob = e_widget_check_add(evas, _("Allow Overlap"), &(cfdata->allow_overlap));
   e_widget_list_object_append(o, ob, 1, 1, 0.5);
   return o;
}

static int 
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Emu *emu;
   
   emu = cfd->data;
   e_border_button_bindings_ungrab_all();
   if ((cfdata->follower) && (!emu->conf->follower)) 
     {
	emu->conf->follower = 1;
     }
   else if (!(cfdata->follower) && (emu->conf->follower)) 
     {
	emu->conf->follower = 0;
     }

   if (cfdata->allow_overlap && !emu->conf->allow_overlap)
     emu->conf->allow_overlap = 1;
   else if (!cfdata->allow_overlap && emu->conf->allow_overlap)
     emu->conf->allow_overlap = 0;
 
   if ((cfdata->autofit) && (emu->conf->width == EMU_WIDTH_FIXED)) 
     {
	emu->conf->width = EMU_WIDTH_AUTO;
     }
   else if (!(cfdata->autofit) && (emu->conf->width == EMU_WIDTH_AUTO)) 
     {
	emu->conf->width = EMU_WIDTH_FIXED;
     }
   e_border_button_bindings_grab_all();
   e_config_save_queue();

   _emu_cb_config_updated(emu);
   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   Emu *emu;
   
   emu = cfd->data;
   _fill_data(emu, cfdata);

   o = e_widget_list_add(evas, 0, 0);
   
   of = e_widget_framelist_add(evas, _("Follower"), 0);
   ob = e_widget_check_add(evas, _("Visible"), &(cfdata->follower));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, _("Follow Speed"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.2f px/s"), 0.0, 1.0, 0.01, 0,  &(cfdata->follow_speed), NULL,200);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("Icon Size"), 0);
   ob = e_widget_slider_add(evas, 1, 0, _("%3.0f pixels"), 8.0, 128.0, 1.0, 0,  NULL, &(cfdata->iconsize), 200);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
      
   of = e_widget_framelist_add(evas, _("Width"), 0);
   ob = e_widget_check_add(evas, _("Auto Fit"), &(cfdata->autofit));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, _("Autoscroll Speed:"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.2f px/s"), 0.0, 1.0, 0.01, 0,  &(cfdata->autoscroll_speed), NULL,200);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   /* allow overlap checkbox */
   of = e_widget_framelist_add(evas, _("Extras"), 0);
   ob = e_widget_check_add(evas, _("Allow Overlap"), &(cfdata->allow_overlap));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int 
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Emu *emu;
   
   emu = cfd->data;
   e_border_button_bindings_ungrab_all();
   if ((cfdata->follower) && (!emu->conf->follower)) 
     {
	emu->conf->follower = 1;
     }
   else if (!(cfdata->follower) && (emu->conf->follower)) 
     {
	emu->conf->follower = 0;
     }

   /* allow overlap check box */
   if (cfdata->allow_overlap && !emu->conf->allow_overlap)
     emu->conf->allow_overlap = 1;
   else if (!cfdata->allow_overlap && emu->conf->allow_overlap)
     emu->conf->allow_overlap = 0;
   
   if ((cfdata->autofit) && (emu->conf->width == EMU_WIDTH_FIXED)) 
     {
	emu->conf->width = EMU_WIDTH_AUTO;
     }
   else if (!(cfdata->autofit) && (emu->conf->width == EMU_WIDTH_AUTO)) 
     {
	emu->conf->width = EMU_WIDTH_FIXED;
     }

   if (cfdata->iconsize != emu->conf->iconsize) 
     {
	emu->conf->iconsize = cfdata->iconsize;
     }
   if (cfdata->follow_speed != emu->conf->follow_speed) 
     {	
	emu->conf->follow_speed = cfdata->follow_speed;
     }
   if (cfdata->autoscroll_speed != emu->conf->autoscroll_speed) 
     {
	emu->conf->autoscroll_speed = cfdata->autoscroll_speed;
     }
   
   e_border_button_bindings_grab_all();
   e_config_save_queue();

   _emu_cb_config_updated(emu);
   return 1;
}

