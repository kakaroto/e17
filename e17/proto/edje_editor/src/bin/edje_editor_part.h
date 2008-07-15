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


Etk_Widget*  part_frame_create   (void);
void         part_frame_update   (void);
char*        part_type_image_get (const char *part);


Etk_Bool _part_EventsCheck_toggled_cb           (Etk_Object *object, void *data);
Etk_Bool _part_EventsRepeatCheck_toggled_cb     (Etk_Object *object, void *data);
Etk_Bool _part_CliptoComboBox_item_activated_cb (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool _part_SourceComboBox_item_activated_cb (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool _part_NameEntry_key_down_cb            (Etk_Object *object, Etk_Event_Key_Down *event, void *data);
Etk_Bool _part_NameEntryImage_clicked_cb        (Etk_Object *object, void *data);



#endif
