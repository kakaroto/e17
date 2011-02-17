#include "e_mod_main.h"
#include <string.h>

static void             _ngi_gadcon_cb_gadcon_min_size_request(void *data, E_Gadcon *gc, Evas_Coord w, Evas_Coord h);
static void             _ngi_gadcon_cb_gadcon_size_request(void *data, E_Gadcon *gc, Evas_Coord w, Evas_Coord h);
static Evas_Object *    _ngi_gadcon_cb_gadcon_frame_request(void *data, E_Gadcon_Client *gcc, const char *style);
static void *           _create_data(E_Config_Dialog *cfd);
static void             _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *    _basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static Eina_Bool        _cb_mod_update(void *data, int type, void *event);
static void             _avail_list_cb_change(void *data, Evas_Object *obj);
static void             _sel_list_cb_change(void *data, Evas_Object *obj);
static void             _load_avail_gadgets(void *data);
static void             _load_sel_gadgets(void *data);
static void             _cb_add(void *data, void *data2);
static void             _cb_del(void *data, void *data2);
static void             _set_description(void *data, const char *name);

static Eina_Hash *ngi_gadcon_hash = NULL;

void
ngi_gadcon_init(void)
{
   Eina_List *l, *ll, *lll;
   Config_Item *ci;
   Config_Box *cb;
   Config_Gadcon *cg;

   ngi_gadcon_hash = eina_hash_string_superfast_new(NULL);

   EINA_LIST_FOREACH (ngi_config->items, l, ci)
     {
	EINA_LIST_FOREACH (ci->boxes, ll, cb)
	  {
	     if (cb->type != gadcon)
	       continue;

	     EINA_LIST_FOREACH(cb->gadcon_items, lll, cg)
	       eina_hash_add(ngi_gadcon_hash, cg->name, cg);
	  }
     }
}

void
ngi_gadcon_shutdown(void)
{
   if (ngi_gadcon_hash)
     eina_hash_free(ngi_gadcon_hash);
}

static char *
_ngi_gadcon_name_new()
{
   char name[256];
   int cnt = 0;

   while(cnt > 1000)
     {
        snprintf(name, 256, "ng_gadcon-%d", cnt++);

        if (!eina_hash_find(ngi_gadcon_hash, name))
	  return strdup(name);
     }
   return NULL;
}

static void
_ngi_gadcon_item_cb_mouse_down(Ngi_Item *it, Ecore_Event_Mouse_Button *ev)
{
}

static void
_ngi_gadcon_item_cb_mouse_up(Ngi_Item *it, Ecore_Event_Mouse_Button *ev)
{
}

static void
_ngi_gadcon_item_cb_mouse_in(Ngi_Item *it)
{
}

static void
_ngi_gadcon_item_cb_mouse_out(Ngi_Item *it)
{
}

static void
_ngi_gadcon_locked_set(void *data, int lock)
{
   Ng *ng = data;

   ngi_bar_lock(ng, lock);
   ngi_animate(ng);
}

