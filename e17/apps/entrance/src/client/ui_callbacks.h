#ifndef _ENTRANCE_UI_CALLBACKS_H
#define _ENTRANCE_UI_CALLBACKS_H

#include "entrance.h"

void hide_session_list(Entrance_Session e);
void show_session_list(Entrance_Session e);

void update_login_face(Entrance_Session e, char *name);

void session_selector_clicked_cb
    (void *_data, Evas *e, Evas_Object *_o, void *event_info);

void session_selector_mousein_cb
    (void *_Data, Evas *e, Evas_Object *_o, void *event_info);

void session_selector_mouseout_cb
    (void *_Data, Evas *e, Evas_Object *_o, void *event_info);

void session_list_clicked_cb
    (void *_data, Evas *e, Evas_Object *_o, void *event_info);

void session_list_mousein_cb
    (void *_data, Evas *e, Evas_Object *_o, void *event_info);
    
void session_list_mouseout_cb
    (void *_data, Evas *e, Evas_Object *_o, void *event_info);

void entrance_update_time_cb (int val, void *data);
#endif
