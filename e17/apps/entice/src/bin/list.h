#ifndef __LIST_H__
#define __LIST_H__

void e_scroll_list(int v, void *data);

void e_list_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void e_list_item_zoom(int v, void *data);

void e_list_item_in(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void e_list_item_out(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void e_list_scroll_up_timer(int v, void *data);
void e_list_scroll_down_timer(int v, void *data);

void list_scroll_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void list_scroll_up_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void list_scroll_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void list_scroll_down_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

#endif /* __LIST_H__ */
