/* TODO
 *
 * - submenus for items in menu ?
 *
 * - post callbacks for apps menuitems ?
 *
 * - more config options: 'move pointer to window', 'item width/item height' 'always group'
 *
 * - group items, so that an item gets swapped to the menu of another item of the same class (half done)
 *
 * - init event handler only if a gcc is shown (this would apply to all modules, imho)
 *
 * - make vertical bar work (initial work done)
 *
 * - sort function by desktops (for menu)
 *
 * -- would be good to have a more specific resize/orientation_callback to know on
 *    changes of gcc if items have to be removed from the bar
 *
 */

/* BUGS
 * - update on border property changes doesnt work correctly: on skipwinlist changes the item has to be added/removed
 *
 * */

#include "e.h"
#include "e_mod_main.h"

static Itask *_itask_new(Evas *evas, E_Zone *zone);
static void	_itask_free(Itask *it);
static int  _itask_fill(Itask *it);
static int  _itask_empty(Itask *it);
static void _itask_orient_set(Itask *it, int horizontal);
static Eina_Bool  _itask_cb_event_desk_show(void *data, int type, void *event);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *itask_config = NULL;
char *itask_theme_path;


/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new (E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);



static const E_Gadcon_Client_Class _gadcon_class =
  {
    GADCON_CLIENT_CLASS_VERSION,
    "itask",
    {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL, e_gadcon_site_is_shelf},
    E_GADCON_CLIENT_STYLE_PLAIN
  };

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Itask *it;
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;

   inst = E_NEW(Instance, 1);

   inst->ci = itask_config_item_get(id);
   // TODO prefix all options with 'option_', better add substruct
   it = _itask_new(gc->evas, gc->zone);
   it->show_label = inst->ci->show_label;
   it->show_zone = inst->ci->show_zone;
   it->show_desk = inst->ci->show_desk;
   it->icon_label = inst->ci->icon_label;
   it->skip_always_below_windows = inst->ci->skip_always_below_windows;
   it->skip_dialogs = inst->ci->skip_dialogs;
   it->swap_on_focus = inst->ci->swap_on_focus;
   it->option_iconify_focused = inst->ci->iconify_focused;
   it->option_ibox_style = inst->ci->ibox_style;
   it->max_items = inst->ci->max_items;
   it->always_group = inst->ci->always_group;
   it->menu_all_window = inst->ci->menu_all_window;
   it->hide_menu_button = inst->ci->hide_menu_button;
   it->inst = inst;
   inst->itask = it;

   o = it->o_box;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_itask = o;

   itask_config->instances = eina_list_append(itask_config->instances, inst);

   it->item_height = 32; // TODO make this selectable between mix max size that the theme provides
   it->item_width = 114;
   it->num_items = 0;
   if(!it->hide_menu_button){
      itask_menu_button(it);
   }
   itask_resize_handle(it);

   _itask_fill(it);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   itask_config->instances = eina_list_remove(itask_config->instances, inst);
   _itask_free(inst->itask);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   Instance *inst;
   Evas_Coord w, h;
   int button_size;
   Itask_Item *ic;

   inst = gcc->data;
   Itask *it = inst->itask;

   evas_object_geometry_get(it->inst->gcc->gadcon->o_container,
			    NULL, NULL,&it->itask_width, &it->itask_height);

   if (it->itask_height < 10)
     it->itask_height = 10;

   switch (gcc->gadcon->orient)
     {
      case E_GADCON_ORIENT_FLOAT:
      case E_GADCON_ORIENT_HORIZ:
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	 //if (!e_box_orientation_get(it->o_box))
	 _itask_orient_set(inst->itask, 1);

	 int width;
	 if(it->option_ibox_style)
	   width = it->itask_height;
	 else
	   width = it->item_width;

	 if(it->items_bar)
	   w = (eina_list_count(it->items_bar) * width) + it->itask_height;
	 else
	   w = it->itask_height;

	 h = it->itask_height;

	 /* printf("_gc_orient: %dx%d\n",w,h); */
	 e_gadcon_client_aspect_set(gcc, w, h);
	 break;

      case E_GADCON_ORIENT_VERT:
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_RIGHT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_LB:
      case E_GADCON_ORIENT_CORNER_RB:
	 //if (e_box_orientation_get(it->o_box))
	 _itask_orient_set(inst->itask, 0);

	 if(it->items_bar)
	   {
	      if(it->option_ibox_style)
		h = (eina_list_count(it->items_bar) * it->itask_width) + it->itask_width;
	      else
		h = (eina_list_count(it->items_bar) * it->item_height) + it->item_height;
	   }
	 else
	   h = it->item_height;

	 if(it->option_ibox_style)
	   w = it->itask_width;
	 else
	   w = it->item_width;
	 /* printf("_gc_orient: %dx%d\n",w,h); */
	 e_gadcon_client_aspect_set(gcc, w, h);

	 break;

      default:
	 break;
     }
   if (eina_list_count(inst->itask->items) < 1)
     e_gadcon_client_aspect_set(gcc, 16, 16);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return N_("Itask");
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-itask.edj",
            e_module_dir_get(itask_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char*
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
   Config_Item *ci;
   ci = itask_config_item_get(NULL);
   return ci->id;
}


