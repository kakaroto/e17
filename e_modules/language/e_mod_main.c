#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "e_mod_keybind.h"
#include "e_mod_lang.h"
#include "config.h"

#define LANG_MODULE_CONFIG_FILE "module.language"
#define SELECTED_LANG_SET_RADIO_GROUP 1

static Lang    *_lang_init(E_Module *m);
static void    _lang_shutdown(Lang *l);

static void    _lang_config_menu_new(Lang *l);

static int     _lang_face_init(Lang_Face *lf);
static void    _lang_face_menu_new(Lang_Face *lf);
static void    _lang_face_enable(Lang_Face *lf);
static void    _lang_face_disable(Lang_Face *lf);
static void    _lang_face_free(Lang_Face *lf);

static void    _lang_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void    _lang_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void    _lang_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi);
static void    _lang_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);

static void    _lang_cb_switch_to_language(void *data, E_Menu *m, E_Menu_Item *mi);

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
   lang = NULL;
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

void lang_face_menu_regenerate(Lang_Face *lf)
{
   E_Menu *mn;
   E_Menu_Item *mi;

   if (!lf) return;
   if (lf->menu) e_object_del(E_OBJECT(lf->menu));

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

   if (evas_list_count(lf->lang->conf->languages))
     {
	Evas_List *l;
	int indx;
	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);

	for (l = lf->lang->conf->languages, indx = 0; l; l = l->next, indx ++)
	  {
	     Language	*lang = l->data;

	     mi = e_menu_item_new(mn);
	     e_menu_item_label_set(mi, lang->lang_name); 
	     e_menu_item_radio_set(mi, 1);
	     e_menu_item_radio_group_set(mi, SELECTED_LANG_SET_RADIO_GROUP);
	     e_menu_item_toggle_set(mi, indx == lf->lang->current_lang_selector ? 1 : 0);
	     e_menu_item_callback_set(mi, _lang_cb_switch_to_language, lf->lang);

	     //e_menu_item_callback_set(mi, _lang_cp_switch_langauge, ..);
	     //e_menu_item_icon_edje_set(mi, (char *)e_theme_edje_file_get(...,...),...);
	  }

     }
}

static Lang *
_lang_init(E_Module *m)
{
   Lang		*l;
   E_Menu_Item	*mi;
   Evas_List	*managers, *l1, *l2;

   lang_load_kbd_models();
   lang_load_xfree_languages();

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

		  lang_face_language_indicator_set(l);

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


   return l;
}

