/*
 * vim:ts=8:sw=3:sts=8:et:cino=>5n-3f0^-2{2,t0,(0
 */
#include "list.h"

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Entry Entry;
typedef struct _Conf Conf;

typedef enum
{
   LIST_TOP,
   LIST_RIGHT,
   LIST_BOTTOM,
   LIST_LEFT,
   LIST_FLOAT
} List_Orient;

typedef enum
{
   LIST_LARGE,
   LIST_MEDIUM,
   LIST_SMALL
} List_View_Type;

struct _Instance 
{
   Drawer_View *view;

   Evas *evas;

   Eina_List *entries;

   Evas_Object *o_box, *o_con;

   Ecore_Timer *scroll_timer;
   Ecore_Animator *scroll_animator;
   double scroll_wanted;
   double scroll_pos;

   char theme_file[4096];
   char item_group[43];

   const char *parent_id;

   List_Orient orient;

   Conf        *conf;
   E_Config_DD *edd_conf;
};

struct _Entry
{
   Instance *inst;
   Evas_Object *o_holder, *o_icon;

   Drawer_Source_Item *si;

   Eina_Bool isa_cat;
};

struct _Conf
{
   const char *id;

   List_View_Type view_type;
};

struct _E_Config_Dialog_Data 
{
   Instance *inst;

   int view_type;
};

static void _list_reconfigure(Instance *inst);
static void _list_containers_create(Instance *inst);
static Entry * _list_horizontal_entry_create(Instance *inst, Drawer_Source_Item *si);
static Entry * _list_vertical_entry_create(Instance *inst, Drawer_Source_Item *si);
static Entry * _list_horizontal_cat_create(Instance *inst, Drawer_Source_Item *si);
static Entry * _list_vertical_cat_create(Instance *inst, Drawer_Source_Item *si);
static void _list_item_pack_options(Instance *inst, Entry *e);
static void _list_autoscroll_update(Instance *inst, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);

static Eina_Bool _list_scroll_timer(void *data);
static Eina_Bool _list_scroll_animator(void *data);
static void _list_cb_list_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static int  _list_sort_by_category_cb(const void *d1, const void *d2);
static void _list_entry_select_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _list_entry_deselect_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _list_entry_activate_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);
static void _list_entry_context_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__);

static void _list_event_activate_free(void *data __UNUSED__, void *event);
static void _list_event_context_free(void *data __UNUSED__, void *event);

static void _list_conf_activation_cb(void *data1, void *data2 __UNUSED__);
static void * _list_cf_create_data(E_Config_Dialog *cfd);
static void _list_cf_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _list_cf_fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object * _list_cf_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _list_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

EAPI Drawer_Plugin_Api drawer_plugin_api = {DRAWER_PLUGIN_API_VERSION, "List"};

static E_Config_Dialog *_cfd = NULL;

EAPI void *
drawer_plugin_init(Drawer_Plugin *p, const char *id)
{
   Instance *inst = NULL;
   char buf[128];

   inst = E_NEW(Instance, 1);

   inst->view = DRAWER_VIEW(p);

   inst->edd_conf = E_CONFIG_DD_NEW("Conf", Conf);
   #undef T
   #undef D
   #define T Conf
   #define D inst->edd_conf
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, view_type, INT);

   inst->parent_id = eina_stringshare_add(id);

   snprintf(inst->theme_file, sizeof(inst->theme_file),
	    "%s/e-module-drawer.edj", drawer_module_dir_get());

   snprintf(buf, sizeof(buf), "module.drawer/%s.list", id);
   inst->conf = e_config_domain_load(buf, inst->edd_conf);
   if (!inst->conf)
     {
	inst->conf = E_NEW(Conf, 1);
        inst->conf->view_type = LIST_LARGE;
	inst->conf->id = eina_stringshare_add(id);

	e_config_save_queue();
     }

   return inst;
}

