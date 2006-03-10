#include "Ewl_Test.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>

#define MAIN_WIDTH 640
#define MAIN_HEIGHT 320

static int ewl_test_setup_tests(void);
static void ewl_test_print_tests(void);

static void run_test_boxed(Ewl_Widget *w, void *ev, void *data);
static void run_window_test(Ewl_Test *test, int width, int height);
static void run_unit_tests(Ewl_Test *test);
static int create_main_test_window(Ewl_Container *win);
static void fill_source_text(Ewl_Test *test);
static void setup_unit_tests(Ewl_Test *test);

static void ewl_test_cb_delete_window(Ewl_Widget *w, void *ev, void *data);
static void cb_run_unit_tests(Ewl_Widget *w, void *ev, void *data);

static Ecore_List *tests = NULL;
static int window_count = 0;

int
main(int argc, char **argv)
{
	int i, unit_test = 0, ran_test = 0;

	/* initialize the ewl library */
	if (!ewl_init(&argc, argv))
	{
		printf("Unable to init Ewl.\n");
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
		else if (!strncmp(argv[i], "-unit", 5))
			unit_test = 1;

		/* see if this thing was a test to run */
		ecore_list_goto_first(tests);
		while ((t = ecore_list_next(tests)))
		{
			if (!strcasecmp(argv[i], t->name))
			{
				if (unit_test)
					run_unit_tests(t);
				else
					run_window_test(t, 0, 0);

				ran_test ++;
				break;
			}
		}
	}

	/* no passed in tests, run the main test app */
	if (!ran_test)
	{
		Ewl_Test test;

		test.name = "The Enlightend Widget Library Test App";
		test.func = create_main_test_window;
		test.filename = NULL;
		test.tip = NULL;

		run_window_test(&test, MAIN_WIDTH, MAIN_HEIGHT);
	}

	if (!unit_test)
		ewl_main();

	return 0;
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
ewl_test_print_tests(void)
{
	Ewl_Test *t;

	printf("Ewl_Test test list:\n");
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

static void
run_unit_tests(Ewl_Test *test)
{
	int i;
	char buf[1024];

	/* no unit tests, nothign to do */
	if (!test->unit_tests) return;

	for (i = 0; test->unit_tests[i].func; i++)
	{
		int ret;

		printf("Running %s: ", test->unit_tests[i].name);
		ret = test->unit_tests[i].func(buf, sizeof(buf));
		printf("%s %s\n", (ret ? "passed" : "failed"), 
						(ret ? "" : buf));
		buf[0] = '\0';
	}
}

static void
run_test_boxed(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
							void *data)
{
	Ewl_Test *t;
	Ewl_Widget *c, *n;

	t = data;

	/* nothing to do if no ui test */
	if (!t->func) return;

	c = ewl_widget_name_find("execute_box");
	ewl_container_reset(EWL_CONTAINER(c));
	t->func(EWL_CONTAINER(c));

	n = ewl_widget_name_find("notebook");
	ewl_notebook_visible_page_set(EWL_NOTEBOOK(n), c);

	fill_source_text(t);
	setup_unit_tests(t);
}

static int
ewl_test_setup_tests(void)
{
	char buf[PATH_MAX], buf2[PATH_MAX];
	Ecore_List *list = NULL, *list2;
	char *file = NULL;

	tests = ecore_list_new();
	if (!tests) return 0;

	list2 = ecore_list_new();
	if (!list2) return 0;

	/* XXX change this when we have real auto* */
	snprintf(buf, sizeof(buf), "%s", PACKAGE_LIB_DIR "/tests");
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
				snprintf(buf2, sizeof(buf2), "%s/%s", 
								buf, file);
				ecore_list_append(list2, strdup(buf2));
			}
			free(file);
		}
		ecore_list_destroy(list);
	}

	/* no tests found ... */
	if (ecore_list_nodes(list2) == 0) return 0;

	/* open each test file and get the Test struct from it */
	ecore_list_goto_first(list2);
	while ((file = ecore_list_remove_first(list2)))
	{
		void *handle;
		
		handle = dlopen(file, RTLD_LAZY | RTLD_GLOBAL);
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
		free(file);
	}
	ecore_list_destroy(list2);

	return 1;
}

