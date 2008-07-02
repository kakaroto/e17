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


Etk_Widget* create_text_frame(Evas *evas);
void UpdateTextFrame(void);
void PopulateFontsComboBox(void);



Etk_Bool on_FontComboBox_item_activated    (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool on_EffectComboBox_changed         (Etk_Combobox *combobox, void *data);
Etk_Bool on_FontSizeSpinner_value_changed  (Etk_Range *range, double value, void *data);
Etk_Bool on_TextEntry_text_changed         (Etk_Object *object, void *data);
Etk_Bool on_FontAlignSpinner_value_changed (Etk_Range *range, double value, void *data);


#endif