/***************************************************************************/

static Itask *
_itask_new(Evas *evas, E_Zone *zone)
{
   Itask *it;

   it = E_NEW(Itask, 1);
   it->o_box = e_box_add(evas);
   e_box_homogenous_set(it->o_box, 0);
   e_box_orientation_set(it->o_box, 1);
   e_box_align_set(it->o_box, 0.0, 0.0);
   it->zone = zone;
   it->items = NULL;
   it->items_menu = NULL;
   it->items_bar = NULL;
   it->item_groups = eina_hash_string_superfast_new(NULL);
   return it;
}

static void
_itask_free(Itask *it)
{
   _itask_empty(it);
   evas_object_del(it->o_box);
   if (it->o_button)
     evas_object_del(it->o_button);

   eina_hash_free(it->item_groups);

   free(it);
}

static int
_itask_fill(Itask *it)
{
   E_Border *bd;
   E_Border_List *bl;
   bl = e_container_border_list_first(it->zone->container);
   Itask_Item *ic;
   while(bd = e_container_border_list_next(bl))
     {
	ic = itask_item_new(it, bd);
	if(ic) itask_item_realize(ic);
     }
   e_container_border_list_free(bl);

   return 0;
}

static int
_itask_empty(Itask *it)
{
   int changed = 0;
   while (it->items)
     {
	itask_item_remove(it->items->data);
	changed = 1;
     }
   return changed;
}

static void
_itask_orient_set(Itask *it, int horizontal)
{
   e_box_orientation_set(it->o_box, horizontal);
   e_box_align_set(it->o_box, 0.5, 0.5);
   itask_resize_handle(it);
}

void
itask_update_gc_orient(Itask *it)
{
   _gc_orient(it->inst->gcc, it->inst->gcc->gadcon->orient);
}

void
itask_resize_handle(Itask *it)
{
   Eina_List *l;
   Itask_Item *ic;
   Evas_Coord w, h, bw;

   evas_object_geometry_get(it->inst->gcc->gadcon->o_container,
			    NULL, NULL,&w, &h);

   if (e_box_orientation_get(it->o_box))
     {
	bw = h;
     }
   else
     {
	bw = it->item_width;
	h = it->item_height;
     }

   e_box_freeze(it->o_box);

   if(it->option_ibox_style)
     {
	e_box_pack_options_set(it->o_button,
			       1, 1, /* fill */
			       1, 1, /* expand */
			       0.0, 0.5, /* align */
			       0, 0, /* min */
			       -1, -1 /* max */
			       );

     }
   else
     {
	e_box_pack_options_set(it->o_button,
			       0, 0, /* fill */
			       0, 0, /* expand */
			       0.0, 0.5, /* align */
			       bw, h, /* min */
			       bw, h /* max */
			       );
     }
   e_box_thaw(it->o_box);
}

