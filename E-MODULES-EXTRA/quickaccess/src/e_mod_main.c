#include "e_mod_main.h"

EINTERN int _e_quick_access_log_dom = -1;
static E_Action *_e_quick_access_toggle = NULL;
static const char _e_quick_access_Name[] = "Quick Access";
static const char _act_toggle[] = "quick_access_toggle";
static const char _lbl_toggle[] = "Toggle Visibility";

static Eina_List *_e_quick_access_entries = NULL;
static Eina_List *_e_quick_access_border_hooks = NULL;
static Eina_List *_e_quick_access_event_handlers = NULL;

/**
 * in priority order:
 *
 * @todo config (see e_mod_config.c)
 *
 * @todo custom border based on E_Quick_Access_Entry_Mode/E_Gadcon_Orient
 *
 * @todo show/hide effects:
 *        - fullscreen
 *        - centered
 *        - slide from top, bottom, left or right
 *
 * @todo match more than one, doing tabs (my idea is to do another
 *       tabbing module first, experiment with that, maybe use/reuse
 *       it here)
 */

#undef DBG
#define DBG(...) ERR(__VA_ARGS__)

static void
_e_quick_access_entry_free(E_Quick_Access_Entry *entry)
{
   if (entry->border) e_border_act_close_begin(entry->border);
   eina_stringshare_del(entry->id);
   eina_stringshare_del(entry->name);
   eina_stringshare_del(entry->class);
   eina_stringshare_del(entry->cmd);
   E_FREE(entry);
}

/* note: id must be stringshared! */
static E_Quick_Access_Entry *
_e_quick_access_entry_find(const char *id)
{
   E_Quick_Access_Entry *entry;
   const Eina_List *n;
   EINA_LIST_FOREACH(_e_quick_access_entries, n, entry)
     if (entry->id == id)
       return entry;
   return NULL;
}

static E_Quick_Access_Entry *
_e_quick_access_entry_find_exe(const Ecore_Exe *exe)
{
   E_Quick_Access_Entry *entry;
   const Eina_List *n;
   EINA_LIST_FOREACH(_e_quick_access_entries, n, entry)
     if (entry->exe == exe)
       return entry;
   return NULL;
}

static E_Quick_Access_Entry *
_e_quick_access_entry_find_border(const E_Border *bd)
{
   E_Quick_Access_Entry *entry;
   const Eina_List *n;
   EINA_LIST_FOREACH(_e_quick_access_entries, n, entry)
     if (entry->border == bd)
       return entry;
   return NULL;
}

static E_Quick_Access_Entry *
_e_quick_access_entry_find_match_stringshared(const char *name, const char *class)
{
   E_Quick_Access_Entry *entry;
   const Eina_List *n;
   EINA_LIST_FOREACH(_e_quick_access_entries, n, entry)
     {
	if (entry->class != class) continue;
	/* no entry name matches all */
	if ((entry->name) && (entry->name != name)) continue;

	return entry;
     }

   return NULL;
}

static E_Quick_Access_Entry *
_e_quick_access_entry_find_match(const E_Border *bd)
{
   /* TODO: why bd->client.icccm.{name,class} are not stringshared? */
   const char *name = eina_stringshare_add(bd->client.icccm.name);
   const char *class = eina_stringshare_add(bd->client.icccm.class);
   E_Quick_Access_Entry *entry;

   entry = _e_quick_access_entry_find_match_stringshared(name, class);

   eina_stringshare_del(class);
   eina_stringshare_del(name);

   return entry;
}

static E_Border *
_e_quick_access_entry_border_get(const E_Quick_Access_Entry *entry)
{
   return entry->border;
}

static void
_e_quick_access_entry_border_associate(E_Quick_Access_Entry *entry, E_Border *bd)
{
   if (entry->exe) entry->exe = NULL; /* not waiting anymore */

   entry->border = bd;

   bd->lock_user_location = 1;
   bd->lock_client_location = 1;
   bd->lock_user_size = 1;
   bd->lock_client_size = 1;
   bd->lock_user_stacking = 1;
   bd->lock_client_stacking = 1;
   bd->lock_user_iconify = 1;
   bd->lock_client_iconify = 1;
   bd->lock_user_desk = 1;
   bd->lock_client_desk = 1;
   bd->lock_user_sticky = 1;
   bd->lock_client_sticky = 1;
   bd->lock_user_shade = 1;
   bd->lock_client_shade = 1;
   bd->lock_user_maximize = 1;
   bd->lock_client_maximize = 1;
   bd->lock_user_fullscreen = 1;
   bd->lock_client_fullscreen = 1;

   bd->user_skip_winlist = 1;
   bd->client.netwm.state.skip_taskbar = 1;
   bd->client.netwm.state.skip_pager = 1;

   bd->client.e.state.centered = 1;

   bd->sticky = 1;
   bd->want_focus = 1;
}

