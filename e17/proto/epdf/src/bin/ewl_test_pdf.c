#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ewl_Test.h>

#include "Epdf.h"
#include "ewl_pdf.h"


static Ewl_Widget     *pdf;
static Ewl_Widget     *list;
static Ewl_Widget     *entry;
static Ewl_Widget     *fd;


static void create_pdf_fd_window_response (Ewl_Widget *w, void *ev, void *user_data);
static void create_pdf_fd_cb              (Ewl_Widget *w, void *ev, void *user_data);
static void _row_data_free_cb             (Ewl_Widget *w, void *ev, void *user_data);
static void pdf_load ();

static int create_test (Ewl_Container *box);

void 
test_info (Ewl_Test *test)
{
  test->name = "Pdf";
  test->tip = "The pdf class is a basic image\n"
    "that displays the content of a pdf file.";
  test->filename = "ewl_test_pdf.c";
  test->func = create_test;
  test->type = EWL_TEST_TYPE_SIMPLE;
}

static void
_change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  Ewl_Pdf *pdf;
  int row_number;

  row_number = *(int *)ewl_widget_data_get (widget, "row-number");
  pdf = EWL_PDF (user_data);
  if (row_number != ewl_pdf_page_get (pdf)) {
    ewl_pdf_page_set (pdf, row_number);
    ewl_callback_call (EWL_WIDGET (pdf), EWL_CALLBACK_REVEAL);
  }
}

static int
create_test (Ewl_Container *box)
{
  Ewl_Widget     *hbox;
  Ewl_Widget     *button;
  Ewl_Widget     *scrollpane;
  char *          pdf_file = NULL;

  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (box), hbox);
  ewl_widget_show (hbox);

  /* We open the pdf file */
  pdf = ewl_pdf_new ();

  list = ewl_tree_new (1);
  ewl_tree_headers_visible_set (EWL_TREE (list), FALSE);
  ewl_tree_expandable_rows_set (EWL_TREE (list), 0);
  ewl_object_fill_policy_set (EWL_OBJECT (list),
                              EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);
  ewl_container_child_append (EWL_CONTAINER (hbox), list);
  ewl_widget_show (list);

  scrollpane = ewl_scrollpane_new ();
  ewl_scrollpane_hscrollbar_flag_set (EWL_SCROLLPANE (scrollpane),
                                      EWL_SCROLLPANE_FLAG_AUTO_VISIBLE);
  ewl_scrollpane_vscrollbar_flag_set (EWL_SCROLLPANE (scrollpane),
                                      EWL_SCROLLPANE_FLAG_AUTO_VISIBLE);
  ewl_container_child_append (EWL_CONTAINER (hbox), scrollpane);
  ewl_widget_show (scrollpane);

  ewl_container_child_append (EWL_CONTAINER (scrollpane), pdf);
  ewl_widget_show (pdf);

  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (box), hbox);
  ewl_widget_show (hbox);

  entry = ewl_entry_new ();
  ewl_object_fill_policy_set (EWL_OBJECT (entry),
                              EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
  ewl_container_child_append (EWL_CONTAINER (hbox), entry);
  ewl_widget_show (entry);

  button = ewl_button_new ();
  ewl_button_label_set (EWL_BUTTON (button), "Browse...");
  ewl_callback_append (button, EWL_CALLBACK_CLICKED,
                       create_pdf_fd_cb, entry);
  ewl_object_fill_policy_set (EWL_OBJECT (button), EWL_FLAG_FILL_NONE);
  ewl_object_alignment_set (EWL_OBJECT (button),
                            EWL_FLAG_ALIGN_CENTER);
  ewl_container_child_append (EWL_CONTAINER (hbox), button);
  ewl_widget_show (button);

  pdf_load (pdf_file);
  
  return 1;
}

static void
create_pdf_fd_cb(Ewl_Widget *w, void *ev_data,
                 void *user_data)
{
  if (fd)
    return;

  fd = ewl_filedialog_new();
  ewl_window_title_set (EWL_WINDOW (fd), "Select a Pdf file...");
  ewl_window_name_set (EWL_WINDOW (fd), "EWL Pdf Test");
  ewl_window_class_set (EWL_WINDOW (fd), "EWL Filedialog");
  ewl_callback_append (fd, EWL_CALLBACK_VALUE_CHANGED, 
                       create_pdf_fd_window_response, user_data);
  ewl_widget_show(fd);
}

static void
create_pdf_fd_window_response (Ewl_Widget *w, void *ev, void *data)
{
  Ewl_Dialog_Event *e;
  Ewl_Widget *entry;
  
  e = ev;
  entry = EWL_WIDGET (data);

  if (e->response == EWL_STOCK_OK) {
    const char *path;
    char *file;
    char *filename;
    int   l1;
    int   l2;

    printf("File open from pdf test: %s/%s\n", 
           path = ewl_filedialog_directory_get (EWL_FILEDIALOG (w)),
           filename = ewl_filedialog_selected_file_get (EWL_FILEDIALOG (w)));

    l1 = strlen (path);
    l2 = strlen (filename);
    if (!path || (path[0] == '\0'))
      file = strdup (filename);
    else {
      file = (char *)malloc (sizeof (char) * (l1 + l2 + 2));
      memcpy (file, path, l1);
      file[l1] = '/';
      memcpy (file + l1 + 1, filename, l2);
      file [l1 + l2 + 1] = '\0';
    }
    printf ("file : %s\n", file);
    pdf_load (file);
    free (file);
    // free(path); FIXME: Is text widget allocated correctly?
    }
  else {
    printf("Test program says bugger off.\n");
  }

  ewl_widget_destroy(fd);
  fd = NULL;
}

static void
pdf_load (const char *filename)
{
  Epdf_Document   *document;
  int              page_count;
  int              i;

  if (!filename)
    return;

  ewl_container_reset (EWL_CONTAINER (list));
  ewl_pdf_file_set (EWL_PDF (pdf), filename);
  document = ewl_pdf_pdf_document_get (EWL_PDF (pdf));
  if (!document) return;

  page_count = epdf_document_page_count_get (document);
  for (i = 0; i < page_count; i++) {
    char        row_text[64];
    char       *txt;
    Ewl_Widget *row;
    int        *num;

    txt = row_text;
    snprintf (row_text, 64, "%d", i + 1);
    row = ewl_tree_text_row_add (EWL_TREE (list), NULL, &txt);
    num = (int *)malloc (sizeof (int));
    *num = i;
    ewl_widget_data_set (row, "row-number", num);
    ewl_callback_append (EWL_WIDGET (row),
                         EWL_CALLBACK_CLICKED,
                         EWL_CALLBACK_FUNCTION (_change_page_cb),
                         NULL);
    ewl_callback_prepend (EWL_WIDGET (row),
                          EWL_CALLBACK_DESTROY,
                          EWL_CALLBACK_FUNCTION (_row_data_free_cb),
                          NULL);
  }  
}

static void
_row_data_free_cb (Ewl_Widget *widget,
                   void       *ev_data,
                   void       *user_data)
{
  int *row_number;

  if (!widget)
    return;
  row_number = (int *)ewl_widget_data_get (EWL_WIDGET (widget), "row-number");
  if (row_number) free (row_number);
}
