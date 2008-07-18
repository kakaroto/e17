/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ewl.h>
#include <Edje.h>
#include <stdio.h>

#if HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

Evas *evas;
Ecore_Evas *ee;
Evas_Object *edje;
Ewl_Widget *text, *entry;

static void
open(Ewl_Widget *w , void *e __UNUSED__, void *d)
{
        ewl_widget_disable(w);
        ewl_widget_enable(EWL_WIDGET(d));
        edje_object_signal_emit(edje, "open", "open");
}

static void
close(Ewl_Widget *w, void *e __UNUSED__, void *d)
{
        ewl_widget_disable(w);
        ewl_widget_enable(EWL_WIDGET(d));
        edje_object_signal_emit(edje, "close", "close");
}

static void
destroy_main_window(Ecore_Evas *ee __UNUSED__)
{
        ewl_main_quit();
        return;
}

static void
resize_window(Ecore_Evas *ee)
{
        Evas_Coord w, h;

        ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
        evas_object_resize(edje, w, h);
}

static void
insert(Ewl_Widget *w __UNUSED__, void *e __UNUSED__, void *d __UNUSED__)
{
        const char * t;

        t = ewl_text_text_get(EWL_TEXT(entry));

        if (t) {
                ewl_text_text_append(EWL_TEXT(text), "\n");
                ewl_text_text_append(EWL_TEXT(text), t);
                ewl_text_clear(EWL_TEXT(entry));
        }
}

int
main(int argc, char **argv)
{
        Ewl_Widget *wg, *c, *vbox;
        Ewl_Widget *button[2];
        Ewl_Widget *emb;
        Evas_Object *eo;
        Evas_Coord x, y, w, h;

        /*
         * initialize
         */
        evas_init();
        ecore_evas_init();
        ewl_init(&argc, argv);

        /*
         * setup ecore evas
         */
        ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 300, 300);
        ecore_evas_title_set(ee, "Ewl_Embed Test");
        ecore_evas_name_class_set(ee, "Ewl_Embed_Test", "Ewl");
        ecore_evas_callback_delete_request_set(ee, destroy_main_window);
        ecore_evas_callback_resize_set(ee, resize_window);
        ecore_evas_size_min_set(ee, 210, 230);
        ecore_evas_show(ee);

        /*
         * get evas and setup the edje
         */
        evas = ecore_evas_get(ee);
        edje = edje_object_add(evas);

        if (!edje_object_file_set(edje,
                        PACKAGE_DATA_DIR "/ewl/themes/ewl_embed_test.edj",
                        "main"))
        {
                fprintf(stderr, "Error in: %s\n",
                        PACKAGE_DATA_DIR "/ewl/themes/ewl_embed_test.edj");
                return 1;
        }

        evas_object_move(edje, 0, 0);
        evas_object_resize(edje, 300, 300);
        evas_object_show(edje);

        /*
         * Setup the ewl embed
         */
        emb = ewl_embed_new();
        ewl_object_fill_policy_set(EWL_OBJECT(emb), EWL_FLAG_FILL_ALL);
        eo = ewl_embed_canvas_set(EWL_EMBED(emb), evas,
                          (void *)(long)ecore_evas_software_x11_window_get(ee));
        ewl_embed_focus_set(EWL_EMBED(emb), TRUE);
        ewl_widget_show(emb);

        /*
         * swallow it into the edje
         */
        edje_object_part_geometry_get(edje, "swallow", &x, &y, &w, &h);
        evas_object_move(eo, x, y);
        evas_object_resize(eo, w, h);
        edje_object_part_swallow(edje, "swallow", eo);
        evas_object_show(eo);

        /*
         * fill it with content
         */
        vbox = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(emb), vbox);
        ewl_widget_show(vbox);

        wg = ewl_scrollpane_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), wg);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
        ewl_widget_show(wg);

        text = ewl_text_new();
        ewl_container_child_append(EWL_CONTAINER(wg), text);
        ewl_text_text_append(EWL_TEXT(text),
                "You can use EWL in side a evas/edje only app.\n"
                "It can actually behave like a normal evas object\n"
                "So forget to figure out how to write an entry object,\n"
                "simply use the ewl widget! :)\n\n");
        ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_HFILL |
                        EWL_FLAG_FILL_VFILL);
        ewl_widget_show(text);

        c = ewl_hbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(c), EWL_FLAG_FILL_NONE |
                                                EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(vbox), c);
        ewl_widget_show(c);

        entry = ewl_entry_new();
        ewl_container_child_append(EWL_CONTAINER(c), entry);
        ewl_entry_editable_set(EWL_ENTRY(entry), TRUE);
        ewl_entry_multiline_set(EWL_ENTRY(entry), FALSE);
        ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED, insert, NULL);
        ewl_widget_show(entry);

        wg = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(wg), "append");
        ewl_object_fill_policy_set(EWL_OBJECT(wg), EWL_FLAG_FILL_NONE);
        ewl_container_child_append(EWL_CONTAINER(c), wg);
        ewl_callback_append(wg, EWL_CALLBACK_CLICKED, insert, NULL);
        ewl_widget_show(wg);

        /*
         * Setup the second ewl embed
         */
        emb = ewl_embed_new();
        ewl_object_fill_policy_set(EWL_OBJECT(emb), EWL_FLAG_FILL_ALL);
        eo = ewl_embed_canvas_set(EWL_EMBED(emb), evas,
                          (void *)(long)ecore_evas_software_x11_window_get(ee));
        ewl_widget_show(emb);

        /*
         * swallow it into the edje
         */
        edje_object_part_geometry_get(edje, "swallow2", &x, &y, &w, &h);
        evas_object_move(eo, x, y);
        evas_object_resize(eo, w, h);
        edje_object_part_swallow(edje, "swallow2", eo);
        evas_object_show(eo);

        c = ewl_hbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(c), EWL_FLAG_FILL_NONE |
                                                EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(emb), c);
        ewl_widget_show(c);

        wg = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(wg), "open");
        ewl_container_child_append(EWL_CONTAINER(c), wg);
        ewl_widget_show(wg);
        button[0] = wg;

        wg = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(wg), "close");
        ewl_container_child_append(EWL_CONTAINER(c), wg);
        ewl_widget_show(wg);
        button[1] = wg;

        ewl_callback_append(button[0], EWL_CALLBACK_CLICKED, open, button[1]);
        ewl_callback_append(button[1], EWL_CALLBACK_CLICKED, close, button[0]);

        ewl_widget_disable(button[1]);

        /*
         * Setup the third ewl embed
         */
        emb = ewl_embed_new();
        ewl_object_fill_policy_set(EWL_OBJECT(emb), EWL_FLAG_FILL_ALL);
        eo = ewl_embed_canvas_set(EWL_EMBED(emb), evas,
                          (void *)(long)ecore_evas_software_x11_window_get(ee));
        ewl_embed_focus_set(EWL_EMBED(emb), TRUE);
        ewl_widget_show(emb);

        /*
         * swallow it into the edje
         */
        edje_object_part_geometry_get(edje, "swallow3", &x, &y, &w, &h);
        evas_object_move(eo, x, y);
        evas_object_resize(eo, w, h);
        edje_object_part_swallow(edje, "swallow3", eo);
        evas_object_show(eo);

        wg = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(wg), "Open the glass");
        ewl_container_child_append(EWL_CONTAINER(emb), wg);
        ewl_widget_show(wg);

        ewl_main();

        return 0;
}

