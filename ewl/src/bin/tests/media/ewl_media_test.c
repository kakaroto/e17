/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_filedialog.h"
#include "ewl_label.h"
#include "ewl_media.h"
#include "ewl_radiobutton.h"
#include "ewl_seeker.h"
#include "ewl_text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

static Ewl_Widget *video, *seeker, *fd_win;
static Ewl_Media_Module_Type module_type;

typedef struct {
        Ewl_Stock_Type name;
        Ewl_Callback_Function func;
} Ewl_Media_Test_Control;

static int create_test(Ewl_Container *box);
static void create_media_window(Ewl_Media_Module_Type type);
static void cb_launch(Ewl_Widget *w, void *ev, void *data);
static void cb_gstreamer_change(Ewl_Widget *w, void *ev, void *data);
static void cb_xine_change(Ewl_Widget *w, void *ev, void *data);
static void del_cb(Ewl_Widget *w, void *event, void *data);
static void play_cb(Ewl_Widget *w, void *event, void *data);
static void stop_cb(Ewl_Widget *w, void *event, void *data);
static void ff_cb(Ewl_Widget *w, void *event, void *data);
static void rew_cb(Ewl_Widget *w, void *event, void *data);
static void video_change_cb(Ewl_Widget *w, void *event, void *data);
static void seeker_move_cb(Ewl_Widget *w, void *event, void *data);
static void open_file_cb(Ewl_Widget *w, void *event, void *data);
static void open_cb(Ewl_Widget *w, void *event, void *data);
static void key_up_cb(Ewl_Widget *w, void *event, void *data);

extern Ewl_Unit_Test media_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Media";
        test->tip = "A widget to display media files.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_ADVANCED;
        test->unit_tests = media_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *o, *o2;

        if (!ewl_media_is_available())
        {
                o = ewl_label_new();
                ewl_label_text_set(EWL_LABEL(o),
                                "Ewl_Media is not available. "
                                "Please install Emotion "
                                "and rebuild Ewl.");
                ewl_container_child_append(box, o);
                ewl_widget_show(o);

                return 1;
        }

        o = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(o), "Gstreamer");
        ewl_container_child_append(box, o);
        ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
                                        cb_gstreamer_change, NULL);
        ewl_widget_show(o);

        o2 = ewl_radiobutton_new();
        ewl_button_label_set(EWL_BUTTON(o2), "Xine");
        ewl_radiobutton_chain_set(EWL_RADIOBUTTON(o2), EWL_RADIOBUTTON(o));
        ewl_togglebutton_checked_set(EWL_TOGGLEBUTTON(o2), TRUE);
        ewl_container_child_append(box, o2);
        ewl_callback_append(o2, EWL_CALLBACK_VALUE_CHANGED,
                                        cb_xine_change, NULL);
        ewl_widget_show(o2);

        module_type = EWL_MEDIA_MODULE_XINE;

        o = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(o), "Launch media test");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, cb_launch, NULL);
        ewl_container_child_append(box, o);
        ewl_widget_show(o);

        return 1;
}

static void
cb_gstreamer_change(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        module_type = EWL_MEDIA_MODULE_GSTREAMER;
}

static void
cb_xine_change(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        module_type = EWL_MEDIA_MODULE_XINE;
}

static void
cb_launch(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data __UNUSED__)
{
        create_media_window(module_type);
}

static void
del_cb(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__)
{
        ewl_widget_destroy(w);
}

static void
play_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__,
                                void *data __UNUSED__)
{
        ewl_media_play_set(EWL_MEDIA(video), 1);
}

static void
stop_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__,
                                void *data __UNUSED__)
{
        ewl_media_play_set(EWL_MEDIA(video), 0);
}

static void
ff_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__,
                                void *data __UNUSED__)
{
        double p;

        p = ewl_media_position_get(EWL_MEDIA(video));
        ewl_media_position_set(EWL_MEDIA(video), p + 10.0);
}

static void
rew_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__,
                                void *data __UNUSED__)
{
        double p;

        p = ewl_media_position_get(EWL_MEDIA(video));
        ewl_media_position_set(EWL_MEDIA(video), p - 10.0);
}

static void
video_change_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__, void *data)
{
        char buf[512];
        int h, m;
        double s;
        Ewl_Text *t;
        double pos;

        t = data;
        pos = ewl_media_position_get(EWL_MEDIA(video));

        ewl_range_value_set(EWL_RANGE(seeker), pos);
        ewl_media_position_time_get(EWL_MEDIA(video), &h, &m, &s);
        snprintf(buf, sizeof(buf), "%02i:%02i:%02.0f", h, m, s);
        ewl_text_text_set(t, buf);
}

static void
seeker_move_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__,
                                        void *data __UNUSED__)
{
        double val;

        val = ewl_range_value_get(EWL_RANGE(seeker));
        if (ewl_media_seekable_get(EWL_MEDIA(video)))
                ewl_media_position_set(EWL_MEDIA(video), val);
}

