#ifndef __PANEL_H__
#define __PANEL_H__

extern Evas_Object *o_panel;

int e_slide_panel_in(void *data);
int e_slide_panel_out(void *data);
void show_panel(void *_data, Evas *_e, Evas_Object *_o, void *event_info);
void hide_panel(void *_data, Evas *_e, Evas_Object *_o, void *event_info);
void setup_panel(Evas *_e);

Panel_Button *panel_button(Evas *_e, char *_label, Eina_List *tests);
void panel_button_free(Panel_Button * pbutton);

#endif				/* __PANEL_H__ */