EAPI int
drawer_plugin_shutdown(Drawer_Plugin *p)
{
   Instance *inst = NULL;
   Entry *e;

   inst = p->data;

   EINA_LIST_FREE(inst->entries, e)
     {
	if (e->o_icon)
	  evas_object_del(e->o_icon);
	if (e->o_holder)
	  evas_object_del(e->o_holder);
	E_FREE(e);
     }

   eina_stringshare_del(inst->parent_id);
   eina_stringshare_del(inst->conf->id);

   if (inst->o_box) evas_object_del(inst->o_box);
   if (inst->o_con) evas_object_del(inst->o_con);

   E_CONFIG_DD_FREE(inst->edd_conf);
   E_FREE(inst);

   return 1;
}

EAPI Evas_Object *
drawer_plugin_config_get(Drawer_Plugin *p, Evas *evas)
{
   Evas_Object *button;

   button = e_widget_button_add(evas, D_("List settings"), NULL, _list_conf_activation_cb, p, NULL);

   return button;
}

EAPI void
drawer_plugin_config_save(Drawer_Plugin *p)
{
   Instance *inst;
   char buf[128];

   inst = p->data;
   snprintf(buf, sizeof(buf), "module.drawer/%s.list", inst->conf->id);
   e_config_domain_save(buf, inst->edd_conf, inst->conf);
}

EAPI Evas_Object *
drawer_view_render(Drawer_View *v, Evas *evas, Eina_List *items)
{
   Instance *inst = NULL;
   Eina_List *l = NULL, *ll = NULL;
   Drawer_Source_Item *si;
   const char *cat = NULL;
   Eina_Bool change = EINA_FALSE;
   Entry *e;

   inst = DRAWER_PLUGIN(v)->data;

   inst->evas = evas;

   if (inst->o_box) evas_object_del(inst->o_box);
   if (inst->o_con) evas_object_del(inst->o_con);

   EINA_LIST_FREE(inst->entries, e)
     {
	if (e->o_icon)
	  evas_object_del(e->o_icon);
	if (e->o_holder)
	  evas_object_del(e->o_holder);
	E_FREE(e);
     }

   if (!items) return NULL;

   _list_containers_create(inst);
   e_box_freeze(inst->o_box);

   EINA_LIST_FOREACH(items, l, si)
      ll = eina_list_append(ll, si);
   ll = eina_list_sort(ll, eina_list_count(ll), _list_sort_by_category_cb);
   switch (inst->orient)
     {
      case LIST_BOTTOM:
      case LIST_RIGHT:
	 ll = eina_list_reverse(ll);
	 break;
      default:
	 break;
     }

   EINA_LIST_FOREACH(ll, l, si)
     {
	Entry *e;

	if (!cat && si->category)
	  {
	     cat = eina_stringshare_add(si->category);
	     change = EINA_TRUE;
	  }
	else if (cat && !si->category)
	  {
	     eina_stringshare_del(cat);
	     cat = NULL;
	     change = EINA_TRUE;
	  }
	else if (cat && si->category && (strcmp(cat, si->category)))
	  {
	     eina_stringshare_del(cat);
	     cat = eina_stringshare_add(si->category);
	     change = EINA_TRUE;
	  }
	else
	  change = EINA_FALSE;

	switch(inst->orient)
	  {
	   case LIST_TOP:
	   case LIST_BOTTOM:
	   case LIST_FLOAT:
	      if (change)
		{
		   Entry *c;

		   c = _list_vertical_cat_create(inst, si);
		   inst->entries = eina_list_append(inst->entries, c);
		   e_box_pack_end(inst->o_box, c->o_holder);
		   _list_item_pack_options(inst, c);
		}
	      e = _list_vertical_entry_create(inst, si);
	      break;
	   case LIST_RIGHT:
	   case LIST_LEFT:
	      if (change)
		{
		   Entry *c;

		   c = _list_horizontal_cat_create(inst, si);
		   inst->entries = eina_list_append(inst->entries, c);
		   e_box_pack_end(inst->o_box, c->o_holder);
		   _list_item_pack_options(inst, c);
		}
	      e = _list_horizontal_entry_create(inst, si);
	      break;
           default:
              e = NULL;
              break;
	  }
        if (e)
          {
             inst->entries = eina_list_append(inst->entries, e);
             e_box_pack_end(inst->o_box, e->o_holder);
          }
     }
   e_box_thaw(inst->o_box);
   eina_stringshare_del(cat);

   _list_reconfigure(inst);
   return inst->o_con;
}

