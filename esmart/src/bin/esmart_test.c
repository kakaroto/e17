/*
 * esmart_main.c
 * 
 * a test program for the thumbnail and container smart objects.
 * usage: esmart_test file1 [file2] [file3] ...
 *
 * esmart_test creates thumbnails out of all valid image files given
 * as arguments, and puts them into a horizontal container.
 * click on the white background to scroll the contents.
 */


#include <stdio.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Esmart_Container.h>
#include <Esmart_Thumb.h>
#include <Esmart_Trans_X11.h>
#include <Esmart_Draggies.h>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#define MOVE_REFRESH 0.06
#define RESIZE_REFRESH 0.02

static Ecore_Timer *refresh_timer = NULL;

static int
fix_bg (void *data)
{
  int x, y, w, h;
  Ecore_Evas *ee = NULL;
  Evas_Object *o = NULL;

  if ((ee = (Ecore_Evas *) data))
    {
      ecore_evas_geometry_get (ee, &x, &y, &w, &h);
      if ((o =
	   evas_object_name_find (ecore_evas_get (ee), "root_background")))
	{
	  evas_object_resize (o, w, h);
	  esmart_trans_x11_freshen (o, x, y, w, h);
	}
    }
  refresh_timer = NULL;
  return (0);
}

static void
window_del_cb (Ecore_Evas * ee)
{
  ecore_main_loop_quit ();
}

static void
window_move_cb (Ecore_Evas * ee)
{
  if (refresh_timer)
    ecore_timer_del (refresh_timer);
  refresh_timer = ecore_timer_add (MOVE_REFRESH, fix_bg, ee);
}

static void
window_resize_cb (Ecore_Evas * ee)
{
  int x, y, w, h;
  Evas_Object *o = NULL;

  ecore_evas_geometry_get (ee, &x, &y, &w, &h);
  if (refresh_timer)
    ecore_timer_del (refresh_timer);
  refresh_timer = ecore_timer_add (RESIZE_REFRESH, fix_bg, ee);

  if ((o = evas_object_name_find (ecore_evas_get (ee), "background")))
    {
      evas_object_resize (o, w, h);
    }
  if ((o = evas_object_name_find (ecore_evas_get (ee), "container")))
    evas_object_resize (o, w, h);
  if ((o = evas_object_name_find (ecore_evas_get (ee), "dragger")))
    evas_object_resize (o, w, h);
}

static int
exit_cb (void *ev, int ev_type, void *data)
{
  ecore_main_loop_quit ();
  return (0);
}

static void
bg_down_cb (void *data, Evas * evas, Evas_Object * obj, void *event_info)
{
  Evas_Event_Mouse_Down *event = event_info;
  Evas_Object *cont = data;
  Evas_Coord x, w;

  evas_object_geometry_get (obj, &x, NULL, &w, NULL);
  if (event->canvas.x < x + w / 2)
    esmart_container_scroll_start (cont, -1);
  else
    esmart_container_scroll_start (cont, 1);
}

static void
bg_up_cb (void *data, Evas * evas, Evas_Object * obj, void *event_info)
{
  Evas_Object *cont = data;

  printf ("up!\n");
  esmart_container_scroll_stop (cont);
}

