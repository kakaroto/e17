#include "e_mod_main.h"
#include "e_mod_keybindings.h"

#define E_XKB_ACTION_GROUP D_("Keyboard Layouts")
#define E_XKB_ACTION_NEXT  D_("Switch to the next layout")
#define E_XKB_ACTION_PREV  D_("Switch to the previous layout")

#define ACT_FN_GO(act) \
   static void _e_actions_act_##act##_go(E_Object *obj, const char *params)

#define ACT_GO(name) \
{ \
    act = e_action_add(#name); \
    if (act) act->func.go = _e_actions_act_##name##_go; \
}

ACT_FN_GO(e_xkb_layout_next);
ACT_FN_GO(e_xkb_layout_prev);

static void _e_xkb_register_module_keybinding(
    E_Config_Binding_Key *key, const char *action
);

void _e_xkb_unregister_module_keybinding(
    E_Config_Binding_Key *key, int save
);

int e_xkb_register_module_actions()
{
    E_Action *act;

    e_action_predef_name_set(
        E_XKB_ACTION_GROUP, E_XKB_ACTION_NEXT, E_XKB_NEXT_ACTION,
        NULL, NULL, 0
    );
    e_action_predef_name_set(
        E_XKB_ACTION_GROUP, E_XKB_ACTION_PREV, E_XKB_PREV_ACTION,
        NULL, NULL, 0
    );

    ACT_GO(e_xkb_layout_next);
    ACT_GO(e_xkb_layout_prev);

    return 1;
}

int e_xkb_unregister_module_actions()
{
    e_action_del(E_XKB_NEXT_ACTION);
    e_action_del(E_XKB_PREV_ACTION);

    e_action_predef_name_del(E_XKB_ACTION_GROUP, E_XKB_ACTION_NEXT); 
    e_action_predef_name_del(E_XKB_ACTION_GROUP, E_XKB_ACTION_PREV);
    e_config_save();

    return 1;
}

int e_xkb_register_module_keybindings()
{
    e_managers_keys_ungrab();

    _e_xkb_register_module_keybinding(
        &(e_xkb_cfg->layout_next_key), E_XKB_NEXT_ACTION
    );

    _e_xkb_register_module_keybinding(
        &(e_xkb_cfg->layout_prev_key), E_XKB_PREV_ACTION
    );

    e_managers_keys_grab();

    return 1;
}

int e_xkb_unregister_module_keybindings()
{
    e_managers_keys_ungrab();

    _e_xkb_unregister_module_keybinding(&(e_xkb_cfg->layout_next_key), 1);

    _e_xkb_unregister_module_keybinding(&(e_xkb_cfg->layout_prev_key), 1);

    e_managers_keys_grab();

    return 1;
}

void _e_xkb_unregister_module_keybinding(E_Config_Binding_Key *key, int save)
{
    E_Config_Binding_Key *eb = NULL;
    Eina_List            *l  = NULL;

    Eina_Bool done  = EINA_FALSE;
    Eina_Bool found = EINA_FALSE;

    if (!key) return;

    while (!done)
    {
        done = EINA_TRUE;

        EINA_LIST_FOREACH(e_config->key_bindings, l, eb)
        {
            if (eb && eb->action && eb->action[0] && !strcmp(
                !eb->action ? "" : eb->action, !key->action ? "" : key->action
            ))
            {
                if (save)
                {
                    if (key->key   ) eina_stringshare_del(key->key);
                    if (key->params) eina_stringshare_del(key->params);

                    key->context   = eb->context;
                    key->key       = eina_stringshare_add(eb->key);
                    key->modifiers = eb->modifiers;
                    key->any_mod   = eb->any_mod;
                    key->params    = (
                        !eb->params ? NULL : eina_stringshare_add(eb->params)
                    );
                }

                e_bindings_key_del(
                    eb->context, eb->key, eb->modifiers,
                    eb->any_mod, eb->action, eb->params
                );

                if (eb->key)    eina_stringshare_del(eb->key);
                if (eb->action)  eina_stringshare_del(eb->action);
                if (eb->params) eina_stringshare_del(eb->params);

                E_FREE(eb);

                e_config->key_bindings = eina_list_remove_list(
                    e_config->key_bindings, l
                );

                found = EINA_TRUE;
                done  = EINA_FALSE;

                break;
            }
        }
    }

    if (!found)
    {
        if (key->key)    eina_stringshare_del(key->key);
        if (key->params) eina_stringshare_del(key->params);

        key->key       = NULL;
        key->context   = E_BINDING_CONTEXT_ANY;
        key->modifiers = E_BINDING_MODIFIER_NONE;
        key->any_mod   = 0;
    }
}

static void _e_xkb_register_module_keybinding(
    E_Config_Binding_Key *key, const char *action
)
{
    E_Config_Binding_Key *eb = NULL;
    E_Config_Binding_Key *t  = NULL;
    Eina_List            *l  = NULL;

    Eina_Bool found = EINA_FALSE;

    if (!key || !key->key || !key->key[0] || !action) return;

    eb = E_NEW(E_Config_Binding_Key, 1);

    eb->context   = key->context;
    eb->key       = eina_stringshare_add(key->key);
    eb->modifiers = key->modifiers;
    eb->any_mod   = key->any_mod;
    eb->action    = (!action      ? NULL : eina_stringshare_add(action));
    eb->params    = (!key->params ? NULL : eina_stringshare_add(key->params));

    EINA_LIST_FOREACH(e_config->key_bindings, l, t)
    {
        if (found) break;

        if (
            !strcmp(!t->action ? "" : t->action, eb->action) &&
            !strcmp(!t->params ? "" : t->params, !eb->params ? "" : eb->params)
        ) found = EINA_TRUE;
    }

    if (!found)
    {
        e_config->key_bindings = eina_list_append(e_config->key_bindings, eb);

        e_bindings_key_add(
            key->context, key->key, key->modifiers, key->any_mod,
            action, key->params
        );
    }
    else
    {
        if (eb->key)    eina_stringshare_del(eb->key);
        if (eb->action) eina_stringshare_del(eb->action);
        if (eb->params) eina_stringshare_del(eb->params);

        E_FREE(eb);
    }
}

ACT_FN_GO(e_xkb_layout_next)
{
    e_xkb_layout_next();
    return;
}

ACT_FN_GO(e_xkb_layout_prev)
{
    e_xkb_layout_prev();
    return;
}
