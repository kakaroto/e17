/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_io_manager.h"
#include "ewl_label.h"
#include "ewl_menu.h"
#include "ewl_menubar.h"
#include "ewl_notebook.h"
#include "ewl_progressbar.h"
#include "ewl_scrollpane.h"
#include "ewl_statusbar.h"
#include "ewl_text.h"
#include "ewl_tree.h"
#include "ewl_icon_theme.h"
#include "ewl_dialog.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Ecore_Str.h>
#include <zlib.h>

#define NULL_PATH "/dev/null"

#define MAIN_WIDTH 640
#define MAIN_HEIGHT 320

static Ewl_Model *expansion_model = NULL;

static char *ewl_test_about_body =
                "The EWL Test application serves two purposes. "
                "The first is to allow us to test the different "
                "components of EWL as we develop them.\n\n"
                "The second is to allow developers to view "
                "tutorials and source listings for the different "
                "widgets in the system.\n\n"
                "If you develop your own EWL widget you can add "
                "a test to the test widget without needing to "
                "recompile or edit the test application.";

static char *ewl_test_help_body =
                "Select the test you want from the test tree under the "
                "'Tests' tab. You can then use the 'UI Tests' tab "
                "to view the UI test for that widget. The 'Unit Tests' "
                "tab will let you execute the available unit tests.\n\n"
                "To view the source for the current test click on the "
                "'Source' tab. A short tutorial on the widget will be "
                "available under the 'Tutorial' tab.\n\n"
                "If you encounter any bugs in the test application "
                "please report them to http://bugs.enlightenment.org.";

static int ewl_test_setup_tests(void);
static void ewl_test_free(Ewl_Test *test);
static int ewl_test_compare(Ewl_Test *test1, Ewl_Test *test2);
static void ewl_test_print_tests(void);
static void ewl_test_stderr_enable(void);
static void ewl_test_stderr_disable(void);
static void ewl_test_string_replace(char *string, char replace, char with);

static void run_test_boxed(Ewl_Test *t);
static void run_unit_test_boxed(Ewl_Test *t);
static void run_window_test(Ewl_Test *test, int width, int height);
static int run_unit_tests(Ewl_Test *test);
static int create_main_test_window(Ewl_Container *win);
static void fill_source_text(Ewl_Test *test);
static void fill_tutorial_text(Ewl_Test *test);
static char *read_file(const char *filename);
static void text_parse(char *str);
static void tutorial_parse(char *str);
static void setup_unit_tests(Ewl_Test *test);

static void statusbar_label_update(Ewl_Widget *w, void *ev, void *data);

static int ewl_test_cb_unit_test_timer(void *data);
static void ewl_test_cb_delete_window(Ewl_Widget *w, void *ev, void *data);
static void ewl_test_cb_exit(Ewl_Widget *w, void *ev, void *data);
static void ewl_cb_mvc_free(Ewl_Widget *w, void *ev, void *data);
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

static Ewl_Widget *cb_unit_test_header_fetch(void *data, unsigned int column,
                                                void *pr_data);
static void *cb_unit_test_header_data_fetch(void *data, unsigned int column);
static void *cb_unit_test_fetch(void *data, unsigned int row, unsigned int column);
static unsigned int cb_unit_test_count(void *data);

static void ewl_test_create_info_window(const char *title, const char *text);

static Ecore_List *tests = NULL;
static Ecore_Path_Group *tests_path_group = NULL;
static int window_count = 0;
static int current_unit_test = 0;
static int hide_passed = 0;
static int show_debug = 0;
static int saved_stderr = -1;

static Ecore_Timer *unit_test_timer = NULL;

static Ewl_Test *current_test = NULL;

