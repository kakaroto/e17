#ifdef E_TYPEDEFS
#else
#ifndef E_MOD_CONFIG_H
#define E_MOD_CONFIG_H
#include "e_mod_main.h"

struct _E_Config_vdesk
{
   int x, y;
   int zone_num;
   int layout;
};

struct _E_Config_Dialog_Data
{
   int tiling_enabled;
   int tiling_mode;
   int dont_touch_borders;
   int tile_dialogs;
   int float_too_big_windows;
   int grid_rows;
   int grid_distribute_equally;
   int space_between;
   int between_x;
   int between_y;
   double big_perc;
   Eina_List *vdesks;
   char *tiling_border;
   char *floating_border;
   Evas_Object *o_zonelist;
   Evas_Object *o_desklist;
   Evas_Object *o_deskscroll;
   Evas_Object *o_space_between;
   Evas *evas;
};

EAPI E_Config_Dialog *e_int_config_tiling_module(E_Container *con, const char *params __UNUSED__);
#endif
#endif
