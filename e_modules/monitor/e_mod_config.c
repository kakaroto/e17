#include "e.h"
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

typedef struct _cfdata CFData;
typedef struct _Cfg_File_Data Cfg_File_Data;

int net_interface_count;
int net_interface_number;
Ecore_List *net_interfaces = NULL;

int wlan_interface_count;
int wlan_interface_number;
Ecore_List *wlan_interfaces = NULL;

struct _cfdata 
{
   double cpu_interval;
   double mem_interval;
   double net_interval;
   double wlan_interval;
   
   char *net_interface;
   char *wlan_interface;
   
   int mem_real_ignore_cached;
   int mem_real_ignore_buffers;
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
_config_monitor_module(E_Container *con, Monitor_Face *f) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View v;
   
   v.create_cfdata = _create_data;
   v.free_cfdata = _free_data;
   v.basic.apply_cfdata = _basic_apply_data;
   v.basic.create_widgets = _basic_create_widgets;
   v.advanced.apply_cfdata = NULL;
   v.advanced.create_widgets = NULL;
   
   cfd = e_config_dialog_new(con, _("Monitor Configuration"), NULL, 0, &v, f);
}

static void 
_fill_data(Monitor_Face *f, CFData *cfdata) 
{
   char *tmp;
   int i = 0;
   
   cfdata->cpu_interval = f->conf->cpu_interval;
   cfdata->mem_interval = f->conf->mem_interval;
   cfdata->net_interval = f->conf->net_interval;
   cfdata->wlan_interval = f->conf->wlan_interval;
   cfdata->net_interface = f->conf->net_interface;
   cfdata->wlan_interface = f->conf->wlan_interface;
   cfdata->mem_real_ignore_cached = f->conf->mem_real_ignore_cached;
   cfdata->mem_real_ignore_buffers = f->conf->mem_real_ignore_buffers;
   cfdata->net_interface = f->conf->net_interface;
   cfdata->wlan_interface = f->conf->wlan_interface;
   
   net_interfaces = ecore_list_new();
   net_interface_count = net_interfaces_get(net_interfaces);
   ecore_list_goto_first(net_interfaces);
   while ((tmp = ecore_list_next(net_interfaces)) != NULL) 
     {
	if (!strcmp(cfdata->net_interface, tmp)) 
	  {
	     net_interface_number = i;
	     break;
	  }
	i = i + 1;	
     }
   i = 0;
   wlan_interfaces = ecore_list_new();
   wlan_interface_count = wlan_interfaces_get(wlan_interfaces);
   ecore_list_goto_first(wlan_interfaces);
   while ((tmp = ecore_list_next(wlan_interfaces)) != NULL) 
     {
	if (!strcmp(cfdata->wlan_interface, tmp)) 
	  {
	     wlan_interface_number = i;
	     break;
	  }
	i = i + 1;	
     }
}

static void
*_create_data(E_Config_Dialog *cfd) 
{
   CFData *cfdata;
   Monitor_Face *f;
   
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
   Evas_Object *o, *of, *ob, *ot;
   E_Radio_Group *rg;
   char *tmp;
   int i;
   Monitor_Face *f;
   
   f = cfd->data;
   _fill_data(f, cfdata);
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("Cpu"), 0);
   ob = e_widget_label_add(evas, _("Check Interval"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1.0, 60.0, 1.0, 0, &(cfdata->cpu_interval), NULL, 150);
   e_widget_framelist_object_append(of, ob);   
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("Memory"), 0);
   ot = e_widget_table_add(evas, 0);
   ob = e_widget_check_add(evas, _("Ignore Cached"), &(cfdata->mem_real_ignore_cached));
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 1, 0, 1, 0);
   ob = e_widget_check_add(evas, _("Ignore Buffers"), &(cfdata->mem_real_ignore_buffers));
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, _("Check Interval"));
   e_widget_table_object_append(ot, ob, 0, 2, 1, 1, 1, 0, 1, 0);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1.0, 60.0, 1.0, 0, &(cfdata->mem_interval), NULL, 150);
   e_widget_table_object_append(ot, ob, 0, 3, 1, 1, 1, 0, 1, 0);
   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   i = 0;
   if (wlan_interface_count > 0) 
     {
	of = e_widget_framelist_add(evas, _("Wireless"), 0);
	ot = e_widget_table_add(evas, 0);
	rg = e_widget_radio_group_new(&(wlan_interface_number));
	ecore_list_goto_first(wlan_interfaces);
	while ((tmp = ecore_list_next(wlan_interfaces)) != NULL) 
	  {
	     ob = e_widget_radio_add(evas, strdup(tmp), i, rg);
	     e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
	     i = i + 1;
	  }
	i = i + 1;
	ob = e_widget_label_add(evas, _("Check Interval"));
	e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
	i = i + 1;
	ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1.0, 60.0, 1.0, 0, &(cfdata->wlan_interval), NULL, 150);
	e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);	
	e_widget_framelist_object_append(of, ot);	
	e_widget_list_object_append(o, of, 1, 1, 0.5);
     }
   
   i = 0;   
   if (net_interface_count > 0) 
     {	
	of = e_widget_framelist_add(evas, _("Network"), 0);
	ot = e_widget_table_add(evas, 0);	
	rg = e_widget_radio_group_new(&(net_interface_number));
	ecore_list_goto_first(net_interfaces);
	while ((tmp = ecore_list_next(net_interfaces)) != NULL) 
	  {
	     ob = e_widget_radio_add(evas, strdup(tmp), i, rg);
	     e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
	     i = i + 1;
	  }
	i = i + 1;
	ob = e_widget_label_add(evas, _("Check Interval"));
	e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
	i = i + 1;
	ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1.0, 60.0, 1.0, 0, &(cfdata->net_interval), NULL, 150);
	e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
	e_widget_framelist_object_append(of, ot);	
	e_widget_list_object_append(o, of, 1, 1, 0.5);
     }
   
   return o;
}

static int 
_basic_apply_data(E_Config_Dialog *cfd, CFData *cfdata) 
{
   Monitor_Face *f;
   char *tmp;
   int i;
   
   f = cfd->data;
   e_border_button_bindings_ungrab_all();
   
   f->conf->cpu_interval = cfdata->cpu_interval;
   f->conf->mem_interval = cfdata->mem_interval;
   f->conf->wlan_interval = cfdata->wlan_interval;
   f->conf->net_interval = cfdata->net_interval;
   f->conf->mem_real_ignore_cached = cfdata->mem_real_ignore_cached;
   f->conf->mem_real_ignore_buffers = cfdata->mem_real_ignore_buffers;
   
   ecore_list_goto_first(net_interfaces);
   while ((tmp = ecore_list_next(net_interfaces)) != NULL) 
     {
	if (i == net_interface_number) 
	  {
	     f->conf->net_interface = (char *)evas_stringshare_add(tmp);
	     break;
	  }
	i = i + 1;
     }
   i = 0;
   ecore_list_goto_first(wlan_interfaces);
   while ((tmp = ecore_list_next(wlan_interfaces)) != NULL) 
     {
	if (i == wlan_interface_number) 
	  {
	     f->conf->wlan_interface = (char *)evas_stringshare_add(tmp);
	     break;
	  }
	i = i + 1;
     }   
   e_config_save_queue();
   e_border_button_bindings_grab_all();
   _monitor_cb_config_updated(f);
   return 1;
}