int
main(int argc, char **argv)
{
        int i, ret = 0;
        int unit_test = 0, ran_test = 0, all_tests = 0, profile_tests = 0;
        double start_time = 0, end_time = 0;

        /* init the randomizer */
        srand(1);

        /* check for any flags before ewl_init to avoid opening the display */
        for (i = 0; i < argc; i++)
        {
                if (!strcmp(argv[i], "-list"))
                {
                        ewl_test_print_tests();
                        if (tests)
                                ecore_list_destroy(tests);
                        if (tests_path_group)
                                ecore_path_group_del(tests_path_group);
                        exit(0);
                }
                else if (!strcmp(argv[i], "-h") ||
                                !strcmp(argv[i], "-help") ||
                                !strcmp(argv[i], "--help"))
                {
                        printf("Usage: %s [options] [test name]\n"
                                        "\t-all\tRun tests for all widgets\n"
                                        "\t-help\tDisplay this help text\n"
                                        "\t-list\tPrint available tests\n"
                                        "\t-profile\tTime test results\n"
                                        "\t-p\tHide passed test information\n"
                                        "\t-d\tEnable all debugging output\n"
                                        "\t-unit\tRun unit tests\n",
                                        argv[0]);
                        exit(0);
                }
                else if (!strcmp(argv[i], "-profile"))
                        profile_tests = 1;
                else if (!strcmp(argv[i], "-p"))
                        hide_passed = 1;
                else if (!strcmp(argv[i], "-d"))
                        show_debug = 1;
                else if (!strcmp(argv[i], "-all"))
                        all_tests = 1;
                else if (!strcmp(argv[i], "-unit"))
                {
                        unit_test = 1;
                        setenv("EWL_ENGINE_NAME", "evas_buffer", 1);
                }
        }

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

        if (profile_tests) start_time = ecore_time_get();

        /* check for any flags */
        for (i = 1; i < argc || all_tests; i++)
        {
                char buffer[128];
                Ewl_Test *t;

                ecore_strlcpy(buffer, argv[i], sizeof(buffer));
                ewl_test_string_replace(buffer, '_', ' ');

                /* see if this thing was a test to run */
                ecore_list_first_goto(tests);
                while ((t = ecore_list_next(tests)))
                {
                        if (all_tests || !strcasecmp(buffer, t->name))
                        {
                                if (unit_test)
                                        ret = run_unit_tests(t);
                                else
                                        run_window_test(t, MAIN_WIDTH, MAIN_HEIGHT);

                                ran_test ++;
                                if (!all_tests) break;
                        }
                }
                if (all_tests) break;
        }

        if (profile_tests)
        {
                end_time = ecore_time_get();
                printf("Tests completed in %.03fs\n", end_time - start_time);
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
        else
                ewl_shutdown();
        
        if (unit_test && !ran_test)
                fprintf(stderr, "Must supply either -all parameter or test "
                                "name when running unit tests.\n");

        if (tests) ecore_list_destroy(tests);
        if (tests_path_group) ecore_path_group_del(tests_path_group);

        return ret;
}

static void
ewl_test_stderr_disable(void)
{
        static int null_fd = -1;

        /*
         * If stderr is already disabled, bail out early.
         */
        if (show_debug || saved_stderr >= 0)
                return;

        /*
         * Open /dev/null the first time it's necessary, and maintain a
         * reference to it for the lifetime of the test app.
         */
        if (null_fd < 0)
                null_fd = open(NULL_PATH, O_RDONLY);

        if (null_fd >= 0)
        {
                saved_stderr = dup(fileno(stderr));

                if (dup2(null_fd, fileno(stderr)) < 0)
                {
                        close(saved_stderr);
                        saved_stderr = -1;
                }
        }
}

static void
ewl_test_stderr_enable(void)
{
        /*
         * Return early if stderr is not disabled.
         */
        if (show_debug || saved_stderr < 0)
                return;

        dup2(saved_stderr, fileno(stderr));
        saved_stderr = -1;
}

static void
ewl_test_string_replace(char *string, char replace, char with)
{
        char *found = string;
        while ((found = strchr(found, replace)))
                *found = with;
}

static int
ewl_test_cb_unit_test_timer(void *data)
{
        char buf[1024];
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
                if (unit_tests[current_unit_test].quiet)
                        ewl_test_stderr_disable();
                val = unit_tests[current_unit_test].func(buf, sizeof(buf));
                if (unit_tests[current_unit_test].quiet)
                        ewl_test_stderr_enable();
                ewl_statusbar_pop(EWL_STATUSBAR(stat));

                tree = ewl_widget_name_find("unit_test_tree");
                progress = ewl_widget_name_find("unit_test_progress");

                unit_tests[current_unit_test].status = val;
                unit_tests[current_unit_test].failure_reason = (val ? NULL : strdup(buf));

                ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

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
ewl_test_cb_delete_window(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                void *data)
{
        Ewl_Widget *win = data;

        ewl_widget_destroy(win);

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
        
        if (!tests)
                ewl_test_setup_tests();
        if (!tests)
                return;

        ecore_list_first_goto(tests);
        while ((t = ecore_list_next(tests)))
                printf("  %s\n", t->name);
        printf("\n");

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
                                        ewl_test_cb_delete_window, win);
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

                if (test->unit_tests[i].quiet)
                        ewl_test_stderr_disable();

                ret = test->unit_tests[i].func(buf, sizeof(buf));
                if (!ret || !hide_passed)
                {
                        printf("Running %s: ", test->unit_tests[i].name);
                        printf("%s %s\n", (ret ? "PASSED" : "FAILED"),
                                                        (ret ? "" : buf));
                }
                buf[0] = '\0';

                if (!ret) failures++;

                if (test->unit_tests[i].quiet)
                        ewl_test_stderr_enable();
        }

        return failures;
}

