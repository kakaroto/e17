#include "etk_test.h"
#include <string.h>
#include "config.h"

static void _etk_test_xdnd_drag_drop_cb(Etk_Object *object, void *data);
static void _etk_test_xdnd_drag_motion_cb(Etk_Object *object, void *data);
static void _etk_test_xdnd_drag_leave_cb(Etk_Object *object, void *data);
static void _etk_test_xdnd_drag_drop_cb2(Etk_Object *object, void *data);
static void _etk_test_xdnd_clipboard_text_request_cb(Etk_Object *object, void *event, void *data);
static void _etk_test_xdnd_button_paste_cb(Etk_Object *object, void *data);
static void _etk_test_xdnd_button_copy_cb(Etk_Object *object, void *data);

/* Creates the window for the xdnd test */
void etk_test_xdnd_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *button;
   Etk_Widget *label;
   Etk_Widget *image;
   Etk_Widget *entry;
   
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
   etk_widget_dnd_aware_set(button, ETK_TRUE);
   etk_signal_connect("drag_drop", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_xdnd_drag_drop_cb), label);
   etk_signal_connect("drag_motion", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_xdnd_drag_motion_cb), NULL);
   etk_signal_connect("drag_leave", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_xdnd_drag_leave_cb), NULL);
   etk_box_pack_start(ETK_BOX(vbox), button, ETK_FALSE, ETK_FALSE, 0);
   
   etk_box_pack_start(ETK_BOX(vbox), label, ETK_FALSE, ETK_FALSE, 0);   

   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/e_icon.png");

   button = etk_button_new_with_label(_("Drag Any Image Onto Me"));
   etk_widget_dnd_aware_set(button, ETK_TRUE);
   etk_signal_connect("drag_drop", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_xdnd_drag_drop_cb2), image);
   etk_box_pack_start(ETK_BOX(vbox), button, ETK_FALSE, ETK_FALSE, 0);  
   etk_box_pack_start(ETK_BOX(vbox), image, ETK_FALSE, ETK_FALSE, 0);

   label = etk_label_new("");
   etk_signal_connect("selection_get", ETK_OBJECT(label), ETK_CALLBACK(_etk_test_xdnd_clipboard_text_request_cb), NULL);
   
   button = etk_button_new_with_label(_("Press me to paste text"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_xdnd_button_paste_cb), label);
   etk_box_pack_start(ETK_BOX(vbox), button, ETK_FALSE, ETK_FALSE, 0);  
   etk_box_pack_start(ETK_BOX(vbox), label, ETK_FALSE, ETK_FALSE, 0);   
   
   entry = etk_entry_new();
   button = etk_button_new_with_label(_("Click me to copy text below"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_xdnd_button_copy_cb), entry);
   etk_box_pack_start(ETK_BOX(vbox), button, ETK_FALSE, ETK_FALSE, 0);   
   etk_box_pack_start(ETK_BOX(vbox), entry, ETK_FALSE, ETK_FALSE, 0);
   
   etk_widget_show_all(win);
}

/* Called when a file is dropped on the button */
static void _etk_test_xdnd_drag_drop_cb(Etk_Object *object, void *data)
{
   int num_files, i;
   const char **files;
   
   files = etk_widget_dnd_files_get(ETK_WIDGET(object), &num_files);
   printf("Our test widget got a drop with %d files\n", num_files);
   
   for (i = 0; i < num_files; i++)
   {
      printf("Widget got the file: %s\n", files[i]);
      etk_label_set(ETK_LABEL(data), files[i]);
   }
}

/* Called when a file is dragged aboved the button */
static void _etk_test_xdnd_drag_motion_cb(Etk_Object *object, void *data)
{   
   printf("You're on top of me!\n");
}

/* Called when a file is dragged out of the button */
static void _etk_test_xdnd_drag_leave_cb(Etk_Object *object, void *data)
{   
   printf("You're leaving me!\n");
}

/* Called when a file is dropped on the second button */
static void _etk_test_xdnd_drag_drop_cb2(Etk_Object *object, void *data)
{
   int num_files, i;
   const char **files;
   
   files = etk_widget_dnd_files_get(ETK_WIDGET(object), &num_files);
   printf("Our test widget got a drop with %d files\n", num_files);
   
   for (i = 0; i < num_files; i++)
   {
      char *image;
      
      printf("Widget got the file: %s\n", files[i]);
      if ((image = strstr(files[i], "file://")) != NULL)
         etk_image_set_from_file(ETK_IMAGE(data), image + strlen("file://"));
   }
}

/* Called when a some text is pasted */
static void _etk_test_xdnd_clipboard_text_request_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Selection_Get *ev;
   
   ev = event;
   etk_label_set(ETK_LABEL(object), (char *)ev->data);
}

/* Called when the "paste" button is clicked */
static void _etk_test_xdnd_button_paste_cb(Etk_Object *object, void *data)
{
   etk_selection_text_request(ETK_WIDGET(data));
}

/* Called when the "copy" button is clicked */
static void _etk_test_xdnd_button_copy_cb(Etk_Object *object, void *data)
{
   etk_selection_text_set(ETK_WIDGET(data), etk_entry_text_get(ETK_ENTRY(data)));
}
