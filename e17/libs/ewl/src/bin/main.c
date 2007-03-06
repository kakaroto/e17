/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>

#define MAIN_WIDTH 640
#define MAIN_HEIGHT 320

static Ewl_Model *expansion_model = NULL;

static char *ewl_test_about_body = 
		"The EWL Test application services two purposes\n"
		"The first is to allow us to test the different\n"
		"pices of EWL as we develop and work on them.\n\n"
		"The second piece is to allow deveopers to see\n"
		"tutorials and source listings for the different\n"
		"widgets in the system.\n\n"
		"If you develop your own EWL widget you can add\n"
		"a test to the test widget without needing to\n"
		"recompile or edit the test application.\n";

static int ewl_test_setup_tests(void);
static void ewl_test_print_tests(void);

static void run_test_boxed(Ewl_Test *t);
static void run_unit_test_boxed(Ewl_Test *t);
static void run_window_test(Ewl_Test *test, int width, int height);
static int run_unit_tests(Ewl_Test *test);
static int create_main_test_window(Ewl_Container *win);
static void fill_source_text(Ewl_Test *test);
static void text_parse(char *str);
static void tutorial_parse(Ewl_Text *tutorial, char *str);
static void setup_unit_tests(Ewl_Test *test);

static int ewl_test_cb_unit_test_timer(void *data);
static void ewl_test_cb_delete_window(Ewl_Widget *w, void *ev, void *data);
static void ewl_test_cb_exit(Ewl_Widget *w, void *ev, void *data);
static void cb_run_unit_tests(Ewl_Widget *w, void *ev, void *data);

static void ewl_test_cb_help(Ewl_Widget *w, void *ev, void *data);
static void ewl_test_cb_about(Ewl_Widget *w, void *ev, void *data);
	
static void *ewl_test_cb_category_fetch(void *data, unsigned int row, 
						unsigned int column);
static int ewl_test_cb_category_expandable(void *data, unsigned int row);
static void *ewl_test_cb_category_expansion_fetch(void *data, unsigned int row);
static Ewl_Model *ewl_test_cb_category_expansion_model_fetch(void *data, 
							unsigned int row);
static void *ewl_test_cb_expansion_fetch(void *data, unsigned int row, 
							unsigned int column);
static void ewl_test_cb_test_selected(Ewl_Widget *w, void *ev, void *data);

static Ecore_List *tests = NULL;
static int window_count = 0;
static int current_unit_test = 0;
static Ecore_Timer *unit_test_timer = NULL;

static Ewl_Test *current_test = NULL;

int
main(int argc, char **argv)
{
	int i, unit_test = 0, ran_test = 0, ret = 0;

	/* initialize the ewl library */
	if (!ewl_init(&argc, argv))
	{
		printf("Unable to init EWL.\n");
		return 1;
	}

	if (!ewl_test_setup_tests())
	{
		printf("Unable to setup tests.\n");
		return 1;
	}

	/* check for any flags */
	for (i = 0; i < argc; i++)
	{
		Ewl_Test *t;

		if (!strncmp(argv[i], "-list", 5))
		{
			ewl_test_print_tests();
			exit(0);
		}
		else if (!strncmp(argv[i], "-h", 2) ||
				!strncmp(argv[i], "-help", 5) ||
				!strncmp(argv[i], "--help", 6))
		{
			printf("Usage: %s [options] [test name]\n"
					"\t-all\tRun tests for all widgets\n"
					"\t-help\tDisplay this help text\n"
					"\t-list\tPrint available tests\n"
					"\t-unit\tRun unit tests\n",
					argv[0]);
			exit(0);
		}
		else if (!strncmp(argv[i], "-all", 4))
		{
			Ewl_Test *t;

			ecore_list_goto_first(tests);
			while ((t = ecore_list_next(tests)))
				run_window_test(t, MAIN_WIDTH, MAIN_HEIGHT);
		}
		else if (!strncmp(argv[i], "-unit", 5))
			unit_test = 1;

		/* see if this thing was a test to run */
		ecore_list_goto_first(tests);
		while ((t = ecore_list_next(tests)))
		{
			if (!strcasecmp(argv[i], t->name))
			{
				if (unit_test)
					ret = run_unit_tests(t);
				else
					run_window_test(t, MAIN_WIDTH, 
								MAIN_HEIGHT);

				ran_test ++;
				break;
			}
		}
	}

	/* no passed in tests, run the main test app */
	if (!ran_test)
	{
		Ewl_Test test;

		test.name = "The Enlightened Widget Library Test App";
		test.func = create_main_test_window;
		test.filename = NULL;
		test.tip = NULL;

		run_window_test(&test, MAIN_WIDTH, MAIN_HEIGHT);
	}

	if (!unit_test)
		ewl_main();

	if (tests) ecore_list_destroy(tests);

	return ret;
}

