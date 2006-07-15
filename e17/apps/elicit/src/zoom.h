#ifndef ELICIT_ZOOM_H
#define ELICIT_ZOOM_H

Evas_Object *elicit_zoom_add(Evas *evas);
void elicit_zoom_zoom_set(Evas_Object *o, int zoom);
void elicit_zoom_grid_visible_set(Evas_Object *o, int visible);
void elicit_zoom(Evas_Object *o);
void elicit_zoom_data_get(Evas_Object *o, void **data, int *w, int *h);
void elicit_zoom_data_set(Evas_Object *o, void *data, int w, int h);

#endif
