#include "etk_test.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"

static void _etk_test_tree2_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static void _etk_test_tree2_row_clicked_cb(Etk_Object *object, Etk_Tree2_Row *row, Etk_Event_Mouse_Up *event, void *data);

/* Creates the window for the tree test */
void etk_test_tree2_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *tree;
   Etk_Tree2_Col *col1, *col2, *col3, *col4;
   Etk_Tree2_Row *row;
   Etk_Widget *statusbar;
   Etk_Widget *alignment;
   char row_name[128];
   int i;

   if (win)
   {
      etk_widget_show(ETK_WIDGET(win));
      return;
   }

   /* Create the window and the vbox where the widgets will be packed into */
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Tree Test");
   etk_window_resize(ETK_WINDOW(win), 440, 500);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   /* TODO: we shouldn't be obliged to create an alignement to do that... */
   alignment = etk_alignment_new(0.5, 0.5, 1.0, 1.0);
   etk_container_border_width_set(ETK_CONTAINER(alignment), 5);
   etk_box_append(ETK_BOX(vbox), alignment, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   /* Create the tree widget */
   tree = etk_tree2_new();
   etk_tree2_mode_set(ETK_TREE2(tree), ETK_TREE2_MODE_TREE);
   etk_tree2_multiple_select_set(ETK_TREE2(tree), ETK_TRUE);
   etk_container_add(ETK_CONTAINER(alignment), tree);

   /* We first create the columns of the tree, and then we "build" the tree with etk_tree2_build() */
   col1 = etk_tree2_col_new(ETK_TREE2(tree), "Column 1", etk_tree2_model_icon_text_new(ETK_TREE2(tree), ETK_TREE_FROM_EDJE), 130);
   col2 = etk_tree2_col_new(ETK_TREE2(tree), "Column 2", etk_tree2_model_double_new(ETK_TREE2(tree)), 60);
   col3 = etk_tree2_col_new(ETK_TREE2(tree), "Column 3", etk_tree2_model_image_new(ETK_TREE2(tree), ETK_TREE_FROM_FILE), 60);
   col4 = etk_tree2_col_new(ETK_TREE2(tree), "Column 4", etk_tree2_model_checkbox_new(ETK_TREE2(tree)), 90);
   etk_tree2_build(ETK_TREE2(tree));
   
   /* Then we add the rows to the tree. etk_tree2_freeze/thaw() is used to improve the speed when you insert a lot
    * of rows. It's not really useful here since we insert only 150 rows */
   etk_tree2_freeze(ETK_TREE2(tree));
   for (i = 0; i < 50; i++)
   {
      sprintf(row_name, "Row %d", (i * 3) + 1);
      row = etk_tree2_row_append(ETK_TREE2(tree), NULL, col1, etk_theme_icon_get(), "places/user-home_16", row_name,
         col2, 10.0, col3, PACKAGE_DATA_DIR "/images/1star.png", col4, ETK_FALSE, NULL);
      
      sprintf(row_name, "Row %d", (i * 3) + 2);
      row = etk_tree2_row_append(ETK_TREE2(tree), row, col1, etk_theme_icon_get(), "places/folder_16", row_name,
         col2, 20.0, col3, PACKAGE_DATA_DIR "/images/2stars.png", col4, ETK_TRUE, NULL);
      
      sprintf(row_name, "Row %d", (i * 3) + 3);
      etk_tree2_row_append(ETK_TREE2(tree), row, col1, etk_theme_icon_get(), "mimetypes/text-x-generic_16", row_name,
         col2, 30.0, col3, PACKAGE_DATA_DIR "/images/3stars.png", col4, ETK_TRUE, NULL);
   }
   etk_tree2_thaw(ETK_TREE2(tree));
   
   /* Finally we create the statusbar used to display the events on the tree */
   statusbar = etk_statusbar_new();
   etk_box_append(ETK_BOX(vbox), statusbar, ETK_BOX_START, ETK_BOX_FILL, 0);
   
   
   etk_signal_connect("key_down", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree2_key_down_cb), NULL);
   etk_signal_connect("row_clicked", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree2_row_clicked_cb), statusbar);
   
   etk_widget_show_all(win);
}

/* Called when a key is pressed while the tree is focused:
 * we use this to delete the selected rows if "DEL" is pressed */
static void _etk_test_tree2_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Tree2 *tree;
   Etk_Tree2_Row *r;
   
   if (!(tree = ETK_TREE2(object)))
      return;
   
   if (strcmp(event->keyname, "Delete") == 0)
   {
      /* We walk through all the rows of the tree, and we delete those which are selected.
       * Note that we can safely manipulate "r" with etk_tree2_row_walk_next(), even if it
       * has been deleted with etk_tree2_row_delete(), because etk_tree2_row_delete() just
       * marks the row as deleted, but the row is not effectively deleted immediately */
      for (r = etk_tree2_first_row_get(tree); r; r = etk_tree2_row_walk_next(r, ETK_TRUE))
      {
         if (etk_tree2_row_is_selected(r))
            etk_tree2_row_delete(r);
      }
      
      etk_signal_stop();
   }
}

/* Called when a row of the tree is clicked: we display the clicked row in the statusbar */
/* TODO: sometimes it's a Etk_Event_Mouse_Up, sometimes a Etk_Event_Mouse_Down... */
static void _etk_test_tree2_row_clicked_cb(Etk_Object *object, Etk_Tree2_Row *row, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Tree2 *tree;
   Etk_Statusbar *statusbar;
   char *row_name;
   char message[1024];
   
   if (!(tree = ETK_TREE2(object)) || !(statusbar = ETK_STATUSBAR(data)) || !row || !event)
      return;
   
   /* We retrieve the name of the row: this information is located in the first column,
    * and it corresponds to the third param of the model (the two first params correspond
    * to the icon used, we ignore them here (that's why there are two NULL params)).
    * etk_tree2_row_fields_get() have to be terminated by NULL */
   etk_tree2_row_fields_get(row, etk_tree2_nth_col_get(tree, 0), NULL, NULL, &row_name, NULL);
   
   sprintf(message, "Row \"%s\" clicked (%s)", row_name,
      (event->flags & ETK_MOUSE_TRIPLE_CLICK) ? "Triple" :
      ((event->flags & ETK_MOUSE_DOUBLE_CLICK) ? "Double" : "Single"));
   
   etk_statusbar_message_push(statusbar, message, 0);
}
