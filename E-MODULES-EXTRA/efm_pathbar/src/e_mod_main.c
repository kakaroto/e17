#include <e.h>
#include "e_mod_main.h"

typedef struct _Instance Instance;
struct _Instance 
{
   // parent
   char *theme_file;
   E_Gadcon_Client *gcc;
   E_Toolbar *tbar;

   // scroll & box
   Evas_Object *o_scroll, *o_box;
   
   // buttons
   Evas_Object *o_root;
   Eina_List *l_buttons;
};

/* local function protos */
static E_Gadcon_Client *_gc_init            (E_Gadcon *gc, const char *name, 
					     const char *id, const char *style);
static void             _gc_shutdown        (E_Gadcon_Client *gcc);
static char            *_gc_label           (E_Gadcon_Client_Class *client_class);
static Evas_Object     *_gc_icon            (E_Gadcon_Client_Class *client_class, Evas *evas);
static const char      *_gc_id_new          (E_Gadcon_Client_Class *client_class);
static void             _cb_mouse_down      (void *data, Evas *e, 
					     Evas_Object *obj, void *event_info);
static void             _cb_root_click      (void *data, Evas_Object *obj, 
                                             const char *emission, 
                                             const char *source);
static void             _cb_button_click    (void *data, Evas_Object *obj, 
					     const char *emission, 
					     const char *source);
static void             _cb_changed         (void *data, Evas_Object *obj, 
					     void *event_info);
static void             _cb_dir_changed     (void *data, Evas_Object *obj, 
					     void *event_info);
static Evas_Object*     _box_button_append  (Instance *inst, const char *label, 
                                             void (*func)(void *data, Evas_Object *obj, const char *emission, const char *source));
static Eina_List*       _delete_buttons_tail(Instance *inst, Eina_List *l);
static void             _subdir_add         (Instance *inst, const char *name);

static Eina_List *instances = NULL;
static E_Module *pathbar_mod = NULL;

/* local gadcon functions */
static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "efm_pathbar", 
   {
      _gc_init, _gc_shutdown, NULL, _gc_label, _gc_icon, _gc_id_new, NULL,
      e_gadcon_site_is_efm_toolbar
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Instance *inst = NULL;

   inst = E_NEW(Instance, 1);
   if (!inst) return NULL;

   asprintf(&inst->theme_file, "%s/e-module-efm_pathbar.edj",
	    e_module_dir_get(pathbar_mod));

   inst->o_root = NULL;
   inst->l_buttons = NULL;

   // scrollframe
   inst->o_scroll = e_scrollframe_add(gc->evas);
   if (!e_scrollframe_custom_theme_set(inst->o_scroll, "base/theme/modules/efm_pathbar", 
                                       "modules/efm_pathbar/main"))
      e_scrollframe_custom_edje_file_set(inst->o_scroll, inst->theme_file, "modules/efm_pathbar/main");
   e_scrollframe_single_dir_set(inst->o_scroll, 1);
   e_scrollframe_policy_set(inst->o_scroll, E_SCROLLFRAME_POLICY_AUTO, E_SCROLLFRAME_POLICY_OFF);
   e_scrollframe_thumbscroll_force(inst->o_scroll, 1);
   evas_object_show(inst->o_scroll);

   // listbox
   inst->o_box = e_box_add(gc->evas);
   e_box_orientation_set(inst->o_box, 1);
   e_box_homogenous_set(inst->o_box, 0);
   e_scrollframe_child_set(inst->o_scroll, inst->o_box);
   evas_object_show(inst->o_box);

   // add the hooks to get signals from efm
   evas_object_smart_callback_add(inst->o_scroll, "changed", 
				  _cb_changed, inst);
   evas_object_smart_callback_add(inst->o_scroll, "dir_changed", 
				  _cb_dir_changed, inst);

   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_scroll);
   inst->gcc->data = inst;
   inst->tbar = e_gadcon_toolbar_get(gc);
   
   evas_object_event_callback_add(inst->o_scroll, EVAS_CALLBACK_MOUSE_DOWN, 
				  _cb_mouse_down, inst);

   instances = eina_list_append(instances, inst);
   
   return inst->gcc;
}

static void 
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   Evas_Object *btn;
   
   inst = gcc->data;
   if (!inst) return;
   
   instances = eina_list_remove(instances, inst);
   EINA_LIST_FREE(inst->l_buttons, btn)
     {
        e_box_unpack(btn);
        evas_object_del(btn);
     }
   if (inst->o_root) evas_object_del(inst->o_root);
   if (inst->o_box) evas_object_del(inst->o_box);
   if (inst->o_scroll) evas_object_del(inst->o_scroll);
   free(inst->theme_file);
   E_FREE(inst);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class) 
{
   return D_("EFM Pathbar");
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas) 
{
   Evas_Object *o = NULL;
   char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s/e-module-efm_pathbar.edj", 
	    e_module_dir_get(pathbar_mod));
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class) 
{
   char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, 
	    (eina_list_count(instances) + 1));
   return strdup(buf);
}

/* E Module API Functions */
EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION, "EFM Pathbar"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   char buf[PATH_MAX];
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   pathbar_mod = m;
   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int 
