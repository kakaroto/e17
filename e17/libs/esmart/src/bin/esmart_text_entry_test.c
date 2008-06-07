/*
 * esmart_text_entry_test.c
 *
 * A test program for the text entry smart objects.
 *
 */


#include <stdio.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Esmart_Text_Entry.h>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif


Evas_Object *bg;

static void
window_del_cb (Ecore_Evas * ee)
{
  ecore_main_loop_quit ();
}

static void
window_resize_cb (Ecore_Evas * ee)
{
  Evas_Coord x, y, w, h;

  ecore_evas_geometry_get (ee, &x, &y, &w, &h);

  evas_object_resize(bg, w, h);
}

static int
exit_cb (void *ev, int ev_type, void *data)
{
  ecore_main_loop_quit ();
  return (0);
}

/*
static void
bg_key_down_cb (void *data, Evas * evas, Evas_Object * obj, void *event_info)
{
  printf ("key down\n");
}

static void
bg_key_up_cb (void *data, Evas * evas, Evas_Object * obj, void *event_info)
{
  printf ("key up\n");
}
*/

void key_data (void *data, const char *str)
{
  printf ("key data: %s\n", str);
}

int main (int argc, char *argv[])
{
  Evas *evas = NULL;
  Ecore_Evas *ee = NULL;
  Evas_Object *o = NULL;
  Evas_Object *edje = NULL;

  const int width = 300;
  const int height = 30;

  ecore_init ();
  ecore_app_args_set (argc, (const char **) argv);

  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);

  if (ecore_evas_init ())
  {
    edje_init();

    ee = ecore_evas_software_x11_new (NULL, 0, 0, 0, width, height);
    ecore_evas_title_set (ee, "Enlightenment Text Entry Test");
    ecore_evas_callback_delete_request_set (ee, window_del_cb);
    ecore_evas_callback_resize_set (ee, window_resize_cb);

    evas = ecore_evas_get (ee);

    bg = evas_object_rectangle_add (evas);
    evas_object_move (bg, 0, 0);
    evas_object_resize (bg, width, height);
    evas_object_color_set (bg, 0, 0, 0, 255);
    evas_object_name_set (bg, "background");
    evas_object_show (bg);

    edje = edje_object_add(evas);
    edje_object_file_set(edje, PACKAGE_DATA_DIR "/esmart_text_entry_test.edj", "text_entry");
    evas_object_move (edje, 0, 0);
    evas_object_resize (edje, width, height);
    evas_object_show (edje);

    o = esmart_text_entry_new (evas);
    esmart_text_entry_edje_part_set (o, edje, "text");
    evas_object_move (o, 0, 0);
    evas_object_resize (o, width, height);
    evas_object_name_set (o, "text");
    evas_object_show (o);

    evas_object_focus_set (o, 1);

    /*evas_object_event_callback_add (bg, EVAS_CALLBACK_KEY_DOWN, bg_key_down_cb,
                                    NULL);
    evas_object_event_callback_add (bg, EVAS_CALLBACK_KEY_UP, bg_key_up_cb,
                                    NULL);*/

    esmart_text_entry_return_key_callback_set (o, key_data, NULL);

    ecore_evas_show (ee);
    ecore_main_loop_begin ();

    // exit

    edje_shutdown();
    ecore_evas_shutdown();
    ecore_shutdown();
  }

  return (0);
}
