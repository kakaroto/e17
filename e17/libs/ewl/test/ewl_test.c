#include <Ewl.h>

static void callback_main_exit(Ewl_Widget * widget, void * func_data);
static void callback_entry_main(Ewl_Widget * widget, void * func_data);
static void callback_entry_print(Ewl_Widget * widget, void * func_data);
static void callback_entry_set_text(Ewl_Widget * widget, void * func_data);

static void callback_seeker_main(Ewl_Widget * widget, void * func_data);
static void callback_seeker_get_val(Ewl_Widget * widget, void * func_data);
static void callback_seeker_set_val(Ewl_Widget * widget, void * func_data);
static void callback_seeker_change_ori(Ewl_Widget * widget, void * func_data);

static void callback_spinner_main(Ewl_Widget * widget, void * func_data);
static void callback_spinner_get_val(Ewl_Widget * widget, void * func_data);
static void callback_spinner_set_val(Ewl_Widget * widget, void * func_data);
static void callback_spinner_set_digits(Ewl_Widget * widget, void * func_data);



static void callback_box_main(Ewl_Widget * widget, void * func_data);
static void callback_button_main(Ewl_Widget * widget, void * func_data);
static void callback_table_main(Ewl_Widget * widget, void * func_data);


int
main(int argc, char ** argv)
{
	Ewl_Widget * main_window;
	Ewl_Widget * table[2];
	Ewl_Widget * button[7];
	Ewl_Widget * text;
	void (** callbacks) (Ewl_Widget * widget, void * func_data) = NULL;
	char * labels[] =
	  {
		"Entry Test", "Seeker Test", "Spinner Test",
		 "Box Test", "Button Test", "Table Test"
	  };
	int row, col, count = 0;

	callbacks = NEW_PTR(7);
	for (count = 0; count < 6;count++)
	  {
		callbacks[count] = NULL;
	  }

	callbacks[0] = callback_entry_main;
	callbacks[1] = callback_seeker_main;
	callbacks[2] = callback_spinner_main;
	callbacks[3] = callback_box_main;
	callbacks[4] = callback_button_main;
	callbacks[5] = callback_table_main;

	count = 0;

	ewl_init(argc, argv);

	main_window = ewl_window_new();
	ewl_window_set_title(main_window, "EWL Demo/Test Application");
	ewl_window_resize(main_window, 759, 346);
	ewl_window_set_min_size(main_window, 759, 346);
	ewl_window_set_max_size(main_window, 759, 346);
	ewl_widget_show(main_window);

	table[0] = ewl_table_new_all(FALSE, 1, 3, 10, 10);
	ewl_container_append_child(main_window, table[0]);
	ewl_widget_show(table[0]);

	text = ewl_text_new();
	ewl_table_attach(table[0], text, 1, 1, 1, 1);
	ewl_text_set_text(text,
		"                             Welcome!\n \n"
		"    This is a test/demonstration Application of what EWL can do.\n"
		"    Atm, its not to much, but it gets better every day.\n"
		"    A bit down in this window you see a table with some buttons \n"
		"    Press a button and a new window should appear with some info\n"
		"    and widgets wich you can play with....\n"
		"    Not to much yet, but there will be more within a short future..\n"
		"        // smugg");
	ewl_widget_show(text);

	table[1] = ewl_table_new_all(FALSE, 3, 2, 2, 2);
	ewl_table_attach(table[0], table[1], 1, 1, 2, 2);
	ewl_widget_show(table[1]);

	for (row=0;row<2;row++)
	  {
		for (col=0;col<3;col++)
		  {
			button[count] = ewl_button_new_with_label(labels[count]);
			ewl_table_attach(table[1],button[count],col+1, col+1, row+1, row+1);
			ewl_callback_append(button[count], EWL_CALLBACK_CLICKED,
									callbacks[count], NULL);
			ewl_widget_show(button[count++]);
		  }
	  }

	button[count] = ewl_button_new_with_label("Exit");
	ewl_table_attach(table[0], button[count], 1, 1, 3, 3);
	ewl_callback_append(button[count], EWL_CALLBACK_CLICKED,
						callback_main_exit, main_window);
	ewl_widget_show(button[count]);

	ewl_main();

	exit(-1);
}

static void
callback_main_exit(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_main_quit();

	printf("Thank You For Using Ewl.. Good bye\n");

	func_data = NULL;
}


