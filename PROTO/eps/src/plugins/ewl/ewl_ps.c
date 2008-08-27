#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ewl_Test.h>
#include <ewl_box.h>
#include <ewl_button.h>
#include <ewl_entry.h>
#include <ewl_filedialog.h>
#include <ewl_scrollpane.h>
#include <ewl_label.h>
#include <ewl_list.h>
#include <ewl_tree.h>

#include <Eps.h>
#include <ewl_ps.h>


static Ewl_Widget     *ps;
static Ewl_Widget     *list;
static Ewl_Widget     *entry;
static Ewl_Widget     *fd;
static Ewl_Model      *model;
static Ewl_View       *view;
static Ecore_List     *str_data = NULL;


static void create_ps_fd_window_response (Ewl_Widget *w, void *ev, void *user_data);
static void create_ps_fd_cb              (Ewl_Widget *w, void *ev, void *user_data);
static void ps_load ();

static int create_test (Ewl_Container *box);

void
test_info (Ewl_Test *test)
{
  test->name = "Ps";
  test->tip = "The ps class is a basic image\n"
    "that displays the content of a postscript file.";
  test->filename = "ewl_test_ps.c";
  test->func = create_test;
  test->type = EWL_TEST_TYPE_SIMPLE;
}

static void
_change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  Ewl_Ps            *ps;
  Ewl_List          *list;
  Ecore_List        *el;
  Ewl_Selection_Idx *idx;

  list = EWL_LIST(widget);
  el = ewl_mvc_data_get(EWL_MVC(list));
  idx = ewl_mvc_selected_get(EWL_MVC(list));

  ps = EWL_PS (user_data);
  if (idx->row != ewl_ps_page_get (ps)) {
    ewl_ps_page_set (ps, idx->row);
    ewl_callback_call (EWL_WIDGET (ps), EWL_CALLBACK_REVEAL);
  }
}

static int
create_test (Ewl_Container *box)
{
  Ewl_Widget *hbox;
  Ewl_Widget *button;
  Ewl_Widget *scrollpane;

  ewl_box_homogeneous_set (EWL_BOX (box), FALSE);

  scrollpane = ewl_scrollpane_new ();
  ewl_container_child_append (EWL_CONTAINER (box/* hbox */), scrollpane);
  ewl_widget_show (scrollpane);

  hbox = ewl_hbox_new ();
  ewl_box_homogeneous_set (EWL_BOX (hbox), FALSE);
  ewl_container_child_append (EWL_CONTAINER (scrollpane/* box */), hbox);
  ewl_widget_show (hbox);

  /* the list pages */
/*   scrollpane = ewl_scrollpane_new (); */
/*   ewl_object_fill_policy_set (EWL_OBJECT (scrollpane), */
/*                               EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL); */
/*   ewl_container_child_append (EWL_CONTAINER (hbox), scrollpane); */
/*   ewl_widget_show (scrollpane); */

  str_data = ecore_list_new();
  ecore_list_free_cb_set (str_data, free);

  model = ewl_model_ecore_list_instance();
  view = ewl_label_view_get();

  list = ewl_list_new ();
  ewl_object_fill_policy_set (EWL_OBJECT (list),
                              EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);
  ewl_container_child_append (EWL_CONTAINER (hbox/* scrollpane */), list);
  ewl_widget_show (list);

  /* We open the ps file */
  ps = ewl_ps_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox/* scrollpane */), ps);
  ewl_widget_show (ps);

  /* the file selector */
  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (box), hbox);
  ewl_widget_show (hbox);

  entry = ewl_entry_new ();
  ewl_object_fill_policy_set (EWL_OBJECT (entry),
                              EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VSHRINK);
  ewl_container_child_append (EWL_CONTAINER (hbox), entry);
  ewl_widget_show (entry);

  button = ewl_button_new ();
  ewl_button_label_set (EWL_BUTTON (button), "Browse...");
  ewl_callback_append (button, EWL_CALLBACK_CLICKED,
                       create_ps_fd_cb, entry);
  ewl_object_fill_policy_set (EWL_OBJECT (button), EWL_FLAG_FILL_NONE);
  ewl_object_alignment_set (EWL_OBJECT (button),
                            EWL_FLAG_ALIGN_CENTER);
  ewl_container_child_append (EWL_CONTAINER (hbox), button);
  ewl_widget_show (button);

  return 1;
}

static void
create_ps_fd_cb(Ewl_Widget *w, void *ev_data,
                 void *user_data)
{
  if (fd)
    return;

  fd = ewl_filedialog_new();
  ewl_window_title_set (EWL_WINDOW (fd), "Select a Ps file...");
  ewl_window_name_set (EWL_WINDOW (fd), "EWL Ps Test");
  ewl_window_class_set (EWL_WINDOW (fd), "EWL Filedialog");
  ewl_callback_append (fd, EWL_CALLBACK_VALUE_CHANGED,
                       create_ps_fd_window_response, user_data);
  ewl_widget_show(fd);
}

static void
create_ps_fd_window_response(Ewl_Widget *w, void *ev, void *data)
{
  Ewl_Event_Action_Response *e;
  Ewl_Widget                *entry;

  e = ev;
  entry = EWL_WIDGET (data);

  if (e->response == EWL_STOCK_OK) {
    char *file;

    printf("File open from ps test: %s\n",
           file = ewl_filedialog_selected_file_get (EWL_FILEDIALOG (w)));

    printf ("file : %s\n", file);
    ewl_text_text_set (EWL_TEXT (data), file);
    ps_load (file);
    // free(path); FIXME: Is text widget allocated correctly?
    }
  else {
    printf("Test program says bugger off.\n");
  }

  ewl_widget_destroy(fd);
  fd = NULL;
}

static void
ps_load (const char *filename)
{
  const Eps_Document *document;
  int                 page_count;
  int                 i;

  if (!filename)
    return;

/*   ewl_container_reset (EWL_CONTAINER (str_data)); */
  ewl_container_reset (EWL_CONTAINER (list));
  ewl_ps_file_set (EWL_PS (ps), filename);
  document = ewl_ps_ps_document_get (EWL_PS (ps));
  if (!document) return;

  page_count = eps_document_page_count_get (document);
  printf ("document : %d\n", page_count);
  for (i = 0; i < page_count; i++) {
    char        row_text[64];
    char       *txt;

    snprintf (row_text, 64, "%d", i + 1);
    txt = strdup (row_text);
    ecore_list_append(str_data, txt);
  }

  ewl_mvc_data_set(EWL_MVC(list), str_data);
  ewl_mvc_model_set(EWL_MVC(list), model);
  ewl_mvc_view_set(EWL_MVC(list), view);

  ewl_callback_append (list,
		       EWL_CALLBACK_VALUE_CHANGED,
		       EWL_CALLBACK_FUNCTION (_change_page_cb),
		       ps);
  ewl_widget_show (list);
}
