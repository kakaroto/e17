#ifndef __EWD_PLUGINS_H__
#define __EWD_PLUGINS_H__

typedef struct _ewd_plugin Ewd_Plugin;
struct _ewd_plugin
{
	int group;
	char *name;
	void *handle;
};

/*
 * Load the specified plugin
 */
Ewd_Plugin *ewd_plugin_load(int group_id, char *plugin);

/*
 * Unload the specified plugin
 */
void ewd_plugin_unload(Ewd_Plugin * plugin);

/*
 * Lookup the specified symbol for the plugin
 */
void *ewd_plugin_call(Ewd_Plugin * plugin, char *symbol_name);

Ewd_List *ewd_plugin_get_available(int group_id);

#endif
