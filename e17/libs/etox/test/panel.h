#ifndef __PANEL_H__
#define __PANEL_H__

void e_slide_panel_in(int v, void *data);
void e_slide_panel_out(int v, void *data);
void show_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void hide_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void setup_panel (Evas _e);

Panel_Button *panel_button (Evas _e, char *_label);
Panel_Button *panel_button_new (Evas _e);

#endif /* __PANEL_H__ */
