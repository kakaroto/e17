#include <Ewl.h>


static void callback_main_exit(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_entry_main(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_entry_print(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_entry_set_text(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_toggle_visibility(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_destroy(Ewl_Widget * widget, Ewl_Callback * cb);

static void callback_seeker_main(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_seeker_get_val(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_seeker_set_val(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_seeker_change_ori(Ewl_Widget * widget, Ewl_Callback * cb);


static void callback_spinner_main(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_spinner_get_val(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_spinner_set_val(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_spinner_set_digits(Ewl_Widget * widget, Ewl_Callback * cb);


static void callback_box_main(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_button_main(Ewl_Widget * widget, Ewl_Callback * cb);


static void callback_table_main(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_table_set_col_space(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_table_set_row_space(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_table_set_homo(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_table_set_alignment(Ewl_Widget * widget, Ewl_Callback * cb);

static void callback_fx_main(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_fx_fade_in(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_fx_fade_out(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_fx_glow(Ewl_Widget * widget, Ewl_Callback * cb);


static void callback_list_main(Ewl_Widget * widget, Ewl_Callback * cb);
static void callback_list_add_5(Ewl_Widget * widget, Ewl_Callback * cb);

static void callback_text_main(Ewl_Widget * widget, Ewl_Callback * cb);

static void callback_window_close(Ewl_Widget * widget, Ewl_Callback * cb);

int
main(int argc, char ** argv)
{
	Ewl_Widget * main_window;
	Ewl_Widget * table[2];
	Ewl_Widget * button[9];
	Ewl_Widget * text;
	void (** callbacks) (Ewl_Widget * widget, Ewl_Callback * cb) = NULL;
	char * labels[] =
	  {
		"Entry Test", "Seeker Test", "Spinner Test",
		 "Box Test", "Button Test", "Table Test",
		 "FX Test", "List Test", "Text Test"
	  };
	int row, col, count = 0;

	callbacks = NEW_PTR(9);
	for (count = 0; count < 8;count++)
	  {
		callbacks[count] = NULL;
	  }

	callbacks[0] = callback_entry_main;
	callbacks[1] = callback_seeker_main;
	callbacks[2] = callback_spinner_main;
	callbacks[3] = callback_box_main;
	callbacks[4] = callback_button_main;
	callbacks[5] = callback_table_main;
	callbacks[6] = callback_fx_main;
	callbacks[7] = callback_list_main;
	callbacks[8] = callback_text_main;
	count = 0;

	ewl_init(argc, argv);

	main_window = ewl_window_new();
	ewl_window_set_title(main_window, "EWL Demo/Test Application");
	ewl_window_resize(main_window, 759, 338);
	ewl_window_set_min_size(main_window, 759, 338);
	ewl_window_set_max_size(main_window, 759, 338);
	ewl_callback_append(main_window, EWL_CALLBACK_DELETE_WINDOW,
				callback_window_close, main_window);
	ewl_widget_show(main_window);

	table[0] = ewl_table_new_all(FALSE, 1, 3, 10, 10);
	ewl_container_append_child(main_window, table[0]);
	ewl_widget_show(table[0]);

	text = ewl_text_new();
	ewl_table_attach(table[0], text, 1, 1, 1, 1);
	ewl_text_set_text(text,
		"                           Welcome!\n\n"
		"  This is a test/demonstration Application of what EWL can do.  \n"
		"  A bit down in this window you see a table with some buttons  \n"
		"  Press a button and a new window should appear with some info  \n"
		"  and widgets that you can play with....\n"
		"  Not to much yet, but there will be more within a short future..  \n"
		"\n        // smugg");
	ewl_fx_add(text, EWL_FX_TYPE_GLOW, NULL, NULL);
	ewl_widget_show(text);

	table[1] = ewl_table_new_all(FALSE, 3, 3, 2, 2);
	ewl_table_attach(table[0], table[1], 1, 1, 2, 2);
	ewl_widget_show(table[1]);

	for (row=0;row<3;row++)
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

	button[count] = ewl_button_new_with_label(" Exit ");
	ewl_table_attach(table[0], button[count], 1, 1, 3, 3);
	ewl_callback_append(button[count], EWL_CALLBACK_CLICKED,
						callback_main_exit, main_window);
	ewl_widget_show(button[count]);

	ewl_main();

	exit(-1);
}

static void
callback_main_exit(Ewl_Widget * widget, Ewl_Callback * cb)
{
	printf("Thank You For Using Ewl.. Good bye\n");

	ewl_main_quit();

	return;
	widget = NULL;
	cb = NULL;
}

static void
callback_window_close(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	if (widget->type == EWL_WIDGET_WINDOW)
		callback_main_exit(NULL, NULL);

	ewl_widget_destroy(cb->user_data);
}

/* Entry Test Section */
static void
callback_entry_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ewl_Widget * entry_win;
	Ewl_Widget * main_vbox;
	Ewl_Widget * text;
	Ewl_Widget * entry;
	Ewl_Widget * button_table;
	Ewl_Widget * button[5];

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
		"This is the entry widget, you can use this if you want to get input\n"
		"from the user.");
	ewl_widget_show(text);

	entry = ewl_entry_new();
	ewl_container_append_child(main_vbox, entry);
	ewl_entry_set_text(entry, "Play With Me...");
	ewl_widget_show(entry);

	button_table = ewl_table_new_all(FALSE, 5, 1, 2, 2);
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

	button[2] = ewl_button_new_with_label("Hide / Show");
	ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
				callback_toggle_visibility, entry);
	ewl_table_attach(button_table, button[2], 3, 3, 1, 1);
	ewl_widget_show(button[2]);

    button[3] = ewl_button_new_with_label("Destroy");
    ewl_callback_append(button[3], EWL_CALLBACK_CLICKED,
                callback_destroy, entry);
    ewl_table_attach(button_table, button[3], 4, 4, 1, 1);
    ewl_widget_show(button[3]);

    button[4] = ewl_button_new_with_label("Close");
/*    ewl_callback_append(button[4], EWL_CALLBACK_CLICKED,
                callback_window_close, entry_win);*/
    ewl_table_attach(button_table, button[4], 5, 5, 1, 1);
    ewl_widget_show(button[4]);

	return;
	widget = NULL;
	cb = NULL;
}

static void
callback_entry_print(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	printf("%s\n", ewl_entry_get_text(cb->user_data));

	return;
	widget = NULL;
}

static void
callback_entry_set_text(Ewl_Widget * widget, Ewl_Callback * cb)
{
    char * texts[] =
      {
        "Please leave a message", "E17", "Enlightenment", "Evas", "Imlib 2",
        "Alpha", "Omega", "smugg", "raster", "RbdPngn"
      };

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ewl_entry_set_text(cb->user_data, texts[rand() % 10]);

    return;
    widget = NULL;
}

static void
callback_toggle_visibility(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	if (!EWL_OBJECT(cb->user_data)->visible)
		ewl_widget_show(cb->user_data);
	else
		ewl_widget_hide(cb->user_data);
}

static void
callback_destroy(Ewl_Widget * widget, Ewl_Callback * cb)
{
    CHECK_PARAM_POINTER("widget", widget);
    CHECK_PARAM_POINTER("cb", cb);

	ewl_widget_destroy(cb->user_data);
}

/* Seeker Test Section */
static void
callback_seeker_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
    Ewl_Widget * seeker_win;
    Ewl_Widget * main_vbox;
    Ewl_Widget * text;
    Ewl_Widget * seeker;
    Ewl_Widget * button_table;
    Ewl_Widget * button[6];
    
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
        "The seeker widget can be compared to a progress bar, the only\n"
		"diffrence is that, the user can change value, by draging the\n"
		"bar with the mouse, or with your arrow keys.");
    ewl_widget_show(text);

	seeker = ewl_vseeker_new();
    ewl_container_append_child(main_vbox, seeker);
    ewl_widget_show(seeker);

    button_table = ewl_table_new_all(FALSE, 3, 2, 2, 2);
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

    button[3] = ewl_button_new_with_label("Hide / Show");
    ewl_callback_append(button[3], EWL_CALLBACK_CLICKED,
                callback_toggle_visibility, seeker);
    ewl_table_attach(button_table, button[3], 1, 1, 2, 2);
    ewl_widget_show(button[3]);

    button[4] = ewl_button_new_with_label("Destroy");
    ewl_callback_append(button[4], EWL_CALLBACK_CLICKED,
                callback_destroy, seeker);
    ewl_table_attach(button_table, button[4], 2, 2, 2, 2);
    ewl_widget_show(button[4]);

    button[5] = ewl_button_new_with_label("Close");
/*    ewl_callback_append(button[5], EWL_CALLBACK_CLICKED,
                callback_window_close, seeker_win);*/
    ewl_table_attach(button_table, button[5], 3, 3, 2, 2);
    ewl_widget_show(button[5]);

	return;
    widget = NULL;
    cb = NULL;
}

static void
callback_seeker_get_val(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	printf("%i\n", ewl_seeker_get_value(cb->user_data));
}

static void
callback_seeker_set_val(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ewl_seeker_set_value(cb->user_data,rand() %
			(EWL_SEEKER(cb->user_data)->range)); 
}

static void
callback_seeker_change_ori(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	if (EWL_SEEKER(cb->user_data)->orientation == EWL_ORIENTATION_HORISONTAL)
		ewl_seeker_set_orientation(cb->user_data, EWL_ORIENTATION_VERTICAL);
	else
		ewl_seeker_set_orientation(cb->user_data, EWL_ORIENTATION_HORISONTAL);
}


/* Spinner Test Section */
static void callback_spinner_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
    Ewl_Widget * spinner_win;
    Ewl_Widget * main_vbox;
    Ewl_Widget * text;
    Ewl_Widget * spinner;
    Ewl_Widget * button_table;
    Ewl_Widget * button[6];

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
		"the value in a seeker widget is declared as a double, for\n"
		"maximum precision you can also set how many digits you\n"
		"want to display. The spinner's value can very easy be\n"
		"changed with the arrow buttons.");
    ewl_widget_show(text);

    spinner = ewl_spinner_new();
    ewl_container_append_child(main_vbox, spinner);
    ewl_widget_show(spinner);

    button_table = ewl_table_new_all(FALSE, 3, 2, 2, 2);
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

    button[3] = ewl_button_new_with_label("Hide / Show");
    ewl_callback_append(button[3], EWL_CALLBACK_CLICKED,
                callback_toggle_visibility, spinner);
    ewl_table_attach(button_table, button[3], 1, 1, 2, 2);
    ewl_widget_show(button[3]);

    button[4] = ewl_button_new_with_label("Destroy");
    ewl_callback_append(button[4], EWL_CALLBACK_CLICKED,
                callback_destroy, spinner);
    ewl_table_attach(button_table, button[4], 2, 2, 2, 2);
    ewl_widget_show(button[4]);

    button[5] = ewl_button_new_with_label("Close");
/*    ewl_callback_append(button[5], EWL_CALLBACK_CLICKED,
                callback_window_close, spinner_win);*/
    ewl_table_attach(button_table, button[5], 3, 3, 2, 2);
    ewl_widget_show(button[5]);

    return;
    widget = NULL;
    cb = NULL;
}

static void
callback_spinner_get_val(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	printf("%f\n", ewl_spinner_get_value(cb->user_data));
}

static void
callback_spinner_set_val(Ewl_Widget * widget, Ewl_Callback * cb)
{
	double val;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	val = (double) (rand() % (int) EWL_SPINNER(cb->user_data)->max_val) / 3;
	printf("%f\n", val);

	ewl_spinner_set_value(cb->user_data, val);
}

static void
callback_spinner_set_digits(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ewl_spinner_set_digits(cb->user_data, rand() % 5);
}


/* Box Test Section */
static void
callback_box_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
    return;
    widget = NULL;
    cb = NULL;
}

/* Button Test Section */
static void
callback_button_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
    return;
    widget = NULL;
    cb = NULL;
}

/* Table Test Section */
static void
callback_table_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
    Ewl_Widget * table_win;
    Ewl_Widget * main_vbox;
    Ewl_Widget * text;
    Ewl_Widget * table;
    Ewl_Widget * button_table;
    Ewl_Widget * button[7];
	Ewl_Widget * table_buttons[5*5];
	int row, col, count = 0;

    table_win = ewl_window_new();
    ewl_window_set_title(table_win, "E W L Table Widget Test");
    ewl_window_resize(table_win, 779, 600);
    ewl_window_set_min_size(table_win, 779, 600);
    ewl_window_set_max_size(table_win, 779, 600);
    ewl_widget_show(table_win); 

    main_vbox = ewl_vbox_new();
    ewl_container_append_child(table_win, main_vbox);
    ewl_widget_show(main_vbox);

    text = ewl_text_new();
    ewl_container_append_child(main_vbox, text);
    ewl_text_set_text(text,
        "With a table widget you can set maximum/minimum value.\n"
        "the value in a seeker widget is declared as a double, for\n"
        "maximum precision you can also set how many digits you\n"
        "want to display. The table's value can very easy be\n"
        "changed with the arrow buttons.");
    ewl_widget_show(text);

    table = ewl_table_new_all(FALSE, 5, 5, 2, 2);
    ewl_container_append_child(main_vbox, table);
    ewl_widget_show(table);

	for (row=0;row<5;row++)
	  {
		for (col=0;col<5;col++)
		  {
			table_buttons[count] = ewl_button_new();
			ewl_table_attach(table, table_buttons[count],
								col+1, col+1, row+1, row+1);
			ewl_widget_show(table_buttons[count++]);
		  }
	  }

    button_table = ewl_table_new_all(FALSE, 3, 3, 2, 2);
    ewl_container_append_child(main_vbox, button_table);
	ewl_table_set_alignment(button_table, EWL_ALIGNMENT_RIGHT);
    ewl_widget_show(button_table);

    button[0] = ewl_button_new_with_label("Change Col Spacing");
    ewl_table_attach(button_table, button[0], 1, 1, 1, 1);
    ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
            callback_table_set_col_space, table);
    ewl_widget_show(button[0]);

    button[1] = ewl_button_new_with_label("Change Row Spacing");
    ewl_table_attach(button_table, button[1], 2, 2, 1, 1);
    ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
            callback_table_set_row_space, table);
    ewl_widget_show(button[1]);

    button[2] = ewl_button_new_with_label("Toggle homogeneous");
    ewl_table_attach(button_table, button[2], 3, 3, 1, 1);
    ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
            callback_table_set_homo, table);
    ewl_widget_show(button[2]);

	button[3] = ewl_button_new_with_label("Change Alignment");
	ewl_table_attach(button_table, button[3], 1, 1, 2, 2);
	ewl_callback_append(button[3], EWL_CALLBACK_CLICKED,
			callback_table_set_alignment, table);
	ewl_widget_show(button[3]);

    button[4] = ewl_button_new_with_label("Hide / Show");
    ewl_callback_append(button[4], EWL_CALLBACK_CLICKED,
                callback_toggle_visibility, table);
    ewl_table_attach(button_table, button[4], 2, 2, 2, 2);
    ewl_widget_show(button[4]);

    button[5] = ewl_button_new_with_label("Destroy");
    ewl_callback_append(button[5], EWL_CALLBACK_CLICKED,
                callback_destroy, table);
    ewl_table_attach(button_table, button[5], 3, 3, 2, 2);
    ewl_widget_show(button[5]);

    button[6] = ewl_button_new_with_label("Close");
/*    ewl_callback_append(button[6], EWL_CALLBACK_CLICKED,
                callback_window_close, table_win);*/
    ewl_table_attach(button_table, button[6], 2, 2, 3, 3);
    ewl_widget_show(button[6]);

    return;
    widget = NULL;
    cb = NULL;
}

static void
callback_table_set_col_space(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ewl_table_set_col_spacing(cb->user_data, rand() % 50);
}

static void
callback_table_set_row_space(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ewl_table_set_row_spacing(cb->user_data, rand() % 50);
}

static void
callback_table_set_homo(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	if (EWL_TABLE(cb->user_data)->homogeneous)
		ewl_table_set_homogeneous(cb->user_data, FALSE);
	else
		ewl_table_set_homogeneous(cb->user_data, TRUE);
}

static void
callback_table_set_alignment(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	if (EWL_TABLE(cb->user_data)->alignment == EWL_ALIGNMENT_LEFT)
		ewl_table_set_alignment(cb->user_data, EWL_ALIGNMENT_CENTER);
	else if (EWL_TABLE(cb->user_data)->alignment == EWL_ALIGNMENT_CENTER)
		ewl_table_set_alignment(cb->user_data, EWL_ALIGNMENT_RIGHT);
	else if (EWL_TABLE(cb->user_data)->alignment == EWL_ALIGNMENT_RIGHT)
		ewl_table_set_alignment(cb->user_data, EWL_ALIGNMENT_LEFT);
}

/* FX Test Section */
static void
callback_fx_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
    Ewl_Widget * fx_win;
    Ewl_Widget * main_vbox;
    Ewl_Widget * text;
    Ewl_Widget * button_table;
    Ewl_Widget * button[5];

    fx_win = ewl_window_new();
    ewl_window_set_title(fx_win, "E W L List Widget Test");
    ewl_window_resize(fx_win, 779, 340);
    ewl_window_set_min_size(fx_win, 779, 340);
    ewl_window_set_max_size(fx_win, 779, 340);
    ewl_widget_show(fx_win);

    main_vbox = ewl_vbox_new();
    ewl_container_append_child(fx_win, main_vbox);
    ewl_widget_show(main_vbox);

    text = ewl_text_new();
    ewl_container_append_child(main_vbox, text);
    ewl_text_set_text(text,
        "I added the FX code just for fun, it's really bad, and will be\n"
		"rewritten completly....\n");
    ewl_widget_show(text);

    button[0] = ewl_button_new_with_label("I'm Useless");
    ewl_container_append_child(main_vbox, button[0]);
    ewl_widget_show(button[0]);

    button_table = ewl_table_new_all(FALSE, 4, 1, 2, 2);
    ewl_container_append_child(main_vbox, button_table);
    ewl_widget_show(button_table);

    button[1] = ewl_button_new_with_label("Fade In");
    ewl_table_attach(button_table, button[1], 1, 1, 1, 1);
    ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
            callback_fx_fade_in, button[0]);
    ewl_widget_show(button[1]);

    button[2] = ewl_button_new_with_label("Fade Out");
    ewl_table_attach(button_table, button[2], 2, 2, 1, 1);
    ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
            callback_fx_fade_out, button[0]);
    ewl_widget_show(button[2]);

    button[3] = ewl_button_new_with_label("Glow");
    ewl_table_attach(button_table, button[3], 3, 3, 1, 1);
    ewl_callback_append(button[3], EWL_CALLBACK_CLICKED,
            callback_fx_glow, button[0]);
    ewl_widget_show(button[3]);

    button[4] = ewl_button_new_with_label("Close");
    ewl_table_attach(button_table, button[4], 4, 4, 1, 1);
/*	ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
				callback_window_close, fx_win);*/
    ewl_widget_show(button[4]);

	return;
	widget = NULL;
	cb = NULL;
}

static void callback_fx_fade_in(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ewl_fx_add(cb->user_data, EWL_FX_TYPE_FADE_IN, NULL, NULL);
}

static void callback_fx_fade_out(Ewl_Widget * widget, Ewl_Callback * cb)
{
    CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ewl_fx_add(cb->user_data, EWL_FX_TYPE_FADE_OUT, NULL, NULL);
}

static void callback_fx_glow(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ewl_fx_add(cb->user_data, EWL_FX_TYPE_GLOW, NULL, NULL);
}

/* List Test Section */
static void
callback_list_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
    Ewl_Widget * list_win;
    Ewl_Widget * main_vbox;
    Ewl_Widget * text;
    Ewl_Widget * list;
    Ewl_Widget * button_table;
    Ewl_Widget * button[4];
	char * row_text[] = { "E E E", "W W W", "L L L", NULL };

    list_win = ewl_window_new();
    ewl_window_set_title(list_win, "E W L List Widget Test");
    ewl_window_resize(list_win, 779, 600);
    ewl_window_set_min_size(list_win, 779, 600);
    ewl_window_set_max_size(list_win, 770, 600);
    ewl_widget_show(list_win);

    main_vbox = ewl_vbox_new();
    ewl_container_append_child(list_win, main_vbox);
    ewl_widget_show(main_vbox);

    text = ewl_text_new();
    ewl_container_append_child(main_vbox, text);
    ewl_text_set_text(text,
		"Add a couple of row's, press one of the tiles, and start\n"
		"Navigating the list with your arrow keys...");
	ewl_widget_show(text);

    list = ewl_list_new(3);
    ewl_container_append_child(main_vbox, list);
	EWL_OBJECT(list)->maximum.w = 300;
	EWL_OBJECT(list)->request.h = 400;
    ewl_widget_show(list);

	ewl_list_set_titles(list, row_text);

    button_table = ewl_table_new_all(FALSE, 3, 2, 2, 2);
    ewl_container_append_child(main_vbox, button_table);
    ewl_widget_show(button_table);

    button[0] = ewl_button_new_with_label("Add 5 Rows");
    ewl_table_attach(button_table, button[0], 1, 1, 1, 1);
    ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
            callback_list_add_5, list);
    ewl_widget_show(button[0]);

    button[1] = ewl_button_new_with_label("Del 5 Rows");
    ewl_table_attach(button_table, button[1], 2, 2, 1, 1);
    ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
            callback_seeker_set_val, list);
    ewl_widget_show(button[1]);

    button[2] = ewl_button_new_with_label("Clear List");
    ewl_table_attach(button_table, button[2], 3, 3, 1, 1);
    ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
            callback_spinner_set_digits, list);
    ewl_widget_show(button[2]);

	button[4] = ewl_button_new_with_label("Hide / Show");
    ewl_callback_append(button[4], EWL_CALLBACK_CLICKED,
                callback_toggle_visibility, list);
    ewl_table_attach(button_table, button[4], 1, 1, 2, 2);
    ewl_widget_show(button[4]);

    button[5] = ewl_button_new_with_label("Destroy");
    ewl_callback_append(button[5], EWL_CALLBACK_CLICKED,
                callback_destroy, list);
    ewl_table_attach(button_table, button[5], 2, 2, 2, 2);
    ewl_widget_show(button[5]);

    button[6] = ewl_button_new_with_label("Close");
/*    ewl_callback_append(button[6], EWL_CALLBACK_CLICKED,
                callback_window_close, list_win);*/
    ewl_table_attach(button_table, button[6], 3, 3, 2, 2);
    ewl_widget_show(button[6]);

    return;
    widget = NULL;
    cb = NULL;
}

static void
callback_list_add_5(Ewl_Widget * widget, Ewl_Callback * cb)
{
	char * texts[] =
	  {
		"E W L !", "E17", "Enlightenment", "Evas", "Imlib 2",
		"Alpha", "Omega", "smugg", "raster", "rbdpngn"
	  };
	char * insert_em[11];
	int count, i;

	for (count=0;count<5;count++)
	  {
		for(i=0;i<11;i++)
		  {
			insert_em[i] = texts[rand() % 10];
		  }

		  ewl_list_append_text(cb->user_data, insert_em);
	  }

	ewl_list_select_row(cb->user_data, 3);

	return;
	widget = NULL;
}

/* Text Test Section */
static void
callback_text_main(Ewl_Widget * widget, Ewl_Callback * cb)
{
	return;
	widget = NULL;
	cb = NULL;
}
