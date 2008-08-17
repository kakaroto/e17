/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include <Ewl.h>
#include <ewl_macros.h>
#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <Edje.h>

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

static void
edje_text(void *data, Evas_Object *obj __UNUSED__,
                const char *emission, const char *source)
{
        char *buf;
        char *text;
        int len;
        Ewl_Widget *label = data;

        if (strstr(emission, "Present") != emission)
                return;

        text = (char *)emission + strlen("Present");
        len = strlen(text);
        buf = NEW(char, len);
        snprintf(buf, len, text);

        ewl_text_clear(EWL_TEXT(label));

        ewl_text_font_set(EWL_TEXT(label), "ewl/default");
        ewl_text_font_size_set(EWL_TEXT(label), 12);
        ewl_text_styles_set(EWL_TEXT(label), EWL_TEXT_STYLE_SOFT_SHADOW);
        ewl_text_text_set(EWL_TEXT(label), (char *)source);

        ewl_text_align_set(EWL_TEXT(label), EWL_FLAG_ALIGN_LEFT);
        ewl_text_styles_set(EWL_TEXT(label), EWL_TEXT_STYLE_NONE);
        ewl_text_font_set(EWL_TEXT(label), "ewl/default");
        ewl_text_font_size_set(EWL_TEXT(label), 9);
        ewl_text_text_append(EWL_TEXT(label), text);

        FREE(buf);
}

static void
start_text(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                        void *user_data)
{
        Ewl_Widget *label = user_data;

        ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
        ewl_text_font_set(EWL_TEXT(label), "ewl/default");
        ewl_text_font_size_set(EWL_TEXT(label), 12);
        ewl_text_styles_set(EWL_TEXT(label), EWL_TEXT_STYLE_SOFT_SHADOW);
        ewl_text_text_append(EWL_TEXT(label), "Welcome to Enlightenment!\n");
        ewl_text_styles_set(EWL_TEXT(label), EWL_TEXT_STYLE_NONE);
        ewl_text_font_set(EWL_TEXT(label), "ewl/default");
        ewl_text_font_size_set(EWL_TEXT(label), 7);
        ewl_text_text_append(EWL_TEXT(label), "We hope you enjoy your stay.\n"
                                              "Please visit us at:\n");
        ewl_text_color_set(EWL_TEXT(label), 0, 0, 255, 190);
        ewl_text_text_append(EWL_TEXT(label), "http://www.enlightenment.org/");
        ewl_text_color_set(EWL_TEXT(label), 0, 0, 0, 255);
}

static void
reveal_logo_cb(Ewl_Widget *w, void *ev_data __UNUSED__, void *user_data)
{
        edje_object_signal_callback_add(w->theme_object, "Present*", "*",
                        edje_text, user_data);
}

static void
obscure_logo_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        edje_object_signal_callback_del(w->theme_object, "Present*", "*",
                        edje_text);
}

static void
close_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        ewl_main_quit();
}

static void
button_down(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                void *user_data)
{
        Ewl_Widget *logo = user_data;
        ewl_widget_state_set(logo, "start_tour", EWL_STATE_PERSISTENT);

        ewl_widget_disable(w);
}

static void
quit_demo(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        ewl_main_quit();
}

int
main(int argc, char **argv)
{
        Ewl_Widget *win;
        Ewl_Widget *vbox;
        Ewl_Widget *hbox;
        Ewl_Widget *button;
        Ewl_Widget *logo;
        Ewl_Widget *label;
        char tmp[PATH_MAX];

        ewl_init(&argc, argv);

        snprintf(tmp, PATH_MAX, "%s/ewl/images/bg.edj", PACKAGE_DATA_DIR);

        win = ewl_window_new();
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, close_cb, NULL);
        ewl_object_size_request(EWL_OBJECT(win), 240, 320);
        ewl_theme_data_str_set(win, "/window/file", tmp);
        ewl_theme_data_str_set(win, "/window/group", "bg");
        ewl_widget_show(win);

        vbox = ewl_vbox_new();
        ewl_theme_data_str_set(vbox, "/vbox/file", tmp);
        ewl_theme_data_str_set(vbox, "/vbox/group", "box");
        ewl_object_alignment_set(EWL_OBJECT(vbox), EWL_FLAG_ALIGN_TOP);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(win), vbox);
        ewl_widget_show(vbox);

        /*
         * Create the label first so we can pass it to the logo's callbacks
         */
        label = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(label), NULL);
        ewl_object_fill_policy_set(EWL_OBJECT(label), EWL_FLAG_FILL_VFILL);

        /*
         * Setup the custom logo which becomes the tour display area.
         */
        logo = NEW(Ewl_Widget, 1);
        ewl_widget_init(logo);
        ewl_widget_appearance_set(logo, "demo_logo");
        ewl_callback_append(logo, EWL_CALLBACK_REVEAL, reveal_logo_cb, label);
        ewl_callback_append(logo, EWL_CALLBACK_OBSCURE, obscure_logo_cb, label);
        ewl_theme_data_str_set(logo, "/demo_logo/file", tmp);
        ewl_theme_data_str_set(logo, "/demo_logo/group", "tour");
        ewl_object_preferred_inner_size_set(EWL_OBJECT(logo), 150, 150);
        ewl_object_fill_policy_set(EWL_OBJECT(logo), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(logo), EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(vbox), logo);
        ewl_widget_show(logo);

        start_text(label, NULL, label);
        ewl_container_child_append(EWL_CONTAINER(vbox), label);
        ewl_widget_show(label);

        hbox = ewl_hbox_new();
        ewl_box_spacing_set(EWL_BOX(hbox), 10);
        ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(hbox);

        button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Take the tour!");
        ewl_container_child_append(EWL_CONTAINER(hbox), button);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, button_down, logo);
        ewl_widget_show(button);

        button = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(button), "Quit");
        ewl_container_child_append(EWL_CONTAINER(hbox), button);
        ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
        ewl_callback_append(button, EWL_CALLBACK_CLICKED, quit_demo, NULL);
        ewl_widget_show(button);

        ewl_widget_configure(win);

        ewl_main();
        return 0;
}