static void
statusbar_text_set(const char *text)
{
        Ewl_Widget *stat;
        char info[1024];

        stat = ewl_widget_name_find("statusbar");
        snprintf(info, sizeof(info), "%s Information/Tests", text);
        ewl_statusbar_push(EWL_STATUSBAR(stat), info);
}

static void
run_unit_test_boxed(Ewl_Test *t)
{
        Ewl_Widget *c, *n;

        fill_source_text(t);
        fill_tutorial_text(t);
        setup_unit_tests(t);

        statusbar_text_set(t->name);

        c = ewl_widget_name_find("execute_box");
        ewl_container_reset(EWL_CONTAINER(c));

        c = ewl_widget_name_find("unit_test_box");

        n = ewl_widget_name_find("test_notebook");
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(n), c);
}

static void
run_test_boxed(Ewl_Test *t)
{
        Ewl_Widget *c, *n;

        statusbar_text_set(t->name);

        fill_source_text(t);
        fill_tutorial_text(t);
        setup_unit_tests(t);

        /* make sure we have a function if we aren't a straight unit test */
        if ((t->type != EWL_TEST_TYPE_UNIT) && (!t->func))
        {
                printf("WARNING: No UI test function defined for (%s).\n",
                                                                t->name);
                return;
        }

        c = ewl_widget_name_find("execute_box");
        ewl_container_reset(EWL_CONTAINER(c));

        t->func(EWL_CONTAINER(c));

        n = ewl_widget_name_find("test_notebook");
        ewl_notebook_visible_page_set(EWL_NOTEBOOK(n), c);

}

static int
ewl_test_setup_tests(void)
{
        Ecore_List *list = NULL;
        char *name = NULL;

        if (tests) return 1;

        tests = ecore_list_new();
        if (!tests) return 0;

        ecore_list_free_cb_set(tests, ECORE_FREE_CB(ewl_test_free));

        tests_path_group = ecore_path_group_new();
        ecore_path_group_add(tests_path_group, PACKAGE_LIB_DIR "/ewl/tests");
        list = ecore_plugin_available_get(tests_path_group);
        /* no tests found ... */
        if (!list) return 0;

        while ((name = ecore_list_first_goto(list)))
        {
                Ecore_Plugin *plugin;
                void (*func_info)(Ewl_Test *test);

                plugin = ecore_plugin_load(tests_path_group, name, NULL);
                if (!plugin)
                        continue;


                /* the UI test info */
                func_info = ecore_plugin_symbol_get(plugin, "test_info");
                if (func_info)
                {
                        Ewl_Test *t;

                        t = calloc(1, sizeof(Ewl_Test));
                        func_info(t);
                        t->plugin = plugin;
                        ecore_list_append(tests, t);
                }

                ecore_list_remove_destroy(list);
        }
        ecore_list_destroy(list);

        /* no tests found ... */
        if (ecore_list_count(tests) == 0) return 0;

        /* sort the tests in alphabetical order */
        ecore_list_sort(tests, ECORE_COMPARE_CB(ewl_test_compare), 
                        ECORE_SORT_MIN);

        return 1;
}

