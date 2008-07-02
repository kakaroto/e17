#ifndef _EDJE_EDITOR_IMAGES_H_
#define _EDJE_EDITOR_IMAGES_H_

/* image frame objects */
Etk_Widget *UI_ImageTweenList;
Etk_Widget *UI_ImageNameLabel;
Etk_Widget *UI_ImageTweenVBox;
Etk_Widget *UI_ImageAlphaSlider;
Etk_Widget *UI_AddTweenButton;
Etk_Widget *UI_DeleteTweenButton;
Etk_Widget *UI_MoveDownTweenButton;
Etk_Widget *UI_MoveUpTweenButton;
Etk_Widget *UI_BorderTopSpinner;
Etk_Widget *UI_BorderLeftSpinner;
Etk_Widget *UI_BorderBottomSpinner;
Etk_Widget *UI_BorderRightSpinner;
Etk_Widget *UI_ImageAddButton;

/* image browser objects */
Etk_Widget *UI_ImageBrowserIconbox;
Etk_Widget *UI_ImageBrowserWin;
Etk_Widget *UI_ImageBrowserSizeLabel;


Etk_Widget* create_image_frame(void);
void PopulateTweenList(void);
void UpdateImageFrame(void);
void ShowImageBrowser(int UpdateCurrent);

Etk_Bool on_ImageAlphaSlider_value_changed (Etk_Object *object, double value, void *data);
Etk_Bool on_ImageComboBox_item_activated   (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool on_ImageTweenList_row_selected    (Etk_Object *object, Etk_Tree_Row *row, void *data);
Etk_Bool on_BorderSpinner_value_changed    (Etk_Range *range, double value, void *data);

Etk_Widget* create_image_browser(void);
void PopulateImageBrowser(void);
void UpdateImageBrowser(void);
Etk_Bool on_ImageBrowserIconbox_selected(Etk_Iconbox *iconbox, Etk_Iconbox_Icon *icon, void *data);



#endif
