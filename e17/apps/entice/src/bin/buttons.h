#ifndef __BUTTONS_H__
#define __BUTTONS_H__

void bt_tip_start(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_tip_stop (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void bt_close_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_close_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void bt_expand_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_expand_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void bt_full_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_full_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void bt_next_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_next_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void bt_prev_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_prev_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void bt_zoom_in_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_zoom_in_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void bt_zoom_normal_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_zoom_normal_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void bt_zoom_out_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void bt_zoom_out_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void e_slide_buttons_in(int v, void *data);
void e_slide_buttons_out(int v, void *data);

void show_buttons (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

#endif /* __BUTTONS_H__ */