EAPI void
drawer_view_orient_set(Drawer_View *v, E_Gadcon_Orient orient)
{
   Instance *inst = NULL;
   
   inst = DRAWER_PLUGIN(v)->data;

   switch (orient)
     {
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
      case E_GADCON_ORIENT_RIGHT:
	e_box_orientation_set(inst->o_box, 1);
	inst->orient = LIST_RIGHT;
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	e_box_orientation_set(inst->o_box, 1);
	inst->orient = LIST_LEFT;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	e_box_orientation_set(inst->o_box, 0);
	inst->orient = LIST_TOP;
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	e_box_orientation_set(inst->o_box, 0);
	inst->orient = LIST_BOTTOM;
	break;
      case E_GADCON_ORIENT_FLOAT:
	e_box_orientation_set(inst->o_box, 0);
	inst->orient = LIST_FLOAT;
	break;
      default:
	break;
     }

   _list_reconfigure(inst);
}

static void
_list_reconfigure(Instance *inst)
{
   Evas_Coord bw, bh, w, h;
   switch(inst->orient)
     {
      case LIST_TOP:
      case LIST_BOTTOM:
      case LIST_FLOAT:
         switch(inst->conf->view_type)
           {
            case LIST_LARGE:
               snprintf(inst->item_group, sizeof(inst->item_group),
                        "modules/drawer/list/vertical_item/large");
               break;
            case LIST_MEDIUM:
               snprintf(inst->item_group, sizeof(inst->item_group),
                        "modules/drawer/list/vertical_item/medium");
               break;
            case LIST_SMALL:
               snprintf(inst->item_group, sizeof(inst->item_group),
                        "modules/drawer/list/vertical_item/small");
               break;
            default:
               break;
           }
         break;
      case LIST_RIGHT:
      case LIST_LEFT:
         switch(inst->conf->view_type)
           {
            case LIST_LARGE:
               snprintf(inst->item_group, sizeof(inst->item_group),
                        "modules/drawer/list/horizontal_item/large");
               break;
            case LIST_MEDIUM:
               snprintf(inst->item_group, sizeof(inst->item_group),
                        "modules/drawer/list/horizontal_item/medium");
               break;
            case LIST_SMALL:
               snprintf(inst->item_group, sizeof(inst->item_group),
                        "modules/drawer/list/horizontal_item/small");
               break;
            default:
               break;
           }
         break;
      default:
         break;
     }

   if (inst->entries)
     {
        Eina_List *l;
        Entry *e;

        e_box_freeze(inst->o_box);
        EINA_LIST_FOREACH(inst->entries, l, e)
          {
             Evas_Coord ew, eh;

             if (e->isa_cat) continue;
             if (e->o_icon) evas_object_del(e->o_icon);
             if (!e_theme_edje_object_set
                 (e->o_holder, "base/theme/modules/drawer", inst->item_group))
               edje_object_file_set(e->o_holder, inst->theme_file, inst->item_group);

             edje_object_part_text_set(e->o_holder, "e.text.label", e->si->label);
             edje_object_part_text_set(e->o_holder, "e.text.description", e->si->description);

             edje_object_part_geometry_get(e->o_holder, "e.swallow.content", NULL, NULL, &ew, &eh);
             e->o_icon = drawer_util_icon_create(e->si, inst->evas, ew, eh);
             edje_object_part_swallow(e->o_holder, "e.swallow.content", e->o_icon);
             evas_object_pass_events_set(e->o_icon, 1);
             evas_object_show(e->o_icon);
             _list_item_pack_options(inst, e);
          }
        e_box_thaw(inst->o_box);
     }

   e_box_size_min_get(inst->o_box, &bw, &bh);
   edje_extern_object_min_size_set(inst->o_box, bw, bh);
   edje_object_size_min_calc(inst->o_con, &w, &h);
   edje_extern_object_min_size_set(inst->o_box, 1, 1);
   edje_extern_object_min_size_set(inst->o_con, w, h);
}

