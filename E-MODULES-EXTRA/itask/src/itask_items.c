#include "e.h"
#include "e_mod_main.h"

static Eina_Bool  _itask_items_cb_event_border_add(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_border_remove(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_border_iconify(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_border_uniconify(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_border_icon_change(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_border_property(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_border_zone_set(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_border_focus_in(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_border_focus_out(void *data, int type, void *event);
static Eina_Bool  _itask_items_cb_event_desk_show(void *data, int type, void *event);

void
itask_items_init(Config *itask_config)
{

   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_ADD, _itask_items_cb_event_border_add, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_REMOVE, _itask_items_cb_event_border_remove, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_ICONIFY, _itask_items_cb_event_border_iconify, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_UNICONIFY, _itask_items_cb_event_border_uniconify, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_ICON_CHANGE, _itask_items_cb_event_border_icon_change, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_PROPERTY, _itask_items_cb_event_border_property, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_ZONE_SET, _itask_items_cb_event_border_zone_set, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_FOCUS_IN, _itask_items_cb_event_border_focus_in, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_BORDER_FOCUS_OUT, _itask_items_cb_event_border_focus_out, NULL));
   itask_config->handlers = eina_list_append(itask_config->handlers, ecore_event_handler_add
					     (E_EVENT_DESK_SHOW, _itask_items_cb_event_desk_show, NULL));
}

static Eina_Bool
_itask_items_cb_event_border_add(void *data, int type, void *event)
{
   E_Event_Border_Add *ev;
   E_Border *bd;
   Itask *it;
   Eina_List *l;

   ev = event;
   bd = ev->border;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	itask_item_new(it, bd);
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_border_remove(void *data, int type, void *event)
{
   E_Event_Border_Remove *ev;
   Itask *it;
   Itask_Item *ic;
   Eina_List *l, *ll;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	EINA_LIST_FOREACH(it->items, ll, ic)
	  {
	     if (ic->border == ev->border)
	       itask_item_free(ic);
	  }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_border_uniconify(void *data, int type, void *event)
{
   E_Event_Border_Iconify *ev;
   Itask *it;
   Itask_Item *ic;
   Eina_List *l, *ll;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	EINA_LIST_FOREACH(it->items, ll, ic)
	  {
	     if (ic->border == ev->border)
	       itask_icon_signal_emit(ic, "uniconify", "");
	  }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_border_iconify(void *data, int type, void *event)
{
   E_Event_Border_Uniconify *ev;
   Itask *it;
   Itask_Item *ic;
   Eina_List *l, *ll;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	EINA_LIST_FOREACH(it->items, ll, ic)
	  {
	     if (ic->border == ev->border)
	       itask_icon_signal_emit(ic, "iconify", "");
	  }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_border_icon_change(void *data, int type, void *event)
{
   E_Event_Border_Icon_Change *ev;
   Itask *it;
   Itask_Item *ic;
   Eina_List *l, *ll;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	EINA_LIST_FOREACH(it->items, ll, ic)
	  {
	     if (ic->border == ev->border)
	       itask_item_set_icon(ic);
	  }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_border_focus_in(void *data, int type, void *event)
{
   E_Event_Border_Focus_In *ev;
   Itask *it;
   Itask_Item *ic;
   Eina_List *l, *ll;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	EINA_LIST_FOREACH(it->items, ll, ic)
	  {
	     if (ic->border == ev->border)
	       itask_icon_signal_emit(ic, "focused", "");
	  }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_border_focus_out(void *data, int type, void *event)
{
   E_Event_Border_Focus_Out *ev;
   Itask *it;
   Itask_Item *ic;
   Eina_List *l, *ll;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	EINA_LIST_FOREACH(it->items, ll, ic)
	  {
	     if (ic->border == ev->border)
	       itask_icon_signal_emit(ic, "unfocused", "");
	  }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_border_property(void *data, int type, void *event)
{
   E_Event_Border_Property *ev;
   Itask *it;
   Itask_Item *ic;
   Eina_List *l, *ll;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	EINA_LIST_FOREACH(it->items, ll, ic)
	  if (ic->border == ev->border)
	    break;

	if (!ic)
	  {
	     itask_item_new(it, ev->border);
	  }
	else if (!itask_item_add_check(it, ev->border))
	  {
	     itask_item_free(ic);
	  }
	else
	  {
	     itask_item_set_label(ic);
	  }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_border_zone_set(void *data, int type, void *event)
{
   E_Event_Border_Zone_Set *ev;
   Itask *it;
   Itask_Item *ic;
   Eina_List *l, *ll;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	if (it->zone == ev->zone)
	  {
	     itask_item_new(it, ev->border);
	  }
	else
	  {
	     EINA_LIST_FOREACH(it->items, ll, ic)
	       if (ic->border == ev->border)
		 break;

	     if (ic)
	       itask_item_free(ic);
	  }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_itask_items_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   Eina_List *l;
   Itask *it;

   ev = event;

   EINA_LIST_FOREACH(itask_config->instances, l, it)
     {
	if (ev->desk->zone != it->zone)
	  continue;

	if (it->ci->show_desk)
	  itask_reload(it);
     }

   return ECORE_CALLBACK_PASS_ON;
}
