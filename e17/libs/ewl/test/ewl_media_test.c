#include <Ewl.h>

static Ewl_Widget *video;
static Ewl_Widget *fd_win;

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
    Ewl_Widget *controls = NULL;
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
    ewl_object_request_size(EWL_OBJECT(win), 200, 300);
    ewl_widget_show(win);

    /* box to contain everything */
    b = ewl_vbox_new();
    ewl_container_append_child(EWL_CONTAINER(win), b);
    ewl_widget_show(b);

    /* the video */
    video = ewl_media_new(file);
    ewl_container_append_child(EWL_CONTAINER(b), video);
    ewl_widget_show(video);

    /* box to contain contols and scrollers */
    controls = ewl_vbox_new();
    ewl_object_set_fill_policy(EWL_OBJECT(controls), 
		EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
    ewl_container_append_child(EWL_CONTAINER(b), controls);
    ewl_widget_show(controls);

    /* hold he controls */
    b = ewl_hbox_new();
    ewl_container_append_child(EWL_CONTAINER(controls), b);
    ewl_widget_show(b);

    {
	Control controls [] = {
		    { "Play", play_cb },
		    { "Stop", stop_cb },
		    { "Rew", rew_cb },
		    { "FF", ff_cb },
		    { "Open", open_cb },
		    { NULL, NULL }
		};
	int i;

	for(i = 0; controls[i].name != NULL; i++) {
	    o = ewl_button_new(controls[i].name);
	    ewl_container_append_child(EWL_CONTAINER(b), o);
	    ewl_callback_append(o, EWL_CALLBACK_CLICKED, 
					controls[i].func, NULL);
	    ewl_widget_show(o);
	}
    }

    ewl_main();
    return 0;
}


