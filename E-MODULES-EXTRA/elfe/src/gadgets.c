#include <e.h>

#include "gadgets.h"
#include "utils.h"
#include "elfe_config.h"

typedef struct _Elfe_Gadget Elfe_Gadget;

struct _Elfe_Gadget
{
   int desktop;
   Evas_Object *layout;
   E_Gadcon_Location *location;
   E_Gadcon *gc;
   Eina_Bool in_use[4][4]; /* FIXME change gor a dynamic and use eina_array instead */
};

static Evas_Object *
_item_add(Elfe_Gadget *egad, Elfe_Desktop_Item_Type type, const char *name, Evas_Object *child, int col, int row, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
   Elfe_Desktop_Config *dc;
   Elfe_Desktop_Item_Config *dic;
   Evas_Object *ly;

   if (egad->in_use[col][row] == EINA_TRUE)
     {
	printf("ERROR : already in use\n");
	return NULL;
     }

   ly = edje_object_add(evas_object_evas_get(egad->layout));

   switch (type)
     {
      case ELFE_DESKTOP_ITEM_APP:
	 edje_object_file_set(ly, elfe_home_cfg->theme, "elfe/desktop/app/frame");
	 edje_object_part_text_set(ly, "elfe.text.label", name);
	 break;
      case ELFE_DESKTOP_ITEM_GADGET:
	 edje_object_file_set(ly, elfe_home_cfg->theme, "elfe/desktop/gadget/frame");
	 break;
      default:
	 break;
     }


   edje_object_part_swallow(ly, "elfe.swallow.content", child);
   evas_object_show(child);
   evas_object_show(ly);

   e_layout_pack(egad->layout, ly);
   //evas_object_move(ly, x,  y);
   //evas_object_resize(ly, w, h);

   e_layout_child_move(ly, x,  y);
   e_layout_child_resize(ly, w, h);

   egad->in_use[col][row] = EINA_TRUE;

   return ly;
}



static int
_gadget_client_add(void *data, const E_Gadcon_Client_Class *cc)
{

   Elfe_Gadget *egad = data;
   E_Gadcon_Client *gcc = NULL;
   Evas_Coord ox, oy, ow, oh;
   int i,j;
   Evas_Coord x, y, w, h;
   Evas_Object *ly;

   /* FIXME cols/row must be a config value */
   evas_object_geometry_get(egad->layout, &ox, &oy, &ow, &oh);

   w = ow / 4;
   h = oh / 4;

   for (i = 0; i < 4; i++)
     for (j = 0; j < 4; j++)
       {
	  if (!egad->in_use[i][j])
	    {
	       x = i*w;
	       y = j*h;
	       gcc = cc->func.init(egad->gc, cc->name, "test", cc->default_style);
               gcc->cf = NULL;
               gcc->client_class = cc;
	       ly = _item_add(egad,  ELFE_DESKTOP_ITEM_GADGET, cc->name, gcc->o_base, i, j, x, y, w, h);
	       if (ly)
		 {
		 elfe_home_config_desktop_item_add(egad->desktop, ELFE_DESKTOP_ITEM_GADGET, i, j, x, y, w, h, cc->name);
		 return 1;
		 }
	       else
		 return 0;
	    }
       }

   return 0;
}

static void
_gadget_client_remove(void *data, E_Gadcon_Client *gcc)
{
}

void
_gadget_populate_class(void *data, E_Gadcon *gc, const E_Gadcon_Client_Class *cc)
{

}

typedef struct _App_Item App_Item;

struct _App_Item
{
    Efreet_Desktop *desktop;
    Elfe_Gadget *gadget;
};

static void*
_app_exec_cb(void *data, Efreet_Desktop *desktop, char *command, int remaining)
{
    ecore_exe_run(command, NULL);
}


static void
_app_mouse_click_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
    App_Item *app_item = data;

    efreet_desktop_command_get(app_item->desktop, NULL,
                               _app_exec_cb, NULL);
}

void
elfe_gadgets_app_add(Evas_Object *obj, Efreet_Menu *app, Evas_Coord x, Evas_Coord y)
{
   Elfe_Gadget *egad =  evas_object_data_get(obj, "elfe_gadget");
   Evas_Object *ic;
   Evas_Object *ly;
   App_Item *app_item;
   Evas_Coord ox, oy, ow, oh;
   Evas_Coord pos_x, pos_y;
   int m,n;
   Evas_Coord w, h;

   evas_object_geometry_get(egad->layout, &ox, &oy, &ow, &oh);
   w = ow / 4;
   h = oh / 4;

   m = (x/w) % 4;
   n = (y/h) % 4;

   pos_x = m*w;
   pos_y = n*h;

   ic = elfe_utils_fdo_icon_add(obj, app->icon, MIN(w,h));
   evas_object_show(ic);

   ly = _item_add(egad, ELFE_DESKTOP_ITEM_APP, app->icon, ic, m, n, pos_x, pos_y, w, h);
   if (ly)
     elfe_home_config_desktop_item_add(egad->desktop, ELFE_DESKTOP_ITEM_APP, m, n, pos_x, pos_y, w, h, app->desktop->orig_path);
   else
     return;

   app_item = calloc(1, sizeof(App_Item));
   app_item->gadget = egad;
   app_item->desktop = app->desktop;

   edje_object_signal_callback_add(ly, "elfe,action,click", "", _app_mouse_click_cb, app_item);
}

