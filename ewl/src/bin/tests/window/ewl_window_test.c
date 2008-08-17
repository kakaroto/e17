/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_window.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int create_test(Ewl_Container *box);
static void run_window(Ewl_Widget *w, void *ev, void *data);
static void del_window_cb(Ewl_Widget *w, void *ev, void *data);
static void flip_border_cb(Ewl_Widget *w, void *ev, void *data);

static int title_set_get(char *buf, int len);
static int name_set_get(char *buf, int len);
static int class_set_get(char *buf, int len);
static int borderless_set_get(char *buf, int len);
static int dialog_set_get(char *buf, int len);
static int fullscreen_set_get(char *buf, int len);
static int skip_taskbar_set_get(char *buf, int len);
static int skip_pager_set_get(char *buf, int len);
static int urgent_set_get(char *buf, int len);
static int modal_set_get(char *buf, int len);
static int keyboard_grab_set_get(char *buf, int len);
static int pointer_grab_set_get(char *buf, int len);
static int override_set_get(char *buf, int len);


/*
 * This set of tests is targeted at window
 */

static Ewl_Unit_Test window_unit_tests[] = {
                {"title set/get", title_set_get, NULL, -1, 0},
                {"name set/get", name_set_get, NULL, -1, 0},
                {"class set/get", class_set_get, NULL, -1, 0},
                {"borderless set/get", borderless_set_get, NULL, -1, 0},
                {"dialog set/get", dialog_set_get, NULL, -1, 0},
                {"fullscreen set/get", fullscreen_set_get, NULL, -1, 0},
                {"skip_taskbar set/get", skip_taskbar_set_get, NULL, -1, 0},
                {"skip_pager set/get", skip_pager_set_get, NULL, -1, 0},
                {"urgent set/get", urgent_set_get, NULL, -1, 0},
                {"modal set/get", modal_set_get, NULL, -1, 0},
                {"keyboard_grab set/get", keyboard_grab_set_get, NULL, -1, 0},
                {"pointer_grab set/get", pointer_grab_set_get, NULL, -1, 0},
                {"override set/get", override_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };

void
test_info(Ewl_Test *test)
{
        test->name = "Window";
        test->tip = "The window system.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_MISC;
        test->unit_tests = window_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *o;

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Launch window");
        ewl_container_child_append(box, o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, run_window, NULL);
        ewl_widget_show(o);

        return 1;
}

static void
run_window(Ewl_Widget *w, void *ev, void *data)
{
        Ewl_Widget *win, *b, *o;

        win = ewl_window_new();
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, del_window_cb, NULL);
        ewl_window_borderless_set(EWL_WINDOW(win), TRUE);
        ewl_widget_show(win);

        b = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(win), b);
        ewl_widget_show(b);

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Flip borderless");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, flip_border_cb, win);
        ewl_container_child_append(EWL_CONTAINER(b), o);
        ewl_widget_show(o);
}

static void
del_window_cb(Ewl_Widget *w, void *ev, void *data)
{
        ewl_widget_destroy(w);
}

static void
flip_border_cb(Ewl_Widget *w, void *ev, void *data)
{
        ewl_window_borderless_set(EWL_WINDOW(data),
                !ewl_window_borderless_get(EWL_WINDOW(data)));
}

/*
 * Set a string to a new window title and retrieve it again
 */
static int
title_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        const char *title;
        int ret = 0;

        win = ewl_window_new();
        title = ewl_window_title_get(EWL_WINDOW(win));

        if (title)
                snprintf(buf, len, "default title set to '%s'", title);
        else {
                ewl_window_title_set(EWL_WINDOW(win), "A title");
                title = ewl_window_title_get(EWL_WINDOW(win));

                if (strcmp(title, "A title"))
                        snprintf(buf, len, "incorrect title set");
                else {
                        ewl_window_title_set(EWL_WINDOW(win), "");
                        title = ewl_window_title_get(EWL_WINDOW(win));
                        if (title)
                                snprintf(buf, len, "non-empty title set");
                        else
                                ret = 1;
                }
        }

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a string to a new window name and retrieve it again
 */
static int
name_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        const char *name;
        int ret = 0;

        win = ewl_window_new();
        name = ewl_window_name_get(EWL_WINDOW(win));

        if (name)
                snprintf(buf, len, "default name set to '%s'", name);
        else {
                ewl_window_name_set(EWL_WINDOW(win), "A name");
                name = ewl_window_name_get(EWL_WINDOW(win));

                if (strcmp(name, "A name"))
                        snprintf(buf, len, "incorrect name set");
                else {
                        ewl_window_name_set(EWL_WINDOW(win), "");
                        name = ewl_window_name_get(EWL_WINDOW(win));
                        if (name)
                                snprintf(buf, len, "non-empty name set");
                        else
                                ret = 1;
                }
        }

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a string to a new window class and retrieve it again
 */
static int
class_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        const char *class;
        int ret = 0;

        win = ewl_window_new();
        class = ewl_window_class_get(EWL_WINDOW(win));

        if (class)
                snprintf(buf, len, "default class set to '%s'", class);
        else {
                ewl_window_class_set(EWL_WINDOW(win), "A class");
                class = ewl_window_class_get(EWL_WINDOW(win));

                if (strcmp(class, "A class"))
                        snprintf(buf, len, "incorrect class set");
                else {
                        ewl_window_class_set(EWL_WINDOW(win), "");
                        class = ewl_window_class_get(EWL_WINDOW(win));
                        if (class)
                                snprintf(buf, len, "non-empty class set");
                        else
                                ret = 1;
                }
        }

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as borderless and check that the value is set.
 */
