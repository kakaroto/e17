#ifndef __PANEL_H__
#define __PANEL_H__

int		   e_slide_panel(void *data);
int                e_slide_panel_in(void *data);
int                e_slide_panel_out(void *data);

void                show_panel(void *data, Evas * e, Evas_Object * obj,
			       void *event_info);
void                hide_panel(void *data, Evas * e, Evas_Object * obj,
			       void *event_info);

#endif /* __PANEL_H__ */
