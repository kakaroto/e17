#ifndef _EDJE_EDITOR_FILL_H_
#define _EDJE_EDITOR_FILL_H_


Etk_Widget *UI_FillRelXSpinner;
Etk_Widget *UI_FillRelYSpinner;
Etk_Widget *UI_FillOffsetXSpinner;
Etk_Widget *UI_FillOffsetYSpinner;


Etk_Widget *UI_FillSizeRelXSpinner;
Etk_Widget *UI_FillSizeRelYSpinner;
Etk_Widget *UI_FillSizeOffsetXSpinner;
Etk_Widget *UI_FillSizeOffsetYSpinner;


Etk_Widget* fill_frame_create   (void);
void        fill_frame_update   (void);


Etk_Bool _fill_spinners_value_changed_cb(Etk_Range *range, double value, void *data);
Etk_Bool _fill_size_spinners_value_changed_cb(Etk_Range *range, double value, void *data);

#endif
