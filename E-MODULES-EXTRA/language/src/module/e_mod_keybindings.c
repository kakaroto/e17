
#include <e.h>
#include "e_mod_lang.h"
#include "e_mod_keybindings.h"

#define LANG_ACTION_GROUP	    D_("Language")
#define LANG_ACTION_NEXT_LANGUAGE   D_("Switch To Next Language")
#define LANG_ACTION_PREV_LANGUAGE   D_("Switch To Previous Language")

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


/********* Private Declarations ***************/

ACT_FN_GO(switch_next_language);
ACT_FN_GO(switch_prev_language);

static void _lang_register_module_keybinding(E_Config_Binding_Key *key, const char *action);
void _lang_unregister_module_keybinding(E_Config_Binding_Key *key, int save);
/************* externs **************/
Config *language_config;
/************************************/



/*************** public **********************/
int
lang_register_module_actions()
{
   E_Action *act;

   e_action_predef_name_set(LANG_ACTION_GROUP, LANG_ACTION_NEXT_LANGUAGE, LANG_NEXT_ACTION,
				 NULL, NULL, 0);
   e_action_predef_name_set(LANG_ACTION_GROUP, LANG_ACTION_PREV_LANGUAGE, LANG_PREV_ACTION,
				 NULL, NULL, 0);

   // these two are strongly connected with LANG_NEXT_ACTION and LANG_PREV_ACTION
   ACT_GO(switch_next_language);
   ACT_GO(switch_prev_language);

   return 1;
}
int
lang_unregister_module_actions()
{
   //FIXME: shold we check if these actions exist?
   e_action_del(LANG_NEXT_ACTION);
   e_action_del(LANG_PREV_ACTION);
   e_action_predef_name_del(LANG_ACTION_GROUP, LANG_ACTION_NEXT_LANGUAGE); 
   e_action_predef_name_del(LANG_ACTION_GROUP, LANG_ACTION_PREV_LANGUAGE);
   return 1;
}

int
lang_register_module_keybindings()
{
   e_managers_keys_ungrab();

   /* switch to next language action */
   _lang_register_module_keybinding(&(language_config->switch_next_lang_key), LANG_NEXT_ACTION);

   /* switch to prev language */
   _lang_register_module_keybinding(&(language_config->switch_prev_lang_key), LANG_PREV_ACTION);

   e_managers_keys_grab();

   return 1;
}

int
lang_unregister_module_keybindings()
{
   e_managers_keys_ungrab();

   /* switch to next language */
   _lang_unregister_module_keybinding(&(language_config->switch_next_lang_key), 1);

   /* switch to prev language */
   _lang_unregister_module_keybinding(&(language_config->switch_prev_lang_key), 1);

   e_managers_keys_grab();

   return 1;
}

void
_lang_unregister_module_keybinding(E_Config_Binding_Key *key, int save)
{
   Eina_List   *l;
   int done = 0, found = 0;
   E_Config_Binding_Key *eb;

   if (!key) return;

   while (!done)
     {
	done = 1;
	for (l = e_config->key_bindings; l; l = l->next)
	  {
	     eb = l->data;

	     if (eb && eb->action && eb->action && 
	         !strcmp(!eb->action ? "" : eb->action, !key->action ? "" : key->action))
	       {
		  if (save)
		    { 
		       if (key->key) evas_stringshare_del(key->key);
		       if (key->params) evas_stringshare_del(key->params);

		       key->context    = eb->context;
		       key->key	       = evas_stringshare_add(eb->key);
		       key->modifiers  = eb->modifiers;
		       key->any_mod    = eb->any_mod;
		       key->params     = !eb->params ? NULL : evas_stringshare_add(eb->params);
		    }

		  e_bindings_key_del(eb->context, eb->key, eb->modifiers, 
				     eb->any_mod, eb->action, eb->params);

		  if (eb->key) evas_stringshare_del(eb->key);
		  if (eb->action) evas_stringshare_del(eb->action);
		  if (eb->params) evas_stringshare_del(eb->params);
		  E_FREE(eb);

		  e_config->key_bindings = eina_list_remove_list(e_config->key_bindings, l);
		  found = 1;
		  done = 0;
		  break;
	       }
	  }
     }

   if (!found)
     {
	//here we actually have to reset the key
	if (key->key) evas_stringshare_del(key->key);
	if (key->params) evas_stringshare_del(key->params);
	key->key = NULL;
	key->context = E_BINDING_CONTEXT_ANY;
	key->modifiers = E_BINDING_MODIFIER_NONE;
	key->any_mod = 0;
     }
}

static void
_lang_register_module_keybinding(E_Config_Binding_Key *key, const char *action)
{
   int found;
   Eina_List   *l;
   E_Config_Binding_Key	*eb;

   if (!key || !key->key || !key->key[0] || !action) return;

   CFG_KEYBIND(key->context, key->key, key->modifiers, key->any_mod, 
	       action, key->params);

   for (l = e_config->key_bindings, found = 0; l && !found; l = l->next)
     {
	E_Config_Binding_Key *t = l->data; 
	
	if (!strcmp(!t->action ? "" : t->action, eb->action) &&
	    !strcmp(!t->params ? "" : t->params, !eb->params ? "" : eb->params)) 
	  found = 1;
     }

   if (!found)
     {
	e_config->key_bindings = eina_list_append(e_config->key_bindings, eb); 
	
	e_bindings_key_add(key->context, key->key, key->modifiers, key->any_mod,
			   action, key->params);
     }
   else
     {
	if (eb->key) evas_stringshare_del(eb->key);
	if (eb->action) evas_stringshare_del(eb->action);
	if (eb->params) evas_stringshare_del(eb->params);
	E_FREE(eb);
     }
}


/*************** Private ********************/

ACT_FN_GO(switch_next_language)
{
   lang_language_switch_to_next(language_config);
   return;
}

ACT_FN_GO(switch_prev_language)
{
   lang_language_switch_to_prev(language_config);
   return;
}
