#include "e.h"
#include "e_mod_main.h"
#include "config.h"

typedef struct _cfdata CFData;
typedef struct _Cfg_File_Data Cfg_File_Data;

struct _cfdata 
{
   int palette;
};

struct _Cfg_File_Data 
{
   E_Config_Dialog *cfd;
   char *file;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, CFData *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, CFData *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, CFData *cfdata);

void
_config_flame_module(E_Container *con, Flame *fl) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View v;
   
   v.create_cfdata = _create_data;
   v.free_cfdata = _free_data;
   v.basic.apply_cfdata = _basic_apply_data;
   v.basic.create_widgets = _basic_create_widgets;
   v.advanced.apply_cfdata = NULL;
   v.advanced.create_widgets = NULL;
   
   cfd = e_config_dialog_new(con, _("Flame Module"), NULL, 0, &v, f);
}

static void 
_fill_data(Flame *f, CFData *cfdata) 
{
   cfdata->palette = f->conf->palette_type;
}

static void
*_create_data(E_Config_Dialog *cfd) 
{
   CFData *cfdata;
   Flame *f;
   
   f = cfd->data;
   cfdata = E_NEW(CFData, 1);
   _fill_data(f, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, CFData *cfdata) 
{
   free(cfdata);
}

static Evas_Object
*_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, CFData *cfdata) 
{
   Evas_Object *o, *of, *ob;
   E_Radio_Group *rg;
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("Flame Color"), 0);
   rg = e_widget_radio_group_new(&(cfdata->palette));
   ob = e_widget_radio_add(evas, _("Gold"), GOLD_PALETTE ,rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Fire"), FIRE_PALETTE ,rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Plasma"), PLASMA_PALETTE ,rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, CFData *cfdata) 
{
   Flame *f;
   
   f = cfd->data;
   e_border_button_bindings_ungrab_all();
   switch (cfdata->palette) 
     {
      case 1:
	f->conf->palette_type = GOLD_PALETTE; // Gold
	break;
      case 2:
	f->conf->palette_type = FIRE_PALETTE;	//Fre
	break;
      case 3:
	f->conf->palette_type = PLASMA_PALETTE;	//Plasma
	break;
      default:
	break;
     }
   e_config_save_queue();
   e_border_button_bindings_grab_all();
   
   _flame_cb_config_updated(f);
   return 1;
}
