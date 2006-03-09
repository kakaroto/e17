/* PACKAGE_DATA_DIR "/examples" to get path of the files */
#include "ewl_test.h"

static Ewl_Widget     *main_text;

void
__close_main_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_main_quit();
}

static void
__realize_main_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	printf("Main window realized with theme_object %p\n", w->theme_object);
}

static void
__fill_main_text(Ewl_Widget * w __UNUSED__, void *ev_data __UNUSED__,
					void *user_data)
{
        FILE *file;
	struct stat buf;
	char *str;
	char *filename;
	
	filename = (char *)malloc (sizeof (char)*(strlen (PACKAGE_DATA_DIR) +
						  strlen ("/examples/") + 
						  strlen ((char *)user_data) + 1));
	sprintf (filename, "%s/examples/%s", PACKAGE_DATA_DIR, (char *)user_data);

	file = fopen (filename, "r");
	if (file)
	  {
	    stat (filename, &buf);
	    str = (char*)malloc (sizeof (char)*buf.st_size + 1);
	    
	    fread(str, buf.st_size, 1, file);
	    str[buf.st_size] = '\0';
	    ewl_text_text_set (EWL_TEXT (main_text), str);
            free(str);
	  }
	free (filename);
}

int
main(int argc, char **argv)
{
	int             i;
	Ewl_Widget     *main_win;
	Ewl_Widget     *main_box;
	Ewl_Widget     *main_tree;
	Ewl_Widget     *main_area;
        /* Ewl_Widget     *separator; */
	Ewl_Widget     *prow;
	static test_set       tests[] = {
		{
			"IMenu",
		  	__create_imenu_test_window,
		  	"ewl_imenu_test.c",
			"Defines a menu used internally. The\n"
				"contents on the menu are not drawn\n"
				"outside of the Evas."
		},
		{
			"Menu",
		 	__create_menu_test_window,
		 	"ewl_menu_test.c",
			"Defines the basic menu classes that\n"
				"are extended to an actual menu\n"
				"implementation by inheriting classes\n"
				"such as Ewl_Menu and Ewl_IMenu."
		},
		{
			"Notebook",
		     	__create_notebook_test_window,
		     	"ewl_notebook_test.c",
			"Provides a container whose children\n"
				"are pages that can be switched\n"
				"between using tab labels along one\nedge"
		},
		{
			"Progressbar",
			__create_progressbar_test_window,
			"ewl_progressbar_test.c",
			"Provides a progress bar from a given value."
		},
		{
			"Spinner",
		    	__create_spinner_test_window,
		    	"ewl_spinner_test.c",
			"Provides a field for entering numerical\n"
				"values, along with buttons to increment\n"
				"and decrement the value."
		},
		{
			"Table",
		  	__create_table_test_window,
		  	"ewl_table_test.c",
			"Defines the Ewl_Table class used for\n"
				"laying out Ewl_Widget's in an array."
		},
		{
			"Theme",
		  	__create_theme_test_window,
		  	"ewl_theme_test.c",
			"Shows the utilization of themes\n"
				"inside a EWL application."
	       	},
		{ 0, NULL, 0, NULL }
	};

	void *heap_start, *heap_end;
	char *header[1] = {
		"Widgets"
	};

	heap_start = sbrk(0);

	if (!ewl_init(&argc, argv)) {
		fprintf(stderr, "Could not init ewl. Exiting...\n");
		return 1;
	}

	if (argc > 1) {
		int j, found = 0;

		for (j = 1; j < argc; j++) {
			for (i = 0; tests[i].func; i++) {
				if (!strcasecmp(argv[j], tests[i].name)) {
					tests[i].func(NULL, NULL, NULL);
					found++;
				}
			}
		}

		if (found) {
			ewl_main();
			exit(0);
		}
	}

	heap_end = sbrk(0);
	printf("HEAP SIZE:\t%u bytes\n", heap_end - heap_start);

	main_win = ewl_window_new();
	ewl_object_size_request(EWL_OBJECT(main_win), 600, 400);
	ewl_window_title_set(EWL_WINDOW(main_win),
			     "The Enlightenment Widget Library");
	ewl_window_name_set(EWL_WINDOW(main_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(main_win), "EFL Test Application");
	ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
			    __close_main_window, NULL);
	ewl_callback_append(main_win, EWL_CALLBACK_REALIZE,
			    __realize_main_window, NULL);
	ewl_widget_show(main_win);

	/*
	 * Create the main box for holding the test scrollpane areas
	 */
	main_box = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(main_win), main_box);
	ewl_box_spacing_set(EWL_BOX(main_box), 6);
	ewl_box_homogeneous_set (EWL_BOX (main_box), FALSE);
	ewl_widget_show(main_box);

	main_tree = ewl_tree_new(1);
	ewl_tree_headers_set (EWL_TREE (main_tree), header);
	ewl_tree_expandable_rows_set(EWL_TREE (main_tree), FALSE);
	ewl_container_child_append(EWL_CONTAINER(main_box), main_tree);
	ewl_object_fill_policy_set (EWL_OBJECT (main_tree),
				    EWL_FLAG_FILL_HSHRINK |
				    EWL_FLAG_FILL_VSHRINK |
				    EWL_FLAG_FILL_VFILL);
	ewl_widget_show(main_tree);

	/*
	separator = ewl_vseparator_new();
	ewl_container_child_append(EWL_CONTAINER(main_box), separator);
	ewl_widget_show(separator);
	*/

	main_area = ewl_scrollpane_new();
	ewl_widget_show(main_area);
	ewl_container_child_append(EWL_CONTAINER(main_box), main_area);

	main_text = ewl_text_new ();
	ewl_text_text_set(EWL_TEXT(main_text),
				"  Click on the objects on the left to\n"
				"show a working example and the source\n"
				"of the corresponding widget.\n"
				"Keep the mouse on the button to see\n"
				"a brief description of the widget.");
	ewl_container_child_append(EWL_CONTAINER(main_area),
				   main_text);
	ewl_widget_show(main_text);

	i = 0;
	while (tests[i].func) {
		/*
		 * Add the row to the tree, and setup it's alignment and
		 * fill.
		 */
		prow = ewl_tree_text_row_add( EWL_TREE (main_tree), NULL,
						 &(tests[i].name));
		ewl_callback_append (EWL_WIDGET (prow), 
				     EWL_CALLBACK_CLICKED,
				     EWL_CALLBACK_FUNCTION (tests[i].func), 
				     NULL);
		ewl_callback_append (EWL_WIDGET (prow), 
				     EWL_CALLBACK_CLICKED,
				     EWL_CALLBACK_FUNCTION (__fill_main_text),
				     tests[i].filename);
		
		/* Add the tooltips */
		if (tests[i].tip) 
			ewl_attach_tooltip_text_set(prow, tests[i].tip);

		i++;
	}

	heap_end = sbrk(0);
	printf("HEAP SIZE:\t%u bytes\n", heap_end - heap_start);

	ewl_main();

	heap_end = sbrk(0);
	printf("HEAP SIZE:\t%u bytes\n", heap_end - heap_start);

	return 0;
}

