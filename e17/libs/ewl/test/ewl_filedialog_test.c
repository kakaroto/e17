#include "ewl_test.h"

static Ewl_Widget *fd_button = NULL;


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
	Ewl_Widget *fd = NULL;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_filedialog_test_window);

	fd_button = w;

	fd = ewl_filedialog_new();
	ewl_window_title_set (EWL_WINDOW (fd), "File Dialog");
	ewl_window_name_set (EWL_WINDOW (fd), "EWL Test Application");
	ewl_window_class_set (EWL_WINDOW (fd), "EFL Test Application");
	ewl_object_size_request (EWL_OBJECT (fd), 500, 450);
	ewl_callback_append (fd, EWL_CALLBACK_DELETE_WINDOW,
			     __destroy_filedialog_test_window, NULL);
	ewl_widget_show(fd);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_fd_window_response (Ewl_Widget *w, int *id, void *data)
{
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
