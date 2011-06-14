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
   Evas_Object *icon;
   Efreet_Desktop *desktop;
   E_Gadcon_Client *gcc;
   E_Gadcon_Client_Class *cc;
   int row;
   int col;
   Eina_Bool edit_mode;

};

static void
_gadget_del(E_Gadcon_Client *gcc)
{
   e_object_del(E_OBJECT(gcc));
}

static Evas_Object *
_gadget_add(Elfe_Desktop_Item *dit, const char *name, E_Gadcon *gc)
{
   Evas_Object *item;
   E_Gadcon_Client_Class *cc = NULL;
   E_Gadcon_Client *gcc = NULL;

   if (!gc) return NULL;

   cc = elfe_utils_gadcon_client_class_from_name(name);
   if (!cc)
     {
	printf("Error unable to retrieve gadcon client class for %s\n", name);
	return NULL;
     }

   gcc = cc->func.init(gc, cc->name, "test", cc->default_style);
   if (!gcc) return NULL;

   e_object_del_func_set(E_OBJECT(gcc), E_OBJECT_CLEANUP_FUNC(_gadget_del));

   item = edje_object_add(evas_object_evas_get(dit->frame));
   edje_object_file_set(item, elfe_home_cfg->theme, "elfe/desktop/gadget/frame");

   gcc->cf = NULL;
   gcc->client_class = cc;
   edje_object_part_swallow(item, "elfe.swallow.content", gcc->o_base);
   dit->gcc = gcc;
   dit->cc = cc;

   return item;
}

static void*
_app_exec_cb(void *data __UNUSED__, Efreet_Desktop *desktop __UNUSED__, char *command, int remaining __UNUSED__)
{
   ecore_exe_run(command, NULL);
   return NULL;
}

static void
_clicked_signal_cb(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Elfe_Desktop_Item *dit = data;

   if (dit->desktop && !dit->edit_mode)
     efreet_desktop_command_get(dit->desktop, NULL,
				_app_exec_cb, NULL);
   else if (!dit->desktop)
       evas_object_smart_callback_call(dit->frame, "clicked", NULL);
}

static void
_delete_signal_cb(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Elfe_Desktop_Item *dit = data;

   if (dit->edit_mode)
	evas_object_smart_callback_call(dit->frame,
	"item,delete", dit->frame);
}


static Evas_Object *
_app_add(Elfe_Desktop_Item *dit, const char *name __UNUSED__)
{
   Evas_Object *item;
   Evas_Object *icon;

   item = edje_object_add(evas_object_evas_get(dit->frame));
   edje_object_file_set(item, elfe_home_cfg->theme, "elfe/desktop/app/frame");

   icon = elfe_utils_fdo_icon_add(dit->frame, dit->desktop->icon, elfe_home_cfg->icon_size); 
   //evas_object_size_hint_min_set(icon, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
   evas_object_size_hint_max_set(icon, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);

   edje_object_part_swallow(item, "elfe.swallow.content", icon);
   dit->icon = icon;


   edje_object_part_text_set(item, "elfe.text.label", dit->desktop->name);
   edje_object_signal_callback_add(item, "mouse,clicked,1", "*", _clicked_signal_cb, dit);

   return item;
}

static Evas_Object *
_icon_add(Elfe_Desktop_Item *dit, const char *name)
{
   Evas_Object *item;
   Evas_Object *icon;

   item = edje_object_add(evas_object_evas_get(dit->frame));
   edje_object_file_set(item, elfe_home_cfg->theme, "elfe/dock/icon/frame");

   if (dit->desktop)
     icon = elfe_utils_fdo_icon_add(dit->frame, dit->desktop->icon, elfe_home_cfg->icon_size);
   else
     icon = elfe_utils_fdo_icon_add(dit->frame, name, elfe_home_cfg->icon_size);
   evas_object_size_hint_min_set(icon, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);
   evas_object_size_hint_max_set(icon, elfe_home_cfg->icon_size, elfe_home_cfg->icon_size);

   edje_object_part_swallow(item, "elfe.swallow.content", icon);
   dit->icon = icon;

   //edje_object_part_text_set(item, "elfe.text.label", name);
   edje_object_signal_callback_add(item, "mouse,clicked,1", "*", _clicked_signal_cb, dit);

   return item;
}

void
elfe_desktop_item_pos_get(Evas_Object *obj, int *row, int *col)
{
   Elfe_Desktop_Item *dit = evas_object_data_get(obj, "desktop_item");

   if (!dit)
     return;

   if (col)
     *col = dit->col;
   if (row)
     *row = dit->row;
}

void
elfe_desktop_item_edit_mode_set(Evas_Object *obj, Eina_Bool mode)
{
   Elfe_Desktop_Item *dit = evas_object_data_get(obj, "desktop_item");

   if (dit->edit_mode == mode)
     return;

   dit->edit_mode = mode;
   if (mode)
       edje_object_signal_emit(dit->frame, "action,edit,on", "elfe");
   else
       edje_object_signal_emit(dit->frame, "action,edit,off", "elfe");
}

static void
_obj_del_cb(void *data , Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elfe_Desktop_Item *dit = data;

   /* FIXME delay object deletion and add edje effect before deleting */

   if (dit->icon)
     evas_object_del(dit->icon);
   if (dit->gcc)
     e_object_del(E_OBJECT(dit->gcc));
   if (dit->item)
     evas_object_del(dit->item);
   if (dit->desktop)
     efreet_desktop_free(dit->desktop);
   free(dit);
   dit = NULL;

}

void
elfe_desktop_item_pos_set(Evas_Object *obj, int row, int col)
{
    Elfe_Desktop_Item *dit = evas_object_data_get(obj, "desktop_item");

    dit->col = col;
    dit->row = row;
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

   dit->row = row;
   dit->col = col;

   layout = edje_object_add(evas_object_evas_get(parent));
   edje_object_file_set(layout, elfe_home_cfg->theme, "elfe/desktop/frame");

   dit->frame = layout;
   printf("ITEM ADD %s\n", name);

   switch (type)
     {
      case ELFE_DESKTOP_ITEM_APP:
	 dit->desktop = efreet_desktop_get(name);
         
	 if (!dit->desktop)
	   {
	      printf("ERROR unable to get efreet desktop from %s\n", name);
	      evas_object_del(layout);
	      free(dit);
	      return NULL;
	   }
	 item = _app_add(dit, name);

	 break;
      case ELFE_DESKTOP_ITEM_GADGET:
	 item = _gadget_add(dit, name, gc);
	 if (!item)
	   {
	      printf("ERROR unable to create gadget %s\n", name);
	      evas_object_del(layout);
	      free(dit);
	      return NULL;
	   }
	 break;
     case ELFE_DESKTOP_ITEM_ICON:
         dit->desktop = efreet_desktop_get(name);
	 item = _icon_add(dit, name);
	 if (!item)
	   {
	      printf("ERROR unable to create icon %s\n", name);
	      evas_object_del(layout);
	      free(dit);
	      return NULL;
	   }
	 break;
      default:
	 break;
     }

   edje_object_part_swallow(layout, "elfe.swallow.content", item);
   edje_object_signal_callback_add(layout, "elfe,delete,clicked", "*", _delete_signal_cb, dit);
   evas_object_show(item);

   dit->item = item;

   evas_object_event_callback_add(dit->frame, EVAS_CALLBACK_DEL,
				  _obj_del_cb, dit);

   evas_object_data_set(dit->frame, "desktop_item", dit);

   return dit->frame;
}
