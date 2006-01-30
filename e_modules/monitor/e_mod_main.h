#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <e.h>
#include <Ecore.h>
#include "config.h"

#include "flow_chart.h"
#include "linux_2.6.h"

#define MONITOR_CONFIG_VERSION 100

typedef struct _Config      Config;
typedef struct _Config_Face Config_Face;
typedef struct _Monitor       Monitor;
typedef struct _Monitor_Face  Monitor_Face;

struct _Config
{
   Evas_List *faces;
   int cpu;
   int mem;
   int net;
   int wlan;
   int hostname;
   int Horz;
   int uptime;
   int time;
};

struct _Config_Face
{
   int config_version;
   unsigned char enabled;

   double cpu_interval;
   double mem_interval;
   double net_interval;
   double wlan_interval;

   char *net_interface;
   char *wlan_interface;

   int mem_real_ignore_cached;
   int mem_real_ignore_buffers;
};

struct _Monitor
{
   Evas_List   *faces;
   E_Menu      *config_menu;

   Config      *conf;
};

struct _Monitor_Face
{
  Monitor *mon;
  E_Container *con;
  E_Menu      *menu;
  Config_Face *conf;
  Ecore_Timer *date_check_timer;   

  Evas_Object *cpu, *net, *mem, *wlan, *hostname, *uptime, *time;
  Evas_Object *table_object, *monitor_object, *monitor_cover_obj;

  Chart_Container *chart_cpu, *chart_net, *chart_mem, *chart_wlan;

  E_Gadman_Client *gmc;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_info     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);
EAPI int   e_modapi_config   (E_Module *m);

void _monitor_cb_config_updated(void *data);
void rebuild_monitor(Monitor_Face *face);

#endif