static Ngi_Item *
_ngi_gadcon_item_new(Ngi_Box *box, const char *name, Ngi_Item *after)
{
   Ngi_Item *it;
   Ng *ng = box->ng;

   int instant = 1;

   it = ngi_item_new(box);
   it->type = gadcon_item;

   it->cb_mouse_in = _ngi_gadcon_item_cb_mouse_in;
   it->cb_mouse_out = _ngi_gadcon_item_cb_mouse_out;
   it->cb_mouse_down = _ngi_gadcon_item_cb_mouse_down;
   it->cb_mouse_up = _ngi_gadcon_item_cb_mouse_up;
   it->cb_drag_start = NULL; //_ngi_gadcon_item_cb_drag_start;

   evas_object_show(it->obj);
   it->gadcon = e_gadcon_swallowed_new(name, 0, it->obj, "e.swallow.content");
   it->gadcon->instant_edit = 0;
   edje_extern_object_min_size_set(it->gadcon->o_container, ng->size, ng->size);

   e_gadcon_min_size_request_callback_set(it->gadcon, _ngi_gadcon_cb_gadcon_min_size_request, it);
   e_gadcon_size_request_callback_set(it->gadcon, _ngi_gadcon_cb_gadcon_size_request, it);
   e_gadcon_frame_request_callback_set(it->gadcon, _ngi_gadcon_cb_gadcon_frame_request, it);
   e_gadcon_orient(it->gadcon, ng->cfg->orient);
   e_gadcon_zone_set(it->gadcon, box->ng->zone);
   e_gadcon_ecore_evas_set(it->gadcon, box->ng->win->popup->ecore_evas);
   e_gadcon_util_lock_func_set(it->gadcon, _ngi_gadcon_locked_set, ng);

   it->label = NULL; //name;

   if (after)
     box->items = eina_list_prepend_relative(box->items, it, after);
   else
     box->items = eina_list_append(box->items, it);

   ngi_item_show(it, instant);

   e_gadcon_populate(it->gadcon);

   if(it->gadcon->clients)
     {
        char buf[256];
        E_Gadcon_Client *gcc = it->gadcon->clients->data;
        snprintf(buf, 256, "%s-%d", gcc->name, gcc->id);
        it->label = e_datastore_get(buf);
     }

   it->o_icon2 = e_icon_add(it->box->ng->evas);

   Evas_Object *o = evas_object_image_add(it->box->ng->evas);
   evas_object_image_source_set(o, it->obj);
   e_icon_object_set(it->o_icon2, o);

   edje_object_part_swallow(it->over, "e.swallow.content", it->o_icon2);
   evas_object_pass_events_set(it->o_icon2, 1);
   evas_object_show(it->o_icon2);

   return it;
}

void
ngi_gadcon_new(Ng *ng, Config_Box *cfg)
{
   Ngi_Box *box = ngi_box_new(ng);
   if(!box)
     return;

   box->cfg = cfg;
   cfg->box = box;

   if(cfg->gadcon_items)
     {
        Eina_List *l;
        Config_Gadcon *cg;
        Ngi_Item *it;

        EINA_LIST_FOREACH (cfg->gadcon_items, l, cg)
	  {
	     it = _ngi_gadcon_item_new(box, cg->name, NULL);
	     it->cfg_gadcon = cg;
	  }
     }
}

void
ngi_gadcon_remove(Ngi_Box *box)
{
   ngi_box_free(box);
}

static void
_ngi_gadcon_cb_gadcon_min_size_request(void *data, E_Gadcon *gc, Evas_Coord w, Evas_Coord h)
{
   Ngi_Item *it;

   it = data;

   return;
}

static void
_ngi_gadcon_cb_gadcon_size_request(void *data, E_Gadcon *gc, Evas_Coord w, Evas_Coord h)
{
   Ngi_Item *it;

   it = data;

   return;
}

static Evas_Object *
_ngi_gadcon_cb_gadcon_frame_request(void *data, E_Gadcon_Client *gcc, const char *style)
{
   /* FIXME: provide an inset look edje thing */
   return NULL;
}

struct _E_Config_Dialog_Data
{
  Evas_Object         *o_avail, *o_sel;
  Evas_Object         *o_add, *o_del;
  Evas_Object         *o_desc;

  E_Gadcon            *gc;

  Ngi_Box             *box;

  Ecore_Event_Handler *hdl;
};

