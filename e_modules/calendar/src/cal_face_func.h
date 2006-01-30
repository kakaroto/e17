#ifdef E_TYPEDEFS
#else
#ifndef CAL_FACE_FUNC_H
#define CAL_FACE_FUNC_H
#include "e_mod_main.h"
void calendar_face_prev_year(void *data, Evas *e,
                             Evas_Object *obj, void *event_info);
void calendar_face_next_year(void *data, Evas *e,
                             Evas_Object *obj, void *event_info);
void calendar_face_next_month(void *data, Evas *e,
                              Evas_Object *obj, void *event_info);
void calendar_face_prev_month(void *data, Evas *e,
                              Evas_Object *obj, void *event_info);
void calendar_face_free(Calendar_Face * face);
void calendar_face_menu_new(Calendar_Face * face, Calendar * calendar);
int calendar_face_config_cb_timer(void *data);
void calendar_face_cb_menu_edit(void *data, E_Menu *m, E_Menu_Item *mi);
void calendar_face_start(Calendar * calendar);
void calendar_face_cb_mouse_down(void *data, Evas *e,
                                 Evas_Object *obj, void *event_info);
void calendar_face_cb_gmc_change(void *data,
                                 E_Gadman_Client *gmc, E_Gadman_Change change);
void calendar_face_menu_day(day_face * face, Calendar * calendar);
Calendar_Face *calendar_face_new(E_Container *con, Calendar * calendar);
#endif
#endif
