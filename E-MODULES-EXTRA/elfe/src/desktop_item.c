#include <e.h>
#include <Elementary.h>

#include "elfe_config.h"
#include "desktop_item.h"
#include "utils.h"

typedef struct _Elfe_Desktop_Item Elfe_Desktop_Item;
struct _Elfe_Desktop_Item
{
   Evas_Object *frame;
   Evas_Object *item;
   int row;
   int col;

};

static Evas_Object *
_gadget_add(Elfe_Desktop_Item *dit, const char *name, E_Gadcon *gc)
{
   Evas_Object *item;
   E_Gadcon_Client_Class *cc = NULL;
   Eina_List *l;
   E_Gadcon_Client *gcc = NULL;

   if (!gc) return NULL;

   item = edje_object_add(evas_object_evas_get(dit->frame));
   edje_object_file_set(item, elfe_home_cfg->theme, "elfe/desktop/gadget/frame");

   cc = elfe_utils_gadcon_client_class_from_name(name);
   if (!cc)
     {
	printf("Error unable to retrieve gadcon client class for %s\n", name);
	return NULL;
     }
   gcc = cc->func.init(gc, cc->name, "test", cc->default_style);
   gcc->cf = NULL;
   gcc->client_class = cc;
   edje_object_part_swallow(item, "elfe.swallow.content", gcc->o_base);

   return item;
}

static void*
_app_exec_cb(void *data, Efreet_Desktop *desktop, char *command, int remaining)
{
   ecore_exe_run(command, NULL);
}

static void
_clicked_signal_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   const char *name = data;
   Efreet_Desktop *desktop;

   desktop = efreet_desktop_get(name);
   efreet_desktop_command_get(desktop, NULL,
			      _app_exec_cb, NULL);
}


static Evas_Object *
_app_add(Elfe_Desktop_Item *dit, const char *name)
{
   Evas_Object *item;
   Evas_Object *icon;
   Efreet_Desktop *desktop;

   desktop = efreet_desktop_get(name);

   if (!desktop)
     return NULL;

   item = edje_object_add(evas_object_evas_get(dit->frame));
   edje_object_file_set(item, elfe_home_cfg->theme, "elfe/desktop/app/frame");

   icon = elfe_utils_fdo_icon_add(dit->frame, desktop->icon, 96);
   edje_object_part_swallow(item, "elfe.swallow.content", icon);

   edje_object_part_text_set(item, "elfe.text.label", desktop->name);
   edje_object_signal_callback_add(item, "mouse,clicked,1", "*", _clicked_signal_cb, (void*)name);

   return item;
}

void
elfe_desktop_item_pos_get(Evas_Object *obj, int *col, int *row)
{
   Elfe_Desktop_Item *dit = evas_object_data_get(obj, "desktop_item");

   if (!dit)
     return;

   if (col)
     *col = dit->col;
   if (row)
     *row = dit->row;
}

Evas_Object *
elfe_desktop_item_add(Evas_Object *parent,
		      int row, int col,
		      const char *name,
		      Elfe_Desktop_Item_Type type,
		      E_Gadcon *gc)
{
   Elfe_Desktop_Item *dit;
   Evas_Object *layout = NULL;
   Evas_Object *item;

   dit = calloc(1, sizeof(Elfe_Desktop_Item));
   if (!dit)
     return NULL;

   dit->col = col;
   dit->row = row;

   layout = edje_object_add(evas_object_evas_get(parent));
   edje_object_file_set(layout, elfe_home_cfg->theme, "elfe/desktop/frame");

   dit->frame = layout;


   switch (type)
     {
      case ELFE_DESKTOP_ITEM_APP:
	 item = _app_add(dit, name);
	 break;
      case ELFE_DESKTOP_ITEM_GADGET:
	 item = _gadget_add(dit, name, gc);
	 break;
      default:
	 break;
     }

   edje_object_part_swallow(layout, "elfe.swallow.content", item);
   evas_object_show(item);

   dit->item = item;

   evas_object_data_set(dit->frame, "desktop_item", dit);

   return dit->frame;
}
