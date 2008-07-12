#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/***   Implementation   ***/
Etk_Widget*
image_frame_create(void)
{
   Etk_Widget *label;
   Etk_Widget *table;
   Etk_Tree_Col *col1;

   //table
   table = etk_table_new(5, 4, ETK_TABLE_NOT_HOMOGENEOUS);

   //AddImageButton
   UI_ImageAddButton = etk_button_new_from_stock(ETK_STOCK_IMAGE_X_GENERIC);
   etk_object_properties_set(ETK_OBJECT(UI_ImageAddButton), "label", "Choose Image", NULL);
   etk_table_attach_default(ETK_TABLE(table), UI_ImageAddButton, 0, 1, 0, 0);
   
   //ImageName
   UI_ImageNameLabel = etk_label_new("none");
   etk_object_properties_set(ETK_OBJECT(UI_ImageNameLabel), "xalign", 0.5, NULL);
   etk_table_attach_default(ETK_TABLE(table), UI_ImageNameLabel, 2, 4, 0, 0);

   //ImageTweenVBox
   UI_ImageTweenVBox = etk_vbox_new(ETK_TRUE, 0);
   etk_table_attach_default(ETK_TABLE(table), UI_ImageTweenVBox, 0, 0, 1, 1);
   
   //AddTweenButton
   UI_AddTweenButton = etk_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_button_style_set(ETK_BUTTON(UI_AddTweenButton), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(UI_AddTweenButton),
      ETK_CALLBACK(_window_all_button_click_cb), (void*)IMAGE_TWEEN_ADD);
   etk_box_append(ETK_BOX(UI_ImageTweenVBox), UI_AddTweenButton,
                     ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   //DeleteTweenButton
   UI_DeleteTweenButton = etk_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_button_style_set(ETK_BUTTON(UI_DeleteTweenButton), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(UI_DeleteTweenButton),
      ETK_CALLBACK(_window_all_button_click_cb), (void*)IMAGE_TWEEN_DELETE);
   etk_box_append(ETK_BOX(UI_ImageTweenVBox), UI_DeleteTweenButton,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   //MoveUpTweenButton
   UI_MoveUpTweenButton = etk_button_new_from_stock(ETK_STOCK_GO_UP);
   etk_button_style_set(ETK_BUTTON(UI_MoveUpTweenButton),  ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(UI_MoveUpTweenButton),
      ETK_CALLBACK(_window_all_button_click_cb), (void*)IMAGE_TWEEN_UP);
   etk_box_append(ETK_BOX(UI_ImageTweenVBox), UI_MoveUpTweenButton, 
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   //MoveDownTweenButton
   UI_MoveDownTweenButton = etk_button_new_from_stock(ETK_STOCK_GO_DOWN);
   etk_button_style_set(ETK_BUTTON(UI_MoveDownTweenButton),  ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(UI_MoveDownTweenButton),
      ETK_CALLBACK(_window_all_button_click_cb), (void*)IMAGE_TWEEN_DOWN);
   etk_box_append(ETK_BOX(UI_ImageTweenVBox), UI_MoveDownTweenButton,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //ImageTweenList
   UI_ImageTweenList = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(UI_ImageTweenList), ETK_TREE_MODE_LIST);
   etk_tree_headers_visible_set(ETK_TREE(UI_ImageTweenList), FALSE);
  // etk_tree_multiple_select_set(ETK_TREE(UI_ImageTweenList), ETK_TRUE);
   col1 = etk_tree_col_new(ETK_TREE(UI_ImageTweenList), "Tween", 130, 0.0);
   etk_tree_col_model_add(col1, etk_tree_model_image_new());
   etk_tree_col_model_add(col1, etk_tree_model_text_new());
   etk_tree_build(ETK_TREE(UI_ImageTweenList));
   etk_table_attach_default(ETK_TABLE(table),UI_ImageTweenList, 1, 4, 1, 1);

   label = etk_label_new("Alpha");
   etk_table_attach_default(ETK_TABLE(table),label, 0, 0, 2, 2);

   //ImageAlphaSlider
   UI_ImageAlphaSlider = etk_hslider_new(0, 255, 15, 1,20);
   etk_table_attach_default(ETK_TABLE(table),UI_ImageAlphaSlider, 1, 4, 2, 2);

   label = etk_label_new("Left");
   etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default(ETK_TABLE(table),label, 1, 1, 3, 3);

   label = etk_label_new("Right");
   etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default(ETK_TABLE(table),label, 2, 2, 3, 3);

   label = etk_label_new("Top");
   etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default(ETK_TABLE(table),label, 3, 3, 3, 3);

   label = etk_label_new("Bottom");
   etk_object_properties_set(ETK_OBJECT(label), "xalign",0.5,NULL);
   etk_table_attach_default(ETK_TABLE(table),label, 4, 4, 3, 3);

   label = etk_label_new("Border");
   //etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_table_attach_default(ETK_TABLE(table),label, 0, 0, 4, 4);

   //UI_BorderLeftSpinner
   UI_BorderLeftSpinner = etk_spinner_new(0, 500, 0, 1, 10);
   etk_widget_size_request_set(UI_BorderLeftSpinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_BorderLeftSpinner, 1, 1, 4, 4);

   //UI_BorderRightSpinner
   UI_BorderRightSpinner = etk_spinner_new(0, 500, 0, 1, 10);
   etk_widget_size_request_set(UI_BorderRightSpinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_BorderRightSpinner, 2, 2, 4, 4);

   //UI_BorderTopSpinner
   UI_BorderTopSpinner = etk_spinner_new(0, 500, 0, 1, 10);
   etk_widget_size_request_set(UI_BorderTopSpinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_BorderTopSpinner, 3, 3, 4, 4);

   //UI_BorderBottomSpinner
   UI_BorderBottomSpinner = etk_spinner_new(0, 500, 0, 1, 10);
   etk_widget_size_request_set(UI_BorderBottomSpinner,45, 20);
   etk_table_attach_default(ETK_TABLE(table),UI_BorderBottomSpinner, 4, 4, 4, 4);

   etk_signal_connect("row-selected", ETK_OBJECT(UI_ImageTweenList),
            ETK_CALLBACK(_image_TweenList_row_selected_cb), NULL);
   etk_signal_connect("clicked", ETK_OBJECT(UI_ImageAddButton),
            ETK_CALLBACK(_window_all_button_click_cb), (void*)IMAGE_BROWSER_SHOW);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_ImageAlphaSlider),
            ETK_CALLBACK(_image_AlphaSlider_value_changed_cb), NULL);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_BorderLeftSpinner),
                      ETK_CALLBACK(_image_border_spinners_value_changed_cb),
                      (void *)BORDER_LEFT);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_BorderRightSpinner),
                      ETK_CALLBACK(_image_border_spinners_value_changed_cb),
                      (void *)BORDER_RIGHT);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_BorderTopSpinner),
                      ETK_CALLBACK(_image_border_spinners_value_changed_cb),
                      (void *)BORDER_TOP);
   etk_signal_connect("value-changed", ETK_OBJECT(UI_BorderBottomSpinner),
                      ETK_CALLBACK(_image_border_spinners_value_changed_cb),
                      (void *)BORDER_BOTTOM);

   return table;
}

