#include <Ewl.h>

static Ewl_Widget *video;
static Ewl_Widget *fd_win;
static Ewl_Widget *seeker;

typedef struct {
    char *name;
    Ewl_Callback_Function func;
} Control;

void del_cb(Ewl_Widget *w, void *event, void *data) {
    ewl_widget_hide(w);
    ewl_widget_destroy(w);
    ewl_main_quit();

    return;
    event = NULL;
    data = NULL;
}

void play_cb(Ewl_Widget *w, void *event, void *data ) {
    ewl_media_play_set(EWL_MEDIA(video), 1);

    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void stop_cb(Ewl_Widget *w, void *event, void *data ) {
    ewl_media_play_set(EWL_MEDIA(video), 0);

    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void ff_cb(Ewl_Widget *w, void *event, void *data ) {
    double p = ewl_media_position_get(EWL_MEDIA(video));
    ewl_media_position_set(EWL_MEDIA(video), p + 10.0);

    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void rew_cb(Ewl_Widget *w, void *event, void *data ) {
    double p = ewl_media_position_get(EWL_MEDIA(video));
    ewl_media_position_set(EWL_MEDIA(video), p - 10.0);
    
    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void video_realize_cb(Ewl_Widget *w, void *event, void *data) {
    double len = ewl_media_length_get(EWL_MEDIA(video));
    ewl_seeker_set_range(EWL_SEEKER(seeker), len);

    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void video_change_cb(Ewl_Widget *w, void *event, void *data) {
    char buf[512];
    int h, m;
    double s;
    Ewl_Text *t = (Ewl_Text *)data;
    double pos = ewl_media_position_get(EWL_MEDIA(video));

    ewl_seeker_set_value(EWL_SEEKER(seeker), pos);
    ewl_media_position_time_get(EWL_MEDIA(video), &h, &m, &s);
    snprintf(buf, sizeof(buf), "%02i:%02i:%02.0f", h, m, s);
    ewl_text_text_set(t, buf);

    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void seeker_move_cb(Ewl_Widget *w, void *event, void *data) {
    double val = ewl_seeker_get_value(EWL_SEEKER(seeker));
    if (ewl_media_seekable_get(EWL_MEDIA(video)))
	    ewl_media_position_set(EWL_MEDIA(video), val);

    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void fd_win_del_cb(Ewl_Widget *w, void *event, void *data) {
    ewl_widget_hide(w);
    ewl_widget_destroy(w);
    
    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void open_file_cb(Ewl_Widget *w, void *event, void *data) {
    char *file = NULL;

    ewl_widget_hide(fd_win);
    file = (char *)event;
    if (file) 
	ewl_media_media_set(EWL_MEDIA(video), file);
    
    return;
    w = NULL;
    data = NULL;
}

void open_cb(Ewl_Widget *w, void *event, void *data ) {
    Ewl_Widget *fd = NULL;

    if (fd_win) {
	ewl_widget_show(fd_win);
	return;
    }

    fd_win = ewl_window_new();
    ewl_window_set_title(EWL_WINDOW(fd_win), "EWL Media Open");
    ewl_window_set_class(EWL_WINDOW(fd_win), "EWL_Media_Open");
    ewl_window_set_name(EWL_WINDOW(fd_win), "EWL_Media_Open");
    ewl_callback_append(fd_win, EWL_CALLBACK_DELETE_WINDOW, 
				    fd_win_del_cb, NULL);
    ewl_widget_show(fd_win);

    fd = ewl_filedialog_new(EWL_FILEDIALOG_TYPE_OPEN);
    ewl_container_append_child(EWL_CONTAINER(fd_win), fd);
    ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED, open_file_cb, NULL);
    ewl_widget_show(fd);

    return;
    w = NULL;
    event = NULL;
    data = NULL;
}

void key_up_cb(Ewl_Widget *w, void *event, void *data) {
    Ewl_Event_Key_Up *e = (Ewl_Event_Key_Up *)event;

    if (!strcmp(e->keyname, "p"))
	ewl_media_play_set(EWL_MEDIA(video), 1);

    else if (!strcmp(e->keyname, "s"))
	ewl_media_play_set(EWL_MEDIA(video), 0);

    else if (!strcmp(e->keyname, "q"))
	del_cb(w, event, data);
}

int main(int argc, char ** argv) {
    Ewl_Widget *win = NULL, *o = NULL, *b = NULL;
    Ewl_Widget *controls = NULL, *time = NULL;
    char * file = NULL;

    if (!ewl_init(&argc, argv)) {
	printf("Can't init ewl");
	return 1;
    }

    if (argc > 1)
	file = argv[1];

    win = ewl_window_new();
    ewl_window_set_title(EWL_WINDOW(win), "EWL Media test");
    ewl_window_set_name(EWL_WINDOW(win), "EWL_Media_test");
    ewl_window_set_class(EWL_WINDOW(win), "EWL_Media_test");
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, del_cb, NULL);
    ewl_callback_append(win, EWL_CALLBACK_KEY_UP, key_up_cb, NULL);
    ewl_object_size_request(EWL_OBJECT(win), 320, 280);
    ewl_object_fill_policy_set(EWL_OBJECT(win), EWL_FLAG_FILL_ALL);
    ewl_widget_show(win);

    /* box to contain everything */
    b = ewl_vbox_new();
    ewl_container_append_child(EWL_CONTAINER(win), b);
    ewl_object_fill_policy_set(EWL_OBJECT(b), EWL_FLAG_FILL_ALL);
    ewl_widget_show(b);

    /* create the time widget now so we can pass it to the video as data */
    time = ewl_text_new("00:00:00");

    /* the video */
    video = ewl_media_new(file);
    ewl_container_append_child(EWL_CONTAINER(b), video);
    ewl_object_fill_policy_set(EWL_OBJECT(video), EWL_FLAG_FILL_ALL);
    ewl_callback_append(video, EWL_CALLBACK_REALIZE, video_realize_cb, NULL);
    ewl_callback_append(video, EWL_CALLBACK_VALUE_CHANGED, video_change_cb, time);
    ewl_widget_show(video);

    /* box to contain contols and scrollers */
    controls = ewl_vbox_new();
    ewl_object_fill_policy_set(EWL_OBJECT(controls), 
		EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
    ewl_container_append_child(EWL_CONTAINER(b), controls);
    ewl_widget_show(controls);

    /* hold he controls */
    b = ewl_hbox_new();
    ewl_container_append_child(EWL_CONTAINER(controls), b);
    ewl_widget_show(b);

    {
	Control controls [] = {
		    { "play", play_cb },
		    { "stop", stop_cb },
		    { "rewind", rew_cb },
		    { "fast forward", ff_cb },
		    { "open", open_cb },
		    { NULL, NULL }
		};
	int i;

	for(i = 0; controls[i].name != NULL; i++) {
	    o = ewl_button_with_stock_new(controls[i].name);
	    ewl_container_append_child(EWL_CONTAINER(b), o);
	    ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
					controls[i].func, NULL);
	    ewl_widget_show(o);
	}
    }

    b = ewl_hbox_new();
    ewl_container_append_child(EWL_CONTAINER(controls), b);
    ewl_widget_show(b);

    /* the video seeker */
    seeker = ewl_seeker_new(EWL_ORIENTATION_HORIZONTAL);
    ewl_container_append_child(EWL_CONTAINER(b), seeker);
    ewl_object_fill_policy_set(EWL_OBJECT(seeker), 
	    EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
    ewl_seeker_set_value(EWL_SEEKER(seeker), 0.0);
    ewl_seeker_set_range(EWL_SEEKER(seeker), 0.0);
    ewl_seeker_set_step(EWL_SEEKER(seeker), 1.0);
    ewl_callback_append(seeker, EWL_CALLBACK_VALUE_CHANGED, seeker_move_cb, video);
    ewl_widget_show(seeker);

    /* the time text spot */
    ewl_container_append_child(EWL_CONTAINER(b), time);
    ewl_object_insets_set(EWL_OBJECT(time), 0, 3, 0, 0);
    ewl_object_fill_policy_set(EWL_OBJECT(time), EWL_FLAG_FILL_SHRINK);
    ewl_widget_show(time);

    ewl_main();
    return 0;
}


