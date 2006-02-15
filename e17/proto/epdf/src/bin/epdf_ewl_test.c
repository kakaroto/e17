#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ewl.h>

#include "ewl_pdf.h"


static void _quit_cb (Ewl_Widget * w, void *ev_data, void *user_data);
static void _change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data);

void
_tree_fill (Ewl_Widget *pdf, Ewl_Tree *tree, Ewl_Row *row, Ecore_List *items)
{
  Ewl_Widget              *prow;
  Evas_Poppler_Index_Item *item;

  if (!items)
    return;
  
  ecore_list_goto_first (items);
  while ((item = ecore_list_next (items)))
    {
      int         page;
      int        *num;
      char       *buf;
      Ecore_List *c;

      buf = strdup (evas_poppler_index_item_title_get (item));
      prow = ewl_tree_text_row_add (tree, row,
                                    &buf);
      page = evas_poppler_index_item_page_get (ewl_pdf_pdf_document_get (EWL_PDF (pdf)), item);
      if (page >= 0)
        {
          num = (int *)malloc (sizeof (int));
          *num = page;
          ewl_widget_data_set (prow, "row-number", num);
          ewl_callback_append (EWL_WIDGET (prow),
                               EWL_CALLBACK_CLICKED,
                               EWL_CALLBACK_FUNCTION (_change_page_cb),
                               pdf);
        }
      free (buf);
      c = evas_poppler_index_item_children_get (item);
      if (c)
        {
          _tree_fill (pdf, tree, EWL_ROW (prow), c);
        }
    }
}

int
main (int argc, char *argv[])
{
  Ewl_Widget *window;
  Ewl_Widget *table;
  Ewl_Widget *list;
  Ewl_Widget *tree;
  Ewl_Widget *pdf;
  Ewl_Widget *row;
  Ecore_List *index;
  Evas_Poppler_Document  *document;
  int         page_count;
  int         i;

  ewl_init (&argc, (char **)argv);

  if (argc == 1)
    {
      printf ("Usage: %s pdf_file\n", argv[0]);
      ewl_main_quit ();
      return -1;
    }

  /* We open the pdf file */
  pdf = ewl_pdf_new ();
  ewl_pdf_file_set (EWL_PDF (pdf), argv[1]);
  document = EWL_PDF (pdf)->pdf_document;
  if (!document)
    {
      printf ("The file %s can't be opened\n", argv[1]);
      ewl_main_quit ();
      return -1;
    }

  index = evas_poppler_index_new (document);

  window = ewl_window_new ();
  ewl_window_title_set (EWL_WINDOW (window), "Ewl Pdf Test Application");
  ewl_callback_append (window, EWL_CALLBACK_DELETE_WINDOW,
                       _quit_cb, NULL);

  table = ewl_table_new (2, 2, NULL);
  ewl_table_homogeneous_set (EWL_TABLE (table), FALSE);
  ewl_container_child_append (EWL_CONTAINER (window), table);
  ewl_widget_show (table);

  if (index)
    {
      tree = ewl_tree_new (1);
      ewl_tree_headers_visible_set (EWL_TREE (tree), FALSE);
      ewl_table_add (EWL_TABLE (table), tree, 1, 1, 1, 1);
      _tree_fill (pdf, EWL_TREE (tree), NULL, index);
      evas_poppler_index_delete (index);
      ewl_widget_show (tree);
    }

  list = ewl_tree_new (1);
  ewl_tree_headers_visible_set (EWL_TREE (list), FALSE);
  if  (index)
    ewl_table_add (EWL_TABLE (table), list, 1, 1, 2, 2);
  else
    ewl_table_add (EWL_TABLE (table), list, 1, 1, 1, 2);
  ewl_widget_show (list);

  ewl_table_add (EWL_TABLE (table), pdf, 2, 2, 1, 2);
  ewl_widget_show (pdf);

  page_count = evas_poppler_document_page_count_get (document);
  for (i = 0; i < page_count; i++)
    {
      char row_text[64];
      char *txt;
      int  *num;

      txt = row_text;
      snprintf (row_text, 64, "%d", i + 1);
      row = ewl_tree_text_row_add (EWL_TREE (list), NULL, &txt);
      num = (int *)malloc (sizeof (int));
      *num = i;
      ewl_widget_data_set (row, "row-number", num);
      ewl_callback_append (EWL_WIDGET (row),
                           EWL_CALLBACK_CLICKED,
                           EWL_CALLBACK_FUNCTION (_change_page_cb),
                           pdf);
    }

  ewl_widget_show (window);

  ewl_main ();

  return 0;
}

static void _quit_cb (Ewl_Widget * w, void *ev_data, void *user_data)
{
  ewl_widget_destroy(w);
  ewl_main_quit();
}

static void
_change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  Ewl_Pdf *pdf;
  int row_number;

  row_number = *(int *)ewl_widget_data_get (widget, "row-number");
  pdf = EWL_PDF (user_data);
  if (row_number != ewl_pdf_page_get (pdf))
    {
      ewl_pdf_page_set (pdf, row_number);
      ewl_callback_call (EWL_WIDGET (pdf), EWL_CALLBACK_REVEAL);
    }
}
