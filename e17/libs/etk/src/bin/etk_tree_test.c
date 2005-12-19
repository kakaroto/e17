#include "etk_test.h"
#include <string.h>
#include "config.h"

/* TODO: Etk_Theme */
#define ETK_DEFAULT_ICON_SET_FILE PACKAGE_DATA_DIR "/stock_icons/default.edj"

static void _etk_test_tree_add_items(Etk_Tree *tree, int n);
static void _etk_test_tree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _etk_test_tree_row_unselected(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _etk_test_tree_clear_list_cb(Etk_Object *object, void *data);
static void _etk_test_tree_add_5_cb(Etk_Object *object, void *data);
static void _etk_test_tree_add_50_cb(Etk_Object *object, void *data);
static void _etk_test_tree_add_500_cb(Etk_Object *object, void *data);
static void _etk_test_tree_add_5000_cb(Etk_Object *object, void *data);

/* Creates the window for the tree test */
void etk_test_tree_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *tree;
   Etk_Tree_Row *row;
   Etk_Tree_Col *col1, *col2, *col3;
   Etk_Widget *table;
   Etk_Widget *label;
   Etk_Widget *frame;
   Etk_Widget *button;
   Etk_Widget *hbox;
   int i;

   if (win)
   {
      etk_widget_show(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Tree Test"));
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
	
   table = etk_table_new(2, 3, FALSE);
   etk_container_add(ETK_CONTAINER(win), table);

   /* The tree: */
   label = etk_label_new(_("<h1>Tree:</h1>"));
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL);

   tree = etk_tree_new();
   etk_widget_size_request_set(tree, 320, 400);
   etk_table_attach_defaults(ETK_TABLE(table), tree, 0, 0, 1, 1);

   etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_TREE);
   etk_tree_multiple_select_set(ETK_TREE(tree), TRUE);
   col1 = etk_tree_col_new(ETK_TREE(tree), _("Column 1"), etk_tree_model_icon_text_new(ETK_TREE(tree), ETK_TREE_FROM_EDJE), 60);
   etk_tree_col_expand_set(col1, TRUE);
   col2 = etk_tree_col_new(ETK_TREE(tree), _("Column 2"), etk_tree_model_double_new(ETK_TREE(tree)), 60);
   col3 = etk_tree_col_new(ETK_TREE(tree), _("Column 3"), etk_tree_model_image_new(ETK_TREE(tree), ETK_TREE_FROM_FILE), 60);
   etk_tree_build(ETK_TREE(tree));

   etk_tree_freeze(ETK_TREE(tree));
   for (i = 0; i < 1000; i++)
   {
      row = etk_tree_append(ETK_TREE(tree), col1, ETK_DEFAULT_ICON_SET_FILE, "mimetypes/x-directory-normal-home", _("Row1"),
         col2, 10.0, col3, PACKAGE_DATA_DIR "/images/1star.png", NULL);
      row = etk_tree_append_to_row(row, col1, ETK_DEFAULT_ICON_SET_FILE, "mimetypes/x-directory-normal", _("Row2"),
         col2, 20.0, col3, PACKAGE_DATA_DIR "/images/2stars.png", NULL);
      etk_tree_append_to_row(row, col1, ETK_DEFAULT_ICON_SET_FILE, "mimetypes/text-x-generic", _("Row3"),
         col2, 30.0, col3, PACKAGE_DATA_DIR "/images/3stars.png", NULL);
   }
   etk_tree_thaw(ETK_TREE(tree));

   /* The list: */
   label = etk_label_new(_("<h1>List:</h1>"));
   etk_table_attach(ETK_TABLE(table), label, 1, 1, 0, 0, 0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL);

   tree = etk_tree_new();
   etk_widget_size_request_set(tree, 320, 400);
   etk_table_attach_defaults(ETK_TABLE(table), tree, 1, 1, 1, 1);

   etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
   etk_tree_multiple_select_set(ETK_TREE(tree), TRUE);
   col1 = etk_tree_col_new(ETK_TREE(tree), _("Column 1"), etk_tree_model_icon_text_new(ETK_TREE(tree), ETK_TREE_FROM_FILE), 90);
   col2 = etk_tree_col_new(ETK_TREE(tree), _("Column 2"), etk_tree_model_int_new(ETK_TREE(tree)), 90);
   col3 = etk_tree_col_new(ETK_TREE(tree), _("Column 3"), etk_tree_model_image_new(ETK_TREE(tree), ETK_TREE_FROM_FILE), 90);
   etk_tree_build(ETK_TREE(tree));

   _etk_test_tree_add_items(ETK_TREE(tree), 5000);
   etk_signal_connect("row_selected", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree_row_selected), NULL);
   etk_signal_connect("row_unselected", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree_row_unselected), NULL);

   /* Frame */
   frame = etk_frame_new(_("List Actions"));
   etk_table_attach(ETK_TABLE(table), frame, 0, 1, 2, 2, 0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL);
   hbox = etk_hbox_new(TRUE, 10);
   etk_container_add(ETK_CONTAINER(frame), hbox);

   button = etk_button_new_with_label(_("Clear"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_clear_list_cb), tree);
   etk_box_pack_start(ETK_BOX(hbox), button, TRUE, TRUE, 0);

   button = etk_button_new_with_label(_("Add 5 rows"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_add_5_cb), tree);
   etk_box_pack_start(ETK_BOX(hbox), button, TRUE, TRUE, 0);

   button = etk_button_new_with_label(_("Add 50 rows"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_add_50_cb), tree);
   etk_box_pack_start(ETK_BOX(hbox), button, TRUE, TRUE, 0);

   button = etk_button_new_with_label(_("Add 500 rows"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_add_500_cb), tree);
   etk_box_pack_start(ETK_BOX(hbox), button, TRUE, TRUE, 0);

   button = etk_button_new_with_label(_("Add 5000 rows"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_add_5000_cb), tree);
   etk_box_pack_start(ETK_BOX(hbox), button, TRUE, TRUE, 0);

   etk_widget_show_all(win);
}

