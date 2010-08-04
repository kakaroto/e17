#ifndef GRID_H
#define GRID_H

#include "../../Drawer.h"

EAPI extern Drawer_Plugin_Api drawer_plugin_api;

EAPI void *drawer_plugin_init(Drawer_Plugin *p, const char *id);
EAPI int   drawer_plugin_shutdown(Drawer_Plugin *p);
EAPI Evas_Object * drawer_view_render(Drawer_View *v, Evas *evas, Eina_List *items);

EAPI void  drawer_view_container_resized(Drawer_View *v);
EAPI void  drawer_view_orient_set(Drawer_View *v, E_Gadcon_Orient orient);

#endif
