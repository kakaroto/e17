#include "e.h"
#include "e_mod_main.h"
#include "config.h"

typedef struct _cfdata CFData;
typedef struct _Cfg_File_Data Cfg_File_Data;

#define DENSITY_SPRINKLE 0
#define DENSITY_DRIZZLE 1
#define DENSITY_DOWNPOUR 2

struct _cfdata
{
   /* Basic */
   int show_clouds;
   int density;
};

struct _Cfg_File_Data
{
   E_Config_Dialog *cfd;
   char *file;
};

/* Protos */
static Evas_Object *_create_widgets(E_Config_Dialog *cfd, Evas *evas, Config *cfdata);
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, CFData *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, CFData *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, CFData *cfdata);

Rain *rn = NULL;

void
  e_int_config_rain(E_Container *con, Rain *r)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View v;

   rn = r;
   v.create_cfdata = _create_data;
   v.free_cfdata = _free_data;
   v.basic.apply_cfdata = _basic_apply_data;
   v.basic.create_widgets = _basic_create_widgets;
   v.advanced.apply_cfdata = NULL;
   v.advanced.create_widgets = NULL;

   cfd = e_config_dialog_new(con, _("Rain Module"), NULL, 0, &v, r);
}

static void
  _fill_data(CFData *cfdata)
{
   cfdata->show_clouds = rn->conf->show_clouds;
   switch (rn->conf->cloud_count)
     {
      case 5:
	cfdata->density = DENSITY_SPRINKLE;
	break;
      case 10:
	cfdata->density = DENSITY_DRIZZLE;
	break;
      case 20:
	cfdata->density = DENSITY_DOWNPOUR;
	break;
      default:
	break;
     }
}

static void
  *_create_data(E_Config_Dialog *cfd)
{
   CFData *cfdata;

   cfdata = E_NEW(CFData, 1);
   _fill_data(cfdata);
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

   of = e_widget_framelist_add(evas, _("General Settings"), 0);
   ob = e_widget_check_add(evas, _("Show Clouds"), &(cfdata->show_clouds));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("Rain Density"), 0);
   rg = e_widget_radio_group_new(&(cfdata->density));
   ob = e_widget_radio_add(evas, _("Sprinkle"), DENSITY_SPRINKLE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Drizzle"), DENSITY_DRIZZLE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Downpour"), DENSITY_DOWNPOUR, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
  _basic_apply_data(E_Config_Dialog *cfd, CFData *cfdata)
{
   e_border_button_bindings_ungrab_all();
   switch (cfdata->density)
     {
      case 0:
	rn->conf->cloud_count = 5;
	rn->conf->drop_count = 20;
	break;
      case 1:
	rn->conf->cloud_count = 10;
	rn->conf->drop_count = 60;
	break;
      case 2:
	rn->conf->cloud_count = 20;
	rn->conf->drop_count = 150;
	break;
      default:
	break;
     }
   rn->conf->show_clouds = cfdata->show_clouds;

   e_config_save_queue();
   e_border_button_bindings_grab_all();

   _rain_cb_config_updated(rn);
   return 1;
}
