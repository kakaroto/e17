#include <Ewl.h>

static Ewl_Widget * window2 = NULL;

static void create_info_win(Ewl_Widget * widget, void * func_data);
static void change_font(Ewl_Widget * widget, void * func_data);
static void change_color(Ewl_Widget * widget, void * func_data);

int
main(int argc, char ** argv)
{
	Ewl_Widget * window;
	Ewl_Widget * button[4];
	Ewl_Widget * vbox[3],  * hbox[1];
	Ewl_Widget * label[4], * text[2];
	Ewl_Widget * entry[1];

	ewl_init(argc, argv);

	window = ewl_window_new();
	ewl_window_set_title(window, "EWL Demonstation Application");
	ewl_widget_show(window);
	ewl_window_resize(window, 299, 500);

	vbox[0] = ewl_box_new(Ewl_Box_Type_Vertical);
	ewl_container_append_child(window, vbox[0]);
	ewl_widget_show(vbox[0]);

    vbox[2] = ewl_box_new(Ewl_Box_Type_Vertical);
    ewl_container_append_child(vbox[0], vbox[2]);
    ewl_widget_show(vbox[2]);

    text[0] = ewl_text_new();
    ewl_container_prepend_child(vbox[2], text[0]);
    ewl_widget_show(text[0]);
    ewl_text_set_text(text[0], "Blehe blaha bleho\nBlah blah blah\nBleh bleh blaha!");

    text[1] = ewl_text_new();
    ewl_container_append_child(vbox[2], text[1]);
    ewl_widget_show(text[1]);
    ewl_text_set_text(text[1], "Bleh blah bloh\nBlahblah!!!");

	entry[0] = ewl_entry_new();
	ewl_container_append_child(vbox[2], entry[0]);
	ewl_widget_show(entry[0]);
	ewl_entry_set_text(entry[0], "Play with me!");

	vbox[1] = ewl_box_new(Ewl_Box_Type_Vertical);
	ewl_container_append_child(vbox[0], vbox[1]);
	ewl_widget_show(vbox[1]);

	button[0] = ewl_button_new(Ewl_Button_Type_Normal);
	ewl_container_append_child(vbox[1], button[0]);
	ewl_callback_append(button[0], Ewl_Callback_Clicked, change_font, text[0]);
	ewl_widget_show(button[0]);

	label[0] = ewl_text_new();
	ewl_container_append_child(button[0], label[0]);
	ewl_widget_show(label[0]);
	ewl_text_set_text(label[0], strdup("Press Me!"));

	button[1] = ewl_button_new(Ewl_Button_Type_Normal);
	ewl_container_append_child(vbox[1], button[1]);
	ewl_callback_append(button[1], Ewl_Callback_Clicked, change_color, text[0]);
	ewl_widget_show(button[1]);

    label[1] = ewl_text_new();    
    ewl_container_append_child(button[1], label[1]);
    ewl_widget_show(label[1]);
    ewl_text_set_text(label[1], strdup("Press Me 2!"));

	hbox[0] = ewl_box_new(Ewl_Box_Type_Horisontal);
	ewl_container_append_child(vbox[0], hbox[0]);
	ewl_widget_show(hbox[0]);

    button[2] = ewl_button_new(Ewl_Button_Type_Normal);
    ewl_container_append_child(hbox[0], button[2]);
	ewl_callback_append(button[2], Ewl_Callback_Clicked, create_info_win, NULL);
    ewl_widget_show(button[2]);

	label[2] = ewl_text_new();
	ewl_container_append_child(button[2], label[2]);
	ewl_widget_show(label[2]);
	ewl_text_set_text(label[2], strdup("And Me!!"));
	
    button[3] = ewl_button_new(Ewl_Button_Type_Normal);
    ewl_container_append_child(hbox[0], button[3]);
	ewl_callback_append(button[3], Ewl_Callback_Clicked, change_color, text[1]);
    ewl_widget_show(button[3]);

    label[3] = ewl_text_new();
    ewl_container_append_child(button[3], label[3]);
    ewl_widget_show(label[3]);
    ewl_text_set_text(label[3], strdup("Hey! Me 2!"));

	ewl_main();

	exit(0);
}

static void
create_info_win(Ewl_Widget * widget, void * func_data)
{
	Ewl_Widget * text;
	Ewl_Widget * vbox = NULL;
	char * str;

	CHECK_PARAM_POINTER("widget", widget);

	if (window2)
		return;

	window2 = ewl_window_new();
	ewl_widget_show(window2);
	ewl_window_resize(window2, 440, 296);
	ewl_window_set_min_size(window2, 440, 296);
	ewl_window_set_max_size(window2, 1024, 768);

	vbox = ewl_box_new(Ewl_Box_Type_Vertical);
	ewl_container_append_child(window2, vbox);
	ewl_widget_show(vbox);

	str = strdup("Bleh bleh bleh blah blah blah bleh!\n"
				 "Bleh bleh bleh blah blah blah bleh!\n"
				 "Bleh bleh bleh blah blah blah bleh!\n"
				 "Bleh bleh bleh blah blah blah bleh!\n"
				 "Bleh bleh bleh blah blah blah bleh!\n"
				 "Bleh bleh bleh blah blah blah bleh!\n"
				 "Bleh bleh bleh blah blah blah bleh!\n"
				 "Bleh bleh bleh blah blah blah bleh!\n"
				 "Bleh bleh bleh blah blah blah bleh!\n"
				 "    // smugg");
	text = ewl_text_new();
	ewl_container_append_child(vbox, text);
	ewl_widget_show(text);
	ewl_text_set_text(text, str);

	return;
	func_data = NULL;
}

static void
change_font(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	if (!strcmp(ewl_text_get_font(EWL_WIDGET(func_data)), "borzoib"))
		ewl_text_set_font(EWL_WIDGET(func_data), "andover");
	else
		ewl_text_set_font(EWL_WIDGET(func_data), "borzoib");
}

static void
change_color(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ewl_text_set_color(EWL_WIDGET(func_data), rand()&0xff,
						rand()&0xff, rand()&0xff, 200);
}
