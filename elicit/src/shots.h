#ifndef ELICIT_SHOTS_H
#define ELICIT_SHOTS_H

int elicit_shots_init(Elicit *el);
void elicit_shots_shutdown(Elicit *el);
void elicit_shots_save(Elicit *el);
void elicit_shots_load(Elicit *el);

void elicit_shot_free(Elicit_Shot *sw);

void elicit_shot_save_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_shot_load_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_shot_del_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void elicit_shot_name_show_cb(void *data, Evas_Object *o, const char *emission, const char *source);

void elicit_shot_scroll_cb(void *data, Evas_Object *o, const char *emission, const char *source);

int elicit_shots_scroll_idler(void *data);

#endif
