#ifndef _EDJE_EDITOR_INTERFACE_H_
#define _EDJE_EDITOR_INTERFACE_H_

#include "main.h"


Evas *UI_evas;
/* main window objects */
Ecore_Evas *UI_ecore_MainWin;


/* etk_embed objects */
Etk_Widget *UI_PartsTreeEmbed;
Etk_Widget *UI_GroupEmbed;
Etk_Widget *UI_PartEmbed;
Etk_Widget *UI_DescriptionEmbed;
Etk_Widget *UI_RectEmbed;
Etk_Widget *UI_TextEmbed;
Etk_Widget *UI_PositionEmbed;
Etk_Widget *UI_ProgramEmbed;
Etk_Widget *UI_ScriptEmbed;
Etk_Widget *UI_ImageEmbed;
Etk_Widget *UI_ToolbarEmbed;


Evas_Object *edje_ui;


void create_main_window(void);

void _embed_position_set(void *position_data, int *x, int *y);
char* GetPartTypeImage(int part_type);
Etk_Widget* create_a_color_button(char* label_text, int color_button_enum,int w,int h, Evas *evas);


/* Called when the window is destroyed */
//Etk_Bool etk_main_quit_cb      (void *data);
void     ecore_delete_cb       (Ecore_Evas *ee);
void     ecore_resize_callback (Ecore_Evas *ecore_evas);

Etk_Bool on_canvas_geometry_changed        (Etk_Object *canvas, const char *property_name, void *data);
void     on_ColorCanvas_click              (void *data, Evas *e, Evas_Object *obj, void *event_info);
Etk_Bool on_AllButton_click                (Etk_Button *button, void *data);
void     on_Mainwin_key_press              (void *data, Evas *e, Evas_Object *obj, void *event_info);
void     on_Editing_click                  (void *data, Evas *e, Evas_Object *obj, void *event_info);

void signal_cb(void *data, Evas_Object *o, const char *sig, const char *src);

#endif