void
image_frame_update(void)
{
   //Etk_Combobox_Item *item = NULL;
   const char *im;
    
   //Stop signal propagation
   etk_signal_block("value-changed",ETK_OBJECT(UI_BorderLeftSpinner),
                    ETK_CALLBACK(_image_border_spinners_value_changed_cb),
                    (void*)BORDER_LEFT);
   etk_signal_block("value-changed",ETK_OBJECT(UI_BorderRightSpinner),
                    ETK_CALLBACK(_image_border_spinners_value_changed_cb),
                    (void*)BORDER_RIGHT);
   etk_signal_block("value-changed",ETK_OBJECT(UI_BorderTopSpinner),
                    ETK_CALLBACK(_image_border_spinners_value_changed_cb),
                    (void*)BORDER_TOP);
   etk_signal_block("value-changed",ETK_OBJECT(UI_BorderBottomSpinner),
                    ETK_CALLBACK(_image_border_spinners_value_changed_cb),
                    (void*)BORDER_BOTTOM);
   etk_signal_block("value-changed",ETK_OBJECT(UI_ImageAlphaSlider),
                    ETK_CALLBACK(_image_AlphaSlider_value_changed_cb), NULL);
   etk_signal_block("icon-selected", ETK_OBJECT(UI_ImageBrowserIconbox),
                    ETK_CALLBACK(_image_browser_iconbox_selected_cb), NULL);
   
   if (!etk_string_length_get(Cur.state)) return;
   if (!etk_string_length_get(Cur.part)) return;
   
   image_tweenlist_populate();
   etk_widget_disabled_set(UI_DeleteTweenButton, TRUE);
   etk_widget_disabled_set(UI_MoveDownTweenButton, TRUE);
   etk_widget_disabled_set(UI_MoveUpTweenButton, TRUE);

   /* Update Image Browser */
   image_browser_update();
   
   //Set the images label for normal image
   im = edje_edit_state_image_get(edje_o, Cur.part->string, Cur.state->string);
   if (im)
   {
      etk_label_set(ETK_LABEL(UI_ImageNameLabel), im);
      edje_edit_string_free(im);
   }
   else
   {
      etk_label_set(ETK_LABEL(UI_ImageNameLabel), "none");
   }

   //Set alpha and borders
   int alpha, t, l, r, b;
   edje_edit_state_color_get(edje_o, Cur.part->string, Cur.state->string,
                             NULL, NULL, NULL, &alpha);
   edje_edit_state_image_border_get(edje_o, Cur.part->string, Cur.state->string,
                                    &l, &r, &t, &b);
   etk_range_value_set(ETK_RANGE(UI_ImageAlphaSlider), alpha);
   etk_range_value_set(ETK_RANGE(UI_BorderLeftSpinner), l);
   etk_range_value_set(ETK_RANGE(UI_BorderRightSpinner), r);
   etk_range_value_set(ETK_RANGE(UI_BorderTopSpinner), t);
   etk_range_value_set(ETK_RANGE(UI_BorderBottomSpinner), b);
   
   //ReEnable Signal Propagation
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_BorderLeftSpinner),
                      _image_border_spinners_value_changed_cb,
                      (void*)BORDER_LEFT);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_BorderRightSpinner),
                      _image_border_spinners_value_changed_cb,
                      (void*)BORDER_RIGHT);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_BorderTopSpinner),
                      _image_border_spinners_value_changed_cb,
                      (void*)BORDER_TOP);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_BorderBottomSpinner),
                      _image_border_spinners_value_changed_cb,
                      (void*)BORDER_BOTTOM);
   etk_signal_unblock("value-changed", ETK_OBJECT(UI_ImageAlphaSlider),
                      _image_AlphaSlider_value_changed_cb, NULL);
   etk_signal_unblock("icon-selected", ETK_OBJECT(UI_ImageBrowserIconbox),
                    ETK_CALLBACK(_image_browser_iconbox_selected_cb), NULL);

}