static int
ewl_test_cb_unit_test_timer(void *data)
{
	char buf[1024];
	char *entries[3];
	Ewl_Unit_Test *unit_tests;
	int ret = 1;

	unit_tests = data;
	if (unit_tests[current_unit_test].func)
	{
		int val;
		Ewl_Widget *tree, *progress, *stat;

		stat = ewl_widget_name_find("statusbar");
		ewl_statusbar_push(EWL_STATUSBAR(stat), 
				(char *)unit_tests[current_unit_test].name);
		val = unit_tests[current_unit_test].func(buf, sizeof(buf));
		ewl_statusbar_pop(EWL_STATUSBAR(stat));

		tree = ewl_widget_name_find("unit_test_tree");
		progress = ewl_widget_name_find("unit_test_progress");

		entries[0] = (char *)unit_tests[current_unit_test].name;
		entries[1] = (val ? "PASS" : "FAIL");
		entries[2] = (val ? "" : buf);
		ewl_tree_text_row_add(EWL_TREE(tree), NULL, entries);

		ewl_range_value_set(EWL_RANGE(progress),
				(double)(++current_unit_test));
	}
	else
	{
		unit_test_timer = NULL;
		current_unit_test = 0;
		ret = 0;
	}

	return ret;
}

static void
ewl_test_cb_delete_window(Ewl_Widget *w, void *ev __UNUSED__, 
				void *data __UNUSED__)
{
	ewl_widget_destroy(w);

	if ((--window_count) < 1)
		ewl_main_quit();
}

static void
ewl_test_cb_exit(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
		 void *data __UNUSED__)
{
	ewl_main_quit();
}

static void
ewl_test_print_tests(void)
{
	Ewl_Test *t;

	printf("Ewl_Test Test List:\n");
	ecore_list_goto_first(tests);
	while ((t = ecore_list_next(tests)))
		printf("  %s\n", t->name);
}

static void
run_window_test(Ewl_Test *test, int width, int height)
{
	Ewl_Widget *win, *box;

	/* nothing to do if there is no ui test */
	if (!test->func) return;

	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), test->name);
	ewl_window_name_set(EWL_WINDOW(win), test->name);
	ewl_window_class_set(EWL_WINDOW(win), "Ewl Test Window");
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
					ewl_test_cb_delete_window, NULL);	
	if ((width > 0) && (height > 0))
		ewl_object_size_request(EWL_OBJECT(win), width, height);

	ewl_widget_show(win);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), box);
	ewl_widget_show(box);
	window_count ++;

	test->func(EWL_CONTAINER(box));
}

/* Return 0 if all tests pased, other wise return the number of failures */
static int
run_unit_tests(Ewl_Test *test)
{
	int i, failures = 0;
	char buf[1024];
	buf[0] = '\0';

	/* no unit tests, nothign to do */
	if (!test->unit_tests) return 0;

	for (i = 0; test->unit_tests[i].func; i++)
	{
		int ret;

		printf("Running %s: ", test->unit_tests[i].name);
		ret = test->unit_tests[i].func(buf, sizeof(buf));
		printf("%s %s\n", (ret ? "PASSED" : "FAILED"), 
						(ret ? "" : buf));

		buf[0] = '\0';

		if (!ret) failures++;
	}

	return failures;
}

static void
statusbar_text_set(const char *text)
{
   	Ewl_Widget *stat;
	char info[1024];

	stat = ewl_widget_name_find("statusbar");
	snprintf(info, sizeof(info), "%s Information/Tests\n", text);
	ewl_statusbar_push(EWL_STATUSBAR(stat), info);
}

static void
run_unit_test_boxed(Ewl_Test *t)
{
	Ewl_Widget *c, *n;

	fill_source_text(t);
	setup_unit_tests(t);

	statusbar_text_set(t->name);

	c = ewl_widget_name_find("execute_box");
	ewl_container_reset(EWL_CONTAINER(c));

	c = ewl_widget_name_find("unit_test_box");

	n = ewl_widget_name_find("notebook");
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(n), c);
}

