/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "etk_test.h"
#include <stdlib.h>
#include <string.h>
#include "config.h"

static Etk_Bool _etk_test_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_test_tree_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_test_tree_checkbox_toggled_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static int _etk_test_tree_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data);
static int _etk_test_tree_sort_button_cb(Etk_Object *object, void *data);
static int _etk_test_tree_insert_sorted_button_cb(Etk_Object *object, void *data);
static int _etk_test_tree_select_all_cb(Etk_Object *object, void *data);
static int _etk_test_tree_unselect_all_cb(Etk_Object *object, void *data);

/* Creates the window for the tree test */
void etk_test_tree_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *tree;
   Etk_Widget *button;
   Etk_Tree_Col *col1, *col2, *col3, *col4, *col5, *col6;
   Etk_Tree_Row *row;
   Etk_Widget *statusbar;
   Etk_Color c_warn;
   char row_name[128];
   const char *stock_key;
   int i, j;

   if (win)
   {
      etk_widget_show(ETK_WIDGET(win));
      return;
   }

   /* Create the window and the vbox where the widgets will be packed into */
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Tree Test");
   etk_window_resize(ETK_WINDOW(win), 640, 500);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   /* Create the tree widget */
   tree = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_TREE);
   etk_scrolled_view_extra_vmargin_set(etk_tree_scrolled_view_get(ETK_TREE(tree)), 250);
   etk_scrolled_view_drag_bouncy_set(etk_tree_scrolled_view_get(ETK_TREE(tree)), ETK_BOUNCY_STOPTOOBJECT);
   etk_scrolled_view_dragable_set(etk_tree_scrolled_view_get(ETK_TREE(tree)), ETK_TRUE);
   etk_tree_multiple_select_set(ETK_TREE(tree), ETK_TRUE);
   etk_widget_padding_set(tree, 5, 5, 5, 5);
   etk_box_append(ETK_BOX(vbox), tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   /* We first create the columns of the tree, and then we "build" the tree with etk_tree_build() */
   col1 = etk_tree_col_new(ETK_TREE(tree), "Column 1", 130, 0.0);
   etk_tree_col_model_add(col1, etk_tree_model_image_new());
   etk_tree_col_model_add(col1, etk_tree_model_text_new());

   col2 = etk_tree_col_new(ETK_TREE(tree), "Column 2", 60, 1.0);
   etk_tree_col_model_add(col2, etk_tree_model_double_new());

   col3 = etk_tree_col_new(ETK_TREE(tree), "Column 3", 80, 0.75);
   etk_tree_col_model_add(col3, etk_tree_model_int_new());

   col4 = etk_tree_col_new(ETK_TREE(tree), "Column 4", 60, 0.0);
   etk_tree_col_model_add(col4, etk_tree_model_image_new());

   col5 = etk_tree_col_new(ETK_TREE(tree), "Column 5", 60, 0.5);
   etk_tree_col_model_add(col5, etk_tree_model_checkbox_new());

   col6 = etk_tree_col_new(ETK_TREE(tree), "Column 6", 60, 0.0);
   etk_tree_col_model_add(col6, etk_tree_model_progress_bar_new());

   etk_tree_build(ETK_TREE(tree));

   /* Then we add the rows to the tree. etk_tree_freeze/thaw() is used to improve
    * the speed when you insert a lot of rows. It is not really important if you
    * insert only some thousands of rows (here, we insert 3000 rows) */
   etk_tree_freeze(ETK_TREE(tree));
   etk_theme_color_get(NULL, ETK_COLOR_WARNING_FG, &c_warn.r, &c_warn.g, &c_warn.b, &c_warn.a);
   for (i = 0; i < 1000; i++)
   {
      sprintf(row_name, "Row %d", (i * 3) + 1);
      stock_key = etk_stock_key_get(ETK_STOCK_PLACES_USER_HOME, ETK_STOCK_SMALL);
      row = etk_tree_row_append(ETK_TREE(tree), NULL,
         col1, etk_theme_icon_path_get(), stock_key, row_name,
         col2, 0.57,
         col3, rand(),
         col4, PACKAGE_DATA_DIR "/images/1star.png", NULL,
         col5, ETK_FALSE,
         col6, 0.57, "57 %",
         NULL);

      for (j = 0; j < 3; j++)
      {
        Etk_Tree_Row *current;
        sprintf(row_name, "Row %d", (i * 3) + 2);
        stock_key = etk_stock_key_get(ETK_STOCK_PLACES_FOLDER, ETK_STOCK_SMALL);
        current = etk_tree_row_append(ETK_TREE(tree), row,
                                      col1, etk_theme_icon_path_get(), stock_key, row_name,
                                      col2, 20.0,
                                      col3, rand(),
                                      col4, PACKAGE_DATA_DIR "/images/2stars.png", NULL,
                                      col5, ETK_TRUE,
                                      col6, 0.20, "20 %",
                                      NULL);
        if (j == 2) row = current;
      }

      sprintf(row_name, "<font color=#%.2X%.2X%.2X%.2X>Row %d</font>",
            c_warn.r, c_warn.g, c_warn.b, c_warn.a, (i * 3) + 3);
      stock_key = etk_stock_key_get(ETK_STOCK_TEXT_X_GENERIC, ETK_STOCK_SMALL);
      etk_tree_row_append(ETK_TREE(tree), row,
         col1, etk_theme_icon_path_get(), stock_key, row_name,
         col2, 300.0,
         col3, 257,
         col4, PACKAGE_DATA_DIR "/images/3stars.png", NULL,
         col5, ETK_TRUE,
         NULL);
   }
   etk_tree_thaw(ETK_TREE(tree));
   etk_tree_col_sort_set(col3, _etk_test_tree_compare_cb, NULL);

   hbox = etk_hbox_new(ETK_TRUE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_FILL, 0);

   button = etk_button_new_with_label("Sort the tree");
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button),
      ETK_CALLBACK(_etk_test_tree_sort_button_cb), col3);

   button = etk_button_new_with_label("Insert a sorted row");
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button),
      ETK_CALLBACK(_etk_test_tree_insert_sorted_button_cb), ETK_TREE(tree));
   
   button = etk_button_new_with_label("Select all");
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button),
      ETK_CALLBACK(_etk_test_tree_select_all_cb), ETK_TREE(tree));

   button = etk_button_new_with_label("Unselect all");
   etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_EXPAND, 0);
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button),
      ETK_CALLBACK(_etk_test_tree_unselect_all_cb), ETK_TREE(tree));

   /* Finally we create the statusbar used to display the events on the tree */
   statusbar = etk_statusbar_new();
   etk_box_append(ETK_BOX(vbox), statusbar, ETK_BOX_START, ETK_BOX_FILL, 0);

   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(tree),
      ETK_CALLBACK(_etk_test_tree_key_down_cb), NULL);
   etk_signal_connect_by_code(ETK_TREE_ROW_CLICKED_SIGNAL, ETK_OBJECT(tree),
      ETK_CALLBACK(_etk_test_tree_row_clicked_cb), statusbar);
   etk_signal_connect_by_code(ETK_TREE_COL_CELL_VALUE_CHANGED_SIGNAL, ETK_OBJECT(col5),
      ETK_CALLBACK(_etk_test_tree_checkbox_toggled_cb), statusbar);

   etk_widget_show_all(win);
}