int
main (int argc, char *argv[])
{
  Evas *evas = NULL;
  Ecore_Evas *ee = NULL;
  Ecore_X_Window win = 0;
  Evas_Object *o = NULL;
  Evas_Object *cont = NULL;
  Evas_Object *image = NULL;
  int iw, ih;

  ecore_init ();
  ecore_app_args_set (argc, (const char **) argv);

  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);

  if (ecore_evas_init ())
    {
      ee = ecore_evas_software_x11_new (NULL, 0, 0, 0, 300, 120);
      win = ecore_evas_software_x11_window_get(ee);
      ecore_evas_title_set (ee, "Enlightenment Thumbnail Test");
      ecore_evas_callback_delete_request_set (ee, window_del_cb);
      ecore_evas_callback_resize_set (ee, window_resize_cb);
      ecore_evas_callback_move_set (ee, window_move_cb);

      evas = ecore_evas_get (ee);
      o = esmart_trans_x11_new (evas);
      esmart_trans_x11_type_set(o, Esmart_Trans_X11_Type_Background);
      esmart_trans_x11_window_set(o, win);
      evas_object_move (o, 0, 0);
      evas_object_resize (o, 300, 120);
      evas_object_layer_set (o, -5);
      evas_object_name_set (o, "root_background");
      evas_object_show (o);

      o = evas_object_rectangle_add (evas);
      evas_object_move (o, 0, 0);
      evas_object_resize (o, 300, 120);
      evas_object_layer_set (o, -6);
      evas_object_color_set (o, 255, 255, 255, 0);
      evas_object_name_set (o, "background");
      evas_object_show (o);

      o = esmart_draggies_new (ee);
      evas_object_move (o, 0, 0);
      evas_object_resize (o, 300, 120);
      evas_object_layer_set (o, 255);
      evas_object_color_set (o, 255, 255, 255, 0);
      evas_object_name_set (o, "dragger");
      esmart_draggies_button_set (o, 1);
      evas_object_show (o);


      if (argc < 2)
	{
	  image = evas_object_image_add (evas);
	  evas_object_image_file_set (image, PACKAGE_DATA_DIR "/esmart.png",
				      NULL);
	  evas_object_image_size_get (image, &iw, &ih);
	  evas_object_resize (image, iw, ih);
	  evas_object_image_fill_set (image, 0.0, 0.0, (Evas_Coord) iw,
				      (Evas_Coord) ih);
	  evas_object_layer_set (image, 1000);
	  evas_object_pass_events_set (image, 1);
	  evas_object_show (image);
	}


      cont = esmart_container_new (evas);
      evas_object_move (cont, 0, 0);
      evas_object_resize (cont, 300, 120);
      evas_object_layer_set (cont, 0);
      evas_object_name_set (cont, "container");
      evas_object_show (cont);
      esmart_container_padding_set (cont, 10, 10, 10, 10);
      esmart_container_spacing_set (cont, 5);
      esmart_container_fill_policy_set (cont,
				   CONTAINER_FILL_POLICY_FILL_Y |
				   CONTAINER_FILL_POLICY_KEEP_ASPECT);

      evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN, bg_down_cb,
				      cont);
      evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_UP, bg_up_cb,
				      cont);


      while (--argc)
	{
	  if ((o = esmart_thumb_new (ecore_evas_get (ee), argv[argc])))
	    {
	      evas_object_layer_set (o, 2);
	      evas_object_show (o);
	      esmart_container_element_append (cont, o);
	    }
	}
      evas_object_resize (cont, 300, 120);
      ecore_evas_show (ee);

      if (ecore_x_composite_query())
        {
           ee = ecore_evas_software_x11_new (NULL, 0, 0, 0, 150, 50);
           ecore_evas_title_set (ee, "Enlightenment Pixmap Test");
           ecore_evas_callback_delete_request_set (ee, window_del_cb);
           ecore_evas_callback_resize_set (ee, window_resize_cb);
           ecore_evas_callback_move_set (ee, window_move_cb);

           evas = ecore_evas_get (ee);
           o = esmart_xpixmap_new (evas, 0, win);
           evas_object_move (o, 400, 400);
           evas_object_resize (o, 150, 50);
           evas_object_layer_set (o, -5);
           evas_object_name_set (o, "root_background");
           evas_object_show (o);

           o = evas_object_rectangle_add (evas);
           evas_object_move (o, 0, 0);
           evas_object_resize (o, 150, 10);
           evas_object_layer_set (o, -3);
           evas_object_color_set (o, 255, 255, 255, 50);
           evas_object_name_set (o, "background");
           evas_object_show (o);

           ecore_evas_show (ee);
        }

      ecore_main_loop_begin ();
    }
  return (0);
}
