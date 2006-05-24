#include <e.h>
#include <Ecore.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   char *device;
   double poll_time;

   Ecore_List *devs;
   int dev_num;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata);
static void _net_config_get_devices(Ecore_List *devs);

/* Config Calls */
void
_config_net_module(Config_Item *ci)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, D_("Net Configuration"), NULL, 0, v, ci);
   net_config->config_dialog = cfd;
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   char *tmp;
   int i;

   cfdata->poll_time = ci->poll_time;

   if (ci->device != NULL)
      cfdata->device = strdup(ci->device);
   else
      cfdata->device = NULL;

   if (!cfdata->device)
      return;

   cfdata->devs = ecore_list_new();
   _net_config_get_devices(cfdata->devs);

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
   Config_Item *ci;

   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (!net_config)
     return;
   net_config->config_dialog = NULL;
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
   of = e_widget_framelist_add(evas, D_("Device Settings"), 0);
   ot = e_widget_table_add(evas, 0);
   rg = e_widget_radio_group_new(&(cfdata->dev_num));
   i = 0;
   ecore_list_goto_first(cfdata->devs);
   while ((tmp = ecore_list_next(cfdata->devs)) != NULL)
     {
        ob = e_widget_radio_add(evas, tmp, i, rg);
        e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
        i++;
     }
   i++;
   ob = e_widget_label_add(evas, D_("Check Interval:"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1, 60, 1, 0, &(cfdata->poll_time), NULL, 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   char *tmp;
   Config_Item *ci;
   
   ci = cfd->data;
   tmp = ecore_list_goto_index(cfdata->devs, cfdata->dev_num);
   if (tmp != NULL)
     {
	evas_stringshare_del(ci->device);
	ci->device = evas_stringshare_add(tmp);
     }

   ci->poll_time = cfdata->poll_time;
   e_config_save_queue();
   _net_config_updated(ci->id);
   return 1;
}

static void
_net_config_get_devices(Ecore_List *devs)
{
   FILE *stat;
   char dev[64];
   char buf[256];
   unsigned long dummy;

   stat = fopen("/proc/net/dev", "r");
   if (!stat)
      return;

   while (fgets(buf, 256, stat))
     {
        int i = 0;

        for (; buf[i] != 0; i++)
          {
             if (buf[i] == ':')
                buf[i] = ' ';
          }
        if (sscanf(buf, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu "
                   "%lu %lu %lu %lu\n", dev, &dummy, &dummy, &dummy,
                   &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy) < 16)
           continue;
        ecore_list_append(devs, strdup(dev));
     }
   fclose(stat);
}