void
ngi_gadcon_config(Ngi_Box *box)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v)
     return;

   con = e_container_current_get(e_manager_current_get());

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;

   cfd = e_config_dialog_new(con, "Gadcon Config", "E", "_ngi_gadcon_config_dialog",
                             "enlightenment/shelf", 0, v, box);
   //gc->config_dialog = cfd;
   e_dialog_resizable_set(cfd->dia, 1);
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   cfdata->box = cfd->data;
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   E_Gadcon *gc = NULL;

   if (cfdata->hdl)
     ecore_event_handler_del(cfdata->hdl);

   E_FREE(cfdata);

   if (!(gc = cfd->data))
     return;

   gc->config_dialog = NULL;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o = NULL, *of = NULL;
   Evas_Object *ow = NULL;

   o = e_widget_table_add(evas, 0);
   of = e_widget_frametable_add(evas, D_("Available Gadgets"), 0);
   ow = e_widget_ilist_add(evas, 24, 24, NULL);
   e_widget_ilist_multi_select_set(ow, 1);
   e_widget_on_change_hook_set(ow, _avail_list_cb_change, cfdata);
   cfdata->o_avail = ow;
   _load_avail_gadgets(cfdata);
   e_widget_frametable_object_append(of, ow, 0, 0, 1, 1, 1, 1, 1, 1);
   ow = e_widget_button_add(evas, D_("Add Gadget"), NULL, _cb_add, cfdata, NULL);
   e_widget_disabled_set(ow, 1);
   cfdata->o_add = ow;
   e_widget_frametable_object_append(of, ow, 0, 1, 1, 1, 1, 1, 1, 0);
   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);

   of = e_widget_frametable_add(evas, D_("Selected Gadgets"), 0);
   ow = e_widget_ilist_add(evas, 24, 24, NULL);
   e_widget_ilist_multi_select_set(ow, 1);
   e_widget_on_change_hook_set(ow, _sel_list_cb_change, cfdata);
   cfdata->o_sel = ow;
   _load_sel_gadgets(cfdata);
   e_widget_frametable_object_append(of, ow, 0, 0, 1, 1, 1, 1, 1, 1);
   ow = e_widget_button_add(evas, D_("Remove Gadget"), NULL, _cb_del, cfdata, NULL);
   e_widget_disabled_set(ow, 1);
   cfdata->o_del = ow;
   e_widget_frametable_object_append(of, ow, 0, 1, 1, 1, 1, 1, 1, 0);
   e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);

   ow = e_widget_textblock_add(evas);
   e_widget_size_min_set(ow, 200, 70);
   e_widget_textblock_markup_set(ow, D_("Description: Unavailable"));
   cfdata->o_desc = ow;
   e_widget_table_object_append(o, ow, 0, 1, 2, 1, 1, 1, 1, 0);

   if (cfdata->hdl)
     ecore_event_handler_del(cfdata->hdl);

   cfdata->hdl = ecore_event_handler_add(E_EVENT_MODULE_UPDATE,
                                         _cb_mod_update, cfdata);
   return o;
}

static Eina_Bool
_cb_mod_update(void *data, int type, void *event)
{
   E_Config_Dialog_Data *cfdata = NULL;

   if (type != E_EVENT_MODULE_UPDATE)
     return EINA_TRUE;

   if (!(cfdata = data))
     return EINA_TRUE;

   _load_avail_gadgets(cfdata);
   _load_sel_gadgets(cfdata);
   return EINA_TRUE;
}

static void
_avail_list_cb_change(void *data, Evas_Object *obj)
{
   E_Config_Dialog_Data *cfdata = NULL;
   const char *name;
   int sel, count;

   if (!(cfdata = data))
     return;

   e_widget_ilist_unselect(cfdata->o_sel);
   e_widget_disabled_set(cfdata->o_del, 1);
   e_widget_disabled_set(cfdata->o_add, 0);
   count = e_widget_ilist_selected_count_get(cfdata->o_avail);
   if ((count > 1) || (count == 0))
     e_widget_textblock_markup_set(cfdata->o_desc, D_("Description: Unavailable"));
   else
     {
        sel = e_widget_ilist_selected_get(cfdata->o_avail);
        name = (char *)e_widget_ilist_nth_data_get(cfdata->o_avail, sel);
        _set_description(cfdata, name);
     }
}

static void
_sel_list_cb_change(void *data, Evas_Object *obj)
{
   E_Config_Dialog_Data *cfdata = NULL;
   const char *name;
   int sel, count;

   if (!(cfdata = data))
     return;

   e_widget_ilist_unselect(cfdata->o_avail);
   e_widget_disabled_set(cfdata->o_add, 1);
   e_widget_disabled_set(cfdata->o_del, 0);
   count = e_widget_ilist_selected_count_get(cfdata->o_sel);
   if ((count > 1) || (count == 0))
     e_widget_textblock_markup_set(cfdata->o_desc, D_("Description: Unavailable"));
   else
     {
        sel = e_widget_ilist_selected_get(cfdata->o_sel);
        name = (char *)e_widget_ilist_nth_data_get(cfdata->o_sel, sel);
        _set_description(cfdata, name);
     }
}

