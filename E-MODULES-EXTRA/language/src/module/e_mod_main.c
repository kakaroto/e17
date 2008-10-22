#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "e_mod_keybindings.h"
#include "e_mod_lang.h"
#include "config.h"

#define LANG_MODULE_CONFIG_FILE "module.language"
#define SELECTED_LANG_SET_RADIO_GROUP 1

/*********************** Shelf Code ************************************/

/*********** variables ***********************/
static	 E_Config_DD *conf_edd = NULL;
static	 E_Config_DD *conf_langlist_edd = NULL;
/*********************************************/

/************ private funcs **************************/
static void _lang_button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _language_face_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _language_face_cb_menu_keybindings_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _language_face_cb_menu_switch_language_to(void *data, E_Menu *m, E_Menu_Item *mi);
static void _lang_menu_cb_post_deactivate(void *data, E_Menu *m);
/*****************************************************/

/* gadcon setup */

static E_Gadcon_Client	*_gc_init    (E_Gadcon *gc, const char *name, const char *id, const char *style);
static void		_gc_shutdown (E_Gadcon_Client *gcc);
static void		_gc_orient   (E_Gadcon_Client *gcc);
static char		*_gc_label   (void);
static Evas_Object	*_gc_icon    (Evas *evas);
static const char       *_gc_id_new  (void);

static const E_Gadcon_Client_Class  _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
   "language",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};

typedef struct _Instance   Instance;

struct _Instance
{
   E_Gadcon_Client   *gcc;
   Evas_Object	     *o_language;
   Evas_Object	     *o_flag;
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   char		     buf[4096];
   Evas_Object	     *o;
   E_Gadcon_Client   *gcc;
   Instance	     *inst;

   inst = E_NEW(Instance, 1);

   o = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(o, "base/theme/modules/language", "modules/language/main"))
     {
	snprintf(buf, sizeof(buf), "%s/language.edj", e_module_dir_get(language_config->module));
	edje_object_file_set(o, buf, "modules/language/main");
     }

   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_language = o;

   inst->o_flag = e_icon_add(gc->evas);
   snprintf(buf, sizeof(buf), "%s/images/unknown_flag.png",
	    e_module_dir_get(language_config->module));
   e_icon_file_set(inst->o_flag, buf);
   edje_object_part_swallow(inst->o_language, "language_flag", inst->o_flag);

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
				  _lang_button_cb_mouse_down, inst);

   language_config->instances = eina_list_append(language_config->instances, inst);
   lang_language_switch_to(language_config, language_config->language_selector);
   return gcc;
}
static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   language_config->instances = eina_list_remove(language_config->instances, inst);
   evas_object_del(inst->o_language);
   evas_object_del(inst->o_flag);
   free(inst);
}
static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}
static char *
_gc_label(void)
{
   return D_("Language");
}
static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char	       buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-language.edj",
	    e_module_dir_get(language_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void)
{
   return _gadcon_class.name;
}

/* module setup */
EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
   "Language"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   int	      load_default_config = 0;
   Eina_List *l;
   char       buf[4096];

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_langlist_edd = E_CONFIG_DD_NEW("Language_List_Config", Language);
#undef T
#undef D
#define T   Language
#define D   conf_langlist_edd
   E_CONFIG_VAL(D, T, id, UINT);
   E_CONFIG_VAL(D, T, lang_name, STR);
   E_CONFIG_VAL(D, T, lang_shortcut, STR);
   E_CONFIG_VAL(D, T, lang_flag, STR);
   E_CONFIG_VAL(D, T, rdefs.model, STR);
   E_CONFIG_VAL(D, T, rdefs.layout, STR);
   E_CONFIG_VAL(D, T, rdefs.variant, STR);

   conf_edd = E_CONFIG_DD_NEW("Language_Config", Config);
