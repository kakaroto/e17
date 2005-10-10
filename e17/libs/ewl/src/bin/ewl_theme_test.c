#include "ewl_test.h"

static Ewl_Widget *notebook;
static char *theme_list_dir = PACKAGE_DATA_DIR "/themes/";

static void
__destroy_theme_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	ewl_widget_destroy (w);
}

static void
__select_theme (Ewl_Widget * w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
        Ewl_Widget *cell;
	Ewl_Widget *text;
	char       *theme_name;
	char       *theme_filename;
	char       *ext;
	int         len1;
	int         len2;
	int         i;
	
	ewl_container_child_iterate_begin (EWL_CONTAINER (w));
	cell = ewl_container_child_next (EWL_CONTAINER (w));
	
	ewl_container_child_iterate_begin (EWL_CONTAINER (cell));
	text = ewl_container_child_next (EWL_CONTAINER (cell));
	
	theme_name = ewl_text_text_get (EWL_TEXT (text));
	ext = ".edj";
	
	len1 = strlen (theme_list_dir);
	len2 = strlen (theme_name);
	theme_filename = (char *)malloc (sizeof (char) * (len1 + len2 + 5));
	for (i = 0 ; i < len1 ; i++)
	  theme_filename[i] = theme_list_dir[i];
	for (i = 0 ; i < len2 ; i++)
	  theme_filename[i + len1] = theme_name[i];
	for (i = 0 ; i < 4 ; i++)
	  theme_filename[i + len1 + len2] = ext[i];
	theme_filename[len1 + len2 + 4] = '\0';
	
	ewl_theme_data_str_set (notebook,
				"/file", theme_filename);
	
	free (theme_filename);
}

