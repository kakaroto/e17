#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "e_mod_keybind.h"
#include "e_mod_lang.h"
#include "config.h"

#define LANG_MODULE_CONFIG_FILE "module.language"

static Lang   *_lang_init(E_Module *m);
static void   _lang_shutdown(Lang *l);

static void   _lang_config_menu_new(Lang *l);

static int    _lang_face_init(Lang_Face *lf);
static void   _lang_face_menu_new(Lang_Face *lf);
static void   _lang_face_enable(Lang_Face *lf);
static void   _lang_face_disable(Lang_Face *lf);
static void   _lang_face_free(Lang_Face *lf);

static void   _lang_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void   _lang_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void   _lang_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi);
static void   _lang_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

static void   _lang_load_config(Lang *l);

static int lang_count;

Lang  *lang = NULL;

EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Language"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   Lang	*l;

   l = _lang_init(m);

   if (!l)
     return NULL;

   lang = l;

   m->config_menu = l->config_menu;
   return l;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   Lang	*l;

   l = m->data;
   if (!l)
     return 0;

   if (m->config_menu)
     {
	e_menu_deactivate(m->config_menu);
	e_object_del(E_OBJECT(m->config_menu));
	m->config_menu = NULL;
     }
   if (l->cfd)
     {
	e_object_del(E_OBJECT(l->cfd));
	l->cfd = NULL;
     }
   _lang_shutdown(l);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Lang *l;

   l = m->data;
   if (!l)
     return 0;
   e_config_domain_save(LANG_MODULE_CONFIG_FILE, l->conf_edd, l->conf);
   e_config_domain_save(LANG_MODULE_CONFIG_FILE, l->conf_bk_next_edd, l->conf->bk_next);
   e_config_domain_save(LANG_MODULE_CONFIG_FILE, l->conf_bk_prev_edd, l->conf->bk_prev);
   return 1;
}