static void
_lang_shutdown(Lang *l)
{
   _lang_unregister_module_keybindings(l);
   _lang_unregister_module_actions();

   lang_free_kbd_models();
   lang_free_xfree_languages();

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

   if (l->conf->bk_next.key) evas_stringshare_del(l->conf->bk_next.key);
   if (l->conf->bk_next.action) evas_stringshare_del(l->conf->bk_next.action);
   if (l->conf->bk_next.params) evas_stringshare_del(l->conf->bk_next.params);

   if (l->conf->bk_prev.key) evas_stringshare_del(l->conf->bk_prev.key);
   if (l->conf->bk_prev.action) evas_stringshare_del(l->conf->bk_prev.action);
   if (l->conf->bk_prev.params) evas_stringshare_del(l->conf->bk_prev.params);

   E_FREE(l->conf);
   E_CONFIG_DD_FREE(l->conf_edd);
   E_CONFIG_DD_FREE(l->conf_lang_list_edd);
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
   lang_face_menu_regenerate(lf);
/*   E_Menu *mn;
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

   if (evas_list_count(lf->lang->conf->languages))
     {
	Evas_List *l;
	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);

	for (l = lf->lang->conf->languages; l; l = l->next)
	  {
	     Language	*lang = l->data;

	     mi = e_menu_item_new(mn);
	     e_menu_item_label_set(mi, lang->lang_name);
	     //e_menu_item_callback_set(mi, _lang_cp_switch_langauge, ..);
	     //e_menu_item_icon_edje_set(mi, (char *)e_theme_edje_file_get(...,...),...);
	  }

     }*/

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
   if (!l) return;

   l->conf_lang_list_edd = E_CONFIG_DD_NEW("Lang_Language_List_Config", Language);
#undef T
#undef D
#define T   Language
#define D   l->conf_lang_list_edd
   E_CONFIG_VAL(D, T, id, UINT);
   E_CONFIG_VAL(D, T, lang_name, STR);
   E_CONFIG_VAL(D, T, lang_shortcut, STR);
   E_CONFIG_VAL(D, T, lang_flag, STR);
   E_CONFIG_VAL(D, T, kbd_model, STR);
   E_CONFIG_VAL(D, T, kbd_layout, STR);
   E_CONFIG_VAL(D, T, kbd_variant, STR);

/*   l->conf_bk_next_edd = E_CONFIG_DD_NEW("E_Config_Binding_Key_Next_Config", E_Config_Binding_Key);
#undef T
#undef D
#define T   E_Config_Binding_Key
#define D   l->conf_bk_next_edd
   E_CONFIG_VAL(D, T, context, INT);
   E_CONFIG_VAL(D, T, modifiers, INT);
   E_CONFIG_VAL(D, T, key, STR);
   E_CONFIG_VAL(D, T, action, STR);
   E_CONFIG_VAL(D, T, params, STR);
   E_CONFIG_VAL(D, T, any_mod, UCHAR);

   l->conf_bk_prev_edd = E_CONFIG_DD_NEW("E_Config_Binding_Key_Prev_COnfig", E_Config_Binding_Key);
#undef T
#undef D
#define T   E_Config_Binding_Key
#define D   l->conf_bk_prev_edd
   E_CONFIG_VAL(D, T, context, INT);
   E_CONFIG_VAL(D, T, modifiers, INT);
   E_CONFIG_VAL(D, T, key, STR);
   E_CONFIG_VAL(D, T, action, STR);
   E_CONFIG_VAL(D, T, params, STR);
   E_CONFIG_VAL(D, T, any_mod, UCHAR);*/

   l->conf_edd = E_CONFIG_DD_NEW("Lang_Config", Config);
#undef T
#undef D
#define T   Config
#define D   l->conf_edd
   E_CONFIG_VAL(D, T, lang_policy, INT);
   E_CONFIG_VAL(D, T, lang_show_indicator, INT);
   //E_CONFIG_SUB(D, T, bk_next, l->conf_bk_next_edd);
   E_CONFIG_VAL(D, T, bk_next.context, INT);
   E_CONFIG_VAL(D, T, bk_next.modifiers, INT);
   E_CONFIG_VAL(D, T, bk_next.key, STR);
   E_CONFIG_VAL(D, T, bk_next.action, STR);
   E_CONFIG_VAL(D, T, bk_next.params, STR);
   E_CONFIG_VAL(D, T, bk_next.any_mod, UCHAR);
   //E_CONFIG_SUB(D, T, bk_prev, l->conf_bk_prev_edd);
   E_CONFIG_VAL(D, T, bk_prev.context, INT);
   E_CONFIG_VAL(D, T, bk_prev.modifiers, INT);
   E_CONFIG_VAL(D, T, bk_prev.key, STR);
   E_CONFIG_VAL(D, T, bk_prev.action, STR);
   E_CONFIG_VAL(D, T, bk_prev.params, STR);
   E_CONFIG_VAL(D, T, bk_prev.any_mod, UCHAR);
   //
   E_CONFIG_LIST(D, T, languages, l->conf_lang_list_edd);

   l->conf = e_config_domain_load(LANG_MODULE_CONFIG_FILE, l->conf_edd);
   if (!l->conf)
     {
	Language  *_lang;

	l->conf = E_NEW(Config, 1);
	l->conf->lang_policy = LS_GLOBAL_POLICY;
	l->conf->lang_show_indicator = 1;

	l->conf->bk_next.context = E_BINDING_CONTEXT_ANY;
	l->conf->bk_next.key = evas_stringshare_add("q");
	l->conf->bk_next.modifiers = E_BINDING_MODIFIER_CTRL;
	l->conf->bk_next.any_mod = 0;
	l->conf->bk_next.action = evas_stringshare_add("lang_next_language");
	l->conf->bk_next.params = NULL;

	l->conf->bk_prev.context = E_BINDING_CONTEXT_ANY;
	l->conf->bk_prev.key = evas_stringshare_add("w");
	l->conf->bk_prev.modifiers = E_BINDING_MODIFIER_CTRL;
	l->conf->bk_prev.any_mod = 0;
	l->conf->bk_prev.action = evas_stringshare_add("lang_prev_language");
	l->conf->bk_prev.params = NULL;
	/*l->conf->bk_next = E_NEW(E_Config_Binding_Key, 1);
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
	l->conf->bk_prev->params = NULL;*/

	_lang  = lang_get_default_language();
	if (_lang) 
	  l->conf->languages = evas_list_append(l->conf->languages, _lang);
     }
   E_CONFIG_LIMIT(l->conf->lang_policy, LS_GLOBAL_POLICY, LS_UNKNOWN_POLICY - 1);
   E_CONFIG_LIMIT(l->conf->lang_show_indicator, 0, 1);
}

