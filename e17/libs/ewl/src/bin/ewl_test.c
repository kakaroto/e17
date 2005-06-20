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
        Ewl_Widget     *tooltip;
	Ewl_Widget     *prow[BUTTONS];
	static test_set       tests[] = {
		{ "Border",      __create_border_test_window,      "ewl_border_test.c" },
		{ "Box",         __create_box_test_window,         "ewl_box_test.c" },
		{ "Button",      __create_button_test_window,      "ewl_button_test.c" },
		{ "ColorDialog", __create_colordialog_test_window, "ewl_colordialog_test.c" },
		{ "ColorPicker", __create_colorpicker_test_window, "ewl_colorpicker_test.c" },
		{ "Combo",       __create_combo_test_window,       "ewl_combo_test.c" },
		{ "Dialog",      __create_dialog_test_window,      "ewl_dialog_test.c" },
		{ "Entry",       __create_entry_test_window,       "ewl_entry_test.c" },
		{ "Filedialog",  __create_filedialog_test_window,  "ewl_filedialog_test.c" },
		{ "Filedialog MultiSelect",  __create_filedialog_multi_test_window,  "ewl_filedialog_multi_test.c" },
		{ "Floater",     __create_floater_test_window,     "ewl_floater_test.c" },
		{ "Image",       __create_image_test_window,       "ewl_image_test.c" },
		{ "IMenu",       __create_imenu_test_window,       "ewl_imenu_test.c" },
		{ "Menu",        __create_menu_test_window,        "ewl_menu_test.c" },
		{ "Menubar",     __create_menubar_test_window,     "ewl_menubar_test.c" },
		{ "Notebook",    __create_notebook_test_window,    "ewl_notebook_test.c" },
		{ "Paned",       __create_paned_test_window,       "ewl_paned_test.c" },
		{ "Password",    __create_password_test_window,    "ewl_password_test.c" },
		{ "Progressbar", __create_progressbar_test_window, "ewl_progressbar_test.c" },
		{ "Spinner",     __create_spinner_test_window,     "ewl_spinner_test.c" },
		{ "Statusbar",   __create_statusbar_test_window,   "ewl_statusbar_test.c" },
		{ "Table",       __create_table_test_window,       "ewl_table_test.c" },
		{ "Textarea",    __create_textarea_test_window,    "ewl_textarea_test.c" },
		{ "Theme",       __create_theme_test_window,       "ewl_theme_test.c" },
		{ "Tooltip",     __create_tooltip_test_window,     "ewl_tooltip_test.c" },
		{ "Tree",        __create_tree_test_window,        "ewl_tree_test.c" },
		{ 0, 0, 0 }
	};
	static char* tooltips[] = {
		"Defines the Ewl_Border class for displaing\n"
			"a container with a label.",
	   	"Defines the Ewl_Box class used for\n"
			"laying out Ewl_Widget's in a horizontal\n"
			"or vertical line.",
	   	"The button class is a basic button\n"
			"with a label. This class inherits from\n"
			"the Ewl_Box to allow for placing any\n"
			"other widget inside the button.",
	   	"Defines a dialog with a color picker.",
	   	"Defines a color picker.",
		"Defines a combo box used internally.\n"
			"The contents on the box are not drawn\n"
			"outside of the Evas.",
		"Defines a dialog window.",
		"Defines the Ewl_Entry class to allow\n"
			"for single line editable text.",
		"The filedialog is intended to be used\n"
			"for a simple file chooser.",
		"The filedialog with multiple selections enabled",
		"Defines a widget for layering above other\n "
			"widgets in EWL's drawing area, with\n"
			"the ability to follow the movement of\n"
			"another widget.",
		"Provides a widget for displaying evas\n"
			"loadable images, and edjes.",
		"Defines a menu used internally. The\n"
			"contents on the menu are not drawn\n"
			"outside of the Evas.",
		"Defines the basic menu classes that\n"
			"are extended to an actual menu\n"
			"implementation by inheriting classes\n"
			"such as Ewl_Menu and Ewl_IMenu.",
		"Defines a menubar that can be used to hold menus.",
		"Provides a container whose children\n"
			"are pages that can be switched\n"
			"between using tab labels along one\nedge",
		"Defines the Ewl_Paned to hold two resizable panes of content.",
		"Defines the Ewl_Password class to allow\n"
			"for single line obscured text.",
		"Provides a progress bar from a given value.",
		"Provides a field for entering numerical\n"
			"values, along with buttons to increment\n"
			"and decrement the value.",
		"Provides a statusbar for the window.",
	   	"Defines the Ewl_Table class used for\n"
		        "laying out Ewl_Widget's in an array.",
		"Defines a class for multi-line text layout\n"
			"and formatting.",
		"Shows the utilization of themes\n"
			"inside a EWL application.",
		"Defines a widget for displaying short\n"
			"messages after a delay.",
		"Defines a widget for laying out other\n"
			"widgets in a tree or list like manner.",
		NULL
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
	ewl_object_size_request(EWL_OBJECT(main_win), 400, 400);
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

	main_text = ewl_text_new ("  Click on the objects on the left to\n"
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
		prow[i] = ewl_tree_text_row_add( EWL_TREE (main_tree), NULL,
						 &(tests[i].name));
		ewl_callback_append (EWL_WIDGET (prow[i]), 
				     EWL_CALLBACK_CLICKED,
				     EWL_CALLBACK_FUNCTION (tests[i].func), 
				     NULL);
		ewl_callback_append (EWL_WIDGET (prow[i]), 
				     EWL_CALLBACK_CLICKED,
				     EWL_CALLBACK_FUNCTION (__fill_main_text),
				     tests[i].filename);
		
		/* Add the tooltips */
		tooltip = ewl_tooltip_new (prow[i]);
		ewl_tooltip_delay_set (EWL_TOOLTIP (tooltip), 1.5);
		ewl_container_child_append (EWL_CONTAINER (main_win),
					    tooltip);
		ewl_tooltip_text_set (EWL_TOOLTIP (tooltip), tooltips[i]);

		i++;
	}

	heap_end = sbrk(0);
	printf("HEAP SIZE:\t%u bytes\n", heap_end - heap_start);

	ewl_main();

	heap_end = sbrk(0);
	printf("HEAP SIZE:\t%u bytes\n", heap_end - heap_start);

	return 0;
}

