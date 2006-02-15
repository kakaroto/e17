#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "Evas_Poppler.h"

static void
document_info_print (Evas_Poppler_Document *document, Evas_Poppler_Page *page);

static void app_resize(Ecore_Evas *ee);
static int app_signal_exit(void *data, int ev_type, void *ev);
static void app_delete_request(Ecore_Evas *ee);

int
main (int argc, char *argv[])
{
  Ecore_Evas *ee;
  Evas *evas;
  Evas_Object *o;
  Evas_Poppler_Document *document;
  Evas_Poppler_Page *page;
  int page_number;

  if (argc < 3) {
    printf ("\nUsage: %s filename page_number\n\n", argv[0]);
    exit (-1);
  }

  document = evas_poppler_document_new (argv[1]);
  if (!document)
    {
      printf ("Bad pdf file\n");
      exit (-1);
    }

  sscanf (argv[2], "%d", &page_number);
  if (page_number >= evas_poppler_document_page_count_get (document)) {
    printf ("Page number exceeds the page count of the PDF document\n");
    evas_poppler_document_delete (document);
    exit (-1);
  }

  page = evas_poppler_document_page_get (document, page_number);
  if (!page) {
    printf ("Bad page\n");
    evas_poppler_document_delete (document);
    exit (-1);
  }

  document_info_print (document, page);

  if (!evas_init()) return -1;
  if (!ecore_init()) {
    evas_shutdown ();
    return -1;
  }
  if (!ecore_evas_init()) {
    evas_shutdown ();
    ecore_shutdown ();
    return -1;
  }

  ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 600, 850);
  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);
  ecore_evas_callback_delete_request_set(ee, app_delete_request);
  ecore_evas_title_set(ee, "Evas Pdf Test");
  ecore_evas_name_class_set(ee, "evas_pdf_test", "test_evas_pdf");
  ecore_evas_callback_resize_set(ee, app_resize);
  ecore_evas_show(ee);
  
  evas = ecore_evas_get(ee);

  o = evas_object_image_add (evas);
  evas_object_move(o, 0, 0);
  evas_poppler_page_render (page, o, 0, 0, 0, 0, 72.0, 72.0);
  evas_object_show(o);

  ecore_main_loop_begin ();

  evas_poppler_page_delete (page);
  evas_poppler_document_delete (document);
   
  ecore_evas_shutdown ();
  ecore_shutdown ();
  evas_shutdown ();

  return 0;
}

static void display_index (Evas_Poppler_Document *document, Ecore_List *children, int n)
{
  Evas_Poppler_Index_Item *item;

  if (!children)
    return;
  
  ecore_list_goto_first (children);
  while ((item = ecore_list_next (children))) {
    char *buf;
    char  buf2[64];
    int page;
    Ecore_List *c;

    buf = (char *)malloc (sizeof (char) * 2 * n + 1);
    memset (buf, ' ', 2 * n);
    buf[2 * n] = '\0';
    page = evas_poppler_index_item_page_get (document, item);
    if (page == -1)
      snprintf (buf2, 64, "no dest. page");
    else
      snprintf (buf2, 64, "page %d", page);
    printf ("%s%s (%s)\n", buf, evas_poppler_index_item_title_get (item), buf2);
    free (buf);
    c = evas_poppler_index_item_children_get (item);
    if (c)
      display_index (document, c, n + 1);
  }
}

