#include <Ewl.h>

static Ewl_Widget *fd_button = NULL;
		

void __create_filedialog_test_window(Ewl_Widget * w, void *ev_data,
					void *user_data);
void __start_fd (Ewl_Widget *w, void *ev_data, void *user_data);
void __open_file (Ewl_Widget *, void *ev_data, void *user_data);

void
__destroy_filedialog_test_window(Ewl_Widget * w, void *ev_data,
				    void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(fd_button, EWL_CALLBACK_CLICKED,
			    __create_filedialog_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_filedialog_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data)
{
	Ewl_Widget *fd_win;
	Ewl_Widget *button;
	
	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_filedialog_test_window);

	fd_button = w;


	fd_win = ewl_window_new();
	ewl_callback_append(fd_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_filedialog_test_window, NULL);
	ewl_widget_show(fd_win);

	
	button = ewl_button_new("Filedialog");
	ewl_object_set_fill_policy(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_object_set_alignment(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);
	ewl_container_append_child(EWL_CONTAINER(fd_win), button);
	ewl_widget_show(button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, __start_fd, NULL);
	
	

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}


void __start_fd (Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Widget *fd;
	
	fd = ewl_filedialog_new(w->parent, EWL_FILEDIALOG_TYPE_OPEN,
			__open_file);
	ewl_object_set_fill_policy(EWL_OBJECT(fd), EWL_FLAG_FILL_FILL);
//	ewl_container_append_child(EWL_CONTAINER(fd_win), fd);
	ewl_widget_show(fd);

	ev_data = NULL;
	user_data = NULL;
}


void __open_file (Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Fileselector *fs;
	Ewl_Table *t;

	fs = EWL_FILESELECTOR(user_data);
	t = EWL_TABLE(fs->files);
	
	printf("file clicked = %s\n", ewl_table_get_selected(t));

	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
