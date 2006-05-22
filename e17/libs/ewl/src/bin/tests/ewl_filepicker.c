#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <string.h>

/**
 * @addtogroup Ewl_Filepicker
 * @section filepicker_tut Tutorial
 *
 * The Ewl_Filepicker widget provides a method for selecting files. The
 * filepicker can support multiple views of the files, this also allows the
 * app to provide it's own view.
 *
 * @code
 * Ewl_Widget *fp;
 * fp = ewl_filepicker_new();
 * ewl_widget_show(fp);
 * @endcode
 *
 * The filepicker will default to the users home directory. If that dosen't
 * exist it will attemp to use /tmp, failing that it will fall back to the /
 * directory. The following two calls can be used to retrieve or set the
 * current directory.
 *
 * @code
 * ewl_filepicker_directory_set(fp);
 * ewl_filepicker_directory_get(fp);
 * @endcode
 *
 * Along with changing the directory you can also specify if the picker will
 * allow for multiple file selections, showing dot files, showing the
 * favorites panel or setting the file filter. This is done with the 
 * following code:
 *
 * @code
 * ewl_filepicker_multiselect_set(fp, TRUE);
 * ms = ewl_filepicker_multiselect_get(fp);
 *
 * ewl_filepicker_show_dot_files_set(fp, TRUE);
 * dot = ewl_filepicker_show_dot_files_get(fp);
 *
 * ewl_filepicker_show_favorites_set(fp, TRUE);
 * fav = ewl_filepicker_show_favorites_get(fp);
 * 
 * ewl_filepicker_filter_set(fp, filter);
 * filter = ewl_filepicker_filter_get(fp);
 * @endcode
 *
 * You can also change the view used for the file list.
 *
 * @code 
 * ewl_filepicker_list_view_set(fp, view);
 * view = ewl_filepicker_list_view_get(fp);
 * @endcode
 *
 * @section filepicker_add_view Adding Views
 * A view of the file list in the filepicker is a subclass of the
 * Ewl_Filelist widget. These subclasses will provide the code for the
 * actual list display. They will receive callbacks as settings are changed
 * on the filepicker to notify of the display changes.
 *
 * If you wish to write your own view you will need to create an Ewl_View
 * and set the constructor for your widget as the construct field. The
 * constructor will need to setup the appropriate callbacks to as specified
 * in the Ewl_Filelist structure.
 */

static int create_test(Ewl_Container *box);
static void ewl_filepicker_cb_value_changed(Ewl_Widget *w, void *ev, 
							void *data);
static int directory_test_set_get(char *buf, int len);
static int directory_test_path_split(char *buf, int len);

static Ewl_Unit_Test filepicker_unit_tests[] = {
		{"directory set/get", directory_test_set_get},
		{"directory path split", directory_test_path_split},
		{NULL, NULL}
	};

void 
test_info(Ewl_Test *test)
{
	test->name = "Filepicker";
	test->tip = "A simple file picking widget";
	test->filename = "ewl_filepicker.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
	test->unit_tests = filepicker_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *w;

	w = ewl_filepicker_new();
	ewl_filepicker_filter_add(EWL_FILEPICKER(w), "C Files", "*.c");
	ewl_filepicker_filter_add(EWL_FILEPICKER(w), "D Files", "*.d");
	ewl_container_child_append(box, w);
	ewl_callback_append(w, EWL_CALLBACK_VALUE_CHANGED,
			ewl_filepicker_cb_value_changed, NULL);
	ewl_widget_show(w);

	return 1;
}

static void
ewl_filepicker_cb_value_changed(Ewl_Widget *w, void *ev, 
					void *data __UNUSED__)
{
	Ewl_Filepicker *fp;
	Ewl_Filepicker_Event *e;

	fp = EWL_FILEPICKER(w);
	e = ev;

	/* need to deal with multiselect in here */
	switch (e->response)
	{
		case EWL_STOCK_OK:
		{
			char *file;

			file = ewl_filepicker_selected_file_get(fp);

			if (!file)
				printf("No file selected.\n");
			else
				printf("Selected %s/%s\n",
					ewl_filepicker_directory_get(fp),
					file);
			break;
		}
		default:
		case EWL_STOCK_CANCEL:
			printf("Cancelled\n");
			break;
	}
}

static int
directory_test_set_get(char *buf, int len)
{
	Ewl_Widget *fp;
	const char *t;
	int ret = 0;

	fp = ewl_filepicker_new();
	ewl_filepicker_directory_set(EWL_FILEPICKER(fp),
					"/foo/bar/baz/test");
	t = ewl_filepicker_directory_get(EWL_FILEPICKER(fp));

	if (strcmp(t, "/foo/bar/baz/test"))
		snprintf(buf, len, "directory_get did not match "
						"directory_set");
	else
		ret = 1;

	return ret;
}

static int
directory_test_path_split(char *buf, int len)
{
	Ewl_Widget *fp;
	char *t;
	int i = 0, ret = 1;
	char *rez[] = {"/foo/bar/baz/test", "/foo/bar/baz", "/foo/bar", "/foo", "/"};

	fp = ewl_filepicker_new();
	ewl_filepicker_directory_set(EWL_FILEPICKER(fp),
					"/foo/bar/baz/test/");

	ecore_list_goto_first(EWL_FILEPICKER(fp)->path);
	while ((t = ecore_list_next(EWL_FILEPICKER(fp)->path)))
	{
		if (strcmp(rez[i], t))
		{
			snprintf(buf, len, "path list not setup correctly. "
					"[%s didn't match %s]", t, rez[i]);
			ret = 0;
			break;
		}
		i++;
	}

	return ret;
}


