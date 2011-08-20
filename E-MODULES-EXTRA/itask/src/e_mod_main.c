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
    {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL, NULL},
    E_GADCON_CLIENT_STYLE_PLAIN
  };

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Itask *it;
   E_Gadcon_Client *gcc;

   it = _itask_new(gc->evas, gc->zone);
   it->ci = itask_config_item_get(id);
   it->ci->it = it;
   it->item_height = 32;
   it->item_width = 114;
   it->num_items = 0;

   gcc = e_gadcon_client_new(gc, name, id, style, it->o_box);
   gcc->data = it;
   it->gcc = gcc;

   itask_config->instances = eina_list_append(itask_config->instances, it);

   if (!it->ci->hide_menu_button)
     itask_menu_button(it);

   itask_resize_handle(it);

   _itask_fill(it);

   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Itask *it;

   it = gcc->data;
   itask_config->instances = eina_list_remove(itask_config->instances, it);
   _itask_free(it);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   Evas_Coord w, h;

   Itask *it = gcc->data;

   evas_object_geometry_get(it->gcc->gadcon->o_container,
			    NULL, NULL,
			    &it->itask_width,
			    &it->itask_height);

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
	 _itask_orient_set(it, 1);

	 int width;
	 if (it->ci->ibox_style)
	   width = it->itask_height;
	 else
	   width = it->item_width;

	 if (it->items_bar)
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
	 _itask_orient_set(it, 0);

	 if (it->items_bar)
	   {
	      if (it->ci->ibox_style)
		h = (eina_list_count(it->items_bar) * it->itask_width) + it->itask_width;
	      else
		h = (eina_list_count(it->items_bar) * it->item_height) + it->item_height;
	   }
	 else
	   h = it->item_height;

	 if (it->ci->ibox_style)
	   w = it->itask_width;
	 else
	   w = it->item_width;
	 /* printf("_gc_orient: %dx%d\n",w,h); */
	 e_gadcon_client_aspect_set(gcc, w, h);

	 break;

      default:
	 break;
     }
   if (eina_list_count(it->items) < 1)
     e_gadcon_client_aspect_set(gcc, 16, 16);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return D_("Itask");
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
   Itask_Item *ic;

   bl = e_container_border_list_first(it->zone->container);

   while ((bd = e_container_border_list_next(bl)))
     {
	ic = itask_item_new(it, bd);
	if (ic) itask_item_realize(ic);
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
   _gc_orient(it->gcc, it->gcc->gadcon->orient);
}

void
itask_resize_handle(Itask *it)
{
   Evas_Coord w, h, bw;

   evas_object_geometry_get(it->gcc->gadcon->o_container,
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

   if (it->ci->ibox_style)
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
   Eina_List *itasks = NULL;
   Eina_List *l;
   Itask *it;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	if (it->zone == zone)
	  itasks = eina_list_append(itasks, it);

     }
   return itasks;
}

static Eina_Bool
_itask_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   Eina_List *l, *ll, *itask, *l_items = NULL;
   Itask *it;
   Itask_Item *ic, *ic2;

   ev = event;
   int items_on_desk_in_menu = 0;
   int items = 0;
   int items_on_desk_in_bar = 0;
   itask = itask_zone_find(ev->desk->zone);

   EINA_LIST_FREE(itask, it)
     {
	int i = 0;

	if (!it->ci->show_desk)
	  continue;

	_itask_empty(it);
	_itask_fill(it);

	EINA_LIST_FOREACH(it->items_menu, l, ic)
	  {
	     if (ic->border->desk == ev->desk)
	       {
		  items_on_desk_in_menu++;
		  l_items = eina_list_append(l_items, ic);
	       }
	  }

	if (items_on_desk_in_menu == 0)
	  continue;

	EINA_LIST_FOREACH(it->items_bar, l, ic)
	  {
	     items++;
	     if (ic->border->desk == ev->desk)
	       {
		  items_on_desk_in_bar++;
		  ic->last_time = ecore_time_get();
	       }
	  }
	if (items == items_on_desk_in_bar)
	  {
	     EINA_LIST_FREE(l_items, ic)
	       ic->last_time = ecore_time_get();

	     continue;
	  }

	EINA_LIST_FREE(l_items, ic)
	  {
	     if ((items - items_on_desk_in_bar >= i) &&
		 (i < items_on_desk_in_menu)) i++;
	     else break;

	     EINA_LIST_FOREACH(l_items, ll, ic2)
	       if (ic->last_time < ic2->last_time)
		 ic = ic2;

	     itask_item_swap_to_bar(ic);
	     itask_item_swap_oldest_from_bar(it);
	  }

	EINA_LIST_FREE(l_items, ic)
	  ic->last_time = ecore_time_get();

	itask_resize_handle(it);
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

	if (itask_config->items)
	  {
	     const char *p;
	     ci = eina_list_data_get(eina_list_last(itask_config->items));

	     p = strrchr (ci->id, '.');
	     if (p) num = atoi (p + 1) + 1;
	  }
	snprintf (buf, sizeof (buf), "%s.%d", "itask", num);
	id = buf;
     }
   else
     {
	EINA_LIST_FOREACH(itask_config->items, l, ci)
	  {
	     if (!ci->id)
	       continue;

	     if (!strcmp (ci->id, id))
	       return ci;
	  }
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->show_label = 0;
   ci->show_desk = 0;
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
   Itask *it;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
        if (it->ci != ci)
	  continue;

	_itask_empty(it);
	_itask_fill(it);
	return;
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
     itask_config = E_NEW(Config, 1);

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
   Config_Item *ci;
   Ecore_Event_Handler *h;

   e_gadcon_provider_unregister(&_gadcon_class);

   EINA_LIST_FREE(itask_config->handlers, h)
     ecore_event_handler_del(h);

   EINA_LIST_FREE(itask_config->items, ci)
     {
	if (ci->cfd) e_object_del(E_OBJECT(ci->cfd));

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



