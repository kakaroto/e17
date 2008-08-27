#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Etk.h>

#include "etk_ps.h"


static void _quit_cb(void *data);
static void _change_page_cb (Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);

int
main (int argc, char *argv[])
{
  Etk_Widget   *window;
  Etk_Widget   *table;
  Etk_Widget   *list;
  Etk_Widget   *ps;
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  Eps_Document *document;
  int           page_count;
  int           i;

  if (argc == 1) {
    printf ("Usage: %s ps_file\n", argv[0]);
    return -1;
  }

  etk_init (argc, argv);

  /* We open the ps file */
  ps = etk_ps_new ();
  etk_ps_file_set (ETK_PS (ps), argv[1]);
  document = ETK_PS (ps)->ps_document;
  if (!document) {
    printf ("The file %s can't be opened\n", argv[1]);
    etk_shutdown ();
    return -1;
  }

  window = etk_window_new ();
  etk_window_title_set (ETK_WINDOW (window), "Etk Ps Test Application");
  etk_signal_connect ("delete-event", ETK_OBJECT (window),
                      ETK_CALLBACK(_quit_cb), NULL);

  table = etk_table_new (2, 2, ETK_FALSE);
  etk_container_add (ETK_CONTAINER (window), table);
  etk_widget_show (table);

  list = etk_tree_new ();
  etk_tree_headers_visible_set (ETK_TREE (list), ETK_FALSE);
  etk_tree_mode_set (ETK_TREE (list), ETK_TREE_MODE_LIST);
  etk_tree_multiple_select_set (ETK_TREE (list), ETK_FALSE);

  /* column */
  col = etk_tree_col_new (ETK_TREE (list), "", 60, 0.0);
  etk_tree_col_model_add (col, etk_tree_model_int_new());

  etk_tree_build (ETK_TREE (list));

  /* rows */
  page_count = eps_document_page_count_get (ETK_PS (ps)->ps_document);
  for (i = 0; i < page_count; i++) {
    int  *num;

    row = etk_tree_row_append (ETK_TREE (list), NULL, col, i + 1, NULL);
    num = (int *)malloc (sizeof (int));
    *num = i;
    etk_tree_row_data_set_full (row, num, free);
  }

  /* change page */
  etk_signal_connect ("row-clicked", ETK_OBJECT (list),
                      ETK_CALLBACK(_change_page_cb), ps);

  /* we attach and show */
  etk_table_attach_default (ETK_TABLE (table), list, 0, 0, 0, 1);
  etk_widget_show (list);

  etk_ps_scale_set (ETK_PS (ps), 0.5, 0.5);
  etk_table_attach (ETK_TABLE (table), ps,
                    1, 1, 0, 1,
                    0, 0, ETK_TABLE_NONE);
  etk_widget_show (ps);

  etk_widget_show (window);

  etk_main ();

  etk_shutdown ();

  return 0;
}

static void
_quit_cb(void *data)
{
  etk_main_quit ();
}

static void
_change_page_cb (Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data)
{
  Etk_Tree *tree;
  Etk_Ps   *ps;
  int       row_number;

  tree = ETK_TREE (object);
  ps = ETK_PS (data);
  row_number = *(int *)etk_tree_row_data_get (row);
  etk_ps_page_set (ps, row_number);
}
