
#include "ewl_test.h"


extern Ewl_Widget *main_menu;


Ewl_Widget     *
ewl_menu_bar_test_create(void)
{
	Ewl_Widget     *main_menu_bar;
	Ewl_Widget     *file_menu, *test_menu, *help_menu;
	Ewl_Widget     *smi_file, *smi_windows, *smi_help;

	main_menu_bar = ewl_menu_bar_new();
	file_menu = ewl_menu_new();
	test_menu = ewl_menu_new();
	help_menu = ewl_menu_new();

	EWL_MENU_ITEM_SUBMENU_NEW(smi_file, "File", NULL, main_menu_bar,
				  file_menu);
	EWL_MENU_ITEM_SUBMENU_NEW(smi_windows, "Test", NULL, main_menu_bar,
				  test_menu);
	EWL_MENU_ITEM_SUBMENU_NEW(smi_help, "Help", NULL, main_menu_bar,
				  help_menu);

	{
		Ewl_Widget     *mi_exit;

		EWL_MENU_ITEM_NEW(mi_exit, "Exit", NULL, file_menu);
		ewl_callback_append(EWL_OBJECT(mi_exit),
				    EWL_WIDGET_CALLBACK_CLICKED,
				    __ewl_test_exit, NULL);
	}

	{
		Ewl_Widget     *mi_box_h, *mi_box_v, *mi_button, *mi_image;
		Ewl_Widget     *mi_menu, *mi_rotate_factory, *mi_table;
		Ewl_Widget     *mi_text, *mi_window;

		EWL_MENU_ITEM_NEW(mi_box_h, "Ewl_Box_Horizontal", NULL,
				  test_menu);
		EWL_MENU_ITEM_NEW(mi_box_v, "Ewl_Box_Vertical", NULL,
				  test_menu);
		EWL_MENU_ITEM_NEW(mi_button, "Ewl_Button", NULL, test_menu);
		EWL_MENU_ITEM_NEW(mi_image, "Ewl_Image", NULL, test_menu);
		EWL_MENU_ITEM_SUBMENU_NEW(mi_menu, "Ewl_Menu", NULL, test_menu,
					  main_menu);
		EWL_MENU_ITEM_NEW(mi_rotate_factory, "Ewl_Rotate_Factory", NULL,
				  test_menu);
		EWL_MENU_ITEM_NEW(mi_table, "Ewl_Table", NULL, test_menu);
		EWL_MENU_ITEM_NEW(mi_text, "Ewl_Text", NULL, test_menu);
		EWL_MENU_ITEM_NEW(mi_window, "Ewl_Window", NULL, test_menu);
	}

	{
		Ewl_Widget     *mi_about;

		EWL_MENU_ITEM_NEW(mi_about, "About", NULL, help_menu);
	}

	return main_menu_bar;
}
