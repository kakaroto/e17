#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Engrave.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void
resize_cb(Ecore_Evas *ee)
{
    Evas_Coord w, h;
    Evas_Object *o;

    if (ee) {
        ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

        if ((o = evas_object_name_find(ecore_evas_get(ee), "bg")))
            evas_object_resize(o, w, h);

        if ((o = evas_object_name_find(ecore_evas_get(ee), "canvas")))
            evas_object_resize(o, w, h);
    }
}

int
main(int argc, char ** argv) 
{
    Ecore_Evas *ee;
    Evas *evas;
    Evas_Object *o;
    Engrave_File *ef;
    Engrave_Group *eg;

    if (argc < 2) {
        printf("need file\n");
        return 1;
    }

    if (strstr(argv[1], ".edj")) {
        if (argc < 3) {
            printf("need edj and group to show\n");
            return 1;
        }
        ef = engrave_load_edj(argv[1]);
    } else {
        if (argc < 5) {
            printf("need img and font dirs with .edc file and group to show\n");
            return 1;
        }
        ef = engrave_load_edc(argv[1], argv[2], argv[3]);
    }

    if (!ecore_init()) {
        printf("Unable to setup ecore\n");
        return 1;
    }

    if (!ecore_evas_init()) {
        printf("Unable to setup ecore_evas\n");
        return 1;
    }

    ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 640, 480);
    ecore_evas_title_set(ee, "Engrave Canvas Test App");
    ecore_evas_callback_resize_set(ee, resize_cb);
    ecore_evas_show(ee);

    evas = ecore_evas_get(ee);

    o = evas_object_rectangle_add(evas);
    evas_object_move(o, 0, 0);
    evas_object_resize(o, 640, 480);
    evas_object_color_set(o, 200, 200, 200, 255);
    evas_object_name_set(o, "bg");
    evas_object_show(o);

    o = engrave_canvas_new(evas);
    evas_object_move(o, 0, 0);
    evas_object_resize(o, 640, 480);
    evas_object_name_set(o, "canvas");

    engrave_canvas_file_set(o, ef);
    eg = engrave_file_group_by_name_find(ef, argv[argc - 1]);
    if (eg) engrave_canvas_current_group_set(o, eg);

    evas_object_show(o);

    ecore_main_loop_begin();

    engrave_file_free(ef);

    ecore_shutdown();
    return 0;
}

