#include <stdlib.h>
#include <stdio.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "esmart_pdf.h"


static void app_resize(Ecore_Evas *ee);
static int app_signal_exit(void *data, int ev_type, void *ev);
static void app_delete_request(Ecore_Evas *ee);

int
main (int argc, char *argv[])
{
  Ecore_Evas  *ee;
  Evas        *evas;
  Evas_Object *o;
  Evas_Object *bg;
  char        *filename;
  double       hscale;
  double       vscale;
  int          page_number;
  int          width;
  int          height;
  int          ee_width;
  int          ee_height;

  if (argc < 3) {
    printf ("\nUsage: %s filename page_number\n\n", argv[0]);
    return EXIT_FAILURE;
  }

  filename = argv[1];

  sscanf (argv[2], "%d", &page_number);

  if (!ecore_init()) {
    evas_shutdown ();
    return EXIT_FAILURE;
  }
  if (!ecore_evas_init()) {
    ecore_shutdown ();
    return EXIT_FAILURE;
  }

  ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 0, 0);
  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);
  ecore_evas_callback_delete_request_set(ee, app_delete_request);
  ecore_evas_title_set(ee, "Esmart Pdf Test");
  ecore_evas_name_class_set(ee, "esmart_pdf_test", "test_esmart_pdf");
  ecore_evas_callback_resize_set(ee, app_resize);
  ecore_evas_show(ee);

  evas = ecore_evas_get(ee);

  bg = evas_object_rectangle_add(evas);
  evas_object_color_set(bg, 0, 0, 0, 255);
  evas_object_show(bg);
  ecore_evas_data_set(ee, "bg", bg);

  o = esmart_pdf_add (evas);
  if (!esmart_pdf_init (o)) {
    ecore_evas_shutdown ();
    ecore_shutdown ();
    return EXIT_FAILURE;
  }

  if (!esmart_pdf_file_set (o, filename)) {
    evas_object_del (o);
    ecore_evas_shutdown ();
    ecore_shutdown ();
    return EXIT_FAILURE;
  }

  esmart_pdf_page_set (o, page_number);
  esmart_pdf_render (o);
  evas_object_move (o, 0, 0);
  evas_object_show (o);

  esmart_pdf_size_get (o, &width, &height);
  esmart_pdf_scale_get (o, &hscale, &vscale);
  ee_width = (int)(width * hscale);
  ee_height = (int)(height * vscale);

  ecore_evas_resize(ee, ee_width, ee_height);
  evas_object_resize(bg, ee_width, ee_height);

  ecore_main_loop_begin ();

  evas_object_del (o);
  ecore_evas_shutdown ();
  ecore_shutdown ();

  return 0;
}

static void
app_resize(Ecore_Evas *ee)
{
   Evas_Coord w, h;
   Evas *evas;
   Evas_Object *bg;

   evas = ecore_evas_get(ee);
   evas_output_viewport_get(evas, NULL, NULL, &w, &h);
   bg = ecore_evas_data_get(ee, "bg");
   evas_object_resize(bg, w, h);
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
