#include <e.h>
#include "e_mod_main.h"

typedef struct _Instance Instance;
struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_base, *o_list;
   Evas_Object *o_back, *o_up, *o_forward, *o_refresh, *o_favorites;
   E_Toolbar *tbar;

   Ecore_List *history;
   int ignore_dir;
};

/* local function protos */
static E_Gadcon_Client *_gc_init          (E_Gadcon *gc, const char *name, 
					   const char *id, const char *style);
static void             _gc_shutdown      (E_Gadcon_Client *gcc);
static void             _gc_orient        (E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char            *_gc_label         (E_Gadcon_Client_Class *client_class);
static Evas_Object     *_gc_icon          (E_Gadcon_Client_Class *client_class, Evas *evas);
static const char      *_gc_id_new        (E_Gadcon_Client_Class *client_class);
static void             _cb_mouse_down    (void *data, Evas *e, 
					   Evas_Object *obj, void *event_info);
static void             _cb_back_click    (void *data, Evas_Object *obj, 
					   const char *emission, 
					   const char *source);
static void             _cb_forward_click (void *data, Evas_Object *obj, 
					   const char *emission, 
					   const char *source);
static void             _cb_up_click      (void *data, Evas_Object *obj, 
					   const char *emission, 
					   const char *source);
static void             _cb_refresh_click (void *data, Evas_Object *obj, 
					   const char *emission, 
					   const char *source);
static void             _cb_favorites_click (void *data, Evas_Object *obj, 
                                             const char *emission, 
                                             const char *source);
static void             _cb_changed       (void *data, Evas_Object *obj, 
					   void *event_info);
static void             _cb_dir_changed   (void *data, Evas_Object *obj, 
					   void *event_info);

static Eina_List *instances = NULL;
static E_Module *nav_mod = NULL;

/* local gadcon functions */
static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "efm_nav", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
   E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Instance *inst = NULL;
   char buf[4096];

   inst = E_NEW(Instance, 1);
   if (!inst) return NULL;

   snprintf(buf, sizeof(buf), "%s/e-module-efm_nav.edj", 
	    e_module_dir_get(nav_mod));

   inst->history = ecore_list_new();
   ecore_list_free_cb_set(inst->history, free);

   inst->o_base = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_base, "base/theme/modules/efm_nav", 
				"modules/efm_nav/main"))
     edje_object_file_set(inst->o_base, buf, "modules/efm_nav/main");
   evas_object_show(inst->o_base);

   inst->o_list = e_widget_list_add(gc->evas, 1, 1);
   edje_object_part_swallow(inst->o_base, "e.swallow.buttons", inst->o_list);

   inst->o_back = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_back, "base/theme/modules/efm_nav", 
				"modules/efm_nav/back"))
     edje_object_file_set(inst->o_back, buf, "modules/efm_nav/back");
   edje_object_signal_callback_add(inst->o_back, "e,action,click", "", 
				   _cb_back_click, inst);
   evas_object_show(inst->o_back);
   e_widget_list_object_append(inst->o_list, inst->o_back, 1, 1, 0.5);

   inst->o_forward = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_forward, "base/theme/modules/efm_nav", 
				"modules/efm_nav/forward"))
     edje_object_file_set(inst->o_forward, buf, "modules/efm_nav/forward");
   edje_object_signal_callback_add(inst->o_forward, "e,action,click", "", 
				   _cb_forward_click, inst);
   evas_object_show(inst->o_forward);
   e_widget_list_object_append(inst->o_list, inst->o_forward, 1, 1, 0.5);

   inst->o_up = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_up, "base/theme/modules/efm_nav", 
				"modules/efm_nav/up"))
     edje_object_file_set(inst->o_up, buf, "modules/efm_nav/up");
   edje_object_signal_callback_add(inst->o_up, "e,action,click", "", 
				   _cb_up_click, inst);
   evas_object_show(inst->o_up);
   e_widget_list_object_append(inst->o_list, inst->o_up, 1, 1, 0.5);

   inst->o_refresh = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_refresh, "base/theme/modules/efm_nav", 
				"modules/efm_nav/refresh"))
     edje_object_file_set(inst->o_refresh, buf, "modules/efm_nav/refresh");
   edje_object_signal_callback_add(inst->o_refresh, "e,action,click", "", 
				   _cb_refresh_click, inst);
   evas_object_show(inst->o_refresh);
   e_widget_list_object_append(inst->o_list, inst->o_refresh, 1, 1, 0.5);

   inst->o_favorites = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_favorites, "base/theme/modules/efm_nav", 
				"modules/efm_nav/favorites"))
     edje_object_file_set(inst->o_favorites, buf, "modules/efm_nav/favorites");
   edje_object_signal_callback_add(inst->o_favorites, "e,action,click", "", 
				   _cb_favorites_click, inst);
   evas_object_show(inst->o_favorites);
   e_widget_list_object_append(inst->o_list, inst->o_favorites, 1, 1, 0.5);

   /* add the hooks to get signals from efm */
   evas_object_smart_callback_add(inst->o_base, "changed", 
				  _cb_changed, inst);
   evas_object_smart_callback_add(inst->o_base, "dir_changed", 
				  _cb_dir_changed, inst);

   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_base);
   inst->gcc->data = inst;
   inst->tbar = e_gadcon_toolbar_get(gc);

   evas_object_event_callback_add(inst->o_base, EVAS_CALLBACK_MOUSE_DOWN, 
				  _cb_mouse_down, inst);

   edje_object_signal_emit(inst->o_back, "e,state,disabled", "e");
   edje_object_message_signal_process(inst->o_back);
   edje_object_signal_emit(inst->o_forward, "e,state,disabled", "e");
   edje_object_message_signal_process(inst->o_forward);

   instances = eina_list_append(instances, inst);
   return inst->gcc;
}

