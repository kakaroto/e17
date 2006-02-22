#include <Ewl.h>
#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

static Ewl_Widget *video;
static Ewl_Widget *fd_win;
static Ewl_Widget *seeker;

typedef struct {
	Ewl_Stock_Type name;
	Ewl_Callback_Function func;
} Control;

static void
del_cb(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__) 
{
	ewl_widget_hide(w);
	ewl_widget_destroy(w);
	ewl_main_quit();
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
video_realize_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__, 
		void *data __UNUSED__)
{
	double len;

	len = ewl_media_length_get(EWL_MEDIA(video));
	ewl_seeker_range_set(EWL_SEEKER(seeker), len);
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

	ewl_seeker_value_set(EWL_SEEKER(seeker), pos);
	ewl_media_position_time_get(EWL_MEDIA(video), &h, &m, &s);
	snprintf(buf, sizeof(buf), "%02i:%02i:%02.0f", h, m, s);
	ewl_text_text_set(t, buf);
}

static void
seeker_move_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__,
		void *data __UNUSED__) 
{
	double val;

	val = ewl_seeker_value_get(EWL_SEEKER(seeker));
	if (ewl_media_seekable_get(EWL_MEDIA(video)))
		ewl_media_position_set(EWL_MEDIA(video), val);
}

static void
fd_win_del_cb(Ewl_Widget *w, void *event __UNUSED__, void *data __UNUSED__)
{
	ewl_widget_hide(w);
	ewl_widget_destroy(w);
}

static void
open_file_cb(Ewl_Widget *w, void *event, void *data __UNUSED__)
{
	Ewl_Dialog_Event *e;
	char *file = NULL;

	ewl_widget_hide(fd_win);

	e = event;
	if (e->response == EWL_STOCK_OPEN)
		file = ewl_filedialog_file_get(EWL_FILEDIALOG(w));

	if (file) 
		ewl_media_media_set(EWL_MEDIA(video), file);
}

static void
open_cb(Ewl_Widget *w __UNUSED__, void *event __UNUSED__,
		void *data __UNUSED__)
{
	Ewl_Widget *fd = NULL;

	if (fd_win) {
		ewl_widget_show(fd_win);
		return;
	}

	fd_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(fd_win), "EWL Media Open");
	ewl_window_class_set(EWL_WINDOW(fd_win), "EWL_Media_Open");
	ewl_window_name_set(EWL_WINDOW(fd_win), "EWL_Media_Open");
	ewl_callback_append(fd_win, EWL_CALLBACK_DELETE_WINDOW, 
			fd_win_del_cb, NULL);
	ewl_widget_show(fd_win);

	fd = ewl_filedialog_new();
	ewl_filedialog_type_set(EWL_FILEDIALOG(fd), EWL_FILEDIALOG_TYPE_OPEN);
	ewl_container_child_append(EWL_CONTAINER(fd_win), fd);
	ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED, open_file_cb, NULL);
	ewl_widget_show(fd);
}

static void
key_up_cb(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Event_Key_Up *e = (Ewl_Event_Key_Up *)event;

	if (!strcmp(e->keyname, "p"))
		ewl_media_play_set(EWL_MEDIA(video), 1);

	else if (!strcmp(e->keyname, "s"))
		ewl_media_play_set(EWL_MEDIA(video), 0);

	else if (!strcmp(e->keyname, "q"))
		del_cb(w, event, data);
}

int
main(int argc, char ** argv) 
{
	Ewl_Widget *win = NULL, *o = NULL, *b = NULL;
	Ewl_Widget *controls = NULL, *time = NULL;
	Ewl_Media_Module_Type type = EWL_MEDIA_MODULE_XINE;
	char * file = NULL;
	int i;

	if (!ewl_init(&argc, argv)) {
		printf("Can't init ewl");
		return 1;
	}

	if (!ewl_media_is_available())
	{
		printf("Ewl_Media is not available. Please install Emotion "
			"and rebuild Ewl\n");
		return 1;
	}

	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-gstreamer"))
			type = EWL_MEDIA_MODULE_GSTREAMER;

		else if (!strcmp(argv[i], "-xine"))
			type = EWL_MEDIA_MODULE_XINE;
		else
			file = argv[i];
	}

	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "EWL Media test");
	ewl_window_name_set(EWL_WINDOW(win), "EWL_Media_test");
	ewl_window_class_set(EWL_WINDOW(win), "EWL_Media_test");
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, del_cb, NULL);
	ewl_callback_append(win, EWL_CALLBACK_KEY_UP, key_up_cb, NULL);
	ewl_object_size_request(EWL_OBJECT(win), 320, 280);
	ewl_object_fill_policy_set(EWL_OBJECT(win), EWL_FLAG_FILL_ALL);
	ewl_widget_show(win);

	/* box to contain everything */
	b = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), b);
	ewl_object_fill_policy_set(EWL_OBJECT(b), EWL_FLAG_FILL_ALL);
	ewl_widget_show(b);

	/* create the time widget now so we can pass it to the video as data */
	time = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(time), "00:00:00");

	/* the video */
	video = ewl_media_new();
	ewl_container_child_append(EWL_CONTAINER(b), video);
	ewl_media_module_set(EWL_MEDIA(video), type);
	ewl_media_media_set(EWL_MEDIA(video), file);
	ewl_object_fill_policy_set(EWL_OBJECT(video), EWL_FLAG_FILL_ALL);
	ewl_callback_append(video, EWL_CALLBACK_REALIZE, video_realize_cb, NULL);
	ewl_callback_append(video, EWL_CALLBACK_VALUE_CHANGED, video_change_cb, time);
	ewl_widget_show(video);

	/* box to contain contols and scrollers */
	controls = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(controls), 
			EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(b), controls);
	ewl_widget_show(controls);

	/* hold he controls */
	b = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(controls), b);
	ewl_widget_show(b);

	{
		Control controls [] = {
			{ EWL_STOCK_PLAY, play_cb },
			{ EWL_STOCK_STOP, stop_cb },
			{ EWL_STOCK_REWIND, rew_cb },
			{ EWL_STOCK_FASTFORWARD, ff_cb },
			{ EWL_STOCK_OPEN, open_cb },
			{ EWL_STOCK_NONE, NULL }
		};
		int i;

		for(i = 0; controls[i].name != EWL_STOCK_NONE; i++) {
			o = ewl_button_new();
			ewl_button_stock_type_set(EWL_BUTTON(o), controls[i].name);
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
			EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
	ewl_seeker_value_set(EWL_SEEKER(seeker), 0.0);
	ewl_seeker_range_set(EWL_SEEKER(seeker), 0.0);
	ewl_seeker_step_set(EWL_SEEKER(seeker), 1.0);
	ewl_callback_append(seeker, EWL_CALLBACK_VALUE_CHANGED, seeker_move_cb, video);
	ewl_widget_show(seeker);

	/* the time text spot */
	ewl_container_child_append(EWL_CONTAINER(b), time);
	ewl_object_insets_set(EWL_OBJECT(time), 0, 3, 0, 0);
	ewl_object_fill_policy_set(EWL_OBJECT(time), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(time);

	ewl_main();
	return 0;
}


