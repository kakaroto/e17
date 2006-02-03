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
			"Border",
		   	__create_border_test_window,
		   	"ewl_border_test.c", 
			"Defines the Ewl_Border class for displaying\n"
				"a container with a label."
		},
		{
			"Box",
			__create_box_test_window,
			"ewl_box_test.c",
			"Defines the Ewl_Box class used for\n"
				"laying out Ewl_Widget's in a horizontal\n"
				"or vertical line."
		},
		{
			"Button",
		   	__create_button_test_window,
		   	"ewl_button_test.c",
			"The button class is a basic button\n"
				"with a label. This class inherits from\n"
				"the Ewl_Box to allow for placing any\n"
				"other widget inside the button."
		},
		{
			"Calendar",
			__create_calendar_test_window,
			"ewl_calendar_test.c",
			"Defines a calendar widget."
		},
		{
			"ColorDialog",
			__create_colordialog_test_window,
			"ewl_colordialog_test.c",
			"Defines a dialog with a color picker."
		},
		{
			"ColorPicker",
			__create_colorpicker_test_window,
			"ewl_colorpicker_test.c",
			"Defines a color picker."
		},
		{
			"Combo",
		  	__create_combo_test_window,
		  	"ewl_combo_test.c",
			"Defines a combo box used internally.\n"
				"The contents on the box are not drawn\n"
				"outside of the Evas."
		},
                {
                        "DatePicker",
                        __create_datepicker_test_window,
                        "ewl_datepicker_test.c",
                        "Defines a datepicker widget."
                },
		{
			"Dialog",
		   	__create_dialog_test_window,
		   	"ewl_dialog_test.c",
			"Defines a dialog window."
		},
		{
			"Entry",
		  	__create_entry_test_window,
		  	"ewl_entry_test.c",
			"Defines the Ewl_Entry class to allow\n"
				"for single line editable text.",
		},
		{
			"Freebox",
			__create_freebox_test_window,
			"ewl_freebox_test.c",
			"The freebox is used for a more free layout system\n"
				"then the other containers",
		},
		{
			"Filedialog",
			__create_filedialog_test_window,
			"ewl_filedialog_test.c",
			"The filedialog is intended to be used\n"
				"for a simple file chooser.",
		},
		{
			"Filedialog MultiSelect",
		       	__create_filedialog_multi_test_window,
		       	"ewl_filedialog_multi_test.c",
			"The filedialog with multiple selections enabled",
		},
		{
			"Floater",
		    	__create_floater_test_window,
		    	"ewl_floater_test.c",
			"Defines a widget for layering above other\n "
				"widgets in EWL's drawing area, with\n"
				"the ability to follow the movement of\n"
				"another widget."
		},
                {
                        "Icon",
                        __create_icon_test_window,
                        "ewl_icon_test.c",
                        "Provides a widget for displaying an icon"
                },
                {
                        "Iconbox",
                        __create_iconbox_test_window,
                        "ewl_iconbox_test.c",
                        "Provides a widget for displaying icons\n"
                                "in an arranged grid, or freeform,\n"
				"with callback and label editing facilities.",
                },
		{
			"Image",
		  	__create_image_test_window,
		  	"ewl_image_test.c",
			"Provides a widget for displaying evas\n"
				"loadable images, and edjes.",
	       	},
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
			"Menubar",
		    	__create_menubar_test_window,
		    	"ewl_menubar_test.c",
			"Defines a menubar that can be used to hold menus."
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
			"Notebook2",
			__create_notebook2_test_window,
			"ewl_notebook2_test.c",
			"Provides a container whose children\n"
			"are pages that can be switched\n"
			"between using tab labels along one\nedge"
		},
		{
			"Paned",
		  	__create_paned_test_window,
		  	"ewl_paned_test.c",
			"Defines the Ewl_Paned to hold two resizable panes."
		},
		{
			"Password",
		     	__create_password_test_window,
		     	"ewl_password_test.c",
			"Defines the Ewl_Password class to allow\n"
				"for single line obscured text."
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
			"Statusbar",
		      	__create_statusbar_test_window,
		      	"ewl_statusbar_test.c",
			"Provides a statusbar for the window."
		},
		{
			"Table",
		  	__create_table_test_window,
		  	"ewl_table_test.c",
			"Defines the Ewl_Table class used for\n"
				"laying out Ewl_Widget's in an array."
		},
		{
			"Text",
		     	__create_text_test_window,
		     	"ewl_text_test.c",
			"Defines a class for multi-line text layout\n"
				"and formatting."
		},
		{
			"Text Editor",
		     	__create_text_editor_test_window,
		     	"ewl_text_editor_test.c",
			"Defines a class for multi-line text layout\n"
				"and formatting."
		},
		{
			"Theme",
		  	__create_theme_test_window,
		  	"ewl_theme_test.c",
			"Shows the utilization of themes\n"
				"inside a EWL application."
	       	},
		{
			"Tooltip",
		    	__create_tooltip_test_window,
		    	"ewl_tooltip_test.c",
			"Defines a widget for displaying short\n"
				"messages after a delay."
		},
		{
			"Tree",
		 	__create_tree_test_window,
		 	"ewl_tree_test.c",
			"Defines a widget for laying out other\n"
				"widgets in a tree or list like manner."
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

