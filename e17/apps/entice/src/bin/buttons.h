#ifndef __BUTTONS_H__
#define __BUTTONS_H__

void                bt_tip_start(void *data, Evas * e, Evas_Object * obj,
				 void *event_info);
void                bt_tip_stop(void *data, Evas * e, Evas_Object * obj,
				void *event_info);

void                bt_close_down(void *data, Evas * e, Evas_Object * obj,
				  void *event_info);
void                bt_close_up(void *data, Evas * e, Evas_Object * obj,
				void *event_info);

void                bt_delete_down(void *data, Evas * e, Evas_Object * obj,
				  void *event_info);
void                bt_delete_up(void *data, Evas * e, Evas_Object * obj,
				void *event_info);

void                bt_expand_down(void *data, Evas * e, Evas_Object * obj,
				   void *event_info);
void                bt_expand_up(void *data, Evas * e, Evas_Object * obj,
				 void *event_info);

void                bt_full_down(void *data, Evas * e, Evas_Object * obj,
				 void *event_info);
void                bt_full_up(void *data, Evas * e, Evas_Object * obj,
			       void *event_info);

void                bt_next_down(void *data, Evas * e, Evas_Object * obj,
				 void *event_info);
void                bt_next_up(void *data, Evas * e, Evas_Object * obj,
			       void *event_info);

void                bt_prev_down(void *data, Evas * e, Evas_Object * obj,
				 void *event_info);
void                bt_prev_up(void *data, Evas * e, Evas_Object * obj,
			       void *event_info);

void                bt_zoom_in_down(void *data, Evas * e, Evas_Object * obj,
				    void *event_info);
void                bt_zoom_in_up(void *data, Evas * e, Evas_Object * obj,
				  void *event_info);

void                bt_zoom_normal_down(void *data, Evas * e, Evas_Object * obj,
					void *event_info);
void                bt_zoom_normal_up(void *data, Evas * e, Evas_Object * obj,
				      void *event_info);

void                bt_zoom_out_down(void *data, Evas * e, Evas_Object * obj,
				     void *event_info);
void                bt_zoom_out_up(void *data, Evas * e, Evas_Object * obj,
				   void *event_info);

void                e_slide_buttons_in(int v, void *data);
void                e_slide_buttons_out(int v, void *data);

void                show_buttons(void *data, Evas * e, Evas_Object * obj,
				 void *event_info);

#endif /* __BUTTONS_H__ */