Eina_List *
itask_zone_find(E_Zone *zone)
{
   Eina_List *itask = NULL;
   Eina_List *l;

   for (l = itask_config->instances; l; l = l->next)
     {
	Instance *inst;
	Config_Item *ci;

	inst = l->data;
	ci = inst->ci;
	if (!ci)
	  continue;

	if ((ci->show_zone == 0 )
            || ((ci->show_zone != 0) && (inst->itask->zone == zone)))
	  itask = eina_list_append(itask, inst->itask);

     }
   return itask;
}

static Eina_Bool
_itask_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   Eina_List *l, *ll, *lll, *itask, *l_items = NULL;
   Itask *it;
   Itask_Item *ic, *ic2;

   ev = event;
   int items_on_desk_in_menu = 0;
   int items = 0;
   int items_on_desk_in_bar = 0;
   itask = itask_zone_find(ev->desk->zone);

   EINA_LIST_FREE(itask, it)
     {
	/* If we're 'this desktop only' then we need to refill when the desk changes */
	if(it->show_zone == 2)
	  {
	     _itask_empty(it);
	     _itask_fill(it);
	  }

	if(it->show_desk)
	  {
	     EINA_LIST_FOREACH(it->items_menu, ll, ic)
	       {
		  if(ic->border->desk == ev->desk)
		    {
		       items_on_desk_in_menu++;
		       l_items = eina_list_append(l_items, ic);
		    }
	       }
	     if(items_on_desk_in_menu == 0) continue;

	     EINA_LIST_FOREACH(it->items_bar, ll, ic)
	       {
		  items++;
		  if(ic->border->desk == ev->desk)
		    {
		       items_on_desk_in_bar++;
		       ic->last_time = ecore_time_get();
		    }
	       }
	     if(items == items_on_desk_in_bar)
	       {
		  while (l_items)
		    {
		       ic = l_items->data;
		       ic->last_time = ecore_time_get();
		       l_items = eina_list_remove_list(l_items, l_items);
		    }
		  continue;
	       }

	     int i;
	     for(i = 0; items - items_on_desk_in_bar >= i  && i < items_on_desk_in_menu ; i++)
	       {
		  ic = l_items->data;
		  for(lll = l_items->next; lll ; lll = lll->next)
		    {
		       ic2 = lll->data;
		       if(ic->last_time < ic2->last_time)
			 {
			    ic = ic2;
			 }
		    }

		  itask_item_swap_to_bar(ic);
		  itask_item_swap_oldest_from_bar(it);
		  l_items = eina_list_remove(l_items, ic);
	       }
	     while (l_items)
	       {
		  ic = l_items->data;
		  ic->last_time = ecore_time_get();
		  l_items = eina_list_remove_list(l_items, l_items);
	       }
	     itask_resize_handle(it);
	  }
     }

   return EINA_TRUE;
}

