#include <e.h>
#include "e_mod_main.h"

typedef struct _Instance Instance;
struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_entry, *o_base, *o_loc, *o_event;
   E_Toolbar *tbar;
   char *path;
};

/* local function protos */
static E_Gadcon_Client *_gc_init          (E_Gadcon *gc, const char *name, 
					   const char *id, const char *style);
static void             _gc_shutdown      (E_Gadcon_Client *gcc);
static void             _gc_orient        (E_Gadcon_Client *gcc);
static char            *_gc_label         (void);
static Evas_Object     *_gc_icon          (Evas *evas);
static const char      *_gc_id_new        (void);
static void             _cb_changed       (void *data, Evas_Object *obj, 
					   void *event_info);
static void             _cb_dir_changed   (void *data, Evas_Object *obj, 
					   void *event_info);
static void             _cb_dir_deleted   (void *data, Evas_Object *obj, 
					   void *event_info);
static void             _cb_files_deleted (void *data, Evas_Object *obj, 
					   void *event_info);
static void             _cb_selected      (void *data, Evas_Object *obj, 
					   void *event_info);
static void             _cb_sel_changed   (void *data, Evas_Object *obj, 
					   void *event_info);
static void             _cb_key_down      (void *data, Evas_Object *obj, 
					   void *event_info);
static void             _cb_mouse_down    (void *data, Evas *e, 
					   Evas_Object *obj, void *event_info);

static Eina_List *instances = NULL;
static E_Module *path_mod = NULL;

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "efm_path", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
   E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Instance *inst = NULL;
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module-efm_path.edj", 
	    e_module_dir_get(path_mod));

   inst = E_NEW(Instance, 1);
   if (!inst) return NULL;

   inst->o_base = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_base, "base/theme/modules/efm_path", 
				"modules/efm_path/main"))
     edje_object_file_set(inst->o_base, buf, "modules/efm_path/main");
   evas_object_event_callback_add(inst->o_base, EVAS_CALLBACK_MOUSE_DOWN, 
				  _cb_mouse_down, inst);
   evas_object_show(inst->o_base);

   inst->o_loc = e_widget_label_add(gc->evas, D_("Location:"));
   edje_object_part_swallow(inst->o_base, "e.swallow.location", inst->o_loc);
   evas_object_show(inst->o_loc);

   inst->o_entry = e_widget_entry_add(gc->evas, &(inst->path), NULL, NULL, NULL);
   edje_object_part_swallow(inst->o_base, "e.swallow.entry", inst->o_entry);
   evas_object_show(inst->o_entry);

   /* add hook to know when user changes entry */
   evas_object_smart_callback_add(inst->o_entry, "key_down", 
				  _cb_key_down, inst);

   /* add the hooks to get signals from efm */
   evas_object_smart_callback_add(inst->o_base, "changed", 
				  _cb_changed, inst);
   evas_object_smart_callback_add(inst->o_base, "dir_changed", 
				  _cb_dir_changed, inst);
   evas_object_smart_callback_add(inst->o_base, "dir_deleted", 
				  _cb_dir_deleted, inst);
   evas_object_smart_callback_add(inst->o_base, "files_deleted", 
				  _cb_files_deleted, inst);
   evas_object_smart_callback_add(inst->o_base, "selected", 
				  _cb_selected, inst);
   evas_object_smart_callback_add(inst->o_base, "selection_change", 
				  _cb_sel_changed, inst);

   inst->o_event = evas_object_rectangle_add(gc->evas);
   evas_object_color_set(inst->o_event, 255, 0, 0, 142);
   evas_object_repeat_events_set(inst->o_event, 1);
   evas_object_event_callback_add(inst->o_event, EVAS_CALLBACK_MOUSE_DOWN, 
				  _cb_mouse_down, inst);
   evas_object_show(inst->o_event);

   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_base);
   inst->gcc->data = inst;

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

   evas_object_event_callback_del(inst->o_event, EVAS_CALLBACK_MOUSE_DOWN, _cb_mouse_down);
   evas_object_event_callback_del(inst->o_base, EVAS_CALLBACK_MOUSE_DOWN, _cb_mouse_down);

   if (inst->o_event) evas_object_del(inst->o_event);
   if (inst->o_loc) evas_object_del(inst->o_loc);
   if (inst->o_entry) evas_object_del(inst->o_entry);
   if (inst->o_base) evas_object_del(inst->o_base);
   if (inst->path) free(inst->path);
   E_FREE(inst);
}

