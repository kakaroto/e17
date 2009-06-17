
#include <stdio.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include "esmart_rsvg.h"

#include <gtk/gtk.h>

#define FILENAME "test.svg"

static Ecore_Timer *refresh_timer = NULL;
static Ecore_Timer *delay_timer = NULL;

/* static int */
/* fix_bg (void *data) */
/* { */
/*   int x, y, w, h; */
/*   Ecore_Evas *ee = NULL; */
/*   Evas_Object *o = NULL; */

/*   if ((ee = (Ecore_Evas *) data)) */
/*     { */
/*       ecore_evas_geometry_get (ee, &x, &y, &w, &h); */
/*       if ((o = */
/* 	   evas_object_name_find (ecore_evas_get (ee), "root_background"))) */
/* 	{ */
/* 	  evas_object_resize (o, w, h); */
/* 	  esmart_trans_x11_freshen (o, x, y, w, h); */
/* 	} */
/*     } */
/*   refresh_timer = NULL; */
/*   return (0); */
/* } */

static void
window_del_cb (Ecore_Evas * ee)
{
  ecore_main_loop_quit ();
}

/* static void */
/* window_move_cb (Ecore_Evas * ee) */
/* { */
/*   Evas_Object *o = NULL; */

/*   if (refresh_timer) */
/*     ecore_timer_del (refresh_timer); */
/*   refresh_timer = ecore_timer_add (MOVE_REFRESH, fix_bg, ee); */
/* } */
/* static void */
/* window_resize_cb (Ecore_Evas * ee) */
/* { */
/*   int x, y, w, h; */
/*   Evas_Object *o = NULL; */

/*   ecore_evas_geometry_get (ee, &x, &y, &w, &h); */
/*   if (refresh_timer) */
/*     ecore_timer_del (refresh_timer); */
/*   refresh_timer = ecore_timer_add (RESIZE_REFRESH, fix_bg, ee); */

/*   if ((o = evas_object_name_find (ecore_evas_get (ee), "background"))) */
/*     { */
/*       evas_object_resize (o, w, h); */
/*     } */
/*   if ((o = evas_object_name_find (ecore_evas_get (ee), "container"))) */
/*     evas_object_resize (o, w, h); */
/*   if ((o = evas_object_name_find (ecore_evas_get (ee), "dragger"))) */
/*     evas_object_resize (o, w, h); */
/* } */

static int
exit_cb (void *ev, int ev_type, void *data)
{
  ecore_main_loop_quit ();
  return (0);
}

/* static void */
/* bg_down_cb (void *data, Evas * evas, Evas_Object * obj, void *event_info) */
/* { */
/*   Evas_Event_Mouse_Down *event = event_info; */
/*   Evas_Object *cont = data; */
/*   double x, w; */

/*   evas_object_geometry_get (obj, &x, NULL, &w, NULL); */
/*   if (event->canvas.x < x + w / 2) */
/*     e_container_scroll_start (cont, -1); */
/*   else */
/*     e_container_scroll_start (cont, 1); */
/* } */

/* static void */
/* bg_up_cb (void *data, Evas * evas, Evas_Object * obj, void *event_info) */
/* { */
/*   Evas_Event_Mouse_Up *event = event_info; */
/*   Evas_Object *cont = data; */

/*   printf ("up!\n"); */
/*   e_container_scroll_stop (cont); */
/* } */

static int
delayed_actions (void *data)
{
    fprintf( stderr, "delayed_actions()\n" );
    
    Evas_Object* o = data;

    evas_object_move (o, 10, 10);
    evas_object_resize (o, 300, 500);
    
}

int
main (int argc, char *argv[])
{
  Evas *evas = NULL;
  Ecore_Evas *ee = NULL;
  Evas_Object *o = NULL;
  Evas_Object *cont = NULL;
  Evas_Object *image = NULL;
  int iw, ih;

  gtk_init( &argc, &argv );

  ecore_init ();
  ecore_app_args_set (argc, (const char **) argv);

  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);

  if (ecore_evas_init ())
    {
      ee = ecore_evas_software_x11_new (NULL, 0, 0, 0, 400, 400);
      ecore_evas_title_set (ee, "Enlightenment Thumbnail Test");
      ecore_evas_callback_delete_request_set (ee, window_del_cb);
//      ecore_evas_callback_resize_set (ee, window_resize_cb);
//      ecore_evas_callback_move_set (ee, window_move_cb);

      evas = ecore_evas_get (ee);

      Evas_Object* eo;
      eo = evas_object_image_add (evas);
      evas_object_image_file_set (eo, "checks.png", 0 );
      evas_object_move (eo, 0, 0);
      evas_object_resize (eo, 3000, 3000);
      evas_object_layer_set (eo, -50);
      evas_object_show (eo);
      
      {
          Evas_Object* eo;
          eo = evas_object_image_add (evas);
          evas_object_image_file_set (eo, "/tmp/esmart.png", 0 );
          evas_object_move (eo, 150, 150);
          evas_object_layer_set (eo, -5);
          evas_object_show (eo);

          int w=0, h=0;
          evas_object_image_size_get( eo, &w, &h );
          printf("evas_obj size for /tmp file w:%ld, h:%ld\n", w, h );
      }
      

      
      o = esmart_rsvg_new_from_file( evas, FILENAME );
      evas_object_move (o, 0, 0);
      evas_object_resize (o, 400, 400);
      evas_object_layer_set (o, 5);
      evas_object_name_set (o, "svg");
      evas_object_show (o);

      delay_timer = ecore_timer_add( 1, delayed_actions, o );
      
      ecore_evas_show (ee);
      ecore_main_loop_begin ();
    }
  return (0);
}
