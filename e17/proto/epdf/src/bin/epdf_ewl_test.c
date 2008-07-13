#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ewl.h>

#include "ewl_pdf.h"


typedef struct Tree_Row_Data Tree_Row_Data;
typedef struct Tree_Data     Tree_Data;

struct Tree_Row_Data
{
  char      *text;
  Tree_Data *subdata;
  int        expandable;
  int        page;
};

struct Tree_Data
{
  unsigned int    count;
  Tree_Row_Data **rows;
};


static void _quit_cb (Ewl_Widget * w, void *ev_data, void *user_data);
static void _change_page_cb (Ewl_Widget *widget, void *ev_data, void *user_data);

void *
_tree_fill (Ewl_Widget *pdf, Ecore_List *items)
{
  Tree_Data       *data;
  Epdf_Index_Item *item;
  int              i = 0;

  if (!items)
    return NULL;

  data = (Tree_Data *)calloc (1, sizeof (Tree_Data));
  if (!data)
    return NULL;

  data->count = ecore_list_count (items);
  data->rows = (Tree_Row_Data **)calloc (data->count, sizeof (Tree_Row_Data *));
  if (!data->rows) {
    free (data);
    return NULL;
  }

  ecore_list_first_goto (items);
  while ((item = ecore_list_next (items))) {
    Ecore_List *c;
    int         page;

    page = epdf_index_item_page_get (ewl_pdf_pdf_document_get (EWL_PDF (pdf)), item);

    data->rows[i] = (Tree_Row_Data *)calloc (1, sizeof (Tree_Row_Data));
    data->rows[i]->text = strdup (epdf_index_item_title_get (item));
    data->rows[i]->expandable = 0;
    data->rows[i]->page = (page < 0) ? 0 : page;

    c = epdf_index_item_children_get (item);
    if (c) {
      data->rows[i]->expandable = 1;
      data->rows[i]->subdata = _tree_fill (pdf, c);
    }

    i++;
  }

  return data;
}

static void *tree_data_fetch(void *data, unsigned int row, unsigned int column)
{
  Tree_Data *d;

  d = (Tree_Data *)data;

  return d->rows[row]->text;
}

static unsigned int
tree_data_count_get (void *data)
{
  Tree_Data *d;

  d = (Tree_Data *)data;

  return d->count;
}

static int
tree_data_expandable_get(void *data, unsigned int row)
{
  Tree_Data *d;

  d = data;

  return d->rows[row]->expandable;
}

static void *
tree_data_expansion_fetch(void *data, unsigned int parent)
{
  Tree_Data *d;

  d = data;

  return d->rows[parent]->subdata;
}

static void
_label_text_set (Ewl_Widget *widget, void *data)
{
  ewl_label_text_set (EWL_LABEL (widget), (const char *)data);
  ewl_object_alignment_set (EWL_OBJECT (widget), EWL_FLAG_ALIGN_LEFT);
}