static void
run_test_boxed(Ewl_Test *t)
{
	Ewl_Widget *c, *n;

	/* make sure we have a function if we aren't a straight unit test */
	if ((t->type != EWL_TEST_TYPE_UNIT) && (!t->func))
	{
		printf("WARNING: No UI test function defined for (%s).\n", 
								t->name);
		return;
	}

	statusbar_text_set(t->name);

	fill_source_text(t);
	setup_unit_tests(t);

	c = ewl_widget_name_find("execute_box");
	ewl_container_reset(EWL_CONTAINER(c));

	t->func(EWL_CONTAINER(c));

	n = ewl_widget_name_find("notebook");
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(n), c);

}

static int
ewl_test_setup_tests(void)
{
	char buf[PATH_MAX], buf2[PATH_MAX];
	Ecore_List *list = NULL;
	char *file = NULL;

	tests = ecore_list_new();
	if (!tests) return 0;

	ecore_list_set_free_cb(tests, ECORE_FREE_CB(free));

	snprintf(buf, sizeof(buf), "%s", PACKAGE_LIB_DIR "/ewl/tests");
	list = ecore_file_ls(buf);
	if (list && ecore_list_nodes(list) > 0)
	{
		ecore_list_goto_first(list);
		while ((file = ecore_list_remove_first(list)))
		{
			int len;

			/* see if this is a .so file */
			len = strlen(file);
			if (!strncmp(file + (len - 3), ".so", 3))
			{
				void *handle;

				snprintf(buf2, sizeof(buf2), "%s/%s", buf, file);
				handle = dlopen(buf2, RTLD_LAZY | RTLD_GLOBAL);
				if (handle)
				{
					void (*func_info)(Ewl_Test *test);

					/* the UI test info */
					func_info = dlsym(handle, "test_info");
					if (func_info)
					{
						Ewl_Test *t;
				
						t = calloc(1, sizeof(Ewl_Test));
						func_info(t);
						t->handle = handle;
						ecore_list_append(tests, t);
					}
				}
			}
			free(file);
		}
		ecore_list_destroy(list);
	}

	/* no tests found ... */
	if (ecore_list_nodes(tests) == 0) return 0;

	return 1;
}

