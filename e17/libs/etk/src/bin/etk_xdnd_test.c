#include <string.h>

#include "etk_test.h"
#include "config.h"

static void _etk_test_xdnd_drag_drop_cb(Etk_Object *object, void *data)
{
   int num_files, i;
   const char **files;
   
   files = etk_widget_xdnd_files_get(ETK_WIDGET(object), &num_files);
   printf("Our test widget got a drop with %d files\n", num_files);
   
   for (i = 0; i < num_files; i++)
     {
	printf("Widget got: file: %s\n", files[i]);
	etk_label_set(ETK_LABEL(data), files[i]);
     }
}

static void _etk_test_xdnd_drag_drop_cb2(Etk_Object *object, void *data)
{
   int num_files, i;
   const char **files;
   
   files = etk_widget_xdnd_files_get(ETK_WIDGET(object), &num_files);
   printf("Our test widget got a drop with %d files\n", num_files);
   
   for (i = 0; i < num_files; i++)
     {
	char *image;
	
	printf("Widget got: file: %s\n", files[i]);
	if((image = strstr(files[i], "file://")) != NULL)
	  etk_image_set_from_file(ETK_IMAGE(data), image + strlen("file://"));
     }   
   
}

/* Creates the window for the xdnd test */

void etk_test_xdnd_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *button;
   Etk_Widget *label;
   Etk_Widget *image;
   
   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk X Drag / Drop Test"));
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(win), vbox);

   label = etk_label_new(_("No File Set"));   
   
   button = etk_button_new_with_label(_("Drag Any File Onto Me"));
   etk_widget_xdnd_set(button, ETK_TRUE);
   etk_signal_connect("drag_drop", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_xdnd_drag_drop_cb), label);
   etk_box_pack_start(ETK_BOX(vbox), button, ETK_FALSE, ETK_FALSE, 0);
   
   etk_box_pack_start(ETK_BOX(vbox), label, ETK_FALSE, ETK_FALSE, 0);   

   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/e_icon.png");

   button = etk_button_new_with_label(_("Drag Any Image Onto Me"));
   etk_widget_xdnd_set(button, ETK_TRUE);
   etk_signal_connect("drag_drop", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_xdnd_drag_drop_cb2), image);
   etk_box_pack_start(ETK_BOX(vbox), button, ETK_FALSE, ETK_FALSE, 0);

   etk_box_pack_start(ETK_BOX(vbox), image, ETK_FALSE, ETK_FALSE, 0);   
   
   etk_widget_show_all(win);
}