EAPI int
e_modapi_info(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/module_icon.png", e_module_dir_get(m));
   m->icon_file = strdup(buf);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show( _("Enlightenment Language Enhancment Module"),
			 _("This module is used to switch between input languages."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   Lang *l;
   E_Container *con;

   l = m->data;
   if (!l)
     return 0;
   if (!l->face)
     return 0;

   con = e_container_current_get(e_manager_current_get());
   if (l->face->con == con)
     _lang_configure_lang_module(con, l);

   return 1;
}

static Lang *
_lang_init(E_Module *m)
{
   Lang		*l;
   E_Menu_Item	*mi;
   Evas_List	*managers, *l1, *l2;

   l = E_NEW(Lang, 1);

   if (!l)
     return NULL;

   l->module   = m;
   l->current_lang_selector = 0;

   _lang_load_config(l);


   _lang_config_menu_new(l);

   managers = e_manager_list();
   for (l1 = managers; l1; l1 = l1->next)
     {
	E_Manager *man;

	man = l1->data;
	for (l2 = man->containers; l2; l2 = l2->next)
	  {
	     E_Container *con;
	     Lang_Face	 *lf;

	     con = l2->data;
	     lf = E_NEW(Lang_Face, 1);

	     if (lf)
	       {
		  lf->conf_face_edd = E_CONFIG_DD_NEW("Lang_Face_Config", Config_Face);
#undef T
#undef D
#define T Config_Face
#define D lf->conf_face_edd
		  E_CONFIG_VAL(D, T, enabled, UCHAR);

		  l->face   = lf;
		  lf->lang  = l;
		  lf->con   = con;
		  lf->evas  = con->bg_evas;

		  lf->conf = E_NEW(Config_Face, 1);
		  lf->conf->enabled = 1;

		  if (!_lang_face_init(lf))
		    return NULL;

		  _lang_face_menu_new(lf);

		  mi = e_menu_item_new(l->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, lf->menu);

		  if (!lf->conf->enabled)
		    _lang_face_disable(lf);
		  else
		    _lang_face_enable(lf);
	       }
	  }
     }

   _lang_register_module_actions();
   _lang_register_module_keybindings(l);

   /*const char *en_l[] = {"en", "en_US", NULL};
   const char *en_v[] = {"basic", NULL};
   const char *ru_l[] = {"ru", "ru_KOI", NULL};
   const char *ru_v[] = {"basic", "difficult", NULL};*/
   lang_register_language("English", "EN", NULL, "en", "basic");

   lang_register_language("Russian", "RU", NULL, "ru", NULL);

   lang_register_language("Lithuanian", "LT", NULL, "lt", NULL);
   return l;
}

static void
_lang_shutdown(Lang *l)
{
   _lang_unregister_module_actions();
   _lang_unregister_module_keybindings(l);

   while (l->conf->languages)
     {
	Language  *lang = l->conf->languages->data;
	if (lang->lang_name) evas_stringshare_del(lang->lang_name);
	if (lang->lang_shortcut) evas_stringshare_del(lang->lang_shortcut);
	if (lang->lang_flag) evas_stringshare_del(lang->lang_flag);
	if (lang->kbd_model) evas_stringshare_del(lang->kbd_model);
	if (lang->kbd_layout) evas_stringshare_del(lang->kbd_layout);
	if (lang->kbd_variant) evas_stringshare_del(lang->kbd_variant);
	E_FREE(lang);

	l->conf->languages = evas_list_remove_list(l->conf->languages, l->conf->languages);
     }

   _lang_face_free(l->face);

   E_FREE(l->conf);
   E_CONFIG_DD_FREE(l->conf_edd);
   E_FREE(l);
}

static void
_lang_config_menu_new(Lang *l)
{
   E_Menu *mn;

   mn = e_menu_new();
   l->config_menu = mn;
}

static int
_lang_face_init(Lang_Face *lf)
{
   Evas_Object *o;
   char buf[4096];

   evas_event_freeze(lf->evas);

   o = edje_object_add(lf->evas);
   lf->lang_obj = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/language", "modules/language/main"))
     {
	snprintf(buf, sizeof(buf), "%s/language.edj", e_module_dir_get(lf->lang->module));
	edje_object_file_set(o, buf, "modules/language/main");
     }
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);

   o = edje_object_add(lf->evas);
   lf->text_obj = o;

   if (!e_theme_edje_object_set(o, "base/theme/modules/language", "modules/language/text"))
     {
	snprintf(buf, sizeof(buf), "%s/language.edj", e_module_dir_get(lf->lang->module));
	edje_object_file_set(o, buf, "modules/language/text");
     }
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 0);
   evas_object_pass_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 255);
   evas_object_show(o);

   o = evas_object_rectangle_add(lf->evas);
   lf->event_obj = o;
   evas_object_layer_set(o, 3);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _lang_face_cb_mouse_down, lf);
   evas_object_show(o);

   lf->gmc = e_gadman_client_new(lf->con->gadman);
   e_gadman_client_domain_set(lf->gmc, LANG_MODULE_CONFIG_FILE, lang_count++);
   e_gadman_client_policy_set(lf->gmc,
			      E_GADMAN_POLICY_EDGES | 
			      E_GADMAN_POLICY_HMOVE | 
			      E_GADMAN_POLICY_VMOVE );
   e_gadman_client_auto_size_set(lf->gmc, 48, 48);
   e_gadman_client_align_set(lf->gmc, 1.0, 1.0);
   e_gadman_client_resize(lf->gmc, 48, 48);
   e_gadman_client_change_func_set(lf->gmc, _lang_face_cb_gmc_change, lf);
   e_gadman_client_load(lf->gmc);

   evas_event_thaw(lf->evas);

   //lf->monitor = ecore_timer_add(1.0, _lang_face_update, lf);
   return 1;
}

static void
_lang_face_menu_new(Lang_Face *lf)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   mn = e_menu_new();
   lf->menu = mn;

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
   e_menu_item_callback_set(mi, _lang_face_cb_menu_configure, lf);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");
   e_menu_item_callback_set(mi, _lang_face_cb_menu_edit, lf);

   /* here should go items indicating selected languages */
}

static void
_lang_face_enable(Lang_Face *lf)
{
   lf->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(lf->lang_obj);
   evas_object_show(lf->text_obj);
   evas_object_show(lf->event_obj);
}

static void
_lang_face_disable(Lang_Face *lf)
{
   lf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(lf->event_obj);
   evas_object_hide(lf->text_obj);
   evas_object_hide(lf->lang_obj);
}