/* Called when a key is pressed while the tree is focused:
 * we use this to delete the selected rows if "DEL" is pressed */
static Etk_Bool _etk_test_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Tree *tree;
   Etk_Tree_Row *r;

   if (!(tree = ETK_TREE(object)))
      return ETK_FALSE;

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

      return ETK_TRUE;
   }

   return ETK_FALSE;
}

/* Called when a row of the tree is clicked: we display the clicked row in the statusbar */
/* TODO: sometimes it's a Etk_Event_Mouse_Up, sometimes a Etk_Event_Mouse_Down... */
static Etk_Bool _etk_test_tree_row_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Tree *tree;
   Etk_Statusbar *statusbar;
   char *row_name;
   char message[1024];

   if (!(tree = ETK_TREE(object)) || !(statusbar = ETK_STATUSBAR(data)) || !row || !event)
      return ETK_TRUE;

   /* We retrieve the name of the row: this information is located in the first column,
    * and it corresponds to the third param of the model (the two first params correspond
    * to the icon used, we ignore them here (that's why there are two NULL params)).
    * etk_tree_row_fields_get() has to be terminated by NULL */
   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, NULL, &row_name, NULL);

   sprintf(message, "Row \"%s\" clicked (%s)", row_name,
      (event->flags & ETK_MOUSE_TRIPLE_CLICK) ? "Triple" :
      ((event->flags & ETK_MOUSE_DOUBLE_CLICK) ? "Double" : "Single"));

   etk_statusbar_message_push(statusbar, message, 0);
   return ETK_TRUE;
}