static void
document_info_print (Evas_Poppler_Document *document, Evas_Poppler_Page *page)
{
  Ecore_List             *fonts;
  Ecore_List             *index;
  Evas_Poppler_Font_Info *font;
  char                   *title;
  char                   *author;
  char                   *subject;
  char                   *keywords;
  char                   *creator;
  char                   *producer;
  const char             *linearized;
  char                   *creation_date;
  char                   *mod_date;
  const char             *page_mode;
  const char             *page_layout;

  printf ("\n");
  printf ("  Poppler version....: %s\n", evas_poppler_version_get ());
  printf ("\n");
      
  printf ("  Document Metadata:\n");
  printf ("\n");

  title = evas_poppler_document_title_get (document);
  if (!title)
    title = strdup ("(none)");
  author = evas_poppler_document_author_get (document);
  if (!author)
    author = strdup ("(none)");
  subject = evas_poppler_document_subject_get (document);
  if (!subject)
    subject = strdup ("(none)");
  keywords = evas_poppler_document_keywords_get (document);
  if (!keywords)
    keywords = strdup ("(none)");
  creator = evas_poppler_document_creator_get (document);
  if (!creator)
    creator = strdup ("(none)");
  producer = evas_poppler_document_producer_get (document);
  if (!producer)
    producer = strdup ("(none)");
  linearized = evas_poppler_document_linearized_get (document);
  creation_date = evas_poppler_document_creation_date_get (document);
  if (!creation_date)
    creation_date = strdup ("(none)");
  mod_date = evas_poppler_document_mod_date_get (document);
  if (!mod_date)
    mod_date = strdup ("(none)");
  page_mode = evas_poppler_document_page_mode_string_get (document);
  page_layout = evas_poppler_document_page_layout_string_get (document);

  printf ("  File name..........: %s\n", evas_poppler_document_filename_get (document));
  printf ("  Title..............: %s\n", title);
  printf ("  Format.............: PDF-%.1f\n", evas_poppler_document_pdf_version_get (document));
  printf ("  Author.............: %s\n", author);
  printf ("  Subject............: %s\n", subject);
  printf ("  Keywords...........: %s\n", keywords);
  printf ("  Creator............: %s\n", creator);
  printf ("  Producer...........: %s\n", producer);
  printf ("  Linearized.........: %s\n", linearized);
  printf ("  Page mode..........: %s\n", page_mode);
  printf ("  Page layout........: %s\n", page_layout);
  printf ("  Creation date......: %s\n", creation_date);
  printf ("  Modification date..: %s\n", mod_date);

  printf ("\n");
      
  printf ("  Document Properties:\n");
  printf ("\n");

  printf ("  Page count.........: %d\n", evas_poppler_document_page_count_get (document));

  printf ("\n");

  printf ("  Fonts:\n");
  printf ("\n");

  fonts = evas_poppler_document_fonts_get (document);
  ecore_list_goto_first (fonts);
  while ((font = ecore_list_next (fonts))) {
    printf ("    %s (", evas_poppler_font_info_font_name_get (font));
    printf ("%s, ", evas_poppler_font_info_type_name_get (font));
    if (!evas_poppler_font_info_is_embedded_get (font))
      printf ("not ");
    printf ("embedded, ");
    if (!evas_poppler_font_info_is_subset_get (font))
      printf ("not ");
    printf ("subset)\n");
  }
  ecore_list_destroy (fonts);
  printf ("\n");

  printf ("  Page Properties:\n");
  printf ("\n");

  printf ("  Number.............: %d\n", evas_poppler_page_number_get (page));
  printf ("  Size (pixels)......: %d x %d\n", evas_poppler_page_width_get (page), evas_poppler_page_height_get (page));
  printf ("  Orientation........: %s\n", evas_poppler_page_orientation_name_get (page));

  printf ("\n");
  
  if (title)
    free (title);
  if (author)
    free (author);
  if (subject)
    free (subject);
  if (keywords)
    free (keywords);
  if (creator)
    free (creator);
  if (producer)
    free (producer);

  if (creation_date)
    free (creation_date);
  if (mod_date)
    free (mod_date);

  index = evas_poppler_index_new (document);
  display_index (document, index, 0);
  evas_poppler_index_delete (index);
}

static void
app_resize(Ecore_Evas *ee)
{
   Evas_Coord w, h;
   Evas *evas;
   
   evas = ecore_evas_get(ee);
   evas_output_viewport_get(evas, NULL, NULL, &w, &h);
/*    bg_resize(w, h); */
}

static int
app_signal_exit(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
   return 1;
}

static void
app_delete_request(Ecore_Evas *ee)
{
   ecore_main_loop_quit();
}
