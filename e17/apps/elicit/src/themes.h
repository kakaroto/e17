#ifndef ELICIT_THEMES_H
#define ELICIT_THEMES_H

void elicit_theme_load_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_theme_name_show_cb(void *data, Evas_Object *o, const char *emission, const char *source);
int elicit_themes_init(Elicit *el);

#endif
