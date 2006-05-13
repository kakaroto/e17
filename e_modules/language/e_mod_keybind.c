
#include <e.h>
#include "e_mod_lang.h"
#include "e_mod_keybind.h"

#define ACT_FN_GO(act) \
   static void _e_actions_act_##act##_go(E_Object *obj, const char *params)

#define ACT_GO(name) \
  { \
     act = e_action_add(#name); \
     if (act) act->func.go = _e_actions_act_##name##_go; \
  }

#define CFG_KEYBIND(_context, _key, _modifiers, _anymod, _action, _params) \
   eb = E_NEW(E_Config_Binding_Key, 1); \
   eb->context = _context; \
   eb->key = evas_stringshare_add(_key); \
   eb->modifiers = _modifiers; \
   eb->any_mod = _anymod; \
   eb->action = _action == NULL ? NULL : evas_stringshare_add(_action); \
   eb->params = _params == NULL ? NULL : evas_stringshare_add(_params)

/**** Extern *******/
extern Lang *lang;
/*******************/

/* private declarations */
ACT_FN_GO(lang_next_language);
ACT_FN_GO(lang_prev_language);

static void    _lang_update_delete_keybinding(E_Config_Binding_Key *e);
/***********************/

int
_lang_register_module_actions()
{
   E_Action *act;

   ACT_GO(lang_next_language);
   ACT_GO(lang_prev_language);

   return 1;
}

int
_lang_unregister_module_actions()
{
   e_action_del("lang_next_language");
   e_action_del("lang_prev_language");
   return 1;
}

int
_lang_register_module_keybindings(Lang *l)
{
   E_Config_Binding_Key *eb;
   Evas_List   *list;
   int found;

   e_register_action_predef_name("Language", "Switch To Next Language", "lang_next_language",
				 NULL, EDIT_RESTRICT_ACTION | EDIT_RESTRICT_PARAMS, 0);
   e_register_action_predef_name("Language", "Switch To Previous Language", "lang_prev_language",
				 NULL, EDIT_RESTRICT_ACTION | EDIT_RESTRICT_PARAMS, 0);

   e_managers_keys_ungrab();

   /* switch to next language */
   if (l->conf->bk_next.key && l->conf->bk_next.key[0])
     {
	CFG_KEYBIND(l->conf->bk_next.context, l->conf->bk_next.key,
		    l->conf->bk_next.modifiers, l->conf->bk_next.any_mod,
		    "lang_next_language", l->conf->bk_next.params);

	found = 0;
	for (list = e_config->key_bindings; list && !found; list = list->next)
	  {
	     E_Config_Binding_Key   *t = list->data;

	     if (!strcmp(!t->action ? "" : t->action, eb->action) &&
	         !strcmp(!t->params ? "" : t->params, !eb->params ? "" : eb->params))
	       {
		  found = 1;
		  break;
	       }
	  }
	if (!found)
	  {
	     e_config->key_bindings = evas_list_append(e_config->key_bindings, eb);

	     e_bindings_key_add(l->conf->bk_next.context, l->conf->bk_next.key,
				l->conf->bk_next.modifiers, l->conf->bk_next.any_mod,
				"lang_next_language", l->conf->bk_next.params);
	  }
     }

   /* switch to prev language */
   if (l->conf->bk_prev.key && l->conf->bk_prev.key[0])
     {
	CFG_KEYBIND(l->conf->bk_prev.context, l->conf->bk_prev.key,
		    l->conf->bk_prev.modifiers, l->conf->bk_prev.any_mod,
		    "lang_prev_language", l->conf->bk_prev.params);

	found = 0;
	for (list = e_config->key_bindings; list && !found; list = list->next)
	  {
	     E_Config_Binding_Key   *t = list->data;

	     if (!strcmp(!t->action ? "" : t->action, eb->action) &&
	         !strcmp(!t->params ? "" : t->params, !eb->params ? "" : eb->params))
	       {
		  found = 1;
		  break;
	       }
	  }

	if (!found)
	  {
	     e_config->key_bindings = evas_list_append(e_config->key_bindings, eb);

	     e_bindings_key_add(l->conf->bk_prev.context, l->conf->bk_prev.key,
				l->conf->bk_prev.modifiers, l->conf->bk_prev.any_mod,
				"lang_prev_language", l->conf->bk_prev.params);
	  }
     }

   e_managers_keys_grab();
   return 1;
}


int _lang_unregister_module_keybindings(Lang *l)
{
   e_managers_keys_ungrab();

   /* switch to next language */
   _lang_update_delete_keybinding(&(l->conf->bk_next));


   /* switch to prev language */
   _lang_update_delete_keybinding(&(l->conf->bk_prev));

   e_managers_keys_grab();

   e_unregister_action_predef_name("Language", "Switch To Next Language");
   e_unregister_action_predef_name("Language", "Switch To Previous Language");
   e_config_save();

   return 1;
}


ACT_FN_GO(lang_next_language)
{

   lang_switch_language_next(lang);
   return;
}

ACT_FN_GO(lang_prev_language)
{
   lang_switch_language_prev(lang);
   return;
}

static void
_lang_update_delete_keybinding(E_Config_Binding_Key *e)
{
   Evas_List *ll;
   int done = 0, found = 0;

   if (!e) return;

   while (!done)
     {
	done = 1;
	for (ll = e_config->key_bindings; ll; ll = ll->next)
	  {
	     E_Config_Binding_Key *eb;

	     eb = ll->data;

	     if (eb->action && !strcmp(eb->action, e->action == NULL ? "" : e->action))
		{
		   if (e->key) evas_stringshare_del(e->key);
		   if (e->params) evas_stringshare_del(e->params);

		   e->context	 = eb->context;
		   e->key	 = evas_stringshare_add(eb->key);
		   e->modifiers	 = eb->modifiers;
		   e->any_mod	 = eb->any_mod;
		   e->params	 = eb->params == NULL ? NULL : evas_stringshare_add(eb->params);

		   e_config->key_bindings = evas_list_remove_list(e_config->key_bindings, ll);
		   E_FREE(eb);

		   found = 1;
		   done = 0;
		   break;
		}
	  }
     }
   if (found)
     {
	e_bindings_key_del(e->context, e->key, e->modifiers,
			   e->any_mod, e->action, e->params);
     }
}

