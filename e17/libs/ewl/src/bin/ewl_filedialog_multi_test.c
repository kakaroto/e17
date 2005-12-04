#include "ewl_test.h"

static void __create_fdm_window_response(Ewl_Widget *w, void *ev, void *data);
static void __fdm_print(void *value, void *data);

static void
__destroy_filedialog_multi_test_window(Ewl_Widget *w, void *ev_data __UNUSED__,
		void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
}

void
__create_filedialog_multi_test_window(Ewl_Widget *w __UNUSED__, 
		void *ev_data __UNUSED__,
		void *user_data __UNUSED__)
{
	Ewl_Widget *fd = NULL;

	fd = ewl_filedialog_multiselect_new();

	if (w)
		ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW,
				__destroy_filedialog_multi_test_window, NULL);
	else
		ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW,
				__close_main_window, NULL);

	ewl_callback_append (fd, EWL_CALLBACK_VALUE_CHANGED, 
				__create_fdm_window_response, NULL);
	ewl_widget_show(fd);
}

static void
__create_fdm_window_response (Ewl_Widget *w __UNUSED__, void *ev, 
					void *data __UNUSED__)
{
	Ewl_Dialog_Event *e;

	e = ev;
	if (e->response == EWL_STOCK_OPEN)
	{
		Ecore_List *list;
		list = ewl_filedialog_select_list_get(EWL_FILEDIALOG(w));

		ecore_list_for_each(list, __fdm_print, NULL);
	}
	else if (e->response == EWL_STOCK_CANCEL)
		printf("Test program says bugger off.\n");
}

static void
__fdm_print(void *value, void *data __UNUSED__)
{
	char *file;

	file = value;
	printf("Selected: %s\n", file);
}



