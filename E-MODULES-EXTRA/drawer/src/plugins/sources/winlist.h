#ifndef WINLIST_H
#define WINLIST_H

#include "../../Drawer.h"

EAPI extern Drawer_Plugin_Api drawer_plugin_api;

EAPI void *drawer_plugin_init(Drawer_Plugin *p, const char *id);
EAPI int   drawer_plugin_shutdown(Drawer_Plugin *p);

EAPI Eina_List *drawer_source_list(Drawer_Source *s);
EAPI void  drawer_source_activate(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone);

EAPI void drawer_plugin_config_save(Drawer_Plugin *p);
EAPI void drawer_source_trigger(Drawer_Source *s, E_Zone *zone);
EAPI const char * drawer_source_description_get(Drawer_Source *s);
EAPI Evas_Object * drawer_source_render_item(Drawer_Source *s, Drawer_Source_Item *si, Evas *evas);

#endif
