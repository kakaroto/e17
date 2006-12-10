#include "etk_test.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"

static void _etk_test_tree2_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);

/* Creates the window for the tree test */
void etk_test_tree2_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *tree;
   Etk_Tree2_Col *col1, *col2, *col3, *col4;
   Etk_Tree2_Row *row;
   int i;

   if (win)
   {
      etk_widget_show(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_window_title_set(ETK_WINDOW(win), "Etk Tree Test");
   etk_window_resize(ETK_WINDOW(win), 440, 500);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   tree = etk_tree2_new();
   etk_container_add(ETK_CONTAINER(win), tree);
   etk_signal_connect("key_down", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree2_key_down_cb), NULL);

   etk_tree2_mode_set(ETK_TREE2(tree), ETK_TREE2_MODE_TREE);
   etk_tree2_multiple_select_set(ETK_TREE2(tree), ETK_TRUE);
   col1 = etk_tree2_col_new(ETK_TREE2(tree), "Column 1", etk_tree2_model_icon_text_new(ETK_TREE2(tree), ETK_TREE_FROM_EDJE), 90);
   col2 = etk_tree2_col_new(ETK_TREE2(tree), "Column 2", etk_tree2_model_double_new(ETK_TREE2(tree)), 60);
   col3 = etk_tree2_col_new(ETK_TREE2(tree), "Column 3", etk_tree2_model_image_new(ETK_TREE2(tree), ETK_TREE_FROM_FILE), 60);
   col4 = etk_tree2_col_new(ETK_TREE2(tree), "Column 4", etk_tree2_model_checkbox_new(ETK_TREE2(tree)), 90);
   etk_tree2_build(ETK_TREE2(tree));
   
   etk_tree2_freeze(ETK_TREE2(tree));
   for (i = 0; i < 50; i++)
   {
      row = etk_tree2_row_append(ETK_TREE2(tree), NULL, col1, etk_theme_icon_get(), "places/user-home_16", "Row1",
         col2, 10.0, col3, PACKAGE_DATA_DIR "/images/1star.png", col4, ETK_FALSE, NULL);
      row = etk_tree2_row_append(ETK_TREE2(tree), row, col1, etk_theme_icon_get(), "places/folder_16", "Row2",
         col2, 20.0, col3, PACKAGE_DATA_DIR "/images/2stars.png", col4, ETK_TRUE, NULL);
      etk_tree2_row_append(ETK_TREE2(tree), row, col1, etk_theme_icon_get(), "mimetypes/text-x-generic_16", "Row3",
         col2, 30.0, col3, PACKAGE_DATA_DIR "/images/3stars.png", col4, ETK_TRUE, NULL);
   }
   etk_tree2_thaw(ETK_TREE2(tree));
   
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
