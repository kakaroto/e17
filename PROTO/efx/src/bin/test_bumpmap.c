#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Efx.h>
#include <Ecore_Evas.h>

static void _move_cb(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj, Evas_Event_Mouse_Move *ev)
{
   int x, y;

   x = ev->cur.output.x;
   y = ev->cur.output.y;

   efx_bumpmap(obj, x, y);
}

static void _zoomout_cb(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj, Evas_Event_Mouse_Up *ev)
{
   efx_zoom(obj, EFX_EFFECT_SPEED_DECELERATE, 0.0, 1.0, &ev->output, 0.5, NULL, NULL);
}

static void _zoomin_cb(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj, Evas_Event_Mouse_Down *ev)
{
   efx_zoom(obj, EFX_EFFECT_SPEED_ACCELERATE, 1.0, 2.0, &ev->output, 0.5, NULL, NULL);
}

static void
_end(Ecore_Evas *ee __UNUSED__)
{
   ecore_main_loop_quit();
}

int
main(void)
{
   Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *o;
   Evas_Coord w;
   Evas_Coord h;

   efx_init();
   ecore_evas_init();

   ee = ecore_evas_new (NULL, 10, 10, 1, 1, NULL);
   if (!ee)
     {
       fprintf(stderr, "merde\n");
       return -1;
     }
   ecore_evas_callback_delete_request_set(ee, _end);
   ecore_evas_title_set(ee, "Bumpmapping");

   evas = ecore_evas_get(ee);

   o = evas_object_image_add(evas);
   evas_object_image_file_set(o, BUILD_DATADIR "/mur.jpg", NULL);
   if (evas_object_image_load_error_get(o))
     evas_object_image_file_set(o, PACKAGE_DATADIR "/mur.jpg", NULL);
   if (evas_object_image_load_error_get(o))
     evas_object_image_file_set(o, "data/mur.jpg", NULL);
   if (evas_object_image_load_error_get(o))
     {
        fprintf(stderr, "Could not locate image!\n");
        return -1;
     }
   evas_object_image_size_get (o, &w, &h);
   evas_object_image_fill_set(o, 0, 0, w, h);
   evas_object_image_size_set(o, w, h);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, w, h);
   evas_object_show(o);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, (Evas_Object_Event_Cb)_move_cb, NULL);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, (Evas_Object_Event_Cb)_zoomin_cb, NULL);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, (Evas_Object_Event_Cb)_zoomout_cb, NULL);

   ecore_evas_resize(ee, w, h);
   ecore_evas_show(ee);

   ecore_main_loop_begin();

   ecore_evas_shutdown();
   efx_shutdown();

   return 0;
}