static void
_load_avail_gadgets(void *data)
{
   E_Config_Dialog_Data *cfdata = NULL;
   E_Gadcon_Client_Class *cc;
   Eina_List *l = NULL;
   Evas_Object *icon = NULL;
   const char *lbl = NULL;
   Evas *evas;
   int w;

   if (!(cfdata = data))
     return;

   evas = evas_object_evas_get(cfdata->o_avail);
   evas_event_freeze(evas);
   edje_freeze();
   e_widget_ilist_freeze(cfdata->o_avail);
   e_widget_ilist_clear(cfdata->o_avail);
   EINA_LIST_FOREACH (e_gadcon_provider_list(), l, cc)
     {
	if (!cc)
	  continue;

	if (cc->func.label)
	  lbl = cc->func.label(cc);

	if (!lbl)
	  lbl = cc->name;

	if (cc->func.icon)
	  icon = cc->func.icon(cc, evas);

	e_widget_ilist_append(cfdata->o_avail, icon, lbl, NULL,
			      (void *)cc->name, NULL);
     }
   e_widget_ilist_go(cfdata->o_avail);
   e_widget_size_min_get(cfdata->o_avail, &w, NULL);
   if (w < 200)
     w = 200;

   e_widget_size_min_set(cfdata->o_avail, w, 250);
   e_widget_ilist_thaw(cfdata->o_avail);
   edje_thaw();
   evas_event_thaw(evas);
}

static void
_load_sel_gadgets(void *data)
{
   E_Config_Dialog_Data *cfdata = NULL;
   Eina_List *ll = NULL, *l = NULL, *l2 = NULL;
   Evas *evas;
   int w;
   Ngi_Item *it;
   E_Config_Gadcon_Client *cgc;
   E_Gadcon_Client_Class *gcc;
   if (!(cfdata = data))
     return;

   evas = evas_object_evas_get(cfdata->o_sel);
   evas_event_freeze(evas);
   edje_freeze();
   e_widget_ilist_freeze(cfdata->o_sel);
   e_widget_ilist_clear(cfdata->o_sel);

   EINA_LIST_FOREACH (cfdata->box->items, ll, it)
     {


	EINA_LIST_FOREACH (it->gadcon->cf->clients, l, cgc)
	  {


	     if (!cgc)
	       continue;

	     EINA_LIST_FOREACH(e_gadcon_provider_list(), l2, gcc)
	       {
		  Evas_Object *icon = NULL;
		  const char *lbl = NULL;

		  if (!gcc) continue;

		  if ((cgc->name) && (gcc->name) &&
		      (!strcmp(cgc->name, gcc->name)))
		    {
		       if (gcc->func.label)
			 lbl = gcc->func.label(gcc);

		       if (!lbl)
			 lbl = gcc->name;

		       if (gcc->func.icon)
			 icon = gcc->func.icon(gcc, evas);

		       e_widget_ilist_append(cfdata->o_sel, icon, lbl, NULL,
					     (void *)gcc->name, NULL);
		    }
	       }
	  }
     }

   e_widget_ilist_go(cfdata->o_sel);
   e_widget_size_min_get(cfdata->o_sel, &w, NULL);
   if (w < 200)
     w = 200;

   e_widget_size_min_set(cfdata->o_sel, w, 250);
   e_widget_ilist_thaw(cfdata->o_sel);
   edje_thaw();
   evas_event_thaw(evas);
   if (l)
     eina_list_free(l);

   if (l2)
     eina_list_free(l2);
}