static void
ewl_test_free(Ewl_Test *test)
{
        if (test->plugin) ecore_plugin_unload(test->plugin);
        free(test);
}

static int
ewl_test_compare(Ewl_Test *test1, Ewl_Test *test2)
{
        return strcasecmp(test1->name, test2->name);
}

static int
create_main_test_window(Ewl_Container *box)
{
        Ewl_Test *t;
        Ewl_Widget *menubar, *note, *tree, *o, *o2;
        Ecore_List *categories;
        Ecore_List *simple, *adv, *container, *misc, *unit;
        Ewl_Model *model;
        Ewl_View *view;

        Ewl_Menu_Info file_menu[] = {
                {"Exit", NULL, ewl_test_cb_exit},
                {NULL, NULL, NULL}
        };

        Ewl_Menu_Info help_menu[] = {
                {"About Ewl_Test ...", NULL, ewl_test_cb_about},
                {"Ewl Test Help ...", NULL, ewl_test_cb_help},
                {NULL, NULL, NULL}
        };

        Ewl_Menubar_Info menubar_info[] = {
                {"File", file_menu},
                {"Help", help_menu},
                {NULL, NULL}
        };

        file_menu[0].img = ewl_icon_theme_icon_path_get(EWL_ICON_APPLICATION_EXIT,
                                                        EWL_ICON_SIZE_SMALL);
        help_menu[0].img = ewl_icon_theme_icon_path_get(EWL_ICON_HELP_ABOUT,
                                                        EWL_ICON_SIZE_SMALL);
        help_menu[1].img = ewl_icon_theme_icon_path_get(EWL_ICON_HELP_FAQ,
                                                        EWL_ICON_SIZE_SMALL);

        menubar = ewl_hmenubar_new();
        ewl_container_child_append(EWL_CONTAINER(box), menubar);
        ewl_menubar_from_info(EWL_MENUBAR(menubar), menubar_info);
        ewl_widget_show(menubar);

        note = ewl_notebook_new();
        ewl_container_child_append(box, note);
        ewl_widget_name_set(note, "test_notebook");
        ewl_notebook_tabbar_alignment_set(EWL_NOTEBOOK(note),
                                        EWL_FLAG_ALIGN_CENTER);
        ewl_widget_show(note);

        /* our data will be a list of lists. each of the child lists will
         * hold a set of tests to be run */
        categories = ecore_list_new();
        ecore_list_free_cb_set(categories, ECORE_FREE_CB(ecore_list_destroy));

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
        ewl_model_data_fetch_set(model, ewl_test_cb_category_fetch);
        ewl_model_data_count_set(model, ewl_model_cb_ecore_list_count);
        ewl_model_data_expandable_set(model, ewl_test_cb_category_expandable);
        ewl_model_data_unref_set(model, 
                                EWL_MODEL_DATA_UNREF(ecore_list_destroy));
        ewl_model_expansion_data_fetch_set(model,
                                ewl_test_cb_category_expansion_fetch);
        ewl_model_expansion_model_fetch_set(model,
                                ewl_test_cb_category_expansion_model_fetch);

        tree = ewl_tree_new();
        ewl_container_child_append(EWL_CONTAINER(note), tree);
        ewl_notebook_page_tab_text_set(EWL_NOTEBOOK(note), tree, "Tests");
        ewl_tree_headers_visible_set(EWL_TREE(tree), FALSE);
        ewl_tree_column_count_set(EWL_TREE(tree), 1);
        ewl_mvc_selection_mode_set(EWL_MVC(tree), EWL_SELECTION_MODE_SINGLE);
        ewl_mvc_model_set(EWL_MVC(tree), model);
        ewl_mvc_view_set(EWL_MVC(tree), ewl_label_view_get());
        ewl_mvc_data_set(EWL_MVC(tree), categories);
        ewl_callback_append(tree, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_test_cb_test_selected, NULL);
        ewl_callback_append(tree, EWL_CALLBACK_DESTROY, ewl_cb_mvc_free,
                                                        NULL);
        ewl_widget_show(tree);


        /* add the tests to the category rows */
        ecore_list_first_goto(tests);
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

        model = ewl_model_new();
        ewl_model_data_fetch_set(model, cb_unit_test_fetch);
        ewl_model_data_header_fetch_set(model, cb_unit_test_header_data_fetch);
        ewl_model_data_count_set(model, cb_unit_test_count);

        view = ewl_label_view_get();
        ewl_view_header_fetch_set(view, cb_unit_test_header_fetch);

        o2 = ewl_tree_new();
        ewl_tree_column_count_set(EWL_TREE(o2), 3);
        ewl_container_child_append(EWL_CONTAINER(o), o2);
        ewl_mvc_model_set(EWL_MVC(o2), model);
        ewl_mvc_view_set(EWL_MVC(o2), view);
        ewl_widget_name_set(o2, "unit_test_tree");
        ewl_callback_append(o2, EWL_CALLBACK_DESTROY, ewl_cb_mvc_free,
                                                        NULL);
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
        ewl_object_fill_policy_set(EWL_OBJECT(o2), EWL_FLAG_FILL_NONE);
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
        ewl_widget_show(o);

        o = ewl_statusbar_new();
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_statusbar_right_show(EWL_STATUSBAR(o));
        ewl_statusbar_push(EWL_STATUSBAR(o), "Select Test");
        ewl_widget_name_set(o, "statusbar");
        ewl_widget_show(o);

        /* create a label for the mouse coordinates */
        o2 = ewl_label_new();
        ewl_container_child_append(EWL_CONTAINER(o), o2);
        ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_MOUSE_MOVE,
                                statusbar_label_update, o2);
        ewl_widget_show(o2);

        return 1;
}

