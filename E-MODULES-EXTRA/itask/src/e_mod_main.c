#include "e.h"
#include "e_mod_main.h"

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *itask_config = NULL;
char *itask_theme_path;

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static const char *_gc_id_new (E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);

static Config_Item *itask_config_item_get(const char *id);

static const E_Gadcon_Client_Class _gadcon_class =
  {
    GADCON_CLIENT_CLASS_VERSION, "itask",
    {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL, NULL
    },
    E_GADCON_CLIENT_STYLE_PLAIN
  };

static Eina_Bool
_cb_timer(void *data)
{
   itask_reload(data);
   
   return ECORE_CALLBACK_CANCEL;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Itask *it;
   E_Gadcon_Client *gcc;

   it = E_NEW(Itask, 1);
   it->o_box = e_box_add(gc->evas);
   it->zone = gc->zone;
   it->ci = itask_config_item_get(id);
   it->ci->it = it;
   it->item_height = 32;
   it->item_width = 200;
   it->horizontal = EINA_TRUE;
   e_box_homogenous_set(it->o_box, 0);

   gcc = e_gadcon_client_new(gc, name, id, style, it->o_box);
   gcc->data = it;
   it->gcc = gcc;

   itask_config->instances = eina_list_append(itask_config->instances, it);

   itask_menu_button(it);

   ecore_timer_add(0.5, _cb_timer, it);
   /* itask_reload(it); */

   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Itask *it;

   it = gcc->data;
   itask_config->instances = eina_list_remove(itask_config->instances, it);

   while (it->items)
     itask_item_free(it->items->data);

   itask_menu_remove(it);

   evas_object_del(it->o_box);

   if (it->idler)
     ecore_idle_enterer_del(it->idler);

   free(it);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   Itask *it = gcc->data;

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

	 e_box_orientation_set(it->o_box, 1);
	 e_box_align_set(it->o_box, 0.0, 0.5);

	 if (it->horizontal)
	   return;

	 it->horizontal = EINA_TRUE;
	 itask_reload(it);
	 break;

      case E_GADCON_ORIENT_VERT:
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_RIGHT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_LB:
      case E_GADCON_ORIENT_CORNER_RB:

	 e_box_orientation_set(it->o_box, 0);
	 e_box_align_set(it->o_box, 0.5, 0.0);
	 it->horizontal = EINA_FALSE;

	 if (it->horizontal)
	   return;

	 it->horizontal = EINA_FALSE;
	 itask_reload(it);
	 break;
     }
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

static int
_get_max(Itask *it)
{
   E_Gadcon_Client *gcc;
   int x, y, w, h;
   int xx, yy, mx, my;
   Eina_List *l;
   E_Gadcon *gc = it->gcc->gadcon;

   evas_object_geometry_get(gc->o_container, NULL,NULL, &w, &h);
   evas_object_geometry_get(it->o_box, &xx, &yy, NULL, NULL);

   mx = w;
   my = h;

   EINA_LIST_FOREACH(it->gcc->gadcon->clients, l, gcc)
     {
	if (gcc->o_frame)
	  evas_object_geometry_get(gcc->o_frame, &x, &y, NULL, NULL);
	else if (gcc->o_base)
	  evas_object_geometry_get(gcc->o_base, &x, &y, NULL, NULL);

	if ((xx < x) && (x < mx))
	  mx = x;

	if ((yy < y) && (y < my))
	  my = y;
     }
   mx -= xx;
   my -= yy;

   return it->horizontal ? mx : my;
}

static Eina_Bool
_cb_itask_update(void *data)
{
   Itask *it = data;
   Eina_List *l;
   Itask_Item *ic;
   int w, h, bw, bh, max, cnt;

   it->idler = NULL;

   e_box_freeze(it->o_box);

   EINA_LIST_FOREACH(it->items, l, ic)
     {
	evas_object_hide(ic->o_holder);
	e_box_unpack(ic->o_holder);
     }

   evas_object_geometry_get(it->o_box, NULL,NULL, &bw, &bh);

   if (it->horizontal)
     e_box_pack_options_set(it->o_button,
			    0, 0, 0, 0, 0.5, 0.5,
			    bh, bh, bh, bh);
   else
     e_box_pack_options_set(it->o_button,
			    0, 0, 0, 0, 0.5, 0.5,
			    bw, bw, bh, bh);

   EINA_LIST_FOREACH(it->items, l, ic)
     {
	e_box_pack_end(it->o_box, ic->o_holder);
	e_box_pack_options_set(ic->o_holder,
			       1, 1, 1, 1, 0.0, 0.5,
			       16, 16, -1, -1);
	evas_object_show(ic->o_holder);
     }

   e_box_thaw(it->o_box);

   cnt = eina_list_count(it->items);

   if (cnt == 0)
     {
	e_gadcon_client_size_request(it->gcc, 16, 16);
   	e_gadcon_client_aspect_set(it->gcc, 16, 16);
     }
   else if (it->horizontal)
       {
	  if (it->ci->ibox_style)
	    w = cnt * bh + bh;
	  else
	    w = cnt * it->item_width + bh;

	  max = _get_max(it);
	  if (w > max) w = max;

	  e_gadcon_client_size_request(it->gcc, w, bh);
	  e_gadcon_client_aspect_set(it->gcc, w, bh);
       }
     else
       {
	  if (it->ci->ibox_style)
	    h = cnt * bw + bw;
	  else
	    h = cnt * it->item_height + bw;

	  max = _get_max(it);
	  if (h > max) h = max;

	  e_gadcon_client_size_request(it->gcc, bw, h);
	  e_gadcon_client_aspect_set(it->gcc, bw, h);
       }

   return ECORE_CALLBACK_CANCEL;
}

void
itask_update(Itask *it)
{
   if (it->idler) return;

   it->idler = ecore_idle_enterer_add(_cb_itask_update, it);
}

void
itask_reload(Itask *it)
{
   E_Border *bd;
   E_Border_List *bl;

   while (it->items)
     itask_item_free(it->items->data);

   bl = e_container_border_list_first(it->zone->container);

   while ((bd = e_container_border_list_next(bl)))
     itask_item_new(it, bd);

   e_container_border_list_free(bl);

   itask_update(it);
}

void
itask_update_gc_orient(Itask *it)
{
   _gc_orient(it->gcc, it->gcc->gadcon->orient);
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