#undef T
#undef D
#define T   Config
#define D   conf_edd
   E_CONFIG_VAL(D, T, lang_policy, INT);
   E_CONFIG_VAL(D, T, lang_show_indicator, INT);
   //E_CONFIG_SUB(D, T, bk_next, l->conf_bk_next_edd);
   E_CONFIG_VAL(D, T, switch_next_lang_key.context, INT);
   E_CONFIG_VAL(D, T, switch_next_lang_key.modifiers, INT);
   E_CONFIG_VAL(D, T, switch_next_lang_key.key, STR);
   E_CONFIG_VAL(D, T, switch_next_lang_key.action, STR);
   E_CONFIG_VAL(D, T, switch_next_lang_key.params, STR);
   E_CONFIG_VAL(D, T, switch_next_lang_key.any_mod, UCHAR);
   //E_CONFIG_SUB(D, T, bk_prev, l->conf_bk_prev_edd);
   E_CONFIG_VAL(D, T, switch_prev_lang_key.context, INT);
   E_CONFIG_VAL(D, T, switch_prev_lang_key.modifiers, INT);
   E_CONFIG_VAL(D, T, switch_prev_lang_key.key, STR);
   E_CONFIG_VAL(D, T, switch_prev_lang_key.action, STR);
   E_CONFIG_VAL(D, T, switch_prev_lang_key.params, STR);
   E_CONFIG_VAL(D, T, switch_prev_lang_key.any_mod, UCHAR);
   //
   E_CONFIG_LIST(D, T, languages, conf_langlist_edd);

   language_config = e_config_domain_load(LANG_MODULE_CONFIG_FILE, conf_edd);
   if (!language_config)
     {
	language_config = E_NEW(Config, 1);
	load_default_config = 1;
     }

   lang_load_xfree_kbd_models(language_config);
   lang_load_xfree_language_kbd_layouts(language_config);

   if (load_default_config)
     {
	Language  *lang;
	language_config->lang_policy = LS_GLOBAL_POLICY;
	language_config->lang_show_indicator = 1;

	/* switch to next language */
	language_config->switch_next_lang_key.context	 = E_BINDING_CONTEXT_ANY;
	language_config->switch_next_lang_key.key	 = evas_stringshare_add("period");
	language_config->switch_next_lang_key.modifiers	 = E_BINDING_MODIFIER_CTRL |
							   E_BINDING_MODIFIER_ALT;
	language_config->switch_next_lang_key.any_mod	 = 0;
	language_config->switch_next_lang_key.action	 = evas_stringshare_add(LANG_NEXT_ACTION);
	language_config->switch_next_lang_key.params	 = NULL;

	/* switch to prev language */
	language_config->switch_prev_lang_key.context	 = E_BINDING_CONTEXT_ANY;
	language_config->switch_prev_lang_key.key	 = evas_stringshare_add("comma");
	language_config->switch_prev_lang_key.modifiers	 = E_BINDING_MODIFIER_CTRL |
							   E_BINDING_MODIFIER_ALT;
	language_config->switch_prev_lang_key.any_mod	 = 0;
	language_config->switch_prev_lang_key.action	 = evas_stringshare_add(LANG_PREV_ACTION);
	language_config->switch_prev_lang_key.params	 = NULL;

	lang = lang_get_default_language(language_config);
	if (lang) language_config->languages = eina_list_append(language_config->languages, lang);
     }
   E_CONFIG_LIMIT(language_config->lang_policy, LS_GLOBAL_POLICY, LS_UNKNOWN_POLICY - 1);
   E_CONFIG_LIMIT(language_config->lang_show_indicator, 0, 1);

   language_config->module = m;

   /* initializing languages */
   for (l = language_config->languages; l; l = l->next)
     {
	lang_language_xorg_values_get(l->data);
     }

   language_config->l.current = e_border_focused_get();

   e_gadcon_provider_register((E_Gadcon_Client_Class *)(&_gadcon_class));

   language_register_callback_handlers();

   lang_register_module_actions();
   lang_register_module_keybindings();

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   Eina_List *l;

   e_gadcon_provider_unregister((E_Gadcon_Client_Class *)(&_gadcon_class));

   language_unregister_callback_handlers();
   language_clear_border_language_setup_list();

   lang_language_switch_to(language_config, 0);

   if (language_config->config_dialog)
     e_object_del(E_OBJECT(language_config->config_dialog));
   if (language_config->menu)
     {
	e_menu_post_deactivate_callback_set(language_config->menu, NULL, NULL);
	e_object_del(E_OBJECT(language_config->menu));
	language_config->menu = NULL;
     }

   lang_free_xfree_language_kbd_layouts(language_config);
   lang_free_xfree_kbd_models(language_config);

   lang_unregister_module_keybindings();
   lang_unregister_module_actions();

   for (l = language_config->languages; l; l = l->next)
     {
	lang_language_free(l->data);
     }

   free(language_config);
   language_config = NULL;

   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_langlist_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save(LANG_MODULE_CONFIG_FILE, conf_edd, language_config);
   return 1;
}