static void
_e_quick_access_border_activate(E_Border *bd)
{
   e_border_raise(bd);
   e_border_show(bd);
   e_border_focus_set(bd, 1, 1);
}

static void
_e_quick_access_border_deactivate(E_Border *bd)
{
   e_border_hide(bd, 1);
}

static void
_e_quick_access_border_new(E_Quick_Access_Entry *entry)
{
   E_Exec_Instance *ei;

   if (entry->exe)
     {
	INF("already waiting '%s' to start for '%s' (name=%s, class=%s), "
	    "run request ignored.",
	    entry->cmd, entry->id, entry->name, entry->class);
	return;
     }

   INF("start quick access '%s' (name=%s, class=%s), "
       "run command '%s'",
       entry->id, entry->name, entry->class, entry->cmd);

   ei = e_exec(NULL, NULL, entry->cmd, NULL, NULL);
   if ((!ei) || (!ei->exe))
     {
	ERR("could not execute '%s'", entry->cmd);
	return;
     }

   entry->exe = ei->exe;
}

static void
_e_quick_access_toggle_cb(E_Object *obj __UNUSED__, const char *params)
{
   E_Quick_Access_Entry *entry;
   E_Border *bd;

   if (!params)
     {
	ERR("%s got params == NULL", _act_toggle);
	return;
     }
   /* params is stringshared according to e_bindings.c */
   DBG("%s %s (stringshared=%p)", _act_toggle, params, params);

   entry = _e_quick_access_entry_find(params);
   if (!entry)
     {
	ERR("unknown quick access identifier: '%s'", params);
	return;
     }

   bd = _e_quick_access_entry_border_get(entry);
   if (bd)
     {
	if (bd->focused)
	  {
	     _e_quick_access_border_deactivate(bd);
	     return;
	  }

	DBG("activate border for identifier '%s' (name=%s, class=%s).",
	    entry->id, entry->name, entry->class);
	_e_quick_access_border_activate(bd);
     }
   else
     {
	DBG("no border known for identifier '%s' (name=%s, class=%s).",
	    entry->id, entry->name, entry->class);
	_e_quick_access_border_new(entry);
     }
}

static void
_e_quick_access_border_eval_pre_post_fetch_cb(void *data __UNUSED__, void *border)
{
   E_Border *bd = border;
   E_Quick_Access_Entry *entry;

   if ((!bd->new_client) || (bd->internal) || (bd->remember)) return;
   if ((!bd->client.icccm.class) || (bd->client.icccm.class[0] == '\0')) return;
   if ((!bd->client.icccm.name) || (bd->client.icccm.name[0] == '\0')) return;

   entry = _e_quick_access_entry_find_match(bd);
   if (!entry) return;
   DBG("border=%p matches entry %s", bd, entry->id);
   _e_quick_access_entry_border_associate(entry, bd);
}

static void
_e_quick_access_border_eval_post_border_assign_cb(void *data __UNUSED__, void *border)
{
   E_Border *bd = border;
   if (!_e_quick_access_entry_find_border(bd)) return;
   _e_quick_access_border_activate(bd);
}

static int
_e_quick_access_event_border_focus_out_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   E_Event_Border_Focus_Out *ev = event;

   if (!_e_quick_access_entry_find_border(ev->border)) return 1;
   _e_quick_access_border_deactivate(ev->border);
   return 1;
}

static int
_e_quick_access_event_border_remove_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   E_Event_Border_Remove *ev = event;
   E_Quick_Access_Entry *entry = _e_quick_access_entry_find_border(ev->border);

   if (entry)
     entry->border = NULL;

   return 1;
}

static int
_e_quick_access_event_exe_del_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Exe_Event_Del *ev = event;
   E_Quick_Access_Entry *entry = _e_quick_access_entry_find_exe(ev->exe);

   if (entry)
     entry->exe = NULL; /* not waiting/running anymore */

   return 1;
}

EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Quick Access"};

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[PATH_MAX];
   E_Border_Hook *h;
   Ecore_Event_Handler *eh;

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   _e_quick_access_log_dom = eina_log_domain_register
     ("quick_access", EINA_COLOR_ORANGE);
   if (_e_quick_access_log_dom < 0)
     {
	EINA_LOG_CRIT("could not register logging domain quick_access");
	return NULL;
     }

   _e_quick_access_toggle = e_action_add(_act_toggle);
   if (!_e_quick_access_toggle)
     {
	CRIT("could not register %s E_Action", _act_toggle);
	goto failed_action;
     }

   _e_quick_access_toggle->func.go = _e_quick_access_toggle_cb;
   e_action_predef_name_set
     (_(_e_quick_access_Name), _(_lbl_toggle),
      _act_toggle, NULL, _("quick access name/identifier"), 1);

#define CB(id, func)							\
   h = e_border_hook_add						\
     (E_BORDER_HOOK_##id,						\
      _e_quick_access_border_##func##_cb, NULL);			\
   if (!h)								\
     {									\
	CRIT("could not add E_BORDER_HOOK_"#id" border hook.");		\
	goto failed_hook;						\
     }									\
   _e_quick_access_border_hooks = eina_list_append			\
     (_e_quick_access_border_hooks, h)

   CB(EVAL_PRE_POST_FETCH, eval_pre_post_fetch);
   CB(EVAL_POST_BORDER_ASSIGN, eval_post_border_assign);
#undef CB

#define CB(id, func)							\
   eh = ecore_event_handler_add						\
     (id, _e_quick_access_event_##func##_cb, NULL);			\
   if (!eh)								\
     {									\
	CRIT("could not add "#id" event handler.");			\
	goto failed_event_handler;					\
     }									\
   _e_quick_access_event_handlers = eina_list_append			\
     (_e_quick_access_event_handlers, eh)

   CB(E_EVENT_BORDER_FOCUS_OUT, border_focus_out);
   CB(E_EVENT_BORDER_REMOVE, border_remove);
   CB(ECORE_EXE_EVENT_DEL, exe_del);
#undef CB

   INF("loaded quick_access module, registered %s action.", _act_toggle);

   // TODO: on first usage (ie: no config), show instructions that user
   // should set a match and keybinding

   // TODO: move below to config file loading
   E_Quick_Access_Entry *entry = E_NEW(E_Quick_Access_Entry, 1);
   entry->id = eina_stringshare_add("urxvt");
   entry->name = eina_stringshare_add("e-urxvt");
   entry->class = eina_stringshare_add("URxvt");
   entry->cmd = eina_stringshare_add("urxvt -name e-urxvt");
   _e_quick_access_entries = eina_list_append(_e_quick_access_entries, entry);
   // TODO: end

   return m;

 failed_event_handler:
   EINA_LIST_FREE(_e_quick_access_event_handlers, eh)
     ecore_event_handler_del(eh);

 failed_hook:
   EINA_LIST_FREE(_e_quick_access_border_hooks, h)
     e_border_hook_del(h);

 failed_action:
   eina_log_domain_unregister(_e_quick_access_log_dom);
   _e_quick_access_log_dom = -1;
   return NULL;
}

EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
   E_Quick_Access_Entry *entry;
   Ecore_Event_Handler *eh;
   E_Border_Hook *h;

   EINA_LIST_FREE(_e_quick_access_entries, entry)
     _e_quick_access_entry_free(entry);

   EINA_LIST_FREE(_e_quick_access_event_handlers, eh)
     ecore_event_handler_del(eh);

   EINA_LIST_FREE(_e_quick_access_border_hooks, h)
     e_border_hook_del(h);

   if (_e_quick_access_toggle)
     {
	e_action_predef_name_del(_(_e_quick_access_Name), _(_lbl_toggle));

	e_action_del(_act_toggle);
	_e_quick_access_toggle = NULL;
     }

   INF("unloaded quick_access module, unregistered %s action.", _act_toggle);
   eina_log_domain_unregister(_e_quick_access_log_dom);
   _e_quick_access_log_dom = -1;
   return 1;
}

EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
   // TODO: save entries in config file
   return 1;
}
