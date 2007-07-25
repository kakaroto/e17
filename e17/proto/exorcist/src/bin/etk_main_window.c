#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Etk.h>

#include <Epdf.h>
#include <etk_pdf.h>


#define __UNUSED__ __attribute__((unused))


typedef struct Exo_Etk_ Exo_Etk;
struct Exo_Etk_
{
  char       *path;
  char       *filename;
  Etk_Widget *pdf;
  Etk_Widget *list_pages;
  Etk_Widget *list_index;
  Etk_Widget *win_index;

  Etk_Widget *search_text;
  Etk_Widget *search_is_case_sensitive;
  Etk_Widget *search_circular;
};

typedef struct _Exo_Ps_Data Exo_Ps_Data;
struct _Exo_Ps_Data {
  Epdf_Document *pdf_doc;
  Etk_Widget    *filename;
  Etk_Widget    *first_page;
  Etk_Widget    *last_page;
  Etk_Widget    *width;
  Etk_Widget    *height;
  Etk_Widget    *duplex;
};

static void     _exo_etk_quit_cb                   (Etk_Object *object, void *user_data);
static Etk_Bool _exo_etk_delete_cb                 (Etk_Object *object, void *user_data);
static Etk_Bool _exo_etk_index_delete_cb           (Etk_Object *object, void *user_data);
static void     _exo_etk_menu_popup_cb             (Etk_Object *object, void *event_info, void *data);
static void     _change_page_cb                    (Etk_Object *object, Etk_Tree_Row *row, void *data);
static void     _exo_etk_document_info_cb          (Etk_Object *object, void *user_data);
static Etk_Bool _exo_etk_document_info_delete_cb   (Etk_Object *object, void *user_data);
static void     _exo_etk_document_info_response_cb (Etk_Object *object, int res, void *user_data);
static void     _exo_etk_search_cb                 (Etk_Object *object, void *user_data);
static Etk_Bool _exo_etk_search_delete_cb          (Etk_Object *object, void *user_data);
static void     _exo_etk_search_response_cb        (Etk_Object *object, int res, void *user_data);
static void     _exo_etk_index_show_cb             (Etk_Object *object, void *user_data);
static void     _exo_etk_row_data_free_cb          (void *data);

static void     _exo_etk_orientation_landscape_cb  (Etk_Object *object, void *user_data);
static void     _exo_etk_orientation_upsidedown_cb (Etk_Object *object, void *user_data);
static void     _exo_etk_orientation_seascape_cb   (Etk_Object *object, void *user_data);
static void     _exo_etk_orientation_portrait_cb   (Etk_Object *object, void *user_data);

static void     _exo_etk_size_150_cb               (Etk_Object *object, void *user_data);
static void     _exo_etk_size_100_cb               (Etk_Object *object, void *user_data);
static void     _exo_etk_size_50_cb                (Etk_Object *object, void *user_data);

static void     _exo_etk_document_open_cb          (Etk_Object *object, void *user_data);
static void     _exo_etk_document_save_cb          (Etk_Object *object, void *user_data);
static void     _exo_etk_document_save_response_cb (Etk_Object *object, int res, void *user_data);
static Etk_Bool _exo_etk_document_save_delete_cb   (Etk_Object *object, void *user_data);


static void
_exo_etk_tree_fill (Etk_Pdf *pdf, Etk_Tree *tree, Etk_Tree_Col *col, Etk_Tree_Row *row, Ecore_List *items)
{
  Etk_Tree_Row    *prow;
  Epdf_Index_Item *item;

  if (!items)
    return;

  ecore_list_first_goto (items);
  while ((item = ecore_list_next (items))) {
    char       *buf;
    Ecore_List *c;
    int         *num;

    buf = strdup (epdf_index_item_title_get (item));
    if (!row)
      prow = etk_tree_append (tree, col, buf, NULL);
    else
      prow = etk_tree_append_to_row (row, col, buf, NULL);

    num = (int *)malloc (sizeof (int));
    *num = epdf_index_item_page_get (etk_pdf_pdf_document_get (pdf), item);
    etk_tree_row_data_set_full (prow, num, _exo_etk_row_data_free_cb);
    free (buf);
    c = epdf_index_item_children_get (item);
    if (c) {
      _exo_etk_tree_fill (pdf, tree, col, prow, c);
    }
  }
}

static void
_exo_etk_row_data_free_cb (void *data)
{
  if (data)
    free (data);
}

