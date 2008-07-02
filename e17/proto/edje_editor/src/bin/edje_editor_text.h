#ifndef _EDJE_EDITOR_TEXT_H_
#define _EDJE_EDITOR_TEXT_H_


/* text frame objects */
Etk_Widget *UI_TextEntry;
Etk_Widget *UI_FontComboBox;
Etk_Widget *UI_FontSizeSpinner;
Etk_Widget *UI_EffectComboBox;
Etk_Widget *UI_FontAddButton;
Etk_Widget *UI_FontAlignVSpinner;
Etk_Widget *UI_FontAlignHSpinner;

Evas_Object *TextColorObject;
Evas_Object *ShadowColorObject;
Evas_Object *OutlineColorObject;


Etk_Widget* text_frame_create        (Evas *evas);
void        text_frame_update        (void);
void        text_font_combo_populate (void);


Etk_Bool _text_FontComboBox_item_activated_cb    (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool _text_EffectComboBox_changed_cb         (Etk_Combobox *combobox, void *data);
Etk_Bool _text_FontSizeSpinner_value_changed_cb  (Etk_Range *range, double value, void *data);
Etk_Bool _text_Entry_text_changed_cb             (Etk_Object *object, void *data);
Etk_Bool _text_FontAlignSpinner_value_changed_cb (Etk_Range *range, double value, void *data);


#endif