static void
_list_containers_create(Instance *inst)
{
   Evas *evas;
   const char *group;

   evas = inst->evas;
   inst->o_con = edje_object_add(evas);

   inst->o_box = e_box_add(evas);
   e_box_homogenous_set(inst->o_box, 0);
   switch(inst->orient)
     {
      case LIST_TOP:
      case LIST_FLOAT:
	 e_box_orientation_set(inst->o_box, 0);
	 e_box_align_set(inst->o_box, 0.5, 1);
	 group = eina_stringshare_add("modules/drawer/list/vertical");
	 break;
      case LIST_BOTTOM:
	 e_box_orientation_set(inst->o_box, 0);
	 e_box_align_set(inst->o_box, 0.5, 0);
	 group = eina_stringshare_add("modules/drawer/list/vertical");
	 break;
      case LIST_LEFT:
	 e_box_orientation_set(inst->o_box, 1);
	 e_box_align_set(inst->o_box, 1, 0.5);
	 group = eina_stringshare_add("modules/drawer/list/horizontal");
	 break;
      case LIST_RIGHT:
	 e_box_orientation_set(inst->o_box, 1);
	 e_box_align_set(inst->o_box, 0, 0.5);
	 group = eina_stringshare_add("modules/drawer/list/horizontal");
	 break;
      default:
	 e_box_orientation_set(inst->o_box, 0);
	 e_box_align_set(inst->o_box, 0.5, 0.5);
	 group = eina_stringshare_add("modules/drawer/list/vertical");
	 break;
     }

   if (!e_theme_edje_object_set(inst->o_con, "base/theme/modules/drawer", group))
     edje_object_file_set(inst->o_con, inst->theme_file, group);

   eina_stringshare_del(group);
   edje_object_part_swallow(inst->o_con, "e.swallow.content", inst->o_box);

   /* Stops the parent context menu from appearing */
   evas_object_propagate_events_set(inst->o_con, 0);

   evas_object_event_callback_add(inst->o_con, EVAS_CALLBACK_MOUSE_MOVE,
				  _list_cb_list_mouse_move, inst);
}

static Entry *
_list_horizontal_entry_create(Instance *inst, Drawer_Source_Item *si)
{
   Entry *e;

   e = E_NEW(Entry, 1);

   e->o_holder = edje_object_add(inst->evas);

   e->inst = inst;
   e->si = si;

   edje_object_signal_callback_add(e->o_holder, "e,action,select", "drawer", 
				   _list_entry_select_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,deselect", "drawer", 
				   _list_entry_deselect_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,activate", "drawer", 
				   _list_entry_activate_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,context", "drawer", 
				   _list_entry_context_cb, e);

   return e;
}

static Entry *
_list_vertical_entry_create(Instance *inst, Drawer_Source_Item *si)
{
   Entry *e;

   e = E_NEW(Entry, 1);

   e->o_holder = edje_object_add(inst->evas);

   e->inst = inst;
   e->si = si;

   edje_object_signal_callback_add(e->o_holder, "e,action,select", "drawer", 
				   _list_entry_select_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,deselect", "drawer", 
				   _list_entry_deselect_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,activate", "drawer", 
				   _list_entry_activate_cb, e);
   edje_object_signal_callback_add(e->o_holder, "e,action,context", "drawer", 
				   _list_entry_context_cb, e);

   return e;
}

static Entry *
_list_horizontal_cat_create(Instance *inst, Drawer_Source_Item *si)
{
   Entry *e;
   char buf[1024];

   e = E_NEW(Entry, 1);

   e->o_holder = edje_object_add(inst->evas);
   if (!e_theme_edje_object_set(e->o_holder, "base/theme/modules/drawer",
				"modules/drawer/list/horizontal_category"))
     edje_object_file_set(e->o_holder, inst->theme_file,
			  "modules/drawer/list/horizontal_category");

   if (si->category)
     snprintf(buf, sizeof(buf), "%s", si->category);
   else
     snprintf(buf, sizeof(buf), "Uncategorised");

   edje_object_part_text_set(e->o_holder, "e.text.category", buf);

   e->inst = inst;
   e->si = si;
   e->isa_cat = EINA_TRUE;

   return e;
}

