#include "etk_test.h"
#include "config.h"

static void _etk_test_button_drag_drop_cb(Etk_Object *object, void *data);

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
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);

   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/e_icon.png");
   button_normal = etk_button_new_with_label(_("Button with an image"));
   etk_button_image_set(ETK_BUTTON(button_normal), ETK_IMAGE(image));
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);
   
   button_normal = etk_button_new();
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);
   
   button_check = etk_check_button_new_with_label(_("Check button"));
   etk_box_pack_start(ETK_BOX(vbox), button_check, ETK_FALSE, ETK_FALSE, 0);
   
   button_check = etk_check_button_new();
   etk_box_pack_start(ETK_BOX(vbox), button_check, ETK_FALSE, ETK_FALSE, 0);

   button_radio = etk_radio_button_new_with_label(_("Radio button"), NULL);
   etk_box_pack_start(ETK_BOX(vbox), button_radio, ETK_FALSE, ETK_FALSE, 0);
   
   button_radio = etk_radio_button_new_from_widget(ETK_RADIO_BUTTON(button_radio));
   etk_box_pack_start(ETK_BOX(vbox), button_radio, ETK_FALSE, ETK_FALSE, 0);
   
   button_toggle = etk_toggle_button_new_with_label(_("Toggle button"));
   etk_widget_dnd_aware_set(button_toggle, ETK_TRUE);
   etk_signal_connect("drag_drop", ETK_OBJECT(button_toggle), ETK_CALLBACK(_etk_test_button_drag_drop_cb), button_toggle);
   etk_box_pack_start(ETK_BOX(vbox), button_toggle, ETK_FALSE, ETK_FALSE, 0);
   
   button_toggle = etk_toggle_button_new();
   etk_box_pack_start(ETK_BOX(vbox), button_toggle, ETK_FALSE, ETK_FALSE, 0);

   etk_widget_show_all(win);
}

/* Called when a file is dropped on the button */
static void _etk_test_button_drag_drop_cb(Etk_Object *object, void *data)
{
   int num_files, i;
   const char **files;
   
   if (!object)
      return;
   
   files = etk_widget_dnd_files_get(ETK_WIDGET(object), &num_files);
   printf("Our test widget got a drop with %d files\n", num_files);
   
   for (i = 0; i < num_files; i++)
      printf("Widget got the file: %s\n", files[i]);
}