int
main (int argc, char *argv[])
{
  Ecore_List     *str_data = NULL;
  Ewl_Widget     *window;
  Ewl_Widget     *hbox;
  Ewl_Widget     *vbox;
  Ewl_Widget     *list;
  Ewl_Model      *model;
  Ewl_View       *view;
  Ewl_Widget     *tree;
  Ewl_Widget     *pdf;
  Ewl_Widget     *sp;
  Ecore_List     *index;
  Epdf_Document  *document;
  int             page_count;
  int             i;

  if (argc == 1) {
    printf ("Usage: %s pdf_file\n", argv[0]);
    return EXIT_FAILURE;
  }

  ewl_init (&argc, (char **)argv);
  str_data = ecore_list_new();
  ecore_list_free_cb_set (str_data, free);

  /* We open the pdf file */
  pdf = ewl_pdf_new ();
  if (!ewl_pdf_file_set (EWL_PDF (pdf), argv[1])) {
    printf ("Can not load the document %s\nExiting...", argv[1]);
    ecore_list_destroy (str_data);
    ewl_main_quit();
    return EXIT_FAILURE;
  }

  window = ewl_window_new ();
  ewl_window_title_set (EWL_WINDOW (window), "Ewl Pdf Test Application");
  ewl_callback_append (window, EWL_CALLBACK_DELETE_WINDOW,
                       _quit_cb, str_data);

  hbox = ewl_hbox_new ();
  ewl_box_homogeneous_set (EWL_BOX (hbox), FALSE);
  ewl_container_child_append (EWL_CONTAINER (window), hbox);
  ewl_object_fill_policy_set (EWL_OBJECT (hbox), EWL_FLAG_FILL_ALL);
  ewl_widget_show (hbox);

  vbox = ewl_vbox_new ();
  ewl_box_homogeneous_set (EWL_BOX (vbox), TRUE);
  ewl_container_child_append (EWL_CONTAINER (hbox), vbox);
  ewl_object_fill_policy_set (EWL_OBJECT (vbox), EWL_FLAG_FILL_ALL);
  ewl_widget_show (vbox);

  document = ewl_pdf_pdf_document_get (EWL_PDF (pdf));
  index = ewl_pdf_pdf_index_get (EWL_PDF (pdf));

  /* view for both the list and the tree (if the index exists) */
  view = ewl_label_view_get();

  if (index) {
    Ewl_Model *model;
    void      *data;

    /* tree */
    tree = ewl_tree_new ();
    ewl_widget_name_set (tree, "tree");
/*     ewl_object_fill_policy_set (EWL_OBJECT (tree), EWL_FLAG_FILL_VFILL); */
    ewl_tree_headers_visible_set (EWL_TREE (tree), FALSE);
    ewl_callback_append (tree, EWL_CALLBACK_VALUE_CHANGED,
                         _change_page_cb, pdf);

    /* data */
    data = _tree_fill (pdf, index);
    epdf_index_delete (index);

    /* model */
    model = ewl_model_new ();
    ewl_model_data_fetch_set (model, tree_data_fetch);
    ewl_model_data_count_set (model, tree_data_count_get);
    ewl_model_data_expandable_set (model, tree_data_expandable_get);
    ewl_model_expansion_data_fetch_set(model, tree_data_expansion_fetch);

    /* MVC */
    ewl_mvc_data_set (EWL_MVC (tree), data);
    ewl_mvc_model_set (EWL_MVC (tree), model);
    ewl_mvc_view_set(EWL_MVC(tree), view);

    /* we attach and show */
    ewl_container_child_append (EWL_CONTAINER (vbox), tree);
    ewl_widget_show (tree);
  }

  sp = ewl_scrollpane_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), sp);
  ewl_object_fill_policy_set (EWL_OBJECT (sp), EWL_FLAG_FILL_ALL);
  ewl_widget_show (sp);

  page_count = epdf_document_page_count_get (document);
  for (i = 0; i < page_count; i++) {
    char row_text[64];
    char *txt;

    snprintf (row_text, 64, "%d", i + 1);
    txt = strdup (row_text);
    ecore_list_append(str_data, txt);
  }

  model = ewl_model_ecore_list_instance();

  list = ewl_list_new ();
  ewl_object_fill_policy_set (EWL_OBJECT (list), EWL_FLAG_FILL_ALL);

  ewl_mvc_data_set(EWL_MVC(list), str_data);
  ewl_mvc_model_set(EWL_MVC(list), model);
  ewl_mvc_view_set(EWL_MVC(list), view);
  ewl_callback_append (list,
                       EWL_CALLBACK_VALUE_CHANGED,
                       EWL_CALLBACK_FUNCTION (_change_page_cb),
                       pdf);
  ewl_container_child_append (EWL_CONTAINER (sp), list);
  ewl_widget_show (list);

  ewl_pdf_scale_set (EWL_PDF (pdf), 0.5, 0.5);
  ewl_container_child_append (EWL_CONTAINER (hbox), pdf);
  ewl_widget_show (pdf);

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
  Ewl_Pdf           *pdf;
  Ewl_Selection_Idx *sel;

  pdf = EWL_PDF (user_data);

  if (ewl_widget_type_is (widget, "list")) {
/*     sel = ewl_mvc_selected_get (EWL_MVC (widget)); */

/*     pdf = EWL_PDF (user_data); */
/*     if (idx->row != ewl_pdf_page_get (pdf)) { */
/*       ewl_pdf_page_set (pdf, idx->row); */
/*       ewl_callback_call (EWL_WIDGET (pdf), EWL_CALLBACK_REVEAL); */
/*     } */
  }
  else {
    Tree_Data     *data;
    Tree_Row_Data *row_data;

    sel = ewl_mvc_selected_get (EWL_MVC (widget));
    if (!sel) return;
    data = (Tree_Data *)EWL_SELECTION(sel)->data;
    row_data = data->rows[sel->row];
    if (row_data) {
      ewl_pdf_page_set (pdf, row_data->page);
      ewl_callback_call (EWL_WIDGET (pdf), EWL_CALLBACK_REVEAL);
    }
  }
}
