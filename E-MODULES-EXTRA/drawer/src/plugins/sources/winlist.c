#include "winlist.h"
#include <e.h>
#ifdef HAVE_XPIXMAP
#include <Esmart/Esmart_XPixmap.h>
#endif

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Conf Conf;

struct _Instance 
{
   Drawer_Source *source;

   Eina_List *items, *handlers;

   Conf *conf;

   E_Menu *menu;

   struct
     {
	E_Config_DD *conf;
     } edd;

   struct
     {
	Eina_Bool changed;
	E_Border *removed, *added;
     } actions;

   const char *description;
};

struct _Conf
{
   const char *id;
};

static void _winlist_description_create(Instance *inst);

static Drawer_Source_Item *_winlist_source_item_fill(Instance *inst, E_Border *bd);
static void _winlist_source_items_free(Instance *inst);
static void _winlist_source_item_free(Drawer_Source_Item *si);
static Drawer_Source_Item * _winlist_item_from_border(Instance *inst, E_Border *bd);
static Eina_Bool _winlist_border_filter(E_Border *bd, E_Zone *zone);
static Eina_Bool _winlist_items_update(Instance *inst);
static void _winlist_border_focus_in(Instance *inst, E_Border *bd);
static void _winlist_event_update(Instance *inst);
static int  _winlist_sort_alpha_cb(const void *d1, const void *d2);
static int  _winlist_sort_z_order_cb(const void *d1, const void *d2);
static Eina_Bool  _winlist_border_focus_in_cb(void *data, int type, void *event);
static Eina_Bool  _winlist_border_focus_out_cb(void *data, int type, void *event);
static Eina_Bool  _winlist_border_add_cb(void *data, int type, void *event);
static Eina_Bool  _winlist_border_remove_cb(void *data, int type, void *event);
static Eina_Bool  _winlist_border_icon_change_cb(void *data, int type, void *event);
static Eina_Bool  _winlist_border_desk_set_cb(void *data, int type, void *event);
static Eina_Bool  _winlist_border_window_prop_cb(void *data, int type, void *event);
/*
static int  _winlist_desk_show_cb(void *data, int type, void *event);
*/
static void _winlist_event_update_free(void *data __UNUSED__, void *event);
static void _winlist_event_update_icon_free(void *data __UNUSED__, void *event);

EAPI Drawer_Plugin_Api drawer_plugin_api = {DRAWER_PLUGIN_API_VERSION, "Winlist"};

EAPI void *
drawer_plugin_init(Drawer_Plugin *p, const char *id)
{
   Instance *inst = NULL;
   E_Border *bd;
   char buf[128];

   inst = E_NEW(Instance, 1);

   inst->source = DRAWER_SOURCE(p);

   /* Define EET Data Storage */
   inst->edd.conf = E_CONFIG_DD_NEW("Conf", Conf);
   #undef T
   #undef D
   #define T Conf
   #define D inst->edd.conf
   E_CONFIG_VAL(D, T, id, STR);

   snprintf(buf, sizeof(buf), "module.drawer/%s.winlist", id);
   inst->conf = e_config_domain_load(buf, inst->edd.conf);
   if (!inst->conf)
     {
	inst->conf = E_NEW(Conf, 1);
	inst->conf->id = eina_stringshare_add(id);

	e_config_save_queue();
     }

   inst->handlers = eina_list_append(
       inst->handlers, ecore_event_handler_add(
	   E_EVENT_BORDER_FOCUS_IN, _winlist_border_focus_in_cb, inst));
   inst->handlers = eina_list_append(
       inst->handlers, ecore_event_handler_add(
	   E_EVENT_BORDER_FOCUS_OUT, _winlist_border_focus_out_cb, inst));
   inst->handlers = eina_list_append(
       inst->handlers, ecore_event_handler_add(
	   E_EVENT_BORDER_ADD, _winlist_border_add_cb, inst));
   inst->handlers = eina_list_append(
       inst->handlers, ecore_event_handler_add(
	   E_EVENT_BORDER_REMOVE, _winlist_border_remove_cb, inst));
   inst->handlers = eina_list_append(
       inst->handlers, ecore_event_handler_add(
	   E_EVENT_BORDER_ICON_CHANGE, _winlist_border_icon_change_cb, inst));
   inst->handlers = eina_list_append(
       inst->handlers, ecore_event_handler_add(
	   E_EVENT_BORDER_DESK_SET, _winlist_border_desk_set_cb, inst));
   inst->handlers = eina_list_append(
       inst->handlers, ecore_event_handler_add(
	   ECORE_X_EVENT_WINDOW_PROPERTY, _winlist_border_window_prop_cb, inst));
   /*
   inst->handlers = eina_list_append(
       inst->handlers, ecore_event_handler_add(
	   E_EVENT_DESK_SHOW, _winlist_desk_show_cb, inst));
   */

   _winlist_description_create(inst);
   _winlist_items_update(inst);

   bd = e_border_focused_get();
   if (bd)
     _winlist_border_focus_in(inst, bd);

   return inst;
}