static int
borderless_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();
        ewl_window_borderless_set(EWL_WINDOW(win), TRUE);
        if (ewl_window_borderless_get(EWL_WINDOW(win)))
                ret = 1;
        else
                snprintf(buf, len, "setting borderless TRUE failed");

        ewl_window_borderless_set(EWL_WINDOW(win), FALSE);
        if (ewl_window_borderless_get(EWL_WINDOW(win)))
                snprintf(buf, len, "setting borderless FALSE failed");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a dialog and check that the value is set.
 */
static int
dialog_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_dialog_get(EWL_WINDOW(win))) {

                ewl_window_dialog_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_dialog_get(EWL_WINDOW(win))) {

                        ewl_window_dialog_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_dialog_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "dialog unset failed");
                }
                else
                        snprintf(buf, len, "dialog set failed");
        }
        else
                snprintf(buf, len, "default dialog set");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a fullscreen and check that the value is set.
 */
static int
fullscreen_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_fullscreen_get(EWL_WINDOW(win))) {

                ewl_window_fullscreen_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_fullscreen_get(EWL_WINDOW(win))) {

                        ewl_window_fullscreen_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_fullscreen_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "fullscreen unset failed");
                }
                else
                        snprintf(buf, len, "fullscreen set failed");
        }
        else
                snprintf(buf, len, "default fullscreen set");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a skip_taskbar and check that the value is set.
 */
static int
skip_taskbar_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_skip_taskbar_get(EWL_WINDOW(win))) {

                ewl_window_skip_taskbar_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_skip_taskbar_get(EWL_WINDOW(win))) {

                        ewl_window_skip_taskbar_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_skip_taskbar_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "skip_taskbar unset failed");
                }
                else
                        snprintf(buf, len, "skip_taskbar set failed");
        }
        else
                snprintf(buf, len, "default skip_taskbar set");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a skip_pager and check that the value is set.
 */
static int
skip_pager_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_skip_pager_get(EWL_WINDOW(win))) {

                ewl_window_skip_pager_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_skip_pager_get(EWL_WINDOW(win))) {

                        ewl_window_skip_pager_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_skip_pager_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "skip_pager unset failed");
                }
                else
                        snprintf(buf, len, "skip_pager set failed");
        }
        else
                snprintf(buf, len, "default skip_pager set");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a urgent and check that the value is set.
 */
static int
urgent_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_urgent_get(EWL_WINDOW(win))) {

                ewl_window_urgent_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_urgent_get(EWL_WINDOW(win))) {

                        ewl_window_urgent_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_urgent_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "urgent unset failed");
                }
                else
                        snprintf(buf, len, "urgent set failed");
        }
        else
                snprintf(buf, len, "default urgent set");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a modal and check that the value is set.
 */
static int
modal_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_modal_get(EWL_WINDOW(win))) {

                ewl_window_modal_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_modal_get(EWL_WINDOW(win))) {

                        ewl_window_modal_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_modal_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "modal unset failed");
                }
                else
                        snprintf(buf, len, "modal set failed");
        }
        else
                snprintf(buf, len, "default modal set");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a keyboard_grab and check that the value is set.
 */
static int
keyboard_grab_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_keyboard_grab_get(EWL_WINDOW(win))) {

                ewl_window_keyboard_grab_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_keyboard_grab_get(EWL_WINDOW(win))) {

                        ewl_window_keyboard_grab_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_keyboard_grab_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "keyboard_grab unset failed");
                }
                else
                        snprintf(buf, len, "keyboard_grab set failed");
        }
        else
                snprintf(buf, len, "default keyboard_grab set");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a pointer_grab and check that the value is set.
 */
static int
pointer_grab_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_pointer_grab_get(EWL_WINDOW(win))) {

                ewl_window_pointer_grab_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_pointer_grab_get(EWL_WINDOW(win))) {

                        ewl_window_pointer_grab_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_pointer_grab_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "pointer_grab unset failed");
                }
                else
                        snprintf(buf, len, "pointer_grab set failed");
        }
        else
                snprintf(buf, len, "default pointer_grab set");

        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a window as a override and check that the value is set.
 */
static int
override_set_get(char *buf, int len)
{
        Ewl_Widget *win;
        int ret = 0;

        win = ewl_window_new();

        if (!ewl_window_override_get(EWL_WINDOW(win))) {

                ewl_window_override_set(EWL_WINDOW(win), TRUE);
                if (ewl_window_override_get(EWL_WINDOW(win))) {

                        ewl_window_override_set(EWL_WINDOW(win), FALSE);
                        if (!ewl_window_override_get(EWL_WINDOW(win)))
                                ret = 1;
                        else
                                snprintf(buf, len, "override unset failed");
                }
                else
                        snprintf(buf, len, "override set failed");
        }
        else
                snprintf(buf, len, "default override set");

        ewl_widget_destroy(win);

        return ret;
}
