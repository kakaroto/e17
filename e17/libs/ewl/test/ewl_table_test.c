#include <Ewl.h>

static int col_spacing = 5;
static int row_spacing = 5;

static void change_col_spacing(Ewl_Widget * widget, void * func_data);
static void change_row_spacing(Ewl_Widget * widget, void * func_data);


int
main(int argc, char **argv)
{
	Ewl_Widget * window = NULL;
	Ewl_Widget * table = NULL;
	Ewl_Widget * vbox = NULL;
	Ewl_Widget * button[100];
	Ewl_Widget * label[100];
	int row, col;
	int b = 0;

	ewl_init(argc, argv);

	window = ewl_window_new();
	ewl_window_set_title(window, "Ewl Table Test Program");
	ewl_widget_show(window);
	ewl_window_resize(window, 800, 768);

	vbox = ewl_box_new(Ewl_Box_Type_Vertical);
	ewl_container_append_child(window, vbox);
	ewl_widget_show(vbox);

	table = ewl_table_new_all(FALSE, 5, 10, 5, 5);
	ewl_container_append_child(vbox, table);
	ewl_widget_show(table);

	for (row=0;row<10;row++)
	 {
	  for (col=0;col<5;col++)
	   {
		button[b] = ewl_button_new(Ewl_Button_Type_Normal);
		ewl_table_attach(table, button[b], col, 0, row, 0);
		if (b >= 25)
		ewl_callback_append(button[b], Ewl_Callback_Clicked,
									change_row_spacing, table);
		else
		ewl_callback_append(button[b], Ewl_Callback_Clicked,
									change_col_spacing, table);
		ewl_widget_show(button[b]);

		label[b] = ewl_text_new();
		ewl_container_append_child(button[b], label[b]);
		ewl_widget_show(label[b]);
		if (b >= 25)
		ewl_text_set_text(label[b], "Row Spacing");
		else
		ewl_text_set_text(label[b], "Col Spacing");
		b++;
	   }
	 }
	ewl_main();

	exit(0);
}

static void
change_col_spacing(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	if (col_spacing != 40)
		col_spacing += 5;
	else
		col_spacing = 0;

	ewl_table_set_col_spacing(func_data, col_spacing);
	printf("Changed table's column's spacing to %i\n", col_spacing);
}

static void
change_row_spacing(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	if (row_spacing != 40)
		row_spacing += 5;
	else
		row_spacing = 0;

	ewl_table_set_row_spacing(func_data, row_spacing);
	printf("Changed table's row's spacing to %i\n", row_spacing);
}
