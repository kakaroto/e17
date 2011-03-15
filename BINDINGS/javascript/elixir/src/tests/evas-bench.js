/*
 * Benchmark Evas using JavaScript.
 *
 * This is based on Evas official benchmark "expedite", doing couple of tests.
 * See also C reference evas-bench.c
 */
elx.load("ecore");
elx.load("evas");
elx.load("ecore-evas");

function anim(ctxt)
{
    var w2 = 120 / 2;
    var h2 = 160 / 2;
    var bx = (480 / 2) - w2;
    var by = (800 / 2) - h2;
    var i;
    var f = ctxt.frames;
    var td;
    var str;

    for (i = 0; i < 128; i++) {
        var x = bx + Math.sin((f + (i * 13)) / 183.5) * w2;
        var y = by + Math.cos((f + (i * 28)) / 219.0) * h2;
        evas_object_move(ctxt.objs[i], x, y);
    }

    ctxt.frames++;
    td = ecore_time_get() - ctxt.start_time;
    if (td > 1.0) {
        str = "FPS=" + (ctxt.frames / td).toFixed(1);
        evas_object_text_text_set(ctxt.fps, str);
    }

    if (td > 20) {
        elx.print("phase " + ctxt.phase + " fps=" +
                  (ctxt.frames / td).toFixed(1) + "\n");
        ctxt.frames = 0;
        ctxt.start_time = ecore_time_get();
        ctxt.phase++;
        evas_object_text_text_set(ctxt.fps, "FPS=?");
        if (ctxt.phase == 1) {
            for (i = 0; i < 128; i++)
                evas_object_color_set(ctxt.objs[i], 128, 128, 128, 128);
        } else if (ctxt.phase == 2) {
            for (i = 0; i < 128; i++)
                evas_object_color_set(ctxt.objs[i], 255, 0, 0, 255);
        } else if (ctxt.phase == 3) {
            for (i = 0; i < 128; i++)
                evas_object_color_set(ctxt.objs[i], 128, 0, 0, 128);
        } else if (ctxt.phase > 3)
            ecore_main_loop_quit();
    }

    ecore_evas_manual_render(ctxt.ee);
    elx.gc();

    return 1;
}


function main()
{
    var i;
    var ctxt = new Object();

    ecore_init();
    evas_init();
    ecore_evas_init();

    ctxt.ee = ecore_evas_new(null, 0, 0, 480, 800, "");
    ctxt.e = ecore_evas_get(ctxt.ee);

    ctxt.bg = evas_object_rectangle_add(ctxt.e);
    evas_object_resize(ctxt.bg, 480, 800);
    evas_object_color_set(ctxt.bg, 255, 255, 255, 255);
    evas_object_show(ctxt.bg);

    ctxt.objs = new Array(128);
    for (i = 0; i < 128; i++) {
        ctxt.objs[i] = evas_object_image_add(ctxt.e);
        evas_object_image_file_set(ctxt.objs[i], "logo.png", null);
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
    //ecore_animator_add(anim, ctxt);

    /* try to do as much as possible */
    ecore_idler_add(anim, ctxt);

    ecore_main_loop_begin();
    return 0;
}

main();
