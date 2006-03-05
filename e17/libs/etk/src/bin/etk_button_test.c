#include "etk_test.h"
#include "config.h"

/* Creates the window for the button test */
void etk_test_button_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *button_normal;
   Etk_Widget *button_toggle;
   Etk_Widget *button_check;
   Etk_Widget *button_radio;
   Etk_Widget *image;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }      
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Button Test"));
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(win), vbox);

   button_normal = etk_button_new_with_label(_("Normal button"));
   etk_tooltips_tip_set(button_normal,  "This is a normal button");
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);

   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/e_icon.png");
   button_normal = etk_button_new_with_label(_("Button with an image"));
   etk_tooltips_tip_set(button_normal,  "This is a normal button with an image");
   etk_button_image_set(ETK_BUTTON(button_normal), ETK_IMAGE(image));
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);
   
   button_normal = etk_button_new();
   etk_tooltips_tip_set(button_normal,  "This is a normal button with nothing");
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);
   
   button_check = etk_check_button_new_with_label(_("Check button"));
   etk_tooltips_tip_set(button_check,  _("This is a check button"));   
   etk_box_pack_start(ETK_BOX(vbox), button_check, ETK_FALSE, ETK_FALSE, 0);
   
   button_check = etk_check_button_new();   
   etk_tooltips_tip_set(button_check,  _("This is another check button"));   
   etk_box_pack_start(ETK_BOX(vbox), button_check, ETK_FALSE, ETK_FALSE, 0);

   button_radio = etk_radio_button_new_with_label(_("Radio button"), NULL);
   etk_tooltips_tip_set(button_radio,  _("This is a radio button"));
   etk_box_pack_start(ETK_BOX(vbox), button_radio, ETK_FALSE, ETK_FALSE, 0);
   
   button_radio = etk_radio_button_new_from_widget(ETK_RADIO_BUTTON(button_radio));
   etk_tooltips_tip_set(button_radio,  _("This is another radio button"));
   etk_box_pack_start(ETK_BOX(vbox), button_radio, ETK_FALSE, ETK_FALSE, 0);
   
   button_toggle = etk_toggle_button_new_with_label(_("Toggle button"));
   etk_tooltips_tip_set(button_toggle,  _("This is a toggle button"));
   etk_box_pack_start(ETK_BOX(vbox), button_toggle, ETK_FALSE, ETK_FALSE, 0);
   
   button_toggle = etk_toggle_button_new();
   etk_tooltips_tip_set(button_toggle,  _("This is another toggle button"));   
   etk_box_pack_start(ETK_BOX(vbox), button_toggle, ETK_FALSE, ETK_FALSE, 0);

   etk_widget_show_all(win);
}