static void
widgets_build(void)
{
        Ewl_Widget *misc;
	Ewl_Widget *item;
	Ewl_Widget *tab;
	Ewl_Widget *vbox;
	char       *str;
	char       *str_col[2];
	
	/* Notebook */
	notebook = ewl_notebook_new ();
	ewl_object_fill_policy_set(EWL_OBJECT(notebook), EWL_FLAG_FILL_ALL);
	ewl_notebook_tabs_position_set (EWL_NOTEBOOK(notebook),
					EWL_POSITION_TOP);
	ewl_widget_appearance_set (notebook, "window");
	
	/* First page: buttons */
	tab = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(tab), "Buttons");
	ewl_widget_show (tab);
	
	vbox = ewl_vbox_new ();
	ewl_widget_show (vbox);
	
	misc = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Normal button");
	ewl_object_alignment_set (EWL_OBJECT (misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_checkbutton_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Check button");
	ewl_object_alignment_set (EWL_OBJECT (misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_radiobutton_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Radio button");
	ewl_object_alignment_set (EWL_OBJECT (misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	ewl_notebook_page_append (EWL_NOTEBOOK(notebook), tab, vbox);
	
	/* Second page: numerical/text entries */
	tab = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(tab), "Numerical & text entries");
	ewl_widget_show (tab);
	
	vbox = ewl_vbox_new ();
	ewl_widget_show (vbox);
	
	misc = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(misc), "Normal entry");
	ewl_object_alignment_set (EWL_OBJECT (misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_password_new ();
	ewl_password_text_set(EWL_PASSWORD(misc), "Password entry");
	ewl_object_alignment_set (EWL_OBJECT (misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_spinner_new ();
	ewl_spinner_digits_set (EWL_SPINNER (misc), 0);
	ewl_spinner_min_val_set (EWL_SPINNER (misc), 0);
	ewl_spinner_max_val_set (EWL_SPINNER (misc), 1024);
	ewl_spinner_step_set (EWL_SPINNER (misc), 1.0);
	ewl_spinner_value_set (EWL_SPINNER (misc), 15.0);
	ewl_object_alignment_set (EWL_OBJECT (misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_hseeker_new ();
	ewl_seeker_range_set (EWL_SEEKER (misc), 10);
	ewl_seeker_step_set (EWL_SEEKER (misc), 1.0);
	ewl_seeker_value_set (EWL_SEEKER (misc), 15.0);
	ewl_object_alignment_set (EWL_OBJECT (misc), EWL_FLAG_ALIGN_LEFT);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	ewl_notebook_page_append (EWL_NOTEBOOK(notebook), tab, vbox);
	
	/* Third page: menus */
	tab = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(tab), "Menus");
	ewl_widget_show (tab);
	
	vbox = ewl_vbox_new ();
	ewl_widget_show (vbox);
	
	misc = ewl_imenu_new (NULL, "Imenu");
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	item = ewl_menu_item_new ("dia-diagram.png", "image");
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	item = ewl_menu_separator_new ();
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	item = ewl_menu_item_new (NULL, "label");
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_menu_new (NULL, "Menu");
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	item = ewl_menu_item_new ("dia-diagram.png", "image");
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	item = ewl_menu_separator_new ();
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	item = ewl_menu_item_new (NULL, "label");
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_combo_new ("Combo entry");
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	item = ewl_menu_item_new ("dia-diagram.png", "image");
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	item = ewl_menu_separator_new ();
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	item = ewl_menu_item_new (NULL, "label");
	ewl_container_child_append (EWL_CONTAINER (misc), item);
	ewl_widget_show (item);
	
	ewl_notebook_page_append (EWL_NOTEBOOK(notebook), tab, vbox);
	
	/* Fourth page: List/tree */
	tab = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(tab), "List & tree");
	ewl_widget_show (tab);
	
	vbox = ewl_vbox_new ();
	ewl_widget_show (vbox);
	
	str = "List";
	misc = ewl_tree_new (1);
	ewl_tree_headers_set (EWL_TREE (misc), &str);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	str = "Row 1";
	item = ewl_tree_text_row_add (EWL_TREE (misc), NULL, &str);
	str = "Row 2";
	item = ewl_tree_text_row_add (EWL_TREE (misc), NULL, &str);
	str = "Row 3";
	item = ewl_tree_text_row_add (EWL_TREE (misc), NULL, &str);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	str_col[0] = "Tree";
	str_col[1] = "Column";
	misc = ewl_tree_new (2);
	ewl_tree_headers_set (EWL_TREE (misc), str_col);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	str_col[0] = "row1";
	str_col[1] = "label";
	item = ewl_tree_text_row_add (EWL_TREE (misc), NULL, str_col);
	str_col[0] = "sub row";
	str_col[1] = "label";
	item = ewl_tree_text_row_add (EWL_TREE (misc), EWL_ROW (item), str_col);
	
	str_col[0] = "row2";
	str_col[1] = "label";
	item = ewl_tree_text_row_add (EWL_TREE (misc), NULL, str_col);
	str_col[0] = "sub row";
	str_col[1] = "label";
	item = ewl_tree_text_row_add (EWL_TREE (misc), EWL_ROW (item), str_col);
	
	ewl_notebook_page_append (EWL_NOTEBOOK(notebook), tab, vbox);
	
	/* Fifth page: Misc */
	tab = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(tab), "Misc");
	ewl_widget_show (tab);
	
	vbox = ewl_vbox_new ();
	ewl_widget_show (vbox);
	
	misc = ewl_hpaned_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);

	ewl_paned_active_area_set(EWL_PANED(misc), EWL_POSITION_LEFT);

	item = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(item), "left");
	ewl_container_child_append(EWL_CONTAINER(misc), item);
	ewl_widget_show (item);

	ewl_paned_active_area_set(EWL_PANED(misc), EWL_POSITION_RIGHT);

	item = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(item), "right");
	ewl_container_child_append(EWL_CONTAINER(misc), item);
	ewl_widget_show (item);
	
	misc = ewl_hseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	misc = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(misc), "Tooltip");
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	ewl_notebook_page_append (EWL_NOTEBOOK(notebook), tab, vbox);
}

void
__create_theme_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
        Ewl_Widget    *theme_win;
        Ewl_Widget    *hbox;
        Ewl_Widget    *vbox;
        Ewl_Widget    *list;
        Ewl_Widget    *row;
        Ewl_Widget    *misc;
        DIR           *rep;
        struct dirent *lecture;
        char          *name;
        char          *list_header;
        int            len;

        theme_win = ewl_window_new ();
        ewl_window_title_set (EWL_WINDOW(theme_win), "Theme Viewer Test");
        ewl_window_name_set (EWL_WINDOW(theme_win), "EWL Test Application");
        ewl_window_class_set (EWL_WINDOW(theme_win), "EFL Test Application");
        ewl_callback_append (theme_win,
			     EWL_CALLBACK_DELETE_WINDOW,
			     EWL_CALLBACK_FUNCTION (__destroy_theme_test_window),
			     NULL);

	if (w)
      	        ewl_callback_append (theme_win,
				     EWL_CALLBACK_DELETE_WINDOW,
				     EWL_CALLBACK_FUNCTION (__destroy_theme_test_window),
				     NULL);
	else
	        ewl_callback_append (theme_win,
				     EWL_CALLBACK_DELETE_WINDOW,
				     EWL_CALLBACK_FUNCTION (__close_main_window),
				     NULL);
	
        hbox = ewl_hbox_new ();
        ewl_object_fill_policy_set (EWL_OBJECT(hbox),
				    EWL_FLAG_FILL_SHRINK);
        ewl_container_child_append (EWL_CONTAINER (theme_win), hbox);
        ewl_widget_show (hbox);

        list_header = "Theme name";
        list = ewl_tree_new (1);
        ewl_tree_headers_set (EWL_TREE (list), &list_header);
        ewl_object_padding_set (EWL_OBJECT (list), 6, 3, 6, 6);
        ewl_object_fill_policy_set (EWL_OBJECT(list),
				    EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append (EWL_CONTAINER (hbox), list);
	ewl_widget_show (list);
	
	rep = opendir (theme_list_dir);
	while ((lecture = readdir(rep))) {
	        name = lecture->d_name;
		len = strlen (name);
		if ((len >= 4) && (strcmp (name + len - 4, ".edj") == 0)) {
		        char *str;

			str = malloc(len - 3);
			if (str) {
				snprintf(str, len - 3, "%s", name);
				row = ewl_tree_text_row_add(EWL_TREE (list),
							    NULL, &str);
				ewl_callback_append(row,
						    EWL_CALLBACK_CLICKED,
						    EWL_CALLBACK_FUNCTION(__select_theme),
						    NULL);
				free (str);
			}
		}
	}
	
	misc = ewl_vseparator_new ();
	ewl_container_child_append (EWL_CONTAINER (hbox), misc);
	ewl_widget_show (misc);
	
	vbox = ewl_vbox_new ();
	ewl_object_padding_set (EWL_OBJECT (vbox), 3, 6, 6, 6);
	ewl_container_child_append (EWL_CONTAINER (hbox), vbox);
	ewl_widget_show (vbox);
	
	misc = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(misc), "Theme Visualization");
	ewl_object_alignment_set (EWL_OBJECT (misc), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT (misc), EWL_FLAG_FILL_NONE);
	ewl_container_child_append (EWL_CONTAINER (vbox), misc);
	ewl_widget_show (misc);
	
	widgets_build ();
	ewl_container_child_append (EWL_CONTAINER (vbox), notebook);
	ewl_widget_show (notebook);
	
	ewl_widget_show (theme_win);
}


