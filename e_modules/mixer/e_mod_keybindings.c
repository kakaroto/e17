/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"
#include "e_mod_types.h"
#include "e_mod_keybindings.h"

#define MIXER_ACTION_GROUP	    D_("Mixer")
#define MIXER_ACTION_DECREASE_VOL   D_("Decrease the volume")
#define MIXER_ACTION_INCREASE_VOL   D_("Increase the volume")
#define MIXER_ACTION_MUTE           D_("Mute the mixer")

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

ACT_FN_GO(decrease_volume);
ACT_FN_GO(increase_volume);
ACT_FN_GO(mute);

static void _mixer_register_module_keybinding(E_Config_Binding_Key *key, const char *action);
static void _mixer_unregister_module_keybinding(E_Config_Binding_Key *key, int save);
/************* externs **************/
Config *mixer_config;
/************************************/



/*************** public **********************/
int
mixer_register_module_actions(void)
{
   E_Action *act;

   e_action_predef_name_set(MIXER_ACTION_GROUP, MIXER_ACTION_DECREASE_VOL, MIXER_DECREASE_ACTION, NULL, NULL, 0);
   e_action_predef_name_set(MIXER_ACTION_GROUP, MIXER_ACTION_INCREASE_VOL, MIXER_INCREASE_ACTION, NULL, NULL, 0);
   e_action_predef_name_set(MIXER_ACTION_GROUP, MIXER_ACTION_MUTE, MIXER_MUTE_ACTION, NULL, NULL, 0);

   ACT_GO(decrease_volume);
   ACT_GO(increase_volume);
   ACT_GO(mute);

   return 1;
}

int
mixer_unregister_module_actions()
{
   e_action_del(MIXER_DECREASE_ACTION);
   e_action_del(MIXER_INCREASE_ACTION);
   e_action_del(MIXER_MUTE_ACTION);
   e_action_predef_name_del(MIXER_ACTION_GROUP, MIXER_ACTION_DECREASE_VOL);
   e_action_predef_name_del(MIXER_ACTION_GROUP, MIXER_ACTION_INCREASE_VOL);
   e_action_predef_name_del(MIXER_ACTION_GROUP, MIXER_ACTION_MUTE);
   e_config_save_queue();

   return 1;
}

int
mixer_register_module_keybindings(void)
{
   e_managers_keys_ungrab();

   if (!mixer_config->decrease_vol_key.key)
     {
	mixer_config->decrease_vol_key.context = E_BINDING_CONTEXT_ANY;
	mixer_config->decrease_vol_key.key     = evas_stringshare_add("Keycode-174");
	mixer_config->decrease_vol_key.action  = evas_stringshare_add(MIXER_DECREASE_ACTION);
     }
   if (!mixer_config->increase_vol_key.key) 
     {
	mixer_config->increase_vol_key.context = E_BINDING_CONTEXT_ANY;
	mixer_config->increase_vol_key.key     = evas_stringshare_add("Keycode-176");
	mixer_config->increase_vol_key.action  = evas_stringshare_add(MIXER_INCREASE_ACTION);
     }
   if (!mixer_config->mute_key.key) 
     {
	mixer_config->mute_key.context = E_BINDING_CONTEXT_ANY;
	mixer_config->mute_key.key     = evas_stringshare_add("Keycode-160");
	mixer_config->mute_key.action  = evas_stringshare_add(MIXER_MUTE_ACTION);
     }
   _mixer_register_module_keybinding(&(mixer_config->decrease_vol_key), MIXER_DECREASE_ACTION);
   _mixer_register_module_keybinding(&(mixer_config->increase_vol_key), MIXER_INCREASE_ACTION);
   _mixer_register_module_keybinding(&(mixer_config->mute_key),      MIXER_MUTE_ACTION);

   e_managers_keys_grab();

   return 1;
}

int
mixer_unregister_module_keybindings(void)
{
   e_managers_keys_ungrab();

   _mixer_unregister_module_keybinding(&(mixer_config->decrease_vol_key), 1);
   _mixer_unregister_module_keybinding(&(mixer_config->increase_vol_key), 1);
   _mixer_unregister_module_keybinding(&(mixer_config->mute_key),      1);

   e_managers_keys_grab();
   e_config_save_queue();

   return 1;
}

static void
_mixer_unregister_module_keybinding(E_Config_Binding_Key *key, int save)
{
   Evas_List *l;
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

		  e_config->key_bindings = evas_list_remove_list(e_config->key_bindings, l);
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
_mixer_register_module_keybinding(E_Config_Binding_Key *key, const char *action)
{
   int found;
   Evas_List   *l;
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
	e_config->key_bindings = evas_list_append(e_config->key_bindings, eb); 
	
	e_bindings_key_add(key->context, key->key, key->modifiers, key->any_mod,
			   action, key->params);
     }
   else
     {
	if (eb->key)    evas_stringshare_del(eb->key);
	if (eb->action) evas_stringshare_del(eb->action);
	if (eb->params) evas_stringshare_del(eb->params);
	E_FREE(eb);
     }
}


/*************** Private ********************/

ACT_FN_GO(decrease_volume)
{
   Instance *inst;

   inst = evas_list_data(mixer_config->instances);
   if (!inst) return;
   mixer_vol_decrease(inst);
}

ACT_FN_GO(increase_volume)
{
   Instance *inst;

   inst = evas_list_data(mixer_config->instances);
   if (!inst) return;
   mixer_vol_increase(inst);
}

ACT_FN_GO(mute)
{
   Instance *inst;
   
   inst = evas_list_data(mixer_config->instances);
   if (!inst) return;
   mixer_mute_toggle(inst);
}