/************************* Just publics **************************************************/
void language_face_language_indicator_update()
{
   Eina_List   *l;
   Instance    *inst; 
   char lbuf[4096];

   if (!language_config) return;

   for (l = language_config->instances; l; l = l->next)
     {
	inst = l->data;

	evas_object_hide(inst->o_flag);
	edje_object_part_unswallow(inst->o_language, inst->o_flag);
	if (language_config->languages)
	  {
	     Language	*lang = eina_list_nth(language_config->languages,
					      language_config->language_selector);

	     snprintf(lbuf, sizeof(lbuf), "%s/images/%s.png",
		      e_module_dir_get(language_config->module), lang->lang_flag);
	     e_icon_file_set(inst->o_flag, lbuf);
	     edje_object_part_swallow(inst->o_language, "language_flag", inst->o_flag);
	     edje_object_part_text_set(inst->o_language, "langout", lang->lang_shortcut); 
	  }
	else 
	  { 
	     snprintf(lbuf, sizeof(lbuf), "%s/images/unknown_flag.png",
		      e_module_dir_get(language_config->module));
	     e_icon_file_set(inst->o_flag, lbuf);
	     edje_object_part_swallow(inst->o_language, "language_flag", inst->o_flag);
	     edje_object_part_text_set(inst->o_language, "langout", "");
	  }

     }
}

