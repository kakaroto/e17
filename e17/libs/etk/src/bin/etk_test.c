#include "etk_test.h"

#define ETK_TEST_NUM_COLS 2

typedef enum _Etk_Test_Categories
{
   ETK_TEST_BASIC_WIDGET,
   ETK_TEST_ADVANCED_WIDGET,
   ETK_TEST_CONTAINER,
   ETK_TEST_DIALOG,
   ETK_TEST_MISC,
   ETK_TEST_NUM_CATEGORIES
} Etk_Test_Categories;

typedef struct _Etk_Test_Example
{
   char *name;
   void (*func)(void *data);
   Etk_Test_Categories category;
} Etk_Test_Example;

static void _etk_test_main_window();
static void _etk_test_main_quit_cb(void *data);

static char *_etk_test_categories[ETK_TEST_NUM_CATEGORIES] =
{ "Basic Widgets", "Advanced Widgets", "Containers", "Dialogs", "Misc" };

static Etk_Test_Example _etk_test_examples[] =
{
   {
      "Button",
      etk_test_button_window_create,
      ETK_TEST_BASIC_WIDGET
   },
   {
      "Entry",
      etk_test_entry_window_create,
      ETK_TEST_BASIC_WIDGET
   },
   {
      "Table",
      etk_test_table_window_create,
      ETK_TEST_CONTAINER
   },
   {
      "Image",
      etk_test_image_window_create,
      ETK_TEST_BASIC_WIDGET
   },
   {
      "Slider",
      etk_test_slider_window_create,
      ETK_TEST_BASIC_WIDGET
   },
   {
      "Canvas",
      etk_test_canvas_window_create,
      ETK_TEST_ADVANCED_WIDGET
   },
   {
      "Colorpicker",
      etk_test_colorpicker_window_create,
      ETK_TEST_DIALOG
   },
   {
      "Tree",
      etk_test_tree_window_create,
      ETK_TEST_ADVANCED_WIDGET
   },
   {
      "Paned",
      etk_test_paned_window_create,
      ETK_TEST_CONTAINER
   },
   {
      "Scrolled View",
      etk_test_scrolled_view_window_create,
      ETK_TEST_CONTAINER
   },
   {
      "Menu",
      etk_test_menu_window_create,
      ETK_TEST_ADVANCED_WIDGET
   },
   {
      "File Chooser",
      etk_test_filechooser_window_create,
      ETK_TEST_DIALOG
   },
   {
      "Notebook",
      etk_test_notebook_window_create,
      ETK_TEST_CONTAINER
   },
   {
      "Progress Bar",
      etk_test_progress_bar_window_create,
      ETK_TEST_BASIC_WIDGET
   },
   {
      "X Drag / Drop",
      etk_test_xdnd_window_create,
      ETK_TEST_MISC
   },
   {
      "Text View",
      etk_test_text_view_window_create,
      ETK_TEST_ADVANCED_WIDGET
   },
   {
      "Combobox",
      etk_test_combobox_window_create,
      ETK_TEST_ADVANCED_WIDGET
   },
   {
      "Iconbox",
      etk_test_iconbox_window_create,
      ETK_TEST_ADVANCED_WIDGET
   }
};
static int _etk_test_num_examples = sizeof(_etk_test_examples) / sizeof (_etk_test_examples[0]);

int main(int argc, char *argv[])
{
   if (!etk_init())
   {
      fprintf(stderr, "Could not init etk. Exiting...\n");
      return 1;
   };

   etk_tooltips_enable();
   _etk_test_main_window();
   etk_main();
   etk_shutdown();

   return 0;
}

/* Creates the main window */
static void _etk_test_main_window()
{
   Etk_Widget *win;
   Etk_Widget *vbox;
   Etk_Widget *tables[ETK_TEST_NUM_CATEGORIES];
   Etk_Widget *button;
   Etk_Widget *frame;
   int current_ids[ETK_TEST_NUM_CATEGORIES];
   int i, j, num_examples;

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Test Application"));
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("destroyed", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_main_quit_cb), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   for (i = 0; i < ETK_TEST_NUM_CATEGORIES; i++)
   {
      current_ids[i] = 0;
      for (num_examples = 0, j = 0; j < _etk_test_num_examples; j++)
      {
         if (_etk_test_examples[j].category == i)
            num_examples++;
      }
      
      frame = etk_frame_new(_(_etk_test_categories[i]));
      etk_box_pack_start(ETK_BOX(vbox), frame, ETK_TRUE, ETK_TRUE, 0);
      tables[i] = etk_table_new(ETK_TEST_NUM_COLS, (num_examples + ETK_TEST_NUM_COLS - 1) / ETK_TEST_NUM_COLS, ETK_TRUE);
      etk_container_add(ETK_CONTAINER(frame), tables[i]);
   }

   for (i = 0; i < _etk_test_num_examples; i++)
   {
      button = etk_button_new_with_label(_(_etk_test_examples[i].name));
      etk_signal_connect_swapped("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_examples[i].func), NULL);
      j = current_ids[_etk_test_examples[i].category];
      etk_table_attach_defaults(ETK_TABLE(tables[_etk_test_examples[i].category]), button,
         j % ETK_TEST_NUM_COLS, j % ETK_TEST_NUM_COLS, j / ETK_TEST_NUM_COLS, j / ETK_TEST_NUM_COLS);
      current_ids[_etk_test_examples[i].category]++;
   }
   
   etk_widget_show_all(win);
}

/* Called when the window is destroyed */
static void _etk_test_main_quit_cb(void *data)
{
   etk_main_quit();
}

