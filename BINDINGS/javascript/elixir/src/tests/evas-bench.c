/*
 * This file is a reference for evas-bench.js
 *
 * Compile with: gcc -o evas-bench evas-bench.c `pkg-config --libs --cflags ecore evas ecore-evas`
 */
#include <Ecore.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <math.h>

struct ctxt {
    Ecore_Evas *ee;
    Evas *e;
    Evas_Object *bg;
    Evas_Object *fps;
    Evas_Object *objs[128];
    double start_time;
    long long frames;
    int phase;
};

Eina_Bool anim(void *data)
{
    struct ctxt *ctxt = data;
    Evas_Coord w2 = 120 / 2;
    Evas_Coord h2 = 160 / 2;
    Evas_Coord bx = (480 / 2) - w2;
    Evas_Coord by = (800 / 2) - h2;
    int i, f = ctxt->frames;
    char str[30];
    double td;

    for (i = 0; i < 128; i++) {
        Evas_Coord x = bx + sin((double)(f + (i * 13)) / 183.5) * w2;
        Evas_Coord y = by + cos((double)(f + (i * 28)) / 219.0) * h2;
        evas_object_move(ctxt->objs[i], x, y);
    }

    ctxt->frames++;
    td = ecore_time_get() - ctxt->start_time;
    if (td > 1.0) {
        snprintf(str, sizeof(str), "FPS=%0.1f", ctxt->frames / td);
        evas_object_text_text_set(ctxt->fps, str);
    }

    if (td > 20) {
        fprintf(stderr, "phase %d fps=%0.1f\n", ctxt->phase, ctxt->frames / td);
        ctxt->frames = 0;
        ctxt->start_time = ecore_time_get();
        ctxt->phase++;
        evas_object_text_text_set(ctxt->fps, "FPS=?");
        if (ctxt->phase == 1) {
            for (i = 0; i < 128; i++)
                evas_object_color_set(ctxt->objs[i], 128, 128, 128, 128);
        } else if (ctxt->phase == 2) {
            for (i = 0; i < 128; i++)
                evas_object_color_set(ctxt->objs[i], 255, 0, 0, 255);
        } else if (ctxt->phase == 3) {
            for (i = 0; i < 128; i++)
                evas_object_color_set(ctxt->objs[i], 128, 0, 0, 128);
        } else if (ctxt->phase > 3)
            ecore_main_loop_quit();
    }

    ecore_evas_manual_render(ctxt->ee);

    return EINA_TRUE;
}


int main(void)
{
    struct ctxt ctxt;
    int i;

    ecore_init();
    evas_init();
    ecore_evas_init();

    ctxt.ee = ecore_evas_new(NULL, 0, 0, 480, 800, NULL);
    ctxt.e = ecore_evas_get(ctxt.ee);

    ctxt.bg = evas_object_rectangle_add(ctxt.e);
    evas_object_resize(ctxt.bg, 480, 800);
    evas_object_color_set(ctxt.bg, 255, 255, 255, 255);
    evas_object_show(ctxt.bg);

    for (i = 0; i < 128; i++) {
        ctxt.objs[i] = evas_object_image_add(ctxt.e);
        evas_object_image_file_set(ctxt.objs[i], "logo.png", NULL);
        evas_object_image_fill_set(ctxt.objs[i], 0, 0, 120, 160);
        evas_object_resize(ctxt.objs[i], 120, 160);
        evas_object_show(ctxt.objs[i]);
    }

    ctxt.fps = evas_object_text_add(ctxt.e);
    evas_object_color_set(ctxt.fps, 0, 0, 128, 255);
    evas_object_text_font_set(ctxt.fps, "Sans:style=Bold", 12);
    evas_object_text_text_set(ctxt.fps, "FPS=?");
    evas_object_resize(ctxt.fps, 480, 30);
    evas_object_show(ctxt.fps);

    ecore_evas_show(ctxt.ee);

    ctxt.frames = 0;
    ctxt.start_time = ecore_time_get();
    ctxt.phase = 0;

    /* try to cope with reference frametime */
    //ecore_animator_frametime_set(1 / 60.0);
    //ecore_animator_add(anim, &ctxt);

    /* try to do as much as possible */
    ecore_idler_add(anim, &ctxt);

    ecore_main_loop_begin();
    return 0;
}