static void
_lang_cb_switch_to_language(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Lang	       *lang;
   Evas_List   *list;
   Language    *l;
   int indx;

   if (!(lang = data) || !mi) return;

   for (list = lang->conf->languages, indx = 0; list; list = list->next, indx ++)
     {
	l = list->data;

	if (!strcmp(l->lang_name, mi->label))
	  {
	     if (lang->current_lang_selector == indx)
	       break;

	     lang_switch_language_to(lang, indx);
	     break;
	  }
     }
}

void lang_face_menu_language_indicator_set(Lang *l)
{
   if (!l || !l->conf || !l->face || !l->face->menu) return;

   if (l->conf->languages)
     {
	Evas_List *list;
	Language  *lang = evas_list_nth(l->conf->languages, l->current_lang_selector);
	for (list = l->face->menu->items; list; list = list->next)
	  {
	     E_Menu_Item   *mi = list->data;
	     if (mi && mi->radio && mi->radio_group == SELECTED_LANG_SET_RADIO_GROUP)
	       {
		  if (!strcmp(lang->lang_name, mi->label))
		    e_menu_item_toggle_set(mi, 1);
		  else
		    e_menu_item_toggle_set(mi, 0);
	       }
	  }
     }
   else
     {
	Evas_List *list;
	for (list = l->face->menu->items; list; list = list->next)
	  {
	     E_Menu_Item   *mi = list->data;
	     if (mi && mi->radio && mi->radio_group == SELECTED_LANG_SET_RADIO_GROUP)
	       e_menu_item_toggle_set(mi, 0);
	  }
     }
}

void lang_face_language_indicator_set(Lang  *l)
{
   Language *lang;

   if (!l || !l->conf || !l->face || !l->face->text_obj) return;

   if (l->conf->languages)
     {
	/*Evas_Coord   x, y, w, h;
	Evas_Coord   tx, ty, tw, th;
	char buf[4096];*/

	lang = evas_list_nth(l->conf->languages, l->current_lang_selector); 
	//evas_event_freeze(l->face->evas);
	edje_object_part_text_set(l->face->text_obj, "in-text", lang->lang_shortcut);

	//edje_object_size_min_calc(l->face->lang_obj, &w, &h);
	//edje_object_size_min_calc(l->face->text_obj, &tw, &th);
	/*evas_object_move(l->face->text_obj, (int)((w - tw)/2), (int)((h - th)/2));*/

	//evas_event_thaw(l->face->evas);
	/*snprintf(buf, sizeof(buf), "w = %d : h = %d : tw = %d : th = %d", w, h, tw, th);
	e_module_dialog_show("bbbbbbbbbbbbbbbbbbbbb", buf);*/
     }
   else 
     edje_object_part_text_set(l->face->text_obj, "in-text", "");
}
