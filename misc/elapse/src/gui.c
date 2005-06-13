#include "Elapse.h"

void elapse_gui_init(Elapse *elapse, int argc, const char **argv)
{
    Evas_Coord iw, ih;
    int load;
    const char *conf_string;
    Ecore_X_Window win;
    int x, y, w, h;

    debug(DEBUG_INFO, elapse, "elapse_gui_init()");

    debug(DEBUG_INFO, elapse, "ecore_init()");
    ecore_init();
    ecore_app_args_set(argc, argv);

    debug(DEBUG_INFO, elapse, "ecore_evas_init()");
    ecore_evas_init();

    debug(DEBUG_INFO, elapse, "edje_init()");
    edje_init();

    if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_X11))
        elapse->ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
    else
    {
        if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_FB))
            elapse->ee = ecore_evas_fb_new(NULL, 0, 0, 0);
    }
    if (!elapse->ee)
    {
        if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_SOFTWARE_FB))
            elapse->ee = ecore_evas_fb_new(NULL, 0, 0, 0);
        if (!elapse->ee)
            elapse->ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 0, 0);
        if (!elapse->ee)
        {
            fprintf(stderr, "Cannot create Canvas!\n");
            exit(-1);
        }
    }

    ecore_evas_title_set(elapse->ee, "Elapse");
    ecore_evas_name_class_set(elapse->ee, "Elapse", "Elapse");
    ecore_evas_borderless_set(elapse->ee, 1);
    if (elapse->conf.sticky)
        ecore_evas_sticky_set(elapse->ee, 1);

    elapse->evas = ecore_evas_get(elapse->ee);

    elapse->edje = edje_object_add(elapse->evas);

    load = edje_object_file_set(elapse->edje, elapse->conf.theme, "elapse");
    if (!load) {
        debug(DEBUG_FUCKED, elapse, "Could not load theme");
        exit(1);
    }

    conf_string = edje_object_data_get (elapse->edje, "alpha");
    if (conf_string)
    {
        if (!strcmp (conf_string, "shape"))
            elapse->conf.alpha = SHAPE;
    }

    evas_object_move(elapse->edje, 0, 0);

    edje_object_size_min_get(elapse->edje, &iw, &ih);

    win = ecore_evas_software_x11_window_get(elapse->ee);

    if (elapse->conf.below)
        ecore_evas_layer_set(elapse->ee, 2);

    if (elapse->conf.alpha == TRANSPARENT)
    {
        debug(DEBUG_INFO, elapse, "esmart_trans_x11_new()");
        elapse->smart = esmart_trans_x11_new(elapse->evas);
        evas_object_move(elapse->smart, 0, 0);
        evas_object_layer_set(elapse->smart, -5);
        evas_object_name_set(elapse->smart, "root_bg");
        evas_object_show(elapse->smart);

        debug(DEBUG_INFO, elapse, "smart object resize");
        evas_object_resize(elapse->smart, (int)iw, (int)ih);

        ecore_evas_geometry_get(elapse->ee, &x, &y, &w, &h);
        esmart_trans_x11_freshen(elapse->smart, x, y, w, h);

        ecore_evas_callback_move_set(elapse->ee, elapse_cb_window_move);
    }
    else if (elapse->conf.alpha == SHAPE)
    {
        ecore_evas_shaped_set (elapse->ee, 1);

        elapse->shape = evas_object_rectangle_add(elapse->evas);
        evas_object_move(elapse->shape, 0, 0);
        evas_object_resize(elapse->shape, (int)iw, (int)ih);

        evas_object_color_set(elapse->shape, 0, 0, 0, 0);
        evas_object_focus_set(elapse->shape, 1);

        evas_object_show(elapse->shape);
    }

    debug(DEBUG_INFO, elapse, "object resize");
    evas_object_resize(elapse->edje, (int)iw, (int)ih);
    ecore_evas_resize(elapse->ee, (int)iw, (int)ih);

    ecore_evas_data_set(elapse->ee, "elapse", elapse);
    ecore_evas_callback_resize_set(elapse->ee, resize_cb);

    debug(DEBUG_INFO, elapse, "ecore_evas_show()");
    ecore_evas_show(elapse->ee);
    evas_object_show(elapse->edje);

    edje_object_signal_callback_add(elapse->edje, "quit", "quit",
                                    (void *)ecore_main_loop_quit, NULL);
}

void elapse_cb_window_move(Ecore_Evas *ee)
{
    int x, y, w, h;
    Evas_Object *o;
    Elapse elapse;

    elapse.conf.debug = 1;

    ecore_evas_geometry_get(ee, &x, &y, &w, &h);

    o = evas_object_name_find(ecore_evas_get(ee), "root_bg");

    if (!o) debug(DEBUG_FUCKED, &elapse, "Smart Object Not Found");

    esmart_trans_x11_freshen(o, x, y, w, h);
}

void resize_cb(Ecore_Evas *ee)
{
    Evas_Coord w, h, x, y;
    Elapse *elapse;

    elapse = ecore_evas_data_get(ee, "elapse");

    evas_output_viewport_get(elapse->evas, NULL, NULL, &w, &h);
    ecore_evas_geometry_get(elapse->ee, &x, &y, &w, &h);

    evas_object_resize(elapse->edje, w, h);

    if (elapse->conf.alpha == TRANSPARENT)
    {
        evas_object_resize(elapse->smart, w, h);
        esmart_trans_x11_freshen(elapse->smart, x, y, w, h);
    }
}