static void
statusbar_label_update(Ewl_Widget *w __UNUSED__, void *ev, void *data)
{
        Ewl_Event_Mouse *event;
        char buf[265];

        event = EWL_EVENT_MOUSE(ev);

        snprintf(buf, sizeof(buf), "x: %d  y: %d", event->x, event->y);
        ewl_label_text_set(EWL_LABEL(data), buf);
}

static void
fill_source_text(Ewl_Test *test)
{
        char *txt;
        char filename[PATH_MAX];

        snprintf(filename, sizeof(filename), 
                        PACKAGE_DATA_DIR "/ewl/examples/%s", test->filename);

        txt = read_file(filename);
        text_parse(txt);
        if (txt) free(txt);
}

static void
fill_tutorial_text(Ewl_Test *test)
{
        char *txt, *file, *p;
        char filename[PATH_MAX];

        file = strdup(test->filename);
        p = strrchr(file, '_');
        if ((p != NULL) && (*p != '\0')) *p = '\0';
        
        snprintf(filename, sizeof(filename),
                        PACKAGE_DATA_DIR "/ewl/tutorials/%s.dox", file);
        free(file);

        txt = read_file(filename);
        tutorial_parse(txt);
        if (txt) free(txt);
}

static char *
read_file(const char *filename)
{
        char *str = NULL;
        struct stat buf;

        if (ecore_file_exists(filename))
        {
                FILE *file;

                file = fopen(filename, "r");
                if (!file) return NULL;

                stat(filename, &buf);

                str = malloc(sizeof(char) * (buf.st_size + 1));
                fread(str, buf.st_size, 1, file);
                str[buf.st_size] = '\0';
                fclose(file);
        }
        else
        {
                gzFile file;
                unsigned int size, step, len = 0;
                char path[PATH_MAX];
                int ret;

                /* let see if a compressed version exists */
                snprintf(path, sizeof(path), "%s.gz", filename);
                if (!ecore_file_exists(path)) return NULL;

                file = gzopen(path, "rb");
                if (!file) return NULL;

                stat(filename, &buf);

                step = buf.st_size;
                size = step * 4;
                str = malloc(sizeof(char) * (size + 1));
                while ((ret = gzread(file, str + (size - 4 * step), step)))
                {
                        if (ret < 0)
                        {
                                fprintf(stderr, "Could not open gzipped file\n");
                                gzclose(file);
                                free(str);
                                return NULL;
                        }
                        size += step;
                        str = realloc(str, sizeof(char) * (size + 1));
                        len += ret;
                }
                str[len] = '\0';
                gzclose(file);
        }
        return str;
}

