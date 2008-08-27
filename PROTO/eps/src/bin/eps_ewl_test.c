#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ewl.h>

#include "ewl_ps.h"


static void _quit_cb (Ewl_Widget * w, void *ev_data, void *user_data);
static void _change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data);


int
main (int argc, char *argv[])
{
  Ecore_List   *str_data = NULL;
  Ewl_Widget   *window;
  Ewl_Widget   *hbox;
  Ewl_Widget   *list;
  Ewl_Model    *model;
  Ewl_View     *view;
  Ewl_Widget   *ps;
  Ewl_Widget   *sp;
  Eps_Document *document;
  int           page_count;
  int           i;

  if (argc == 1) {
    printf ("Usage: %s ps_file\n", argv[0]);
    return EXIT_FAILURE;
  }

  ewl_init (&argc, (char **)argv);

  /* We open the ps file */
  ps = ewl_ps_new ();
  if (!ewl_ps_file_set (EWL_PS (ps), argv[1])) {
    printf ("The file %s can't be opened\n", argv[1]);
    ewl_main_quit ();
    return EXIT_FAILURE;
  }

  window = ewl_window_new ();
  ewl_window_title_set (EWL_WINDOW (window), "Ewl Ps Test Application");
  ewl_callback_append (window, EWL_CALLBACK_DELETE_WINDOW,
                       _quit_cb, str_data);

  hbox = ewl_hbox_new ();
  ewl_box_homogeneous_set (EWL_BOX (hbox), FALSE);
  ewl_container_child_append (EWL_CONTAINER (window), hbox);
/*   ewl_object_fill_policy_set (EWL_OBJECT (hbox), EWL_FLAG_FILL_ALL); */
  ewl_widget_show (hbox);

  sp = ewl_scrollpane_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox), sp);
/*   ewl_object_fill_policy_set (EWL_OBJECT (sp), EWL_FLAG_FILL_FILL); */
  ewl_widget_show (sp);

  str_data = ecore_list_new();
  ecore_list_free_cb_set (str_data, free);

  document = EWL_PS (ps)->ps_document;

  page_count = eps_document_page_count_get (document);
  for (i = 0; i < page_count; i++) {
    char row_text[64];
    char *txt;

    snprintf (row_text, 64, "%d", i + 1);
    txt = strdup (row_text);
    ecore_list_append(str_data, txt);
  }

  view = ewl_label_view_get();
  model = ewl_model_ecore_list_instance();

  list = ewl_list_new ();
/*   ewl_object_,fill_policy_set (EWL_OBJECT (list), EWL_FLAG_FILL_ALL); */

  ewl_mvc_data_set(EWL_MVC(list), str_data);
  ewl_mvc_model_set(EWL_MVC(list), model);
  ewl_mvc_view_set(EWL_MVC(list), view);
  ewl_callback_append (list,
                       EWL_CALLBACK_VALUE_CHANGED,
                       EWL_CALLBACK_FUNCTION (_change_page_cb),
                       ps);
  ewl_container_child_append (EWL_CONTAINER (sp), list);
  ewl_widget_show (list);

  ewl_ps_scale_set (EWL_PS (ps), 0.5, 0.5);
  ewl_container_child_append (EWL_CONTAINER (hbox), ps);
  ewl_widget_show (ps);

  ewl_widget_show (window);

  ewl_main ();

  return EXIT_SUCCESS;
}

static void _quit_cb (Ewl_Widget * w, void *ev_data, void *user_data)
{
  Ecore_List *list;

  if (user_data) {
    list = (Ecore_List *)user_data;
    ecore_list_destroy (list);
  }
  ewl_main_quit();
}

static void
_change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  Ewl_Ps            *ps;
  Ewl_List          *list;
  Ewl_Selection_Idx *idx;

  list = EWL_LIST(widget);
  idx = ewl_mvc_selected_get(EWL_MVC(list));

  ps = EWL_PS (user_data);
  if (idx->row != ewl_ps_page_get (ps)) {
    ewl_ps_page_set (ps, idx->row);
  }

  free (idx);
}