/* Entry Test Section */
static void
callback_entry_main(Ewl_Widget * widget, void * func_data)
{
	Ewl_Widget * entry_win;
	Ewl_Widget * main_vbox;
	Ewl_Widget * text;
	Ewl_Widget * entry;
	Ewl_Widget * button_table;
	Ewl_Widget * button[3];

	entry_win = ewl_window_new();
	ewl_window_set_title(entry_win, "EWL Entry Widget Test");
    ewl_window_resize(entry_win, 779, 463);
    ewl_window_set_min_size(entry_win, 779, 463);
    ewl_window_set_max_size(entry_win, 779, 463);
	ewl_widget_show(entry_win);

	main_vbox = ewl_vbox_new();
	ewl_container_append_child(entry_win, main_vbox);
	ewl_widget_show(main_vbox);

	text = ewl_text_new();
	ewl_container_append_child(main_vbox, text);
	ewl_text_set_text(text,
		"This is entry widget, it's designed so it uses the text widget\n"
		"for text, and it has its own cursor and selection widget So far\n"
		"you can only do text query's, but you will be able to hide the text,\n"
		"could be useful when you want to type a password and toggle\n"
		"numberic/alphabetic, with means you can only type one of those, or\n"
		"both as you can now.");
	ewl_widget_show(text);

	entry = ewl_entry_new();
	ewl_container_append_child(main_vbox, entry);
	ewl_entry_set_text(entry, "Play With Me...");
	ewl_widget_show(entry);

	button_table = ewl_table_new_all(FALSE, 3, 1, 5, 0);
	ewl_container_append_child(main_vbox, button_table);
	ewl_widget_show(button_table);

	button[0] = ewl_button_new_with_label("Get Text");
	ewl_table_attach(button_table, button[0], 1, 1, 1, 1);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			callback_entry_print, entry);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new_with_label("Set Text");
	ewl_table_attach(button_table, button[1], 2, 2, 1, 1);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			callback_entry_set_text, entry);
	ewl_widget_show(button[1]);

	button[2] = ewl_button_new_with_label("Close");
	ewl_table_attach(button_table, button[2], 3, 3, 1, 1);
	ewl_widget_show(button[2]);

	return;
	widget = NULL;
	func_data = NULL;
}

static void
callback_entry_print(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	printf("%s\n", ewl_entry_get_text(func_data));

	return;
	widget = NULL;
}

static void
callback_entry_set_text(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

    return;
    widget = NULL;
    func_data = NULL;
}


/* Seeker Test Section */
static void callback_seeker_main(Ewl_Widget * widget, void * func_data)
{
    Ewl_Widget * seeker_win;
    Ewl_Widget * main_vbox;
    Ewl_Widget * text;
    Ewl_Widget * seeker;
    Ewl_Widget * button_table;
    Ewl_Widget * button[4];
    
    seeker_win = ewl_window_new();
    ewl_window_set_title(seeker_win, "E W L Seeker Widget Test");
    ewl_window_resize(seeker_win, 779, 463);
    ewl_window_set_min_size(seeker_win, 779, 463);
    ewl_window_set_max_size(seeker_win, 779, 463);
    ewl_widget_show(seeker_win);

    main_vbox = ewl_vbox_new();
    ewl_container_append_child(seeker_win, main_vbox);
    ewl_widget_show(main_vbox);

    text = ewl_text_new();
    ewl_container_append_child(main_vbox, text);
    ewl_text_set_text(text,
        "The seeker widget can be very useful to let the user change value\n"
		"of something, or for example change volume in a multimedia system.\n"
		"The seeker can be very useful");
    ewl_widget_show(text);

	seeker = ewl_vseeker_new();
    ewl_container_append_child(main_vbox, seeker);
    ewl_widget_show(seeker);

    button_table = ewl_table_new_all(FALSE, 4, 1, 5, 0);
    ewl_container_append_child(main_vbox, button_table);
    ewl_widget_show(button_table);

    button[0] = ewl_button_new_with_label("Get Val");
    ewl_table_attach(button_table, button[0], 1, 1, 1, 1);
    ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
            callback_seeker_get_val, seeker);
    ewl_widget_show(button[0]);

    button[1] = ewl_button_new_with_label("Set Val");
    ewl_table_attach(button_table, button[1], 2, 2, 1, 1);
    ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
            callback_seeker_set_val, seeker);
    ewl_widget_show(button[1]);

	button[2] = ewl_button_new_with_label("Change Orientation");
	ewl_table_attach(button_table, button[2], 3, 3, 1, 1);
	ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
			callback_seeker_change_ori, seeker);
	ewl_widget_show(button[2]);

    button[3] = ewl_button_new_with_label("Close");
    ewl_table_attach(button_table, button[3], 4, 4, 1, 1);
    ewl_widget_show(button[3]);

	return;
    widget = NULL;
    func_data = NULL;
}

