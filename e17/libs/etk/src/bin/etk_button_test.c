#include "etk_test.h"
#include "config.h"

/**************************
 *
 * Creation of the test-app window
 *
 **************************/

/* Creates the window for the button test */
void etk_test_button_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *table;
   Etk_Widget *frame;
   Etk_Widget *vbox;
   Etk_Widget *button;
   Etk_Widget *image;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Button Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   table = etk_table_new(2, 2, ETK_TABLE_HHOMOGENEOUS);
   etk_container_add(ETK_CONTAINER(win), table);


   /*******************
    * Normal buttons
    *******************/
   frame = etk_frame_new("Buttons");
   etk_table_attach_default(ETK_TABLE(table), frame, 0, 0, 0, 0);
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(frame), vbox);

   button = etk_button_new_with_label("Normal button");
   etk_tooltips_tip_set(button, "A normal button");
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_button_new_with_label("Button with an image");
   etk_tooltips_tip_set(button, "A button with an image inside");
   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/right_arrow.png", NULL);
   etk_button_image_set(ETK_BUTTON(button), ETK_IMAGE(image));
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_button_new();
   etk_tooltips_tip_set(button, "A mute button");
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_button_new_with_label("Disabled button");
   etk_tooltips_tip_set(button, "A disabled button");
   etk_widget_disabled_set(button, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);


   /*******************
    * Toggle buttons
    *******************/
   frame = etk_frame_new("Toggle Buttons");
   etk_table_attach_default(ETK_TABLE(table), frame, 1, 1, 0, 0);
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(frame), vbox);

   button = etk_toggle_button_new_with_label("Toggle button");
   etk_tooltips_tip_set(button, "An on/off button");
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_toggle_button_new();
   etk_tooltips_tip_set(button, "A mute toggle");
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_toggle_button_new_with_label("Disabled toggle button 1");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(button), ETK_TRUE);
   etk_widget_disabled_set(button, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_toggle_button_new_with_label("Disabled toggle button 2");
   etk_widget_disabled_set(button, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);


   /*******************
    * Check buttons
    *******************/
   frame = etk_frame_new("Check Buttons");
   etk_table_attach_default(ETK_TABLE(table), frame, 0, 0, 1, 1);
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(frame), vbox);

   button = etk_check_button_new_with_label("Check button");
   etk_tooltips_tip_set(button, "A Checkbox button");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(button), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_check_button_new();
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_check_button_new_with_label("Disabled check button 1");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(button), ETK_TRUE);
   etk_widget_disabled_set(button, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_check_button_new_with_label("Disabled check button 2");
   etk_widget_disabled_set(button, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);


   /*******************
    * Radio buttons
    *******************/
   frame = etk_frame_new("Radio Buttons");
   etk_table_attach_default(ETK_TABLE(table), frame, 1, 1, 1, 1);
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(frame), vbox);

   button = etk_radio_button_new_with_label("Radio button", NULL);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_radio_button_new_from_widget(ETK_RADIO_BUTTON(button));
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_radio_button_new_with_label("Disabled radio button 1", NULL);
   etk_widget_disabled_set(button, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_radio_button_new_with_label_from_widget("Disabled radio button 2", ETK_RADIO_BUTTON(button));
   etk_widget_disabled_set(button, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);


   etk_widget_show_all(win);
}
