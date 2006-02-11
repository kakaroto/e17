#include "etk_test.h"

static void _etk_test_main_window();
static void _etk_test_main_quit_cb(void *data);

static Etk_Test_Set tests[] =
{
   {
      "Button",
      etk_test_button_window_create
   },
   {
      "Entry",
      etk_test_entry_window_create
   },
   {
      "Table",
      etk_test_table_window_create
   },
   {
      "Image",
      etk_test_image_window_create
   },
   {
      "Slider",
      etk_test_slider_window_create
   },
   {
      "Canvas",
      etk_test_canvas_window_create
   },
   {
      "Colorpicker",
      etk_test_colorpicker_window_create
   },
   {
      "Tree",
      etk_test_tree_window_create
   },
   {
      "Paned",
      etk_test_paned_window_create
   },
   {
      "Scrolled View",
      etk_test_scrolled_view_window_create
   },
   {
      "Menu",
      etk_test_menu_window_create
   },
   {
      "File Chooser",
      etk_test_filechooser_window_create
   },
   {
      "Notebook",
      etk_test_notebook_window_create
   },
   {
      "Progress Bar",
      etk_test_progress_bar_window_create
   },
   {
      "Spin Button",
      etk_test_spin_button_window_create
   },
   {
      "X Drag / Drop",
      etk_test_xdnd_window_create
   }
};
static int num_tests = sizeof(tests) / sizeof (tests[0]);

int main(int argc, char *argv[])
{
   if (!etk_init())
   {
      fprintf(stderr, "Could not init etk. Exiting...\n");
      return 0;
   };

   _etk_test_main_window();
   etk_main();
   etk_shutdown();

   return 1;
}

/* Creates the main window */
static void _etk_test_main_window()
{
   Etk_Widget *win;
   Etk_Widget *table;
   Etk_Widget *button;
   int i;

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Test Application"));
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("destroyed", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_main_quit_cb), NULL);
   
   table = etk_table_new((num_tests + 4) / 5, 5, ETK_TRUE);
   etk_container_add(ETK_CONTAINER(win), table);

   for (i = 0; i < num_tests; i++)
   {
      button = etk_button_new_with_label(_(tests[i].name));
      etk_signal_connect_swapped("clicked", ETK_OBJECT(button), ETK_CALLBACK(tests[i].func), NULL);
      etk_table_attach_defaults(ETK_TABLE(table), button, i / 5, i / 5, i % 5, i % 5);
   }
   etk_widget_show_all(win);
}

/* Called when the window is destroyed */
static void _etk_test_main_quit_cb(void *data)
{
   etk_main_quit();
}