void
elfe_gadgets_gadget_add(Evas_Object *obj, E_Gadcon_Client_Class *gcc)
{
   Elfe_Gadget *egad = evas_object_data_get(obj, "elfe_gadget");

   _gadget_client_add(egad, gcc);
}

static void
_object_resize_cb(void *data , Evas *e , Evas_Object *obj, void *event_info )
{
   Elfe_Gadget *egad = data;
   Evas_Coord x, y, w, h;


   evas_object_geometry_get(egad->layout, &x, &y, &w, &h);
   e_layout_virtual_size_set(egad->layout, w, h);
   evas_object_show(egad->layout);
}


Evas_Object *
elfe_gadgets_zone_add(Evas_Object *parent, E_Zone *zone, int desktop, const char *desktop_name)
{
   const char *location_name;
   Elfe_Gadget *egad;
   int i,j;
   Elfe_Desktop_Config *dc;
   Elfe_Desktop_Item_Config *dic;
   Eina_List *l;

   egad = calloc(1, sizeof(Elfe_Gadget));
   if (!egad)
     return NULL;

   egad->layout = e_layout_add(evas_object_evas_get(parent));
   evas_object_data_set(egad->layout, "elfe_gadget", egad);
   evas_object_event_callback_add(egad->layout, EVAS_CALLBACK_RESIZE,
				  _object_resize_cb, egad);

   //e_layout_virtual_size_set(egad->layout, 480, 480);

   egad->gc = E_OBJECT_ALLOC(E_Gadcon, E_GADCON_TYPE, NULL);
   if (!egad->gc)
     {
	evas_object_del(egad->layout);
	free(egad);
	return NULL;
     }

   /* FIXME already done by calloc ? */
   for (i = 0; i < 4; i++)
     for (j = 0; j < 4; j++)
       egad->in_use[i][j] = EINA_FALSE;


   location_name = eina_stringshare_printf("Elfe %s\n", desktop_name);
   egad->desktop = desktop;
   egad->location = e_gadcon_location_new (location_name, E_GADCON_SITE_DESKTOP,
					   _gadget_client_add, egad,
					   _gadget_client_remove, NULL);
   eina_stringshare_del(location_name);
   e_gadcon_location_set_icon_name(egad->location, "preferences-desktop");
   e_gadcon_location_register(egad->location);

   egad->gc->name = eina_stringshare_add(location_name);
   egad->gc->layout_policy = E_GADCON_LAYOUT_POLICY_PANEL;
   egad->gc->orient = E_GADCON_ORIENT_FLOAT;
   egad->gc->location = egad->location;
   egad->gc->evas = evas_object_evas_get(parent);
   e_gadcon_populate_callback_set(egad->gc, _gadget_populate_class, egad->gc);
   egad->gc->id = 0;
   egad->gc->edje.o_parent = NULL;
   egad->gc->edje.swallow_name = NULL;
   egad->gc->shelf = NULL;
   egad->gc->toolbar = NULL;
   egad->gc->editing = 0;
   egad->gc->o_container = NULL;
   egad->gc->frame_request.func = NULL;
   egad->gc->resize_request.func = NULL;
   egad->gc->min_size_request.func = NULL;
   e_gadcon_zone_set(egad->gc, zone);
   e_gadcon_custom_new(egad->gc);

   /* Fill zone with config items*/
  dc = eina_list_nth(elfe_home_cfg->desktops, egad->desktop);
   EINA_LIST_FOREACH(dc->items, l, dic)
     {
	Evas_Object *child = NULL;
	Efreet_Desktop *desktop;

	switch (dic->type)
	  {
	   case ELFE_DESKTOP_ITEM_APP:
	     {
		Evas_Object *ly;
		App_Item *app_item;

		desktop = efreet_desktop_get(dic->name);

		child = elfe_utils_fdo_icon_add(egad->layout, desktop->icon, MIN(dic->size_w,dic->size_h));
		evas_object_show(child);


		ly = _item_add(egad, dic->type, desktop->name,
			       child, dic->col, dic->row,
			       dic->pos_x, dic->pos_y,
			       dic->size_w, dic->size_h);

		app_item = calloc(1, sizeof(App_Item));
		app_item->gadget = egad;
		app_item->desktop = desktop;

		edje_object_signal_callback_add(ly, "elfe,action,click", "", _app_mouse_click_cb, app_item);

		break;
	     }
	   case ELFE_DESKTOP_ITEM_GADGET:
	      break;
	   default:
	      break;
	  }

     }


   return egad->layout;
}