EAPI int
drawer_plugin_shutdown(Drawer_Plugin *p)
{
   Instance *inst = NULL;
   
   inst = p->data;

   _winlist_source_items_free(inst);

   eina_stringshare_del(inst->description);
   eina_stringshare_del(inst->conf->id);

   E_CONFIG_DD_FREE(inst->edd.conf);
   E_FREE_LIST(inst->handlers, ecore_event_handler_del);
   E_FREE(inst->conf);
   E_FREE(inst);

   return 1;
}

EAPI void
drawer_plugin_config_save(Drawer_Plugin *p)
{
   Instance *inst;
   char buf[128];

   inst = p->data;
   snprintf(buf, sizeof(buf), "module.drawer/%s.winlist", inst->conf->id);
   e_config_domain_save(buf, inst->edd.conf, inst->conf);
}

EAPI Eina_List *
drawer_source_list(Drawer_Source *s)
{
   Instance *inst;

   if (!(inst = DRAWER_PLUGIN(s)->data)) return NULL;

   return inst->items;
}

EAPI void
drawer_source_activate(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone)
{
   E_Border *bd = si->data;

   E_OBJECT_CHECK(bd);
   if (bd->iconic)
     {
	if (!bd->lock_user_iconify)
	  e_border_uniconify(bd);
     }
   e_desk_show(bd->desk);
   if (!bd->lock_user_stacking)
     e_border_raise(bd);
   if (!bd->lock_focus_out)
     {
	if (e_config->focus_policy != E_FOCUS_CLICK)
	  e_border_pointer_warp_to_center(bd);
	e_border_focus_set(bd, 1, 1);
     }
}

EAPI const char *
drawer_source_description_get(Drawer_Source *s)
{
   Instance *inst;

   inst = DRAWER_PLUGIN(s)->data;

   return inst->description;
}

EAPI void
drawer_source_trigger(Drawer_Source *s, E_Zone *zone)
{
   E_Border *bd = e_border_focused_get();
   Drawer_Source_Item *si;

   if (bd)
     {
	si = _winlist_item_from_border(DRAWER_PLUGIN(s)->data, bd);
	if (si)
	  s->func.activate(s, si, zone);
     }
}

EAPI Evas_Object *
drawer_source_render_item(Drawer_Source *s, Drawer_Source_Item *si, Evas *evas)
{
   Evas_Object *o = NULL;
#ifdef HAVE_XPIXMAP
   Evas_Coord w, h;

   o = esmart_xpixmap_new(evas, 0, ((E_Border *) si->data)->win);
   //Evas_Object *ic = e_icon_add(evas);

   evas_object_size_hint_request_get(o, &w, &h);
   evas_object_resize(o, w, h);
   evas_object_show(o);
#else

   return e_border_icon_add(si->data, evas);
#endif

   //e_icon_object_set(ic, o);
   return o;
}

static void
_winlist_description_create(Instance *inst)
{
   eina_stringshare_replace(&(inst->description), "Applications");
}

static Drawer_Source_Item *
_winlist_source_item_fill(Instance *inst, E_Border *bd)
{
   Drawer_Source_Item *si = NULL;
   char buf[512];

   si = E_NEW(Drawer_Source_Item, 1);

   e_object_ref(E_OBJECT(bd));
   si->data = bd;
   si->data_type = SOURCE_DATA_TYPE_OTHER;
   si->label = eina_stringshare_add(e_border_name_get(bd));
   switch (e_config->clientlist_group_by)
     {
      case E_CLIENTLIST_GROUP_CLASS:
	 si->category = eina_stringshare_add(bd->client.icccm.class);
	 snprintf(buf, sizeof(buf), "%s - %s", bd->desk->name, bd->client.icccm.class);
	 si->description = eina_stringshare_add(buf);
	 break;
      case E_CLIENTLIST_GROUP_DESK:
	 si->category = eina_stringshare_add(bd->desk->name);
	 snprintf(buf, sizeof(buf), "%s - %s", bd->client.icccm.class, bd->desk->name);
	 si->description = eina_stringshare_add(buf);
	 break;
     }

   si->source = inst->source;

   return si;
}

