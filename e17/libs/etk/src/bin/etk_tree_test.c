#include "etk_test.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"

static void _etk_test_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static void _etk_test_tree_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);
static void _etk_test_tree_checkbox_toggled_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);

/* Creates the window for the tree test */
void etk_test_tree_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *tree;
   Etk_Tree_Col *col1, *col2, *col3, *col4, *col5;
   Etk_Tree_Row *row;
   Etk_Widget *statusbar;
   Etk_Color c_warn, c_default2;
   char row_name[128];
   const char *stock_key;
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
   
   /* Create the tree widget */
   tree = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_TREE);
   etk_tree_multiple_select_set(ETK_TREE(tree), ETK_TRUE);
   etk_widget_padding_set(tree, 5, 5, 5, 5);
   etk_box_append(ETK_BOX(vbox), tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   /* We first create the columns of the tree, and then we "build" the tree with etk_tree_build() */
   col1 = etk_tree_col_new(ETK_TREE(tree), "Column 1", 130, 0.0);
   etk_tree_col_model_add(col1, etk_tree_model_image_new());
   etk_tree_col_model_add(col1, etk_tree_model_text_new());
   
   col2 = etk_tree_col_new(ETK_TREE(tree), "Column 2", 60, 1.0);
   etk_tree_col_model_add(col2, etk_tree_model_double_new());
   
   col3 = etk_tree_col_new(ETK_TREE(tree), "Column 3", 60, 0.75);
   etk_tree_col_model_add(col3, etk_tree_model_int_new());
   
   col4 = etk_tree_col_new(ETK_TREE(tree), "Column 4", 60, 0.0);
   etk_tree_col_model_add(col4, etk_tree_model_image_new());
   
   col5 = etk_tree_col_new(ETK_TREE(tree), "Column 5", 60, 0.5);
   etk_tree_col_model_add(col5, etk_tree_model_checkbox_new());
   
   etk_tree_build(ETK_TREE(tree));
   
   /* Then we add the rows to the tree. etk_tree_freeze/thaw() is used to improve
    * the speed when you insert a lot of rows. It is not really important if you
    * insert only some thousands of rows (here, we insert 3000 rows) */
   etk_tree_freeze(ETK_TREE(tree));
   c_warn = etk_theme_color_get(ETK_COLOR_WARNING_FG);
   c_default2 = etk_theme_color_get(ETK_COLOR_DEFAULT2_FG);
   for (i = 0; i < 1000; i++)
   {
      sprintf(row_name, "<font color=#%.2X%.2X%.2X%.2X>Row %d</font>", 
	    c_warn.r, c_warn.g, c_warn.b, c_warn.a, (i * 3) + 1);
      stock_key = etk_stock_key_get(ETK_STOCK_PLACES_USER_HOME, ETK_STOCK_SMALL);
      row = etk_tree_row_append(ETK_TREE(tree), NULL,
         col1, etk_theme_icon_get(), stock_key, row_name,
         col2, 0.57,
         col3, 7,
         col4, PACKAGE_DATA_DIR "/images/1star.png", NULL,
         col5, ETK_FALSE,
         NULL);
      
      sprintf(row_name, "<font color=#%.2X%.2X%.2X%.2X>Row %d</font>",
	    c_default2.r, c_default2.g, c_default2.b, c_default2.a, (i * 3) + 2);
      stock_key = etk_stock_key_get(ETK_STOCK_PLACES_FOLDER, ETK_STOCK_SMALL);
      row = etk_tree_row_append(ETK_TREE(tree), row,
         col1, etk_theme_icon_get(), stock_key, row_name,
         col2, 20.0,
         col3, 19,
         col4, PACKAGE_DATA_DIR "/images/2stars.png", NULL,
         col5, ETK_TRUE,
         NULL);
      
      sprintf(row_name, "Row %d", (i * 3) + 3);
      stock_key = etk_stock_key_get(ETK_STOCK_TEXT_X_GENERIC, ETK_STOCK_SMALL);
      etk_tree_row_append(ETK_TREE(tree), row,
         col1, etk_theme_icon_get(), stock_key, row_name,
         col2, 300.0,
	 col3, 257,
         col4, PACKAGE_DATA_DIR "/images/3stars.png", NULL,
         col5, ETK_TRUE,
         NULL);
   }
   etk_tree_thaw(ETK_TREE(tree));
   
   /* Finally we create the statusbar used to display the events on the tree */
   statusbar = etk_statusbar_new();
   etk_box_append(ETK_BOX(vbox), statusbar, ETK_BOX_START, ETK_BOX_FILL, 0);
   
   etk_signal_connect("key_down", ETK_OBJECT(tree),
      ETK_CALLBACK(_etk_test_tree_key_down_cb), NULL);
   etk_signal_connect("row_clicked", ETK_OBJECT(tree),
      ETK_CALLBACK(_etk_test_tree_row_clicked_cb), statusbar);
   etk_signal_connect("cell_value_changed", ETK_OBJECT(col5),
      ETK_CALLBACK(_etk_test_tree_checkbox_toggled_cb), statusbar);
   
   etk_widget_show_all(win);
}