static Entry *
_list_vertical_cat_create(Instance *inst, Drawer_Source_Item *si)
{
   Entry *e;
   char buf[1024];

   e = E_NEW(Entry, 1);

   e->o_holder = edje_object_add(inst->evas);
   if (!e_theme_edje_object_set(e->o_holder, "base/theme/modules/drawer",
				"modules/drawer/list/vertical_category"))
     edje_object_file_set(e->o_holder, inst->theme_file,
			  "modules/drawer/list/vertical_category");

   if (si->category)
     snprintf(buf, sizeof(buf), "%s", si->category);
   else
     snprintf(buf, sizeof(buf), "Uncategorised");

   edje_object_part_text_set(e->o_holder, "e.text.category", buf);

   e->inst = inst;
   e->si = si;
   e->isa_cat = EINA_TRUE;

   return e;
}

static void
_list_item_pack_options(Instance *inst, Entry *e)
{
   Evas_Coord w, h, mw, mh;

   w = h = 0;
   edje_object_size_min_calc(e->o_holder, &w, &h);
   edje_object_size_max_get(e->o_holder, &mw, &mh);
   e_box_pack_options_set(e->o_holder,
			  1, 1, /* fill */
			  1, 1, /* expand */
			  0.5, 0.5, /* align */
			  w, h, /* min */
			  mw, mh /* max */
			 );
   evas_object_show(e->o_holder);
}

static void
_list_autoscroll_update(Instance *inst, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
   double d;
   
   if (e_box_orientation_get(inst->o_box))
     {
	if (w > 1) d = (double)x / (double)(w - 1);
	else d = 0;
     }
   else
     {
	if (h > 1) d = (double)y / (double)(h - 1);
	else d = 0;
     }
   if (d < 0.0) d = 0.0;
   else if (d > 1.0) d = 1.0;
   if (!inst->scroll_timer)
     inst->scroll_timer = 
     ecore_timer_add(0.01, _list_scroll_timer, inst);
   if (!inst->scroll_animator)
     inst->scroll_animator = 
     ecore_animator_add(_list_scroll_animator, inst);
   inst->scroll_wanted = d;
}

static Eina_Bool
_list_scroll_timer(void *data)
{
   Instance *inst = NULL;
   double d;
   
   inst = data;
   d = inst->scroll_wanted - inst->scroll_pos;
   if (d < 0) d = -d;
   if (d < 0.001)
     {
	inst->scroll_pos =  inst->scroll_wanted;
	inst->scroll_timer = NULL;
	return EINA_FALSE;
     }
   inst->scroll_pos = (inst->scroll_pos * 0.95) + (inst->scroll_wanted * 0.05);
   return EINA_TRUE;
}

static Eina_Bool
_list_scroll_animator(void *data)
{
   Instance *inst = NULL;
   
   inst = data;
   if (e_box_orientation_get(inst->o_box))
     e_box_align_set(inst->o_box, 1.0 - inst->scroll_pos, 0.5);
   else
     e_box_align_set(inst->o_box, 0.5, 1.0 - inst->scroll_pos);
   if (!inst->scroll_timer)
     {
	inst->scroll_animator = NULL;
	return EINA_FALSE;
     }

   /* Have scroll_cb func if d&d is ever implemented. See e_gadcon.c */

   return EINA_TRUE;
}

static void
_list_cb_list_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst = NULL;
   Evas_Event_Mouse_Move *ev;
   Evas_Coord x, y, w, h;
   
   ev = event_info;
   inst = data;
   evas_object_geometry_get(inst->o_box, &x, &y, &w, &h);
   _list_autoscroll_update(inst, ev->cur.output.x - x, 
			   ev->cur.output.y - y, w, h);
}

static int
_list_sort_by_category_cb(const void *d1, const void *d2)
{
   const Drawer_Source_Item *si1;
   const Drawer_Source_Item *si2;
   int ret;

   if (!(si1 = d1)) return -1;
   if (!(si2 = d2)) return 1;

   if (!si1->category) return -1;
   if (!si2->category) return 1;

   ret = strcmp(si1->category, si2->category);
   
   return ret > 0 ? 1 : -1;
}

