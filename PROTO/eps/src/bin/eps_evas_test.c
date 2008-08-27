#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "Eps.h"

static void
document_info_print (Eps_Document *document, Eps_Page *page);

static void app_resize(Ecore_Evas *ee);
static int  app_signal_exit(void *data, int ev_type, void *ev);
static void app_delete_request(Ecore_Evas *ee);

int
main (int argc, char *argv[])
{
  Ecore_Evas   *ee;
  Evas         *evas;
  Evas_Object  *o;
  Eps_Document *document;
  Eps_Page     *page;
  double        hscale;
  double        vscale;
  int           page_number;
  int           width;
  int           height;
  int           ee_width;
  int           ee_height;

  if (argc < 3) {
    printf ("\nUsage: %s filename page_number\n\n", argv[0]);
    return EXIT_FAILURE;
  }

  document = eps_document_new (argv[1]);
  if (!document) {
    printf ("Bad ps file\n");
    return EXIT_FAILURE;
  }

  sscanf (argv[2], "%d", &page_number);
  if (page_number >= eps_document_page_count_get (document)) {
    printf ("Page number exceeds the page count of the PS document\n");
    eps_document_delete (document);
    return EXIT_FAILURE;
  }

  page = eps_page_new (document);
  if (!page) {
    printf ("Bad page\n");
    eps_document_delete (document);
    return EXIT_FAILURE;
  }

  eps_page_page_set (page, page_number);
  eps_page_size_get (page, &width, &height);

  document_info_print (document, page);

  if (!ecore_init()) {
    eps_page_delete (page);
    eps_document_delete (document);
    return EXIT_FAILURE;
  }

  if (!ecore_evas_init()) {
    ecore_shutdown ();
    eps_page_delete (page);
    eps_document_delete (document);
    return EXIT_FAILURE;
  }

  eps_page_scale_get (page, &hscale, &vscale);
  ee_width = (int)(width * hscale);
  ee_height = (int)(height * vscale);

  ee = ecore_evas_software_x11_new (NULL, 0,  0, 0, ee_width, ee_height);
  /* these tests can be improved... */
  if (!ee) {
    printf ("Can not find Software X11 engine. Trying DirectDraw engine...\n");
    ee = ecore_evas_software_ddraw_new (NULL,  0, 0, ee_width, ee_height);
    if (!ee) {
      printf ("Can not find Software X11 engine, nor DirectDraw engine...\n");
      printf ("Exiting...\n");
      ecore_evas_shutdown ();
      ecore_shutdown ();
      eps_page_delete (page);
      eps_document_delete (document);
      return EXIT_FAILURE;
    }
  }
  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);
  ecore_evas_callback_delete_request_set (ee, app_delete_request);
  ecore_evas_title_set (ee, "Evas Ps Test");
  ecore_evas_name_class_set (ee, "evas_ps_test", "test_evas_ps");
  ecore_evas_callback_resize_set (ee, app_resize);
  ecore_evas_show (ee);

  evas = ecore_evas_get (ee);

  o = evas_object_image_add (evas);
  evas_object_move (o, 0, 0);
  eps_page_render (page, o);
  evas_object_show (o);

  ecore_main_loop_begin ();

  eps_page_delete (page);
  eps_document_delete (document);

  ecore_evas_shutdown ();
  ecore_shutdown ();

  return EXIT_SUCCESS;
}

static void
document_info_print (Eps_Document *document, Eps_Page *page)
{
  const char *title;
  const char *author;
  const char *for_;
  const char *format;
  const char *creation_date;
  const char *is_eps;
  const char *orientation;
  int         ll;
  int         width;
  int         height;

  printf ("\n");
  printf ("  Spectre version....: %s\n", eps_spectre_version_get ());
  printf ("\n");

  printf ("  Document Metadata:\n");
  printf ("\n");

  title = eps_document_title_get (document);
  if (!title)
    title = "(none)";
  author = eps_document_author_get (document);
  if (!author)
    author = "(none)";
  for_ = eps_document_for_get (document);
  if (!for_)
    for_ = "(none)";
  format = eps_document_format_get (document);
  if (!format)
    format = "(none)";
  creation_date = eps_document_creation_date_get (document);
  if (!creation_date)
    creation_date = "(none)";
  is_eps = eps_document_is_eps_get(document) ? "yes" : "no";
  ll = eps_document_language_level_get (document);

  printf ("  Title..............: %s\n", title);
  printf ("  Author.............: %s\n", author);
  printf ("  For................: %s\n", for_);
  printf ("  Format.............: %s\n", format);
  printf ("  Creation date......: %s\n", creation_date);
  printf ("  Is Eps.............: %s\n", is_eps);
  printf ("  Language level.....: %d\n", ll);

  printf ("\n");

  printf ("  Document Properties:\n");
  printf ("\n");

  printf ("  Page count.........: %d\n", eps_document_page_count_get (document));

  printf ("\n");

  printf ("  Page Properties:\n");
  printf ("\n");

  eps_page_size_get (page, &width, &height);

  printf ("  Number.............: %d\n", eps_page_page_get (page));
  printf ("  Size (pixels)......: %d x %d\n", width, height);

  switch (eps_page_orientation_get (page)) {
  case EPS_PAGE_ORIENTATION_PORTRAIT:
    orientation = "Portrait";
    break;
  case EPS_PAGE_ORIENTATION_LANDSCAPE:
    orientation = "Landscape";
    break;
  case EPS_PAGE_ORIENTATION_UPSIDEDOWN:
    orientation = "Upside Down";
    break;
  case EPS_PAGE_ORIENTATION_SEASCAPE:
    orientation = "Seascape";
    break;
  }
  printf ("  Orientation........: %s\n", orientation);

  printf ("\n");
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