static void 
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst = NULL;

   inst = gcc->data;
   if (!inst) return;
   instances = eina_list_remove(instances, inst);
   if (inst->history) ecore_list_destroy(inst->history);
   if (inst->o_favorites) evas_object_del(inst->o_favorites);
   if (inst->o_back) evas_object_del(inst->o_back);
   if (inst->o_up) evas_object_del(inst->o_up);
   if (inst->o_forward) evas_object_del(inst->o_forward);
   if (inst->o_refresh) evas_object_del(inst->o_refresh);
   if (inst->o_favorites) evas_object_del(inst->o_favorites);
   if (inst->o_list) evas_object_del(inst->o_list);
   if (inst->o_base) evas_object_del(inst->o_base);
   E_FREE(inst);
}

static void 
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient) 
{
   Instance *inst;

   inst = gcc->data;
   switch (orient) 
     {
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_BOTTOM:
	e_gadcon_client_aspect_set(gcc, 16 * 3, 16);
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_RIGHT:
	e_gadcon_client_aspect_set(gcc, 16, 16 * 3);
	break;
      default:
	break;
     }
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class) 
{
   return D_("EFM Navigation");
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas) 
{
   Evas_Object *o = NULL;
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module-efm_nav.edj", 
	    e_module_dir_get(nav_mod));
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class) 
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, 
	    (eina_list_count(instances) + 1));
   return strdup(buf);
}

/* E Module API Functions */
EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION, "EFM Navigation"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   char buf[4095];
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   nav_mod = m;
   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int 
e_modapi_shutdown(E_Module *m) 
{
   e_gadcon_provider_unregister(&_gc_class);
   nav_mod = NULL;
   return 1;
}

EAPI int 
e_modapi_save(E_Module *m) 
{
   return 1;
}

/* local functions */
static void 
_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   E_Menu *mn;
   E_Zone *zone;
   int x, y;

   inst = data;
   ev = event_info;
   if ((ev->button != 3) || (inst->gcc->menu)) return;
   zone = e_util_zone_current_get(e_manager_current_get());
   mn = e_menu_new();
   e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
   ecore_x_pointer_xy_get(zone->container->win, &x, &y);
   e_menu_activate_mouse(mn, zone, x, y, 1, 1, 
			 E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
}

static void 
_cb_back_click(void *data, Evas_Object *obj, const char *emission, const char *source) 
{
   Instance *inst;
   Evas_Object *o_fm;
   char *hist;
   int i = 0;

   inst = data;
   if ((!inst) || (!inst->tbar)) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;
   if (ecore_list_empty_is(inst->history)) return;
   i = ecore_list_index(inst->history);
   hist = ecore_list_index_goto(inst->history, (i + 1));
   if (!hist) 
     {
	edje_object_signal_emit(inst->o_back, "e,state,disabled", "e");
	edje_object_message_signal_process(inst->o_back);
	return;
     }
   inst->ignore_dir = 1;
   e_fm2_path_set(o_fm, hist, "/");
}

