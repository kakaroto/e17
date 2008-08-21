/** @file enna_module.c */


#include "enna.h"

static Evas_List *_enna_modules = NULL;
static Ecore_Path_Group *path_group = NULL;

/**
 * @brief Init Module, Save create Ecore_Path_Group and add default module path
 * @return 1 if Initilisation is done correctly, 0 otherwise or if init is called more then twice
 */

EAPI int
enna_module_init(void)
{
  if (!path_group)
    {
      path_group = ecore_path_group_new();
      ecore_path_group_add(path_group, PACKAGE_LIB_DIR"/enna/modules/");
      return 0;
    }

  return -1;
}

/**
 * @brief Free all modules registered and delete Ecore_Path_Group
 * @return 1 if succes 0 otherwise
 */

EAPI int
enna_module_shutdown(void)
{
    Evas_List *l;

    for (l = _enna_modules; l; l = evas_list_remove(l, l->data))
        {
            Enna_Module *m;
            m = l->data;
            if (m->enabled)
                {
                    if (m->func.shutdown)
                        m->func.shutdown(m);
                    m->enabled = 0;
                }
            ecore_plugin_unload(m->plugin);
	    free(m);
        }


    if (path_group)
        {
            ecore_path_group_del(path_group);
            path_group = NULL;
        }

    return 0;
}

EAPI int
enna_module_enable(Enna_Module *m)
{
  if (!m) return -1;
  if(m->enabled) return 0;
  if (m->func.init)
    m->func.init(m);
  m->enabled = 1;
  return 0;
}

EAPI int
enna_module_disable(Enna_Module *m)
{
    if (!m ) return -1;
    if (!m->enabled) return 0;
    if (m->func.shutdown)
        {
            m->func.shutdown(m);
            m->enabled = 0;
            return 0;
        }
    return -1;
}

/**
 * @brief Open a module
 * @param name the module name
 * @return E_Module loaded
 * @note Module music can be loaded like this : enna_module_open("music") this module in loaded from file /usr/lib/enna/modules/music.so
 */
EAPI Enna_Module *
enna_module_open(const char *name, Evas *evas)
{
    char module_name[4096];
    Ecore_Plugin *plugin;
    Enna_Module *m;

    if (!name || !evas) return NULL;
    m = malloc(sizeof(Enna_Module));


    if (!path_group)
        {
            dbg("Error : enna Module should be Init before call this function\n");
            return NULL;
        }

    snprintf(module_name, sizeof(module_name), "enna_module_%s", name);
    dbg("Try to load %s\n", module_name);
    plugin = ecore_plugin_load(path_group, module_name, NULL);

    if (plugin)
        {
            m->api = ecore_plugin_symbol_get(plugin, "module_api");
            if (!m->api || m->api->version != ENNA_MODULE_VERSION )
                {
                    /* FIXME: popup error message */
                    /* Module version doesn't match enna version */
                    dbg("Error : Bad module version, unload %s module\n", m->api->name);
                    ecore_plugin_unload(plugin);
                    return NULL;
                }
            m->func.init = ecore_plugin_symbol_get(plugin, "module_init");
            m->func.shutdown = ecore_plugin_symbol_get(plugin, "module_shutdown");
            m->name = m->api->name;
            dbg("Module \'%s\' loaded succesfully\n", m->api->name);
            m->enabled = 0;
            m->plugin = plugin;
            m->evas = evas;
            _enna_modules = evas_list_append(_enna_modules, m);
            return m;
        }
    else
        dbg("Unable to load module %s\n", name);

    return NULL;
}



