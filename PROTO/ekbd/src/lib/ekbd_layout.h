#ifndef EVKB_LAYOUT_H
#define EVKB_LAYOUT_H

void ekbd_layout_update(Smart_Data *sd);
Ekbd_Layout *ekbd_layouts_list_default_get(Smart_Data *sd, Eina_Bool vertical);
void ekbd_layouts_free(Smart_Data *sd);
void ekbd_layout_free(Smart_Data *sd);
void ekbd_layout_select(Smart_Data *sd, Ekbd_Layout *kil);
Evas_Object *ekbd_layout_theme_obj_new(Evas *e, const char *custom_file, const char *group);
void ekbd_layout_keys_calc(Smart_Data *sd);
Ekbd_Layout *ekbd_layout_add(Smart_Data *sdc, const char *path);
void ekbd_layout_aspect_fixed_set(Smart_Data *sd, Eina_Bool fixed);
Eina_Bool ekbd_layout_aspect_fixed_get(const Smart_Data *sd);

#endif /* EVKB_LAYOUT_H */
