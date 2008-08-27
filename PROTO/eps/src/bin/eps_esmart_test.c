#include <stdlib.h>
#include <stdio.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "esmart_ps.h"


static void app_resize(Ecore_Evas *ee);
static int app_signal_exit(void *data, int ev_type, void *ev);
static void app_delete_request(Ecore_Evas *ee);

int
main (int argc, char *argv[])
{
  Ecore_Evas  *ee;
  Evas        *evas;
  Evas_Object *o;
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

  if (!ecore_init())
    return EXIT_FAILURE;

  if (!ecore_evas_init()) {
    ecore_shutdown ();
    return EXIT_FAILURE;
  }

  ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 0, 0);
  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, app_signal_exit, NULL);
  ecore_evas_callback_delete_request_set(ee, app_delete_request);
  ecore_evas_title_set(ee, "Esmart Ps Test");
  ecore_evas_name_class_set(ee, "esmart_ps_test", "test_esmart_ps");
  ecore_evas_callback_resize_set(ee, app_resize);
  ecore_evas_show(ee);

  evas = ecore_evas_get(ee);

  o = esmart_ps_add (evas);
  if (!esmart_ps_init (o)) {
    ecore_evas_shutdown ();
    ecore_shutdown ();
    return EXIT_FAILURE;
  }
  esmart_ps_file_set (o, filename);
  esmart_ps_page_set (o, page_number);
  esmart_ps_scale_set (o, 0.5, 0.5);
  esmart_ps_render (o);
  evas_object_move (o, 0, 0);
  evas_object_show (o);

  esmart_ps_size_get (o, &width, &height);
  esmart_ps_scale_get (o, &hscale, &vscale);
  ee_width = (int)(width * hscale);
  ee_height = (int)(height * vscale);

  ecore_evas_resize (ee, ee_width, ee_height);

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