static void
open_file_cb(Ewl_Widget *w, void *event, void *data __UNUSED__)
{
        Ewl_Event_Action_Response *e;

        ewl_widget_hide(fd_win);

        e = event;
        if (e->response == EWL_STOCK_OPEN)
        {
                int len;
                char *file = NULL;

                file = ewl_filedialog_selected_file_get(EWL_FILEDIALOG(w));
                ewl_media_media_set(EWL_MEDIA(video), file);
                if (file) free(file);

                len = ewl_media_length_get(EWL_MEDIA(video));
                ewl_range_maximum_value_set(EWL_RANGE(seeker), len);
        }
}

static void
open_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__,
                                        void *data __UNUSED__)
{
        if (!fd_win)
        {
                fd_win = ewl_filedialog_new();
                ewl_callback_append(fd_win, EWL_CALLBACK_VALUE_CHANGED, open_file_cb, NULL);
        }

        ewl_widget_show(fd_win);
}

static void
key_up_cb(Ewl_Widget *w, void *event, void *data)
{
        Ewl_Event_Key *e = event;

        if (!strcmp(e->keyname, "p"))
                ewl_media_play_set(EWL_MEDIA(video), 1);

        else if (!strcmp(e->keyname, "s"))
                ewl_media_play_set(EWL_MEDIA(video), 0);

        else if (!strcmp(e->keyname, "q"))
                del_cb(w, event, data);
}

static void
create_media_window(Ewl_Media_Module_Type type)
{
        Ewl_Widget *win, *o = NULL, *b = NULL;
        Ewl_Widget *controls = NULL, *time = NULL;

        win = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(win), "EWL Media test");
        ewl_window_name_set(EWL_WINDOW(win), "EWL_Media_test");
        ewl_window_class_set(EWL_WINDOW(win), "EWL_Media_test");
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, del_cb, NULL);
        ewl_callback_append(win, EWL_CALLBACK_KEY_UP, key_up_cb, NULL);
        ewl_object_h_request(EWL_OBJECT(win), 280);
        ewl_object_fill_policy_set(EWL_OBJECT(win), EWL_FLAG_FILL_FILL);
        ewl_widget_show(win);

        /* box to contain everything */
        b = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(win), b);
        ewl_object_fill_policy_set(EWL_OBJECT(b), EWL_FLAG_FILL_FILL);
        ewl_widget_show(b);

        /* create the time widget now so we can pass it to the video as data */
        time = ewl_text_new();
        ewl_text_text_set(EWL_TEXT(time), "00:00:00");

        /* the video */
        video = ewl_media_new();
        ewl_container_child_append(EWL_CONTAINER(b), video);
        ewl_media_module_set(EWL_MEDIA(video), type);
        ewl_object_fill_policy_set(EWL_OBJECT(video), EWL_FLAG_FILL_ALL);
        ewl_callback_append(video, EWL_CALLBACK_VALUE_CHANGED, video_change_cb, time);
        ewl_widget_show(video);

        /* box to contain contols and scrollers */
        controls = ewl_vbox_new();
        ewl_object_fill_policy_set(EWL_OBJECT(controls), EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(b), controls);
        ewl_widget_show(controls);

        /* hold the controls */
        b = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(controls), b);
        ewl_widget_show(b);

        {
                Ewl_Media_Test_Control controls [] = {
                        { EWL_STOCK_MEDIA_PLAY, play_cb },
                        { EWL_STOCK_MEDIA_STOP, stop_cb },
                        { EWL_STOCK_MEDIA_REWIND, rew_cb },
                        { EWL_STOCK_MEDIA_FASTFORWARD, ff_cb },
                        { EWL_STOCK_OPEN, open_cb },
                        { EWL_STOCK_NONE, NULL }
                };
                int i;

                for(i = 0; controls[i].name != EWL_STOCK_NONE; i++) {
                        o = ewl_button_new();
                        ewl_stock_type_set(EWL_STOCK(o), controls[i].name);
                        ewl_container_child_append(EWL_CONTAINER(b), o);
                        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        controls[i].func, NULL);
                        ewl_widget_show(o);
                }
        }

        b = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(controls), b);
        ewl_widget_show(b);

        /* the video seeker */
        seeker = ewl_hseeker_new();
        ewl_container_child_append(EWL_CONTAINER(b), seeker);
        ewl_object_fill_policy_set(EWL_OBJECT(seeker),
                        EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_HFILL);
        ewl_range_value_set(EWL_RANGE(seeker), 0.0);
        ewl_range_maximum_value_set(EWL_RANGE(seeker), 0.0);
        ewl_range_step_set(EWL_RANGE(seeker), 1.0);
        ewl_callback_append(seeker, EWL_CALLBACK_VALUE_CHANGED, seeker_move_cb, video);
        ewl_widget_show(seeker);

        /* the time text spot */
        ewl_container_child_append(EWL_CONTAINER(b), time);
        ewl_object_insets_set(EWL_OBJECT(time), 0, 3, 0, 0);
        ewl_object_fill_policy_set(EWL_OBJECT(time), EWL_FLAG_FILL_NONE);
        ewl_widget_show(time);
}

