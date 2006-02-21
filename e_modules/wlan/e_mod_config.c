#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

struct _E_Config_Dialog_Data
{
   char *device;
   int check_interval;
   
   Ecore_List *devs;
   int dev_num;
};

/* Protos */
static void        *_create_data            (E_Config_Dialog *cfd);
static void         _free_data              (E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets   (E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _basic_apply_data       (E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void         _fill_data              (Wlan_Face *nf, E_Config_Dialog_Data *cfdata);
static void         _wlan_config_get_devices (Ecore_List *devs);

/* Config Calls */
void
_configure_wlan_module(Wlan_Face *nf)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   
   cfd = e_config_dialog_new(nf->con, _("Wlan Configuration"), NULL, 0, v, nf);
   nf->wlan->cfd = cfd;
}

static void
_fill_data(Wlan_Face *nf, E_Config_Dialog_Data *cfdata)
{
   char *tmp;
   int i;

   cfdata->check_interval = nf->conf->check_interval;
   
   if (nf->conf->device != NULL)
     cfdata->device = strdup(nf->conf->device);
   else
     cfdata->device = NULL;
   
   if (!cfdata->device)
     return;

   cfdata->devs = ecore_list_new();
   _wlan_config_get_devices(cfdata->devs);

   if (!cfdata->devs)
     return;
   
   i = 0;
   ecore_list_goto_first(cfdata->devs);
   while ((tmp = ecore_list_next(cfdata->devs)) != NULL) 
     {
	if (!strcmp(cfdata->device, tmp)) 
	  {
	     cfdata->dev_num = i;
	     break;
	  }
	i++;
     }   
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Wlan_Face *nf;

   nf = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(nf, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Wlan_Face *nf;

   nf = cfd->data;
   nf->wlan->cfd = NULL;
   E_FREE(cfdata->device);
   if (cfdata->devs)
     ecore_list_destroy(cfdata->devs);
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *ot;
   E_Radio_Group *rg;
   char *tmp;
   int i;
   
   o = e_widget_list_add(evas, 0, 0);      
   of = e_widget_framelist_add(evas, _("Device Settings"), 0);
   ot = e_widget_table_add(evas, 0);   
   rg = e_widget_radio_group_new(&(cfdata->dev_num));
   i = 0;
   ecore_list_goto_first(cfdata->devs);
   while ((tmp = ecore_list_next(cfdata->devs)) != NULL) 
     {
	ob = e_widget_radio_add(evas, tmp, i, rg);
	e_widget_table_object_append (ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
	i++;
     }
   i++;
   ob = e_widget_label_add(evas, _("Check Interval:"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1, 60, 1, 0, NULL, &(cfdata->check_interval), 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   char *tmp;
   Wlan_Face *nf;

   nf = cfd->data;
   tmp = ecore_list_goto_index(cfdata->devs, cfdata->dev_num);
   if (tmp != NULL)
     nf->conf->device = (char *)evas_stringshare_add(strdup(tmp));
   nf->conf->check_interval = cfdata->check_interval;
   e_config_save_queue ();

   if (nf->monitor)
     ecore_timer_interval_set(nf->monitor, (double)cfdata->check_interval);
   
   return 1;
}

static void
_wlan_config_get_devices(Ecore_List *devs) 
{
   FILE *stat;
   char iface[64];
   char buf[256];
   unsigned long dummy;
   int wlan_status = 0;
   int wlan_link = 0;
   int wlan_level = 0;
   int wlan_noise = 0;
   
   stat = fopen("/proc/net/wireless", "r");
   if (!stat)
     return;

   while (fgets(buf, 256, stat)) 
     {
	int i = 0;
        for (; buf[i] != 0; i++)
	  if (buf[i] == ':' || buf[i] == '.')
	    buf[i] = ' ';

        if (sscanf(buf, "%s %u %u %u %u %u %u %u %u %u %u",
                   iface, &wlan_status, &wlan_link, &wlan_level,
                   &wlan_noise, &dummy, &dummy, &dummy, &dummy,
                   &dummy, &dummy) < 11)
	  continue;
	ecore_list_append(devs, strdup(iface));	
     }
   fclose(stat);
}