static void 
_cb_forward_click(void *data, Evas_Object *obj, const char *emission, const char *source) 
{
   Instance *inst;
   Evas_Object *o_fm;
   char *hist;
   int i = 0;

   inst = data;
   if ((!inst) || (!inst->tbar)) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;
   if (ecore_list_empty_is(inst->history)) return;
   i = ecore_list_index(inst->history);
   hist = ecore_list_index_goto(inst->history, (i - 1));
   if (!hist) 
     {
	edje_object_signal_emit(inst->o_forward, "e,state,disabled", "e");
	edje_object_message_signal_process(inst->o_forward);
	return;
     }
   inst->ignore_dir = 1;
   e_fm2_path_set(o_fm, hist, "/");
}

static void 
_cb_refresh_click(void *data, Evas_Object *obj, const char *emission, const char *source) 
{
   Instance *inst;
   Evas_Object *o_fm;
   char *hist;
   int i = 0;

   inst = data;
   if ((!inst) || (!inst->tbar)) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;
   // Don't destroy forward history when refreshing
   inst->ignore_dir = 1;
   e_fm2_path_set(o_fm, NULL, e_fm2_real_path_get(o_fm));
}

static void 
_cb_up_click(void *data, Evas_Object *obj, const char *emission, const char *source) 
{
   Instance *inst;
   Evas_Object *o_fm;
   char *p, *t;

   inst = data;
   if ((!inst) || (!inst->tbar)) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;
   t = strdup(e_fm2_real_path_get(o_fm));
   p = strrchr(t, '/');
   if (p)
     {
        *p = 0;
        p = t;
        if (p[0] == 0) p = "/";
        e_fm2_path_set(o_fm, NULL, p);
        //edje_object_signal_emit(inst->o_up, "e,state,enabled", "e");
     }
   else 
     edje_object_signal_emit(inst->o_up, "e,state,disabled", "e");
   edje_object_message_signal_process(inst->o_up);
   
   free(t);
}

static void 
_cb_favorites_click(void *data, Evas_Object *obj, const char *emission, const char *source) 
{
   Instance *inst;
   Evas_Object *o_fm;

   inst = data;
   if ((!inst) || (!inst->tbar)) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;
   e_fm2_path_set(o_fm, "favorites", "/");
}

static void 
_cb_changed(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;

   inst = data;
   inst->tbar = event_info;
}

static void 
_cb_dir_changed(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   Evas_Object *o_fm;
   const char *path;
   int i = 0, count = 0;

   inst = data;
   if ((!inst) || (!inst->tbar)) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;
   path = e_fm2_real_path_get(o_fm);
   if (!path) return;
   if (!inst->ignore_dir) 
     {
        const char *t;
        t = ecore_list_current(inst->history);
        if(!t || strcmp(t, path))
          {
	     if (t)
	       {
		  int i, current;
		  current = ecore_list_index(inst->history);
		  ecore_list_first_goto(inst->history);
		  for(i = 0; i < current; i++)
			  ecore_list_remove_destroy(inst->history);
	       }
             ecore_list_prepend(inst->history, strdup(path));
             ecore_list_first_goto(inst->history);
          }
     }
   inst->ignore_dir = 0;

   if (!strcmp(path, "/"))
	 edje_object_signal_emit(inst->o_up, "e,state,disabled", "e");
   else
	 edje_object_signal_emit(inst->o_up, "e,state,enabled", "e");
   edje_object_message_signal_process(inst->o_up);

   count = ecore_list_count(inst->history);
   i = ecore_list_index(inst->history);

   if (count <= 1) 
     {
	edje_object_signal_emit(inst->o_back, "e,state,disabled", "e");
	edje_object_signal_emit(inst->o_forward, "e,state,disabled", "e");
	edje_object_message_signal_process(inst->o_back);
	edje_object_message_signal_process(inst->o_forward);
	return;
     }
   else 
     {
	if (i == (count - 1))
	  edje_object_signal_emit(inst->o_back, "e,state,disabled", "e");
	else
	  edje_object_signal_emit(inst->o_back, "e,state,enabled", "e");
	edje_object_message_signal_process(inst->o_back);
	if (i == 0) 
	  edje_object_signal_emit(inst->o_forward, "e,state,disabled", "e");
	else
	  edje_object_signal_emit(inst->o_forward, "e,state,enabled", "e");
	edje_object_message_signal_process(inst->o_forward);
     }
}
