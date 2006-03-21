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
  char *path;
  char       *filename;
  Etk_Widget *pdf;
  Etk_Widget *list_pages;
  Etk_Widget *list_index;
  Etk_Widget *win_index;

  Etk_Widget *search_text;
  Etk_Widget *search_is_case_sensitive;
  Etk_Widget *search_circular;
};

static void     _exo_etk_quit_cb            (Etk_Object *object, void *data);
static Etk_Bool _exo_etk_delete_cb          (Etk_Object *object, void *user_data);
static void     _exo_etk_menu_popup_cb      (Etk_Object *object, void *event_info, void *data);
static void     _change_page_cb             (Etk_Object *object, Etk_Tree_Row *row, void *data);
static void     _exo_etk_row_data_free_cb   (Etk_Object *object);
static void     _exo_etk_info_cb            (Etk_Object *object, void *user_data);
static Etk_Bool _exo_etk_info_delete_cb     (Etk_Object *object, void *user_data);
static void     _exo_etk_info_response_cb   (Etk_Object *object, int res, void *user_data);
static void     _exo_etk_search_cb          (Etk_Object *object, void *user_data);
static void     _exo_etk_search_response_cb (Etk_Object *object, int res, void *user_data);


static void
_exo_etk_tree_fill (Etk_Pdf *pdf, Etk_Tree *tree, Etk_Tree_Col *col, Etk_Tree_Row *row, Ecore_List *items)
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
/*       etk_signal_connect ("destroyed", ETK_OBJECT (row), */
/*                           ETK_CALLBACK(_exo_etk_row_data_free_cb), NULL); */
      c = evas_poppler_index_item_children_get (item);
      if (c)
        {
          _exo_etk_tree_fill (pdf, tree, col, prow, c);
        }
    }
}

static void
_exo_etk_update_document (Exo_Etk      *data,
                          Etk_Tree_Col *col)
{
  Evas_Poppler_Document   *document;
  Ecore_List              *index;
  int                      page_count;
  int                      i;

  if (!data || !data->filename) return;

/*   etk_container_reset (ETK_CONTAINER (data->list_pages)); */
/*   etk_container_reset (ETK_CONTAINER (data->list_index)); */

  printf ("page : %d %s\n", etk_pdf_page_get (ETK_PDF (data->pdf)), data->filename);
  etk_pdf_file_set (ETK_PDF (data->pdf), data->filename);
  document = etk_pdf_pdf_document_get (ETK_PDF (data->pdf));
  if (!document) return;

  page_count = evas_poppler_document_page_count_get (document);
  etk_tree_freeze (ETK_TREE (data->list_pages));
  for (i = 0; i < page_count; i++)
    {
      Etk_Tree_Row *row;
      int          *num;

      row = etk_tree_append (ETK_TREE (data->list_pages), col, i + 1, NULL);
      num = (int *)malloc (sizeof (int));
      *num = i;
      etk_tree_row_data_set (row, num);
/*       etk_signal_connect ("destroyed", ETK_OBJECT (row), */
/*                           ETK_CALLBACK(_exo_etk_row_data_free_cb), NULL); */
    }
  etk_tree_thaw (ETK_TREE (data->list_pages));

  index = etk_pdf_pdf_index_get (ETK_PDF (data->pdf));
  _exo_etk_tree_fill (ETK_PDF (data->pdf), ETK_TREE (data->list_index), col, NULL, index);
}

void
_exo_etk_index_window (Exo_Etk *data)
{
  char title[4096];

  snprintf (title, 4096, "Exorcist - %s - Index", data->filename);

  data->win_index = etk_window_new ();
  etk_window_title_set (ETK_WINDOW (data->win_index), title);
  etk_window_wmclass_set (ETK_WINDOW (data->win_index), title, title);

  etk_tree_headers_visible_set (ETK_TREE (data->list_index), ETK_FALSE);
  etk_container_add (ETK_CONTAINER (data->win_index), data->list_index);
  etk_widget_show (data->list_index);
}