/* Called when a checkbox of the tree is toggled: we display its new state in the statusbar */
static Etk_Bool _etk_test_tree_checkbox_toggled_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Tree *tree;
   Etk_Tree_Col *col;
   Etk_Statusbar *statusbar;
   Etk_Bool checked;
   char *row_name;
   char message[1024];

   if (!(col = ETK_TREE_COL(object)) || !(statusbar = ETK_STATUSBAR(data)) || !row)
      return ETK_TRUE;

   tree = etk_tree_col_tree_get(col);

   /* We retrieve the name of the row: see _etk_test_tree_row_clicked_cb() for more info*/
   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, NULL, &row_name, NULL);
   /* We then retrieve the new state of the checkbox */
   etk_tree_row_fields_get(row, col, &checked, NULL);

   sprintf(message, "Row \"%s\" has been %s", row_name, checked ? "checked" : "unchecked");
   etk_statusbar_message_push(statusbar, message, 0);

   return ETK_TRUE;
}

/* Used to sort the first column of the tree... */
static int _etk_test_tree_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data)
{
   int str1, str2;

   etk_tree_row_fields_get(row1, col, &str1, NULL);
   etk_tree_row_fields_get(row2, col, &str2, NULL);
   if (str1 < str2)
     return -1;
   else if (str1 > str2)
     return 1;
   else
     return 0;
}

/* Sort the tree with the third column */
static int _etk_test_tree_sort_button_cb(Etk_Object *object, void *data)
{
   Etk_Tree_Col *col = data;
   etk_tree_col_sort(col, !(col->tree->sorted_asc));
   return 0;
}

/* Insert a row sorted in the tree */
static int _etk_test_tree_insert_sorted_button_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree = data;
   Etk_Tree_Row *row;
   Etk_Tree_Col *col1, *col2, *col3, *col4, *col5, *col6;
   const char *stock_key;

   col1 = etk_tree_nth_col_get(tree, 0);
   col2 = etk_tree_nth_col_get(tree, 1);
   col3 = etk_tree_nth_col_get(tree, 2);
   col4 = etk_tree_nth_col_get(tree, 3);
   col5 = etk_tree_nth_col_get(tree, 4);
   col6 = etk_tree_nth_col_get(tree, 5);
   stock_key = etk_stock_key_get(ETK_STOCK_PLACES_USER_HOME, ETK_STOCK_SMALL);
   row = etk_tree_row_insert_sorted(ETK_TREE(tree), NULL,
                                    col1, etk_theme_icon_path_get(), stock_key, "Sorted Row",
                                    col2, 0.42,
                                    col3, rand(),
                                    col4, PACKAGE_DATA_DIR "/images/1star.png", NULL,
                                    col5, ETK_TRUE,
                                    col6, 0.42, "42 %",
                                    NULL);
   etk_tree_row_select(row);
   etk_tree_row_scroll_to(row, ETK_TRUE);
   return 0;
}

static int _etk_test_tree_select_all_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree = data;
   
   etk_tree_select_all(ETK_TREE(tree));
   
   return 0;
}

static int _etk_test_tree_unselect_all_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree = data;
   
   etk_tree_unselect_all(ETK_TREE(tree));
   
   return 0;
}
