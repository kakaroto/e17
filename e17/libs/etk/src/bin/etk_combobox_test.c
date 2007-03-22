#include "etk_test.h"
#include <stdlib.h>

#define STARTING_STOCK_ID ETK_STOCK_DIALOG_APPLY
#define ENDING_STOCK_ID ETK_STOCK_FOLDER_NEW

static void _active_item_changed_cb(Etk_Object *object, void *data);

/**************************
 *
 * Creation of the test-app window
 *
 **************************/
 
/* Creates the window for the combobox test */
void etk_test_combobox_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *combobox;
   Etk_Widget *image;
   Etk_Widget *checkbox;
   Etk_Widget *frame;
   Etk_Combobox_Item *item;
   int *stock_id;
   int i;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Combobox Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete-event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(win), vbox);
   

   /*******************
    * Simple combobox
    *******************/
   frame = etk_frame_new("Simple combobox");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* Create a simple combobox containing one column of labels and add 3 items to it */
   combobox = etk_combobox_new_default();
   etk_container_add(ETK_CONTAINER(frame), combobox);
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 1");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 2");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 3");
   
   
   /*******************
    * Disabled combobox
    *******************/
   frame = etk_frame_new("Disabled combobox");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* Create a simple combobox containing one column of labels and add 3 items to it */
   combobox = etk_combobox_new_default();
   etk_container_add(ETK_CONTAINER(frame), combobox);
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Disabled text");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 2");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 3");
   etk_widget_disabled_set(combobox, ETK_TRUE);
   
   
   /*******************
    * Complex combobox
    *******************/
   frame = etk_frame_new("Some stock icons");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   image = etk_image_new_from_stock(STARTING_STOCK_ID, ETK_STOCK_BIG);
   etk_box_append(ETK_BOX(vbox), image, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* Create a more complex combobox with three columns (an image, a label and a checkbox) */
   combobox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_EXPAND, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_OTHER, 24, ETK_COMBOBOX_NONE, 1.0);
   etk_combobox_build(ETK_COMBOBOX(combobox));
   etk_box_append(ETK_BOX(vbox), combobox, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("active-item-changed", ETK_OBJECT(combobox), ETK_CALLBACK(_active_item_changed_cb), image);
   
   /* We fill the combobox with some stock-ids */
   for (i = STARTING_STOCK_ID; i <= ENDING_STOCK_ID; i++)
   {
      image = etk_image_new_from_stock(i, ETK_STOCK_SMALL);
      checkbox = etk_check_button_new();
      item = etk_combobox_item_append(ETK_COMBOBOX(combobox), image, etk_stock_label_get(i), checkbox);
      
      stock_id = malloc(sizeof(int));
      *stock_id = i;
      etk_combobox_item_data_set_full(item, stock_id, free);
   }

   etk_widget_show_all(win);
}

/**************************
 *
 * Callbacks
 *
 **************************/

/* Called when the active item of the combobox has been changed */
static void _active_item_changed_cb(Etk_Object *object, void *data)
{
   Etk_Combobox *combobox;
   Etk_Image *image;
   Etk_Combobox_Item *active_item;
   int *stock_id;
   
   if (!(combobox = ETK_COMBOBOX(object)) || !(image = ETK_IMAGE(data)))
      return;
   if (!(active_item = etk_combobox_active_item_get(combobox)))
      return;
   if (!(stock_id = etk_combobox_item_data_get(active_item)))
      return;
   
   etk_image_set_from_stock(image, *stock_id, ETK_STOCK_BIG);
}
