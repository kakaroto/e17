#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ewl.h>

#include "ewl_dvi.h"


static void _quit_cb (Ewl_Widget * w, void *ev_data, void *user_data);
static void _change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data);

int
main (int argc, char *argv[])
{
  Ewl_Widget     *window;
  Ewl_Widget     *hbox;
  Ewl_Widget     *list;
  Ewl_Widget     *dvi;
  Ewl_Widget     *row;
  Edvi_Document  *document;
  int             page_count;
  int             i;

  if (argc == 1) {
    printf ("Usage: %s dvi_file\n", argv[0]);
    return -1;
  }

  printf ("[DVI] version       : %s\n", edvi_version ());
  if (!edvi_init (300, "cx", 4,
                  1.0, 1.0,
                  0, 0, 0, 0, 255, 255, 255)) {
    return -1;
  }

  ewl_init (&argc, (char **)argv);

  /* We open the dvi file */
  dvi = ewl_dvi_new ();
  ewl_dvi_file_set (EWL_DVI (dvi), argv[1]);
  document = EWL_DVI (dvi)->dvi_document;
  if (!document) {
    printf ("The file %s can't be opened\n", argv[1]);
    ewl_main_quit ();
    edvi_shutdown ();
    return -1;
  }

  window = ewl_window_new ();
  ewl_window_title_set (EWL_WINDOW (window), "Ewl Dvi Test Application");
  ewl_callback_append (window, EWL_CALLBACK_DELETE_WINDOW,
                       _quit_cb, NULL);

  hbox = ewl_hbox_new ();
  ewl_box_homogeneous_set (EWL_BOX (hbox), FALSE);
  ewl_container_child_append (EWL_CONTAINER (window), hbox);
  ewl_widget_show (hbox);

  list = ewl_tree_new (1);
  ewl_tree_headers_visible_set (EWL_TREE (list), FALSE);
  ewl_container_child_append (EWL_CONTAINER (hbox), list);
  ewl_widget_show (list);

  ewl_container_child_append (EWL_CONTAINER (hbox), dvi);
  ewl_widget_show (dvi);

  page_count = edvi_document_page_count_get (document);
  for (i = 0; i < page_count; i++) {
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
                         dvi);
  }

  ewl_widget_show (window);

  ewl_main ();

  return 0;
}

static void _quit_cb (Ewl_Widget * w, void *ev_data, void *user_data)
{
  ewl_widget_destroy(w);
  ewl_main_quit();
  edvi_shutdown ();
}

static void
_change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  Ewl_Dvi *dvi;
  int row_number;

  row_number = *(int *)ewl_widget_data_get (widget, "row-number");
  dvi = EWL_DVI (user_data);
  if (row_number != ewl_dvi_page_get (dvi)) {
    ewl_dvi_page_set (dvi, row_number);
    ewl_callback_call (EWL_WIDGET (dvi), EWL_CALLBACK_REVEAL);
  }
}
