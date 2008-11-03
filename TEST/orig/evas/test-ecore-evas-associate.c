#include <Ecore.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void on_keydown(void *data, Evas *e, Evas_Object *obj, void *einfo)
{
    Ecore_Evas *ee = data;
    Evas_Event_Key_Down *ev = einfo;
    Evas_Coord ox, oy, ow, oh, wx, wy, ww, wh;

    ecore_evas_geometry_get(ee, &wx, &wy, &ww, &wh);
    evas_object_geometry_get(obj, &ox, &oy, &ow, &oh);

    if (strcmp(ev->keyname, "j") == 0)
      ecore_evas_resize(ee, ww, wh + 5);
    else if (strcmp(ev->keyname, "k") == 0)
      ecore_evas_resize(ee, ww, wh - 5);
    else if (strcmp(ev->keyname, "Down") == 0)
      evas_object_resize(obj, ow, oh + 5);
    else if (strcmp(ev->keyname, "Up") == 0)
      evas_object_resize(obj, ow, oh - 5);
    else if (strcmp(ev->keyname, "Escape") == 0)
      ecore_main_loop_quit();
    else
      printf("unhandled key: %s\n", ev->keyname);
}

static void on_free(void *data, Evas *e, Evas_Object *obj, void *einfo)
{
    ecore_main_loop_quit();
}

int main(int argc, char *argv[])
{
    Ecore_Evas *ee;
    Evas_Object *o;

    ecore_init();
    evas_init();
    ecore_evas_init();

    ee = ecore_evas_new(NULL, 0, 0, 1, 1, NULL);
    o = evas_object_rectangle_add(ecore_evas_get(ee));
    ecore_evas_object_associate(ee, o, 0);

    evas_object_resize(o, 320, 240);
    evas_object_color_set(o, 255, 128, 128, 255);
    evas_object_show(o);
    evas_object_focus_set(o, 1);

    evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, on_keydown, ee);
    evas_object_event_callback_add(o, EVAS_CALLBACK_FREE, on_free, NULL);

    ecore_main_loop_begin();

    ecore_evas_shutdown();
    evas_shutdown();
    ecore_shutdown();

    return 0;
}