Config_Item *
itask_config_item_get(const char *id)
{
   Eina_List *l;
   Config_Item *ci;
   char buf[128];

   if (!id)
     {
	int  num = 0;

	/* Create id */
	if (itask_config->items)
	  {
	     const char *p;
	     ci = eina_list_last (itask_config->items)->data;
	     p = strrchr (ci->id, '.');
	     if (p) num = atoi (p + 1) + 1;
	  }
	snprintf (buf, sizeof (buf), "%s.%d", "itask", num);
	id = buf;
     }
   else
     {
	for (l = itask_config->items; l; l = l->next)
	  {
	     ci = l->data;
	     if (!ci->id)
	       continue;
	     if (!strcmp (ci->id, id))
	       return ci;
	  }
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->show_label = 0;
   ci->show_zone = 1;
   ci->show_desk = 1;
   ci->icon_label = 0;
   ci->skip_always_below_windows = 0;
   ci->skip_dialogs = 0;
   ci->swap_on_focus = 1;
   ci->iconify_focused = 1;
   ci->ibox_style = 0;
   ci->max_items = 7;
   ci->always_group = 0;
   ci->menu_all_window = 1;
   ci->hide_menu_button = 0;
   itask_config->items = eina_list_append(itask_config->items, ci);
   return ci;
}

void
itask_config_update(Config_Item *ci)
{
   Eina_List *l;
   Instance *inst;
   Itask *it;

   for (l = itask_config->instances; l; l = l->next)
     {
	inst = l->data;
        if(inst->ci == ci){
	   it = inst->itask;
	   it->show_label = ci->show_label;
	   it->show_zone = ci->show_zone;
	   it->show_desk = ci->show_desk;
	   it->icon_label = ci->icon_label;
	   it->skip_always_below_windows = ci->skip_always_below_windows;
	   it->skip_dialogs = ci->skip_dialogs;
	   it->swap_on_focus = ci->swap_on_focus;
	   it->option_iconify_focused = ci->iconify_focused;
	   it->option_ibox_style = ci->ibox_style;
	   it->max_items = ci->max_items;
	   it->always_group = ci->always_group;
	   it->menu_all_window = ci->menu_all_window;
	   it->hide_menu_button = ci->hide_menu_button;
	   _itask_empty(it);
	   _itask_fill(it);
	   return;
	}
     }
}

/***************************************************************************/


EAPI E_Module_Api e_modapi = { E_MODULE_API_VERSION, "Itask" };



EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   /* Location of message catalogs for localization */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Itask_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, show_label, INT);
   E_CONFIG_VAL(D, T, show_zone, INT);
   E_CONFIG_VAL(D, T, show_desk, INT);
   E_CONFIG_VAL(D, T, icon_label, INT);
   E_CONFIG_VAL(D, T, skip_always_below_windows, INT);
   E_CONFIG_VAL(D, T, skip_dialogs, INT);
   E_CONFIG_VAL(D, T, swap_on_focus, INT);
   E_CONFIG_VAL(D, T, iconify_focused, INT);
   E_CONFIG_VAL(D, T, ibox_style, INT);
   E_CONFIG_VAL(D, T, max_items, INT);
   E_CONFIG_VAL(D, T, always_group, INT);
   E_CONFIG_VAL(D, T, menu_all_window, INT);
   E_CONFIG_VAL(D, T, hide_menu_button, INT);
   conf_edd = E_CONFIG_DD_NEW("Itask_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   itask_config = e_config_domain_load("module.itask", conf_edd);
   if (!itask_config)
     {
	Config_Item *ci;

	itask_config = E_NEW(Config, 1);

	ci = E_NEW(Config_Item, 1);
	ci->id = eina_stringshare_add("0");
	ci->show_label = 0;
	ci->show_zone = 1;
	ci->show_desk = 1;
	ci->icon_label = 0;
	ci->skip_always_below_windows = 0;
	ci->skip_dialogs = 0;
	ci->swap_on_focus = 1;
	ci->iconify_focused = 1;
	ci->ibox_style = 0;
	ci->max_items = 7;
	ci->always_group = 0;
	ci->menu_all_window = 1;
	ci->hide_menu_button = 0;
	itask_config->items = eina_list_append(itask_config->items, ci);
     }

   itask_config->module = m;

   itask_config->handlers = eina_list_append
     (itask_config->handlers, ecore_event_handler_add
      (E_EVENT_DESK_SHOW, _itask_cb_event_desk_show, NULL));

   e_gadcon_provider_register(&_gadcon_class);

   snprintf(buf, sizeof(buf), "%s/itask.edj", e_module_dir_get(itask_config->module));
   itask_theme_path = strdup(buf);

   itask_items_init(itask_config);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{

   e_gadcon_provider_unregister(&_gadcon_class);

   while (itask_config->handlers)
     {
	ecore_event_handler_del(itask_config->handlers->data);
	itask_config->handlers = eina_list_remove_list(itask_config->handlers, itask_config->handlers);
     }

   while (itask_config->config_dialog)
     e_object_del(E_OBJECT(itask_config->config_dialog->data));

   while (itask_config->items)
     {
	Config_Item *ci;

	ci = itask_config->items->data;
	itask_config->items = eina_list_remove_list(itask_config->items, itask_config->items);
	if (ci->id)
	  eina_stringshare_del(ci->id);
	free(ci);
     }

   free(itask_config);
   itask_config = NULL;

   free(itask_theme_path);
   itask_theme_path = NULL;

   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.itask", conf_edd, itask_config);
   return 1;
}