static void
_winlist_source_items_free(Instance *inst)
{
   Drawer_Source_Item *si;

   EINA_LIST_FREE(inst->items, si)
      _winlist_source_item_free(si);
}

static void
_winlist_source_item_free(Drawer_Source_Item *si)
{
   e_object_unref(E_OBJECT(si->data));
   eina_stringshare_del(si->label);
   eina_stringshare_del(si->description);
   eina_stringshare_del(si->category);

   free(si);
}

static Drawer_Source_Item *
_winlist_item_from_border(Instance *inst, E_Border *bd)
{
   Eina_List *l;
   Drawer_Source_Item *si;

   if (!inst->items) return NULL;
   EINA_LIST_FOREACH(inst->items, l, si)
      if (si->data == bd) return si;

   return NULL;
}

static Eina_Bool
_winlist_border_filter(E_Border *bd, E_Zone *zone)
{
   if (bd->client.netwm.state.skip_taskbar) return EINA_TRUE;
   if (bd->user_skip_winlist) return EINA_TRUE;
   if ((bd->zone != zone) && (!bd->iconic)) return EINA_TRUE;

   return EINA_FALSE;
}

static Eina_Bool
_winlist_items_update(Instance *inst)
{
   E_Zone *zone = e_util_zone_current_get(e_manager_current_get());
   Eina_List *l;
   E_Border *bd;
   Eina_Bool ret = EINA_TRUE;

   if (inst->actions.removed)
     {
	Drawer_Source_Item *si;

	ret = EINA_FALSE;
	si = _winlist_item_from_border(inst, inst->actions.removed);
	if (si)
	  {
	     inst->items = eina_list_remove(inst->items, si);
	     _winlist_source_item_free(si);

	     ret = EINA_TRUE;
	  }
	inst->actions.removed = NULL;
     }
   else if (inst->actions.added)
     {
	Drawer_Source_Item *si = NULL;

	bd = inst->actions.added;
	inst->actions.added = NULL;
	si = _winlist_source_item_fill(inst, bd);
	inst->items = eina_list_append(inst->items, si);
     }
   else if (!inst->actions.changed)
     {
	_winlist_source_items_free(inst);

	EINA_LIST_FOREACH(e_border_client_list(), l, bd)
	  {
	     Drawer_Source_Item *si = NULL;

	     if (_winlist_border_filter(bd, zone)) continue;
	     si = _winlist_source_item_fill(inst, bd);
	     inst->items = eina_list_append(inst->items, si);
	  }
     }

   if (inst->items)
     {
	switch (e_config->clientlist_sort_by)
	  {
	   case E_CLIENTLIST_SORT_ALPHA:
	      inst->items = eina_list_sort(inst->items, eina_list_count(inst->items),
					   _winlist_sort_alpha_cb);
	      break;
	   case E_CLIENTLIST_SORT_ZORDER:
	      inst->items = eina_list_sort(inst->items, eina_list_count(inst->items),
					   _winlist_sort_z_order_cb);
	      break;
	  }
     }

   return ret;
}

static void
_winlist_border_focus_in(Instance *inst, E_Border *bd)
{
   Eina_List *l;
   Drawer_Source_Item *si;

   if (inst->items)
     EINA_LIST_FOREACH(inst->items, l, si)
       {
	  if (si->data == bd)
	    {
	       Drawer_Event_Source_Main_Icon_Update *ev;

	       ev = E_NEW(Drawer_Event_Source_Main_Icon_Update, 1);
	       ev->source = inst->source;
	       ev->id = eina_stringshare_add(inst->conf->id);
	       ev->si = si;
	       ecore_event_add(
		   DRAWER_EVENT_SOURCE_MAIN_ICON_UPDATE, ev,
		   _winlist_event_update_icon_free, NULL);
	       return;
	    }
       }

   if (_winlist_border_filter(bd, e_util_zone_current_get(e_manager_current_get()))) return;
   inst->actions.added = bd;
   _winlist_items_update(inst);
}

