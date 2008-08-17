#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ewl.h>

#include "ewl_dvi.h"

#include "config.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif


static void _quit_cb (Ewl_Widget * w, void *ev_data, void *user_data);
static void _change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data);

int
main (int argc, char *argv[])
{
  Ecore_List     *str_data = NULL;
  Ewl_Widget     *window;
  Ewl_Widget     *hbox;
  Ewl_Widget     *list;
  Ewl_Model      *model;
  Ewl_View       *view;
  Ewl_Widget     *dvi;
  Ewl_Widget     *sp;
  const Edvi_Document  *document;
  int             page_count;
  int             i;

  if (argc == 1) {
    printf ("Usage: %s dvi_file\n", argv[0]);
    return EXIT_FAILURE;
  }

  printf ("[DVI] version       : %s\n", edvi_version_get ());
  if (!edvi_init (300, "cx", 4,
                  1.0, 1.0,
                  0, 255, 255, 255, 0, 0, 0))
    return EXIT_FAILURE;

  ewl_init (&argc, (char **)argv);
  str_data = ecore_list_new();
  ecore_list_free_cb_set (str_data, free);

  /* We open the dvi file */
  dvi = ewl_dvi_new ();
  if (!ewl_dvi_file_set (EWL_DVI (dvi), argv[1])) {
    printf ("Can not load the document %s\nExiting...", argv[1]);
    ecore_list_destroy (str_data);
    ewl_main_quit();
    return EXIT_FAILURE;
  }

  window = ewl_window_new ();
  ewl_window_title_set (EWL_WINDOW (window), "Ewl Dvi Test Application");
  ewl_callback_append (window, EWL_CALLBACK_DELETE_WINDOW,
                       _quit_cb, str_data);

  hbox = ewl_hbox_new ();
  ewl_box_homogeneous_set (EWL_BOX (hbox), FALSE);
  ewl_container_child_append (EWL_CONTAINER (window), hbox);
  ewl_widget_show (hbox);

  sp = ewl_scrollpane_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox), sp);
  ewl_widget_show (sp);

  document = ewl_dvi_dvi_document_get (EWL_DVI (dvi));
  page_count = edvi_document_page_count_get (document);
  for (i = 0; i < page_count; i++) {
    char row_text[64];
    char *txt;

    snprintf (row_text, 64, "%d", i + 1);
    txt = strdup (row_text);
    ecore_list_append(str_data, txt);
  }

  model = ewl_model_ecore_list_instance();
  view = ewl_label_view_get();

  list = ewl_list_new ();
  ewl_mvc_model_set(EWL_MVC(list), model);
  ewl_mvc_view_set(EWL_MVC(list), view);
  ewl_mvc_data_set(EWL_MVC(list), str_data);
  ewl_callback_append (list,
                       EWL_CALLBACK_VALUE_CHANGED,
                       EWL_CALLBACK_FUNCTION (_change_page_cb),
                       dvi);
  ewl_container_child_append (EWL_CONTAINER (sp), list);
  ewl_widget_show (list);

  ewl_dvi_mag_set (EWL_DVI (dvi), 0.5);
  ewl_container_child_append (EWL_CONTAINER (hbox), dvi);
  ewl_widget_show (dvi);

  ewl_widget_show (window);

  ewl_main ();

  edvi_shutdown ();

  return EXIT_SUCCESS;
}

static void _quit_cb (Ewl_Widget * w, void *ev_data __UNUSED__, void *user_data)
{
  Ecore_List *list;

  if (user_data) {
    list = (Ecore_List *)user_data;
    ecore_list_destroy (list);
  }
  ewl_widget_destroy(w);
  ewl_main_quit();
}

static void
_change_page_cb (Ewl_Widget *widget, void *ev_data __UNUSED__, void *user_data)
{
  Ewl_Dvi           *dvi;
  Ewl_List          *list;
  Ecore_List        *el;
  Ewl_Selection_Idx *idx;

  list = EWL_LIST(widget);
  el = ewl_mvc_data_get(EWL_MVC(list));
  idx = ewl_mvc_selected_get(EWL_MVC(list));

  dvi = EWL_DVI (user_data);
  if (idx->row != (unsigned int)ewl_dvi_page_get (dvi)) {
    ewl_dvi_page_set (dvi, idx->row);
    ewl_widget_configure(EWL_WIDGET(dvi));
  }
}