static void
_lang_face_free(Lang_Face *lf)
{
   if (lf->monitor)
     ecore_timer_del(lf->monitor);
   if (lf->menu)
     e_object_del(E_OBJECT(lf->menu));
   if (lf->event_obj)
     evas_object_del(lf->event_obj);
   if (lf->text_obj)
     evas_object_del(lf->text_obj);
   if (lf->lang_obj)
     evas_object_del(lf->lang_obj);

   if (lf->gmc)
     {
	e_gadman_client_save(lf->gmc);
	e_object_del(E_OBJECT(lf->gmc));
     }
   E_FREE(lf->conf);
   E_FREE(lf);
   lang_count--;
}

static void
_lang_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change)
{
   Lang_Face *lf;
   Evas_Coord x, y, w, h;

   lf = data;
   switch (change)
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	 e_gadman_client_geometry_get(lf->gmc, &x, &y, &w, &h);
	 evas_object_move(lf->event_obj, x, y);
	 evas_object_move(lf->text_obj, x, y);
	 evas_object_move(lf->lang_obj, x, y);
	 evas_object_resize(lf->event_obj, w, h);
	 evas_object_resize(lf->text_obj, w, h);
	 evas_object_resize(lf->lang_obj, w, h);
	 break;
      case E_GADMAN_CHANGE_RAISE:
	 evas_object_raise(lf->lang_obj);
	 evas_object_raise(lf->event_obj);
	 evas_object_raise(lf->text_obj);
	 break;
      case E_GADMAN_CHANGE_EDGE:
      case E_GADMAN_CHANGE_ZONE:
	 /* ignore */
	 break;
      default:
	 break;
     }
}

static void
_lang_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Lang_Face  *lf;
   Evas_Event_Mouse_Down  *ev;

   ev = event_info;
   lf = data;

   if (ev->button == 3)
     {
	/* here goes a context menu */
	e_menu_activate_mouse(lf->menu, e_zone_current_get(lf->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN,
			      ev->timestamp);
	e_util_container_fake_mouse_up_all_later(lf->con);
     }
   else if (ev->button == 1)
     {
	/* here goes code for switching language */
     }
}