static int
create_main_test_window(Ewl_Container *box)
{
	Ewl_Test *t;
	Ewl_Widget *menubar, *note, *tree, *o, *o2;
	Ecore_List *categories;
	Ecore_List *simple, *adv, *container, *misc, *unit;
	char *headers[] = {"Test", "Status", "Failure Reason"};
	Ewl_Model *model;
	Ewl_View *view;

	Ewl_Menu_Info file_menu[] = {
		{"Exit",NULL,  ewl_test_cb_exit},
		{NULL, NULL, NULL}
	};

	Ewl_Menu_Info help_menu[] = {
		{"About Ewl ...", NULL, ewl_test_cb_about},
		{"Ewl Test Help ...", NULL, ewl_test_cb_help},
		{NULL, NULL, NULL}
	};

	Ewl_Menubar_Info menubar_info[] = {
		{"File", file_menu},
		{"Help", help_menu},
		{NULL, NULL}
	};

	menubar = ewl_hmenubar_new();
	ewl_container_child_append(EWL_CONTAINER(box), menubar);
	ewl_menubar_from_info(EWL_MENUBAR(menubar), menubar_info);
	ewl_widget_show(menubar);

	note = ewl_notebook_new();
	ewl_container_child_append(box, note);
	ewl_widget_name_set(note, "notebook");
	ewl_notebook_tabbar_alignment_set(EWL_NOTEBOOK(note),
					EWL_FLAG_ALIGN_CENTER);
	ewl_widget_show(note);

	/* our data will be a list of lists. each of the child lists will
	 * hold a set of tests to be run */
	categories = ecore_list_new();

	simple = ecore_list_new();
	ecore_list_append(categories, simple);

	adv = ecore_list_new();
	ecore_list_append(categories, adv);

	container = ecore_list_new();
	ecore_list_append(categories, container);

	misc = ecore_list_new();
	ecore_list_append(categories, misc);

	unit = ecore_list_new();
	ecore_list_append(categories, unit);

	model = ewl_model_new();
	ewl_model_fetch_set(model, ewl_test_cb_category_fetch);
	ewl_model_count_set(model, ewl_model_cb_ecore_list_count);
	ewl_model_expandable_set(model, ewl_test_cb_category_expandable);
	ewl_model_expansion_data_fetch_set(model, 
				ewl_test_cb_category_expansion_fetch);
	ewl_model_expansion_model_fetch_set(model, 
				ewl_test_cb_category_expansion_model_fetch);

	tree = ewl_tree2_new();
	ewl_container_child_append(EWL_CONTAINER(note), tree);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), tree, "Tests");
	ewl_tree2_headers_visible_set(EWL_TREE2(tree), FALSE);
	ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
	ewl_mvc_model_set(EWL_MVC(tree), model);
	ewl_mvc_data_set(EWL_MVC(tree), categories);
	ewl_callback_append(tree, EWL_CALLBACK_VALUE_CHANGED,
				ewl_test_cb_test_selected, NULL);
	ewl_widget_show(tree);

	view = ewl_label_view_get();
	ewl_tree2_column_append(EWL_TREE2(tree), view, FALSE);

	/* add the tests to the category rows */
	ecore_list_goto_first(tests);
	while ((t = ecore_list_next(tests)))
	{
		Ecore_List *parent = NULL;

		if (t->type == EWL_TEST_TYPE_SIMPLE)
			parent = simple;
		else if (t->type == EWL_TEST_TYPE_ADVANCED)
			parent = adv;
		else if (t->type == EWL_TEST_TYPE_CONTAINER)
			parent = container;
		else if (t->type == EWL_TEST_TYPE_MISC)
			parent = misc;
		else if (t->type == EWL_TEST_TYPE_UNIT)
			parent = unit;

		ecore_list_append(parent, t);

		/* if we've got unit tests add to the unit category */
		if ((parent != unit) &&
				(t->unit_tests && t->unit_tests[0].func))
			ecore_list_append(unit, t);
	}

	o = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(note), o);
	ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_ALL);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), o, "UI Test");
	ewl_widget_name_set(o, "execute_box");
	ewl_widget_show(o);

	o = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(note), o);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), o, "Unit Tests");
	ewl_widget_name_set(o, "unit_test_box");
	ewl_widget_show(o);

	o2 = ewl_tree_new(3);
	ewl_container_child_append(EWL_CONTAINER(o), o2);
	ewl_tree_headers_set(EWL_TREE(o2), headers);
	ewl_tree_mode_set(EWL_TREE(o2), EWL_SELECTION_MODE_SINGLE);
	ewl_widget_name_set(o2, "unit_test_tree");
	ewl_widget_show(o2);

	o2 = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(o), o2);
	ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_HFILL);
	ewl_widget_show(o2);

	o = o2;

	o2 = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(o), o2);
	ewl_button_label_set(EWL_BUTTON(o2), "Run unit tests");
	ewl_callback_append(o2, EWL_CALLBACK_CLICKED, cb_run_unit_tests, NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_SHRINK);
	ewl_widget_name_set(o2, "unit_test_button");
	ewl_widget_show(o2);

	o2 = ewl_progressbar_new();
	ewl_widget_name_set(o2, "unit_test_progress");
	ewl_container_child_append(EWL_CONTAINER(o), o2);
	ewl_widget_show(o2);

	o2 = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(note), o2);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), o2, "Source");
	ewl_widget_name_set(o2, "source_pane");
	ewl_widget_show(o2);

	o2 = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(note), o2);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), o2, "Tutorial");
	ewl_widget_show(o2);

	o = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(o2), o);
	ewl_widget_name_set(o, "tutorial_text");
	ewl_text_selectable_set(EWL_TEXT(o), TRUE);
	ewl_text_wrap_set(EWL_TEXT(o), EWL_TEXT_WRAP_WORD);
	ewl_widget_show(o);

	o = ewl_statusbar_new();
	ewl_container_child_append(EWL_CONTAINER(box), o);
	ewl_statusbar_push(EWL_STATUSBAR(o), "Select Test");
	ewl_widget_name_set(o, "statusbar");
	ewl_widget_show(o);

	return 1;
}

static void
fill_source_text(Ewl_Test *test)
{
	FILE *file;
	struct stat buf;
	char filename[PATH_MAX];

	snprintf(filename, sizeof(filename), PACKAGE_DATA_DIR "/ewl/examples/%s", 
								test->filename);
	file = fopen(filename, "r");
	if (file)
	{
		char *str;

		stat(filename, &buf);

		str = malloc(sizeof(char) * (buf.st_size + 1));
		fread(str, buf.st_size, 1, file);
		str[buf.st_size] = '\0';
		fclose(file);

		text_parse(str);
		free(str);
	}
}