static void
callback_seeker_get_val(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	printf("%i\n", ewl_seeker_get_value(func_data));
}

static void
callback_seeker_set_val(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ewl_seeker_set_value(func_data,rand() % (EWL_SEEKER(func_data)->range)); 
}

static void
callback_seeker_change_ori(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	if (EWL_SEEKER(func_data)->orientation == EWL_ORIENTATION_HORISONTAL)
		ewl_seeker_set_orientation(func_data, EWL_ORIENTATION_VERTICAL);
	else
		ewl_seeker_set_orientation(func_data, EWL_ORIENTATION_HORISONTAL);
}


/* Spinner Test Section */
static void callback_spinner_main(Ewl_Widget * widget, void * func_data)
{
    Ewl_Widget * spinner_win;
    Ewl_Widget * main_vbox;
    Ewl_Widget * text;
    Ewl_Widget * spinner;
    Ewl_Widget * button_table;
    Ewl_Widget * button[4];

    spinner_win = ewl_window_new();
    ewl_window_set_title(spinner_win, "E W L Seeker Widget Test");
    ewl_window_resize(spinner_win, 779, 463);
    ewl_window_set_min_size(spinner_win, 779, 463);
    ewl_window_set_max_size(spinner_win, 779, 463);
    ewl_widget_show(spinner_win);

    main_vbox = ewl_vbox_new();
    ewl_container_append_child(spinner_win, main_vbox);
    ewl_widget_show(main_vbox);

    text = ewl_text_new();
    ewl_container_append_child(main_vbox, text);
    ewl_text_set_text(text,
        "With a spinner widget you can set maximum/minimum value.\n"
		"the value in a seeker widget is a double, for maximum precision\n"
		"you can also set how many digits you want to display.");
    ewl_widget_show(text);

    spinner = ewl_spinner_new();
    ewl_container_append_child(main_vbox, spinner);
    ewl_widget_show(spinner);

    button_table = ewl_table_new_all(FALSE, 4, 1, 5, 0);
    ewl_container_append_child(main_vbox, button_table);
    ewl_widget_show(button_table);

    button[0] = ewl_button_new_with_label("Get Val");
    ewl_table_attach(button_table, button[0], 1, 1, 1, 1);
    ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
            callback_spinner_get_val, spinner);
    ewl_widget_show(button[0]);

    button[1] = ewl_button_new_with_label("Set Val");
    ewl_table_attach(button_table, button[1], 2, 2, 1, 1);
    ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
            callback_spinner_set_val, spinner);
    ewl_widget_show(button[1]);

    button[2] = ewl_button_new_with_label("Set Digits");
    ewl_table_attach(button_table, button[2], 3, 3, 1, 1);
    ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
            callback_spinner_set_digits, spinner);
    ewl_widget_show(button[2]);

    button[3] = ewl_button_new_with_label("Close");
    ewl_table_attach(button_table, button[3], 4, 4, 1, 1);
    ewl_widget_show(button[3]);

    return;
    widget = NULL;
    func_data = NULL;
}

static void
callback_spinner_get_val(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	printf("%f\n", ewl_spinner_get_value(func_data));
}

static void
callback_spinner_set_val(Ewl_Widget * widget, void * func_data)
{
	double val;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	val = (double) (rand() % (int) EWL_SPINNER(func_data)->max_val) / 3;
	printf("%f\n", val);

	ewl_spinner_set_value(func_data, val);
}

static void
callback_spinner_set_digits(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ewl_spinner_set_digits(func_data, rand() % 5);
}


/* Box Test Section */
static void callback_box_main(Ewl_Widget * widget, void * func_data)
{
    return;
    widget = NULL;
    func_data = NULL;
}

/* Button Test Section */
static void callback_button_main(Ewl_Widget * widget, void * func_data)
{
    return;
    widget = NULL;
    func_data = NULL;
}

/* Table Test Section */
static void callback_table_main(Ewl_Widget * widget, void * func_data)
{
    return;
    widget = NULL;
    func_data = NULL;
}