static Etk_Widget *
_exo_etk_menu_options (Exo_Etk *data)
{
  Etk_Widget *menu;
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

  menu_item = etk_menu_item_new_with_label ("Index");
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));

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
/*   etk_signal_connect("selected", ETK_OBJECT(menu_item), */
/*                      ETK_CALLBACK(_exo_etk_document_open_cb), data); */

  menu_item = etk_menu_item_new_with_label ("Info");
  etk_menu_shell_append (ETK_MENU_SHELL (menu), ETK_MENU_ITEM (menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item),
                     ETK_CALLBACK(_exo_etk_info_cb), data);

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
exo_etk_main_window (char *filename)
{
  Etk_Widget *window;
  Etk_Widget *vbox;
  Etk_Widget *menu_bar;
  Etk_Widget *popup;
  Etk_Widget *hbox;
  Exo_Etk    *data;
  Etk_Tree_Col *col;

  data = (Exo_Etk *)calloc (sizeof (Exo_Etk), 1);
  if (!data) return;

  if (filename)
    data->filename = strdup (filename);
  data->pdf = etk_pdf_new ();
  data->list_pages = etk_tree_new ();
  data->list_index = etk_tree_new ();

  window = etk_window_new ();
  etk_window_title_set (ETK_WINDOW (window), "Exorcist");
  etk_window_wmclass_set (ETK_WINDOW (window), "Exorcist", "Exorcist");
  etk_signal_connect ("delete_event", ETK_OBJECT (window),
                      ETK_CALLBACK (_exo_etk_delete_cb), data);

  vbox = etk_vbox_new (ETK_FALSE, 0);
  etk_container_add (ETK_CONTAINER (window), vbox);
  etk_widget_show (vbox);

  menu_bar = _exo_etk_menu_bar (data);
  etk_box_pack_start (ETK_BOX (vbox), menu_bar, ETK_FALSE, ETK_FALSE, 0);
  etk_widget_show (menu_bar);

  hbox = etk_hbox_new (ETK_FALSE, 0);
  etk_box_pack_start (ETK_BOX (vbox), hbox, ETK_FALSE, ETK_FALSE, 0);
  etk_widget_show (hbox);

  etk_tree_headers_visible_set (ETK_TREE (data->list_pages), ETK_FALSE);
  etk_widget_size_request_set (data->list_pages, 60, -1);
  etk_signal_connect ("row_selected", ETK_OBJECT (data->list_pages),
                      ETK_CALLBACK (_change_page_cb), data->pdf);
  etk_tree_mode_set (ETK_TREE (data->list_pages), ETK_TREE_MODE_LIST);
  col = etk_tree_col_new (ETK_TREE (data->list_pages), "",
                          etk_tree_model_int_new (ETK_TREE (data->list_pages)),
                          60);
  etk_tree_build (ETK_TREE (data->list_pages));
  etk_box_pack_start (ETK_BOX (hbox), data->list_pages, ETK_FALSE, ETK_FALSE, 0);
  etk_widget_show (data->list_pages);

  popup = _exo_etk_menu_options (data);

  etk_box_pack_start (ETK_BOX (hbox), data->pdf, ETK_FALSE, ETK_FALSE, 0);
  etk_signal_connect("mouse_down", ETK_OBJECT(data->pdf),
                     ETK_CALLBACK(_exo_etk_menu_popup_cb), popup);
  etk_widget_show (data->pdf);

  if (data->filename)
    _exo_etk_update_document (data, col);

  etk_widget_show (window);

  _exo_etk_index_window (data);
}

static Etk_Bool
_exo_etk_delete_cb (Etk_Object *object, void *user_data)
{
  Exo_Etk *data;

  data = (Exo_Etk *)user_data;
  etk_object_destroy (object);
  etk_main_quit ();
  if (data->filename)
    free (data->filename);
  free (data);

  return ETK_TRUE;
}

