/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H
#include <libintl.h>
#define D_(str) dgettext(PACKAGE, str)

#define TILE_GRID 	0
#define TILE_BIGMAIN 	1
#define TILE_NONE	2
#define TILE_INDIVIDUAL 3

typedef struct _Config		Config;
typedef struct _Tiling_Info	Tiling_Info;

struct _Config_vdesk
{
   int x, y;
   int zone_num;
   int layout;
};

struct _Config
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
};

struct _Tiling_Info
{
   /* This is the main border for the bigmain layout */
   E_Border *mainbd;

   /* border width */
   int mainbd_width;

   /* If there is only one window on this zone, it's saved here to unminize
    * as soon as there are more windows */
   E_Border *single_win;

   /* The desk for which this _Tiling_Info is used. Needed because (for example)
    * on e restart all desks are shown on all zones but no change events are
    * triggered */
   E_Desk *desk;

   /* List of windows which were toggled floating */
   Eina_List *floating_windows;

   /* List of windows in our own sorting */
   Eina_List *client_list;

   /* big_perc (percentage of the screen which the mainbd-border will get) has
    * to be stored individually for each desk, the one in Tiling_Config is only
    * the default */
   double big_perc;

   /* When sending a border to another desktop, it has to be updated as soon
    * as the user switches to it. This is stored in the following flag. */
   int need_rearrange;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

EAPI void  e_mod_tiling_rearrange();

#endif
