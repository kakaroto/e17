/* PACKAGE_DATA_DIR "/examples" to get path of the files */
#include "ewl_test.h"

static Ewl_Widget     *main_text;

void
__close_main_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_main_quit();

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__realize_main_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	printf("Main window realized with theme_object %p\n", w->theme_object);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__fill_main_text(Ewl_Widget * w, void *ev_data, void *user_data)
{
#define MAXLINELENGTH 1024
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
	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

int
main(int argc, char **argv)
{
	int             i;
	Ewl_Widget     *main_win;
	Ewl_Widget     *main_box;
	Ewl_Widget     *main_tree;
	Ewl_Widget     *main_area;
        Ewl_Widget *tooltip;
	Ewl_Widget     *prow[BUTTONS];
	static test_set       tests[] = {
		{ "Box",         __create_box_test_window,         "ewl_box_test.c" },
		{ "Button",      __create_button_test_window,      "ewl_button_test.c" },
		{ "ColorPicker", __create_colorpicker_test_window, "ewl_colorpicker_test.c" },
		{ "Combo",       __create_combo_test_window,       "ewl_combo_test.c" },
		{ "Entry",       __create_entry_test_window,       "ewl_entry_test.c" },
		{ "Filedialog",  __create_filedialog_test_window,  "ewl_filedialog_test.c" },
		{ "Floater",     __create_floater_test_window,     "ewl_floater_test.c" },
		{ "Image",       __create_image_test_window,       "ewl_image_test.c" },
		{ "IMenu",       __create_imenu_test_window,       "ewl_imenu_test.c" },
		{ "Menu",        __create_menu_test_window,        "ewl_menu_test.c" },
		{ "Notebook",    __create_notebook_test_window,    "ewl_notebook_test.c" },
		{ "Password",    __create_password_test_window,    "ewl_password_test.c" },
		{ "Progressbar", __create_progressbar_test_window, "ewl_progressbar_test.c" },
		{ "Spinner",     __create_spinner_test_window,     "ewl_spinner_test.c" },
		{ "Textarea",    __create_textarea_test_window,    "ewl_textarea_test.c" },
		{ "Tooltip",     __create_tooltip_test_window,     "ewl_tooltip_test.c" },
		{ "Tree",        __create_tree_test_window,        "ewl_tree_test.c" },

		
		{ 0, 0, 0 }
	};
	static char* tooltips[] = {
	        "Defines the Ewl_Box class used for\nlaying out Ewl_Widget's in a horizontal\nor vertical line.",
		"The button class is a basic button\nwith a label. This class inherits from\nthe Ewl_Box to allow for placing any\nother widget inside the button.",
		"Defines a combo box used internally.\nThe contents on the box are not drawn\noutside of the Evas.",
		"Defines the Ewl_Entry class to allow\nfor single line editable text.",
		"The filedialog is intended to be used\nfor a simple file chooser.",
		"Defines a widget for layering above other\n widgets in EWL's drawing area, with\nthe ability to follow the movement of\nanother widget.",
		"Provides a widget for displaying evas\nloadable images, and edjes.",
		"Defines a menu used internally. The\ncontents on the menu are not drawn\noutside of the Evas.",
		"Defines the basic menu classes that\nare extended to an actual menu\nimplementation by inheriting classes\nsuch as Ewl_Menu and Ewl_IMenu.",
		"Provides a container whose children\nare pages that can be switched\nbetween using tab labels along one\nedge",
		"Defines the Ewl_Password class to allow\nfor single line obscured text.",
		"Provides a statusbar from a given value.",
		"Provides a field for entering numerical\nvalues, along with buttons to increment\nand decrement the value.",
		"Defines a class for multi-line text layout\nand formatting.",
		"Defines a widget for displaying short\nmessages after a delay.",
		"Defines a widget for laying out other\nwidgets in a tree or list like manner.",
		0
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

	heap_end = sbrk(0);
	printf("HEAP SIZE:\t%u bytes\n", heap_end - heap_start);

	main_win = ewl_window_new();
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
	 * Create the main box for holding the button widgets
	 */
	main_box = ewl_hbox_new();
	ewl_container_append_child(EWL_CONTAINER(main_win), main_box);
	ewl_box_set_spacing(EWL_BOX(main_box), 6);
	ewl_box_set_homogeneous (EWL_BOX (main_box), FALSE);
	ewl_widget_show(main_box);

	main_tree = ewl_tree_new(1);
	ewl_tree_headers_set (EWL_TREE (main_tree), header);
	ewl_container_append_child(EWL_CONTAINER(main_box), main_tree);
	ewl_object_fill_policy_set (EWL_OBJECT (main_tree),
				     EWL_FLAG_FILL_HSHRINK |
				     EWL_FLAG_FILL_VFILL);
	ewl_widget_show(main_tree);

	main_area = ewl_scrollpane_new();
/* 	ewl_object_fill_policy_set (EWL_OBJECT (main_area), */
/* 				     EWL_FLAG_FILL_SHRINK || EWL_FLAG_FILL_FILL); */
	ewl_widget_show(main_area);
	ewl_container_append_child(EWL_CONTAINER(main_box), main_area);

	main_text = ewl_text_new ("  Click on the objects on the left to\nshow a working example and the source\nof the corresponding widget.\n  Keep the mouse on the button to see\na brief description of the widget.");
	ewl_container_append_child(EWL_CONTAINER(main_area),
				   main_text);
/* 	ewl_text_font_set (EWL_TEXT (main_text), "Vera", 8); */
	ewl_widget_show(main_text);

	i = 0;
	while (tests[i].func) {
		int         j;

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
		ewl_tooltip_set_delay (EWL_TOOLTIP (tooltip), 1.5);
		ewl_container_append_child (EWL_CONTAINER (main_win),
					    tooltip);
		ewl_tooltip_set_text (EWL_TOOLTIP (tooltip), tooltips[i]);

		for (j = 1; j < argc; j++) {
			if (!strcasecmp(argv[j], tests[i].name))
				tests[i].func(prow[i], NULL, NULL);
		}

		i++;
	}

	heap_end = sbrk(0);
	printf("HEAP SIZE:\t%u bytes\n", heap_end - heap_start);

	ewl_main();

	heap_end = sbrk(0);
	printf("HEAP SIZE:\t%u bytes\n", heap_end - heap_start);

	return 0;
}