static void
_exo_etk_quit_cb (Etk_Object *object __UNUSED__, void *user_data)
{
  Exo_Etk *data;

  etk_main_quit ();
  data = (Exo_Etk *)user_data;
  if (data->filename)
    free (data->filename);
  free (data);
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
_exo_etk_row_data_free_cb  (Etk_Object *object)
{
  int *row_number;

  if (!object)
    return;
  row_number = (int *)etk_tree_row_data_get ((Etk_Tree_Row *)object);
  if (row_number) free (row_number);
}

static void
_exo_etk_info_cb (Etk_Object *object __UNUSED__, void *user_data)
{
  Exo_Etk               *data;
  Etk_Widget            *dialog_info;
  Etk_Widget            *table;
  Etk_Widget            *notebook;
  Etk_Widget            *label;
  Etk_Widget            *entry;
  Etk_Widget            *list;
  Ecore_List            *fonts;
  Evas_Poppler_Document *doc;
  Evas_Poppler_Font_Info *font;

  data = (Exo_Etk *)user_data;
  doc = etk_pdf_pdf_document_get (ETK_PDF (data->pdf));

  if (!doc) return;

  /* FIXME: what is that "close" signal on dialog ? */

  dialog_info = etk_dialog_new ();
  etk_window_title_set (ETK_WINDOW (dialog_info), "Exorcist - Info");
  etk_window_wmclass_set (ETK_WINDOW (dialog_info), "Exorcist - Info", "Exorcist - Info");
  etk_signal_connect ("delete_event", ETK_OBJECT (dialog_info),
                      ETK_CALLBACK (_exo_etk_info_delete_cb), NULL);

  etk_dialog_button_add (ETK_DIALOG (dialog_info),
                         etk_stock_label_get (ETK_STOCK_DIALOG_CANCEL), ETK_RESPONSE_CLOSE);
  etk_signal_connect ("response", ETK_OBJECT (dialog_info),
                      ETK_CALLBACK (_exo_etk_info_response_cb), NULL);

  table = etk_table_new (2, 2, ETK_FALSE);
  etk_dialog_pack_in_main_area (ETK_DIALOG (dialog_info), table, ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);
  etk_widget_show (table);

  label = etk_label_new ("Poppler version:");
  etk_table_attach_defaults (ETK_TABLE (table), label, 0, 0, 0, 0);
  etk_widget_show (label);

  entry = etk_entry_new ();
  printf ("version %s\n", evas_poppler_version_get ());
  etk_entry_text_set (ETK_ENTRY (entry), "toto");
/*   etk_entry_text_set (ETK_ENTRY (entry), evas_poppler_version_get ()); */
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

    etk_tree_append (ETK_TREE (list),
                     col_title, "File name",
                     col_prop, evas_poppler_document_filename_get (doc),
                     NULL);

    text = evas_poppler_document_title_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Title",
                     col_prop, text,
                     NULL);
    free (text);

    snprintf (buf, 16, "PDF-%.1f", evas_poppler_document_pdf_version_get (doc));
    etk_tree_append (ETK_TREE (list),
                     col_title, "Format",
                     col_prop, buf,
                     NULL);

    text = evas_poppler_document_author_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Author",
                     col_prop, text,
                     NULL);
    free (text);

    text = evas_poppler_document_subject_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Subject",
                     col_prop, text,
                     NULL);
    free (text);

    text = evas_poppler_document_keywords_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Keywords",
                     col_prop, text,
                     NULL);
    free (text);

    text = evas_poppler_document_creator_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Creator",
                     col_prop, text,
                     NULL);
    free (text);

    text = evas_poppler_document_producer_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Producer",
                     col_prop, text,
                     NULL);
    free (text);

    if (evas_poppler_document_is_linearized (doc))
      text = "yes";
    else
      text = "no";
    etk_tree_append (ETK_TREE (list),
                     col_title, "Linearized",
                     col_prop, text,
                     NULL);

    etk_tree_append (ETK_TREE (list),
                     col_title, "Page mode",
                     col_prop, evas_poppler_document_page_mode_string_get (doc),
                     NULL);

    etk_tree_append (ETK_TREE (list),
                     col_title, "Page layout",
                     col_prop, evas_poppler_document_page_layout_string_get (doc),
                     NULL);

    text = evas_poppler_document_creation_date_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Creation date",
                     col_prop, text,
                     NULL);
    free (text);

    text = evas_poppler_document_mod_date_get (doc);
    if (!text) text = strdup ("unknown");
    etk_tree_append (ETK_TREE (list),
                     col_title, "Modification date",
                     col_prop, text,
                     NULL);
    free (text);

    snprintf (buf, 16, "%d", evas_poppler_document_page_count_get (doc));
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

    fonts = evas_poppler_document_fonts_get (doc);
    ecore_list_goto_first (fonts);
    while ((font = ecore_list_next (fonts))) {
      etk_tree_append (ETK_TREE (list),
                       col_name, evas_poppler_font_info_font_name_get (font),
                       col_type, evas_poppler_font_info_type_name_get (font),
                       col_embedded, evas_poppler_font_info_is_embedded_get (font) ? "yes" : "no",
                       col_subset, evas_poppler_font_info_is_subset_get (font) ? "yes" : "no",
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
    Evas_Poppler_Page *page;
    Evas_Poppler_Page_Orientation o;

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

    snprintf (buf, 16, "%d x %d", evas_poppler_page_width_get (page), evas_poppler_page_height_get (page));
    etk_tree_append (ETK_TREE (list),
                     col_title, "Size (pixels)",
                     col_prop, buf,
                     NULL);

    o = evas_poppler_page_orientation_get (page);
    switch (o) {
    case EVAS_POPPLER_PAGE_ORIENTATION_LANDSCAPE:
      text = "landscape";
      break;
    case EVAS_POPPLER_PAGE_ORIENTATION_UPSIDEDOWN:
      text = "upside down";
      break;
    case EVAS_POPPLER_PAGE_ORIENTATION_SEASCAPE:
      text = "seascape";
      break;
    case EVAS_POPPLER_PAGE_ORIENTATION_PORTRAIT:
      text = "portrait";
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
_exo_etk_info_delete_cb (Etk_Object *object, void *user_data __UNUSED__)
{
  etk_object_destroy (object);

  return ETK_TRUE;
}

static void
_exo_etk_info_response_cb (Etk_Object *object, int res, void *user_data __UNUSED__)
{
  printf ("res : %d\n", res, ETK_RESPONSE_CLOSE);
  switch (res) {
  case ETK_RESPONSE_CLOSE:
    etk_object_destroy (object);
    break;
  }
}

static void
_exo_etk_search_cb (Etk_Object *object, void *user_data)
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
/*   etk_signal_connect ("delete_event", ETK_OBJECT (dialog_search), */
/*                       ETK_CALLBACK (_exo_etk_search_delete_cb), NULL); */

  etk_dialog_button_add (ETK_DIALOG (dialog_search),
                         etk_stock_label_get (ETK_STOCK_DIALOG_CANCEL), ETK_RESPONSE_CLOSE);
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

static void
_exo_etk_search_response_cb (Etk_Object *object, int res, void *user_data)
{
  printf ("response !\n");
}