static void
setup_unit_tests(Ewl_Test *test)
{
        Ewl_Widget *button, *tree, *progress;
        int i;

        button = ewl_widget_name_find("unit_test_button");
        tree = ewl_widget_name_find("unit_test_tree");

        current_test = test;

        /* just clean up if no tests */
        if (!test->unit_tests) return;

        /* set the mvc widget and reset the test information */
        ewl_mvc_data_set(EWL_MVC(tree), current_test);
        for (i = 0; test->unit_tests[i].func; i++)
        {
                if (test->unit_tests[i].failure_reason)
                        free(test->unit_tests[i].failure_reason);
                test->unit_tests[i].failure_reason = NULL;

                test->unit_tests[i].status = -1;
        }
        ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

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
        Ewl_Widget *txt;
        Ewl_Widget *txtpane;

        txtpane = ewl_widget_name_find("source_pane");

        ewl_container_reset(EWL_CONTAINER(txtpane));
        if (!str) return;

        txt = ewl_io_manager_string_read(str, "text/c");
        if (txt)
        {
                ewl_text_selectable_set(EWL_TEXT(txt), TRUE);
                ewl_widget_show(txt);
                ewl_container_child_append(EWL_CONTAINER(txtpane), txt);
        }
}

static void
tutorial_parse(char *str)
{
        char *start, *end;
        int handled_newline = 0;
        int in_codeblock = 0;
        int not_double = 0;
        Ewl_Text *tutorial;

        tutorial = EWL_TEXT(ewl_widget_name_find("tutorial_text"));
        ewl_text_text_set(tutorial, NULL);
        ewl_text_wrap_set(tutorial, EWL_TEXT_WRAP_WORD);
        if (!str) return;

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
                else if ((*end == '*' || *end == ' ' || *end == '/') && handled_newline)
                {
                        if (*end == ' ') end++;
                        while (*end == '*') end++;

                        /* we only want to skip "* " if in a code block */
                        if (in_codeblock)
                        {
                                if (*end == ' ') end ++;
                        }
                        /* otherwise skip the * and all spaces */
                        else
                        {
                                while ((*end == '*') || (*end == ' ') || 
                                                (*end == '/') || (*end == '\t'))
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
                                ewl_text_font_set(tutorial, NULL);
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
ewl_test_create_info_window(const char *title, const char *text)
{
        Ewl_Widget *win, *o;

        window_count++;

        win = ewl_dialog_new();
        ewl_window_title_set(EWL_WINDOW(win), title);
        ewl_window_class_set(EWL_WINDOW(win), "ewl_test");
        ewl_window_name_set(EWL_WINDOW(win), "ewl_test");
        ewl_window_dialog_set(EWL_WINDOW(win), TRUE);
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
                                        ewl_test_cb_delete_window, win);
        ewl_widget_show(win);
        
        /* add the dialog content */
        ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_TOP);

        o = ewl_text_new();
        ewl_text_font_set(EWL_TEXT(o), "ewl/default/bold");
        /* give it a size hint to calculate a good preferred size in the first 
         * place */
        ewl_object_w_request(EWL_OBJECT(o), 300);
        ewl_object_padding_set(EWL_OBJECT(o), 10, 10, 20, 10);
        ewl_text_font_size_set(EWL_TEXT(o), 22);
        ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_CENTER);
        ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_FAR_SHADOW);
        ewl_text_shadow_color_set(EWL_TEXT(o), 0, 0, 0, 30);
        ewl_text_text_append(EWL_TEXT(o), title);

        /* enter the new line before change the alignment */
        ewl_text_text_append(EWL_TEXT(o), "\n\n");

        ewl_text_align_set(EWL_TEXT(o), EWL_FLAG_ALIGN_LEFT);
        ewl_text_styles_set(EWL_TEXT(o), EWL_TEXT_STYLE_NONE);
        ewl_text_wrap_set(EWL_TEXT(o), EWL_TEXT_WRAP_WORD);
        ewl_text_font_size_set(EWL_TEXT(o), 12);
        ewl_text_font_set(EWL_TEXT(o), NULL);
        ewl_text_text_append(EWL_TEXT(o), text);
        ewl_container_child_append(EWL_CONTAINER(win), o);
        ewl_widget_show(o);

        /* add the ok button now */
        ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_BOTTOM);

        o = ewl_button_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
        ewl_container_child_append(EWL_CONTAINER(win), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_test_cb_delete_window, win);
        ewl_widget_show(o);
}