static void
setup_unit_tests(Ewl_Test *test)
{
	Ewl_Widget *button, *tree, *progress;
	char *entries[3];
	int i;

	button = ewl_widget_name_find("unit_test_button");
	tree = ewl_widget_name_find("unit_test_tree");

	ewl_container_reset(EWL_CONTAINER(tree));

	current_test = test;

	/* just clean up if no tests */
	if (!test->unit_tests) return;

	for (i = 0; test->unit_tests[i].func; i++)
	{
		entries[0] = (char *)test->unit_tests[i].name;
		entries[1] = "";
		entries[2] = "";

		ewl_tree_text_row_add(EWL_TREE(tree), NULL, entries);
	}

	progress = ewl_widget_name_find("unit_test_progress");
	ewl_range_maximum_value_set(EWL_RANGE(progress), (double)(i));
	ewl_range_value_set(EWL_RANGE(progress), 0.0);

}

static void
cb_run_unit_tests(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	int i;
	Ewl_Test *test;
	Ewl_Widget *tree;
	Ewl_Widget *progress;

	tree = ewl_widget_name_find("unit_test_tree");
	ewl_container_reset(EWL_CONTAINER(tree));

	test = current_test;
	if ((!test) || (!test->unit_tests)) return;

	for (i = 0; test->unit_tests[i].func; i++)
		;

	progress = ewl_widget_name_find("unit_test_progress");
	ewl_range_maximum_value_set(EWL_RANGE(progress), (double)(i));
	ewl_range_value_set(EWL_RANGE(progress), 0.0);

	if (unit_test_timer) 
	{
		ecore_timer_del(unit_test_timer);
		unit_test_timer = NULL;
		current_unit_test = 0;
	}

	unit_test_timer = ecore_timer_add(0.1, ewl_test_cb_unit_test_timer, 
							test->unit_tests);
}

static void
text_parse(char *str)
{
	Ewl_Widget *txt, *tutorial;
	Ewl_Widget *txtpane;
	char *start, *end, tmp;

	txtpane = ewl_widget_name_find("source_pane");
	tutorial = ewl_widget_name_find("tutorial_text");

	ewl_container_reset(EWL_CONTAINER(txtpane));

	start = strstr(str, "/**");
	if (!start)
	{
		txt = ewl_io_manager_string_read(str, "text/c");
		if (txt) {
			ewl_text_wrap_set(EWL_TEXT(txt), EWL_TEXT_WRAP_WORD);
			ewl_text_selectable_set(EWL_TEXT(txt), TRUE);
			ewl_widget_show(txt);
			ewl_container_child_append(EWL_CONTAINER(txtpane), txt);
		}
		ewl_text_clear(EWL_TEXT(tutorial));
		return;
	}

	end = strstr(start, "*/");
	end++;

	while (*(start - 1) == '\n') start --;

	tmp = *start;
	*start = '\0';

	{
		size_t len1 = strlen(str);
		size_t len2 = strlen(end + 1);
		char *source = malloc(sizeof(char) * (len1 + len2 + 1));

		if (!source)
			return;
		
		strncpy(source, str, len1);
		strncpy(source + len1, end + 1, len2 + 1);

		txt = ewl_io_manager_string_read(source, "text/c");

		if (txt) {
			ewl_text_wrap_set(EWL_TEXT(txt), EWL_TEXT_WRAP_WORD);
			ewl_text_selectable_set(EWL_TEXT(txt), TRUE);
			ewl_container_child_append(EWL_CONTAINER(txtpane), txt);
			ewl_widget_show(txt);
		}

		free(source);
	}

	*start = tmp;
	tmp = *end;
	*end = '\0';

	ewl_text_clear(EWL_TEXT(tutorial)); 
	tutorial_parse(EWL_TEXT(tutorial), start);

	*end = tmp;
}

