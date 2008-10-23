#ifndef ELICIT_THEMES_H
#define ELICIT_THEMES_H

int elicit_themes_init(Elicit *el);
void elicit_themes_shutdown(Elicit *el);

void elicit_theme_load_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_theme_name_show_cb(void *data, Evas_Object *o, const char *emission, const char *source);

Eina_List *elicit_themes_list(void);

void elicit_theme_scroll_cb(void *data, Evas_Object *o, const char *emission, const char *source);

int elicit_themes_scroll_idler(void *data);


#endif