static void 
_gc_orient(E_Gadcon_Client *gcc) 
{
   e_gadcon_client_min_size_set(gcc, 32, 16);
}

static char *
_gc_label(void) 
{
   return D_("EFM Path");
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o = NULL;
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module-efm_path.edj", 
	    e_module_dir_get(path_mod));
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void) 
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, 
	    (eina_list_count(instances) + 1));
   return strdup(buf);
}

/* E Module API functions */
EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION, "EFM Path"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   path_mod = m;
   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int 
e_modapi_shutdown(E_Module *m) 
{
   e_gadcon_provider_unregister(&_gc_class);
   path_mod = NULL;
   return 1;
}

EAPI int 
e_modapi_save(E_Module *m) 
{
   return 1;
}

/* local functions */
static void 
_cb_changed(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   E_Toolbar *tbar;
   Evas_Object *o_fm;
   E_Win *fwin;

   inst = data;
   tbar = event_info;
   o_fm = e_toolbar_fm2_get(tbar);
   if (!o_fm) return;
   fwin = e_toolbar_fwin_get(tbar);
   if (!fwin) return;
//   printf("\n\nPath Module Changed\n\n");
}

static void 
_cb_dir_changed(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   E_Toolbar *tbar;
   Evas_Object *o_fm;
   const char *path;

   inst = data;
   tbar = event_info;
   inst->tbar = tbar;
   o_fm = e_toolbar_fm2_get(tbar);
   if (!o_fm) return;
   path = e_fm2_real_path_get(o_fm);
   if (!inst->o_entry) return;
   e_widget_entry_text_set(inst->o_entry, path);
}

static void 
_cb_dir_deleted(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   E_Toolbar *tbar;
   Evas_Object *o_fm;
   E_Win *fwin;

   inst = data;
   tbar = event_info;
   o_fm = e_toolbar_fm2_get(tbar);
   if (!o_fm) return;
   fwin = e_toolbar_fwin_get(tbar);
   if (!fwin) return;
//   printf("\n\nPath Module Dir Deleted\n\n");
}

static void 
_cb_files_deleted(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   E_Toolbar *tbar;
   Evas_Object *o_fm;
   E_Win *fwin;

   inst = data;
   tbar = event_info;
   o_fm = e_toolbar_fm2_get(tbar);
   if (!o_fm) return;
   fwin = e_toolbar_fwin_get(tbar);
   if (!fwin) return;
//   printf("\n\nPath Module Files Deleted\n\n");
}

static void 
_cb_selected(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   E_Toolbar *tbar;
   Evas_Object *o_fm;
   E_Win *fwin;

   inst = data;
   tbar = event_info;
   o_fm = e_toolbar_fm2_get(tbar);
   if (!o_fm) return;
   fwin = e_toolbar_fwin_get(tbar);
   if (!fwin) return;
//   printf("\n\nPath Module Selected\n\n");
}

static void 
_cb_sel_changed(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   E_Toolbar *tbar;
   Evas_Object *o_fm;
   E_Win *fwin;

   inst = data;
   tbar = event_info;
   o_fm = e_toolbar_fm2_get(tbar);
   if (!o_fm) return;
   fwin = e_toolbar_fwin_get(tbar);
   if (!fwin) return;
//   printf("\n\nPath Module Selection Change\n\n");
}

static void 
_cb_key_down(void *data, Evas_Object *obj, void *event_info) 
{
   Evas_Event_Key_Down *ev;
   Instance *inst;
   Evas_Object *o_fm;
   const char *p;

   ev = event_info;
   if (strcmp(ev->keyname, "Return")) return;

   inst = data;
   if ((!inst) || (!inst->tbar)) return;
   p = e_widget_entry_text_get(inst->o_entry);
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;
   e_fm2_path_set(o_fm, p, "/");
}

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