static void
tutorial_parse(Ewl_Text *tutorial, char *str)
{
	char *start, *end;
	int handled_newline = 0;
	int in_codeblock = 0; 
	int not_double = 0;
	
	start = str;
	
	/* skip the comment block start and any blank space after */
	while ((*start == '/') || (*start == '*') || (isspace(*start))) start++;
	end = start;
	
	while (start && (*start != '\0'))
	{
		int did_newline = 0;
		
		/* we strip out newlines so the text will wrap nicely */
		if (*end == '\n')
		{
			/* we just handled a \n and got a second one so this is a new
			 * paragraph so actually insert the two \n's */
			if (handled_newline)
			{
				if (in_codeblock || not_double)
					ewl_text_text_append(tutorial, "\n");
				else
					ewl_text_text_append(tutorial, "\n\n");
			}

			/* append the text before and skip the newline */
			else
			{
				char tmp;

				tmp = *(end + 1);
				*(end + 1) = '\0';

				if (!in_codeblock)
					*end = ' ';

				ewl_text_text_append(tutorial, start);

				*(end + 1) = tmp;
			}
			start = ++end;
			did_newline = 1;
			not_double = 0;
		}

		/* The * is only special after a newline character */
		else if ((*end == '*' || *end == ' ') && handled_newline)
		{
			if (*end == ' ') end++;
			while (*end == '*') end++;

			/* we only want ot skip "* " if in a code block */
			if (in_codeblock)
			{
				if (*end == ' ') end ++;
			}
			/* otherwise skip the * and all spaces */
			else
			{
				while ((*end == '*') || (*end == ' ') || (*end == '\t'))
					end++;
			}

			if (*end == '\n')
			/* don't let the *'s at the begining of lines effect the \n
			 * handling */
			did_newline = handled_newline;

			start = end;
		}
		else if (*end == '@')
		{
			/* stick on everything before the @ symbol. If we don't have a
			 * keyword here we'll just end up sticking the next chunk onto
			 * this node so shouldn't be a big deal */
			*end = '\0';

			ewl_text_text_append(tutorial, start);
			*end = '@';
			start = end;

			if ((!strncasecmp(end, "@addtogroup ", 12))
					|| (!strncasecmp(end, "@section ", 9)))
			{
				char tmp, key;
				int size = 14;

				key = *(end + 1);
				end += 9;

				/* we increment end so that we can skip over the keyword and
				 * the space after it */
				if (key == 'a')
				{
					size = 22;
					end += 3;
				}

				while (*end == ' ') end ++;
				start = end;

				/* skip to the end of the line */
				end = strchr(end, '\n');

				/* if this is the section header we need to skip 
				 * the section name */
				if (key == 's')
				{
					while (!isspace(*start)) start ++;
					while (*start == ' ') start ++;
				}

				not_double = 1;

				tmp = *(end + 1);
				*(end + 1) = '\0';

				ewl_text_font_size_set(tutorial, size);
				ewl_text_text_append(tutorial, start);
				ewl_text_font_size_set(tutorial, 12);

				*(end + 1) = tmp;
				start = ++end;

				did_newline = 1;
			}
			else if (!strncasecmp(end, "@code", 5))
			{
				in_codeblock = 1;
				end += 5;
				while (*end == ' ') end ++;

				/* if there is nothing after the @code we want to skip the
				 * \n or we'll end up with too much space before the block */
				if (*end == '\n')
				{
					end ++;
					did_newline = 1;
				}

				ewl_text_color_set(tutorial, 32, 71, 109, 255);
				ewl_text_font_set(tutorial, "ewl/monospace");
				start = end;
			}
			else if (!strncasecmp(end, "@endcode", 8))
			{
				*end = '\0';

				ewl_text_text_append(tutorial, start);
				ewl_text_color_set(tutorial, 0, 0, 0, 255);
				in_codeblock = 0;

				end += 8;
				while (*end == ' ') end ++;

				/* if there is nothing after the @code we want to skip the
				 * \n or we'll end up with too much space before the block */
				if (*end == '\n')
				{
					end ++;
					did_newline = 1;
				}
				not_double = 1;
				start = end;
			}
#if 0
			else if (!strncasecmp(end, "@note", 5))
			{

			}
			/* make next word bold */
			else if (!strncasecmp(end, "@b ", 3))
			{
			}
			/* supost to be typewriter font, make next word italic */
			else if (!strncasecmp(end, "@c ", 3))
			{
			}
#endif
			/* if we don't know what it is just write it into the text */
			else
				end ++;
		}
		else if (*end == '<')
		{
			if (0)
			{
			}
			/* skip it if we don't know what it is */
			else
				end ++;
		}
		else
			end ++;

		handled_newline = did_newline;
		if (!end || (*end == '\0'))
		{
			ewl_text_text_append(tutorial, start);
			start = end;
		}
	}
}