static void
_winlist_event_update(Instance *inst)
{
   Drawer_Event_Source_Update *ev;

   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
   ev->id = eina_stringshare_add(inst->conf->id);
   ecore_event_add(DRAWER_EVENT_SOURCE_UPDATE, ev, _winlist_event_update_free, NULL);
}

static int
_winlist_sort_alpha_cb(const void *d1, const void *d2)
{
   const Drawer_Source_Item *si1, *si2;
   const char *name1, *name2;
   
   if (!d1) return 1;
   if (!d2) return -1;

   si1 = d1;
   si2 = d2;
   name1 = e_border_name_get(si1->data);
   name2 = e_border_name_get(si2->data);
   
   if (strcasecmp(name1, name2) > 0) return 1;
   if (strcasecmp(name1, name2) < 0) return -1;
   return 0;
}

static int
_winlist_sort_z_order_cb(const void *d1, const void *d2)
{
   const Drawer_Source_Item *si1, *si2;
   E_Border *bd1, *bd2;

   if (!d1) return 1;
   if (!d2) return -1;

   si1 = d1;
   si2 = d2;
   bd1 = si1->data;
   bd2 = si2->data;

   if (bd1->layer < bd2->layer) return 1;
   if (bd1->layer > bd2->layer) return -1;
   return 0;   
}

static Eina_Bool
_winlist_border_focus_in_cb(void *data, int type, void *event)
{
   E_Event_Border_Focus_In *ev;
   E_Border *bd;
   Instance *inst;

   ev = event;
   inst = data;
   if (!(bd = ev->border))
     return EINA_TRUE;

   _winlist_border_focus_in(inst, bd);

   return EINA_TRUE;
}

static Eina_Bool
_winlist_border_focus_out_cb(void *data, int type, void *event)
{
   E_Event_Border_Focus_Out *ev;
   Instance *inst;
   Drawer_Event_Source_Main_Icon_Update *ev2;

   ev = event;
   inst = data;
   if (!ev->border) return EINA_TRUE;

   ev2 = E_NEW(Drawer_Event_Source_Main_Icon_Update, 1);
   ev2->source = inst->source;
   ev2->id = eina_stringshare_add(inst->conf->id);
   ev2->si = NULL;
   ecore_event_add(
       DRAWER_EVENT_SOURCE_MAIN_ICON_UPDATE, ev2,
       _winlist_event_update_icon_free, NULL);

   return EINA_TRUE;
}

static Eina_Bool
_winlist_border_add_cb(void *data, int type, void *event)
{
   E_Event_Border_Add *evt;
   E_Border *bd;
   Instance *inst;
   E_Zone *zone = e_util_zone_current_get(e_manager_current_get());

   evt = event;
   inst = data;
   if (!(bd = evt->border)) return EINA_TRUE;
   if (_winlist_border_filter(bd, zone)) return EINA_TRUE;

   inst->actions.added = bd;
   if (_winlist_items_update(inst))
     _winlist_event_update(inst);

   return EINA_TRUE;
}

static Eina_Bool
_winlist_border_remove_cb(void *data, int type, void *event)
{
   E_Event_Border_Remove *evt;
   E_Border *bd;
   Instance *inst;
   E_Zone *zone = e_util_zone_current_get(e_manager_current_get());

   evt = event;
   inst = data;
   if (!(bd = evt->border)) return EINA_TRUE;
   if (_winlist_border_filter(bd, zone)) return EINA_TRUE;

   inst->actions.removed = bd;
   if (_winlist_items_update(inst))
     _winlist_event_update(inst);

   return EINA_TRUE;
}

static Eina_Bool
_winlist_border_icon_change_cb(void *data, int type, void *event)
{
   E_Event_Border_Icon_Change *ev;
   Instance *inst;

   ev = event;
   inst = data;
   if (!ev->border) return EINA_TRUE;

   _winlist_event_update(inst);

   return EINA_TRUE;
}