static void
_exo_etk_update_document (Exo_Etk      *data,
                          Etk_Tree_Col *col_pages,
                          Etk_Tree_Col *col_index)
{
  Epdf_Document   *document;
  Ecore_List      *index;
  char            *file;
  int              page_count;
  int              i;
  int              l1;
  int              l2;

  if (!data || !data->filename) return;

  etk_tree_clear (ETK_TREE (data->list_pages));
  etk_tree_clear (ETK_TREE (data->list_index));

  l1 = strlen (data->path);
  l2 = strlen (data->filename);
  if (!data->path || (data->path[0] == '\0'))
    etk_pdf_file_set (ETK_PDF (data->pdf), data->filename);
  else {
    file = (char *)malloc (sizeof (char) * (l1 + l2 + 2));
    memcpy (file, data->path, l1);
    file[l1] = '/';
    memcpy (file + l1 + 1, data->filename, l2);
    file [l1 + l2 + 1] = '\0';
    etk_pdf_file_set (ETK_PDF (data->pdf), file);
    free (file);
  }
  document = etk_pdf_pdf_document_get (ETK_PDF (data->pdf));
  if (!document) {
    char        buf[1024];
    Etk_Widget *dialog_error;
    Etk_Widget *label;

    dialog_error = etk_dialog_new ();
    etk_window_title_set (ETK_WINDOW (dialog_error), "Exorcist - Error");
    etk_window_wmclass_set (ETK_WINDOW (dialog_error), "Exorcist - Error", "Exorcist - Error");
    etk_container_border_width_set (ETK_CONTAINER (dialog_error), ETK_TRUE);

    etk_dialog_button_add (ETK_DIALOG (dialog_error),
                           etk_stock_label_get (ETK_STOCK_DIALOG_OK), ETK_RESPONSE_OK);
    etk_signal_connect ("response", ETK_OBJECT (dialog_error),
                        ETK_CALLBACK (etk_object_destroy), NULL);

    snprintf (buf, 1024, "Error: Can not open file<br>%s", etk_pdf_file_get (ETK_PDF (data->pdf)));
    label = etk_label_new (buf);
    etk_dialog_pack_in_main_area (ETK_DIALOG (dialog_error), label, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);
    etk_widget_show (label);


    etk_widget_show (dialog_error);

    return;
  }

  page_count = epdf_document_page_count_get (document);
  etk_tree_freeze (ETK_TREE (data->list_pages));
  for (i = 0; i < page_count; i++) {
    Etk_Tree_Row *row;
    int          *num;

    row = etk_tree_append (ETK_TREE (data->list_pages), col_pages, i + 1, NULL);
    num = (int *)malloc (sizeof (int));
    *num = i;
    etk_tree_row_data_set_full (row, num, _exo_etk_row_data_free_cb);
  }
  etk_tree_thaw (ETK_TREE (data->list_pages));

  index = etk_pdf_pdf_index_get (ETK_PDF (data->pdf));
  _exo_etk_tree_fill (ETK_PDF (data->pdf), ETK_TREE (data->list_index), col_index, NULL, index);
}

void
_exo_etk_index_window (Exo_Etk *data)
{
  char        title[4096];

  snprintf (title, 4096, "Exorcist - %s - Index", data->filename);

  data->win_index = etk_window_new ();
  etk_window_title_set (ETK_WINDOW (data->win_index), title);
  etk_window_wmclass_set (ETK_WINDOW (data->win_index), title, title);
  etk_widget_size_request_set (data->win_index, 150, 200);
  etk_signal_connect ("delete_event", ETK_OBJECT (data->win_index),
                      ETK_CALLBACK (_exo_etk_index_delete_cb), data);

  etk_container_add (ETK_CONTAINER (data->win_index), data->list_index);
  etk_widget_size_request_set (data->list_index, 150, 150);
  etk_widget_show (data->list_index);
}

