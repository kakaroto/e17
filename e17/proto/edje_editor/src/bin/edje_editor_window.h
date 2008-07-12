#ifndef _EDJE_EDITOR_INTERFACE_H_
#define _EDJE_EDITOR_INTERFACE_H_


/* main windows objects */
Evas        *UI_evas;
Ecore_Evas  *UI_ecore_MainWin;
Evas_Object *edje_ui;


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
Etk_Widget *UI_GradientEmbed;
Etk_Widget *UI_FillEmbed;
Etk_Widget *UI_ToolbarEmbed;


void        window_main_create         (void);
Etk_Widget* window_color_button_create (char* label_text, int color_button_enum,int w,int h, Evas *evas);
void        window_embed_position_set  (void *position_data, int *x, int *y);

void     _window_delete_cb           (Ecore_Evas *ee);
void     _window_resize_cb           (Ecore_Evas *ecore_evas);

void     _window_edit_obj_signal_cb  (void *data, Evas_Object *o, const char *sig, const char *src);
void     _window_edit_obj_click      (void *data, Evas *e, Evas_Object *obj, void *event_info);

Etk_Bool _window_all_button_click_cb (Etk_Button *button, void *data);
void     _window_logo_key_press      (void *data, Evas *e, Evas_Object *obj, void *event_info);
void     _window_color_canvas_click  (void *data, Evas *e, Evas_Object *obj, void *event_info);


#endif
