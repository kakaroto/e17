#ifndef __LIST_H__
#define __LIST_H__

int                 e_scroll_list(void *data);

void                e_list_item_drag(void *data, Evas * e, Evas_Object * obj,
				     void *event_info);
void                e_list_item_click(void *data, Evas * e, Evas_Object * obj,
				      void *event_info);
void                e_list_item_select(void *data, Evas * e, Evas_Object * obj,
				       void *event_info);

int                 e_list_item_zoom(void *data);

void                e_list_item_in(void *data, Evas * e, Evas_Object * obj,
				   void *event_info);
void                e_list_item_out(void *data, Evas * e, Evas_Object * obj,
				    void *event_info);

int                 e_list_scroll_up_timer(void *data);
int                 e_list_scroll_down_timer(void *data);

void                list_scroll_up(void *data, Evas * e, Evas_Object * obj,
				   void *event_info);
void                list_scroll_up_up(void *data, Evas * e, Evas_Object * obj,
				      void *event_info);

void                list_scroll_down(void *data, Evas * e, Evas_Object * obj,
				     void *event_info);
void                list_scroll_down_up(void *data, Evas * e, Evas_Object * obj,
					void *event_info);

#endif /* __LIST_H__ */