static void
_cb_add(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata = NULL;
   Eina_List *l = NULL, *g = NULL;
   int i = 0, update = 0;

   char *gadcon_name;

   if (!(cfdata = data))
     return;

   for (i = 0, l = e_widget_ilist_items_get(cfdata->o_avail); l; l = l->next, i++)
     {
        E_Ilist_Item *item = NULL;
        const char *name = NULL;

        if (!(item = l->data))
	  continue;

        if (!item->selected)
	  continue;

        name = (char *)e_widget_ilist_nth_data_get(cfdata->o_avail, i);
        if (!name)
	  continue;

        printf("add gadget %s\n", name);

        gadcon_name = _ngi_gadcon_name_new();
        if (!gadcon_name)
	  continue;

        Config_Gadcon *cg = E_NEW(Config_Gadcon, 1);

        cg->name = eina_stringshare_add(gadcon_name);

        eina_hash_add(ngi_gadcon_hash, cg->name, cg);

        cfdata->box->cfg->gadcon_items = eina_list_append(cfdata->box->cfg->gadcon_items, cg);

        Ngi_Item *it = _ngi_gadcon_item_new(cfdata->box, cg->name, NULL);
        it->cfg_gadcon = cg;

        /* FIXME this removes left over client configs */
        for (g = it->gadcon->cf->clients; g; g = g->next)
          {
             E_Config_Gadcon_Client *cgc;

             if (!(cgc = g->data))
	       continue;

             e_gadcon_unpopulate(it->gadcon);
             e_gadcon_client_config_del(it->gadcon->cf, cgc);
          }

        if (!e_gadcon_client_config_new(it->gadcon, name))
          {
             // free item
          }
        else
          {
             e_gadcon_populate(it->gadcon);
             update = 1;
          }
     }
   if (update)
     {
        e_config_save_queue();

        _load_sel_gadgets(cfdata);
        e_widget_ilist_selected_set(cfdata->o_sel, i);

        ngi_thaw(cfdata->box->ng);
     }

   if (l)
     eina_list_free(l);
}

static void
_cb_del(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata = NULL;
   Eina_List *l = NULL, *g = NULL;
   int i = 0, update = 0;
   Ngi_Item *it;

   if (!(cfdata = data))
     return;

   for (i = 0, l = e_widget_ilist_items_get(cfdata->o_sel); l; l = l->next, i++)
     {
        E_Ilist_Item *item = NULL;
        E_Config_Gadcon_Client *cgc;
        const char *name = NULL;
        if (!(item = l->data))
	  continue;

        if (!item->selected)
	  continue;

        name = (char *)e_widget_ilist_nth_data_get(cfdata->o_sel, i);
        if (!name)
	  continue;

        it = eina_list_nth(cfdata->box->items, i);

        if (!it)
	  continue;

        EINA_LIST_FOREACH (it->gadcon->cf->clients, g, cgc)
	  {
	     if (!cgc)
	       continue;

	     if (strcmp(name, cgc->name))
	       continue;

	     e_gadcon_client_config_del(it->gadcon->cf, cgc);

	     cfdata->box->cfg->gadcon_items =
	       eina_list_remove(cfdata->box->cfg->gadcon_items, it->cfg_gadcon);
	     eina_hash_del_by_key(ngi_gadcon_hash, it->cfg_gadcon->name);

	     e_gadcon_unpopulate(it->gadcon);
	     it->obj = NULL;
	     it->box->items = eina_list_remove(it->box->items, it);
	     ngi_item_free(it);
	     e_config_save_queue();

	     update = 1;
	  }
     }
   if (update)
     {
        _load_sel_gadgets(cfdata);

        /* we just default to selecting first one here as the user may have had
         * more than one selected */
        e_widget_ilist_selected_set(cfdata->o_sel, 0);
        ngi_thaw(cfdata->box->ng);
     }

   if (g)
     eina_list_free(g);

   if (l)
     eina_list_free(l);
}

static void
_set_description(void *data, const char *name)
{
   E_Config_Dialog_Data *cfdata = NULL;
   E_Module *mod = NULL;
   Efreet_Desktop *desk = NULL;
   char buf[4096];

   if (!(cfdata = data))
     return;

   if (!name)
     return;

   if (!(mod = e_module_find(name)))
     return;

   snprintf(buf, sizeof(buf), "%s/module.desktop", e_module_dir_get(mod));
   if (!ecore_file_exists(buf))
     return;

   if (!(desk = efreet_desktop_get(buf)))
     return;

   if (desk->comment)
     e_widget_textblock_markup_set(cfdata->o_desc, desk->comment);

   efreet_desktop_free(desk);
}