static void
ewl_test_cb_help(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	Ewl_Widget *win, *vbox, *o;

	window_count++;

	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "EWL Test Help");
	ewl_window_class_set(EWL_WINDOW(win), "ewl_test");
	ewl_window_name_set(EWL_WINDOW(win), "ewl_test");
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
					ewl_test_cb_delete_window, NULL);
	ewl_widget_show(win);

	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), vbox);
	ewl_widget_show(vbox);

	o = ewl_text_new();
	ewl_text_font_size_set(EWL_TEXT(o), 22);
	ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_CENTER);
	ewl_text_text_set(EWL_TEXT(o), "Ewl_ Test App Help");
	ewl_text_text_append(EWL_TEXT(o), "\n\n");
	ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_LEFT);
	ewl_text_font_size_set(EWL_TEXT(o), 12);
	ewl_text_text_append(EWL_TEXT(o), "Still need to write this, eh.");
	ewl_container_child_append(EWL_CONTAINER(vbox), o);
	ewl_widget_show(o);
}

static void
ewl_test_cb_about(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	Ewl_Widget *win, *vbox, *o;

	window_count++;

	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "EWL Test About");
	ewl_window_class_set(EWL_WINDOW(win), "ewl_test");
	ewl_window_name_set(EWL_WINDOW(win), "ewl_test");
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
					ewl_test_cb_delete_window, NULL);
	ewl_widget_show(win);

	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), vbox);
	ewl_widget_show(vbox);

	o = ewl_text_new();
	ewl_text_font_size_set(EWL_TEXT(o), 22);
	ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_CENTER);
	ewl_text_text_set(EWL_TEXT(o), "Ewl_ Test App");
	ewl_text_text_append(EWL_TEXT(o), "\n\n");
	ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_LEFT);
	ewl_text_font_size_set(EWL_TEXT(o), 12);
	ewl_text_text_append(EWL_TEXT(o), ewl_test_about_body);
	ewl_container_child_append(EWL_CONTAINER(vbox), o);
	ewl_widget_show(o);
}

static void *
ewl_test_cb_category_fetch(void *data __UNUSED__, unsigned int row, 
					unsigned int column __UNUSED__)
{
	switch(row)
	{
		case 0: return "Simple";
		case 1: return "Advanced";
		case 2: return "Container";
		case 3: return "Misc";
		case 4: return "Unit";
	}
	return "ERROR";
}

static int
ewl_test_cb_category_expandable(void *data __UNUSED__, 
				unsigned int row __UNUSED__)
{
	return 1;
}

static void *
ewl_test_cb_category_expansion_fetch(void *data, unsigned int row)
{
	ecore_list_goto_index(data, row);
	return ecore_list_current(data);
}

static Ewl_Model *
ewl_test_cb_category_expansion_model_fetch(void *data __UNUSED__, 
						unsigned int row __UNUSED__)
{
	if (expansion_model) return expansion_model;

	expansion_model = ewl_model_new();
	ewl_model_fetch_set(expansion_model, ewl_test_cb_expansion_fetch);
	ewl_model_count_set(expansion_model, ewl_model_cb_ecore_list_count);

	return expansion_model;
}

static void *
ewl_test_cb_expansion_fetch(void *data, unsigned int row, 
				unsigned int column __UNUSED__)
{
	Ewl_Test *test;

	ecore_list_goto_index(data, row);
	test = ecore_list_current(data);

	return (char *)test->name;
}

static void
ewl_test_cb_test_selected(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ecore_List *tree_data, *unit;
	Ewl_Selection_Idx *sel;
	Ewl_Test *test;

	tree_data = ewl_mvc_data_get(EWL_MVC(w));
	sel = ewl_mvc_selected_get(EWL_MVC(w));

	/* don't care about the top level rows */
	if (tree_data == sel->sel.data) 
	{
		free(sel);
		return;
	}

	/* get the test */
	ecore_list_goto_index(sel->sel.data, sel->row);
	test = ecore_list_current(sel->sel.data);

	/* we need to determine if this is the unit test case. if it is we
	 * need to treat it specially */
	ecore_list_goto_last(tree_data);
	unit = ecore_list_current(tree_data);
	if (unit == sel->sel.data)
		run_unit_test_boxed(test);
	else
		run_test_boxed(test);

	free(sel);
}