e_modapi_shutdown(E_Module *m) 
{
   e_gadcon_provider_unregister(&_gc_class);
   pathbar_mod = NULL;
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
   E_Menu *m;
   E_Zone *zone;
   int x, y;

   inst = data;
   ev = event_info;
   if ((ev->button != 3) || inst->gcc->menu) return;
   zone = e_util_zone_current_get(e_manager_current_get());

   m = e_menu_new();
   m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);
   ecore_x_pointer_xy_get(zone->container->win, &x, &y);
   e_menu_activate_mouse(m, zone, x, y, 1, 1, 
			 E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
}

static void 
_cb_root_click(void *data, Evas_Object *obj, const char *emission, const char *source) 
{
   Instance *inst = data;
   Evas_Object *o_fm;
   
   if (!inst || !inst->tbar) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;

   e_fm2_path_set(o_fm, "/", "/");
}

static void 
_cb_button_click(void *data, Evas_Object *obj, const char *emission, const char *source) 
{
   Instance *inst = data;
   Eina_List *l;
   Evas_Object *o_fm, *btn;
   char path[PATH_MAX] = "/";
   
   if (!inst || !inst->tbar) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;

   EINA_LIST_FOREACH(inst->l_buttons, l, btn)
     {
        strcat(path, edje_object_part_text_get(btn, "e.text.label"));
        if (btn == obj) break;
        strcat(path, "/");
     }
   e_fm2_path_set(o_fm, "/", path);
}

static void 
_cb_changed(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst;

   inst = data;
   inst->tbar = event_info;
}

static Evas_Object*
_box_button_append(Instance *inst, const char *label,
                   void (*func)(void *data, Evas_Object *obj, const char *emission, const char *source))
{
   Evas_Object *o;
   Evas_Coord mw = 0, mh = 0;
   
   if (!inst || !label || !*label || !func) 
      return NULL;
   
   o = edje_object_add(evas_object_evas_get(inst->o_box));
   if (!e_theme_edje_object_set(o, "base/theme/modules/efm_pathbar", 
                                "modules/efm_pathbar/button"))
      edje_object_file_set(o, inst->theme_file, "modules/efm_pathbar/button");
   edje_object_signal_callback_add(o, "e,action,click", "", func, inst);
   edje_object_part_text_set(o, "e.text.label", label);
   edje_object_size_min_calc(o, &mw, &mh);
   e_box_pack_end(inst->o_box, o);
   evas_object_show(o);   
   e_box_pack_options_set(o, 1, 0, 0, 0, 0.5, 0.5, mw, mh, 9999, 9999);
   e_box_size_min_get(inst->o_box, &mw, NULL);
   evas_object_geometry_get(inst->o_scroll, NULL, NULL, NULL, &mh);
   evas_object_resize(inst->o_box, mw, mh);

   return o;
}

static void
_subdir_add(Instance *inst, const char *name)
{
   Evas_Object *btn;
   
   btn = _box_button_append(inst, name, _cb_button_click);
   if (btn)
      inst->l_buttons = eina_list_append(inst->l_buttons, btn);
}

static Eina_List*
_delete_buttons_tail(Instance *inst, Eina_List *l)
{
   Eina_List *l_next;
   Evas_Object *btn;
   
   for(l_next = eina_list_next(l), btn = eina_list_data_get(l); l; 
       l = l_next, l_next = eina_list_next(l), btn = eina_list_data_get(l))
     {
        e_box_unpack(btn);
        evas_object_del(btn);
        inst->l_buttons = eina_list_remove_list(inst->l_buttons, l);
     }

   return NULL;
}

static void
_cb_dir_changed(void *data, Evas_Object *obj, void *event_info) 
{
   Instance *inst = data;
   Evas_Object *o_fm, *btn, *sel;
   const char *realpath;
   char *path, *p, *dir;
   Eina_List *l;

   // safety checks
   if (!inst || !inst->tbar) return;
   o_fm = e_toolbar_fm2_get(inst->tbar);
   if (!o_fm) return;
   realpath = e_fm2_real_path_get(o_fm);
   if (!realpath) return;

   // root button
   if (!inst->o_root)
      inst->o_root = _box_button_append(inst, D_("Root"), _cb_root_click);
      
   // split path, make buttons
   l = inst->l_buttons;
   path = p = strdup(realpath);
   while (p)
     {
        dir = strsep(&p, "/");
        if (*dir)
          if (l)
             if (strcmp(dir, edje_object_part_text_get(eina_list_data_get(l),
                                                       "e.text.label")))
               {
                  l = _delete_buttons_tail(inst, l);
                  _subdir_add(inst, dir);
               }
             else
               l = eina_list_next(l);
          else
            _subdir_add(inst, dir);
     }
   free(path);

   // calc active button
   l = l ? eina_list_prev(l) : eina_list_last(inst->l_buttons);
   sel = l ? eina_list_data_get(l) : inst->o_root;
   
   // send signals
   edje_object_signal_emit(sel, "e,state,selected", "e");
   if (inst->o_root != sel)
      edje_object_signal_emit(inst->o_root, "e,state,default", "e");
   EINA_LIST_FOREACH(inst->l_buttons, l, btn)
     if (btn != sel)
        edje_object_signal_emit(btn, "e,state,default", "e");
   
   // scroll to selected button
   if (sel)
     {
        Evas_Coord x, y, w, h;
        
        evas_object_geometry_get(sel, &x, &y, &w, &h);
        e_scrollframe_child_region_show(inst->o_scroll, x, y, w, h);
     }
}