static Etk_Widget *
_exo_etk_menu_options (Exo_Etk *data)
{
  Etk_Widget *menu;
  Etk_Widget *submenu;
  Etk_Widget *menu_item;

  menu = etk_menu_new ();

  menu_item = etk_menu_item_new_with_label ("Zoom in");
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_new_with_label ("Zoom out");
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_new_with_label ("Search");
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_search_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_check_new_with_label ("Index");
  etk_signal_connect ("toggled", ETK_OBJECT (menu_item), ETK_CALLBACK (_exo_etk_index_show_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_new_with_label ("Orientation");
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

  submenu = etk_menu_new ();
  etk_menu_item_submenu_set (ETK_MENU_ITEM (menu_item), ETK_MENU (submenu));

  menu_item = etk_menu_item_radio_new_with_label_from_widget ("Portrait", NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_orientation_portrait_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (submenu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_radio_new_with_label_from_widget ("Landscape", ETK_MENU_ITEM_RADIO (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_orientation_landscape_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (submenu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_radio_new_with_label_from_widget ("Upside down", ETK_MENU_ITEM_RADIO (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_orientation_upsidedown_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (submenu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_radio_new_with_label_from_widget ("Seascape", ETK_MENU_ITEM_RADIO (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_orientation_seascape_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (submenu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_new_with_label ("Size");
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

  submenu = etk_menu_new ();
  etk_menu_item_submenu_set (ETK_MENU_ITEM (menu_item), ETK_MENU (submenu));

  menu_item = etk_menu_item_radio_new_with_label_from_widget ("150%%", NULL);
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_size_150_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (submenu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_radio_new_with_label_from_widget ("100%%", ETK_MENU_ITEM_RADIO (menu_item));
  etk_menu_item_check_active_set (ETK_MENU_ITEM_CHECK (menu_item), TRUE);
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_size_100_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (submenu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_radio_new_with_label_from_widget ("50%%", ETK_MENU_ITEM_RADIO (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_size_50_cb), data);
  etk_menu_shell_append (ETK_MENU_SHELL (submenu), ETK_MENU_ITEM (menu_item));

  return menu;
}

static Etk_Widget *
_exo_etk_menu_bar (Exo_Etk *data)
{
  Etk_Widget *menu_bar;
  Etk_Widget *menu_item;
  Etk_Widget *menu;

  menu_bar = etk_menu_bar_new ();

  /* File menu */
  menu_item = etk_menu_item_new_with_label ("File");
  etk_menu_shell_append (ETK_MENU_SHELL (menu_bar), ETK_MENU_ITEM (menu_item));

  menu = etk_menu_new ();
  etk_menu_item_submenu_set (ETK_MENU_ITEM (menu_item), ETK_MENU (menu));

  menu_item = etk_menu_item_image_new_with_label ("Open");
  {
    Etk_Widget *image;

    image = etk_image_new_from_stock (ETK_STOCK_DOCUMENT_OPEN, ETK_STOCK_SMALL);
    etk_menu_item_image_set (ETK_MENU_ITEM_IMAGE (menu_item), ETK_IMAGE (image));
  }
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_document_open_cb), data);

  menu_item = etk_menu_item_image_new_with_label ("Save");
  {
    Etk_Widget *image;

    image = etk_image_new_from_stock (ETK_STOCK_DOCUMENT_SAVE, ETK_STOCK_SMALL);
    etk_menu_item_image_set (ETK_MENU_ITEM_IMAGE (menu_item), ETK_IMAGE (image));
  }
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_document_save_cb), data);

  menu_item = etk_menu_item_image_new_with_label ("Info");
  {
    Etk_Widget *image;

    image = etk_image_new_from_stock (ETK_STOCK_DIALOG_INFORMATION, ETK_STOCK_SMALL);
    etk_menu_item_image_set (ETK_MENU_ITEM_IMAGE (menu_item), ETK_IMAGE (image));
  }
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_document_info_cb), data);

  menu_item = etk_menu_item_separator_new();
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

  menu_item = etk_menu_item_image_new_with_label ("Quit");
  {
    Etk_Widget *image;

    image = etk_image_new_from_stock (ETK_STOCK_SYSTEM_LOG_OUT, ETK_STOCK_SMALL);
    etk_menu_item_image_set (ETK_MENU_ITEM_IMAGE (menu_item), ETK_IMAGE (image));
  }
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_quit_cb), data);

  /* options menu */
  menu_item = etk_menu_item_new_with_label ("Options");
  etk_menu_shell_append (ETK_MENU_SHELL (menu_bar), ETK_MENU_ITEM (menu_item));

  menu = _exo_etk_menu_options (data);
  etk_menu_item_submenu_set (ETK_MENU_ITEM (menu_item), ETK_MENU (menu));

  /* help menu */
  menu_item = etk_menu_item_new_with_label ("Help");
  etk_menu_shell_append (ETK_MENU_SHELL (menu_bar), ETK_MENU_ITEM (menu_item));

  menu = etk_menu_new ();
  etk_menu_item_submenu_set (ETK_MENU_ITEM (menu_item), ETK_MENU (menu));

  menu_item = etk_menu_item_new_with_label ("About");
/*   { */
/*     Etk_Widget *image; */

/*     image = etk_image_new_from_stock (ETK_STOCK_HELP_CONTENTS, ETK_STOCK_SMALL); */
/*     etk_menu_item_image_set (ETK_MENU_ITEM (menu_item), ETK_IMAGE (image)); */
/*   } */
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

  return menu_bar;
}

void
exo_etk_main_window (const char *filename)
{
  Etk_Widget *window;
  Etk_Widget *vbox;
  Etk_Widget *menu_bar;
  Etk_Widget *popup;
  Etk_Widget *hbox;
  Etk_Widget *scrollview;
  Exo_Etk    *data;
  Etk_Tree_Col *col_pages;
  Etk_Tree_Col *col_index;

  data = (Exo_Etk *)calloc (sizeof (Exo_Etk), 1);
  if (!data) return;

  if (filename) {
    char *tmp;
    int   l;

    tmp = strrchr (filename, '/');
    if (tmp) {
      l = tmp - filename;
      data->path = (char *)malloc (sizeof (char) * (l + 1));
      memcpy (data->path, filename, l);
      data->path[l] = '\0';
      data->filename = strdup (tmp + 1);
    }
    else
      data->filename = strdup (filename);
  }
  data->pdf = etk_pdf_new ();
  data->list_pages = etk_tree_new ();
  data->list_index = etk_tree_new ();

  window = etk_window_new ();
  etk_window_title_set (ETK_WINDOW (window), "Exorcist");
  etk_window_wmclass_set (ETK_WINDOW (window), "Exorcist", "Exorcist");
/*   etk_widget_size_request_set (window, 300, 400); */
  etk_signal_connect ("delete_event", ETK_OBJECT (window),
                      ETK_CALLBACK (_exo_etk_delete_cb), data);

  vbox = etk_vbox_new (ETK_FALSE, 0);
  etk_container_add (ETK_CONTAINER (window), vbox);
  etk_widget_show (vbox);

  menu_bar = _exo_etk_menu_bar (data);
  etk_box_append (ETK_BOX (vbox), menu_bar, ETK_BOX_START, ETK_BOX_NONE, 0);
  etk_widget_show (menu_bar);

  hbox = etk_hbox_new (ETK_FALSE, 0);
  etk_box_append (ETK_BOX (vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_widget_show (hbox);

  etk_tree_headers_visible_set (ETK_TREE (data->list_pages), ETK_FALSE);
  etk_widget_size_request_set (data->list_pages, 60, -1);
  etk_signal_connect ("row_selected", ETK_OBJECT (data->list_pages),
                      ETK_CALLBACK (_change_page_cb), data->pdf);
  etk_tree_mode_set (ETK_TREE (data->list_pages), ETK_TREE_MODE_LIST);
  col_pages = etk_tree_col_new (ETK_TREE (data->list_pages), "",
                                etk_tree_model_int_new (ETK_TREE (data->list_pages)),
                                60);
  etk_tree_build (ETK_TREE (data->list_pages));
  etk_box_append (ETK_BOX (hbox), data->list_pages, ETK_BOX_START, ETK_BOX_FILL, 0);
  etk_widget_show (data->list_pages);

  popup = _exo_etk_menu_options (data);

  scrollview = etk_scrolled_view_new ();
  etk_box_append (ETK_BOX (hbox), scrollview, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_widget_show (scrollview);

  etk_scrolled_view_add_with_viewport (ETK_SCROLLED_VIEW (scrollview), data->pdf);
  etk_signal_connect("mouse_down", ETK_OBJECT(data->pdf),
                     ETK_CALLBACK(_exo_etk_menu_popup_cb), popup);
  etk_widget_show (data->pdf);

  etk_tree_headers_visible_set (ETK_TREE (data->list_index), ETK_FALSE);
  etk_widget_size_request_set (data->list_index, 60, -1);
  etk_tree_mode_set (ETK_TREE (data->list_index), ETK_TREE_MODE_TREE);
  col_index = etk_tree_col_new (ETK_TREE (data->list_index), "",
                                etk_tree_model_text_new (ETK_TREE (data->list_index)),
                                60);
  etk_tree_build (ETK_TREE (data->list_index));
  if (data->filename)
    _exo_etk_update_document (data, col_pages, col_index);

  etk_widget_show_all (window);

  printf ("%d\n", evas_object_visible_get(data->pdf->smart_object));

  _exo_etk_index_window (data);
}

static Etk_Bool
_exo_etk_delete_cb (Etk_Object *object, void *user_data)
{
  Exo_Etk *data;

  data = (Exo_Etk *)user_data;
  if (data->list_pages)
    etk_tree_clear (ETK_TREE (data->list_pages));
/*   if (data->list_index) */
    etk_tree_clear (ETK_TREE (data->list_index));

  etk_object_destroy (object);
  etk_main_quit ();

  if (data->path)
    free (data->path);
  if (data->filename)
    free (data->filename);
  free (data);

  return ETK_TRUE;
}

static void
_exo_etk_quit_cb (Etk_Object *object __UNUSED__, void *user_data)
{
  Exo_Etk *data;

  data = (Exo_Etk *)user_data;
  etk_tree_clear (ETK_TREE (data->list_pages));
  etk_tree_clear (ETK_TREE (data->list_index));

  etk_main_quit ();

  if (data->path)
    free (data->path);
  if (data->filename)
    free (data->filename);
  free (data);
}

static Etk_Bool
_exo_etk_index_delete_cb (Etk_Object *object, void *user_data __UNUSED__)
{
  etk_widget_hide (ETK_WIDGET (object));

  return ETK_TRUE;
}

static void
_exo_etk_menu_popup_cb (Etk_Object *object __UNUSED__, void *event_info __UNUSED__, void *data)
{
  etk_menu_popup (ETK_MENU (data));
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

static void
_exo_etk_document_info_cb (Etk_Object *object __UNUSED__, void *user_data)
{
  Exo_Etk        *data;
  Etk_Widget     *dialog_info;
  Etk_Widget     *table;
  Etk_Widget     *notebook;
  Etk_Widget     *label;
  Etk_Widget     *entry;
  Etk_Widget     *list;
  Ecore_List     *fonts;
  Epdf_Document  *doc;
  Epdf_Font_Info *font;

  data = (Exo_Etk *)user_data;
  doc = etk_pdf_pdf_document_get (ETK_PDF (data->pdf));

  if (!doc) return;

  /* FIXME: what is that "close" signal on dialog ? */

  dialog_info = etk_dialog_new ();
  etk_window_title_set (ETK_WINDOW (dialog_info), "Exorcist - Info");
  etk_window_wmclass_set (ETK_WINDOW (dialog_info), "Exorcist - Info", "Exorcist - Info");
  etk_signal_connect ("delete_event", ETK_OBJECT (dialog_info),
                      ETK_CALLBACK (_exo_etk_document_info_delete_cb), NULL);

  etk_dialog_button_add (ETK_DIALOG (dialog_info),
                         etk_stock_label_get (ETK_STOCK_DIALOG_OK), ETK_RESPONSE_OK);
  etk_signal_connect ("response", ETK_OBJECT (dialog_info),
                      ETK_CALLBACK (_exo_etk_document_info_response_cb), NULL);

  table = etk_table_new (2, 2, ETK_FALSE);
  etk_dialog_pack_in_main_area (ETK_DIALOG (dialog_info), table, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);
  etk_widget_show (table);

  label = etk_label_new ("Poppler version:");
  etk_table_attach_defaults (ETK_TABLE (table), label, 0, 0, 0, 0);
  etk_widget_show (label);

  entry = etk_entry_new ();
  etk_entry_text_set (ETK_ENTRY (entry), epdf_poppler_version_get ());
  etk_table_attach_defaults (ETK_TABLE (table), entry, 1, 1, 0, 0);
  etk_widget_show (entry);

  notebook = etk_notebook_new ();
  etk_table_attach_defaults (ETK_TABLE (table), notebook, 0, 1, 1, 1);
  etk_widget_show (notebook);

  {
    char  buf[16];
    Etk_Tree_Col *col_title;
    Etk_Tree_Col *col_prop;
    char *text;

    list = etk_tree_new ();
/*     etk_tree_headers_visible_set (ETK_TREE (list), ETK_FALSE); */
    etk_widget_size_request_set (list, 60, -1);
    etk_tree_mode_set (ETK_TREE (list), ETK_TREE_MODE_LIST);
    col_title = etk_tree_col_new (ETK_TREE (list), "",
                                  etk_tree_model_text_new (ETK_TREE (list)),
                                  60);
    col_prop = etk_tree_col_new (ETK_TREE (list), "",
                                 etk_tree_model_text_new (ETK_TREE (list)),
                                 60);
    etk_tree_build (ETK_TREE (list));
    etk_widget_show (list);

    etk_tree_freeze (ETK_TREE (list));

    etk_tree_append (ETK_TREE (list),
                     col_title, "File name",
                     col_prop, epdf_document_filename_get (doc),
                     NULL);

    text = epdf_document_title_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Title",
                     col_prop, text,
                     NULL);
    free (text);

    snprintf (buf, 16, "PDF-%.1f", epdf_document_pdf_version_get (doc));
    etk_tree_append (ETK_TREE (list),
                     col_title, "Format",
                     col_prop, buf,
                     NULL);

    text = epdf_document_author_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Author",
                     col_prop, text,
                     NULL);
    free (text);

    text = epdf_document_subject_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Subject",
                     col_prop, text,
                     NULL);
    free (text);

    text = epdf_document_keywords_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Keywords",
                     col_prop, text,
                     NULL);
    free (text);

    text = epdf_document_creator_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Creator",
                     col_prop, text,
                     NULL);
    free (text);

    text = epdf_document_producer_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Producer",
                     col_prop, text,
                     NULL);
    free (text);

    if (epdf_document_is_linearized (doc))
      text = "yes";
    else
      text = "no";
    etk_tree_append (ETK_TREE (list),
                     col_title, "Linearized",
                     col_prop, text,
                     NULL);

    etk_tree_append (ETK_TREE (list),
                     col_title, "Page mode",
                     col_prop, epdf_document_page_mode_string_get (doc),
                     NULL);

    etk_tree_append (ETK_TREE (list),
                     col_title, "Page layout",
                     col_prop, epdf_document_page_layout_string_get (doc),
                     NULL);

    text = epdf_document_creation_date_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Creation date",
                     col_prop, text,
                     NULL);
    free (text);

    text = epdf_document_mod_date_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Modification date",
                     col_prop, text,
                     NULL);
    free (text);

    snprintf (buf, 16, "%d", epdf_document_page_count_get (doc));
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Page count",
                     col_prop, buf,
                     NULL);

    etk_tree_thaw (ETK_TREE (list));

    etk_notebook_page_append (ETK_NOTEBOOK (notebook), "Document", list);
  }

  {
    Etk_Tree_Col *col_name;
    Etk_Tree_Col *col_type;
    Etk_Tree_Col *col_embedded;
    Etk_Tree_Col *col_subset;

    list = etk_tree_new ();
    etk_widget_size_request_set (list, 60, -1);
    etk_tree_mode_set (ETK_TREE (list), ETK_TREE_MODE_LIST);
    col_name = etk_tree_col_new (ETK_TREE (list), "Name",
                                 etk_tree_model_text_new (ETK_TREE (list)),
                                 60);
    col_type = etk_tree_col_new (ETK_TREE (list), "Type",
                                 etk_tree_model_text_new (ETK_TREE (list)),
                                 60);
    col_embedded = etk_tree_col_new (ETK_TREE (list), "Embedded",
                                     etk_tree_model_text_new (ETK_TREE (list)),
                                     60);
    col_subset = etk_tree_col_new (ETK_TREE (list), "Subset",
                                   etk_tree_model_text_new (ETK_TREE (list)),
                                   60);
    etk_tree_build (ETK_TREE (list));
    etk_widget_show (list);

    etk_tree_freeze (ETK_TREE (list));

    fonts = epdf_document_fonts_get (doc);
    ecore_list_first_goto (fonts);
    while ((font = ecore_list_next (fonts))) {
      etk_tree_append (ETK_TREE (list),
                       col_name, epdf_font_info_font_name_get (font),
                       col_type, epdf_font_info_type_name_get (font),
                       col_embedded, epdf_font_info_is_embedded_get (font) ? "yes" : "no",
                       col_subset, epdf_font_info_is_subset_get (font) ? "yes" : "no",
                       NULL);
    }
    ecore_list_destroy (fonts);

    etk_tree_thaw (ETK_TREE (list));

    etk_notebook_page_append (ETK_NOTEBOOK (notebook), "Fonts", list);
  }

  {
    Etk_Tree_Col *col_title;
    Etk_Tree_Col *col_prop;
    char  buf[16];
    char *text;
    Epdf_Page *page;
    Epdf_Page_Orientation o;

    page = etk_pdf_pdf_page_get (ETK_PDF (data->pdf));

    list = etk_tree_new ();
    etk_tree_headers_visible_set (ETK_TREE (list), ETK_FALSE);
    etk_widget_size_request_set (list, 60, -1);
    etk_tree_mode_set (ETK_TREE (list), ETK_TREE_MODE_LIST);
    col_title = etk_tree_col_new (ETK_TREE (list), "",
                                  etk_tree_model_text_new (ETK_TREE (list)),
                                  60);
    col_prop = etk_tree_col_new (ETK_TREE (list), "",
                                 etk_tree_model_text_new (ETK_TREE (list)),
                                 60);
    etk_tree_build (ETK_TREE (list));
    etk_widget_show (list);

    etk_tree_freeze (ETK_TREE (list));

    snprintf (buf, 16, "%d", etk_pdf_page_get (ETK_PDF (data->pdf)));
    etk_tree_append (ETK_TREE (list),
                     col_title, "Page number",
                     col_prop, buf,
                     NULL);

    snprintf (buf, 16, "%d x %d", epdf_page_width_get (page), epdf_page_height_get (page));
    etk_tree_append (ETK_TREE (list),
                     col_title, "Size (pixels)",
                     col_prop, buf,
                     NULL);

    o = epdf_page_orientation_get (page);
    switch (o) {
    case EPDF_PAGE_ORIENTATION_LANDSCAPE:
      text = "landscape";
      break;
    case EPDF_PAGE_ORIENTATION_UPSIDEDOWN:
      text = "upside down";
      break;
    case EPDF_PAGE_ORIENTATION_SEASCAPE:
      text = "seascape";
      break;
    case EPDF_PAGE_ORIENTATION_PORTRAIT:
      text = "portrait";
      break;
    default:
      text = "landscape";
      break;
    }
    etk_tree_append (ETK_TREE (list),
                     col_title, "Orientation",
                     col_prop, text,
                     NULL);

    etk_tree_thaw (ETK_TREE (list));

    etk_notebook_page_append (ETK_NOTEBOOK (notebook), "Fonts", list);
  }

  etk_widget_show (dialog_info);

}

static Etk_Bool
_exo_etk_document_info_delete_cb (Etk_Object *object, void *user_data __UNUSED__)
{
  etk_object_destroy (object);

  return ETK_TRUE;
}

static void
_exo_etk_document_info_response_cb (Etk_Object *object, int res, void *user_data __UNUSED__)
{
  switch (res) {
  case ETK_RESPONSE_OK:
    etk_object_destroy (object);
    break;
  }
}

static void
_exo_etk_search_cb (Etk_Object *object __UNUSED__, void *user_data)
{
  Exo_Etk    *data;
  Etk_Widget *dialog_search;
  Etk_Widget *table;
  Etk_Widget *label;

  data = (Exo_Etk *)user_data;

  /* FIXME: what is that "close" signal on dialog ? */

  dialog_search = etk_dialog_new ();
  etk_window_title_set (ETK_WINDOW (dialog_search), "Exorcist - Search");
  etk_window_wmclass_set (ETK_WINDOW (dialog_search), "Exorcist - Search", "Exorcist - Search");
  etk_signal_connect ("delete_event", ETK_OBJECT (dialog_search),
                      ETK_CALLBACK (_exo_etk_search_delete_cb), NULL);

  etk_dialog_button_add (ETK_DIALOG (dialog_search),
                         etk_stock_label_get (ETK_STOCK_DIALOG_CLOSE), ETK_RESPONSE_CLOSE);
  etk_dialog_button_add (ETK_DIALOG (dialog_search),
                         etk_stock_label_get (ETK_STOCK_GO_NEXT), ETK_RESPONSE_APPLY);
  etk_signal_connect ("response", ETK_OBJECT (dialog_search),
                      ETK_CALLBACK (_exo_etk_search_response_cb), user_data);

  table = etk_table_new (2, 3, ETK_FALSE);
  etk_dialog_pack_in_main_area (ETK_DIALOG (dialog_search), table, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);
  etk_widget_show (table);

  label = etk_label_new ("Word:");
  etk_table_attach_defaults (ETK_TABLE (table), label, 0, 0, 0, 0);
  etk_widget_show (label);

  data->search_text = etk_entry_new ();
  etk_table_attach_defaults (ETK_TABLE (table), data->search_text, 1, 1, 0, 0);
  etk_widget_show (data->search_text);

  data->search_is_case_sensitive = etk_check_button_new_with_label ("Case sensitive");
  etk_table_attach_defaults (ETK_TABLE (table), data->search_is_case_sensitive, 1, 2, 1, 1);
  etk_widget_show (data->search_is_case_sensitive);

  data->search_circular = etk_check_button_new_with_label ("Circular search");
  etk_table_attach_defaults (ETK_TABLE (table), data->search_circular, 1, 2, 2, 2);
  etk_widget_show (data->search_circular);

  etk_widget_show (dialog_search);
}

static Etk_Bool
_exo_etk_search_delete_cb (Etk_Object *object, void *user_data __UNUSED__)
{
  etk_object_destroy (object);

  return ETK_TRUE;
}

static void
_exo_etk_search_response_cb (Etk_Object *object, int res, void *user_data)
{
  switch (res) {
  case ETK_RESPONSE_CLOSE:
    etk_object_destroy (object);
    break;
  case ETK_RESPONSE_APPLY: {
    Exo_Etk    *data;
    Etk_Pdf    *pdf;
    const char *text;
    int         is_case_sensitive;
    int         is_circular;

    data = (Exo_Etk *)user_data;
    pdf =  ETK_PDF (data->pdf);
    text = etk_entry_text_get (ETK_ENTRY (data->search_text));
    if (!text || (text[0] == '\0'))
      return;
    is_case_sensitive = etk_toggle_button_active_get (ETK_TOGGLE_BUTTON (data->search_is_case_sensitive));
    is_circular = etk_toggle_button_active_get (ETK_TOGGLE_BUTTON (data->search_circular));
  etk_pdf_search_text_set (pdf, text);
  etk_pdf_search_is_case_sensitive (pdf, is_case_sensitive);
  printf ("we search...\n");
  res = etk_pdf_search_next (pdf);

  if (!res)
    printf ("FIN\n");
    break;
  }
  }
}

static void
_exo_etk_index_show_cb (Etk_Object *object, void *user_data)
{
  Exo_Etk *data;

  data = (Exo_Etk *)user_data;

  if (etk_menu_item_check_active_get (ETK_MENU_ITEM_CHECK (object)))
    etk_widget_show (data->win_index);
  else
    etk_widget_hide (data->win_index);
}

static void
_exo_etk_orientation_landscape_cb (Etk_Object *object, void *user_data)
{
  Etk_Pdf         *pdf;

  pdf =  ETK_PDF (((Exo_Etk *)user_data)->pdf);

  if (etk_menu_item_check_active_get (ETK_MENU_ITEM_CHECK (object))) {
    etk_pdf_orientation_set (pdf, EPDF_PAGE_ORIENTATION_LANDSCAPE);
  }
}

static void
_exo_etk_orientation_upsidedown_cb (Etk_Object *object, void *user_data)
{
  Etk_Pdf         *pdf;

  pdf =  ETK_PDF (((Exo_Etk *)user_data)->pdf);

  if (etk_menu_item_check_active_get (ETK_MENU_ITEM_CHECK (object))) {
    etk_pdf_orientation_set (pdf, EPDF_PAGE_ORIENTATION_UPSIDEDOWN);
  }
}

static void
_exo_etk_orientation_seascape_cb (Etk_Object *object, void *user_data)
{
  Etk_Pdf         *pdf;

  pdf =  ETK_PDF (((Exo_Etk *)user_data)->pdf);

  if (etk_menu_item_check_active_get (ETK_MENU_ITEM_CHECK (object))) {
    etk_pdf_orientation_set (pdf, EPDF_PAGE_ORIENTATION_SEASCAPE);
  }
}

static void
_exo_etk_orientation_portrait_cb (Etk_Object *object, void *user_data)
{
  Etk_Pdf         *pdf;

  pdf =  ETK_PDF (((Exo_Etk *)user_data)->pdf);

  if (etk_menu_item_check_active_get (ETK_MENU_ITEM_CHECK (object))) {
    etk_pdf_orientation_set (pdf, EPDF_PAGE_ORIENTATION_PORTRAIT);
  }
}

static void
_exo_etk_size_150_cb (Etk_Object *object, void *user_data)
{
  Etk_Pdf         *pdf;

  pdf =  ETK_PDF (((Exo_Etk *)user_data)->pdf);

  if (etk_menu_item_check_active_get (ETK_MENU_ITEM_CHECK (object))) {
    etk_pdf_scale_set (pdf, 1.5, 1.5);
  }
}

static void
_exo_etk_size_100_cb (Etk_Object *object, void *user_data)
{
  Etk_Pdf         *pdf;

  pdf =  ETK_PDF (((Exo_Etk *)user_data)->pdf);

  if (etk_menu_item_check_active_get (ETK_MENU_ITEM_CHECK (object))) {
    etk_pdf_scale_set (pdf, 1.0, 1.0);
  }
}

static void
_exo_etk_size_50_cb (Etk_Object *object, void *user_data)
{
  Etk_Pdf         *pdf;

  pdf =  ETK_PDF (((Exo_Etk *)user_data)->pdf);

  if (etk_menu_item_check_active_get (ETK_MENU_ITEM_CHECK (object))) {
    etk_pdf_scale_set (pdf, 0.5, 0.5);
  }
}

static void
_exo_etk_document_open_cb (Etk_Object *object __UNUSED__, void *user_data)
{
  Etk_Pdf         *pdf;

  pdf =  ETK_PDF (((Exo_Etk *)user_data)->pdf);
}

static void
_exo_etk_document_save_cb (Etk_Object *object __UNUSED__, void *user_data)
{
  char         buf[128];
  Exo_Etk     *data;
  Etk_Pdf     *pdf;
  Etk_Widget  *dialog_save;
  Etk_Widget  *table;
  Etk_Widget  *label;
  char        *tmp;
  const char  *filename;
  char        *psfilename;
  int          l;
  int          page_count;
  Exo_Ps_Data *ps_data;

  data = (Exo_Etk *)user_data;
  pdf =  ETK_PDF (data->pdf);
  filename = epdf_document_filename_get (pdf->pdf_document);
  l = strlen (filename);
  tmp = strstr (filename, ".pdf");
  if (!tmp) {
    psfilename = (char *)malloc (sizeof (char) * (l + 4));
    memcpy (psfilename, filename, l);
    psfilename[l + 0] = '.';
    psfilename[l + 1] = 'p';
    psfilename[l + 2] = 's';
    psfilename[l + 3] = '\0';
  }
  else {
    psfilename = (char *)malloc (sizeof (char) * l);
    memcpy (psfilename, filename, l);
    psfilename[l - 2] = 's';
    psfilename[l - 1] = '\0';
  }
  page_count = epdf_document_page_count_get (pdf->pdf_document);

  ps_data = (Exo_Ps_Data *)malloc (sizeof (Exo_Ps_Data));
  ps_data->pdf_doc = pdf->pdf_document;

  dialog_save = etk_dialog_new ();
  etk_window_title_set (ETK_WINDOW (dialog_save), "Exorcist - Save");
  etk_window_wmclass_set (ETK_WINDOW (dialog_save), "Exorcist - Save", "Exorcist - Save");
  etk_signal_connect ("delete_event", ETK_OBJECT (dialog_save),
                      ETK_CALLBACK (_exo_etk_document_save_delete_cb), NULL);

  etk_dialog_button_add (ETK_DIALOG (dialog_save),
                         etk_stock_label_get (ETK_STOCK_DIALOG_OK), ETK_RESPONSE_OK);

  etk_dialog_button_add (ETK_DIALOG (dialog_save),
                         etk_stock_label_get (ETK_STOCK_DIALOG_CANCEL), ETK_RESPONSE_CANCEL);
  etk_signal_connect ("response", ETK_OBJECT (dialog_save),
                      ETK_CALLBACK (_exo_etk_document_save_response_cb), ps_data);

  table = etk_table_new (2, 6, ETK_FALSE);
  etk_dialog_pack_in_main_area (ETK_DIALOG (dialog_save), table, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);
  etk_widget_show (table);

  label = etk_label_new ("File name");
  etk_table_attach_defaults (ETK_TABLE (table), label, 0, 0, 0, 0);
  etk_widget_show (label);

  ps_data->filename = etk_entry_new ();
  etk_entry_text_set (ETK_ENTRY (ps_data->filename), psfilename);
  etk_table_attach_defaults (ETK_TABLE (table), ps_data->filename, 1, 1, 0, 0);
  etk_widget_show (ps_data->filename);

  label = etk_label_new ("First page");
  etk_table_attach_defaults (ETK_TABLE (table), label, 0, 0, 1, 1);
  etk_widget_show (label);

  ps_data->first_page = etk_entry_new ();
  etk_entry_text_set (ETK_ENTRY (ps_data->first_page), "1");
  etk_table_attach_defaults (ETK_TABLE (table), ps_data->first_page, 1, 1, 1, 1);
  etk_widget_show (ps_data->first_page);

  label = etk_label_new ("Last page");
  etk_table_attach_defaults (ETK_TABLE (table), label, 0, 0, 2, 2);
  etk_widget_show (label);

  snprintf (buf, 128, "%d", page_count);
  ps_data->last_page = etk_entry_new ();
  etk_entry_text_set (ETK_ENTRY (ps_data->last_page), buf);
  etk_table_attach_defaults (ETK_TABLE (table), ps_data->last_page, 1, 1, 2, 2);
  etk_widget_show (ps_data->last_page);

  label = etk_label_new ("Width");
  etk_table_attach_defaults (ETK_TABLE (table), label, 0, 0, 3, 3);
  etk_widget_show (label);

  ps_data->width = etk_entry_new ();
  etk_entry_text_set (ETK_ENTRY (ps_data->width), "-1");
  etk_table_attach_defaults (ETK_TABLE (table), ps_data->width, 1, 1, 3, 3);
  etk_widget_show (ps_data->width);

  label = etk_label_new ("Height");
  etk_table_attach_defaults (ETK_TABLE (table), label, 0, 0, 4, 4);
  etk_widget_show (label);

  ps_data->height = etk_entry_new ();
  etk_entry_text_set (ETK_ENTRY (ps_data->height), "-1");
  etk_table_attach_defaults (ETK_TABLE (table), ps_data->height, 1, 1, 4, 4);
  etk_widget_show (ps_data->height);

  ps_data->duplex = etk_check_button_new_with_label ("Duplex");
  etk_toggle_button_active_set (ETK_TOGGLE_BUTTON (ps_data->duplex), ETK_TRUE);
  etk_table_attach_defaults (ETK_TABLE (table), ps_data->duplex, 0, 1, 5, 5);
  etk_widget_show (ps_data->duplex);

  etk_widget_show (dialog_save);

  free (psfilename);
}

static Etk_Bool
_exo_etk_document_save_delete_cb (Etk_Object *object, void *user_data __UNUSED__)
{
  etk_object_destroy (object);

  return ETK_TRUE;
}

static void
_exo_etk_document_save_response_cb (Etk_Object *object, int res, void *user_data)
{
switch (res) {
  case ETK_RESPONSE_OK: {
    Epdf_Postscript *eps;
    Exo_Ps_Data     *ps_data = (Exo_Ps_Data *)user_data;
    const char      *filename;
    int              page_first;
    int              page_last;
    int              width;
    int              height;
    int              duplex;

    filename = etk_entry_text_get (ETK_ENTRY (ps_data->filename));
    page_first = atoi (etk_entry_text_get (ETK_ENTRY (ps_data->first_page)));
    page_last = atoi (etk_entry_text_get (ETK_ENTRY (ps_data->last_page)));
    width = atoi (etk_entry_text_get (ETK_ENTRY (ps_data->width)));
    height = atoi (etk_entry_text_get (ETK_ENTRY (ps_data->height)));
    duplex = etk_toggle_button_active_get (ETK_TOGGLE_BUTTON (ps_data->duplex));
    eps = epdf_postscript_new (ps_data->pdf_doc, filename, page_first, page_last);
    epdf_postscript_size_set (eps, width, height);
    epdf_postscript_duplex_set (eps, duplex);
    if (!eps) break;
    epdf_postscript_print (eps);
    epdf_postscript_delete (eps);
    break;
  }
  case ETK_RESPONSE_CANCEL:
    break;
  }
  etk_object_destroy (object);
}
