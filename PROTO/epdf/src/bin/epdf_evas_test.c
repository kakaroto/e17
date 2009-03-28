#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "Epdf.h"

static void
document_info_print (Epdf_Document *document, Epdf_Page *page);

static void app_resize(Ecore_Evas *ee);
static int app_signal_exit(void *data, int ev_type, void *ev);
static void app_delete_request(Ecore_Evas *ee);

int
main (int argc, char *argv[])
{
  Ecore_Evas    *ee;
  Evas          *evas;
  Evas_Object   *o;
  Epdf_Document *document;
  Epdf_Page     *page;
  int            page_number;
  int            width;
  int            height;

  if (argc < 3) {
    printf ("\nUsage: %s filename page_number\n\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (!epdf_init ()) {
    printf ("Epdf can't be initialized\n");
    return EXIT_FAILURE;
  }

  document = epdf_document_new (argv[1]);
  if (!document) {
    printf ("Bad pdf file\n");
    goto epdf_shutdown;
  }

  sscanf (argv[2], "%d", &page_number);
  if (page_number >= epdf_document_page_count_get (document)) {
    printf ("Page number exceeds the page count of the PDF document\n");
    goto document_delete;
  }

  page = epdf_page_new (document);
  if (!page) {
    printf ("Bad page\n");
    goto document_delete;
  }
  epdf_page_page_set (page, page_number);
  epdf_page_size_get (page, &width, &height);

  document_info_print (document, page);

  if (!ecore_evas_init())
    goto page_delete;

  ee = ecore_evas_new ("software_x11",  0, 0, width, height, NULL);
  /* these tests can be improved... */
  if (!ee) {
    printf ("Can not find Software X11 engine. Trying DirectDraw engine...\n");
    ee = ecore_evas_software_ddraw_new (NULL,  0, 0, width, height);
    if (!ee) {
      printf ("Can not find DirectDraw engine...\n");
      printf ("Exiting...\n");
      goto ecore_evas_shutdown;
    }
  }
  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);
  ecore_evas_callback_delete_request_set (ee, app_delete_request);
  ecore_evas_title_set (ee, "Evas Pdf Test");
  ecore_evas_name_class_set (ee, "evas_pdf_test", "test_evas_pdf");
  ecore_evas_callback_resize_set (ee, app_resize);
  ecore_evas_show (ee);

  evas = ecore_evas_get (ee);

  o = evas_object_image_add (evas);
  evas_object_move (o, 0, 0);
  epdf_page_render (page, o);
  evas_object_show (o);

  ecore_main_loop_begin ();

  epdf_page_delete (page);
  epdf_document_delete (document);

  ecore_evas_shutdown ();
  epdf_shutdown ();

  return EXIT_SUCCESS;

 ecore_evas_shutdown:
  ecore_evas_shutdown ();
 page_delete:
  epdf_page_delete (page);
 document_delete:
  epdf_document_delete (document);
 epdf_shutdown:
  epdf_shutdown ();

  return EXIT_FAILURE;
}

static void display_index (Epdf_Document *document, Eina_List *children, int n)
{
  Epdf_Index_Item *item;
  Eina_List       *l;

  if (!children)
    return;

  EINA_LIST_FOREACH(children, l, item) {
    char *buf;
    char  buf2[64];
    int page;
    Eina_List *c;

    buf = (char *)malloc (sizeof (char) * 2 * n + 1);
    memset (buf, ' ', 2 * n);
    buf[2 * n] = '\0';
    page = epdf_index_item_page_get (document, item);
    if (page == -1)
      snprintf (buf2, 64, "no dest. page");
    else
      snprintf (buf2, 64, "page %d", page);
    printf ("%s%s (%s)\n", buf, epdf_index_item_title_get (item), buf2);
    free (buf);
    c = epdf_index_item_children_get (item);
    if (c)
      display_index (document, c, n + 1);
  }
}

static void
document_info_print (Epdf_Document *document, Epdf_Page *page)
{
  Eina_List      *fonts;
  Eina_List      *l;
  Eina_List      *index;
  Epdf_Font_Info *font;
  char           *title;
  char           *author;
  char           *subject;
  char           *keywords;
  char           *creator;
  char           *producer;
  const char     *linearized;
  char           *creation_date;
  char           *mod_date;
  const char     *page_mode;
  const char     *page_layout;
  const char     *orientation;
  int             width;
  int             height;

  printf ("\n");
  printf ("  Poppler version....: %s\n", epdf_poppler_version_get ());
  printf ("\n");

  printf ("  Document Metadata:\n");
  printf ("\n");

  title = epdf_document_title_get (document);
  if (!title)
    title = strdup ("(none)");
  author = epdf_document_author_get (document);
  if (!author)
    author = strdup ("(none)");
  subject = epdf_document_subject_get (document);
  if (!subject)
    subject = strdup ("(none)");
  keywords = epdf_document_keywords_get (document);
  if (!keywords)
    keywords = strdup ("(none)");
  creator = epdf_document_creator_get (document);
  if (!creator)
    creator = strdup ("(none)");
  producer = epdf_document_producer_get (document);
  if (!producer)
    producer = strdup ("(none)");
  linearized = epdf_document_linearized_get (document);
  creation_date = epdf_document_creation_date_get (document);
  if (!creation_date)
    creation_date = strdup ("(none)");
  mod_date = epdf_document_mod_date_get (document);
  if (!mod_date)
    mod_date = strdup ("(none)");
  page_mode = epdf_document_page_mode_string_get (document);
  page_layout = epdf_document_page_layout_string_get (document);

  printf ("  File name..........: %s\n", epdf_document_filename_get (document));
  printf ("  Title..............: %s\n", title);
  printf ("  Format.............: PDF-%.1f\n", epdf_document_pdf_version_get (document));
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

  printf ("  Page count.........: %d\n", epdf_document_page_count_get (document));

  printf ("\n");

  printf ("  Fonts:\n");
  printf ("\n");

  fonts = epdf_document_fonts_get (document);
  EINA_LIST_FOREACH(fonts, l, font) {
    printf ("    %s (", epdf_font_info_font_name_get (font));
    printf ("%s, ", epdf_font_info_font_path_get (font));
    printf ("%s, ", epdf_font_info_type_name_get (font));
    if (!epdf_font_info_is_embedded_get (font))
      printf ("not ");
    printf ("embedded, ");
    if (!epdf_font_info_is_subset_get (font))
      printf ("not ");
    printf ("subset)\n");
  }
  while (fonts) {
    Epdf_Font_Info *font;

    font = eina_list_data_get (fonts);
    epdf_font_info_delete (font);
    fonts = eina_list_remove_list(fonts, fonts);
  }
  printf ("\n");

  printf ("  Page Properties:\n");
  printf ("\n");

  epdf_page_size_get (page, &width, &height);

  printf ("  Number.............: %d\n", epdf_page_page_get (page));
  printf ("  Size (pixels)......: %d x %d\n", width, height);
  switch (epdf_page_orientation_get (page))
    {
    case EPDF_PAGE_ORIENTATION_PORTRAIT:
      orientation = "portrait";
      break;
    case EPDF_PAGE_ORIENTATION_LANDSCAPE:
      orientation = "landscape";
      break;
    case EPDF_PAGE_ORIENTATION_UPSIDEDOWN:
      orientation = "upside down";
      break;
    case EPDF_PAGE_ORIENTATION_SEASCAPE:
      orientation = "seascape";
      break;
    }
  printf ("  Orientation........: %s\n", orientation);

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

  index = epdf_index_new (document);
  display_index (document, index, 0);
  epdf_index_delete (index);
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
