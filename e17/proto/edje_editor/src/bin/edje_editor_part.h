#ifndef _EDJE_EDITOR_PART_H_
#define _EDJE_EDITOR_PART_H_



/* part frame objects */
Etk_Widget *UI_PartNameEntry;
Etk_Widget *UI_PartNameEntryImage;
Etk_Widget *UI_PartTypeComboBox;
Etk_Widget *UI_PartEventsCheck;
Etk_Widget *UI_PartEventsRepeatCheck;
Etk_Widget *UI_CliptoComboBox;
Etk_Widget *UI_PartSourceComboBox;
Etk_Widget *UI_PartSourceLabel;


Etk_Widget *create_part_frame(void);
void UpdatePartFrame(void);

Etk_Bool on_PartEventsCheck_toggled        (Etk_Object *object, void *data);
Etk_Bool on_PartEventsRepeatCheck_toggled  (Etk_Object *object, void *data);
Etk_Bool on_CliptoComboBox_item_activated  (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool on_PartSourceComboBox_item_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool on_PartNameEntry_key_down         (Etk_Object *object, Etk_Event_Key_Down *event, void *data);
Etk_Bool on_PartNameEntryImage_mouse_clicked(Etk_Object *object, void *data);



#endif
