#include <Ewl.h>

int
main(int argc, char **argv)
{
	Ewl_Widget * window, * vbox, * text[4];

	ewl_init(argc, argv);

	window = ewl_window_new();
	ewl_widget_show(window);

	vbox = ewl_box_new(Ewl_Box_Type_Vertical);
	ewl_container_append_child(window, vbox);
	ewl_widget_show(vbox);

	text[0] = ewl_text_new();
	ewl_container_append_child(vbox, text[0]);
	ewl_widget_show(text[0]);
	ewl_text_set_text(text[0], "TTTTTTTTTTTTTTTTT\n"
							   "TTTTTTTTTTTTTTTTT\n"
							   "TTTTTTTTTTTTTTTTT");
    
	text[1] = ewl_text_new();
	ewl_container_append_child(vbox, text[1]);
	ewl_widget_show(text[1]);
			
    ewl_text_set_text(text[1], "UUUUUUUUUUUUUUUUU\n"
                               "UUUUUUUUUUUUUUUUU\n"
                               "UUUUUUUUUUUUUUUUU");

    text[2] = ewl_text_new();
	ewl_container_append_child(vbox, text[2]);
	ewl_widget_show(text[2]);		
    ewl_text_set_text(text[2], "TTTTTTTTTTTTTTTTT\n"
                               "TTTTTTTTTTTTTTTTT\n"
                               "TTTTTTTTTTTTTTTTT");
    
    text[3] = ewl_text_new();
	ewl_container_append_child(vbox, text[3]);
	ewl_widget_show(text[3]);		
    ewl_text_set_text(text[3], "AAAAAAAAAAAAAAAAA\n"
                               "AAAAAAAAAAAAAAAAA\n"
                               "AAAAAAAAAAAAAAAAA");

	ewl_main();

	exit(-1);
}
