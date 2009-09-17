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

#define NUM_COLS 2

typedef enum Etk_Test_Categories
{
   BASIC_WIDGET,
   ADVANCED_WIDGET,
   CONTAINER,
   DIALOG,
   MISC,
   NUM_CATEGORIES
} Etk_Test_Categories;

typedef struct Etk_Test_App
{
   char *name;
   void (*func)(void *data);
   Etk_Test_Categories category;
} Etk_Test_App;

static void _main_quit_cb(void *data);
static void _main_window_create(void);

/* The different categories of test-apps */
static char *_categories[NUM_CATEGORIES] =
{ "Basic Widgets", "Advanced Widgets", "Containers", "Dialogs", "Misc" };

/* The different test-apps */
static Etk_Test_App _test_apps[] =
{
   {
      "Button",
      etk_test_button_window_create,
      BASIC_WIDGET
   },
   {
      "Entry",
      etk_test_entry_window_create,
      BASIC_WIDGET
   },
   {
      "Table",
      etk_test_table_window_create,
      CONTAINER
   },
   {
      "Image",
      etk_test_image_window_create,
      BASIC_WIDGET
   },
   {
      "Slider",
      etk_test_slider_window_create,
      BASIC_WIDGET
   },
   {
      "Canvas",
      etk_test_canvas_window_create,
      ADVANCED_WIDGET
   },
   {
      "Colorpicker",
      etk_test_colorpicker_window_create,
      DIALOG
   },
   {
      "Tree",
      etk_test_tree_window_create,
      ADVANCED_WIDGET
   },
   {
      "Paned",
      etk_test_paned_window_create,
      CONTAINER
   },
   {
      "Scrolled View",
      etk_test_scrolled_view_window_create,
      CONTAINER
   },
   {
      "Menu",
      etk_test_menu_window_create,
      ADVANCED_WIDGET
   },
   {
      "File Chooser",
      etk_test_filechooser_window_create,
      DIALOG
   },
   {
      "Mdi Window",
      etk_test_mdi_window_window_create,
      DIALOG
   },
   {
      "Notebook",
      etk_test_notebook_window_create,
      CONTAINER
   },
   {
      "Progress Bar",
      etk_test_progress_bar_window_create,
      BASIC_WIDGET
   },
   {
      "Stock Icons",
      etk_test_stock_window_create,
      BASIC_WIDGET
   },
   {
      "Drag and Drop",
      etk_test_dnd_window_create,
      MISC
   },
   {
      "Combobox",
      etk_test_combobox_window_create,
      ADVANCED_WIDGET
   },
   {
      "Iconbox",
      etk_test_iconbox_window_create,
      ADVANCED_WIDGET
   },
   {
      "Text View",
      etk_test_text_view_window_create,
      ADVANCED_WIDGET
   },
   {
      "Embed Widget",
      etk_test_embed_window_create,
      CONTAINER
   },
   {
      "Shadow",
      etk_test_shadow_window_create,
      MISC
   },
   {
      "Evas Objects",
      etk_test_evas_object_window_create,
      MISC
   }
};
static int _num_test_apps = sizeof(_test_apps) / sizeof (_test_apps[0]);


/**************************
 *
 * Main
 *
 **************************/

int main(int argc, char *argv[])
{
   if (!etk_init(argc, argv))
   {
      fprintf(stderr, "Could not init Etk. Exiting...\n");
      return 1;
   };
   etk_tooltips_enable();
   _main_window_create();
   etk_main();
   etk_shutdown();

   return 0;
}

/**************************
 *
 * Creation of the main window
 *
 **************************/

/* Creates the main window */
static void _main_window_create(void)
{
   Etk_Widget *win;
   Etk_Widget *vbox;
   Etk_Widget *tables[NUM_CATEGORIES];
   Etk_Widget *button;
   Etk_Widget *frame;
   int current_ids[NUM_CATEGORIES];
   int i, j, num_examples;

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Test Application");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect_swapped_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(win),
				   ETK_CALLBACK(_main_quit_cb), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   /* Create a frame and a table for each category */
   for (i = 0; i < NUM_CATEGORIES; i++)
   {
      current_ids[i] = 0;
      for (num_examples = 0, j = 0; j < _num_test_apps; j++)
      {
         if (_test_apps[j].category == i)
            num_examples++;
      }

      frame = etk_frame_new(_categories[i]);
      etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
      tables[i] = etk_table_new(NUM_COLS, (num_examples + NUM_COLS - 1) / NUM_COLS, ETK_TABLE_HOMOGENEOUS);
      etk_container_add(ETK_CONTAINER(frame), tables[i]);
   }

   /* For each test-apps, we add a button in the corresponding table */
   for (i = 0; i < _num_test_apps; i++)
   {
      button = etk_button_new_with_label(_test_apps[i].name);
      etk_signal_connect_swapped_by_code(ETK_BUTTON_CLICKED_SIGNAL,
				      ETK_OBJECT(button),
				      ETK_CALLBACK(_test_apps[i].func), NULL);
      j = current_ids[_test_apps[i].category]++;
      etk_table_attach_default(ETK_TABLE(tables[_test_apps[i].category]), button,
            j % NUM_COLS, j % NUM_COLS, j / NUM_COLS, j / NUM_COLS);
   }

   etk_widget_show_all(win);
}

/**************************
 *
 * Callbacks
 *
 **************************/

/* Called when the main window is destroyed */
static void _main_quit_cb(void *data)
{
   etk_main_quit();
}

