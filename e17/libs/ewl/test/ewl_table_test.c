#include <Ewl.h>


int
main(int argc, char **argv)
{
	Ewl_Widget * window = NULL;
	Ewl_Widget * table = NULL;
	Ewl_Widget * vbox = NULL;
	Ewl_Widget * button[4];
	Ewl_Widget * label[2];

	ewl_init(argc, argv);

	window = ewl_window_new();
	ewl_window_set_title(window, "Ewl Table Test Program");
	ewl_widget_show(window);

	vbox = ewl_box_new(Ewl_Box_Type_Vertical);
	ewl_container_append_child(window, vbox);
	ewl_widget_show(vbox);

	table = ewl_table_new_all(FALSE, 2, 2, 5, 5);
	ewl_container_append_child(vbox, table);
	ewl_widget_show(table);

	button[0] = ewl_button_new(Ewl_Button_Type_Normal);
	ewl_table_attach(table, button[0], 0, 0, 0, 0);
	ewl_widget_show(button[0]);

	label[0] = ewl_text_new();
	ewl_container_append_child(button[0], label[0]);
	ewl_widget_show(label[0]);
	ewl_text_set_text(label[0], "E W L ! !");

	button[1] = ewl_button_new(Ewl_Button_Type_Normal);
	ewl_table_attach(table, button[1], 1, 1, 0, 0);
	ewl_widget_show(button[1]);

    label[1] = ewl_text_new();
    ewl_container_append_child(button[1], label[1]);
    ewl_widget_show(label[1]);
    ewl_text_set_text(label[1], "s m u g g");

	button[2] = ewl_button_new(Ewl_Button_Type_Normal);
	ewl_table_attach(table, button[2], 0, 0, 1, 1);
	ewl_widget_show(button[2]);

	button[3] = ewl_button_new(Ewl_Button_Type_Normal);
	ewl_table_attach(table, button[3], 1, 1, 1, 1);
	ewl_widget_show(button[3]);

	ewl_main();

	exit(0);
}