void
image_tweenlist_populate(void)
{
   Evas_List *tweens, *l;
   Etk_Tree_Col *col;
   
   if (!etk_string_length_get(Cur.state)) return;
   if (!etk_string_length_get(Cur.part)) return;
   
   col = etk_tree_nth_col_get(ETK_TREE(UI_ImageTweenList), 0);
   
   etk_tree_clear(ETK_TREE(UI_ImageTweenList));
   
   tweens = l = edje_edit_state_tweens_list_get(edje_o, Cur.part->string, Cur.state->string);
   while (l)
   {
      //printf("RET: %s (id: %d)\n", l->data, edje_edit_image_id_get(edje_o, l->data));
      //snprintf(buf, sizeof(buf), "images/%d", edje_edit_image_id_get(edje_o, l->data)); TODO: find a way to append image directly from the edje file.
      etk_tree_row_append(ETK_TREE(UI_ImageTweenList), NULL,
                          col, NULL, NULL, l->data,
                          NULL);
      l = l->next;
   }
   edje_edit_string_list_free(tweens);
}

Etk_Widget*
image_browser_create(void)
{
   Etk_Widget *hbox, *vbox, *vbox2;
   Etk_Widget *button;
   Etk_Widget *frame;
   Etk_Widget *label;
   Etk_Widget *image;
   
   //ImageBrowserWin
   UI_ImageBrowserWin = etk_window_new();
   etk_window_title_set(ETK_WINDOW(UI_ImageBrowserWin), "Image Browser");
   etk_window_resize(ETK_WINDOW(UI_ImageBrowserWin), 590, 370);
   etk_signal_connect("delete-event", ETK_OBJECT(UI_ImageBrowserWin),
                      ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_container_border_width_set(ETK_CONTAINER(UI_ImageBrowserWin), 5);

   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add (ETK_CONTAINER(UI_ImageBrowserWin), hbox);
   
   //ImageBrowserIconbox
   UI_ImageBrowserIconbox = etk_iconbox_new();
   etk_signal_connect("icon-selected", ETK_OBJECT(UI_ImageBrowserIconbox),
                      ETK_CALLBACK(_image_browser_iconbox_selected_cb), NULL);
   etk_box_append(ETK_BOX(hbox), UI_ImageBrowserIconbox,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_box_append (ETK_BOX(hbox), vbox, ETK_BOX_START, ETK_BOX_NONE, 5);
   
   //AddImageButton
   button = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   etk_object_properties_set(ETK_OBJECT(button), "label","Import image",NULL);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_BOTH_VERT);
   etk_signal_connect("clicked", ETK_OBJECT(button),
            ETK_CALLBACK(_window_all_button_click_cb), (void*)TOOLBAR_IMAGE_FILE_ADD);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   //DeleteImageButton
   button = etk_button_new_from_stock(ETK_STOCK_EDIT_DELETE);
   etk_object_properties_set(ETK_OBJECT(button), "label","Remove image",NULL);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_BOTH_VERT);
   etk_widget_disabled_set(button, 1);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   //EditImageButton
   button = etk_button_new_with_label("Edit Image");
   image = etk_image_new_from_edje(EdjeFile, "GIMP.PNG");
   etk_button_image_set(ETK_BUTTON(button), ETK_IMAGE(image));
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_BOTH_VERT);
   etk_widget_disabled_set(button, 1);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   //ImageInfo
   frame = etk_frame_new("Image Info");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 10);
   
   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox2);
   
   label = etk_label_new("<b>name:</b>");
   etk_box_append(ETK_BOX(vbox2), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   label = etk_label_new("Unknow");
   etk_box_append(ETK_BOX(vbox2), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   label = etk_label_new(" ");
   etk_box_append(ETK_BOX(vbox2), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   UI_ImageBrowserSizeLabel = etk_label_new("<b>size:</b> Unknow");
   etk_object_properties_set(ETK_OBJECT(UI_ImageBrowserSizeLabel), "xalign", 0, NULL);
   etk_box_append(ETK_BOX(vbox2), UI_ImageBrowserSizeLabel,
                  ETK_BOX_START, ETK_BOX_NONE, 0);
   
   label = etk_label_new("<b>comp:</b> Unknow");
   etk_box_append(ETK_BOX(vbox2), label, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   //CloseButton
   button = etk_button_new_from_stock(ETK_STOCK_DIALOG_CLOSE);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_BOTH_VERT);
   etk_signal_connect("clicked", ETK_OBJECT(button),
            ETK_CALLBACK(_window_all_button_click_cb), (void*)IMAGE_BROWSER_CLOSE);
   etk_box_append(ETK_BOX(vbox), button, 0, ETK_BOX_NONE, 0);
   
   return UI_ImageBrowserWin;
}

void
image_browser_populate(void)
{
   Evas_List *l, *images;
   char buf[4096];
   
   etk_iconbox_clear(ETK_ICONBOX(UI_ImageBrowserIconbox));
   
   etk_iconbox_freeze(ETK_ICONBOX(UI_ImageBrowserIconbox));
   images = l = edje_edit_images_list_get(edje_o);
   while (l)
   {
      snprintf(buf,4096,"images/%d",edje_edit_image_id_get(edje_o, (char*)l->data));
      etk_iconbox_append(ETK_ICONBOX(UI_ImageBrowserIconbox),
                         Cur.edj_temp_name->string, buf, (char*)l->data);
      l = l->next;
   }
   edje_edit_string_list_free(images);
   etk_iconbox_thaw(ETK_ICONBOX(UI_ImageBrowserIconbox));
}

void
image_browser_show(int UpdateCurrent)
{
   ImageBroserUpdate = UpdateCurrent;
   image_browser_populate();
   image_browser_update();
   etk_widget_show_all(UI_ImageBrowserWin);
}

void
image_browser_update(void)
{
   const char *pi;
   char buf[4096];
   Etk_Iconbox_Icon *icon;
   
   if (ImageBroserUpdate)
   {
      //window title
      snprintf(buf, 4096, "Choose image for part \"%s\" state: \"%s\"",Cur.part->string, Cur.state->string);
      etk_window_title_set(ETK_WINDOW(UI_ImageBrowserWin), buf);
      
      //iconbox
      etk_iconbox_unselect_all(ETK_ICONBOX(UI_ImageBrowserIconbox));
      pi = edje_edit_state_image_get(edje_o, Cur.part->string, Cur.state->string);
      if (pi)
      {
         icon = etk_iconbox_icon_get_by_label( ETK_ICONBOX(UI_ImageBrowserIconbox), pi);
         etk_iconbox_icon_select(icon);
      }
      //preview
      //~ snprintf(buf, 4096, "images/%d", edje_edit_image_id_get(edje_o, pi));
      //~ printf("IM: %s\n", buf);
      //~ etk_image_set_from_file(UI_ImageBrowserPreview, Cur.edj_temp_name->string, buf);
     
      //~ edje_edit_string_free(pi);
      
      //int w, h;
      //etk_image_size_get (etk_image_new_from_file(icon->filename, icon->edje_group), &w, &h);
      //printf("W: %d H: %d (%d)\n", w, h, etk_image_new_from_file(icon->filename, icon->edje_group));
      //etk_label_set(UI_ImageBrowserSizeLabel, "ciccio");
      
   }
   else
   {
      etk_window_title_set(ETK_WINDOW(UI_ImageBrowserWin), "Image Browser");
   }

}


/* Image Frame Callbacks */
Etk_Bool
_image_TweenList_row_selected_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Tree_Col *col;
   char *selected = NULL;
   printf("Row selected signal on ImageTweenList EMITTED\n");
   
   col = etk_tree_nth_col_get(ETK_TREE(UI_ImageTweenList), 0);
   etk_tree_row_fields_get(row, col, NULL, NULL, &selected, NULL);
   if (!selected) return ETK_TRUE;
   
   Cur.tween = etk_string_set(Cur.tween, selected);
   etk_widget_disabled_set(UI_DeleteTweenButton, FALSE);
  // etk_widget_disabled_set(UI_MoveDownTweenButton, FALSE);
  // etk_widget_disabled_set(UI_MoveUpTweenButton, FALSE);
   
   return ETK_TRUE;
}

Etk_Bool
_image_AlphaSlider_value_changed_cb(Etk_Object *object, double va, void *data)
{
   printf("ImageSlieder value_changed signale EMIT: %.2f\n",va);

   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   edje_edit_state_color_set(edje_o, Cur.part->string, Cur.state->string,
                             -1, -1, -1, (int)va);
   canvas_redraw();

   return ETK_TRUE;
}

Etk_Bool
_image_border_spinners_value_changed_cb(Etk_Range *range, double value, void *data)
{
   printf("Value Changed signal on BorderSpinner EMITTED (value: %f)\n",etk_range_value_get(range));

   if (!etk_string_length_get(Cur.state)) return ETK_TRUE;
   if (!etk_string_length_get(Cur.part)) return ETK_TRUE;
   edje_edit_state_image_border_set(edje_o, Cur.part->string, Cur.state->string,
      (int)etk_range_value_get(ETK_RANGE(UI_BorderLeftSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderRightSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderTopSpinner)),
      (int)etk_range_value_get(ETK_RANGE(UI_BorderBottomSpinner)));

   canvas_redraw();

   return ETK_TRUE;
}
/* Image Browser Callbacks */
Etk_Bool
_image_browser_iconbox_selected_cb(Etk_Iconbox *iconbox, Etk_Iconbox_Icon *icon, void *data)
{
   const char *image;
   image = etk_iconbox_icon_label_get(icon);

   printf("CLICK ON: %s :)\n", image);
   if (ImageBroserUpdate && etk_string_length_get(Cur.part) && etk_string_length_get(Cur.state))
     {
        edje_edit_state_image_set(edje_o, Cur.part->string, Cur.state->string, image);
     }
   image_frame_update();

   return ETK_TRUE;
}
