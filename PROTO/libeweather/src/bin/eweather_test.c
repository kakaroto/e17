
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "EWeather_Smart.h"

Ecore_Evas *ecore_evas;
Evas *evas;
Evas_Object *ow;
EWeather *eweather;

int i_theme = 0;
static const char* _themes[] =
{
     PACKAGE_DATA_DIR"/default/theme.edj",
     PACKAGE_DATA_DIR"/simple/theme.edj"
};


    static void
_resize_cb(Ecore_Evas *ee)
{
    Evas_Coord w, h;

    evas_output_viewport_get(evas, NULL, NULL, &w, &h);
    evas_object_resize(ow, w, h);
}

    static void
_delete_request_cb(Ecore_Evas *ee)
{
    ecore_main_loop_quit();
}

   static void
_key_up_cb(void *data, Evas *evas, Evas_Object *o_day, void *event)
{
    i_theme = (i_theme + 1 ) %2;

    printf("Set theme : %s\n", _themes[i_theme]);
    eweather_theme_set(ow, _themes[i_theme]);
}


int main(int argc, char **argv)
{
    Eina_Array *array;
    Eina_Module *m;
    int i;
    Eina_Array_Iterator it;

    eina_init();

    edje_init();
    edje_frametime_set(1.0 / 30.0);

    if (!ecore_evas_init()) return -1;
    ecore_evas = ecore_evas_new(NULL, 0, 0, 400, 600, NULL);
    if (!ecore_evas) return -1;
    ecore_evas_callback_delete_request_set(ecore_evas, _delete_request_cb);

    ecore_evas_callback_resize_set(ecore_evas, _resize_cb);
    ecore_evas_title_set(ecore_evas, "EWeather Test Program");
    ecore_evas_name_class_set(ecore_evas, "eweather_test", "main");
    ecore_evas_show(ecore_evas);
    evas = ecore_evas_get(ecore_evas);

    //eweather object
    ow = eweather_object_add(evas);
    eweather = eweather_object_eweather_get(ow);
    evas_object_show(ow);
    _resize_cb(ecore_evas);

    array = eweather_plugins_list_get(eweather);

    EINA_ARRAY_ITER_NEXT(array, i, m, it)
    {
        if(!strcmp(eweather_plugin_name_get(eweather, i), "Test"))
        {
            eweather_plugin_set(eweather, m);
            break;
        }
    }

    evas_object_focus_set(ow, EINA_TRUE);
    evas_object_event_callback_add(ow, EVAS_CALLBACK_KEY_UP,
		   _key_up_cb, NULL);

    printf("Press any key to change the theme\n");
    //

    ecore_main_loop_begin();

    return 1;
}