static void
ewl_test_cb_help(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        ewl_test_create_info_window("Ewl Test Help", ewl_test_help_body);
}

static void
ewl_test_cb_about(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        ewl_test_create_info_window("Ewl Test About", ewl_test_about_body);
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
        ecore_list_index_goto(data, row);
        return ecore_list_current(data);
}

static Ewl_Model *
ewl_test_cb_category_expansion_model_fetch(void *data __UNUSED__,
                                                unsigned int row __UNUSED__)
{
        if (expansion_model) return expansion_model;

        expansion_model = ewl_model_new();
        ewl_model_data_fetch_set(expansion_model, ewl_test_cb_expansion_fetch);
        ewl_model_data_count_set(expansion_model, ewl_model_cb_ecore_list_count);

        return expansion_model;
}

static void *
ewl_test_cb_expansion_fetch(void *data, unsigned int row,
                                unsigned int column __UNUSED__)
{
        Ewl_Test *test;

        ecore_list_index_goto(data, row);
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
        if (!sel)
                goto EXIT_CALLBACK;

        /* don't care about the top level rows */
        if (tree_data == sel->sel.data)
                goto CLEAR_SELECTION;

        /* get the test */
        ecore_list_index_goto(sel->sel.data, sel->row);
        test = ecore_list_current(sel->sel.data);
        if (!test)
                goto FREE_SELECTION;

        /* we need to determine if this is the unit test case. if it is we
         * need to treat it specially */
        ecore_list_last_goto(tree_data);
        unit = ecore_list_current(tree_data);
        if (unit == sel->sel.data)
                run_unit_test_boxed(test);
        else
                run_test_boxed(test);

CLEAR_SELECTION:
        ewl_mvc_selected_clear(EWL_MVC(w));
FREE_SELECTION:
        free(sel);
EXIT_CALLBACK:
        return;
}

static void *
cb_unit_test_header_data_fetch(void *data __UNUSED__, unsigned int column)
{
        if (column == 0)
                return "Test";

        if (column == 1)
                return "Status";

        return "Failure Reason";
}

static Ewl_Widget *
cb_unit_test_header_fetch(void *data, unsigned int column __UNUSED__,
                                void *pr_data __UNUSED__)
{
        Ewl_Widget *label;

        label = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(label), data);
        ewl_widget_show(label);

        return label;
}

static void *
cb_unit_test_fetch(void *data, unsigned int row, unsigned int column)
{
        Ewl_Test *t;

        t = data;
        if (column == 0)
                return (void *)t->unit_tests[row].name;

        else if (column == 1)
                return ((t->unit_tests[row].status < 0) ? "" :
                        (t->unit_tests[row].status == 0) ? "FAIL" : "PASS");

        else
                return t->unit_tests[row].failure_reason;
}

static unsigned int
cb_unit_test_count(void *data)
{
        Ewl_Test *t;
        unsigned int i;

        t = data;
        if (!data) return 0;

        for (i = 0; t->unit_tests[i].func; i++)
                ;

        return i;
}

static void
ewl_cb_mvc_free(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Model *model;
        Ewl_View *view;

        model = (Ewl_Model *)ewl_mvc_model_get(EWL_MVC(w));
        free(model);

        view = (Ewl_View *)ewl_mvc_view_get(EWL_MVC(w));
        free(view);
}
