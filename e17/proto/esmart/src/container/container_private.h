#ifndef CONTAINER_PRIVATE_H
#define CONTAINER_PRIVATE_H

#include "container.h"

Container *_container_fetch(Evas_Object *obj);
Container_Element *_container_element_new(Container *cont, Evas_Object *obj);
void _container_elements_fix(Container *cont);
void _container_elements_changed(Container *cont);
double _container_elements_orig_length_get(Container *cont);
void _cb_container(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_move(void *data, Evas *e, Evas_Object *obj, void *event_info);

void _container_scale_scroll(Container *cont, double old_length);
int is_dir(const char *dir);
Evas_List *_dir_get_files(const char *directory);

void _container_layout_plugin_free(Container_Layout_Plugin *p);
Container_Layout_Plugin *_container_layout_plugin_new(const char *name);

#endif