void language_register_callback_handlers()
{
   if (language_config->lang_policy == LS_GLOBAL_POLICY ||
       language_config->handlers) return;

   language_config->handlers = eina_list_append
      (language_config->handlers, ecore_event_handler_add
       (E_EVENT_DESK_SHOW, lang_cb_event_desk_show, language_config));

   language_config->handlers = eina_list_append
      (language_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_FOCUS_IN, lang_cb_event_border_focus_in, language_config));
   language_config->handlers = eina_list_append
      (language_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_REMOVE, lang_cb_event_border_remove, language_config));

   language_config->handlers = eina_list_append
      (language_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_ICONIFY, lang_cb_event_border_iconify, language_config));
}
void language_unregister_callback_handlers()
{ 
   while (language_config->handlers) 
     { 
	ecore_event_handler_del(language_config->handlers->data);
	language_config->handlers = eina_list_remove_list(language_config->handlers,
							  language_config->handlers);
     }
}
void language_clear_border_language_setup_list()
{
   if (!language_config) return;

   language_config->l.current = NULL;
   while (language_config->l.border_lang_setup)
     {
	Border_Language_Settings *bls = language_config->l.border_lang_setup->data;

	if (bls->language_name) evas_stringshare_del(bls->language_name);
	E_FREE(bls);

	language_config->l.border_lang_setup = 
	   eina_list_remove_list(language_config->l.border_lang_setup,
				 language_config->l.border_lang_setup);
     }
}
/************************* Private funcs *************************************************/
static void 
_lang_button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down   *ev;
   char buf[4096];

   if (!(inst = data)) return;
   ev = event_info;
   if ((ev->button == 3) && (!language_config->menu))
     {
	E_Menu	     *mn, *mn2;
	E_Menu_Item  *mi;
	int cx, cy, cw, ch;

	mn2 = e_menu_new();

	mi = e_menu_item_new(mn2);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _language_face_cb_menu_configure, NULL);

	mi = e_menu_item_new(mn2);
	e_menu_item_label_set(mi, D_("Configure Key Bindings"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _language_face_cb_menu_keybindings_configure, NULL);
	
	e_gadcon_client_util_menu_items_append(inst->gcc, mn2, 0);

	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);

	if (eina_list_count(language_config->languages) > 1)
	  { 
	     Eina_List	*l;
	     Language	*lang;
	     int	indx;

	     mn = e_menu_new(); 

	     mi = e_menu_item_new(mn); 
	     e_menu_item_label_set(mi, D_("Module Configuration")); 
	     e_menu_item_submenu_set(mi, mn2); 
	     e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration"); 

	     mi = e_menu_item_new(mn);
	     e_menu_item_separator_set(mi, 1);

	     for (l = language_config->languages, indx = 0; l; l = l->next, indx ++)
	       {
		  lang = l->data;

		  mi = e_menu_item_new(mn);
		  e_menu_item_label_set(mi, lang->lang_name);
		  snprintf(buf, sizeof(buf), "%s/images/%s.png", 
			   e_module_dir_get(language_config->module), lang->lang_flag);
		  e_menu_item_icon_file_set(mi, buf);
		  e_menu_item_radio_set(mi, 1);
		  e_menu_item_radio_group_set(mi, SELECTED_LANG_SET_RADIO_GROUP);
		  e_menu_item_toggle_set(mi, indx == language_config->language_selector ? 1 : 0);
		  e_menu_item_callback_set(mi, _language_face_cb_menu_switch_language_to, NULL);
	       }
	     
	     e_menu_post_deactivate_callback_set(mn, _lang_menu_cb_post_deactivate, inst); 
	     language_config->menu = mn; 
	     
	     e_menu_activate_mouse(mn,
				   e_util_zone_current_get(e_manager_current_get()), 
				   cx + ev->output.x, cy + ev->output.y, 1, 1, 
				   E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	  }
	else
	  {
	     e_menu_post_deactivate_callback_set(mn2, _lang_menu_cb_post_deactivate, inst); 
	     language_config->menu = mn2; 
	     
	     e_menu_activate_mouse(mn2,
				   e_util_zone_current_get(e_manager_current_get()), 
				   cx + ev->output.x, cy + ev->output.y, 1, 1, 
				   E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	  }

	evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
				 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
   else if (ev->button == 1)
     {
	lang_language_switch_to_next(language_config);
     }
}
static void 
_language_face_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   if (!language_config) return;
   if (language_config->config_dialog) return;
   _lang_configure_language_module(language_config);
}
static void
_language_face_cb_menu_keybindings_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   e_configure_registry_call("keyboard_and_mouse/key_bindings", e_container_current_get(e_manager_current_get()), NULL);
}
static void
_language_face_cb_menu_switch_language_to(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Eina_List   *l;
   Language    *lang;
   int	       indx;

   if (!language_config || !mi) return;

   for (l = language_config->languages, indx = 0; l; l = l->next, indx ++)
     {
	lang = l->data;

	if (!strcmp(lang->lang_name, mi->label))
	  {
	     if (language_config->language_selector == indx)
	       break;

	     lang_language_switch_to(language_config, indx);
	     break;
	  }
     }
}
static void
_lang_menu_cb_post_deactivate(void *data, E_Menu *m)
{
   if (!language_config->menu) return;
   e_object_del(E_OBJECT(language_config->menu));
   language_config->menu = NULL;
}
/***********************************************************************/
