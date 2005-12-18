#ifndef ELICIT_SWATCHES_H
#define ELICIT_SWATCHES_H

int elicit_swatches_init(Elicit *el);
void elicit_swatches_shutdown(Elicit *el);
void elicit_swatches_load(Elicit *el);
void elicit_swatches_save(Elicit *el);

void elicit_swatch_free(Elicit_Swatch *sw);

Elicit_Swatch *elicit_swatch_new(Elicit *el, char *name, int r, int g, int b);
void elicit_swatch_save_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_swatch_load_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_swatch_del_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_swatch_name_show_cb(void *data, Evas_Object *o, const char *emission, const char *source);

void elicit_swatch_scroll_cb(void *data, Evas_Object *o, const char *emission, const char *source);

int elicit_swatches_scroll_idler(void *data);

#endif