/* Adds n items to the tree */
static void _etk_test_tree_add_items(Etk_Tree *tree, int n)
{
   int i;
   Etk_Tree_Col *col1, *col2, *col3;
   char row_name[256];
   char star_path[256];

   if (!tree)
      return;

   col1 = etk_tree_nth_col_get(tree, 0);
   col2 = etk_tree_nth_col_get(tree, 1);
   col3 = etk_tree_nth_col_get(tree, 2);

   etk_tree_freeze(tree);
   for (i = 0; i < n; i++)
   {
      snprintf(row_name, 256, "Row%d", i);
      if (1/*i % 3 == 0*/)
         strncpy(star_path, PACKAGE_DATA_DIR "/images/1star.png", 256);
      else if (i % 3 == 1)
         strncpy(star_path, PACKAGE_DATA_DIR "/images/2stars.png", 256);
      else
         strncpy(star_path, PACKAGE_DATA_DIR "/images/3stars.png", 256);
      etk_tree_append(ETK_TREE(tree), col1, PACKAGE_DATA_DIR "/images/1star.png", row_name, col2, i, col3, star_path, NULL);
   }
   etk_tree_thaw(tree);
}

/* Called when a row is selected */
static void _etk_test_tree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   Etk_Tree *tree;
   char *col1_string, *col3_path;
   int col2_value;

   tree = ETK_TREE(object);
   printf(_("Row selected %p %p\n"), object, row);
   etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, &col1_string, etk_tree_nth_col_get(tree, 1), &col2_value, etk_tree_nth_col_get(tree, 2), &col3_path, NULL);
   printf("\"%s\" %d %s\n\n", col1_string, col2_value, col3_path);
}

/* Called when a row is unselected */
static void _etk_test_tree_row_unselected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
   printf(_("Row unselected %p %p\n"), object, row);
}

/* Called when the "Clear" button is clicked */
static void _etk_test_tree_clear_list_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree;

   if (!(tree = ETK_TREE(data)))
      return;
   etk_tree_clear(tree);
}

/* Called when the "Add 5 items" button is clicked */
static void _etk_test_tree_add_5_cb(Etk_Object *object, void *data)
{
   _etk_test_tree_add_items(ETK_TREE(data), 5);
}

/* Called when the "Add 50 items" button is clicked */
static void _etk_test_tree_add_50_cb(Etk_Object *object, void *data)
{
   _etk_test_tree_add_items(ETK_TREE(data), 50);
}

/* Called when the "Add 500 items" button is clicked */
static void _etk_test_tree_add_500_cb(Etk_Object *object, void *data)
{
   _etk_test_tree_add_items(ETK_TREE(data), 500);
}

/* Called when the "Add 5000 items" button is clicked */
static void _etk_test_tree_add_5000_cb(Etk_Object *object, void *data)
{
   _etk_test_tree_add_items(ETK_TREE(data), 5000);
}
