#include "ewl_test.h"

static void __create_fd_window_response (Ewl_Widget *w, void *ev, void *data);

static void
__destroy_filedialog_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				    void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
}

void
__create_filedialog_test_window(Ewl_Widget *w, void *ev_data __UNUSED__,
				   void *user_data __UNUSED__)
{
	Ewl_Widget *fd = NULL, *win = NULL;

	win = ewl_window_new();
	ewl_window_title_set (EWL_WINDOW (win), "File Dialog Foo");
	ewl_window_name_set (EWL_WINDOW (win), "EWL Test Application");
	ewl_window_class_set (EWL_WINDOW (win), "EFL Test Application");

	if (w)
		ewl_callback_append (win, EWL_CALLBACK_DELETE_WINDOW,
				     __destroy_filedialog_test_window, NULL);
	else
		ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
						__close_main_window, NULL);
	ewl_widget_show(win);

	fd = ewl_filedialog_new();
	ewl_filedialog_type_set(EWL_FILEDIALOG(fd), EWL_FILEDIALOG_TYPE_OPEN);
	ewl_callback_append (fd, EWL_CALLBACK_VALUE_CHANGED, 
			    __create_fd_window_response, NULL);
	ewl_container_child_append(EWL_CONTAINER(win), fd);
	ewl_widget_show(fd);
}

static void
__create_fd_window_response (Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
  int *response;
  
  response = (int *)ev;
  switch (*response)
    {
    case EWL_RESPONSE_OPEN:
      {
	printf("file open from test program: %s\n", 
	       ewl_filedialog_file_get (EWL_FILEDIALOG (w)));
	break;
      }
    case EWL_RESPONSE_CANCEL:
      {
	printf("Test program says bugger off.\n");
        break;
      }
    }
}