static void
_lang_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi)
{
   Lang_Face  *lf;

   lf = data;
   e_gadman_mode_set(lf->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void
_lang_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi)
{
   Lang_Face  *lf;

   lf = data;
   _lang_configure_lang_module(lf->con, lf->lang);
}

static void
_lang_load_config(Lang *l)
{
   l->conf_bk_next_edd = E_CONFIG_DD_NEW("Lang_BK_Next_Config", E_Config_Binding_Key);
#undef T
#undef D
#define T E_Config_Binding_Key
#define D l->conf_bk_next_edd
   E_CONFIG_VAL(D, T, context, INT);
   E_CONFIG_VAL(D, T, modifiers, INT);
   E_CONFIG_VAL(D, T, key, STR);
   E_CONFIG_VAL(D, T, action, STR);
   E_CONFIG_VAL(D, T, params, STR);
   E_CONFIG_VAL(D, T, any_mod, UCHAR);

   l->conf_bk_prev_edd = E_CONFIG_DD_NEW("Lang_BK_Prev_Config", E_Config_Binding_Key);
#undef T
#undef D
#define T E_Config_Binding_Key
#define D l->conf_bk_prev_edd
   E_CONFIG_VAL(D, T, context, INT);
   E_CONFIG_VAL(D, T, modifiers, INT);
   E_CONFIG_VAL(D, T, key, STR);
   E_CONFIG_VAL(D, T, action, STR);
   E_CONFIG_VAL(D, T, params, STR);
   E_CONFIG_VAL(D, T, any_mod, UCHAR);

   l->conf_edd = E_CONFIG_DD_NEW("Lang_Config", Config);
#undef T
#undef D
#define T Config
#define D l->conf_edd
   E_CONFIG_VAL(D, T, lang_policy, INT);
   E_CONFIG_VAL(D, T, lang_show_indicator, INT);
   E_CONFIG_SUB(D, T, bk_next, l->conf_bk_next_edd);
   E_CONFIG_SUB(D, T, bk_prev, l->conf_bk_prev_edd);

   l->conf = e_config_domain_load(LANG_MODULE_CONFIG_FILE, l->conf_edd);
   if (!l->conf)
     {
	l->conf = E_NEW(Config, 1);
	l->conf->lang_policy = LS_GLOBAL_POLICY;
	l->conf->lang_show_indicator = 1;

	l->conf->bk_next = E_NEW(E_Config_Binding_Key, 1);
	l->conf->bk_next->context = E_BINDING_CONTEXT_ANY;
	l->conf->bk_next->key = evas_stringshare_add("q");
	l->conf->bk_next->modifiers = E_BINDING_MODIFIER_CTRL;
	l->conf->bk_next->any_mod = 0;
	l->conf->bk_next->action = evas_stringshare_add("lang_next_language");
	l->conf->bk_next->params = NULL;

	l->conf->bk_prev = E_NEW(E_Config_Binding_Key, 1);
	l->conf->bk_prev->context = E_BINDING_CONTEXT_ANY;
	l->conf->bk_prev->key = evas_stringshare_add("w");
	l->conf->bk_prev->modifiers = E_BINDING_MODIFIER_CTRL;
	l->conf->bk_prev->any_mod = 0;
	l->conf->bk_prev->action = evas_stringshare_add("lang_prev_language");
	l->conf->bk_prev->params = NULL;
     }
   E_CONFIG_LIMIT(l->conf->lang_policy, LS_GLOBAL_POLICY, LS_UNKNOWN_POLICY - 1);
   E_CONFIG_LIMIT(l->conf->lang_show_indicator, 0, 1);

   /* this is for debug */
   Language *ll;

   ll = E_NEW(Language, 1);
   if (ll)
     {
	ll->lang_name = evas_stringshare_add("English");
	ll->lang_shortcut = evas_stringshare_add("EN");
	ll->lang_flag = evas_stringshare_add("en_flag");
	ll->kbd_model = evas_stringshare_add("compaqik13");
	ll->kbd_layout = evas_stringshare_add("us");
	ll->kbd_variant = evas_stringshare_add("basic");

	l->conf->languages = evas_list_append(l->conf->languages, ll);
     }

   ll = E_NEW(Language, 1);
   if (ll)
     {
	ll->lang_name = evas_stringshare_add("Russian");
	ll->lang_shortcut = evas_stringshare_add("RU");
	ll->lang_flag = evas_stringshare_add("ru_flag");
	ll->kbd_model = evas_stringshare_add("compaqik13");
	ll->kbd_layout = evas_stringshare_add("ru");
	ll->kbd_variant = evas_stringshare_add("basic");

	l->conf->languages = evas_list_append(l->conf->languages, ll);
     }

   ll = E_NEW(Language, 1);
   if (ll)
     {
	ll->lang_name = evas_stringshare_add("Italian");
	ll->lang_shortcut = evas_stringshare_add("IT");
	ll->lang_flag = evas_stringshare_add("it_flag");
	ll->kbd_model = evas_stringshare_add("compaqik13");
	ll->kbd_layout = evas_stringshare_add("it");
	ll->kbd_variant = evas_stringshare_add("basic");

	l->conf->languages = evas_list_append(l->conf->languages, ll);
     }
}

/*static void
_lang_load_config(Lang *l)
{
   E_Config_Binding_Key	*eb;

   l->conf_edd = E_CONFIG_DD_NEW("Lang_Config", Config);
#undef T
#undef D
#define T Config
#define D l->conf_edd
   E_CONFIG_VAL(D, T, lang_policy, INT);
   E_CONFIG_VAL(D, T, lang_show_indicator, INT);
   //E_CONFIG_SUB(D, T, bk_next, l->conf_bk_next_edd);

   l->conf = e_config_domain_load(LANG_MODULE_CONFIG_FILE, l->conf_edd);
   if (!l->conf)
     {
	l->conf = E_NEW(Config, 1);

	l->conf->lang_policy = LS_GLOBAL_POLICY;
	l->conf->lang_show_indicator = 1;

     }
   E_CONFIG_LIMIT(l->conf->lang_policy, LS_GLOBAL_POLICY, LS_UNKNOWN_POLICY - 1);
   E_CONFIG_LIMIT(l->conf->lang_show_indicator, 0, 1);

   l->conf_bk_next_edd = E_CONFIG_DD_NEW("Lang_BK_Next_Config", E_Config_Binding_Key);
#undef T
#undef D
#define T E_Config_Binding_Key
#define D l->conf_bk_next_edd
   E_CONFIG_VAL(D, T, context, INT);
   E_CONFIG_VAL(D, T, modifiers, INT);
   E_CONFIG_VAL(D, T, key, STR);
   E_CONFIG_VAL(D, T, action, STR);
   E_CONFIG_VAL(D, T, params, STR);
   E_CONFIG_VAL(D, T, any_mod, UCHAR);

   eb = e_config_domain_load(LANG_MODULE_CONFIG_FILE, l->conf_bk_next_edd);
   if (!eb)
     {
	l->conf->bk_next.context = E_BINDING_CONTEXT_ANY;
	l->conf->bk_next.key = evas_stringshare_add("q");
	l->conf->bk_next.modifiers = E_BINDING_MODIFIER_CTRL;
	l->conf->bk_next.any_mod = 0;
	l->conf->bk_next.action = evas_stringshare_add("lang_next_language");
	l->conf->bk_next.params = NULL;
     }
   else
     {
	l->conf->bk_next.context = eb->context;
	l->conf->bk_next.key = evas_stringshare_add(eb->key);
	l->conf->bk_next.modifiers = eb->modifiers;	
	l->conf->bk_next.any_mod = eb->any_mod;
	l->conf->bk_next.action = evas_stringshare_add(eb->action);
	l->conf->bk_next.params = eb->params == NULL ? NULL : evas_stringshare_add(eb->params);

	if (eb->key) evas_stringshare_del(eb->key);
	if (eb->action) evas_stringshare_del(eb->action);
	if (eb->params) evas_stringshare_del(eb->params);
	E_FREE(eb);
     }

   l->conf_bk_prev_edd = E_CONFIG_DD_NEW("Lang_BK_Prev_Config", E_Config_Binding_Key);
#undef T
#undef D
#define T E_Config_Binding_Key
#define D l->conf_bk_prev_edd
   E_CONFIG_VAL(D, T, context, INT);
   E_CONFIG_VAL(D, T, modifiers, INT);
   E_CONFIG_VAL(D, T, key, STR);
   E_CONFIG_VAL(D, T, action, STR);
   E_CONFIG_VAL(D, T, params, STR);
   E_CONFIG_VAL(D, T, any_mod, UCHAR);

   eb = e_config_domain_load(LANG_MODULE_CONFIG_FILE, l->conf_bk_prev_edd);
   if (!eb)
     {
	l->conf->bk_prev.context = E_BINDING_CONTEXT_ANY;
	l->conf->bk_prev.key = evas_stringshare_add("w");
	l->conf->bk_prev.modifiers = E_BINDING_MODIFIER_CTRL;
	l->conf->bk_prev.any_mod = 0;
	l->conf->bk_prev.action = evas_stringshare_add("lang_prev_language");
	l->conf->bk_prev.params = NULL;
     }
   else
     {
	l->conf->bk_prev.context = eb->context;
	l->conf->bk_prev.key = evas_stringshare_add(eb->key);
	l->conf->bk_prev.modifiers = eb->modifiers;	
	l->conf->bk_prev.any_mod = eb->any_mod;
	l->conf->bk_prev.action = evas_stringshare_add(eb->action);
	l->conf->bk_prev.params = eb->params == NULL ? NULL : evas_stringshare_add(eb->params);

	if (eb->key) evas_stringshare_del(eb->key);
	if (eb->action) evas_stringshare_del(eb->action);
	if (eb->params) evas_stringshare_del(eb->params);
	E_FREE(eb);
     }
}*/

/*static int
_cpu_face_update_values(void *data)
{
   Cpu_Face *cf;
   char str[100];
   int i = 0;
   char str_tmp[100];

   cf = data;
   _cpu_face_get_load(cf);

   if (cpu_stats[0] == -1)
      return 1;

   if (cf->cpu->conf->show_text)
     {
        snprintf(str, sizeof(str), "%d%%", cpu_stats[0]);
        i = 1;
        while (i < cpu_count)
          {
             snprintf(str_tmp, sizeof(str_tmp), " / %d%%", cpu_stats[i]);
             strncat(str, str_tmp, sizeof(str));
             i++;
          }
        edje_object_part_text_set(cf->txt_obj, "in-text", str);
     }
   else
      edje_object_part_text_set(cf->txt_obj, "in-text", "");

   if ((cf->cpu->conf->show_graph) && (edje_object_part_exists(cf->cpu_obj, "lines")))
      _cpu_face_graph_values(cf);
   else
      _cpu_face_graph_clear(cf);

   return 1;
}*/
