#include "ewl_test.h"

static Ewl_Widget *fd_button = NULL;
static Ewl_Widget *vbox;


void __start_fd (Ewl_Widget *w, void *ev_data, void *user_data);
void __destroy_fd_window (Ewl_Widget *w, void *ev, void *data);

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
	Ewl_Widget *button;
	Ewl_Widget *fd_win;
	Ewl_Widget *separator;
	
	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_filedialog_test_window);

	fd_button = w;

	fd_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(fd_win), "File Dialog Test");
	ewl_window_name_set(EWL_WINDOW(fd_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(fd_win), "EFL Test Application");
	ewl_object_size_request(EWL_OBJECT(fd_win), 500, 400);
	ewl_object_fill_policy_set(EWL_OBJECT(fd_win), EWL_FLAG_FILL_FILL |
			EWL_FLAG_FILL_SHRINK);
	ewl_callback_append(fd_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_filedialog_test_window, NULL);
	ewl_widget_show(fd_win);

	vbox = ewl_vbox_new ();
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_FILL |
				EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(fd_win), vbox);
	ewl_widget_show (vbox);

	button = ewl_button_new("Start filedialog");
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_NONE);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append(EWL_CONTAINER(vbox), button);
	ewl_widget_show(button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, __start_fd, NULL);

	separator = ewl_hseparator_new ();
	ewl_container_child_append(EWL_CONTAINER(vbox), separator);
	ewl_widget_show (separator);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void __start_fd (Ewl_Widget *w, void *ev_data, void *user_data)
{
	static Ewl_Widget *fd = NULL;

	if (fd) {
	        ewl_widget_show (EWL_WINDOW (fd));
	        ewl_window_raise (EWL_WINDOW (fd));
		return;
	}

	fd = ewl_filedialog_new();
	ewl_window_title_set (EWL_WINDOW (fd), "File Dialog");
	ewl_window_name_set (EWL_WINDOW (fd), "EWL Test Application");
	ewl_window_class_set (EWL_WINDOW (fd), "EFL Test Application");
	ewl_object_size_request (EWL_OBJECT (fd), 500, 450);
	ewl_callback_append (fd, EWL_CALLBACK_DELETE_WINDOW,
			     EWL_CALLBACK_FUNCTION (__destroy_fd_window), NULL);
	ewl_widget_show(fd);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__destroy_fd_window (Ewl_Widget *w, void *ev, void *data)
{
  ewl_widget_destroy (EWL_WIDGET (w));

  return;
  w = NULL;
  ev = NULL;
  data = NULL;
}

void
__create_fd_window_response (Ewl_Widget *w, int *id, void *data)
{
  Ewl_Widget        *label;
  int                response = (int)*id;

  switch (response)
    {
    case EWL_RESPONSE_OK:
      {
	printf("file open from test program: %s\n", 
	       ewl_filedialog_file_get (EWL_FILEDIALOG (w)));

	ewl_widget_hide(w);

	break;
      }
    case EWL_RESPONSE_CANCEL:
      {
	ewl_widget_hide(w);

	break;
      }
    }

  return;

  w = NULL;
  id = NULL;
  data = NULL;
}
