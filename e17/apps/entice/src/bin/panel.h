#ifndef __PANEL_H__
#define __PANEL_H__

void                e_slide_panel_in(int v, void *data);
void                e_slide_panel_out(int v, void *data);

void                show_panel(void *data, Evas * e, Evas_Object * obj,
			       void *event_info);
void                hide_panel(void *data, Evas * e, Evas_Object * obj,
			       void *event_info);

#endif /* __PANEL_H__ */
