#include <e.h>
#include <Eina.h>
#include <Elementary.h>

#include "main.h"
#include "elfe_config.h"

/* local function prototypes */

/* local variables */
EAPI Elfe_Home_Config *elfe_home_cfg = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_desktop_edd = NULL;
static E_Config_DD *conf_desktop_item_edd = NULL;
Ecore_Timer *_elfe_home_config_change_timer = NULL;
Evas_Object *delay_label, *delay_slider;

/* public functions */
int
elfe_home_config_init(E_Module *m)
{
   conf_desktop_item_edd = E_CONFIG_DD_NEW("Elfe_Desktop_Item_Cfg", Elfe_Desktop_Item_Config);

#undef T
#undef D
#define T Elfe_Desktop_Item_Config
#define D conf_desktop_item_edd
   E_CONFIG_VAL(D, T, type, INT);
   E_CONFIG_VAL(D, T, name, STR);
   E_CONFIG_VAL(D, T, pos_x, INT);
   E_CONFIG_VAL(D, T, pos_y, INT);
   E_CONFIG_VAL(D, T, size_w, INT);
   E_CONFIG_VAL(D, T, size_h, INT);
   E_CONFIG_VAL(D, T, col, INT);
   E_CONFIG_VAL(D, T, row, INT);

   conf_desktop_edd = E_CONFIG_DD_NEW("Elfe_Desktop_Cfg", Elfe_Desktop_Config);
#undef T
#undef D
#define T Elfe_Desktop_Config
#define D conf_desktop_edd
   E_CONFIG_LIST(D, T, items, conf_desktop_item_edd);


   conf_edd = E_CONFIG_DD_NEW("Elfe_Cfg", Elfe_Home_Config);
#undef T
#undef D
#define T Elfe_Home_Config
#define D conf_edd
   E_CONFIG_LIST(D, T, desktops, conf_desktop_edd);
   E_CONFIG_VAL(D, T, cols, INT);
   E_CONFIG_VAL(D, T, rows, INT);
   E_CONFIG_VAL(D, T, icon_size, INT);
   E_CONFIG_LIST(D, T, dock_items, conf_desktop_item_edd);

   elfe_home_cfg = e_config_domain_load("module.elfe", conf_edd);

   if (!elfe_home_cfg)
     {
         int i;
         elfe_home_cfg = E_NEW(Elfe_Home_Config, 1);
         elfe_home_cfg->cols = 4;
         elfe_home_cfg->rows = 4;
         elfe_home_cfg->icon_size = 72;
         for (i = 0; i < 5; i++)
	   {
	      Elfe_Desktop_Config *dc;
	      dc = E_NEW(Elfe_Desktop_Config, 1);
	      elfe_home_cfg->desktops = eina_list_append(elfe_home_cfg->desktops, dc);
	   }

     }

   if (!elfe_home_cfg->cols)
       elfe_home_cfg->cols = 4;
   if (!elfe_home_cfg->rows)
       elfe_home_cfg->rows = 4;
   if (!elfe_home_cfg->icon_size)
       elfe_home_cfg->icon_size = 72;

   elfe_home_cfg->mod_dir = eina_stringshare_add(m->dir);
   elfe_home_cfg->theme = eina_stringshare_printf("%s/default.edj", elfe_home_cfg->mod_dir);

   return 1;
}

int
elfe_home_config_shutdown(void)
{
   e_configure_registry_item_del("illume/elfe");
   e_configure_registry_category_del("illume");

   if (elfe_home_cfg->mod_dir) eina_stringshare_del(elfe_home_cfg->mod_dir);

   E_FREE(elfe_home_cfg);
   elfe_home_cfg = NULL;

   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

int
elfe_home_config_save(void)
{
   e_config_domain_save("module.elfe", conf_edd, elfe_home_cfg);
   return 1;
}

void elfe_home_config_desktop_item_add(int desktop,
                                       Elfe_Desktop_Item_Type type,
                                       int row, int col,
                                       Evas_Coord x, Evas_Coord y,
                                       Evas_Coord w, Evas_Coord h,
                                       const char *name)
{
   Elfe_Desktop_Config *dc;
   Elfe_Desktop_Item_Config *dic;

   dic = calloc(1, sizeof(Elfe_Desktop_Item_Config));
   dic->type = type;
   dic->name = eina_stringshare_add(name);

   dic->size_w = w;
   dic->size_h = h;
   dic->pos_x = x;
   dic->pos_y = y;
   dic->col = col;
   dic->row = row;

   dc = eina_list_nth(elfe_home_cfg->desktops, desktop);
   if (dc)
     dc->items = eina_list_append(dc->items, dic);

   elfe_home_config_save();
}

void elfe_home_config_desktop_item_del(int desktop,
				       int row, int col)
{
    Elfe_Desktop_Config *dc;
    Elfe_Desktop_Item_Config *dic;
    Eina_List *l;

    dc = eina_list_nth(elfe_home_cfg->desktops, desktop);
    EINA_LIST_FOREACH(dc->items, l, dic)
      {
	 if ((dic->row == row) && (dic->col == col))
	   {
	      dc->items = eina_list_remove(dc->items, dic);
	      elfe_home_config_save();
	      return;
	   }
      }
}


void elfe_home_config_dock_item_add(int col,
                                    const char *name)
{
   Elfe_Desktop_Item_Config *dic;

   dic = calloc(1, sizeof(Elfe_Desktop_Item_Config));
   dic->type = ELFE_DESKTOP_ITEM_ICON;
   dic->name = eina_stringshare_add(name);
   dic->col = col;
   elfe_home_cfg->dock_items = eina_list_append(elfe_home_cfg->dock_items, dic);
   elfe_home_config_save();
}

void elfe_home_config_dock_item_del(int col)
{
    Elfe_Desktop_Item_Config *dic;

    dic = eina_list_nth(elfe_home_cfg->dock_items, col);
    elfe_home_cfg->dock_items = eina_list_remove(elfe_home_cfg->dock_items, dic);
    elfe_home_config_save();
}