static void
_list_entry_select_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Entry *e = NULL;
   Instance *inst = NULL;
   Drawer_Source_Item *si = NULL;

   e = data;
   inst = e->inst;
   si = e->si;
   edje_object_part_text_set(inst->o_con, "e.text.label", si->label);
   edje_object_part_text_set(inst->o_con, "e.text.description", si->description);
}

static void
_list_entry_deselect_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Entry *e = NULL;
   Instance *inst = NULL;

   e = data;
   inst = e->inst;
   edje_object_part_text_set(inst->o_con, "e.text.label", NULL);
   edje_object_part_text_set(inst->o_con, "e.text.description", NULL);
}

static void
_list_entry_activate_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Entry *e = NULL;
   Drawer_Event_View_Activate *ev;

   e = data;
   ev = E_NEW(Drawer_Event_View_Activate, 1);
   ev->data = e->si;
   ev->view = e->inst->view;
   ev->id = eina_stringshare_add(e->inst->parent_id);
   ecore_event_add(DRAWER_EVENT_VIEW_ITEM_ACTIVATE, ev, _list_event_activate_free, NULL);

   /* XXX: this doesn't seem to work */
   edje_object_signal_emit(e->inst->o_con, "e,action,activate", "drawer");
}

static void
_list_entry_context_cb(void *data, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Entry *e = NULL;
   Drawer_Event_View_Context *ev;
   Evas_Coord ox, oy;

   evas_object_geometry_get(obj, &ox, &oy, NULL, NULL);

   e = data;
   ev = E_NEW(Drawer_Event_View_Context, 1);
   ev->data = e->si;
   ev->view = e->inst->view;
   ev->x = ox;
   ev->y = oy;
   ev->id = eina_stringshare_add(e->inst->parent_id);
   ecore_event_add(DRAWER_EVENT_VIEW_ITEM_CONTEXT, ev, _list_event_context_free, NULL);
}

static void
_list_event_activate_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_View_Activate *ev;

   ev = event;
   eina_stringshare_del(ev->id);
   free(ev);
}

static void
_list_event_context_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_View_Context *ev;

   ev = event;
   eina_stringshare_del(ev->id);
   free(ev);
}

static void
_list_conf_activation_cb(void *data1, void *data2 __UNUSED__)
{
   Drawer_Plugin *p = NULL;
   Instance *inst = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   p = data1;
   inst = p->data;
   /* is this config dialog already visible ? */
   if (e_config_dialog_find("Drawer_List", "_e_module_drawer_cfg_dlg"))
     return;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;

   v->create_cfdata = _list_cf_create_data;
   v->free_cfdata = _list_cf_free_data;
   v->basic.create_widgets = _list_cf_basic_create;
   v->basic.apply_cfdata = _list_cf_basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", drawer_module_dir_get());

   /* create new config dialog */
   _cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
	 D_("Drawer Plugin : List"), "Drawer_List", 
	 "_e_module_drawer_cfg_dlg", buf, 0, v, inst);

   e_dialog_resizable_set(_cfd->dia, 1);
}

static void *
_list_cf_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   cfdata->inst = cfd->data;
   _list_cf_fill_data(cfdata);
   return cfdata;
}

static void 
_list_cf_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   _cfd = NULL;
   E_FREE(cfdata);
}

static void 
_list_cf_fill_data(E_Config_Dialog_Data *cfdata)
{
   cfdata->view_type = cfdata->inst->conf->view_type;
}

static Evas_Object *
_list_cf_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *of, *ob;
   E_Radio_Group *rg;

   rg = e_widget_radio_group_new(&(cfdata->view_type));
   of = e_widget_framelist_add(evas, D_("Item size options"), 0);
   ob = e_widget_radio_add(evas, D_("Large"), LIST_LARGE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Medium"), LIST_MEDIUM, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Small"), LIST_SMALL, rg);
   e_widget_framelist_object_append(of, ob);

   return of;
}

static int 
_list_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   cfdata->inst->conf->view_type = cfdata->view_type;

   _list_reconfigure(cfdata->inst);

   e_config_save_queue();
   return 1;
}

