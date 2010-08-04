#ifndef LIST_H
#define LIST_H

#include "../../Drawer.h"

EAPI extern Drawer_Plugin_Api drawer_plugin_api;

EAPI void *drawer_plugin_init(Drawer_Plugin *p, const char *id);
EAPI int   drawer_plugin_shutdown(Drawer_Plugin *p);
EAPI Evas_Object * drawer_view_render(Drawer_View *v, Evas *evas, Eina_List *items);

EAPI void drawer_view_orient_set(Drawer_View *v, E_Gadcon_Orient orient);

EAPI Evas_Object * drawer_plugin_config_get(Drawer_Plugin *p, Evas *evas);
EAPI void drawer_plugin_config_save(Drawer_Plugin *p);

#endif
