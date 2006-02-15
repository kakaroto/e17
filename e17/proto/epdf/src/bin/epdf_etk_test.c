#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Etk.h>

#include "etk_pdf.h"


static void _quit_cb(void *data);
static void _change_page_cb (Etk_Object *object, Etk_Tree_Row *row, void *data);

void
_tree_fill (Etk_Pdf *pdf, Etk_Tree *tree, Etk_Tree_Col *col, Etk_Tree_Row *row, Ecore_List *items)
{
  Etk_Tree_Row            *prow;
  Evas_Poppler_Index_Item *item;

  if (!items)
    return;
  
  ecore_list_goto_first (items);
  while ((item = ecore_list_next (items)))
    {
      char       *buf;
      Ecore_List *c;
      int         *num;

      buf = strdup (evas_poppler_index_item_title_get (item));
      if (!row)
        prow = etk_tree_append (tree, col, buf, NULL);
      else
        prow = etk_tree_append_to_row (row, col, buf, NULL);
      
      num = (int *)malloc (sizeof (int));
      *num = evas_poppler_index_item_page_get (etk_pdf_pdf_document_get (pdf), item);
      etk_tree_row_data_set (prow, num);
      free (buf);
      c = evas_poppler_index_item_children_get (item);
      if (c)
        {
          _tree_fill (pdf, tree, col, prow, c);
        }
    }
}

int
main (int argc, char *argv[])
{
  Etk_Widget   *window;
  Etk_Widget   *table;
  Etk_Widget   *tree;
  Etk_Widget   *list;
  Etk_Widget   *pdf;
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  Ecore_List   *index;
  Evas_Poppler_Document  *document;
  int           page_count;
  int           i;

  etk_init ();

  if (argc == 1)
    {
      printf ("Usage: %s pdf_file\n", argv[0]);
      etk_main_quit ();
      return -1;
    }

  /* We open the pdf file */
  pdf = etk_pdf_new ();
  etk_pdf_file_set (ETK_PDF (pdf), argv[1]);
  document = ETK_PDF (pdf)->pdf_document;
  if (!document)
    {
      printf ("The file %s can't be opened\n", argv[1]);
      etk_main_quit ();
      return -1;
    }

  index = etk_pdf_pdf_index_get (ETK_PDF (pdf));

  window = etk_window_new ();
  etk_window_title_set (ETK_WINDOW (window), "Etk Pdf Test Application");
  etk_signal_connect ("delete_event", ETK_OBJECT (window),
                      ETK_CALLBACK(_quit_cb), NULL);

  table = etk_table_new (2, 2, ETK_FALSE);
  etk_container_add (ETK_CONTAINER (window), table);
  etk_widget_show (table);
  
  if (index)
    {
      tree = etk_tree_new ();
      etk_tree_headers_visible_set (ETK_TREE (tree), ETK_FALSE);
      etk_widget_size_request_set (tree, 60, -1);
      etk_tree_mode_set (ETK_TREE(tree), ETK_TREE_MODE_TREE);
      col = etk_tree_col_new (ETK_TREE(tree), "",
                              etk_tree_model_text_new (ETK_TREE (tree)),
                              60);
      etk_tree_build (ETK_TREE (tree));
      etk_tree_freeze (ETK_TREE (tree));
      _tree_fill (ETK_PDF (pdf), ETK_TREE (tree), col, NULL, index);
      etk_tree_thaw (ETK_TREE (tree));
      evas_poppler_index_delete (index);
      etk_signal_connect ("row_selected", ETK_OBJECT (tree),
                          ETK_CALLBACK(_change_page_cb), pdf);
      etk_table_attach_defaults (ETK_TABLE (table), tree, 0, 0, 0, 0);
      etk_widget_show (tree);
    }

  list = etk_tree_new ();
  etk_tree_headers_visible_set (ETK_TREE (list), FALSE);
  etk_widget_size_request_set (list, 60, -1);
  if  (index)
    etk_table_attach_defaults (ETK_TABLE (table), list, 0, 0, 1, 1);
  else
    etk_table_attach_defaults (ETK_TABLE (table), list, 0, 0, 0, 1);
  etk_widget_show (list);

  etk_signal_connect ("row_selected", ETK_OBJECT (list),
                      ETK_CALLBACK(_change_page_cb), pdf);
  etk_tree_mode_set (ETK_TREE(list), ETK_TREE_MODE_LIST);
  col = etk_tree_col_new (ETK_TREE(list), "",
                          etk_tree_model_int_new (ETK_TREE (list)),
                          60);
  etk_tree_build (ETK_TREE (list));

  page_count = evas_poppler_document_page_count_get (ETK_PDF (pdf)->pdf_document);
  etk_tree_freeze (ETK_TREE (list));
  for (i = 0; i < page_count; i++)
    {
      int  *num;

      row = etk_tree_append (ETK_TREE (list), col, i + 1, NULL);
      num = (int *)malloc (sizeof (int));
      *num = i;
      etk_tree_row_data_set (row, num);
    }
  etk_tree_thaw (ETK_TREE (list));

  etk_table_attach_defaults (ETK_TABLE (table), pdf, 1, 1, 0, 1);
  etk_widget_show (pdf);
  
  etk_widget_show (window);

  etk_main ();

  return 0;
}

static void
_quit_cb(void *data)
{
  etk_main_quit ();
}

static void
_change_page_cb (Etk_Object *object, Etk_Tree_Row *row, void *data)
{
  Etk_Tree *tree;
  Etk_Pdf  *pdf;
  int       row_number;

  tree = ETK_TREE (object);
  pdf = ETK_PDF (data);
  row_number = *(int *)etk_tree_row_data_get (row);
  etk_pdf_page_set (pdf, row_number);
}
