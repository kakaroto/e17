/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "../../e_mod_main.h"

EAPI extern Drawer_Plugin_Api drawer_plugin_api;

EAPI void *drawer_plugin_init(Drawer_Plugin *p, const char *id);
EAPI int   drawer_plugin_shutdown(Drawer_Plugin *p);

EAPI Eina_List *drawer_source_list(Drawer_Source *s, Evas *evas __UNUSED__);
EAPI void  drawer_source_activate(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone);
EAPI void  drawer_source_context(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone, Drawer_Event_View_Context *ev);

EAPI Evas_Object * drawer_plugin_config_get(Drawer_Plugin *p, Evas *evas);
EAPI void drawer_plugin_config_save(Drawer_Plugin *p);
EAPI const char * drawer_source_description_get(Drawer_Source *s);

#endif