/* Called when a key is pressed while the tree is focused:
 * we use this to delete the selected rows if "DEL" is pressed */
static void _etk_test_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Tree *tree;
   Etk_Tree_Row *r;
   
   if (!(tree = ETK_TREE(object)))
      return;
   
   if (strcmp(event->keyname, "Delete") == 0)
   {
      /* We walk through all the rows of the tree, and we delete those which are selected.
       * Note that we can safely manipulate "r" with etk_tree_row_walk_next(), even if it
       * has been deleted with etk_tree_row_delete(), because etk_tree_row_delete() just
       * marks the row as deleted, but the row is not effectively deleted immediately */
      for (r = etk_tree_first_row_get(tree); r; r = etk_tree_row_walk_next(r, ETK_TRUE))
      {
         if (etk_tree_row_is_selected(r))
            etk_tree_row_delete(r);
      }
      
      etk_signal_stop();
   }
}

/* Called when a row of the tree is clicked: we display the clicked row in the statusbar */
/* TODO: sometimes it's a Etk_Event_Mouse_Up, sometimes a Etk_Event_Mouse_Down... */
static void _etk_test_tree_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Tree *tree;
   Etk_Statusbar *statusbar;
   char *row_name;
   char message[1024];
   
   if (!(tree = ETK_TREE(object)) || !(statusbar = ETK_STATUSBAR(data)) || !row || !event)
      return;
   
   /* We retrieve the name of the row: this information is located in the first column,
    * and it corresponds to the third param of the model (the two first params correspond
    * to the icon used, we ignore them here (that's why there are two NULL params)).
    * etk_tree_row_fields_get() has to be terminated by NULL */
   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, NULL, &row_name, NULL);
   
   sprintf(message, "Row \"%s\" clicked (%s)", row_name,
      (event->flags & ETK_MOUSE_TRIPLE_CLICK) ? "Triple" :
      ((event->flags & ETK_MOUSE_DOUBLE_CLICK) ? "Double" : "Single"));
   
   etk_statusbar_message_push(statusbar, message, 0);
}

/* Called when a checkbox of the tree is toggled: we display its new state in the statusbar */
static void _etk_test_tree_checkbox_toggled_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Tree *tree;
   Etk_Tree_Col *col;
   Etk_Statusbar *statusbar;
   Etk_Bool checked;
   char *row_name;
   char message[1024];
   
   if (!(col = ETK_TREE_COL(object)) || !(statusbar = ETK_STATUSBAR(data)) || !row)
      return;
   
   tree = etk_tree_col_tree_get(col);
   
   /* We retrieve the name of the row: see _etk_test_tree_row_clicked_cb() for more info*/
   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, NULL, &row_name, NULL);
   /* We then retrieve the new state of the checkbox */
   etk_tree_row_fields_get(row, col, &checked, NULL);
   
   sprintf(message, "Row \"%s\" has been %s", row_name, checked ? "checked" : "unchecked");
   etk_statusbar_message_push(statusbar, message, 0);
}
