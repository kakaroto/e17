
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H


#include <e.h>

#include "flow_chart.h"
#include "linux_2.6.h"

typedef struct _Config      Config;
typedef struct _Config_Face Config_Face;
typedef struct _Monitor       Monitor;
typedef struct _Monitor_Face  Monitor_Face;

struct _Config
{
   Evas_List *faces;
};

struct _Config_Face
{
   unsigned char enabled;
};

struct _Monitor
{
   Evas_List   *faces;
   E_Menu      *config_menu;
   
   Config      *conf;
};

struct _Monitor_Face
{
  E_Container *con;
  E_Menu      *menu;
  Config_Face *conf;

  Evas_Object *cpu, *net, *mem;
  Evas_Object *cpu_ev_obj, *net_ev_obj, *mem_ev_obj;
  Evas_Object *table_object, *monitor_object;

  double cpu_rate;
  double mem_rate;
  double net_rate;
   
  Chart_Container *chart_cpu, *chart_net, *chart_mem;

   
  E_Gadman_Client *gmc;
};

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);
EAPI int   e_modapi_info     (E_Module *m);
EAPI int   e_modapi_about    (E_Module *m);

#endif
