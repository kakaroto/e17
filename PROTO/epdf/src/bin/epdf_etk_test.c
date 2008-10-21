#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Etk.h>

#include "etk_pdf.h"


static void _quit_cb(void *data);
static void _tree_fill (Etk_Pdf *pdf, Etk_Tree *tree, Etk_Tree_Col *col, Etk_Tree_Row *row, Ecore_List *items);
static void _change_page_cb (Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);

int
main (int argc, char *argv[])
{
  Etk_Widget    *window;
  Etk_Widget    *hpaned, *vpaned;
  Etk_Widget    *tree;
  Etk_Widget    *list;
  Etk_Widget    *pdf;
  Etk_Tree_Col  *col;
  Etk_Tree_Row  *row;
  Ecore_List    *index;
  Epdf_Document *document;
  int            page_count;
  int            i;

  if (argc == 1) {
    printf ("Usage: %s pdf_file\n", argv[0]);
    return -1;
  }

  etk_init (argc, argv);

  /* We open the pdf file */
  pdf = etk_pdf_new ();
  etk_pdf_file_set (ETK_PDF (pdf), argv[1]);
  document = ETK_PDF (pdf)->pdf_document;
  if (!document) {
    printf ("The file %s can't be opened\n", argv[1]);
    etk_shutdown ();
    return -1;
  }

  window = etk_window_new ();
  etk_window_title_set (ETK_WINDOW (window), "Etk Pdf Test Application");
  etk_signal_connect ("delete-event", ETK_OBJECT (window),
                      ETK_CALLBACK(_quit_cb), NULL);

  hpaned = etk_hpaned_new();
  etk_container_add (ETK_CONTAINER (window), hpaned);
  etk_widget_show (hpaned);

  vpaned = etk_vpaned_new();
  etk_paned_child1_set (ETK_PANED (hpaned), vpaned, 0);
  etk_widget_show (vpaned);

  index = etk_pdf_pdf_index_get (ETK_PDF (pdf));
  if (index) {
    Etk_Tree_Col *col;

    tree = etk_tree_new ();
    etk_tree_mode_set (ETK_TREE (tree), ETK_TREE_MODE_TREE);
    etk_tree_multiple_select_set (ETK_TREE (tree), ETK_FALSE);

    /* column */
    col = etk_tree_col_new (ETK_TREE (tree), "Index", 130, 0.0);
    etk_tree_col_model_add (col, etk_tree_model_text_new());

    etk_tree_build (ETK_TREE (tree));

    /* rows */
    _tree_fill (ETK_PDF (pdf), ETK_TREE (tree), col, NULL, index);
    epdf_index_delete (index);

    /* change page */
    etk_signal_connect ("row-clicked", ETK_OBJECT (tree),
                        ETK_CALLBACK(_change_page_cb), pdf);

    /* we attach and show */
    etk_paned_child1_set (ETK_PANED (vpaned), tree, 0);
    etk_widget_show (tree);
  }

  list = etk_tree_new ();
  etk_tree_headers_visible_set (ETK_TREE (list), ETK_FALSE);
  etk_tree_mode_set (ETK_TREE (list), ETK_TREE_MODE_LIST);
  etk_tree_multiple_select_set (ETK_TREE (list), ETK_FALSE);

  /* column */
  col = etk_tree_col_new (ETK_TREE (list), "", 60, 0.0);
  etk_tree_col_model_add (col, etk_tree_model_int_new());

  etk_tree_build (ETK_TREE (list));

  /* rows */
  page_count = epdf_document_page_count_get (ETK_PDF (pdf)->pdf_document);
  for (i = 0; i < page_count; i++) {
    int  *num;

    row = etk_tree_row_append (ETK_TREE (list), NULL, col, i + 1, NULL);
    num = (int *)malloc (sizeof (int));
    *num = i;
    etk_tree_row_data_set_full (row, num, free);
  }

  /* change page */
  etk_signal_connect ("row-clicked", ETK_OBJECT (list),
                      ETK_CALLBACK(_change_page_cb), pdf);

  /* we attach and show */
  etk_paned_child2_set (ETK_PANED (vpaned), list, 0);
  etk_widget_show (list);

  etk_pdf_scale_set (ETK_PDF (pdf), 0.5, 0.5);
  etk_paned_child2_set (ETK_PANED (hpaned), pdf, 1);
  etk_widget_show (pdf);

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
_tree_fill (Etk_Pdf *pdf, Etk_Tree *tree, Etk_Tree_Col *col, Etk_Tree_Row *row, Ecore_List *items)
{
  Etk_Tree_Row   *prow;
  Epdf_Index_Item *item;

  if (!items)
    return;

  ecore_list_first_goto (items);
  while ((item = ecore_list_next (items))) {
    char       *buf;
    Ecore_List *c;
    int        *num;

    buf = strdup (epdf_index_item_title_get (item));
    prow = etk_tree_row_append (tree, row, col, buf, NULL);

    num = (int *)malloc (sizeof (int));
    *num = epdf_index_item_page_get (etk_pdf_pdf_document_get (pdf), item);
    etk_tree_row_data_set_full (prow, num, free);
    free (buf);
    c = epdf_index_item_children_get (item);
    if (c) {
      _tree_fill (pdf, tree, col, prow, c);
    }
  }
}

static void
_change_page_cb (Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data)
{
  Etk_Tree *tree;
  Etk_Pdf  *pdf;
  int       row_number;

  tree = ETK_TREE (object);
  pdf = ETK_PDF (data);
  row_number = *(int *)etk_tree_row_data_get (row);
  etk_pdf_page_set (pdf, row_number);
}
