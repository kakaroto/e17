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


Etk_Widget* image_frame_create       (void);
void        image_frame_update       (void);
void        image_tweenlist_populate (void);

Etk_Widget* image_browser_create     (void);
void        image_browser_populate   (void);
void        image_browser_show       (int UpdateCurrent);
void        image_browser_update     (void);


Etk_Bool _image_AlphaSlider_value_changed_cb     (Etk_Object *object, double value, void *data);
Etk_Bool _image_TweenList_row_selected_cb        (Etk_Object *object, Etk_Tree_Row *row, void *data);
Etk_Bool _image_border_spinners_value_changed_cb (Etk_Range *range, double value, void *data);
Etk_Bool _image_browser_iconbox_selected_cb      (Etk_Iconbox *iconbox, Etk_Iconbox_Icon *icon, void *data);


#endif
