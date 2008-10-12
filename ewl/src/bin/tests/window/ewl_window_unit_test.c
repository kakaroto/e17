/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_window.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int test_constructor(char *buf, int len);
static int test_title_set_get(char *buf, int len);
static int test_name_set_get(char *buf, int len);
static int test_class_set_get(char *buf, int len);
static int test_borderless_set_get(char *buf, int len);
static int test_dialog_set_get(char *buf, int len);
static int test_fullscreen_set_get(char *buf, int len);
static int test_skip_taskbar_set_get(char *buf, int len);
static int test_skip_pager_set_get(char *buf, int len);
static int test_urgent_set_get(char *buf, int len);
static int test_modal_set_get(char *buf, int len);
static int test_keyboard_grab_set_get(char *buf, int len);
static int test_pointer_grab_set_get(char *buf, int len);
static int test_override_set_get(char *buf, int len);


/*
 * This set of tests is targeted at window
 */

Ewl_Unit_Test window_unit_tests[] = {
                {"constructor", test_constructor, NULL, -1, 0},
                {"title set/get", test_title_set_get, NULL, -1, 0},
                {"name set/get", test_name_set_get, NULL, -1, 0},
                {"class set/get", test_class_set_get, NULL, -1, 0},
                {"borderless set/get", test_borderless_set_get, NULL, -1, 0},
                {"dialog set/get", test_dialog_set_get, NULL, -1, 0},
                {"fullscreen set/get", test_fullscreen_set_get, NULL, -1, 0},
                {"skip_taskbar set/get", test_skip_taskbar_set_get, NULL, -1, 0},
                {"skip_pager set/get", test_skip_pager_set_get, NULL, -1, 0},
                {"urgent set/get", test_urgent_set_get, NULL, -1, 0},
                {"modal set/get", test_modal_set_get, NULL, -1, 0},
                {"keyboard_grab set/get", test_keyboard_grab_set_get, NULL, -1, 0},
                {"pointer_grab set/get", test_pointer_grab_set_get, NULL, -1, 0},
                {"override set/get", test_override_set_get, NULL, -1, 0},
                {NULL, NULL, NULL, -1, 0}
        };
 
/*
 * Test the default values of a newly created window
 */
static int
test_constructor(char *buf, int len)
{
        Ewl_Widget *win;
        const char *name;
        int ret = 0;

        win = ewl_window_new();

        if (!EWL_WINDOW_IS(win))
        {
                LOG_FAILURE(buf, len, "window is not of the type WINDOW");
                goto DONE;
        }

        name = ewl_window_title_get(EWL_WINDOW(win));
        if (name)
        {
                LOG_FAILURE(buf, len, "default title is '%s'", name);
                goto DONE;
        }
        name = ewl_window_name_get(EWL_WINDOW(win));
        if (name)
        {
                LOG_FAILURE(buf, len, "default name is '%s'", name);
                goto DONE;
        }
        name = ewl_window_class_get(EWL_WINDOW(win));
        if (name)
        {
                LOG_FAILURE(buf, len, "default class is '%s'", name);
                goto DONE;
        }
        if (ewl_window_borderless_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window is borderless");
                goto DONE;
        }
        if (ewl_window_dialog_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window is a dialog");
                goto DONE;
        }
        if (ewl_window_fullscreen_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window is fullscreen");
                goto DONE;
        }
        if (ewl_window_skip_taskbar_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window has the \'skip taskbar\' flag");
                goto DONE;
        }
        if (ewl_window_skip_pager_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window has the \'skip pager\' flag");
                goto DONE;
        }
        if (ewl_window_urgent_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window has the \'urgent\' flag");
                goto DONE;
        }
        if (ewl_window_leader_foreign_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window has a foreign leader set");
                goto DONE;
        }
        if (ewl_window_leader_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window has a leader set");
                goto DONE;
        }
        if (ewl_window_modal_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window is modal");
                goto DONE;
        }
        if (ewl_window_keyboard_grab_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window grabs keyboard");
                goto DONE;
        }
        if (ewl_window_pointer_grab_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window grabs pointer");
                goto DONE;
        }
        if (ewl_window_override_get(EWL_WINDOW(win)))
        {
                LOG_FAILURE(buf, len, "window is an override redirect window");
                goto DONE;
        }
        if (!ewl_embed_render_get(EWL_EMBED(win)))
        {
                LOG_FAILURE(buf, len, "window doesn't render itself");
                goto DONE;
        }
        ret = 1;
DONE:
        ewl_widget_destroy(win);

        return ret;
}

/*
 * Set a string to a new window title and retrieve it again
 */
static int
test_title_set_get(char *buf, int len)
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
test_name_set_get(char *buf, int len)
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
test_class_set_get(char *buf, int len)
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
test_borderless_set_get(char *buf, int len)
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
test_dialog_set_get(char *buf, int len)
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
test_fullscreen_set_get(char *buf, int len)
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
test_skip_taskbar_set_get(char *buf, int len)
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
test_skip_pager_set_get(char *buf, int len)
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
test_urgent_set_get(char *buf, int len)
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
test_modal_set_get(char *buf, int len)
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
test_keyboard_grab_set_get(char *buf, int len)
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
test_pointer_grab_set_get(char *buf, int len)
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
test_override_set_get(char *buf, int len)
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