static Eina_Bool
_winlist_border_desk_set_cb(void *data, int type, void *event)
{
   E_Event_Border_Icon_Change *ev;
   E_Border *bd;
   Instance *inst;
   Drawer_Source_Item *si;
   Eina_Bool changed = EINA_FALSE;
   char buf[512];

   ev = event;
   inst = data;
   if (!(bd = ev->border)) return EINA_TRUE;

   si = _winlist_item_from_border(inst, bd);
   if (si)
     {
	switch (e_config->clientlist_group_by)
	  {
	   case E_CLIENTLIST_GROUP_CLASS:
	      snprintf(buf, sizeof(buf), "%s - %s", bd->desk->name, bd->client.icccm.class);
	      if (strcmp(si->description, buf))
		{
		   eina_stringshare_replace(&(si->description), buf);
		   changed = EINA_TRUE;
		}
	      break;
	   case E_CLIENTLIST_GROUP_DESK:
	      snprintf(buf, sizeof(buf), "%s - %s", bd->client.icccm.class, bd->desk->name);
	      if (strcmp(si->description, buf))
		{
		   eina_stringshare_replace(&(si->description), buf);
		   changed = EINA_TRUE;
		}
	      break;
	  }
     }

   if (changed)
     _winlist_event_update(inst);

   return EINA_TRUE;
}

static Eina_Bool
_winlist_border_window_prop_cb(void *data, int type, void *event)
{
   E_Border *bd;
   Ecore_X_Event_Window_Property *ev;
   Instance *inst;
   Eina_Bool changed = EINA_FALSE;
   E_Zone *zone = e_util_zone_current_get(e_manager_current_get());
   Drawer_Source_Item *si;
   char *tmp;

   ev = event;
   inst = data;
   bd = e_border_find_by_client_window(ev->win);
   if (!bd) return EINA_TRUE;
   if (_winlist_border_filter(bd, zone)) return EINA_TRUE;
   if (ev->atom == ECORE_X_ATOM_WM_NAME)
     {
	if ((!bd->client.netwm.name) &&
	    (!bd->client.netwm.fetch.name))
	  {
	     si = _winlist_item_from_border(inst, bd);
	     if (si)
	       {
		  tmp = ecore_x_icccm_title_get(bd->client.win);

                  if (tmp)
                    {
                       if (strcmp(si->label, tmp))
                         {
                            eina_stringshare_replace(&(si->label), tmp);
                            changed = EINA_TRUE;
                         }
                       free(tmp);
                    }
	       }
	  }
     }
   else if (ev->atom == ECORE_X_ATOM_NET_WM_NAME)
     {
	si = _winlist_item_from_border(inst, bd);
	if (si)
	  {
	     ecore_x_netwm_name_get(bd->client.win, &tmp);
             if (tmp)
               {
                  if (strcmp(si->label, tmp))
                    {
                       eina_stringshare_replace(&(si->label), tmp);
                       changed = EINA_TRUE;
                    }
                  free(tmp);
               }
	  }
     }
   else if (ev->atom == ECORE_X_ATOM_WM_CLASS)
     {
	char buf[512];
	si = _winlist_item_from_border(inst, bd);
	if (si)
	  {
	     ecore_x_icccm_name_class_get(bd->client.win, NULL, &tmp);
	     switch (e_config->clientlist_group_by)
	       {
		case E_CLIENTLIST_GROUP_CLASS:
		   snprintf(buf, sizeof(buf), "%s - %s", bd->desk->name, tmp);
		   if (strcmp(si->description, buf))
		     {
			eina_stringshare_replace(&(si->description), buf);
			changed = EINA_TRUE;
		     }
		   break;
		case E_CLIENTLIST_GROUP_DESK:
		   snprintf(buf, sizeof(buf), "%s - %s", tmp, bd->desk->name);
		   if (strcmp(si->description, buf))
		     {
			eina_stringshare_replace(&(si->description), buf);
			changed = EINA_TRUE;
		     }
		   break;
	       }
	  }
     }

   if (changed)
     _winlist_event_update(inst);

   return EINA_TRUE;
}

/*
static int
_winlist_desk_show_cb(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   Instance *inst;

   ev = event;
   inst = data;

   inst->actions.changed = EINA_TRUE;
   if (_winlist_items_update(inst))
     _winlist_event_update(inst);

   return 1;
}
*/

static void
_winlist_event_update_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_Source_Update *ev;

   ev = event;
   eina_stringshare_del(ev->id);
   free(ev);
}

static void
_winlist_event_update_icon_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_Source_Main_Icon_Update *ev;

   ev = event;
   eina_stringshare_del(ev->id);
   free(ev);
}
