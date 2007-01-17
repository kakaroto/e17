#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Etk.h>

#include "etk_dvi.h"


static void _quit_cb(void *data);
static void _change_page_cb (Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);

int
main (int argc, char *argv[])
{
  Etk_Widget    *window;
  Etk_Widget    *hbox;
  Etk_Widget    *list;
  Etk_Widget    *dvi;
  Etk_Tree_Col  *col;
  Etk_Tree_Row  *row;
  Edvi_Document *document;
  int            page_count;
  int            i;

  if (argc == 1) {
    printf ("Usage: %s dvi_file\n", argv[0]);
    return -1;
  }

  printf ("[DVI] version : %s\n", edvi_version ());
  if (!edvi_init (300, "cx", 4,
                  1.0, 1.0,
                  0, 255, 255, 255, 0, 0, 0)) {
    return -1;
  }

  etk_init (&argc, &argv);

  /* We open the dvi file */
  dvi = etk_dvi_new ();
  etk_dvi_file_set (ETK_DVI (dvi), argv[1]);
  document = ETK_DVI (dvi)->dvi_document;
  if (!document) {
    printf ("The file %s can't be opened\n", argv[1]);
    etk_shutdown ();
    edvi_shutdown ();
    return -1;
  }

  window = etk_window_new ();
  etk_window_title_set (ETK_WINDOW (window), "Etk Dvi Test Application");
  etk_signal_connect ("delete_event", ETK_OBJECT (window),
                      ETK_CALLBACK(_quit_cb), NULL);

  hbox = etk_hbox_new (ETK_FALSE, 6);
  etk_container_add (ETK_CONTAINER (window), hbox);
  etk_widget_show (hbox);

  list = etk_tree_new ();
  etk_tree_headers_visible_set (ETK_TREE (list), ETK_FALSE);
  etk_tree_mode_set (ETK_TREE (list), ETK_TREE_MODE_LIST);
  etk_tree_multiple_select_set (ETK_TREE (list), ETK_FALSE);

  /* column */
  col = etk_tree_col_new (ETK_TREE (list), "", 60, 0.0);
  etk_tree_col_model_add (col, etk_tree_model_int_new());

  etk_tree_build (ETK_TREE (list));

  /* rows */
  page_count = edvi_document_page_count_get (ETK_DVI (dvi)->dvi_document);
  for (i = 0; i < page_count; i++) {
    int  *num;

    row = etk_tree_row_append (ETK_TREE (list), NULL, col, i + 1, NULL);
    num = (int *)malloc (sizeof (int));
    *num = i;
    etk_tree_row_data_set_full (row, num, free);
  }

  /* change page */
  etk_signal_connect ("row_clicked", ETK_OBJECT (list),
                      ETK_CALLBACK(_change_page_cb), dvi);

  /* we attach and show */
  etk_box_append (ETK_BOX (hbox), list, ETK_BOX_START, ETK_BOX_NONE, 0);
  etk_widget_show (list);

  etk_box_append (ETK_BOX (hbox), dvi, ETK_BOX_START, ETK_BOX_NONE, 0);
  etk_widget_show (dvi);
  
  etk_widget_show (window);

  etk_main ();

  etk_shutdown ();
  edvi_shutdown ();

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
  Etk_Dvi  *dvi;
  int       row_number;

  tree = ETK_TREE (object);
  dvi = ETK_DVI (data);
  row_number = *(int *)etk_tree_row_data_get (row);
  etk_dvi_page_set (dvi, row_number);
}