static int
create_main_test_window(Ewl_Container *box)
{
	Ewl_Test *t;
	Ewl_Widget *note, *tree, *o, *o2;
	Ewl_Widget *sim, *adv, *misc, *container;
	char *entries[1];
	char *headers[] = {"Test", "Pass/Fail", "Reason"};

	note = ewl_notebook_new();
	ewl_container_child_append(box, note);
	ewl_widget_name_set(note, "notebook");
	ewl_notebook_tabbar_alignment_set(EWL_NOTEBOOK(note),
					EWL_FLAG_ALIGN_CENTER);
	ewl_widget_show(note);

	tree = ewl_tree_new(1);
	ewl_container_child_append(EWL_CONTAINER(note), tree);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), tree, "Tests");
	ewl_tree_headers_visible_set(EWL_TREE(tree), FALSE);
	ewl_widget_show(tree);

	/* create the test category rows */
	entries[0] = "Simple";
	sim = ewl_tree_text_row_add(EWL_TREE(tree), NULL, entries);

	entries[0] = "Advanced";
	adv = ewl_tree_text_row_add(EWL_TREE(tree), NULL, entries);

	entries[0] = "Container";
	container = ewl_tree_text_row_add(EWL_TREE(tree), NULL, entries);

	entries[0] = "Misc";
	misc = ewl_tree_text_row_add(EWL_TREE(tree), NULL, entries);

	/* add the tests to the category rows */
	ecore_list_goto_first(tests);
	while ((t = ecore_list_next(tests)))
	{
		Ewl_Widget *parent = NULL, *w;
		entries[0] = (char *)t->name;

		if (t->type == EWL_TEST_TYPE_SIMPLE)
			parent = sim;
		else if (t->type == EWL_TEST_TYPE_ADVANCED)
			parent = adv;
		else if (t->type == EWL_TEST_TYPE_CONTAINER)
			parent = container;
		else if (t->type == EWL_TEST_TYPE_MISC)
			parent = misc;

		w = ewl_tree_text_row_add(EWL_TREE(tree), 
					EWL_ROW(parent), entries);
		ewl_callback_append(w, EWL_CALLBACK_CLICKED,
						run_test_boxed, t);
	}

	o2 = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(note), o2);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), o2, "Source");
	ewl_widget_show(o2);

	o = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(o2), o);
	ewl_widget_name_set(o, "source_text");
	ewl_widget_show(o);

	o = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(note), o);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), o, "Execute");
	ewl_widget_name_set(o, "execute_box");
	ewl_widget_show(o);

	o = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(note), o);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), o, "Unit Tests");
	ewl_widget_show(o);

	o2 = ewl_tree_new(3);
	ewl_container_child_append(EWL_CONTAINER(o), o2);
	ewl_tree_headers_set(EWL_TREE(o2), headers);
	ewl_widget_name_set(o2, "unit_test_tree");
	ewl_widget_show(o2);

	o2 = ewl_button_new();
	ewl_container_child_append(EWL_CONTAINER(o), o2);
	ewl_button_label_set(EWL_BUTTON(o2), "Run unit tests");
	ewl_callback_append(o2, EWL_CALLBACK_CLICKED, cb_run_unit_tests, NULL);
	ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_SHRINK);
	ewl_widget_name_set(o2, "unit_test_button");
	ewl_widget_show(o2);

	o = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(note), o);
	ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), o, "Tutorial");
	ewl_widget_name_set(o, "tutorial_text");
	ewl_widget_show(o);

	return 1;
}

static void
fill_source_text(Ewl_Test *test)
{
	Ewl_Widget *txt;
	FILE *file;
	struct stat buf;
	char filename[PATH_MAX];

	snprintf(filename, sizeof(filename), PACKAGE_DATA_DIR "/examples/%s", 
								test->filename);
	file = fopen(filename, "r");
	if (file)
	{
		char *str;

		txt = ewl_widget_name_find("source_text");
		stat(filename, &buf);

		str = malloc(sizeof(char) * (buf.st_size + 1));
	    
		fread(str, buf.st_size, 1, file);
		str[buf.st_size] = '\0';

		ewl_text_text_set(EWL_TEXT(txt), str);
		free(str);
		fclose(file);
	}
}

static void
setup_unit_tests(Ewl_Test *test)
{
	Ewl_Widget *button, *tree;
	char *entries[3];
	int i;

	button = ewl_widget_name_find("unit_test_button");
	tree = ewl_widget_name_find("unit_test_tree");

	ewl_container_reset(EWL_CONTAINER(tree));

	/* attach the test data to the button */
	ewl_widget_data_set(button, "test", test);

	/* just clean up if no tests */
	if (!test->unit_tests) return;

	for (i = 0; test->unit_tests[i].func; i++)
	{
		entries[0] = (char *)test->unit_tests[i].name;
		entries[1] = "";
		entries[2] = "";

		ewl_tree_text_row_add(EWL_TREE(tree), NULL, entries);
	}
}

static void
cb_run_unit_tests(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
	Ewl_Test *test;
	Ewl_Widget *tree;
	char *entries[3];
	char buf[1024];
	int i;

	test = ewl_widget_data_get(w, "test");
	if ((!test) || (!test->unit_tests)) return;

	tree = ewl_widget_name_find("unit_test_tree");
	ewl_container_reset(EWL_CONTAINER(tree));

	for (i = 0; test->unit_tests[i].func; i++)
	{
		int ret;
		ret = test->unit_tests[i].func(buf, sizeof(buf));

		entries[0] = (char *)test->unit_tests[i].name;
		entries[1] = (ret ? "PASS" : "FAIL");
		entries[2] = (ret ? "" : buf);
		ewl_tree_text_row_add(EWL_TREE(tree), NULL, entries);
	}
}


